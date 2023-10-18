/*
 *		$Archive: /video/nfl/cambot.c $
 *		$Revision: 176 $
 *		$Date: 10/07/99 6:24p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_cambot_c = "$Workfile: cambot.c $ $Revision: 176 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <assert.h>

/*
 *		USER INCLUDES
 */
#include "/Video/Nfl/Include/Anim.h"
#include "/Video/Nfl/Include/Game.h"
#include "/Video/Nfl/Include/Cambot.h"
#include "/Video/Nfl/Include/CamData.h"

#ifdef DEBUG
	#define CAMERA_BT		(P_A)				// P5 button for view select (see IOASIC.H)

	#define STICK_CAM_SW	4					// Stick (0-4) to use
	#define STICK_CAM_BT	(P_A)				// Button of stick to use (see IOASIC.H)

	#define STICK_CAM_ON						// Def to enable stick control
//	#define CAMERA_GLUE							// Def to freeze camera during game play
#ifdef JASON
	#define CAMERA_DRONE						// Def to elevate & pull back CAM_NORMAL mode
#endif
#endif

int playback = FALSE;			// Remove if Replay system need to go back into blitz 2000 - JTA

#include "include/game.h"
#include "include/player.h"
#include "include/camdefs.h"
#include "include/id.h"
#include "include/fontid.h"

/*
 *		DEFINES
 */

#define PLAYOVER_TICKS	2						// Play over transition ticks (must be at least 2)

#define	DOBYPASS		0						// Post mode-specific action flags
#define	UNROTATE		(1 << 0)				// Unrotate target coords
#define	SNAPTARG		(1 << 1)				// Snap target to coords/angles
#define	ACCLTPOS		(1 << 2)				// Do target coords acceleration math
#define	ACCLTANG		(1 << 3)				// Do target angles acceleration math
#define	MOVETPOS		(1 << 4)				// Accelerate target coords
#define	MOVETANG		(1 << 5)				// Accelerate target angles
#define	MOVEVIEW		(1 << 6)				// Move viewpt coords
#define	NEWTHETA		(1 << 7)				// Calculate new theta values

#define	ACCLTARG		(ACCLTPOS | ACCLTANG)	// Do target coords/angles acceleration math
#define	MOVETARG		(MOVETPOS | MOVETANG)	// Accelerate target coords/angles

#define	DOROTCAMX(x,z)	( x * cambot.cos_theta - z * cambot.sin_theta)
#define	DOROTCAMZ(x,z)	( x * cambot.sin_theta + z * cambot.cos_theta)
#define	DONOXCAMX(z)	(-z * cambot.sin_theta)
#define	DONOZCAMX(x)	( x * cambot.cos_theta)
#define	DONOXCAMZ(z)	( z * cambot.cos_theta)
#define	DONOZCAMZ(x)	( x * cambot.sin_theta)
#define	UNROTCAMX(x,z)	( z * cambot.sin_theta + x * cambot.cos_theta)
#define	UNROTCAMZ(x,z)	( z * cambot.cos_theta - x * cambot.sin_theta)


#ifdef CAMERA_DRONE
#undef NORMAL_DIST_MIN
#undef NORMAL_DIST_MINPB
#undef NORMAL_PHI
#define NORMAL_DIST_MIN		(420.0f)		// Distance, minimum
#define NORMAL_DIST_MINPB	(420.0f)		// Distance, minimum for pull-back
#define NORMAL_PHI			(DG2RD(-40.0f))		// Phi, default
#endif //CAMERA_DRONE


// Tracking buffer constants

#define TRACK_DEFAULT		2.0f				// Track zone: default
#define TRACK_P2P			0.25f				// Track zone: <cambot_p2p> mode

// Acceleration constants

#define ACCEL_DEFAULT		0.08f				// Accel rate: default
#define ACCEL_DEF_NOBC		0.0005f				// Accel rate: default but no ball carrier
#define ACCEL_ATTRACT		0.04f				// Accel rate: attract mode
#define ACCEL_P2P			0.25f				// Accel rate: <cambot_p2p> mode

// Deceleration constants

#define DECEL_DEFAULT	0.35f					// Decel rate: default

/*
 *		STRUCTURES/TYPEDEFS
 */

// info structure for cambot positioning

typedef struct tagCAMBOTINFO {
	float frot_dxmin,frot_dxmax;
	float frot_zmin,frot_zmax;
	int carrier_mode;
	int player_valid,player_count;
	float fa,fb,fc,fd;
	float fdx,fdz;
	float * p_track;
	float * p_accel;
	float frot_x[NUM_PLAYERS];
	float frot_z[NUM_PLAYERS];
} CAMBOTINFO;

/*
 *		PROTOTYPES
 */

//void set_cambot_theta(float);
//void set_cambot_phi(float);
//void update_cambot_xform(void);

static float ValidateTheta(float);
static float ValidatePhi(float);
static void BuffAccel(float *, float, float, float);

static void TrackObjects(VIEWCOORD *, CAMBOTINFO *);
static void DoSpline(VERTEX *, SPLINE *, int, int);
static void DoAccelDecel(int, int);
static void SetModeAngleTrgs(VIEWCOORD *, int);
static int ModeRevert(VIEWCOORD *, CAMBOTINFO *);
static int cambot_normal(VIEWCOORD *, CAMBOTINFO *);
static int cambot_side(VIEWCOORD *, CAMBOTINFO *);
static int cambot_top(VIEWCOORD *, CAMBOTINFO *);
static int cambot_attract(VIEWCOORD *, CAMBOTINFO *);
static int cambot_zoom(VIEWCOORD *, CAMBOTINFO *);
static int cambot_spline(VIEWCOORD *, CAMBOTINFO *);
static int cambot_rail(VIEWCOORD *, CAMBOTINFO *);
static int cambot_path(VIEWCOORD *, CAMBOTINFO *);
static int cambot_course(VIEWCOORD *, CAMBOTINFO *);
static int cambot_p2p(VIEWCOORD *, CAMBOTINFO *);
static int cambot_gopher(VIEWCOORD *, CAMBOTINFO *);
static int cambot_resume(VIEWCOORD *, CAMBOTINFO *);
#ifdef SPECIAL_MODES
static int cambot_helmet(VIEWCOORD *, CAMBOTINFO *);
static int cambot_norm(VIEWCOORD *, CAMBOTINFO *);
#endif //SPECIAL_MODES
static int cambot_special(VIEWCOORD *, CAMBOTINFO *, int);

/*
 *		GLOBAL REFERNCES
 */

extern POP_INFO * ppi_fumble[];
extern POP_INFO * ppi_fg_hitpost[];

extern int flg_hhteam;							// Set per case at start of each play
extern int flg_pullback;						// Set per case at start of each play

CAMSEQINFO * select_csi_ptr(CAMSEQINFO *);
void show_message_proc(int *);
//float rsqrt(float);

#ifdef DBG_KO
#define DBG_KO_BOOL	dbg_ko
extern int dbg_ko;
#else
#define DBG_KO_BOOL	0
#endif

/*
 *		GLOBAL VARIABLES
 */

VIEWCOORD cambot;

//float p2p_dist[NUM_PLAYERS][NUM_PLAYERS];

int shaker_amp;									// Shaker amplification level
int shaker_tot;									// Shaker count total
int shaker_cnt;									// Shaker count

/*
 *		STATIC VARIABLES
 */

// Tracking value tables

static float track_def[] = {
	TRACK_DEFAULT,	TRACK_DEFAULT,	TRACK_DEFAULT,	TRACK_DEFAULT
};
#if 0
static float track_tight[] = {
	TRACK_P2P,		TRACK_P2P,		TRACK_P2P,		TRACK_P2P
};
#endif

static float track_p2p[] = {
	TRACK_P2P,		TRACK_P2P,		TRACK_P2P,		0.0f
};

// Acceleration value tables

static float accel_def[] = {
	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEFAULT
};
static float accel_def_nobc[] = {
	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEFAULT,	ACCEL_DEF_NOBC
};
static float accel_attract[] = {
	ACCEL_ATTRACT,	ACCEL_ATTRACT,	ACCEL_ATTRACT,	0.0f
};
#if 0
static float accel_tight[] = {
	ACCEL_P2P,		ACCEL_P2P,		ACCEL_P2P,		ACCEL_P2P
};
#endif
static float accel_p2p[] = {
	ACCEL_P2P,		ACCEL_P2P,		ACCEL_P2P,		0.0f
};

// Mode Parameters
#define NQ	0
#define QB	1

enum {
	MP_DIST,
	MP_SIDE,
	MP_CNT
};
//                	    M   MP    QB 4P PB
static float mode_parms[][MP_CNT][2][2][2] =
{
 {	// CAM_NORMAL
  {
   {{NORMAL_DIST_MIN,		NORMAL_DIST_MINPB},
    {NORMAL_DIST_MIN4P,		NORMAL_DIST_MIN4PPB}},
   {{NORMAL_DIST_QB,		NORMAL_DIST_QBPB},
    {NORMAL_DIST_QB4P,		NORMAL_DIST_QB4PPB}}
  },
  {
   {{NORMAL_SIDELIM,		NORMAL_SIDELIM_PB},
    {NORMAL_SIDELIM_4P,		NORMAL_SIDELIM_4PPB}},
   {{NORMAL_SIDELIM_QB,		NORMAL_SIDELIM_QBPB},
    {NORMAL_SIDELIM_QB4P,	NORMAL_SIDELIM_QB4PPB}}
  }
 }
};


static int idiot_flag;
//static int inplay_flag;
//static int attract_flag;
static int show_sideline;

static float sin_alpha;
static float sin_delta;

// Set/clr of <mode_trgy_adjust> should be handled carefully on start/finish
//  of special camera modes; will be clr'd when transients revert in
//  [cambot_proc] or [ModeRevert] or [cambot_resume]; looked at by:
//    CAM_ZOOM
//    CAM_SPLINE

static float mode_trgy_adjust;
static float carrier_goal_dist;
//static float carrier_side_dist;
//static float carrier_los_per;

#ifdef SPECIAL_MODES
static int helmet_fang;							// helmet-cam facing angle
static int helmet_tang;							// helmet-cam target angle
#endif //SPECIAL_MODES

// Function calls for modal camera control

static int (*cambot_funcs[])(VIEWCOORD *, CAMBOTINFO *) = {
		cambot_normal, cambot_side,   cambot_top,
		cambot_attract,
// transient modes
		cambot_zoom,   cambot_zoom,   cambot_zoom,
		cambot_spline, cambot_spline, cambot_spline,
		cambot_rail,   cambot_rail,   cambot_rail,
		cambot_path,   cambot_path,   cambot_path,
		cambot_course, cambot_course, cambot_course,
		cambot_p2p,
		cambot_gopher, cambot_resume,
#ifdef SPECIAL_MODES
		NULL,          NULL,
		cambot_helmet, cambot_norm,
		cambot_normal, cambot_normal, cambot_normal
#endif //SPECIAL_MODES
};

// Shaker exponent table (2^(-x) with 8 steps between each integer)

static float shaker_exp[8 * 8] = {
//      +.000       +.125       +.250       +.375       +.500       +.625       +.750       +.875
/*0*/	1.00000000f,0.91700404f,0.84089641f,0.77110541f,0.70710678f,0.64841977f,0.59460355f,0.54525386f,
/*1*/	0.50000000f,0.45850202f,0.42044820f,0.38555270f,0.35355339f,0.32420988f,0.29730177f,0.27262693f,
/*2*/	0.25000000f,0.22925101f,0.21022410f,0.19277635f,0.17677669f,0.16210494f,0.14865088f,0.13631346f,
/*3*/	0.12500000f,0.11462550f,0.10511205f,0.09638817f,0.08838834f,0.08105247f,0.07432544f,0.06815673f,
/*4*/	0.06250000f,0.05731275f,0.05255602f,0.04819408f,0.04419417f,0.04052623f,0.03716272f,0.03407836f,
/*5*/	0.03125000f,0.02865637f,0.02627801f,0.02409704f,0.02209708f,0.02026311f,0.01858136f,0.01703918f,
/*6*/	0.01562500f,0.01432818f,0.01313900f,0.01204852f,0.01104854f,0.01013155f,0.00929068f,0.00851959f,
/*7*/	0.00781250f,0.00716409f,0.00656950f,0.00602426f,0.00552427f,0.00506577f,0.00464534f,0.00425979f
};

static CAMBOTINFO cambotinfo;
static CAMBOTINFO cambotinfo_init = {
	2000.0f,-2000.0f,			// frot_dxmin,frot_dxmax
	2000.0f,-2000.0f,			// frot_zmin,frot_zmax
	NOBALLCARRIER,				// carrier_mode
	0,0,						// player_valid,player_count
	0.0f,0.0f,0.0f,0.0f,		// fa,fb,fc,fd
	0.0f,0.0f,					// fdx,fdz
	track_def,					// p_track
	accel_def					// p_accel
};

#ifdef CAMCRAP
static char * mtxt[] = {"GM_LINING_UP","GM_PRE_SNAP","GM_IN_PLAY","GM_PLAY_OVER","GM_GAME_OVER","GM_PRE_KICKOFF"};
#endif //CAMCRAP

/******************************************************************************
** set_cambot_theta() - Set new cambot XZ theta,trigs
*/
void set_cambot_theta(float theta)
{
	cambot.theta = cambot.theta_trg = ValidateTheta(theta);
	cambot.sin_theta = fsin(cambot.theta);
	cambot.cos_theta = fcos(cambot.theta);
}

/******************************************************************************
** set_cambot_phi() - Set new cambot Y phi,trigs
*/
void set_cambot_phi(float phi)
{
	cambot.phi = cambot.phi_trg = ValidatePhi(phi);
	cambot.sin_phi = fsin(cambot.phi);
	cambot.cos_phi = fcos(cambot.phi);
}

/******************************************************************************
** update_cambot_xform() - Recompute cambot_xform based on current cambot
** settings.
*/
void update_cambot_xform(void)
{
	/* see Foley / van Dam pg 220 */

	vec3d p1p2;
	vec3d p1p3;
	vec3d p13xp12;
	vec3d ry;

	p1p2.x = cambot.a - cambot.x;
	p1p2.y = cambot.b - cambot.y;
	p1p2.z = cambot.c - cambot.z;
	p1p2.w = 1.0f;

	p1p3.x = cambot.e - cambot.x;
	p1p3.y = cambot.f - cambot.y;
	p1p3.z = cambot.g - cambot.z;
	p1p3.w = 1.0f;

	vxv(&p1p3,&p1p2,&p13xp12);
	norm(&p13xp12);
	norm(&p1p2);
	vxv(&p1p2,&p13xp12,&ry);

	cambot.xform[0] = p13xp12.x;
	cambot.xform[1] = p13xp12.y;
	cambot.xform[2] = p13xp12.z;
	cambot.xform[3] = 0.0f;

	cambot.xform[4] = ry.x;
	cambot.xform[5] = ry.y;
	cambot.xform[6] = ry.z;
	cambot.xform[7] = 0.0f;

	cambot.xform[8] = p1p2.x;
	cambot.xform[9] = p1p2.y;
	cambot.xform[10] = p1p2.z;
	cambot.xform[11] = 0.0f;
}

/******************************************************************************
** ValidateTheta()
*/
static float ValidateTheta(float theta)
{
	register float fy = THETA_MAX;
	register float fz = THETA_MIN;

	while (theta >= fy) theta -= fy;
	while (theta <  fz) theta += fy;

	return theta;
}

/******************************************************************************
** ValidatePhi()
*/
static float ValidatePhi(float phi)
{
	register float fy = PHI_MAX;
	register float fz = PHI_MIN;

	if (phi < fy) phi = fy;
	if (phi > fz) phi = fz;

	return phi;
}

/******************************************************************************
** BuffAccel()
*/
#define P_DA	&cambot.da
#define P_XA	&cambot.xa
#define P_YA	&cambot.ya
#define P_ZA	&cambot.za

#define P_S_X1	&cambot.s.x1
#define P_S_Y1	&cambot.s.y1
#define P_S_Z1	&cambot.s.z1

static void BuffAccel(float * pf, float diff, float buff, float rate)
{
	if (diff < -buff) buff = -buff;
	else
	if (diff <= buff) {*pf = 0.0f; return;}

	*pf = rate * (diff - buff);
}

#ifdef DEBUG
/******************************************************************************
*/
void display_mode(int *);
void display_mode(int *parg)
{
	static char * szmode[] = {
		"NORMAL CAM","SIDE CAM","TOP CAM",
		"ATTRACT CAM",
		"ZOOM CAM",  "ZO-NR CAM", "ZO-NFR CAM",
		"SPLINE CAM","SP-NFR CAM","SP-CNFR CAM",
		"RAIL CAM",  "RA-NF CAM", "RA-NFT CAM",
		"PATH CAM",  "PA-NF CAM", "PA-NFT CAM",
		"COURSE CAM","CO-C CAM",  "CO-CNT CAM",
		"P2P CAM",
		"GOPHER CAM","",
#ifdef SPECIAL_MODES
		NULL,          NULL,
		"HELMET CAM","NORM CAM",
		"NORM-A CAM","NORM-SP1 CAM","NORM-SP2 CAM"
#endif //SPECIAL_MODES
	};
	int mylast = -1;
	int timer = 0;

	while (1) {
		if (timer) {
			timer--;
			if (!timer)
				delete_multiple_strings(0xfabe, 0xffffffff);
		}
		if (mylast != cambot.csi.mode) {
			mylast = cambot.csi.mode;
			delete_multiple_strings(0xfabe, 0xffffffff);
			set_text_font(FONTID_BAST10);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_text_position(256, 376, 1.2f);
			set_string_id(0xfabe);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, (mylast >= 0) ? szmode[mylast]:"(NULL)");
			timer = 57 * 3;
		}
		sleep(1);
	}
}
#endif //DEBUG

/******************************************************************************
** cambot_proc() - Top-level cambot control process.
*/
void cambot_proc(int *parg)
{
	// Set some initial defaults
	cambot.csi.mode     = CAM_NORMAL;
	cambot.mode_prev    = CAM_NORMAL;
	cambot.was_playover = 1;
	//cambot.was_playover = PLAYOVER_TICKS;
	cambot.new_quarter  = 1;
	cambot.kickoff_flag = 0;

	set_cambot_theta(NORMAL_THETA);
	set_cambot_phi(NORMAL_PHI);

	cambot.dv = cambot.xv = cambot.yv = cambot.zv = cambot.tv = cambot.pv = 0.0f;
	cambot.da = cambot.xa = cambot.ya = cambot.za = cambot.ta = cambot.pa = 0.0f;

	cambot.d = 100.0f;
//	cambot.length = 2.0f;

	idiot_flag = 0;
//	inplay_flag = 0;
//	attract_flag = 0;
	mode_trgy_adjust = 0.0f;

	// Set constants
	sin_alpha = fsin(ALPHA);

	
	// Do setup for initial kickoff
	cambot.x = (game_info.last_pos) ? FIELDHZ : -FIELDHZ;
	cambot.y =  25.0f;
	cambot.z = (game_info.last_pos) ? -35.0f : 35.0f;

	cambot.e = (cambot.a = WORLDX(FIRSTKO_LOS));
	cambot.f = (cambot.b = TRGY_NORMAL) + 10.0f;
	cambot.g = (cambot.c = 0.0f);

	update_cambot_xform();

	// Wait for all the other stuff to initialize
	while (game_info.game_mode == GM_INITIALIZING)
		sleep(1);

//#ifdef DBG_KO
//	fprintf(stderr,"%f  %f  %f\r\n",ball_obj.odata.x,ball_obj.odata.y,ball_obj.odata.z);
//	fprintf(stderr,"%f  %f  %f\r\n",cambot.x,cambot.y,cambot.z);
//	fprintf(stderr,"%f  %f  %f\r\n\r\n",cambot.a,cambot.b,cambot.c);
//#endif //DBG_KO
#ifdef DEBUG
	set_process_run_level( iqcreate( "camdisp", CAMBOT_PID, display_mode, 0, 0, 0, 0 ), RL_CAMBOT+1 );
#endif //DEBUG


	// Top of proc loop
	while (1) {
		register VIEWCOORD *pcam = &cambot;
		register CAMBOTINFO *pci = &cambotinfo;
		int flag = DOBYPASS;

#ifdef STICK_CAM_ON
		static int ko_idx = -1;
		static int bt_state = -1;
		static int roll = 0;
		static float svelx = 0.0f, svely = 0.0f;
//		{
//			static int h = 1;
//			if (h) halt = h;
//			h = 0;
//		}
#ifdef DBG_KO
		if (get_dip_coin_current() & COINDOOR_INTERLOCK_SW)
			dbg_ko = TRUE;
#endif //DBG_KO

		if (halt || DBG_KO_BOOL) {
			static int last_sw = 0;
			static int repeat = 0;
#if STICK_CAM_SW == 4
			int sw = get_p5_sw_current();
#else
			int sw = get_player_sw_current() >> (STICK_CAM_SW * 8);
#endif //STICK_CAM_SW == 4
			int sc = (sw ^ last_sw) & sw;
			if (!repeat)
				repeat = 2;
			if (sw && sw == last_sw)
				repeat--;
			else
				repeat = 20;
			last_sw = sw;
			if (sc & STICK_CAM_BT) {
				static char * btxt[] = {"MOVE X/Z","MOVE Y/D",     "ROTATE",  "ROLL",  "FREEZE"};
				static char * ktxt[] = {"MOVE X/Z","TIME A/MOVE Y","TIME T/M","SELECT"};

				bt_state++;
				if (bt_state == 5 || (bt_state == 4 && DBG_KO_BOOL))
					bt_state = 0;
				if (ko_idx < 0 && DBG_KO_BOOL)
					bt_state = 3;
				svelx = svely = 0.0f;

				delete_multiple_strings(0xfeed, 0xffffffff);
				set_text_font(FONTID_BAST10);
				set_string_id(0xfeed);
				set_text_transparency_mode(TRANSPARENCY_ENABLE);
				if (DBG_KO_BOOL)
					set_text_position(256, 210, 1.2f);
				else
					set_text_position(256, 364, 1.2f);

				oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW,
					DBG_KO_BOOL ? ktxt[bt_state] : btxt[bt_state]);
			}
			else
			if (sw & STICK_CAM_BT) {
				// Check stick movement
				if ((sw &= 15)) {
					static float sw_vel[] = {0.0f, -0.5f,  0.5f};
					static float sw_acc[] = {0.0f, -0.04f, 0.04f};

					if (svelx == 0.0f || !(sw>>2))
						svelx  = sw_vel[sw>>2];
					else
						svelx += sw_acc[sw>>2];
					if (svely == 0.0f || !(sw &3))
						svely  = sw_vel[sw &3];
					else
						svely += sw_acc[sw &3];

					switch (bt_state) {
						case 0:
							if (DBG_KO_BOOL) {
								((KO_INFO *)csi_kickoff[ko_idx].pseqinfo)->x -= svely;
								((KO_INFO *)csi_kickoff[ko_idx].pseqinfo)->z -= svelx;
							}
							else {
								pcam->a -= svely * pcam->sin_theta + svelx * pcam->cos_theta;
								pcam->c -= svely * pcam->cos_theta - svelx * pcam->sin_theta;
								flag = (MOVEVIEW | ACCLTARG);
							}
							break;
						case 1:
							if (DBG_KO_BOOL) {
								((KO_INFO *)csi_kickoff[ko_idx].pseqinfo)->y -= svely;
								if ((sc & P_LEFT) ||
									(sw == P_LEFT && !repeat))
								{
									if (--((KO_INFO *)csi_kickoff[ko_idx].pseqinfo)->auto_hike_cntr < 0)
										((KO_INFO *)csi_kickoff[ko_idx].pseqinfo)->auto_hike_cntr = 0;
								}
								else
								if ((sc & P_RIGHT) ||
									(sw == P_RIGHT && !repeat))
								{
									++((KO_INFO *)csi_kickoff[ko_idx].pseqinfo)->auto_hike_cntr;
								}
							}
							else {
								pcam->d -= svelx;
								pcam->b -= svely;
								flag = (MOVEVIEW | ACCLTARG);
							}
							break;
						case 2:
							if (DBG_KO_BOOL) {
								if ((sc & P_LEFT) ||
									(sw == P_LEFT && !repeat))
								{
									--csi_kickoff[ko_idx].trav_cnt;
								}
								else
								if ((sc & P_RIGHT) ||
									(sw == P_RIGHT && !repeat))
								{
									if (++csi_kickoff[ko_idx].trav_cnt >
										csi_kickoff[ko_idx].mode_cnt)
									{
										csi_kickoff[ko_idx].mode_cnt =
											csi_kickoff[ko_idx].trav_cnt;
									}
								}
								else
								if ((sc & P_UP) ||
									(sw == P_UP && !repeat))
								{
									if (--csi_kickoff[ko_idx].mode_cnt <
										csi_kickoff[ko_idx].trav_cnt)
									{
										csi_kickoff[ko_idx].trav_cnt =
											csi_kickoff[ko_idx].mode_cnt;
									}
								}
								else
								if ((sc & P_DOWN) ||
									(sw == P_DOWN && !repeat))
								{
									++csi_kickoff[ko_idx].mode_cnt;
								}
							}
							else {
								if (svelx < 0.0f)
									set_cambot_theta(pcam->theta_trg-DG2RD(1.0f));
								else if (svelx > 0.0f)
									set_cambot_theta(pcam->theta_trg+DG2RD(1.0f));
								if (svely < 0.0f)
									set_cambot_phi(pcam->phi_trg-DG2RD(1.0f));
								else if (svely > 0.0f)
									set_cambot_phi(pcam->phi_trg+DG2RD(1.0f));
								flag = (MOVEVIEW | ACCLTARG);
							}
							break;
						case 3:
							if (DBG_KO_BOOL) {
								if (svely != 0 && ko_idx < 0)
								{
									ko_idx = 0;
								}
								else
								if (sc & P_UP) {
									if (--ko_idx < 0) {
										for (ko_idx = 0;
											!(csi_kickoff[ko_idx+1].mode < 0);
											ko_idx++);
									}
								}
								else
								if (sc & P_DOWN) {
									if (csi_kickoff[++ko_idx].mode < 0) {
										ko_idx = 0;
									}
								}
							}
							else {
								roll = (roll + (int)svelx + 1024) % 1024;
								if (svely) roll = 0;
							}
							break;
						default:
							svelx = svely = 0.0f;
							break;
					}
				}
				else {
					svelx = svely = 0.0f;
				}
			}
			else if (bt_state != 4)
				delete_multiple_strings(0xfeed, 0xffffffff);
		}
		else if (bt_state != -1) {
			delete_multiple_strings(0xfeed, 0xffffffff);
			bt_state = -1;
		}
#endif //STICK_CAM_ON

//		// Do player-to-player distancing
//		{
//			register fbplyr_data *ppdi = player_blocks;
//			register fbplyr_data *ppdj;
//			register fbplyr_data *ppdk;
//			register float dx,dz;
//			register int i,j;
//			for (i = 0; i < NUM_PERTEAM; ppdi++, i++) {
//				ppdj = &player_blocks[NUM_PERTEAM];
//
//				for (j = NUM_PERTEAM; j < NUM_PLAYERS; ppdj++, j++) {
//
//					// Do team 0 against team 1
//					dx = ppdi->odata.x - ppdj->odata.x;
//					dz = ppdi->odata.z - ppdj->odata.z;
//					p2p_dist[i][j] = p2p_dist[j][i] = fsqrt(dx*dx + dz*dz);
//
//					if ((j - NUM_PERTEAM) > i) {
//
//						// Do team 0 against itself
//						ppdk = &player_blocks[j -= NUM_PERTEAM];
//						dx = ppdi->odata.x - ppdk->odata.x;
//						dz = ppdi->odata.z - ppdk->odata.z;
//						p2p_dist[i][j] = p2p_dist[j][i] = fsqrt(dx*dx + dz*dz);
//						j += NUM_PERTEAM;
//
//						// Do team 1 against itself
//						ppdk = &player_blocks[i += NUM_PERTEAM];
//						dx = ppdj->odata.x - ppdk->odata.x;
//						dz = ppdj->odata.z - ppdk->odata.z;
//						p2p_dist[i][j] = p2p_dist[j][i] = fsqrt(dx*dx + dz*dz);
//						i -= NUM_PERTEAM;
//					}
//				}
//			}
//		}

#ifdef STICK_CAM_ON
#if STICK_CAM_SW == 4
		if (!halt || (halt && (playback || (get_p5_sw_close()) & P_B)))
		{
#else
		if (!halt || (halt && (playback || (get_player_sw_close() >> (STICK_CAM_SW * 8)) & P_B)))
		{
#endif //STICK_CAM_SW == 4
#endif //STICK_CAM_ON

		// Revert to previous mode if in a transient & starting a new play;
		//  resets <mode_trgy_adjust> if reverts
		{
			register int i = 0;

			if (game_info.play_type != PT_KICKOFF)
				pcam->kickoff_flag = 0;

			if (pcam->kickoff_flag && (
					game_info.ball_carrier >= 0 ||
					ball_obj.flags & BF_BOUNCE)) {
				pcam->kickoff_flag = 0;
				i++;
			}
			if (pcam->csi.mode != CAM_NORMAL &&
					pcam->csi.mode != CAM_SIDE &&			//!!!
					pcam->csi.mode != CAM_TOP) {			//!!!
				if (i || (
						pcam->was_playover && (
						game_info.game_mode == GM_LINING_UP ||
						game_info.game_mode == GM_PRE_SNAP))) {
#ifdef CAMCRAP
					if (pcam->csi.mode != pcam->mode_prev)
						printf("reverting to mode %d\n",pcam->mode_prev);
#endif //CAMCRAP
					pcam->csi.mode = pcam->mode_prev;
					mode_trgy_adjust = 0.0f;
				}
			}
		}

		// Do mode specifics if its not a n-mode
		if (!(pcam->csi.mode < 0))
			flag = cambot_funcs[pcam->csi.mode](pcam, pci);

		// Track any mode-switching
		if (pcam->mode_last != pcam->csi.mode) {
			// Only let <mode_prev> be a non-transient
			if (pcam->mode_last == CAM_NORMAL
					|| pcam->mode_last == CAM_SIDE
					|| pcam->mode_last == CAM_TOP
#ifdef SPECIAL_MODES
					|| pcam->mode_last == CAM_HELMET
					|| pcam->mode_last == CAM_NORM
					|| pcam->mode_last == CAM_NORMAL_A
					|| pcam->mode_last == CAM_NORMAL_SP1
					|| pcam->mode_last == CAM_NORMAL_SP2
#endif //SPECIAL_MODES
					)
				pcam->mode_prev = pcam->mode_last;
		}
		pcam->mode_last = pcam->csi.mode;

		// Validate cambot angles
		pcam->theta     = ValidateTheta(pcam->theta);
		pcam->theta_trg = ValidateTheta(pcam->theta_trg);

		pcam->phi     = ValidatePhi(pcam->phi);
		pcam->phi_trg = ValidatePhi(pcam->phi_trg);

		// Do the unrotate if mode spec'd
		if (flag & UNROTATE) {
			register float fa = pci->fa;
			pci->fa = UNROTCAMX(fa,pci->fc);
			//pci->fb = pci->fb;
			pci->fc = UNROTCAMZ(fa,pci->fc);
		}

		// Chk and do any special cameras
		flag = cambot_special( pcam, pci, flag );

#ifdef CAMERA_GLUE
		if (game_info.game_mode != GM_IN_PLAY)
		{
#endif //CAMERA_GLUE

		// Process diffs to determine new velocities
		if (flag) {
			// Snap target to coords/angles
			// Overrides any further position/velocity processing
			if (flag & SNAPTARG) {
				pcam->a = pci->fa;
				pcam->b = pci->fb;
				pcam->c = pci->fc;
				pcam->d = pci->fd;

				if (pcam->theta != pcam->theta_trg)
					set_cambot_theta(pcam->theta_trg);
				if (pcam->phi   != pcam->phi_trg)
					set_cambot_phi(pcam->phi_trg);

				pcam->xv = pcam->yv = pcam->zv = pcam->dv = pcam->tv = pcam->pv = 0.0f;
				pcam->xa = pcam->ya = pcam->za = pcam->da = pcam->ta = pcam->pa = 0.0f;
			}
			else {
				if (flag & ACCLTPOS) {
					// Do target coords acceleration math
					// from current to desired
					register float * pf1 = pci->p_track;
					register float * pf2 = pci->p_accel;

					BuffAccel(P_XA, pci->fa - pcam->a, *pf1++, *pf2++);
					BuffAccel(P_YA, pci->fb - pcam->b, *pf1++, *pf2++);
					BuffAccel(P_ZA, pci->fc - pcam->c, *pf1++, *pf2++);
					BuffAccel(P_DA, pci->fd - pcam->d, *pf1++, *pf2++);

					pcam->xv *= DECEL_DEFAULT;
					pcam->yv *= DECEL_DEFAULT;
					pcam->zv *= DECEL_DEFAULT;
					pcam->dv *= DECEL_DEFAULT;
				}
				if (flag & ACCLTANG) {
					// Do target angles acceleration math
					// from current to desired
					register float fx = pcam->theta_trg - pcam->theta;

					if (fx) {
						if (fx >  ( THETA_MAX / 2)) fx -= THETA_MAX;
						if (fx <= (-THETA_MAX / 2)) fx += THETA_MAX;
					}
					pcam->ta = ACCEL_DEFAULT * fx;
					pcam->pa = ACCEL_DEFAULT * (pcam->phi_trg - pcam->phi);

					pcam->tv *= DECEL_DEFAULT;
					pcam->pv *= DECEL_DEFAULT;
				}
				if (flag & MOVETPOS) {
					// Accelerate target coords
					pcam->a += (pcam->xv += pcam->xa);
					pcam->b += (pcam->yv += pcam->ya);
					pcam->c += (pcam->zv += pcam->za);
					pcam->d += (pcam->dv += pcam->da);
				}
				if (flag & MOVETANG) {
					// Accelerate target angles
					pcam->theta += (pcam->tv += pcam->ta);
					pcam->phi   += (pcam->pv += pcam->pa);
					if (pcam->tv) {
						pcam->sin_theta = fsin(pcam->theta);
						pcam->cos_theta = fcos(pcam->theta);
					}
					if (pcam->pv) {
						pcam->sin_phi = fsin(pcam->phi);
						pcam->cos_phi = fcos(pcam->phi);
					}
				}
			}
		}

#ifdef CAMERA_GLUE
		}
#endif //CAMERA_GLUE
#ifdef STICK_CAM_ON
		}
#endif //STICK_CAM_ON

#ifdef CAMCRAP
		if (get_dip_coin_close() & (VOLUME_UP_SW|VOLUME_DOWN_SW)) {
			fprintf(stderr," x= %f   y= %f   z= %f\r\n",pcam->x,pcam->y,pcam->z);
			fprintf(stderr," a= %f   b= %f   c= %f   d= %f\r\n",pcam->a,pcam->b,pcam->c,pcam->d);
			fprintf(stderr," T= %f   P= %f\r\n\r\n",((pcam->theta*180.0f)/M_PI),((pcam->phi*180.0f)/M_PI));
		}
#endif //CAMCRAP

		// Put the viewpoint where it goes
		if (flag & MOVEVIEW) {
			pcam->x = pcam->a - pcam->d * pcam->cos_phi * pcam->sin_theta;
			pcam->y = pcam->b - pcam->d * pcam->sin_phi;
			pcam->z = pcam->c - pcam->d * pcam->cos_phi * pcam->cos_theta;
		}

		// Calculate new theta values
		if (flag & NEWTHETA) {
			register float fx  = pcam->a - pcam->x;
			register float fz  = pcam->c - pcam->z;
			register float ooh = rsqrt(fx * fx + fz * fz);

			pcam->sin_theta = (fx *= ooh);
			pcam->cos_theta = (fz *= ooh);

			// Just to be sure...
			if (fz < -1.0f) fz = -1.0f;
			if (fz >  1.0f) fz =  1.0f;

			fz = acos(fz);
			if (fx < 0)
				fz = M_TWOPI - fz;

			pcam->theta = ValidateTheta(fz);
		}

		// Do camera shaker
		if (shaker_cnt < shaker_tot) {
			register float expx;
			register float expz;
			expx = expz = shaker_exp[(sizeof(shaker_exp)/sizeof(*shaker_exp))*shaker_cnt/shaker_tot];
			shaker_cnt++;

			expx *= (float)(shaker_amp * (randrng(3) - 1));
			expz *= (float)(shaker_amp * (randrng(3) - 1));
			pcam->a += expx;
			pcam->c += expz;
			pcam->x += expx;
			pcam->z += expz;
		}

		// Finish positioning & update transform matrix
		pcam->e = pcam->a;
		pcam->f = pcam->b + 10.0f;
		pcam->g = pcam->c;
#ifdef STICK_CAM_ON
		pcam->e = pcam->a - (10.0f * isin(roll))*pcam->cos_theta;
		pcam->f = pcam->b + (10.0f * icos(roll));
		pcam->g = pcam->c + (10.0f * isin(roll))*pcam->sin_theta;
#endif //STICK_CAM_ON

		update_cambot_xform();

#ifdef DBG_KO
		if (dbg_ko)
		{
			pcam->new_quarter = 1;
			pcam->kickoff_flag = 0;
#ifdef STICK_CAM_ON
			if (ko_idx >= 0)
				pcam->new_quarter = -1 - ko_idx;
#endif //STICK_CAM_ON
		}
		else
		{
#endif //DBG_KO
#ifdef CAMERA_BT
		{
#ifdef STICK_CAM_ON
		if (!halt)
		{
#endif //STICK_CAM_ON

			// Check for camera view change (only if there is an active button)
			if (get_p5_sw_close() & CAMERA_BT) {
				switch (pcam->csi.mode) {
					case CAM_NORMAL:
#ifdef SPECIAL_MODES
						if (get_p5_sw_current() & P_UP) {
							pcam->csi.mode = CAM_SIDE;
							break;
						}

					case CAM_SIDE:
						if (get_p5_sw_current() & P_UP) {
							pcam->csi.mode = CAM_TOP;
							break;
						}

					case CAM_TOP:
						if (get_p5_sw_current() & P_UP) {
							pcam->csi.mode = CAM_ATTRACT;
							break;
						}

					case CAM_ATTRACT:
						if (get_p5_sw_current() & P_DOWN) {
							pcam->csi.mode = CAM_GOPHER;
							break;
						}

					case CAM_GOPHER:
						if (get_p5_sw_current() & P_DOWN) {
							int pnum = PPOS_QBACK;
							pcam->csi.mode = CAM_HELMET;
							// Determine player # to track
							if (game_info.last_pos) pnum += NUM_PERTEAM;
							if (game_info.game_mode == GM_IN_PLAY) {
								if ((pnum = game_info.ball_carrier) < 0)
									pnum = ball_obj.int_receiver;
							}
							helmet_tang = player_blocks[pnum].odata.fwd & 1023;
							helmet_fang = helmet_tang;
							break;
						}

					case CAM_HELMET:
						if (get_p5_sw_current() & P_DOWN) {
							pcam->csi.mode = CAM_NORMAL_A;
							break;
						}

					case CAM_NORMAL_A:
						if (get_p5_sw_current() & P_DOWN) {
							pcam->csi.mode = CAM_NORMAL_SP1;
							break;
						}

					case CAM_NORMAL_SP1:
						if (get_p5_sw_current() & P_DOWN) {
							pcam->csi.mode = CAM_NORMAL_SP2;
							break;
						}

					case CAM_NORMAL_SP2:
#endif //SPECIAL_MODES
						pcam->csi.mode = CAM_NORMAL;
						break;
				}
			}

#ifdef STICK_CAM_ON
		}
#endif //STICK_CAM_ON
		}
#endif //CAMERA_BT
#ifdef DBG_KO
		}
#endif //DBG_KO

		if (game_info.game_mode == GM_PLAY_OVER ||
			game_info.game_mode == GM_GAME_OVER)
		{
#ifdef CAMCRAP
			if (pcam->was_playover != PLAYOVER_TICKS) printf("was_playover = %d   %s\n",PLAYOVER_TICKS,(game_info.game_flags & GF_QUICK_KICK) ? "GF_QUICK_KICK":"\0");
#endif //CAMCRAP
			pcam->was_playover = PLAYOVER_TICKS;
		}
		else
		if (pcam->was_playover)
		{
			pcam->was_playover--;
#ifdef CAMCRAP
			printf("sleep - was_playover: %d   game_mode: %d   %s\n",pcam->was_playover,game_info.game_mode,(game_info.game_flags & GF_QUICK_KICK) ? "GF_QUICK_KICK":"\0");
#endif //CAMCRAP
		}

#ifdef DEBUG
		assert( cambot.csi.mode >= NMODE_FG );
		assert( cambot.csi.mode <= CAM_NORMAL_SP2 );
#endif
		sleep(1);

#ifdef DEBUG
		assert( cambot.csi.mode >= NMODE_FG );
		assert( cambot.csi.mode <= CAM_NORMAL_SP2 );
#endif
	}
}

/******************************************************************************
** Determine cambot positioning info for valid objects
*/
#define XTGT_OFFSCRN	(1.0f * (hres/hres/2/FOCFAC))	//0.90f
#define YTGT_OFFSCRN	(1.0f * (vres/hres/2/FOCFAC))	//0.90f

#define MODE_QB_DFAC	(0.40f)			// Higher = go further away as QB goes to sideline
#define MODE_QB_ZFAC	(0.68f* 0.82f)	//0.90f Higher = quicker pan to sideline
#define SNAP_RATIO		(0.75f)			//0.60f Higher = slower pan to off screen target

static void TrackObjects(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register game_info_t *pgi = &game_info;
	register ball_data *pbo = &ball_obj;
	register fbplyr_data *ppd;
	register float fxbc,fzbc,fda,fdb;
	register int player_valid = 0;
	register int player_count = 0;
	register int pnum = NUM_PLAYERS;
	register int carrier;
	float fx,fz;

	// Init <cambotinfo>
	*pci = cambotinfo_init;

	fxbc = (fzbc = 0.0f);

	if ((carrier = pgi->ball_carrier) >= 0 ||
			((carrier = pbo->int_receiver) >= 0 &&
			pgi->play_result == PR_INCOMPLETE &&
			pgi->game_flags & GF_PLAY_ENDING))
			//pbo->flags & BF_BOUNCE))
	{
		// Set player as primary tracking point
		ppd = &player_blocks[carrier];
		pci->carrier_mode = ppd->odata.plyrmode;
		fxbc = ppd->odata.x;
		fzbc = ppd->odata.z;
		// Set player distance to his goal line
		carrier_goal_dist = FIELDHZ + fxbc * (ppd->team * 2 - 1);
		// Clr off-screen target flag
//if (pbo->flags & BF_TGT_OFFSCRN) {
//	printf("RESET RESET RESET RESET RESET RESET RESET RESET RESET: %d\n",pbo->flags & ~BF_TGT_OFFSCRN);
//	while (!(get_dip_coin_close() & SLAM_SW));
//}
		pbo->flags &= ~BF_TGT_OFFSCRN;
	}
	else
	{
//delete_multiple_strings(0xabe, 0xffffffff);
//set_text_transparency_mode(TRANSPARENCY_ENABLE);
//set_text_font(FONTID_BAST10);
//set_string_id(0xabe);
//set_text_position(200, 72, 1.2f);
//oprintf("%dj%dcPxz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 60, 1.2f);
//oprintf("%dj%dcPyz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 48, 1.2f);
//oprintf("%dj%dcPZ", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 36, 1.2f);
//oprintf("%dj%dcBxz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 24, 1.2f);
//oprintf("%dj%dcByz", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//set_text_position(200, 12, 1.2f);
//oprintf("%dj%dcBZ", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW);
//
//set_text_position(230, 72, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(player_blocks[pbo->int_receiver].tsx / player_blocks[pbo->int_receiver].tsz) > XTGT_OFFSCRN) ? LT_RED:LT_YELLOW), player_blocks[pbo->int_receiver].tsx / player_blocks[pbo->int_receiver].tsz);
//set_text_position(230, 60, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(player_blocks[pbo->int_receiver].tsy / player_blocks[pbo->int_receiver].tsz) > YTGT_OFFSCRN) ? LT_RED:LT_YELLOW), player_blocks[pbo->int_receiver].tsy / player_blocks[pbo->int_receiver].tsz);
//set_text_position(230, 48, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, player_blocks[pbo->int_receiver].tsz);
//set_text_position(230, 36, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(pbo->tsx / pbo->tsz) > XTGT_OFFSCRN) ? LT_RED:LT_YELLOW), pbo->tsx / pbo->tsz);
//set_text_position(230, 24, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), ((fabs(pbo->tsy / pbo->tsz) > YTGT_OFFSCRN) ? LT_RED:LT_YELLOW), pbo->tsy / pbo->tsz);
//set_text_position(230, 12, 1.2f);
//oprintf("%dj%dc%f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, pbo->tsz);

		carrier = -1;
		if (!(pgi->game_flags & GF_QUICK_KICK && pcam->kickoff_flag) &&
				!(pgi->game_flags & GF_HIKING_BALL))
		{
			// Assume loop should process the ball/on-screen pass target
			pnum++;

			// Is it an as yet unretouched pass?
			if (pbo->type == LB_PASS && !(pbo->flags & BF_BOUNCE))
					//!(pgi->game_flags & (GF_POSS_CHANGE)))
			{
				// Assume loop should process an on-screen pass target
				pnum++;

				// Chk if target is off-screen if it wasn't before
				if ( !(pbo->flags & BF_TGT_OFFSCRN) && pbo->flight_time < 2 && (
						player_blocks[pbo->int_receiver].tsz < 10.0f ||
						pbo->tsz < 10.0f ||
						(fabs(player_blocks[pbo->int_receiver].tsy / player_blocks[pbo->int_receiver].tsz) > YTGT_OFFSCRN &&
						 fabs(pbo->tsy / pbo->tsz) > YTGT_OFFSCRN)
						||
						(fabs(player_blocks[pbo->int_receiver].tsx / player_blocks[pbo->int_receiver].tsz) > XTGT_OFFSCRN &&
						 fabs(pbo->tsx / pbo->tsz) > XTGT_OFFSCRN) ))
				{
					// Set off-screen target flag
					pbo->flags |= BF_TGT_OFFSCRN;
//printf("SET   SET   SET   SET   SET   SET   SET   SET   SET  : %d\n",pbo->flags);
//while (!(get_dip_coin_close() & SLAM_SW));
				}
				// Treat as on- or off-screen pass?
				if (pbo->flags & BF_TGT_OFFSCRN)
				{
					// Make loop process the off-screen pass target
					pnum ++;
					pnum ++;
				}
			}
		}
	}

//{
//	static last_flags;
//	if (last_flags != pbo->flags)
//	{
//		static char * crap[] = {"BF_WHITE","BF_VEL_PITCH","BF_TURBO_PASS","BF_FIRE_PASS","BF_BOBBLE","BF_BOUNCE","BF_ALT","BF_FREEZE","BF_TGT_OFFSCRN"};
//		int i = 0;
//		last_flags = pbo->flags;
//		while (last_flags)
//		{
//			if (last_flags & 1)
//				printf("%s  ",crap[i]);
//
//			last_flags >>= 1;
//			i++;
//		}
//		printf("\ntsx= %f  tsz= %f  irx= %f  irz= %f\n",pbo->tsx, pbo->tsz, player_blocks[pbo->int_receiver].tsx, player_blocks[pbo->int_receiver].tsz);
//		last_flags = pbo->flags;
//	}
//}

	while (--pnum >= 0)
	{
		if (pnum == (NUM_PLAYERS + 3))
		{
			// Do ball for the off-screen pass target
			fxbc = ball_obj.odata.x - ball_obj.tx;
			fzbc = ball_obj.odata.z - ball_obj.tz;
			fxbc *= SNAP_RATIO;
			fzbc *= SNAP_RATIO;
			fxbc += ball_obj.tx;
			fzbc += ball_obj.tz;

			if (FIELDX(ball_obj.tx) < pgi->los) fzbc *= MODE_QB_ZFAC;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);

			pci->carrier_mode = (NOBALLCARRIER * 2);
		}
		else
		if (pnum == (NUM_PLAYERS + 2))
		{
			// Do the off-screen pass target
			fxbc = ball_obj.tx;
			fzbc = ball_obj.tz;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);

			pnum = NUM_PLAYERS;	//!!!skip to the players
		}
		else
		if (pnum == (NUM_PLAYERS + 1))
		{
			// Do unretouched ball for the on-screen pass target
			fxbc = ball_obj.odata.x;
			fzbc = ball_obj.odata.z;

			if (FIELDX(ball_obj.tx) < pgi->los) fzbc *= MODE_QB_ZFAC;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);

			pnum = NUM_PLAYERS;	//!!!skip to the players
		}
		else
		if (pnum == NUM_PLAYERS)
		{
			// Do ball for the on-screen pass target
			fxbc = ball_obj.odata.x;
			fzbc = ball_obj.odata.z;

			// Rotate to the Z axis
			fx = DOROTCAMX(fxbc,fzbc);
			fz = DOROTCAMZ(fxbc,fzbc);
		}
		else
		{
			ppd = &player_blocks[pnum];
			fda = ppd->odata.x;
			fdb = ppd->odata.z;
			player_valid <<= 1;

			if (pgi->play_type == PT_KICKOFF)
			{
				if (pnum != carrier)
				{
					if (carrier >= 0) continue;
					if (pgi->game_flags & GF_QUICK_KICK)
					{
						if (pnum != 3 && pnum != 10) continue;		//3 & 10 for kick rcvrs
					}
					else
					{
						if (pnum != 0 && pnum != 7) continue;		//0 & 7 for kickers
					}
					//if (game_info.game_flags & GF_QUICK_KICK) {
						if (ppd->team != pgi->last_pos) continue;
					//}
					//else {
					//	if (ppd->team == pgi->last_pos) continue;
					//}
				}
			}
			else
			if (pgi->game_mode == GM_PLAY_OVER)
			{
				// Watch only the ball carrier
				if (pnum != carrier)
				{
					if (carrier >= 0) continue;
					if (pnum != 3 && pnum != 10) continue;
					if (ppd->team != pgi->last_pos) continue;
					if (!(pgi->game_flags & GF_BALL_KICKED)) continue;
				}
			}
			else
			if (pgi->game_mode == GM_LINING_UP ||
					 pgi->game_mode == GM_PRE_SNAP)
			{
				// Watch only the offense before the snap
				if (pgi->last_pos != ppd->team) continue;
			}
			else
			if (pci->carrier_mode == MODE_QB)
			{
				// Watch only the ball carrier
				if (pnum != carrier) continue;
				// Do Z percentage to make the field move sooner
				//  Probably only good for the behind view
				fdb *= MODE_QB_ZFAC;
			}
			else
			if (pci->carrier_mode > NOBALLCARRIER)
			{
				// Watch only the ball carrier
				if (pnum != carrier) continue;
			}
			else
			if (pgi->game_flags & GF_HIKING_BALL)
			{
				// Watch only the QB
				if (pnum != 3 && pnum != 10) continue;				//3 & 10 for qbs
				if (ppd->team != pgi->last_pos) continue;
			}
			else
			{
				continue;
			}
			// If got here, its time to process a player coordinate
			// Count # of valid plyrs
			player_valid++;
			player_count++;
			// Rotate to the Z axis
			pci->frot_x[pnum] = fx = DOROTCAMX(fda,fdb);
			pci->frot_z[pnum] = fz = DOROTCAMZ(fda,fdb);
		}
		// Process a valid coordinate
		// Make sure <fda> & <fdb> stay available after the rotate
		// Do X delta for phi-biased Z-axis intercepts; keep min/max intercepts
		fda = fz / 2.0f * pcam->cos_phi;
		if ((fdb = fx + fda) < pci->frot_dxmin) pci->frot_dxmin = fdb;
		if ((fdb = fx - fda) > pci->frot_dxmax) pci->frot_dxmax = fdb;
		// Keep min/max rotated Z for Z-based min distance
		if (fz < pci->frot_zmin) pci->frot_zmin = fz;
		if (fz > pci->frot_zmax) pci->frot_zmax = fz;
	} // End of while

	pci->player_valid = player_valid;
	pci->player_count = player_count;

	// Do default positioning
	// Rotated Y target
	pci->fb = pcam->b;

	if (pgi->play_type != PT_KICKOFF &&
			(pgi->game_mode == GM_LINING_UP || pgi->game_mode == GM_PRE_SNAP))
	{
		// Rotated X target is QB rotated X
		pci->fa = pci->frot_x[pgi->last_pos*NUM_PERTEAM+PPOS_QBACK];
		// Rotated Z target is rotated LOS
		pci->fc = DOROTCAMZ(WORLDX(pgi->los),0);

		// Calc intercept-to-target X delta
		fda = pci->frot_dxmax - pci->fa;
		fdb = pci->fa - pci->frot_dxmin;
		if (fdb > fda) fda = fdb;
		// Set Z/X ratio value
		fdb = FIX_FDX_PS;
	}
	else
	{
		// Rotated X target is mid of min/max X intercepts
		pci->fa = (pci->frot_dxmin + pci->frot_dxmax) / 2;
		// Rotated Z target is phi-biased from mid of min/max Z
		//   Goes from min-to-mid Z as phi goes from ground-to-overhead
		pci->fc = pci->frot_zmin - ((pci->frot_zmax - pci->frot_zmin) / 2 * pcam->sin_phi);

		// Calc intercept-to-target X delta
		fda = pci->frot_dxmax - pci->fa;
		// Set Z/X ratio value
		fdb = FIX_FDX;
	}

	// Do X-based min distance; use Y=mX+b on slope <m> [cos_phi/2],
	//   rotated Z target <X> [fc], & Z-axis intercept <b> -to-target
	//   X delta [fda]; take the ABS() times the Z/X ratio [fdb]
	//   to get the distance
	pci->fdx = fabs(
			(pci->fc * pcam->cos_phi / 2.0f) + fda
		) * fdb;

	// Do Z-based min distance; use Law of Sines on angle <A> [ALPHA],
	//   opposite side <a> [Ztrg-Zmin], & 2nd angle <B> [DELTA]
	//   to do 2nd opposite side <b> times buffer-factor [FIX_FDZ]
	//   to get the distance
	pci->fdz = (
			(pci->fc - pci->frot_zmin) *
			(sin_delta = fsin(DELTA + pcam->phi)) /
			(sin_alpha)
		) * FIX_FDZ;

#ifdef SHOWDISTANCES
{
	char * t = (pci->fdx < pci->fdz) ? "Z":"X";
	delete_multiple_strings(0xab, 0xffffffff);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_font(FONTID_BAST10);
	set_string_id(0xab);
	set_text_position(150, 368, 1.2f);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, t);
	set_text_position(170, 368, 1.2f);
	oprintf("%dj%dcfdx %.2f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, pci->fdx);
	set_text_position(170, 354, 1.2f);
	oprintf("%dj%dcfdz %.2f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, pci->fdz);
}
#endif //SHOWDISTANCES

}

/******************************************************************************
** DoSpline()
**  Standard Bezier algorithm
*/
static void DoSpline(VERTEX * pv, SPLINE * ps, int cur_tick, int tot_tick)
{
	register float k1, k2, k3, k4, u1;

	k2 = (float)cur_tick / (float)tot_tick;		// u
	k3 = 1.0f - k2;								// 1-u
	k4 = k2 * k2;								// u^2
	u1 = k3;									// 1-u

	k1  = k3 * k3;								//     (1-u)^2
	k3 *= k4 * 3.0f;							// 3u^2(1-u)
	k4 *= k2;									//  u^3
	k2 *= k1 * 3.0f;							//   3u(1-u)^2
	k1 *= u1;									//     (1-u)^3

	pv->x = ps->x1 * k1 + ps->x2 * k2 + ps->x3 * k3 + ps->x4 * k4;
	pv->y = ps->y1 * k1 + ps->y2 * k2 + ps->y3 * k3 + ps->y4 * k4;
	pv->z = ps->z1 * k1 + ps->z2 * k2 + ps->z3 * k3 + ps->z4 * k4;
}

/******************************************************************************
** accelerate/decelerate
**   Call with cur_tick = 0 to do setup calc;
**    not meant to have accels used on cur_tick = 0 as this
**    would actually add 1 to tot_tick
*/
static void DoAccelDecel(int cur_tick, int tot_tick)
{
	register VIEWCOORD *pcam = &cambot;
	register CAMBOTINFO *pci = &cambotinfo;
	register float fs,ft;
	register int ts;

	// Kill acceleration after <tot_tick> ticks, then exit
	if (cur_tick > tot_tick) {
		pcam->xa = pcam->ya = pcam->za = pcam->da = pcam->ta = pcam->pa = 0.0f;
		return;
	}

	// Nothing to do if <tot_tick> is < 2
	if (tot_tick < 2)
		return;

	// If <cur_tick> is 0, do calc for half the distance in half the time
	if (!cur_tick) {
		fs = 4.0f;
		ts = tot_tick;
		// Start from a stand-still
		pcam->xv = pcam->yv = pcam->zv = pcam->dv = pcam->tv = pcam->pv = 0.0f;
	}
	else {
		// Calc the halfway tick #; is also the tick cnt to use for the
		//  accel recalc after halfway thru an odd-tick case
		ts = tot_tick / 2 + 1;
		// At or just passed halfway?
		if (cur_tick != ts)
			return;
		// Doing an odd-tick case?
		if (tot_tick & 1)
			// Odd; just passed halfway
			fs = -2.0f;
		else {
			// Not; at halfway
			pcam->xa = -pcam->xa;
			pcam->ya = -pcam->ya;
			pcam->za = -pcam->za;
			pcam->da = -pcam->da;

			pcam->ta = -pcam->ta;
			pcam->pa = -pcam->pa;
			return;
		}
	}
	fs /= (float)(ts * ts);
	pcam->xa = fs * (pci->fa - pcam->a);
	pcam->ya = fs * (pci->fb - pcam->b);
	pcam->za = fs * (pci->fc - pcam->c);
	pcam->da = fs * (pci->fd - pcam->d);

	ft = pcam->theta_trg - pcam->theta;
	if (ft >   THETA_MAX / 2) ft -= THETA_MAX;
	if (ft <= -THETA_MAX / 2) ft += THETA_MAX;
	pcam->ta = fs * ft;
	pcam->pa = fs * (pcam->phi_trg - pcam->phi);
}

/******************************************************************************
**
*/
static void SetModeAngleTrgs(VIEWCOORD * pcam, int mode)
{
	register float theta = (game_info.last_pos) ? NORMAL_THETA_OPP : NORMAL_THETA;
	register float phi   = NORMAL_PHI;

	// Set mode angle targets
	switch (mode) {
#ifdef SPECIAL_MODES
		case CAM_NORMAL_A:
			phi   = NORMAL_PHI_A;
		case CAM_NORMAL_SP1:
		case CAM_NORMAL_SP2:
#endif //SPECIAL_MODES
		case CAM_NORMAL:
			break;

		case CAM_SIDE:
			theta = SIDE_THETA;
			phi   = SIDE_PHI;
			break;

		case CAM_TOP:
			theta = (game_info.last_pos) ? TOP_THETA_OPP : TOP_THETA;
			phi   = TOP_PHI;
			break;

		default:
			return;
	}
	pcam->theta_trg = theta;
	pcam->phi_trg   = phi;
}

/*****************************************************************************/
static int ModeRevert(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	// Revert to previous mode
	// Called when transients expire
	register int flag = (pcam->csi.mode = pcam->mode_prev);
	if (pcam->csi.trav_cnt < 2) {
		// Unsnap angles from snap mode
		SetModeAngleTrgs(pcam, flag);
		set_cambot_theta(pcam->theta_trg);
		set_cambot_phi(pcam->phi_trg);
	}
	flag = cambot_funcs[flag](pcam, pci);
	if (pcam->csi.trav_cnt < 2) {
		flag |= SNAPTARG | MOVEVIEW;
	}
	mode_trgy_adjust = 0.0f;
	return flag;
}

/*****************************************************************************/
#if 0		// Used by replays
static void * select_ptr( void * p )
{
	// Count # of table entries & select a random entry in that range
	register int count = 0;
	while ( ((int *)p)[count] ) count++;
	(int *)p += randrng(count);

	return (void *)((int *)p)[0];
};
#endif

/*****************************************************************************/
// Primary Game Camera
//  CAM_NORMAL rear 3/4 view 

#ifdef SPECIAL_MODES
#define SIDEPAN_PARTRNG	(FIELDW * 0.88f)
#define SIDEPAN_PARTPER	0.20f

#define SIDEPAN_FULLPER	0.15f
#endif //SPECIAL_MODES

static int cambot_normal(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static int amode_tog = 0;

	register game_info_t *pgi = &game_info;
	register float sidelim, dist, la, frot_z;
	register int hh = flg_hhteam;
	register int pb = flg_pullback;
	register int flag = UNROTATE | ACCLTARG | MOVETARG | MOVEVIEW;	// default

	sidelim = mode_parms[CAM_NORMAL][MP_SIDE][NQ][hh][pb];			// default

	TrackObjects(pcam,pci);

	SetModeAngleTrgs(pcam, pcam->csi.mode);

	pci->fb = TRGY_NORMAL;

	// Do goal-approach mods
	dist = carrier_goal_dist / 3.0f - NORMAL_DIST_GOAL;
	if (dist > 0.0f)
		dist = 0.0f;
	else if (dist < -NORMAL_DIST_GOAL)
		dist = -NORMAL_DIST_GOAL;
	pcam->phi_trg -= GR2RD(dist);

	// Do distance limits
	dist += mode_parms[CAM_NORMAL][MP_DIST][NQ][hh][pb];
	if ((pci->fd = pci->fdx) < pci->fdz) pci->fd = pci->fdz;
	if (pci->fd < dist) pci->fd = dist;

	// Set Z to use for sideline limiting
	frot_z = pci->fc;												// default

	// Do any special setups
	switch (pgi->game_mode)
	{
		case GM_LINING_UP:
		case GM_PRE_SNAP:
			if (pgi->game_flags != GF_QUICK_KICK)
			{
//				// Do pre-snap max distance
//				if (pci->fd > DIST_MAX_NORMAL_PS)
//					pci->fd = DIST_MAX_NORMAL_PS;
//
//				if (pci->fd < DIST_QB_NORMAL)
//					pci->fd = DIST_QB_NORMAL;

				// Currently don't want any pre-snap distancing
				pci->fd = mode_parms[CAM_NORMAL][MP_DIST][QB][hh][pb];

				if (pgi->play_type == PT_KICKOFF)
					// Offset rotated Z target
					pci->fc += ZOFF_NORMAL_ONSIDE;
			}

			if (pcam->was_playover)
			{
				// Set the kickoff flag if about to do a quick kickoff
				if (pgi->play_type == PT_KICKOFF && pgi->game_flags & GF_QUICK_KICK)
					pcam->kickoff_flag = 1;
#ifdef CAMCRAP
				printf("%s - %s - was_playover: %d\n  theta: %f  phi: %f   %s\n",
					((pcam->was_playover > 1) ? "GOPHER" : "SNAPTARG"),
					mtxt[pgi->game_mode],
					pcam->was_playover,
					pcam->theta/M_PI*180.0f,
					pcam->phi/M_PI*180.0f,
					(pgi->game_flags & GF_QUICK_KICK) ? "GF_QUICK_KICK":"\0");
#endif //CAMCRAP
				// Chk & set gopher if its time for a clean transition
				if (pcam->was_playover > 1)
				{
					// Use gopher to snap the camera for alignment next time
					if (pcam->theta != pcam->theta_trg)
						set_cambot_theta(pcam->theta_trg);
					if (pcam->phi   != pcam->phi_trg)
						set_cambot_phi(pcam->phi_trg);
					return cambot_funcs[CAM_GOPHER](pcam, pci);
				}
				// Did gopher - chk if we want to hop into CAM_ATTRACT
				if (pgi->play_type != PT_KICKOFF
						&& pgi->game_state == GS_ATTRACT_MODE
						//&& (amode_tog ^= 1)
						)
				{
					cambot.csi.mode = CAM_ATTRACT;
					return cambot_funcs[CAM_ATTRACT](pcam, pci);
				}
				// Nuthin' do'in - now just snap-to so things start where we want'em
				flag = UNROTATE | SNAPTARG | MOVEVIEW;
			}
			// Reset odds flag for random GM_PLAY_OVER ob tracking
			show_sideline = -1;
			break;

		case GM_IN_PLAY:
		case GM_PRE_KICKOFF:
			if ((pci->carrier_mode == MODE_QB) ||
				(pgi->ball_carrier < 0 && ball_obj.flight_time &&
				!(pgi->game_flags & GF_QUICK_KICK) &&
				ball_obj.type != LB_FUMBLE))
			{
				// Offset rotated Z target & lock distance
				pci->fc += ZOFF_NORMAL_QB;
				pci->fd  = mode_parms[CAM_NORMAL][MP_DIST][QB][hh][pb];

				if (pci->carrier_mode == MODE_QB)
				{
					// Set Z to use for sideline limiting
					frot_z = pci->frot_z[pgi->ball_carrier];
#ifdef MODE_QB_DFAC
					pci->fd += fabs(player_blocks[pgi->ball_carrier].odata.z) * MODE_QB_DFAC;
				}
				else
				if (pci->carrier_mode == (NOBALLCARRIER * 2))
				{
					pci->fd += fabs(ball_obj.odata.z) * MODE_QB_DFAC;
					pci->p_accel = accel_def_nobc;
#endif //MODE_QB_DFAC
				}
				sidelim = 0.0f;
			}
			// Fall thru
		default:
			// Do backfield limits
			dist = pci->fd;
			la = 0.0f;
			if (pcam->sin_phi)
			{
				// Add linear distance to ground-target per target Y [fb];
				//   subtract since <sin_phi> will always be negative
				dist -= (la = pci->fb / pcam->sin_phi);
				// Calc ground-target offset from target Z [fc]
				la *= pcam->cos_phi;
			}
			// Calc ground distance from target Z to lower Z extreme &
			//  push target up if goes below extreme
			dist = dist * sin_alpha / sin_delta + la;
			if ((pci->fc - dist) < -NORMAL_BACKLIM)
				pci->fc = dist - NORMAL_BACKLIM;

			// Do sideline limits
			if (pgi->game_mode == GM_PLAY_OVER || pgi->game_mode == GM_GAME_OVER)
			{
				if (show_sideline < 0)
					// Since 0 disallows sidelines, odds will be 1 of (n)
					show_sideline = randrng(3);

				if (!0)//show_sideline)
				{
					if (pgi->ball_carrier >= 0)
						frot_z = pci->frot_x[pgi->ball_carrier];
					frot_z = fabs(frot_z * 1.1f);
					if (sidelim < frot_z)
						sidelim = frot_z;
				}
			}
			// Do default or position-based sideline limit? Default if != 0
			if (!sidelim)
				sidelim = mode_parms[CAM_NORMAL][MP_SIDE]
					[!!(pci->carrier_mode == MODE_QB || pci->carrier_mode == (NOBALLCARRIER * 2))]
					[hh][pb] + pcam->cos_phi * (pci->fc - frot_z) * 0.5f;

			dist = pci->fd / 2.0f;

			if (dist > sidelim) pci->fa = 0.0f;
			else if ((pci->fa - dist) < -sidelim) pci->fa = dist - sidelim;
			else if ((pci->fa + dist) >  sidelim) pci->fa = sidelim - dist;

#ifdef SPECIAL_MODES
			// Do submodes
			if (pcam->csi.mode != CAM_NORMAL && pcam->csi.mode != CAM_NORMAL_A)
			{
				if (pgi->ball_carrier >= 0)
				{
					la = SIDEPAN_FULLPER;
					dist = player_blocks[pgi->ball_carrier].odata.z;
					if (pcam->csi.mode == CAM_NORMAL_SP1)
					{
						la = SIDEPAN_PARTPER;
						if ((dist -= (SIDEPAN_PARTRNG/2)) <= 0.0f &&
								(dist += SIDEPAN_PARTRNG) >= 0.0f)
							dist = 0.0f;
					}
					pcam->theta_trg -= GR2RD(dist * la * (pgi->last_pos * 2 - 1));
				}
			}
#endif //SPECIAL_MODES
			break;
	}
	return flag;
}

/*****************************************************************************/
// Primary Game Camera
//  CAM_SIDE side view 
static int cambot_side(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register float dist_min;
	register int flag = ACCLTARG | MOVETARG | MOVEVIEW;

	TrackObjects(pcam,pci);

	SetModeAngleTrgs(pcam, pcam->csi.mode);

	// Do any special setups
	switch (game_info.game_mode) {
		case GM_LINING_UP:
		case GM_PRE_SNAP:
			// Set unrotated target/distance
			pci->fa = WORLDX(game_info.los);
			pci->fb = TRGY_SIDE;
			pci->fc = 20.0f;
			pci->fd = 150.0f;
			break;

		default:
			pci->fb = TRGY_SIDE;
			// Do goal-approach mods
			dist_min = carrier_goal_dist / 3.0f - SIDE_DIST_GOAL;
			if (dist_min > 0.0f)
				dist_min = 0.0f;
			else if (dist_min < -SIDE_DIST_GOAL)
				dist_min = -SIDE_DIST_GOAL;
			pcam->phi_trg -= GR2RD(dist_min);

			// Do distance limits
			dist_min += SIDE_DIST_MIN;
			if ((pci->fd = pci->fdx) < pci->fdz) pci->fd = pci->fdz;
			if (pci->fd < dist_min) pci->fd = dist_min;
			flag |= UNROTATE;
			break;
	}
	return flag;
}

/*****************************************************************************/
// Primary Game Camera
//  CAM_TOP top-down view 
static int cambot_top(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register float dist_min;
	register int flag = ACCLTARG | MOVETARG | MOVEVIEW;

	TrackObjects(pcam,pci);

	SetModeAngleTrgs(pcam, pcam->csi.mode);

	// Do any special setups
	switch (game_info.game_mode) {
		case GM_LINING_UP:
		case GM_PRE_SNAP:
			// Set unrotated target/distance
			pci->fa = WORLDX(game_info.los);
			pci->fb = TRGY_TOP;
			pci->fc = 0.0f;
			pci->fd = 190.0f;
			break;

		default:
			pci->fb = TRGY_TOP;
			// Do goal-approach mods
			dist_min = carrier_goal_dist / 3.0f - TOP_DIST_GOAL;
			if (dist_min > 0.0f)
				dist_min = 0.0f;
			else if (dist_min < -TOP_DIST_GOAL)
				dist_min = -TOP_DIST_GOAL;
			pcam->phi_trg -= GR2RD(dist_min) * 4.0f;	//augment pan by 4x

			// Do distance limits
			dist_min += TOP_DIST_MIN;
//			pci->fdz *= 0.85f;
			if ((pci->fd = pci->fdx) < pci->fdz) pci->fd = pci->fdz;
			if (pci->fd < dist_min) pci->fd = dist_min;
			flag |= UNROTATE;

//			dist_min = pci->fd/60.0f;		//patchy attempt to pull camera
//			pci->fd -= dist_min*dist_min;	// closer as distance gets large

			break;
	}
	return flag;
}

/*****************************************************************************/
// Attract mode camera
//  CAM_ATTRACT
static int cambot_attract(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register game_info_t *pgi = &game_info;
	register obj_3d * pobj;

	// Pick obj to watch
	pobj = (obj_3d *)((pgi->ball_carrier >= 0) ?
			&(player_blocks[pgi->ball_carrier].odata) : &(ball_obj.odata));

	// Set default track/accel for this mode
	pci->p_track = track_def;
	pci->p_accel = accel_attract;

	if (pcam->was_playover && pgi->game_mode == GM_PRE_SNAP) {
		pcam->s.x1 = pci->fa = pobj->x;
		pcam->s.y1 = pci->fb = pobj->y;
		pcam->s.z1 = pci->fc = pobj->z;
		pci->fd = (float)randrng((int)(25.0f*YD2UN)) + 12.0f*YD2UN;

		pcam->theta_trg =  DG2RD(randrng(THETA_MAX_DG));
		pcam->phi_trg   = -DG2RD(randrng(40) + 20);

		pcam->s.x2 = pcam->s.y2 = pcam->s.z2 = 0.0f;

		return SNAPTARG | MOVEVIEW;
	}

	if (!(pcam->s.x2) && !(pcam->s.y2) && !(pcam->s.z2)) {
		pcam->s.x2 = pcam->x;
		pcam->s.y2 = pcam->y;
		pcam->s.z2 = pcam->z;
	}

	pcam->x = pcam->s.x2 + (pcam->a - pcam->s.x1) * 0.5f;
	pcam->y = pcam->s.y2 + (pcam->b - pcam->s.y1) * 0.5f;
	pcam->z = pcam->s.z2 + (pcam->c - pcam->s.z1) * 0.5f;

	pcam->theta_trg = pcam->theta;

	pci->fa = pobj->x;
	pci->fb = pobj->y;
	pci->fc = pobj->z;

	return ACCLTPOS | MOVETPOS | NEWTHETA;
}

/*****************************************************************************/
// Zoom the viewpoint to the specified position from <pmodeobj>
//  camera will snap if <trav_cnt> <2
//  CAM_ZOOM
//  CAM_ZO_NR (no ranging)
//  CAM_ZO_NFR (no facing, no ranging)
static int cambot_zoom(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register obj_3d * pobj = (obj_3d *)pcam->pmodeobj;
	register float fa, fb, fc;
	register int m;
	register int flag = SNAPTARG | MOVEVIEW;

	// Calc current-to-last target diffs so we can follow the target
	fa = pobj->x - pci->fa;
	fb = pobj->y + TRGY_ZOOM + mode_trgy_adjust - pci->fb;
	fc = pobj->z - pci->fc;

	// Set unrotated target/distance
	pci->fa = pobj->x;
	pci->fb = pobj->y + TRGY_ZOOM + mode_trgy_adjust;
	pci->fc = pobj->z;
	pci->fd = pcam->csi.d;

	if ((unsigned int)(pcam->tick_cnt) >= (unsigned int)(pcam->csi.mode_cnt)) {
		// Revert to previous mode
		flag = ModeRevert(pcam, pci);
	}
	else if (!pcam->tick_cnt) {
		// Set facing-relative theta
		pcam->csi.theta += (pcam->csi.mode != CAM_ZO_NFR) ?
				GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
		pcam->csi.theta = ValidateTheta(pcam->csi.theta);

		// Does this sub-mode want d-theta limits?
		if (pcam->csi.mode == CAM_ZOOM) {
			// Do corrected-ranging for current target to desired target diff
			fa = pcam->csi.theta - pcam->theta_trg;
			fc = THETA_MAX / 2;
			if (fa <= -fc) fa += fc + fc;
			if (fa >=  fc) fa -= fc + fc;

			// Do +/- 90 deg swing limit
			fc = THETA_MAX / 4;
			if (fa >  fc) pcam->csi.theta -= 2.0f * (fa - fc);
			if (fa < -fc) pcam->csi.theta -= 2.0f * (fa + fc);
		}

		if (pcam->csi.trav_cnt < 2) {
			// Snap mode
			set_cambot_theta(pcam->csi.theta);
			set_cambot_phi(pcam->csi.phi);
		}
		else {
			// Zoom mode
			// Do initial accels to target
			pcam->theta_trg = pcam->csi.theta;
			pcam->phi_trg   = pcam->csi.phi;
			DoAccelDecel(0,pcam->csi.trav_cnt);
			// Blow off tick 0 of accelerations
			flag = MOVEVIEW;
		}
	}
	else {
		if (pcam->csi.trav_cnt < 2) {
			// Snap mode
			pcam->a += fa;
			pcam->b += fb;
			pcam->c += fc;
		}
		else {
			// Zoom mode
			// Follow the target
			pcam->a += fa;
			pcam->b += fb;
			pcam->c += fc;

			DoAccelDecel(pcam->tick_cnt,pcam->csi.trav_cnt);
			flag = MOVETARG | MOVEVIEW;
		}
	}

	pcam->tick_cnt++;

	return flag;
}

/*****************************************************************************/
// Attach the viewpoint to a 2-point Float Spline in <*pseqinfo>
//  spline is projected over a period of <trav_cnt> ticks
//  starting point is the current viewpoint
//  ending point is specified by the CAMSEQINFO offsets from target
//  points are assumed to have been rotated & normalized to the Z-axis
//    such that pt1 would have been (0,0,0) & pt4 would have been (0,0,1)
//  points are rotated, in XZ & Y, per angles formed by start & end points
//  points are ratioed, in XZ & Y, per distance between start & end points
//  points are translated, in XZ & Y, per position of start & end points
//  CAM_SPLINE initial theta is offset (limited) by <pmodeobj.fwd> (facing);
//         spline is stretched to follow <pmodeobj> target
//  CAM_SP_NFR initial theta is offset by current <theta_trg> (no facing,
//         no ranging); spline is stretched to follow <pmodeobj> target
//  CAM_SP_CNFR initial theta is offset by current <theta_trg> (no facing,
//         no ranging); uses <cambot> as the target & watches <pmodeobj>
static int cambot_spline(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float trgy;
	register obj_3d * pobj = (obj_3d *)pcam->pmodeobj;
	register float tsin, tcos, fd1, fd2;

	if ((unsigned int)(pcam->tick_cnt) >= (unsigned int)(pcam->csi.mode_cnt)) {
		// Revert to previous mode
		return ModeRevert(pcam, pci);
	}
	else if (!pcam->tick_cnt) {
		// 1st time here; setup spline matrix
		register float * pfs = (float *)pcam->csi.pseqinfo;
		float fdx, fdy, fdz;

		// Set facing-relative theta
		pcam->csi.theta += (pcam->csi.mode == CAM_SPLINE) ?
				GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
		pcam->csi.theta = ValidateTheta(pcam->csi.theta);

		// Does this sub-mode want d-theta limits?
		if (pcam->csi.mode == CAM_SPLINE) {
			// Do corrected-ranging for current target to desired target diff
			fd1 = pcam->csi.theta - pcam->theta_trg;
			fd2 = THETA_MAX / 2;
			if (fd1 <= -fd2) fd1 += fd2 + fd2;
			if (fd1 >=  fd2) fd1 -= fd2 + fd2;

			// Do +/- 90 deg swing limit
			fd2 = THETA_MAX / 4;
			if (fd1 < -fd2) pcam->csi.theta -= 2.0f * (fd1 + fd2);
			if (fd1 >  fd2) pcam->csi.theta -= 2.0f * (fd1 - fd2);
		}

		// Do spline destination pt4 & starting pt1 & deltas
		fd1 = fcos(pcam->csi.phi) * (fd2 = pcam->csi.d);

		if (pcam->csi.mode == CAM_SP_CNFR) {
			pcam->s.x4 = pcam->a;
			pcam->s.y4 = pcam->b;
			pcam->s.z4 = pcam->c;

			// Init focus pt & distance
			pcam->a = pobj->x;
			pcam->b = pobj->y;
			pcam->c = pobj->z;
			pcam->d = pcam->csi.d;
		}
		else {
			pcam->s.x4 = pobj->x;
			pcam->s.y4 = pobj->y;
			pcam->s.z4 = pobj->z;
		}
		trgy = pcam->s.y4;

		fdx = (pcam->s.x4 -= fsin(pcam->csi.theta) * fd1) - (pcam->s.x1 = pcam->x);
		fdy = (pcam->s.y4 -= fsin(pcam->csi.phi) * fd2)   - (pcam->s.y1 = pcam->y);
		fdz = (pcam->s.z4 -= fcos(pcam->csi.theta) * fd1) - (pcam->s.z1 = pcam->z);

		fd1 = fsqrt((fd2 = fdx * fdx + fdz * fdz) + fdy * fdy);
		fd2 = fsqrt(fd2);

		// Rotate in X
		tsin = fdy / fd1;
		tcos = fd2 / fd1;
		pcam->s.y2 = pfs[2] * tsin + pfs[1] * tcos;
		pcam->s.z2 = pfs[2] * tcos - pfs[1] * tsin;
		pcam->s.y3 = pfs[5] * tsin + pfs[4] * tcos;
		pcam->s.z3 = pfs[5] * tcos - pfs[4] * tsin;

		// Rotate in Y
		tsin = fdx / fd2;
		tcos = fdz / fd2;
		fd2 = pfs[0];
		pcam->s.x2 = pcam->s.z2 * tsin + fd2 * tcos;
		pcam->s.z2 = pcam->s.z2 * tcos - fd2 * tsin;
		fd2 = pfs[3];
		pcam->s.x3 = pcam->s.z3 * tsin + fd2 * tcos;
		pcam->s.z3 = pcam->s.z3 * tcos - fd2 * tsin;

		// Scale & translate
		pcam->s.x2 = pcam->s.x2 * fd1 + pcam->s.x1;
		pcam->s.y2 = pcam->s.y2 * fd1 + pcam->s.y1;
		pcam->s.z2 = pcam->s.z2 * fd1 + pcam->s.z1;
		pcam->s.x3 = pcam->s.x3 * fd1 + pcam->s.x1;
		pcam->s.y3 = pcam->s.y3 * fd1 + pcam->s.y1;
		pcam->s.z3 = pcam->s.z3 * fd1 + pcam->s.z1;

		// Clear accelerations we care about
		pcam->xa = pcam->ya = pcam->za = pcam->da = 0.0f;
	}

	if (pcam->csi.mode != CAM_SP_CNFR) {
		// Stretch the spline to follow any object movement
		fd1 = pcam->xv;
		fd2 = pcam->zv;
		pcam->s.x4 += (fd1);
		pcam->s.z4 += (fd2);
		pcam->s.x3 += (fd1 * (2.0f/3.0f));
		pcam->s.z3 += (fd2 * (2.0f/3.0f));
		pcam->s.x2 += (fd1 * (1.0f/3.0f));
		pcam->s.z2 += (fd2 * (1.0f/3.0f));

		// Set default track/accel for this mode
		pci->p_track = track_def;
		pci->p_accel = accel_def;

		pcam->b = pobj->y + TRGY_SPLINE + mode_trgy_adjust;
	}
	else {
		// Set default track/accel for this mode
		pci->p_track = track_p2p;
		pci->p_accel = accel_p2p;

		pcam->b = pobj->y * 0.4f;

		// Disallow Y from going less than starting target Y
		if (pcam->b < (trgy + mode_trgy_adjust))
			pcam->b =  trgy + mode_trgy_adjust;
	}

	// Update target
	// Follow Y realtime while doing buffered acceleration for X & Z
	pci->fa = pobj->x;
	pci->fb = pcam->b;
	pci->fc = pobj->z;
	pci->fd = pcam->d;

	if (pcam->tick_cnt <= pcam->csi.trav_cnt)
		DoSpline((VERTEX *)&(pcam->x), &(pcam->s), pcam->tick_cnt, pcam->csi.trav_cnt);

	pcam->tick_cnt++;

	return ACCLTPOS | MOVETPOS | NEWTHETA;
}

/*****************************************************************************/
// Attach the viewpoint to a 4-point Rail Spline in <*pseqinfo>
//  spline is projected over a period of <trav_cnt> ticks
//  points are assumed to have been rotated to the Z-axis
//  points are rotated, in Y, per initial theta angle
//  points are ratioed, in XZ & Y, per specified <csi.d>
//  points are translated, in XZ & Y, to initial <pmodeobj> viewpoint target
//  CAM_RAIL initial theta is offset by <pmodeobj.fwd> (facing)
//  CAM_RA_NF initial theta is offset by current <theta_trg> (no facing)
//  CAM_RA_NFT initial theta is offset by current <theta_trg> (no facing);
//         offsets from the world-origin while it watches the <pmodeobj>
//         viewpoint target (no tracking)
static int cambot_rail(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register obj_3d * pobj = (obj_3d *)pcam->pmodeobj;

	if ((unsigned int)(pcam->tick_cnt) >= (unsigned int)(pcam->csi.mode_cnt)) {
		// Revert to previous mode
		return ModeRevert(pcam, pci);
	}
	else if (!pcam->tick_cnt) {
		// 1st time here; setup spline matrix
		register float * pfs = (float *)pcam->csi.pseqinfo;
		register float * pfc = (float *)&pcam->s.x1;
		register float tsin, tcos, fx, fz, fd;
		register int i;

		// Set facing-relative theta
		pcam->csi.theta += (pcam->csi.mode == CAM_RAIL) ?
				GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
		pcam->csi.theta = ValidateTheta(pcam->csi.theta);

		tsin = fsin(pcam->csi.theta);
		tcos = fcos(pcam->csi.theta);
		fd = pcam->csi.d;

		// Rotate in Y
		i = 4;
		while (i--) {
			fx = pfs[0];
			fz = pfs[2];
			pfc[0] = fd * (fz * tsin + fx * tcos);
			pfc[1] = fd * (pfs[1]);
			pfc[2] = fd * (fz * tcos - fx * tsin);
			if (pcam->csi.mode != CAM_RA_NFT) {
				pfc[0] += pobj->x;
				pfc[1] += pobj->y;
				pfc[2] += pobj->z;
			}
			pfs += 3;
			pfc += 3;
		}
	}

	// Update target
	pcam->a = pobj->x;
	pcam->b = pobj->y + TRGY_RAIL;
	pcam->c = pobj->z;

	if (pcam->tick_cnt <= pcam->csi.trav_cnt)
		DoSpline((VERTEX *)&(pcam->x), &(pcam->s), pcam->tick_cnt, pcam->csi.trav_cnt);
//	if (pcam->tick_cnt == pcam->csi.trav_cnt)
//		pcam->s.y1 = pcam->s.y2 = pcam->s.y3 = pcam->s.y4;
//	DoSpline((VERTEX *)&(pcam->x), &(pcam->s), pcam->tick_cnt, pcam->csi.trav_cnt);

	pcam->tick_cnt++;

	return NEWTHETA;
}

/*****************************************************************************/
// Play back a Coordinate Data point-stream from <*pseqinfo>
//  0,0,0 signifies the end of the stream
//  points are assumed to have been rotated to the Z-axis
//  points are rotated, in Y, per initial theta angle
//  points are ratioed, in XZ & Y, per specified <csi.d>
//  CAM_PATH initial theta is offset by <pmodeobj.fwd> (facing);
//         follows the <pmodeobj> viewpoint target
//  CAM_PA_NF initial theta is offset by current <theta_trg> (no facing);
//         follows the <pmodeobj> viewpoint target
//  CAM_PA_NFT initial theta is offset by current <theta_trg> (no facing)
//         offsets from the world-origin while it watches the <pmodeobj>
//         viewpoint target (no tracking)
static int cambot_path(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float v_sin, v_cos;

	while (!pcam->csi.mode_cnt ||
			(unsigned int)(pcam->tick_cnt) < (unsigned int)(pcam->csi.mode_cnt)) {
		register obj_3d * pobj;
		register float * pf = (float *)pcam->csi.pseqinfo;
		register float fa = pf[0];
		register float fb = pf[1];
		register float fc = pf[2];
		register float fd;

		if (!fa && !fb && !fc) break;

		pobj = (obj_3d *)pcam->pmodeobj;

		if (!pcam->tick_cnt) {
			// Do first time setup
			// Set facing-relative theta
			pcam->csi.theta += (pcam->csi.mode == CAM_PATH) ?
					GR2RD((float)(pobj->fwd)) : pcam->theta_trg;
			pcam->csi.theta = ValidateTheta(pcam->csi.theta);

			v_sin = fsin(pcam->csi.theta);
			v_cos = fcos(pcam->csi.theta);
		}
		// Unrotate to new angle
		fd = fa;
		fa = fc * v_sin + fd * v_cos;
		fc = fc * v_cos - fd * v_sin;

		fd = pcam->csi.d;

		if (pcam->csi.mode == CAM_PA_NFT) {
			pcam->a = pobj->x;
			pcam->b = pobj->y * 0.5f;
			pcam->c = pobj->z;
			pcam->x = fa * fd;
			pcam->y = fb * fd;
			pcam->z = fc * fd;
		}
		else {
			pcam->x = fa * fd + (pcam->a = pobj->x);
			pcam->y = fb * fd + (pcam->b = pobj->y);
			pcam->z = fc * fd + (pcam->c = pobj->z);
		}
		pcam->b += TRGY_PATH;

		pcam->csi.pseqinfo = pf + 3;
		pcam->tick_cnt++;

		return NEWTHETA;
	}

	// Revert to previous mode
	pcam->csi.trav_cnt = 0;
	return ModeRevert(pcam, pci);
}

/*****************************************************************************/
// Play back a Coordinate Data point-stream from <*pseqinfo>
//  0,0,0 signifies the end of the stream & would be the viewpoint target
//  points are rotated, in Y, to the Z-axis
//  points are rotated, in Y, per initial angle of the viewpoint to target
//  points are ratioed, in XZ & Y, per initial distances from viewpoint to target
//  point projection results in 1st point being at the current viewpoint
//  CAM_COURSE uses as the target & follows <pmodeobj>
//  CAM_CO_C uses <cambot> as the target & follows <pmodeobj>
//  CAM_CO_CNT uses <cambot> as the target (no tracking)
static int cambot_course(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float n_sin, n_cos;
	static float v_sin, v_cos;
	static float drat_h, drat_y;

	while (!pcam->csi.mode_cnt ||
			(unsigned int)(pcam->tick_cnt) < (unsigned int)(pcam->csi.mode_cnt)) {
		register obj_3d * pobj;
		register float * pf = (float *)pcam->csi.pseqinfo;
		register float fa = pf[0];
		register float fb = pf[1];
		register float fc = pf[2];
		register float fd, fe;

		if (!fa && !fb && !fc) break;

		pobj = (obj_3d *)pcam->pmodeobj;

		if (!pcam->tick_cnt) {
			// Do first time setup
			n_sin = fa * (fd = rsqrt(fa * fa + fc * fc));
			n_cos = fc * fd;

			if (pcam->csi.mode == CAM_COURSE) {
				pcam->a = pobj->x;
				pcam->b = pobj->y;
				pcam->c = pobj->z;
			}
			if (pcam->csi.mode != CAM_CO_CNT) {
				pci->fa = pobj->x;
				pci->fb = pobj->y;
				pci->fc = pobj->z;
			}
			fe = fsqrt(((fe = v_sin = pcam->x - pcam->a) * fe) +
					   ((fe = v_cos = pcam->z - pcam->c) * fe));
			v_sin /= fe;
			v_cos /= fe;

			drat_h = fe * fd;
			drat_y = fabs((pcam->y - pcam->b) / fb);

			// Set up to quickly compensate for any position offset between the
			//  object & the initial camera focus, making the object the focus
			pci->fb += TRGY_COURSE;	//pcam->csi.d;
			DoAccelDecel(0, 50);
			pci->fb -= TRGY_COURSE;	//pcam->csi.d;

			pcam->s.x1 = pcam->s.y1 = pcam->s.z1 = 0.0f;
		}
		else {
			if (pcam->csi.mode != CAM_CO_CNT) {
				// Track object new position offsets
				BuffAccel(P_S_X1, pobj->x - pci->fa, TRACK_DEFAULT, ACCEL_DEFAULT);
				BuffAccel(P_S_Y1, pobj->y - pci->fb, TRACK_DEFAULT, ACCEL_DEFAULT);
				BuffAccel(P_S_Z1, pobj->z - pci->fc, TRACK_DEFAULT, ACCEL_DEFAULT);
				pcam->s.x1 *= DECEL_DEFAULT;
				pcam->s.y1 *= DECEL_DEFAULT;
				pcam->s.z1 *= DECEL_DEFAULT;
				pci->fa += pcam->s.x1;
				pci->fb += pcam->s.y1;
				pci->fc += pcam->s.z1;

				// Update focus with initial compensation & any new offset
				DoAccelDecel(pcam->tick_cnt, 50);
				pcam->a += (pcam->xv += pcam->xa);
				pcam->b += (pcam->yv += pcam->ya);
				pcam->c += (pcam->zv += pcam->za);

				pcam->a += pcam->s.x1;
				pcam->b += pcam->s.y1;
				pcam->c += pcam->s.z1;
			}
		}
		// Rotate to Z
		fd = fa;
		fa = fd * n_cos - fc * n_sin;
		fc = fd * n_sin + fc * n_cos;

		// Unrotate to new angle
		fd = fa;
		fa = fc * v_sin + fd * v_cos;
		fc = fc * v_cos - fd * v_sin;

		pcam->x = fa * drat_h + pcam->a;
		pcam->y = fb * drat_y + pcam->b;
		pcam->z = fc * drat_h + pcam->c;

		pcam->csi.pseqinfo = pf + 3;
		pcam->tick_cnt++;

		return NEWTHETA;
	}
	// Revert to previous mode
	pcam->csi.trav_cnt = 0;
	return ModeRevert(pcam, pci);
}

/*****************************************************************************/
// Play back a Normalized Data point-stream from <*pseqinfo>
//  0,0,0 signifies the end of the stream
//  points are assumed to have been rotated & normalized to the Z-axis
//    such that 1st pt (deleted) would have been (0,0,0) & last will be (0,0,x)
//  points are rotated, in XZ & Y, per angles formed by start & end points
//  points are ratioed, in XZ & Y, per initial distance from viewpoint to target
//  point projection results in 1st point being at the current viewpoint
//  CAM_P2P uses <cambot> as the target & watches <pmodeobj>
static int cambot_p2p(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	static float t_sin, t_cos;
	static float p_sin, p_cos;
	static float drat;

	while (!pcam->csi.mode_cnt ||
			(unsigned int)(pcam->tick_cnt) < (unsigned int)(pcam->csi.mode_cnt)) {
		register obj_3d * pobj = (obj_3d *)pcam->pmodeobj;
		register float * pf = (float *)pcam->csi.pseqinfo;
		register float fa = pf[0];
		register float fb = pf[1];
		register float fc = pf[2];
		register float fd, fe;

		if (pobj == &ball_obj.odata && game_info.ball_carrier >= 0)
			pobj = &(player_blocks[game_info.ball_carrier].odata);

		if (!fa && !fb && !fc) {
			// If <mode_cnt> is specified, the viewpoint remains where it
			//  was last while still tracking the target
			if (!pcam->csi.mode_cnt)
				break;
		}
		else {
			if (!pcam->tick_cnt) {
				register int cnt;
				// Do first time setup
				// Set theta for destination pt calc
				pcam->csi.theta = ValidateTheta((pcam->csi.theta += pcam->theta_trg));

				// Do destination pt & deltas; save target pt; save starting pt for later translations
				fe = (fd = pcam->csi.d) * fcos(pcam->csi.phi);
				fa = ((pcam->s.x2 = pcam->a) - fe * fsin(pcam->csi.theta)) - (pcam->s.x1 = pcam->x);
				fb = ((pcam->s.y2 = pcam->b) - fd * fsin(pcam->csi.phi))   - (pcam->s.y1 = pcam->y);
				fc = ((pcam->s.z2 = pcam->c) - fe * fcos(pcam->csi.theta)) - (pcam->s.z1 = pcam->z);

				fd = fsqrt((fe = fa * fa + fc * fc) + fb * fb);
				fe = fsqrt(fe);

				// Set trigs for rotate in X
				p_sin = fb / fd;
				p_cos = fe / fd;

				// Set trigs for rotate in Y
				t_sin = fa / fe;
				t_cos = fc / fe;

				// Find last pt; abort after <cnt> pts
				cnt = 2000;
				while ((pf[0] || pf[1] || pf[2]) && --cnt)
					pf += 3;
				if (!cnt)
					break;
				// We "know" X & Y should be 0 so just use Z for unit equivalence
				pf--;
				drat = fd / *pf;

				// Init focus pt & distance
				pcam->a = pobj->x;
				pcam->b = pobj->y;
				pcam->c = pobj->z;
				pcam->d = pcam->csi.d;

				// Clear accelerations we care about
				pcam->xa = pcam->ya = pcam->za = pcam->da = 0.0f;

				// "Retrieve" 1st normalized pt
				fa = fb = fc = 0.0f;
			}
			else
				pcam->csi.pseqinfo = pf + 3;

			// Set default track/accel for this mode
			pci->p_track = track_p2p;
			pci->p_accel = accel_p2p;

			// Unrotate in X
			fd = fb;
			fb = fc * p_sin + fd * p_cos;
			fc = fc * p_cos - fd * p_sin;
			// Unrotate in Y
			fd = fa;
			fa = fc * t_sin + fd * t_cos;
			fc = fc * t_cos - fd * t_sin;

			pcam->x = fa * drat + pcam->s.x1;
			pcam->y = fb * drat + pcam->s.y1;
			pcam->z = fc * drat + pcam->s.z1;
		}

		// Update target
		pci->fa = pobj->x;
		pci->fb = pobj->y * 0.4f;
		pci->fc = pobj->z;

		// Disallow Y from going less than starting target Y
		if (pci->fb < pcam->s.y2) pci->fb = pcam->s.y2;
		if (pcam->b < pcam->s.y2) pcam->b = pcam->s.y2;

		pcam->tick_cnt++;

		return ACCLTPOS | MOVETPOS | NEWTHETA;
	}

	// Revert to previous mode
	pcam->csi.trav_cnt = 2;
	pcam->dv = pcam->xv = pcam->yv = pcam->zv = pcam->da = pcam->xa = pcam->ya = pcam->za = 0.0f;
/*{
int flag;
fprintf(stderr,"th= %f  ph= %f\r\n\r\n",pcam->theta,pcam->phi);
fprintf(stderr," x= %f   y= %f   z= %f\r\n\r\n",pcam->x,pcam->y,pcam->z);
fprintf(stderr," a= %f   b= %f   c= %f   d= %f\r\n",pcam->a,pcam->b,pcam->c,pcam->d);
fprintf(stderr,"ta= %f  tb= %f  tc= %f  td= %f\r\n\r\n",pcam->s.x2,pcam->s.y2,pcam->s.z2,pcam->csi.d);
fprintf(stderr,"xv= %f  yv= %f  zv= %f  dv= %f\r\n",pcam->xv,pcam->yv,pcam->zv,pcam->dv);
fprintf(stderr,"xa= %f  ya= %f  za= %f  da= %f\r\n\r\n",pcam->xa,pcam->ya,pcam->za,pcam->da);
	flag = ModeRevert(pcam, pci);
fprintf(stderr," a= %f   b= %f   c= %f   d= %f\r\n",pci->fa,pci->fb,pci->fc,pci->fd);
fprintf(stderr,"th= %f  ph= %f\r\n\r\n",pcam->theta,pcam->phi);
	return(flag);
}*/
	return ModeRevert(pcam, pci);
}

/*****************************************************************************/
// Put the camera undergound, looking down
//  CAM_GOPHER
static int cambot_gopher(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	// Set unrotated target/distance
	pcam->x = -1000.0f;
	pcam->y = -10.0f;
	pcam->z = 0.0f;

	pcam->a = -900.0f;
	pcam->b = -75.0f;
	pcam->c = 0.0f;

	return DOBYPASS;
}

/*****************************************************************************/
// Resume previous camera mode
//  CAM_RESUME
static int cambot_resume(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	register int flag;

	TrackObjects(pcam,pci);

	switch (pcam->mode_prev) {
		case CAM_ZOOM:
		case CAM_ZO_NR:
		case CAM_ZO_NFR:
		case CAM_SPLINE:
		case CAM_SP_NFR:
		case CAM_SP_CNFR:
		case CAM_RAIL:
		case CAM_RA_NF:
		case CAM_RA_NFT:
		case CAM_PATH:
		case CAM_PA_NF:
		case CAM_PA_NFT:
		case CAM_COURSE:
		case CAM_CO_C:
		case CAM_CO_CNT:
		case CAM_P2P:
			flag = cambot_funcs[pcam->csi.mode = pcam->mode_prev](pcam, pci);
			flag |= SNAPTARG | MOVEVIEW;
			set_cambot_theta(pcam->theta_trg);
			set_cambot_phi(pcam->phi_trg);
			mode_trgy_adjust = 0.0f;
			break;

		case CAM_NORMAL:
		case CAM_SIDE:
		case CAM_TOP:
		case CAM_ATTRACT:
		case CAM_GOPHER:
#ifdef SPECIAL_MODES
		case CAM_HELMET:
		case CAM_NORM:
		case CAM_NORMAL_A:
		case CAM_NORMAL_SP1:
		case CAM_NORMAL_SP2:
#endif //SPECIAL_MODES
		default:
			flag = cambot_funcs[pcam->csi.mode = pcam->mode_prev](pcam, pci);
			flag |= SNAPTARG | MOVEVIEW;
			set_cambot_theta(pcam->theta_trg);
			set_cambot_phi(pcam->phi_trg);
			mode_trgy_adjust = 0.0f;
			break;
	}
	return flag;
}

#ifdef SPECIAL_MODES
/*****************************************************************************/
// "Helmet" camera
//  CAM_HELMET

#define DIFF_MAX 20

static int cambot_helmet(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	obj_3d * pobj;
	int pnum,diff;

	SetModeAngleTrgs(pcam, CAM_NORMAL);

	// Determine player # to track
	pnum = PPOS_QBACK;
	if (game_info.last_pos) pnum += NUM_PERTEAM;
	if (game_info.game_mode == GM_IN_PLAY) {
		if ((pnum = game_info.ball_carrier) < 0)
			pnum = ball_obj.int_receiver;
	}
	pobj = &player_blocks[pnum].odata;

	pcam->x = pobj->x;
	pcam->y = pobj->y + 15.0f;
	pcam->z = pobj->z;

	helmet_tang = pobj->fwd & (1024-1);
	diff = helmet_tang - helmet_fang;
	if (diff) {
		if (diff < -512)
			diff += 1024;
		else if (diff > 512)
			diff -= 1024;
		if (diff < -DIFF_MAX)
			diff = -DIFF_MAX;
		else if (diff > DIFF_MAX)
			diff = DIFF_MAX;
		helmet_fang += diff;
	}
	helmet_fang &= (1024-1);
	pcam->a = pcam->x + 10.0f * isin(helmet_fang);
	pcam->b = pcam->y - 2.0f;
	pcam->c = pcam->z + 10.0f * icos(helmet_fang);

	return DOBYPASS;
}

/*****************************************************************************/
// Alternate "normal" camera
//  CAM_NORM
static int cambot_norm(VIEWCOORD * pcam, CAMBOTINFO * pci)
{
	float fx;
	int pnum;

	// Determine player # to track
	pnum = PPOS_QBACK;
	if (game_info.last_pos) pnum += NUM_PERTEAM;
	if (game_info.game_mode == GM_IN_PLAY) {
		if ((pnum = game_info.ball_carrier) < 0)
			pnum = ball_obj.int_receiver;
	}

	pcam->a = player_blocks[pnum].odata.x;
	pcam->b = 0.0f;
	pcam->c = player_blocks[pnum].odata.z;
	pcam->d = 100.0f;

	if (pcam->d < NORMAL_DIST_MIN) pcam->d = NORMAL_DIST_MIN;

	fx = pcam->d / 2.0f;
	if (fx > NORMAL_SIDELIM) pcam->c = 0.0f;
	else if ((pcam->c - fx) < -NORMAL_SIDELIM) pcam->c = fx - NORMAL_SIDELIM;
	else if ((pcam->c + fx) >  NORMAL_SIDELIM) pcam->c = NORMAL_SIDELIM - fx;

	pcam->x = pcam->a - pcam->d * pcam->cos_phi * pcam->sin_theta;
	pcam->y = pcam->b - pcam->d * pcam->sin_phi;
	pcam->z = pcam->c - pcam->d * pcam->cos_phi * pcam->cos_theta;

	return DOBYPASS;
}
#endif //SPECIAL_MODES

/*****************************************************************************/
// Special-case camera controls/modifiers
//
#define FG_XOFF		(2.5f *YD2UN)		// X offset when at the lineup

#define POP_PER_FUMBLE	3				// 1 in (x) chance of fumble replay

static int cambot_special( VIEWCOORD * pcam, CAMBOTINFO * pci, int flag )
{
#ifndef NOSETCAM
	register game_info_t * pgi = &game_info;
	register ball_data * pball = &ball_obj;

	static POP_INFO * ppi;
	static CAMSEQINFO * pcsi;
	static VERTEX event = {0.0f, 0.0f, 0.0f};
	static int pop_btn;
	static int pop_cnt;
	static int pop_dly;
	static int pop_flg;
	static int pop_itm;
	static int gip_cnt;
	static int gpo_cnt;

	// Chk special camera for initial QB idiot box
	if (idiot_flag < 1)
	{
		static obj_3d obj_qb_idiot;
		static VIEWCOORD cam;
		static CAMSEQINFO csi_qb_idiot = {
			CAM_ZO_NR, 10000, 0, DG2RD(180.0f), DG2RD(-5.0f), 300.0f, NULL
		};

		if (idiot_boxes_shown < 0)
		{
			idiot_flag = idiot_boxes_shown;

			if (pcam->csi.mode != CAM_ZO_NR)
			{
				register obj_3d * pobj = &obj_qb_idiot;
				int flg;

				cam = *pcam;
				SetModeAngleTrgs(&cam, cam.csi.mode);
				flg = cambot_funcs[cam.csi.mode](&cam, pci);

				if (flg & UNROTATE) {
					register float fa = pci->fa;
					pci->fa = UNROTCAMX(fa,pci->fc);
					//pci->fb = pci->fb;
					pci->fc = UNROTCAMZ(fa,pci->fc);
				}

				pobj->x   = pcam->a = pci->fa;
				pobj->y   = pcam->b = pci->fb;
				pobj->z   = pcam->c = pci->fc;
				pobj->fwd = 0;

				cam.d = pci->fd;
				mode_trgy_adjust = -TRGY_ZOOM;

				pcam->csi      = csi_qb_idiot;
				pcam->pmodeobj = pobj;
				pcam->tick_cnt = 0;
			}
		}
		if (idiot_flag < 0)
		{
			pcam->theta_trg += DG2RD(0.25);

			if (idiot_boxes_shown > 0)
			{
				pcam->csi.mode     = CAM_ZO_NR;
				pcam->csi.mode_cnt = pcam->csi.trav_cnt = 30;
				pcam->csi.theta    = cam.theta_trg;
				pcam->csi.phi      = cam.phi_trg;
				pcam->csi.d        = cam.d;
				pcam->tick_cnt     = 0;

				idiot_flag = idiot_boxes_shown;
			}
		}
	}

	// Do special camera for possession changes; works only if other procs
	//  don't use <suspend> during GM_IN_PLAY/GM_PLAY_OVER else contention
	//  is possible
	if (pgi->game_mode == GM_IN_PLAY || pgi->game_mode == GM_PLAY_OVER)
	{
		static int tout;
		int msgnum = -1;

		if (pcam->off_prev != pgi->last_pos)
		{
			if (pcam->off_prev < 0)
			{
				// Wait for message to timeout before turning everyone back on
				if (!existp(MSG_CAMBOT_PID,0xFFFFFFFF) || !(--tout))
				{
					resume_all();
					pcam->off_prev = (pgi->game_mode == GM_IN_PLAY) ?
							pgi->last_pos : pgi->off_side;
				}
			}
			else if (pgi->game_mode == GM_IN_PLAY)
			{
				if (!(pball->flags & BF_BOUNCE) &&
						(pball->type == LB_PASS ||
						 pball->type == LB_BOBBLE ||
						 pball->type == LB_LATERAL))
				{
					msgnum = MS_INTERCEPTION;
				}
				else if (pball->type == LB_FUMBLE ||
						 pball->type == LB_LATERAL)
				{
					msgnum = MS_TURNOVER2;
				}
				else if (((pgi->game_flags & (GF_BOBBLE | GF_BALL_PUNTED | GF_FUMBLE)) == GF_BALL_PUNTED) &&
						(pgi->game_state != GS_ATTRACT_MODE))
				{
					msgnum = MS_PUNTRETURN;
				}
				else
				{
					pcam->off_prev = pgi->last_pos;
				}
			}
		}
		// Chk very special camera for interception at play over
		else if (pgi->game_mode == GM_PLAY_OVER)
		{
			if (pcam->off_prev != pgi->off_side)
			{
				if (!(pball->flags & BF_BOUNCE) &&
						pgi->play_result != PR_FAILED_CONVERSION &&
						!pgi->off_turnover &&
						(pball->type == LB_PASS ||
						 pball->type == LB_BOBBLE ||
						 pball->type == LB_LATERAL))
				{
					msgnum = MS_INTERCEPTION;
				}
				else
				{
					pcam->off_prev = pgi->off_side;
				}
			}
		}
		if (msgnum >= 0)
		{
			suspend_multiple_processes(0, NODESTRUCT);
			resume_multiple_processes(PLYRINFO_PID, 0xfffffffe);	// don't look at low bit
			resume_multiple_processes(POST_PLAYER_PID, -1);

			qcreate("message", MSG_CAMBOT_PID, show_message_proc, msgnum, 0, 0, 0);

			sleep(40);

			tout = 6 * 57;

			pcam->off_prev = -1;
		}
	}
	else
	{
		pcam->off_prev = pgi->last_pos;
	}

	// Do special camera for field goal
	{
		static vec3d base, targ;
		static int time;
		static int mode;

		if (pgi->team_play[pgi->last_pos]->flags & (PLF_FIELD_GOAL|PLF_FAKE_FG))// &&
//			pgi->game_state != GS_ATTRACT_MODE)
		{
			if (!mode)
			{
				if (pgi->game_mode == GM_PRE_SNAP ||
					(pgi->team_play[pgi->last_pos]->flags & PLF_FIELD_GOAL &&
					!(pgi->game_flags & GF_BALL_KICKED)))
				{
					if (pgi->game_mode == GM_PRE_SNAP)
					{
						base.x = WORLDX(pgi->los) + (pgi->last_pos ? FG_XOFF : -FG_XOFF);
						base.y = 0.0f;
						base.z = pci->fc;
						base.w = pci->fd;
					}
					*(vec3d *)&pci->fa = base;
				}
				else
				if (pgi->team_play[pgi->last_pos]->flags & PLF_FIELD_GOAL)
				{
					targ.w = fabs((pgi->last_pos ? -GOAL_X : GOAL_X) - base.x);
					time = (int)(targ.w / fabs(pball->odata.vx) + 0.5f);

					select_csi_item(csi_fs_fgtrk);
					pcam->csi.trav_cnt = time;

					pcam->a = pgi->last_pos ? -GOAL_X:GOAL_X;
					pcam->b = (GOAL_TOP+GOAL_Y)*0.5f;
					pcam->c = 0;
					mode_trgy_adjust = -pcam->b;

					pcam->pmodeobj = &targ;
					mode = 1;
				}
			}
			if (mode == 1)
			{
				if (pball->flags & BF_DEFLECTED)
				{
					flag = ModeRevert(pcam,pci);
					mode = -1;
				}
				else
				{
					if (!(pball->flags & BF_FG_BOUNCE))
					{
						targ.x = pball->odata.x - pball->ox + base.x;
						targ.y = pball->odata.y / 0.4f * 0.7f;
						targ.z = pball->odata.z - pball->oz + base.z;
					}
//					if ((pcam->tick_cnt - pcam->csi.trav_cnt) > 30 && pop_flg == POP_IDLE)
					if ((pcam->tick_cnt - pcam->csi.trav_cnt) > 30)
					{
						select_csi_item(csi_zm_endplayfg);
						mode_trgy_adjust = 0;

						pcam->sin_phi = pcam->b - pcam->y;
						pcam->cos_phi = (pcam->a - pcam->x) * (pcam->a - pcam->x) +
										(pcam->c - pcam->z) * (pcam->c - pcam->z);
						pcam->d = fsqrt(pcam->sin_phi * pcam->sin_phi + pcam->cos_phi);
						pcam->sin_phi /= pcam->d;
						pcam->cos_phi  = fsqrt(pcam->cos_phi) / pcam->d;

						// Just to be sure...
						if (pcam->sin_phi < -1.0f) pcam->sin_phi = -1.0f;
						if (pcam->sin_phi >  1.0f) pcam->sin_phi =  1.0f;

						pcam->phi = asin(pcam->sin_phi);

						if ((mode = pgi->ball_carrier) < 0)
						{
							mode = pball->who_threw;
							pgi->last_carrier = mode;
						}
						cambot.pmodeobj = &player_blocks[mode].odata;
						mode = 2;
					}
				}
			}
			if (mode == 2)
			{
				if (pcam->tick_cnt >= pcam->csi.trav_cnt)
				{
					if (!select_csi_item(csi_fs_endplay))
						mode = -1;
					else
						mode++;
				}
			}
			if (mode > 0 && !pcam->tick_cnt)
				flag = cambot_funcs[pcam->csi.mode](pcam, pci);
		}
		if (pcam->was_playover == 1)
			mode = 0;
	}

#ifdef DEBUG
	// The Big Delay
	if (get_player_sw_current() & P3_D)
	{
		suspend_multiple_processes( 0, 0 );
		sleep(20);
		resume_multiple_processes( 0, 0 );

//N_RPLAY		if (playback) suspend_players();

		fprintf(stderr,"x:%f  y:%f  z:%f\r\n",cambot.x,cambot.y,cambot.z);
		fprintf(stderr,"a:%f  b:%f  c:%f\r\n",cambot.a,cambot.b,cambot.c);
		fprintf(stderr,"T:%f  P:%f  D:%f\r\n",cambot.theta,cambot.phi,cambot.d);
	}
#endif //DEBUG

//N_RPLAY
#if 0
	// Do instant replays
	{
		// Chk pre-snap conditions
		if (pgi->game_mode == GM_PRE_SNAP)
		{
			// Re-init stuff
			pcsi    = NULL;
			pop_flg = POP_IDLE;
			pop_dly = 0;
			gip_cnt = 0;
			gpo_cnt = 0;
		}
		else
		// Chk in-play conditions
		if (pgi->game_mode == GM_IN_PLAY && recorder)
		{
			gip_cnt++;

			// Chk for fumble event
			if (pgi->ball_carrier == NOBALLCARRIER &&
				pball->type == LB_FUMBLE)
			{
				if (pop_flg < POP_FUMBLE &&
					pball->flight_time < 2 &&
					!(attract_flag & (1<<POP_FUMBLE)) &&
					!randrng(POP_PER_FUMBLE))
				{
					if ((ppi = select_ptr(ppi_fumble)))
					{
						// New; set event vertex
						event.x = pball->odata.x;
						event.y = 0.0f;//pball->odata.y;
						event.z = pball->odata.z;

						pop_flg = POP_FUMBLE;
						pop_cnt = 0;
					}
				}
			}
			// Not a fumble; reset pop-replay if it was before
			else if (pop_flg == POP_FUMBLE) pop_flg = POP_IDLE;

			// Chk for missed fg hitting uprights
			if (pball->flags & BF_FG_HITPOST &&
				pgi->play_result != PR_FIELD_GOAL)
			{
				if (pop_flg < POP_FG_UPRIGHTS &&
					(pcount - pball->bounce_time < 2))
				{
					if ((ppi = select_ptr(ppi_fg_hitpost)))
					{
						// New; set event vertex
						event.x = pball->odata.x;
						event.y = pball->odata.y;
						event.z = pball->odata.z;

						pop_flg = POP_FG_UPRIGHTS;
						pop_cnt = 0;
					}
				}
			}
			// Not a missed fg; reset pop-replay if it was before
			else if (pop_flg == POP_FG_UPRIGHTS) pop_flg = POP_IDLE;
		}
		else
		// Chk play-over conditions
		if (pgi->game_mode == GM_PLAY_OVER && recorder)
		{
			gpo_cnt++;

			switch (pgi->play_result)
			{
				case PR_FAILED_CONVERSION:
					if (gpo_cnt >= 60 && pop_flg != POP_ACTIVE && !pcsi && 0)	//!!!
					{
						pcsi = select_csi_ptr(csi_replay1);
						pop_flg = POP_IDLE;
					}
					break;
			}
		}

#ifdef DAN
{static char * sz[]={"PASS","FMBL","PUNT","KOFF","FLDG","BOBL","LTRL",};
delete_multiple_strings(0xbeefface,-1);
set_string_id(0xbeefface);
set_text_font(FONTID_BAST10);
set_text_position(256,16,1.1f);
if (pball->type >= 0 && pball->type <= 6)oprintf("%dj%dc%s %d", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, sz[pball->type], pball->flight_time);
if (pop_flg == POP_FUMBLE){
set_text_position(256,6,1.1f);
oprintf("%dj%dcPF", (HJUST_CENTER|VJUST_CENTER), LT_CYAN);}}
#endif //DAN

		// Chk to inc the pop-replay post-event count
		if (pop_flg != POP_IDLE && pop_flg != POP_ACTIVE)
		{
			if (++pop_cnt >= ppi->ecnt)
			{
				// Do any required event-validation before
				// starting the pop-replay
				switch (pop_flg)
				{
					case POP_FUMBLE:
					{
						if (pgi->game_state != GS_ATTRACT_MODE &&
							pgi->game_mode  != GM_PLAY_OVER)
							//pgi->play_result != PR_FUMBLED_OOB)
						{
							pop_flg = POP_IDLE;
							break;
						}
					}
					if (0)
					case POP_FG_UPRIGHTS:
					{
						// Wait till it goes one way or the other
						if (pgi->play_result == PR_FIELD_GOAL)
						{
							pop_flg = POP_IDLE;
							break;
						}
						if (pgi->play_result != PR_MISSED_KICK)
						{
							pop_dly++;
							//fprintf(stderr,"-> %d\r\n",pop_dly);
							break;
						}
					}
					default:
					{
						int pmsk = 1 << (MAX_PLYRS - 1);
						int bmsk = (P_A|P_B|P_C) << ((MAX_PLYRS - 1) * 8);

						// Threshhold crossed; flag replay events
						if (pgi->game_state == GS_ATTRACT_MODE)
							attract_flag |= (1<<pop_flg);
						else
							inplay_flag |= (1<<pop_flg);

						// Start the pop-replay
						pcsi    = ppi->pcsi;
						pop_flg = POP_ACTIVE;
						pop_btn = 0;
						pop_cnt = 0;
						pop_itm = 0;

						// Make active button mask
						while (pmsk)
						{
							if (p_status & pmsk)
								pop_btn |= bmsk;
							bmsk >>= 8;
							pmsk >>= 1;
						}
					}
				}
			}
			else
			{
				pop_dly = 0;
			}
		}

		if (pop_flg == POP_ACTIVE && pop_cnt > 0)
		{
			if (--pop_cnt)
			{
				// Chk for <tick_cnt> of 2 since we did the new <cambot_funcs>
				// on setup & just now, a difference of only 1 realtime tick
				if (pcam->csi.trav_cnt && pcam->tick_cnt == 2)
				{
					add_sleep_process_id( REPLAY_PID, pcam->csi.trav_cnt );
					pop_cnt += pcam->csi.trav_cnt;
				}
				// Chk for button-out
				if (pcam->tick_cnt > 25 || pop_itm > 1)
				{
					if (get_player_sw_close() & pop_btn)
					{
						pcam->tick_cnt = pcam->csi.mode_cnt;
						pcam->csi.trav_cnt = 0;
						pop_cnt = 0;
						while (pcsi->mode != -1)
							pcsi++;
					}
				}
			}
		}

		if ((pop_flg == POP_ACTIVE && pop_cnt == 0) && pcsi->mode != -1)
		{
			// Do replay only if any delay is still within the tape limits
			if ((pop_dly + ppi->rcnt) < REPLAY_FRAMES)
				pop_cnt = set_playback_mode(RP_START,
					pop_dly + ppi->rcnt,
					pop_dly + 1,
					ppi->rdly);

			if ((pcsi->mode_cnt - pcsi->trav_cnt) == pop_cnt)
			{
				int idx, mode;

				// Set target obj ptr per PI spec
				// Will default to the ball if a player was specified
				// but that value was < 0
				pcam->pmodeobj = &event;		// assume default
				switch (ppi->tobj)
				{
					case POP_TOBJ_WHO_THREW:
						idx = pball->who_threw;
						if (0)					// skip next line
					case POP_TOBJ_INT_RECEIVER:
						idx = pball->int_receiver;
						if (idx < 0)			// do next line if invalid
					case POP_TOBJ_BALL:
						idx = BALL_IDX;
						pcam->pmodeobj = &replay_blocks[idx];
				}
				// Set target Y offset
				mode_trgy_adjust = ((float)(int)pcsi->pseqinfo) / TRGY_FAC;
				SetModeAngleTrgs(pcam, pcam->mode_prev);

				// Copy seq control data to cambot struct &
				// reset elapsed-time to start`er up
				pcam->csi = *pcsi;
				pcam->tick_cnt = 0;

				// Do mode specifics to setup for upcoming replay draw
				flag = cambot_funcs[pcam->csi.mode](pcam, pci);

				// Inc <mode_cnt> to make up for the <cambot_funcs> call
				pcam->csi.mode_cnt++;

				pcsi++;
				pop_itm++;
			}
			else
			{
				// Abort if desired # of ticks couldn't happen
				pop_flg = POP_ACTIVE;
				pop_cnt = 0;
			}
		}
		if (pop_flg == POP_ACTIVE && pop_cnt == 0)
		{
			// Revert should happen next tick
			// Doing it this way shows the 1st new frame with the replay
			//  camera; a <ModeRevert> from here would need to be unrotated
			set_playback_mode(RP_STOP, 0, 0, 0);
			set_recorder_mode(RR_START);

			pop_flg = POP_IDLE;
		}
	}
#endif // end of ripping out replay camera


#ifdef DAN
	{
		static int msgflg = FALSE;
		if (pball->type == LB_FUMBLE && pgi->ball_carrier < 0)
		{
			if (!msgflg)
			{
				set_text_font(FONTID_BAST10);
				set_text_transparency_mode(TRANSPARENCY_ENABLE);
				set_string_id(0xfbed);
				set_text_position(256, 32, 1.2f);
				oprintf("%dj%dcFUMBLE", (HJUST_CENTER|VJUST_CENTER), LT_CYAN);
				msgflg = TRUE;
			}
		}
		else if (msgflg)
		{
			delete_multiple_strings(0xfbed, 0xffffffff);
			msgflg = FALSE;
		}
	}
#endif //DAN

#ifndef NOKICKOFF
	// Do special camera for kickoff at start of game
	if (pcam->new_quarter && pcam->was_playover <= 1 && !pgi->game_quarter)
	{
		// Select a kickoff camseq; continue if successful
		if (select_csi_item(csi_kickoff))
		{
			KO_INFO * pki;

#ifdef DBG_KO
			if (pcam->new_quarter < 0)
				pcam->csi = csi_kickoff[-pcam->new_quarter - 1];
#endif //DBG_KO

			pki = pcam->csi.pseqinfo;

			pcam->x = (pgi->last_pos) ? pki->x : -pki->x;
			pcam->y = pki->y;
			pcam->z = (pgi->last_pos) ? pki->z : -pki->z;

			pcam->a = (pgi->last_pos) ? -331.0f : 331.0f;
			pcam->b = TRGY_NORMAL;
			pcam->c = 0.0f;

			// Set <auto_hike_cntr> delay, actual <pseqinfo> & obj ptr to watch
			pgi->auto_hike_cntr = pki->auto_hike_cntr;
			pcam->csi.pseqinfo  = pki->pseqinfo;
			pcam->pmodeobj      = (void *)&(pball->odata.x);

			pcam->csi.d = mode_parms[pcam->mode_prev][MP_DIST][NQ][!!flg_hhteam][!!flg_pullback];

			// Should be done needing to know about a new quarter &
			//  flag that its a kickoff
			pcam->new_quarter  = 0;
			pcam->kickoff_flag = 1;

			// Do mode specifics
			flag = cambot_funcs[pcam->csi.mode](pcam, pci);

#ifdef DBG_KO
			delete_multiple_strings(0xfdeb, 0xffffffff);
			if (!dbg_ko) {
				fprintf(stderr,"Cam\tx= %f\ty= %f\tz= %f\r\n",pcam->x,pcam->y,pcam->z);
				fprintf(stderr,"\ta= %f\tb= %f\tc= %f\r\n",pcam->a,pcam->b,pcam->c);
				fprintf(stderr,"Ball\tx= %f\ty= %f\tz= %f\r\n",ball_obj.odata.x,ball_obj.odata.y,ball_obj.odata.z);
			}
			else {
				int color = LT_GREEN;
#if STICK_CAM_SW == 4
				if (get_p5_sw_current() & STICK_CAM_BT)
#else
				if (get_player_sw_current() >> (STICK_CAM_SW * 8))
#endif //STICK_CAM_SW == 4
					color = WHITE;
				set_text_font(FONTID_BAST10);
				set_text_transparency_mode(TRANSPARENCY_ENABLE);
				set_string_id(0xfdeb);
				set_text_position(210, 192, 1.2f);
				oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, pki->szname);
				set_text_position(210, 180, 1.2f);
				oprintf("%dj%dcX: %dc%f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, color, pki->x);
				set_text_position(210, 168, 1.2f);
				oprintf("%dj%dcY: %dc%f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, color, pki->y);
				set_text_position(210, 156, 1.2f);
				oprintf("%dj%dcZ: %dc%f", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, color, pki->z);
				set_text_position(210, 144, 1.2f);
				oprintf("%dj%dcA: %dc%d", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, color, pki->auto_hike_cntr);
				set_text_position(210, 132, 1.2f);
				oprintf("%dj%dcT: %dc%d", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, color, pcam->csi.trav_cnt);
				set_text_position(210, 120, 1.2f);
				oprintf("%dj%dcM: %dc%d", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, color, pcam->csi.mode_cnt);
			}
#endif //DBG_KO
		}
	}
#endif //NOKICKOFF
#endif //NOSETCAM

#if 0
{
	#include "include/debris.h"
	static struct object_node * dobj = NULL;
	static debris_config_t dct = {
		0.0f, 0.0f, 0.0f,					// x,y,z
		0.0f, 0.0f, 0.0f,					// mx,my,mz
		0.1f, 0.0f,							// velocity,mvelocity
		0.1f, 0.1f,							// friction,gravity
		1024, 0, 1024,						// x_ang,y_ang,z_ang
		0, 0,								// num_particles,create_ticks
		NULL								// texture_list
	};
	if (!dobj)
	{
		dct.num_particles = randrng(40) + 40;
		dct.create_ticks  = randrng(6) + 2;

		dobj = make_debris(&dct);
	}
}
#endif

	return flag;
}
