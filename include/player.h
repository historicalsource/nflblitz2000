#ifndef PLAYER_H

#include "ani3D.h"

// replay flags/masks
#define TWOPART_CNT	2					// # of two-parters allowed
#define TWOPART_MSK	0x3					// bits to contain # of two-parters allowed
#define TWOPART_BIT	7					// position of first two-parter bit

#define R_INTTOSEQ	0x01
#define R_INTSTREAM	0x02
#define R_LBALL		0x04
#define R_RBALL		0x08
#define R_ONSCREEN	0x10
#define R_UNUSED	0x20
#define R_DRAWBALL	0x40
#define R_TWOPART	(TWOPART_MSK<<TWOPART_BIT)

#define RF_TURNHACK	0x43210000			// added to lower 16 bits of <adata.hackval> to flag a MODE_TURNHACK

// replay recorder commands; all will stop any playback
#define RR_START	0x01				// start recorder at current frame
#define RR_RESTART	0x02				// rewind tape; start recorder; reload player data
#define RR_REWIND	0x03				// rewind tape; stop recorder; reload player data
#define RR_STOP		0x04				// stop recorder

// replay playback commands; playback will stop the recorder
#define RP_STARTALL	0x01				// start or restart playback of entire tape
#define RP_START	0x02				// start or restart playback at specified parms
#define RP_STOP		0x03				// stop playback

// replay_blocks indecies
#define TWOPART_IDX		(NUM_PLAYERS)
#define BALL_IDX		(TWOPART_IDX+TWOPART_CNT)
#define NUM_ROBJS		(BALL_IDX+1)

#define REPLAY_FRAMES	(57 * 6)		// total # of frames in circular buffer ("tape")


///////////////////////////////////////////////////////////////////////////////
// models
#define MOD_266		0x0
#define MOD_FAT		0x1
#define MOD_498		0x2
#define MOD_LBALL	0x4
#define	MOD_RBALL	0x8
#define MOD_JUMBO	0x10
#define MOD_2DIGITS	0x4

// jobs
#define JOB_NONE	0
#define JOB_RUSH	1				// tackle carrier, evade blockers
#define JOB_COVER	2				// cover a receiver
#define JOB_BLOCK	3				// defend carrier, block defenders
#define JOB_CATCH	4				// go for ball, ignore defenders
#define JOB_BREAKUP	5				// hit carrier as ball arrives
#define JOB_ZONE	6				// defend a zone

#define JERSEYNUM(x)	(((x) > -1) ? (player_blocks[(x)].static_data->number) : (0))
#define ISHUMAN(x)		((x)->station != -1)
#define ISDRONE(x)		((x)->station == -1)

//
// flags (for obj_3d.flags, mirrored in player.equ)
//
#define PF_FLIP				0x01		// player is flipped in X (doesn't affect animation
										// system.)  Just a flag that says to choose flipped
										// animation sequences.

#define PF_ONSCREEN			0x02		// player is on screen this tick

#define PF_HICOUNT			0x04		// draw player with high polygon-count model

#define PF_TURBO			0x08		// player is in turbo mode

#define PF_OLD_TURBO		0x10		// previous tick's turbo

#define PF_NOCOLL			0x20		// collisions disabled

#define PF_ANITICK			0x40		// animated to frame break this tick

#define PF_INBND			0x80		// player is in bounds this tick

#define PF_CELEBRATE		0x100		// player is reacting/celebrating.  don't end action

#define PF_REVWPN			0x200		// player is a weapon running a reversed route

#define PF_FLASHWHITE		0x400		// draw player in bright white

#define PF_BACKPEDAL		0x800		// player may backpedal based on stick and direction
										// to ppdata->bptarget
#define PF_LATEHITABLE		0x1000		// player has been tackled...has ball...and get be hit again

#define PF_WAVE_ARM 		0x2000		// player is gonna wave to qb

#define PF_DIZZY    		0x4000		// player is dizzy...allow to be hit again (puppet moved)

#define PF_SPEED_OVERRIDE	0x8000		// use ppdata->speed regardless of mode/turbo

#define PF_DBUT_ANIM_TWOPART 0x00010000	// used to say wheter 'drone_script' is two part!!

#define PF_UNBLOCKABLE		0x00020000		// always wins in block_vs_tackle

#define PF_DRONEFACE		0x00040000		// drone has given a new face dir (for stances only,
										// and standard_run is free to ignore it.)

#define PF_THROWING			0x00080000		// player has started to throw, hasn't released ball yet

#define PF_DRAGGING_PLR		0x00100000	// player is carrying(dragging) someone on back

#define PF_PEND_FCHECK		0x00200000	// player will check for fire if he lands in bounds
										// after this AIR_CATCH

#define PF_FLASH_RED		0x00400000	// player will flash red

#define PF_POP_HELMET		0x00800000	// players helmet flys off

// positions
#define PPOS_OLINE1	0
#define PPOS_OLINE2	1
#define PPOS_OLINE3	2
#define PPOS_QBACK	3
#define PPOS_WPN1	4
#define PPOS_WPN2	5
#define PPOS_WPN3	6
#define PPOS_OCNT	7

#define PPOS_DLINE1	0
#define PPOS_DLINE2	1
#define PPOS_DLINE3	2
#define PPOS_LBACK1	3
#define PPOS_LBACK2	4
#define PPOS_DBACK1	5
#define PPOS_DBACK2	6
#define PPOS_DCNT	7

#define PLAYER_H

// Drone flags
#define DF_RUNYDS		0x0001			// recvr in RUN_YDS command
#define DF_WAITINT		0x0002			// waiting for UNINT flag to clear
#define DF_GHOST_ACTIVE	0x0004			// receiver has an active ghost
#define DF_BLITZ		0x0008			// use blitz rules
#define DF_TURBO		0x0010			// hold turbo
#define DF_CANNED_CATCH	0x0020			// gonna do a canned catch
#define DF_ORDERED		0x0040			// defensive back taking orders from recvr
#define DF_WAITING		0x0080			// in a DS_WAIT
#define DF_MOTION		0x0100			// man in motion
#define DF_WILL_INT		0x0200			// drone db plans to intercept pass
#define DF_WILL_ATTNOW	0x0400			// drone db plans to attack receiver ASAP
#define DF_WILL_ATTLATER 0x0800			// drone db plans to attack receiver at catch time
#define DF_WILL_POS		0x1000			// drone db can't reach recvr. position for later tackle
#define DF_REACHED_X	0x2000			// drone wpn reached X, can now turn to face ball
#define DF_IMPROVISING	0x4000			// drone recvr is winging it
#define DF_SMART_INT	0x8000			// uses smart intercept
#define DF_CAN_DIVE		0x00010000		// drone can use turbo-a attacks
#define DF_QB_RUN		0x00020000		// drone QB has decided to run
//#define DF_QB_RLEFT	0x00040000		// drone QB has decided to roll left
//#define DF_QB_RRIGHT	0x00080000		// drone QB has decided to roll right
#define DF_CAN_CHEAP	0x00100000		// drone can take cheap shots at receivers
#define DF_CAN_EVADE	0x00200000		// drone carrier get to evade once before checking permissions again
#define DF_DEBUG		0x00400000		// drone is in verbose debug mode
#define DF_RUSH_WEAK	0x00800000		// drone will rush QB if no wpn is on his side of QB at snap
#define DF_ZONE_PRESNAP	0x01000000		// drone db in zone coverage will move before snap
#define DF_DELAY_BLOCK	0x02000000		// drone will wait before going into block mode
#define DF_THROW_NEXT	0x04000000		// drone will throw the ball ASAP

// number of textures used on the ball (on fire) smoke
#define	NUM_SMOKE_TXTRS	    17
#define	NUM_DUST_TXTRS	    10
#define	NUM_BLOOD_TXTRS	    10
#define	NUM_SPARK_TXTRS	     6
#define	NUM_BSMOKE_TXTRS    11

// player run speeds
#define QB_SPEED				1.30f
#define QB_TURBO_SPEED			1.60f
#define QB_BACKP_SPEED			1.30f
#define QB_BACKP_TURBO_SPEED	1.45f

#define WPN_SPEED				1.15f
#define WPNPS_SPEED				1.35f
#define WPN_TURBO_SPEED			1.45f
#define WPN_BACKP_SPEED			1.35f
#define WPN_BACKP_TURBO_SPEED	1.65f

#define CARRIER_SPEED	   		1.35f
#define CARRIER_TURBO_SPEED		1.65f

#define DB_SPEED				1.15f
#define DBPS_SPEED				1.25f
#define DBPS_TURBO_SPEED		1.60f
#define DB_TURBO_SPEED			1.85f
#define DB_BACKP_SPEED			1.05f
#define DB_BACKP_TURBO_SPEED	1.60f

#define LINE_SPEED				1.25f
#define LINE_TURBO_SPEED		1.50f
#define LINE_BACKP_SPEED		1.25f
#define LINE_BACKP_TURBO_SPEED	1.60f

#define HUMAN_DEF_SPEED			1.45f
#define HUMAN_DEF_TURBO_SPEED	1.85f

// player modes - mirrored in player.equ
#define MODE_LINEUP		0		// moving into position between plays
#define MODE_DEAD		1		// dead -- just sitting & doing nothing
#define	MODE_LINEPS		2		// lineman before the snap
#define MODE_LINE		3		// lineman
#define	MODE_QBPS		4		// QB before the snap
#define	MODE_QB			5		// QB, with ball, behind LOS
#define MODE_WPNPS		6		// WR|RB before the snap
#define MODE_WPN  		7		// WR|RB after the snap
#define MODE_DBPS		8		// DB|LB before the snap
#define MODE_DB			9		// DB|LB after the snap
#define MODE_KICKER		10		// any kind of kicker

// limb definitions
#define JOINT_PELVIS	0
#define JOINT_TORSO		1
#define JOINT_RSHOULDER	2
#define JOINT_RELBOW	3
#define JOINT_RWRIST	4
#define JOINT_LSHOULDER	5
#define JOINT_LELBOW	6
#define JOINT_LWRIST	7
#define JOINT_NECK		8
#define JOINT_RHIP		9
#define JOINT_RKNEE		10
#define JOINT_RANKLE	11
#define JOINT_LHIP		12
#define JOINT_LKNEE		13
#define JOINT_LANKLE	14

// attack modes
#define ATTMODE_NONE	    0
#define	ATTMODE_PUSH	    1
#define ATTMODE_DIVE	    2
#define ATTMODE_KICK	    3
#define ATTMODE_LATE_HIT    4
#define ATTMODE_SPEAR_HEAD  5
#define ATTMODE_SWIPE       6
#define ATTMODE_HEAD        7
#define ATTMODE_HURDLE      8

//
// Loose ball types
//
#define LB_PASS			0
#define LB_FUMBLE		1
#define LB_PUNT			2
#define LB_KICKOFF		3
#define LB_FIELD_GOAL	4
#define LB_BOBBLE		5
#define LB_LATERAL      6

//
// Ball flags
//
#define BF_WHITE		0x01			// ball is solid white
#define BF_VEL_PITCH	0x02			// ball pitch follows velocity
#define BF_TURBO_PASS	0x04			// pass was turbo
#define BF_FIRE_PASS	0x08			// pass was fire
#define BF_BOBBLE		0x10			// ball has been bobbled
#define BF_BOUNCE		0x20			// ball has bounced
#define BF_ALT			0x40			// ball uses alternate texture
#define BF_FREEZE		0x80			// ball doesn't move
#define BF_TGT_OFFSCRN	0x100			// pass target is/was off-screen
#define BF_DEFLECTED	0x200			// ball was deflected
#define BF_SPINING		0x400			// ball is spinning (phi...on ground)
#define BF_PHANTOM		0x800			// Regardless of carrier, draw ball independent
										// of player, don't update ball position based
										// on player pos, and don't draw the ball in the
										// players hand.  (BIG HACK)
#define BF_FG_BOUNCE	0x1000			// (FG) ball has bounced
#define BF_FG_HITPOST	0x2000			// (FG) ball hit a post

// Player Size attributes
enum { PS_NORM, PS_FAT };

// Player Color attributes
enum { PC_BLACK, PC_WHITE };

// Player Position attributes
enum { PP_QB, PP_WR, PP_RB, PP_TE, PP_OL, PP_K,
	PP_LB, PP_CB, PP_DL, PP_S };

// Team attributes
enum {
	TA_PASSING,
	TA_RUSHING,
	TA_OFFENSE,
	TA_DEFENSE,
	TA_SPTEAMS,
	NUM_TM_ATTRIBS
};

// misc. 3d obj. struct
typedef struct _obj3d_data
{
	obj_3d				odata;					// 3D object data (see note in ani3dD.h)
	LIMB				*limb_obj;
#ifdef SEATTLE
	GrMipMapId_t		decal;
#else
	Texture_node_t		*decal;
#endif
	int					ratio;
} obj3d_data_t;

// used for the VS. screen helmets
typedef struct _obj3d2_data
{
	obj_3d				odata;					// 3D object data (see note in ani3dD.h)
	LIMB				*limb_obj;
#ifdef SEATTLE
	GrMipMapId_t		decal[2];
#else
	Texture_node_t		*decal[2];
#endif
	int					ratio[2];
	int					move;						// flag which determines if obj moves relative to "right_control" or "left_control"
} obj3d2_data_t;


// ball data struct
typedef struct _ball_data
{
	obj_3d		odata;					// 3D object data (see note in ani3dD.h)
	float		tx,ty,tz;				// target spot
	float		tsx,tsy,tsz;			// last cambot xformed X,Y,Z for target
	float		ox,oy,oz;				// origination point
	int			phi;					// Z-rotate
	int			pitch;					// X-rotate
	int			pitch_rate;				// rate of X rotation
	int			alpha;					// ball transparency
    float       scale;					// size
	int			int_receiver;			// intended receiver (plyrnum)
	int			flight_time;			// ticks since throw
	int			total_time;				// total ticks to target
	int			who_threw;				// guy who threw me (passes only, not fumbles)
	int			catch_pcount;			// pcnt when most recently caught
	int			type;					// LB_???, defined below
	int			flags;					// BF_???, defined below
	int			bounces;				// # of bounces on ground - used for bnc snd
	int			bounce_time;			// pcount last time ball bounced off field/goal
} ball_data;

// throw solution struct
struct	throw_solution
{
	int			t;						// time to target (in ticks)
	float		vx,vy,vz;				// initial velocity (units/tick)
	float		xi,yi,zi;				// initial position (qb hand, probably)
	float		xf,yf,zf;				// numbers-height target position (for X)
	int			angle;					// angle of shot (world, from QB)
	float		dist;					// dist from i to f
	float		v;						// net (xyz) velocity
};

// STATIC player data struct
struct st_plyr_data
{
	int			at_spd;
	int			at_pwr;
	int			at_qb;
	int			at_wr;
	int			at_off;
	int			at_def;
	float		scale;					// 1.0f = normal size
	int			size;					// PS_NORM, PS_FAT
	int			color;					// PC_BLACK, PS_WHITE
	int			number;					// jersey number
	int			position;				// official field position played
	void *		pii_mugshot;			// mugshot image_info *
	void *		pii_selname;			// name image_info *
	void *		pii_name;				// name image_info *
	int			ancr;					// announcer speech call #
	char		lastname[16];
	char		frstname[16];
};

// team data struct
struct fbteam_data
{
	short		attr[NUM_TM_ATTRIBS];	/* team attributes */
	int			drone_base;				/* base diff. level as drone */
//	int			panic;					/* don't go to 11 unless down by */
//	int			drone_min;				/* min diff. level as drone */
//	int			human_max;				/* max diff. for humans facing this team */
	char		name[32];				/* team name */
	char		home[32];				/* team home city */
	char		abbrev[4];				/* 3-letter abbreviation + NUL */
	char		prefix[4];				/* 3-letter prefix + NUL */
	const struct st_plyr_data * static_data;					
	void *		pii_logo;				/* team logo <image_info_t> ptr */
	void *		pii_logo30;				/* team logo30 <image_info_t> ptr */
	void *		pii_cn;					/* team city name <image_info_t> ptr */
	void *		pii_cnb;				/* team hilited city name <image_info_t> ptr */
	void *		prl;					/* rotating team logo <image_info_t> table ptr */
	void *		pti;					/* team info <image_info_t> table ptr */
};

struct diff_set
{
	int			panic;					/* don't go to 11 unless down by */
	int			drone_min;				/* min diff. level as drone */
	int			human_max;				/* max diff. for humans facing this team */
};

// from teamdata.c
extern const struct fbteam_data teaminfo[];


// player data struct
typedef struct _plyr_data
{
	obj_3d		odata;					// 3D object data (see note in ani3D.h)
	float		anidata[6][60];			// four quat blocks:
										// 0: primary anim uncompressed quats
										// 1: primary anim interpolation src
										// 2: secondary anim uncompressed quats
										// 3: secondary anim interpolation src
										// 4: primary anim interpolation dest
										// 5: secondary anim interpolation dest
	float		sx,sy,sz;				// last PROJECTED screen X,Y,Z of center ground
	float		tsx,tsy,tsz;			// last cambot xformed X,Y,Z of center ground
	float		jpos[15][3];			// joint positions (in camera space, for collisions)
	float		matrices[15][12];		// limb xlation & rotation matrices
	int			attack_mode;			// attack mode
	int			attack_time;			// ticks remaining on current attack
	int			celebrate_time;			// ticks remaining on current attack
	int			override_time;			// ticks remaining until speed override kicks in
	int			acj1,acj2;				// attack cylinder joints 1 and 2
	float		acx1,acx2;				// attack cylinder extensions 1 and 2
	float		acr;					// attack cylinder radius
	float		speed;					// speed to use if PF_SPEED_OVERRIDE is set
	int			plyrnum;				// player number
	int			position;
	int			team;					// 0 or 1
	int			station;				// 0-3, control panel station that drives us
	int			torso_mode, legs_mode;	// only used for QB (so far)
	int			stick_cur,but_cur;		// current switches
	int			stick_old,but_old;		// last tick's switches
	int			but_new;				// cur & ~old
	int			but_hist[5];			// 5 ticks of button history
	int			non_turbo_time;			// # of ticks turbo has not been held down (For spin mv)
	int			stick_time;				// # of ticks stick has been at current value
	struct _plyr_data *puppet_link;		// pointer to linked puppet type character
	int			puppet_rel_time;		// release timestamp on puppet moves (synch check)
	int			throw_target;			// guy I mean to throw/leap at
	int			drone_throw_target;		// guy drone means to throw at
	int			job;					// JOB_XXX, defined below
	int			bptarget;				// guy we're watching.  Used to compute backpedals.
	int			bp_expire_time;			// see notes in player.c
	const struct st_plyr_data *static_data;
	int			non_buta_time;			// # of ticks BUT A has not been held down (For dive mv)
	int			non_butb_time;			// # of ticks BUT B has not been held down (For lateral mv)
	int			block_count;			// # of times plyr has been blocked (Not how many blocks given!)
	int			no_catch_time;			// # of ticks before I'm again allowed to touch the ball
    char        *drone_script;          // anim script that drone runs on D button press
	int			drone_face;				// direction standing drones want to face (they can turn in place)
	int			fake_zone_time;			// Counter before a fake zone guy goes into a man 2 man
} fbplyr_data;


///////////////////////////////////////////////////////////////////////////////
// Replay data structs/defines
//
typedef struct _replay_t
{
	float		x,y,z;					// [obj_3d] world position
	int			fwd;					// [obj_3d] facing direction (0-1023)
	int			seq1;					// [obj_3d.adata] sequence index (from xxxxxSEQ.H)
	int			seq2;					// [obj_3d.adata] interpolation source frame seq index
	int			frame1,frame2;			// [obj_3d.adata] frame numbers
	float		t;						// [obj_3d.adata] interpolation frame number (fractional)
	int			hackval;				// [obj_3d.adata] MODE_TURNHACK arg
	int			flags;					// replay ops flag
} replay_t;

// Needs to cast into <_replay_t> so must never be larger!
typedef struct _replay_ball_t
{
	float		x,y,z;					// [obj_3d] world position
	int			fwd;					// [obj_3d] facing direction (0-1023)
	int			fwd2;					// [obj_3d] second facing direction (0-1023) (mostly unused)
	float		vx,vy,vz;				// [obj_3d] velocity
	int			flags;					// replay ops flag
} replay_ball_t;

typedef struct _replay_obj_t
{
	float		x,y,z;					// [obj_3d] world position
	int			fwd;					// [obj_3d] facing direction (0-1023)
	float		ascale;					// [obj_3d] x & z animpt scale factor
#ifdef SEATTLE
	GrMipMapId_t *pdecal;				// [obj_3d] array of decal indices
#else
	Texture_node_t	**pdecal;				// [obj_3d] array of decal indices
#endif
	float		jpos[15][3];			// [fbplyr_data] joint positions (in camera space, for collisions)
	float		matrices[15][12];		// [fbplyr_data] limb xlation & rotation matrices
	int			plyrnum;				// [fbplyr_data] player number
	const struct st_plyr_data *static_data;	// [fbplyr_data] * to static player data struct
	float		pq[60];					// current frame data (script or object struct ram)
	int			hackval;				// MODE_TURNHACK arg
	int			last_seq1;				// last seq1 index
	int			last_frame1;			// last frame1 number
	int			last_seq2;				// last seq2 index
	int			last_frame2;			// last frame2 number
	int			flags;					// replay ops flag
} replay_obj_t;

// from player.c
extern fbplyr_data		player_blocks[NUM_PLAYERS];	// player data blocks
extern ball_data		ball_obj;				// ball data
extern int plowhd_snd;
extern int halt;
extern int audible_shown[2];	// set to 1 if audibles message is being shown
extern int audible_turbo_timer[2];	//	used as audible double-tap mechanism
extern int audible_tap_action[2];	//	used as audible double-tap mechanism


extern int taunted;
extern int fire_pflash;
extern int almost_fire_pflash;
extern int db_pflash;
extern void player_burst_into_flames(int *parg);
extern void tmsel_player_proc( int * );
extern void draw_player2( void * );
extern void player_proc( int * );
extern void last_player_proc( int * );
extern void pre_player_proc( int * );
extern void load_uniform( int, int, int, int );
extern void ball_proc( int * );
extern void loose_helmet_proc(int *);
extern void cursor_proc( int * );
extern void debug_switch_proc( int * );
extern void reinit_player( int, int );
extern void draw_player_shadow( void * );
extern void yell_signals_proc( int * );
extern void scale_plyrs_proc(int *args);
extern void maybe_pop_helmet(obj_3d *);

#endif	// #ifndef PLAYER_H
