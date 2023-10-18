/****************************************************************************/
/*                                                                          */
/* player.c - top-level player code                                         */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1997 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
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
#include <string.h>

#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Anim/genseq.h"
#include "/Video/Nfl/Include/plyrseq.h"
#include "/Video/Nfl/Include/ani3d.h"
#include "/Video/Nfl/Include/pproto.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/handicap.h"
#include "/Video/Nfl/Include/profile.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/PlayStuf.h"
#include "/Video/Nfl/Include/CamBot.h"
#include "/Video/Nfl/Include/PInfo.h"
#include "/Video/Nfl/Include/PMisc.h"
#include "/Video/Nfl/Include/Drone.h"
#include "/Video/Nfl/Include/TeamData.h"
#include "/video/nfl/include/cap.h"

#define	HRES	512

// sunlight angle vector
#define SUN_X	1.54f
#define SUN_Y	-3.07f
#define SUN_Z	-1.44f

//#define CHANGE_PLAYER_DEBUG

#ifdef JASON
//#define THROW_DEBUG
#endif

#ifndef DEBUG
#ifdef JASON
"Hey, don't build in release mode with your debug LOCALSET.MAK."
#endif
#endif

//
// global data & function prototypes
//
extern struct texture_node	*smkpuf_decals[NUM_SMOKE_TXTRS];
extern struct texture_node	*dustcld_decals[NUM_DUST_TXTRS];
//extern struct texture_node	*blood_decals[NUM_BLOOD_TXTRS];
extern struct texture_node	*spark_decals[NUM_SPARK_TXTRS];
extern struct texture_node	*bsmkpuf_decals[NUM_BSMOKE_TXTRS];
extern int forward_catcher;

void get_drone_switches( fbplyr_data *, int );

extern void crowd_cheer_snds(void);
extern void qb_runs_past_los_sp(void);

extern int punt_strength;
extern LIMB *head_limbs[];

extern LIMB limb_phead_obj;
extern LIMB limb_phelm_obj;

#ifdef DEBUG
int		halt = FALSE;
#endif //DEBUG

//
// local data & function prototypes
//
void clear_random_control_flags(void);
int unique_random(int range, int control);
//static void draw_spark_func(void *oi);
static void qb_throw_monitor( int *args);
static void whistle_proc(int *args);
static void ball_fire_trail_proc(int *parg);
static void dust_cloud_proc(int *args);
void flash_plyr_proc(int *args);
void flash_plyr_red_proc(int *args);
void delay_deflect_proc(int *args);
void first_snd_proc(int *args);
//static void pre_blood_spray_proc(int *args);
//static void blood_spray_proc(int *args);
static void plyr_fire_trail_proc(int *parg);
static void plyr_bfire_trail_proc(int *args);
static int farthest_teammate( fbplyr_data * );
void dump_last_pass( int, int );

static void draw_spfx(void *oi);
static void draw_loose_helmet( void *oi );
static void draw_ball( void *oi );
static void draw_cursors( void *oi );
static void draw_player( void * );
static void zerovels( obj_3d * );
static void turn_toward_tgt( obj_3d * );
static int in_bounds( obj_3d * );
static int over_first_down( obj_3d * );
static int oob_zone( obj_3d * );
static int pick_receiver( obj_3d * );
static void set_ball_vels( obj_3d *, obj_3d * );
static void set_ball_vels2( obj_3d *, obj_3d * );
static void set_hike_ball_vels( obj_3d *, obj_3d * );
static void set_punt_ball_vels( obj_3d *);
static void set_field_goal_ball_vels( obj_3d *);
static void set_spike_ball_vels(obj_3d *,int);
static void kick_ball(obj_3d *);
static void player_pre_draw( void * );
static void player_shadow_init( void );
static void player_shadow_predraw( void * );
static void standard_catch( obj_3d *pobj );
static void standard_run( obj_3d *pobj );
static void qb_run( obj_3d *pobj );
static int db_change_player( fbplyr_data *, int);
static int wpn_change_player( fbplyr_data * );
static void wpnps_change_player( fbplyr_data * );
static float weighted_dist( fbplyr_data *, fbplyr_data * );
#ifdef DEBUG
static void sanity_check( char * );
static void dump_joint_positions( void );
#endif
static void trail_ball_proc( int * );
static void maybe_plyr_fire(fbplyr_data *);
static void motion_block_monitor( int * );
static int over_def_ez( obj_3d * );
static float dist3d( float *, float * );
//static void intercept_snd_proc(int *args);
static void maybe_lateral(obj_3d *pobj);
static void check_receiver_fire( fbplyr_data * );
static void clean_audible_text(int tm_nbr);
static void reset_audible_check(void);

//static void print_audible_message(int *pargs);
void print_audible_message(int off_def, int audible_num, int station, int tm_nbr, int first);
static int audible_check(fbplyr_data *ppdata,int off_pos);

extern void show_receiver_name_proc(int *);
extern void qb_actions_sp(int *args);
extern void throw_ball_sounds(void);
extern void catch_sounds(obj_3d *);
extern void lateral_speech(void);
extern void bobble_ball_sounds(void);
extern void ko_idiot_box_proc(int *args);
extern void fumble_ball_sounds(void);
extern void deflected_ball_sp(void);
extern void hurdle_speech(void);
extern void spin_move_speech(void);
extern void stiff_arm_speech(void);
//extern void stiff_arm_f_speech(void);
extern void almost_touchdown_sp(obj_3d *);
extern void disappointed_speech(void);
extern play_t *Playsel__GetAudiblePlay(int pnum, int off_def, int aud_num);

// player modes
static int _mode_lineup( fbplyr_data *ppdata );
static int _mode_dead( fbplyr_data *ppdata );
static int _mode_line( fbplyr_data *ppdata );
static int _mode_lineps( fbplyr_data *ppdata );
static int _mode_qbps( fbplyr_data *ppdata );
static int _mode_qb( fbplyr_data *ppdata );
static int _mode_wpnps( fbplyr_data *ppdata );
static int _mode_wpn( fbplyr_data *ppdata );
static int _mode_dbps( fbplyr_data *ppdata );
static int _mode_db( fbplyr_data *ppdata );
static int _mode_kicker( fbplyr_data *ppdata );

float odistxz( obj_3d *, obj_3d * );

void begin_twopart_mode( obj_3d * );

// ANI_CODE functions
void set_trgt_fwd( obj_3d *, int, int);
void rotate_anim( obj_3d *, int);
void turn_player( obj_3d *, int);
void maybe_receiver_mad( obj_3d * );
void maybe_taunt( obj_3d * );
void celebrate( obj_3d *, int );
void dust_cloud( obj_3d *, int );
//void blood_spray( obj_3d *, int, int, int );
void lower_player( obj_3d * );
void set_unint_lowerhalf (obj_3d * );
void rotate_to_ball( obj_3d *, int );
int maybe_drop_kick(obj_3d *);
int	is_plr_on_grnd(obj_3d *);
//int	maybe_dizzy(obj_3d *);
void maybe_get_up_c_hurt(obj_3d *);
void maybe_get_up_b_hurt(obj_3d *);
void maybe_qb_mad(obj_3d *);
int slower_spin(obj_3d *);
int maybe_spear_head(obj_3d *);
int rotate_torso(obj_3d *, int, int);
void change_to_attacker_fwd(obj_3d *);
void set_plyr_y_pos(obj_3d *);

#ifdef SEATTLE
GrMipMapId_t	team_decals[4][10];
GrMipMapId_t	all_teams_decals[NUM_TEAMS][4];
GrMipMapId_t	misc_decals[4];
#else
Texture_node_t	*team_decals[4][10];
Texture_node_t	*all_teams_decals[NUM_TEAMS][4];
Texture_node_t	*misc_decals[4];
#endif

// move direction table
int dir49_table[] = {
		128,  96,  52,   0, 972, 928, 896,
		160, 128,  76,   0, 948, 896, 864,
		204, 180, 128,   0, 896, 844, 820,
		256, 256, 256,  -1, 768, 768, 768,
		308, 332, 384, 512, 640, 692, 716,
		352, 384, 436, 512, 588, 640, 672,
		384, 416, 460, 512, 564, 608, 640
};

static const float field_friction_mult_factor[]=
{
	1.00f,  // grass
	1.00f,	// old grass
	1.05f,  // astro turf
	0.90f,	// Dirt
	0.60f,  // Snow
	1.20f,	// Asphalt     
	0.75f,  // Mud
};

// distance table (recomputed every tick)
extern int tick_counter;

//int audible_shown=0;	// set to 1 if audibles message is being shown
//int audible_done=0;

#ifdef SEATTLE
GrMipMapId_t	target_decal;
GrMipMapId_t	ball_decal;
GrMipMapId_t	ball_decal2;
#else
Texture_node_t	*target_decal;
Texture_node_t	*ball_decal;
Texture_node_t	*ball_decal2;
#endif

#ifdef SEATTLE
GrMipMapId_t	loose_helmet_decal;
#else
Texture_node_t	*loose_helmet_decal;
#endif

#ifdef SEATTLE
GrMipMapId_t	ball_shadow_decal;
GrMipMapId_t	cursor_decals[4];
#else
Texture_node_t	*ball_shadow_decal;
Texture_node_t	*cursor_decals[4];
#endif

#if defined(SEATTLE)
int				cursor_ratio = GR_ASPECT_1x1;
#else
int				cursor_ratio = 3;
#endif
int				ball_ratio;
int				ball_shadow_ratio;
//int				bozo;
int				firecnt;
int				throw_target;

int				loose_helmet_ratio;
int				loose_helmet_shadow_ratio;

int				plowhd_snd;
int				taunted = 0;
int almost_fire_pflash;
int fire_pflash;
int	db_pflash;
//int do_fire_burst;
#ifdef DEBUG
int freeze = FALSE;
#endif

int	audible_turbo_timer[2];
int	audible_tap_action[2];
//int audible_hike[2];

fbplyr_data		player_blocks[NUM_PLAYERS];	// player data blocks
ball_data 		ball_obj;			// ball
ball_data 		loose_helmet_obj;	// loose helmet 

float	bighead_scale;				// Big head scale for current player draw (0 = no scaling)
int		target_human;				// target human receiver?
int 	last_pass_hist[5];

// shadow data
static float		fx,fy,fz;		// world origin in camera coords
static float		sx,sy,sz;		// sunlight vector in camera system
static float		nx,ny,nz;		// field normal in camera system (xform middle row)

//static int waiting_for_landing;		// receiver has caught the ball in the air.
									// waiting for him to land to apply fire effects

//static process_node_t *pflasher_audible_text = NULL;

//
// referenced data
//
extern int			ptex_ratios[];
extern SNODE		fbplyr_skel[];
extern SNODE		tophalf_skel[];
extern SNODE		bothalf_skel[];

extern LIMB			*model266_limbs[], *model266bl_limbs[], *model266br_limbs[];
extern LIMB			*model266f_limbs[], *model266fbl_limbs[], *model266fbr_limbs[];
extern LIMB			*model498_limbs[], *model498bl_limbs[], *model498br_limbs[];
extern LIMB			*model498f_limbs[], *model498fbl_limbs[], *model498fbr_limbs[];

extern LIMB			*model266jbl_limbs[], *model266jbr_limbs[];
extern LIMB			*model266fjbl_limbs[], *model266fjbr_limbs[];
extern LIMB			*model498jbl_limbs[], *model498jbr_limbs[];
extern LIMB			*model498fjbl_limbs[], *model498fjbr_limbs[];

extern LIMB			limb_numbers_obj2661,limb_numbers_obj2662,limb_numbers_obj266f;
extern LIMB			limb_numbers_obj4981,limb_numbers_obj4982,limb_numbers_obj498f;

extern LIMB			limb_gndsquare_obj;
extern LIMB			limb_fieldline_obj;
extern LIMB			limb_loball_obj;
extern LIMB			limb_medball;
extern LIMB			limb_medball_obj_big;
extern LIMB			limb_ballshadow_obj;

extern LIMB			limb_Polyset;			// actually a small SQUARE for the FIRE fx
//extern LIMB			limb_node;		// actually a RECTANGLE for special fx
//extern LIMB			limb_square_obj;		// actually a RECTANGLE for special fx

extern float trigs[];
extern int			p2p_angles[NUM_PLAYERS][NUM_PLAYERS];
extern float		distxz[NUM_PLAYERS][NUM_PLAYERS];

char	*last_catch;
int		did_catch;

static char *running_taunt_tbl[] = {
	r_histep_anim,
	r_skip_anim,
	r_hihand_anim,
	r_bktau2_anim };

static char *push_anim_tbl[] = {
	t_push_anim,
	t_runkic_anim,
	t_push_anim };

static char *intercept_anim_tbl[] = {
	c_jmpint_anim,
	c_jmpint2_anim };

static char *hurdle_tbl[] = {
	hurdle_anim,
	tf_hurdle_anim,
	hurdle2_anim,
	spin_hurdle_anim,
	hurd03_anim,
	hurd04_anim,
	hurd07_anim,
	hurdflip_anim
};

static char *turbo_hurdle_tbl[] = {
	turbo_hurdle_anim,
	turbo_tf_hurdle_anim,
	turbo_hurdle2_anim,
	turbo_spin_hurdle_anim,
	turbo_hurd03_anim,
	turbo_hurd04_anim,
	turbo_hurd07_anim,
	turbo_hurdflip_anim
};

static char *mad_qb_anim_tbl[] = {
	m_qbwhat_anim,
	m_qbwhat_anim,
	m_qbfite_anim 
};

static char *get_up_c_hurt_tbl[] = {
	get_up_c2_hurt_anim,
	get_up_c3_hurt_anim,
	get_up_c3_hurt_anim 
};

static char *get_up_b_hurt_tbl[] = {
	get_up_b1_hurt_anim,
	get_up_b1_hurt_anim,
	get_up_b2_hurt_anim 
};

static char *breath_seq_tbl[] = {
	fb_mill1,
//#ifndef SEATTLE
	fb_mill3,
	fb_mill5,
	fb_mill10,
	fb_mill14,
//#endif
	fb_mill4,
	fb_mill6,
	fb_mill7,
	fb_mill11,
	fb_mill12,
	fb_mill15,
	breath_anim,
	breath2_anim,
	breath3_anim,
};

static char *new_endzone_dances_tbl[] = {
	endz_06,
	endz_08,
//#ifndef SEATTLE
	endz_20,
//#endif
};

static char *endzone_dances_tbl[] = {
	end_zone1_anim,
	end_zone5_anim,
	end_zone6_anim,
	end_zone9_anim,
//	end_zone17_anim,
//#ifndef SEATTLE
	end_zone3_anim,
	end_zone2_anim,
	end_zone4_anim,
	end_zone7_anim,
	end_zone8_anim,
	end_zone10_anim,
	end_zone11_anim,
	end_zone13_anim,
	end_zone14_anim,
	end_zone15_anim,
	end_zone18_anim,
	end_zone19_anim,
//#endif
};

//static char *tm_selected_dance_tbl[] = {
//	tmsel_dance1_anim,
//	tmsel_dance2_anim };

//static char *tm_selected_dance_f_tbl[] = {
//	tmsel_dance1f_anim,
//	tmsel_dance2f_anim };


static char *endzone_reactions_tbl[] = {
	disapointed1_anim,
	disapointed2_anim 
};

static char *miss_pass_reactions_tbl[] = {
//	disapointed1_anim,
//	disapointed2_anim,
	m_mscat1_anim,
	m_mscat2_anim };

static char *wr_anims_tbl[] = {
		wr_sprint1_anim,
		stance2pt_anim_rnd,
		wr_sprint2_anim,
		wr_set_anim,
		wr_sprint3_anim,
		wr_set2_anim,
		wr_sprint4_anim 
};


static char *qb_anims_tbl[] = {
		qbset_anim,
		qb_claps_set_anim,
		qb_rubs_knee_set_anim };

static char *qb_shotgun_anims_tbl[] = {
		shotgun_anim,
		qb_pumps_crowd_set_anim,
		qb_claps_set_anim,
		qb_rubs_knee_set_anim };

static char *linemen_anims_tbl[] = {
		lm_set1_anim,
		lm_set2_anim,
		lm_set3_anim,
		lm_set4_anim 
};

static char *blood_lust_tbl[] = {
	m_dblood_anim,
	m_dblood2_anim,
	m_dblood3_anim,
	dline_03b,
	dline_01a,
	dline_04c,
	dline_02a
};

static char *man_2_man_tbl[] = {
//#ifndef SEATTLE
	shift_02,
//#endif
	shift_10,
	dline_01c,
	dline_02c,
	dline_03a,
	dline_04b,
};

static char *db_zone_shift_tbl[] = {
//#ifndef SEATTLE
	shift_01,
	shift_02,
	shift_04,
//#endif	
	shift_03,
	shift_08,
	shift_09
};

static char *db_anims_tbl[] = {
//		shift_09,
		stance2pt2_anim_rnd,
		dline_01a,
		dline_01b,
		dline_01c,
		dline_02a,
		dline_02b,
		dline_02c,
		safty2pt_anim_rnd,
		dline_03a,
		dline_03b,
		dline_03c,
		dline_04a,
		dline_04b,
		dline_04c
};

//static char *slow_down_anims_tbl[] = {
//	run_slowdwn1_anim,
//	run_slowdwn2_anim,
//	run_slowdwn3_anim,
//	run_slowdwn4_anim,
//	run_slowdwn5_anim };

//
// 'Late hit' ANIMATIONS
//
static char *db_late_hits_tbl[] = {
	elbow_drp_anim,
//	handstand_anim,
	butt_drp_anim,
	belly_flop_anim };

static char *violent_db_late_hits_tbl[] = {
	elbow_drp_anim,
	butt_drp_anim };
//	gut_stomp_anim,
//	knee_drp_anim,
//	gut_stomp_anim };

typedef struct _catch_t
{
	int		prob;
	int		twopart;
	char 	*anim;
	int		launch_time;
	float	jump_height;
	float	z_offset;
	int		control_time;	// ticks from seq launch before you regain control
							// used to avoid jumping out of bounds on catches
	int		flags;			// CF_???, defined below
} catch_t;

#define CF_NO_UNINT		0x1 	// don't use if recvr is unint
								// put CF_NO_UNINT on anything with a jump
								// height greater than 22 or an x offset
								// greater than 2


// CATCH anims...based on direction ball to player
//catch_t	catch_no_vel_tbl[] = {
//	{2,FALSE,c_no_vel1_anim,	18,	29.0f,	0.0f,	23,	CF_NO_UNINT},
//	{2,FALSE,c_no_vel2_anim,	18,	29.0f,	0.0f,	23,	CF_NO_UNINT},
//	{0,FALSE,NULL,0,0.0f,0.0f,0} };


catch_t	catch_front_tbl[] = {
	{3,FALSE,c_jgut1_anim,	15,	22.0f,	0.0f,	35,	0},
//	{2,FALSE,c_jmp1_anim,	20,	21.0f,	0.0f,	35,	0},
//	{1,FALSE,c_jmpovr_anim,	25,	40.0f,	1.0f,	45,	CF_NO_UNINT},
	{0,FALSE,NULL,0,0.0f,0.0f,0} };

catch_t	catch_rside_tbl[] = {
	{60,TRUE,c2p_sidef_anim,	8,	14.0f,	0.0f,	19,	0},
	{3,FALSE,c_jgut1f_anim,	15,	22.0f,	0.0f,	35,	0},
	{3,FALSE,c_jmp1f_anim,	20,	31.0f,	3.3f,	35,	CF_NO_UNINT},
	{3,FALSE,c_jmp2f_anim,	20,	32.3f,	2.2f,	37,	CF_NO_UNINT},
	{3,FALSE,c_sidrolf_anim,17,	29.0f,	7.9f,	41,	CF_NO_UNINT},
	{3,FALSE,c_splitsf_anim,23,	21.0f,	1.1f,	39,	0},
	{2,FALSE,c_sdgutf_anim,	10,	13.0f,	1.1f,	10,	0},
	{1,FALSE,c_spin2f_anim,	24,	25.0f,	1.0f,	44,	CF_NO_UNINT},
	{3,FALSE,c_bicycf_anim,	23,	29.0f,	7.3f,	56,	CF_NO_UNINT},
	{1,FALSE,c_div2h_anim,	18,	26.3f,	11.1f,	39,	CF_NO_UNINT},
	{2,FALSE,catch_01,	20,	29.0f,	0.0f,		48,	CF_NO_UNINT},
	{2,FALSE,catch_2f,	20,	29.0f,	0.0f,		64,	CF_NO_UNINT},
	{2,FALSE,catch_03f,	20,	37.0f,	0.0f,		48,	CF_NO_UNINT},
	{2,FALSE,catch_05f,	21,	37.0f,	0.0f, 		37,	CF_NO_UNINT},
	{2,FALSE,catch_06,	10,	22.0f,	0.0f,	19,	CF_NO_UNINT},
	{0,FALSE,NULL,0,0.0f,0.0f,0} };

catch_t	catch_behind_tbl[] = {
	{60,TRUE,c2p_back_anim,	8,	21.0f,	0.0f,	19,	0},
	{3,FALSE,c_run1_anim,	20,	26.0f,	2.5f,	27,	CF_NO_UNINT},
	{4,FALSE,c_run1a_anim,	25,	40.0f,	4.0f,	46,	CF_NO_UNINT},
	{3,FALSE,c_jgut1f_anim,	20,	22.0f,	0.0f,	35,	0},
	{3,FALSE,c_sidrol_anim,	17,	29.0f,	7.9f,	41,	CF_NO_UNINT},
	{2,FALSE,c_roll_anim,	20,	27.0f,	1.0f,	39,	CF_NO_UNINT},
	{2,FALSE,c_bicycf_anim,	23,	29.0f,	7.3f,	56,	CF_NO_UNINT},
	{1,FALSE,c_div2h_anim,	18,	26.3f,	11.1f,	39,	CF_NO_UNINT},
	{2,FALSE,catch_01,	20,	29.0f,	0.0f,	48,	CF_NO_UNINT},
	{2,FALSE,catch_02,	20,	29.0f,	0.0f,	64,	CF_NO_UNINT},
	{2,FALSE,catch_2f,	20,	29.0f,	0.0f,	64,	CF_NO_UNINT},
	{2,FALSE,catch_03,	20,	37.0f,	0.0f,	48,	CF_NO_UNINT},
 	{2,FALSE,catch_03f,	20,	37.0f,	0.0f,	48,	CF_NO_UNINT},
	{2,FALSE,catch_05,	21,	37.0f,	0.0f,	37,	CF_NO_UNINT},
	{2,FALSE,catch_05f,	21,	37.0f,	0.0f, 	37,	CF_NO_UNINT},
	{2,FALSE,catch_06,	10,	22.0f,	0.0f,	19,	CF_NO_UNINT},
	{0,FALSE,NULL,0,0.0f,0.0f,0} };

catch_t	catch_lside_tbl[] = {
	{60,TRUE,c2p_side_anim,	8,	14.0f,	0.0f,	19,	0},
	{2,FALSE,c_jgut1_anim,	15,	22.0f,	0.0f,	35,	0},
	{2,FALSE,c_jmp1_anim,	20,	31.0f,	3.3f,	35,	CF_NO_UNINT},
	{2,FALSE,c_jmp2_anim,	20,	32.3f,	2.2f,	37,	CF_NO_UNINT},
	{2,FALSE,c_sidrol_anim,	17,	29.0f,	7.9f,	41,	CF_NO_UNINT},
	{2,FALSE,c_splits_anim,	23,	21.0f,	1.1f,	39,	0},
	{2,FALSE,c_sdgut_anim,	10,	13.0f,	1.1f,	10,	0},
	{1,FALSE,c_spin2_anim,	24,	25.0f,	1.0f,	44,	CF_NO_UNINT},
	{2,FALSE,c_bicyc_anim,	23,	29.0f,	7.3f,	56,	CF_NO_UNINT},
	{1,FALSE,c_div2h_anim,	18,	26.3f,	11.1f,	39,	CF_NO_UNINT},
	{2,FALSE,catch_01,	20,	29.0f,	0.0f,	48,	CF_NO_UNINT},
	{2,FALSE,catch_02,	20,	29.0f,	0.0f,	64,	CF_NO_UNINT},
	{2,FALSE,catch_03,	20,	37.0f,	0.0f,	48,	CF_NO_UNINT},
	{2,FALSE,catch_05,	21,	37.0f,	0.0f,	37,	CF_NO_UNINT},
	{2,FALSE,catch_06,	10,	22.0f,	0.0f,	19,	CF_NO_UNINT},
	{0,FALSE,NULL,0,0.0f,0.0f,0} };



LIMB **models[] =
{
	model266_limbs, model266f_limbs,
	model498_limbs, model498f_limbs,

	model266bl_limbs, model266fbl_limbs,
	model498bl_limbs, model498fbl_limbs,

	model266br_limbs, model266fbr_limbs,
	model498br_limbs, model498fbr_limbs,

	// if the guy wants balls in both hands, just use the right
	model266br_limbs, model266fbr_limbs,
	model498br_limbs, model498fbr_limbs,

	// jumbo balls
	model266_limbs, model266f_limbs,
	model498_limbs, model498f_limbs,

	model266jbl_limbs, model266fjbl_limbs,
	model498jbl_limbs, model498fjbl_limbs,

	model266jbr_limbs, model266fjbr_limbs,
	model498jbr_limbs, model498fjbr_limbs,

	// if the guy wants balls in both hands, just use the right
	model266jbr_limbs, model266fjbr_limbs,
	model498jbr_limbs, model498fjbr_limbs
};

LIMB *jerseys[] =
{
	&limb_numbers_obj2661,&limb_numbers_obj266f,
	&limb_numbers_obj4981,&limb_numbers_obj498f,
	&limb_numbers_obj2662,&limb_numbers_obj266f,
	&limb_numbers_obj4982,&limb_numbers_obj498f
};

unsigned	los_flash_time, fd_flash_time;
unsigned	los_flash_rgb, fd_flash_rgb;

//////////////////////////////////////////////////////////////////////////////
//
// player section
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
//
void do_pup_player_scale(fbplyr_data * ppdata);

void do_pup_player_scale(fbplyr_data * ppdata)
{
	// Do nothing if scale is already different
	if (ppdata->odata.ascale != ppdata->static_data->scale)
		return;

	// Scale up or down if selected; baby has priority
	if (pup_babyplyrs & TEAM_MASK(ppdata->team))
		ppdata->odata.ascale *= 0.8f;
	else if (pup_bigplyrs & TEAM_MASK(ppdata->team))
		ppdata->odata.ascale *= 1.2f;
}

//////////////////////////////////////////////////////////////////////////////
// player_proc - main player process.
// arg0 = plyrnum
// arg1 = team
// arg2 = field position
// arg3 = who to be (0 to (NUM_PERTEAM_SPD-1)) on which team (tm# * NUM_PERTEAM_SPD)
//
void player_proc(int *parg)
{
	fbplyr_data	*ppdata;
	int			plyrnum, plyrteam, i;
	obj_3d		*pobj;
	char		cbuf[8];
	float		mxz,frx,frz;

	int (*mode_functions[])(fbplyr_data *) = {
			_mode_lineup, _mode_dead, _mode_lineps, _mode_line, _mode_qbps,
			_mode_qb, _mode_wpnps, _mode_wpn, _mode_dbps, _mode_db,
			_mode_kicker };

	// extract arguments
	plyrnum = parg[0];
	plyrteam = parg[1];

	ppdata = &player_blocks[plyrnum];
	pobj = (obj_3d *)ppdata;

	// wipe player data
//	memset( (void *)ppdata, 0, sizeof( fbplyr_data ));

	// init player data
	ppdata->plyrnum     = plyrnum;
	ppdata->team        = plyrteam;
	ppdata->position    = parg[2];
	ppdata->static_data = teaminfo[parg[3]/NUM_PERTEAM_SPD].static_data + (parg[3] % NUM_PERTEAM_SPD);
	pobj->pdecal        = team_decals[(plyrteam * 2) + ((ppdata->static_data->color == PC_WHITE) ? 0 : 1)];
	ppdata->stick_cur   = 24;
	ppdata->but_cur     = 0;
	pobj->ascale        = ppdata->static_data->scale;
	pobj->default_ascale = pobj->ascale;
	pobj->plyrmode      = MODE_DEAD;
	pobj->friction      = 0.0f;
	ppdata->attack_time = 0;
	ppdata->celebrate_time = 0;
	ppdata->stick_time  = 0;

	do_pup_player_scale(ppdata);

	// initialize anim data
	pobj->adata[0].adata = ppdata->anidata[0];
	pobj->adata[0].idata = ppdata->anidata[1];
	pobj->adata[1].adata = ppdata->anidata[2];
	pobj->adata[1].idata = ppdata->anidata[3];

	pobj->adata[2].adata = ppdata->anidata[4];
	pobj->adata[3].adata = ppdata->anidata[5];

	pobj->adata[1].animode = MODE_SECONDARY;

	sprintf( cbuf, "plyr%d", ppdata->plyrnum );
	create_object( cbuf, OID_PLAYER, OF_NONE, DRAWORDER_PLAYER + ppdata->plyrnum, (void *)ppdata, draw_player );
	sprintf( cbuf, "shad%d", ppdata->plyrnum );
	create_object( cbuf, OID_SHADOW, OF_NONE, DRAWORDER_SHADOW + ppdata->plyrnum, (void *)ppdata, draw_player_shadow );

	if( ppdata->plyrnum == 0 )
	{
		create_object( "preplyr", OID_PLAYER, OF_NONE, DRAWORDER_PLAYER-1, NULL, player_pre_draw );
		player_shadow_init();
		create_object( "preshad", OID_PRESHADOW, OF_NONE, DRAWORDER_PRESHADOW, NULL, player_shadow_predraw );
	}

	// init animation
	change_anim( pobj, into_stance_anim );


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

	// main player loop
	while( 1 )
	{
		int		was_backp, needed_oob_correction;
		int		human_control, fake_drone;

		if( ppdata->stick_cur == ppdata->stick_old )
			ppdata->stick_time += 1;
		else
			ppdata->stick_time = 0;

		ppdata->stick_old = ppdata->stick_cur;
		ppdata->but_old = ppdata->but_cur;

		// update button history
		ppdata->but_hist[4] = ppdata->but_hist[3];
		ppdata->but_hist[3] = ppdata->but_hist[2];
		ppdata->but_hist[2] = ppdata->but_hist[1];
		ppdata->but_hist[1] = ppdata->but_hist[0];
		ppdata->but_hist[0] = ppdata->but_cur;

		// clear backpedal bit (I moved this before 'get_drone_switches' otherwise...never be set)
		was_backp = (pobj->flags & PF_BACKPEDAL);
		pobj->flags &= ~PF_BACKPEDAL;

		// get current switches
		// figure out if we're under human or drone control
		human_control = ISHUMAN(ppdata);
		fake_drone = FALSE;

		// always under drone control on kickoffs until ball is caught
		if (human_control &&
			( game_info.play_type == PT_KICKOFF) &&
			(game_info.ball_carrier == -1 ) &&
			!(game_info.game_flags & GF_ONSIDE_KICK) &&
			(ball_obj.type == LB_KICKOFF ))
			human_control = FALSE;

		// always under drone control when in PRE_KICKOFF on on onside kick
		if (human_control &&
			(game_info.game_flags & GF_ONSIDE_KICK) &&
			(game_info.game_mode == GM_PRE_KICKOFF))
			human_control = FALSE;

		// always under drone control when intended receiver on pass or lateral
		if (human_control &&
			(game_info.ball_carrier == -1) &&
			((ball_obj.type == LB_PASS) || (ball_obj.type == LB_LATERAL))&&
			(ball_obj.int_receiver == ppdata->plyrnum))
		{
			human_control = FALSE;
			fake_drone = TRUE;
		}

		if (human_control)
		{
			assert( ppdata->station >= 0 );	assert( ppdata->station <= 3 );
			ppdata->stick_cur = (get_player_49way_current() >> (ppdata->station<<3)) & P_SWITCH_MASK;
			ppdata->but_cur = (get_player_sw_current() >> (ppdata->station<<3)) & (P_A|P_B|P_C);
		}
		else
		{
			if( fake_drone )
			{
				ppdata->but_cur = (get_player_sw_current() >> (ppdata->station<<3)) & (P_A|P_B|P_C);
			}

			get_drone_switches( ppdata, fake_drone );
		}

		// if we're backpedaling now, set bp_expire_time to 20
		// if we weren't backpedaling and we aren't now, do nothing
		// if we were backpedaling and we aren't now, decrement bp_expire_time.
		//   if it's non-zero, keep backpedaling.
		if (pobj->flags & PF_BACKPEDAL)
			ppdata->bp_expire_time = 20;
		else if ((was_backp) && (ppdata->bp_expire_time -=1))
			pobj->flags |= PF_BACKPEDAL;

		ppdata->but_new = ppdata->but_cur & ~(ppdata->but_old);

#ifdef DEBUG
#if 0
		// update ani debug bit
//		if( ppdata->but_cur & P_C )
//		if(( ppdata->plyrnum == ball_obj.int_receiver ) && halt )
		if(( ppdata->plyrnum == game_info.ball_carrier ) && halt )
		{
			if(!( pobj->adata[0].animode & MODE_ANIDEBUG ))
			{
				pobj->adata[0].animode |= MODE_ANIDEBUG;
				pobj->adata[1].animode |= MODE_ANIDEBUG;
				fprintf( stderr, "anim debug enabled on player %d\n", ppdata->plyrnum );
			}
		}
		else
		{
			if( pobj->adata[0].animode & MODE_ANIDEBUG )
			{
				pobj->adata[0].animode &= ~MODE_ANIDEBUG;
				pobj->adata[1].animode &= ~MODE_ANIDEBUG;
				fprintf( stderr, "anim debug disabled on player %d\n", ppdata->plyrnum );
			}
		}
#endif
#endif //DEBUG

		// decrement timer on attack boxes
		if( ppdata->attack_time )
		{
			if(( ppdata->attack_time -= 1) == 0 )
				ppdata->attack_mode = 0;
		}

		// decrement timer on celebration bit
		if( ppdata->celebrate_time )
		{
			if(( ppdata->celebrate_time -= 1) == 0 )
				pobj->flags &= ~PF_CELEBRATE;
		}

		// decrement override timer
		if( ppdata->override_time )
		{
			if(( ppdata->override_time -= 1) == 0 )
				pobj->flags |= PF_SPEED_OVERRIDE;
		}

		// decrement no_catch_time
		ppdata->no_catch_time = GREATER(ppdata->no_catch_time-1,0);

		// set or clear turbo bit
		if( pobj->flags & PF_TURBO )
			pobj->flags |= PF_OLD_TURBO;
		else
			pobj->flags &= ~PF_OLD_TURBO;

		if ((ppdata->but_cur & P_C) &&
			((ISDRONE(ppdata)) ||
			((game_info.ball_carrier == -1) && (ball_obj.type == LB_PASS) && (ball_obj.int_receiver == ppdata->plyrnum)) ||
			((ppdata->station >= 0) && (pdata_blocks[ppdata->station].turbo > 0.0f))))
		{
			pobj->flags |= PF_TURBO;

			// must hit turbo within 10 ticks of last release for spin move
			if ((ppdata->non_turbo_time > 0) && (ppdata->non_turbo_time < 9))
				ppdata->non_turbo_time = -1;
			else
				ppdata->non_turbo_time = 0;
		}
		else
		{
			pobj->flags &= ~PF_TURBO;
			ppdata->non_turbo_time = ppdata->non_turbo_time + 1;
		}

		// read double tap of JUMP (A) button
		if( ppdata->but_cur & P_A )
		{
			// must hit turbo within 5 ticks of last release for dive move
			if ( (ppdata->non_buta_time > 0) && (ppdata->non_buta_time < 9))
				ppdata->non_buta_time = -1;
			else ppdata->non_buta_time = 0;
		}
		else
			ppdata->non_buta_time++;


		// read double tap of ???? (B) button
		if( ppdata->but_cur & P_B )
		{
			// must hit turbo within 5 ticks of last release for dive move
			if ( (ppdata->non_butb_time > 0) && (ppdata->non_butb_time < 9))
				ppdata->non_butb_time = -1;
			else ppdata->non_butb_time = 0;
		}
		else
			ppdata->non_butb_time++;


		// call plyrmode function
		mode_functions[pobj->plyrmode]( ppdata );

		// animate
		if( !(pobj->flags & PF_ANITICK ))
			animate_model( pobj );

		if( !(pobj->adata[0].animode & MODE_PUPPET ))
		{
			// stop player from running too far outta bounds!!
			pobj->z = LESSER(pobj->z,FIELDW+120.0f);
			pobj->z = GREATER(pobj->z,-(FIELDW+120.0f));
			pobj->x = LESSER(pobj->x,FIELDHB+35.0f);
			pobj->x = GREATER(pobj->x,-(FIELDHB+35.0f));

			if ((!(pobj->adata[0].animode & MODE_UNINT) || (pobj->adata[0].seq_index == (SEQ_SPIN))) &&
				(game_info.game_mode == GM_IN_PLAY) &&
				(!(pup_runob & TEAM_MASK(ppdata->team))))
			{
				needed_oob_correction = FALSE;

				// out-of-bounds correction
				if( pobj->z + pobj->vz > (FIELDW - 4.0f) )
				{
					needed_oob_correction = TRUE;
					if( pobj->vz > 0.0f )
					{
						// velocity points out of bounds
						pobj->vz = -0.5f;
					}
					else
					{
						// velocity isn't enough to put us in bounds
						if( pobj->vz > -0.5f )
							pobj->vz = -0.5f;
					}

					if( pobj->z + pobj->vz <= (FIELDW - 4.0f) )
						pobj->vz = (FIELDW - 4.0f) - pobj->z;
				}

				if( pobj->z + pobj->vz < -(FIELDW - 4.0f) )
				{
					needed_oob_correction = TRUE;
					if( pobj->vz < 0.0f )
					{
						// velocity points out of bounds
						pobj->vz = 0.5f;
					}
					else
					{
						// velocity isn't enough to put us in bounds
						if( pobj->vz < 0.5f )
							pobj->vz = 0.5f;
					}

					if( pobj->z + pobj->vz >= -(FIELDW - 4.0f) )
						pobj->vz = -(FIELDW - 4.0f) - pobj->z;
				}

				if( pobj->x + pobj->vx > (FIELDHB - 5.0f) )
				{
					needed_oob_correction = TRUE;
					if( pobj->vx > 0.0f )
					{
						// velocity points out of bounds
						pobj->vx = -0.5f;
					}
					else
					{
						// velocity isn't enough to put us in bounds
						if( pobj->vx > -0.5f )
							pobj->vx = -0.5f;
					}

					if( pobj->x + pobj->vx <= (FIELDHB - 5.0f) )
						pobj->vx = (FIELDHB - 5.0f) - pobj->x;
				}

				if( pobj->x + pobj->vx < -(FIELDHB - 5.0f) )
				{
					needed_oob_correction = TRUE;
					if( pobj->vx < 0.0f )
					{
						// velocity points out of bounds
						pobj->vx = 0.5f;
					}
					else
					{
						// velocity isn't enough to put us in bounds
						if( pobj->vx < 0.5f )
							pobj->vx = 0.5f;
					}

					if( pobj->x + pobj->vx >= -(FIELDHB - 5.0f) )
						pobj->vx = -(FIELDHB - 5.0f) - pobj->x;
				}

				if (needed_oob_correction && (ppdata->odata.plyrmode == MODE_WPN))
					abort_drone_route( ppdata->plyrnum );

			} // if !UNINT

			// add velocity
			pobj->x += pobj->vx;
			pobj->y += pobj->vy + GRAVITY / 2.0f;
			pobj->z += pobj->vz;

			// apply gravity
			pobj->vy += GRAVITY;
			if( pobj->y <= 0.0f )
			{
				pobj->y = 0.0f;
				pobj->vy = 0.0f;
			}
		} // if !PUPPET

		// apply friction
		if( pobj->friction != 0.0f )
		{
			mxz = fsqrt( pobj->vx * pobj->vx + pobj->vz * pobj->vz );

			if( mxz < 0.0001f )
			{
				pobj->vx = 0.0f;
				pobj->vz = 0.0f;
				pobj->friction = 0.0f;
			}
			else
			{
				if( pobj->friction > mxz )
				{
					frx = pobj->vx;
					frz = pobj->vz;
					pobj->friction = 0;
				}
				else
				{
					frx = pobj->friction * pobj->vx / mxz;
					frz = pobj->friction * pobj->vz / mxz;
					frx *= field_friction_mult_factor[pup_field];
					frz *= field_friction_mult_factor[pup_field];
				}

				pobj->vx -= frx;
				pobj->vz -= frz;
			}
		}

		pobj->flags |= PF_INBND;
		if (!in_bounds(pobj))
			pobj->flags ^= PF_INBND;

		#ifdef DEBUG
		sanity_check("pre player sleep");
		#endif

		pftimer_end( PRF_PLAYER1 + ppdata->plyrnum );

		// sleep
		sleep( 1 );

		pftimer_start( PRF_PLAYER1 + ppdata->plyrnum );


		#ifdef DEBUG
		sanity_check("post player sleep");
		#endif
	}
}


//////////////////////////////////////////////////////////////////////////////
// player_proc - main player process.
// arg0 = plyrnum
// arg1 = team
// arg2 = field position
// arg3 = who to be (0 to (NUM_PERTEAM_SPD-1)) on which team (tm# * NUM_PERTEAM_SPD)
//
void tmsel_player_proc(int *parg)
{
	fbplyr_data	*ppdata;
	int			plyrnum, plyrteam;
	obj_3d		*pobj;
	char		cbuf[8];
	float		mxz,frx,frz;
	int			did_celebrate = 0;

//	int (*mode_functions[])(fbplyr_data *) = {
//			_mode_lineup, _mode_dead, _mode_lineps, _mode_line, _mode_qbps,
//			_mode_qb, _mode_wpnps, _mode_wpn, _mode_dbps, _mode_db,
//			_mode_kicker };

	// extract arguments
	plyrnum = parg[0];
	plyrteam = parg[1];

	ppdata = &player_blocks[plyrnum];
	pobj = (obj_3d *)ppdata;

	// wipe player data
	memset( (void *)ppdata, 0, sizeof( fbplyr_data ));

	// init player data
	pobj->x     = (plyrteam ? -18.0f : 18.0f);
	pobj->y     = 0.0f;
	pobj->z     = 65.0f;
	pobj->fwd   = (plyrteam ? 512 : 490);
	if (!plyrteam)
		pobj->flags |= PF_FLIP;


	ppdata->plyrnum     = plyrnum;
	ppdata->team        = plyrteam;
	ppdata->position    = parg[2];
//	ppdata->static_data = teaminfo[parg[3]/NUM_PERTEAM_SPD].static_data + (parg[3] % NUM_PERTEAM_SPD);
	ppdata->static_data = teaminfo[parg[3]].static_data + 3;

	pobj->pdecal        = team_decals[(plyrteam * 2) + ((ppdata->static_data->color == PC_WHITE) ? 0 : 1)];
	ppdata->stick_cur   = 24;
	ppdata->but_cur     = 0;
	pobj->ascale        = ppdata->static_data->scale;
	pobj->plyrmode      = MODE_QB;
	pobj->friction      = 0.0f;
	ppdata->attack_time = 0;
	ppdata->celebrate_time = 0;
	ppdata->stick_time  = 0;

//	do_pup_player_scale(ppdata);

	// clear puppet shit...just in case
//	pobj->adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);
//	ppdata->puppet_link = NULL;
//	ppdata->puppet_link->puppet_link = NULL;
//	ppdata->puppet_rel_time = pcount;

	// initialize anim data
	pobj->adata[0].adata = ppdata->anidata[0];
	pobj->adata[0].idata = ppdata->anidata[1];
	pobj->adata[1].adata = ppdata->anidata[2];
	pobj->adata[1].idata = ppdata->anidata[3];

	pobj->adata[2].adata = ppdata->anidata[4];
	pobj->adata[3].adata = ppdata->anidata[5];

	pobj->adata[1].animode = MODE_SECONDARY;

	create_object(cbuf, OID_PLAYER, OF_NONE, DRAWORDER_PLAYER, (void *)ppdata, draw_player2);

	// init animation
	if (plyrteam)
		change_anim(pobj, qb_ready_anim);
	else
		change_anim(pobj, qb_ready_f_anim);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

	// 'team select' player loop
	while( 1 )
	{
		int		was_backp, needed_oob_correction;
		int		human_control;


		human_control = FALSE;

		// decrement timer on celebration bit
		if( ppdata->celebrate_time )
		{
			if(( ppdata->celebrate_time -= 1) == 0 )
				pobj->flags &= ~PF_CELEBRATE;
		}

// TEMP TEMP
//		if ((game_info.team[plyrteam] != -1) && (!did_celebrate))
//		{
//			if (pobj->flags & PF_FLIP)
//				change_anim(pobj, tmsel_dance3_anim);
////				change_anim(pobj, tm_selected_dance_f_tbl[randrng(sizeof(tm_selected_dance_f_tbl)/sizeof(char *))]);
//			else
//				change_anim(pobj, tmsel_dance3_anim);
////				change_anim(pobj, tm_selected_dance_tbl[randrng(sizeof(tm_selected_dance_tbl)/sizeof(char *))]);
//			did_celebrate = 1;
//		}

		// animate
  		animate_model( pobj );

		// sleep
		sleep( 1 );
	}
}


//////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
static void sanity_check( char *text )
{
	int			i;
	obj_3d		*pobj;
	fbplyr_data	*ppdata;

	int			twopart_count = 0;

	for( i = 0; i < 14; i++ )
	{
		ppdata = player_blocks + i;
		pobj = (obj_3d *)ppdata;

		if (pobj->adata->animode & MODE_TWOPART)
			twopart_count += 1;

		if( pobj->adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER))
		{
			if( ppdata->puppet_link == NULL )
			{
				fprintf( stderr, "sanity check failed - no puppet link on "
						"player %d\n", i );
				fprintf( stderr, "sanity check \"%s\"\n", text );
				dump_anim_data( 0 );
				lockup();
			}

			if( ppdata->puppet_link->puppet_link != ppdata )
			{
				fprintf( stderr, "sanity check failed - bogus puppet link on "
						"player %d\n", i );
				fprintf( stderr, "sanity check \"%s\"\n", text );
				dump_anim_data( 0 );
				lockup();
			}
		}
	}

	if (twopart_count > 2)
	{
		fprintf( stderr, "twopart_count = %d\n", twopart_count );
		lockup();
	}

	#ifdef MALLOC_DEBUG
	check_fences();
	#endif
}
#endif

//////////////////////////////////////////////////////////////////////////////
//  This process causes QB to yell signals, including audibles - but not the
//  word "hike"
//////////////////////////////////////////////////////////////////////////////
void yell_signals_proc(int *args)
{
	int	snd1;

	int set_snds[] =
	{
		P_SET1_SP,
		P_SET2_SP,
		P_SET3_SP
	};

	int audible_snds[] =
	{
		P_44_SP,
		P_BLUE_SP,
		P_RED_SP,
		P_42_SP
	};

	snd1 = randrng( sizeof(set_snds)/sizeof(int) );
	snd_scall_bank(plyr_bnk_str,set_snds[snd1],VOLUME1,127,PRIORITY_LVL1);

	sleep(120);

	while (!(game_info.game_mode == GM_IN_PLAY))
	{
/*
		if( game_info.audible == 1)
		{
			game_info.audible = 2;
//#ifdef DAN
//			game_info.audible = 0;
//#endif
			snd1 = randrng( sizeof(audible_snds)/sizeof(int) );
			snd_scall_bank(plyr_bnk_str,audible_snds[snd1],VOLUME1,127,PRIORITY_LVL1);

			sleep(40+(randrng(35)));

			snd1 = randrng( sizeof(audible_snds)/sizeof(int) );
			snd_scall_bank(plyr_bnk_str,audible_snds[snd1],VOLUME1,127,PRIORITY_LVL1);

			sleep(40+(randrng(35)));

			snd1 = randrng( sizeof(audible_snds)/sizeof(int) );
			snd_scall_bank(plyr_bnk_str,audible_snds[snd1],VOLUME1,127,PRIORITY_LVL1);

			sleep(40+(randrng(35)));

			game_info.audible = 3;
//#ifdef DAN
//			game_info.audible = 0;
//#endif
		}
*/
		sleep(1);
	}
}


//////////////////////////////////////////////////////////////////////////////
// reinit_player - reinitializes a player to match a different static player data
// block.
void reinit_player( int plyrnum, int dnum )
{
	fbplyr_data	*ppdata;

	ppdata = player_blocks + plyrnum;

	ppdata->static_data  = teaminfo[dnum / NUM_PERTEAM_SPD].static_data + (dnum % NUM_PERTEAM_SPD);
	//ppdata->static_data = plyrinfo + dnum;

	ppdata->odata.pdecal = team_decals[(ppdata->team * 2) + ((ppdata->static_data->color == PC_WHITE) ? 0 : 1)];
	ppdata->odata.ascale = ppdata->static_data->scale;

	do_pup_player_scale(ppdata);
}

//////////////////////////////////////////////////////////////////////////////
//
// player modes
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// lining up between plays
static int _mode_lineup( fbplyr_data *ppdata )
{
#ifdef DEBUG
	fprintf( stderr, "Hey, nobody should ever be in mode_lineup!!!\n" );
	lockup();
#endif
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
// do nothing
static int _mode_dead( fbplyr_data *ppdata )
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// lineman before the snap
static int _mode_lineps( fbplyr_data *ppdata )
{
	obj_3d *pobj = (obj_3d *)ppdata;

	// has the ball been snapped?
	if( game_info.game_mode == GM_IN_PLAY )
	{
		if( ppdata->team == game_info.off_side )
		{
			// offense
			if (ppdata->position != 1)
				change_anim( pobj, stance3pt_jumpback_anim );
		}
		else
		{
			// defenese
			change_anim( pobj, charge_anim );
//			change_anim( pobj, block_ready_anim );
		}
		pobj->plyrmode = MODE_LINE;
	}
	#ifdef JASON
	else
	{
		if ((ppdata->position != 1) &&
			(pobj->y > 0.0f))
		{
			pobj->y = 0.0f;
//			#ifdef DEBUG
//			fprintf( stderr, "HEY! - lineman %2x is airborne prior to the snap\n",
//				JERSEYNUM(ppdata->plyrnum));
//			lockup();
//			#endif
		}
	}
	#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// lineman after the snap
static int _mode_line( fbplyr_data *ppdata )
{
	obj_3d *pobj = (obj_3d *)ppdata;
	int		face_dir;

	// HACK! - just call mode_db instead.  see what happens
	if (ppdata->team != game_info.off_side)
	{
		_mode_db( ppdata );
		return 0;
	}

	pobj->turn_rate = 64;
	pobj->turn_rate2 = 48;

	// turn toward target heading
	turn_toward_tgt( pobj );

	// if ball carrier is in mode_wpn, switch to DB
	if ((game_info.ball_carrier != -1) &&
		(player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_WPN))
	{
		if (ppdata->team == game_info.off_side)
			ppdata->odata.plyrmode = MODE_WPN;
		else
			ppdata->odata.plyrmode = MODE_DB;
		return 0;
	}

	if(!( pobj->adata[0].animode & MODE_UNINT ))
	{
		// we're not in MODE_UNINT
		standard_run( pobj );

		if (ppdata->but_new & P_A)
		{
			if ((game_info.ball_carrier == -1) && (game_info.game_mode != GM_PLAY_OVER))
			{
				if (ppdata->but_cur & P_C)
				{
				 	if ((face_dir = dir49_table[ppdata->stick_cur]) < 0) face_dir = 0;
				 	face_dir = 1023 & (face_dir + (int)RD2GR(cambot.theta));
				 	pobj->fwd = face_dir;
				 	pobj->tgt_fwd = face_dir;

					if ((ball_obj.type == LB_BOBBLE) || (ball_obj.type == LB_FUMBLE))
						change_anim(pobj, c_dive_for_ball_anim);
					else
						change_anim(pobj, t_crzdiv_anim);
				}
				else
				{	// just A button...no turbo
					if ((ball_obj.type == LB_BOBBLE) || (ball_obj.type == LB_FUMBLE))
						change_anim(pobj, c_dive_for_ball_anim);
					else if (ball_obj.total_time - ball_obj.flight_time > 10)
						change_anim(pobj, intercept_anim_tbl[randrng(sizeof(intercept_anim_tbl)/sizeof(char *))]);
				}
			}
			else
			{
				// if ball carrier is hittable and were close to em...fuck'em up!!!
				if ((player_blocks[game_info.ball_carrier].odata.flags & PF_LATEHITABLE) &&
					(distxz[ppdata->plyrnum][game_info.ball_carrier]/YD2UN < 5.0f))
				{
					// don't allow drones to hit after play ends
					if (ISHUMAN(ppdata) || (game_info.game_mode != GM_PLAY_OVER))
					{
						if (pup_violence)
							change_anim(pobj, violent_db_late_hits_tbl[randrng(sizeof(violent_db_late_hits_tbl)/sizeof(char *))]);
						else
							change_anim(pobj, db_late_hits_tbl[randrng(sizeof(db_late_hits_tbl)/sizeof(char *))]);
					}
				}
				else
				{
				 	if ((face_dir = dir49_table[ppdata->stick_cur]) < 0) face_dir = 0;
				 	face_dir = 1023 & (face_dir + (int)RD2GR(cambot.theta));
				 	pobj->fwd = face_dir;
				 	pobj->tgt_fwd = face_dir;
					change_anim( pobj, t_crzdiv_anim );
				}
			}
		}

		if (game_info.ball_carrier == ppdata->plyrnum)
			maybe_lateral(pobj);
	}

	standard_catch(pobj);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// qb before the snap
static int _mode_qbps( fbplyr_data *ppdata )
{
	obj_3d	*pobj = (obj_3d *)ppdata;
	int		i;
	int delay;

//	// wait for the lineup to finish
//	if( game_info.game_mode == GM_LINING_UP)	// mode never used!!
//		return 0;

	// Don't do anything if QB in middle of pointing/audible seq.
	if (pobj->adata[0].animode & MODE_UNINT)
		return 0;

	if ((game_info.team_play[game_info.off_side]->flags &
			(PLF_PUNT|PLF_FAKE_PUNT|PLF_FIELD_GOAL|PLF_FAKE_FG)) == 0)
	{
		if(audible_check(ppdata, OFFENSE) == 0)
			return(0);
	}



	if ((audible_tap_action[!game_info.off_side] == 1) &&
		( !(game_info.team_play[game_info.off_side]->flags & (PLF_FIELD_GOAL|PLF_FAKE_FG|PLF_PUNT|PLF_FAKE_PUNT))) )
		return (0);

	// we're not in MODE_UNINT

//	// If he has already done an audible, skip checking for more...
//	if( game_info.audible == 0)
//	{
//		if(0)
//		{
//			if( randrng(400) < 2)		// do 10% of the time
//			{
//				game_info.audible = 1;
//				if (randrng(100) < 50 )
//					change_anim(pobj, qbptr_anim);
//				else
//					change_anim(pobj, qbptl_anim);
//			}
//		}
//	}

	// snap?  Auto hike cntr is set in NFL.C, player can not snap ball for first 1.5 seconds or so
	// snap?  Auto hike cntr is set in NFL.C, player can not snap ball for first 1.5 seconds or so
	delay = (game_info.team_play[game_info.off_side]->flags & (PLF_FIELD_GOAL|PLF_FAKE_FG|PLF_PUNT|PLF_FAKE_PUNT) ?  MAX_HIKE_TIME - (10): MAX_HIKE_TIME - (tsec));

#ifndef NOAUTOHIKE
	// Shipping code here

	if ((--game_info.auto_hike_cntr  < 1) ||									// If hike cntr gets to 0, then auto hike
	    ( (( ppdata->but_new & P_B ) || (ppdata->but_new & P_A )) &&			// Both A or B butn will hike
		(game_info.auto_hike_cntr  < (delay)) &&  				// Don't allow hike in first 1 second
//		(audible_tap_action[!game_info.off_side] != 1) 
		(1)))
#else
	// Debug code here
	if(game_info.auto_hike_cntr-- < 10)
		game_info.auto_hike_cntr =  20;

	if(((( ppdata->but_new & P_B ) || (ppdata->but_new & P_A )) &&
		(game_info.auto_hike_cntr  < (MAX_HIKE_TIME - (30)))))
#endif
	{
		// create the A+B snap monitor
		qcreate("snapmon",SNAP_MON_PID,motion_block_monitor,ppdata->station,0,0,0);

		if (game_info.auto_hike_cntr == 0)
			// The ball was auto hiked!  Don't allow an accidental pass in next 15 ticks or so!
			game_info.disallow_pass_cntr = DISALLOW_PASS_TIME;

		// hike!  gimme the ball, change player mode, animate
		snd_scall_bank(plyr_bnk_str,P_HIKE_SP,VOLUME1,127,PRIORITY_LVL0);

		// Do funny blitzing yell/screams
		if ((game_info.team_play[!game_info.off_side]->flags & PLF_BLITZ) && (randrng(100) < 5))
			snd_scall_bank(plyr_bnk_str,P_BLITZ_YELL_SND,VOLUME4,127,PRIORITY_LVL5);

		// Kill QB yelling process
		killall(QBYELL_PID,-1);

		if( game_info.play_type != PT_EXTRA_POINT )
		{
			clock_active = 1;		// let game clock run again
			// Cntr used for slowing clock down if too much time is being burned
			// Make sure we reset this on all other plays that can start the clock
			game_info.play_time = 0;
		}

		// set here for cambot, normally set in hike_ball()
		game_info.game_flags |= GF_HIKING_BALL;
		ball_obj.bounces = 0;

		clean_audible_text(0);	// Clean up audible box if we have one up there still
		clean_audible_text(1);	// Clean up audible box if we have one up there still
		reset_audible_check();	// Reset "Shown" flags 'n stuff

		// center snaps ball
		change_anim( (obj_3d *)&player_blocks[(game_info.off_side ? 8 : 1)], center_snap_anim );


		pobj->plyrmode = MODE_QB;

		// quarterback speech handler
		qcreate("qbspch",QB_SPEECH_PID,qb_actions_sp,ppdata->plyrnum,0,0,0);

		// set play_pcount
		game_info.play_pcount = pcount;

		// set us UNINT so that qb_run will initialize
//		pobj->adata[0].animode |= MODE_UNINT;
//		qb_run( pobj );

		// go through all the receivers and set their REVWPN bits if
		// they're left of the ball.
		for( i = 4; i < 7; i++ )
		{
			if(( game_info.off_side && ( player_blocks[i+game_info.off_side*7].odata.z > (game_info.line_pos + 2.0f))) ||
				( !game_info.off_side && ( player_blocks[i+game_info.off_side*7].odata.z < (game_info.line_pos - 2.0f))))
			 	player_blocks[i+game_info.off_side*7].odata.flags |= PF_REVWPN;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// qb - with ball, behind LOS
static int _mode_qb( fbplyr_data *ppdata )
{
	obj_3d	*pobj = (obj_3d *)ppdata;
	int		face_dir,brng;
	float	dist2tgt;			// in yards
	float	tgt_pt[3];


	// MUST wait to catch ball...before anything else
	if ( ((game_info.game_flags & GF_HIKING_BALL) && (game_info.ball_carrier == -1)) ||
		 ((game_info.game_flags & GF_HIKING_BALL) && (game_info.team_play[game_info.off_side]->flags)) )
	{
		// and within 4 feet of us in xz
		// and within a reasonable Y distance
		if ((odistxz(pobj, (obj_3d *)&ball_obj ) < 8.0f) &&
			(ball_obj.odata.y >= pobj->y) &&
			(ball_obj.odata.y < (pobj->y + pobj->ay + 12.0f)))
		{
			clean_audible_text(0);	// Clean up audible box if we have one up there still
			clean_audible_text(1);	// Clean up audible box if we have one up there still

			// Catch ball snd
			snd_scall_bank(plyr_bnk_str,P_CATCH_BALL_SND,VOLUME4,127,PRIORITY_LVL4);

			// snag it
			game_info.game_flags &= ~GF_HIKING_BALL;
			game_info.ball_carrier = ppdata->plyrnum;

#ifdef TM	// test tackles
		pobj->plyrmode = MODE_WPN;
#endif

//			if( p_status & (1<<ppdata->team))
//			{
//				game_info.plyr_control[ppdata->team] = ppdata->plyrnum;
//				update_stations();
//			}

			// if punt play...kick it!!!
			if (game_info.team_play[game_info.off_side]->flags & PLF_PUNT)
			{
				snd_scall_bank(plyr_bnk_str,P_CATCH_BALL_SND,VOLUME3,127,PRIORITY_LVL4);
				change_anim(pobj, punt_ball_anim);
				pobj->plyrmode = MODE_WPN;
				return 0;
			}

			// if field goal....
			if (game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL)
			{
				change_anim(pobj, qbhut_place_ball_anim);
				return 0;
			}

			// if fake field goal....
			if (game_info.team_play[game_info.off_side]->flags & PLF_FAKE_FG)
			{
				// let qb move...
//				qb_run(pobj);

				// kicker becomes a weapon.
				player_blocks[game_info.off_side ? 13 : 6].odata.plyrmode = MODE_WPN;
//				return 0;
			}


			// set UNINT so that qb_run will initialize, only if player didnt move stick already
			if ((ppdata->stick_cur == 24) && (ppdata->stick_old == 24))
			{
				pobj->adata[0].animode |= MODE_UNINT;
				qb_run(pobj);
			}
		}
		else
			return 0;			// wait to catch ball
	}


	// if we're past of the line of scrimmage, shift into MODE_WPN
	if(( game_info.game_flags & GF_BALL_CROSS_LOS ) && (game_info.game_mode == GM_IN_PLAY ))
	{
		if (!(pobj->adata[0].animode & MODE_UNINT))
			change_anim( pobj, ppdata->plyrnum == game_info.ball_carrier ? run_wb_anim : run_anim );
		pobj->plyrmode = MODE_WPN;
		snd_scall_bank(plyr_bnk_str,OVER_LOS_SND,VOLUME2,127,PRIORITY_LVL3);

		// Start a flash player white process
		qcreate ("flash", PFLASH_PID, flash_plyr_proc, ppdata->plyrnum, 4, 3, 0);

		los_flash_time = 64;

		if (game_info.ball_carrier != -1)
			qb_runs_past_los_sp();
		killall(QB_SPEECH_PID,0xffffffff);

		return 0;
	}

	turn_toward_tgt( pobj );

 	if ((face_dir = dir49_table[ppdata->stick_cur]) < 0) face_dir = 0;
	face_dir = 1023 & (face_dir + (int)RD2GR(cambot.theta));

	// out of bounds?
	if( ppdata->plyrnum == game_info.ball_carrier )
	{
		if( !in_bounds( pobj ) && (game_info.game_mode == GM_IN_PLAY))
		{
#ifdef PLAY_DEBUG
			fprintf( stderr, "  off QB %2x(%d) goes out of bounds at %5.2f, field %d\n",
					JERSEYNUM(ppdata->plyrnum), ppdata->team, pobj->x,
					FIELDX( pobj->x ));
#endif

			if (game_info.game_mode != GM_PLAY_OVER)
				snd_scall_bank(plyr_bnk_str,WHISTLE1_SND+randrng(2),VOLUME4,127,PRIORITY_LVL5);

			// mark the play dead
			game_info.game_mode = GM_PLAY_OVER;

			// if GF_AIR_CATCH is set, this is either a FUMBLE_OOB or INCOMPLETE
			if( game_info.game_flags & GF_AIR_CATCH )
			{
				game_info.play_end_cause = PEC_BALL_OOB;
#ifdef PLAY_DEBUG
				fprintf( stderr, "  pec set to BALL_OOB because of AIR_CATCH\n" );
#endif
			}
			else if( oob_zone( pobj ) == 1 )
			{
				game_info.play_end_cause = PEC_CAR_TACKLED_OWNEZ;
#ifdef PLAY_DEBUG
				fprintf( stderr, "  carrier ran out of bounds in own EZ\n" );
#endif
			}
			else
			{
				game_info.play_end_cause = PEC_CAR_OOB;
			}

			game_info.play_end_pos = pobj->x;
			game_info.play_end_z = pobj->z;

			return 0;
		}
	}

	if (!(pobj->adata[0].animode & MODE_UNINT) &&
		!(game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL) &&
		!(game_info.team_play[game_info.off_side]->flags & PLF_PUNT))
	{
		// we're not in MODE_UNINT
		qb_run( pobj );

		// Has double turbo been pressed, flagging a spin move or bitch slap?
		if (((ppdata->non_turbo_time == -1) || (ppdata->but_new & P_D)) &&
			(!( pobj->adata[1].animode & MODE_UNINT)))
		{
//				fprintf( stderr, "qb spinning\n" );
			 	game_info.spin_cnt[ppdata->team] += 1;
				change_anim(pobj, spin_anim);
#if 1	//(DHS)
				if (player_blocks[ppdata->plyrnum].plyrnum == game_info.ball_carrier)
#endif
					spin_move_speech();
		}

		// hurdle?
//		if ((ppdata->but_new & P_A) && (!(pobj->adata[1].animode & MODE_UNINT)) && (game_info.game_mode != GM_PLAY_OVER))
		if ((ppdata->but_new & P_A) && (!(pobj->adata[1].animode & MODE_UNINT)) )
		{
			if (ppdata->but_cur & P_C)
			{
				// turbo hurdle
				pobj->fwd = face_dir;
				pobj->tgt_fwd = face_dir;
				if (pdata_blocks[ppdata->team].turbo > 2.0f)
				{
					game_info.hurdle_cnt[ppdata->team] += 1;
					change_anim(pobj, turbo_hurdle_anim);
				}
				else
					change_anim(pobj, hurdle_anim);
			}
			else
			{
		   		pobj->fwd = face_dir;
		   		pobj->tgt_fwd = face_dir;
				change_anim(pobj, hurdle_anim);
			}
		}

		// dec cntr which disallows passing for x ticks after an auto hike
		if( game_info.disallow_pass_cntr )
			game_info.disallow_pass_cntr--;

		// throw ball if B butn pressed & disallow pass cntr is 0
		if ((ppdata->but_new & P_B) &&
			(game_info.disallow_pass_cntr == 0) &&
			(ppdata->plyrnum == game_info.ball_carrier) &&
			(!( pobj->adata[1].animode & MODE_UNINT)))
		{
			// only use pick_receiver if it's a human-controlled QB.
			// if it's a drone QB, read from drone_throw_target
			if (ISHUMAN(ppdata))
			{
				ppdata->throw_target = pick_receiver( pobj );
				// flash target receiver
//				qcreate ("flash", PLAYER_PID, flash_plyr_proc, ppdata->throw_target, 4, 3, 0);
				qcreate ("flash", PFLASH_PID, flash_plyr_proc, ppdata->throw_target, 4, 3, 0);
			}
			else
				ppdata->throw_target = ppdata->drone_throw_target;

			pobj->flags |= PF_THROWING;
			qcreate( "thmon", THROW_MON_PID,qb_throw_monitor,ppdata->plyrnum,0,0,0);

			// stop announcing qb stuff
			killall(QB_SPEECH_PID,0xffffffff);

			if( ppdata->throw_target != -1 )
			{
				brng = bearing( ppdata->plyrnum, ppdata->throw_target );

				dist2tgt = distxz[ppdata->plyrnum][ppdata->throw_target]/YD2UN;
				if ((dist2tgt < 7.0f) && (FIELDX(pobj->x) > FIELDX(player_blocks[ppdata->throw_target].odata.x)))
				{
					// lateral pass
					// guess where player will be in 70 ticks (1.3 seconds)
					tgt_pt[0] = player_blocks[ppdata->throw_target].odata.x + (player_blocks[ppdata->throw_target].odata.vx * 70.0f);
					tgt_pt[1] = 0.0f;
					tgt_pt[2] = player_blocks[ppdata->throw_target].odata.z + (player_blocks[ppdata->throw_target].odata.vz * 70.0f);

					brng = (1024 + pobj->fwd - ptangle(&(pobj->x),tgt_pt)) % 1024;

					pobj->tgt_fwd2 = p2p_angles[ppdata->plyrnum][ppdata->throw_target];
					pobj->turn_rate2 = brng/4;

//					if (randrng(100) < 80)
//						change_anim(pobj, (pobj->flags & PF_FLIP) ? new_lateral_r_anim : new_lateral_l_anim);
//					else
//					{
					if (game_info.off_side)
					{
						if (pobj->z > tgt_pt[2])
							change_anim(pobj, m_laterall_anim);
						else
							change_anim(pobj, m_lateralr_anim);
					}
					else
					{
						if (pobj->z <= tgt_pt[2])
							change_anim(pobj, m_laterall_anim);
						else
							change_anim(pobj, m_lateralr_anim);
					}
//					}
					lateral_speech();
				}
				else if ((dist2tgt < 15.0f) && (FIELDX(pobj->x) <= FIELDX(player_blocks[ppdata->throw_target].odata.x)))
				{
					// Short pass...target in front of me
					if (brng <= 320)
					{
						// running pass
						begin_twopart_mode(pobj);
						pobj->tgt_fwd2 = p2p_angles[ppdata->plyrnum][ppdata->throw_target];
						pobj->turn_rate2 = brng/4;
						if (pobj->flags & PF_FLIP)
							change_anim2(pobj, qb_rthrow_sl_anim);
						else
							change_anim2(pobj, qb_rthrow_sr_anim);
					}
					else
					{
						pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ppdata->throw_target];
						pobj->turn_rate = brng/4;
//						change_anim(pobj, qb_shuttle_pass_anim);
						if (pobj->flags & PF_FLIP)
							change_anim(pobj, qb_throw_sl_anim);
						else
							change_anim(pobj, qb_throw_sr_anim);
					}
				}
				else if (dist2tgt < 35.0f)
				{
					// Medium pass
					pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ppdata->throw_target];
					pobj->turn_rate = brng/4;
					if (pobj->flags & PF_FLIP)
						change_anim(pobj, qb_throw_sl_anim);
					else
						change_anim(pobj, qb_throw_sr_anim);
				}
				else
				{
					// Long pass
					pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ppdata->throw_target];
					pobj->turn_rate = brng/4;
					if (pobj->flags & PF_FLIP)
						change_anim(pobj, qb_throw_ll_anim);
					else
						change_anim(pobj, qb_throw_lr_anim);
				}

				if( pobj->flags & PF_TURBO )
					ball_obj.flags |= BF_TURBO_PASS;
				else
					ball_obj.flags &= ~BF_TURBO_PASS;
			}
		}
	}
	// allow qb to throw ball from hurdle and his stiff arm move!
	else if ((ppdata->but_new & P_B) &&
		(ppdata->plyrnum == game_info.ball_carrier) &&
		(game_info.disallow_pass_cntr == 0) &&
		(!(pobj->adata[1].animode & MODE_UNINT)))
	{
		if ((pobj->adata[0].seq_index == (SEQ_HURDLE)) || 
			(pobj->adata[0].seq_index == (SEQ_SPIN)) ||
			(pobj->adata[0].seq_index == (SEQ_QB_STIFF)))
		{
		
			ppdata->throw_target = pick_receiver(pobj);
			if (ppdata->throw_target != -1)
			{
				begin_twopart_mode(pobj);
				pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ppdata->throw_target];
				pobj->tgt_fwd2 = pobj->tgt_fwd;
				if (pobj->flags & PF_FLIP)
					change_anim2(pobj, qb_rthrow_sl_anim);
				else
					change_anim2(pobj, qb_rthrow_sr_anim);

				pobj->flags |= PF_THROWING;
				qcreate( "thmon", THROW_MON_PID,qb_throw_monitor,ppdata->plyrnum,0,0,0);
			}
		}
	}
	// allow qb to throw ball from spin move
	else if ((ppdata->but_new & P_B) &&
		(ppdata->plyrnum == game_info.ball_carrier) &&
		(game_info.disallow_pass_cntr == 0) &&
		(pobj->adata[0].seq_index == (SEQ_SPIN)) &&
		(!(pobj->adata[1].animode & MODE_UNINT)))
	{
		ppdata->throw_target = pick_receiver(pobj);
		if (ppdata->throw_target != -1)
		{
			pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ppdata->throw_target];
			if (pobj->flags & PF_FLIP)
				change_anim(pobj, qb_throw_sl_anim);
			else
				change_anim(pobj, qb_throw_sr_anim);

			pobj->flags |= PF_THROWING;
			qcreate( "thmon", THROW_MON_PID,qb_throw_monitor,ppdata->plyrnum,0,0,0);
		}
	}


	// TEMP
//	game_info.team_fire[ppdata->team] = 1;
	// TEMP
	maybe_plyr_fire(ppdata);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// weapon before the snap
static int _mode_wpnps( fbplyr_data *ppdata )
{
	obj_3d	*pobj = (obj_3d *)ppdata;

	pobj->turn_rate = 64;
	pobj->turn_rate2 = 48;

	// has the ball been snapped?
	if (game_info.game_mode == GM_IN_PLAY)
		pobj->plyrmode = MODE_WPN;

	if (game_info.play_type == PT_KICKOFF)
	{
		if(( game_info.game_flags & GF_QUICK_KICK ) &&
			(ppdata->plyrnum % 7 == 3))
		{
			if((--game_info.auto_hike_cntr <= 0 ) ||
				(( ppdata->but_new & (P_A|P_B)) &&
				(game_info.auto_hike_cntr < (MAX_HIKE_TIME - tsec))))
			{
				// start clock
				clock_active = 1;
				game_info.play_time = 0;

				// drop ball from suspended position
				ball_obj.flags &= ~BF_FREEZE;
				game_info.ko_catch = -1;
				game_info.game_mode = GM_IN_PLAY;
				pobj->plyrmode = MODE_WPN;
			}

			return 0;
		}
		else
		{
			// regular kick
		}
	}

	// maybe change player
//	if(( ppdata->but_new & P_B ) && ( ISHUMAN(ppdata) ))
//		wpnps_change_player( ppdata );

	// turn toward target heading
	turn_toward_tgt( pobj );

	if(!( pobj->adata[0].animode & MODE_UNINT ))
	{
		// we're not in MODE_UNINT

		// no moving unless kickoff or we're the motion guy
		if((game_info.play_type == PT_KICKOFF) ||
			(game_info.team_play[ppdata->team]->formation[ppdata->plyrnum % 7].control & LU_MOTION))
		{
			standard_run(pobj);
		}
	}

	// this is an almost on-fire case...flash dude...if next catch will put him on fire
	if ((tdata_blocks[ppdata->team].last_receiver == ppdata->plyrnum) &&
		(tdata_blocks[ppdata->team].consecutive_comps == OFFENSE_FIRECNT-1) &&
		!(game_info.game_flags & GF_ONSIDE_KICK) &&
		(game_info.play_type != PT_KICKOFF) &&
		(!almost_fire_pflash))
	{
		qcreate ("flash", PFLASH_PID, flash_plyr_proc, ppdata->plyrnum, 10, 3, 0);
//		qcreate ("flash", PFLASH_PID, flash_plyr_red_proc, ppdata->plyrnum, 10, 3, 0);
		snd_scall_bank(plyr_bnk_str,ALMOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL5);
		almost_fire_pflash = 1;
	}

	// this is dude on fire effect at pre-snap
	if ((tdata_blocks[ppdata->team].last_receiver == ppdata->plyrnum) &&
		(tdata_blocks[ppdata->team].consecutive_comps >= OFFENSE_FIRECNT) &&
		!(game_info.game_flags & GF_ONSIDE_KICK) &&
		(game_info.play_type != PT_KICKOFF) &&
		(!fire_pflash))
	{
		qcreate ("fburst", SPFX_PID, player_burst_into_flames, ppdata->plyrnum, 5, 25, 0);
		fire_pflash = 1;
	}

	// if team on fire... burst the man who got them on fire!!
	if ((game_info.team_fire[ppdata->team]) &&
		(tdata_blocks[ppdata->team].last_receiver == ppdata->plyrnum) &&
		(game_info.play_type != PT_KICKOFF) &&
		!(game_info.game_flags & GF_ONSIDE_KICK) &&
		(!fire_pflash))
	{
		qcreate ("fburst", SPFX_PID, player_burst_into_flames, ppdata->plyrnum, 5, 25, 0);
		fire_pflash = 1;
	}


	return 0;

}

//////////////////////////////////////////////////////////////////////////////
// weapon after the snap
static int _mode_wpn( fbplyr_data *ppdata )
{
	obj_3d	*pobj = (obj_3d *)ppdata;
	int		face_dir,i;

	pobj->turn_rate = 64;
	pobj->turn_rate2 = 48;

	// If we're human and carrier is in MODE_WPN, set our BLOCK bit
	// Drone code normally does this, but doesn't run for humans
	if ((ISHUMAN(ppdata)) &&
		(ppdata->job != JOB_BLOCK) &&
		(game_info.ball_carrier != -1) &&
		(player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_WPN))
		ppdata->job = JOB_BLOCK;

	// if we've got the ball, GF_OLS_SPEEDS_SET isn't set, and GF_BALL_CROSS_LOS
	// IS set, it's time to assign new speeds/times to the defensive players.
	if ((game_info.ball_carrier == ppdata->plyrnum) &&
		!(game_info.game_flags & GF_OLS_SPEEDS_SET) &&
		(game_info.game_flags & GF_BALL_CROSS_LOS))
	{
		set_defense_speeds();
		game_info.game_flags |= GF_OLS_SPEEDS_SET;
	}

	// turn toward target heading
	turn_toward_tgt( pobj );

	// maybe catch the ball?
	standard_catch(pobj);

	// ball carrier checks (NOT control logic)
	if (ppdata->plyrnum == game_info.ball_carrier)
	{
#ifndef NO_FIRE
		// normal control check only comes into play on interceptions
		// and the like.  It won't catch an intended receiver gaining
		// control of the ball.  Check for THAT case (for fire purposes)
		// right here.
		if 	((ppdata->team == game_info.last_pos) &&
			(ball_obj.type == LB_PASS) &&
			(game_info.game_mode == GM_IN_PLAY) &&
			((pcount - ball_obj.catch_pcount) == CONTROL_TIME) &&
			!(game_info.team_fire[ppdata->team]) &&
			(FIELDX(game_info.catch_x) >= game_info.los))
		{	// completed forward pass - check fire

			// if this is an AIR_CATCH, set the pending fire check flag
			if (game_info.game_flags & GF_AIR_CATCH)
				ppdata->odata.flags |= PF_PEND_FCHECK;
			else
				check_receiver_fire( ppdata );
		}
#endif

		// check for CONTROL
		if ((ppdata->team != game_info.last_pos ) &&
			(game_info.game_mode == GM_IN_PLAY ) &&
			((pcount - ball_obj.catch_pcount) >= CONTROL_TIME ))
		{
			#ifdef PLAY_DEBUG
			fprintf( stderr, "  player %2x(%d) gets control, last_pos goes from %d to %d\n",
				JERSEYNUM(ppdata->plyrnum), ppdata->team, game_info.last_pos, game_info.off_side );
			#endif
			game_info.game_flags |= GF_POSS_CHANGE;
			update_status_box = 1;

			// turnover?
			if (game_info.last_pos != game_info.off_side)
			{
				if ((ball_obj.type == LB_BOBBLE) ||
					(ball_obj.type == LB_PASS) ||
					((ball_obj.type == LB_LATERAL) && !(ball_obj.flags & BF_BOUNCE)))
				{
					// pass, bobble, or lateral that hasn't bounced - INTERCEPTION
					tdata_blocks[ppdata->team].interceptions++;
					snd_scall_bank(plyr_bnk_str,INTERCEPT_SND,VOLUME4,127,PRIORITY_LVL5);
					crowd_cheer_snds();
					#ifdef PLAY_DEBUG
					fprintf( stderr, "  interception recorded\n" );
					#endif
				}
				else if(( ball_obj.type == LB_FUMBLE ) || ( ball_obj.type == LB_LATERAL ))
				{
					// fumble or lateral that bounced - FUMBLE
					tdata_blocks[!ppdata->team].lost_fumbles++;
					#ifdef PLAY_DEBUG
					fprintf( stderr, "  lost_fumble recorded\n" );
					#endif
				}

				game_info.last_pos = game_info.off_side;

				#ifndef NO_FIRE
				#ifdef FIRE_DEBUG
				if ((game_info.team_fire[!game_info.off_side]) &&
					(ball_obj.type != LB_KICKOFF))
					fprintf( stderr, "Team %d loses fire (Turnover)\n", !game_info.off_side );
				#endif

				// kill fire for team that blew it
				if (ball_obj.type != LB_KICKOFF)
					reset_team_fire( !game_info.off_side );

				// if they're no already on fire, reset fire for team that recovered it
				if (!game_info.team_fire[game_info.off_side])
					reset_team_fire( game_info.off_side );
				#endif
			}

		}

		// have I run across the first down marker?
		if(( over_first_down(pobj) == 1 ) &&
			( game_info.game_mode == GM_IN_PLAY ) &&
			!(game_info.game_flags & GF_FD_LINE_CROSSED) &&
			!( pobj->adata[0].animode & MODE_REACTING ))
		{
			// The first time we go across the 1st down marker, play a sound
//			qcreate ("first", 0, first_snd_proc, ppdata->plyrnum, 0,0,0);
			// Set a flag telling us that the snd has already been played for this play
			game_info.game_flags |= GF_FD_LINE_CROSSED;
			fd_flash_time = 64;
		}

		// have I drifted over/landed in my own end zone after having jumped
		// to intercept?
		if(( game_info.game_flags & GF_AIR_CATCH ) &&
			!(game_info.game_flags & GF_REC_IN_EZ ) &&
			in_bounds( pobj ) &&
			(FIELDX( pobj->x ) < 2 ))
		{
#ifdef PLAY_DEBUG
			fprintf( stderr, "  GF_REC_IN_EZ set as intercepting player drifts into EZ\n" );
#endif
			game_info.game_flags |= GF_REC_IN_EZ;
			game_info.game_flags &= ~GF_LEFT_EZ;
		}

		// have my feet touched the ground in bounds?
		if(( game_info.game_flags & GF_AIR_CATCH ) &&
			( in_bounds( pobj )) &&	( pobj->y == 0.0f ))
		{
#ifdef PLAY_DEBUG
			fprintf( stderr, "  GF_AIR_CATCH cleared\n" );
#endif
			game_info.game_flags &= ~GF_AIR_CATCH;

			// if I've got a pending fire check, run it
			if (ppdata->odata.flags & PF_PEND_FCHECK)
			{
				ppdata->odata.flags &= ~PF_PEND_FCHECK;
				check_receiver_fire( ppdata );
			}
		}

		// have I scored a touchdown?
		if( td_check( 0 ))
		{
			game_info.game_mode = GM_PLAY_OVER;
			game_info.play_end_cause = PEC_CAR_TOUCHDOWN;
#ifdef PLAY_DEBUG
			fprintf( stderr, "  player loop TOUCHDOWN\n" );
#endif
		}

		if ((game_info.game_mode == GM_PLAY_OVER) &&
			(game_info.play_end_cause == PEC_CAR_TOUCHDOWN) &&
			(pobj->y == 0.0f) &&
			(!(pobj->adata[0].animode & MODE_UNINT) || (pobj->adata[0].seq_index == (SEQ_SPIN))))
		{

			// unattach me if in puppet DRAGGING mode

// NEW
			if (ppdata->odata.flags & PF_DRAGGING_PLR)
			{
				ppdata->puppet_rel_time = pcount;
				ppdata->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);
				ppdata->puppet_link->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);
				ppdata->odata.flags &= ~PF_DRAGGING_PLR;
				change_anim((obj_3d *)ppdata->puppet_link ,h_bsider_anim);
				ppdata->puppet_link->puppet_link = NULL;				// guy being dragged
				ppdata->puppet_link = NULL;								// guy being dragged
			}
// NEW
			if (!(ppdata->odata.flags & PF_DRAGGING_PLR))
			{


#if 0
				change_anim(pobj, new_endzone_dances_tbl[0]);
#else
				if (randrng(100) < 40)
					change_anim(pobj, new_endzone_dances_tbl[randrng(sizeof(new_endzone_dances_tbl)/sizeof(char *))]);
//#ifndef SEATTLE
				else
					if ((pobj->adata[0].seq_index == (SEQ_SPIN)) && (randrng(100) < 40))
						change_anim(pobj, end_zone10_anim);
//#endif
				else
					change_anim(pobj, endzone_dances_tbl[randrng(sizeof(endzone_dances_tbl)/sizeof(char *))]);
#endif
				// put closet defender into a reaction seq...only if not on ground
				for (i=0; i < NUM_PLAYERS; i++)
				{
					// don't check myself
					if (ppdata->plyrnum == i)
						continue;

					// ignore if on same team
					if (ppdata->team == player_blocks[i].team)
						continue;

					// find first defender within X yards...
					if (distxz[ppdata->plyrnum][i] > 50.0f)
						continue;

					if (!(player_blocks[i].odata.adata[0].animode & MODE_UNINT))
					{
						change_anim( (obj_3d *)&player_blocks[i], endzone_reactions_tbl[randrng(sizeof(endzone_reactions_tbl)/sizeof(char *))]);
						disappointed_speech();
						break;
					}
				}
			}
		}

		// have I run out of bounds?
	 	else if(( !in_bounds( pobj )) && (game_info.game_mode != GM_PLAY_OVER))
	 	{
#ifdef PLAY_DEBUG
			fprintf( stderr, "  off %2x(%d) goes out of bounds at %5.2f, field %d\n",
					JERSEYNUM(ppdata->plyrnum), ppdata->team, pobj->x,
					FIELDX( pobj->x ));
#endif

			if (game_info.game_mode != GM_PLAY_OVER)
				snd_scall_bank(plyr_bnk_str,WHISTLE1_SND+randrng(2),VOLUME4,127,PRIORITY_LVL5);

			// mark the play dead
			game_info.game_mode = GM_PLAY_OVER;

			// if GF_AIR_CATCH is set, this is either a FUMBLE_OOB or INCOMPLETE
			// also FUMBLE or INCOMPLETE if catch time < 145
			if(( game_info.game_flags & GF_AIR_CATCH ) ||
				( pcount - ball_obj.catch_pcount < CONTROL_TIME ))
			{
				game_info.play_end_cause = PEC_BALL_OOB;
#ifdef PLAY_DEBUG
				fprintf( stderr, "  pec set to BALL_OOB because of AIR_CATCH\n" );
#endif
			}
			else if( oob_zone( pobj ) == 1 )
			{
				game_info.play_end_cause = PEC_CAR_TACKLED_OWNEZ;
#ifdef PLAY_DEBUG
				fprintf( stderr, "  carrier ran out of bounds in own EZ\n" );
#endif
			}
			else
				game_info.play_end_cause = PEC_CAR_OOB;
			game_info.play_end_pos = pobj->x;
			game_info.play_end_z = pobj->z;
		}

		// am I moving outside my own end zone under my own power?
		if( in_bounds( pobj ) &&
			( FIELDX(pobj->x) > 2 ) &&
			( game_info.game_flags & GF_REC_IN_EZ ) &&
			!( game_info.game_flags & GF_LEFT_EZ ))
		{
#ifdef PLAY_DEBUG
			fprintf( stderr, "  GF_LEFT_EZ_SET by player %2x\n", JERSEYNUM(ppdata->plyrnum));
#endif
			game_info.game_flags |= GF_LEFT_EZ;
		}

		// allow player to hurdle outta head plow unless dragging someone.
		if (!(pobj->flags & PF_DRAGGING_PLR))
		{
			if ((pobj->adata[1].seq_index == (SEQ_M_PLOWHD)) &&
				(pobj->adata[1].animode & MODE_UNINT) &&
				(ppdata->but_new & P_A) &&
				(ppdata->but_cur & P_C))
			{
				game_info.hurdle_cnt[ppdata->team] += 1;
				change_anim(pobj, turbo_hurdle_tbl[randrng(sizeof(turbo_hurdle_tbl)/sizeof(char *))]);
				hurdle_speech();
			}
		}

		// fire processing
		maybe_plyr_fire(ppdata);

	}

	if ((!(pobj->adata[0].animode & MODE_UNINT) && !(pobj->adata[1].animode & MODE_UNINT)) ||
	 	((pobj->adata[1].seq_index == (SEQ_M_WAVQB)) &&
			 (ppdata->plyrnum == game_info.ball_carrier) &&
			 (pobj->adata[1].animode & MODE_TWOPART)) ||
		 ((pobj->adata[1].seq_index == (SEQ_M_WAVQBF)) &&
		 	(ppdata->plyrnum == game_info.ball_carrier) &&
			(pobj->adata[1].animode & MODE_TWOPART)))
//		 (pobj->adata[0].seq_index == (SEQ_BREATH)))
	{
		// we're not in MODE_UNINT
		standard_run(pobj);

		// stuff ANY weapon can do:

		// double-turbo = spin
		if ((ppdata->non_turbo_time == -1) &&
			(!(pobj->flags & PF_DRAGGING_PLR)) &&
			(game_info.game_mode != GM_PRE_KICKOFF) &&
			(game_info.game_mode != GM_PRE_SNAP))
		{
			game_info.spin_cnt[ppdata->team] += 1;
			change_anim(pobj, spin_anim);
#if 1	//(DHS)
			if (player_blocks[ppdata->plyrnum].plyrnum == game_info.ball_carrier)
#endif
			spin_move_speech();
		}

		// THREE main cases for weapon control logic
		// 1 - Ball carrier
		// 2 - Blocker
		// 3 - Other
		if (game_info.ball_carrier == ppdata->plyrnum)
		{	// CASE 1 - BALL CARRIER
			// check for stiff-arm while dragging
			if ((pobj->flags & PF_DRAGGING_PLR) &&
				(ppdata->but_new & P_B) &&
				(ppdata->but_cur & P_C) &&
				!(pobj->adata[1].animode & MODE_STIFF_ARM))
				pobj->adata[1].animode |= MODE_STIFF_ARM;

			if (!(pobj->flags & PF_DRAGGING_PLR))
			{
				// hurdle?
				if ((ppdata->but_new & P_A) &&
//					(game_info.game_mode != GM_PLAY_OVER))
					(1))
				{
					game_info.hurdle_cnt[ppdata->team] += 1;
					if (ppdata->but_cur & P_C)
						change_anim(pobj, turbo_hurdle_tbl[randrng(sizeof(turbo_hurdle_tbl)/sizeof(char *))]);
					else
						change_anim(pobj, hurdle_tbl[randrng(sizeof(hurdle_tbl)/sizeof(char *))]);
					hurdle_speech();
				}

				// stiff arm?
				if ((ppdata->but_new & P_B) &&
					(ppdata->but_cur & P_C) &&
					(fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.00f) &&
					!(pobj->adata[1].animode & MODE_STIFF_ARM) &&
					(pobj->adata[0].seq_index != (SEQ_SPIN)) &&
					(pobj->adata[0].seq_index != (SEQ_HURDLE)) &&
					(pobj->adata[0].seq_index != (SEQ_R_HURDSP)) &&
					(pobj->adata[0].seq_index != (SEQ_HURDLE1)) &&
					(pobj->adata[0].seq_index != (SEQ_HURDLE2)))
					pobj->adata[1].animode |= MODE_STIFF_ARM;

				// lateral?
				if (ppdata->plyrnum == game_info.ball_carrier)
				{
					maybe_lateral(pobj);
					almost_touchdown_sp(pobj);
				}
			}
		}
		else if (ppdata->job == JOB_BLOCK)
		{	// CASE 2 - BLOCKER
			// change player on B+!C.
//			if ((ppdata->but_new & P_B) && !(ppdata->but_cur & P_C))
//			if (ppdata->but_new & P_B)
			if ((ppdata->but_new & P_B) &&
				(game_info.play_type != PT_KICKOFF))
			{
				// don't process further if any change took place
				if (wpn_change_player(ppdata))
					return 0;
			}
	
			if (ppdata->but_new & P_A)
			{
				if ((game_info.ball_carrier == -1) && (game_info.game_mode != GM_PLAY_OVER))
				{
					if (ppdata->but_cur & P_C)
					{
					 	if ((face_dir = dir49_table[ppdata->stick_cur]) < 0) face_dir = 0;
				 		face_dir = 1023 & (face_dir + (int)RD2GR(cambot.theta));
					 	pobj->fwd = face_dir;
					 	pobj->tgt_fwd = face_dir;

						if ((ball_obj.type == LB_BOBBLE) || (ball_obj.type == LB_FUMBLE))
							change_anim(pobj, c_dive_for_ball_anim);
						else
							change_anim(pobj, t_crzdiv_anim);
					}
					else
					{
						if ((ball_obj.type == LB_BOBBLE) || (ball_obj.type == LB_FUMBLE))
							change_anim(pobj, c_dive_for_ball_anim);
						else if (ball_obj.total_time - ball_obj.flight_time > 10)
						{	// just A button...no TURBO
							if (ISDRONE(ppdata))
								change_anim(pobj, c_jmpint3_anim );
							else
								change_anim(pobj, intercept_anim_tbl[randrng(sizeof(intercept_anim_tbl)/sizeof(char *))]);
						}
					}
				}
				else
				{
				 	if ((face_dir = dir49_table[ppdata->stick_cur]) < 0) face_dir = 0;
				 	face_dir = 1023 & (face_dir + (int)RD2GR(cambot.theta));
				 	pobj->fwd = face_dir;
				 	pobj->tgt_fwd = face_dir;
					change_anim( pobj, t_crzdiv_anim );
				}
			}

			// check for push
//			if ((ppdata->but_new & P_B) &&
//				(ppdata->but_cur & P_C) &&
//				(ISHUMAN(ppdata)))
			if ((ppdata->but_new & P_B) &&
				(ppdata->but_cur & P_C))
			{
				if (!(pobj->adata[1].animode & MODE_RUNNING_PUSH) &&
					(fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.10f))
				{
					pobj->adata[1].animode |= MODE_RUNNING_PUSH;
				}
				else if (!(pobj->adata[0].animode & MODE_UNINT))
					change_anim(pobj, push_anim_tbl[randrng(sizeof(push_anim_tbl)/sizeof(char *))]);
			}
		}
		else
		{	// CASE 3 - OTHER (receiver, mostly)
			// change player on B+!C.
			// only rarely will a non-blocking weapon be allowed to change players
			if ((ppdata->but_new & P_B) && !(ppdata->but_cur & P_C))
			{
				// don't process further if any change took place
				if (wpn_change_player(ppdata))
					return 0;
			}

			if (ppdata->but_new & P_A)
			{
				if (game_info.ball_carrier == -1)
				{	// loose ball
					switch (ball_obj.type)
					{
						case LB_BOBBLE:
						case LB_FUMBLE:
							if ((odistxz(pobj, (obj_3d *)&ball_obj)/YD2UN <= 8.0f) &&
								(game_info.game_mode != GM_PLAY_OVER))
							{
								if (ball_obj.odata.y < 20.0f)		// was 25.0f
									change_anim(pobj, c_dive_for_ball_anim);
								else
									change_anim(pobj, intercept_anim_tbl[randrng(sizeof(intercept_anim_tbl)/sizeof(char *))]);
							}
							break;
						case LB_PASS:
							if ((game_info.ball_carrier == -1) &&
								(ball_obj.int_receiver == ppdata->plyrnum))
								change_anim( pobj, c_jmpint_anim );
							break;
						default:
							break;
					}
				}
				else
				{	// carried ball
					change_anim(pobj, t_crzdiv_anim);
				}
			}

			// check for push
			if ((ppdata->but_new & P_B) &&
				(ppdata->but_cur & P_C))
//				(ISHUMAN(ppdata)))
			{
				if (!(pobj->adata[1].animode & MODE_RUNNING_PUSH) &&
					(fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.10f))
				{
					pobj->adata[1].animode |= MODE_RUNNING_PUSH;
				}
				else if (!(pobj->adata[0].animode & MODE_UNINT))
					change_anim(pobj, push_anim_tbl[randrng(sizeof(push_anim_tbl)/sizeof(char *))]);
			}
		}


		// DRONE ANIMATION ?
		if ((ppdata->but_new & P_D) &&
			(ISDRONE(ppdata)))
		{
			did_catch |= 2;
			if( (int)(ppdata->drone_script) & 0x80000000 )
			{
				change_anim(pobj, ppdata->drone_script);
				did_catch |= 4;
			}
			else
			{
				char *script;

				begin_twopart_mode(pobj);
				script = (char *)((int)ppdata->drone_script | 0x80000000);
				change_anim2(pobj, script);
				did_catch |= 8;
			}
		}

	}	// if !UNINT

	// allow dude in dive move to change player
	if ((ppdata->but_new & P_B) &&
		(ppdata->plyrnum != game_info.ball_carrier) &&
		!(pobj->adata[1].animode & MODE_RUNNING_PUSH))	// no changing/pushing from running push
	{
//		if ((!(ppdata->but_cur & P_C)) && (ISHUMAN(ppdata)))
			wpn_change_player(ppdata);
	}



	// TEST TEST
	if ((ppdata->plyrnum == game_info.ball_carrier) &&
		(!(pobj->adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER))) &&
		(!(game_info.game_flags & GF_PLAY_ENDING)) &&
		(pobj->adata[0].seq_index != (SEQ_M_LATERR)) &&
		(pobj->adata[0].seq_index != (SEQ_M_LATERL)) &&
//		(pobj->adata[0].seq_index != (SEQ_QB_SHTLR)) &&
//		(pobj->adata[0].seq_index != (SEQ_QB_SHTLL)) &&
		(game_info.game_mode != GM_PLAY_OVER))
	{
		maybe_lateral(pobj);
	}


	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// defensive back/linebacker before the snap

extern play_t play_block_punt;
extern play_t play_block_fg;
extern play_t play_punt_return;

static int _mode_dbps( fbplyr_data *ppdata )
{
	obj_3d			*pobj = (obj_3d *)ppdata;
	int				new, count;

	// change guys? - although, don't allow changing guys in first 15 ticks out of play select
	// ALSO, no changing players before the kick on an onside kick
	if(( ppdata->but_new & P_B ) &&
		!(game_info.game_flags & GF_ONSIDE_KICK) &&
		(game_info.auto_hike_cntr  < (MAX_HIKE_TIME - (15))) &&
		ISHUMAN(ppdata))
	{
		// cycle until we find a guy in mode DBPS who isn't human-controlled
		new = ppdata->plyrnum;
		count = 10;
		do {
			new += 1;
			count -= 1;
			if( new % 7 == 0 )
				new -= 7;
		} while(( count > 0) &&
			(( player_blocks[new].station != -1 ) ||
			(( player_blocks[new].odata.plyrmode != MODE_DBPS ) &&
			( player_blocks[new].odata.plyrmode != MODE_DB ))));

		game_info.plyr_control[ppdata->station] = new;
		update_stations();

		// fake the new guy's B button.
		player_blocks[new].but_cur |= P_B;

		if (randrng(10) < 35)
			change_anim((obj_3d *)&player_blocks[new], m_hopclp_anim);
		else
			change_anim((obj_3d *)&player_blocks[new], m_hophop_anim);
	}

//	if (( game_info.team_play[game_info.off_side^1] != &play_block_punt ) &&
//		( game_info.team_play[game_info.off_side^1] != &play_block_fg ) &&
//		( game_info.team_play[game_info.off_side^1] != &play_punt_return ))
	{
		if (audible_check(ppdata, DEFENSE)==0)
			return(0);
	}

	// has the ball been snapped?
	if( game_info.game_mode == GM_IN_PLAY )
	{
		pobj->plyrmode = MODE_DB;
	}

	// no crossing the line of scrimmage
	if (FIELDX(pobj->x) < 1 + game_info.los)
		pobj->x = WORLDX( 1 + game_info.los );

	// if stick is neutral, face los
	if( ppdata->stick_cur == 24 )
		pobj->tgt_fwd = (game_info.off_side) ? 256 : 768;

	pobj->turn_rate = 64;
	pobj->turn_rate2 = 48;

	// turn toward target heading
	turn_toward_tgt( pobj );

	if (!(pobj->adata[0].animode & MODE_UNINT))
	{
		// at LOS ? if so...stop dude from being in run anim...against LOS
//		if (((pobj->x == WORLDX(1 + game_info.los)) || (pobj->x == WORLDX(2 + game_info.los))) &&
//			(ppdata->stick_cur >= 28))
//		{
//			// stop dude from being in running anim...against the LOS
//			ppdata->stick_cur = 24;
//		}
		standard_run(pobj);
	}

	
	// this is dude on fire effect at pre-snap (only for the non-linemen)
	if ((game_info.team_fire[ppdata->team]) &&
		(db_pflash < 4))
	{
		qcreate ("fburst", SPFX_PID, player_burst_into_flames, ppdata->plyrnum, 2, 4, 0);
		db_pflash++;
	}



	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// defensive back/linebacker after the snap
static int _mode_db( fbplyr_data *ppdata )
{
	obj_3d *pobj = (obj_3d *)ppdata;
	int		face_dir;
	int		prev;

	pobj->turn_rate = 64;
	pobj->turn_rate2 = 48;

	// turn toward target heading
	turn_toward_tgt(pobj);

	if (!(pobj->adata[0].animode & MODE_UNINT))
	{
		// we're not in MODE_UNINT
		standard_run(pobj);

		if ((ppdata->but_new & P_A)	&&
			!(pobj->adata[1].animode & MODE_RUNNING_PUSH))	// no changing/pushing from running push
		{
			if ((game_info.ball_carrier == -1) && (game_info.game_mode != GM_PLAY_OVER))
			{
				if (ppdata->but_cur & P_C)
				{
					if ((ball_obj.type == LB_BOBBLE) || (ball_obj.type == LB_FUMBLE))
						change_anim(pobj, c_dive_for_ball_anim);
					else
					{
						if (ppdata->stick_cur == 24)
							face_dir = 1023 & (pobj->fwd + (int)RD2GR(cambot.theta));
						else
							face_dir = 1023 & (dir49_table[ppdata->stick_cur] + (int)RD2GR(cambot.theta));
						pobj->fwd = face_dir;
						pobj->tgt_fwd = face_dir;
						change_anim(pobj, t_crzdiv_anim);
					}
				}
				else
				{
					if ((ball_obj.type == LB_BOBBLE) || (ball_obj.type == LB_FUMBLE))
						change_anim(pobj, c_dive_for_ball_anim);
					else if (ball_obj.total_time - ball_obj.flight_time > 10)
					{
						if (ISDRONE(ppdata))
							change_anim(pobj, c_jmpint3_anim );
						else
							change_anim(pobj, intercept_anim_tbl[randrng(sizeof(intercept_anim_tbl)/sizeof(char *))]);
					}

				}
			}
			else
			{
				// if ball carrier is hittable and we're close to em...fuck'em up!!!
				if ((player_blocks[game_info.ball_carrier].odata.flags & PF_LATEHITABLE) &&
					(distxz[ppdata->plyrnum][game_info.ball_carrier]/YD2UN < 5.0f))
				{
					if (pup_violence)
						change_anim(pobj, violent_db_late_hits_tbl[randrng(sizeof(violent_db_late_hits_tbl)/sizeof(char *))]);
					else
						change_anim(pobj, db_late_hits_tbl[randrng(sizeof(db_late_hits_tbl)/sizeof(char *))]);
				}
				else
				{
					if (ppdata->stick_cur == 24)
						face_dir = 1023 & pobj->fwd;
//						face_dir = 1023 & (pobj->fwd + (int)RD2GR(cambot.theta));
					else
					 	face_dir = 1023 & (dir49_table[ppdata->stick_cur] + (int)RD2GR(cambot.theta));
				 	pobj->fwd = face_dir;
				 	pobj->tgt_fwd = face_dir;
					change_anim( pobj, t_crzdiv_anim );
				}
			}
		}
	}

	// change player or push!!
	if ((ppdata->but_new & P_B) &&
		!(pobj->adata[1].animode & MODE_RUNNING_PUSH))	// no changing/pushing from running push
	{
		if ((!(ppdata->but_cur & P_C)) && (ISHUMAN(ppdata)))
		{
			#ifdef CHANGE_PLAYER_DEBUG
			prev = ppdata->station;
			if (db_change_player(ppdata,1) != -1)
				fprintf( stderr, "player %d changes player (B only)\n", prev );
			#else
			db_change_player(ppdata,1);
			#endif
		}
		else if (ppdata->but_cur & P_C)
		{
			if (game_info.ball_carrier != -1)
			{	// carrier
				if ((player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_QB) &&
					( FIELDX(pobj->x) >= game_info.los-1 ))
				{	// carrier is QB and we're not rushing him
					if ((fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.10f) &&
						!(pobj->adata[0].animode & MODE_UNINT))
						pobj->adata[1].animode |= MODE_RUNNING_PUSH;
					else if (!(pobj->adata[0].animode & MODE_UNINT))
						change_anim(pobj, push_anim_tbl[randrng(sizeof(push_anim_tbl)/sizeof(char *))]);
				}
				else
				{	// trying to kill carrier
					if (db_change_player(ppdata,0) == -1)			// already closet dude ?
					{												// yep
						if (!(pobj->adata[1].animode & MODE_RUNNING_PUSH) &&
							(fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.10f))
						{
							pobj->adata[1].animode |= MODE_RUNNING_PUSH;
						}
						else if (!(pobj->adata[0].animode & MODE_UNINT))
							change_anim(pobj, push_anim_tbl[randrng(sizeof(push_anim_tbl)/sizeof(char *))]);
					}
					else
					{
						#ifdef CHANGE_PLAYER_DEBUG
						prev = ppdata->station;
						if (db_change_player(ppdata,1) != -1)
							fprintf( stderr, "player %d changes player (A|B, going for carrier, not closest)\n", prev );
						#else
						db_change_player(ppdata,1);
						#endif
					}
				}
			}
			else
			{	// loose ball

				// If the ball has been thrown to a target at or behind the line of scrimmage, observe
				// normal ball-in-QBs-hand change rules
				if ((FIELDX(ball_obj.tx) <= game_info.los) &&
					(ball_obj.type == LB_PASS) &&
					(FIELDX(pobj->x) >= game_info.los-1))
				{
					if ((fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.10f) &&
						!(pobj->adata[0].animode & MODE_UNINT))
						pobj->adata[1].animode |= MODE_RUNNING_PUSH;
					else if (!(pobj->adata[0].animode & MODE_UNINT))
						change_anim(pobj, push_anim_tbl[randrng(sizeof(push_anim_tbl)/sizeof(char *))]);
				}
				else if ((db_change_player(ppdata,0) == -1) ||		// already closet dude ?
					(game_info.game_flags & GF_HIKING_BALL))		// no changing during snap
				{
					if (!(pobj->adata[1].animode & MODE_RUNNING_PUSH) &&
						(fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.10f))
					{
						pobj->adata[1].animode |= MODE_RUNNING_PUSH;
					}
					else if (!(pobj->adata[0].animode & MODE_UNINT))
						change_anim(pobj, push_anim_tbl[randrng(sizeof(push_anim_tbl)/sizeof(char *))]);
				}
				else
				{
					// !!!FIX need sound here
					#ifdef CHANGE_PLAYER_DEBUG
					prev = ppdata->station;
					if (db_change_player(ppdata,1) != -1)
						fprintf( stderr, "player %d changes player (A|B, loose ball)\n", prev );
					#else
					db_change_player(ppdata,1);
					#endif
				}
			}
		}
	}

	standard_catch(pobj);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// any kicker, any mode
static int _mode_kicker( fbplyr_data *ppdata )
{
	obj_3d	*pobj = (obj_3d *)ppdata;
	float	t;


	if ((game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL) &&
		!(game_info.game_flags & GF_BALL_KICKED))
	{
		if (game_info.game_mode == GM_PRE_SNAP)
			return 0;

		// if ball is loose or setting player has been hit, turn into a weapon
		if ((game_info.ball_carrier == -1 ) && ( ball_obj.type != LB_LATERAL))
		{
			pobj->plyrmode = MODE_WPN;
			return 0;
		}

		if (odistxz(pobj, (obj_3d *)&ball_obj ) < 8.0f)
		{
			// is the ball kickable?
			if ((player_blocks[game_info.ball_carrier].odata.adata[0].seq_index != (SEQ_M_KHHOLD)) &&
				(player_blocks[game_info.ball_carrier].odata.adata[0].seq_index != (SEQ_M_KHHUT)))
			{
				pobj->plyrmode = MODE_WPN;
				return 0;
			}

			// kick when we reach the ball
			// Field goal seq.
			change_anim(pobj, kickball_anim);



			// Field goal seq. has a several tick delay before it kicks
			change_anim((obj_3d *)&player_blocks[game_info.off_side ? 10 : 3], qbhut_unhold_ball_anim);
			snd_scall_bank(ancr_swaped_bnk_str,KICK_IS_UP_SP,ANCR_VOL1,127,LVL2);
			kick_ball(pobj);
			pobj->plyrmode = MODE_WPN;
			return 0;
		}

	}

	if (game_info.play_type == PT_KICKOFF)
	{
		switch( game_info.game_mode )
		{
			case GM_PRE_SNAP:
//				if( ppdata->but_new & P_A )
				// This is for first kickoff of the game - "can" its timing
				if(game_info.auto_hike_cntr-- < 1)
				{
					game_info.game_mode = GM_PRE_KICKOFF;
					return 0;
				}
				break;
			case GM_PRE_KICKOFF:
				// kick when we reach the ball
				if( odistxz( pobj, (obj_3d *)&ball_obj ) < 8.0f )
				{
					change_anim(pobj, kickball_anim);

					game_info.ko_catch = -1;
					game_info.game_mode = GM_IN_PLAY;
					ball_obj.odata.vx = game_info.off_side ? -2.83f : 2.83f;
					if(game_info.game_flags & GF_ONSIDE_KICK)
					{
//						ball_obj.odata.vx = game_info.off_side ? -1.90f : 1.90f;
//						ball_obj.odata.vy = 2.5f + (((float)(randrng(10)))/10.0f);
//						ball_obj.odata.vz = randrng(2) ? -1.0f : 1.0f;
						ball_obj.odata.vx = (float)(25 + randrng(9)) / 10.0f;
						ball_obj.odata.vx *= game_info.off_side ? -1.0f : 1.0f;
						ball_obj.odata.vy = 1.2f + (((float)(randrng(5)))/10.0f);
						ball_obj.odata.vz = (float)(randrng(21) - 10) / 10.0f;
						snd_scall_bank(ancr_bnk_str,ONSIDE_KICK_SP,ANCR_VOL1,127,LVL3);
					}
					else
					{
						ball_obj.odata.vx = game_info.off_side ? -2.83f : 2.83f;
						ball_obj.odata.vy = 7.0f;
						ball_obj.odata.vz = 0.0f;
						snd_scall_bank(ancr_swaped_bnk_str,GAME_UNDER_WAY_SP,ANCR_VOL1,127,LVL2);
#ifdef DBG_KO
						fprintf(stderr,"tick_cnt: %d\r\n",cambot.tick_cnt);
#endif //DBG_KO
					}

					ball_obj.pitch_rate = -45;
					ball_obj.type = LB_KICKOFF;
					game_info.game_flags |=  GF_BALL_KICKED;
					ball_obj.flags &= ~(BF_VEL_PITCH|BF_FG_BOUNCE|BF_FG_HITPOST);
					ball_obj.who_threw = ppdata->plyrnum;
					ball_obj.flight_time = 0;

					// place target X
					t = -2.0f * ball_obj.odata.vy / GRAVITY;
					ball_obj.tx = ball_obj.odata.x + t * ball_obj.odata.vx;
					ball_obj.tz = ball_obj.odata.z + t * ball_obj.odata.vz;

					// become a wpn
					pobj->plyrmode = MODE_WPN;

				}
				break;
			default:
				break;
		}
	}

	pobj->turn_rate = 64;

	// turn toward target heading
	turn_toward_tgt(pobj);

	if (!(pobj->adata[0].animode & MODE_UNINT))
	{
		// we're not in MODE_UNINT
		standard_run(pobj);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//
// misc player functions
//
//////////////////////////////////////////////////////////////////////////////
static void maybe_lateral(obj_3d *pobj)
{
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;
	int		i,brng;
	float	tgt_pt[3];

 	if ((ppdata->but_new & P_B) &&
 		!(ppdata->but_cur & P_C) &&
 		(game_info.game_mode != GM_PLAY_OVER))
 	{
 		// no target
 		ppdata->throw_target = -1;

 		// find closet teammate...
 		for (i=0; i < NUM_PLAYERS; i++)
 		{
 			// dont check myself
 			if (ppdata->plyrnum == i)
 				continue;

 			// ignore if on other team
 			if (ppdata->team != player_blocks[i].team)
 				continue;

 			// if he is farther up-field...ignore
 			if (FIELDX(pobj->x) < FIELDX(player_blocks[i].odata.x ))
 				continue;

 			// if player is unint...ignore
 			if (player_blocks[i].odata.adata[0].animode & MODE_UNINT)
 				continue;

 			if (distxz[ppdata->plyrnum][i]/YD2UN <= 10.0f)
 			{
 				ppdata->throw_target = i;
 				break;
 			}
 		}

 		if (ppdata->throw_target != -1)
 		{
 			// lateral pass
 			// where the player will be in 70 ticks (1.3 seconds)
 			tgt_pt[0] = player_blocks[ppdata->throw_target].odata.x + (player_blocks[ppdata->throw_target].odata.vx * 70.0f);
 			tgt_pt[1] = 0.0f;
 			tgt_pt[2] = player_blocks[ppdata->throw_target].odata.z + (player_blocks[ppdata->throw_target].odata.vz * 70.0f);

 			brng = (1024 + pobj->fwd - ptangle(&(pobj->x),tgt_pt)) % 1024;

//			if (randrng(100) < 80)
//				change_anim(pobj, (pobj->flags & PF_FLIP) ? new_lateral_r_anim : new_lateral_l_anim );
			if (game_info.off_side)
			{
				if (pobj->z > tgt_pt[2])
					change_anim(pobj, m_laterall_anim);
				else
					change_anim(pobj, m_lateralr_anim);
			}
			else
			{
				if (pobj->z <= tgt_pt[2])
					change_anim(pobj, m_laterall_anim);
				else
					change_anim(pobj, m_lateralr_anim);
			}
 			lateral_speech();
 		}
 	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define CR_NONE		0
#define CR_CATCH	1
#define CR_MISS		2
#define CR_BOBBLE	3
#define CR_BATDOWN	4
#define CR_DEFLECT	5

static void standard_catch( obj_3d *pobj )
{
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;
	int			offense, close=FALSE, catch_result = CR_NONE, i;
	float		wpos[3], *tm, rx, ry, rz;

	// set offense TRUE if we're on offense, FALSE otherwise
	offense = (game_info.off_side == ppdata->team);

	// ball loose and alive?
	if(( game_info.ball_carrier != -1 ) || ( game_info.game_mode != GM_IN_PLAY ))
		return;

	// no catching in PUPPET, PUPPETEER, or NOCATCH modes
	if( pobj->adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER))
		return;

	// no catching in NOCATCH mode (hit reactions) unless it's for variety when losing
	if ((pobj->adata[0].animode & (MODE_NOCATCH)) &&
		(randrng(100) >= nocatch_ht[PLYRH(ppdata)]))
		return;

	// don't allow man-in-motion to catch ball...unless FAKE FG play
	if ((game_info.game_flags & GF_HIKING_BALL) &&
		!(game_info.team_play[game_info.off_side]->flags & PLF_FAKE_FG))
		return;

	// are we in bounds?
	if( !in_bounds( pobj ))
		return;

	// is the ball in catching distance?
	//  - within 12 units of pelvis or
	//  - within 5 units of either wrist
	wpos[0] = pobj->x;
	wpos[1] = pobj->y + pobj->ay;
	wpos[2] = pobj->z;

	if( dist3d( &(ball_obj.odata.x), wpos) < 14.0f )
//	if( dist3d( &(ball_obj.odata.x), wpos) < 12.0f )
		close = TRUE;

	tm = cambot.xform;
	rx = ppdata->jpos[JOINT_LWRIST][0];
	ry = ppdata->jpos[JOINT_LWRIST][1];
	rz = ppdata->jpos[JOINT_LWRIST][2];
	wpos[0] = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	wpos[1] = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	wpos[2] = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	if( dist3d( &(ball_obj.odata.x), wpos) < 5.0f )
		close = TRUE;

	rx = ppdata->jpos[JOINT_RWRIST][0];
	ry = ppdata->jpos[JOINT_RWRIST][1];
	rz = ppdata->jpos[JOINT_RWRIST][2];
	wpos[0] = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	wpos[1] = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	wpos[2] = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	if( dist3d( &(ball_obj.odata.x), wpos) < 5.0f )
		close = TRUE;

	if( !close )
		return;

	// if it's a field goal and offense is on fire, don't touch
	if ((game_info.team_fire[game_info.off_side]) &&
		(ball_obj.flight_time == LB_FIELD_GOAL))
		return;

	// if it's a pass or kickoff and we threw it, don't catch it within
	// ten ticks
	// if we threw it, don't catch it within 10 ticks
	if(( ball_obj.who_threw == ppdata->plyrnum ) &&
		( ball_obj.flight_time <= 10 ))
		return;

	// if I'm on offense and it was thrown 15 or fewer ticks ago and I'm
	// not the intended receiver, don't catch it.
	if( offense && (ball_obj.flight_time <= 15 ) &&
		( ppdata->plyrnum != ball_obj.int_receiver ))
		return;

	// if our no_catch_time is greater than zero, don't catch
	if( ppdata->no_catch_time > 0 )
		return;

	// if it's a bobble or fumble, don't catch it within 30 ticks
	if(( ball_obj.type == LB_BOBBLE ) || ( ball_obj.type == LB_FUMBLE ))
		if( ball_obj.flight_time <= 30 )
			return;

	// do tons of other condition type stuff here
	switch( ball_obj.type )
	{
		case LB_PASS:
		case LB_BOBBLE:
			// If this is a drone, or human up by more than 3, he can bobble a wide open catch,
			// unless he's behind the line of scrimmage.  Humans won't bobble behind the los.
			if ((ISDRONETEAM(ppdata)) ||
				((compute_score(game_info.off_side) - compute_score(!game_info.off_side) > 3) &&
				(FIELDX(pobj->x) > game_info.los)))
			{
				if( randrng(100) < of_bobble_ht[PLYRH(ppdata)] )
					catch_result = CR_BOBBLE;
				else
					catch_result = CR_CATCH;

				if( drones_lose && !offense && drones_winning )
					catch_result = CR_MISS;
			}
			else
				catch_result = CR_CATCH;


			// if MODE_REACTING, bobble or miss
			if( pobj->adata[0].animode & MODE_REACTING )
			{
				// reacting defense:
				// p(deflect) = 0.1
				// p(miss) = 0.9
				// reacting offense:
				// p(catch) = 0.4
				// p(bobble) = 0.12
				// p(deflect) = 0.24
				// p(miss) = 0.24
				if( !offense )
				{
					if( randrng(10))
						catch_result = CR_MISS;
					else
						catch_result = CR_DEFLECT;
				}
				else if( randrng(100) < 40 )
					catch_result = CR_CATCH;
				else if( randrng(100) < 20 )
					catch_result = CR_BOBBLE;
				else if (randrng(2))
					catch_result = CR_DEFLECT;
				else
					catch_result = CR_MISS;
			}

			// within 30 ticks of throw, turn defender
			// bobbles and catches into deflections (0.9) or misses (0.1)
			if(( ball_obj.flight_time <= 30 ) && !offense )
				if((catch_result == CR_BOBBLE) || (catch_result == CR_CATCH))
					if (randrng(10))
						catch_result = CR_DEFLECT;
					else
						catch_result = CR_MISS;

			// if we're on defense and in a situation where we'd rather down
			// the ball than intercept it, do that.
			if ((!offense) &&
				(!intercept_ok( pobj )) &&
				(catch_result == CR_CATCH))
				catch_result = CR_BATDOWN;

			// if we're on defense and we're airborne, maybe bat down instead
			if ((!offense) &&
				(catch_result == CR_CATCH) &&
				(ppdata->odata.y >= 0.0f) &&
				(randrng(100) < db_air_batdown_ht[PLYRH(ppdata)]))
				catch_result = CR_BATDOWN;

			// if we're on defense and we're standing and it's not a bobble,
			// maybe bobble it
			if ((!offense) &&
				(catch_result == CR_CATCH) &&
				(ppdata->odata.y == 0.0f) &&
				!(ball_obj.flags & BF_BOBBLE) &&
				(randrng(100) < db_stand_bobble_ht[PLYRH(ppdata)]))
				catch_result = CR_BOBBLE;

			// if we're on defense, defense is on fire, and this is a pass, not
			// a bobble, turn half of batdowns & bobbles into catches
			if ((!offense) && (ball_obj.type == LB_PASS) &&
				(game_info.team_fire[!game_info.off_side]) &&
				((catch_result == CR_BOBBLE) || (catch_result == CR_BATDOWN)) &&
				(randrng(2)))
				catch_result = CR_CATCH;

			// if we're on defense and it's a catch and offense has powerup no
			// interceptions, bat it down.
			if ((!offense) &&
				(pup_nointercept & TEAM_MASK(game_info.off_side)) &&
				(catch_result == CR_CATCH))
				catch_result = CR_BATDOWN;

			// fire receiver never bobbles
			if ((offense) &&
				(game_info.team_fire[ppdata->team]) &&
				(tdata_blocks[ppdata->team].last_receiver == ppdata->plyrnum))
				catch_result = CR_CATCH;

			// if offense is on fire, turn deflections into misses
			if ((game_info.team_fire[game_info.off_side]) &&
				(catch_result == CR_DEFLECT))
				catch_result = CR_MISS;

			break;

		case LB_LATERAL:
			if ((!offense) &&
				!(pup_nointercept & TEAM_MASK(game_info.off_side)) &&
				(ball_obj.odata.x > game_info.los) &&
				!(ball_obj.flags & BF_BOUNCE) &&
				(ball_obj.total_time - ball_obj.flight_time > 0))
				catch_result = CR_MISS;
			else
				catch_result = CR_CATCH;

//			catch_result = CR_CATCH;
			break;

		case LB_FUMBLE:
			catch_result = CR_CATCH;
			break;

		case LB_PUNT:
//			reset_onfire_values();
			if (ppdata->team == game_info.off_side)
			{
				// kicking team got ball...
				game_info.play_end_pos = ball_obj.odata.x;
				game_info.play_end_cause = PEC_DEAD_BALL_IB;
				game_info.game_mode = GM_PLAY_OVER;
				game_info.game_flags |= (GF_PLAY_ENDING|GF_POSS_CHANGE);
				catch_result = CR_MISS;
#ifdef PLAY_DEBUG
				fprintf( stderr, "  off %2x(%d) downs punt at %5.2f, field %d\n",
					JERSEYNUM(ppdata->plyrnum), ppdata->team, pobj->x,
					FIELDX( pobj->x ));
#endif
			}
			else
			{
				// non-kicking team got ball....
				catch_result = CR_CATCH;
			}
			break;

		case LB_KICKOFF:
			if (game_info.game_flags & GF_ONSIDE_KICK)
			{
				if (randrng(45) < ball_obj.flight_time)
					catch_result = CR_CATCH;
				else
					catch_result = CR_DEFLECT;
			}
			else
				catch_result = CR_CATCH;
			break;

		case LB_FIELD_GOAL:
			// offensive players won't interfere
			if( offense )
				catch_result = CR_MISS;
			else if ((ball_obj.flags & BF_DEFLECTED) &&
							(randrng(100) < 34))
				catch_result = CR_CATCH;
			else
				catch_result = CR_DEFLECT;
			break;
	}

	// catch it
	switch (catch_result)
	{
		case CR_CATCH:
			if (!pup_rcvrname && (game_info.game_state != GS_ATTRACT_MODE))
				qcreate ("rcvrname", 0, show_receiver_name_proc, (int)ppdata, 0, 0, 0);

			if (ball_obj.type == LB_KICKOFF)
			{
				clock_active = 1;
				game_info.play_time = 0;
				if(!(game_info.game_flags & GF_ONSIDE_KICK ))
				{
					game_info.last_pos = ppdata->team;
					if(( game_info.last_pos != game_info.off_init ) ||
						( game_info.game_flags & GF_QUICK_KICK ))
						game_info.game_flags |= GF_POSS_CHANGE;
				}
			}

			// Give all humans at least half a turbo bar on any catch
			if (pdata_blocks[0].turbo < 50.0f)
				pdata_blocks[0].turbo = 50.0f;
			if (pdata_blocks[1].turbo < 50.0f)
				pdata_blocks[1].turbo = 50.0f;
			if (pdata_blocks[2].turbo < 50.0f)
				pdata_blocks[2].turbo = 50.0f;
			if (pdata_blocks[3].turbo < 50.0f)
				pdata_blocks[3].turbo = 50.0f;

			// clear the HIKING_BALL flag, if set
			game_info.game_flags &= ~GF_HIKING_BALL;

			// set carrier and ball stuff
			game_info.ball_carrier = ppdata->plyrnum;
			ball_obj.catch_pcount = pcount;
			game_info.catch_x = pobj->x;

			// if catcher is a drone on human-controlled team:
			// offense: set control to guy who threw the ball
			// defense: set control to human farthest away
			if(( ISDRONE( ppdata )) && ISHUMANTEAM( ppdata ))
			{
				int		station;

				if (four_plr_ver)
				{
					if ((offense) &&
						(player_blocks[ball_obj.who_threw].station != -1) &&
						((ball_obj.type == LB_LATERAL) || (ball_obj.type == LB_PASS)))
						station = player_blocks[ball_obj.who_threw].station;
					else
						station = player_blocks[farthest_teammate( ppdata )].station;
	
					assert( ppdata->team == station/2 );
				}
				else
				{
					station = ppdata->team;
				}

				game_info.plyr_control[station] = ppdata->plyrnum;
				update_stations();

				// if receiver is not in the air, give control back to the human
				if (pobj->y == 0.0f)
				{
					pobj->adata[0].animode &= ~MODE_UNINT;
					pobj->adata[1].animode &= ~MODE_UNINT;
				}

				ppdata->but_cur |= P_A;
				ppdata->but_cur |= P_B;
			}

			// stop the accidental spin moves
			ppdata->non_turbo_time = 500;

			// side-specific stuff
			if( offense )
			{
#ifdef PLAY_DEBUG
				fprintf( stderr, "  off %2x(%d) catches at %5.2f, field %d\n",
						JERSEYNUM(ppdata->plyrnum), ppdata->team, pobj->x,
						FIELDX( pobj->x ));
				fprintf( stderr, "  ball was type %s with flags ", loose_ball_names[ball_obj.type] );
				print_ball_flags();
#endif
				// if we're behind the line of scrimmage, go to QB mode.
				if(( FIELDX(pobj->x) < game_info.los ) &&
					(ppdata->team == game_info.off_init) &&
					( game_info.play_type != PT_KICKOFF ) &&
					!( game_info.game_flags & GF_BALL_CROSS_LOS ))
					pobj->plyrmode = MODE_QB;
				else
					pobj->plyrmode = MODE_WPN;

				// Burst player into flames...on COULD be ON-FIRE catch.
				if ((tdata_blocks[ppdata->team].last_receiver == ppdata->plyrnum) &&
					(tdata_blocks[ppdata->team].consecutive_comps == OFFENSE_FIRECNT-1) &&
					(game_info.team_fire[ppdata->team] != 1) &&
					(FIELDX(pobj->x) >= game_info.los) &&
					(game_info.game_mode != GM_PLAY_OVER))
				{
					snd_scall_bank(plyr_bnk_str,FIRE_BURST_SND,VOLUME4,127,PRIORITY_LVL5);
					snd_scall_bank(plyr_bnk_str,FIRE_BURST_SND,VOLUME4,127,PRIORITY_LVL5);		// play twice for EFFECT
					qcreate ("fburst", SPFX_PID, player_burst_into_flames, ppdata->plyrnum, 6, 4, 0);
				}

				// If I'm human, I'm past the line of scrimmage, and there's another human
				// on my team and he's NOT on screen, force him to change player.
				if ((ISHUMAN(ppdata)) &&
					(FIELDX(pobj->x) >= game_info.los) &&
					four_plr_ver &&
					((TEAM_MASK(ppdata->team) & p_status) == TEAM_MASK(ppdata->team)) &&
					(ppdata->station >= 0) &&
					(game_info.plyr_control[ppdata->station^1] >= 0) &&
					!(player_blocks[game_info.plyr_control[ppdata->station^1]].odata.flags & PF_ONSCREEN))
					wpn_change_player( player_blocks + game_info.plyr_control[ppdata->station^1] );
			}
			else
			{
#ifdef PLAY_DEBUG
				fprintf( stderr, "  def %2x(%d) INTERCEPTS at %5.2f, field %d\n",
						JERSEYNUM(ppdata->plyrnum), ppdata->team, pobj->x,
						FIELDX( pobj->x ));
				fprintf( stderr, "  ball was type %s with flags ", loose_ball_names[ball_obj.type] );
				print_ball_flags();
#endif
//				reset_onfire_values();
				game_info.off_side = ppdata->team;
				game_info.old_los = FIELDX( pobj->x );
				pdata_blocks[0].turbo = 100.0f;
				pdata_blocks[1].turbo = 100.0f;
				pdata_blocks[2].turbo = 100.0f;
				pdata_blocks[3].turbo = 100.0f;

				// update player modes & clear appropriate PF bits
				for( i = 0; i < NUM_PLAYERS; i++ )
				{
					player_blocks[i].odata.flags &= ~PF_SPEED_OVERRIDE;

					if( (i/7) == game_info.off_side )
						player_blocks[i].odata.plyrmode = MODE_WPN;
					else
						player_blocks[i].odata.plyrmode = MODE_DB;
				}
				drone_poss_change();

				if( ball_obj.type == LB_PUNT )
					game_info.ko_catch = FIELDX(pobj->x);
			}

			// flash player
			qcreate ("flash", PFLASH_PID, flash_plyr_proc, ppdata->plyrnum, 4, 3, 0);

			// make catch speech and sounds
			catch_sounds(pobj);

			// First catch on each kickoff
			if (ball_obj.type == LB_KICKOFF)
			{
				game_info.ko_catch = FIELDX(pobj->x);
				if (!idiot_boxes_shown)
					qcreate("idiots",0,ko_idiot_box_proc,0,0,0,0);
			}

			// check for catch in end zone
			if ((FIELDX( pobj->x ) < 2) &&
				((!offense) || (ball_obj.type == LB_KICKOFF)))
			{
				game_info.game_flags |= GF_REC_IN_EZ;
				game_info.game_flags &= ~GF_LEFT_EZ;
#ifdef PLAY_DEBUG
				fprintf( stderr, "  REC_IN_EZ set on catch\n" );
				fprintf( stderr, "    FIELDX(pobj->x) = %d\n", FIELDX(pobj->x));
				fprintf( stderr, "    offense = %s\n", offense ? "TRUE" : "FALSE" );
				fprintf( stderr, "    ball_obj.type = %s\n", loose_ball_names[ball_obj.type] );
#endif
			}
			// no need to clear these bits except in the above case.
//			else
//			{
//				if( game_info.game_flags & (GF_REC_IN_EZ|GF_LEFT_EZ) )
//				{
//					game_info.game_flags &= ~(GF_REC_IN_EZ|GF_LEFT_EZ);
//#ifdef PLAY_DEBUG
//					fprintf( stderr, "  REC_IN_EZ and LEFT_EZ cleared on new catch\n" );
//#endif
//				}
//			}

			// if our y > 0, set GF_AIR_CATCH
			if( pobj->y > 0.0f )
			{
#ifdef PLAY_DEBUG
				fprintf( stderr, "  GF_AIR_CATCH set\n" );
#endif
				game_info.game_flags |= GF_AIR_CATCH;
			}
			else
			{
#ifdef PLAY_DEBUG
				if( game_info.game_flags & GF_AIR_CATCH )
					fprintf( stderr, "  GF_AIR_CATCH cleared by new catch\n" );
#endif
				game_info.game_flags &= ~GF_AIR_CATCH;
			}

			break;
		case CR_MISS:
			ppdata->no_catch_time = 20;
			break;
		case CR_BOBBLE:
			// stop fire ball if ball is bobbled
			ball_obj.flags &= ~BF_FIRE_PASS;

			ppdata->no_catch_time = 20;

			// take control of that receiver
			// if catcher is a drone on human-controlled team:
			// offense: set control to guy who threw the ball
			// defense: set control to human farthest away
			if(( ISDRONE( ppdata )) && ISHUMANTEAM( ppdata ))
			{
				int		station;

				if( offense )
					station = player_blocks[ball_obj.who_threw].station;
				else
					station = player_blocks[farthest_teammate( ppdata )].station;

				game_info.plyr_control[station] = ppdata->plyrnum;
				update_stations();

				ppdata->but_cur |= P_A;
				ppdata->but_cur |= P_B;
			}

#ifdef PLAY_DEBUG
			fprintf( stderr, "  %2x(%d) bobbles (no hit) at %5.2f, field %d\n",
				JERSEYNUM(ppdata->plyrnum), ppdata->team, pobj->x,
				FIELDX(pobj->x) );
#endif
			bobble_pass( ppdata, 0.0f, 0.0f, 0.0f );
			bobble_ball_sounds();
			break;
		case CR_BATDOWN:
			ppdata->no_catch_time = 20;
			bobble_pass( ppdata, 0.0f, -2.0f, 0.0f );
			if ((pobj->adata[0].seq_index == (SEQ_C_JMPINT)) || (pobj->adata[0].seq_index == (SEQ_C_JMPIN3)))
				change_anim(pobj, air_swat_anim);
			snd_scall_bank(plyr_bnk_str,DOINK_SND,VOLUME4,127,PRIORITY_LVL5);
			qcreate ("deflct", 0, delay_deflect_proc, 0,0,0,0);
//			deflected_ball_sp();
			break;
		case CR_DEFLECT:
			ppdata->no_catch_time = 20;
			if (!(game_info.game_flags & GF_ONSIDE_KICK))
				snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME4,127,PRIORITY_LVL4);
			deflect_pass( pobj );
			break;
		default:
#ifdef DEBUG
			fprintf( stderr, "bad catch case\n" );
			lockup();
#endif
			break;

	}
}

//////////////////////////////////////////////////////////////////////////////
//static void intercept_snd_proc(int *args)
//{
//	sleep(40);
//}

//////////////////////////////////////////////////////////////////////////////
static float dist3d( float *p1, float *p2 )
{
	float	dx,dy,dz;

	dx = p2[0] - p1[0];
	dy = p2[1] - p1[1];
	dz = p2[2] - p1[2];

	return fsqrt(dx*dx + dy*dy + dz*dz);
}

//////////////////////////////////////////////////////////////////////////////
void general_run( obj_3d *pobj )
{
//	if( pobj->plyrmode == MODE_QB )
	if( (pobj->plyrmode == MODE_QB) && (game_info.game_mode != GM_PLAY_OVER))
		qb_run( pobj );
	else
		standard_run( pobj );
}

//////////////////////////////////////////////////////////////////////////////
unsigned int breath_control = 0;

#define MOTION_WIDTH	60.0f
static void standard_run( obj_3d *pobj )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	int		run_seqs[] = { SEQ_RUN, SEQ_R_TRB, SEQ_RUN_WB, SEQ_R_TRB_WB, SEQ_RUN_B, SEQ_R_TRB_B,
							SEQ_HUFFRUN2, SEQ_HUFFRUN };
	char	*run_scripts[] = { run_anim, run_t_anim, run_wb_anim, run_t_wb_anim,
			run_back_anim, run_t_back_anim, r_huff_anim, r_t_huff_anim };
	char	*int_run_scripts[] = { run_anim_int, run_t_anim_int, run_wb_anim_int,
			run_t_wb_anim_int, run_back_anim_int, run_t_back_anim_int, r_huff_anim, r_t_huff_anim };

//	int		run_seqs[] = { SEQ_RUN, SEQ_R_TRB, SEQ_RUN_WB, SEQ_R_TRB_WB, SEQ_RUN_B, SEQ_R_TRB_B };
//	char	*run_scripts[] = { run_anim, run_t_anim, run_wb_anim, run_t_wb_anim,
//			run_back_anim, run_t_back_anim };
//	char	*int_run_scripts[] = { run_anim_int, run_t_anim_int, run_wb_anim_int,
//			run_t_wb_anim_int, run_back_anim_int, run_t_back_anim_int };
	int		seq_index, speed_index, new_dir, i, n, brng, skip_unint;
	int		face_dir, view_move_angle, do_backp = FALSE;
	float	speed;
	int rnd;

	float	speeds[] =
	{	// speeds by player mode, turbo, possession/human, & backpedaling
		0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	// mode_lineup
		0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	// mode_dead

		LINE_SPEED,		LINE_TURBO_SPEED,	0.0f,				0.0f,					LINE_BACKP_SPEED,	LINE_BACKP_TURBO_SPEED,
		LINE_SPEED,		LINE_TURBO_SPEED,	HUMAN_DEF_SPEED,	HUMAN_DEF_TURBO_SPEED,	LINE_BACKP_SPEED,	LINE_BACKP_TURBO_SPEED,

		// following two are irrelevant -- QB never calls this
		0.0f,		0.0f,		0.0f,		0.0f,		0.0f,		0.0f,
		0.0f,		0.0f,		0.0f,		0.0f,		0.0f,		0.0f,

		WPNPS_SPEED,	WPN_TURBO_SPEED,	0.0f,	0.0f,	0.0f, 0.0f,
		WPN_SPEED,		WPN_TURBO_SPEED,	CARRIER_SPEED,	CARRIER_TURBO_SPEED,	WPN_BACKP_SPEED,	WPN_BACKP_TURBO_SPEED,
		DBPS_SPEED,		DBPS_TURBO_SPEED,	HUMAN_DEF_SPEED,	HUMAN_DEF_TURBO_SPEED,	DB_BACKP_SPEED,	DB_BACKP_TURBO_SPEED,
		DB_SPEED,		DB_TURBO_SPEED,		HUMAN_DEF_SPEED,	HUMAN_DEF_TURBO_SPEED,	DB_BACKP_SPEED,	DB_BACKP_TURBO_SPEED,

		// kicker
		WPNPS_SPEED,	WPNPS_SPEED,		0.0f,	0.0f, 0.0f, 0.0f,

	};

	skip_unint = 0;
	if (ppdata->stick_cur != 24)
	{
		// stick is down.  set vels & facing and make sure we're in the right anim
		if ((new_dir = dir49_table[ppdata->stick_cur]) < 0) new_dir = 0;
		new_dir = 1023 & (new_dir + (int)RD2GR(cambot.theta));

#if 1	//(DHS)
		if(( pobj->plyrmode == MODE_WPNPS ) && ( game_info.play_type != PT_KICKOFF ) && (!(game_info.team_play[game_info.off_side]->flags & PLF_FAKE_FG)))
#else
		if((pobj->plyrmode == MODE_WPNPS) && (game_info.play_type != PT_KICKOFF))
#endif
		{
			if(( new_dir <= 128 ) || ( new_dir >= 896 ))
				new_dir = 0;
			else if( new_dir < 384 )
				ppdata->stick_cur = 24;
			else if( new_dir <= 640 )
				new_dir = 512;
			else
				ppdata->stick_cur = 24;

			if( pobj->z < (game_info.line_pos - MOTION_WIDTH))
				if( new_dir == 512 )
					ppdata->stick_cur = 24;
			if( pobj->z > (game_info.line_pos + MOTION_WIDTH))
				if( new_dir == 0 )
					ppdata->stick_cur = 24;
		}

		if (ppdata->stick_cur != 24)
		{
			pobj->tgt_fwd = new_dir;
			seq_index = 0;

			if (ppdata->plyrnum == game_info.ball_carrier)
				seq_index += 2;
			if (pobj->flags & PF_TURBO)
				seq_index += 1;

			// no turbo before kick on onside kicks
			if ((game_info.game_flags & GF_ONSIDE_KICK) &&
				(game_info.game_mode == GM_PRE_KICKOFF))
				seq_index &= ~1;

			if ((pobj->flags & PF_BACKPEDAL) &&
				(ppdata->bptarget >= 0 ))
			{
				view_move_angle = (1024 + new_dir - p2p_angles[ppdata->plyrnum][ppdata->bptarget]) % 1024;
				if( view_move_angle > 512 )
					view_move_angle = 1024 - view_move_angle;

				if ((ppdata->plyrnum != game_info.ball_carrier) &&
				(ISDRONE(ppdata)) &&
				(seq_index <= 1) &&	// just to be safe
				(view_move_angle >= 384))
				{
					seq_index += 4;
					do_backp = TRUE;
				}
			}

			speed_index = seq_index + pobj->plyrmode * 6;

			// human WPNPS uses WPN speeds

			if ((pobj->plyrmode == MODE_WPNPS) &&
				ISHUMAN(ppdata))
				speed_index += 6;

			// human defenders (line or db) use different speeds
			if ((ppdata->team != game_info.off_side ) &&
				(ISHUMAN(ppdata)))
				speed_index += 2;

			// human blockers use human defender speeds
			if ((ppdata->team == game_info.off_side) &&
				(ISHUMAN(ppdata)) &&
				(game_info.ball_carrier != -1) &&
				(game_info.ball_carrier != ppdata->plyrnum) &&
				(player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_WPN) &&
				(pobj->plyrmode == MODE_WPN))
				speed_index += 14;

			speed = speeds[speed_index];

			// do different run...if on defense (do after all speeds)
			if ((ppdata->team != game_info.off_side) &&
				(pobj->plyrmode == MODE_LINE) &&
				(!do_backp))
			{
				seq_index = 6;
			}
			if ((pobj->flags & PF_TURBO) &&
				(ppdata->team != game_info.off_side) &&
				(pobj->plyrmode == MODE_LINE) &&
				(!do_backp))
			{
				seq_index = 7;
			}

			// if this is a drone with the PF_SPEED_OVERRIDE bit set,
			// use the alternate speed (unless we're already going faster
			if ((ISDRONE(ppdata)) &&
				(pobj->flags & PF_SPEED_OVERRIDE))
				speed = GREATER(speed,ppdata->speed);

			// offscreen defenders get big speed boost
			if( (ppdata->team != game_info.off_side ) && !(pobj->flags & PF_ONSCREEN ))
			{
				if (game_info.ball_carrier == -1)
					speed *= 1.5f;
				else
					speed *= 2.5f;
			}

			// wpns on kickoff (in flight) get speed boost
			if ((game_info.play_type == PT_KICKOFF) &&
				!(game_info.game_flags & GF_ONSIDE_KICK) &&
				(ball_obj.type == LB_KICKOFF) &&
				(game_info.ball_carrier == -1) &&
				(pobj->plyrmode == MODE_WPN))
				speed *= 1.38f;

			// humans chasing humans get big speed boost
			if ((ppdata->team != game_info.off_side) &&
				(ISHUMAN(ppdata)) &&
				(game_info.ball_carrier != -1) &&
				(FIELDX( pobj->x ) < FIELDX( ball_obj.odata.x)))
				speed *= 1.20f;

			// team speed boost
			if (pup_speed)
				speed *= 1.3f;

			if ((pup_fastturborun & TEAM_MASK(ppdata->team)) && (pobj->flags & PF_TURBO) && !pup_speed)
				speed *= 1.1f;

			// if we're going pretty slow, don't use turbo anim
			if (speed < 1.45f)
			   seq_index &= ~1;

			// slow the DRAGGER down a bit
			if (pobj->flags & PF_DRAGGING_PLR)
				speed *= .85f;

			pobj->vx = FRELX( 0.0f, speed, pobj->tgt_fwd );
			pobj->vz = FRELZ( 0.0f, speed, pobj->tgt_fwd );

			// this allows the man who is dragging a dude...to still stiff arm!!!
			if ((ppdata->plyrnum == game_info.ball_carrier) &&
				(pobj->flags & PF_DRAGGING_PLR))
			{
				if ((pobj->adata[1].animode & MODE_STIFF_ARM) && (!(pobj->adata[1].animode & MODE_TWOPART)))
				{
					skip_unint = 1;
					begin_twopart_mode(pobj);
					if (randrng(100) < 50)
					{
						change_anim2(pobj, stiff_arm_anim);
//						stiff_arm_speech();
					}
					else
					{
						change_anim2(pobj, stiff_arm_f_anim);
//						stiff_arm_f_speech();
					}
					stiff_arm_speech();
				}
			}

//			if (ppdata->plyrnum == game_info.ball_carrier)
			if ((ppdata->plyrnum == game_info.ball_carrier) &&
				(!(pobj->flags & PF_DRAGGING_PLR)))
			{
				// head plow move ?
				if ((pobj->adata[1].seq_index != (SEQ_M_PLOWHD)) &&
					(!(pobj->adata[1].animode & MODE_TWOPART)) &&
					(ppdata->but_cur & P_C) &&
					(pobj->adata[0].seq_index == run_seqs[seq_index]) &&
					(fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.20f) &&
					((game_info.off_side ? -pobj->vx : pobj->vx) > 0.0))
				{
					for (i=0,n=0; i < NUM_PLAYERS; i++)
					{
						// dont check myself
						if (ppdata->plyrnum == i)
							continue;

						// ignore if on same team
						if (ppdata->team == player_blocks[i].team)
							continue;

						// check table probability
						if (randrng(100) > plow_ht[PLYRH(ppdata)])
							continue;

						// If more than 2 opponents are within 10 yards and in my front 90 degrees...
						if (distxz[ppdata->plyrnum][i]/YD2UN > 8.0f)
							continue;

						// but only if opponent is in front of me
						brng = bearing( ppdata->plyrnum, i );

						// opponent is within +- 45 degrees of me
						if ((brng < 64) && (++n >= 2))
						{
							begin_twopart_mode(pobj);
							plowhd_snd = 0;
							change_anim2(pobj, running_plow_anim);
							break;
						}
					}
				}
				// stiff arm ?
				if ((pobj->adata[1].animode & MODE_STIFF_ARM) && (!(pobj->adata[1].animode & MODE_TWOPART)))
				{
					skip_unint = 1;
					begin_twopart_mode(pobj);
					// If no turbo left, do a weaker version of stiff arm - same look, different attack box
					if (pdata_blocks[ppdata->team].turbo > 5.0f)
					{
						if (randrng(100) < 50)
						{
							change_anim2(pobj, stiff_arm_anim);
//							stiff_arm_speech();
						}
						else
						{
							change_anim2(pobj, stiff_arm_f_anim);
//							stiff_arm_f_speech();
						}
						stiff_arm_speech();
					}
					else
					{
						change_anim2(pobj, stiff_arm_nt_anim);
						stiff_arm_speech();
					}
				}
				// if the ball carrier is past the enemy 20 yard line
				else if ((FIELDX(pobj->x) > 80) &&
						 (!(pobj->adata[1].animode & MODE_TWOPART)) &&
				    	 (pobj->adata[0].seq_index != (SEQ_R_HISTEP)) &&
				    	 (pobj->adata[0].seq_index != (SEQ_R_HIHAND)) &&
				    	 (pobj->adata[0].seq_index != (SEQ_R_SKIP)) &&
//				    	 (pobj->adata[0].seq_index != (SEQ_R_BKTAU1)) &&
				    	 (pobj->adata[0].seq_index != (SEQ_R_BKTAU2)) &&
				    	 (pobj->adata[0].seq_index != (SEQ_R_INBTA2)) &&
						 (pobj->adata[0].seq_index != (SEQ_R_EZSPRI)))
				{
					skip_unint = 1;

					change_anim(pobj, running_taunt_tbl[randrng(sizeof(running_taunt_tbl)/sizeof(char *))]);

					// get distance of others players from me
					for (i=0; i < NUM_PLAYERS; i++)
					{
						// ignore myself
						if (i == ppdata->plyrnum)
							continue;

						// ignore teammates
						if (ppdata->team == player_blocks[i].team)
							continue;

						// If within 10 yards of me...do sprint
						if (distxz[ppdata->plyrnum][i]/YD2UN < 6.0f)
						{
							change_anim(pobj, r_ezsprint_anim);
							break;
						}
					}
				}
				// if ball carrier is behind the enemy 20 yard line
				else if (FIELDX(pobj->x) <= 80)
				{
					if ((pobj->adata[0].seq_index != run_seqs[seq_index]) &&
				 	    (!(pobj->adata[1].animode & MODE_TWOPART)) &&
					    (!(pobj->adata[0].animode & MODE_ROTONLY)))
					{
						if (pobj->adata[0].animode & MODE_INTXITION)
							change_anim(pobj, int_run_scripts[seq_index]);
						else
							change_anim(pobj, run_scripts[seq_index]);
					}
				}
			}
//			else
			else if (ppdata->plyrnum != game_info.ball_carrier)
			{	// NOT the BALL CARRIER

				// running push ?
				if ((pobj->adata[1].animode & MODE_RUNNING_PUSH) &&
					(!(pobj->adata[1].animode & MODE_TWOPART)))
				{
					skip_unint = 1;
					begin_twopart_mode(pobj);
					// If no turbo left, do a weaker version of stiff arm - same look, different attack box
					if (pdata_blocks[ppdata->team].turbo > 5.0f)
						change_anim2(pobj, t_running_push_anim);
					else
						change_anim2(pobj, t_running_push_anim);
				}

				i = game_info.off_side ? 10 : 3;
				if ((game_info.team_play[!game_info.off_side]->flags & PLF_BLITZ) &&
					!(pobj->adata[1].animode & MODE_TWOPART) &&
					!(pobj->adata[0].animode & MODE_UNINT) &&
					(FIELDX(pobj->x) < game_info.los) &&
					(ppdata->team != game_info.off_side) &&	(pobj->plyrmode == MODE_LINE) &&
					((distxz[ppdata->plyrnum][i])/YD2UN < 8.0f))
//					(((1024 + player_blocks[i].odata.fwd - p2p_angles[i][ppdata->plyrnum]) % 1024) < 128))
				{
					skip_unint = 1;
					begin_twopart_mode(pobj);
					change_anim2(pobj, r_crzbl2_anim);
				}

				if ((pobj->flags & PF_WAVE_ARM) && (fsqrt(pobj->vx*pobj->vx + pobj->vz*pobj->vz) > 1.0f))
				{
					pobj->flags &= ~PF_WAVE_ARM;
					skip_unint = 1;
					if ((game_info.off_side ? -pobj->vz : pobj->vz) > 0.0)
					{
						begin_twopart_mode(pobj);
						change_anim2(pobj, m_wavqb_anim);
					}
					else
					{
						begin_twopart_mode(pobj);
						change_anim2(pobj, m_wavqbf_anim);
					}
				}
				else if ((pobj->adata[0].seq_index != run_seqs[seq_index]) &&
//				 		 (pobj->adata[0].seq_index != (SEQ_BK_PEDAL)) &&
			 	 		 !(pobj->adata[0].animode & MODE_ROTONLY))
				{
					if( pobj->adata[0].animode & MODE_INTXITION )
						change_anim( pobj, int_run_scripts[seq_index] );
					else
						change_anim( pobj, run_scripts[seq_index] );
				}

				if (do_backp)
				{
					pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][ppdata->bptarget];
//					fprintf( stderr, "%2x backp watching %2x\n",
//						JERSEYNUM(ppdata->plyrnum),JERSEYNUM(ppdata->bptarget));
				}
			}
		}
	}

	// this isn't an else to the above if because the test value can change in
	// the above block and we have to run both cases.

	if ((ppdata->stick_cur == 24) &&
		(!((ppdata->plyrnum == game_info.ball_carrier) && (pobj->flags & PF_DRAGGING_PLR))))
	{
		pobj->vx = 0.0f;
		pobj->vz = 0.0f;

		// stick is neutral.
		if ((ppdata->stick_old-24) || (pobj->adata[0].animode & MODE_UNINT))
		{
			// stance ?
			if (ppdata->plyrnum == game_info.ball_carrier)
				change_anim(pobj, stance_wb_anim_int);
			else
			{
				if ((pobj->plyrmode == MODE_WPNPS) && (game_info.play_type != PT_KICKOFF))
				{
					// running back in motion prior to snap.  do special stuff
					pobj->tgt_fwd = game_info.off_side ? 768 : 256;
					change_anim( pobj, wr_set_anim_int );
					if( ppdata->team == 0 )
						pobj->x = WORLDX(game_info.los) + game_info.team_play[0]->formation[ppdata->position].x;
					else
						pobj->x = WORLDX(game_info.los) - game_info.team_play[1]->formation[ppdata->position].x;
				}
				else
				{
					if (game_info.game_mode != GM_PLAY_OVER)
					{
						if ((ppdata->team != game_info.off_side) &&
							(game_info.team_fire[!game_info.off_side]) &&
							(game_info.game_mode == GM_PRE_SNAP))
							change_anim(pobj, m_dblood_anim);
						else if (ISHUMAN(ppdata))
							change_anim(pobj, safty2pt_anim_rnd);
						else
							change_anim(pobj, (pobj->adata[0].animode & MODE_INTXITION) ? breath2_anim : breath2_anim);
//							change_anim(pobj, (pobj->adata[0].animode & MODE_INTXITION) ? block_ready_anim_int : block_ready_anim );
					} else {
#if 0
						change_anim(pobj, breath_seq_tbl[0]);
#else
//fprintf(stderr, "A> table size = %d\n", sizeof(breath_seq_tbl)/sizeof(char *));
						rnd = unique_random((sizeof(breath_seq_tbl)/sizeof(char *)), breath_control);
						if (!(breath_control & (1<<rnd)) )
							breath_control |= (1<<rnd);
						change_anim(pobj, breath_seq_tbl[rnd]);
#endif
					}
				}
			}
		}

		if (pobj->flags & PF_DRONEFACE)
		{
			pobj->flags &= ~PF_DRONEFACE;
			pobj->tgt_fwd = ppdata->drone_face;
		}
	}

	// if UNINT bit was set on bottom half when we got here, clear it
	if (!skip_unint)
		pobj->adata[0].animode &= ~MODE_UNINT;
}


//////////////////////////////////////////////////////////////////////////////
static void qb_run( obj_3d *pobj )
{
	int		face_dir, adj_dir, qb_index;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	float	speed, turbo_speed;
	char *qb_runs[] = {
	//	torso			turbo torso			legs			turbo legs
		qb_run_anim,	qb_t_run_anim,		qb_run_anim,	qb_t_run_anim,		// forward
		qb_runl_anim,	qb_t_runl_anim,		qb_run_anim,	qb_t_run_anim,		// left
		qb_runb_anim,	qb_t_runb_anim,		qb_runl45_anim,	qb_t_runl45_anim,	// left rear
		qb_runb_anim,	qb_t_runb_anim,		qb_runb_anim,	qb_t_runb_anim,		// back
		qb_runb_anim,	qb_t_runb_anim,		qb_runr45_anim,	qb_t_runr45_anim,	// right rear
		qb_runr_anim,	qb_t_runr_anim,		qb_run_anim,	qb_t_run_anim,		// right
	};

	int qb_seqs[] = {
		SEQ_QBRUNF,	SEQ_QBRUNF_T,	SEQ_QBRUNF,		SEQ_QBRUNF_T,
		SEQ_QBRUNL,	SEQ_QBRUNL_T,	SEQ_QBRUNF,		SEQ_QBRUNF_T,
		SEQ_QBRUNB,	SEQ_QBRUNB_T,	SEQ_QBRUNL45,	SEQ_QBRL45_T,
		SEQ_QBRUNB,	SEQ_QBRUNB_T,	SEQ_QBRUNB,		SEQ_QBRUNB_T,
		SEQ_QBRUNB,	SEQ_QBRUNB_T,	SEQ_QBRUNR45,	SEQ_QBRR45_T,
		SEQ_QBRUNR,	SEQ_QBRUNR_T,	SEQ_QBRUNF,		SEQ_QBRUNF_T
	};

	// if qb is throwing ball...do nothing here
	if (pobj->adata[1].animode & MODE_UNINT)
		return;

 	if ((adj_dir = dir49_table[ppdata->stick_cur]) < 0) adj_dir = 0;
	adj_dir =  1023 & (adj_dir + (int)RD2GR(cambot.theta) - 256 + (512*ppdata->team));
 	if ((face_dir = dir49_table[ppdata->stick_cur]) < 0) face_dir = 0;
	face_dir = 1023 & (face_dir + (int)RD2GR(cambot.theta));

	qb_index = 0;
	speed = QB_SPEED;
	turbo_speed = QB_TURBO_SPEED;

	if(( adj_dir <= 128 ) || ( adj_dir >= 896 ))
	{
		qb_index = 0;
	}
	if(( adj_dir > 128 ) && ( adj_dir <= 332 ))
	{
		qb_index = 1;
		pobj->flags |= PF_FLIP;
	}
	if(( adj_dir > 332 ) && ( adj_dir < 460 ))
	{
		qb_index = 2;
		pobj->flags |= PF_FLIP;
		speed = QB_BACKP_SPEED;
		turbo_speed = QB_BACKP_TURBO_SPEED;
	}
	if(( adj_dir >= 460 ) && ( adj_dir <= 564 ))
	{
		qb_index = 3;
		speed = QB_BACKP_SPEED;
		turbo_speed = QB_BACKP_TURBO_SPEED;
	}
	if(( adj_dir > 564 ) && ( adj_dir < 692 ))
	{
		qb_index = 4;
		pobj->flags &= ~PF_FLIP;
		speed = QB_BACKP_SPEED;
		turbo_speed = QB_BACKP_TURBO_SPEED;
	}
	if(( adj_dir >= 692 ) && ( adj_dir < 896 ))
	{
		qb_index = 5;
		pobj->flags &= ~PF_FLIP;
	}

	qb_index *= 4;
	if( pobj->flags & PF_TURBO )
		qb_index += 1;

	// dont allow qb to far from line-of-scrimage - slow him down
	if (FIELDX(pobj->x) < game_info.los - 18)
	{
		speed /= 1.50f;
		turbo_speed /= 1.50f;
	}

	// set velocities
	if( ppdata->stick_cur-24 )
	{
		pobj->vx = FRELX( 0.0f, ((pobj->flags & PF_TURBO) ? turbo_speed : speed), face_dir );
		pobj->vz = FRELZ( 0.0f, ((pobj->flags & PF_TURBO) ? turbo_speed : speed), face_dir );
	}

	if (ppdata->stick_cur != 24)
	{
		// set sequences
		begin_twopart_mode( pobj );

		// stick is down. set vels & facing and make sure we're in the right run.
		pobj->tgt_fwd = face_dir;
		if((adj_dir > 332) && (adj_dir < 692))
			pobj->tgt_fwd = game_info.off_side ? 768 : 256;
		pobj->tgt_fwd2 = pobj->tgt_fwd;
		pobj->turn_rate = 64;
		pobj->turn_rate2 = 48;

		if(( qb_seqs[qb_index] != pobj->adata[1].seq_index ) && !( pobj->adata[0].animode & MODE_ROTONLY ))
		{
			if( pobj->fwd2 != pobj->tgt_fwd )
			{
				pobj->adata[1].animode |= MODE_TURNHACK;
				pobj->adata[1].hackval = pobj->fwd2 - pobj->tgt_fwd;
				pobj->fwd2 = pobj->tgt_fwd;
				pobj->tgt_fwd2 = pobj->tgt_fwd;
			}

			// torso interpolates 4 ticks to proportional frame of new sequence
			pobj->adata[1].vframe = AA_PROPFRAME;
			pobj->adata[1].vcount = 4;

			change_anim2( pobj, qb_runs[qb_index] );
		}

		if(( qb_seqs[qb_index+2] != pobj->adata[0].seq_index ) && !( pobj->adata[0].animode & MODE_ROTONLY ))
		{
			// legs interpolate 2 ticks to proportional frame of new sequence
			pobj->adata[0].vframe = AA_PROPFRAME;
			pobj->adata[0].vcount = 2;

			change_anim1( pobj, qb_runs[qb_index+2] );
		}
	}
	else
	{
		zerovels( pobj );

		// stick is neutral
		if(( ppdata->stick_old-24 ) || ( pobj->adata[0].animode & MODE_UNINT ))
		{
			// set sequences
			begin_twopart_mode( pobj );

			// wasn't neutral before--go to stance
			pobj->tgt_fwd = game_info.off_side ? 768 : 256;
			pobj->adata[1].animode |= MODE_TURNHACK;
			pobj->adata[1].hackval = pobj->fwd2 - pobj->tgt_fwd;
			pobj->tgt_fwd2 = pobj->tgt_fwd;
			pobj->fwd = pobj->tgt_fwd;
			pobj->fwd2 = pobj->tgt_fwd;

			change_anim( pobj, (pobj->flags & PF_FLIP) ? qb_flip_stance_anim : qb_stance_anim );

//			change_anim2( pobj, (pobj->flags & PF_FLIP) ? qb_flip_stance_anim_top : qb_stance_anim_top );
//			change_anim1( pobj, (pobj->flags & PF_FLIP) ? qb_flip_stance_anim_bot : qb_stance_anim_bot );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void begin_twopart_mode( obj_3d *pobj )
{
	int		i;

	if( pobj->adata[1].animode & MODE_TWOPART )
		return;

	// begin with the top half animating just like the bottom
	pobj->adata[1].seq_index = pobj->adata[0].seq_index;
	pobj->adata[1].pscrhdr = pobj->adata[0].pscrhdr;
	pobj->adata[1].pscript = pobj->adata[0].pscript;
	pobj->adata[1].anicnt = pobj->adata[0].anicnt;
	pobj->adata[1].pframe0 = pobj->adata[0].pframe0;
	pobj->adata[1].pframe = pobj->adata[0].pframe;
	pobj->adata[1].pxlate = pobj->adata[0].pxlate;
	pobj->adata[1].iframe = pobj->adata[0].iframe;
	pobj->adata[1].pahdr = pobj->adata[0].pahdr;
	pobj->adata[3].iframe = pobj->adata[2].iframe;
	pobj->adata[1].animode = pobj->adata[0].animode;



	if( pobj->adata[0].pahdr->flags & AH_COMPRESSED )
	{
		pobj->adata[1].pq = pobj->adata[1].adata;
		for( i = 0; i < 60; i++ )
		{
			pobj->adata[1].pq[i] = pobj->adata[0].pq[i];
		}
	}
	else
	{
		pobj->adata[1].pq = pobj->adata[0].pq;
	}

	pobj->fwd2 = pobj->fwd;
	pobj->tgt_fwd2 = pobj->tgt_fwd;

	pobj->adata[1].animode |= MODE_TWOPART;
}


//////////////////////////////////////////////////////////////////////////////
static void maybe_plyr_fire(fbplyr_data *ppdata)
{
#ifndef NO_FIRE
	obj_3d	*pobj = (obj_3d *)ppdata;

	// no fire effect for man carrying defender
	if (pobj->flags & PF_DRAGGING_PLR)
		return;

	if ((game_info.team_fire[ppdata->team]) &&
		(ISHUMAN(ppdata)) &&
		((fabs(ppdata->odata.vx) > 0.0f) || (fabs(ppdata->odata.vz) > 0.0f) || (pobj->adata[0].animode & MODE_REACTING)))
	{
		if ((pcount % (randrng(4)+1)) == 0)
		{
			if (ppdata->team == game_info.off_side)
			{
				if (pobj->plyrmode == MODE_QB)
				{
					// orange flames for offense
					if (pobj->flags & PF_FLIP)
						qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_LWRIST, 0, 0);
					else
						qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_RWRIST, 0, 0);
//					qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_LANKLE, 0, 0);
//					qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_RANKLE, 0, 0);
//					qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_PELVIS, 0, 0);
				}
				else
				{
					// orange flames for offense
					qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_RWRIST, 0, 0);
//					qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_LWRIST, 0, 0);
//					qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_LANKLE, 0, 0);
//					qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_RANKLE, 0, 0);
				}

			}
			else
			{
				// flames for defense
				qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_LANKLE, 0, 0);
				qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_RANKLE, 0, 0);
				qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, ppdata->plyrnum, JOINT_PELVIS, 0, 0);
////			qcreate ("dfire", SPFX_PID, plyr_bfire_trail_proc, ppdata->plyrnum, JOINT_LANKLE, 0, 0);
////			qcreate ("dfire", SPFX_PID, plyr_bfire_trail_proc, ppdata->plyrnum, JOINT_LKNEE, 0, 0);
//				qcreate ("dfire", SPFX_PID, plyr_bfire_trail_proc, ppdata->plyrnum, JOINT_RANKLE, 0, 0);
////			qcreate ("dfire", SPFX_PID, plyr_bfire_trail_proc, ppdata->plyrnum, JOINT_RKNEE, 0, 0);
			}
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////
void player_burst_into_flames(int *args)
{
	int pnum,i;
	int rnd;

	pnum = args[0];
	for (i=0; i < args[1]; i++)
	{


		qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, pnum, JOINT_LWRIST, 0, 0);
		qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, pnum, JOINT_RANKLE, 0, 0);
//		rnd = randrng(2)+1;
//		sleep(rnd);

//		rnd = randrng(10);
//		sleep(rnd);

		qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, pnum, JOINT_RWRIST, 0, 0);
		qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, pnum, JOINT_RSHOULDER, 0, 0);
//		rnd = randrng(2)+1;
//		sleep(rnd);


		qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, pnum, JOINT_LSHOULDER, 0, 0);
		qcreate ("ofire", SPFX_PID, plyr_fire_trail_proc, pnum, JOINT_LANKLE, 0, 0);
//		rnd = randrng(10);
//		sleep(rnd);

		sleep(args[2]);

		// dont do puffs after play starts
		if (game_info.game_mode == GM_IN_PLAY)
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////
#if 0
void scale_plyrs_proc(int *args)
{
	fbplyr_data		*ppdata;
	int				steps,i;
//	float			scale_tbl[] = {1.04,1.08,1.12,1.16,1.20,1.24,1.28,1.32,1.36,1.40,
//								   1.44,1.48,1.52,1.56,1.60,1.64,1.68,1.72,1.76,1.80,
//								   1.70,1.60,1.50,1.40,1.30,1.20,1.15};

	float			scale_tbl[] = {1.00,1.14,1.04,1.18,1.06,1.22,1.08,1.26,1.10,1.30,
								   1.12,1.34,1.14,1.38,1.16,1.42,1.18,1.46,1.20,1.50,
								   1.15,1.40,1.10,1.30,1.05,1.15};

	// scale defensive players
	for(steps=0; steps < (int)(sizeof(scale_tbl)/sizeof(float)); steps++)
	{
		for(i = 0; i < 7; i++)
		{
			ppdata = player_blocks + i + !game_info.off_side * 7;
			ppdata->odata.ascale = ppdata->static_data->scale * scale_tbl[steps];
		}
		sleep(2);
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
void reset_onfire_values(void)
{
#ifndef NO_FIRE
	#ifdef FIRE_DEBUG
	if (game_info.team_fire[0])
		fprintf( stderr, "FIRE: Team 0 loses fire (reset)\n" );
	if (game_info.team_fire[1])
		fprintf( stderr, "FIRE: Team 1 loses fire (reset)\n" );
	#endif

	game_info.team_fire[0] = 0;
	tdata_blocks[0].consecutive_comps = 0;
	tdata_blocks[0].last_receiver = -1;
	tdata_blocks[0].consecutive_sacks = 0;
	game_info.team_fire[1] = 0;
	tdata_blocks[1].consecutive_comps = 0;
	tdata_blocks[1].last_receiver = -1;
	tdata_blocks[1].consecutive_sacks = 0;

#endif
}

//////////////////////////////////////////////////////////////////////////////
void reset_team_fire( int team )
{
	game_info.team_fire[team] = 0;
	tdata_blocks[team].consecutive_comps = 0;
	tdata_blocks[team].last_receiver = -1;
	tdata_blocks[team].consecutive_sacks = 0;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
void set_trgt_fwd( obj_3d *pobj, int tfwd, int trot)
{
	pobj->tgt_fwd = (pobj->fwd - tfwd) % 1024;
	pobj->turn_rate = trot;
}
#endif


//////////////////////////////////////////////////////////////////////////////
void rotate_anim( obj_3d *pobj, int angle)
{
	if (pobj->pobj_hit == NULL)
		return;

	pobj->fwd = (pobj->pobj_hit->fwd + angle + 1024) % 1024;
	pobj->tgt_fwd = pobj->fwd;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
void turn_player( obj_3d *pobj, int angle)
{
	pobj->fwd = (pobj->fwd + angle + 1024) % 1024;
	pobj->tgt_fwd = pobj->fwd;
}
#endif

//////////////////////////////////////////////////////////////////////////////
void face_attacker( obj_3d *pobj, int angle)
{
	int		me,him;

	me = ((fbplyr_data *)pobj)->plyrnum;
	him = ((fbplyr_data *)(pobj->pobj_hit))->plyrnum;

	pobj->fwd = p2p_angles[me][him];
	pobj->tgt_fwd = p2p_angles[me][him];
}

//////////////////////////////////////////////////////////////////////////////
// if angle between my face dir and dir from me to carrier is <= angle, turn
// to face him.  Return 0 if successful, 1 if not.
int face_carrier( obj_3d *pobj, int angle )
{
	int		me,him;
	int		brng;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	fbplyr_data *ppdata2 = (fbplyr_data *)pobj->pobj_hit;

	me = ppdata->plyrnum;
	him = game_info.ball_carrier;

	if ((him == -1) && (ppdata2->plyrnum != ball_obj.int_receiver))
		return 1;

	// face the intended receiver, if no ball carrier
	if ((him == -1) && (ppdata2->plyrnum == ball_obj.int_receiver))
		him = ppdata2->plyrnum;

	brng = bearing(me, him);

	if (brng <= angle)
	{
		pobj->tgt_fwd = p2p_angles[me][him];
		return 0;
	}
	else
		return 1;
}

//////////////////////////////////////////////////////////////////////////////
// if angle between my face dir and dir from me to my throw_target is <= angle,
// turn to face him.  Return 0 if successful, 1 if not.
int face_throw_target( obj_3d *pobj, int angle )
{
	int		me,him;
	int		brng;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	me = ppdata->plyrnum;
	him = ppdata->throw_target;

	if( him == -1 )
		return 1;

	brng = bearing( me, him );

	if( brng <= angle )
	{
		pobj->tgt_fwd = p2p_angles[me][him];
		return 0;
	}
	else
	{
		return 1;
	}
}


//////////////////////////////////////////////////////////////////////////////
// if angle between my face dir and ball is <= angle,
// turn to the ball.  Return 0 if successful, 1 if not.
void face_ball( obj_3d *pobj, int angle )
{
	int		angle2ball;
	int		brng;

	angle2ball = ptangle(&(pobj->x), &(ball_obj.tx));
	brng = (1024 + pobj->fwd - angle2ball) % 1024;

	if (brng <= angle)
		pobj->tgt_fwd = angle2ball;
}

//////////////////////////////////////////////////////////////////////////////
int choose_target( obj_3d *pobj, int mode )
{
	int			i,tgt = -1;
	fbplyr_data	*ppdata,*ptgt;
	float		cur,best = 10000.0f;

	ppdata = (fbplyr_data *)pobj;

	for( i = 0; i < NUM_PLAYERS; i++ )
	{
		ptgt = player_blocks + i;

		// ignore self
		if (ptgt == ppdata)
			continue;

		// ignore same team, if seeking ball_carrier or intended reciever!!
		if ((mode == 1) && (ptgt->team == ppdata->team))
			continue;

		cur = weighted_dist(ppdata, ptgt);

		// if guy is behind us, skip him
		if (cur < 0.0f)
			continue;

		// if guy is more than 12 yards away (adjusted), skip him
		if (cur > 12*YD2UN)
			continue;

		if ((mode == 1) && (game_info.ball_carrier == i))
		{	// first, favor the BALL carrier...duh!
			tgt = i;
			best = 0.0f;
		}

		if ((mode == 1) && (ball_obj.int_receiver == i))
		{	// next...favor the intended receiver
			tgt = i;
			best = 1.0f;
		}

		if (cur < best)
		{	// favor closet opponent
			tgt = i;
			best = cur;
		}
	}
	ppdata->throw_target = tgt;

	if( tgt == -1 )
		return 1;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////////
int my_choose_target(obj_3d *pobj)
{
	int			i;
	fbplyr_data	*ppdata,*ptgt;
	int			tgt;

	ppdata = (fbplyr_data *)pobj;
	tgt = -1;

	// returns the player number to hit (in ppdata->throw_target) and 0 ... else 1 (no target)
	choose_target(pobj, 1);

	if (ppdata->throw_target == -1)
		return 1;			// no target
	else
	{
		// FOUND target...turn to face the him
//		pobj->tgt_fwd2 = p2p_angles[ppdata->plyrnum][player_blocks[ppdata->throw_target].plyrnum];
		pobj->tgt_fwd = p2p_angles[ppdata->plyrnum][player_blocks[ppdata->throw_target].plyrnum];
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
void lower_player( obj_3d *pobj )
{
fbplyr_data		*ppdata = (fbplyr_data *)pobj;

	// put motha'fkr on da ground
	ppdata->odata.y = -3.0f;
}

//////////////////////////////////////////////////////////////////////////////
void set_unint_lowerhalf( obj_3d *pobj)
{
	pobj->adata[0].animode |= MODE_UNINT;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
int maybe_drop_kick(obj_3d *pobj)
{
fbplyr_data		*ppdata = (fbplyr_data *)pobj;
int				brng,me,him;

	if (!pup_violence)
		return 0;

	if (ppdata->throw_target == -1)
		return 0;						// no chosen target

	me = ppdata->plyrnum;
	him = ppdata->throw_target;

	// get targets' brng to attacker...
	brng = (1024 + player_blocks[ppdata->throw_target].odata.fwd - p2p_angles[him][me]) % 1024;
	if( brng > 512 )
		brng = 1024 - brng;

	if (brng > 128)
		return 0;				// dont do drop kick
	else
		return 1;				// drop kick'em
}
#endif
//////////////////////////////////////////////////////////////////////////////
int maybe_spear_head(obj_3d *pobj)
{
fbplyr_data		*ppdata = (fbplyr_data *)pobj;

#ifdef TM	// test tackles
	return 1;
#endif

	// always do spear head...if blocking for ball carrier.
	if (ppdata->team == game_info.off_side)
		return 1;			// spear head


	// if diver has no target...who cares...just do one of the dives
	if (ppdata->throw_target == -1)
	{
		if (randrng(100) < 55)
			return 0;			// no spear head
		else
			return 1;			// spear head
	}


	// sometimes...do spear head..or...crazy dive at ball carrier.
	if (ppdata->throw_target == game_info.ball_carrier)
	{
		if (randrng(100) < 55)
			return 0;			// no spear head
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
int rotate_torso(obj_3d *pobj, int angle, int flag)
{
fbplyr_data		*ppdata = (fbplyr_data *)pobj;
fbplyr_data		*pplyr;
int				i,pnum,brng;
float			plyr_dist,x,z,dist;
obj_3d 			*o1;
obj_3d 			*o2;

	if (angle !=0)
	{
		pobj->fwd2 = pobj->fwd;
		pobj->tgt_fwd2 = pobj->fwd;
		return 0;
	}

	pnum = -1;


	plyr_dist = 1000;
	// find closet player on opposing team...and rotate torso toward him
	for (i=0; i < NUM_PLAYERS; i++)
	{
		// dont check myself
		if (ppdata->plyrnum == i)
			continue;

		pplyr = &player_blocks[i];

		// dont check man IM dragging
		if ((pobj->flags & PF_DRAGGING_PLR) &&
			((ppdata->puppet_link == pplyr) && (pplyr->puppet_link == ppdata)))
			continue;

		// ignore if on same team
		if (ppdata->team == pplyr->team)
			continue;

		// ignore players reacting to moves.
		if (pplyr->odata.adata[0].animode & MODE_REACTING)
			continue;

		// Instead of just checking distance... take into account where the
		// opponent will be in 7 ticks (according to stiff_arm_anim)

		// o1 is plyr, o2 is i
		o1 = &player_blocks[ppdata->plyrnum].odata;
		o2 = &player_blocks[i].odata;
		x = o2->x + (o2->vx * 7.0f);
		z = o2->z + (o2->vz * 7.0f);
		dist = fsqrt((o2->x - o1->x) * (o2->x - o1->x) + (o2->z - o1->z) * (o2->z - o1->z));
		if (dist < plyr_dist)

//		// get distance of opponent to me
//		if (distxz[ppdata->plyrnum][i]/YD2UN < plyr_dist)
		{
//			plyr_dist = distxz[ppdata->plyrnum][i]/YD2UN;
			plyr_dist = dist;
			pnum = i;
		}

	}

	// If guy is 5 yards behind or in front, allow it
	if (FIELDX(pobj->x)-3 <= FIELDX(player_blocks[pnum].odata.x))
	{
		brng = (1024 + pobj->fwd - p2p_angles[ppdata->plyrnum][pnum]) % 1024;
		brng = 1024 - brng;

		// test for maybe doing the head plow move
		if ((flag) &&
			(randrng(100) < 35) &&
			((game_info.off_side ? -pobj->vx : pobj->vx) > 0.0))
		{
			if (brng <= 64)
				return 1;
			if (brng >= 960)
				return 1;
		}

		// stiff arm
		if ((brng >= 192) && (brng <= 512))
			brng = 192;
		if ((brng >= 513) && (brng <= 832))
			brng = 832;
		pobj->fwd2 += brng + 13.0f;			// add fudge
		pobj->tgt_fwd2 = pobj->fwd2;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
#if 0
void change_to_attacker_fwd(obj_3d *pobj)
{
	pobj->fwd = pobj->pobj_hit->fwd;
	pobj->tgt_fwd = pobj->fwd;
}
#endif

//////////////////////////////////////////////////////////////////////////////
int	slower_spin(obj_3d *pobj)
{
fbplyr_data		*ppdata = (fbplyr_data *)pobj;

	if (game_info.spin_cnt[ppdata->team] > 2)
		return 1;
	else
		return 0;
}


//////////////////////////////////////////////////////////////////////////////
void set_plyr_y_pos(obj_3d *pobj)
{
	pobj->y = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
int is_plr_on_grnd(obj_3d *pobj)
{
	fbplyr_data		*ppdata = (fbplyr_data *)pobj;

	if ((player_blocks[ppdata->throw_target].odata.y + player_blocks[ppdata->throw_target].odata.ay) < 2.25f)
		return 1;			// player is on lying on the ground
	else
		return 0;			// player isnt lying down on ground
}
#endif

//////////////////////////////////////////////////////////////////////////////
#if 0
int	maybe_dizzy(obj_3d *pobj)
{

#ifdef GETUPAGAIN
		return 0;			// get up
#endif

	// have player get up...if tackled in end-zone
//	if (in_end_zone(pobj) == -1)
//		return FALSE;

	// a non-ball-carrier weapon...dont make dizzy!
	if ((((fbplyr_data *)pobj)->plyrnum != game_info.ball_carrier) &&
		(pobj->plyrmode == MODE_WPN))
		return FALSE;

	// dont allow QB to become dizzy
	if (pobj->plyrmode == MODE_QB)
		return FALSE;

	if (pobj->plyrmode == MODE_DB)
	{
		// different percentaging for defensive backs
		if (randrng(100) < 25)
			return TRUE;			// get up for dizzy
		else
			return FALSE;			// dont get up
	}
	else
	{
		if (randrng(100) < 20)
			return TRUE;			// get up for dizzy
		else
			return FALSE;			// dont get up
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
void rotate_to_ball(obj_3d *pobj, int trate)
{
int				angle,brng;

	if (game_info.ball_carrier != -1)
		angle = ptangle( &(pobj->x), &(ball_obj.odata.x) );
	else
		angle = ptangle( &(pobj->x), &(ball_obj.ox) );

	brng = (1024 + pobj->fwd - angle) % 1024;
	if( brng > 512 )
		brng = 1024 - brng;

	pobj->tgt_fwd = angle;
	pobj->turn_rate = brng/trate;
}

//////////////////////////////////////////////////////////////////////////////
// return distance from a to b, weighted by brng such that if b is bearing
// 90 degrees, effective distance is tripled, and return a negative value if
// bearing is greater than 90.
static float weighted_dist( fbplyr_data *a, fbplyr_data *b )
{
	int		me,him;
	int		brng;
	float	dist;
	float	brng_factor;

	me = a->plyrnum;
	him = b->plyrnum;
	brng = (1024 + a->odata.tgt_fwd - p2p_angles[me][him]) % 1024;
//	brng = (1024 + a->odata.fwd - p2p_angles[me][him]) % 1024;
	if( brng > 512 )
		brng = 1024 - brng;

	if( brng > 256 )
		return -1.0f;

	brng_factor = 1.0f + (float)brng / 256.0f * 2.0f;

	dist = distxz[me][him];

	dist *= brng_factor;


	return dist;
}

//////////////////////////////////////////////////////////////////////////////
void maybe_dive( obj_3d *pobj )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	if (ppdata->non_buta_time == -1)
	{
//		if (game_info.game_flags & GF_BALL_CROSS_LOS)
		if (ppdata->plyrnum == (game_info.off_side ? 10 : 3))
			change_anim( pobj, qb_slide_anim );			// only true qb... does slide
		else
		{
		 	pobj->flags &=  ~PF_FLIP;		// ball should always be in right hand for this anim.
		 	change_anim( pobj, r_diveoh_anim );
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
int is_inplay( obj_3d *pobj )
{
	if( game_info.game_mode == GM_IN_PLAY )
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
int is_ball_kicked( obj_3d *pobj )
{
	if(( ball_obj.odata.y > 2.0f ) &&
		( game_info.ball_carrier == -1 ) &&
		( ball_obj.odata.vy > 0.0f ) &&
		( ball_obj.type == LB_KICKOFF ))
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
int carrier_bearing_le( obj_3d *pobj, int angle)
{
	int		me,him;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	me = ppdata->plyrnum;
	him = game_info.ball_carrier;

	if( him == -1 )
		return FALSE;

	if( bearing( me, him ) <= angle )
		return TRUE;
	else
		return FALSE;
}
#endif
//////////////////////////////////////////////////////////////////////////////
// return 1 if player's face-relative Z velocity is less than zero, else 0.
#if 0
int moving_backwards( obj_3d *pobj )
{
	float	frelz;

	frelz = FRELZ(pobj->vx,pobj->vz,(1024 - pobj->fwd));

	if( frelz < 0.0f )
		fprintf( stderr, "Backwards." );

	if( frelz < 0.0f )
		return 1;
	else
		return 0;
}
#endif
//////////////////////////////////////////////////////////////////////////////
#if 0
void face_velocity( obj_3d *pobj, int offset )
{
	float		newpt[3];
	int			angle;

	newpt[0] = pobj->x + pobj->vx;
	newpt[2] = pobj->z + pobj->vz;

	angle = ptangle( &(pobj->x), newpt );

	angle = (angle + offset + 1024) % 1024;

	pobj->tgt_fwd = angle;
}
#endif
//////////////////////////////////////////////////////////////////////////////
void release_puppet( obj_3d *pobj, int flag )
{
	// either puppeteer or puppet can do this.
	// clears both ways and writes tick stamp.  If your link
	// is clear when you hit this, check stamp to make sure it
	// happened this tick, and yell about synch error if it's not.
	fbplyr_data		*ppdata = (fbplyr_data *)pobj;
	fbplyr_data		*pvdata = ppdata->puppet_link;

	if( pvdata == NULL )
	{
		// other guy released us.  Make sure it happened this tick.
		if( pcount != ppdata->puppet_rel_time )
		{
#ifdef DEBUG
			fprintf( stderr, "release_puppet synch error.  Released on %d, "
					"hit release on %d\n", ppdata->puppet_rel_time, pcount );
			fprintf( stderr, "Caller in seq:frame %d:%d\n",
					pobj->adata[0].seq_index, pobj->adata[0].iframe );
#endif
		}

		return;
	}

	ppdata->puppet_link = NULL;
	pvdata->puppet_link = NULL;
	pvdata->puppet_rel_time = pcount;

	ppdata->odata.adata[0].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);
	pvdata->odata.adata[0].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);

	if( !flag )
	{
		ppdata->odata.vx = 0.0f;
		ppdata->odata.vy = 0.0f;
		ppdata->odata.vz = 0.0f;

		pvdata->odata.vx = 0.0f;
		pvdata->odata.vy = 0.0f;
		pvdata->odata.vz = 0.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////
void qb_release_ball( obj_3d *pobj )
{
	int			target_wr, i;
	obj_3d		*ptgt;
	fbplyr_data	*ppdata;

	ppdata = (fbplyr_data *)pobj;

	assert( game_info.ball_carrier == ppdata->plyrnum );

	pobj->flags &= ~PF_THROWING;

	game_info.ball_carrier = -1;

	// if we're human and target_human is TRUE and there's a human receiver, target him.
	if ((ISHUMAN(ppdata)) && (target_human))
	{
		for( i = ppdata->team * 7 + 3; i < ppdata->team * 7 + 7; i++ )
		{
			ptgt = (obj_3d *)(player_blocks + i);

			// skip non-humans
			if( player_blocks[i].station == -1 )
				continue;

			// skip non-weapons
			if( ptgt->plyrmode != MODE_WPN )
				continue;

			// skip self
			if( ptgt == pobj )
				continue;

			// skip target
			if( i == ppdata->throw_target )
				continue;

			// got one
			ppdata->throw_target = i;
		}
	}


#if 1	//(DHS)
	if (ppdata->throw_target==-1)
	{
		// problem we have no receiver to throw the ball to, find any weapon

		int j;
		
		i=ppdata->team * 7 + 3;
		j=i+7;
		do
		{
			ptgt = (obj_3d *)(player_blocks + i);

			// skip non-weapons
			if( ptgt->plyrmode != MODE_WPN )
				continue;

			// skip self
			if( ptgt == pobj )
				continue;

			// skip target
			if( i == ppdata->throw_target )
				continue;

			// got one
			ppdata->throw_target = i;
		}
		while(++i<j);
	}
#endif

	target_wr = ppdata->throw_target;
	ball_obj.int_receiver = target_wr;
	ball_obj.type = LB_PASS;
	ball_obj.flags &= ~(BF_FG_BOUNCE|BF_FG_HITPOST);
	ball_obj.flags |= BF_VEL_PITCH;

	// if team on fire, do fire pass
#ifndef NO_FIRE
	if (game_info.team_fire[((fbplyr_data *)pobj)->team] == 1)
		ball_obj.flags |= BF_FIRE_PASS;
#endif

	// tell ball who threw him
	ball_obj.who_threw = ((fbplyr_data *)pobj)->plyrnum;

	ptgt = ((obj_3d *)(player_blocks + target_wr));

	set_ball_vels( pobj, ptgt );

	game_info.game_flags |= GF_PASS_THROWN;

	pobj->plyrmode = MODE_WPN;

	// inc stat
	if( FIELDX(ball_obj.tx) >= game_info.los )
		tdata_blocks[game_info.off_side].passes++;

	throw_ball_sounds();
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
int unique_random(int range, int control)
{
	int value;
	int done;
	int cnt;

	done = 0;
	cnt = 0;
	while (!done)
	{
		value = randrng(range);
		done = 1;
		if (control & (1<<value))
			done = 0;

		cnt +=1;
		if (cnt > (range<<1))	// find something 2*range cycles or quit
		{
			value = -1;
			for (cnt = 0; cnt < range; cnt++)
			{
				if (!(control & (1<<cnt)))
				{
					value = cnt;
					break;
				}
			}
			#if 0
			if (value == -1)
				fprintf(stderr, "can't find unique numbah! decided on DEFAULT!!!");
			else
				fprintf(stderr, "can't find unique numbah! decided on [%d / %d]\n", value, range);
			fflush(stdout);
			#endif

			if (value == -1)
			{
				value = randrng(range);
#if 0
			fprintf(stderr, " [%d / %d]\n", value, range);
#endif

			}

			done = 1;
		}
	}
	return (value);
}

//////////////////////////////////////////////////////////////////////////////
unsigned int db_control = 0;
unsigned int db_zone_control = 0;
unsigned int db_m2m_control = 0 ;

void pick_random_db_m2m_anim(obj_3d *pobj)
{
	int	rnd;
	fbplyr_data			*ppdata;

	ppdata = (fbplyr_data *)pobj;
	rnd  = randrng(100);
//	if (0)	
	if (game_info.team_fire[!game_info.off_side])
	{
		rnd = randrng(sizeof(blood_lust_tbl)/sizeof(char *));
		change_anim(pobj, blood_lust_tbl[rnd]);
	}
	else
	{
		{
			rnd = randrng(100);
			if (rnd < 40)
			{
				rnd = (sizeof(man_2_man_tbl)/sizeof(char *));
//fprintf(stderr, "B> table size = %d\n", rnd);
				rnd = unique_random( rnd, db_m2m_control);
				if (!(db_m2m_control & (1<<rnd)) )
					db_m2m_control |= (1<<rnd);
				change_anim(pobj, man_2_man_tbl[rnd]);
			} else {
//fprintf(stderr, "C> table size = %d\n", sizeof(db_anims_tbl)/sizeof(char *));
				rnd = unique_random((sizeof(db_anims_tbl)/sizeof(char *)), db_control);
				if (!(db_control & (1<<rnd)))
					db_control |= (1<<rnd);
				change_anim(pobj, db_anims_tbl[rnd]);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void pick_random_db_zone_anim(obj_3d *pobj)
{
	int	rnd;
	fbplyr_data			*ppdata;

	ppdata = (fbplyr_data *)pobj;
	rnd  = randrng(100);

//	if (0)	
	if (game_info.team_fire[!game_info.off_side])
	{
		rnd = randrng(sizeof(blood_lust_tbl)/sizeof(char *));
		change_anim(pobj, blood_lust_tbl[rnd]);
	}
	else
	{
//		if (ISHUMAN(ppdata))
//		{
//			change_anim(pobj, safty2pt_anim_rnd);
//		} else {
		{
			rnd = randrng(100);
			if (rnd < 80)
			{
				rnd = (sizeof(db_zone_shift_tbl)/sizeof(char *));
//				change_anim(pobj, db_zone_shift_tbl[rnd]);
//fprintf(stderr, "D> table size = %d\n", rnd);
				rnd = unique_random( rnd, db_zone_control);
				if (!(db_zone_control & (1<<rnd)))
					db_zone_control |= (1<<rnd);
				change_anim(pobj, db_zone_shift_tbl[rnd]);
			} else {
//				change_anim(pobj, db_anims_tbl[0]);
//fprintf(stderr, "E> table size = %d\n", sizeof(db_anims_tbl)/sizeof(char *));
				rnd = unique_random((sizeof(db_anims_tbl)/sizeof(char *)), db_control);
				if (!(db_control & (1<<rnd)))
					db_control |= (1<<rnd);
				change_anim(pobj, db_anims_tbl[rnd]);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void pick_random_db_anim(obj_3d *pobj)
{
	int	rnd;
	fbplyr_data			*ppdata;

	ppdata = (fbplyr_data *)pobj;
	rnd  = randrng(100);
//	if (0)	
	if (game_info.team_fire[!game_info.off_side])
	{
		rnd = randrng(sizeof(blood_lust_tbl)/sizeof(char *));
		change_anim(pobj, blood_lust_tbl[rnd]);
	} else {
		if (ISHUMAN(ppdata))
		{
			change_anim(pobj, safty2pt_anim_rnd);
		} else {
//			change_anim(pobj, db_anims_tbl[0]);
//fprintf(stderr, "F> table size = %d\n", sizeof(db_anims_tbl)/sizeof(char *));
			rnd = unique_random((sizeof(db_anims_tbl)/sizeof(char *)), db_control);
			if (!(db_control & (1<<rnd)))
				db_control |= (1<<rnd);
			change_anim(pobj, db_anims_tbl[rnd]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void pick_random_mill(obj_3d *pobj)
{
	int rnd;

//fprintf(stderr, "G> table size = %d\n", sizeof(breath_seq_tbl)/sizeof(char *));
	rnd = unique_random((sizeof(breath_seq_tbl)/sizeof(char *)), breath_control);
	if (!(breath_control |= (1<<rnd)))
		breath_control |= (1<<rnd);
//	rnd = randrng(sizeof(breath_seq_tbl)/sizeof(char *));
	change_anim(pobj, breath_seq_tbl[rnd]);
}

//////////////////////////////////////////////////////////////////////////////
unsigned int lm_control = 0;

void pick_random_linemen_anim(obj_3d *pobj)
{
	int rnd;
#if 0
	change_anim(pobj, linemen_anims_tbl[randrng(sizeof(linemen_anims_tbl)/sizeof(char *))]);
#else
//fprintf(stderr, "H> table size = %d\n", sizeof(linemen_anims_tbl)/sizeof(char *));
	rnd = unique_random((sizeof(linemen_anims_tbl)/sizeof(char *)), lm_control);
	if (!(lm_control & (1<<rnd)))
		lm_control |= (1<<rnd);
	change_anim(pobj, linemen_anims_tbl[rnd]);
#endif
}

//////////////////////////////////////////////////////////////////////////////
void pick_random_shotgun_qb_anim(obj_3d *pobj)
{
	change_anim(pobj, qb_shotgun_anims_tbl[randrng(sizeof(qb_shotgun_anims_tbl)/sizeof(char *))]);
}

//////////////////////////////////////////////////////////////////////////////
void pick_random_qb_anim(obj_3d *pobj)
{
	change_anim(pobj, qb_anims_tbl[randrng(sizeof(qb_anims_tbl)/sizeof(char *))]);
}

//////////////////////////////////////////////////////////////////////////////
unsigned int wr_control = 0;

void pick_random_wr_anim(obj_3d *pobj)
{
	int rnd;

#if 1
//fprintf(stderr, "I> table size = %d\n", sizeof(wr_anims_tbl)/sizeof(char *));
	rnd = unique_random((sizeof(wr_anims_tbl)/sizeof(char *)), wr_control);
	if (!(wr_control & (1<<rnd)))
		wr_control |= (1<<rnd);
	change_anim(pobj, wr_anims_tbl[rnd]);

#else
	change_anim(pobj, wr_anims_tbl[randrng(sizeof(wr_anims_tbl)/sizeof(char *))]);
#endif
}

//////////////////////////////////////////////////////////////////////////////
void drop_ball( obj_3d *pobj )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	float	rx,ry,rz, *tm;
//	float	bx,bz;

//	bx = (ppdata->odata.vx / 2.0f) / 2.0f;
//	bz = (ppdata->odata.vz / 2.0f) / 2.0f;

	if (ball_obj.flags & BF_PHANTOM)		// If we have a ball already phantom'ed, don't do this again!
		return;

	// start point
	rx = ppdata->jpos[(pobj->flags & PF_FLIP) ? JOINT_LWRIST : JOINT_RWRIST][0];
	ry = ppdata->jpos[(pobj->flags & PF_FLIP) ? JOINT_LWRIST : JOINT_RWRIST][1];
	rz = ppdata->jpos[(pobj->flags & PF_FLIP) ? JOINT_LWRIST : JOINT_RWRIST][2];

	tm = cambot.xform;

	ball_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	ball_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	ball_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	ball_obj.odata.vx = ((float)randrng(3)) * 0.15f;			// 0.10f
	ball_obj.odata.vz = ((float)randrng(3)) * 0.15f;			// 0.10f
	ball_obj.odata.vy = .43f;
	ball_obj.flight_time = 0;
	ball_obj.int_receiver = -1;
//	ball_obj.who_threw = ppdata->plyrnum;
	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|BF_FG_BOUNCE|BF_FG_HITPOST);
//	game_info.ball_carrier = -1;
	ball_obj.flags |= BF_PHANTOM;
	ball_obj.type = LB_FUMBLE;


//	ball_obj.flags &= ~BF_VEL_PITCH;
//	ball_obj.pitch_rate = 60;

	// if qb had ball... put him in mode weapon ?
	pobj->plyrmode = MODE_WPN;
}


//////////////////////////////////////////////////////////////////////////////
void lateral_ball( obj_3d *pobj )
{
	obj_3d	*ptgt;

	game_info.ball_carrier = -1;

	ball_obj.int_receiver = ((fbplyr_data *)pobj)->throw_target;
	ball_obj.type = LB_LATERAL;
	ball_obj.flags &= ~BF_VEL_PITCH;
	ball_obj.pitch_rate = 60;

	ball_obj.who_threw = ((fbplyr_data *)pobj)->plyrnum;

	game_info.fumble_x = pobj->x;

	ptgt = ((obj_3d *)(player_blocks + ball_obj.int_receiver));

	set_ball_vels2(pobj, ptgt);		// thrower, receiver

	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|BF_FG_BOUNCE|BF_FG_HITPOST);

	pobj->plyrmode = MODE_WPN;
}

//////////////////////////////////////////////////////////////////////////////
void hike_ball(obj_3d *pobj)
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	// free ball
	game_info.ball_carrier = -1;

	// change game mode...even though in some cases the qb doesnt have the ball...but its on the way!
	game_info.game_mode = GM_IN_PLAY;

	// mark qb as intended receiver
	ball_obj.int_receiver = (ppdata->team ? 10 : 3);

	ball_obj.type = LB_LATERAL;
	ball_obj.flags |= BF_VEL_PITCH;
	ball_obj.who_threw = ppdata->plyrnum;		// center linemen

	game_info.fumble_x = pobj->x;

	set_hike_ball_vels(pobj,(obj_3d *)(player_blocks + ball_obj.int_receiver));		// center, qb

	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|BF_FG_BOUNCE|BF_FG_HITPOST);
}


//////////////////////////////////////////////////////////////////////////////
void spike_ball(obj_3d *pobj, int type)
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	game_info.ball_carrier = -1;
	if (type == 0)
		ball_obj.flags |= BF_VEL_PITCH;

	ball_obj.who_threw = ppdata->plyrnum;		// center linemen
	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|GF_PASS_THROWN|BF_FG_BOUNCE|BF_FG_HITPOST);

	set_spike_ball_vels(pobj,type);
}

//////////////////////////////////////////////////////////////////////////////
void spin_ball(obj_3d *pobj)
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	float		rx,ry,rz, *tm;


//	ball_obj.who_threw = ppdata->plyrnum;
	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|GF_PASS_THROWN|BF_FG_BOUNCE|BF_FG_HITPOST);

	// make ball stand on pointy end... and rotate on the X axis
	ball_obj.flags |= BF_SPINING;
	ball_obj.odata.fwd2 = 0;		// start ball lying on side
	ball_obj.phi = 45;				// spin Y 

	tm = cambot.xform;

	rx = ppdata->jpos[JOINT_RWRIST][0];
	ry = ppdata->jpos[JOINT_RWRIST][1];
	rz = ppdata->jpos[JOINT_RWRIST][2];

	ball_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	ball_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	ball_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

//	ball_obj.odata.x = pobj->x + 3.0f;
//	ball_obj.odata.z = pobj->z + 5.0f;			// put ball by right wrist

	ball_obj.odata.y = 3.0f;
	ball_obj.odata.vx = 0.0f;
	ball_obj.odata.vy = 0.0f;
	ball_obj.odata.vz = 0.0f;

	game_info.ball_carrier = -1;

}

//////////////////////////////////////////////////////////////////////////////
void punt_ball(obj_3d *pobj)
{
	game_info.ball_carrier = -1;
	game_info.fumble_x = pobj->x;
	game_info.game_flags |= GF_BALL_PUNTED;
	game_info.ko_catch = -1;

	ball_obj.who_threw = ((fbplyr_data *)pobj)->plyrnum;
	ball_obj.flags &= ~BF_VEL_PITCH;
	ball_obj.pitch_rate = 60;
	ball_obj.type = LB_PUNT;
	ball_obj.int_receiver = -1;
	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|BF_FG_BOUNCE|BF_FG_HITPOST);

	set_punt_ball_vels(pobj);

	pobj->plyrmode = MODE_DB;
}

//////////////////////////////////////////////////////////////////////////////
void kick_ball(obj_3d *pobj)
{
	game_info.ball_carrier = -1;
	game_info.fumble_x = pobj->x;
	game_info.game_flags |= GF_BALL_KICKED;

	ball_obj.who_threw = ((fbplyr_data *)pobj)->plyrnum;
	ball_obj.flags &= ~BF_VEL_PITCH;
	ball_obj.pitch_rate = 60;
	ball_obj.type = LB_FIELD_GOAL;
	ball_obj.int_receiver = -1;
	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|BF_FG_BOUNCE|BF_FG_HITPOST);

	set_field_goal_ball_vels(pobj);

	pobj->plyrmode = MODE_DB;
}


//////////////////////////////////////////////////////////////////////////////
void maybe_taunt( obj_3d *pobj )
{
	fbplyr_data *pphit = (fbplyr_data *)pobj->pobj_hit;
//	fbplyr_data *ppdata = (fbplyr_data *)pobj->pobj_hit;
	char	*taunts[] = {
		taunt_1_anim,
		taunt_2_anim,
		taunt_3_anim,
		taunt_4_anim,
		m_laugh1_anim
	};

	if (pobj->pobj_hit == NULL)
		return;

	// someone on team is already taunting
	if (taunted)
		return;

	// only taunt if PLAY OVER
	if (!(game_info.game_flags & GF_PLAY_ENDING))
		return;

	// only defensive players can taunt!!
	if (pphit->team != game_info.off_side)
		return;

	// not on kickoffs
	if (game_info.play_type == PT_KICKOFF)
		return;

	// dont taunt if player made first down or greater
	if (over_first_down(pobj->pobj_hit) == 1)
		return;

	// dont taunt on first down or second down (really no reason too)
	if (game_info.down == 1)
		return;

	// dont taunt if change of possession
	if (game_info.game_flags & GF_POSS_CHANGE)
		return;

	// No taunting on a touchdown or successful conversion!
	if ((game_info.play_result == PR_TOUCHDOWN) ||
		(game_info.play_result == PR_CONVERSION))
		return;

// WHOOOPS!!!!
	// No taunting on a touchdown or successful conversion!
//	if ((game_info.play_result == PR_TOUCHDOWN) ||
//		(game_info.play_result == PR_CONVERSION))
//		return;

#if 1
	// Don't taunt the guy if the direction is in between -128 and +128 from my facing dir
	{
		int		me,him;
		int		brng;
		fbplyr_data *ppdata = (fbplyr_data *)pobj;
		fbplyr_data *ppdata2 = (fbplyr_data *)pobj->pobj_hit;

		me = ppdata->plyrnum;
		him = game_info.ball_carrier;

		// face the intended receiver, if no ball carrier
		if ((him == -1) && (ppdata2->plyrnum == ball_obj.int_receiver))
			him = ppdata2->plyrnum;

		brng = bearing(me, him);

		if (brng > 256)
			return;
	}
#endif

	// okay... we can taunt.. check to see if we hit the ball carrier...
	if ((pphit->plyrnum == game_info.ball_carrier) &&
	 	(randrng(100) < (game_info.team_fire[!game_info.off_side] ? 70 : 45)))
	{
		// have defender go into a taunt
		change_anim(pobj, taunts[randrng(sizeof(taunts)/sizeof(char *))]);
		taunted = 1;
	}
	else
	{	// maybe knocked down intended receiver
		if ((pphit->plyrnum == ball_obj.int_receiver) &&
			(game_info.game_flags & GF_PASS_THROWN) &&
			(game_info.ball_carrier == -1) &&
//			!(pobj->flags & PF_DIZZY) &&
			(randrng(100) < 60))
		{
			// have defender go into a taunt
		 	change_anim(pobj, taunts[randrng(sizeof(taunts)/sizeof(char *))]);
		 	taunted = 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
void celebrate( obj_3d *pobj, int time )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	pobj->flags |= PF_CELEBRATE;
	ppdata->celebrate_time = time;

	return;
}

//////////////////////////////////////////////////////////////////////////////
void dust_cloud( obj_3d *pobj, int num )
{
fbplyr_data *ppdata = (fbplyr_data *)pobj;

	// no dust clouds on TURF field
//	if (pup_field == 2)
//		return;

	if (num == 4)
	{
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_RKNEE, 1, 0);
	} else if (num == 3) {
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_LKNEE, 1, 0);
	} else if (num == 1) {
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_LANKLE, 1, 0);
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_RANKLE, 1, 0);
	}
	else if (num == 2)
	{
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_LKNEE, 1, 0);
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_RKNEE, 1, 0);
	}
	else
	{
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_NECK, 1, 0);
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_PELVIS, 1, 0);
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_LKNEE, 1, 0);
		qcreate ("dustcld", SPFX_PID, dust_cloud_proc, ppdata->plyrnum, JOINT_RKNEE, 1, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////
//void blood_spray( obj_3d *pobj, int num_trails, int delay, int limit)
//{
//fbplyr_data *ppdata = (fbplyr_data *)pobj;
//
//  	qcreate ("pre_bld", SPFX_PID, pre_blood_spray_proc, ppdata->plyrnum, num_trails, delay, limit);
//}


//////////////////////////////////////////////////////////////////////////////
// If this guy has the ball, he's down and the play is over.
void player_down( obj_3d *pobj )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

#ifdef TM	// test tackles
	return;
#endif

	if( game_info.game_mode != GM_IN_PLAY )
		return;

#ifndef NODOWNS
	if( ppdata->plyrnum != game_info.ball_carrier )
		return;

	if (fabs(game_info.play_end_pos) > 600.0f)
		game_info.play_end_pos = WORLDX(-10);

	#ifdef PLAY_DEBUG
	fprintf( stderr, "  player_down wb at %5.2f, field %d\n", pobj->x, FIELDX(pobj->x) );
	#endif
	qcreate("whistle",0, whistle_proc, 0, 0, 0, 0);

	game_info.game_mode = GM_PLAY_OVER;
	game_info.play_end_cause = PEC_CAR_TACKLED_IB;

	// if he's down in bounds, he has control.
	if( game_info.last_pos != game_info.off_side )
	{
		if ((ball_obj.type == LB_BOBBLE) ||
			(ball_obj.type == LB_PASS) ||
			((ball_obj.type == LB_LATERAL) && !(ball_obj.flags & BF_BOUNCE)))
		{
			// pass, bobble, or lateral that hasn't bounced - INTERCEPTION
			tdata_blocks[ppdata->team].interceptions++;
			snd_scall_bank(plyr_bnk_str,INTERCEPT_SND,VOLUME4,127,PRIORITY_LVL5);
			#ifdef PLAY_DEBUG
			fprintf( stderr, "  interception recorded\n" );
			#endif
		}
		else if(( ball_obj.type == LB_FUMBLE ) || ( ball_obj.type == LB_LATERAL ))
		{
			// fumble or lateral that bounced - FUMBLE
			tdata_blocks[!ppdata->team].lost_fumbles++;
			#ifdef PLAY_DEBUG
			fprintf( stderr, "  lost_fumble recorded\n" );
			#endif
		}
		#ifdef PLAY_DEBUG
		fprintf( stderr, "  last_pos goes from %d to %d on controlled down\n",
			game_info.last_pos, game_info.off_side );
		#endif
		game_info.last_pos = game_info.off_side;
		game_info.game_flags |= GF_POSS_CHANGE;
	}

	// if he landed behind the point where he got hit, spot the ball
	// at the point of the hit.
	if( FIELDX( game_info.play_end_pos ) < FIELDX( pobj->x ))
		game_info.play_end_pos = pobj->x;

	// if he got hit in the end zone but wasn't far enough inside
	// that a TD was called, and he's brought down outside the EZ,
	// spot the ball on the 1 yard line.
	if ((FIELDX(game_info.play_end_pos) >= 100) &&
		(FIELDX(pobj->x) < 100 ))
		game_info.play_end_pos = WORLDX(99);

	if( FIELDX( game_info.play_end_pos ) >= 100 )
	{
		game_info.play_end_cause = PEC_CAR_TOUCHDOWN;
		#ifdef PLAY_DEBUG
		fprintf( stderr, "  player_down touchdown!\n" );
		#endif
	}

	if( FIELDX( game_info.play_end_pos ) <= 0 )
		game_info.play_end_cause = PEC_CAR_TACKLED_OWNEZ;

	// FIX!!!  Do smart celebrations
	celebrate( pobj, 60 );
#endif
}

//////////////////////////////////////////////////////////////////////////////
static void whistle_proc(int *args)
{
	if (randrng(100) < 25)
	{
//		sleep (20 + randrng(50));
		sleep(30);
		snd_scall_bank(plyr_bnk_str,WHISTLE1_SND+randrng(2),VOLUME3,127,PRIORITY_LVL4);
	}
}

//////////////////////////////////////////////////////////////////////////////
static int in_bounds( obj_3d *pobj )
{
	if( fabs( pobj->z ) > FIELDW )
		return FALSE;

	if( fabs( pobj->x ) > FIELDHB )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// returns 1 if in enemy end zone, -1 if in own end zone, and 0 if in neither.
int in_end_zone( obj_3d *pobj )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	if( !in_bounds( pobj ))
		return 0;		// out of bounds

	if( fabs( pobj->x ) <= FIELDHZ )
		return 0;		// on regular field

	// in an end zone.  which?
	if((ppdata->team && (pobj->x > 0.0f)) || (!ppdata->team && (pobj->x < 0.0f)))
		return -1;
	else
		return 1;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// returns TRUE if:
//  game mode is IN_PLAY
//  someone's got the ball
//  either they or the ball is over the enemy end zone
//  they've had the ball for CONTROL_TIME or more ticks
int td_check( int control )
{
	if( game_info.game_mode != GM_IN_PLAY )
		return FALSE;

	if( game_info.ball_carrier == -1 )
		return FALSE;

	if(!(control & TDC_NOCONTROL))
		if( pcount - ball_obj.catch_pcount < CONTROL_TIME )
			return FALSE;

	if(!(control & TDC_NOAIRCATCH))
		if( game_info.game_flags & GF_AIR_CATCH )
			return FALSE;
//return TRUE;

	if( over_def_ez( &(ball_obj.odata )))
		return TRUE;

	if( over_def_ez( (obj_3d *)(player_blocks + game_info.ball_carrier )))
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// returns TRUE if pobj is over !game_info.off_side's end zone.
static int over_def_ez( obj_3d *pobj )
{
	float	f;

	f = game_info.off_side ? -1.0f : 1.0f;

	if(( f*pobj->x ) < FIELDHZ+YD2UN )
		return FALSE;

	if(( f*pobj->x ) > FIELDHB )
		return FALSE;

	if( fabs( pobj->z ) > FIELDW )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// returns 1 if going across first down marker
static int over_first_down( obj_3d *pobj )
{
	// out of bounds?
	if( !in_bounds( pobj ))
		return 0;

	if( game_info.first_down >= 100)
		return 0;

	if( game_info.game_flags & GF_POSS_CHANGE )
		return 0;

	if( FIELDX( pobj->x ) >= game_info.first_down )
		return 1;	// Over 1st down

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// returns 1 if near own endzone
int near_my_endzone( obj_3d *pobj )
{
	if (FIELDX(pobj->x) <= 4)
		return 1;	// Over 1st down

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// returns 0 if in_bounds, -1 if out of bounds between goal lines, 1 if out of
// bounds at own end zone, 2 if out_of_bounds at enemy end zone.
static int oob_zone( obj_3d *pobj )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	if( in_bounds( pobj ))
		return 0;		// in bounds

	if( fabs( pobj->x ) <= FIELDHZ )
		return -1;		// between goal lines

	// in an end zone.  which?
	if((ppdata->team && (pobj->x > 0.0f)) || (!ppdata->team && (pobj->x < 0.0f)))
		return 1;
	else
		return 2;
}

//////////////////////////////////////////////////////////////////////////////
static void zerovels( obj_3d *pobj )
{
	pobj->vx = 0.0f;
	pobj->vy = 0.0f;
	pobj->vz = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
static void turn_toward_tgt( obj_3d *pobj )
{
	int 	turn_dist;

	turn_dist = pobj->tgt_fwd - pobj->fwd;

	// move toward target direction
	if( turn_dist != 0 )
	{
		if( ABS(turn_dist) <= pobj->turn_rate )
		{
			pobj->fwd = pobj->tgt_fwd;
		}
		else
		{
			if((( turn_dist > -1024 ) && ( turn_dist < -512 )) ||
			   (( turn_dist > 0 ) && ( turn_dist < 513 )))
			{
				pobj->fwd += pobj->turn_rate;
			}
			else
			{
				pobj->fwd -= pobj->turn_rate;
			}
		}
		pobj->fwd &= 1023;
	}

	turn_dist = pobj->tgt_fwd2 - pobj->fwd2;

	// move toward target direction
	if( turn_dist != 0 )
	{
		if( ABS(turn_dist) <= pobj->turn_rate2 )
		{
			pobj->fwd2 = pobj->tgt_fwd2;
		}
		else
		{
			if((( turn_dist > -1024 ) && ( turn_dist < -512 )) ||
			   (( turn_dist > 0 ) && ( turn_dist < 513 )))
			{
				pobj->fwd2 += pobj->turn_rate2;
			}
			else
			{
				pobj->fwd2 -= pobj->turn_rate2;
			}
		}
		pobj->fwd2 &= 1023;
	}

}

//////////////////////////////////////////////////////////////////////////////
// returns -1 if no change
//
static int db_change_player( fbplyr_data *ppdata, int do_change )
{
	obj_3d	*pobj;
	fbplyr_data *pnew;
	float	tx = 0.0f, tz = 0.0f;
	float	best,dist;
	int		new,i;
	int		seeking_player;		// TRUE if target is player, FALSE if location

	// only humans can use this function
	if (ISDRONE(ppdata))
		return -1;

	// find the defensive player who's closest to the action
	if (game_info.ball_carrier != -1)
	{
		// goto ball carrier
		tx = player_blocks[game_info.ball_carrier].odata.x;
		tz = player_blocks[game_info.ball_carrier].odata.z;
		seeking_player = TRUE;
	}
	else
	{
		switch (ball_obj.type)
		{
			case LB_PASS:
			case LB_FIELD_GOAL:
			case LB_KICKOFF:
//				if (ball_obj.int_receiver !=-1)	//(DHS)
//				{
					// goto intended receiver
					tx = player_blocks[ball_obj.int_receiver].odata.x;
					tz = player_blocks[ball_obj.int_receiver].odata.z;
					seeking_player = TRUE;
//				}
//				else
//				{
//					tx = ball_obj.tx;
//					tz = ball_obj.tz;
//					seeking_player = FALSE;
//				}

				break;
			case LB_PUNT:
			case LB_FUMBLE:
			case LB_BOBBLE:
			case LB_LATERAL:
			default:
				// goto ball target
				tx = ball_obj.tx;
				tz = ball_obj.tz;
				seeking_player = FALSE;
				break;
		}
	}

	// if play is over and human in question is offscreen
	if ((game_info.game_mode == GM_PLAY_OVER) &&
		(ppdata->odata.flags & PF_ONSCREEN))
	{
		if (game_info.ball_carrier == -1)
		{
			// closet to human player
			// FIXME!
//			tx = player_blocks[game_info.plyr_control[ppdata->team]].odata.x;
//			tz = player_blocks[game_info.plyr_control[ppdata->team]].odata.z;
			return -1;
		}
		else
		{
			// closet to ball carrier
			tx = player_blocks[game_info.ball_carrier].odata.x;
			tz = player_blocks[game_info.ball_carrier].odata.z;
		}
	}

	// default to new guy
	new = ppdata->plyrnum;
	best = 100000.0f;

	// find new closest
	for( i = 7 * ppdata->team; i < (7 + 7 * ppdata->team); i++ )
	{
		pobj = (obj_3d *)(player_blocks + i);

		// skip unint guys, unless in block move
		if ((pobj->adata[0].seq_index < (SEQ_SPIN_A)) && (pobj->adata[0].animode & MODE_UNINT))
			continue;

		if ((pobj->adata[0].seq_index >= (SEQ_BREATH)) && (pobj->adata[0].animode & MODE_UNINT))
			continue;

		// skip human-controlled guys who AREN'T ME
		if (( player_blocks[i].station != -1 ) && (ppdata->plyrnum != i))
			continue;

		dist = fsqrt((tx-pobj->x)*(tx-pobj->x)+(tz-pobj->z)*(tz-pobj->z));

		if( seeking_player )
		{
			int brng;
			// target is a player, not the ball.  favor those in front of him
			brng = (1024 + (game_info.off_side ? 768 : 256 ) -
					p2p_angles[game_info.ball_carrier][i]) % 1024;
			if( brng > 512 )
				brng = 1024 - brng;

			brng -= 128;
			if( brng < 0 )
				brng = 0;

			dist += dist * ((float)brng * 2.0f / 512.0f);
		}

		if( dist < best )
		{
			best = dist;
			new = i;
		}
	}

	// if new is old, return
	if( ppdata->plyrnum == new )
		return(-1);

	// set new control
	if (do_change)
	{
		obj_3d *pvic;

		pobj = (obj_3d *)(player_blocks + new);
		pnew = (fbplyr_data *)pobj;
		pvic = (obj_3d *)(pnew->puppet_link);

		// change to player in block seq...but then bust em out of it!!!
		if ((pobj->adata[0].seq_index >= (SEQ_SPIN_A)) &&
			(pobj->adata[0].seq_index <= (SEQ_PUSH_BL3)) &&
			((pobj->plyrmode != MODE_LINE) || (randrng(100) < cp_breakout_ht[OFFH])))
		{
			pobj->adata[0].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);
			if( pvic != NULL )
			{
				pvic->adata[0].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);
				((fbplyr_data *)pvic)->puppet_link = NULL;
				change_anim(pvic, h_flailm_anim);
			}

			pnew->puppet_link = NULL;

			standard_run(pobj);
		}

		game_info.plyr_control[ppdata->station] = new;
		update_stations();

		// fake the new guy's B button.
		player_blocks[new].but_cur |= P_B;
	}

	return(0);
}


//////////////////////////////////////////////////////////////////////////////
//static void wpnps_change_player( fbplyr_data *ppdata )
//{
//	int		old,new,station;
//
//	if( ISDRONE(ppdata) )
//		return;
//
//	// just move to the next receiver
//	station = ppdata->station;
//	old = ppdata->plyrnum;
//	new = old + 1;
//	if( (new % 7) == 0 )
//		new -= 3;
//
//	// fake the new guy's B button
//	player_blocks[new].but_cur |= P_B;
//
//	game_info.plyr_control[station] = new;
//	update_stations();
//}

//////////////////////////////////////////////////////////////////////////////
// returns true if a change took place
static int wpn_change_player( fbplyr_data *ppdata )
{
	obj_3d	*pobj;
	float	tx, tz;
	float	best,dist;
	int		new,i;

	// ignore this if I'm a drone
	if (ISDRONE(ppdata))
		return 0;

	// ignore this if I'm carrying the ball
	if (game_info.ball_carrier == ppdata->plyrnum)
		return 0;

	// ignore this if the ball is being hiked
	if (game_info.game_flags & GF_HIKING_BALL)
		return 0;

	// ignore this if I'm the intended receiver
	if ((game_info.ball_carrier == -1) &&
		((ball_obj.type == LB_PASS) ||
		(ball_obj.type == LB_BOBBLE)) &&
		(ball_obj.int_receiver == ppdata->plyrnum))
		return 0;

	// ignore this if the ball is in play and a QB is holding it
	if ((game_info.game_mode == GM_IN_PLAY) &&
		(game_info.ball_carrier != -1) &&
		(player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_QB ))
		return 0;

	// ignore this if the ball is in flight because we threw it
//	if ((game_info.ball_carrier == -1) &&
//		((ball_obj.type == LB_PASS) || (ball_obj.type == LB_BOBBLE)) &&
//		(ball_obj.who_threw == ppdata->plyrnum))
//		return 0;

	// find the offensive player who's closest to the ball,
	// or to the X if the ball is in flight (or fumbled).
	if( game_info.ball_carrier == -1)
	{
		tx = ball_obj.tx;
		tz = ball_obj.tz;
	}
	else
	{
		tx = player_blocks[game_info.ball_carrier].odata.x;
		tz = player_blocks[game_info.ball_carrier].odata.z;
	}

	// default to current
	new = ppdata->plyrnum;
	best = 100000.0f;

	// find new closest
	for( i = 7 * ppdata->team; i < (7 + 7 * ppdata->team); i++ )
	{
		pobj = (obj_3d *)(player_blocks + i);

		// skip unint guys, unless in block move
		if( pobj->adata[0].animode & MODE_UNINT )
			continue;

		// skip intended receiver
//		if ((ball_obj.type == LB_PASS) &&
//			(game_info.ball_carrier == -1) &&
//			(ball_obj.int_receiver == i))
//			continue;

		// skip humans who aren't me
		if ((ISHUMAN((fbplyr_data *)pobj)) &&
			(i != ppdata->plyrnum))
			continue;

		dist = fsqrt((tx-pobj->x)*(tx-pobj->x)+(tz-pobj->z)*(tz-pobj->z));

		if( dist < best )
		{
			best = dist;
			new = i;
		}
	}

	// if new is old, return
	if( ppdata->plyrnum == new )
		return 0;

	// set new control
	game_info.plyr_control[ppdata->station] = new;
	update_stations();

	// fake the new guy's B button.
	player_blocks[new].but_cur |= P_B;

	// abort any wave on the new guy
	player_blocks[new].odata.flags &= ~PF_WAVE_ARM;

	return 1;
}


//////////////////////////////////////////////////////////////////////////////
// Stuff that needs to happen before any player processes execute.
void pre_player_proc( int *parg )
{
	int i;

	while( 1 )
	{
		// clear anitick bits
		for( i = 0; i < 14; i++ )
		{
			player_blocks[i].odata.flags &= ~PF_ANITICK;
		}

		// do some per-tick drone stuff
		drone_update();

		sleep( 1 );
	}
}

//////////////////////////////////////////////////////////////////////////////
// Various player bookkeeping.  Have this go off every tick AFTER all the
// player processes, but before the collision checks.
//////////////////////////////////////////////////////////////////////////////
#define BIGHEAD_SCALE	2.0f
#define HUGEHEAD_SCALE	3.0f

#define FOCAL_LENGTH	1.3f
#define EDGE_BUFFER		120.0f
//#define EDGE_BUFFER		400.0f
//#define FRONT_PLANE		25.0f
//#define FRONT_PLANE		26.0f
#define FRONT_PLANE		28.0f

void last_player_proc( int *parg )
{
	fbplyr_data	*ppdata;
	obj_3d		*pobj;
	float		*cam = cambot.xform;
	float		mat[12],cm[12],cm2[12];
	float		rx,ry,rz,x,y,z,xf,yf;
	int			i,onscreen;

	while( 1 )
	{
		// Reinit to assume controlled players will be drones; gets
		//  set in the <for> loop if humans are found
		arrowinfo[0].pnum = -1;
		arrowinfo[1].pnum = -1;
		arrowinfo[2].pnum = -1;
		arrowinfo[3].pnum = -1;

		ppdata = player_blocks;

		// flag onscreen players
		onscreen = 0;
		for( i = 0; i < 14; i++ )
		{
			pobj = (obj_3d *)ppdata;

			rx = pobj->x - cambot.x;
			ry = pobj->y + pobj->ay - cambot.y;
			rz = pobj->z - cambot.z;

			// compute screen coords of player center
			x = cam[0]*rx+cam[1]*ry+cam[2]*rz+cam[3];
			y = cam[4]*rx+cam[5]*ry+cam[6]*rz+cam[7];
			z = cam[8]*rx+cam[9]*ry+cam[10]*rz+cam[11];

			xf = z*((0.5f + EDGE_BUFFER/hres)/FOCAL_LENGTH);
			yf = z*((vres+2.0f*EDGE_BUFFER)/(2.0f*hres*FOCAL_LENGTH));

			// set or clear onscreen bit
			if(( z < FRONT_PLANE ) || ( fabs(x) > xf ) || ( fabs(y) > yf ))
			{
				pobj->flags &= ~PF_ONSCREEN;
			}
			else
			{
				pobj->flags |= PF_ONSCREEN;
				onscreen++;
			}

#ifdef DEBUG
			// verify that all quats are normalized
			{
				int j;
				for( j = 0; j < 15; j++ )
				{
					float *fp = pobj->adata[0].pq + j*4;
					float   n = fsqrt( fp[0]*fp[0] + fp[1]*fp[1] + fp[2]*fp[2] + fp[3]*fp[3] );
					if( fabs( n - 1.0f ) > 0.15f )	//0.15f ???
					{
						fprintf( stderr, "Non-normalized quat on last_player %d limb %d\r\n", i, j );
						fprintf( stderr, "%f %f %f %f - mag %f\r\n", fp[0], fp[1], fp[2], fp[3], n );
						fprintf( stderr, "seq_index:%s  seq_index2:%s\r\n"
										 "iframe:%d   icnt:%d   icnt(2):%d   flags:%X\r\n",
										 seq_names[pobj->adata[0].seq_index], seq_names[pobj->adata[0].seq_index2],
										 pobj->adata[0].iframe, pobj->adata[0].icnt, pobj->adata[2].icnt, pobj->flags);
						dump_anim_data( 0 );
						lockup();
					}
//					if( pobj->adata[1].animode & MODE_TWOPART )
//					{
//						fp = pobj->adata[1].pq;
//						n = fsqrt( fp[0]*fp[0] + fp[1]*fp[1] + fp[2]*fp[2] + fp[3]*fp[3] );
//						if( fabs( n - 1.0f ) > 0.05 )
//						{
//							fprintf( stderr, "Non-normalized quat on %s:%d:%d (part 2)\n",
//									seq_names[pobj->adata[1].seq_index],
//									pobj->adata[1].iframe, j );
//							fprintf( stderr, "%f %f %f %f\n", fp[0], fp[1], fp[2], fp[3] );
//							dump_anim_data( 0 );
//N_RPLAY							write_replay_data();
//							while( 1 );
//						}
//					}
				}
			}
#endif

			// onscreen or not, we need the matrices for collision checking
			roty( pobj->fwd, mat );
			mat[3] = pobj->x - cambot.x;
			mat[7] = pobj->y + pobj->ay - cambot.y;
			mat[11] = pobj->z - cambot.z;
			mxm( cambot.xform, mat, cm );
			cm[0] *= pobj->ascale; cm[1] *= pobj->ascale; cm[2] *= pobj->ascale;
			cm[4] *= pobj->ascale; cm[5] *= pobj->ascale; cm[6] *= pobj->ascale;
			cm[8] *= pobj->ascale; cm[9] *= pobj->ascale; cm[10] *= pobj->ascale;

			// do big head logic
			if ((ISHUMANTEAM(ppdata)) &&
				(pup_bigheadteam & TEAM_MASK(ppdata->team)))
				bighead_scale =	BIGHEAD_SCALE;
			else
				bighead_scale = 0.0f;

			if (ISHUMAN(ppdata))
			{
				if (pup_bighead & (1<<ppdata->station))
					bighead_scale = BIGHEAD_SCALE;
				if (pup_hugehead & (1<<ppdata->station))
					bighead_scale = HUGEHEAD_SCALE;
			}

			if( pobj->adata[1].animode & MODE_TWOPART )
			{
				// do top half first so bottom overwrites pelvis matrix (top and bottom
				// each have a pelvis, but only the bottom's pelvis is displayed.)
				roty( pobj->fwd2, mat );
				mxm( cambot.xform, mat, cm2 );
				cm2[0] *= pobj->ascale; cm2[1] *= pobj->ascale; cm2[2] *= pobj->ascale;
				cm2[4] *= pobj->ascale; cm2[5] *= pobj->ascale; cm2[6] *= pobj->ascale;
				cm2[8] *= pobj->ascale; cm2[9] *= pobj->ascale; cm2[10] *= pobj->ascale;

				generate_matrices( tophalf_skel, cm2, pobj->adata[1].pq, ppdata->matrices[0], ppdata->jpos[0] );
				generate_matrices( bothalf_skel, cm,  pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0] );
			}
			else
			{
				generate_matrices( fbplyr_skel, cm, pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0] );
			}

//N_RPLAY
#if 0
			// make sure all the matrices are normalized
			for( j = 0; j < 45; j++ )
			{
				float	*fp, n, *qp;
				int		limb;

				fp = (float *)ppdata->matrices + j*4;

				limb = j / 3;

				n = fsqrt(fp[0]*fp[0]+fp[1]*fp[1]+fp[2]*fp[2]);

				if( fabs(1.0f - n) > 0.25 )
				{
					fprintf( stderr, "
					non-normalized matrix line %f %f %f, mag %f\r\n",
							fp[0], fp[1], fp[2], n );
					fprintf( stderr, "%s:%d:%d\r\n", seq_names[pobj->adata[0].seq_index],
						pobj->adata[0].iframe, limb );
					qp = pobj->adata[0].pq + 4*limb;
					fprintf( stderr, "src quat: %9f %9f %9f %9f\r\n",
						qp[0], qp[1], qp[2], qp[3] );

					if( pobj->adata[1].animode & MODE_TWOPART )
					{
						fprintf( stderr, "second part\r\n" );
						qp = pobj->adata[1].pq + 4*limb;
						fprintf( stderr, "src quat: %9f %9f %9f %9f\r\n",
						qp[0], qp[1], qp[2], qp[3] );
					}
					dump_anim_data( 0 );
					write_replay_data();
					lockup();
				}
			}
#endif

			// compute screen coords of player center on ground
			ry = -cambot.y;
			ppdata->tsx = x = cam[0]*rx+cam[1]*ry+cam[2]*rz+cam[3];
			ppdata->tsy = y = cam[4]*rx+cam[5]*ry+cam[6]*rz+cam[7];
			ppdata->tsz = z = cam[8]*rx+cam[9]*ry+cam[10]*rz+cam[11];

			{
				register int idx = -1;
				if (!four_plr_ver &&
					ball_obj.type == LB_PASS &&
					game_info.ball_carrier < 0 &&
					game_info.game_mode == GM_IN_PLAY &&
					game_info.off_side == ppdata->team)
				{
					if (i == ball_obj.int_receiver)
						idx = ppdata->team;
				}
				else
				if (game_info.plyr_control[0] == i) idx = 0;
				else
				if (game_info.plyr_control[1] == i) idx = 1;
				else
				if (game_info.plyr_control[2] == i) idx = 2;
				else
				if (game_info.plyr_control[3] == i) idx = 3;

				if (idx != -1)
				{
					arrowinfo[idx].ppdata  = ppdata;
					arrowinfo[idx].plyr_sx = x;
					arrowinfo[idx].plyr_sy = y;
					arrowinfo[idx].plyr_sz = z;
					arrowinfo[idx].pnum    = i;
					// Set onscreen flag per player
					if (pobj->flags & PF_ONSCREEN)
						arrowinfo[idx].ai_flags |=  AF_ONSCREEN;
					else
						arrowinfo[idx].ai_flags &= ~AF_ONSCREEN;
				}
			}

			if( fabs(z) < 0.0001 )
				z = 0.0001;

			ppdata->sx = -(x/z)*hres + hres*0.5f;
			ppdata->sy = (y/z)*hres + vres*0.5f;
			ppdata->sz = z;

			ppdata++;
		}

		for( i = 0; i < 14; i++ )
		{
			player_blocks[i].odata.flags &= ~PF_HICOUNT;

//			if( i == game_info.ball_carrier )
//				player_blocks[i].odata.flags |= PF_HICOUNT;

//			if( game_info.plyr_control[i/7] == i )
//				player_blocks[i].odata.flags |= PF_HICOUNT;
		}

		pftimer_end( PRF_LASTPLAYER );
		sleep( 1 );
		pftimer_start( PRF_LASTPLAYER );

	}
}

//////////////////////////////////////////////////////////////////////////////
// player draw functions
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// call this one before any players get drawn
//////////////////////////////////////////////////////////////////////////////
static void player_pre_draw( void *oi )
{
	// init glide states
	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_LESS );
	grDepthMask( FXTRUE );
	grDepthBiasLevel( 0 );
	guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
//	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
//			GR_BLEND_ONE_MINUS_SRC_ALPHA,
//			GR_BLEND_SRC_ALPHA,
//			GR_BLEND_ONE_MINUS_SRC_ALPHA );
	grAlphaBlendFunction( GR_BLEND_ONE,
			GR_BLEND_ZERO,
			GR_BLEND_ONE,
			GR_BLEND_ZERO );
	grChromakeyMode( GR_CHROMAKEY_DISABLE );
}

//////////////////////////////////////////////////////////////////////////////
#define JNUM(x) ((x) ? ((x)-1) : 9 )
void draw_player( void *oi )
{
	int		mod;
	obj_3d	*pobj;
	fbplyr_data	*ppdata;
	int		j1,j2;
	LIMB	*tmplimb;

	pobj = (obj_3d *)oi;
	ppdata = (fbplyr_data *)oi;

	if(!(pobj->flags & PF_ONSCREEN ))
		return;

	// select model
	mod = (pobj->flags & PF_HICOUNT ) ? MOD_498 : MOD_266;

	if( ppdata->static_data->size == PS_FAT )
		mod |= MOD_FAT;

	if ((ppdata->plyrnum == game_info.ball_carrier) &&
		!(ball_obj.flags & BF_PHANTOM))
	{
		if((pobj->plyrmode == MODE_QB) && (pobj->flags & PF_FLIP))
			mod |= MOD_LBALL;
		else
			mod |= MOD_RBALL;
	}

	if (pup_bigball)
		mod |= MOD_JUMBO;

//	if (pobj->flags & PF_FLASH_RED)
//	{
//		guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA);
//		grConstantColorValue(0x00FF0000);
//	}
	if( pobj->flags & PF_FLASHWHITE )
	{
		guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA);
		grConstantColorValue(0xc0000000);
	}

	grDepthBiasLevel( 0L );


	// if special head player, swap heads
	if (ISHUMAN(ppdata) && (game_info.team_head[ppdata->station] != -1))
	{
		int				head_swap = FALSE;
#ifdef SEATTLE
		GrMipMapId_t	swp;
#else
		Texture_node_t	*swp;
#endif

		tmplimb = models[mod][JOINT_NECK];

		// if we're in four player mode, and there's two custom heads on this team, AND
		// we're the rightmost player, quickly swap elements 7 and 8 in our texture list,
		// then un-swap when we're done.  This is a hack.  Ugh.
		if ((four_plr_ver) &&
			(game_info.team_head[ppdata->team*2] != -1) &&
			(game_info.team_head[ppdata->team*2+1] != -1) &&
			(ppdata->station & 0x1))
		{
			head_swap = TRUE;
			swp = pobj->pdecal[7];
			pobj->pdecal[7] = pobj->pdecal[8];
			pobj->pdecal[8] = swp;
		}


		models[mod][JOINT_NECK] = head_limbs[game_info.team_head[ppdata->station]];
		render_node_mpc( fbplyr_skel, models[mod], ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0] );
		models[mod][JOINT_NECK] = tmplimb;
		
		if (head_swap)
		{
			swp = pobj->pdecal[7];
			pobj->pdecal[7] = pobj->pdecal[8];
			pobj->pdecal[8] = swp;
		}
	}
	else if (pobj->flags & PF_POP_HELMET)
	{
		tmplimb = models[mod][JOINT_NECK];

		limb_phead_obj.ptri->texture = 4;
		models[mod][JOINT_NECK] = &limb_phead_obj;
// 		models[mod][JOINT_NECK] = head_limbs[0];
		render_node_mpc(fbplyr_skel, models[mod], ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0]);
		models[mod][JOINT_NECK] = tmplimb;
	}
	else
	{
		render_node_mpc( fbplyr_skel, models[mod], ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0] );
	}

	grDepthBiasLevel( -8L );
	guTexSource( pobj->pdecal[5] );
	j1 = ppdata->static_data->number >> 4;
	j2 = ppdata->static_data->number & 0x0F;

	mod &= MOD_FAT|MOD_498;
	if( j1 )
		mod |= MOD_2DIGITS;

	grTexClampMode(0,GR_TEXTURECLAMP_CLAMP,GR_TEXTURECLAMP_CLAMP);
	render_limb_jers( jerseys[mod], ppdata->matrices[1], JNUM(j1), JNUM(j2) );
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	if (pobj->flags & PF_FLASH_RED)
	{
		grConstantColorValue(0x00FF0000);
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
//		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	}
	else if( pobj->flags & PF_FLASHWHITE )
	{
		guColorCombineFunction( GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA );
		grConstantColorValue(0xff000000);
	}
}


//////////////////////////////////////////////////////////////////////////////
#define JNUM(x) ((x) ? ((x)-1) : 9 )
void draw_player2( void *oi )
{

	float	xlate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
	 		  			  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.0f };

	float	rotate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, 1.0f, 0.0f, 0.0f,
						   0.0f, 0.0f, 1.0f, 0.0f };

	float	cm[12];

	int		mod;
	obj_3d	*pobj;
	fbplyr_data	*ppdata;
	int		j1,j2;
	LIMB	*tmplimb;

	pobj = (obj_3d *)oi;
	ppdata = (fbplyr_data *)oi;


	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_LESS );
	grDepthMask( FXTRUE );
	grDepthBiasLevel( 0 );
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
//	grAlphaTestFunction( GR_CMP_GREATER );
//	grAlphaTestReferenceValue( 128L );

	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

//	grAlphaBlendFunction( GR_BLEND_ONE,
//			GR_BLEND_ZERO,
//			GR_BLEND_ONE,
//			GR_BLEND_ZERO );
//
	grChromakeyMode(GR_CHROMAKEY_DISABLE);


	// select model
	mod =  MOD_498;
//	mod =  MOD_266;

//	if (game_info.team[ppdata->team] == -1)
//	{
		if((pobj->plyrmode == MODE_QB) && (pobj->flags & PF_FLIP))
			mod |= MOD_LBALL;
		else
			mod |= MOD_RBALL;
//	}

	grDepthBiasLevel(0L);

	xlate[3] = pobj->x;
	xlate[7] = pobj->y;
	xlate[11] = pobj->z;
	roty(pobj->fwd,rotate);
	mxm(xlate, rotate, cm);
	generate_matrices( fbplyr_skel, cm, pobj->adata[0].pq, ppdata->matrices[0], ppdata->jpos[0] );

	render_node_mpc( fbplyr_skel, models[mod], ppdata->matrices[0], pobj->pdecal, ppdata->jpos[0] );

	grDepthBiasLevel(-8L);
//	grDepthBiasLevel(0L);
	guTexSource( pobj->pdecal[5] );
	j1 = ppdata->static_data->number >> 4;
	j2 = ppdata->static_data->number & 0x0F;

	mod &= MOD_FAT|MOD_498;
	if( j1 )
		mod |= MOD_2DIGITS;

	grTexClampMode(0,GR_TEXTURECLAMP_CLAMP,GR_TEXTURECLAMP_CLAMP);
	render_limb_jers( jerseys[mod], ppdata->matrices[1], JNUM(j1), JNUM(j2) );
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
}

//////////////////////////////////////////////////////////////////////////////
static int target_cmp( const void *, const void * );

float pr_sort_mode_x;

#define LEFTMOST_FWD			0
#define CENTERMOST_FWD		1
#define RIGHTMOST_FWD			2
#define LEFTMOST_REAR			3
#define CENTERMOST_REAR		4
#define RIGHTMOST_REAR		5
#define LEFTMOST_ANY			6
#define CENTERMOST_ANY		7
#define RIGHTMOST_ANY			8

static int pick_receiver( obj_3d *pobj )
{
	int			targets[3], i, tcount = 0, stick_cur;
	int			fwd[3],rear[3],lfwd[3],rfwd[3],lrear[3],rrear[3];
	int			nfwd=0,nlfwd=0,nrfwd=0,nrear=0,nlrear=0,nrrear=0;
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;
	obj_3d		*ptgt;
//	int			xf,yf;
	int			result = -1,*list,lnum;
	float		f = game_info.off_side ? -1.0f:1.0f;

//fprintf(stderr,"pick_receiver() station:%d\r\n",ppdata->station);

	//FIX!!!???
	// Chk station; if < 0, set <stick_cur> to what it would have been anyhow
	//  instead of "shifting" it right 4,294,967,288 times. This is an UP+LEFT
	//  stick press; do drones always want this?!?
	stick_cur = (ppdata->station >= 0) ?
		(get_player_49way_current() >> (ppdata->station<<3)) & P_SWITCH_MASK :
		0;

	// identify eligible receivers
	for( i = ppdata->team * 7 + 3; i < ppdata->team * 7 + 7; i++ )
	{
		ptgt = (obj_3d *)(player_blocks + i);

		// skip non-weapons
		if( ptgt->plyrmode != MODE_WPN )
			continue;

		// skip blocking weapons (unless human)
		if ((((fbplyr_data *)ptgt)->job == JOB_BLOCK ) && (ISDRONE((fbplyr_data *)ptgt)))
			continue;

		// skip self
		if (ptgt == pobj)
			continue;

		targets[tcount++] = i;

		if ((f*ptgt->x) >= (f*pobj->x))
		{
			fwd[nfwd++] = i;

			if( f*ptgt->z > f*pobj->z )
				rfwd[nrfwd++] = i;
			else
				lfwd[nlfwd++] = i;
		}
		else
		{
			rear[nrear++] = i;

			if( f*ptgt->z > f*pobj->z )
				rrear[nrrear++] = i;
			else
				lrear[nlrear++] = i;
		}
	}

//	fprintf( stderr, "targets: %p = {%2x, %2x, %2x}  tcount: %d\n",
//			targets,
//			(tcount <= 0) ? 0xFF : JERSEYNUM(targets[0]),
//			(tcount <= 1) ? 0xFF : JERSEYNUM(targets[1]),
//			(tcount <= 2) ? 0xFF : JERSEYNUM(targets[2]),
//			tcount );
//	fprintf( stderr, "fwd: %p = {%2x, %2x, %2x}  nfwd: %d\n",
//			fwd,
//			(nfwd <= 0) ? 0xFF : JERSEYNUM(fwd[0]),
//			(nfwd <= 1) ? 0xFF : JERSEYNUM(fwd[1]),
//			(nfwd <= 2) ? 0xFF : JERSEYNUM(fwd[2]),
//			nfwd );
//	fprintf( stderr, "rear: %p = {%2x, %2x, %2x}  nrear: %d\n",
//			rear,
//			(nrear <= 0) ? 0xFF : JERSEYNUM(rear[0]),
//			(nrear <= 1) ? 0xFF : JERSEYNUM(rear[1]),
//			(nrear <= 2) ? 0xFF : JERSEYNUM(rear[2]),
//			nrear );
//	fprintf( stderr, "lfwd: %p = {%2x, %2x, %2x}  nlfwd: %d\n",
//			lfwd,
//			(nlfwd <= 0) ? 0xFF : JERSEYNUM(lfwd[0]),
//			(nlfwd <= 1) ? 0xFF : JERSEYNUM(lfwd[1]),
//			(nlfwd <= 2) ? 0xFF : JERSEYNUM(lfwd[2]),
//			nlfwd );
//	fprintf( stderr, "rfwd: %p = {%2x, %2x, %2x}  nrfwd: %d\n",
//			rfwd,
//			(nrfwd <= 0) ? 0xFF : JERSEYNUM(rfwd[0]),
//			(nrfwd <= 1) ? 0xFF : JERSEYNUM(rfwd[1]),
//			(nrfwd <= 2) ? 0xFF : JERSEYNUM(rfwd[2]),
//			nrfwd );
//	fprintf( stderr, "lrear: %p = {%2x, %2x, %2x}  nlrear: %d\n",
//			lrear,
//			(nlrear <= 0) ? 0xFF : JERSEYNUM(lrear[0]),
//			(nlrear <= 1) ? 0xFF : JERSEYNUM(lrear[1]),
//			(nlrear <= 2) ? 0xFF : JERSEYNUM(lrear[2]),
//			nlrear );
//	fprintf( stderr, "rrear: %p = {%2x, %2x, %2x}  nrrear: %d\n",
//			rrear,
//			(nrrear <= 0) ? 0xFF : JERSEYNUM(rrear[0]),
//			(nrrear <= 1) ? 0xFF : JERSEYNUM(rrear[1]),
//			(nrrear <= 2) ? 0xFF : JERSEYNUM(rrear[2]),
//			nrrear );

	// no receivers?  Shouldn't really happen.
	if( tcount == 0 )
		return -1;

	// if there's only one receiver, hit him
	if( tcount == 1 )
		return targets[0];

	switch (stick_cur / 7)
	{
		case 0: case 1: // forward
			switch (stick_cur % 7)
			{
				case 0: case 1: // left
					result = nlfwd ? LEFTMOST_FWD : LEFTMOST_ANY;
					break;
				case 2: case 3: case 4: // center (left-right)
					result = nfwd ? CENTERMOST_FWD : CENTERMOST_ANY;
					break;
				case 5: case 6:	// right
					result = nrfwd ? RIGHTMOST_FWD : RIGHTMOST_ANY;
					break;
			}
			break;
		case 2: case 3: case 4: // center (up-down)
			switch (stick_cur % 7)
			{
				case 0: case 1: // left
					result = LEFTMOST_ANY;
					break;
				case 2: case 3: case 4: // center (left-right)
					result = CENTERMOST_ANY;
					break;
				case 5: case 6:	// right
					result = RIGHTMOST_ANY;
					break;
			}
			break;
		case 5: case 6: // rear
			switch (stick_cur % 7)
			{
				case 0: case 1: // left
					result = nlrear ? LEFTMOST_REAR : LEFTMOST_ANY;
					break;
				case 2: case 3: case 4: // center (left-right)
					result = (nlrear+nrrear) ? CENTERMOST_REAR : CENTERMOST_ANY;
					break;
				case 5: case 6:	// right
					result = nrrear ? RIGHTMOST_REAR : RIGHTMOST_ANY;
					break;
			}
			break;
	}

	assert( result != -1 );

	// if there's two guys in any of fwd, rear, or targets lists, add a third
	// so that centermost will catch the correct guy.
	if( tcount == 2 )
	{
		tcount = 3;
		if( fabs(player_blocks[targets[0]].odata.z - pobj->z ) <
			fabs(player_blocks[targets[1]].odata.z - pobj->z ))
			targets[2] = targets[0];
		else
			targets[2] = targets[1];
	}
	if( nfwd == 2 )
	{
		nfwd = 3;
		if( fabs(player_blocks[fwd[0]].odata.z - pobj->z ) <
			fabs(player_blocks[fwd[1]].odata.z - pobj->z ))
			fwd[2] = fwd[0];
		else
			fwd[2] = fwd[1];
	}
	if( nrear == 2 )
	{
		nrear = 3;
		if( fabs(player_blocks[rear[0]].odata.z - pobj->z ) <
			fabs(player_blocks[rear[1]].odata.z - pobj->z ))
			rear[2] = rear[0];
		else
			rear[2] = rear[1];
	}

	// sort forward,rear,and all targets in ascending Z coordinate order
	pr_sort_mode_x = game_info.off_side ? -1.0f : 1.0f;
	qsort( targets, tcount, sizeof( int ), target_cmp );
	qsort( fwd, nfwd, sizeof( int ), target_cmp );
	qsort( rear, nrear, sizeof( int ), target_cmp );

//	fprintf( stderr, "result: %d\n", result );

	switch( result )
	{
		case LEFTMOST_FWD: case RIGHTMOST_FWD: case CENTERMOST_FWD:
			list = fwd;
			lnum = nfwd;
			break;
		case LEFTMOST_REAR: case RIGHTMOST_REAR: case CENTERMOST_REAR:
			list = rear;
			lnum = nrear;
			break;
		case LEFTMOST_ANY: case RIGHTMOST_ANY: case CENTERMOST_ANY:
			list = targets;
			lnum = tcount;
			break;
		default:
#ifdef DEBUG
			fprintf( stderr, "bogosity #1\n" );
			lockup();	// not fatal -- keep going
#endif
			list = targets;
			lnum = tcount;
			break;
	}

//	fprintf( stderr, "list: %p = {%2x, %2x, %2x}  lnum: %d\n",
//			list,
//			(lnum <= 0 ) ? 0xFF : JERSEYNUM(list[0]),
//			(lnum <= 1 ) ? 0xFF : JERSEYNUM(list[1]),
//			(lnum <= 2 ) ? 0xFF : JERSEYNUM(list[2]),
//			lnum );

	switch( result )
	{
		case LEFTMOST_FWD: case LEFTMOST_REAR: case LEFTMOST_ANY:
//			fprintf( stderr, "returning %d(%2x)\n", list[0], JERSEYNUM(list[0]));
			return list[0];
		case RIGHTMOST_FWD: case RIGHTMOST_REAR: case RIGHTMOST_ANY:
//			fprintf( stderr, "returning %d(%2x)\n", list[lnum-1], JERSEYNUM(list[lnum-1]));
			return list[lnum-1];
		case CENTERMOST_FWD: case CENTERMOST_REAR: case CENTERMOST_ANY:
//			fprintf( stderr, "returning %d(%2x)\n", list[1], JERSEYNUM(list[(lnum>1)?1:0]));
			return list[(lnum>1)?1:0];
		default:
#ifdef DEBUG
			fprintf( stderr, "bogosity #2\n" );
			lockup(); // not fatal
#endif
			return list[(lnum>1)?1:0];
	}
}

//////////////////////////////////////////////////////////////////////////////
static int target_cmp( const void *a, const void *b )
{
	fbplyr_data		*pa,*pb;
	int				xa,xb;

	pa = player_blocks + *(int *)a;
	pb = player_blocks + *(int *)b;

	xa = (int)(pr_sort_mode_x * pa->odata.z * 10.0f);
	xb = (int)(pr_sort_mode_x * pb->odata.z * 10.0f);

	return( xa - xb );
}

//////////////////////////////////////////////////////////////////////////////
#define BALL_VELOCITY		5.50f
#define BALL_YARDS			60.0f
#define BALL_YMAX			(-1.0f*GRAVITY*BALL_YARDS*YD2UN/(2.0f*BALL_VELOCITY))
#define TIME_MAX			(BALL_YARDS*YD2UN/BALL_VELOCITY)
#define COS45				0.7071f

#define TBALL_VELOCITY		5.70f
#define TBALL_YMAX			(-1.0f*GRAVITY*BALL_YARDS*YD2UN/(2.0f*TBALL_VELOCITY))
#define TTIME_MAX			(BALL_YARDS*YD2UN/TBALL_VELOCITY)

#define FBALL_YARDS			120.0f
#define FBALL_VELOCITY		7.50f
#define FBALL_YMAX			(-1.0f*GRAVITY*FBALL_YARDS*YD2UN/(2.0f*TBALL_VELOCITY))
#define FTIME_MAX			(FBALL_YARDS*YD2UN/TBALL_VELOCITY)

#ifdef PASS_DEBUG
float		ball_start[3];
float		tgt_start[3];
float		tgt_vel[3];
float		ball_vel[3];
float		int_point[3];
#endif

#ifdef PASS_DEBUG
void dump_last_pass( int sw_state, int sw_id )
{
	if(sw_state != SWITCH_CLOSE)
		return;

	printf( "\n\nlast pass:\n" );
	printf( "ball start =\t%6.1f %6.1f %6.1f\n", ball_start[0], ball_start[1],
		ball_start[2] );
	printf( "tgt start =\t%6.1f %6.1f %6.1f\n", tgt_start[0], tgt_start[1],
		tgt_start[2] );
	printf( "tgt vel =\t%6.1f %6.1f %6.1f\n", tgt_vel[0], tgt_vel[1],
		tgt_vel[2] );
	printf( "ball vel =\t%6.1f %6.1f %6.1f\n", ball_vel[0], ball_vel[1],
		ball_vel[2] );
	printf( "int point =\t%6.1f %6.1f %6.1f\n", int_point[0], int_point[1],
		int_point[2] );
	printf( "last catch = %x\n", (int)last_catch );
	printf( "did catch = %d\n", did_catch );
}

#endif

static void set_ball_vels( obj_3d *pobj, obj_3d *ptgt )
{
	float	a,b,c,vx,vy,vz,xob,yob,zob,xop,zop,vxp,vzp,b2m4ac;
	float	root1, root2, tx,tz;
	int		throw_joint, ttt, canned = TRUE, prob=0, select, i;
	float	rx,ry,rz, *tm, bdotp, bxpy, otx,otz, inbounds_time;
	fbplyr_data *ppdata;
	catch_t	*pcatch;
	int		unint_receiver = FALSE, clip = FALSE;
	float	ball_velocity, ball_ymax, time_max, d;

	ppdata = (fbplyr_data *)pobj;
	last_catch = (char *)0;
	did_catch = FALSE;

	// make sure we're throwing at a teammate
	if ((((fbplyr_data *)pobj)->team) != (((fbplyr_data *)ptgt)->team))
	{
		fprintf( stderr, "Error! - QB throwing at someone on the other team!\n" );
		#ifdef DEBUG
		lockup();
		#else
		while(1);
		#endif
	}

	if ( ((ppdata->station >= 0) && (pup_fastpass & (1<<ppdata->station))) ||
		game_info.team_fire[ppdata->team] )
	{
		ball_velocity = FBALL_VELOCITY;
		ball_ymax = FBALL_YMAX;
		time_max = FTIME_MAX;
	}
	else if (ball_obj.flags & BF_TURBO_PASS)
	{
		ball_velocity = TBALL_VELOCITY;
		ball_ymax = TBALL_YMAX;
		time_max = TTIME_MAX;
	}
	else
	{
		ball_velocity = BALL_VELOCITY;
		ball_ymax = BALL_YMAX;
		time_max = TIME_MAX;
	}

	throw_joint = ( pobj->flags & PF_FLIP ) ? JOINT_LWRIST : JOINT_RWRIST;

	// start point
	rx = ppdata->jpos[throw_joint][0];
	ry = ppdata->jpos[throw_joint][1];
	rz = ppdata->jpos[throw_joint][2];

	tm = cambot.xform;

	xob = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	yob = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	zob = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	ball_obj.ox = xob;
	ball_obj.oy = yob;
	ball_obj.oz = zob;

	#ifdef PASS_DEBUG
	ball_start[0] = xob;
	ball_start[1] = yob;
	ball_start[2] = zob;
	tgt_start[0] = ptgt->x;
	tgt_start[1] = 0.0f;
	tgt_start[2] = ptgt->z;
	tgt_vel[0] = ptgt->vx;
	tgt_vel[1] = 0.0f;
	tgt_vel[2] = ptgt->vz;
	#endif

	xop = ptgt->x;
	zop = ptgt->z;

	vxp = ptgt->vx;
	vzp = ptgt->vz;

	a = vxp * vxp + vzp * vzp - (ball_velocity*ball_velocity);
	b = 2.0f * ((vxp * (xop - xob) + vzp * (zop - zob)));
	c = (xop - xob) * (xop - xob) + (zop - zob) * (zop - zob);

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> recvr x,z=%7.2f,%7.2f\n", xop,zop );
	fprintf( stderr, "  THROW> recvr vx,vz=%7.2f,%7.2f\n", vxp,vzp );
	fprintf( stderr, "  THROW> a,b,c=%7.2f,%7.2f,%7.2f\n", a,b,c );
#endif

	b2m4ac = b * b - 4.0f * a * c;

	if( b2m4ac < 0 )
	{
#ifdef DEBUG
		fprintf( stderr, "no solution on set_ball_vels()\n" );
		lockup();		// oops!  no solution. should never happen
#endif					// as long as ball is faster than player
		root1 = 150.0f;	// just throw the thing
		root2 = 150.0f;
	}
	else
	{
		root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
		root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	}

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> r1,r2=%7.2f,%7.2f\n", root1, root2 );
#endif

	if(( root1 <= 0.0f ) && (root2 <= 0.0f ))
	{
		// uh-oh.  can't hit the guy.
#ifdef DEBUG
		fprintf( stderr, "no positive solution on set_ball_vels()\n" );
		lockup();		// oops!  no solution. should never happen
#endif					// as long as ball is faster than player
		root1 = 150.0f;	// just throw the thing
		root2 = 150.0f;
	}

	if(( root1 > 0.0f ) && (root2 > 0.0f ))
	{
		if( root1 <= root2 )
			root1 = root2;
	}
	else
	{
		// one of the roots is less than zero
		if( root1 <=0.0f )
			root1 = root2;
	}

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> r1,r2=%7.2f,%7.2f\n", root1, root2 );
#endif

	ttt = (int)(root1 + 0.5f);

	vx = (xop - xob) / root1 + vxp;
	vz = (zop - zob) / root1 + vzp;
//	vy = (14.0f - yob) / root1 - GRAVITY / 2.0f * root1;
	vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;

	if( vy > ball_ymax )
	{
		vy = ball_ymax;
		root1 = time_max;
		ttt = (int)(root1+0.5f);
		canned = FALSE;
	}

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> ball(1) vx,vy,vz=%7.2f,%7.2f,%7.2f\n", vx,vy,vz );
#endif

#if 0
	// OLD METHOD - If target is out of bounds, reduce Y velocity such that target
	// is IN bounds.  Essentially, draw a line from the target X to the QB and set
	// new target X at the point where this line crosses the edge of the field.
	// Turns out this gives crummy results.
	// keep the target X in bounds
	tx = xob + root1*vx;
	if (fabs(tx) >= (FIELDHB-YD2UN))
	{
		tx = (tx>0.0f) ? (FIELDHB-YD2UN) : (YD2UN-FIELDHB);
		root1 = (tx - xob) / vx;
		ttt = (int)(root1+0.5f);
		vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;
		canned = FALSE;
#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> x out of range\n" );
#endif
	}
	tz = zob + root1*vz;
	if (fabs(tz) >= (FIELDW-YD2UN))
	{
		tz = (tz>0.0f) ? (FIELDW-YD2UN) : (YD2UN-FIELDW);
		root1 = (tz - zob) / vz;
		ttt = (int)(root1+0.5f);
		vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;
		canned = FALSE;
#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> z out of range\n" );
#endif
	}
#else
	// NEW METHOD - If target is out of bounds, trace a line from the X to the
	// receiver and set new target point at the spot where this line crosses the
	// edge.  Should give way better results than the old (crummy) method.
	tx = xob + root1*vx;
	tz = zob + root1*vz;

	if (fabs(tx) >= (FIELDHB-YD2UN))
	{
		tx = (tx>0.0f) ? (FIELDHB-YD2UN) : (YD2UN-FIELDHB);
		clip = TRUE;
	}

	if (fabs(tz) >= (FIELDW-YD2UN))
	{
		tz = (tz>0.0f) ? (FIELDW-YD2UN) : (YD2UN-FIELDW);
		clip = TRUE;
	}

	if (clip)
	{
		d = fsqrt((tx-xob)*(tx-xob) + (tz-zob)*(tz-zob));
		root1 = d / ball_velocity;
		vx = (tx-xob) / root1;
		vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;
		vz = (tz-zob) / root1;
		canned = FALSE;
		#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> out of bounds\n" );
		fprintf( stderr, "  THROW> tgt x,z = %f,%f\n", tx,tz );
		fprintf( stderr, "  THROW> vx,vz = %f,%f\n", vx,vz );
		#endif
	}
#endif

	if ((ptgt->vx == 0.0f) && (ptgt->vz == 0.0f))
		canned = FALSE;

	// dont do spectacular catches behind the line of scrimmage
	if (FIELDX(ptgt->x) <= game_info.los)
		canned = FALSE;

	// no canned catch if human
	if (ISHUMAN((fbplyr_data *)ptgt))
		canned = FALSE;

	if( canned )
	{
		float	nvxp, nvzp, nvxb, nvzb, mag;

#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> doing special catch\n" );
#endif

		// is receiver unint?
		if( ptgt->adata[0].animode & MODE_UNINT )
			unint_receiver = TRUE;
		if(( ptgt->adata[1].animode & MODE_TWOPART ) &&
			( ptgt->adata[1].animode & MODE_UNINT ))
			unint_receiver = TRUE;

		// in how many ticks will receiver, at present course & speed,
		// run out of bounds?
		if( fabs(ptgt->vx) < 0.01 )
			otx = 10000;
		else
		{
			otx = (FIELDHB - ptgt->x) / ptgt->vx;
			if( otx < 0.0f )
				otx = (-FIELDHB - ptgt->x) / ptgt->vx;
		}

		if( fabs(ptgt->vz) < 0.01 )
			otz = 10000;
		else
		{
			otz = (FIELDW - ptgt->z) / ptgt->vz;
			if( otz < 0.0f )
				otz = (-FIELDW - ptgt->z) / ptgt->vz;
		}

		inbounds_time = LESSER(otx,otz);

		mag = fsqrt( vx*vx + vz*vz );
		nvxb = vx / mag;
		nvzb = vz / mag;

		mag = fsqrt( vxp*vxp + vzp*vzp );

		if( mag > 0.01f )
		{
			nvxp = vxp / mag;
			nvzp = vzp / mag;
		}
		else
		{
			nvxp = FRELX( 0.0f, 1.0f, ptgt->fwd );
			nvzp = FRELZ( 0.0f, 1.0f, ptgt->fwd );
		}

		// we're gonna choreograph this catch.  Pick a table.
		// first, figure out the angle from which ball will hit receiver
		bdotp = nvxb*nvxp + nvzb*nvzp;
		if( bdotp >= COS45 )
			pcatch = catch_behind_tbl;
		else if( bdotp <= -COS45 )
			pcatch = catch_front_tbl;
		else
		{
			bxpy = nvzb*nvxp - nvxb*nvzp;
			if( bxpy > 0 )
				pcatch = catch_lside_tbl;
			else
				pcatch = catch_rside_tbl;
		}

		// now sum the probabilities from the legal catches
		for( i = 0; pcatch[i].anim != NULL; i++ )
		{
			// make sure there's enough time to launch
			// (and don't cut it close, either)
			if( pcatch[i].launch_time >= (ttt-2))
				continue;

			// check for exclusions among the flags
			if(( pcatch[i].flags & CF_NO_UNINT ) && unint_receiver )
				continue;

			// make sure he won't have to jump out of bounds
			if( ttt - pcatch[i].launch_time + pcatch[i].control_time > (int)inbounds_time )
				continue;

			prob += pcatch[i].prob;
		}

		if( prob )
		{
			select = 1+randrng(prob);
			i = -1;
			while( select > 0 )
			{
				i += 1;

				// enough launch time?
				if( pcatch[i].launch_time >= ttt )
					continue;

				// check for exclusions among the flags
				if(( pcatch[i].flags & CF_NO_UNINT ) && unint_receiver )
					continue;

				// make sure he won't have to jump out of bounds
				if( pcatch[i].control_time > inbounds_time )
					continue;

				select -= pcatch[i].prob;
			}

			// correct for y offset
			vy += ((pcatch[i].jump_height - 21.0f) / root1 );

			// correct for z offset
			vx += FRELX(0.0f,pcatch[i].z_offset,ptgt->fwd) / root1;
			vz += FRELZ(0.0f,pcatch[i].z_offset,ptgt->fwd) / root1;

			// tell receiver about it
			if (pcatch[i].twopart == TRUE)
				drone_catch( ball_obj.int_receiver, (char *)(0x7fffffff & (int)pcatch[i].anim), pcatch[i].launch_time );
			else
				drone_catch( ball_obj.int_receiver, pcatch[i].anim, pcatch[i].launch_time );

			last_catch = pcatch[i].anim;
		}
	}

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> ball(2) vx,vy,vz=%7.2f,%7.2f,%7.2f\n", vx,vy,vz );
#endif

	// set ball initial position
	ball_obj.odata.x = xob;
	ball_obj.odata.y = yob;
	ball_obj.odata.z = zob;

	// set ball target X
	ball_obj.tx = xob + root1 * vx;
	ball_obj.tz = zob + root1 * vz;
	ball_obj.ty = 0.0f;

	// set ball vels
	ball_obj.odata.vx = vx;
	ball_obj.odata.vy = vy;
	ball_obj.odata.vz = vz;

	#ifdef PASS_DEBUG
	ball_vel[0] = vx;
	ball_vel[1] = vy;
	ball_vel[2] = vz;
	int_point[0] = ball_obj.tx;
	int_point[1] = ball_start[1] + root1 * vy + 0.5f * GRAVITY * root1 * root1;
	int_point[2] = ball_obj.tz;
	#endif

	// init ball timers
	ball_obj.flight_time = 0;
	ball_obj.total_time = ttt;

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> init x,y,z=%7.2f,%7.2f,%7.2f\n", xob, yob, zob );
	fprintf( stderr, "  THROW> tgt x,y,z=%7.2f,%7.2f,%7.2f\n", ball_obj.tx,ball_obj.ty,ball_obj.tz );
	fprintf( stderr, "  THROW> vel x,y,z=%7.2f,%7.2f,%7.2f\n", ball_obj.odata.vx,ball_obj.odata.vy,ball_obj.odata.vz );
	fprintf( stderr, "  THROW> vel xz=%7.2f\n", fsqrt(vx*vx+vz*vz));
	fprintf( stderr, "  THROW> time=%7.2f(%d)\n", root1,ttt );
#endif

//#ifdef DAN
//{
//#include "include/fontid.h"
//	delete_multiple_strings(0xde, 0xffffffff);
//	set_text_transparency_mode(TRANSPARENCY_ENABLE);
//	set_text_font(FONTID_BAST10);
//	set_string_id(0xde);
//	set_text_position(256, 30, 1.2f);
//      oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, ball_obj.total_time);
//}
//#endif //DAN
}

//////////////////////////////////////////////////////////////////////////////
#define BALL_VELOCITY2		2.70f		//2.00 for drone teams or humans up by 10!
#define BALL_YARDS2			20.0f
#define BALL_YMAX2			(-1.0f*GRAVITY*BALL_YARDS2*6.0f*FOCFAC/(2.0f*BALL_VELOCITY2))
#define TIME_MAX2			(BALL_YARDS2*6.0f*FOCFAC/BALL_VELOCITY2)

static void set_ball_vels2(obj_3d *pobj, obj_3d *ptgt)
{
	float	a,b,c,vx,vy,vz,xob,yob,zob;
	float	xop,zop,vxp,vzp,b2m4ac;
	float	root1, root2;
	int		throw_joint, ttt;
	float	rx,ry,rz, *tm, tx,tz;
	fbplyr_data *ppdata;

	throw_joint = ( pobj->flags & PF_FLIP ) ? JOINT_LWRIST : JOINT_RWRIST;

	ppdata = (fbplyr_data *)pobj;

	// start point
	rx = ppdata->jpos[throw_joint][0];
	ry = ppdata->jpos[throw_joint][1];
	rz = ppdata->jpos[throw_joint][2];

	tm = cambot.xform;

	xob = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	yob = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	zob = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	ball_obj.ox = xob;
	ball_obj.oy = yob;
	ball_obj.oz = zob;

	xop = ptgt->x;
	zop = ptgt->z;

	if (game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL)
	{
		zop += ((( pobj->flags & PF_FLIP ) ? 1 : 0) ^ game_info.off_side) ? 4.0f : -4.0f;
		vy = 5.0f;
	}
	else
	{
		vy = 14.0f;						//14.0f aims for torso
	}

	vxp = ptgt->vx;
	vzp = ptgt->vz;

	a = vxp * vxp + vzp * vzp - (BALL_VELOCITY2*BALL_VELOCITY2);
	b = 2.0f * ((vxp * (xop - xob) + vzp * (zop - zob)));
	c = (xop - xob) * (xop - xob) + (zop - zob) * (zop - zob);

	b2m4ac = b * b - 4.0f * a * c;

	if( b2m4ac < 0 )
	{
#ifdef DEBUG
		fprintf( stderr, "no solution on set_ball_vels()\n" );
		lockup();		// oops!  no solution. should never happen
#endif					// as long as ball is faster than player
		root1 = 150.0f;	// just throw the thing
		root2 = 150.0f;
	}
	else
	{
		root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
		root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	}

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> r1,r2=%7.2f,%7.2f\n", root1, root2 );
#endif

	if(( root1 <= 0.0f ) && (root2 <= 0.0f ))
	{
		// uh-oh.  can't hit the guy.
#ifdef DEBUG
		fprintf( stderr, "no positive solution on set_ball_vels()\n" );
		lockup();		// oops!  no solution. should never happen
#endif					// as long as ball is faster than player
		root1 = 150.0f;	// just throw the thing
		root2 = 150.0f;
	}

	if(( root1 > 0.0f ) && (root2 > 0.0f ))
	{
		if( root1 <= root2 )
			root1 = root2;
	}
	else
	{
		// one of the roots is less than zero
		if( root1 <=0.0f )
			root1 = root2;
	}

	ttt = (int)(root1 + 0.5f);

	vx = (xop - xob) / (float)ttt + vxp;
	vz = (zop - zob) / (float)ttt + vzp;
	vy =  (vy - yob) / (float)ttt + GRAVITY / -2.0f * root1;

	if( vy > BALL_YMAX2 )
	{
		vy = BALL_YMAX2;
		ttt = (int)(TIME_MAX2+0.5f);
	}

	// keep the target X in bounds
	tx = xob + root1*vx;
	if (fabs(tx) >= (FIELDHB-YD2UN))
	{
		tx = (tx>0.0f) ? (FIELDHB-YD2UN) : (YD2UN-FIELDHB);
		root1 = (tx - xob) / vx;
		ttt = (int)(root1+0.5f);
		vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;
#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> x out of range\n" );
#endif
	}
	tz = zob + root1*vz;
	if (fabs(tz) >= (FIELDW-YD2UN))
	{
		tz = (tz>0.0f) ? (FIELDW-YD2UN) : (YD2UN-FIELDW);
		root1 = (tz - zob) / vz;
		ttt = (int)(root1+0.5f);
		vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;
#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> z out of range\n" );
#endif
	}

	// set ball initial position
	ball_obj.odata.x = xob;
	ball_obj.odata.y = yob;
	ball_obj.odata.z = zob;

	// set ball target X
	ball_obj.tx = xob + (float)ttt * vx;
	ball_obj.tz = zob + (float)ttt * vz;
	ball_obj.ty = 0.0f;

	// set ball vels
	ball_obj.odata.vx = vx;
	ball_obj.odata.vy = vy;
	ball_obj.odata.vz = vz;

	// init ball timers
	ball_obj.flight_time = 0;
	ball_obj.total_time = ttt;
}


//////////////////////////////////////////////////////////////////////////////
#define BALL_HIKE_VELOCITY		5.40f		//2.00 for drone teams or humans up by 10!
#define BALL_YARDS3			20.0f
#define BALL_YMAX3			(-1.0f*GRAVITY*BALL_YARDS3*6.0f*FOCFAC/(2.0f*BALL_HIKE_VELOCITY))
#define TIME_MAX3			(BALL_YARDS3*6.0f*FOCFAC/BALL_HIKE_VELOCITY)

static void set_hike_ball_vels(obj_3d *pobj, obj_3d *ptgt)
{
	float	a,b,c,vx,vy,vz,xob,yob,zob;
	float	xop,zop,vxp,vzp,b2m4ac;
	float	root1, root2;
	int		throw_joint, ttt;
	float	rx,ry,rz, *tm, tx,tz;
	fbplyr_data *ppdata;

	throw_joint = ( pobj->flags & PF_FLIP ) ? JOINT_LWRIST : JOINT_RWRIST;

	ppdata = (fbplyr_data *)pobj;

	// start point
	rx = ppdata->jpos[throw_joint][0];
	ry = ppdata->jpos[throw_joint][1];
	rz = ppdata->jpos[throw_joint][2];

	tm = cambot.xform;

	xob = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	yob = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	zob = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	ball_obj.ox = xob;
	ball_obj.oy = yob;
	ball_obj.oz = zob;

	xop = ptgt->x;
	zop = ptgt->z;

	if (game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL)
	{
		zop += ((( pobj->flags & PF_FLIP ) ? 1 : 0) ^ game_info.off_side) ? 4.0f : -4.0f;
		vy = 5.0f;
	}
	else
	{
		vy = 14.0f;						//14.0f aims for torso
	}

	vxp = ptgt->vx;
	vzp = ptgt->vz;

	a = vxp * vxp + vzp * vzp - (BALL_HIKE_VELOCITY*BALL_HIKE_VELOCITY);
	b = 2.0f * ((vxp * (xop - xob) + vzp * (zop - zob)));
	c = (xop - xob) * (xop - xob) + (zop - zob) * (zop - zob);

	b2m4ac = b * b - 4.0f * a * c;

	if( b2m4ac < 0 )
	{
#ifdef DEBUG
		fprintf( stderr, "no solution on set_ball_vels()\n" );
		lockup();		// oops!  no solution. should never happen
#endif					// as long as ball is faster than player
		root1 = 150.0f;	// just throw the thing
		root2 = 150.0f;
	}
	else
	{
		root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
		root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	}

#ifdef THROW_DEBUG
	fprintf( stderr, "  THROW> r1,r2=%7.2f,%7.2f\n", root1, root2 );
#endif

	if(( root1 <= 0.0f ) && (root2 <= 0.0f ))
	{
		// uh-oh.  can't hit the guy.
#ifdef DEBUG
		fprintf( stderr, "no positive solution on set_ball_vels()\n" );
		lockup();		// oops!  no solution. should never happen
#endif					// as long as ball is faster than player
		root1 = 150.0f;	// just throw the thing
		root2 = 150.0f;
	}

	if(( root1 > 0.0f ) && (root2 > 0.0f ))
	{
		if( root1 <= root2 )
			root1 = root2;
	}
	else
	{
		// one of the roots is less than zero
		if( root1 <=0.0f )
			root1 = root2;
	}

	ttt = (int)(root1 + 0.5f);

	vx = (xop - xob) / (float)ttt + vxp;
	vz = (zop - zob) / (float)ttt + vzp;
	vy =  (vy - yob) / (float)ttt + GRAVITY / -2.0f * root1;

	if (vy > BALL_YMAX3)
	{
		vy = BALL_YMAX3;
		ttt = (int)(TIME_MAX3+0.5f);
	}

	// keep the target X in bounds
	tx = xob + root1*vx;
	if (fabs(tx) >= (FIELDHB-YD2UN))
	{
		tx = (tx>0.0f) ? (FIELDHB-YD2UN) : (YD2UN-FIELDHB);
		root1 = (tx - xob) / vx;
		ttt = (int)(root1+0.5f);
		vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;
#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> x out of range\n" );
#endif
	}
	tz = zob + root1*vz;
	if (fabs(tz) >= (FIELDW-YD2UN))
	{
		tz = (tz>0.0f) ? (FIELDW-YD2UN) : (YD2UN-FIELDW);
		root1 = (tz - zob) / vz;
		ttt = (int)(root1+0.5f);
		vy = (21.0f - yob) / root1 - GRAVITY / 2.0f * root1;
#ifdef THROW_DEBUG
		fprintf( stderr, "  THROW> z out of range\n" );
#endif
	}

	// set ball initial position
	ball_obj.odata.x = xob;
	ball_obj.odata.y = yob;
	ball_obj.odata.z = zob;

	// set ball target X
	ball_obj.tx = xob + (float)ttt * vx;
	ball_obj.tz = zob + (float)ttt * vz;
	ball_obj.ty = 0.0f;

	// set ball vels
	ball_obj.odata.vx = vx;
	ball_obj.odata.vy = vy;
	ball_obj.odata.vz = vz;

	// init ball timers
	ball_obj.flight_time = 0;
	ball_obj.total_time = ttt;
}


//////////////////////////////////////////////////////////////////////////////
static void set_punt_ball_vels(obj_3d *pobj)
{
	float	xob,yob,zob;
	int		throw_joint, t;
	float	rx,ry,rz, *tm, et;
	int		max_vx;
	int		max_vy;
	fbplyr_data *ppdata;

	throw_joint = ( pobj->flags & PF_FLIP ) ? JOINT_LANKLE : JOINT_RANKLE;

	ppdata = (fbplyr_data *)pobj;

	// start point
	rx = ppdata->jpos[throw_joint][0];
	ry = ppdata->jpos[throw_joint][1];
	rz = ppdata->jpos[throw_joint][2];

	tm = cambot.xform;

	xob = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	yob = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	zob = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	// set ball initial position
	ball_obj.ox = xob;
	ball_obj.oy = yob;
	ball_obj.oz = zob;

	ball_obj.odata.x = xob;
	ball_obj.odata.y = yob;
	ball_obj.odata.z = zob;

	// set ball vels
//	et = -2.0f * 5.5f / GRAVITY;
//	ball_obj.odata.vx = YD2UN * (float)(100 - game_info.los) / et;
//	ball_obj.odata.vx = LESSER(ball_obj.odata.vx, 3.6f);

	//	punt_strength: { 0 ... 100 }
	//	vx: { 2.0f ... 4.0f }
	//	vy: { 4.0f ... 6.0f }

	max_vx = ((PUNT_MAX_VX - PUNT_MIN_VX) * (float)(100 - game_info.los)) / 100;
	max_vy = ((PUNT_MAX_VY - PUNT_MIN_VY) * (float)(100 - game_info.los)) / 100;

	ball_obj.odata.vx = (PUNT_MIN_VX + ((max_vx * punt_strength) / 100)) / 10.0f;
	ball_obj.odata.vy = (PUNT_MIN_VY + ((max_vy * punt_strength) / 100)) / 10.0f;
	ball_obj.odata.vz = -1.0f + ((float)randrng(201)) / 100.0f;

	if(game_info.off_side == 1)
		ball_obj.odata.vx *= -1.0f;

	ball_obj.pitch_rate = -45;
	ball_obj.flags &= ~(BF_VEL_PITCH | BF_FG_BOUNCE | BF_FG_HITPOST);

	t = -2.0f * ball_obj.odata.vy / GRAVITY;

	// place target X
	ball_obj.tx = ball_obj.odata.x + t * ball_obj.odata.vx;
	ball_obj.tz = ball_obj.odata.z + t * ball_obj.odata.vz;
	ball_obj.ty = 0.0f;

	// init ball timers
	ball_obj.flight_time = 0;
	ball_obj.total_time = t;
}

//////////////////////////////////////////////////////////////////////////////
extern image_info_t playresult;

void show_fg_per(int *);

//	args[0] = value to be displayed
void show_fg_per(int * args)
{
	sprite_info_t * bx_obj = beginobj( &playresult, 151, 343 , 1.21f, SCR_PLATE_TID );
	bx_obj->w_scale = 40.0f / playresult.w;	// 40p wide
	bx_obj->h_scale = 25.0f / playresult.h;	// 25p tall
	bx_obj->id = OID_FGPER;
	generate_sprite_verts( bx_obj );

	set_text_transparency_mode( TRANSPARENCY_ENABLE );
	set_text_font( FONTID_BAST10 );
	set_string_id( SID_FGPER );
	set_text_position( 152, 343, 1.2f );

#ifndef DEBUG
	// Don't mod the % in debug
	if (args[0] < 5)  args[0] = 5;
	if (args[0] > 95) args[0] = 95;
#endif
	oprintf( "%dj%dc%d%%", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, args[0] );

	while (game_info.game_mode != GM_PLAY_OVER)
	{
		sleep(1);
	}
	del1c( OID_FGPER, -1 );
	delete_multiple_strings( SID_FGPER, -1 );
}

//////////////////////////////////////////////////////////////////////////////
static void set_field_goal_ball_vels(obj_3d *pobj)
{
	extern float kmtr_per;
	fbplyr_data * ppdata = (fbplyr_data *)pobj;
	float	target_x, target_y, target_z;
	float	total_delta_x, delta_x, ft;
//	float	dist, v;
	int		psucc, asucc, up_by;
	int		min_acc, max_acc, yd_att;

	//	notes about calculating F.G.
	//		game_info.los is the line of scrimage
	//		line of scrimage ranges from 0 (farthest from offensive goal) to 100 (closest to offensive goal)
	//		the x-axis runs up/down the field
	//		the z-axis runs parallel to the yard lines on the field

	up_by = compute_score(game_info.off_side) - compute_score(!game_info.off_side);

	ball_obj.ox = ball_obj.odata.x;
	ball_obj.oy = ball_obj.odata.y;
	ball_obj.oz = ball_obj.odata.z;

	// SET CHANCE OF SUCCESS

	if (ISHUMANTEAM(ppdata))
	{
		// Was it auto-hiked?
		if (game_info.auto_hike_cntr <= 0)
		{
			// Give'em low default odds since he timed-out
			psucc = 3 * game_info.los - 175;
		}
		else
		{
				yd_att = 100 - game_info.los;
				yd_att = GREATER(5,yd_att);
				yd_att = LESSER(50,yd_att);
				min_acc = 20 + (((yd_att-5) * 4) / 3);
				max_acc = 80 + ((yd_att-5) / 3);

				if (kmtr_per >= 0)
					psucc = ((100.0f - kmtr_per) - min_acc) * 100 / (max_acc - min_acc);
				else
					psucc = ((100.0f + kmtr_per) - min_acc) * 100 / (max_acc - min_acc);
#ifdef DEBUG
				fprintf( stderr, "kick from %d, min acc = %d, max acc = %d\n",
							yd_att, min_acc, max_acc );
				fprintf( stderr, "acc is %d, psucc = %d\n", (int)kmtr_per, psucc );
#endif
		}
	}
	else
		// Default drone odds
		psucc = 3 * game_info.los - 125;

	// swap left & right if we're team 1
	if (!game_info.off_side)
		kmtr_per = -kmtr_per;

	// sometimes just swap left & right for the heck of it
	if (!randrng(5))
		kmtr_per = -kmtr_per;

	// discourage tie-breaking field goals
	if ((up_by == 0) && (game_info.game_quarter > 2) && !pup_noassist &&
		(game_info.game_time < 0x1E00))
	{
		psucc -= 30;
	}
	// encourage tie-making field goals
	if ((up_by == -3) && (game_info.game_quarter > 2) && !pup_noassist)
	{
		psucc += 35;
	}
	// drones in drone_lose mode shouldn't make game-winning FGs
	if ((ISDRONETEAM(ppdata)) && (drones_lose) &&
		(up_by < 0) && (up_by > -3) && (game_info.game_quarter > 2 ) &&
		(game_info.game_time < 0x2800))
	{
		psucc -= 20;
	}
	// cap at 95% success
	psucc = LESSER(psucc,95);

	// adjust for diff settings
	psucc += fg_bonus_ht[PLYRH(ppdata)];

	// Set default target values
	target_x = game_info.off_side ? -GOAL_X : GOAL_X;
	target_y = FG_MIN_TARGET_Y + (float)randrng(FG_MAX_TARGET_Y - FG_MIN_TARGET_Y);
	target_z = (kmtr_per > 0) ? -GOAL_Z : GOAL_Z;

	if (game_info.team_fire[game_info.off_side] || pup_perfect_fg)
	{
		// Team is on fire or has power-up of "Perfect Field Goals"
		//  give them perfect field goal settings
		target_z = 0.0f;
		asucc = 100;
	}

	if (ISHUMANTEAM(ppdata) & pup_showfgper)
	  	qcreate ("shwfgp", 0, show_fg_per, psucc, 0, 0, 0);

	asucc = psucc - randrng(100);

	if (asucc < -50)
		asucc = -50;

//#define FG_POSTS
#ifdef FG_POSTS
	asucc = -24;
//{
//	static int inc = -25;
//	asucc = inc--;
//	if(inc < -50) inc = -25;
//	fprintf(stderr,"----> asucc = %d\r\n",asucc);
//}
#endif

	if (target_z)
	{
		if (asucc > 0 && target_z)
		{
			// Score the FG
			target_z = (float)(GREATER(40 - asucc, 0)) / 40.0f * GOAL_Z * (randrng(2) ? -1.0f : 1.0f);
		}
		else
		if (asucc > -25)
		{
			// Aim for posts or crossbar; might score anyway
			if (randrng(100) < 40)
			{	
				// Aim for crossbar
				target_y = GOAL_CROSSBAR - 5;
				target_z = target_z * (float)randrng(86) * 0.01f;
			}
			else
			{
				// Aim for an upright
//				target_z = target_z;
			}
		}
		else
		{
			// Miss it clean
			target_z = target_z * -65.0f / (float)(asucc);
		}
	}

	//	CALCULATE BALL VELOCITY

//	dist = fabs(target_x - ball_obj.odata.x);
//	v = dist / (5.0f * fsqrt(dist - tagret_y));
//	ft = dist / v;
//
//	ball_obj.odata.vx = game_info.off_side ? -v : v;
//	ball_obj.odata.vy = v;
//	ball_obj.odata.vz = (zt-ball_obj.odata.z) / ft;

	total_delta_x = fabs(target_x - ball_obj.odata.x);
	ft = (5.0f * fsqrt(total_delta_x - target_y));
	delta_x = total_delta_x / (float)ft;

	ball_obj.odata.vx = game_info.off_side ? -delta_x : delta_x;
	ball_obj.odata.vy = delta_x;
	ball_obj.odata.vz = (target_z - ball_obj.odata.z) / ft;

	// init ball timers
	ball_obj.flight_time = 0;
	ball_obj.total_time = ft;
}

//////////////////////////////////////////////////////////////////////////////
static void set_spike_ball_vels(obj_3d *pobj, int type)
{
	float	xob,yob,zob;
	int		a,b,c,t;
	float	rx,ry,rz, *tm;
	fbplyr_data *ppdata;

	ppdata = (fbplyr_data *)pobj;

	// start point
	rx = ppdata->jpos[JOINT_RWRIST][0];
	ry = ppdata->jpos[JOINT_RWRIST][1];
	rz = ppdata->jpos[JOINT_RWRIST][2];

	tm = cambot.xform;

	xob = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	yob = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	zob = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	// set ball initial position
	ball_obj.ox = xob;
	ball_obj.oy = yob;
	ball_obj.oz = zob;

	ball_obj.odata.x = xob;
	ball_obj.odata.y = yob;
	ball_obj.odata.z = zob;

	// set ball vels

	if (type == 0)
	{
		// spike ball
		ball_obj.odata.vx = 1.5f;
		ball_obj.odata.vx += ((float)randrng(10000)) / 9000.0f;
		ball_obj.odata.vy = -5.5f;
		ball_obj.odata.vz = 0.0f;
	}
	else
	{
		// fling ball
		ball_obj.odata.vx = FRELX(0.0f, 1.5f, pobj->fwd-576);
		ball_obj.odata.vz = FRELZ(0.0f, 1.5f, pobj->fwd-576);
		ball_obj.odata.vy = 1.8f;
	}

	a = GRAVITY / 2.0f;
	b = ball_obj.odata.vy;
	c = ball_obj.odata.y;

	t = solve_quad( a, b, c );

	// place target X
	ball_obj.tx = ball_obj.odata.x + t * ball_obj.odata.vx;
	ball_obj.tz = ball_obj.odata.z + t * ball_obj.odata.vz;
	ball_obj.ty = 0.0f;

	// init ball timers
	ball_obj.flight_time = 0;
	ball_obj.total_time = t;
}


//////////////////////////////////////////////////////////////////////////////
float odistxz( obj_3d *o1, obj_3d *o2 )
{
	return fsqrt((o2->x - o1->x) * (o2->x - o1->x) +
				 (o2->z - o1->z) * (o2->z - o1->z));
}

//////////////////////////////////////////////////////////////////////////////
// player shadow functions
//////////////////////////////////////////////////////////////////////////////
#ifdef SEATTLE
		GrMipMapId_t	ps_decals[4];
#else
		Texture_node_t	*ps_decals[4];
#endif
int				ps_ratios[4];

static void player_shadow_init( void )
{
	char *shadow_names[] = { "psshould.wms", "pslimb.wms", "pstorso.wms",
			"pshand.wms", NULL };

	load_textures( shadow_names, ps_ratios, ps_decals, 4, TXID_PLRSHDW,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
}

static void player_shadow_predraw( void *oi )
{
	float		cx,cy,cz;		// cambot x,y,z

	// do some fancy math
	cx = -cambot.x;	cy = -cambot.y;	cz = -cambot.z;

	fx = cx * cambot.xform[0] + cy * cambot.xform[1] + cz * cambot.xform[2];
	fy = cx * cambot.xform[4] + cy * cambot.xform[5] + cz * cambot.xform[6];
	fz = cx * cambot.xform[8] + cy * cambot.xform[9] + cz * cambot.xform[10];

	sx = SUN_X * cambot.xform[0] + SUN_Y * cambot.xform[1] + SUN_Z * cambot.xform[2];
	sy = SUN_X * cambot.xform[4] + SUN_Y * cambot.xform[5] + SUN_Z * cambot.xform[6];
	sz = SUN_X * cambot.xform[8] + SUN_Y * cambot.xform[9] + SUN_Z * cambot.xform[10];

	nx = cambot.xform[1];
	ny = cambot.xform[5];
	nz = cambot.xform[9];

	// init glide states
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
	grAlphaTestFunction( GR_CMP_GREATER );
	grAlphaTestReferenceValue( 100L );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_GREATER );
	grDepthMask( FXTRUE );
}

typedef struct sh_link {
	int		l1,l2;		// joint indices
	float	w1,w2;		// (half)width
	float	k1,k2;		// length extensions
	int		tex;		// texture to use
} sh_link_t;

void draw_player_shadow( void *oi )
{
	fbplyr_data	*ppdata = (fbplyr_data *)oi;
	obj_3d		*pobj = (obj_3d *)oi;
	int			depth = 30;

	vec3d		vtx[15];
	float		p1p2[3],vr[3],vu[3],p1p2xn[3],rmagp1p2,rmagp1p2xn,t;
	float		*p1,*p2;
	MidVertex	va,vb,vc,vd;
	int			used[] = {0,2,3,4,5,6,7,8,10,11,13,14};
	float		tx,ty,tz,d;
	float		xf,yf;
	int			i,j;
	sh_link_t links[] = {
		{0,8,	2.4f,2.4f,	0.3f,0.8f, 2},
		{2,5,	1.92f,1.92f,	0.3f,0.3f, 0},
		{3,2,	1.44f,1.56f,	0.3f,0.3f, 1},
		{4,3,	1.44f,1.44f,	0.3f,0.3f, 3},
		{6,5,	1.44f,1.56f,	0.3f,0.3f, 1},
		{7,6,	1.44f,1.44f,	0.3f,0.3f, 3},
		{10,0,	1.92f,1.92f,	0.3f,0.3f, 1},
		{11,10,	1.44f,1.44f,	0.3f,0.3f, 1},
		{13,0,	1.92f,1.92f,	0.3f,0.3f, 1},
		{14,13,	1.44f,1.44f,	0.3f,0.3f, 1}};

	if(!(pobj->flags & PF_ONSCREEN ))
		return;

	pftimer_start( PRF_SHADOW );

	xf = (float)(hres/2);
	yf = (float)(hres/2) * (is_low_res ? 0.666667f : 1.0f);

	// project the joints onto the ground
	for( j = 0; j < 12; j++ )
	{
		i = used[j];
		t = (nx*(fx-ppdata->jpos[i][0]) + ny*(fy-ppdata->jpos[i][1]) + nz*(fz-ppdata->jpos[i][2])) / SUN_Y;
		vtx[i].x = ppdata->jpos[i][0] + t*sx;
		vtx[i].y = ppdata->jpos[i][1] + t*sy;
		vtx[i].z = ppdata->jpos[i][2] + t*sz;
	}

	// now draw each shadow link
	for( i = 0; i < 10; i++ )
	{
		p1 = (float *)&vtx[links[i].l1];
		p2 = (float *)&vtx[links[i].l2];

		p1p2[0] = vtx[links[i].l2].x - vtx[links[i].l1].x;
		p1p2[1] = vtx[links[i].l2].y - vtx[links[i].l1].y;
		p1p2[2] = vtx[links[i].l2].z - vtx[links[i].l1].z;

		d = fsqrt( p1p2[0]*p1p2[0] + p1p2[1]*p1p2[1] + p1p2[2]*p1p2[2] );
		rmagp1p2 = 1.0f / d;
		p1p2xn[0] = p1p2[1] * nz - p1p2[2] * ny;
		p1p2xn[1] = p1p2[2] * nx - p1p2[0] * nz;
		p1p2xn[2] = p1p2[0] * ny - p1p2[1] * nx;
		rmagp1p2xn = 1.0f / fsqrt( p1p2xn[0]*p1p2xn[0] + p1p2xn[1]*p1p2xn[1] + p1p2xn[2]*p1p2xn[2] );

		vr[0] = p1p2xn[0] * rmagp1p2xn;
		vr[1] = p1p2xn[1] * rmagp1p2xn;
		vr[2] = p1p2xn[2] * rmagp1p2xn;

		vu[0] = p1p2[0] * rmagp1p2;
		vu[1] = p1p2[1] * rmagp1p2;
		vu[2] = p1p2[2] * rmagp1p2;

		tx = p1[0] + links[i].w1 * vr[0] - d * links[i].k1 * vu[0];
		ty = p1[1] + links[i].w1 * vr[1] - d * links[i].k1 * vu[1];
		tz = p1[2] + links[i].w1 * vr[2] - d * links[i].k1 * vu[2];
		va.oow = 1.0f / tz;
		tx = - (tx * (FOCFAC*2.0f) * va.oow);
		ty = ty * (FOCFAC*2.0f) * va.oow;
		tx *= xf;
		ty *= yf;
		tx += (float)(hres/2);
		ty += (float)(vres/2);
		tx += (float)(1<<21);
		ty += (float)(1<<21);
		tx -= (float)(1<<21);
		ty -= (float)(1<<21);
		va.x = tx;
		va.y = ty;
		va.sow = 255.0f * va.oow;
		va.tow = 255.0f * va.oow;

		tx = p2[0] + links[i].w2 * vr[0] + d * links[i].k2 * vu[0];
		ty = p2[1] + links[i].w2 * vr[1] + d * links[i].k2 * vu[1];
		tz = p2[2] + links[i].w2 * vr[2] + d * links[i].k2 * vu[2];
		vb.oow = 1.0f / tz;
		tx = - (tx * (FOCFAC*2.0f) * vb.oow);
		ty = ty * (FOCFAC*2.0f) * vb.oow;
		tx *= xf;
		ty *= yf;
		tx += (float)(hres/2);
		ty += (float)(vres/2);
		tx += (float)(1<<21);
		ty += (float)(1<<21);
		tx -= (float)(1<<21);
		ty -= (float)(1<<21);
		vb.x = tx;
		vb.y = ty;
		vb.sow = 255.0f * vb.oow;
		vb.tow = 0.0f;

		tx = p2[0] - links[i].w2 * vr[0] + d * links[i].k2 * vu[0];
		ty = p2[1] - links[i].w2 * vr[1] + d * links[i].k2 * vu[1];
		tz = p2[2] - links[i].w2 * vr[2] + d * links[i].k2 * vu[2];
		vc.oow = 1.0f / tz;
		tx = - (tx * (FOCFAC*2.0f) * vc.oow);
		ty = ty * (FOCFAC*2.0f) * vc.oow;
		tx *= xf;
		ty *= yf;
		tx += (float)(hres/2);
		ty += (float)(vres/2);
		tx += (float)(1<<21);
		ty += (float)(1<<21);
		tx -= (float)(1<<21);
		ty -= (float)(1<<21);
		vc.x = tx;
		vc.y = ty;
		vc.sow = 0.0f;
		vc.tow = 0.0f;

		tx = p1[0] - links[i].w1 * vr[0] - d * links[i].k1 * vu[0];
		ty = p1[1] - links[i].w1 * vr[1] - d * links[i].k1 * vu[1];
		tz = p1[2] - links[i].w1 * vr[2] - d * links[i].k1 * vu[2];
		vd.oow = 1.0f / tz;
		tx = - (tx * (FOCFAC*2.0f) * vd.oow);
		ty = ty * (FOCFAC*2.0f) * vd.oow;
		tx *= xf;
		ty *= yf;
		tx += (float)(hres/2);
		ty += (float)(vres/2);
		tx += (float)(1<<21);
		ty += (float)(1<<21);
		tx -= (float)(1<<21);
		ty -= (float)(1<<21);
		vd.x = tx;
		vd.y = ty;
		vd.sow = 0.0f;
		vd.tow = 255.0f * vd.oow;

		grDepthBiasLevel( depth-=2 );
		guTexSource( ps_decals[links[i].tex] );

		/*
		** perform trivial accept
		*/
		if (( va.x >= -HRESf) && ( va.x < (HRESf*2.0f)) &&
			( va.y >= -VRESf) && ( va.y < (VRESf*2.0f)) &&
			( vb.x >= -HRESf) && ( vb.x < (HRESf*2.0f)) &&
			( vb.y >= -VRESf) && ( vb.y < (VRESf*2.0f)) &&
			( vc.x >= -HRESf) && ( vc.x < (HRESf*2.0f)) &&
			( vc.y >= -VRESf) && ( vc.y < (VRESf*2.0f)) &&
			( va.oow > 0.0F ) && ( va.oow <= 1.0F ) &&
			( vb.oow > 0.0F ) && ( vb.oow <= 1.0F ) &&
			( vc.oow > 0.0F ) && ( vc.oow <= 1.0F )
			)
		{
			grDrawTriangleDma( &va, &vb, &vc, 0x04 );
			grDrawTriangleDma( &vc, &vd, &va, 0x04 );
		}
	}
	pftimer_end( PRF_SHADOW );
}

//////////////////////////////////////////////////////////////////////////////
//
// debug switches
//
//////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
//extern int dbg_playback;
extern struct texture_node *tlist;
void debug_switch_proc( int *parg )
{
	static int	dbsw_cur, dbsw_old, dbsw_new;
	static int	step = FALSE;

#ifdef JASON
//	die(0);
#endif

	while( 1 )
	{
		dbsw_cur = get_p5_sw_current();
		dbsw_new = get_p5_sw_close();

		if( dbsw_cur & P_C )
		{
			if( dbsw_new & P_UP )
				dump_drone_states();
			else if( dbsw_new & P_DOWN )
				dump_game_state(FALSE);
			else if( dbsw_new & P_LEFT )
				dump_anim_data( 1 );
			else if( dbsw_new & P_RIGHT )
				dump_anim_data( 0 );
		}

		if ((dbsw_new & P_D) || freeze)
		{
			halt = !halt;

			if (freeze)
				freeze = FALSE;

			if( halt )
			{
				fprintf( stderr, "anim debug enabled\n" );
				suspend_multiple_processes( PLAYER_PID, -1 );
				suspend_multiple_processes( PRE_PLAYER_PID, -1 );
				suspend_multiple_processes( BALL_PID, -1 );
				suspend_multiple_processes( GAME_CLOCK_PID, -1 );
				suspend_multiple_processes( GAME_PROC_PID, -1 );
			}
			else
			{
				fprintf( stderr, "anim debug disabled\n" );
//				if( !dbg_playback )
				{
					resume_multiple_processes( PLAYER_PID, -1 );
					resume_multiple_processes( PRE_PLAYER_PID, -1 );
					resume_multiple_processes( BALL_PID, -1 );
					resume_multiple_processes( GAME_CLOCK_PID, -1 );
				}
				resume_multiple_processes( GAME_PROC_PID, -1 );
			}
		}

		if( halt && step )
		{
			suspend_multiple_processes( PLAYER_PID, -1 );
			suspend_multiple_processes( PRE_PLAYER_PID, -1 );
			suspend_multiple_processes( BALL_PID, -1 );
			suspend_multiple_processes( GAME_CLOCK_PID, -1 );
			suspend_multiple_processes( GAME_PROC_PID, -1 );
		}
		step = FALSE;

		if( halt && !(dbsw_cur & P_ABCD_MASK) )
		{
			if( (dbsw_new & P_RLDU_MASK) == P_UP )
			{
				step = TRUE;
//				if( !dbg_playback )
				{
					resume_multiple_processes( PLAYER_PID, -1 );
					resume_multiple_processes( PRE_PLAYER_PID, -1 );
					resume_multiple_processes( BALL_PID, -1 );
					resume_multiple_processes( GAME_CLOCK_PID, -1 );
				}
				resume_multiple_processes( GAME_PROC_PID, -1 );
			}
			if( (dbsw_new & P_RLDU_MASK) == P_DOWN )
			{
				dump_joint_positions();
			}
			// Object list dump is a Goose debug option (TILT btn)
			// Process list dump is a Goose debug option (TILT btn)
		}

		delete_multiple_strings(0xdddd, 0xffffffff);
		if( halt )
		{
			set_text_font(FONTID_BAST10);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_string_id(0xdddd);
			set_text_position(256, 8, 1.2f);
			oprintf("%dj%dcHALT", (HJUST_CENTER|VJUST_CENTER), LT_CYAN);
		}

		sleep( 1 );
	}
}
#endif //DEBUG

//////////////////////////////////////////////////////////////////////////////
//
// cursors
//
//////////////////////////////////////////////////////////////////////////////
extern struct image_info arrow_blue;
extern struct image_info arrow_red;
extern struct image_info arrow_green;
extern struct image_info arrow_yellow;
extern struct image_info arrow_num1;
extern struct image_info arrow_num2;
extern struct image_info arrow_num3;
extern struct image_info arrow_num4;

extern struct image_info aud_menu;

//struct texture_node * ptn_gndarrow = NULL;

VERTEX gndarrow_obj_vtx[4];
ST     gndarrowp1_obj_st[4];
ST     gndarrowp2_obj_st[4];
ST     gndarrowp3_obj_st[4];
ST     gndarrowp4_obj_st[4];

#if 0
TRI    gndarrow_obj_tris[] =
{
	{0,6,3, 0,2,1, 0},
	{9,3,6, 3,255,255, -12},
	{0,0,0, 0,0,0, -20}
};
#endif

#if 0
LIMB limb_gndarrowp1_obj =
{
	4,2,NULL,
	gndarrow_obj_vtx,
	gndarrowp1_obj_st,
	gndarrow_obj_tris
#if (MULTIPART_IMAGES & 1)
	,NULL,0
#endif
};
#endif

#if 0
LIMB limb_gndarrowp2_obj =
{
	4,2,NULL,
	gndarrow_obj_vtx,
	gndarrowp2_obj_st,
	gndarrow_obj_tris
#if (MULTIPART_IMAGES & 1)
	,NULL,0
#endif
};
#endif

#define ARROW_SCALE	0.3f
//////////////////////////////////////////////////////////////////////////////
void cursor_proc( int *parg )
{
	int 	tgt, i;
	char	*cursor_names[] = { "c_o_blu.wms", "c_xo_blu.wms" };
	struct texture_node	*pnode;
	fbplyr_data	*carrier;

//////////////////////////////////////////////////////////////////////////////

// Turn on arrow under feet
//	tmppnode = create_texture(arrow_blue.texture_name,
//		SCR_PLATE_TID,
//		0,
//		CREATE_NORMAL_TEXTURE,
//		GR_TEXTURECLAMP_WRAP,
//		GR_TEXTURECLAMP_WRAP,
//		GR_TEXTUREFILTER_BILINEAR,
//		GR_TEXTUREFILTER_BILINEAR );
//
//	if(!tmppnode)
//	{
//#ifdef DEBUG
//		fprintf(stderr, "%s failed to load\r\n", arrow_blue.texture_name);
//#endif
//		increment_audit(TEX_LOAD_FAIL_AUD);
//	}

	gndarrowp1_obj_st[0].s = gndarrowp1_obj_st[1].s = arrow_blue.s_start;
	gndarrowp1_obj_st[2].s = gndarrowp1_obj_st[3].s = arrow_blue.s_end;
	gndarrowp1_obj_st[0].t = gndarrowp1_obj_st[2].t = arrow_blue.t_start;
	gndarrowp1_obj_st[1].t = gndarrowp1_obj_st[3].t = arrow_blue.t_end;

	gndarrowp2_obj_st[0].s = gndarrowp2_obj_st[1].s = arrow_green.s_start;
	gndarrowp2_obj_st[2].s = gndarrowp2_obj_st[3].s = arrow_green.s_end;
	gndarrowp2_obj_st[0].t = gndarrowp2_obj_st[2].t = arrow_green.t_start;
	gndarrowp2_obj_st[1].t = gndarrowp2_obj_st[3].t = arrow_green.t_end;

	gndarrowp3_obj_st[0].s = gndarrowp3_obj_st[1].s = arrow_red.s_start;
	gndarrowp3_obj_st[2].s = gndarrowp3_obj_st[3].s = arrow_red.s_end;
	gndarrowp3_obj_st[0].t = gndarrowp3_obj_st[2].t = arrow_red.t_start;
	gndarrowp3_obj_st[1].t = gndarrowp3_obj_st[3].t = arrow_red.t_end;

	gndarrowp4_obj_st[0].s = gndarrowp4_obj_st[1].s = arrow_yellow.s_start;
	gndarrowp4_obj_st[2].s = gndarrowp4_obj_st[3].s = arrow_yellow.s_end;
	gndarrowp4_obj_st[0].t = gndarrowp4_obj_st[2].t = arrow_yellow.t_start;
	gndarrowp4_obj_st[1].t = gndarrowp4_obj_st[3].t = arrow_yellow.t_end;

	gndarrow_obj_vtx[0].x = gndarrow_obj_vtx[1].x = ARROW_SCALE * (0.0f - arrow_blue.ax);
	gndarrow_obj_vtx[2].x = gndarrow_obj_vtx[3].x = ARROW_SCALE * (arrow_blue.w - arrow_blue.ax);

	gndarrow_obj_vtx[0].y = gndarrow_obj_vtx[1].y = gndarrow_obj_vtx[2].y = gndarrow_obj_vtx[3].y = 0.0f;

	gndarrow_obj_vtx[0].z = gndarrow_obj_vtx[2].z = ARROW_SCALE * (-arrow_blue.h - arrow_blue.ay);
	gndarrow_obj_vtx[1].z = gndarrow_obj_vtx[3].z = ARROW_SCALE * (0.0f - arrow_blue.ay);


//	printf("%f  %f\n%f  %f\n%f  %f\n%f  %f\n",
//		gndarrow_obj_st[0].s,gndarrow_obj_st[0].t,
//		gndarrow_obj_st[1].s,gndarrow_obj_st[1].t,
//		gndarrow_obj_st[2].s,gndarrow_obj_st[2].t,
//		gndarrow_obj_st[3].s,gndarrow_obj_st[3].t
//	);
//	printf("%f  %f  %f\n%f  %f  %f\n%f  %f  %f\n%f  %f  %f\n",
//		gndarrow_obj_vtx[0].x,gndarrow_obj_vtx[0].y,gndarrow_obj_vtx[0].z,
//		gndarrow_obj_vtx[1].x,gndarrow_obj_vtx[1].y,gndarrow_obj_vtx[1].z,
//		gndarrow_obj_vtx[2].x,gndarrow_obj_vtx[2].y,gndarrow_obj_vtx[2].z,
//		gndarrow_obj_vtx[3].x,gndarrow_obj_vtx[3].y,gndarrow_obj_vtx[3].z
//	);
//////////////////////////////////////////////////////////////////////////////
	// load cursors
	for( i = 0; i < 2; i++ )
	{
		pnode = create_texture( cursor_names[i], TXID_PLRCURSOR, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

		if( pnode == NULL )
		{
#ifdef DEBUG
			fprintf( stderr, "Error loading %s\n", cursor_names[i] );
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}
		else
			cursor_decals[i] = pnode->texture_handle;
	}

	create_object( "cursors", OID_CURSOR, OF_NONE, DRAWORDER_FIELDFX+0x10, NULL, draw_cursors);

	tgt = -1;

	while( TRUE )
	{
		if (tgt != -1)
			player_blocks[tgt].odata.flags &= ~PF_FLASHWHITE;

		if (game_info.ball_carrier != -1)
			carrier = player_blocks + game_info.ball_carrier;
		else
			carrier = NULL;

		// if carrier is QB and human, mode is IN_PLAY, and QB isn't throwing, highlight receiver
		if ((carrier != NULL) &&
			(ISHUMAN(carrier)) &&
			!(pup_notarget & (1<<(carrier->station))) &&
			( player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_QB ) &&
			( game_info.game_mode == GM_IN_PLAY ) &&
			!( player_blocks[game_info.ball_carrier].odata.flags & PF_THROWING ))
		{
			tgt = pick_receiver( (obj_3d *)(player_blocks+game_info.ball_carrier));

			if (tgt >=0)
				player_blocks[tgt].odata.flags |= PF_FLASHWHITE;
		}
		else
			tgt = -1;

		sleep(1);
	}
}

//////////////////////////////////////////////////////////////////////////////

//#define SHOW_GOAL_Y		// put in to visually show GOAL_Y detection range

static void draw_cursors( void *oi )
{
	obj_3d			*pobj;
	int				i, stick_cur, face_dir;
#ifdef SEATTLE
	GrMipMapId_t	mycursor;
#else
	int				mycursor;
#endif
	float			mat[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
							    0.0f, 1.0f, 0.0f, 0.0f,
							    0.0f, 0.0f, 1.0f, 0.0f };
	float			rot[12], cm[12], bsf;

	static int cursor_colors[][2] = {
		{0xFF0600FF, 0xFF0600FF},
		{0xFF00C800, 0xFF00C800},
		{0x00000000, 0xFFFF0000},
		{0x00000000, 0xFFF5F505},
#ifdef SHOW_GOAL_Y
		{0xFF0600FF, 0xFF0600FF},
		{0xFF00C800, 0xFF00C800},
		{0xFFFF0000, 0xFFFF0000},
		{0xFFF5F505, 0xFFF5F505},
#endif
	};

	static int cyc_table[] = {
		0x00ff00, 0x20ff00, 0x40ff00, 0x60ff00,
		0x80ff00, 0xa0ff00, 0xc0ff00, 0xffff00,
		0xffc000, 0xffa000, 0xff8000, 0xff6000,
		0xff4000, 0xff0020, 0xff0040, 0xff0060,
		0xff0080, 0xff00c0, 0xff00ff, 0xc000ff,
		0xa000ff, 0x8020ff, 0x6020ff, 0x4020ff,
		0x4060ff, 0x00a0ff, 0x00ffff, 0x20ffff,
		0x40ffff, 0x60ffff, 0xa0ffff, 0xffffff,
		0xffff80, 0xffff00, 0xc0ff00, 0x80ff00,
	};

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_ALWAYS );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );


	for (i = 0; i < (int)(sizeof(cursor_colors)/sizeof(int)/2); i++ )
	{
#ifdef SHOW_GOAL_Y
		if (i < MAX_PLYRS)
#endif
		pobj = (obj_3d *)(player_blocks + game_info.plyr_control[i]);

		if (
#ifdef SHOW_GOAL_Y
			(i >= MAX_PLYRS) ||
#endif
			( (p_status & (1<<i)) &&
			  (game_info.plyr_control[i] >= 0) &&
			  (pobj->flags & PF_ONSCREEN) ))
		{
#ifdef SHOW_GOAL_Y
			static float xs[] = { GOAL_X,   GOAL_X,  -GOAL_X,  -GOAL_X};
			static float ys[] = { GOAL_Y+2, GOAL_Y-2, GOAL_Y+2, GOAL_Y-2};
			if (i >= MAX_PLYRS)
			{
				mat[3]  = rot[3]  = xs[i-4] - cambot.x;
				mat[7]  = rot[7]  = ys[i-4] - cambot.y;
				mat[11] = rot[11] = -cambot.z;

				mycursor = 0;
			}
			else
#endif
			{
				mat[3]  = rot[3]  = pobj->x - cambot.x;
				mat[7]  = rot[7]  = -cambot.y;
				mat[11] = rot[11] = pobj->z - cambot.z;

				mycursor = (game_info.ball_carrier == game_info.plyr_control[i] ? 1 : 0 );
			}

			mxm( cambot.xform, mat, cm );
			guColorCombineFunction(GR_COLORCOMBINE_CCRGB);
			grConstantColorValue( cursor_colors[i][four_plr_ver] );
			render_limb( &limb_gndsquare_obj, cm, cursor_decals + mycursor,
					&cursor_ratio );
			guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);

			// show arrows for all players who can move
//			if(( game_info.plyr_control[i] != game_info.captain_player[game_info.off_side] ) ||
//				( game_info.game_mode == GM_IN_PLAY ))
//			{
//				if (!(pobj->adata[0].animode & MODE_UNINT) && ptn_gndarrow)
//				{
//					if ((stick_cur = ((fbplyr_data *)pobj)->stick_cur) != 24)
//					{
//					 	if ((face_dir = dir49_table[stick_cur]) < 0) face_dir = 0;
//						face_dir = 1023 & (face_dir + (int)RD2GR(cambot.theta));
//						roty( face_dir, rot );
//						mxm( cambot.xform, rot, cm );
//						render_limb( &limb_gndarrowp1_obj, cm, &(ptn_gndarrow->texture_handle),
//								&cursor_ratio );
//					}
//				}
//			}
		}
	}

	// ball shadow & target cursor
	if((( game_info.ball_carrier == -1 ) &&
		( ball_obj.type != LB_FIELD_GOAL )) ||
		( ball_obj.flags & BF_PHANTOM ))
	{
		pobj = (obj_3d *)&ball_obj;

		mat[3]  = ball_obj.tx - cambot.x;
		mat[7]  = -cambot.y;
		mat[11] = ball_obj.tz - cambot.z;

		mxm( cambot.xform, mat, cm );

		ball_obj.tsx = cm[3];
		ball_obj.tsy = cm[7];
		ball_obj.tsz = cm[11];

		if (game_info.game_mode == GM_IN_PLAY)
			render_limb( &limb_gndsquare_obj, cm, &target_decal, &cursor_ratio );

		roty( pobj->fwd, rot );
		rot[3]  = pobj->x - cambot.x;
		rot[7]  = -cambot.y;
		rot[11] = pobj->z - cambot.z;

		mxm( cambot.xform, rot, cm );

		bsf = 2.5f;

		cm[0] *= bsf;	cm[1] *= bsf;	cm[2] *= bsf;
		cm[4] *= bsf;	cm[5] *= bsf;	cm[6] *= bsf;
		cm[8] *= bsf;	cm[9] *= bsf;	cm[10] *= bsf;

		render_limb( &limb_ballshadow_obj, cm, &ball_shadow_decal, &ball_shadow_ratio );
	}

	guColorCombineFunction(GR_COLORCOMBINE_CCRGB);

	// line of scrimmage marker
	if( game_info.show_los != -1 )
	{
		if( los_flash_time )
		{
			int l = los_flash_time % 36;

			los_flash_rgb = 0xff000000 | cyc_table[36-l];

			grConstantColorValue( los_flash_rgb );
			los_flash_time--;
		} else {
//			los_flash_rgb = ((pup_field == 5) ? 0xff00f0f0 : 0xffffa800);
//			grConstantColorValue( los_flash_rgb );							// JTA
			grConstantColorValue( 0xFFFFA800 );

		}

		mat[3] = WORLDX2(game_info.show_los,game_info.off_init) - cambot.x;
		mat[7] = -cambot.y;
		mat[11] = -cambot.z;

		mxm( cambot.xform, mat, cm );
		render_limb( &limb_fieldline_obj, cm, cursor_decals, &cursor_ratio );
	}

	// first down marker
	if( game_info.show_fd != -1 )
	{
		if( fd_flash_time )
		{
			int l = fd_flash_time % 36;

			fd_flash_rgb = 0xff000000 | cyc_table[36-l];

			grConstantColorValue( fd_flash_rgb );
			fd_flash_time--;
		}
		else
			grConstantColorValue( 0xFFFF1200 );
		mat[3] = WORLDX2(game_info.show_fd,game_info.off_init) - cambot.x;
		mat[7] = -cambot.y;
		mat[11] = -cambot.z;
		mxm( cambot.xform, mat, cm );

		render_limb( &limb_fieldline_obj, cm, cursor_decals, &cursor_ratio );
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// ball
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void maybe_pop_helmet(obj_3d *pobj)
{
	fbplyr_data			*ppdata;
	int	rnd;

	ppdata = (fbplyr_data *)pobj;
	rnd = randrng(100);

	// no helmet-off after plays over
	if (game_info.game_mode != GM_IN_PLAY)
		return;

	// no helmet-off for secret heads
	if (ISHUMAN(ppdata) && (game_info.team_head[ppdata->station] != -1))
		return;

	// get rid of previous one...
	killall(LOOSE_HELMET_PID,0xFFFFFFFF);
	delete_multiple_objects(OID_LOOSE_HELMET, 0xFFFFFFFF);

	if (ppdata->plyrnum != game_info.ball_carrier)
		return;
	
	if ((game_info.team_fire[ppdata->team]) && (rnd < 45))
	{
		pobj->flags |= PF_POP_HELMET;
		snd_scall_bank(ancr_swaped_bnk_str,165,ANCR_VOL2,127,LVL4);		// say "Decapitated him...."
		qcreate("loosehlm", LOOSE_HELMET_PID, loose_helmet_proc, ppdata->plyrnum, 0, 0, 0);
	}
	else if (randrng(100) < 10)
	{
		pobj->flags |= PF_POP_HELMET;
		snd_scall_bank(ancr_swaped_bnk_str,166,ANCR_VOL2,127,LVL4);		// say "Decapitated him...." (version 2)
		snd_scall_bank(plyr_bnk_str,P_HARD_HIT3_SND,ANCR_VOL2,127,PRIORITY_LVL3);	// do hard hit sound
		qcreate("loosehlm", LOOSE_HELMET_PID, loose_helmet_proc, ppdata->plyrnum, 0, 0, 0);
	}
}


//////////////////////////////////////////////////////////////////////////////
#define HELMET_FLOOR		0.9f

void loose_helmet_proc(int *parg)
{
	fbplyr_data			*ppdata;
	float				vx,vy,vz,vxz,vxyz;
	int					angle;
	float				prev[3], f;
	float				rx,ry,rz, *tm;
	

	ppdata = player_blocks + parg[0];
	rx = ppdata->jpos[JOINT_NECK][0];
	ry = ppdata->jpos[JOINT_NECK][1];
	rz = ppdata->jpos[JOINT_NECK][2];
	tm = cambot.xform;
	loose_helmet_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	loose_helmet_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	loose_helmet_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	loose_helmet_obj.odata.vy = 1.6f;
	loose_helmet_obj.odata.vx = .35f;
	loose_helmet_obj.odata.vz = .35f;
	if (randrng(100) < 45)
	{
		loose_helmet_obj.odata.vx *= -1;
		loose_helmet_obj.odata.vz *= -1;
	}
	loose_helmet_obj.alpha = 0xFF;
	loose_helmet_obj.pitch = 0;
	loose_helmet_obj.pitch_rate = 40;
	loose_helmet_obj.scale = 2.0f;
	loose_helmet_obj.bounces = 0;

	loose_helmet_decal = ppdata->odata.pdecal[2];
	loose_helmet_ratio = 4;

	create_object("losehmlt", OID_LOOSE_HELMET, OF_NONE, DRAWORDER_BALL, (void *)&loose_helmet_obj, draw_loose_helmet);

	while (1)
	{
	 	loose_helmet_obj.odata.x += loose_helmet_obj.odata.vx;
	 	loose_helmet_obj.odata.y += loose_helmet_obj.odata.vy + GRAVITY / 2.0f;
	 	loose_helmet_obj.odata.z += loose_helmet_obj.odata.vz;
	 	loose_helmet_obj.odata.vy += GRAVITY;
//	 	loose_helmet_obj.odata.vy += GRAVITY / 2.0f;

		// hit ground?
		if (loose_helmet_obj.odata.y <= (HELMET_FLOOR*loose_helmet_obj.scale))
		{
			if (loose_helmet_obj.bounces++ < 3)
				snd_scall_bank(plyr_bnk_str,BALL_HIT_GRND_SND,VOLUME1,127,PRIORITY_LVL2);

			// bounce
			loose_helmet_obj.flags |= BF_BOUNCE;

			loose_helmet_obj.odata.y = HELMET_FLOOR * loose_helmet_obj.scale;
			loose_helmet_obj.bounce_time = pcount;

			loose_helmet_obj.odata.vy *= -0.5;
			loose_helmet_obj.odata.vy -= 0.10;

			if (loose_helmet_obj.odata.vy <= 0.0f)
			{
				loose_helmet_obj.odata.vx = 0.0f;
				loose_helmet_obj.odata.vy = 0.0f;
				loose_helmet_obj.odata.vz = 0.0f;

				loose_helmet_obj.tx = loose_helmet_obj.odata.x;
				loose_helmet_obj.tz = loose_helmet_obj.odata.z;
				loose_helmet_obj.ty = 0.0f;
			}
			else
			{
				float	time;
				int		theta;

				vxz = fsqrt(loose_helmet_obj.odata.vx * loose_helmet_obj.odata.vx +
						loose_helmet_obj.odata.vz * loose_helmet_obj.odata.vz );
				if (vxz > 0.0f)
					theta = iasin(loose_helmet_obj.odata.vz / vxz);
				else
					theta = 0;

				if (loose_helmet_obj.odata.vx < 0.0f)
					theta = 512 - theta;

				theta += randrng(201);
				theta += randrng(157);
				theta += randrng(157);
				theta -= randrng(201);
				theta -= randrng(157);
				theta -= randrng(157);

				loose_helmet_obj.odata.vx = 0.5f * vxz * icos(theta);
				loose_helmet_obj.odata.vz = 0.5f * vxz * isin(theta);

				// plot new X
				time = -2.0f * loose_helmet_obj.odata.vy / GRAVITY;
				loose_helmet_obj.tx = loose_helmet_obj.odata.x + time * loose_helmet_obj.odata.vx;
				loose_helmet_obj.tz = loose_helmet_obj.odata.z + time * loose_helmet_obj.odata.vz;
				loose_helmet_obj.ty = 0.0f;
			}

		}

		loose_helmet_obj.flight_time += 1;


		// heading
		vx = loose_helmet_obj.odata.vx;
		vy = loose_helmet_obj.odata.vy;
		vz = loose_helmet_obj.odata.vz;
		vxz = fsqrt(vx*vx + vz*vz);
		vxyz = fsqrt(vx*vx + vy*vy + vz*vz);

		if ((vx == 0.0f) && (vz == 0.0f))
			loose_helmet_obj.odata.fwd2 = 0;
		else
		{
			angle = iasin( vx / vxz );

			if( vz < 0.0f )
				angle = 1023 & (512 - angle);

			loose_helmet_obj.odata.fwd = angle;

			// pitch
			if (loose_helmet_obj.flags & BF_VEL_PITCH)
			{
				angle = iasin(vy / vxyz);
				loose_helmet_obj.odata.fwd2 = angle;
			}
			else
			{
				loose_helmet_obj.pitch += loose_helmet_obj.pitch_rate;
				loose_helmet_obj.odata.fwd2 = loose_helmet_obj.pitch % 1024;
			}

  			loose_helmet_obj.phi += randrng(35)+30;
		}
		sleep(1);
	}

}



//////////////////////////////////////////////////////////////////////////////
void draw_loose_helmet( void *oi )
{
	float		mat1[12], mat2[12], mat3[12];
	float		bsf;

	obj_3d	*pobj = (obj_3d *)(oi);
	ball_data	*pball = (ball_data *)(oi);

	grConstantColorValue( pball->alpha << 24 );
	guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_LESS );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	if( ball_obj.flags & BF_WHITE )
	{
#if ! defined(GLIDE3)
		guColorCombineFunction(GR_COLORCOMBINE_ONE);
#else
		grConstantColorValue(0xff000000);
		guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA);
#endif
	}
	else
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);

	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_ONE,
			GR_BLEND_ZERO );

	mat1[3] = 0.0f;	mat1[7] = 0.0f;	mat1[11] = 0.0f;
	mat3[3] = 0.0f;	mat3[7] = 0.0f;	mat3[11] = 0.0f;

	rotx( -pobj->fwd2, mat3 );
	roty( pobj->fwd, mat1 );
	mxm( mat1, mat3, mat2 );
	rotz( pball->phi, mat1 );
	mxm( mat2, mat1, mat3 );

	mat3[3] = pobj->x - cambot.x;
	mat3[7] = pobj->y - cambot.y;
	mat3[11] = pobj->z - cambot.z;

	bsf = pball->scale;

	mat3[0] *= bsf;	mat3[1] *= bsf;	mat3[2] *= bsf;
	mat3[4] *= bsf;	mat3[5] *= bsf;	mat3[6] *= bsf;
	mat3[8] *= bsf;	mat3[9] *= bsf;	mat3[10] *= bsf;

	mxm(cambot.xform, mat3, mat1);

	render_limb(&limb_phelm_obj, mat1, &loose_helmet_decal, &loose_helmet_ratio);
}


//////////////////////////////////////////////////////////////////////////////
void ball_fire_trail_proc(int *args)
{
	obj3d_data_t		smkpuff_obj;
	struct object_node	*p_smkobj;
	int					i,j;

	smkpuff_obj.ratio = smkpuf_decals[0]->texture_info.header.aspect_ratio;
	smkpuff_obj.decal = smkpuf_decals[0]->texture_handle;
	smkpuff_obj.limb_obj = &limb_Polyset;

	smkpuff_obj.odata.x = ball_obj.odata.x - randrng(args[0]);
	smkpuff_obj.odata.y = ball_obj.odata.y + args[1];
	smkpuff_obj.odata.z = ball_obj.odata.z + args[2];

	p_smkobj = create_object( "puff", OID_SMKPUFF, OF_NONE, DRAWORDER_BALL+1, (void *)&smkpuff_obj, draw_spfx);

	smkpuff_obj.odata.vy = .35f;

	// flame into smoke frames
	j = NUM_SMOKE_TXTRS - randrng(5);
	for (i=randrng(5); i < j; i++)
	{
		// change texture on 3d obj.
		smkpuff_obj.ratio = smkpuf_decals[i]->texture_info.header.aspect_ratio;
		smkpuff_obj.decal = smkpuf_decals[i]->texture_handle;

		smkpuff_obj.odata.y += smkpuff_obj.odata.vy;

		sleep(1);
	}
	delete_object(p_smkobj);
}

//////////////////////////////////////////////////////////////////////////////
#if 0
void plyr_bfire_trail_proc(int *args)
{
obj3d_data_t		bfire_obj;
struct object_node	*p_bfire;
int					i,throw_joint;
fbplyr_data			*pplyr;
float				rx,ry,rz, *tm;

	pplyr = &player_blocks[args[0]];

	throw_joint = args[1];

	rx = pplyr->jpos[throw_joint][0];
	ry = pplyr->jpos[throw_joint][1];
	rz = pplyr->jpos[throw_joint][2];

	tm = cambot.xform;

	// set flame X,Y,Z to that of appropiate limb
	bfire_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	bfire_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	bfire_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	bfire_obj.odata.x += ((float)randrng(10000))/3500.0f;
	bfire_obj.odata.y += ((float)randrng(10000))/5000.0f;
	bfire_obj.odata.z += ((float)randrng(10000))/3500.0f;

	bfire_obj.ratio = bsmkpuf_decals[0]->texture_info.header.aspect_ratio;
	bfire_obj.decal = bsmkpuf_decals[0]->texture_handle;
	bfire_obj.limb_obj = &limb_Polyset;

	p_bfire = create_object( "bfire", OID_SMKPUFF, OF_NONE, DRAWORDER_BALL+1, (void *)&bfire_obj, draw_spfx);

	// randomize this....i did :)
	bfire_obj.odata.vy = 0.40f + ((float)randrng(10000))/15000;

	for (i=0; i < NUM_BSMOKE_TXTRS; i++)
	{

		// change texture on 3d obj.
		bfire_obj.ratio = bsmkpuf_decals[i]->texture_info.header.aspect_ratio;
		bfire_obj.decal = bsmkpuf_decals[i]->texture_handle;

		if (game_info.game_mode == GM_PRE_SNAP)
			break;

		sleep (1);

		bfire_obj.odata.y += bfire_obj.odata.vy;

		if (game_info.game_mode == GM_PRE_SNAP)
			break;

		if (randrng(100) < 50)
			sleep(1);
	}
	delete_object(p_bfire);
}
#endif

//////////////////////////////////////////////////////////////////////////////
void plyr_fire_trail_proc(int *args)
{
	obj3d_data_t		firepuff_obj;
	struct object_node	*p_fireobj;
	int					throw_joint,i;
	fbplyr_data			*pplyr;
	float				rx,ry,rz, *tm;

	pplyr = &player_blocks[args[0]];

	throw_joint = args[1];

	rx = pplyr->jpos[throw_joint][0];
	ry = pplyr->jpos[throw_joint][1];
	rz = pplyr->jpos[throw_joint][2];

	tm = cambot.xform;

	firepuff_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	firepuff_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	firepuff_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	firepuff_obj.odata.x += randrng(5);
	firepuff_obj.odata.y += randrng(3);

	firepuff_obj.ratio = smkpuf_decals[0]->texture_info.header.aspect_ratio;
	firepuff_obj.decal = smkpuf_decals[0]->texture_handle;
	firepuff_obj.limb_obj = &limb_Polyset;

	p_fireobj = create_object( "puff", OID_SMKPUFF, OF_NONE, DRAWORDER_BALL+1, (void *)&firepuff_obj, draw_spfx);

	firepuff_obj.odata.vy = 0.40f + ((float)randrng(10000))/15000.0f;

	// flame into smoke frames
	for (i = randrng(4); i < NUM_SMOKE_TXTRS; i++)
	{
		// change texture on 3d obj.
		firepuff_obj.ratio = smkpuf_decals[i]->texture_info.header.aspect_ratio;
		firepuff_obj.decal = smkpuf_decals[i]->texture_handle;

//		if (game_info.game_mode == GM_PRE_SNAP)
//			break;

		sleep (1);

		firepuff_obj.odata.y += firepuff_obj.odata.vy;

//		if (game_info.game_mode == GM_PRE_SNAP)
//			break;

		if (randrng(100) < 50)
			sleep(1);
	}
	delete_object(p_fireobj);
}

//////////////////////////////////////////////////////////////////////////////
static void dust_cloud_die_alert( struct process_node *proc, int cause );
static void dust_cloud_proc(int *args)
{
	obj3d_data_t		dstcld_obj;
	struct object_node	*p_dstcld;
	int					throw_joint,i,j,sleep_time;
	fbplyr_data			*pplyr;
	float				rx,ry,rz, *tm;
	struct object_node	**process_data;


	process_data = (struct object_node **)(cur_proc->process_data);

	pplyr = &player_blocks[args[0]];

	throw_joint = args[1];

	rx = pplyr->jpos[throw_joint][0];
	ry = pplyr->jpos[throw_joint][1];
	rz = pplyr->jpos[throw_joint][2];

	tm = cambot.xform;

	dstcld_obj.ratio = dustcld_decals[0]->texture_info.header.aspect_ratio;
	dstcld_obj.decal = dustcld_decals[0]->texture_handle;
	dstcld_obj.limb_obj = &limb_Polyset;

	dstcld_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
//	dstcld_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	dstcld_obj.odata.y = 0.0f;
	dstcld_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	p_dstcld = create_object( "dust", OID_SMKPUFF, OF_NONE, DRAWORDER_BALL+1, (void *)&dstcld_obj, draw_spfx);
	if (!p_dstcld)
		return;

	// store object pointer in pdata
	*process_data = p_dstcld;

	// hook an exit function to complain if we die while the object is still alive
	cur_proc->process_exit = dust_cloud_die_alert;

	sleep_time = args[2];

	dstcld_obj.odata.vx = FRELX(0.0f,.35f,((obj_3d *)pplyr)->fwd);
	dstcld_obj.odata.vz = FRELZ(0.0f,.35f,((obj_3d *)pplyr)->fwd);
	dstcld_obj.odata.vy = .10f;

	// dust cloud under player
	for (i=randrng(3),j=0; i < NUM_DUST_TXTRS; j++)
	{
		if (j>sleep_time)
		{
			j=0;
			// change texture on 3d obj.
			dstcld_obj.ratio = dustcld_decals[i]->texture_info.header.aspect_ratio;
			dstcld_obj.decal = dustcld_decals[i]->texture_handle;
			i++;
		}

		dstcld_obj.odata.x += dstcld_obj.odata.vx;
		dstcld_obj.odata.z += dstcld_obj.odata.vz;
		dstcld_obj.odata.y += dstcld_obj.odata.vy;

		if (game_info.game_mode == GM_PRE_SNAP)
			break;

		sleep(1);
	}
	delete_object(p_dstcld);
}

//////////////////////////////////////////////////////////////////////////////
static void dust_cloud_die_alert( struct process_node *proc, int cause )
{
	struct object_node	*p_dstcld;

	p_dstcld = *((struct object_node **)(proc->process_data));

	if (object_exists(p_dstcld))
	{
		#ifdef CATCH_CLOUDBUG
		fprintf( stderr, "Hey! Someone just killed (cause = %d) a dust cloud process"
			" without destroying the dust cloud object.  Stop that.\n", cause );
		#ifdef DEBUG
		lockup();
		#endif
		#endif
		delete_object( p_dstcld );
	}
}

//////////////////////////////////////////////////////////////////////////////
//void pre_blood_spray_proc(int *args)
//{
//	int	i,limit;
//
//	limit = args[3];
//	while (limit--)
//	{
//		// create blood trail starts
//		for (i=0; i < args[1]; i++)
//		  	qcreate ("blood", PLAYER_PID, blood_spray_proc, args[0], randrng(5), 0, 0);
//
//		if (game_info.game_mode == GM_PRE_SNAP)
//			break;
//
//		sleep (args[2]);
//	}
//}


//////////////////////////////////////////////////////////////////////////////
//void blood_spray_proc(int *args)
//{
//	obj3d_data_t		blood_obj;
//	struct object_node	*p_blood;
//	int					i,j;
//	fbplyr_data			*pplyr;
//	float				rx,ry,rz, *tm;
//
//
//	pplyr = &player_blocks[args[0]];
//
////	throw_joint = args[1];
//
//	rx = pplyr->jpos[JOINT_NECK][0];
//	ry = pplyr->jpos[JOINT_NECK][1];
//	rz = pplyr->jpos[JOINT_NECK][2];
//
//	tm = cambot.xform;
//
//	blood_obj.ratio = blood_decals[0]->texture_info.header.aspect_ratio;
//	blood_obj.decal = blood_decals[0]->texture_handle;
//	blood_obj.limb_obj = &limb_Polyset;
//
//	blood_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
//	blood_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
//	blood_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;
//
////	blood_obj.odata.y += randrng(3);
//
//	p_blood = create_object( "blood", OID_SMKPUFF, OF_NONE, DRAWORDER_BALL+1, (void *)&blood_obj, draw_spfx);
//
//	blood_obj.odata.vx = FRELX(0.0f,.50f,((obj_3d *)pplyr)->pobj_hit->fwd);
//	blood_obj.odata.vz = FRELZ(0.0f,.50f,((obj_3d *)pplyr)->pobj_hit->fwd);
////	blood_obj.odata.vy = .002f;
//
//	// blood spray near player mouth
//	for (i=0,j=0; i < NUM_BLOOD_TXTRS; j++)
//	{
//		if (j>1)
//		{
//			j=0;
//			// change texture on 3d obj.
//			blood_obj.ratio = blood_decals[i]->texture_info.header.aspect_ratio;
//			blood_obj.decal = blood_decals[i]->texture_handle;
//			i++;
//		}
//		blood_obj.odata.x += blood_obj.odata.vx;
//		blood_obj.odata.z += blood_obj.odata.vz;
//
//		if (game_info.game_mode == GM_PRE_SNAP)
//			break;
//
//		sleep(1);
//	}
//	delete_object(p_blood);
//}

//////////////////////////////////////////////////////////////////////////////
// TEMP
//////////////////////////////////////////////////////////////////////////////
//void draw_spark_func(void *oi)
//{
//	float		cm[12] = { 0.0f, 1.0f, 0.0f, 0.0f,
//						   1.0f, 0.0f, 0.0f, 0.0f,
//						   0.0f, 0.0f, -1.0f, 0.0f };
//	float		tx,ty,tz;
//
//	obj_3d	*pobj = (obj_3d *)oi;
//	obj3d_data_t *ppuff = (obj3d_data_t *)oi;
//
//	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
////	guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
//	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
//	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
//
//	grDepthBufferMode( GR_DEPTHBUFFER_DISABLE );
//	grDepthBufferFunction( GR_CMP_ALWAYS );
//	grAlphaTestFunction( GR_CMP_ALWAYS );
//	grDepthMask( FXTRUE );
//
//	grChromakeyMode( GR_CHROMAKEY_ENABLE );
//
//	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
//	grAlphaBlendFunction( GR_BLEND_DST_COLOR,GR_BLEND_SRC_COLOR, 0, 0 );
//
//	tx = pobj->x - cambot.x;
//	ty = pobj->y - cambot.y;
//	tz = pobj->z - cambot.z;
//
//	cm[3] = tx * cambot.xform[0] + ty * cambot.xform[1] + tz * cambot.xform[2];
//	cm[7] = tx * cambot.xform[4] + ty * cambot.xform[5] + tz * cambot.xform[6];
//	cm[11] = tx * cambot.xform[8] + ty * cambot.xform[9] + tz * cambot.xform[10];
//
//
//	render_limb( ppuff->limb_obj, cm, &(ppuff->decal), &(ppuff->ratio) );
//
//	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
//
//}

//////////////////////////////////////////////////////////////////////////////
void draw_spfx(void *oi)
{
	float		cm[12] = { 0.0f, 1.0f, 0.0f, 0.0f,
						   1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, 0.0f, -1.0f, 0.0f };
	float		tx,ty,tz;

	obj_3d	*pobj = (obj_3d *)oi;
	obj3d_data_t *ppuff = (obj3d_data_t *)oi;

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_ALWAYS );
	grAlphaTestFunction( GR_CMP_ALWAYS );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	tx = pobj->x - cambot.x;
	ty = pobj->y - cambot.y;
	tz = pobj->z - cambot.z;

	cm[3] = tx * cambot.xform[0] + ty * cambot.xform[1] + tz * cambot.xform[2];
	cm[7] = tx * cambot.xform[4] + ty * cambot.xform[5] + tz * cambot.xform[6];
	cm[11] = tx * cambot.xform[8] + ty * cambot.xform[9] + tz * cambot.xform[10];


	render_limb( ppuff->limb_obj, cm, &(ppuff->decal), &(ppuff->ratio) );
}



//-------------------------------------------------------------------------------------------------
//
//  		This process flashes player white several times
//
//			INPUT:  pobj of player to flash, cnt, sleep time
//			OUTPUT:	Nothing
//
//-------------------------------------------------------------------------------------------------
void flash_plyr_proc(int *args)
{
	obj_3d *pobj;
	fbplyr_data *pplyr;
	int delay,x,t;

	pplyr = player_blocks + args[0];
	pobj = (obj_3d *)pplyr;
	x = args[1];
	delay = args[2];

	for (t=0; t < x; t++)
	{
		pobj->flags |= PF_FLASHWHITE;
		sleep(delay);
		pobj->flags &= ~PF_FLASHWHITE;
		sleep(delay);
	}
}

//-------------------------------------------------------------------------------------------------
//
//  		This process flashes player RED several times
//
//			INPUT:  pobj of player to flash, cnt, sleep time
//			OUTPUT:	Nothing
//
//-------------------------------------------------------------------------------------------------
#if 0
void flash_plyr_red_proc(int *args)
{
	obj_3d *pobj;
	fbplyr_data *pplyr;
	int delay,x,t;

	pplyr = player_blocks + args[0];
	pobj = (obj_3d *)pplyr;
	x = args[1];
	delay = args[2];

	for (t=0; t < x; t++)
	{
		pobj->flags |= PF_FLASH_RED;
		sleep(delay);
		pobj->flags &= ~PF_FLASH_RED;
		sleep(delay);
	}
}
#endif

//-------------------------------------------------------------------------------------------------
//
//  		This process does a snd for crossing the first down marker after a few ticks & he owns ball
//
//			INPUT:  pobj of player who owned ball
//			OUTPUT:	Nothing
//
//-------------------------------------------------------------------------------------------------
#if 0
void first_snd_proc(int *args)
{
	// Ball may have popped out of his hands
	// Perhaps check to see how far I am over first down line - if way past,
	// don't play snd
	sleep(8);
	if (game_info.ball_carrier == args[0])
	{
		// The first time we go across the 1st down marker, play a sound
		snd_scall_bank(cursor_bnk_str,FIRSTDOWN_SND,VOLUME4,127,PRIORITY_LVL5);
		crowd_cheer_snds();
	}
}
#endif

//-------------------------------------------------------------------------------------------------
//
//  		This process delays before speaking deflected
//			Perhaps we need to check to see if ball is owned, if so, don't do speech
//
//			INPUT:  Nothing
//			OUTPUT:	Nothing
//
//-------------------------------------------------------------------------------------------------
void delay_deflect_proc(int *args)
{
	sleep(30);
	deflected_ball_sp();
	crowd_cheer_snds();
}

//////////////////////////////////////////////////////////////////////////////
#define BALL_FLOOR		0.8f
#define EFFECT_NONE		0
#define EFFECT_BTRAIL	1
#define EFFECT_PCLOUD	2
#define EFFECT_FIRE		3
#define EFFECT_BLINK	4

void ball_proc( int *parg )
{
	struct texture_node	*pnode;
	int					angle;
	float				vx,vy,vz,vxz,vxyz;
	int					effect = EFFECT_NONE;
	fbplyr_data			*ppdata;
	float				prev[3], f;

	// load the ball textures
	pnode = create_texture( "hedshu.wms", TXID_PLAYER, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	if( pnode == NULL )
	{
#ifdef DEBUG
		fprintf( stderr, "Error loading hedshu.wms\n" );
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
	else
		ball_decal = pnode->texture_handle;

	pnode = create_texture( "hedshu2.wms", TXID_PLAYER, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	if( pnode == NULL )
	{
#ifdef DEBUG
		fprintf( stderr, "Error loading hedshu2.wms\n" );
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
	else
		ball_decal2 = pnode->texture_handle;

	ball_ratio = pnode->texture_info.header.aspect_ratio;

	// load the target marker
	pnode = create_texture( "c_x_yel.wms", 0, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	if( pnode == NULL )
	{
#ifdef DEBUG
		fprintf( stderr, "Error loading c_x_yel.wms\n" );
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
	else
		target_decal = pnode->texture_handle;

	// load the ball shadow
	pnode = create_texture( "pslimb.wms", TXID_PLRSHDW, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

//	pnode = create_texture( "shadow.wms", 0, 0, CREATE_NORMAL_TEXTURE,
//		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
//		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
//
	if( pnode == NULL )
	{
#ifdef DEBUG
		fprintf( stderr, "Error loading shadow.wms\n" );
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
	else
		ball_shadow_decal = pnode->texture_handle;

	ball_shadow_ratio = pnode->texture_info.header.aspect_ratio;


	ball_obj.odata.x = 0.0f;
	ball_obj.odata.y = 10.0f;
	ball_obj.odata.z = 0.0f;
	ball_obj.odata.vx = 0.0f;
	ball_obj.odata.vy = 0.0f;
	ball_obj.odata.vz = 0.0f;
	ball_obj.alpha = 0xFF;
	ball_obj.pitch = 0;
	ball_obj.pitch_rate = 40;
	ball_obj.scale = 2.0f;

	create_object( "ball", OID_BALL, OF_NONE, DRAWORDER_BALL, (void *)&ball_obj, draw_ball);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
	while( 1 )
	{
		if ((game_info.ball_carrier == -1) ||
			(ball_obj.flags & BF_PHANTOM))
		{
			if (!(game_info.game_flags & GF_FWD_PASS) &&
				(ball_obj.type == LB_PASS) &&
				!(ball_obj.flags & BF_PHANTOM) &&
				(FIELDX( ball_obj.odata.x ) >= game_info.los))
				game_info.game_flags |= GF_FWD_PASS;

			// save old position
			prev[0] = ball_obj.odata.x;
			prev[1] = ball_obj.odata.y;
			prev[2] = ball_obj.odata.z;

			// add velocities
			if ((!(ball_obj.flags & BF_FREEZE)) && 
				(!(ball_obj.flags & BF_SPINING)))
			{
				ball_obj.odata.x += ball_obj.odata.vx;
				ball_obj.odata.y += ball_obj.odata.vy + GRAVITY / 2.0f;
				ball_obj.odata.z += ball_obj.odata.vz;

				ball_obj.odata.vy += GRAVITY;
			}

			// check for a bounce off the goal posts
			if (!(ball_obj.flags & BF_FG_HITPOST) &&
				!(pcount - ball_obj.bounce_time < 15) &&
				fabs(ball_obj.odata.x) >= GOAL_X &&
				fabs(prev[0]) <= GOAL_X )
			{
				// ball is crossing the plane of a goal.
				// check uprights
				float temp = fabs(ball_obj.odata.z);

				if(( fabs( temp - GOAL_Z) < 1.9f ) &&
					( ball_obj.odata.y >= (GOAL_Y - 1.0f )) &&
					( ball_obj.odata.y <= GOAL_TOP + 2.0f ))
				{
					// Hit an upright -- bounce
					float	btx,btz;
					int		bounce_angle;

					snd_scall_bank(plyr_bnk_str,P_MISS_BALL_SND,VOLUME3,127,PRIORITY_LVL3);

					bounce_angle = randrng(1024);
					btx = ball_obj.odata.vx;
					btz = ball_obj.odata.vz;
					ball_obj.odata.vx = FRELX(btx,btz,bounce_angle);
					ball_obj.odata.vz = FRELZ(btx,btz,bounce_angle);

				//	// set the Z pos of the ball such that it won't bounce again
				//	prev[2] = (ball_obj.odata.z < 0.0f) ? -GOAL_Z : GOAL_Z;
				//	prev[2] += (ball_obj.odata.vz < 0.0f) ? -4.0f : 4.0f;

					ball_obj.odata.x = prev[0] + ball_obj.odata.vx;
					ball_obj.odata.z = prev[2] + ball_obj.odata.vz;
					ball_obj.bounce_time = pcount;

					ball_obj.flags |= BF_FG_HITPOST;
				}
				else
				// check cross-bars
				if(( temp <= ( GOAL_Z + 2.0f)) &&
					( ball_obj.odata.y >= GOAL_Y - 2.0f ) &&
					( ball_obj.odata.y <= GOAL_Y + 2.0f ))
				{
					// Hit a cross-bar -- bounce
					float vel = fsqrt(	ball_obj.odata.vx * ball_obj.odata.vx +
										ball_obj.odata.vy * ball_obj.odata.vy +
										ball_obj.odata.vz * ball_obj.odata.vz );

					// range: +60% to +90%
					int v_per = randrng(31) + 60;

					// range: 0% to 20%
					int z_per = randrng(21);
					// range: none to all of remaining
					int y_per = randrng(101 - z_per);
					// range: what remains
					int x_per = 100 - y_per - z_per;

					snd_scall_bank(plyr_bnk_str,P_MISS_BALL_SND,VOLUME3,127,PRIORITY_LVL3);

					vel *= (float)v_per * 0.01f;
					ball_obj.odata.vx = (ball_obj.odata.vx >= 0.0f ? vel : -vel) * (float)(randrng(4) ?  x_per : -x_per) * 0.01f;
					ball_obj.odata.vy = (ball_obj.odata.vy >= 0.0f ? vel : -vel) * (float)(randrng(3) ? -y_per :  y_per) * 0.01f;
					ball_obj.odata.vz = (ball_obj.odata.vz >= 0.0f ? vel : -vel) * (float)(randrng(2) ?  z_per : -z_per) * 0.01f;

					ball_obj.odata.x = prev[0] + ball_obj.odata.vx;
					ball_obj.odata.y = prev[1] + (ball_obj.odata.vy-GRAVITY) + GRAVITY / 2.0f;
					ball_obj.odata.z = prev[2] + ball_obj.odata.vz;
					ball_obj.bounce_time = pcount;

					ball_obj.flags |= BF_FG_HITPOST;
				}
			}


			// set GF_THROUGH_UPRIGHTS bit if we've just crossed the plane of the goalposts
			f = game_info.off_side ? -1.0f : 1.0f;

			if((ball_obj.type == LB_FIELD_GOAL ) && ( f*prev[0] < GOAL_X ) &&
					( f*ball_obj.odata.x >= GOAL_X ))
			{
				float	t, iy,iz;

				t = (GOAL_X-f*prev[0]) / ( ball_obj.odata.x - f*prev[0]);
				iy = prev[1] + t * (ball_obj.odata.y - prev[1]);
				iz = prev[2] + t * (ball_obj.odata.z - prev[2]);

				if(( iy >= GOAL_Y ) && ( fabs(iz) <= GOAL_Z))
				{
					// it's good!
					game_info.game_flags |= GF_THROUGH_UPRIGHTS;
					game_info.game_mode = GM_PLAY_OVER;
					game_info.play_end_cause = PEC_DEAD_BALL_IB;
					game_info.game_flags |= GF_PLAY_ENDING;
				}
			}


			// hit ground?
			if (ball_obj.odata.y <= (BALL_FLOOR*ball_obj.scale))
			{
				// if the HIKING_BALL flag is set, clear it
				game_info.game_flags &= ~GF_HIKING_BALL;

				// ball has hit the ground.  what type of ball is it?
				if(( game_info.game_mode == GM_IN_PLAY ) &&
				   ( game_info.ball_carrier == -1 ))
				{

					if (ball_obj.bounces++ < 3)
						// ball hit ground sound on first few ground hits
						snd_scall_bank(plyr_bnk_str,BALL_HIT_GRND_SND,VOLUME1,127,PRIORITY_LVL2);

					if( in_bounds( &ball_obj.odata ))
					{

						// grounded in bounds
						switch( ball_obj.type )
						{
							case LB_PASS:
							case LB_BOBBLE:
								// dead ball
								game_info.game_mode = GM_PLAY_OVER;
								game_info.play_end_cause = PEC_DEAD_BALL_IB;
								game_info.game_flags |= GF_PLAY_ENDING;
								break;
							case LB_KICKOFF:
								clock_active = 1;
								// Counter used for slowing clock down if too much time is being burned
								// Make sure we reset this on all other plays that can start the clock
								game_info.play_time = 0;
								break;

							case LB_FIELD_GOAL:
								game_info.game_mode = GM_PLAY_OVER;
								game_info.play_end_cause = PEC_DEAD_BALL_IB;
								game_info.game_flags |= GF_PLAY_ENDING;
								break;

							case LB_LATERAL:
							case LB_FUMBLE:
							case LB_PUNT:
								// just sit there
								break;
						}
					}
				}

				// bounce
				ball_obj.flags |= BF_FG_BOUNCE;

				if( game_info.game_mode == GM_IN_PLAY )
					ball_obj.flags |= BF_BOUNCE;

				if (!(ball_obj.flags & BF_SPINING))
				{
					ball_obj.odata.y = BALL_FLOOR * ball_obj.scale;
					ball_obj.bounce_time = pcount;
				}

				// ball is livelier on onside kicks
				if ((game_info.game_flags & GF_ONSIDE_KICK) &&
					(ball_obj.type == LB_KICKOFF))
				{
					ball_obj.odata.vy *= -0.9;
					ball_obj.odata.vy -= 0.02;
				}
				else if (!(ball_obj.flags & BF_SPINING))
//				else 
				{
					ball_obj.odata.vy *= -0.5;
					ball_obj.odata.vy -= 0.10;
				}

				if( ball_obj.odata.vy <= 0.0f )
				{
					ball_obj.odata.vx = 0.0f;
					ball_obj.odata.vy = 0.0f;
					ball_obj.odata.vz = 0.0f;

					ball_obj.tx = ball_obj.odata.x;
					ball_obj.tz = ball_obj.odata.z;
					ball_obj.ty = 0.0f;
				}
				else
				{
					float	time;
					int		theta;

					vxz = fsqrt( ball_obj.odata.vx * ball_obj.odata.vx +
							ball_obj.odata.vz * ball_obj.odata.vz );
					if( vxz > 0.0f )
						theta = iasin( ball_obj.odata.vz / vxz );
					else
						theta = 0;

					if( ball_obj.odata.vx < 0.0f )
						theta = 512 - theta;

					// ball bounce more crazily on onside kicks
					if ((game_info.game_flags & GF_ONSIDE_KICK) &&
						(ball_obj.type == LB_KICKOFF))
					{
						theta += randrng( 513 );
						theta -= randrng( 513 );
					}
					else
					{
						theta += randrng( 201 );
						theta += randrng( 157 );
						theta += randrng( 157 );
						theta -= randrng( 201 );
						theta -= randrng( 157 );
						theta -= randrng( 157 );
					}

					ball_obj.odata.vx = 0.5f * vxz * icos( theta );
					ball_obj.odata.vz = 0.5f * vxz * isin( theta );

					// plot new X
					time = -2.0f * ball_obj.odata.vy / GRAVITY;
					ball_obj.tx = ball_obj.odata.x + time * ball_obj.odata.vx;
					ball_obj.tz = ball_obj.odata.z + time * ball_obj.odata.vz;
					ball_obj.ty = 0.0f;
				}

			}

			// ball out of bounds?
			if( !in_bounds( &(ball_obj.odata )) && (game_info.game_mode == GM_IN_PLAY))
			{
#ifdef PLAY_DEBUG
				fprintf( stderr, "  ball goes out of bounds at x,z: %5.2f, %5.2f, field %d\n",
					ball_obj.odata.x,ball_obj.odata.z,FIELDX(ball_obj.odata.x));
				fprintf( stderr, "  ball type : %s flags: ", loose_ball_names[ball_obj.type] );
				print_ball_flags();
#endif
				game_info.game_mode = GM_PLAY_OVER;
				game_info.game_flags |= GF_PLAY_ENDING;
				game_info.play_end_cause = PEC_BALL_OOB;
				game_info.play_end_pos = ball_obj.odata.x;
				game_info.play_end_z = ball_obj.odata.z;

				// stop ball
				if (ball_obj.bounces > 2)
				{
					ball_obj.odata.vx = 0.0f;
					ball_obj.odata.vy = 0.0f;
					ball_obj.odata.vz = 0.0f;
				}
			}

			ball_obj.flight_time += 1;
			ball_obj.flags &= ~BF_WHITE;

			// ball special effects
			switch( ball_obj.type )
			{
				case LB_PASS:
					if( ball_obj.flags & BF_FIRE_PASS )
						effect = EFFECT_FIRE;
					else if( ball_obj.flags & BF_TURBO_PASS )
						effect = EFFECT_PCLOUD;
					else
						effect = EFFECT_BTRAIL;
					break;

				case LB_BOBBLE:
					// just do whatever you were doing before
					break;

				case LB_LATERAL:
				case LB_FUMBLE:
					effect = EFFECT_NONE;
					break;

				case LB_KICKOFF:
				case LB_PUNT:
					if( game_info.game_mode != GM_IN_PLAY )
						effect = EFFECT_NONE;
					else if( ball_obj.flags & BF_BOUNCE )
						effect = EFFECT_BLINK;
					else
						effect = EFFECT_BTRAIL;
					break;

				case LB_FIELD_GOAL:
					if( ball_obj.flags & BF_BOUNCE )
						effect = EFFECT_BLINK;
					else if( game_info.team_fire[game_info.off_side] )
						effect = EFFECT_FIRE;
					else
						effect = EFFECT_NONE;
					break;
			}

			// no effects once play is dead
			if( game_info.game_mode == GM_PLAY_OVER )
				effect = EFFECT_NONE;

			switch( effect )
			{
				case EFFECT_PCLOUD:
//					break;
				case EFFECT_BTRAIL:
//					set_process_run_level( qcreate( "balltrl", BALL_PID, trail_ball_proc, 0, 0, 0, 0 ), RL_BALL );
					qcreate( "balltrl", BALL_PID, trail_ball_proc, 0, 0, 0, 0 );
					break;
				case EFFECT_FIRE:
					if( ball_obj.flight_time % 2 == 0 )
					{
						// create smoke puff and proc
						qcreate( "smkpuf", PLAYER_PID, ball_fire_trail_proc, 4, 2, 0, 0 );
						qcreate( "smkpuf", PLAYER_PID, ball_fire_trail_proc, 3,-1, 2, 0 );
						qcreate( "smkpuf", PLAYER_PID, ball_fire_trail_proc, 3,-1,-2, 0 );
					}
					break;
				case EFFECT_BLINK:
					if( ball_obj.flight_time % 10 == 0 )
						ball_obj.flags |= BF_WHITE;
					break;
				case EFFECT_NONE:
				default:
					break;
			}

			// heading
			vx = ball_obj.odata.vx;
			vy = ball_obj.odata.vy;
			vz = ball_obj.odata.vz;
			vxz = fsqrt( vx*vx + vz*vz );
			vxyz = fsqrt( vx*vx + vy*vy + vz*vz );

			if(( vx == 0.0f ) && ( vz == 0.0f ))
			{
				if((( game_info.game_mode == GM_PRE_SNAP ) ||
						( game_info.game_mode == GM_PRE_KICKOFF )) &&
						( game_info.play_type == PT_KICKOFF ))
				{
					ball_obj.odata.fwd2 = 384;
					ball_obj.odata.fwd = ( game_info.off_side ? 768 : 256 );
				}
				else
				{
					if (ball_obj.flags & BF_SPINING)
					{
		  				ball_obj.phi += 80;
						if (ball_obj.odata.fwd2 < 256)
							ball_obj.odata.fwd2 += 5;
						else
							ball_obj.odata.fwd2 = 256;

					}
					else
						ball_obj.odata.fwd2 = 0;
				}
			}
			else
			{
				angle = iasin( vx / vxz );

				if( vz < 0.0f )
					angle = 1023 & (512 - angle);

				ball_obj.odata.fwd = angle;

				// pitch
				if( ball_obj.flags & BF_VEL_PITCH )
				{
					angle = iasin( vy / vxyz );
					ball_obj.odata.fwd2 = angle;
				}
				else
				{
					ball_obj.pitch += ball_obj.pitch_rate;
					ball_obj.odata.fwd2 = ball_obj.pitch % 1024;
				}

				// roll
				if(( ball_obj.type == LB_PASS ) && !( ball_obj.flags & BF_BOBBLE ))
	  				ball_obj.phi += 60;
			}

		}
		else
		{
			float	rx,ry,rz, *tm;

			game_info.last_carrier = game_info.ball_carrier;
			ppdata = player_blocks + game_info.ball_carrier;

			// ball is in someone's hand.  set it's position to that players
			rx = ppdata->jpos[JOINT_LWRIST][0];
			ry = ppdata->jpos[JOINT_LWRIST][1];
			rz = ppdata->jpos[JOINT_LWRIST][2];

			tm = cambot.xform;

			ball_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
			ball_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
			ball_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;
		}

		// if play_type is NORMAL, FIELD_GOAL, PUNT, or EXTRA POINT and ball is past
		// los, set GF_BALL_CROSS_LOS
		if(( game_info.play_type != PT_KICKOFF ) &&
			!( game_info.game_flags & GF_BALL_CROSS_LOS ) &&
			( FIELDX(ball_obj.odata.x) >= game_info.los ))
		{
			game_info.game_flags |= GF_BALL_CROSS_LOS;
		}

		sleep( 1 );
	}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////////
void draw_ball( void *oi )
{
	float		mat1[12], mat2[12], mat3[12];
	float		bsf;

	obj_3d	*pobj = (obj_3d *)(oi);
	ball_data	*pball = (ball_data *)(oi);

	if ((game_info.ball_carrier == -1) ||
		(ball_obj.flags & BF_PHANTOM))
	{
		grConstantColorValue( pball->alpha << 24 );
		guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
		grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
		grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

		grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
		grDepthBufferFunction( GR_CMP_LESS );
		grDepthMask( FXTRUE );

		grChromakeyMode( GR_CHROMAKEY_DISABLE );

		if( ball_obj.flags & BF_WHITE )
		{
#if ! defined(GLIDE3)
			guColorCombineFunction(GR_COLORCOMBINE_ONE);
#else
			grConstantColorValue(0xff000000);
			guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA);
#endif
		}
		else
			guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);

		grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
				GR_BLEND_ONE_MINUS_SRC_ALPHA,
				GR_BLEND_ONE,
				GR_BLEND_ZERO );

		mat1[3] = 0.0f;	mat1[7] = 0.0f;	mat1[11] = 0.0f;
		mat3[3] = 0.0f;	mat3[7] = 0.0f;	mat3[11] = 0.0f;

		rotx( -pobj->fwd2, mat3 );
		roty( pobj->fwd, mat1 );
		mxm( mat1, mat3, mat2 );
		rotz( pball->phi, mat1 );
		mxm( mat2, mat1, mat3 );

		mat3[3] = pobj->x - cambot.x;
		mat3[7] = pobj->y - cambot.y;
		mat3[11] = pobj->z - cambot.z;

		bsf = pball->scale;

		if (pup_bigball)
			bsf *= 1.5f;

		mat3[0] *= bsf;	mat3[1] *= bsf;	mat3[2] *= bsf;
		mat3[4] *= bsf;	mat3[5] *= bsf;	mat3[6] *= bsf;
		mat3[8] *= bsf;	mat3[9] *= bsf;	mat3[10] *= bsf;

		mxm( cambot.xform, mat3, mat1 );

		if( pball->flags & BF_ALT )
			render_limb( &limb_medball_obj_big, mat1, &ball_decal2, &ball_ratio );
		else
			render_limb( &limb_medball_obj_big, mat1, &ball_decal, &ball_ratio );
	}
}

//////////////////////////////////////////////////////////////////////////////
static void trail_ball_proc( int *parg )
{
	ball_data	bobj;
	struct object_node *myball;

	bobj = ball_obj;
	bobj.flags = BF_ALT;
	bobj.alpha = 215;
	bobj.scale = 2.25f;

	myball = create_object( "ball", OID_BALL, OF_NONE, DRAWORDER_BALL, (void *)&bobj, draw_ball);
	sleep( 1 );
	bobj.alpha = 200;
	bobj.scale = 2.15f;
	sleep( 1 );
	bobj.alpha = 185;
	bobj.scale = 2.05f;
	sleep( 1 );
	bobj.alpha = 170;
	bobj.scale = 1.95f;
	sleep( 1 );
	bobj.alpha = 155;
	bobj.scale = 1.85f;
	sleep( 1 );
	delete_object( myball );
}

//////////////////////////////////////////////////////////////////////////////
void load_uniform( int side, int team, int home_away, int white_black )
{
	// elements 0-6 are uniform & ball.  elements 7 and 8 are special heads
	// 7 is the first (leftmost) custom head on a team.  8 is for the second.
	char	hedshu[12], leghel[12], jersf[12], slvmsk[12];
	char	armhnd[12], jnum[12],ball[12];
	char	*files[10] = {armhnd,hedshu,jersf,leghel,slvmsk,jnum,ball,NULL,NULL,NULL};
	int		ratios[9];	/* junk */
	int		head_count = 0;

	sprintf( armhnd, white_black == PC_WHITE ? "ARMHND.WMS" : "ARMHND2.WMS" );

	sprintf( hedshu, white_black == PC_WHITE ? "HEDSHU.WMS" : "HEDSHU2.WMS" );

	sprintf( leghel, "%s%cLOD.WMS", teaminfo[team].prefix,
		( home_away == HA_HOME ) ? 'H' : 'A' );
	sprintf( jersf, "%s%cLOD.WMS", teaminfo[team].prefix,
		( home_away == HA_HOME ) ? 'H' : 'A' );
	sprintf( slvmsk, "%s%cLOD.WMS", teaminfo[team].prefix,
		( home_away == HA_HOME ) ? 'H' : 'A' );
	sprintf( jnum, "%s%cLOD.WMS", teaminfo[team].prefix,
		( home_away == HA_HOME ) ? 'H' : 'A' );
	
	sprintf( ball, "HEDSHU.WMS" );

	// assign secret head texture to array element 7 if there is one
	if (four_plr_ver)
	{
		if (game_info.team_head[(side/2)*2] != -1)
		{
			files[7] = head_images[game_info.team_head[(side/2)*2]];
			head_count = 1;
		}
		if (game_info.team_head[(side/2)*2+1] != -1)
		{
			files[7+head_count] = head_images[game_info.team_head[(side/2)*2+1]];
		}
	}
	else
	{
		if (game_info.team_head[side/2] != -1)
		{
			files[7] = head_images[game_info.team_head[side/2]];
//			#ifdef DEBUG
//			fprintf( stderr, "Loading %s...", files[7] );
//			#endif
		}
	}

	load_textures( files, ratios, team_decals[side], 10, TXID_PLAYER,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

//#ifdef DEBUG
//	if( files[7] )
//	{
//		if( team_decals[side][7] )
//			fprintf( stderr, "ok\n" );
//		else
//			fprintf( stderr, "failed\n" );
//	}
//#endif
}

//////////////////////////////////////////////////////////////////////////////
int plyr_rand( obj_3d *pobj, int arg )
{
	return( rand() % arg );
}

//////////////////////////////////////////////////////////////////////////////
int rand_func( obj_3d *pobj, int arg )
{
	if (randrng(100) < arg)
		return 1;
	else
		return 0;
}


//////////////////////////////////////////////////////////////////////////////
void maybe_get_up_b_hurt(obj_3d *pobj)
{
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;

	// a non-ball-carrier weapon...dont be injured!!!
	if ((ppdata->plyrnum == game_info.ball_carrier) && (randrng(100) < 30))
	 	change_anim(pobj, get_up_b_hurt_tbl[randrng(sizeof(get_up_b_hurt_tbl)/sizeof(char *))]);
}

//////////////////////////////////////////////////////////////////////////////
void maybe_get_up_c_hurt(obj_3d *pobj)
{
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;

	if (game_info.game_mode == GM_IN_PLAY)
		return;

	// a non-ball-carrier weapon...dont be injured!!!
	if ((ppdata->plyrnum == game_info.ball_carrier) && (randrng(100) < 30))
	 	change_anim(pobj, get_up_c_hurt_tbl[randrng(sizeof(get_up_c_hurt_tbl)/sizeof(char *))]);
}


//////////////////////////////////////////////////////////////////////////////
void maybe_qb_mad(obj_3d *pobj)
{
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;

	if (game_info.game_mode == GM_IN_PLAY)
		return;

	if ((pobj->plyrmode == MODE_QB) &&
	 	(ppdata->plyrnum == game_info.ball_carrier) &&
		(game_info.game_mode == GM_PLAY_OVER) &&
		(!taunted) &&
	    (randrng(100) < 40))
	{
		taunted = 1;		// dont taunt dude... getting up!!
	 	change_anim(pobj, mad_qb_anim_tbl[randrng(sizeof(mad_qb_anim_tbl)/sizeof(char *))]);
	}
}

//////////////////////////////////////////////////////////////////////////////
void maybe_receiver_mad(obj_3d *pobj)
{
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;

	// only be mad if PLAY OVER or ENDING and intended receiver
	if ((ppdata->plyrnum == ball_obj.int_receiver) &&
		(game_info.game_mode == GM_PLAY_OVER) &&
		(ball_obj.type == LB_PASS) &&
		(game_info.ball_carrier == -1) &&
		(!taunted) &&
	    (randrng(100) < 50))
	{
		taunted = 1;		// dont taunt dude... getting up!!
	 	change_anim(pobj, miss_pass_reactions_tbl[randrng(sizeof(miss_pass_reactions_tbl)/sizeof(char *))]);
	}
}

//////////////////////////////////////////////////////////////////////////////
int plyr_have_ball( obj_3d *pobj )
{
	// if the play isn't over, the carrier (me) hasn't been downed.  This function
	// is used to determine if he'll get up after the hit, and if he hasn't been
	// downed, he should.
	if (game_info.game_mode == GM_IN_PLAY)
		return FALSE;

	// have player get up...if tackled in end-zone
//	if (in_end_zone(pobj) == -1)
//		return FALSE;

	if (pobj->flags & PF_POP_HELMET)
		return TRUE;

	if ((((fbplyr_data *)pobj)->plyrnum == game_info.ball_carrier) && (randrng(100) < 90))
		return TRUE;
	else
		return FALSE;
}

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
static void dump_joint_positions( void )
{
	int		i, j;
	float	rx,ry,rz, *tm, tx,ty,tz;
	fbplyr_data *ppdata;
	obj_3d	*pobj;

	tm = cambot.xform;

	for( i = 0; i < 14; i++ )
	{
		ppdata = player_blocks + i;
		pobj = (obj_3d *)ppdata;

		if(!( pobj->flags & PF_ONSCREEN ))
			continue;

		fprintf( stderr, "%2x: %10s:%2f\n",
				ppdata->static_data->number,
				seq_names[pobj->adata[0].seq_index], pobj->adata[0].fframe );

		for( j = 0; j < 15; j++ )
		{
			rx = ppdata->jpos[j][0];
			ry = ppdata->jpos[j][1];
			rz = ppdata->jpos[j][2];
			tx = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
			ty = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
			tz = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

			tx -= ppdata->odata.x;
			ty -= ppdata->odata.y;
			tz -= ppdata->odata.z;

			// rotate x and z offsets into player frame
			rx = FRELX(tx,tz,-pobj->fwd);
			rz = FRELZ(tx,tz,-pobj->fwd);

			fprintf( stderr, "  joint %d: %f,%f(%f),%f\n", j, rx, ty, ty + ppdata->odata.y, rz );
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
void bobble_pass( fbplyr_data *ppdata, float bx, float by, float bz )
{
	float	a,b,c,b2m4ac,root1,root2;
	float	rx,ry,rz, *tm;

	// inc stat
	tdata_blocks[ppdata->team].bobbles++;

	// start point
	rx = ppdata->jpos[JOINT_LWRIST][0];
	ry = ppdata->jpos[JOINT_LWRIST][1];
	rz = ppdata->jpos[JOINT_LWRIST][2];

	tm = cambot.xform;

	ball_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	ball_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	ball_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	ball_obj.odata.vx = bx + ((float)randrng(12) - 6.0f) * 0.10f;
	ball_obj.odata.vz = bz + ((float)randrng(12) - 6.0f) * 0.10f;
	ball_obj.odata.vy = by + ((float)randrng(10) + 10.0f) * 0.10f;

	ball_obj.flight_time = 0;
	ball_obj.int_receiver = -1;
	if ((ball_obj.type == LB_PASS) ||
		(ball_obj.type == LB_BOBBLE))
		ball_obj.type = LB_BOBBLE;
	else
		ball_obj.type = LB_FUMBLE;

	ball_obj.who_threw = ppdata->plyrnum;
	game_info.ball_carrier = -1;

	// tell game flags that there's been a bobble already
	game_info.game_flags |= GF_BOBBLE;
	game_info.game_flags &= ~GF_PLAY_ENDING;
	ball_obj.flags |= BF_BOBBLE;

	// reset target x for where it's likely to hit.
	a = GRAVITY / 2.0f;
	b = ball_obj.odata.vy;
	c = ball_obj.odata.y;

	root1 = solve_quad( a, b, c );

	#ifdef 0 //DEBUG
	if (root1 == 100000.0f)
	{
		fprintf( stderr, "THIS IS THE CRASH FROM FRIAR TUCK'S/HALLWAY\n" );
		lockup();
	}
	#endif

//	b2m4ac = b * b - 4.0f * a * c;
//	root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
//	root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
//
//	if( root1 < root2 )
//		root1 = root2;

	ball_obj.tx = ball_obj.odata.x + root1 * ball_obj.odata.vx;
	ball_obj.tz = ball_obj.odata.z + root1 * ball_obj.odata.vz;
	ball_obj.ty = 0.0f;

}

//////////////////////////////////////////////////////////////////////////////
void fumble_ball( fbplyr_data *ppdata, float bx, float bz )
{
	float	a,b,c,b2m4ac,root1,root2;
	float	rx,ry,rz, *tm;

	// make sounds
	fumble_ball_sounds();

	// inc stat
	tdata_blocks[ppdata->team].fumbles += 1;

	// if fumbler is in MODE_QB, go to WPN
	if( ppdata->odata.plyrmode == MODE_QB )
		ppdata->odata.plyrmode = MODE_WPN;

	// start point
	rx = ppdata->jpos[JOINT_LWRIST][0];
	ry = ppdata->jpos[JOINT_LWRIST][1];
	rz = ppdata->jpos[JOINT_LWRIST][2];

	tm = cambot.xform;

	ball_obj.odata.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
	ball_obj.odata.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
	ball_obj.odata.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;

	ball_obj.odata.vx = bx + ((float)randrng(8) - 4.0f) * 0.30f;			// 0.10f
	ball_obj.odata.vz = bz + ((float)randrng(8) - 4.0f) * 0.30f;			// 0.10f
	ball_obj.odata.vy = (float)randrng(4);		//0.6f;	// 0.10f
	ball_obj.flight_time = 0;
	ball_obj.int_receiver = -1;
	ball_obj.type = LB_FUMBLE;
	ball_obj.who_threw = ppdata->plyrnum;
	ball_obj.flags &= ~(BF_BOUNCE|BF_BOBBLE|BF_FIRE_PASS|BF_TURBO_PASS|BF_FG_BOUNCE|BF_FG_HITPOST);
	game_info.ball_carrier = -1;
	game_info.fumble_x = ppdata->odata.x;

	// tell game flags that there's been a fumble
	game_info.game_flags |= GF_FUMBLE;
	game_info.game_flags &= ~GF_PLAY_ENDING;

	// reset target x for where it's likely to hit.
	a = GRAVITY / 2.0f;
	b = ball_obj.odata.vy;
	c = ball_obj.odata.y;

	b2m4ac = b * b - 4.0f * a * c;
	if (b2m4ac < 0.0f)
	{
		b2m4ac = 0.0f;
		root1 = 0.0f;
		root2 = 0.0f;
	}
	else
	{
		root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
		root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	}

	if( root1 < root2 )
		root1 = root2;

	ball_obj.tx = ball_obj.odata.x + root1 * ball_obj.odata.vx;
	ball_obj.tz = ball_obj.odata.z + root1 * ball_obj.odata.vz;
	ball_obj.ty = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
// watches for offensive player A and B buttons down.  If it sees them,
// turns motion man into a blocker.  Created at snap, lives for only a few
// ticks thereafter.
static void motion_block_monitor( int *args )
{
	int		life = 2, but, station;

	station = args[0];

	// if I've got a teammate, die
	if ((station < 0) ||
		(four_plr_ver && ((p_status & TEAM_MASK(game_info.off_side)) == TEAM_MASK(game_info.off_side)) ))
		die(0);

	while (life)
	{
		// die if there's been a turnover. (Pretty damn fast.)
		if (game_info.off_side != game_info.off_init)
			die(0);

//fprintf(stderr,"motion_block_monitor() station:%d\r\n",station);

		but = (get_player_sw_current() >> (station<<3)) & P_ABCD_MASK;

		if ((but & (P_A|P_B)) == (P_A|P_B))
		{
			drone_motion_block();
			die(0);
		}
		sleep( 1 );
		life -= 1;
	}
}

//////////////////////////////////////////////////////////////////////////////
// watches for A+B after a QB throw
static void qb_throw_monitor( int *args )
{
	int		but, i;
	int		plyr = args[0];

	target_human = FALSE;

	// quit right now if I'm not human
	if (ISDRONE(player_blocks+plyr))
		die(0);

	sleep( 1 );

	last_pass_hist[0] = player_blocks[plyr].but_hist[0];
	last_pass_hist[1] = player_blocks[plyr].but_hist[1];
	last_pass_hist[2] = player_blocks[plyr].but_hist[2];
	last_pass_hist[3] = player_blocks[plyr].but_hist[3];
	last_pass_hist[4] = player_blocks[plyr].but_hist[4];

	// quit right now if I have no station
	if (player_blocks[plyr].station < 0)
		die(0);

	but = (get_player_sw_current() >> ((player_blocks[plyr].station)<<3)) & P_ABCD_MASK;
	if ((but & P_A) && (but & P_B))
	{
		#ifdef TURMELL_DEBUG
		// dump switch history
		fprintf( stderr, "but_hist: %c%c%c %c%c%c %c%c%c %c%c%c %c%c%c\n",
			player_blocks[plyr].but_hist[0] & P_A ? 'A' : '-',
			player_blocks[plyr].but_hist[0] & P_B ? 'B' : '-',
			player_blocks[plyr].but_hist[0] & P_C ? 'C' : '-',
	
			player_blocks[plyr].but_hist[1] & P_A ? 'A' : '-',
			player_blocks[plyr].but_hist[1] & P_B ? 'B' : '-',
			player_blocks[plyr].but_hist[1] & P_C ? 'C' : '-',

			player_blocks[plyr].but_hist[2] & P_A ? 'A' : '-',
			player_blocks[plyr].but_hist[2] & P_B ? 'B' : '-',
			player_blocks[plyr].but_hist[2] & P_C ? 'C' : '-',

			player_blocks[plyr].but_hist[3] & P_A ? 'A' : '-',
			player_blocks[plyr].but_hist[3] & P_B ? 'B' : '-',
			player_blocks[plyr].but_hist[3] & P_C ? 'C' : '-',

			player_blocks[plyr].but_hist[4] & P_A ? 'A' : '-',
			player_blocks[plyr].but_hist[4] & P_B ? 'B' : '-',
			player_blocks[plyr].but_hist[4] & P_C ? 'C' : '-' );
		#endif

		// make sure A button has been down less than five ticks
		for( i = 0; i < 3; i++ )
		{
			if (!(player_blocks[plyr].but_hist[i] & P_A))
			{	// A is a new press
				target_human = TRUE;
				return;
			}
		}
	} // if (A+B)
}

//////////////////////////////////////////////////////////////////////////////
// returns player number of the human player on target's
// team farthest away from target.
// there MUST be a human player on the team besides target
// or there will be trouble.
static int farthest_teammate( fbplyr_data *target )
{
	int		i, tgt = target->plyrnum;
	int		far;

	far = target->plyrnum;

	for( i = 7*target->team; i < 7 + 7 * target->team; i++ )
	{
		if ((ISHUMAN(player_blocks + i)) &&
			(distxz[tgt][i] > distxz[tgt][far]))
			far = i;
	}

	return far;
}

//////////////////////////////////////////////////////////////////////////////
static void check_receiver_fire( fbplyr_data *ppdata )
{
	forward_catcher = ppdata->plyrnum;

	if (tdata_blocks[ppdata->team].last_receiver == ppdata->plyrnum)
	{
		// increment completions
		tdata_blocks[ppdata->team].consecutive_comps++;
		if (tdata_blocks[ppdata->team].consecutive_comps > OFFENSE_FIRECNT-1)
		{
			game_info.team_fire[ppdata->team] = 1;

			#ifdef FIRE_DEBUG
			fprintf( stderr, "FIRE: Offense catches fire\n" );
			#endif
		}
		else
		{
			#ifdef FIRE_DEBUG
			fprintf( stderr, "FIRE: Pass %d to receiver %x\n",
				tdata_blocks[ppdata->team].consecutive_comps,
				JERSEYNUM(ppdata->plyrnum));
			#endif
		}
	}
	else
	{
		#ifdef FIRE_DEBUG
		fprintf( stderr, "FIRE: First pass to receiver %x\n", JERSEYNUM(ppdata->plyrnum));
		#endif
		tdata_blocks[ppdata->team].last_receiver = ppdata->plyrnum;
		tdata_blocks[ppdata->team].consecutive_comps = 1;
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void clean_audible_text(int tm_nbr)
{
	int sid;
	int oid;

	sid = (tm_nbr ? SID_AUDIBLE_T2 : SID_AUDIBLE_T1);
	oid = (tm_nbr ? OID_AUDIBLE_T2 : OID_AUDIBLE_T1);

	del1c( oid, -1 );
	delete_multiple_strings(sid, 0xffffffff);
	killall(AUDIBLE_MSG_PID+tm_nbr, -1);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//void print_audible_message(int *pargs)
void print_audible_message(int off_def, int audible_num, int station, int tm_nbr, int first)
{
	PLAYSTUF__SLOT * slot_ptr;
	sprite_info_t *	bx_obj;
	signed short id, flip;

	float	x, y;
	int		sid, oid;
//	int		off_def     = pargs[0];
//	int		audible_num = pargs[1];
//	int		station     = pargs[2];
//	int		tm_nbr      = pargs[3];
	char	sz_pam[PLAYSTUF__AUDIBLES_PER_PLAYER][24];
	
	for (sid = 0; sid < PLAYSTUF__AUDIBLES_PER_PLAYER; sid++)
	{
		id = audible_ids[station].ids[off_def][sid];
		flip = id < 0;
		if (flip)
			id = -id;
		id--;
		oid = (id / 9) == PLAYSTUF__OFF_PAGE_ID_CUSTOM;

		if (oid && (pup_playbook == 2))
		{
			sprintf(sz_pam[sid],"NO CUSTOM");
		}
		else
		if (!pup_showaudibles)
		{
			sprintf(sz_pam[sid],"AUDIBLE %d", sid+1);
		}
		else
		if (oid && (off_def == OFFENSE))
		{
			sprintf(sz_pam[sid], "%s %s%s",
				custom_plays[station][id % 9].name1,
				custom_plays[station][id % 9].name2,
				(flip ? " :F":""));
		}
		else
		{
			slot_ptr = PlayStuf__GetSlot(off_def, id / 9, id % 9);
			sprintf(sz_pam[sid], "%s%s", slot_ptr->name, (flip ? " :F":""));
		}
	}

	sid = (tm_nbr ? SID_AUDIBLE_T2 : SID_AUDIBLE_T1);
	oid = (tm_nbr ? OID_AUDIBLE_T2 : OID_AUDIBLE_T1);

	x = (tm_nbr ? 442.0f : 70.0f);
	y = 192.0f;

	clean_audible_text(tm_nbr);

	bx_obj = beginobj( &aud_menu, x, y, 1.4f, SCR_PLATE_TID );
	bx_obj->id = oid;
	if (first)
	{
		iqcreate("flash",AUDIBLE_MSG_PID+tm_nbr,flash_obj_proc,(int)bx_obj,3,2,0);
	}

	set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );
	set_text_transparency_mode( TRANSPARENCY_ENABLE );
	set_text_font( FONTID_BAST10 );
	set_string_id( sid );

	set_text_position(x, y+36.0f, 1.2f);
	oprintf("%dc%s", LT_YELLOW, teaminfo[game_info.team[tm_nbr]].name);

	set_text_font( FONTID_BAST8T );

	set_text_position(x, y+11, 1.2f);
	set_text_color( audible_num == 0 ? LT_YELLOW : LT_GRAY );
	oprintf("ORIGINAL PLAY");

	set_text_position(x, y-4, 1.2f);
	set_text_color( audible_num == 1 ? LT_YELLOW : LT_GRAY );
	oprintf("%s", sz_pam[0]);

	set_text_position(x, y-19, 1.2f);
	set_text_color( audible_num == 2 ? LT_YELLOW : LT_GRAY );
	oprintf("%s", sz_pam[1]);
	
	set_text_position(x, y-34, 1.2f);
	set_text_color( audible_num == 3 ? LT_YELLOW : LT_GRAY );
	oprintf("%s", sz_pam[2]);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//ppdata - ptr to player block
//off_def - 0=offense, 1=defense

static int audible_shown[2];


static int audible_check(fbplyr_data *ppdata,int off_pos)
{
	play_t * aud_play = NULL;
	int	disallow[PLAYSTUF__NUM_AUDIBLE_PLAYS] = {0,0,0,0};
	int	input;
	int	joy;
	int	sw;
	int	do_it_flag;
	int	id, i;
	int	pnum;
	int	tm_nbr;
//	int	args[4];

	do_it_flag = 0;
	tm_nbr = ppdata->team;

	if (game_info.game_flags & (GF_ONSIDE_KICK | GF_QUICK_KICK))
		return 1;

	if (game_info.game_flags & GF_HIKING_BALL)
		return 1;

	if (game_info.game_mode != GM_PRE_SNAP)
		return 1;

	if (pup_noplaysel)
		return 1;
	
	if (!ISHUMAN(ppdata))
		return 1;

//	if (game_info.has_custom[ppdata->station] == FALSE)
//		return 1;

	if (audible_ids[ppdata->station].ids[off_pos][0] == 0)
		return 1;

	if (game_info.auto_hike_cntr > (MAX_HIKE_TIME-3))		// Check we are delaying auto_hike_cntr for some reason!
		return 1;
		 
	if (off_pos == OFFENSE && pup_playbook == 2)
	{
		for (i = 0; i < PLAYSTUF__AUDIBLES_PER_PLAYER; i++)
		{
			id = audible_ids[ppdata->station].ids[OFFENSE][i];
			if (id < 0)
				id = -id;
			disallow[i+1] = (--id / PLAYSTUF__PLAYS_PER_PAGE) == PLAYSTUF__OFF_PAGE_ID_CUSTOM;
		}
	}

	// Determine which stick we should be paying attention to
	pnum = game_info.audible_plyr[tm_nbr];

//	fprintf(stderr, "%d - %d - %d\n", pnum, ppdata->station, game_info.captain);


#if 1
	if (pnum == ppdata->station)
	{
		// read double tap of TURBO (C) button
		if( ppdata->but_cur & P_C )
		{

			// must hit turbo within 10 ticks of last release for audible menu
			if ( (audible_turbo_timer[tm_nbr] > 0) &&  (audible_turbo_timer[tm_nbr] < AUDIBLE_DOUBLE_TAP_TIME_LIMIT))
			{
				audible_turbo_timer[tm_nbr] = -1;
				if (!audible_shown[tm_nbr])
					audible_tap_action[tm_nbr] = 1;
			} else {
				if (!audible_shown[tm_nbr])
					audible_turbo_timer[tm_nbr] = 0;
			}
		} else {
			audible_turbo_timer[tm_nbr]++;

		}

		// Check for audible action
		if (audible_tap_action[tm_nbr] == 1)
		{
			if (!(audible_shown[tm_nbr]) )
			{
				// First time audbile hitter
				print_audible_message(off_pos, game_info.audible_num[off_pos], pnum, tm_nbr, 1);

				snd_scall_bank(cursor_bnk_str,100,VOLUME4,(tm_nbr ? 255 : 0),PRIORITY_LVL1);

				audible_shown[tm_nbr] = 1;
			} else {
				// Check for stick movment
				//	Start doing the audible process (put up the AUDIBLE MODE info box and check for UP/DOWN joystick)
				do_it_flag = 0;
				input = (get_player_sw_close() >> (ppdata->station << 3)) & P_SWITCH_MASK;

				if(input & P_UP)
				{
					i = game_info.audible_num[off_pos];
					do
					{
						if(game_info.audible_num[off_pos] <= 0)
							game_info.audible_num[off_pos] = (PLAYSTUF__NUM_AUDIBLE_PLAYS - 1);
						else
							game_info.audible_num[off_pos]--;
					} while (disallow[game_info.audible_num[off_pos]]);

					do_it_flag = (i == game_info.audible_num[off_pos]) ? -1 : 1;
//					audible_turbo_timer[tm_nbr] = 4;
				}
				else if(input & P_DOWN)
				{
					i = game_info.audible_num[off_pos];
					do
					{
						if(game_info.audible_num[off_pos] >= (PLAYSTUF__NUM_AUDIBLE_PLAYS - 1))
							game_info.audible_num[off_pos] = 0;
						else
							game_info.audible_num[off_pos]++;
					} while (disallow[game_info.audible_num[off_pos]]);
	
					do_it_flag = (i == game_info.audible_num[off_pos]) ? -1 : 1;
//					audible_turbo_timer[tm_nbr] = 4;
				}

				
				// Check for a button press!!!
				if (audible_turbo_timer[tm_nbr] > 3)
				{
//fprintf(stderr, "%d\n", audible_turbo_timer[tm_nbr]);
					if ( (ppdata->but_cur & (P_A|P_B|P_C)) || (audible_turbo_timer[tm_nbr] > 120))
					{
						audible_tap_action[tm_nbr] = 0;
						do_it_flag = 0;
						clean_audible_text(tm_nbr);
						snd_scall_bank(cursor_bnk_str,102,VOLUME4,(tm_nbr ? 255 : 0),PRIORITY_LVL1);

						if (ppdata->but_cur & P_C) 
							ppdata->but_cur ^= P_C;

						// Put autohike counter back up so we can ignore button strokes!
						game_info.auto_hike_cntr= (MAX_HIKE_TIME - (tsec/2));
					}
				}
			}
		}



		if (do_it_flag)
		{
			snd_scall_bank(cursor_bnk_str,101,VOLUME4,(tm_nbr ? 255 : 0),PRIORITY_LVL1);

			// call print audible #
			clean_audible_text(tm_nbr);
			print_audible_message(off_pos, game_info.audible_num[off_pos], pnum, tm_nbr, 0);

			if(do_it_flag > 0)
			{
				aud_play = PlayStuf__GetAudiblePlay( ppdata->station, off_pos, game_info.audible_num[off_pos] );
#ifdef DEBUG
				if(aud_play == NULL)
				{
					fprintf(stderr, "bogus audible play ptr!\n");
					fflush(stdout);
					lockup();
				}
#endif
				// mark that an audible is active
				game_info.audible_flags[off_pos] = 1;

				// set the new play
				game_info.team_play[tm_nbr] = aud_play;

				return 0;
			}

		}

	}
#else
	if (pnum == ppdata->station)
	{
	//	The turbo timer is used to detect a double-tap for audibles.
	//	If the TURBO button is pressed (the first time), the turbo timer is started.
	//	Then, if the TURBO button is pressed again within the time limit (6 ticks),
	//	a "double-tap" occured. After that, if the TURBO button is currently held down
	//	and a "double-tap" already occured, then check for LEFT or RIGHT joystick 
	//	action to perform the next audible.

		if (ppdata->but_cur & P_C)
		{
			if((audible_tap_action[tm_nbr] == 0))
			{
				audible_tap_action[tm_nbr] = 1;
			}
			else
			if(audible_tap_action[tm_nbr] == 1)
			{
				// must hit turbo within X ticks of last release for double-tap
				if((audible_turbo_timer[tm_nbr] > 0) && (audible_turbo_timer[tm_nbr] < AUDIBLE_DOUBLE_TAP_TIME_LIMIT))
				{
					audible_tap_action[tm_nbr] = 2;
					print_audible_message(off_pos, game_info.audible_num[off_pos], pnum, tm_nbr, 1);
					if (tm_nbr)
						snd_scall_bank(cursor_bnk_str,100,VOLUME4,255,PRIORITY_LVL1);
					else
						snd_scall_bank(cursor_bnk_str,100,VOLUME4,0,PRIORITY_LVL1);
				}
			}
		}
		else
		{
			if(audible_tap_action[tm_nbr] == 1)
			{
				if(audible_turbo_timer[tm_nbr] >= AUDIBLE_DOUBLE_TAP_TIME_LIMIT)
				{
					audible_turbo_timer[tm_nbr] = 0;
					audible_tap_action[tm_nbr] = 0;
					clean_audible_text(tm_nbr);
				}
				else
				{
					audible_turbo_timer[tm_nbr]++;
				}
			}
			else if(audible_tap_action[tm_nbr] == 2)
			{
				if (tm_nbr)
					snd_scall_bank(cursor_bnk_str,102,VOLUME4,255,PRIORITY_LVL1);
				else
					snd_scall_bank(cursor_bnk_str,102,VOLUME4,0,PRIORITY_LVL1);
				
				audible_turbo_timer[tm_nbr] = 0;
				audible_tap_action[tm_nbr] = 0;
				clean_audible_text(tm_nbr);
			} else {
				audible_turbo_timer[tm_nbr] = 0;
				audible_tap_action[tm_nbr] = 0;
				clean_audible_text(tm_nbr);
			}
		}

		if((ppdata->but_cur & P_C) && (audible_tap_action[tm_nbr] == 2))
		{
		//	A double-tap finally occured and the TURBO button is being held down.
		//	Start doing the audible process (put up the AUDIBLE MODE info box and check for UP/DOWN joystick)
			do_it_flag = 0;
			input = (get_player_sw_close() >> (ppdata->station << 3)) & P_SWITCH_MASK;

			if(input & P_UP)
			{
				i = game_info.audible_num[off_pos];
				do
				{
					if(game_info.audible_num[off_pos] <= 0)
						game_info.audible_num[off_pos] = (PLAYSTUF__NUM_AUDIBLE_PLAYS - 1);
					else
						game_info.audible_num[off_pos]--;
				} while (disallow[game_info.audible_num[off_pos]]);

				do_it_flag = (i == game_info.audible_num[off_pos]) ? -1 : 1;
			}
			else if(input & P_DOWN)
			{
				i = game_info.audible_num[off_pos];
				do
				{
					if(game_info.audible_num[off_pos] >= (PLAYSTUF__NUM_AUDIBLE_PLAYS - 1))
						game_info.audible_num[off_pos] = 0;
					else
						game_info.audible_num[off_pos]++;
				} while (disallow[game_info.audible_num[off_pos]]);

				do_it_flag = (i == game_info.audible_num[off_pos]) ? -1 : 1;
			}

			if(do_it_flag)
			{
				if (tm_nbr)
					snd_scall_bank(cursor_bnk_str,101,VOLUME4,255,PRIORITY_LVL1);
				else
					snd_scall_bank(cursor_bnk_str,101,VOLUME4,0,PRIORITY_LVL1);

				// call print audible #
				print_audible_message(off_pos, game_info.audible_num[off_pos], pnum, tm_nbr, 0);

				if(do_it_flag > 0)
				{
					aud_play = PlayStuf__GetAudiblePlay( ppdata->station, off_pos, game_info.audible_num[off_pos] );
#ifdef DEBUG
					if(aud_play == NULL)
					{
						fprintf(stderr, "bogus audible play ptr!\n");
						fflush(stdout);
						lockup();
					}
#endif
					// mark that an audible is active
					game_info.audible_flags[off_pos] = 1;

					// set the new play
					game_info.team_play[tm_nbr] = aud_play;

					return 0;
				}
			}
		}
	}
#endif

	return 1;
}

//-------------------------------------------------------------------------------------------------
// reset_audible_check
//-------------------------------------------------------------------------------------------------
static void reset_audible_check(void)
{
	audible_tap_action[0] = 0;		// TEAM1 - No tap action yet
	audible_tap_action[1] = 0;		// TEMA2 - No tap action yet

//	game_info.audible_flags[0] = 0;
//	game_info.audible_flags[1] = 0;

	audible_shown[0] = 0;
	audible_shown[1] = 0;

	audible_turbo_timer[0] = 0;
	audible_turbo_timer[1] = 0;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void clear_random_control_flags(void)
{
	db_control = 0;			// Reset DB Anim Control Bits
	db_zone_control = 0;	// Reset DB Zone Anim Control Bits
	db_m2m_control = 0;		// Reset DB m2m Anim Control Bits
	breath_control = 0;		// Reset Breath Anim Control Bits
	lm_control = 0;			// Reset Linemen Anim Control Bits
	wr_control = 0;			// Reset Wide Receiver Anim Control Bits
}
//-------------------------------------------------------------------------------------------------
