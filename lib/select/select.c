#if defined(VEGAS)

#define USE_DISK_CMOS
void update_other_cmos_to_disk(int sleep_allowed);

#endif

//******************************************************************
//
// DESCRIPTION: Enter Initials and Select teams code for FOOTBALL
// AUTHOR: Jeff Johnson
// HISTORY: Started Sept 29. 1996
// $Revillon: 16
// $Archive: /video/nfl/lib/select/select.c $
// $Modtime: 10/14/99 5:22p $
// *******************************************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <assert.h>
#include <dir.h>

#include "/video/nfl/include/fontid.h"
#include "/video/nfl/include/id.h"
#include "/video/nfl/include/game.h"
#include "/video/nfl/include/ani3D.h"
#include "/video/nfl/include/sndcalls.h"
#include "/video/nfl/include/select.h"
#include "/video/nfl/include/nflcmos.h"
#include "/video/nfl/include/coin.h"
#include "/video/nfl/include/player.h"
#include "/video/nfl/include/audits.h"
#include "/video/nfl/include/gameadj.h"
#include "/video/nfl/include/sysfont.h"
#include "/video/nfl/include/cap.h"
#include "/video/nfl/include/movie.h"
#include "/video/nfl/include/stream.h"
#include "/video/nfl/include/Coin.h"
#include "/video/nfl/include/PlaySel.h"
#include "/video/nfl/include/Record.h"
#include "/video/nfl/include/game_cfg.h"
#include "/video/nfl/include/game_sel.h"		// Game selection screen

//temp   !!!FIX (add these to DATA.C)
//#include	"\video\nfl\models\logo.h"
//temp

#include "/video/nfl/include/vsscreen.h"

#define	IMAGEDEFINE	1
#include "/video/nfl/include/plaq.h"
#include "/video/nfl/include/tmsel.h"
#include "/video/nfl/include/statpg.h"

extern void load_title_flash(int tex, int snd);
extern void free_title_flash(void);

int trivia_contest_init(void);
void trivia_contest(void);
int	set_coin_hold(int);
int count_quarters_purchased(void);
int count_players_in_game(void);
int read_plays_hd( int, int, cap_play_t *, audible_ids_t * );
int	get_joy_val_down(int pnum);
int	get_joy_val_cur(int pnum);
int	check_credits_to_start(void);
void start_enable(int);
void do_team_percent_aud(int);
void option_select_flash(sprite_info_t *obj, int tid);

extern int cap_pnum;

extern int random_drones, human_winstreak;
extern int full_game_credits;
extern image_info_t playresult;
extern float idiot_box_xys[2][2];
extern int	is_low_res;

extern int randrng(int);
extern void spawn_sparkles_proc(int *args);
extern void do_team_percent_aud(int);
extern void launch_game_proc(void);

extern const int team_ladder[8][5];
extern const int beat_all_teams[4];

#if defined(SEATTLE)
extern GrMipMapId_t	team_decals[4][10];
extern GrMipMapId_t	all_teams_decals[NUM_TEAMS][4];
extern GrMipMapId_t	misc_decals[4];
#else
extern Texture_node_t *team_decals[4][10];
extern Texture_node_t *all_teams_decals[NUM_TEAMS][4];
extern Texture_node_t *misc_decals[4];
#endif

extern struct texture_node * tlist;

//extern world_record_t  secret_heads[];

extern int team_audits[60];				// from audinfo.c
extern int reload;

static int pick_cpu_team(int);
static void load_tunes(void);
static void load_gameover_tunes(void);

void draw_backdrop(void);
void draw_backdrop_flip(void);
void flash_text_slow_proc(int *args);
void get_ready_for(void);
void wait_for_any_butn_press_active_plyr(int, int, int);
void start_enable(int);
void show_player_stats(int);
int check_secret_heads(int pnum);
int teammate_in_tmsel(int p_num);

void option_screen(int);
void plyr_select_option_proc( int *args);
void lighting_proc(int *args);

int	perspective_correct = 1;
int	reset_on_coin = 0;
int	got_coin = 0;
int	show_them_hiscore_tables_first = 0;

extern plyr_record_t	*rec_sort_ram;			// pointer to the array of PLAYER RECORDs from cmos into ram

//extern LIMB limb_num_3d_0,limb_num_3d_1,limb_num_3d_2,limb_num_3d_3,limb_num_3d_4;
//extern LIMB limb_num_3d_5,limb_num_3d_6,limb_num_3d_7,limb_num_3d_8,limb_num_3d_9;

int get_credits(void);

extern image_info_t cheerpg1,cheerpg1_c1,cheerpg1_c2,cheerpg1_c3;
extern image_info_t cheerpg2,cheerpg2_c1,cheerpg2_c2,cheerpg2_c3;
extern image_info_t cheerpg3,cheerpg3_c1,cheerpg3_c2,cheerpg3_c3;
extern image_info_t cheerpg4,cheerpg4_c1,cheerpg4_c2,cheerpg4_c3;
extern image_info_t cheerpg5,cheerpg5_c1,cheerpg5_c2,cheerpg5_c3;
extern image_info_t cheerpg6,cheerpg6_c1,cheerpg6_c2,cheerpg6_c3;
extern image_info_t cheerpg7,cheerpg7_c1,cheerpg7_c2,cheerpg7_c3;

// variables
int			option;
short		exit_status;
short		force_selection;							// !=0 force cursor to current choice
short		load_failed;
short		loading_textures;
short		players;
short		screen;
short		flash_status;
volatile int imgs_loaded;
//volatile int uniforms_half_loaded;
volatile int uniforms_loaded;
volatile int uniform_loading;

//static obj3d_data_t nbr_3d_obj;

typedef struct coach_msg {
	char * msg;
	float  yoff;
} coach_msg_t;

//
// Tables
//
//static LIMB *nbr_3d_limbs[] = {&limb_num_3d_0,&limb_num_3d_1,&limb_num_3d_2,&limb_num_3d_3,&limb_num_3d_4,
//															 &limb_num_3d_5,&limb_num_3d_6,&limb_num_3d_7,&limb_num_3d_8,&limb_num_3d_9};

//
// ** NOTE **  all points are based on MEDIUM RES.
//
//static float stat_pg_name_xys[MAX_PLYRS][2][2] = {		// [plyr_num][2plr/4plr][x,y]
//	{{ 66.0F,206.0F},{ 66.0F,206.0F}},					// player 1
//	{{449.0F,206.0F},{449.0F,206.0F}},					// player 2
//	{{  0.0F,0.0F},  {  0.0F, 0.0F}},					// player 3
//	{{  0.0F,0.0F},  {  0.0F, 0.0F}}					// player 4
//	};

static float option_xys[2][3][2] = {				// [res][objects][x,y]
	// low res
	{{SPRITE_HRES/2.0f-168.0f,SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f, SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f+168.0f, SPRITE_VRES/2.0f}},
	// mid res
	{{SPRITE_HRES/2.0f-168.0f,SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f, SPRITE_VRES/2.0f}, {SPRITE_HRES/2.0f+168.0f, SPRITE_VRES/2.f}}

	};


static float plaq_xys[MAX_PLYRS][2] = {				// [plyr_num][2/4 plyr -> x]
	{128.0F, 68.0f}, 		 						// player 1
	{384.0F,192.0f},  								// player 2
	{  0.0F,320.0F},  								// player 3
	{  0.0F,444.0F}	  								// player  4
};

float japbx_cntr_xys[MAX_PLYRS][2][2] = {		// [plyr_num][2plr/4plr][x,y]
	{{128.0F,45.0F},{ 68.0f,45.0F}},					// player 1
	{{384.0F,45.0F},{192.0f,45.0F}},					// player 2
	{{  0.0F,45.0F},{320.0F,45.0F}},					// player 3
	{{  0.0F,45.0F},{444.0F,45.0F}}						// player 4
};

float station_xys[MAX_PLYRS][2][2] = {			// [plyr_num][2plr/4plr][x,y]
	{{128.0F,192.0F},{ 68.0f,192.0F}},					// player 1
	{{384.0F,192.0F},{192.0f,192.0F}},					// player 2
	{{  0.0F,192.0F},{320.0F,192.0F}},					// player 3
	{{  0.0F,192.0F},{444.0F,192.0F}}					// player 4
};

//static float plaq_xys[MAX_PLYRS][2] = {				// [plyr_num][2/4 plyr -> x]
//	{128.0F, 64.0f}, 		 						// player 1
//	{384.0F,190.0f},  								// player 2
//	{  0.0F,322.0F},  								// player 3
//	{  0.0F,448.0F}	  								// player  4
//	};
//
//float japbx_cntr_xys[MAX_PLYRS][2][2] = {		// [plyr_num][2plr/4plr][x,y]
//	{{128.0F,45.0F},{ 72.0f,45.0F}},					// player 1
//	{{384.0F,45.0F},{198.0f,45.0F}},					// player 2
//	{{  0.0F,45.0F},{306.0F,45.0F}},					// player 3
//	{{  0.0F,45.0F},{433.0F,45.0F}}						// player 4
//	};
//
//float station_xys[MAX_PLYRS][2][2] = {			// [plyr_num][2plr/4plr][x,y]
//	{{128.0F,192.0F},{ 64.0f,192.0F}},					// player 1
//	{{384.0F,192.0F},{192.0f,192.0F}},					// player 2
//	{{  0.0F,192.0F},{320.0F,192.0F}},					// player 3
//	{{  0.0F,192.0F},{448.0F,192.0F}}					// player 4
//	};

#define CREDIT_MSG_X	279.0f
static float credit_msg_ys[2] = {
	7.0f, (5.0f * 3.0f/2.0f)
};

#if 0
static float credit_msg_xys[SCREENS][2][2] = {			// [screen nbr][res][x,y]
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// name entry screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// team selection screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// vs screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// buy-in screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// hhalftime screen
	{{279.0F,7.0F},{279.0F,(5.0f *3.0f/2.0f)}},			// play select screen
};
#endif

float logo_xys[SCREENS][2][2][2] = {				// [screen nbr][side0 or 1][2plr/4plr][x,y]
	{{{  0.0F,    0.0F},  {  0.0F,   0.0F}},   {{  0.0F,    0.0F},    {0.0F,    0.0F}}},	// name entry screen
	{{{ 77.0F,  310.0F},  { 77.0F, 310.0F}},   {{435.0F,  310.0F},  {435.0F,  310.0F}}},	// team selection screen
	{{{128.0F,  225.0F},  {256.0F, 225.0F}},   {{  0.0F,    0.0F},  {  0.0F,    0.0F}}},	// vs screen
	{{{  0.0F,    0.0F},  {  0.0F,   0.0F}},   {{  0.0F,    0.0F},  {  0.0F,    0.0F}}},	// buy-in screen
	{{{ 64.0F,  272.0F},  { 64.0F,  272.0F}},  {{445.0F,  272.0F},  {445.0F,  272.0F}}},	// halftime screen
	{{{ 49.0F+6,340.0F-6},{ 49.0F+6,340.0F-6}},{{448.0F-6,340.0F-6},{448.0F-6,340.0F-6}}},	// play select screen
	{{{ 49.0F+6,293.0F-6},{ 49.0F+6,293.0F-6}},{{446.0F-6,293.0F-6},{446.0F-6,293.0F-6}}}	// Coaching tips screen
	};

static int table_sizes[MENU_LEVELS][2][2] = {			// [menu_level],[2plr/4plr],[width,height]
	{{0,0},					{0,0}},						//  yes no layout
	{{LTR_COLS,LTR_ROWS},	{LTR_COLS,LTR_ROWS}},		//  enter inits layout
	{{PIN_COLS,PIN_ROWS},	{PIN_COLS,PIN_ROWS}},		//  pin nbr. layout
	{{1,NUM_TEAMS}, 		{1,NUM_TEAMS}}				//  team select
	};

#define	Y 280.0F
static float city_names_xs[2] = {200.0F,314.0F};

static float city_names_ys[2][7] = {
	{284.0F, 268.0F, 252.0F, 234.0F, 211.0F, 195.0F, 179.0F},		// mid res
	{190.0F, 179.0F, 168.0F, 156.0F, 141.0F, 130.0F, 119.0F}		// low res
};

static int fullgame_bought_ok = 0;		// set only when its OK to show the plaqs


//static float roster_names_ys[2][NUM_ON_ROSTER] = {
//	{222.0F,208.0F,194.0F,180.0F,131.0F,117.0F,103.0F, 89.0F,166.0F},
//	{147.0F,137.0F,127.0F,117.0F, 83.0F, 73.0F, 63.0F, 53.0F,107.0F}
//};

//static float stat_title_xs[MAX_PLYRS] = {128.0F, 384.0F, 0.0F, 0.0F};  // only used in 2 plr ver
//
//static float stat_title_ys[MAX_PLYRS] = {360.0F, 360.0F, 360.0F, 360.0F};
#define STAT_TITLE_Y	360.0f


static int plr_ranks[MAX_PLYRS][NUM_STATS];				// [plyr num][rank value for each stat]

#define	Y_SPC		44.0F
#define	ST_Y1		325.0F
#define	ST_Y2		ST_Y1-Y_SPC
#define	ST_Y3		ST_Y2-Y_SPC
#define	ST_Y4		ST_Y3-Y_SPC
#define	ST_Y5		ST_Y4-Y_SPC
#define	ST_Y6		ST_Y5-Y_SPC
//#define	ST_Y2		ST_Y1-(1*Y_SPC)
//#define	ST_Y3		ST_Y1-(2*Y_SPC)
//#define	ST_Y4		ST_Y1-(3*Y_SPC)
//#define	ST_Y5		ST_Y1-(4*Y_SPC)
//#define	ST_Y6		ST_Y1-(5*Y_SPC)

static float stat_plqs_xys[2][NUM_STATS][MAX_PLYRS][2] = {
		{{{ 64, ST_Y1}, {324, ST_Y1}, {  0,     0}, {  0,     0}},
		 {{ 64, ST_Y2}, {324, ST_Y2}, {  0,     0}, {  0,     0}},
		 {{ 64, ST_Y3}, {324, ST_Y3}, {  0,     0}, {  0,     0}},
		 {{ 64, ST_Y4}, {324, ST_Y4}, {  0,     0}, {  0,     0}},
		 {{ 64, ST_Y5}, {324, ST_Y5}, {  0,     0}, {  0,     0}},
		 {{ 64, ST_Y6}, {324, ST_Y6}, {  0,     0}, {  0,     0}},
		 {{185, ST_Y1}, {445, ST_Y1}, {  0,     0}, {  0,     0}},
		 {{185, ST_Y2}, {445, ST_Y2}, {  0,     0}, {  0,     0}},
		 {{185, ST_Y3}, {445, ST_Y3}, {  0,     0}, {  0,     0}},
		 {{185, ST_Y4}, {445, ST_Y4}, {  0,     0}, {  0,     0}},
		 {{185, ST_Y5}, {445, ST_Y5}, {  0,     0}, {  0,     0}},
		 {{185, ST_Y6}, {445, ST_Y6}, {  0,     0}, {  0,     0}}},
			// 4 player ver.
		{{{ 64, ST_Y1}, {192, ST_Y1}, {320, ST_Y1}, {448, ST_Y1}},
		 {{ 64, ST_Y2}, {192, ST_Y2}, {320, ST_Y2}, {448, ST_Y2}},
		 {{ 64, ST_Y3}, {192, ST_Y3}, {320, ST_Y3}, {448, ST_Y3}},
		 {{ 64, ST_Y4}, {192, ST_Y4}, {320, ST_Y4}, {448, ST_Y4}},
		 {{ 64, ST_Y5}, {192, ST_Y5}, {320, ST_Y5}, {448, ST_Y5}},
		 {{ 64, ST_Y6}, {192, ST_Y6}, {320, ST_Y6}, {448, ST_Y6}},
		 {{  0,     0}, {  0,     0}, {  0,     0}, {  0,     0}},
		 {{  0,     0}, {  0,     0}, {  0,     0}, {  0,     0}},
		 {{  0,     0}, {  0,     0}, {  0,     0}, {  0,     0}},
		 {{  0,     0}, {  0,     0}, {  0,     0}, {  0,     0}},
		 {{  0,     0}, {  0,     0}, {  0,     0}, {  0,     0}},
		 {{  0,     0}, {  0,     0}, {  0,     0}, {  0,     0}}}
};

static float stat_plqs_yoff1[2][2][NUM_STATS] = {	// [ver][res][stat]
	{
		{  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f},	// [2p][hi]
		{  2.0f,  3.0f,  2.0f,  2.0f,  1.0f,  2.0f,  2.0f,  3.0f,  2.0f,  2.0f,  4.0f,  2.0f},	// [2p][lo]
	},
	{
		{  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f},	// [4p][hi]
		{  2.0f,  3.0f,  2.0f,  2.0f,  3.0f,  3.0f,  2.0f,  3.0f,  2.0f,  2.0f,  3.0f,  2.0f},	// [4p][lo]
	},
};

static float stat_plqs_yoff2[2][2][NUM_STATS] = {	// [ver][res][stat]
	{
		{ 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f},	// [2p][hi]
		{ 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f},	// [2p][lo]
	},
	{
		{ 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f},	// [4p][hi]
		{ 22.0f, 22.0f, 22.0f, 22.0f, 20.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f, 22.0f},	// [4p][lo]
	},
};

#undef		ST_Y
#define		ST_Y		320.0F

static float stat_plq_msg_xys[2][MAX_PLYRS][2] = {
	{{65.0F,ST_Y}, {325.0F,ST_Y}, 	{0.0F,0.0F},   {0.0F,0.0F}},
	{{65.0F,ST_Y}, {160.0F,ST_Y}, {280.0F,ST_Y}, {480.0F,ST_Y}}
};
				
#define	XO	50.0f

#if 0
static float stat_plqs_awrd_xys[NUM_STATS][MAX_PLYRS][2] = {
						// left column (for all players)
					{{65.0F-XO,ST_Y-(0*Y_SPC)}, {325.0F-XO,ST_Y-(0*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{65.0F-XO,ST_Y-(1*Y_SPC)}, {325.0F-XO,ST_Y-(1*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{65.0F-XO,ST_Y-(2*Y_SPC)}, {325.0F-XO,ST_Y-(2*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{65.0F-XO,ST_Y-(3*Y_SPC)}, {325.0F-XO,ST_Y-(3*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{65.0F-XO,ST_Y-(4*Y_SPC)}, {325.0F-XO,ST_Y-(4*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{65.0F-XO,ST_Y-(5*Y_SPC)}, {325.0F-XO,ST_Y-(5*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
						// right column (for all players)
					{{185.0F-XO,ST_Y-(0*Y_SPC)}, {445.0F-XO,ST_Y-(0*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{185.0F-XO,ST_Y-(1*Y_SPC)}, {445.0F-XO,ST_Y-(1*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{185.0F-XO,ST_Y-(2*Y_SPC)}, {445.0F-XO,ST_Y-(2*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{185.0F-XO,ST_Y-(3*Y_SPC)}, {445.0F-XO,ST_Y-(3*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{185.0F-XO,ST_Y-(4*Y_SPC)}, {445.0F-XO,ST_Y-(4*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}},
					{{185.0F-XO,ST_Y-(5*Y_SPC)}, {445.0F-XO,ST_Y-(5*Y_SPC)}, {0.0F,0.0F}, {0.0F,0.0F}}};
#endif


#undef		ST_Y
#define		ST_Y 	315.0F
#define		Y_SPC2	20.0F
static float tm_stat_pg_stats_xs[] = {178.0F,336.0F};

static float tm_stat_pg_stats_ys[NUM_HLFTME_STATS] = {
	ST_Y-Y_SPC2*0,ST_Y-Y_SPC2*1,ST_Y-Y_SPC2*2,ST_Y-Y_SPC2*3,
	ST_Y-Y_SPC2*4,ST_Y-Y_SPC2*5,ST_Y-Y_SPC2*6,ST_Y-Y_SPC2*7,
	ST_Y-Y_SPC2*8,ST_Y-Y_SPC2*9,ST_Y-Y_SPC2*10
};

static int tm_stat_pg_stats_js[] = {HJUST_RIGHT | VJUST_CENTER, HJUST_LEFT | VJUST_CENTER};
																				
static float stat_pg_scrs_xys[2][2][2] = {
	{{ 66, 168 }, { 449, 168 }},
	{{ 66, 162 }, { 449, 162 }}};
			
#define	XST		140.0F			
#define	XSP		42.0F
static float stat_pg_scr_sprd_xs[] = {30.0F,184.0F,227.0F,270.0F,313.0F,356.0F,399.0F,445.0F};
static float stat_pg_scr_sprd_ys[][2] = {{58.0F,35.0F},{57.0F,35.0F}};
			

#define FB_Y	17.0f
#define FB_D	19.0f
static float credit_fb_flsh_xys[2][MAX_PLYRS][5][2] = {						// [kit][plrs][pos.][x,y]
	{{{0.0F,0.0F}, {99.0F,FB_Y},{118.0F,FB_Y},{136.0F,FB_Y},{154.0F,FB_Y}},		// plyr 1 (2 plyr ver)
	 {{0.0F,0.0F},{355.0F,FB_Y},{374.0F,FB_Y},{392.0F,FB_Y},{410.0F,FB_Y}},		// plyr 2
	 {{0.0F,0.0F},{  0.0F,0.0F},{  0.0F,0.0F},{  0.0F,0.0F},{  0.0F,0.0F}},		// plyr 3
	 {{0.0F,0.0F},{  0.0F,0.0F},{  0.0F,0.0F},{  0.0F,0.0F},{  0.0F,0.0F}}},	// plyr 4

	{{{0.0F,0.0F}, {35.0F,FB_Y},{ 54.0F,FB_Y},{ 73.0F,FB_Y},{ 92.0F,FB_Y}},		// plyr 1 (4 plyr ver)
	 {{0.0F,0.0F},{160.0F,FB_Y},{179.0F,FB_Y},{198.0F,FB_Y},{217.0F,FB_Y}},		// plyr 2
	 {{0.0F,0.0F},{290.0F,FB_Y},{309.0F,FB_Y},{328.0F,FB_Y},{347.0F,FB_Y}},		// plyr 3
	 {{0.0F,0.0F},{415.0F,FB_Y},{434.0F,FB_Y},{453.0F,FB_Y},{472.0F,FB_Y}}} };	// plyr 4


#define STATS_BCKDRP_Y	330.0f
static float stats_bckdrp_xs[2][MAX_PLYRS] = {
	{ 128.0f, 384.0f, 0.0f, 0.0f },
	{  59.0f, 187.0f, 315.0f, 443.0f }
};
#if 0
static float stats_bckdrp_xys[2][MAX_PLYRS][2] = {	//[ver][plyr#][x,y]
	 {{128, 330}, {384,330}, {  0,  0}, {  0,  0}},
	 {{ 59, 330}, {187,330}, {315,330}, {443,330}},
};
#endif

//static float tm_city_plq_xys[2][2] = { {256.0f,192.0f}, {370.0F,192.0f} };


// for OPTION SCREEN (minimum textures needed to start game)
static tmu_ram_texture_t opt_scrn_textures_tbl[] = {
 {"plqcrd00.wms",JAPPLE_BOX_TID},			// 256 * 256 * 16bits
 {"plqcrd01.wms",JAPPLE_BOX_TID},

 {"plqfsh.wms",JAPPLE_BOX_TID},				// 256 * 256 * 16bits

 {"optbk00.wms",BKGRND_TID},
 {"optbk01.wms",BKGRND_TID},
 {"optbk02.wms",BKGRND_TID},

 {"optscr00.wms",OPT_SCR_TID},
 {"optscr01.wms",OPT_SCR_TID},
 {"optscr02.wms",OPT_SCR_TID},
 {"optscr03.wms",OPT_SCR_TID},

 {"optfsh00.wms",OPT_SCR_TID},
 {"optfsh01.wms",OPT_SCR_TID},
 {"optfsh02.wms",OPT_SCR_TID},
 {"optfsh03.wms",OPT_SCR_TID},
 {NULL,0} };

// for enter initials (minimum textures needed)
static tmu_ram_texture_t name_entry_textures_tbl[] = {
 {"bkgrnd00.wms",BKGRND_TID},
 {"bkgrnd01.wms",BKGRND_TID},

 {"plqcur00.wms",YES_NO_PLAQ_TID},
 {"plqcur01.wms",YES_NO_PLAQ_TID},
 {"plqcur02.wms",YES_NO_PLAQ_TID},
 {"plqcur03.wms",YES_NO_PLAQ_TID},

 {"plqnme07.wms", YES_NO_PLAQ_TID},
 {"plqnme08.wms", YES_NO_PLAQ_TID},
 {"plqnme09.wms", YES_NO_PLAQ_TID},
 {"plqnme10.wms", YES_NO_PLAQ_TID},

 {"plqnme12.wms", YES_NO_PLAQ_TID},
 {"plqnme13.wms", YES_NO_PLAQ_TID},
 {NULL,0} };


// for enter initials (rest of needed images)
static tmu_ram_texture_t name_entry_textures_tbl2[] = {
 {"tmbkgd00.wms",BKGRND2_TID},
 {"tmbkgd01.wms",BKGRND2_TID},

 {"plqnme00.wms", YES_NO_PLAQ_TID},
 {"plqnme01.wms", YES_NO_PLAQ_TID},
 {"plqnme02.wms", YES_NO_PLAQ_TID},
 {"plqnme03.wms", YES_NO_PLAQ_TID},
 {"plqnme04.wms", YES_NO_PLAQ_TID},
 {"plqnme05.wms", YES_NO_PLAQ_TID},
 {"plqnme06.wms", YES_NO_PLAQ_TID},
 {"plqnme11.wms", YES_NO_PLAQ_TID},

 {"plqlet.wms",YES_NO_PLAQ_TID},
 {NULL,0} };


// for TEAM SELECT
static tmu_ram_texture_t team_select_textures_tbl[] = {
// {"tmbkgd00.wms",BKGRND_TID},
// {"tmbkgd01.wms",BKGRND_TID},

 {"tmplq00.wms",TM_SELECT_TID},
 {"tmplq01.wms",TM_SELECT_TID},

 {"tmsel00.wms",TM_SELECT_TID},
 {"tmsel01.wms",TM_SELECT_TID},

// {"cg_ts.wms", TM_SELECT_TID},

 {"tmlogo00.wms",TM_SEL_LOGOS_TID},
 {"tmlogo01.wms",TM_SEL_LOGOS_TID},
 {"tmlogo02.wms",TM_SEL_LOGOS_TID},
 {"tmlogo03.wms",TM_SEL_LOGOS_TID},
 {"tmlogo04.wms",TM_SEL_LOGOS_TID},

// {"vslite00.wms",JAPPLE_BOX_TID},			// 256 * 256
// {"vslite01.wms",JAPPLE_BOX_TID},			// 256 * 128

// {"plqnme07.wms", 463},
// {"plqnme07.wms", 9087},
// {"plqnme07.wms", 98890},
// {"plqnme07.wms", 4674},
// {"plqnme07.wms", 246356},
// {"plqnme07.wms", 23523},
// {"plqnme07.wms", 8567},
// {"plqnme07.wms", 9845},
// {"plqnme07.wms", 14554},
// {"plqnme07.wms", 1511},
// {"plqnme07.wms", 154657},
// {"plqnme07.wms", 75272},
// {"plqnme07.wms", 836865},
// {"plqnme07.wms", 96868},
// {"plqnme07.wms", 563},
// {"plqnme07.wms", 4452},
// {"plqnme07.wms", 3223},
// {"plqnme07.wms", 293},
// {"plqnme07.wms", 3888},
// {"plqnme07.wms", 596},
// {"plqnme07.wms", 28},
// {"plqnme07.wms", 967},
// {"plqnme07.wms", 457},
// {"plqnme07.wms", 734},
// {"plqnme07.wms", 347},
// {"plqnme07.wms", 3465},
// {"plqnme07.wms", 4545},
// {"plqnme07.wms", 1324},
// {"plqnme07.wms", 234},
// {"plqnme07.wms", 300},

 {NULL,0} };

// for HALFTIME stats page
static tmu_ram_texture_t hlf_stats_pg_textures_tbl[] = {
 {"statpg00.wms",STATS_PG_TID},
 {"statpg01.wms",STATS_PG_TID},
 {"statpg02.wms",STATS_PG_TID},
 {NULL,0} };

//static tmu_ram_texture_t hlf_stats_pg_textures_tbl_lr[] = {
// {"statlr00.wms",STATS_PG_TID},
// {"statlr01.wms",STATS_PG_TID},
// {NULL,0} };


// for ENDGAME stats page
static tmu_ram_texture_t final_stats_pg_textures_tbl[] = {
 {"statpg03.wms",STATS_PG_TID},
 {"statpg04.wms",STATS_PG_TID},
 {"statpg05.wms",STATS_PG_TID},
 {NULL,0} };

//static tmu_ram_texture_t final_stats_pg_textures_tbl_lr[] = {
// {"statlr02.wms",STATS_PG_TID},
// {"statlr03.wms",STATS_PG_TID},
// {NULL,0} };



static char *pin_strs[] = {
									"----",
									"*---",
									"**--",
									"***-",
									"****" };

static char *censor_name_tbl[] = {
		"FUC",
		"SUC",
		"FUK",
		"ASS",
		"CUM",
		"JIZ",
		"COCK",
		"DICK",
		"SHIT",
		"PISS",
		"CUNT",
		"CLIT",
		"PUSS",
		"PRICK",
		"VAGINA",
		"PUTANG",
		"JAGOFF",
		"NIGGER",
		NULL
		};
		
//static image_info_t *qtr_name_imgs[] = {&play_1st, &play_2nd, &play_3rd, &play_4th};

static image_info_t *yn_plaq_imgs[MAX_PLYRS][2] = {
	{&ynplq1, &ynplq1_c1},
	{&ynplq2, &ynplq2_c1},
	{&ynplq3, &ynplq3_c1},
	{&ynplq4, &ynplq4_c1}
};


static image_info_t *init_plaq_imgs[MAX_PLYRS][2] =  {
	{&initplq1, &initplq1_c1},
	{&initplq2, &initplq2_c1},
	{&initplq3, &initplq3_c1},
	{&initplq4, &initplq4_c1}
};


static image_info_t *pin_plaq_imgs[MAX_PLYRS][2] =  {
	{&pinplq1, &pinplq1_c1},
	{&pinplq2, &pinplq2_c1},
	{&pinplq3, &pinplq3_c1},
	{&pinplq4, &pinplq4_c1}
};


static image_info_t *no_flsh_imgs[2][MAX_PLYRS][11] = {
{{&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 1
 {&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 2
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}},
	// 4 plyr ver
{{&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 1
 {&nglwl01,&nglwl02,&nglwl03,&nglwl04,&nglwl05,&nglwl06,&nglwl07,&nglwl08,&nglwl09,&nglwl10,&nglwl11},			// plr 2
 {&nglwr01,&nglwr02,&nglwr03,&nglwr04,&nglwr05,&nglwr06,&nglwr07,&nglwr08,&nglwr09,&nglwr10,&nglwr11},			// plr 3
 {&nglwr01,&nglwr02,&nglwr03,&nglwr04,&nglwr05,&nglwr06,&nglwr07,&nglwr08,&nglwr09,&nglwr10,&nglwr11}} };			// plr 4


static image_info_t *yes_flsh_imgs[2][MAX_PLYRS][11] = {
{{&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 1
 {&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 2
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
 {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}},
	// 4 plyr ver
{{&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 1
 {&yglwl01,&yglwl02,&yglwl03,&yglwl04,&yglwl05,&yglwl06,&yglwl07,&yglwl08,&yglwl09,&yglwl10,&yglwl11},			// plr 2
 {&yglwr01,&yglwr02,&yglwr03,&yglwr04,&yglwr05,&yglwr06,&yglwr07,&yglwr08,&yglwr09,&yglwr10,&yglwr11},			// plr 3
 {&yglwr01,&yglwr02,&yglwr03,&yglwr04,&yglwr05,&yglwr06,&yglwr07,&yglwr08,&yglwr09,&yglwr10,&yglwr11}} };		// plr 4

static image_info_t *optscr_flsh_imgs[] = {&lt1,&lt2,&lt3,&lt4,&lt5,&lt6,&lt7,&lt8,&lt9,&lt10,
											&lt11,&lt12,&lt13,&lt14,&lt15};

//static image_info_t *yes_flsh_imgs[] = {&flashy06,&flashy08,&flashy10,
//										&flashy12,&flashy14}; //,&flashy16,&flashy18,&flashy20};

//static image_info_t *no_flsh_imgs[] =  {&flashn06,&flashn08,&flashn10,
//										&flashn12,&flashn14}; //,&flashn16,&flashn18,&flashn20};

#if 0
static image_info_t *lighting_imgs[] = {&light1,&light2,&light3,&light4,&light5,&light6,
										 &light7,&light8,&light9,&light10,&light11};
#endif

image_info_t *stat_flsh_imgs[] = {&flshbx01,&flshbx01,&flshbx02,&flshbx03,&flshbx04,&flshbx05,
		  							 &flshbx06,&flshbx07,&flshbx08,&flshbx09};			// 226x76

static image_info_t *burst_flsh_imgs[9] = {&flasha01,&flasha02,&flasha03,&flasha04,&flasha05,
										   &flasha06,&flasha07,&flasha08,&flasha09};

static image_info_t *credit_box_imgs[4][5] = {{&japlxxxx,&japltxxx,&japlttxx,&japltttx,&japltttt},	 // <=1st qtr
										  	  {&japl_xxx,&japl_txx,&japl_ttx,&japl_ttt,&japl____},	 // 2nd qtr
											  {&japl__xx,&japl__tx,&japl__tt,&japl____,&japl____},	 // 3rd qtr.
											  {&japl___x,&japl___t,&japl____,&japl____,&japl____}};	 // 4th qtr and OT's
																

//static image_info_t *japple_box_imgs[2][MAX_PLYRS] = {{&jplplq_l,&jplplq_r,NULL,NULL},
//													  {&jplplq_l,&jplplq_l,&jplplq_r,&jplplq_r}};

#if 0
static image_info_t *japple_box_imgs[2][MAX_PLYRS] = {{&jplplq_l,&jplplq_l,NULL,NULL},
													  {&jplplq_l,&jplplq_l,&jplplq_l,&jplplq_l}};
#endif
//static image_info_t *tm_sel_nbr_cursor_imgs[2][2] = { {&p1_l_cur, &p1_r_cur}, {&p2_l_cur, &p2_r_cur} };
//static image_info_t *tm_roster_plqs[2][2] = {{&roster_l,&roster_r},{&roster_l_lr,&roster_r_lr}};
static image_info_t *tm_city_plqs[2] = {&tmplaq_l,&tmplaq_r};
//static image_info_t *tm_city_plqs[2] = {&tmplaq_l,&tmplaq_l};

//static sprite_info_t *tm_plaqs[2] = { NULL, NULL };

static image_info_t *tm_selct_nbrs_imgs[MAX_PLYRS] = {&ts1, &ts2, &ts3, &ts4};

static sprite_info_t *tm_selct_img_ptrs[][2] = {		// [num ptrs][side 0 or 1]
	{0,0},					// team logo ptrs
	{0,0},					// city name 1 img. ptrs
	{0,0},					// city name 2 img. ptrs
	{0,0},					// city name 3 img. ptrs
	{0,0},					// city name 4 img. ptrs
	{0,0},					// city name 5 img. ptrs
	{0,0},					// city name 6 img. ptrs
	{0,0},					// city name 7 img. ptrs
	{0,0},					// city name 8 img. ptrs
	{0,0},					// city name 9 img. ptrs
	{0,0},					// 'passing' attrib. meter img ptr
	{0,0},					// 'rushing' attrib. meter img ptr
	{0,0},					// 'linemen' attrib. meter img ptr
	{0,0},					// 'defence' attrib. meter img ptr
	{0,0},					// 'sp.teams' attrib. meter img ptr
	{0,0},					// team roster name 1 img. ptr
	{0,0},					// team roster name 2 img. ptr
	{0,0},					// team roster name 3 img. ptr
	{0,0},					// team roster name 4 img. ptr
	{0,0},					// team roster name 5 img. ptr
	{0,0},					// team roster name 6 img. ptr
	{0,0},					// team roster name 7 img. ptr
	{0,0},					// team roster name 8 img. ptr
	{0,0}					// team roster name 9 img. ptr
};
									
														
static image_info_t * meter_imgs[2][2][6] = {
	{
	{&meterl_1,&meterl_1,&meterl_2,&meterl_3,&meterl_4,&meterl_5},
	{&meterr_1,&meterr_1,&meterr_2,&meterr_3,&meterr_4,&meterr_5}
	},
	{
	{&meterl_1_lr,&meterl_1_lr,&meterl_2_lr,&meterl_3_lr,&meterl_4_lr,&meterl_5_lr},
	{&meterr_1_lr,&meterr_1_lr,&meterr_2_lr,&meterr_3_lr,&meterr_4_lr,&meterr_5_lr}
	}
//	{
//	{&meterl_0,&meterl_1,&meterl_2,&meterl_3,&meterl_4,&meterl_5},
//	{&meterr_0,&meterr_1,&meterr_2,&meterr_3,&meterr_4,&meterr_5}
//	},
//	{
//	{&meterl_0_lr,&meterl_1_lr,&meterl_2_lr,&meterl_3_lr,&meterl_4_lr,&meterl_5_lr},
//	{&meterr_0_lr,&meterr_1_lr,&meterr_2_lr,&meterr_3_lr,&meterr_4_lr,&meterr_5_lr}
//	}
};


//static image_info_t *yes_no_cursor_imgs[] = {&hilite_n,&hilite_y};
static image_info_t *yes_no_cursor_imgs[] = {
	&no_l,&yes_l,		// plyr 1
	&no_l,&yes_l,		// plyr 2
	&no_r,&yes_r,		// plyr 3
	&no_r,&yes_r		// plyr 4
};

static image_info_t *inits_cursor_imgs[] = {
	// player 1 (in 2 player version)
	&l_a,&l_b,&l_c,&l_d,&l_e,&l_f,&l_g,&l_h,&l_i,&l_j,&l_k,&l_l,&l_m,&l_n,&l_o,&l_p,&l_q,&l_r,&l_s,&l_t,
	&l_u,&l_v,&l_w,&l_x,&l_y,&l_z,&l_dsh,&l_ast,&l_exc,&l_spc,&l_del,&l_end,
		
	// player 2
	&l_a,&l_b,&l_c,&l_d,&l_e,&l_f,&l_g,&l_h,&l_i,&l_j,&l_k,&l_l,&l_m,&l_n,&l_o,&l_p,&l_q,&l_r,&l_s,&l_t,
	&l_u,&l_v,&l_w,&l_x,&l_y,&l_z,&l_dsh,&l_ast,&l_exc,&l_spc,&l_del,&l_end,

	// player 3
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	// player 4
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	// player 1 (in 4 player version)
	&l_a,&l_b,&l_c,&l_d,&l_e,&l_f,&l_g,&l_h,&l_i,&l_j,&l_k,&l_l,&l_m,&l_n,&l_o,&l_p,&l_q,&l_r,&l_s,&l_t,
	&l_u,&l_v,&l_w,&l_x,&l_y,&l_z,&l_dsh,&l_ast,&l_exc,&l_spc,&l_del,&l_end,
		
	// player 2
	&l_a,&l_b,&l_c,&l_d,&l_e,&l_f,&l_g,&l_h,&l_i,&l_j,&l_k,&l_l,&l_m,&l_n,&l_o,&l_p,&l_q,&l_r,&l_s,&l_t,
	&l_u,&l_v,&l_w,&l_x,&l_y,&l_z,&l_dsh,&l_ast,&l_exc,&l_spc,&l_del,&l_end,

	// player 3
	&r_a,&r_b,&r_c,&r_d,&r_e,&r_f,&r_g,&r_h,&r_i,&r_j,&r_k,&r_l,&r_m,&r_n,&r_o,&r_p,&r_q,&r_r,&r_s,&r_t,
	&r_u,&r_v,&r_w,&r_x,&r_y,&r_z,&r_dsh,&r_ast,&r_exc,&r_spc,&r_del,&r_end,

	// player 4
	&r_a,&r_b,&r_c,&r_d,&r_e,&r_f,&r_g,&r_h,&r_i,&r_j,&r_k,&r_l,&r_m,&r_n,&r_o,&r_p,&r_q,&r_r,&r_s,&r_t,
	&r_u,&r_v,&r_w,&r_x,&r_y,&r_z,&r_dsh,&r_ast,&r_exc,&r_spc,&r_del,&r_end
};

static image_info_t *pin_cursor_imgs[] = {
	// player 1 (in 2 player version)
	&l_1,&l_2,&l_3,
	&l_4,&l_5,&l_6,
	&l_7,&l_8,&l_9,
	&l_pspc,&l_0,&l_pdel,

	// player 2
	&l_1,&l_2,&l_3,
	&l_4,&l_5,&l_6,
	&l_7,&l_8,&l_9,
	&l_pspc,&l_0,&l_pdel,

	// player 3
	NULL,NULL,NULL,
	NULL,NULL,NULL,
	NULL,NULL,NULL,
	NULL,NULL,NULL,

	// player 4
	NULL,NULL,NULL,
	NULL,NULL,NULL,
	NULL,NULL,NULL,
	NULL,NULL,NULL,

	// player 1 (4 player version)
	&l_1,&l_2,&l_3,
	&l_4,&l_5,&l_6,
	&l_7,&l_8,&l_9,
	&l_pspc,&l_0,&l_pdel,

	// player 2
	&l_1,&l_2,&l_3,
	&l_4,&l_5,&l_6,
	&l_7,&l_8,&l_9,
	&l_pspc,&l_0,&l_pdel,

	// player 3
	&r_1,&r_2,&r_3,
	&r_4,&r_5,&r_6,
	&r_7,&r_8,&r_9,
	&r_pspc,&r_0,&r_pdel,

	// player 4
	&r_1,&r_2,&r_3,
	&r_4,&r_5,&r_6,
	&r_7,&r_8,&r_9,
	&r_pspc,&r_0,&r_pdel
};

static image_info_t **cursor_tbl_ptrs[] = {yes_no_cursor_imgs,inits_cursor_imgs,pin_cursor_imgs};
			

#if 0			
static image_info_t *stats_title_imgs[2][MAX_PLYRS] = {
														{&statpl_r,&statpl_r,NULL,NULL},
														{&statpl_r,&statpl_r,&statpl_r,&statpl_r}};


static image_info_t *stats_plq_imgs[2][MAX_PLYRS] = {
														{&sstatplr,&sstatplr,NULL,NULL},
														{&sstatplr,&sstatplr,&sstatplr,&sstatplr}};
#endif

static int plr_nme_str_ids[MAX_PLYRS] = {P1_NAME_STR_ID,P2_NAME_STR_ID,P3_NAME_STR_ID,P4_NAME_STR_ID};
static int plr_pin_str_ids[MAX_PLYRS] = {P1_PIN_STR_ID,P2_PIN_STR_ID,P3_PIN_STR_ID,P4_PIN_STR_ID};
static int plyr_gen_str_ids[MAX_PLYRS] = {P1_GEN_STR_ID,P2_GEN_STR_ID,P3_GEN_STR_ID,P4_GEN_STR_ID};

static char alpha_table[LTR_COLS*LTR_ROWS] = {
	CH_A,CH_B,CH_C,CH_D,
	CH_E,CH_F,CH_G,CH_H,
	CH_I,CH_J,CH_K,CH_L,
	CH_M,CH_N,CH_O,CH_P,
	CH_Q,CH_R,CH_S,CH_T,
	CH_U,CH_V,CH_W,CH_X,
	CH_Y,CH_Z,CH_DSH,CH_AST,
	CH_EXC,CH_SPC,CH_DEL,CH_END
};

//	CH_A,CH_B,CH_C,CH_D,CH_E,
//	CH_F,CH_G,CH_H,CH_I,CH_J,
//	CH_K,CH_L,CH_M,CH_N,CH_O,
//	CH_P,CH_Q,CH_R,CH_S,CH_T,
//	CH_U,CH_V,CH_W,CH_X,CH_Y,
//	CH_Z,CH_EXC,CH_SPC,CH_DEL,CH_END};

static char pin_nbr_tbl[PIN_COLS*PIN_ROWS] = {
	CH_1,CH_2,CH_3,
	CH_4,CH_5,CH_6,
	CH_7,CH_8,CH_9,
	CH_SPC,CH_0,CH_DEL
};

//static	snd_pan_tbl[MAX_PLYRS] = {127,127,127,127};

//char *down_str[] = { "1ST", "2ND", "3RD", "4TH" };
char *qtr_str[] = { "1ST QUARTER", "2ND QUARTER", "3RD QUARTER", "4TH QUARTER", "OVERTIME 1", "OVERTIME 2", "OVERTIME 3" };

char *period_names[] =
{
	"2ND PERIOD",
	"3RD PERIOD",
	"4TH PERIOD",
	"OVERTIME",
	"OVERTIME 2",
	"OVERTIME 3",
	"OVERTIME 4"
};


static world_record_t  secret_heads[] = {
    {{'T','U','R','M','E','L'},{'0','3','2','2'},0},
    {{'B','R','A','I','N',' '},{'1','1','1','1'},1},
    {{'D','A','N','I','E','L'},{'0','6','0','4'},2},
//    {{'L','E','X',' ',' ',' '},{'7','7','7','7'},12},
    {{'J','A','S','O','N',' '},{'3','1','4','1'},4},
    {{'J','A','P','P','L','E'},{'6','6','6','0'},5},
    {{'J','E','N','I','F','R'},{'3','3','3','3'},6},
    {{'G','E','N','T','I','L'},{'1','1','1','1'},7},
    {{'L','U','I','S',' ',' '},{'3','3','3','3'},8},
    {{'R','A','I','D','E','N'},{'3','6','9','1'},9},
    {{'R','O','O','T',' ',' '},{'6','0','0','0'},10},
    {{'S','H','I','N','O','K'},{'8','3','3','7'},11},
    {{'S','K','U','L','L',' '},{'1','1','1','1'},12},
    {{'T','H','U','G',' ',' '},{'1','1','1','1'},13},
    {{'S','A','L',' ',' ',' '},{'0','2','0','1'},14},
    {{'F','O','R','D','E','N'},{'1','1','1','1'},15},
    {{'C','A','R','L','T','N'},{'1','1','1','1'},16},
    {{'M','I','K','E',' ',' '},{'3','3','3','3'},17},
    {{'G','R','I','N','C','H'},{'0','2','2','2'},18},
    {{'P','A','U','L','O',' '},{'0','5','1','7'},19},
    {{'M','X','V',' ',' ',' '},{'1','0','1','4'},12},
    {{'E','D','D','I','E',' '},{'3','3','3','3'},1}
    };

//static int load_order[NUM_TEAMS] = {0,29,1,28,2,27,3,26,4,25,5,24,6,23,7,22,8,21,9,20,10,
//									19,11,18,12,17,13,16,14,15};

							
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void dim_text_start(int *args)
{
int	i,tm_num;

	tm_num = args[0];

	// change the text to a constant color ... so I can fade it down to a darker (dimmer) color.
	for (i=TM_NME1_OBJ_PTR; i <= TM_NME7_OBJ_PTR; i++)
	{
		if (i != TM_NME4_OBJ_PTR)
		{
			tm_selct_img_ptrs[i][tm_num]->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
			tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffd0d0d0;
		}
	}

	die(0);
}

//-------------------------------------------------------------------------------------------------
// 						This process controls the flow of all the PRE-GAME screens
//
//  called by FIRST valid attract mode start button hit
//-------------------------------------------------------------------------------------------------

extern int texture_load_in_progress;

void pre_game_scrns(int *args)
{
	static audible_ids_t audible_ids_default =
	{{{ // Offense
		1 + (PLAYSTUF__PLAYS_PER_PAGE * 0) + 0,						// page 1,play 1 (team play 1)
		1 + (PLAYSTUF__PLAYS_PER_PAGE * 0) + 1,						// page 1,play 2 (team play 2)
		1 + (PLAYSTUF__PLAYS_PER_PAGE * 0) + 2,						// page 1,play 3 (team play 3)
	  },
	  { // Defense
	  	1 + (PLAYSTUF__PLAYS_PER_PAGE * 0) + 0,						// page 1,play 1 (safe cover)
		1 + (PLAYSTUF__PLAYS_PER_PAGE * 0) + 7,						// page 1,play 8 (near zone)
		1 + (PLAYSTUF__PLAYS_PER_PAGE * 0) + 6,						// page 1,play 7 (1 man blitz)
	}}};

	int no_name_entry, i, pnum;

	screen = NAME_ENTRY_SCRN;

	// Make sure <game_proc> finishes loads &| dies
	if (existp(GAME_PROC_PID, 0xFFFFFFFF) && texture_load_in_progress)
		game_info.attract_coin_in = 1;

	// Don't kill any disk loads
	while (texture_load_in_progress) sleep(1);
	// Make sure the disk is in normal mode
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

	/* set game mode master volume level */
	snd_master_volume(game_info.master_volume);
	/* enable sounds in case attract mode disabled them */
	snd_bank_lockout(0);

//	no_name_entry = *args;
	no_name_entry = args[0];

	// player who started game!
	pnum = args[1];

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
	{
		// Enable drawing
		draw_enable(1);

		// Abort the movie player
		movie_abort();
//		stream_stop();
//		do { sleep (1); } while (stream_check_active());
//		stream_release_key();

		// Fade the last displayed movie frame out
		fade(0.0f, 5, NULL);
		sleep(6);

		// Disable drawing
		draw_enable(0);
	}

	// In case we get here as a result of awarding a free game
	start_enable(TRUE);

	// Take care of proper handling of transition
	if (plates_on != PLATES_ON)
	{
		// Enable drawing since we know something is being or will be drawn
		draw_enable(1);

		if (plates_on != PLATES_COMING_ON)
		{
			if (plates_on != PLATES_OFF)
			{
				// Wait for plates; set time-out so we don't hang
				i = PLATE_TRANSITION_TICKS * 2;
				// Do it this way so we can sync-up the sound
				while (plates_on != PLATES_OFF && --i) sleep(1);
				// Do sound only if the transition finished, not the time-out
//				if (i) snd_scall_bank(plyr_bnk_str, P_HARD_HIT2_SND, VOLUME4, 127, PRIORITY_LVL5);
				if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
				// Make sure plates go away
				plates_on = PLATES_OFF;
				sleep(1);
			}
			// Turn on the plates
			iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		}
		// Wait for plates; set time-out so we don't hang
		i = PLATE_TRANSITION_TICKS * 2;
		// Do it this way so we can sync-up the sound
		while (plates_on != PLATES_ON && --i) sleep(1);
		// Do sound only if the transition finished, not the time-out
//		if (i) snd_scall_bank(plyr_bnk_str, P_HARD_HIT2_SND, VOLUME4, 127, PRIORITY_LVL5);
		if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
	}

	// Disable drawing (re-enabled by name_entry or below)
	draw_enable(0);
	wipeout();
	sleep(1);

	// In case we were in a fade
	normal_screen();

//	snd_scall_bank(intro_bnk_str,9,VOLUME2,127,SND_PRI_SET|SND_PRI_TRACK0);

	for (i = 0; i < MAX_PLYRS; i++)
	{
		// default secret head values (none)
		game_info.team_head[i] = -1;

		// wipe old secret plays
		game_info.has_custom[i] = FALSE;

		// Set default audibles
		audible_ids[i] = audible_ids_default;
	}

	// clear out record ram
	clear_cmos_record((char *)&player1_data,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player2_data,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player3_data,sizeof(plyr_record_t));
	clear_cmos_record((char *)&player4_data,sizeof(plyr_record_t));

#if defined(VEGAS)
	if (!reload)
	{
		// Check game select & game lock DIPs
		if ((get_dip_coin_current() & DIP_BIT_6) == 0 ||	// 0 if no lock, 1 if lock
			(get_dip_coin_current() & DIP_BIT_7) == 1)		// 0 if NFL, 1 if NBA
		{
			if (game_select(NFL_RUNNING))		// for NBA change to NBA_RUNNING
			{
				// REBOOT PLEASE!!!!
				#ifdef DEBUG
					fprintf( stderr,"SELECT> REBOOT! reload = %d\n", reload);
				//	while(1) sleep(1);
				#endif

				display_loading(NBA_RUNNING);
				// Re-create the wipeout'd plate objects
				create_plates();
				// Take away the plates
				iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
				draw_enable(1);
				sleep(120);
				fade(0.0f, 28, NULL);
				sleep(30);
				{
					int sargs[4];

					sargs[0] = 1;
					sargs[1] = pnum;
					sargs[2] = 0;
					sargs[3] = 0;
#ifdef USE_DISK_CMOS
					write_cmos_to_disk(FALSE);
					update_other_cmos_to_disk(FALSE);
#endif
					draw_enable(0);
					snd_master_volume(0);
					setdisk(0);				// Set NBA Partition
					write_game_info();		// Write out valuable info
					exec("GAME.EXE", 0xbebeefed, sargs);
				}
			}
		}
	}
	else
	{
		for (i = 0; i < MAX_PLYRS; i++)
		{
			if (pdata_blocks[i].quarters_purchased >= 4)
				qcreate("fgbought",0,fullgame_bought_msg,i,0,0,0);
		}
	}

	// chalk 'game start' audit
	set_coin_hold(FALSE);
	reload = 0;
#endif

	increment_audit(GAME_START_AUD);
	update_gamestart_percents();

	free_title_flash();

	snd_scall_bank(intro_bnk_str,9,VOLUME5,127,SND_PRI_SET|SND_PRI_TRACK0);

	// have player choose between "create play", "enter name", "select team"
	option_screen(pnum);

	// was CREATE-A-PLAY selected ?
	if (option == 0)
	{
		cap_pnum = pnum;
//		qcreate("cap", CREATE_PLAY_PID, create_play_proc, pnum, 0, 0, 0);
		create_process( "cap", CREATE_PLAY_PID, NULL, create_play_proc, 2048 );
		die(0);
	}

	if ((!no_name_entry) && (option == 1))
		name_entry(0);
	//!!!FIX
//	else
//	{
//		// !!!FIX (this happens if won free game... and going right to team selection)
//		// must load textures assumed to be in tmu from NAME ENTRY screen
//		if (!create_texture("plqcrd00.wms", JAPPLE_BOX_TID, 0, CREATE_NORMAL_TEXTURE,
//			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
//			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR))
//		{
//#ifdef DEBUG
//				fprintf( stderr,"Error loading plqcrd00.wms' texture \n");
//#endif
//				increment_audit(TEX_LOAD_FAIL_AUD);
//		}
//
//		if (!create_texture("plqcrd01.wms", JAPPLE_BOX_TID, 0, CREATE_NORMAL_TEXTURE,
//			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
//			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR))
//		{
//#ifdef DEBUG
//				fprintf( stderr,"Error loading plqcrd01.wms' texture \n");
//#endif
//				increment_audit(TEX_LOAD_FAIL_AUD);
//		}
//
//		draw_backdrop();

//		qcreate("credbx1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//		qcreate("credbx2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//
//		if (four_plr_ver)
//		{
//			qcreate("credbx3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//			qcreate("credbx4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//		}
//
//		qcreate("cred_msg",CREDIT_PID,credit_msg,screen,JAPPLE_BOX_TID,0,0);

//		qcreate("p1chlngr",CREDIT_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
//		qcreate("p2chlngr",CREDIT_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);
//
//		if (four_plr_ver)
//		{
//			qcreate("p3chlngr",CREDIT_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);
//			qcreate("p4chlngr",CREDIT_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
//		}

//		qcreate("timer",TIMER_PID,timedown,1,9,SPRITE_HRES/2.0f,50.0F);							//wait 1 second then cnt from 9

//		draw_enable(1);
//	}
	team_selection();
	fullgame_bought_ok = 0;				// set only when its OK to show the plaqs
	sleep(50-PLATE_TRANSITION_TICKS);	//let slam sound finish
	vs_screen();

	launch_game_proc();
}


//-------------------------------------------------------------------------------------------------
//	This process loads up proper rotating logo textures for show_team_info_pg
//	then rotates it
//
//	INPUT:	team #
//	RETURN:	Nothing
//-------------------------------------------------------------------------------------------------
//volatile int	lg_loaded;
//
//void show_logo_proc(int *args)
//{
//	sprite_info_t * plogo;
//	image_info_t ** ppii;
//	int	i;
//
//	// must fill in the rl_???00.wms with the appropiate team abbrev.
//	// 4 textures are used
//	for (i=0; i < 4; i++)
//	{
//		rotate_logo_textures_2_tbl[i].t_name[3] = teaminfo[args[0]].prefix[0];
//		rotate_logo_textures_2_tbl[i].t_name[4] = teaminfo[args[0]].prefix[1];
//		rotate_logo_textures_2_tbl[i].t_name[5] = teaminfo[args[0]].prefix[2];
//	}
//
//	// now load the rotating logo textures
//	if(load_textures_into_tmu(rotate_logo_textures_2_tbl))
//	{
//		fprintf(stderr, "Couldn't load all rotating logo textures into tmu\r\n");
//#ifdef DEBUG
//		lockup();
//#else
//		return;
//#endif
//	}
//
//	lg_loaded = 1;
//
//	i = 0;
//	ppii = teaminfo[args[0]].prl;
//	plogo = beginobj(ppii[i], 109.0f, SPRITE_VRES-70, 5.0f, ROTATE_LOGO_TID);
//	plogo->w_scale = 1.40f;
//	plogo->h_scale = 1.50f;
//	generate_sprite_verts(plogo);
//
//	while (1)
//	{
//		change_img_tmu(plogo, ppii[i], ROTATE_LOGO_TID);
//		generate_sprite_verts(plogo);
//
////		fprintf(stderr, "Frame %d \r\n",i);
//
//		if (!ppii[++i]) i = 0;
//
//		sleep(2);
//	}
//}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void team_stats_page(void)
{
	sprite_info_t * bkgrnd_obj;
	game_info_t	* pgi;
	tdata_t 	* t1data;
	tdata_t 	* t2data;
	float 		stat_z;
	int i, t1, t2, t1a, t2a, t1b, t2b, t1avg, t2avg;
	int scr0, scr1;
	int tci;
	int			was_low_res = is_low_res;

	screen = HALFTIME_SCRN;

//fprintf(stderr, "STATS> in\n");

	if (game_info.game_state != GS_ATTRACT_MODE)
	{

		pgi = &game_info;
		t1data = tdata_blocks;
		t2data = tdata_blocks + 1;

//fprintf(stderr, "STATS> non-attract\n");
		if (pgi->game_quarter < 3)
		{
			// While stadium is still being shown, load up next tune
			load_tunes();
			// Won't return here until banks are loaded

			// Stop curent music
			snd_stop_track(SND_TRACK_0);
			snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

			snd_scall_bank(eoq_tune_bnk_str,0,VOLUME5,127,SND_PRI_SET|SND_PRI_TRACK1);
//fprintf(stderr, "HALFTIME> %s bank\n", eoq_tune_bnk_str);
//			pgi = &game_info;

		} else {
			load_gameover_tunes();
			// Stop curent music
			snd_stop_track(SND_TRACK_0);
			snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

			snd_scall_bank("gameover",0,VOLUME5,127,SND_PRI_SET|SND_PRI_TRACK1);

		}
	}
	else
	{
		pgi = &game_info_last;

		t1data = tdata_blocks_last;
		t2data = tdata_blocks_last + 1;
	}


//	if (0)//was_low_res)
//	{
//		if(pgi->game_quarter < 3)
//		{
//			if (load_textures_into_tmu(hlf_stats_pg_textures_tbl_lr))
//			{
//				fprintf(stderr, "Couldn't load all stat page textures into tmu\r\n");
//	#ifdef DEBUG
//				lockup();
//	#else
//				return;
//	#endif
//			}
//		}
//		else
//		{
//			if (load_textures_into_tmu(final_stats_pg_textures_tbl_lr))
//			{
//				fprintf(stderr, "Couldn't load final stats page textures into tmu\r\n");
//	#ifdef DEBUG
//				lockup();
//	#else
//				return;
//	#endif
//			}
//		}
//	}
//	else
	{
		if(pgi->game_quarter < 3)
		{
			if (load_textures_into_tmu(hlf_stats_pg_textures_tbl))
			{
				fprintf(stderr, "Couldn't load all stat page textures into tmu\r\n");
	#ifdef DEBUG
				lockup();
	#else
				return;
	#endif
			}
		}
		else
		{
			if (load_textures_into_tmu(final_stats_pg_textures_tbl))
			{
				fprintf(stderr, "Couldn't load final stats page textures into tmu\r\n");
	#ifdef DEBUG
				lockup();
	#else
				return;
	#endif
			}
		}
	}
//	lock_multiple_textures(STATS_PG_TID,0xFFFFFFFF);

	stat_z = 5.0F;

//	if (0)//was_low_res)
//	{
//		// Make sure you put this back before sleeping!!!
//		is_low_res = 0;
//
//
//		if(pgi->game_quarter < 3)
//		{
//			bkgrnd_obj = beginobj(&halfstat_lr,    0.0f, 256.0f, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//			bkgrnd_obj = beginobj(&halfstat_lr_c1, 0.0f, 256.0f, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;																
//		}
//		else
//		{
//			bkgrnd_obj = beginobj(&finalstat_lr,    0.0f, 256.0f, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;
//			bkgrnd_obj = beginobj(&finalstat_lr_c1, 0.0f, 256.0f, stat_z+1, STATS_PG_TID);
//			bkgrnd_obj->id = 1;																
//		}
//
//
//		// Back it goes!!!
//		is_low_res = was_low_res;
//	}
//	else
	{
		if(pgi->game_quarter < 3)
		{
			bkgrnd_obj = beginobj(&halfstat,    0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
			bkgrnd_obj = beginobj(&halfstat_c1, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;																
			bkgrnd_obj = beginobj(&halfstat_c2, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
			bkgrnd_obj = beginobj(&halfstat_c3, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
		}
		else
		{
			bkgrnd_obj = beginobj(&finalstat,    0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
			bkgrnd_obj = beginobj(&finalstat_c1, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;																
			bkgrnd_obj = beginobj(&finalstat_c2, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
			bkgrnd_obj = beginobj(&finalstat_c3, 0.0f, SPRITE_VRES, stat_z+1, STATS_PG_TID);
			bkgrnd_obj->id = 1;
		}
	}


	// create TEAM 1 logo
	bkgrnd_obj = beginobj(teaminfo[pgi->team[0]].pii_logo,
			logo_xys[screen][0][four_plr_ver][X_VAL],
			logo_xys[screen][0][four_plr_ver][Y_VAL]+2,
			1.5F,
			PLAY_SELECT_TID);
	bkgrnd_obj->id = 1;


	// create TEAM 2 logo
	bkgrnd_obj = beginobj(teaminfo[pgi->team[1]].pii_logo,
			logo_xys[screen][1][four_plr_ver][X_VAL],
			logo_xys[screen][1][four_plr_ver][Y_VAL]+2,
			1.5F,
			PLAY_SELECT_TID);
	bkgrnd_obj->id = 1;

	// print players names
	{
		float y1 = was_low_res ? 138 : 207;
		float y2 = was_low_res ? 124 : 185;

		is_low_res = 0;
		print_players_name(0,  64, y1, player1_data.plyr_name, 0);
		if (four_plr_ver)
		{
			print_players_name(1,  64, y2, player2_data.plyr_name, 0);
			print_players_name(2, 448, y1, player3_data.plyr_name, 0);
			print_players_name(3, 448, y2, player4_data.plyr_name, 0);
		}
		else
		{
			print_players_name(1, 448, y1, player2_data.plyr_name, 0);
		}
		is_low_res = was_low_res;
	}

	// print team stats
	if (was_low_res)
		set_text_font(FONTID_BAST10LOW);
	else
		set_text_font(FONTID_BAST10);
		
	set_string_id(STAT_PG_TXT_ID);

	// time of possesion
	t1 = t2 = WHITE;
	if (t1data->time_of_poss > t2data->time_of_poss) t1 = LT_GREEN;
	else
	if (t1data->time_of_poss < t2data->time_of_poss) t2 = LT_GREEN;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[0], stat_z);
	oprintf("%dj%dc%d:%02d", tm_stat_pg_stats_js[0], t1,
		t1data->time_of_poss / 60,
		t1data->time_of_poss % 60 );
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[0], stat_z);
	oprintf("%dj%dc%d:%02d", tm_stat_pg_stats_js[1], t2,
		t2data->time_of_poss / 60,
		t2data->time_of_poss % 60 );

	// 1st downs
	t1 = t2 = WHITE;
	if (t1data->first_downs > t2data->first_downs) t1 = LT_GREEN;
	else
	if (t1data->first_downs < t2data->first_downs) t2 = LT_GREEN;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[1], stat_z);
	oprintf("%dj%dc%d", tm_stat_pg_stats_js[0], t1, t1data->first_downs);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[1], stat_z);
	oprintf("%dj%dc%d", tm_stat_pg_stats_js[1], t2, t2data->first_downs);

	// 4th down conversions
	t1 = t2 = WHITE;
	if (t1data->forthdwn_convs > t2data->forthdwn_convs) t1 = LT_GREEN;
	else
	if (t1data->forthdwn_convs < t2data->forthdwn_convs) t2 = LT_GREEN;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[2], stat_z);
	oprintf("%dj%dc%d/%d", tm_stat_pg_stats_js[0], t1, t1data->forthdwn_convs, t1data->forthdwn_attempts);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[2], stat_z);
	oprintf("%dj%dc%d/%d", tm_stat_pg_stats_js[1], t2, t2data->forthdwn_convs, t2data->forthdwn_attempts);

	// interceptions
	t1 = t2 = WHITE;
	if (t1data->interceptions > t2data->interceptions) t1 = LT_GREEN;
	if (t1data->interceptions < t2data->interceptions) t2 = LT_GREEN;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[3], stat_z);
	oprintf("%dj%dc%d", tm_stat_pg_stats_js[0], t1, t1data->interceptions);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[3], stat_z);
	oprintf("%dj%dc%d", tm_stat_pg_stats_js[1], t2, t2data->interceptions);

	// fumbles
	t1 = t2 = t1a = t2a = t1b = t2b = WHITE;
	if (t1data->fumbles > t2data->fumbles) t1 = LT_RED2;
	else
	if (t1data->fumbles < t2data->fumbles) t2 = LT_RED2;
	if (t1data->lost_fumbles > t2data->lost_fumbles) t1b = LT_RED2;
	else
	if (t1data->lost_fumbles < t2data->lost_fumbles) t2b = LT_RED2;
	if (t1 == t1b) t1a = t1;
	if (t2 == t2b) t2a = t2;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[4], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[0], t1, t1data->fumbles, t1a, t1b, t1data->lost_fumbles);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[4], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[1], t2, t2data->fumbles, t2a, t2b, t2data->lost_fumbles);

	// tackles / sacks
	t1 = t2 = t1a = t2a = t1b = t2b = WHITE;
	if (t1data->tackles > t2data->tackles) t1 = LT_GREEN;
	else
	if (t1data->tackles < t2data->tackles) t2 = LT_GREEN;
	if (t1data->sacks > t2data->sacks) t1b = LT_GREEN;
	else
	if (t1data->sacks < t2data->sacks) t2b = LT_GREEN;
	if (t1 == t1b) t1a = t1;
	if (t2 == t2b) t2a = t2;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[5], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[0], t1, t1data->tackles, t1a, t1b, t1data->sacks);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[5], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[1], t2, t2data->tackles, t2a, t2b, t2data->sacks);

	// completions / attempts
	t1 = t2 = WHITE;
	if (t1data->completions > t2data->completions) t1 = LT_GREEN;
	else
	if (t1data->completions < t2data->completions) t2 = LT_GREEN;
	else
	if (t1data->passes < t2data->passes) t1 = LT_GREEN;
	else
	if (t1data->passes > t2data->passes) t2 = LT_GREEN;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[6], stat_z);
	oprintf("%dj%dc%d/%d", tm_stat_pg_stats_js[0], t1, t1data->passes, t1data->completions);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[6], stat_z);
	oprintf("%dj%dc%d/%d", tm_stat_pg_stats_js[1], t2, t2data->passes, t2data->completions);

	// pass yards total/avg
	t1 = t2 = t1a = t2a = t1b = t2b = WHITE;
	if ((t1avg = t1data->completions)) t1avg = (int)(0.49f + (float)(t1data->passing_yards) / (float)t1avg);
	if ((t2avg = t2data->completions)) t2avg = (int)(0.49f + (float)(t2data->passing_yards) / (float)t2avg);
	if (t1data->passing_yards > t2data->passing_yards) t1 = LT_GREEN;
	else
	if (t1data->passing_yards < t2data->passing_yards) t2 = LT_GREEN;
	if (t1avg > t2avg) t1b = LT_GREEN;
	else
	if (t1avg < t2avg) t2b = LT_GREEN;
	if (t1 == t1b) t1a = t1;
	if (t2 == t2b) t2a = t2;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[7], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[0], t1, t1data->passing_yards, t1a, t1b, t1avg);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[7], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[1], t2, t2data->passing_yards, t2a, t2b, t2avg);

	// rush yards total/avg
	t1 = t2 = t1a = t2a = t1b = t2b = WHITE;
	if ((t1avg = t1data->rushes)) t1avg = (int)(0.49f + (float)(t1data->rushing_yards) / (float)t1avg);
	if ((t2avg = t2data->rushes)) t2avg = (int)(0.49f + (float)(t2data->rushing_yards) / (float)t2avg);
	if (t1data->rushing_yards > t2data->rushing_yards) t1 = LT_GREEN;
	else
	if (t1data->rushing_yards < t2data->rushing_yards) t2 = LT_GREEN;
	if (t1avg > t2avg) t1b = LT_GREEN;
	else
	if (t1avg < t2avg) t2b = LT_GREEN;
	if (t1 == t1b) t1a = t1;
	if (t2 == t2b) t2a = t2;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[8], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[0], t1, t1data->rushing_yards, t1a, t1b, t1avg);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[8], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[1], t2, t2data->rushing_yards, t2a, t2b, t2avg);

	// returns yards total/avg
	t1 = t2 = t1a = t2a = t1b = t2b = WHITE;
	if ((t1avg = t1data->kick_returns)) t1avg = (int)(0.49f + (float)(t1data->return_yards) / (float)t1avg);
	if ((t2avg = t2data->kick_returns)) t2avg = (int)(0.49f + (float)(t2data->return_yards) / (float)t2avg);
	if (t1data->return_yards > t2data->return_yards) t1 = LT_GREEN;
	else
	if (t1data->return_yards < t2data->return_yards) t2 = LT_GREEN;
	if (t1avg > t2avg) t1b = LT_GREEN;
	else
	if (t1avg < t2avg) t2b = LT_GREEN;
	if (t1 == t1b) t1a = t1;
	if (t2 == t2b) t2a = t2;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[9], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[0], t1, t1data->return_yards, t1a, t1b, t1avg);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[9], stat_z);
	oprintf("%dj%dc%d%dc/%dc%d", tm_stat_pg_stats_js[1], t2, t2data->return_yards, t2a, t2b, t2avg);

	// total yards
	t1 = t2 = WHITE;
	if (t1data->total_yards > t2data->total_yards) t1 = LT_GREEN;
	else
	if (t1data->total_yards < t2data->total_yards) t2 = LT_GREEN;
	set_text_position(tm_stat_pg_stats_xs[0], tm_stat_pg_stats_ys[10], stat_z);
	oprintf("%dj%dc%d", tm_stat_pg_stats_js[0], t1, t1data->total_yards);
	set_text_position(tm_stat_pg_stats_xs[1], tm_stat_pg_stats_ys[10], stat_z);
	oprintf("%dj%dc%d", tm_stat_pg_stats_js[1], t2, t2data->total_yards);


	// print team names
	set_text_position(stat_pg_scr_sprd_xs[0], stat_pg_scr_sprd_ys[!!was_low_res][0], stat_z);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, teaminfo[pgi->team[0]].home);

	set_text_position(stat_pg_scr_sprd_xs[0], stat_pg_scr_sprd_ys[!!was_low_res][1], stat_z);
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, teaminfo[pgi->team[1]].home);

	// print team 1's and team 2's score spread
	// FIX!!! we only have room for two overtime slots on stats pages
	for (i = 0; i < pgi->game_quarter; i++)
	{
		if (i < (MAX_QUARTERS-1))
		{
			set_text_position(stat_pg_scr_sprd_xs[i+1], stat_pg_scr_sprd_ys[!!was_low_res][0], stat_z);
			oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), WHITE, pgi->scores[0][i]);

			set_text_position(stat_pg_scr_sprd_xs[i+1], stat_pg_scr_sprd_ys[!!was_low_res][1], stat_z);
			oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), WHITE, pgi->scores[1][i]);
		}
	}

	// calc final scores
	scr0 = scr1 = 0;
	for (i = 0; i < MAX_QUARTERS; i++)
	{
		scr0 += pgi->scores[0][i];
		scr1 += pgi->scores[1][i];
	}

	// print final scores
	set_text_position(stat_pg_scr_sprd_xs[7], stat_pg_scr_sprd_ys[!!was_low_res][0], stat_z);
	oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, scr0);

	set_text_position(stat_pg_scr_sprd_xs[7], stat_pg_scr_sprd_ys[!!was_low_res][1], stat_z);
	oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, scr1);

	// print scores in big font
	set_text_font(FONTID_BAST25);
	set_text_position(stat_pg_scrs_xys[!!was_low_res][0][X_VAL], stat_pg_scrs_xys[!!was_low_res][0][Y_VAL], stat_z);
	oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_TOP), LT_YELLOW, scr0);

	set_text_position(stat_pg_scrs_xys[!!was_low_res][1][X_VAL], stat_pg_scrs_xys[!!was_low_res][1][Y_VAL], stat_z);
	oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_TOP), LT_YELLOW, scr1);

	if (game_info.game_state == GS_ATTRACT_MODE)
	{
		// Attract-mode stats stuff goes here
		return;
	}

	// Take away the plates
	iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);

	// wait for X ticks or for a button press after N ticks
	wait_for_any_butn_press(tsec*1,tsec*22);

	tci = (game_info.game_quarter < 3) ? 0 : trivia_contest_init();

	if (!tci && (game_info.game_quarter < 3 || !record_me))
	{
		// NOPE - Do nothing
		// Turn on the plates
		iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		// Wait for plates; set time-out so we don't hang
		i = PLATE_TRANSITION_TICKS * 2;
		// Do it this way so we can sync-up the sound
		while (plates_on != PLATES_ON && --i) sleep(1);

		// Stop current music
		snd_stop_track(SND_TRACK_1);
		snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

		// Play drum loop under transition plates
		snd_scall_bank(generic_bnk_str, 16, VOLUME5, 127, SND_PRI_SET|SND_PRI_TRACK0);

		// Do sound only if the transition finished, not the time-out
		if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME5, 127, PRIORITY_LVL5);
	}
//	else if ((i = trivia_contest_init())) draw_enable(0);

	// do clean up
	del1c(1, 0xffffffff);
//	killall(ROTATE_LOGO_PID, 0xffffffff);
	delete_multiple_strings(STAT_PG_TXT_ID, 0xffffffff);
	delete_multiple_textures(STATS_PG_TID,0xffffffff);
//	delete_multiple_textures(ROTATE_LOGO_TID,0xffffffff);

	// delete player name strings
	delete_multiple_strings(plr_nme_str_ids[0], 0xffffffff);
	delete_multiple_strings(plr_nme_str_ids[1], 0xffffffff);

	if (four_plr_ver)
	{
		delete_multiple_strings(plr_nme_str_ids[2], 0xffffffff);
		delete_multiple_strings(plr_nme_str_ids[3], 0xffffffff);
	}

	if (tci) trivia_contest();
}

//-------------------------------------------------------------------------------------------------
//				This process handles the rotating of a team logo
//
// INPUT: side 0 or 1, x, y, and z
//
//-------------------------------------------------------------------------------------------------
//void rotate_logo(int *args)
//{
//	sprite_info_t * plogo;
//	image_info_t ** ppii;
//	int	i;
//
//	i = 0;
//	ppii = teaminfo[args[0]].prl;
//	plogo = beginobj(ppii[i], args[1], args[2], args[3], ROTATE_LOGO_TID);
//	plogo->id = 1;
//	plogo->w_scale = 1.21f;
//	plogo->h_scale = 1.21f;
//	generate_sprite_verts(plogo);
//
//	while (1)
//	{
//		change_img_tmu(plogo, ppii[i], ROTATE_LOGO_TID);
//		plogo->w_scale = 1.21f;
//		plogo->h_scale = 1.21f;
//		generate_sprite_verts(plogo);
//
//		if (!ppii[++i]) i = 0;
//
//		sleep(2);
//	}
//}

//-------------------------------------------------------------------------------------------------
//	This process flashes bonus price message on YES/NO screen
//
// 	INPUT:	Strings to flash (1 to 4) or sleep ticks if > 0
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void purchase_flasher(int *args)
{
	ostring_t	*pmsg[4] = {0, 0, 0, 0};
	int			pmsg_org_color[4];
	int			ticks, i;

	ticks = 15;

	for (i = 0; i < 4; i++)
	{
		if (args[i] > 0)
			ticks = args[i];
		if (args[i] < 0)
		{
			pmsg[i] = (ostring_t *)*(args+i);
			pmsg_org_color[i] = pmsg[i]->state.color;
		}
	}

	while(1)
	{
		sleep(ticks);
		for (i = 0; i < 4; i++)
		{
			if (pmsg[i])
			{
				pmsg[i]->state.color = WHITE;
				change_string_state(pmsg[i]);
			}
		}
		sleep(ticks);
		for (i = 0; i < 4; i++)
		{
			if (pmsg[i])
			{
				pmsg[i]->state.color = pmsg_org_color[i];
				change_string_state(pmsg[i]);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	This process turns on a bonus pricing message on YES/NO screen if available
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
static process_node_t	*pflasher = NULL;
static ostring_t			*pmsg[2];

void show_bonus_proc(int *args)
{
//	int i;

	if (!(coin_check_freeplay()))
	{
		// Allow each sequential letter to overlap the previous letter - turn off at end!
		set_text_z_inc(1.0F);
		set_text_id (1);
		set_string_id (1);

		if (full_game_credits < (get_credits_to_start() + (get_credits_to_continue() * 3)))
		{
			// If credits for full game is not zero - then it is valid
			if (full_game_credits)
			{
				// create the "N credits to purchase full game" string
			 	set_text_font (FONTID_BAST10);
				set_text_color(LT_RED);

//				set_text_position(SPRITE_HRES/2, 95.0F, 5.0F);
//				pmsg[0] = oprintf("%djBONUS PRICE:", (HJUST_CENTER|VJUST_CENTER));

				set_text_position(SPRITE_HRES/2, 80.0f, 5.0f);//79.0F, 5.0F);
				pmsg[0] = oprintf("%djPURCHASE FULL GAME UP FRONT FOR ONLY %d CREDITS", (HJUST_CENTER|VJUST_CENTER), get_full_game_credits());

				qcreate("pflashA",BONUS_PID, purchase_flasher, (int)pmsg[0], 0, 0, 0);
			}
		}
//		// Chk for & show 4 player free game msg
//		if (!get_adjustment(H4_FREE_GAME_ADJ, &i))
//		{
//			if (four_plr_ver && i)
//			{
//			 	set_text_font (FONTID_BAST10);
//				set_text_color(LT_GREEN);
//
//				set_text_position(SPRITE_HRES/2, 76.0f, 5.0f);
//				pmsg[1] = oprintf("%djWINNERS OF A 4 PLAYER GAME STAY ON FREE!", HJUST_CENTER|VJUST_CENTER);
//
//				qcreate("pflash",BONUS_PID, purchase_flasher, (int)pmsg[1], 10, 0, 0);
//			}
//		}

		set_text_z_inc(0.0F);
	}
}

//-------------------------------------------------------------------------------------------------
//	This process welcomes the player with some speech
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
static void welcome_proc(int *args)
{
	int scalls[] = { 11, 6, 7 };
	int val;

	sleep(50);		// 90
	val = randrng(sizeof(scalls) / sizeof(int));
	snd_scall_bank(intro_bnk_str,scalls[val],VOLUME4,127,SND_PRI_SET|SND_PRI_TRACK5);
//	snd_scall_bank(intro_bnk_str,6,VOLUME3,127,SND_PRI_SET|SND_PRI_TRACK5);
}

//-------------------------------------------------------------------------------------------------
//			 Have player choose between "create play", "enter name", "select team"
//
// returns: -1, 0, or 1 
//-------------------------------------------------------------------------------------------------
void option_screen(int p_num)
{
	int	i;
	sprite_info_t * psprite;
	process_node_t * ptimer;

	// Disable the disk interrupt callback (IN CASE...started on some attract screens..ie team info)
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

	if (load_textures_into_tmu(opt_scrn_textures_tbl))
	{
		fprintf(stderr, "Couldn't load all option screen textures into tmu\r\n");
#ifdef DEBUG
		lockup();
#else
		// Kill any that did load
		delete_multiple_textures(OPT_SCR_TID,0xffffffff);

		// Re-create the wipeout'd plate objects
		create_plates();

		// Default select initials entry
		option = 1;
		return;
#endif
	}

	// must LOCK these textures so that one player doesnt delete them for the other player
	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

	// set some exit variables
	force_selection = 0;
	exit_status = 0;
	option = 1;					// default to ENTER NAME

	qcreate("cred_msg",CREDIT_PID,credit_msg,screen,JAPPLE_BOX_TID,0,0);
//	ptimer = qcreate("timer",TIMER_PID,timedown,10,9,SPRITE_HRES/2.0f,50.0F);							//wait 1 second then cnt from 9
  	ptimer = qcreate("timer",TIMER_PID,timedown,1,9,SPRITE_HRES/2.0f,80.0F);							//wait 1 second then cnt from 9

	qcreate("credbx1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	qcreate("credbx2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);

	if (four_plr_ver)
	{
		qcreate("credbx3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("credbx4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
	}

	// draw background
	psprite = beginobj(&mbg,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&mbg_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&mbg_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&mbg_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;

	// Re-create the wipeout'd plate objects
	create_plates();

	// Welcome speech with slight delay
	qcreate("welcome",0,welcome_proc,0,0,0,0);

	// Show bonus price message if available
	qcreate("bonus",CREDIT_PID,show_bonus_proc,0,0,0,0);		

	// Take away the plates
	iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);

	sleep(1);

	// Lets see them
	draw_enable(1);
	normal_screen();

	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way since we can't know how many ticks have lapsed since transition began
	while (plates_on != PLATES_OFF && --i) sleep(1);
	// Make sure plates are gone
	plates_on = PLATES_OFF;

	fullgame_bought_ok = 1;		// set only when its OK to show the plaqs

	// Chk for & show 4 player free game msg
	if (!get_adjustment(H4_FREE_GAME_ADJ, &i))
	{
		if (four_plr_ver && i)
		{
			sprite_info_t * idiot_bx_obj;
			int ticks = 0;

			idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f, 1.01f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 2.22f;
			idiot_bx_obj->h_scale = 1.9f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			set_text_z_inc(0.0f);

			set_string_id(0x34);
			set_text_font(is_low_res ? FONTID_BAST23LOW : FONTID_BAST25);
			set_text_color(LT_RED);

			if (is_low_res)
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+25.0f-7, 1.0F);
			else
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+25.0f, 1.0F);
			pmsg[0] = oprintf( "%djWINNERS OF A 4 PLAYER", (HJUST_CENTER|VJUST_CENTER));

			if (is_low_res)
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-20.0f-7, 1.0F);
			else
				set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-20.0f, 1.0F);
			pmsg[1] = oprintf( "%djGAME STAY ON FREE!", (HJUST_CENTER|VJUST_CENTER));

			pflasher = qcreate("pflashB", MESSAGE_PID, flash_text_proc, (int)pmsg[0], (int)pmsg[1], 0, 0);
			suspend_process(ptimer);

			snd_scall_bank(cursor_bnk_str,WARNING_SND,VOLUME4,127,PRIORITY_LVL5);

			// Wait for timeout or button press
			do
			{
				sleep(1);
				ticks++;

				if (ticks >= 50)	// min ticks
				{
					i = get_player_sw_current();

					if ((p_status & 1) && (i & P1_ABCD_MASK)) break;
					if ((p_status & 2) && (i & P2_ABCD_MASK)) break;
					if ((p_status & 4) && (i & P3_ABCD_MASK)) break;
					if ((p_status & 8) && (i & P4_ABCD_MASK)) break;
				}
			} while (ticks < 240);	// max ticks

			resume_process(ptimer);
			delete_multiple_strings(0x34, -1);
			delobj(idiot_bx_obj);
			kill(pflasher, 0);
		}
	}

	// start 'selection' process
	qcreate("slctopt",0,plyr_select_option_proc,p_num,0,0,0);

	do
	{
		sleep(1);

	} while (exit_status);
//	} while ((exit_status) && (!force_selection));

	// Turn on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way so we can sync-up the sound
	while (plates_on != PLATES_ON && --i)
		sleep(1);
	// Do sound only if the transition finished, not the time-out
	if (i)
		snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

	//
	// Now delete all objects and textures from this screen
	//
	del1c(1, 0xFFFFFFFF);
	killall(TIMER_PID,0xFFFFFFFF);										//	kill_countdown_timer();
	delete_multiple_textures(OPT_SCR_TID,0xffffffff);
	delete_multiple_strings(0x45, 0xffffffff);
	delete_multiple_strings(0x46, 0xffffffff);

	if (option == 0)
	{
		killall(CREDIT_PID,0xFFFFFFFF);										//	kill_countdown_timer();
		del1c(0, 0xFFFFFFFF);
//		del1c(2, 0xFFFFFFFF);			// this will kill the transition plates
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void plyr_select_option_proc(int *args)
{
	int pnum            = args[0];
	int plyrs           = four_plr_ver ? 4:2;
	int btn_dly         = 30;
	int cap_allowed     = 1;
	int option_selected = 0;
	int asking_yesno    = 0;
	int answer          = 0;
	int switches, buttons, i;

	sprite_info_t * cap_plq_obj  = beginobj(&cboxt,    option_xys[four_plr_ver][0][X_VAL], option_xys[four_plr_ver][0][Y_VAL], 14.0F, OPT_SCR_TID);
	sprite_info_t * init_plq_obj = beginobj(&cbox,     option_xys[four_plr_ver][1][X_VAL], option_xys[four_plr_ver][1][Y_VAL], 14.0F, OPT_SCR_TID);
	sprite_info_t * sel_plq_obj  = beginobj(&cboxt,    option_xys[four_plr_ver][2][X_VAL], option_xys[four_plr_ver][2][Y_VAL], 14.0F, OPT_SCR_TID);
	sprite_info_t * cap_txt_obj  = beginobj(&mcreatet, option_xys[four_plr_ver][0][X_VAL], option_xys[four_plr_ver][0][Y_VAL], 12.0F, OPT_SCR_TID);
	sprite_info_t * init_txt_obj = beginobj(&menter,   option_xys[four_plr_ver][1][X_VAL], option_xys[four_plr_ver][1][Y_VAL], 12.0F, OPT_SCR_TID);
	sprite_info_t * sel_txt_obj  = beginobj(&mselectt, option_xys[four_plr_ver][2][X_VAL], option_xys[four_plr_ver][2][Y_VAL], 12.0F, OPT_SCR_TID);
	sprite_info_t * yn_obj       = beginobj(&myn,      option_xys[four_plr_ver][0][X_VAL], option_xys[four_plr_ver][0][Y_VAL], 10.0F, OPT_SCR_TID);
	sprite_info_t * yn_cur_obj   = beginobj(&mno,      option_xys[four_plr_ver][0][X_VAL], option_xys[four_plr_ver][0][Y_VAL],  9.0F, OPT_SCR_TID);
	sprite_info_t * psprite;

	cap_plq_obj->id  = 1;
	cap_txt_obj->id  = 1;
	init_plq_obj->id = 1;
	init_txt_obj->id = 1;
	sel_plq_obj->id  = 1;
	sel_txt_obj->id  = 1;
	yn_obj->id       = 1;
	yn_cur_obj->id   = 1;

	// set variables
	exit_status |= 1<<pnum;												// SET players bit in variable

	// control loop
	while (!option_selected)
	{
		// Read INPUT
		switches = 0;
		buttons = 0;

		// read joysticks from any active player
		for (i=0; i < plyrs; i++)
		{
			if ((p_status & (1<<i)) && (switches = get_joy_val_down(i)))
				break;
		}
//		switches = get_joy_val_down(pnum);

		if (btn_dly <= 0)
		{
			// read buttons from any active player
			for (i=0; i < plyrs; i++)
			{
				if ((p_status & (1<<i)) && (buttons = get_but_val_down(i)))
					break;
			}
		}
//		buttons = get_but_val_down(pnum);
	
		// if another player buys in... dont allow them to select create-a-play
		if (cap_allowed &&
			(count_players_in_game() > 1			// someone else started?
#ifndef CAP_FIX
			|| count_quarters_purchased() > 1		// same player > 1 qrtr?
#endif
			))
		{
			cap_allowed = 0;
			// put a big X over the create play option
			psprite = beginobj(&mx, option_xys[four_plr_ver][0][X_VAL], option_xys[four_plr_ver][0][Y_VAL], 10.0F, OPT_SCR_TID);
			psprite->id = 1;

			// if cursor on create-a-play...must move it off
			if (option == 0)
			{
				asking_yesno = 0;
				switches = JOY_RIGHT;			// move cursor
				buttons = 0;
			}
			delete_multiple_strings(0x46, 0xffffffff);
			set_string_id(0x46);
			set_text_font(FONTID_BAST10);
			set_text_color(LT_YELLOW);
			set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
			set_text_position(option_xys[four_plr_ver][0][X_VAL]-4, option_xys[four_plr_ver][0][Y_VAL]-97.0f,  1.1F);
			oprintf("ONE PLAYER WITH");
			set_text_position(option_xys[four_plr_ver][0][X_VAL]-4, option_xys[four_plr_ver][0][Y_VAL]-112.0f, 1.1F);
			oprintf("ONE PERIOD ONLY");
		}

		// if screen times out... default to SELECT TEAM
		if (force_selection)
		{
			asking_yesno = 0;
			switches = JOY_RIGHT;				// move cursor
			option   = 1;						// to select team option
			buttons  = 1;						// and select it!
		}

		// Handle Joystick
		if (!asking_yesno)
		{
			i = option;

			if (switches == JOY_LEFT  && option > 0) option--;
			else
			if (switches == JOY_RIGHT && option < 2) option++;

			if (option == 0 && cap_allowed == 0) option = 1;

			if (i != option)
			{
				change_img_tmu(cap_plq_obj, (option == 0 ? &cbox:&cboxt),      OPT_SCR_TID);
				change_img_tmu(cap_txt_obj, (option == 0 ? &mcreate:&mcreatet),OPT_SCR_TID);

				change_img_tmu(init_plq_obj,(option == 1 ? &cbox:&cboxt),      OPT_SCR_TID);
				change_img_tmu(init_txt_obj,(option == 1 ? &menter:&mentert),  OPT_SCR_TID);

				change_img_tmu(sel_plq_obj, (option == 2 ? &cbox:&cboxt),      OPT_SCR_TID);
				change_img_tmu(sel_txt_obj, (option == 2 ? &mselect:&mselectt),OPT_SCR_TID);

				snd_scall_bank(cursor_bnk_str,OPTION_CURSR_SND,VOLUME5,127,PRIORITY_LVL1);
			}
		}
		else
		{
			if ((switches == JOY_UP   && answer == 0) ||
				(switches == JOY_DOWN && answer == 1))
			{
				answer ^= 1;
				change_img_tmu(yn_cur_obj,(answer ? &myes:&mno),OPT_SCR_TID);
				snd_scall_bank(cursor_bnk_str,OPTION_YN_CUR_SND,VOLUME5,127,PRIORITY_LVL1);
			}
		}

		// Handle BUTTONS
		if (buttons)
		{
			// if on CAP option... ask YES/NO
			if (option == 0 && (!asking_yesno || answer == 0))
			{
				answer = 0;
				asking_yesno ^= 1;
				snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME5,127,PRIORITY_LVL1);
			}
			else
				option_selected = 1;
		}

		// Update CREATE PLAY option text/imgs
		delete_multiple_strings(0x45, 0xffffffff);

		if (option == 0)
		{
			set_string_id(0x45);
			set_text_font(FONTID_BAST10);
			set_text_color(LT_YELLOW);
			set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
			set_text_position(option_xys[four_plr_ver][0][X_VAL]-4, option_xys[four_plr_ver][0][Y_VAL]-97.0f,  1.2F);
			if (count_quarters_purchased() > 1)
				oprintf("OPTION WILL USE");
			else
				oprintf("OPTION WILL HAVE");
			set_text_position(option_xys[four_plr_ver][0][X_VAL]-4, option_xys[four_plr_ver][0][Y_VAL]-112.0f, 1.2F);
			if (count_quarters_purchased() > 1)
				oprintf("ONE QUARTER");
			else
				oprintf("NO GAME PLAY");
		}
		if (asking_yesno)
		{
			unhide_sprite(yn_obj);
			unhide_sprite(yn_cur_obj);
			hide_sprite(cap_txt_obj);
		}
		else
		{
			hide_sprite(yn_obj);
			hide_sprite(yn_cur_obj);
			unhide_sprite(cap_txt_obj);
		}

		sleep(1);

		btn_dly--;
	}

	// flash cursor object.
	snd_scall_bank(cursor_bnk_str,20,VOLUME5,127,PRIORITY_LVL4);

	if (option == 0)
		option_select_flash(cap_plq_obj,OPT_SCR_TID);
	else if (option == 1)
		option_select_flash(init_plq_obj,OPT_SCR_TID);
	else
		option_select_flash(sel_plq_obj,OPT_SCR_TID);

	sleep(20);				// let lighting happen

	if (option == 0 &&								// cap selected?
		(count_players_in_game() > 1				// someone else started?
#ifndef CAP_FIX
		|| count_quarters_purchased() > 1			// same player > 1 qrtr?
#endif
		))
	{
		// Someone bought in during flash and sleep; do name entry instead
		option = 1;
	}
	exit_status &= ~(1<<pnum);						// CLEAR players bit in variable
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int count_quarters_purchased(void)
{
	int i;
	int total = 0;

	for (i=0; i < MAX_PLYRS; i++)
		total += pdata_blocks[i].quarters_purchased;
	return(total);
}

//------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int count_players_in_game(void)
{
	int i;
	int plrs = 0;

	for (i=0; i < MAX_PLYRS; i++)
	{
		if (p_status & (1<<i))
			plrs++;
	}
	return(plrs);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0
void lighting_proc(int *args)
{
	sprite_info_t 	*plighting;
	float			x,y;
	int				i;


	x = args[0];
	y = args[1];

	snd_scall_bank(cursor_bnk_str,SEL_YESNO_SND,VOLUME4,127,PRIORITY_LVL3);

	plighting = beginobj(lighting_imgs[0], x, y, 5.0F, JAPPLE_BOX_TID);

	plighting->w_scale = 1.3f;
	plighting->h_scale = 2.0f;
	generate_sprite_verts(plighting);

	for (i=0; i < (sizeof(lighting_imgs)/sizeof(image_info_t *))-1; i++)
//	for (i=0; i < 11; i++)
	{
		change_img_tmu(plighting,lighting_imgs[i],JAPPLE_BOX_TID);
		sleep(2);
	}
	delobj(plighting);

}
#endif

//-------------------------------------------------------------------------------------------------
//  		This process gets the players NAME and PIN NUMBER.			
//-------------------------------------------------------------------------------------------------
void name_entry(int none)
{
	sprite_info_t *ps;
	int i;

	// Disable the disk interrupt callback (IN CASE...started on some attract screens..ie team info)
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

	screen = NAME_ENTRY_SCRN;
	force_selection = 0;

	get_all_records();

	// load the bare minimun texture set just to show screen...
	if (load_textures_into_tmu(name_entry_textures_tbl))
	{
		fprintf(stderr, "Couldn't load all name-entry textures into tmu\r\n");
#ifdef DEBUG
		lockup();
#else
		return;
#endif
	}

	// must LOCK these textures so that one player doesnt delete them for the other player
	lock_multiple_textures(YES_NO_PLAQ_TID,0xFFFFFFFF);
	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

	// Re-create the wipeout'd plate objects
//	create_plates();

	// this will load a texture then sleep using the dma disk code
	qcreate("ldtxturs",0,load_name_entry_textures,0,0,0,0);

	draw_backdrop();

	ps = beginobj(&i99, SPRITE_HRES/2.0f, SPRITE_VRES, 340.0F, JAPPLE_BOX_TID);
//	ps = beginobj(&i99, SPRITE_HRES/2.0f, SPRITE_VRES-55.0f, 340.0F, JAPPLE_BOX_TID);
	ps->state.texture_filter_mode = GR_TEXTUREFILTER_BILINEAR;

	set_text_z_inc(0.0f);

//	// Welcome speech with slight delay
//	qcreate("welcome",CREDIT_PID,welcome_proc,0,0,0,0);
//	// Show bonus price message if available
//	qcreate("bonus",CREDIT_PID,show_bonus_proc,0,0,0,0);		

  	qcreate("timer",TIMER_PID,timedown,40,9,SPRITE_HRES/2.0f,50.0F);							//wait 1 second then cnt from 9
//	qcreate("timer",TIMER_PID,timedown3d,40,9,0,0);												//wait 60 seconds then cnt from 9
	qcreate("cred_msg",CREDIT_PID,credit_msg,screen,JAPPLE_BOX_TID,0,0);

	qcreate("p1_cur",P1_CURSOR_PID,player_cursor,PLYR_1,0,0,0);
	qcreate("p1chlngr",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
	qcreate("p2_cur",P2_CURSOR_PID,player_cursor,PLYR_2,0,0,0);
	qcreate("p2chlngr",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);

	if (four_plr_ver)
	{
		// 4 player four_plr_ver
		qcreate("p3_cur",P3_CURSOR_PID,player_cursor,PLYR_3,0,0,0);
		qcreate("p3chlngr",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);
		qcreate("p4_cur",P4_CURSOR_PID,player_cursor,PLYR_4,0,0,0);
		qcreate("p4chlngr",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
	}

	exit_status = 0;
	players = 0;												// min value

	// Take away the plates
	iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);

	sleep(1);

	// Lets see them
	draw_enable(1);
	normal_screen();

	// wait for rest of images to be loaded
	while(!imgs_loaded)
	{
		sleep(1);
	}


	// must LOCK these textures so that one player doesnt delete them for the other player
	lock_multiple_textures(YES_NO_PLAQ_TID,0xFFFFFFFF);
	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way since we can't know how many ticks have lapsed since transition began
	while (plates_on != PLATES_OFF && --i) sleep(1);
	// Make sure plates are gone
	plates_on = PLATES_OFF;

	do
	{
		sleep(1);
		if ((exit_status == 0) && (players == 1))
		{
			// player selected NO and hes the only player (should we give extra time for other to buy-in or FUCK em ?)

		}
	} while (exit_status);


	// Turn on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way so we can sync-up the sound
	while (plates_on != PLATES_ON && --i)
		sleep(1);
	// Do sound only if the transition finished, not the time-out
	if (i)
		snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

	// freeze the screen
	draw_enable(0);
	sleep(1);

	// Do some clean-up
	// Clean up bonus flashing message
	delete_multiple_strings(1, 0xffffffff);
	killall(BONUS_PID,0xFFFFFFFF);

	// kill processes (if not dead already)
	killall(P1_CURSOR_PID,0xFFFFFFFF);
	killall(P1_CHALNGR_PID,0xFFFFFFFF);
	killall(P2_CURSOR_PID,0xFFFFFFFF);
	killall(P2_CHALNGR_PID,0xFFFFFFFF);

	if (four_plr_ver)
	{
		killall(P3_CURSOR_PID,0xFFFFFFFF);
		killall(P3_CHALNGR_PID,0xFFFFFFFF);
		killall(P4_CURSOR_PID,0xFFFFFFFF);
		killall(P4_CHALNGR_PID,0xFFFFFFFF);
	}
	killall(TIMER_PID,0xFFFFFFFF);										//	kill_countdown_timer();
	delete_multiple_objects( OID_NBR3D, ~0xFF );

	// unlock these textures so they can be deleted
	unlock_multiple_textures(YES_NO_PLAQ_TID,0xFFFFFFFF);

	// delete all the objects with ID of 0 and 1
	del1c(0,0xFFFFFFFF);
	del1c(1,0xFFFFFFFF);
	del1c( OID_CHLNGR_BOX, ~3 );				// delete all challenger boxes

	delete_multiple_textures(YES_NO_PLAQ_TID,0xFFFFFFFF);
	delete_multiple_textures(NUMBER_3D_TID,0xFFFFFFFF);
	delete_multiple_textures(BKGRND_TID,0xFFFFFFFF);

	if(rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
// 				   This process controls the players cursor and data entry
//-------------------------------------------------------------------------------------------------
void player_cursor(int *args)
{
int 		i;
int			pnum;					// player number
int			cur_pos,cur_img;			// index into alpha table and pin table
char		*cur_tbl;					// pointer to current alpha table
int			entered_cnt;				// entered characters. (name and pin)
int			menu_level;
float		tvbox_cntr_x;				// center X pos. for players tv box
float		tvbox_cntr_y;				// center Y pos. for players tv box
float		sectn_cntr_x,tmp_x;			// center X pos. for players section
float		sectn_cntr_y;				// center Y pos. for players section
int			switches;
int			wait_secs;
int			entered_char;
int			tbl_h;					// height of cursor movement
int			tbl_w;					// width of cursor movement
int			x_moves;					// used to flag the changing of rows...for cursor movement
int			rnum;						// cmos record number
int			answer;
plyr_record_t	*prec;
sprite_info_t	*cursor_obj;
sprite_info_t 	*plaq_obj;
sprite_info_t 	*plaq_obj2;
//sprite_info_t 	*plaq_obj3;
//sprite_info_t 	*plaq_obj4;
//int was_low_res = is_low_res;

	pnum = args[0];
	exit_status &= ~(1<<pnum);														// CLEAR players bit in variable (andn)
	
	prec = plyr_data_ptrs[pnum];
	clear_cmos_record((char *)prec,sizeof(plyr_record_t));    // clear ram record

	// get player's section coordinates
	sectn_cntr_x = station_xys[pnum][four_plr_ver][X_VAL];
	sectn_cntr_y = station_xys[pnum][four_plr_ver][Y_VAL];
	tvbox_cntr_x = japbx_cntr_xys[pnum][four_plr_ver][X_VAL];
	tvbox_cntr_y = japbx_cntr_xys[pnum][four_plr_ver][Y_VAL];
	
	// create the players credits box
	qcreate("credbox",CREDIT_PID,plyr_credit_box,pnum,sectn_cntr_x,sectn_cntr_y,JAPPLE_BOX_TID);
		
	// Wait for player to join
	while (!(p_status & (1<<pnum)))
		sleep(1);

	// Player is active
	if (force_selection == 1)
	{
		exit_status &= ~(1<<pnum);														// CLEAR players bit in variable (andn)
		die(0);
	}
//	if (can_enter_inits == 0)
//		die(0);

	// keep count of active players
	players++;

	exit_status |= 1<<pnum;															// SET players bit in variable

	// Create plyr plaque obj.
	// top half
//	plaq_obj  = beginobj(yn_plaq_imgs[four_plr_ver][pnum][0], plaq_xys[pnum][four_plr_ver], SPRITE_VRES, 14.0F, YES_NO_PLAQ_TID);
	plaq_obj  = beginobj(yn_plaq_imgs[pnum][0], plaq_xys[pnum][four_plr_ver], SPRITE_VRES, 14.0F, YES_NO_PLAQ_TID);
	plaq_obj->id = 1000 + pnum;
	// bottom half
//	plaq_obj2 = beginobj(yn_plaq_imgs[four_plr_ver][pnum][1], plaq_xys[pnum][four_plr_ver], SPRITE_VRES, 14.0F, YES_NO_PLAQ_TID);
	plaq_obj2 = beginobj(yn_plaq_imgs[pnum][1], plaq_xys[pnum][four_plr_ver], SPRITE_VRES, 14.0F, YES_NO_PLAQ_TID);
	plaq_obj2->id = 1000 + pnum;

	sleep(2);

	// !!!FIX
	trans_to_yn_plaq(plaq_obj, plaq_obj2, pnum, sectn_cntr_y);


//	if (was_low_res)
//	{
//		delobj(plaq_obj);
//
//		// Make sure you put this back before sleeping!!!
//		is_low_res = 0;
//
//		// Don't auto scale this 2D img.  We want to use it as is.
//		plaq_obj  = beginobj(&yesno30_lr, sectn_cntr_x, SPRITE_VRES, 14.0F, YES_NO_PLAQ_TID);
//
//		// Back it goes!!!
//		is_low_res = was_low_res;
//	}


	menu_level = YESNO;											// get yes/no first
	cur_pos = 0;
	answer = NO;					// default (time out case)

  // Create cursor object
	tmp_x = sectn_cntr_x;
//	if ((pnum == 1) && (four_plr_ver))
//		tmp_x -= 2.0f;
//	if ((pnum == 2) && (four_plr_ver))
//		tmp_x += 2.0f;

	cursor_obj = beginobj(yes_no_cursor_imgs[(pnum * 2) + cur_pos], tmp_x, sectn_cntr_y, 10.0F, YES_NO_PLAQ_TID);
	if(!cursor_obj)
	{
#ifdef DEBUG
		lockup();
#endif
		return;
	}
	
	// Ask player (YES/NO) if he wants to enter name and pin for record keeping / retrival ?
	wait_secs = tsec * 7;
	while (--wait_secs)
	{
		switches = get_joy_val_down(pnum);
		if ((switches == JOY_UP) && (cur_pos != YES))
		{
			cur_pos = (pnum * 2) + YES;
			answer = YES;
	 		update_cursor_imgs(cursor_obj,menu_level,cur_pos,YES_NO_PLAQ_TID);
			snd_scall_bank(cursor_bnk_str,YESNO_CURSR_SND,VOLUME5,127,PRIORITY_LVL1);
		}
		else if ((switches == JOY_DOWN) && (cur_pos != NO))
	 	{
			cur_pos = (pnum * 2) + NO;
			answer = NO;
	 		update_cursor_imgs(cursor_obj,menu_level,cur_pos,YES_NO_PLAQ_TID);
			snd_scall_bank(cursor_bnk_str,YESNO_CURSR_SND,VOLUME5,127,PRIORITY_LVL1);
	 	}
		if (get_but_val_down(pnum))
	 		break;													// exit
		sleep(1);
	}
	flash_yes_no(cursor_obj,sectn_cntr_x,sectn_cntr_y,pnum,answer);

	// wait until all the images are loaded.
	while (!imgs_loaded)
		sleep(1);

	if (answer == NO)
	{
		exit_status &= ~(1<<pnum);														// CLEAR players bit in variable (andn)

		// show please wait.
		beginobj(&mblank, sectn_cntr_x, sectn_cntr_y, 16.0F, JAPPLE_BOX_TID);
		change_img_tmu(cursor_obj,&mplwait,JAPPLE_BOX_TID);

		// delete plaque
		del1c(1000 + pnum,0xFFFFFFFF);																		// delete players plaque
	 	sleep (1);													// let buttons reset
	  die(0);
	}

	// Init name and pin array
	for (i=0; i < LETTERS_IN_NAME; i++)
		prec->plyr_name[i] = '-';
	for (i=0; i < PIN_NUMBERS; i++)
		prec->plyr_pin_nbr[i] = '-';
	
	entered_cnt = 0;											// no entered data
	x_moves = 0;
	cur_pos = 0;
	cur_img = ((pnum * LTR_COLS * LTR_ROWS) + (MAX_PLYRS * LTR_COLS * LTR_ROWS * four_plr_ver)) + cur_pos;

	menu_level = INITS;
	tbl_w = table_sizes[menu_level][four_plr_ver][W_VAL];
	tbl_h = table_sizes[menu_level][four_plr_ver][H_VAL];
	cur_tbl = alpha_table;


	hide_sprite(cursor_obj);
	// change cursor to be first cursor of letters plaq
	update_cursor_imgs(cursor_obj,menu_level,cur_img,YES_NO_PLAQ_TID);
	
	//
	// transition to INITIALS plaque
	//
//	trans_to_initials_plaq(plaq_obj, plaq_obj2, pnum);

	// delete yes/no plaque
	del1c(1000 + pnum,0xFFFFFFFF);																		// delete players plaque

	// create plaque under...current yes/no plaque
	// top half of initials plaque
	plaq_obj  = beginobj(init_plaq_imgs[pnum][0], plaq_xys[pnum][four_plr_ver], SPRITE_VRES/2, 14.0F, YES_NO_PLAQ_TID);
	plaq_obj->id = 1000 + pnum;
	// bottom half of initials plaque
	plaq_obj2 = beginobj(init_plaq_imgs[pnum][1], plaq_xys[pnum][four_plr_ver], SPRITE_VRES/2, 14.0F, YES_NO_PLAQ_TID);
	plaq_obj2->id = 1000 + pnum;

	unhide_sprite(cursor_obj);

	//
	// Get name and pin from player
	//
	while ((entered_cnt < (LETTERS_IN_NAME+PIN_NUMBERS)) && (!force_selection))
	{
		prec->plyr_name[entered_cnt] = cur_tbl[cur_pos];						// save selection

		if (get_but_val_down(pnum))
		{
			snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME3,127,PRIORITY_LVL2);				// play sound

			flash_cursor(cursor_obj, JAPPLE_BOX_TID);

			entered_char = prec->plyr_name[entered_cnt];
			switch(entered_cnt)
			{
				// name
				case 0:																			// first letter of name
					if (entered_char == CH_DEL)
						break;																	// do nothing
					if (entered_char == CH_END)
						break;																	// do nothing
					entered_cnt++;
					break;
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:																			// last letter in name
					if (entered_char == CH_DEL)
					{
						prec->plyr_name[entered_cnt] = '-';					// erase current letter
						entered_cnt--;
						break;
					}
					if (entered_char == CH_END)
					{
						// fill out name with spaces!
						while (entered_cnt < LETTERS_IN_NAME)
						{
							prec->plyr_name[entered_cnt] = CH_SPC;					// erase current letter
							entered_cnt++;											// start entering PIN NUMBER
						}
						entered_cnt = LETTERS_IN_NAME-1;				// start entering PIN NUMBER
					 }
		
					// dont allow bad words or abbrevs.
					if (censor_players_name(prec->plyr_name,entered_cnt))
						break;														// bad fucking name found
		
					// valid character
					entered_cnt++;
		
					if (entered_cnt == LETTERS_IN_NAME)
					{
						// switch to entering PIN NUMBER
						menu_level = PIN_NBR;
						tbl_w = table_sizes[menu_level][four_plr_ver][W_VAL];
						tbl_h = table_sizes[menu_level][four_plr_ver][H_VAL];
	 					cur_tbl = pin_nbr_tbl;
						cur_pos = 0;
						cur_img = ((pnum * PIN_COLS * PIN_ROWS) + (MAX_PLYRS * PIN_COLS * PIN_ROWS * four_plr_ver)) + cur_pos;
	 					x_moves = 0;
						delete_multiple_strings(plr_nme_str_ids[pnum], 0xffffffff);		// delete players name
	 					hide_sprite(cursor_obj);
						update_cursor_imgs(cursor_obj,menu_level,cur_img,YES_NO_PLAQ_TID);

						// delete 'enter inits' plaque
						del1c(1000 + pnum,0xFFFFFFFF);
																								// delete players plaque
						plaq_obj  = beginobj(pin_plaq_imgs[pnum][0], plaq_xys[pnum][four_plr_ver], SPRITE_VRES/2, 15.0F, YES_NO_PLAQ_TID);
						plaq_obj->id = 1000 + pnum;
						// bottom half of pin number plaque
						plaq_obj2 = beginobj(pin_plaq_imgs[pnum][1], plaq_xys[pnum][four_plr_ver], SPRITE_VRES/2, 15.0F, YES_NO_PLAQ_TID);
						plaq_obj2->id = 1000 + pnum;
	 					unhide_sprite(cursor_obj);
					}
					break;
				// pin number
				case 6:																			// first nbr of pin nbr.
					if (entered_char == CH_DEL)
						break;																	// do nothing
					entered_cnt++;
					break;
				case 7:
				case 8:
				case 9:																			// last nbr of pin nbr.
					if (entered_char == CH_DEL)
					{
						prec->plyr_name[entered_cnt] = '-';					// erase current letter
						entered_cnt--;
						break;
					}
					entered_cnt++;
					if (entered_cnt == LETTERS_IN_NAME+PIN_NUMBERS)
						menu_level = TM_SELECT;														// player ready for team select
					break;
			}
		}
		switches = get_joy_val_down(pnum);
		if ((switches)	&& (entered_cnt < LETTERS_IN_NAME+PIN_NUMBERS))
		{
			switch(switches)
			{
				case JOY_UP:																					// up
					if (cur_pos > (tbl_w-1))									// on top row ?
					{
						cur_pos-=tbl_w;													// no... allow up
						cur_img = ((pnum * tbl_w * tbl_h) + (MAX_PLYRS * tbl_w * tbl_h * four_plr_ver)) + cur_pos;
						update_cursor_imgs(cursor_obj,menu_level,cur_img,YES_NO_PLAQ_TID);
						snd_scall_bank(cursor_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL1);				// play sound
					}
					break;
				case JOY_DOWN:																					// down
					if (cur_pos < ((tbl_h*tbl_w)-tbl_w))			// on last row ?
					{
						cur_pos+=tbl_w;													// no... allow down
						cur_img = ((pnum * tbl_w * tbl_h) + (MAX_PLYRS * tbl_w * tbl_h * four_plr_ver)) + cur_pos;
						update_cursor_imgs(cursor_obj,menu_level,cur_img,YES_NO_PLAQ_TID);
						snd_scall_bank(cursor_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL1);				// play sound
					}
					break;
				case JOY_LEFT:																					// left
				case JOY_DWN_LFT:																					// left
				case JOY_UP_LFT:																					// left
					if (cur_pos > 0)															// on upper left most pos ?
					{
						cur_pos--;																	// no... allow left
						x_moves--;
						if (x_moves < 0)
							x_moves =tbl_w-1;
						cur_img = ((pnum * tbl_w * tbl_h) + (MAX_PLYRS * tbl_w * tbl_h * four_plr_ver)) + cur_pos;
						update_cursor_imgs(cursor_obj,menu_level,cur_img,YES_NO_PLAQ_TID);
						snd_scall_bank(cursor_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL1);				// play sound
					}
					break;
				case JOY_RIGHT:																					// right
				case JOY_DWN_RGT:																					// left
				case JOY_UP_RGT:																					// left
					if (cur_pos < ((tbl_w*tbl_h)-1))			// on lower right most pos ?
					{
						cur_pos++;																	// no... allow right
						x_moves++;
						if (x_moves > (tbl_w-1))
							x_moves = 0;
						cur_img = ((pnum * tbl_w * tbl_h) + (MAX_PLYRS * tbl_w * tbl_h * four_plr_ver)) + cur_pos;
						update_cursor_imgs(cursor_obj,menu_level,cur_img,YES_NO_PLAQ_TID);
						snd_scall_bank(cursor_bnk_str,CURSOR_SND1,VOLUME4,127,PRIORITY_LVL1);				// play sound
					}
					break;
			}
		}
		if (entered_cnt < LETTERS_IN_NAME)
			print_players_name(pnum, tvbox_cntr_x, tvbox_cntr_y, prec->plyr_name, 0);
		else
		{
			// Print pin number
			delete_multiple_strings(plr_pin_str_ids[pnum], 0xffffffff);

			set_string_id(plr_pin_str_ids[pnum]);
			set_text_font(FONTID_BAST18);

			set_text_position(tvbox_cntr_x, tvbox_cntr_y + (is_low_res ? 1.0f : 0.0f), 1.2F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, pin_strs[entered_cnt-LETTERS_IN_NAME]);
		}
		sleep (1);
	}

	// AT this point either the player entered NAME and PIN NUMBER or the screen timed out

	delete_multiple_strings(plr_pin_str_ids[pnum], 0xffffffff);					// delete pin number
	delete_multiple_strings(plr_nme_str_ids[pnum], 0xffffffff);
	delobj(cursor_obj);

	// delete plaque
	del1c(1000 + pnum,0xFFFFFFFF);																		// delete players plaque
	
	// did player enter name and pin ?
	if ((entered_cnt >= LETTERS_IN_NAME+PIN_NUMBERS) && (prec->plyr_name[0] != NULL))
	{
		// check name and pin for SECRET heads
		game_info.team_head[pnum] = check_secret_heads(pnum);
		if (game_info.team_head[pnum] != -1)
			snd_scall_bank(ancr_bnk_str,231,VOLUME4,127,LVL3);
	
		rnum = find_record_in_cmos(prec->plyr_name);
		if (rnum != -1)
		{
			// Clean up bonus flashing message
			delete_multiple_strings(1, 0xffffffff);
			killall(BONUS_PID,0xFFFFFFFF);
//			if(pflasher)
//			{
//				kill(pflasher, 0);
//				pflasher = NULL;
//			}

			// copy cmos record into ram	
			get_generic_record(PLYR_RECS_ARRAY,rnum,plyr_data_ptrs[pnum]);			// get record from cmos into ram

			print_players_name(pnum, tvbox_cntr_x, tvbox_cntr_y, prec->plyr_name, 1);
			
			snd_scall_bank(cursor_bnk_str,RECORD_FOUND_SND,VOLUME3,127,PRIORITY_LVL3);				// play souund

			// try to load the guy's custom plays
			if ((plyr_data_ptrs[pnum]->disk_bank >= 0) &&
				(read_plays_hd(
					plyr_data_ptrs[pnum]->cmos_rec_num,
					plyr_data_ptrs[pnum]->disk_bank,
					custom_plays[pnum],
					&audible_ids[pnum]) > 0))
			{
				game_info.has_custom[pnum] = TRUE;
			}

// !!!FIX... tell player that plays are loaded!!!

			if (game_info.has_custom[pnum] == TRUE)
			{

				plaq_obj = beginobj(&gridbox,sectn_cntr_x,145.0f,100.0F,TM_SELECT_TID);
				plaq_obj->id = 40000 + pnum;
				plaq_obj = beginobj(&gridbox,sectn_cntr_x,145.0f+84.0f,100.0F,TM_SELECT_TID);
				plaq_obj->id = 40000 + pnum;
				plaq_obj->mode = FLIP_TEX_V;
				generate_sprite_verts(plaq_obj);


				set_text_font(FONTID_BAST18);
				set_string_id(STAT_PG_TXT_ID+pnum);
				set_text_position(sectn_cntr_x, 260.0f, 30.0F);
				oprintf("%dj%dcCREATED", (HJUST_CENTER|VJUST_CENTER), WHITE);
				set_text_position(sectn_cntr_x, 235.0f, 30.0F);
				oprintf("%dj%dcPLAYS", (HJUST_CENTER|VJUST_CENTER), WHITE);
				set_text_position(sectn_cntr_x, 210.0f, 30.0F);
				oprintf("%dj%dcLOADED", (HJUST_CENTER|VJUST_CENTER), WHITE);

				set_text_font(FONTID_BAST10);
				set_text_position(sectn_cntr_x, 170.0f, 30.0F);
				oprintf("%dj%dcYOUR PLAYS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
				set_text_position(sectn_cntr_x, 155.0f, 30.0F);
				oprintf("%dj%dcWILL APPEAR", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
				set_text_position(sectn_cntr_x, 140.0f, 30.0F);
				oprintf("%dj%dcON THE LAST", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
				set_text_position(sectn_cntr_x, 125.0f, 30.0F);
				oprintf("%dj%dcPAGE DURING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
				set_text_position(sectn_cntr_x, 110.0f, 30.0F);
				oprintf("%dj%dcPLAY SELECT", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

				wait_for_any_butn_press_active_plyr(15,tsec*5,pnum);
				snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME3,127,PRIORITY_LVL2);				// play sound
				delete_multiple_strings(STAT_PG_TXT_ID+pnum, 0xffffffff);							// delete the stats
				del1c( 40000 + pnum, -1 );
			}

			// now...print the stats
			if (four_plr_ver)
				print_four_player_stats(pnum,sectn_cntr_x,rnum);
			else
				print_player_stats(pnum,sectn_cntr_x,rnum);
		}
		else
		{
			// RECORD NOT FOUND..msg
			float cntr_y = station_xys[pnum][four_plr_ver][Y_VAL]+35;
			game_info.has_custom[pnum] = FALSE;

			print_players_name(pnum, tvbox_cntr_x, tvbox_cntr_y, prec->plyr_name, 0);

			beginobj(&mblank, sectn_cntr_x, cntr_y, 16.0F, JAPPLE_BOX_TID);
			beginobj(&mrecnf, sectn_cntr_x, cntr_y, 15.0F, JAPPLE_BOX_TID);
			sleep(120);					// show for a minium of 2 seconds
		}
	}
	else
	{
		// no, make invalid record
		float cntr_y = station_xys[pnum][four_plr_ver][Y_VAL]+35;
		prec->plyr_name[0] = NULL;
		beginobj(&mblank, sectn_cntr_x, cntr_y, 16.0F, JAPPLE_BOX_TID);
		beginobj(&mrecnf, sectn_cntr_x, cntr_y, 15.0F, JAPPLE_BOX_TID);
		sleep(120);					// show for a minium of 2 seconds

	}

	delete_multiple_strings(plyr_gen_str_ids[pnum], 0xFFFFFFFF);					// delete pin number
	exit_status &= ~(1<<pnum);																				// CLEAR players bit in variable (andn)
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int check_secret_heads(int pnum)
{
	plyr_record_t	*prec;
	int				i;

	prec = plyr_data_ptrs[pnum];
	for (i=0; i < sizeof(secret_heads)/sizeof(world_record_t); i++)
	{
		if (!(compare_record((char *)prec->plyr_name,secret_heads[i].plyr_name,LETTERS_IN_NAME+PIN_NUMBERS)))
			return(secret_heads[i].value);	
	}
	return(-1);																												// match found
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
float print_players_name(int pnum, float x, float y, char *name, int showpts)
{
	char pname[LETTERS_IN_NAME];
	int  h = get_font_height(FONTID_BAST10);
	int  i = LETTERS_IN_NAME - 1;

	if (showpts >= 0)
	{
		strncpy(pname, name, LETTERS_IN_NAME);
		while(i >= 0)
		{
			if(pname[i] != ' ')
			{
				break;
			}
			i--;
		}
		pname[i+1] = 0;
		delete_multiple_strings(plr_nme_str_ids[pnum], 0xffffffff);
		set_string_id(plr_nme_str_ids[pnum]);
	}
	set_text_font(FONTID_BAST10);
	set_text_justification_mode(HJUST_CENTER|VJUST_TOP);
	y += (float)(h/2);

	if (is_low_res) y -= 3.0f;

	if (showpts) y += (is_low_res ? 8.0f:6.0f);

	set_text_position(x, y, 1.2f);
	if (showpts < 0)
		oprintf("%dcFREE", LT_YELLOW);
	else
		oprintf("%dc%s", LT_YELLOW, pname);

	if (showpts)
	{
		y -= (float)(h + (is_low_res ? 6 : 3));

		set_text_position(x, y, 1.19f);
		if (showpts < 0)
			oprintf("%dcGAME", LT_YELLOW);
		else
		if (plyr_data_ptrs[pnum]->trivia_pts)
			oprintf("%dc%d%dc POINT%c",
				LT_RED2,//0xffff5050,//LT_CYAN,
				plyr_data_ptrs[pnum]->trivia_pts,
				WHITE,
				plyr_data_ptrs[pnum]->trivia_pts != 1 ? 'S' : '\0');
		else
			oprintf("%dcNO POINTS",
				WHITE);
	}
	return x;
}

//-------------------------------------------------------------------------------------------------
//								This routine prints the players STATS in the players section
//-------------------------------------------------------------------------------------------------
static plyr_record_t	plrs_rec;

static void copy_record(plyr_record_t *to, plyr_record_t *from)
{
	int	i;

	for(i = 0; i < LETTERS_IN_NAME; i++)
	{
		to->plyr_name[i] = from->plyr_name[i];
	}
	for(i = 0; i < PIN_NUMBERS; i++)
	{
		to->plyr_pin_nbr[i] = from->plyr_pin_nbr[i];
	}
	to->last_used = from->last_used;
	to->games_played = from->games_played;
	to->games_won = from->games_won;
	to->winstreak = from->winstreak;
	to->points_scored = from->points_scored;
	to->points_allowed = from->points_allowed;
	to->teams_defeated = from->teams_defeated;
	to->tackles = from->tackles;
	to->sacks = from->sacks;
	to->passing_yds = from->passing_yds;
	to->rushing_yds = from->rushing_yds;
	to->world_records = from->world_records;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void print_four_player_stats(int p_num, int cntr_x, int rec_num)
{
plyr_record_t	*_plr_rec;
plyr_record_t	*plr_rec = &plrs_rec;
float			msg_spc;
sprite_info_t 	*stat_obj;
sprite_info_t 	*dshdw_obj;
int				i,j;
int				ver;


	// make sure records are in ram!!
	if (!rec_sort_ram)
		return;

	ver = four_plr_ver;

	// print stats
	msg_spc = 15.0F;

	_plr_rec = plyr_data_ptrs[p_num];

	// point to the players record in ram
	_plr_rec = &rec_sort_ram[find_record_in_ram(_plr_rec->plyr_name)-1];
	copy_record(&plrs_rec, _plr_rec);

	// create STAT header image
	stat_obj = beginobj(&statpl_r, cntr_x, STAT_TITLE_Y, 20.0F, YES_NO_PLAQ_TID);
	stat_obj->id = OID_STAT_PG+p_num;

	// create Stats backdrop
	dshdw_obj = beginobj(&statfade, stats_bckdrp_xs[ver][p_num], STATS_BCKDRP_Y, 70.0F, YES_NO_PLAQ_TID);
	dshdw_obj->id = OID_STAT_PG+p_num;
	dshdw_obj->h_scale = 2.6f;
	dshdw_obj->w_scale = 1.8f;
	generate_sprite_verts(dshdw_obj);

	// create STATS headers (first page)
	for (i=0; i<STATS_PER_PAGE; i++)
	{
		stat_obj = beginobj(&sstatplr, stat_plqs_xys[ver][i][p_num][X_VAL], stat_plqs_xys[ver][i][p_num][Y_VAL], 50.0F, YES_NO_PLAQ_TID);
		stat_obj->id = OID_STAT2_PG+p_num;
	}
	
	//
	// Print Stats:
	//
	set_text_font(FONTID_BAST10);
	set_string_id(STAT_PG_TXT_ID+p_num);

	//
	// *** PAGE 1 ***
	//

	// print OVERALL rank
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_wins);
	plr_ranks[p_num][OVR_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OVR_RNK], NUM_PLYR_RECORDS);

	// print WIN-LOSS record
	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc RECORD", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc %d W/ %d L", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->games_won, (plr_rec->games_played-plr_rec->games_won));

	// print OFFFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_scored);
	plr_ranks[p_num][OFF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc OFF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OFF_RNK], NUM_PLYR_RECORDS);

	// print DEFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_allowed);
	plr_ranks[p_num][DEF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc DEF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][DEF_RNK], NUM_PLYR_RECORDS);

	// print WINSTREAK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_streaks);
	plr_ranks[p_num][STK_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][4], 30.0F);
	oprintf("%dj%dc STREAK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][4], 30.0F);
	if (plr_rec->winstreak <= 1)
		oprintf("%dj%dc %d WIN", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->winstreak);
	else
		oprintf("%dj%dc %d WINS", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->winstreak);
		

	// print TEAMS DEFEATED
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][5], 30.0F);
	oprintf("%dj%dc DEFEATED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][5], 30.0F);
	oprintf("%dj%dc %d / 31 TEAMS", (HJUST_CENTER|VJUST_CENTER), WHITE, calc_tms_def(plr_rec->teams_defeated));

	// Show player rank awards
	// !!!FIX
//	qcreate("plrawrds",PLAQ_AWARD_PID+p_num,plyr_plaques_awards,p_num,0,0,0);

	// wait for player to change page
	wait_for_any_butn_press_active_plyr(15,tsec*10,p_num);
	snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME3,127,PRIORITY_LVL2);				// play sound

	delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xffffffff);							// delete the stats
//	sleep(1);
	
	if (plr_rec->games_won > 0)
	{
		//
		// *** PAGE 2 ***
		//
		set_text_font(FONTID_BAST10);
		set_string_id(STAT_PG_TXT_ID+p_num);

		// print AVG. PTS. SCORED per game
		set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
		oprintf("%dj%dc SCORED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][0], 30.0F);
		oprintf("%dj%dc %hd PTS/GM", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->points_scored/plr_rec->games_played);

		// print AVG. PTS. ALLOWED per game
		set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1], 30.0F);
		oprintf("%dj%dc ALLOWED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][1], 30.0F);
		oprintf("%dj%dc %u PTS/GM", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)(plr_rec->points_allowed/plr_rec->games_played));

		// print RUSHING YARDS
		set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][2], 30.0F);
		oprintf("%dj%dc RUSHING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][2], 30.0F);
		oprintf("%dj%dc %d YDS.", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->rushing_yds/(int)plr_rec->games_played);

		// print PASSING YARDS
		set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][3], 30.0F);
		oprintf("%dj%dc PASSING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][3], 30.0F);
		oprintf("%dj%dc %d YDS.", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->passing_yds/plr_rec->games_played);

		// print SACKS
		set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][4], 30.0F);
		oprintf("%dj%dc SACKS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][4], 30.0F);
		oprintf("%dj%dc %hd / GM", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->sacks/plr_rec->games_played);

		// print TACKLES per game
		set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][5], 30.0F);
		oprintf("%dj%dc TACKLES", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][5], 30.0F);
		oprintf("%dj%dc %d / GM", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->tackles/plr_rec->games_played);

		// Show player rank awards
		// !!!FIX
//		qcreate("plrawrds",PLAQ_AWARD_PID+p_num,plyr_plaques_awards,p_num,0,0,0);

		// wait for player to change page
		wait_for_any_butn_press_active_plyr(15,tsec*10,p_num);
		snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME3,127,PRIORITY_LVL2);				// play sound

		del1c(OID_STAT2_PG+p_num, 0xffffffff);
		killall(PLAQ_AWARD_PID+p_num, 0xffffffff);					// kill the plyr_plaques_awards process
		del1c(OID_STAT3_PG+p_num, 0xffffffff);
		delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xffffffff);							// delete the stats
//		sleep(1);

		//
		// *** PAGE 3 *** (teams defeated)
		//
		plr_rec = plyr_data_ptrs[p_num];
	
		set_text_z_inc(0.0f);
		set_text_font(FONTID_BAST10);
		set_string_id(STAT_PG_TXT_ID+p_num);
	
		// print 'DEFEATED' header
		stat_obj = beginobj(&sstatplr, stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL], 52.0F, YES_NO_PLAQ_TID);
		stat_obj->id = OID_STAT2_PG+p_num;
		if (calc_tms_def(plr_rec->teams_defeated) < 14)
		{
			set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL] - (is_low_res ? 2:0), 30.0F);
//			set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
			oprintf("%dj%dcDEFEATED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
			// print TEAMS DEFEATED
			for (i=0,j=0; i < NUM_TEAMS; i++)
			{
				if (plr_rec->teams_defeated & (1<<i))			// bit set ?
				{
					j++;
					set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plq_msg_xys[ver][p_num][Y_VAL]-(14.0F * j)-5, 30.0F);
					oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[i].name);
				}
			}
		}
		else
		{
			set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
			oprintf("%dj%dcREMAINING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
			// print REMAINING teams
			set_text_font(FONTID_BAST8T);
			for (i=0,j=0; i < NUM_TEAMS; i++)
			{
				if (!(plr_rec->teams_defeated & (1<<i)))			// bit clear ?
				{
					j++;
					set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plq_msg_xys[ver][p_num][Y_VAL]-(14.0F * j)-5, 30.0F);
					oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[i].name);
				}
			}
		}	
		// wait for player to change page
		wait_for_any_butn_press_active_plyr(15,tsec*10,p_num);	//50
		snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME3,127,PRIORITY_LVL2);				// play sound
	
	}
	delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xFFFFFFFF);							// delete the stats
	del1c(OID_STAT_PG+p_num, 0xffffffff);
	del1c(OID_STAT2_PG+p_num, 0xffffffff);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void print_player_stats(int p_num, int cntr_x, int rec_num)
{
plyr_record_t	*_plr_rec;
plyr_record_t	*plr_rec = &plrs_rec;
float			msg_spc;
sprite_info_t 	*stat_obj;
sprite_info_t 	*dshdw_obj;
int				i,j,tier;
int				unbeaten[4],nu=0;
int				ver;

	// make sure records are in ram!!
	if (!rec_sort_ram)
		return;

	ver = four_plr_ver;

	// print stats
	msg_spc = 15.0F;

	_plr_rec = plyr_data_ptrs[p_num];

	// point to the players record in ram
	_plr_rec = &rec_sort_ram[find_record_in_ram(_plr_rec->plyr_name)-1];
	copy_record(&plrs_rec, _plr_rec);

	// create STAT header image
	stat_obj = beginobj(&statpl_r, cntr_x, STAT_TITLE_Y, 20.0F, YES_NO_PLAQ_TID);
	stat_obj->id = OID_STAT_PG+p_num;

	// create Stats backdrop
	dshdw_obj = beginobj(&statfade, stats_bckdrp_xs[ver][p_num], STATS_BCKDRP_Y, 70.0F, YES_NO_PLAQ_TID);
	dshdw_obj->id = OID_STAT_PG+p_num;
	dshdw_obj->h_scale = 2.6f;
	dshdw_obj->w_scale = 3.8f;
	generate_sprite_verts(dshdw_obj);

	// create STATS headers (left column)
	for (i=0; i<NUM_STATS/2; i++)
	{
		stat_obj = beginobj(&sstatplr, stat_plqs_xys[ver][i][p_num][X_VAL], stat_plqs_xys[ver][i][p_num][Y_VAL], 50.0F, YES_NO_PLAQ_TID);
		stat_obj->id = OID_STAT2_PG+p_num;
	}

	// create STATS headers (right column)
	for (i=NUM_STATS/2; i<NUM_STATS; i++)
	{
		stat_obj = beginobj(&sstatplr, stat_plqs_xys[ver][i][p_num][X_VAL], stat_plqs_xys[ver][i][p_num][Y_VAL], 52.0F, YES_NO_PLAQ_TID);
		stat_obj->id = OID_STAT2_PG+p_num;
	}


	//
	// Print Stats:
	//
	set_text_font(FONTID_BAST10);
	set_string_id(STAT_PG_TXT_ID+p_num);

	// print OVERALL rank
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_wins);
	plr_ranks[p_num][OVR_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OVR_RNK], NUM_PLYR_RECORDS);

	// print OFFFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_scored);
	plr_ranks[p_num][OFF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc OFF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][1][p_num][X_VAL], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OFF_RNK], NUM_PLYR_RECORDS);

	// print AVG. PTS. SCORED per game
	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc SCORED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][2][p_num][X_VAL], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc %hd PTS/GM", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->points_scored/GREATER(1,plr_rec->games_played));

	// print RUSHING YARDS
	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc RUSHING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][3][p_num][X_VAL], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc %d YDS.", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->rushing_yds/(int)(GREATER(1,plr_rec->games_played)));

	// print SACKS
	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][4], 30.0F);
	oprintf("%dj%dc SACKS", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][4][p_num][X_VAL], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][4], 30.0F);
	oprintf("%dj%dc %hd / GM", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->sacks/GREATER(1,plr_rec->games_played));

	// print WINSTREAK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_streaks);
	plr_ranks[p_num][STK_RNK] = find_record_in_ram(plr_rec->plyr_name);

	// point to the players record in ram
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][5], 30.0F);
	oprintf("%dj%dc STREAK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][5][p_num][X_VAL], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][5], 30.0F);
	if (plr_rec->winstreak <= 1)
		oprintf("%dj%dc %d WIN", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->winstreak);
	else
		oprintf("%dj%dc %d WINS", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->winstreak);

	// print WIN-LOSS record
	set_text_position(stat_plqs_xys[ver][6][p_num][X_VAL], stat_plqs_xys[ver][6][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][6], 30.0F);
	oprintf("%dj%dc RECORD", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][6][p_num][X_VAL], stat_plqs_xys[ver][6][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][6], 30.0F);
	oprintf("%dj%dc %d W/ %d L", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->games_won, (plr_rec->games_played-plr_rec->games_won));

	// print AVG. PTS. ALLOWED per game
	// point to the players record in ram
	set_text_position(stat_plqs_xys[ver][8][p_num][X_VAL], stat_plqs_xys[ver][8][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][8], 30.0F);
	oprintf("%dj%dc ALLOWED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][8][p_num][X_VAL], stat_plqs_xys[ver][8][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][8], 30.0F);
	oprintf("%dj%dc %u PTS/GM", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)(plr_rec->points_allowed/GREATER(1,plr_rec->games_played)));

	// print average PASSING YARDS
	set_text_position(stat_plqs_xys[ver][9][p_num][X_VAL], stat_plqs_xys[ver][9][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][9], 30.0F);
	oprintf("%dj%dc PASSING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][9][p_num][X_VAL], stat_plqs_xys[ver][9][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][9], 30.0F);
	oprintf("%dj%dc %d YDS.", (HJUST_CENTER|VJUST_CENTER), WHITE, (int)plr_rec->passing_yds/GREATER(1,plr_rec->games_played));

	// print TACKLES per game
	set_text_position(stat_plqs_xys[ver][10][p_num][X_VAL], stat_plqs_xys[ver][10][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][10], 30.0F);
	oprintf("%dj%dc TACKLES", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][10][p_num][X_VAL], stat_plqs_xys[ver][10][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][10], 30.0F);
	oprintf("%dj%dc %d / GM", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->tackles/GREATER(1,plr_rec->games_played));

	// print TEAMS DEFEATED
	set_text_position(stat_plqs_xys[ver][11][p_num][X_VAL], stat_plqs_xys[ver][11][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][11], 30.0F);
	oprintf("%dj%dc DEFEATED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][11][p_num][X_VAL], stat_plqs_xys[ver][11][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][11], 30.0F);
	oprintf("%dj%dc %d / 31 TEAMS", (HJUST_CENTER|VJUST_CENTER), WHITE, calc_tms_def(plr_rec->teams_defeated));

	// print DEFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_allowed);
	plr_ranks[p_num][DEF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(stat_plqs_xys[ver][7][p_num][X_VAL], stat_plqs_xys[ver][7][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][7], 30.0F);
	oprintf("%dj%dc DEF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(stat_plqs_xys[ver][7][p_num][X_VAL], stat_plqs_xys[ver][7][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][7], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][DEF_RNK], NUM_PLYR_RECORDS);

	// Show player rank awards
//	qcreate("plrawrds",PLAQ_AWARD_PID+p_num,plyr_plaques_awards,p_num,0,0,0);

	// wait for player to change page
	wait_for_any_butn_press_active_plyr(15,tsec*10,p_num);
	snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME3,127,PRIORITY_LVL2);				// play sound

	del1c(OID_STAT2_PG+p_num, 0xffffffff);
	killall(PLAQ_AWARD_PID+p_num, 0xffffffff);					// kill the plyr_plaques_awards process
	del1c(OID_STAT3_PG+p_num, 0xffffffff);
	delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xffffffff);							// delete the stats
	sleep(1);
	
	// *** PAGE 2 *** (teams defeated)
	
	plr_rec = plyr_data_ptrs[p_num];

	if (plr_rec->games_played > 0)
	{	
		set_text_z_inc(0.0f);
		set_text_font(FONTID_BAST10);
		set_string_id(STAT_PG_TXT_ID+p_num);
	
		// print 'DEFEATED' header
		stat_obj = beginobj(&sstatplr, stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL], 50.0F, YES_NO_PLAQ_TID);
		stat_obj->id = OID_STAT2_PG+p_num;
		if (calc_tms_def(plr_rec->teams_defeated) < 14)
		{
			set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
			oprintf("%dj%dcDEFEATED", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
			// print TEAMS DEFEATED
			for (i=0,j=0; i < NUM_TEAMS; i++)
			{
				if (plr_rec->teams_defeated & (1<<i))			// bit set ?
				{
					j++;
					set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plq_msg_xys[ver][p_num][Y_VAL]-(14.0F * j)-5, 30.0F);
					oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[i].name);
				}
			}
		}
		else
		{
			set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
			oprintf("%dj%dcREMAINING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
			// print REMAINING teams
			set_text_font(FONTID_BAST8T);
			for (i=0,j=0; i < NUM_TEAMS; i++)
			{
				if (!(plr_rec->teams_defeated & (1<<i)))			// bit clear ?
				{
					j++;
					set_text_position(stat_plqs_xys[ver][0][p_num][X_VAL], stat_plq_msg_xys[ver][p_num][Y_VAL]-(14.0F * j)-5, 30.0F);
					oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[i].name);
				}
			}
		}	
	
		set_text_font(FONTID_BAST10);
	
		// print 'NEXT TIER' header and teams in tier
		stat_obj = beginobj(&sstatplr, stat_plqs_xys[ver][6][p_num][X_VAL], stat_plqs_xys[ver][6][p_num][Y_VAL], 52.0F, YES_NO_PLAQ_TID);
		stat_obj->id = OID_STAT2_PG+p_num;
		set_text_position(stat_plqs_xys[ver][6][p_num][X_VAL], stat_plqs_xys[ver][6][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][6], 30.0F);
		oprintf("%dj%dcNEXT TIER", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	
		for (tier=0; (tier < 9) && !nu; tier++)
		{
			// has the guy defeated every team on this tier
			nu = 0;
			for (i=0; team_ladder[tier][i] != -1; i++)
			{
				if (!(plr_rec->teams_defeated & (1<<team_ladder[tier][i])))
					unbeaten[nu++] = team_ladder[tier][i];
			}
		}

		for (i=0,j=0; i < nu; i++)
		{
			j++;
			set_text_position(stat_plqs_xys[ver][6][p_num][X_VAL], stat_plq_msg_xys[ver][p_num][Y_VAL]-(14.0F * j)-5, 30.0F);
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[unbeaten[i]].name);
		}
	
		// wait for player to change page
		wait_for_any_butn_press_active_plyr(15,tsec*10,p_num);	//50
		snd_scall_bank(cursor_bnk_str,SELECT_SND1,VOLUME3,127,PRIORITY_LVL2);				// play sound
	}
	
	delete_multiple_strings(STAT_PG_TXT_ID+p_num, 0xFFFFFFFF);							// delete the stats
	del1c(OID_STAT_PG+p_num, 0xffffffff);
	del1c(OID_STAT2_PG+p_num, 0xffffffff);
//	killall(PLAQ_AWARD_PID+p_num, 0xffffffff);					// kill the plyr_plaques_awards process
//	del1c(OID_STAT3_PG+p_num, 0xffffffff);
}

//-------------------------------------------------------------------------------------------------
//		This process handles the showing of the players STANDING for certain records
//
// INPUT: player number
//-------------------------------------------------------------------------------------------------
#if 0
void plyr_plaques_awards(int *args)
{
plyr_record_t		*prec;
int		 			pnum,i,awarded;
float	 			x,y;
sprite_info_t 		*plq_awrd_obj;;

	pnum = args[0];
	prec = plyr_data_ptrs[pnum];

	// first check to see if player is '#1' in any cateogory
	awarded = 0;
	for (i=0; i < NUM_STATS; i++)
	{
		if(plr_ranks[pnum][i] == 1)
		{
			awarded = 1;
			x = stat_plqs_awrd_xys[i][pnum][X_VAL];
			y = stat_plqs_awrd_xys[i][pnum][Y_VAL];
			plq_awrd_obj = beginobj(&st_num1, x, y, 15.0F, YES_NO_PLAQ_TID);
			plq_awrd_obj->id = OID_STAT3_PG+pnum;
			qcreate("rankflsh",0,flash_rank_plq, x, y, OID_STAT3_PG+pnum,0);
		}
	}
	if (awarded)
		sleep(50);

	// check to see if player is 'top 5' in any cateogory
	awarded = 0;
	for (i=0; i < NUM_STATS; i++)
	{
		if((plr_ranks[pnum][i] > 1) && (plr_ranks[pnum][i] <= 5))
		{
			awarded = 1;
			x = stat_plqs_awrd_xys[i][pnum][X_VAL];
			y = stat_plqs_awrd_xys[i][pnum][Y_VAL];
			plq_awrd_obj = beginobj(&st_top5, x, y, 15.0F, YES_NO_PLAQ_TID);
			plq_awrd_obj->id = OID_STAT3_PG+pnum; //STAT_PG_OBJ_ID+pnum;
			qcreate("rankflsh",0,flash_rank_plq, x, y, OID_STAT3_PG+pnum,0);
		}
	}
	if (awarded)
		sleep(50);

	// check to see if player is 'top 10' in any cateogory
	for (i=0; i < NUM_STATS; i++)
	{
		if((plr_ranks[pnum][i] > 5) && (plr_ranks[pnum][i] <= 10))
		{
			x = stat_plqs_awrd_xys[i][pnum][X_VAL];
			y = stat_plqs_awrd_xys[i][pnum][Y_VAL];
			plq_awrd_obj = beginobj(&st_top10, x, y, 15.0F, YES_NO_PLAQ_TID);
			plq_awrd_obj->id = OID_STAT3_PG+pnum; //STAT_PG_OBJ_ID+pnum;
			qcreate("rankflsh",0,flash_rank_plq, x, y, OID_STAT3_PG+pnum,0);
		}
	}
}
#endif

//-------------------------------------------------------------------------------------------------
//				This process flashes the stat "area" of any stat inwhich the player in is the top 10
//
//  INPUT: X, Y, Z
//-------------------------------------------------------------------------------------------------
void stat_flash(int *args)
{
	sprite_info_t 	*flsh_obj;
	int	i;


	snd_scall_bank(cursor_bnk_str,TM_SELECTED_SND,VOLUME5,127,PRIORITY_LVL3);

//	flsh_obj = beginobj(stat_flsh_imgs[0], args[0], args[1], 10.0F+args[3], JAPPLE_BOX_TID);
	flsh_obj = beginobj(stat_flsh_imgs[0], args[0], args[1], 1.0f+args[3], JAPPLE_BOX_TID);
	flsh_obj->w_scale = 2.0f;
	flsh_obj->h_scale = 2.4f;
	generate_sprite_verts(flsh_obj);

	for (i=0; i < (sizeof(stat_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flsh_obj,stat_flsh_imgs[i],JAPPLE_BOX_TID);
		sleep(3);
	}
	delobj(flsh_obj);
	flash_status &= ~(1<<(args[3]));														// CLEAR teams (1 or 2) bit in variable
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void option_select_flash(sprite_info_t *obj, int tid)
{
	sprite_info_t 	*flash_obj;
	int				i;

	flash_obj = beginobj(optscr_flsh_imgs[0],
						 obj->x-obj->ii->ax + (obj->ii->w / 2.0f),
						 obj->y+obj->ii->ay + (obj->ii->h / 2.0f),
						 2.0F,
						 tid);

	flash_obj->w_scale = 2.0f;
	flash_obj->h_scale = 2.0f;
	flash_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	generate_sprite_verts(flash_obj);

	for (i=0; i < (sizeof(optscr_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flash_obj,optscr_flsh_imgs[i],tid);
		sleep(2);
	}
	delobj(flash_obj);
}

//-------------------------------------------------------------------------------------------------
//				This process flashes the stat "area" of any stat inwhich the player in is the top 10
//
//  INPUT: X, Y, Z, texture id
//-------------------------------------------------------------------------------------------------
void flash_cursor(sprite_info_t *obj, int tid)
{
	sprite_info_t 	*burst_obj;
	int		i;

	burst_obj = beginobj(burst_flsh_imgs[0],
						 obj->x-obj->ii->ax + (obj->ii->w / 2.0f),
						 obj->y+obj->ii->ay + (obj->ii->h / 2.0f),
						 5.0F,
						 JAPPLE_BOX_TID);

	burst_obj->w_scale = 2.5f;
	burst_obj->h_scale = 3.5f;
	burst_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	generate_sprite_verts(burst_obj);

	for (i=0; i < (sizeof(burst_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(burst_obj,burst_flsh_imgs[i],tid);
		sleep(1);
	}
	delobj(burst_obj);
}

//-------------------------------------------------------------------------------------------------
//	Flash sprite white - change it back to normal after 2 ticks
//
//	INPUT:  Sprite pntr, # of times to flash
//	OUTPUT: Nothing
//-------------------------------------------------------------------------------------------------
void flash_obj_white(sprite_info_t *obj, int cnt, int delay)
{
int i,cnst_clr,com_func;

	#ifdef DEBUG
	void	*parent;
	int		id;

	parent = obj->parent;
	id = obj->id;
	#endif

	#ifdef DEBUG
	assert( sprite_exists(obj));
	#else
	if (!sprite_exists(obj))
		return;
	#endif

	// save obj. state
	cnst_clr = obj->state.constant_color;
	com_func = obj->state.color_combiner_function;

//	obj->state.constant_color = 0xffFFFFFF;

	for (i=0; i < cnt; i++)
	{
		// turn obj. white
//#if ! defined(GLIDE3)
		obj->state.color_combiner_function = GR_COLORCOMBINE_ONE;
//#else
//		obj->state.constant_color = 0xffffffff;
//		obj->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
//#endif
		obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;

		sleep(delay);
		#ifdef DEBUG
		assert( sprite_exists(obj));
		#else
		if (!sprite_exists(obj))
			return;
		#endif

		// obj. back to color
		obj->state.color_combiner_function = com_func;
		obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
		sleep(delay);
		#ifdef DEBUG
		assert( sprite_exists(obj));
		#else
		if (!sprite_exists(obj))
			return;
		#endif
	}

	// restore obj. back to original state
	obj->state.constant_color = cnst_clr;
	obj->state.color_combiner_function = com_func;
	obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	#ifdef DEBUG
	obj->parent = parent;
	obj->id = id;							
	#endif
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void trans_to_yn_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum, float y_dest)
{
	signed int delta_y;
	int time, i;

	time = 15;			// in ticks
	delta_y = y_dest - p_plaq_obj->y;
	p_plaq_obj->y_vel = (float)delta_y/time;
	p_plaq_obj2->y_vel = (float)delta_y/time;
//	p_shdw_obj->y_vel = (float)delta_y/time;
//	p_shdw_obj2->y_vel = (float)delta_y/time;

	p_plaq_obj->y_end = y_dest;
	p_plaq_obj2->y_end = y_dest;
//	p_shdw_obj->y_end = y_dest;
//	p_shdw_obj2->y_end = y_dest;

	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way since we can't know how many ticks have lapsed since transition began
	while (plates_on != PLATES_OFF && --i) sleep(1);
	// Make sure plates are gone
	plates_on = PLATES_OFF;

	set_veladd_mode(p_plaq_obj,1);
	set_veladd_mode(p_plaq_obj2,1);
//	set_veladd_mode(p_shdw_obj,1);
//	set_veladd_mode(p_shdw_obj2,1);

	snd_scall_bank(cursor_bnk_str,PLAQ_SLIDE_SND,VOLUME4,127,PRIORITY_LVL1);

	do
	{
		sleep(1);
	} while (--time);

	set_veladd_mode(p_plaq_obj,0);
	set_veladd_mode(p_plaq_obj2,0);
//	set_veladd_mode(p_shdw_obj,0);
//	set_veladd_mode(p_shdw_obj2,0);

	snd_scall_bank(cursor_bnk_str,PLAQ_STOP_SND,VOLUME2,127,PRIORITY_LVL2);
}

//-------------------------------------------------------------------------------------------------
//								This routine changes the image and/or texture on the players plaque
//
//	NOTE: These PLAQUE TEXTURES are ASSUMED to be LOADED into SYSTEM RAM
//
//   INPUT: top plaque obj, bottom plaque obj and player number
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
#if 0
void trans_to_initials_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum)
{
	short	nxt_img,trans_imgs;

	// starting sound effect
	snd_scall_bank(cursor_bnk_str,PLAQ_ROTATE_SND,VOLUME2,127,PRIORITY_LVL1);

	nxt_img = -1;
	trans_imgs = (sizeof(init_plaq_imgs)/sizeof(image_info_t *))-1;						// get nbr. of frames

	// since these transition images are 8bit (no alpha channel) must turn on chroma key
	p_plaq_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	p_plaq_obj2->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;

	while (nxt_img < trans_imgs)
	{
		nxt_img++;

		// top half of plaque
		change_img_tmu(p_plaq_obj,init_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		nxt_img++;

		// bottom half of plaque
		change_img_tmu(p_plaq_obj2,init_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		nxt_img++;

		// right edge drop shadow for plaque
		change_img_tmu(p_shdw_obj,init_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		nxt_img++;

		// bottom edge drop shadow for plaque
		change_img_tmu(p_shdw_obj2,init_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		sleep(1);
	}
	p_plaq_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	p_plaq_obj2->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	// ending sound effect
	snd_scall_bank(cursor_bnk_str,PLAQ_STOP_SND,VOLUME2,127,PRIORITY_LVL2);
}
#endif

//-------------------------------------------------------------------------------------------------
//								This routine changes the image and/or texture on the players plaque
//
//	NOTE: These PLAQUE TEXTURES are ASSUMED to be LOADED into SYSTEM RAM
//
//   INPUT: top plaque obj, bottom plaque obj and player number
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
#if 0

void trans_to_pin_nbr_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum)
{
	short	nxt_img,trans_imgs;

	// starting sound effect
	snd_scall_bank(cursor_bnk_str,PLAQ_ROTATE_SND,VOLUME2,127,PRIORITY_LVL1);

	nxt_img = -1;
	trans_imgs = (sizeof(pin_plaq_imgs)/sizeof(image_info_t *))-1;						// get nbr. of frames

	// since these transition images are 8bit (no alpha channel) must turn on chroma key
	p_plaq_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	p_plaq_obj2->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;

	while (nxt_img < trans_imgs)
	{
		nxt_img++;

		// top half of plaque
		change_img_tmu(p_plaq_obj,pin_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		nxt_img++;

		// bottom half of plaque
		change_img_tmu(p_plaq_obj2,pin_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		nxt_img++;

		// right edge drop shadow for plaque
		change_img_tmu(p_shdw_obj,pin_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		nxt_img++;

		// bottom edge drop shadow for plaque
		change_img_tmu(p_shdw_obj2,pin_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);

		sleep(1);
	}
	p_plaq_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	p_plaq_obj2->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;


	// ending sound effect
	snd_scall_bank(cursor_bnk_str,PLAQ_STOP_SND,VOLUME2,127,PRIORITY_LVL2);
}
#endif

//-------------------------------------------------------------------------------------------------
//								This routine changes the image and/or texture on the players plaque
//
//	NOTE: These PLAQUE TEXTURES are ASSUMED to be LOADED into the TMU
//
//   INPUT: top plaque obj, bottom plaque obj and player number
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
//void	trans_to_stats_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum)
//	{
//	short	nxt_img,trans_imgs;
//	int		tn_hndle1,tn_hndle2;

//	tn_hndle1 = p_plaq_obj->tn->texture_handle;
//	tn_hndle2 = p_plaq_obj2->tn->texture_handle;
//
//	nxt_img = -1;
//	trans_imgs = (sizeof(stat_plaq_imgs)/sizeof(image_info_t *))-1;						// get nbr. of frames
//
//	while (nxt_img < trans_imgs)
//		{
//		nxt_img++;
//
//		// top half of plaque
//		change_img_tmu(p_plaq_obj,stat_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);
//
//		nxt_img++;
//
//		// bottom half of plaque
//		change_img_tmu(p_plaq_obj2,stat_plaq_imgs[nxt_img],YES_NO_PLAQ_TID);
//
//		sleep(1);
//		}
//	}


//-------------------------------------------------------------------------------------------------
//											This routine changes the image and/or texture on the players cursor
//
//	NOTE: THE CURSOR TEXTURES are ASSUMED TO BE LOADED INTO THE TMU
//
//   INPUT: cursor obj, menu level and cursor position
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void update_cursor_imgs(sprite_info_t *cur_obj, int level, int c_pos, int tid)
{
	image_info_t	**tbl_ptr;

	// get ptr. to current cursor image table
	tbl_ptr = cursor_tbl_ptrs[level];

	change_img_tmu(cur_obj, tbl_ptr[c_pos], tid);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void flash_yes_no(sprite_info_t *cur_obj, float flsh_x, float flsh_y, int pnum, short answer)
{
	int						i;

	if (answer == YES)
	{
		snd_scall_bank(cursor_bnk_str,SEL_YESNO_SND,VOLUME4,127,PRIORITY_LVL2);
		for (i=0; i < 11; i++)
		{
			change_img_tmu(cur_obj, yes_flsh_imgs[four_plr_ver][pnum][i], YES_NO_PLAQ_TID);
			sleep(3);
		}
	}
	else
	{
		snd_scall_bank(cursor_bnk_str,SEL_YESNO_SND,VOLUME4,127,PRIORITY_LVL2);
		for (i=0; i < 11; i++)
		{
			change_img_tmu(cur_obj, no_flsh_imgs[four_plr_ver][pnum][i], YES_NO_PLAQ_TID);
			sleep(3);
		}
	}

}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void team_selection(void)
{
	struct texture_node * tn;
	sprite_info_t	*b_logo;
	sprite_info_t	*bkgrnd_obj;
	int				i;
	int				show_box;
	int				exit_box;
	int				old_pstatus;
	sprite_info_t	*idiot_bx_obj;
	int				was_low_res = is_low_res;
	char			leghel[12], jersf[12], slvmsk[12], jnum[12];
	char			*files[5] = {jersf,leghel,slvmsk,jnum,NULL};
	int				ratios[4];
	int config_game;

	config_game = 0;

	// Clean up bonus flashing message
	delete_multiple_strings(1, 0xffffffff);
	killall(BONUS_PID,0xFFFFFFFF);

	qcreate("cred_msg",CREDIT_PID,credit_msg,screen,JAPPLE_BOX_TID,0,0);

	exit_status = 0;
	force_selection = 0;
	screen = TM_SELECT_SCRN;
	uniforms_loaded = 0;

	load_title_flash(FALSE, TRUE);

	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info((void **)&cur_proc, suspend_self, resume_process);
#else
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

	if (load_textures_into_tmu(team_select_textures_tbl))
	{
		fprintf(stderr, "Couldn't load all team-select textures into tmu\r\n");
#ifdef DEBUG
		lockup();
#else
		return;
#endif
	}

	// Disable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

//		for (i=0; i < NUM_TEAMS; i++)
//		load_tmsel_uniform(i);
	// load first uniform then rest are loaded via when hilight new team

	// load all player misc textures
	sprintf( jersf, "ARMHND.WMS");
	sprintf( leghel,"ARMHND2.WMS");
	sprintf( slvmsk,"HEDSHU.WMS");
	sprintf( jnum, 	"HEDSHU2.WMS");
	load_textures( files, ratios, &(misc_decals[0]), 4, TXID_PLAYER,	// ball and black player
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
	// set BALL textures for both players
	team_decals[0][6] = misc_decals[2];
	team_decals[1][6] = misc_decals[2];
	team_decals[2][6] = misc_decals[2];
	team_decals[3][6] = misc_decals[2];

	// set hedshu textures for both players
	team_decals[0][0] = misc_decals[0];
	team_decals[1][0] = misc_decals[1];
	team_decals[2][0] = misc_decals[0];
	team_decals[3][0] = misc_decals[1];

	team_decals[0][1] = misc_decals[2];
	team_decals[1][1] = misc_decals[3];
	team_decals[2][1] = misc_decals[2];
	team_decals[3][1] = misc_decals[3];

  	load_tmsel_uniform(0);

	// Control will not reach this point until texture is loaded into TMU!
	// Process will wake up and then the beginobj can happen

	// draw background
	bkgrnd_obj = beginobj(&teamsel,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
	bkgrnd_obj->id = 1;
	change_sprite_alpha_state(bkgrnd_obj, 0);

	bkgrnd_obj = beginobj(&teamsel_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
	bkgrnd_obj->id = 1;
	change_sprite_alpha_state(bkgrnd_obj, 0);

	bkgrnd_obj = beginobj(&teamsel,  		 256.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
	bkgrnd_obj->id = 1;
	change_sprite_alpha_state(bkgrnd_obj, 0);
	bkgrnd_obj->mode = FLIP_TEX_H;
	generate_sprite_verts(bkgrnd_obj);

	bkgrnd_obj = beginobj(&teamsel_c1,       256.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
	bkgrnd_obj->id = 1;
	change_sprite_alpha_state(bkgrnd_obj, 0);
	bkgrnd_obj->mode = FLIP_TEX_H;
	generate_sprite_verts(bkgrnd_obj);

	lock_multiple_textures(BKGRND2_TID,-1);

	qcreate("timer",TIMER_PID,timedown,25,9,SPRITE_HRES/2.0f,SPRITE_VRES/2.0f);			//wait 25 seconds cnt from 9

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	// create ATTRIBUTE NAMES plaque
	b_logo = beginobj(((was_low_res) ? &attnames_lr : &attnames),
		0,
		((was_low_res) ? SPRITE_VRES*0.6666666f : SPRITE_VRES),
		12,
		BKGRND2_TID);
	b_logo->id = 2;

	// Back it goes!!!
	is_low_res = was_low_res;

	// create BLITZ logo
	b_logo = beginobj(&i99, SPRITE_HRES/2.0f, SPRITE_VRES, 10, JAPPLE_BOX_TID);
//	b_logo = beginobj(&blitzlog, 0, SPRITE_VRES, 10, TM_SELECT_TID);
	b_logo->id = OID_TMSEL_LOGO;

	qcreate("sparkle",ROTATE_LOGO_PID,spawn_sparkles_proc,SPRITE_HRES/2.0f, SPRITE_VRES/2.0f,0,0);

	qcreate("p1_chal",P1_CHALNGR_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,0,0);
	qcreate("p2_chal",P2_CHALNGR_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,0,0);

	if (four_plr_ver)
	{
		qcreate("p3_chal",P3_CHALNGR_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,0,0);
		qcreate("p4_chal",P4_CHALNGR_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,0,0);
	}

	qcreate("tm1_sel",TM1_TM_SELECT_PID,team_select_cntrl,SIDE_0,PLYR_1,0,0);
	qcreate("tm2_sel",TM2_TM_SELECT_PID,team_select_cntrl,SIDE_1,PLYR_2,0,0);

//	sleep(5);			// allow above processes to start

	// Take away the plates 
	iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	sleep(1);
	draw_enable(1);
	normal_screen();


	// wait until player(s) are done selecting their teams
allow_new:
	do
	{
		sleep(1);
	} while (exit_status);


	//
	// If credits remain, show a box and force humans to use them up - allow new human to join, etc.
	//
	show_box = 0;

	// Never show the box if we are in free play
	if (!coin_check_freeplay())
	{
		for (i = 0; i < (four_plr_ver ? MAX_PLYRS : MAX_PLYRS-2); i++)
		{
			if ((p_status & (1<<i)) &&							// player in game?
				pdata_blocks[i].quarters_purchased < 4 &&		// not bought full game?
				check_credits_to_continue())					// more credits to eat?
			{
				show_box |= 1<<i;								// YES - Show the box
			}
		}
	}

	old_pstatus = p_status;
	if (show_box)
	{
		delete_multiple_strings( TIMER_MSG_ID, -1 );						// kill_countdown_timer() text
		killall( TIMER_PID, -1 );											// kill_countdown_timer()
		force_selection = 0;

		show_plysel_idiot_box = show_plysel_idiot_box + 10;
		idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2, SPRITE_VRES/2, 1.01f, SCR_PLATE_TID);
		idiot_bx_obj->w_scale = 1.52f;
		idiot_bx_obj->h_scale = 1.9f;
		idiot_bx_obj->id = OID_MESSAGE;
		generate_sprite_verts(idiot_bx_obj);

		set_string_id(0x34);
		set_text_color(LT_RED);
		set_text_z_inc(0.0f);

		set_text_font( is_low_res ? FONTID_BAST23LOW : FONTID_BAST25 );
		set_text_position(
			(SPRITE_HRES/2),
			(SPRITE_VRES/2 + (is_low_res ?  18 :  25)),
			1);
		pmsg[0] = oprintf( "%djPLEASE USE UP", (HJUST_CENTER|VJUST_CENTER) );

		set_text_position(
			(SPRITE_HRES/2),
			(SPRITE_VRES/2 + (is_low_res ? -27 : -20)),
			1);
		pmsg[1] = oprintf( "%djALL CREDITS", (HJUST_CENTER|VJUST_CENTER) );

		pflasher = qcreate("pflashC", MESSAGE_PID, flash_text_proc, (int)pmsg[0], (int)pmsg[1], 0, 0);

		snd_scall_bank(cursor_bnk_str,WARNING_SND,VOLUME4,127,PRIORITY_LVL5);

		// Wait for all of the credits to get used up
		exit_box = 0;
		while (check_credits_to_continue() && (p_status ^ exit_box))
		{
			sleep(1);

			// Got a new player?
			if (old_pstatus != p_status)
			{
				// Yes; Got a new team?
				if ((!four_plr_ver) ||
					(
						((old_pstatus & 12) == 0 && (p_status & 12) != 0) ||
						((old_pstatus &  3) == 0 && (p_status &  3) != 0)
					))
				{
					// Yes; clear out everything & go back to team select
					delobj( idiot_bx_obj );
					delete_multiple_strings( 0x34, -1 );
					kill( pflasher, 0 );
					qcreate("timer",TIMER_PID,timedown,2,9,SPRITE_HRES/2.0f,SPRITE_VRES/2.0f);			//wait 2 seconds cnt from 9

					exit_status |= (old_pstatus^p_status);
					goto allow_new;
				}
			}
			old_pstatus = p_status;

			if ((p_status & 1) && (pdata_blocks[0].quarters_purchased >= 4))
				exit_box |= 1;

			if ((p_status & 2) && (pdata_blocks[1].quarters_purchased >= 4))
				exit_box |= 2;

			if ((p_status & 4) && (pdata_blocks[2].quarters_purchased >= 4))
				exit_box |= 4;

			if ((p_status & 8) && (pdata_blocks[3].quarters_purchased >= 4))
				exit_box |= 8;
		}
		delobj( idiot_bx_obj );
		delete_multiple_strings( 0x34, -1 );
		kill( pflasher, 0 );
	}

	// If new guy bought in for first time with last remaining credits - go back to picking team for him

	// Once we get to this point, we must pick CPU team opponent (if necessary) before any SLEEP can happen!
	// otherwise, PSTATUS could get set!

	//
	// Pick opposing team if needed
	//

	// check player 1/player 2 of team 1 (4 player version)
	if ( (!(p_status & 0x03)) && (four_plr_ver))
	{
		// left team is drones
		game_info.home_team =1;
		if ((player3_data.plyr_name[0] != 0) || (player4_data.plyr_name[0] != 0))
		{
			
			game_info.team[0] = pick_cpu_team( player3_data.teams_defeated|player4_data.teams_defeated );
			random_drones = FALSE;
			human_winstreak = GREATER(player3_data.winstreak,player4_data.winstreak);
		}
		else
		{	// didn't enter inits, pick randomly
			game_info.team[0] = randrng(NUM_TEAMS);
			if( game_info.team[0] == game_info.team[1] )
			{
				// Old way would never select Washington unless player & <randrng>
				//  had both selected Tennessee
				if (game_info.team[0])
					game_info.team[0] -= 1;
				else
					game_info.team[0] += 1;
			}
			random_drones = TRUE;
			human_winstreak = 0;
		}
	}
	// check player 3/player 4 of team 2 (4 player version)
	if ( (!(p_status & 0x0c)) && (four_plr_ver))
	{
		// right team is drones
		game_info.home_team = 0;
		if ((player1_data.plyr_name[0] != 0) || (player2_data.plyr_name[0] != 0))
		{
			
			game_info.team[1] = pick_cpu_team( player1_data.teams_defeated|player2_data.teams_defeated );
			random_drones = FALSE;
			human_winstreak = GREATER(player1_data.winstreak,player2_data.winstreak);
		}
		else
		{	// didn't enter inits, pick randomly
			game_info.team[1] = randrng(NUM_TEAMS);
			if( game_info.team[1] == game_info.team[0] )
			{
				// Old way would never select Washington unless player & <randrng>
				//  had both selected Tennessee
				if (game_info.team[1])
					game_info.team[1] -= 1;
				else
					game_info.team[1] += 1;
			}
			random_drones = TRUE;
			human_winstreak = 0;
		}
	}

	// check player 1/team 1 (2 player version)
	if ( !(p_status & (1<<0)) && (!four_plr_ver))
	{	// left side is drones.  pick 'em, make right side home team
		game_info.home_team = 1;
		if (player2_data.plyr_name[0] != 0)
		{
			game_info.team[0] = pick_cpu_team( player2_data.teams_defeated );
			random_drones = FALSE;
			human_winstreak = player2_data.winstreak;
		}
		else
		{	// didn't enter inits, pick randomly
			game_info.team[0] = randrng(NUM_TEAMS);
			if( game_info.team[0] == game_info.team[1] )
			{
				// Old way would never select Washington unless player & <randrng>
				//  had both selected Tennessee
				if (game_info.team[0])
					game_info.team[0] -= 1;
				else
					game_info.team[0] += 1;
			}
			random_drones = TRUE;
			human_winstreak = 0;
		}
	}
	// check player 2/team 2 (2 player version)
	else if ( !(p_status & (1<<1)) && (!four_plr_ver))
	{	// right side is drones.  pick 'em, make left side home team
		game_info.home_team = 0;
		if (player1_data.plyr_name[0] != 0)
		{
			game_info.team[1] = pick_cpu_team( player1_data.teams_defeated );
			random_drones = FALSE;
			human_winstreak = player1_data.winstreak;
		}
		else
		{	// didn't enter inits, pick randomly
			game_info.team[1] = randrng(NUM_TEAMS);
			if( game_info.team[1] == game_info.team[0] )
			{
				// Old way would never select Washington unless player & <randrng>
				//  had both selected Tennessee
				if (game_info.team[1])
					game_info.team[1] -= 1;
				else
					game_info.team[1] += 1;
			}
			random_drones = TRUE;
			human_winstreak = 0;
		}
	}
	else
		// no drones.  random home team
		game_info.home_team = randrng(2);


	if ( ((p_status & 1) && (!four_plr_ver)) || (((p_status & 1)||(p_status & 2)) && (four_plr_ver)) )
	{
		do_team_percent_aud(game_info.team[0]+ARIZONA_CARDINALS_AUD);
	}
	if ( ((p_status & 2) && (!four_plr_ver)) || (((p_status & 4)||(p_status & 8)) && (four_plr_ver)) )
	{
		do_team_percent_aud(game_info.team[1]+ARIZONA_CARDINALS_AUD);
	}


	delete_multiple_strings( TIMER_MSG_ID, -1 );						// kill_countdown_timer() text
	killall( TIMER_PID, -1 );											// kill_countdown_timer()
	force_selection = 0;
	qcreate("timer",TIMER_PID,timedown,2,3,SPRITE_HRES/2.0f,70.0f);		//wait 2 seconds cnt from 3


	tn = tlist;
	while (tn)
	{
		tn->texture_count = 0;
		tn = tn->next;
	}

	tn = find_texture("plqfsh.wms", JAPPLE_BOX_TID);
	if (tn)
	{
		unlock_texture(tn);
		delete_texture(tn);
	}
	if (tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][0] != NULL)
	{
		lock_texture(tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][0]->tn);
	}
	if (tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][1] != NULL)
	{
		lock_texture(tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][1]->tn);
	}
	delete_multiple_textures(TM_SEL_LOGOS_TID, -1);
	unlock_multiple_textures(TM_SEL_LOGOS_TID, -1);


	if (game_config_confirm())
	{
		// delete PLAYER models
		delete_multiple_objects( OID_PLAYER, ~0xFF );

		sleep(1);
		draw_enable(0);

		delete_multiple_strings( TIMER_MSG_ID, -1 );
		killall( TIMER_PID, -1 );

		// kill processes (if not dead already)
		killall(TM1_TM_SELECT_PID,0xFFFFFFFF);
		killall(TM2_TM_SELECT_PID,0xFFFFFFFF);
		delete_multiple_textures(TXID_PLAYER,0xFFFFFFFF);

		killall(ROTATE_LOGO_PID,0xFFFFFFFF);							// Kill sparkles on top of blitz logo
		del1c(JAPPLE_BOX_TID, -1);		// this blows away sparkles

		killall(P1_CHALNGR_PID, 0xffffffff);
		killall(P2_CHALNGR_PID, 0xffffffff);
		killall(P3_CHALNGR_PID, 0xffffffff);
		killall(P4_CHALNGR_PID, 0xffffffff);
//		killall(CREDIT_PID, 0xffffffff);
//		del1c(0, -1);
//		del1c(1, -1);
//		unlock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
		qcreate("timer",TIMER_PID,timedown,15,5,SPRITE_HRES/2.0f,70.0f);	//wait 15 seconds, cnt from 5
		game_config();		
	} else {

		iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		// Wait for plates; set time-out so we don't hang
		i = PLATE_TRANSITION_TICKS * 2;
		// Do it this way so we can sync-up the sound
		while (plates_on != PLATES_ON && --i) sleep(1);
		// Do sound only if the transition finished, not the time-out
		if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

		// freeze the screen
		draw_enable(0);
		sleep(1);
	}

	// delete PLAYER models
	delete_multiple_objects( OID_PLAYER, ~0xFF );

	// kill processes (if not dead already)
	killall(TM1_TM_SELECT_PID,0xFFFFFFFF);
	killall(TM2_TM_SELECT_PID,0xFFFFFFFF);

	killall(P1_CHALNGR_PID,0xFFFFFFFF);
	killall(P2_CHALNGR_PID,0xFFFFFFFF);
	if (four_plr_ver)
	{
		killall(P3_CHALNGR_PID,0xFFFFFFFF);
		killall(P4_CHALNGR_PID,0xFFFFFFFF);
	}
	killall(TIMER_PID,0xFFFFFFFF);										//	kill_countdown_timer();
	killall(CREDIT_PID,0xFFFFFFFF);
	killall(ROTATE_LOGO_PID,0xFFFFFFFF);							// Kill sparkles on top of blitz logo
							
	// delete all the objects with ID of 0 (thats any non screen to screen object) and 1
	delete_all_sprites();
	delete_all_background_sprites();
//	delete_multiple_objects( OID_NBR3D, ~0xFF );

	// delete textures
	delete_multiple_textures(TXID_PLAYER,0xFFFFFFFF);

	delete_multiple_textures(BKGRND2_TID,0xFFFFFFFF);
	unlock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
	delete_multiple_textures(TM_SELECT_TID,0xFFFFFFFF);
	delete_multiple_textures(TM_SEL_LOGOS_TID,0xFFFFFFFF);
	delete_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
	unlock_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);
	delete_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);
}

//-------------------------------------------------------------------------------------------------
//	   	This process handles everything to do with getting the TEAM selection from players
//-------------------------------------------------------------------------------------------------
#define UNIFORM_LD_DELAY		12
#define UPDATE_UNIFORM			0
#define MAYBE_UPDATE_UNIFORM	1

#define LEFT_COLOR	0xff6400ff
#define RIGHT_COLOR 0xffff0000

void team_select_cntrl(int *args)
{
	short 		sw_select_delay_cnt = 40;				// # of ticks to wait before allowing team selection
	short 		sw_delay_cnt = 0;
	short	 	sw_repeat_cnt = 0;
	int 		tm_nbr;
	int 		tm_mask;
	int			butn_sws = 0;
	int			butn_sws_cur = 0;
	int			prev_joy_sws = 0;
	int			joy_sws = 0;
	int			p_num = 0;
	int			i,j;
	int			auto_select_tm;
	int 		rnd_team;
	int			mv_cnt;							// number of times to move once random select enabled
	short		menu_level;
	short		cur_pos;						// index into alpha table and pin table
	short		tm_selected;
	float		tmp_y;
	int			was_low_res = is_low_res;
	sprite_info_t * plaq_obj;
	int			load_delay = UNIFORM_LD_DELAY;		// ticks to wait after no joy move...to load team uniform
//	image_info_t * pii;


	auto_select_tm = 0;						// no
	rnd_team = 0;
	mv_cnt = 0;
	tm_nbr = args[0];						// left side or right side
	cur_pos = 0;							// start on arizona
	game_info.team[tm_nbr] = -1;			// no team selected yet.

	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr] = NULL;

	if (four_plr_ver)
	{	// 4 plyr four_plr_ver
		if (tm_nbr)
			tm_mask = 0x0C;					// team 2 (players bits 3-4) mask
		else
			tm_mask = 0x03;					// team 1 (players bits 1-2) mask
	}
	else
	{	// 2 plyr version
		if (tm_nbr)
			tm_mask = 0x02;					// team 2 (player 2) mask
		else
			tm_mask = 0x01;					// team 1 (player 1) mask
	}

	// wait for one of the players from TEAM to join
	while (!(p_status & tm_mask))
		sleep(1);
//	exit_status |= 1<<tm_nbr;				// set TEAMS bit

	// determine which player picks team
	if (!four_plr_ver)
	{
		p_num = args[1];					// 2 player version (each player picks a team)
		exit_status |= 1<<p_num;			// set TEAMS bit
	}
	else
	{
		// 4 player version (one player from side picks a team)
		if (tm_nbr == 0)					// team 1 (left side)
		{
			if (bought_in_first & 1)
				p_num = 0;					// player 1 picks team (this makes player 1 the default for team 1)
			else
			if (bought_in_first & 2)
				p_num = 1;					// player 2 picks team
			exit_status |= 1<<p_num;		// set TEAMS bit

		}
		else
		{
			if (bought_in_first & 4)
				p_num = 2;					// plyr 3 picks team (this makes player 1 the default for team 1)
			else
			if (bought_in_first & 8)
				p_num = 3;					// plyr 4 picks team
			exit_status |= 1<<p_num;		// set TEAMS bit
		}
	}


	menu_level = TM_SELECT;																									//team selection

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	tmp_y = (was_low_res) ? SPRITE_VRES * 0.6666666f: SPRITE_VRES;

	// create team attribute meters
	for (i=TM_PASS_METR_OBJ_PTR,j=0; j < NUM_TM_ATTRIBS; i++,j++)
	{
		tm_selct_img_ptrs[i][tm_nbr] = beginobj(meter_imgs[!!was_low_res][tm_nbr][teaminfo[cur_pos].attr[j]],
				0,
				tmp_y,
				11,
				TM_SELECT_TID);
		tm_selct_img_ptrs[i][tm_nbr]->id = 2;
		tmp_y -= (was_low_res) ? 9.0f : 14.0f;
	}

	//
	// create city names (on plaque)
	//
	// top 3
	tm_selct_img_ptrs[TM_NME1_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_BUCS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][0], 12.7F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_NME1_OBJ_PTR][tm_nbr]->id = 2;

	tm_selct_img_ptrs[TM_NME2_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_TITANS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][1], 12.6F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_NME2_OBJ_PTR][tm_nbr]->id = 2;

	tm_selct_img_ptrs[TM_NME3_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_REDSKINS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][2], 12.5F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_NME3_OBJ_PTR][tm_nbr]->id = 2;

	// middle (hi-lited city name)
	if (!!was_low_res)
	{
		tm_selct_img_ptrs[TM_NME4_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_CARDINALS].pii_cnb,
				city_names_xs[tm_nbr], city_names_ys[!!was_low_res][3], 12.4F, TM_SELECT_TID);
		tm_selct_img_ptrs[TM_NME4_OBJ_PTR][tm_nbr]->id = 2;
	}
	else
	{
		tm_selct_img_ptrs[TM_NME4_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_CARDINALS].pii_cn,
				city_names_xs[tm_nbr], city_names_ys[!!was_low_res][3], 12.4F, TM_SELECT_TID);
		tm_selct_img_ptrs[TM_NME4_OBJ_PTR][tm_nbr]->id = 2;
	}
	// bottom 3	
	tm_selct_img_ptrs[TM_NME5_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_FALCONS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][4], 12.3F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_NME5_OBJ_PTR][tm_nbr]->id = 2;

	tm_selct_img_ptrs[TM_NME6_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_RAVENS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][5], 12.2F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_NME6_OBJ_PTR][tm_nbr]->id = 2;

	tm_selct_img_ptrs[TM_NME7_OBJ_PTR][tm_nbr] = beginobj(teaminfo[TEAM_BILLS].pii_cn,
			city_names_xs[tm_nbr], city_names_ys[!!was_low_res][6], 12.1F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_NME7_OBJ_PTR][tm_nbr]->id = 2;

	// create QB name
	tm_selct_img_ptrs[TM_QB_NME_OBJ_PTR][tm_nbr] = beginobj(
			teaminfo[cur_pos].static_data[PPOS_QBACK].pii_selname,
			logo_xys[screen][tm_nbr][four_plr_ver][X_VAL]-20.0f, (was_low_res ? 54.0f : 80.0f), 5.0F, TM_SELECT_TID);
	tm_selct_img_ptrs[TM_QB_NME_OBJ_PTR][tm_nbr]->id = 2;

	// create GRID behind the football player
	i = (tm_nbr ? RIGHT_COLOR : LEFT_COLOR);

	tmp_y = (was_low_res ? 0.6666666f : 1.0f);

	plaq_obj = beginobj((was_low_res ? &gridbox_lr : &gridbox),
			logo_xys[screen][tm_nbr][four_plr_ver][X_VAL],
			tmp_y * (135.0f),
			200,
			TM_SELECT_TID);
	plaq_obj->id = 2;
	plaq_obj->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	plaq_obj->state.constant_color = i;

	plaq_obj = beginobj((was_low_res ? &gridbox_lr : &gridbox),
			logo_xys[screen][tm_nbr][four_plr_ver][X_VAL],
			tmp_y * (135.0f + 127.0f),
			200,
			TM_SELECT_TID);
	plaq_obj->id = 2;
	plaq_obj->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	plaq_obj->state.constant_color = i;
	plaq_obj->mode = FLIP_TEX_V;
	generate_sprite_verts(plaq_obj);

	// Back it goes!!!
	is_low_res = was_low_res;

	// create number of player selecting team
	if (four_plr_ver)
	{
		sprite_info_t * psi = beginobj(tm_selct_nbrs_imgs[p_num],
//					city_names_xs[tm_nbr] + (tm_nbr ? 40.0f : -68.0f),
					city_names_xs[tm_nbr] + (tm_nbr ? 55.0f : -85.0f),
					city_names_ys[is_low_res][3] + (is_low_res ? 80.0f : 7.0f),
					4.5F,
					TM_SELECT_TID);
		psi->id = 2;
	}

	// create team logo
	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr] = beginobj(teaminfo[cur_pos].pii_logo30,
			logo_xys[screen][tm_nbr][four_plr_ver][X_VAL],
			logo_xys[screen][tm_nbr][four_plr_ver][Y_VAL],
			190.0F,
			TM_SEL_LOGOS_TID);
	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr]->id = 2;
	tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr]->tn->texture_count = 0;

	// create CITY NAMES plaque
//	tm_plaqs[tm_nbr] = beginobj(tm_city_plqs[tm_nbr], 0.0f, SPRITE_VRES, 190.0F+tm_nbr, TM_SELECT_TID);
//	tm_plaqs[tm_nbr]->id = 2;

	plaq_obj = beginobj(tm_city_plqs[tm_nbr], 0.0f, SPRITE_VRES, 190.0F+tm_nbr, TM_SELECT_TID);
	plaq_obj->id = 2;

	// brighten city names (except current team)
	qcreate("dimtext",FLASH_PID,dim_text_start,tm_nbr,0,0,0);

	// put up the FOOTBALL PLAYER model (QB looking back and forth)
	update_tm_data(cur_pos,tm_nbr,UPDATE_UNIFORM);

	qcreate( "3dqb", PLAYER_PID, tmsel_player_proc, (tm_nbr ? 10 : 3), tm_nbr, PPOS_QBACK, cur_pos);
//	qcreate( "3dqb", PLAYER_PID, tmsel_player_proc, (tm_nbr ? 10 : 3), tm_nbr, PPOS_QBACK, cur_pos * NUM_PERTEAM_SPD + 3);

	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way since we can't know how many ticks have lapsed since transition began
	while (plates_on != PLATES_OFF && --i)
		sleep(1);
	// Make sure plates are gone
	plates_on = PLATES_OFF;

	tm_selected = NO;
	while ((force_selection != 1) && (tm_selected != YES))
	{
		if (!auto_select_tm)																	// basically... during random select just stick joy to UP
		{
			prev_joy_sws = joy_sws;
			joy_sws = get_joy_val_cur(p_num);
			butn_sws = get_but_val_down(p_num);
			butn_sws_cur = get_but_val_cur(p_num);
		}
		//
		// handle joystick
		//
		if (joy_sws)
		{
			load_delay = UNIFORM_LD_DELAY;				// wait 45 ticks before load new uniform
			if ((joy_sws != prev_joy_sws) || ((!sw_delay_cnt) && (!sw_repeat_cnt)))
			{
				if (!auto_select_tm)
				{
					if (joy_sws != prev_joy_sws)
						sw_delay_cnt = TICKS_BEFORE_START_REPEAT;
					sw_repeat_cnt = TICKS_BETWEEN_REPEATS;
				}
				switch(joy_sws)
				{
					case JOY_LEFT:
					case JOY_UP_LFT:
					case JOY_UP_RGT:
					case JOY_UP:																					// up
						if (!auto_select_tm)
							snd_scall_bank(cursor_bnk_str,TMSEL_CURSR_SND,VOLUME4,127,PRIORITY_LVL1);
						if (cur_pos > 0)
							cur_pos--;
						else
						 	cur_pos = NUM_TEAMS-1;

						while (uniform_loading)
							sleep(1);
						uniform_loading = 1;
						load_tmsel_uniform(cur_pos);		// if needed
						uniform_loading = 0;

						update_tm_data(cur_pos,tm_nbr,MAYBE_UPDATE_UNIFORM);
						break;
					case JOY_RIGHT:
					case JOY_DWN_LFT:
					case JOY_DWN_RGT:
					case JOY_DOWN:
						snd_scall_bank(cursor_bnk_str,TMSEL_CURSR_SND,VOLUME4,127,PRIORITY_LVL1);
						if (cur_pos < NUM_TEAMS-1)																			// on last row ?
							cur_pos++;																					// no... allow down
						else
						 	cur_pos = 0;

						while (uniform_loading)
							sleep(1);
						uniform_loading = 1;
						load_tmsel_uniform(cur_pos);		// if needed
						uniform_loading = 0;

						update_tm_data(cur_pos,tm_nbr,MAYBE_UPDATE_UNIFORM);
						break;
				}
			}
			else
			{
				if (sw_delay_cnt != 0)
					sw_delay_cnt--;
				 else if (sw_repeat_cnt != 0)
					sw_repeat_cnt--;													// timedown before allow auto-repeat
			}
		}
//		else
//		{
//			if (load_delay > 0)
//			{
//				load_delay--;
//				if (load_delay == 0)
//				{
//					while (uniform_loading)
//						sleep(1);
//					uniform_loading = 1;
//					load_tmsel_uniform(cur_pos);		// if needed
//					uniform_loading = 0;
//					update_tm_data(cur_pos,tm_nbr,UPDATE_UNIFORM);
//				}
//			}
//		}
		//
		// handle button logic
		//
		if ((butn_sws) && (!auto_select_tm))
		{
//#if 0
			if ((butn_sws == TURBO_BUTTON) && (joy_sws == JOY_LEFT))
			{
				// start random team select; immediately available
				sw_delay_cnt = 0;
				sw_repeat_cnt = 0;
				prev_joy_sws = 0;				// in case player was already auto-repeating
				auto_select_tm = 1;
				rnd_team = randrng(NUM_TEAMS);
				mv_cnt = rnd_team + NUM_TEAMS*2;   								// loop at least twice
				snd_scall_bank(cursor_bnk_str,RND_SELCT_STRT_SND,VOLUME5,127,PRIORITY_LVL2);
			}
				// allow team selection after waiting <sw_select_delay_cnt> ticks
			else if (!sw_select_delay_cnt)
				tm_selected = YES;
//#endif
//			tm_selected = YES;
		}

		// update random select stuff
		if (auto_select_tm)
		{
			if (mv_cnt == 0)
			{
				auto_select_tm = 0;
				tm_selected = YES;
			}
			else
				mv_cnt--;
		}

		sleep(1);

		if (sw_select_delay_cnt)
			sw_select_delay_cnt--;
	}

	// team picked or time ran out
//	snd_scall_bank(cursor_bnk_str,TM_SELECTED_SND,VOLUME2,127,PRIORITY_LVL3);

	game_info.team[tm_nbr] = cur_pos;

	// Quickly flash logo white a few times
//	qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr],4,3,0);

	// flash something -- OK for lores Y to be scaled midres Y
	flash_status |= (1<<p_num);														// set bit in variable
	qcreate("cityflsh",FLASH_PID,stat_flash,city_names_xs[tm_nbr],city_names_ys[0][3]+1.0F,0,p_num);

	// dim TEAMS city names
	qcreate("dimtext",FLASH_PID,dim_text,tm_nbr,0,0,0);

	// wait for flash
	while (flash_status & 1<<p_num)
		sleep(2);

	sleep(60);

	exit_status &= ~(1<<p_num);				// clear TEAMS bit
//	exit_status &= ~(1<<tm_nbr);			// CLEAR teams (1 or 2) bit in variable
}


//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void update_tm_data(short c_pos, short tm_nbr, int flag)
{
	signed short		i,j;
	int					was_low_res = is_low_res;
	fbplyr_data			*ppdata;


	if ((flag == UPDATE_UNIFORM) || 
		((flag == MAYBE_UPDATE_UNIFORM) && (uniforms_loaded & (1<<c_pos))))
	{
		// update PLAYERS uniform and NUMBER
		for (i=0; i < 4; i++)				// only 5 of the textures have to change
		{
			team_decals[tm_nbr*2][i+2] = all_teams_decals[c_pos][i];
			team_decals[1+tm_nbr*2][i+2] = all_teams_decals[c_pos][i];
		}

		ppdata = player_blocks + (tm_nbr ? 10 : 3);
		ppdata->static_data = teaminfo[c_pos].static_data+3;
		ppdata->odata.pdecal = team_decals[tm_nbr * 2 + (ppdata->static_data->color==PC_WHITE ? 0:1)];
	}

	// update team logo
	change_img_tmu(tm_selct_img_ptrs[TM_LOGO_OBJ_PTR][tm_nbr], teaminfo[c_pos].pii_logo30, TM_SEL_LOGOS_TID);

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	// update team QB roster string
	change_img_tmu(tm_selct_img_ptrs[TM_QB_NME_OBJ_PTR][tm_nbr],
					teaminfo[c_pos].static_data[PPOS_QBACK].pii_selname,
					TM_SELECT_TID);

	// update team attributes meters
	for (i=TM_PASS_METR_OBJ_PTR,j=0; j < NUM_TM_ATTRIBS; i++,j++)
		change_img_tmu(tm_selct_img_ptrs[i][tm_nbr], meter_imgs[!!was_low_res][tm_nbr][teaminfo[c_pos].attr[j]], TM_SELECT_TID);

	// update top three city names
	for (i=TM_NME3_OBJ_PTR,j=c_pos-1; i >= TM_NME1_OBJ_PTR; i--,j--)
	{
		if (j < 0)
			j = NUM_TEAMS-1;
		change_img_tmu(tm_selct_img_ptrs[i][tm_nbr], teaminfo[j].pii_cn, TM_SELECT_TID);
	}

	// update hi-lited (middle) city name
	change_img_tmu(tm_selct_img_ptrs[TM_NME4_OBJ_PTR][tm_nbr], teaminfo[c_pos].pii_cnb, TM_SELECT_TID);

	// update bottom four city names
	for (i=TM_NME5_OBJ_PTR,j=c_pos+1; i <= TM_NME7_OBJ_PTR; i++,j++)
	{
		if (j > NUM_TEAMS-1)
			j = 0;
		change_img_tmu(tm_selct_img_ptrs[i][tm_nbr], teaminfo[j].pii_cn, TM_SELECT_TID);
	}

	// update team roster strings
//		j = TM_QB_NME_OBJ_PTR;
//		while (j <= TM_RNME9_OBJ_PTR)
//		{
//			if (pspd->pii_selname)
//			{
//				change_img_tmu(tm_selct_img_ptrs[j][tm_nbr],pspd->pii_selname,TM_SELECT_TID);
//				j++;
//			}
//			pspd++;
//		}

	// Back it goes!!!
	is_low_res = was_low_res;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void dim_text(int *args)
{
int	i,j,tm_num;

	tm_num = args[0];

	// Make rosters more yellow	in color
//	for (i=TM_QB_NME_OBJ_PTR,j=0; i <= TM_RNME9_OBJ_PTR; i++,j++)
//		tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffFFFF00;

	// change the text to a constant color ... so I can fade it down to a darker (dimmer) color.
	for (i=TM_NME1_OBJ_PTR; i <= TM_NME3_OBJ_PTR; i++)
	{
		tm_selct_img_ptrs[i][tm_num]->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
		tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffB0B0B0;
	}
	for (i=TM_NME5_OBJ_PTR; i <= TM_NME7_OBJ_PTR; i++)
	{
		tm_selct_img_ptrs[i][tm_num]->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
		tm_selct_img_ptrs[i][tm_num]->state.constant_color = 0xffB0B0B0;
	}

	for (j=0; j < 20; j++)
	{
		// dim top four city names
		for (i=TM_NME1_OBJ_PTR; i <= TM_NME3_OBJ_PTR; i++)
			tm_selct_img_ptrs[i][tm_num]->state.constant_color -= 0x00020202;

		// dim bottom four city names
		for (i=TM_NME5_OBJ_PTR; i <= TM_NME7_OBJ_PTR; i++)
			tm_selct_img_ptrs[i][tm_num]->state.constant_color -= 0x00020202;

		sleep (2);
	}
}

//-------------------------------------------------------------------------------------------------
//						This routine controls the in-between period buying and removing of credits
//
//
// RETURNS: 1 = game over, else 0
//-------------------------------------------------------------------------------------------------
int buyin_scrn(void)
{
	register int	i, mode;
	int				status;
	int				score[2];
	int				free_quarters = 0;

	// Turn on start buttons
	start_enable(TRUE);

	// If any player was in the game and is now out and is ahead (score)
	// then award that player another quarter and put him back in the game.
	// This code handles quarters 1 through 3.  If the score is tied at the
	// end of the 4th quarter or overtime quarters 1 and/or 2 we end up here,
	// but because the score is tied, the player MUST buy the next quarter.

	// Are we awarding free quarters to the player that is ahead ?
	while (1)
	{
		// Chk HH vs HH - game must be humans-only to qualify
		if (!get_adjustment(H4_FREE_QUARTER_ADJ, &status))
			if (qtr_pstatus[0] == qtr_pstatus[2] &&
					qtr_pstatus[0] == qtr_pstatus[1] &&
					qtr_pstatus[2] == qtr_pstatus[3])
				break;

		// Chk H vs H - game must be same H vs H to qualify
		if (!get_adjustment(HH_FREE_QUARTER_ADJ, &status))
		{
			if (four_plr_ver)
			{
				if ((qtr_pstatus[0] + qtr_pstatus[1]) ==
						(qtr_pstatus[2] + qtr_pstatus[3]) &&
						(!qtr_pstatus[0] || !qtr_pstatus[1]) &&
						(!qtr_pstatus[2] || !qtr_pstatus[3]))
					break;
			}
			else if (qtr_pstatus[0] == qtr_pstatus[1])
				break;
		}

		// Chk H vs CPU - game must be one human team to qualify
		if (!get_adjustment(HC_FREE_QUARTER_ADJ, &status))
		{
			if (four_plr_ver)
			{
				if ((qtr_pstatus[0] == qtr_pstatus[1] &&
						!(qtr_pstatus[2] + qtr_pstatus[3])) ||
						(qtr_pstatus[2] == qtr_pstatus[3] &&
						!(qtr_pstatus[0] + qtr_pstatus[1])))
					break;
			}
			else if ((p_status2 == 1 && !qtr_pstatus[1]) ||
					(p_status2 == 2 && !qtr_pstatus[0]))
				break;
		}

		// Nothing to chk
		status = 0;
		break;
	}

	// Do the chk if set
	if (status)
	{
		score[0] = compute_score(0);
		score[1] = compute_score(1);

		for (mode = 0, i = 0; i < ((four_plr_ver) ? MAX_PLYRS : 2); i++)
		{
			if (i == ((four_plr_ver) ? (MAX_PLYRS/2) : (2/2)))
				mode = 1;

			// Was this player in and now out ?
			if ((p_status ^ p_status2) & (1 << i))
			{
				// YES - Is this player ahead ?
				if (score[mode] > score[mode ^ 1])
				{
					// YES - Give him another quarter and put him back in
					// the game
					pdata_blocks[i].quarters_purchased++;
					if(pdata_blocks[i].quarters_purchased > 4)
					{
						pdata_blocks[i].quarters_purchased = 1;
					}

					// Put player in game
					p_status |= (1 << i);

					// Chalk a free quarter audit
					increment_audit(FREE_QUARTERS_AUD);

					// Mark player that got free quarter
					free_quarters |= (1 << i);
				}
			}
		}
	}

	// If either player/team is out - give 'em a chance to buy in
	if (((p_status != 3) && !(four_plr_ver)) ||
		((p_status != 15) && (four_plr_ver)))
	{
		screen = BUY_IN_SCRN;

		// load end-of-quarter images
		if (!create_texture("plqcrd00.wms", JAPPLE_BOX_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR))
		{
#ifdef DEBUG
				fprintf( stderr,"Error loading plqcrd00.wms' texture \n");
#endif
				increment_audit(TEX_LOAD_FAIL_AUD);
		}

		if (!create_texture("plqcrd01.wms", JAPPLE_BOX_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR))
		{
#ifdef DEBUG
				fprintf( stderr,"Error loading plqcrd01.wms' texture \n");
#endif
				increment_audit(TEX_LOAD_FAIL_AUD);
		}

		lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

		// If halftime, we let halftime music from stats page continue over buyin page
		if (game_info.game_quarter != 2)
		{
			// Stop coaching tips/halftime music
			snd_stop_track(SND_TRACK_0);
			// reset track allocations
			snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
			// Play buyin groove
			snd_scall_bank(eoq_tune_bnk_str,0,VOLUME5,127,SND_PRI_SET|SND_PRI_TRACK1);
		}

		draw_backdrop();
//		draw_backgrnd2(2.6f, JAPPLE_BOX_TID);

		qcreate("cred_msg",CREDIT_PID,credit_msg,screen,JAPPLE_BOX_TID,0,0);

		qcreate("crdbox1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("crdbox2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
		qcreate("p1chlngr",CREDIT_PID,challenger_needed_msg,PLYR_1,JAPPLE_BOX_TID,free_quarters,0);
		qcreate("p2chlngr",CREDIT_PID,challenger_needed_msg,PLYR_2,JAPPLE_BOX_TID,free_quarters,0);

		if (four_plr_ver)
		{
			qcreate("crdbox3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
			qcreate("crdbox4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
			qcreate("p3chlngr",CREDIT_PID,challenger_needed_msg,PLYR_3,JAPPLE_BOX_TID,free_quarters,0);
			qcreate("p4chlngr",CREDIT_PID,challenger_needed_msg,PLYR_4,JAPPLE_BOX_TID,free_quarters,0);
		}

		// Announce "get ready for x period at top of buy in screen" - unless we need to show free quarter message
		if (free_quarters == 0)
			get_ready_for();		

		reset_on_coin = 1;
		qcreate("timer",TIMER_PID,timedown,0,9,SPRITE_HRES/2.0f,90.0F);	//wait 1 second then cnt from 9

		// print names in japple boxes
		if (player1_data.plyr_name[0] != NULL)
			print_players_name(0, japbx_cntr_xys[0][four_plr_ver][X_VAL], japbx_cntr_xys[0][four_plr_ver][Y_VAL], player1_data.plyr_name, 0);
		if (player2_data.plyr_name[0] != NULL)
			print_players_name(1, japbx_cntr_xys[1][four_plr_ver][X_VAL], japbx_cntr_xys[1][four_plr_ver][Y_VAL], player2_data.plyr_name, 0);

		if (four_plr_ver)
		{
			if (player3_data.plyr_name[0] != NULL)
				print_players_name(2, japbx_cntr_xys[2][four_plr_ver][X_VAL], japbx_cntr_xys[2][four_plr_ver][Y_VAL], player3_data.plyr_name, 0);
			if (player4_data.plyr_name[0] != NULL)
				print_players_name(3, japbx_cntr_xys[3][four_plr_ver][X_VAL], japbx_cntr_xys[3][four_plr_ver][Y_VAL], player4_data.plyr_name, 0);
		}

		if (plates_on != PLATES_OFF)
		{
			// Take away the plates
			iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
			// Be sure they go away (try anyway)
			sleep(PLATE_TRANSITION_TICKS);
			plates_on = PLATES_OFF;
		}

		// Make em see it for at least 2 seconds
//		sleep(120);

		do
		{
			sleep(1);
		}	while(!force_selection);

		unlock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

		// kill processes
		killall(BONUS_PID, -1);
		killall(TIMER_PID,0xFFFFFFFF);										//	kill_countdown_timer();
		killall(CREDIT_PID,0xFFFFFFFF);										//	kill_countdown_timer();

		// Turn on the plates
		iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		// Wait for plates; set time-out so we don't hang
		i = PLATE_TRANSITION_TICKS * 2;

		// Do it this way so we can sync-up the sound
		while (plates_on != PLATES_ON && --i) sleep(1);

		// Stop buyin music
		snd_stop_track(SND_TRACK_1);
		snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

		// Play drum loop under transition plates
		snd_scall_bank(generic_bnk_str, 16, VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK0);

		// Do sound only if the transition finished, not the time-out,
		//  and the game didn't end
		if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

		// delete objects and strings
		delete_multiple_strings(CREDIT_MSG_ID, 0xffffffff);
		delete_multiple_strings(CREDIT_MSG_ID + 1, -1);
		delete_multiple_strings(TIMER_MSG_ID, 0xffffffff);
		delete_multiple_objects( OID_NBR3D, ~0xFF );

		// delete player name strings
		delete_multiple_strings(plr_nme_str_ids[0], 0xffffffff);
		delete_multiple_strings(plr_nme_str_ids[1], 0xffffffff);

		if (four_plr_ver)
		{
			delete_multiple_strings(plr_nme_str_ids[2], 0xffffffff);
			delete_multiple_strings(plr_nme_str_ids[3], 0xffffffff);
		}
//		{
//			extern sprite_node_t * alpha_sprite_node_list;
//			sprite_node_t * slist = alpha_sprite_node_list;
//			while (slist)
//			{
//				if ((slist->si->id & ~3) == OID_CHLNGR_BOX)
//				{
//					fprintf(stderr,"%s\r\n",slist->si->ii->texture_name);
//				}
//				slist = slist->next;
//			}
//			fflush(stdout);
//			sleep(1);
//		}
		del1c( OID_MESSAGE, -1 );					// Result box underneath "get ready for" message
		del1c( 1, -1 );								// delete background
		del1c( OID_CHLNGR_BOX, ~3 );				// delete all challenger boxes
		del1c( OID_JAPPLE_BOX, -1 );				// all japple box stuff

		delete_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

		// if all players dropped out...then end game
		if (!p_status)
		{
			start_enable(FALSE);
			reset_on_coin = 0;
			return(1);
		}

//#if DEBUG
//		printf("Restoring ingame textures\n");
//#endif

		load_ingame_textures();

		start_enable(FALSE);
		reset_on_coin = 0;
		return(0);
	}

//#if DEBUG
//	printf("Restoring ingame textures after coaching tips screen only\n");
//#endif

	// Delete background texture from coaching tips scrn
	delete_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
	load_ingame_textures();

	start_enable(FALSE);
	reset_on_coin = 0;
	return(0);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void coaching_tip_scrn(void)
{
	coach_msg_t * phint;
	process_node_t	*flash = NULL;
	sprite_info_t *bkgrnd_obj;
	ostring_t * f_msg;
	float ypos = 0;
	int i;

	static coach_msg_t hint1_txt[] =
	{
		{"QB JUMP PASS",-50},
		{"RUN AWAY FROM BLITZING",-20},
		{"DEFENDERS, HIT TURBO & JUMP.",-20},
		{"WHILE IN THE AIR, CHOOSE A",-20},
		{"RECEIVER AND PRESS THE",-20},
		{"PASS BUTTON!",-35},
		{"HELPS AVOID QB SACKS!",0}
	};

	static coach_msg_t hint2_txt[] =
	{
		{"TURBO BUTTON",-50},
		{"USE THE TURBO BUTTON TO MAKE",-20},
		{"YOUR PLAYER RUN FASTER, JUMP",-20},
		{"HIGHER, AND HIT HARDER!",-20},
		{"HARD HITS WILL CAUSE THE BALL",-20},
		{"CARRIER TO FUMBLE MORE OFTEN!",-35},
		{"USE YOUR TURBO WISELY!",0}
	};

	static coach_msg_t hint3_txt[] =
	{
		{"SPIN MOVE",-50},
		{"QUICKLY TAP THE TURBO BUTTON",-20},
		{"TO PERFORM A SPIN MOVE.",-20},
		{"SPIN MOVES CAUSE THE DEFENDER",-20},
		{"TO MISS HIS TACKLE!",-20},
		{"TURBO METER CAN'T BE EMPTY!",-35},
		{"USE YOUR TURBO WISELY!",0}
	};

	static coach_msg_t hint4_txt[] =
	{
		{"STIFF ARM",-50},
		{"WHILE CARRYING THE BALL BEYOND",-20},
		{"THE LINE OF SCRIMMAGE, HOLD",-20},
		{"TURBO, THEN PRESS THE PASS",-20},
		{"BUTTON.  THIS WILL KNOCK",-20},
		{"DOWN A WOULD BE TACKLER!",0}
	};

	static coach_msg_t hint5_txt[] =
	{
		{"DIVE FORWARD",-50},
		{"WHEN ON OFFENSE, DIVE FOR A",-20},
		{"FEW EXTRA YARDS BY HOLDING",-20},
		{"TURBO AND QUICKLY TAPPING",-20},
		{"THE JUMP BUTTON.",0}
	};

	static coach_msg_t hint6_txt[] =
	{
		{"PUSH DOWNS",-50},
		{"WHEN ON DEFENSE, HOLD TURBO",-20},
		{"AND PRESS THE CHANGE PLAYER",-20},
		{"BUTTON TO KNOCK DOWN YOUR",-20},
		{"OPPONENT.  KNOCK DOWN A WIDE",-20},
		{"RECEIVER RUNNING HIS ROUTE!",0}
	};

	static coach_msg_t hint7_txt[] =
	{
		{"SPEED THROUGH",-50},
		{"PRESS AND HOLD ALL 3 BUTTONS",-20},
		{"AT THE END OF ANY PLAY TO",-20},
		{"QUICKLY ADVANCE TO THE NEXT",-20},
		{"SCREEN.",0}
	};

	static coach_msg_t hint8_txt[] =
	{
		{"HIGH HURDLE",-50},
		{"WHEN ON OFFENSE, HOLD TURBO",-20},
		{"AND PRESS THE JUMP BUTTON",-20},
		{"TO LEAP OVER WOULD BE",-20},
		{"TACKLERS!  HOWEVER, FUMBLES",-20},
		{"HAPPEN MORE ON HURDLES!",0}
	};

	static coach_msg_t hint9_txt[] =
	{
		{"STRONG TACKLE",-50},
		{"HOLD TURBO AND PRESS THE",-20},
		{"TACKLE BUTTON TO PERFORM AN",-20},
		{"AGGRESSIVE TACKLE.  STRONG",-20},
		{"TACKLES WILL CAUSE MORE",-20},
		{"FUMBLES TO HAPPEN!",0}
	};

	static coach_msg_t hint10_txt[] =
	{
		{"HURDLE LINE",-50},
		{"WHEN ON DEFENSE, HOLD",-20},
		{"TURBO AND PRESS TACKLE TO",-20},
		{"HURDLE OVER THE LINE OF",-20},
		{"SCRIMMAGE AND QUICKLY SACK",-20},
		{"THE QUARTERBACK!",0}
	};

	static coach_msg_t hint11_txt[] =
	{
		{"ONSIDE KICK",-50},
		{"AFTER ANY SCORE, PRESS UP",-20},
		{"ON THE STICK AND PRESS ALL",-20},
		{"3 BUTTONS.  THIS WILL CAUSE",-20},
		{"THE KICKING TEAM TO PERFORM",-20},
		{"A DANGEROUS ONSIDE KICK!",0}
	};

	static coach_msg_t hint12_txt[] =
	{
		{"MAN IN MOTION",-50},
		{"BEFORE THE SNAP, PUSH STICK",-20},
		{"LEFT OR RIGHT TO REPOSITION",-20},
		{"ONE RECEIVER.  HIKE THE BALL",-20},
		{"WITH BOTH JUMP AND PASS",-20},
		{"BUTTONS TO MAKE HIM BLOCK!",0}
	};

	static coach_msg_t hint13_txt[] =
	{
		{"EXTRA BLOCKER",-50},
		{"HIKE THE BALL WITH BOTH THE",-20},
		{"JUMP AND PASS BUTTONS TO",-20},
		{"CAUSE MAN IN MOTION TO BLOCK",-20},
		{"ONCE YOU HIKE THE BALL!",0}
	};

	static coach_msg_t hint14_txt[] =
	{
		{"LOOSE BALL",-50},
		{"ON BOBBLES OR FUMBLES, HOLD",-20},
		{"THE TURBO BUTTON AND PRESS",-20},
		{"THE JUMP BUTTON TO LEAP AT",-20},
		{"AND RECOVER THE LOOSE BALL.",0}
	};

	static coach_msg_t hint15_txt[] =
	{
		{"JUMP CATCH",-50},
		{"IF YOU PRESS THE JUMP",-20},
		{"BUTTON WHEN THE PASS IS NEAR",-20},
		{"THE TARGET WIDE RECEIVER, HE",-20},
		{"WILL JUMP UP TO CATCH IT.",-20},
		{"IT SURPRISES THE DEFENDER!",0}
	};

	static coach_msg_t hint16_txt[] =
	{
		{"INTERCEPTIONS",-50},
		{"IF YOU PRESS THE JUMP",-20},
		{"BUTTON WHEN THE PASS IS NEAR",-20},
		{"YOUR DEFENDER, HE WILL JUMP",-20},
		{"UP TO INTERCEPT IT!",-20},
		{"DON'T MISS IT THOUGH!",0}
	};

	static coach_msg_t hint17_txt[] =
	{
		{"HIDDEN CURSOR",-50},
		{"PUSH STICK UP TWICE ON THE",-20},
		{"PLAY SELECT SCREEN TO HIDE",-20},
		{"YOUR CURSOR.  THIS KEEPS",-20},
		{"YOUR OPPONENT FROM SEEING",-20},
		{"WHICH PLAY YOU PICK.",0}
	};

	static coach_msg_t hint18_txt[] =
	{
		{"RANDOM PLAY",-50},
		{"PLAYERS CAN HAVE THE",-20},
		{"CPU CHOOSE A RANDOM",-20},
		{"PLAY ON THE PLAY",-20},
		{"SELECT PAGE.",0}
	};

	static coach_msg_t hint19_txt[] =
	{
		{"SECRET PLAYS",-50},
		{"TRY DIFFERENT STICK AND",-20},
		{"BUTTON COMBINATIONS TO",-20},
		{"FIND HIDDEN PLAYS!",0}
	};

	static coach_msg_t hint20_txt[] =
	{
		{"PASS LIMIT",-50},
		{"NO QUARTERBACK CAN PASS",-20},
		{"THE BALL FURTHER THAN",-20},
		{"60 YARDS!",-20},
		{"EACH QUARTERBACK HAS HIS",-20},
		{"OWN PASS RATING!",0}
	};

	static coach_msg_t hint21_txt[] =
	{
		{"SPIN FUMBLES",-50},
		{"IF A PLAYER PERFORMS",-20},
		{"MULTIPLE SPINS DURING",-20},
		{"ONE PLAY, HE IS MORE",-20},
		{"LIKELY TO FUMBLE WHEN HE",-20},
		{"GETS TACKLED!",0}
	};

	static coach_msg_t hint22_txt[] =
	{
		{"FIRE RULES",-50},
		{"IF THE DEFENSIVE TEAM",-20},
		{"TACKLES THE BALL CARRIER",-20},
		{"2 TIMES FOR A LOSS OF",-20},
		{"YARDAGE DURING ANY ONE",-20},
		{"POSSESSION, HE IS ON FIRE!",0}
	};

	static coach_msg_t hint23_txt[] =
	{
		{"EXPERIENCE",-50},
		{"YOU NEED TO PLAY AT",-20},
		{"LEAST 5 GAMES BEFORE YOUR",-20},
		{"NAME WILL APPEAR ON THE",-20},
		{"HIGH SCORE PAGES.  EXCEPT",-20},
		{"FOR THE WORLD RECORDS PAGE.",0}
	};

	static coach_msg_t hint24_txt[] =
	{
		{"EASY PASSING",-50},
		{"POINT STICK TO LEFT TO",-20},
		{"CHOOSE LEFT MOST RECEIVER.",-20},
		{"POINT STICK TO RIGHT TO",-20},
		{"CHOOSE RIGHT MOST RECEIVER.",-20},
		{"UP CHOOSES MIDDLE RECEIVER.",0}
	};

	static coach_msg_t hint25_txt[] =
	{
		{"CHANGE PLAYER",-50},
		{"WHEN ON DEFENSE, THE CHANGE",-20},
		{"PLAYER BUTTON WILL CAUSE",-20},
		{"YOU TO TAKE CONTROL OF THE",-20},
		{"DEFENDER NEAREST THE BALL",-20},
		{"CARRIER.",0}
	};

	static coach_msg_t hint26_txt[] =
	{
		{"SWAT BALL",-50},
		{"WHEN ON DEFENSE, AND THE",-20},
		{"BALL IS IN THE AIR, THE JUMP",-20},
		{"BUTTON WILL CAUSE YOUR MAN",-20},
		{"TO JUMP UP AND SWAT THE BALL",-20},
		{"DOWN INTO THE GROUND!",0}
	};

	static coach_msg_t hint27_txt[] =
	{
		{"FIELD GOALS",-50},
		{"THE LINE OF SCRIMMAGE MUST",-20},
		{"BE IN ENEMY TERRITORY BEFORE",-20},
		{"A FIELD GOAL CAN BE ATTEMPTED",0}
	};

	static coach_msg_t hint28_txt[] =
	{
		{"LATERAL BALL",-50},
		{"WHEN CARRYING THE BALL, AND A",-20},
		{"TEAMMATE IS NEAR, PRESS THE",-20},
		{"PASS BUTTON TO LATERAL THE",-20},
		{"BALL TO HIM.",-20},
		{"WORKS DOWNFIELD ALSO!",0}
	};

	static coach_msg_t hint29_txt[] =
	{
		{"STOP CLOCK",-50},
		{"WHEN CARRYING THE BALL, JUMP",-20},
		{"OUT OF BOUNDS TO STOP THE",-20},
		{"CLOCK.  YOU CAN NOT RUN OUT",-20},
		{"OF BOUNDS!",0}
	};

	static coach_msg_t hint30_txt[] = {
		{"AUDIBLES", -50},
		{"DOUBLE TAP TURBO TO SHOW", -20},
		{"AUDIBLE MENU. PUSH STICK", -20},
		{"UP OR DOWN TO CHANGE YOUR", -20},
		{"PLAY AT THE LINE.", -20},
		{"CREATE A PLAY ALLOWS YOU", -20},
		{"TO PERSONALIZE THE AUDIBLE PLAYS.", 0},

	};

	static coach_msg_t *hint_list[] =
	{
		hint1_txt,
		hint2_txt,
		hint3_txt,
		hint4_txt,
		hint5_txt,
		hint6_txt,
		hint7_txt,
		hint8_txt,
		hint9_txt,
		hint10_txt,
		hint11_txt,
		hint12_txt,
		hint13_txt,
		hint14_txt,
		hint15_txt,
		hint16_txt,
		hint17_txt,
		hint18_txt,
		hint19_txt,
		hint20_txt,
		hint21_txt,
		hint22_txt,
		hint23_txt,
		hint24_txt,
		hint25_txt,
		hint26_txt,
		hint27_txt,
		hint28_txt,
		hint29_txt,
		hint30_txt
	};

	static image_info_t *cheer_imgs[] =
	{
		&cheerpg1,&cheerpg1_c1,&cheerpg1_c2,&cheerpg1_c3,
		&cheerpg2,&cheerpg2_c1,&cheerpg2_c2,&cheerpg2_c3,
		&cheerpg3,&cheerpg3_c1,&cheerpg3_c2,&cheerpg3_c3,
		&cheerpg4,&cheerpg4_c1,&cheerpg4_c2,&cheerpg4_c3,
		&cheerpg5,&cheerpg5_c1,&cheerpg5_c2,&cheerpg5_c3,
		&cheerpg6,&cheerpg6_c1,&cheerpg6_c2,&cheerpg6_c3,
		&cheerpg7,&cheerpg7_c1,&cheerpg7_c2,&cheerpg7_c3
	};

	// Stop crowd
	snd_stop_track(SND_TRACK_1);
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME1,127,PRIORITY_LVL4);

	// Play drum loop under coaching tips screen if overtime
//	snd_scall_bank(generic_bnk_str, 16, VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK0);
//	snd_scall_bank(generic_bnk_str, 17, VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK0);

	// CHANGE THIS IF THE NUMBER OF CHEERLEADER PICTURES CHANGES!!!
	i = randrng(7);
	i = i * 4;

	// piece 1 of background
	bkgrnd_obj = beginobj(cheer_imgs[i], 0, SPRITE_VRES, 4.0f, BKGRND_TID);
	bkgrnd_obj->id = 1;

	// piece 2 of background
	bkgrnd_obj = beginobj(cheer_imgs[i+1], 0, SPRITE_VRES, 4.0f, BKGRND_TID);
	bkgrnd_obj->id = 1;

	// piece 3 of background
	bkgrnd_obj = beginobj(cheer_imgs[i+2], 0, SPRITE_VRES, 4.0f, BKGRND_TID);
	bkgrnd_obj->id = 1;

	// piece 4 of background
	bkgrnd_obj = beginobj(cheer_imgs[i+3], 0, SPRITE_VRES, 4.0f, BKGRND_TID);
	bkgrnd_obj->id = 1;

	// title bar
	bkgrnd_obj = beginobj(&strattips, 4.0f, SPRITE_VRES, 2.5f, BKGRND_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj = beginobj(&strattips_c1, 4.0f, SPRITE_VRES, 2.5f, BKGRND_TID);
	bkgrnd_obj->id = 1;

	phint = hint_list[randrng(sizeof(hint_list)/sizeof(void *))];		// want to be rndper
	ypos = SPRITE_VRES - 100;

	set_string_id(0);

	i = 0;
	do {
		set_text_position(SPRITE_HRES/2.0f-90, ypos, 1.1F);
		if (!i)
		{
			set_text_color(LT_RED);
			if (is_low_res)
				set_text_font(FONTID_BAST23);
			else	
				set_text_font(FONTID_BAST18);

			f_msg = oprintf("%dj%s", (HJUST_CENTER|VJUST_BOTTOM), phint->msg);
			flash = qcreate("flash", MESSAGE_PID, flash_text_slow_proc, (int)f_msg, 0, 0, 0);

			set_text_color(WHITE);
			if (is_low_res)
				set_text_font(FONTID_BAST10LOW);
			else	
				set_text_font(FONTID_BAST10);
		}
		else
		{
			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_BOTTOM), WHITE , phint[i].msg);
		}
		ypos += phint[i].yoff;
	}	while (phint[i++].yoff);


	// While hint is flashing on screen, load up next tune
	load_tunes();
	// Won't return here until banks are loaded

	// Take away the plates
	iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Be sure they go away (try anyway)
	sleep(PLATE_TRANSITION_TICKS);
	plates_on = PLATES_OFF;

	wait_for_any_butn_press(20,tsec*10);

	// Turn on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way so we can sync-up the sound
	while (plates_on != PLATES_ON && --i) sleep(1);
	// Do sound only if the transition finished, not the time-out
	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

	// do clean-up
	kill(flash, 0);
	delete_multiple_strings(0, 0xffffffff);
	del1c(1,0xFFFFFFFF);							// delete background
}

//-------------------------------------------------------------------------------------------------------------------------
//							This process displays some effect when the player buys a full game.
//
//   INPUT: player number
// RETURNS: Nothing
//-------------------------------------------------------------------------------------------------------------------------
void fullgame_bought_msg(int *args)
{
	sprite_info_t	*fg_plq_obj;
	sprite_info_t	*flsh_obj;
	int p_num, i;

	while (!fullgame_bought_ok)
		sleep(1);

	p_num = args[0];
	snd_scall_bank(cursor_bnk_str,FULLGM_APPEAR_SND,VOLUME5,127,PRIORITY_LVL3);

	fg_plq_obj = beginobj(&japl_fg,
		station_xys[p_num][four_plr_ver][X_VAL],
		station_xys[p_num][four_plr_ver][Y_VAL]-64.0F,
		1.18f - 0.005f * (float)p_num,
		JAPPLE_BOX_TID);
	fg_plq_obj->w_scale = 2.2f;
	fg_plq_obj->h_scale = 2.2f;
	fg_plq_obj->id = OID_JAPPLE_BOX;

	while (fg_plq_obj->w_scale > 1.0F)
	{
		fg_plq_obj->w_scale -= .10f;
		fg_plq_obj->h_scale -= .10f;
		generate_sprite_verts(fg_plq_obj);
		sleep(2);
	}

	snd_scall_bank(cursor_bnk_str,FULLGM_FLASH_SND,VOLUME3,127,PRIORITY_LVL3);

//	flsh_obj = beginobj(lighting_imgs[0], japbx_cntr_xys[p_num][four_plr_ver][X_VAL], japbx_cntr_xys[p_num][four_plr_ver][Y_VAL]+2.0f, 1.15F, JAPPLE_BOX_TID);
	flsh_obj = beginobj(stat_flsh_imgs[0],
		japbx_cntr_xys[p_num][four_plr_ver][X_VAL],
		japbx_cntr_xys[p_num][four_plr_ver][Y_VAL]+2.0f,
		1.15f - 0.005f * (float)p_num,
		JAPPLE_BOX_TID);
	flsh_obj->w_scale = 2.4f;
	flsh_obj->h_scale = 4.2f;
	flsh_obj->id = OID_JAPPLE_BOX;
	generate_sprite_verts(flsh_obj);

//	for (i=0; i < (sizeof(lighting_imgs)/sizeof(image_info_t *))-1; i++)
	for (i=0; i < (sizeof(stat_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(flsh_obj,stat_flsh_imgs[i],JAPPLE_BOX_TID);
//		change_img_tmu(flsh_obj,lighting_imgs[i],JAPPLE_BOX_TID);
		sleep(3);
	}
	delobj(flsh_obj);
	sleep(60);
	delobj(fg_plq_obj);
}

//-------------------------------------------------------------------------------------------------------------------------
//				This process updpdates the credits message on the botton of the screen
//-------------------------------------------------------------------------------------------------------------------------
void credit_msg(int *args)
{
	sprite_info_t * credit_box_obj;
	float cntr_x;
	float cntr_y;
	int was_low_res = is_low_res;
	int old_creds = -1;
	int cred = coin_check_freeplay();

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	credit_box_obj = beginobj((cred ? &freeplay : &credbox),
		(SPRITE_HRES/2.0f),
		(was_low_res ? (SPRITE_VRES/2.0f-3.0f) : (SPRITE_VRES/2.0f)),
		1.1f,
		args[1]);
	credit_box_obj->id = OID_JAPPLE_BOX;

	// Back it goes!!!
	is_low_res = was_low_res;

	if (cred) die(0);

//	cntr_x = credit_msg_xys[args[0]][!!was_low_res][X_VAL];
//	cntr_y = credit_msg_xys[args[0]][!!was_low_res][Y_VAL];

	cntr_x = CREDIT_MSG_X;
	cntr_y = credit_msg_ys[!!was_low_res];

	while (1)
	{
		if (old_creds != (cred = get_total_credits()))
		{
			old_creds = cred;
			delete_multiple_strings(CREDIT_MSG_ID, -1);
			set_string_id(CREDIT_MSG_ID);
			set_text_font(FONTID_BAST7T);
			set_text_color(LT_YELLOW);
			set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);
			set_text_position(cntr_x, cntr_y, 1.1f);
			oprintf("%d", old_creds);
		}
		sleep(3);
	}
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific button bits
//
// INPUT : player number
// RETURN: players buttons bits
//-------------------------------------------------------------------------------------------------
int	get_but_val_down(int pnum)
{
 int sw;

	sw = get_player_sw_close();
	sw >>=((pnum<<3)+4);
	return(sw & 0x7);		//ignore D button
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific button bits
//
// INPUT : player number
// RETURN: players buttons bits
//-------------------------------------------------------------------------------------------------
int	get_but_val_cur(int pnum)
{
 int sw;

	sw = get_player_sw_current();
	sw >>=((pnum<<3)+4);
	return(sw & 0x7);		// ignore D button
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific joystick bits
//
// INPUT : player number
// RETURN: players joystick bits
//-------------------------------------------------------------------------------------------------
int	get_joy_val_down(int pnum)
{
 int sw;

	sw = get_player_sw_close();
	sw >>=(pnum<<3);
	return(sw & 0x0f);
}

//-------------------------------------------------------------------------------------------------
//									This routine returns the player specific joystick bits
//
// INPUT : player number
// RETURN: players joystick bits
//-------------------------------------------------------------------------------------------------
int	get_joy_val_cur(int pnum)
{
 int sw;

	sw = get_player_sw_current();
	sw >>=(pnum<<3);
	return(sw & 0x0f);
}

//-------------------------------------------------------------------------------------------------
//
// This function waits for a button press from player
//
// INPUT:  player #, Time to sleep, minimum time to sleep before returning
// OUTPUT: Nothing
//
//-------------------------------------------------------------------------------------------------
void wait_for_button_press(int plyr_num, int min_ticks, int wait_ticks)

{
	while (--min_ticks)
		sleep (1);

	while (--wait_ticks)
	{
		sleep (1);
		if (get_but_val_down(plyr_num))
			break;
	}
}

//-------------------------------------------------------------------------------------------------
//    This process shows the "get ready for x quarter" message at the top of the buy in page
//
//
//		INPUT: Nothing
//-------------------------------------------------------------------------------------------------
void get_ready_for(void)
{
	float	ypos;
	static ostring_t *pmsg[4];
	static sprite_info_t *idiot_bx_obj;

	// Backdrop for get ready message
	ypos = SPRITE_VRES * 0.90f - 3.0f;
	idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2, ypos-15.0f, 2.4f, SCR_PLATE_TID);
	idiot_bx_obj->w_scale = 1.00f;
	idiot_bx_obj->h_scale = 1.00f;
	idiot_bx_obj->id = OID_MESSAGE;
	generate_sprite_verts(idiot_bx_obj);

	set_text_position((SPRITE_HRES/2), ypos, 2.2f);
	set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
	set_text_color(LT_GREEN);
	if (is_low_res)
		set_text_font(FONTID_BAST23);
	else	
		set_text_font(FONTID_BAST18);
	set_string_id(CREDIT_MSG_ID + 1);

	pmsg[0] = oprintf("GET READY FOR");

	ypos -= ((float)get_font_height(FONTID_BAST25) + 0.0f);
	if (is_low_res)
		set_text_position((SPRITE_HRES / 2.0f), ypos+2-14, 2.2f);
	else
		set_text_position((SPRITE_HRES / 2.0f), ypos+2, 2.2f);
		
	pmsg[1] = oprintf("%s",period_names[game_info.game_quarter-1]);
	pflasher = qcreate("pflashD", MESSAGE_PID, flash_text_proc, (int)pmsg[0], (int)pmsg[1], 0, 0);
}

//-------------------------------------------------------------------------------------------------
//	  This process show the "join in" and/or "continue" msg
//		for players who aren't in the game.
//
//  	INPUT: player number,texture id,0,0
//-------------------------------------------------------------------------------------------------
void challenger_needed_msg(int *args)
{
ostring_t * pmsg[] = {0, 0, 0};
int pnum,cntr_x,cntr_y,i,tid;
sprite_info_t		*bck_obj;
sprite_info_t		*msg_obj;
int					free_quarter;
float				xpos, ypos;

	pnum = args[0];
	tid = args[1];
	free_quarter = args[2];

	// !!!FIX or delete
//	if (p_status & (1<<pnum))															// does player have any QUARTERS purchased ?
//		die(0);																								// yes, no challngr msg

	// player isn't in game
	cntr_x = station_xys[pnum][four_plr_ver][X_VAL];
	cntr_y = station_xys[pnum][four_plr_ver][Y_VAL]+35;

	bck_obj = beginobj(&mblank, cntr_x, cntr_y, 4.3F, tid);
	bck_obj->id = OID_CHLNGR_BOX + pnum;
	msg_obj = beginobj(&mpresst, cntr_x, cntr_y, 4.2F, tid);
	msg_obj->id = OID_CHLNGR_BOX + pnum;

	while ( ((!(p_status & (1<<pnum))) && (!(teammate_in_tmsel(pnum)))) || (game_info.game_state == GS_EOQ_BUY_IN))
	{
		// player has already bought this qtr. just tell him to get ready!
		if (p_status & (1<<pnum))
		{
			change_img_tmu(msg_obj,&mgetread,tid);

			// Also tell him if he won a free quarter
			if(free_quarter & (1<<pnum))
			{
				i = !four_plr_ver ? (!pnum?1:3) : (pnum<2?1:3);

				xpos = (SPRITE_HRES / 4.0f) * (float)i;
				ypos = (SPRITE_VRES * 0.90f);
				set_text_position(xpos, ypos, 1.1f);
				set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
				set_text_color(LT_GREEN);
				set_text_font(FONTID_BAST18);
				set_string_id(CREDIT_MSG_ID + 1);

				i = 0;
				if (four_plr_ver &&
						((pnum == 1 && free_quarter & (1<<0)) ||
						 (pnum == 3 && free_quarter & (1<<2))))
					i = 1;

				if (!i) pmsg[0] = oprintf("CONGRATULATIONS");

				ypos -= ((float)get_font_height(FONTID_BAST18) + 3.0f) * (float)(i+1);
				if (is_low_res) ypos -= !i?5.0f:8.0f;
				set_text_position(xpos, ypos, 1.1f);
				pmsg[1] = oprintf("PLAYER %d", pnum + 1);

				ypos -= (float)get_font_height(FONTID_BAST18) + 3.0f;
				ypos -= (float)get_font_height(FONTID_BAST18) + 3.0f;
				if (is_low_res) ypos -= 3.0f;
				set_text_font(FONTID_BAST10);
				set_text_position(xpos, ypos, 1.1f);
				if (!i) pmsg[2] = oprintf("YOUR NEXT PERIOD IS FREE");

				qcreate("pflash",BONUS_PID, purchase_flasher, (int)pmsg[0], (int)pmsg[1], (int)pmsg[2], 5 );

				// don't wanna do this again!
				free_quarter = 0;
			}
		}
		else																								// player hasn't bought any more quarters.
		{
			if (coin_check_freeplay())
				change_img_tmu(msg_obj,&mfreepl,tid);						// game in FREE PLAY -- just show freeplay msg
	
			else if (p_status2 & (1<<pnum))										// was player in last qtr. ?
			{																									// yes (continue state)
				if (check_credits_to_continue())
					change_img_tmu(msg_obj,&mpresst,tid);
				else
					change_img_tmu(msg_obj,&minsert,tid);
			}
			else																							// player wasn't in last qtr. (new start state)
			{
				if (check_credits_to_start())
					change_img_tmu(msg_obj,&mpresst,tid);
				else
					change_img_tmu(msg_obj,&minsert,tid);
			}
		}

		// show first msg for a bit
		for (i=0; i<25; i++)
		{
			if (teammate_in_tmsel(pnum))				// used only if 4 player version
				break;
			if ((p_status & (1<<pnum)) && (game_info.game_state != GS_EOQ_BUY_IN))
				break;
			sleep(2);
		}

		//
		// toggle message
		//
		if (!(p_status & (1<<pnum)))
		{																										// player hasn't bought this qtr.
			if (coin_check_freeplay())
				change_img_tmu(msg_obj,&mpresst,tid);						// game in FREE PLAY -- show 'press start' msg
	
			else if (p_status2 & (1<<pnum))										// was player in last qtr. ?
				change_img_tmu(msg_obj,&mcontin,tid);
	
			else																							// player wasn't in last qtr. (new start state)
				change_img_tmu(msg_obj,&mjoinin,tid);
		}

		// show second msg for a bit
		for (i=0; i<25; i++)
		{
			if (teammate_in_tmsel(pnum))				// used only if 4 player version
				break;
			if ((p_status & (1<<pnum)) && (game_info.game_state != GS_EOQ_BUY_IN))
				break;
			sleep(2);
		}
	}
	killall( BONUS_PID, -1 );
	del1c( OID_CHLNGR_BOX + pnum, -1 );
	delete_multiple_strings( CREDIT_MSG_ID + 1, -1 );
}

//-------------------------------------------------------------------------------------------------------------------------
// Just returns 1 if teammate is in game else 0
//-------------------------------------------------------------------------------------------------------------------------
int teammate_in_tmsel(int p_num)
{
	if (screen != TM_SELECT_SCRN)
		return(0);

	if (four_plr_ver)
	{
		if ((p_num == 0) && (p_status & (1<<1)))
			return(1);											// teammate
		else if ((p_num == 1) && (p_status & (1<<0)))
			return(1);											// teammate
		else if ((p_num == 2) && (p_status & (1<<3)))
			return(1);											// teammate
		else if ((p_num == 3) && (p_status & (1<<2)))
			return(1);											// teammate
	}
	return(0);													// no teammate
}

//-------------------------------------------------------------------------------------------------------------------------
//    This process updates/shows the PLAYERS credits (quarters) bought
//
//	This ASSUMES CREDIT TEXTURES are in TMU
//
//  INPUT: player number,cntr X,cntr Y,0
//-------------------------------------------------------------------------------------------------------------------------
void plyr_credit_box(int *args)
{
int    				pnum,tid;
int	   				gq,qp,oqp;
pdata_t				*pdata;
sprite_info_t		*credit_mrkr_obj;
sprite_info_t		*plr_namebx_obj;
//sprite_info_t		*plr_bx_shdw_obj;
image_info_t		*img;
float				x,y;

	pnum = args[0];
	x = args[1];
	y = args[2];
	tid = args[3];
	pdata = pdata_blocks + pnum;

	// drop shadow for box
//	plr_bx_shdw_obj = beginobj(&japlsh_4, args[1], args[2], 1.4F, tid);
//	plr_bx_shdw_obj->id = OID_JAPPLE_BOX;

//!!!	// name entry box
//!!!	if ((pnum == 0) && (four_plr_ver))
//!!!		x += 4.0f;
//!!!	if ((pnum == 3) && (four_plr_ver))
//!!!		x -= 4.0f;

//	plr_namebx_obj = beginobj(&japlbox, args[1], args[2], 1.3F, tid);
	plr_namebx_obj = beginobj(&jplplq_l, x, y, 1.3F, tid);
	plr_namebx_obj->id = OID_JAPPLE_BOX;

	gq = game_info.game_quarter;
	if (gq >= NUM_QUARTERS)
		gq = NUM_QUARTERS-1;

	// little green footballs
	if (game_info.game_state == GS_GAME_OVER || (game_info.game_state == GS_EOQ_BUY_IN && game_info.game_mode == GM_GAME_OVER))
		credit_mrkr_obj = beginobj(&japl____, x, y, 1.25F, tid);
	else	
		credit_mrkr_obj = beginobj(credit_box_imgs[gq][pdata->quarters_purchased], x, y, 1.25F, tid);
	credit_mrkr_obj->id = OID_JAPPLE_BOX;

	oqp = pdata->quarters_purchased;

	while (1)
	{
		qp = pdata->quarters_purchased;
		if (oqp != qp)
		{
			oqp = qp;
//			gq = game_info.game_quarter;
//			if (gq >= NUM_QUARTERS)
//				gq = NUM_QUARTERS-1;

			// change image for credit box if needed
			img = credit_box_imgs[gq][qp];

			if (credit_mrkr_obj->ii != img)
			{
				change_img_tmu(credit_mrkr_obj,img,tid);
//				credit_mrkr_obj->ii = img;
//				generate_sprite_verts(credit_mrkr_obj);
			}
	
			qcreate("flshcrdt",CREDIT_PID,flash_credit_football,pnum,tid,0,0);
	
		}
		sleep(1);
	}
	delobj(credit_mrkr_obj);
	delobj(plr_namebx_obj);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void flash_credit_football(int *args)
{
sprite_info_t 		*burst_obj;
pdata_t				*pdata;
int					i, qp, gq, tid, pnum;
float				zs[] = {1.200f, 1.201f, 1.202f, 1.203f, 1.204f, 1.205f, 1.206f};

	pnum = args[0];
	tid = args[1];
	pdata = pdata_blocks + pnum;

	gq = game_info.game_quarter;
	if (gq >= NUM_QUARTERS)
		gq = NUM_QUARTERS-1;

	qp = pdata->quarters_purchased + gq;

	burst_obj = beginobj(burst_flsh_imgs[0], credit_fb_flsh_xys[four_plr_ver][pnum][qp][X_VAL], credit_fb_flsh_xys[four_plr_ver][pnum][qp][Y_VAL], zs[qp], tid);
	burst_obj->w_scale = 2.2f;
	burst_obj->h_scale = 2.2f;
	burst_obj->id = OID_JAPPLE_BOX;		// Survive del1c
	burst_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	generate_sprite_verts(burst_obj);

	for (i=0; i < (sizeof(burst_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(burst_obj,burst_flsh_imgs[i],tid);
		sleep(2);
	}
	delobj(burst_obj);
}

//-------------------------------------------------------------------------------------------------------------------------
//			This process display a startburst over the RANK award plaques
//
// INPUT: x,y, obj id
//-------------------------------------------------------------------------------------------------------------------------
#if 0
void flash_rank_plq(int *args)
{
sprite_info_t 	*burst_obj;
int	i;

//	snd_scall_bank(cursor_bnk_str,STAT_AWARD_SND,VOLUME2,127,PRIORITY_LVL1);
	snd_scall_bank(cursor_bnk_str,STAT_AWARD_SND,VOLUME2,127,LVL3);

	burst_obj = beginobj(burst_flsh_imgs[0], args[0], args[1], 12.0F, JAPPLE_BOX_TID);
	burst_obj->w_scale = 2.5f;
	burst_obj->h_scale = 3.5f;
	burst_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	burst_obj->id = args[2];
	generate_sprite_verts(burst_obj);

	for (i=0; i < (sizeof(burst_flsh_imgs)/sizeof(image_info_t *))-1; i++)
	{
		change_img_tmu(burst_obj,burst_flsh_imgs[i],JAPPLE_BOX_TID);
		sleep(2);
	}
	delobj(burst_obj);
}
#endif

//-------------------------------------------------------------------------------------------------------------------------
//    				This process wait X ticks then counts down from 9 to 0
//
//  INPUT: ticks b4 cnt dwn from SECONDS, seconds, x, y
//-------------------------------------------------------------------------------------------------------------------------
void timedown(int *args)
{
	int		wait_secs,old_credits;
	int		seconds,i;
	char	old_pstatus;
	float	x,y;


	force_selection = 0;

	x = args[2];
	y = args[3];

plyradd:

//	fprintf(stderr,"screen: %d\r\n",screen);
//	fflush(stdout);

	old_credits = get_credits();

	seconds = args[1];
	wait_secs = args[0];
	old_pstatus = p_status;

	// delete any prevoius timer
	delete_multiple_strings(TIMER_MSG_ID, 0xffffffff);

	while (wait_secs)
	{
		wait_secs--;
		sleep(tsec);
	}

	while (seconds >= 0)
	{
		// show timer number
		set_string_id(TIMER_MSG_ID);
		set_text_position(x, y, 1.1F);

		set_text_font(FONTID_BAST25);

		pmsg[0] = oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_RED, seconds);

		// play time down sound
		snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME4,127,PRIORITY_LVL3);
//		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME1,127,PRIORITY_LVL1);

		// !!!FIX   if PSTATUS changes..goto plyradd:
		if (old_pstatus != p_status)
			goto plyradd;

		// show number
		if((screen == BUY_IN_SCRN) && (get_player_sw_current() & ALL_ABCD_MASK))
		{
			// If game is in overtime, don't allow quick whack out for first few seconds
			if ((game_info.game_quarter >= 4) && (seconds >= 9))
				sleep(60);
			else
				sleep(3);
		}
		else if(screen == BUY_IN_SCRN)
		{
			for(i = 0; i < tsec; i++)
			{
				if((get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) && !check_credits_to_continue())
				{
					seconds = args[1];
					// delete number
		  			delete_multiple_strings(TIMER_MSG_ID, 0xffffffff);
//					snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME2,127,PRIORITY_LVL3);
					// show timer number
					set_string_id(TIMER_MSG_ID);
					set_text_position(x, y, 1.5F);

					set_text_font(FONTID_BAST25);
			
					pmsg[0] = oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, seconds);
//					pmsg[0] = oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_RED, seconds);
					sleep(30);
					break;
				}
				else
				{
					if((screen == BUY_IN_SCRN) && (get_player_sw_current() & ALL_ABCD_MASK))
					{
						if (!(game_info.game_quarter >= 4 && !p_status))
							i = tsec;
					}	
				}
				sleep(1);
			}
		}
		else
		{
			for(i = 0; i < tsec; i++)
			{
				if((screen == BUY_IN_SCRN) && (get_player_sw_current() & ALL_ABCD_MASK))
					i = tsec;
				sleep(1);
			}
		}

		// delete number
		delete_multiple_strings(TIMER_MSG_ID, 0xffffffff);
		snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME2,127,PRIORITY_LVL3);
//		snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME1,127,PRIORITY_LVL3);

		// !!!FIX   if PSTATUUS changes..goto plyradd:
		if (old_pstatus != p_status)
			goto plyradd;

		// show no number
		if ((screen == BUY_IN_SCRN) && (get_player_sw_current() & ALL_ABCD_MASK))
		{
			sleep(3);
		}
		else if(screen == BUY_IN_SCRN)
		{
			for(i = 0; i < 25; i++)
			{
				if((get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) && !check_credits_to_continue())
				{
					seconds = args[1] + 1;
					break;
				}
				sleep(1);
			}
		}
		else
		{
			sleep(25);
		}

		// If a coin has been dropped on buyin screen, reset timer
		if ((screen == BUY_IN_SCRN) && (old_credits != get_credits()))
		{
			old_credits = get_credits();
			goto plyradd;
		}

		// 1 less second
		seconds--;

		// number went negative
		if (seconds > args[1])
			break;
  }

	if (!(screen == BUY_IN_SCRN) && (get_player_sw_current() & ALL_ABCD_MASK))
		sleep(25);																// a little grace period

	force_selection = 1;											// forces a button press

	// delete number
	delete_multiple_strings(TIMER_MSG_ID, 0xffffffff);
}



//-------------------------------------------------------------------------------------------------------------------------
//	This process wait X ticks then counts down from 9 to 0
//	This is used on the play select screen
//
//  INPUT: seconds, x, y
//-------------------------------------------------------------------------------------------------------------------------
void timedown_play_select(int *args)
{
  int seconds;
	float	x,y;

	force_selection = 0;

	x = args[1];
	y = args[2];

	seconds = args[0];

	while (seconds >= 0)
	{
		// show timer number
		set_string_id(TIMER_MSG_ID);
		set_text_position(x, y, 1.0F);
		set_text_font(FONTID_BAST18);
		set_text_color(LT_RED);

		pmsg[0] = oprintf("%dj%d", (HJUST_CENTER|VJUST_CENTER), seconds);
		pflasher = qcreate("pflashE", MESSAGE_PID, flash_text_proc, (int)pmsg[0], 0, 0, 0);

		snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME4,127,PRIORITY_LVL3);

		// Is idiot box up for play select scrn?
		while (show_plysel_idiot_box >= 9)
			sleep(1);

		if (seconds)
			sleep(tsec);
		else
		// a little grace period
		sleep(tsec+25);

		// delete number
		delete_multiple_strings(TIMER_MSG_ID, 0xffffffff);
		kill(pflasher, 0);

		// 1 less second
		seconds--;
  }
	force_selection = 1;											// forces a button press
}

//-------------------------------------------------------------------------------------------------------------------------
//    		This process wait X ticks then counts down from 9 to 0 (using 3d objects)
//
//  INPUT: ticks before count down from SECONDS, seconds, x, y
//-------------------------------------------------------------------------------------------------------------------------
#if 0
void timedown3d(int *args)
{

	int tmdn_delay;
	int tmdn_ticks;
	int	old_pstatus;
	struct texture_node	*pnode;
	obj3d_data_t	*p3dnum = &nbr_3d_obj;

	force_selection = 0;

	// Load the number texture
	pnode = create_texture("numtx_3d.wms", NUMBER_3D_TID, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
	if (pnode == NULL)
	{
#ifdef DEBUG
		fprintf(stderr, "Error loading 'numtx_3d.wms'\r\n");
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
		force_selection = 1;											// forces a button press
		die(0);
	}

	while (!force_selection)
	{
		old_pstatus = p_status;
		tmdn_delay = args[0] * tsec;
		tmdn_ticks = args[1] * tsec;
		while ((tmdn_delay != 0) || (tmdn_ticks > 0))
		{
			if (old_pstatus != p_status)
			{
				// timer gets reset...
				old_pstatus = p_status;
				tmdn_delay = args[0] * tsec;
				tmdn_ticks = args[1] * tsec;
				delete_multiple_objects( OID_NBR3D, ~0xFF );
				killall(TIMER_PID,0xFFFFFFFF);										//	kill 'timedown3d' process and/or 'num3d_proc'
			}
			if (!tmdn_delay)
			{
				tmdn_ticks--;
				if (!(tmdn_ticks % tsec))
				{
//					snd_scall_bank(cursor_bnk_str,TIMEDOWN_SND,VOLUME1,127,PRIORITY_LVL3);
					snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME4,127,PRIORITY_LVL3);
					// switch limb (numbers)
					p3dnum->limb_obj = nbr_3d_limbs[tmdn_ticks/tsec];
					p3dnum->odata.z = 1.25f;
					p3dnum->odata.fwd = 0;
				}
			}
			else
			{
				tmdn_delay--;
				if (tmdn_delay == 0)
				{
					p3dnum->decal = pnode->texture_handle;
					p3dnum->ratio = pnode->texture_info.header.aspect_ratio;
					p3dnum->limb_obj = nbr_3d_limbs[args[1]];
					p3dnum->odata.x = 0.0f;
					p3dnum->odata.y = 0.0f;
					p3dnum->odata.z = 1.25f;			// starting Z
					p3dnum->odata.fwd = 0;
					create_object("nbr3d", OID_NBR3D, OF_NONE, 0, (void *)p3dnum, nbr3d_draw_function);
					qcreate("rotnum",TIMER_PID,num3d_proc,0,0,0,0);
				}
			}
			sleep(1);
		}
		sleep(60);																// a little grace period for the '0'
		force_selection = 1;											// forces a button press
	}

	// delete number object
	delete_multiple_objects( OID_NBR3D, ~0xFF );
	killall(TIMER_PID,0xFFFFFFFF);										//	kill 'timedown3d' process and/or 'num3d_proc'
}
#endif

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
#if 0
void num3d_proc(int *args)
{


	obj_3d	*p3dnum;

	p3dnum = (obj_3d *)(&nbr_3d_obj);
	while(1)
	{
		if (p3dnum->z >= 5.0f)
		{
			p3dnum->z = 5.0f;
			p3dnum->fwd = 511;
		}
		else
		{
			p3dnum->z += .20f;		// .20
			p3dnum->fwd = 1023 & (p3dnum->fwd + 24);		//12
		}
		sleep(1);
	}
}
#endif

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
#if 0
void nbr3d_draw_function( void *oi )
{
	float	xlate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
	 		  			  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.0f };

	float	rotate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, 1.0f, 0.0f, 0.0f,
						   0.0f, 0.0f, 1.0f, 0.0f };

	float	cm[12];

	obj_3d	*pobj = (obj_3d *)oi;
	obj3d_data_t *pnum3d = (obj3d_data_t *)oi;

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthMask(FXTRUE);
	grChromakeyMode(GR_CHROMAKEY_DISABLE);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction(GR_BLEND_SRC_ALPHA,GR_BLEND_ONE_MINUS_SRC_ALPHA,GR_BLEND_SRC_ALPHA,GR_BLEND_ONE_MINUS_SRC_ALPHA);

	xlate[3] = pobj->x;
	xlate[7] = pobj->y;
	xlate[11] = pobj->z;

	roty(pobj->fwd,rotate);
	mxm(xlate, rotate, cm);

	render_limb(pnum3d->limb_obj, cm, &(pnum3d->decal), &(pnum3d->ratio));
}
#endif

//-------------------------------------------------------------------------------------------------
//								This routine checks to make sure the player doesnt enter a NAUGHTY word
//
//   INPUT: players current entered name
//  OUTPUT: 1 = naughty word, 0 = ok...so far
//-------------------------------------------------------------------------------------------------
int censor_players_name(char *name, int ec)
{
int			i,j;
char		new_str[LETTERS_IN_NAME] = {0,0,0,0,0,0};				// new str (name) w/o spaces

	// form new string from current without spaces
	i = 0;
	j = 0;
	while ((i < LETTERS_IN_NAME) && (i < ec))
	{
		if (name[i] != CH_SPC)
		{
			new_str[j] = name[i];
			j++;
		}
		i++;
	}

	// now see if the new string matches any censor strings
	for( i = 0; censor_name_tbl[i]; i++ )
	{
		if (strstr(name, censor_name_tbl[i]))
			return 1;
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------
//															This routines loads all textures in table into system ram
//
//   INPUT: ptr. to list of texture names
// RETURNS: 1 if load failed else 0
//-------------------------------------------------------------------------------------------------
//int load_textures_into_ram(sys_ram_texture_t *tex_tbl, int tex_id)
//	{
//	texture_node_t	*tex_node;
//
//	while (tex_tbl->t_name != NULL)
//		{
//		tex_node = texture_to_mem(tex_tbl->t_name,te_id,0,CREATE_NORMAL_TEXTURE);
//		if (!tex_node)								// texture loaded ?
//			return(1);									// no
//		tex_tbl->tn = tex_node;			// save texture node ptr.
//		tex_tbl++;
//		}
//	return(0);
//	}

//-------------------------------------------------------------------------------------------------
//															This routines loads all textures in table into TMU ram
//
//   INPUT: ptr. to list of texture names
// RETURNS: 1 if load failed else 0
//-------------------------------------------------------------------------------------------------
int load_textures_into_tmu(tmu_ram_texture_t *tex_tbl)
{
	texture_node_t	*tex_node;

	while (tex_tbl->t_name != NULL)
	{
		tex_node = create_texture(tex_tbl->t_name,
									tex_tbl->tex_id,
									0,
									CREATE_NORMAL_TEXTURE,
									GR_TEXTURECLAMP_CLAMP,
									GR_TEXTURECLAMP_CLAMP,
									GR_TEXTUREFILTER_BILINEAR,
									GR_TEXTUREFILTER_BILINEAR);

		if (!tex_node)
		{
			increment_audit(TEX_LOAD_FAIL_AUD);
			return(1);									// no
		}
		tex_tbl++;
	// TEMP
//		fprintf(stderr, "tex:%s  hndle:%d\r\n" ,tex_tbl->t_name,tex_node->texture_handle);
	}
	return(0);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void load_name_entry_textures(int *args)
{
	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info((void **)&cur_proc, suspend_self, resume_process);
#else
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

	imgs_loaded = 0;

	if (load_textures_into_tmu(name_entry_textures_tbl2))
	{
		fprintf(stderr, "Couldn't load all name-entry textures into tmu\r\n");
#ifdef DEBUG
		lockup();
#else
		return;
#endif
	}
	imgs_loaded = 1;

	// Disable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void load_tmsel_uniform(int team)
{
//	char			leghel[12], jersf[12], slvmsk[12], jnum[12];
	char			leghel[12];
	char			*files[2] = {leghel,NULL};
//	char			*files[5] = {jersf,leghel,slvmsk,jnum,NULL};
	int				ratios[4];
	int				i;


	if (uniforms_loaded & (1<<team))
		return;

	// Enable the disk interrupt callback
//	install_disk_info((void **)&cur_proc, suspend_self, resume_process);

	// ok, uniform not already loaded
	uniforms_loaded |= (1<<team);									// set bit of team loaded.
	sprintf( leghel,"%sH.WMS", teaminfo[team].prefix);
//	sprintf( leghel,"%sLGHLT.WMS", teaminfo[team].prefix);
//	sprintf( jersf, "%sJERFT.WMS", teaminfo[team].prefix);
//	sprintf( slvmsk,"%sSVMKT.WMS", teaminfo[team].prefix);
//	sprintf( jnum, 	"%sNUMT.WMS",  teaminfo[team].prefix);

	load_textures( files, ratios, all_teams_decals[team], 1, TXID_PLAYER,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	for (i=1; i<4; i++)
		all_teams_decals[team][i] = all_teams_decals[team][0];

//	load_textures( files, ratios, all_teams_decals[team], 4, TXID_PLAYER,
//		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	// Disable the disk interrupt callback
//	install_disk_info(0, 0, 0);
}

//-------------------------------------------------------------------------------------------------
//												This routine changes the IMAGE for a given sprite
//
//   INPUT: ptr. to sprite and image ptr.
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void change_img_tmu(sprite_info_t *obj, image_info_t *ii, int tid)
{
	struct texture_node	*tn;

	if (obj->ii != ii)
	{
		if (strncmp(obj->tn->texture_name,ii->texture_name,8))							// is img in current texture ?
		{
			tn = find_texture(ii->texture_name,tid);													// get new texture ptr
			if (!tn)																													// find it ?
			{																																	// no
				fprintf(stderr, "Couldn't find: %s in TMU\r\n", obj->tn->texture_name);
				fflush(stdout);
#ifdef DEBUG
				sleep(1);
				lockup();
#else
				return;
#endif
			}
			obj->tn = tn;
		}
		obj->ii = ii;
		generate_sprite_verts(obj);
	}
}

//---------------------------------------------------------------------------------------------------------------------------
// 	THIS creates the background as a TRUE BACKGROUND OBJECT where erase is turned off, thus the image erases the screen
//---------------------------------------------------------------------------------------------------------------------------
void draw_backdrop(void)
{
sprite_info_t		*bkgrnd_obj;


	// draw background
	bkgrnd_obj = beginobj(&selback,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj = beginobj(&selback_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	bkgrnd_obj->id = 1;

	bkgrnd_obj = beginobj(&selback,  		 SPRITE_HRES/2.0f, SPRITE_VRES, 500.0F, BKGRND_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->mode = FLIP_TEX_H;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj = beginobj(&selback_c1,       SPRITE_HRES/2.0f, SPRITE_VRES, 500.0F, BKGRND_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->mode = FLIP_TEX_H;
	generate_sprite_verts(bkgrnd_obj);
}

//---------------------------------------------------------------------------------------------------------------------------
// 	THIS creates the background as a TRUE BACKGROUND OBJECT where erase is turned off, thus the image erases the screen
// 	Flipped
//---------------------------------------------------------------------------------------------------------------------------
#if 0
void draw_backdrop_flip(void)
{
sprite_info_t		*bkgrnd_obj;

	bkgrnd_obj = beginbobj(&selback, 0.0F, SPRITE_VRES, 500.0F, JAPPLE_BOX_TID);
	bkgrnd_obj->w_scale = 2.0f;
	bkgrnd_obj->h_scale = 2.0f;
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj->mode = FLIP_TEX_H;
	generate_sprite_verts(bkgrnd_obj);
}
#endif
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
#if 0
void draw_backgrnd2(float z, int tid)
{
	sprite_info_t			*bkgrnd_obj;

	// top row
	bkgrnd_obj = beginobj(&selbck2, 0.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;			
	bkgrnd_obj->h_scale = 1.203125F;			
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj = beginobj(&selbck2, 154.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;			
	bkgrnd_obj->h_scale = 1.203125F;			
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj = beginobj(&selbck2, 308.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;			
	bkgrnd_obj->h_scale = 1.203125F;			
	bkgrnd_obj->id = 1;		
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj = beginobj(&selbck2, 462.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;			
	bkgrnd_obj->h_scale = 1.203125F;			
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);

	// bottom row
	bkgrnd_obj = beginobj(&selbck2_c1, 0.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;
	bkgrnd_obj->h_scale = 1.203125F;
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj = beginobj(&selbck2_c1, 154.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;
	bkgrnd_obj->h_scale = 1.203125F;
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj = beginobj(&selbck2_c1, 308.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;
	bkgrnd_obj->h_scale = 1.203125F;
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj = beginobj(&selbck2_c1, 462.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->w_scale = 1.203125F;
	bkgrnd_obj->h_scale = 1.203125F;
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	generate_sprite_verts(bkgrnd_obj);
}
#endif

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void show_final_player_stats(void)
{
	int		pnum, i;
	float	cntr_x;
	int		show_me = 0;

	// Are any players valid to show?
	for(pnum = 0; pnum < MAX_PLYRS; pnum++)
	{
		if ((plyr_data_ptrs[pnum]->plyr_name[0] != '\0') &&
			(plyr_data_ptrs[pnum]->plyr_name[0] != '-') &&
			(qtr_pstatus[pnum] == ((1 << game_info.game_quarter) - 1)))
		{
			show_me |= 1 << pnum;
		}
	}
	if (!show_me)
		return;		// NOPE - Do nothing

	draw_enable(0);
	wipeout();
	rec_sort_ram = NULL;
	get_all_records();

	draw_backdrop();

	qcreate("cred_msg",CREDIT_PID,credit_msg,screen,JAPPLE_BOX_TID,0,0);
	
	// Show stats for each player
	for(pnum = 0; pnum < MAX_PLYRS; pnum++)
	{
		if (show_me & (1 << pnum))
		{
			// show japple boxes
			qcreate("credbox", CREDIT_PID, plyr_credit_box,
				pnum,
				station_xys[pnum][four_plr_ver][X_VAL],
				station_xys[pnum][four_plr_ver][Y_VAL],
				JAPPLE_BOX_TID);
			print_players_name(pnum,
				japbx_cntr_xys[pnum][four_plr_ver][X_VAL],
				japbx_cntr_xys[pnum][four_plr_ver][Y_VAL],
				plyr_data_ptrs[pnum]->plyr_name,
				1);

			// print the stats
			cntr_x = station_xys[pnum][four_plr_ver][X_VAL];

			show_player_stats(pnum);
		}
	}

	draw_enable(1);
//	fade(0.0f, 1, NULL);
//	sleep(2);
//	fade(1.0f, 10, NULL);

	wait_for_any_butn_press(tsec,tsec*20);

	delete_multiple_strings(STAT_PG_TXT_ID+0, 0xFFFFFFFF);							// delete the stats
	delete_multiple_strings(STAT_PG_TXT_ID+1, 0xFFFFFFFF);							// delete the stats
	delete_multiple_strings(0, 0);
	del1c(OID_STAT_PG, 0xffffffff);
	killall(PLAQ_AWARD_PID, 0xffffffff);					// kill the plyr_paques_awards process
	killall(CREDIT_PID, 0xffffffff);					// kill the plyr_plaques_awards process
	if(rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}
	show_them_hiscore_tables_first = 1;

	// Turn on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way so we can sync-up the sound
	while (plates_on != PLATES_ON && --i) sleep(1);
	// Do sound only if the transition finished, not the time-out
//	if (i) snd_scall_bank(plyr_bnk_str, P_HARD_HIT2_SND, VOLUME4, 127, PRIORITY_LVL5);
	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

	// Disable drawing
	draw_enable(0);

	// do END-OF-GAME clean up
	wipeout();

	// Re-create the wipeout'd plate objects
	create_plates();

	// Enable drawing
	draw_enable(1);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void show_player_stats(int p_num)
{
	static float sps_xs[2][MAX_PLYRS] = {
		{ 128, 384,   0,   0 },
		{  64, 192, 320, 448 },
	};
	float			msg_spc;
	sprite_info_t 	*stat_obj;
	sprite_info_t 	*dshdw_obj;
	int				i;
	plyr_record_t	*_plr_rec = plyr_data_ptrs[p_num];
	plyr_record_t	*plr_rec;
	int				ver;
	float			cntr_x;			// center X pos. for players section


#ifdef DEBUG
	if (p_num > 3)
	{
		fprintf(stderr, "Bad p_num in show_player_stats!\r\n");
		lockup();
	}
#endif

	ver = four_plr_ver;
	cntr_x = station_xys[p_num][ver][X_VAL];

	// print stats
	msg_spc = 15.0F;

	// create STAT header image
	stat_obj = beginobj(&statpl_r, cntr_x, STAT_TITLE_Y, 20.0F, YES_NO_PLAQ_TID);
	stat_obj->id = OID_STAT_PG;

	// create Stats backdrop
	dshdw_obj = beginobj(&statfade, stats_bckdrp_xs[ver][p_num], STATS_BCKDRP_Y, 70.0F, YES_NO_PLAQ_TID);
	dshdw_obj->id = OID_STAT_PG;
	dshdw_obj->h_scale = 2.6f;
	dshdw_obj->w_scale = 1.8f;
	generate_sprite_verts(dshdw_obj);

	// create STATS headers (left column)
	for (i=0; i<NUM_STATS/2; i++)
	{
		stat_obj = beginobj(&sstatplr, sps_xs[ver][p_num], stat_plqs_xys[ver][i][p_num][Y_VAL], 50.0F, YES_NO_PLAQ_TID);
		stat_obj->id = OID_STAT_PG;
	}

	// create STATS headers (right column)
//	for (i=NUM_STATS/2; i<NUM_STATS; i++)
//	{
//		stat_obj = beginobj(&sstatplr, stat_plqs_xys[ver][i][p_num][X_VAL], stat_plqs_xys[ver][i][p_num][Y_VAL], 52.0F, YES_NO_PLAQ_TID);
//		stat_obj->id = OID_STAT_PG;
//	}

	i = find_record_in_ram(_plr_rec->plyr_name) - 1;
	_plr_rec = &rec_sort_ram[i];
	copy_record(&plrs_rec, _plr_rec);
	plr_rec = &plrs_rec;

	//
	// Print Stats:
	//
	set_text_font(FONTID_BAST10);
	set_string_id(STAT_PG_TXT_ID+p_num);

	// print OVERALL rank
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_wins);
	plr_ranks[p_num][OVR_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][0][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][0], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OVR_RNK], NUM_PLYR_RECORDS);

	// print OFFFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_scored);
	plr_ranks[p_num][OFF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc OFF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][1][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][1], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][OFF_RNK], NUM_PLYR_RECORDS);

	// print DEFENSIVE RANK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_allowed);
	plr_ranks[p_num][DEF_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc DEF. RANK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][2][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][2], 30.0F);
	oprintf("%dj%dc%d / %d", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_ranks[p_num][DEF_RNK], NUM_PLYR_RECORDS);

	// print WINSTREAK
	qsort(rec_sort_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_streaks);
	plr_ranks[p_num][STK_RNK] = find_record_in_ram(plr_rec->plyr_name);

	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc STREAK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][3][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][3], 30.0F);
	oprintf("%dj%dc %d WINS", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->winstreak);

	// print WIN-LOSS record
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][4], 30.0F);
	oprintf("%dj%dc RECORD", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][4][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][4], 30.0F);
	oprintf("%dj%dc %d W/ %d L", (HJUST_CENTER|VJUST_CENTER), WHITE, plr_rec->games_won, (plr_rec->games_played-plr_rec->games_won));

	// print TEAMS DEFEATED
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff1[ver][!!is_low_res][5], 30.0F);
	oprintf("%dj%dc DEFEATED", (HJUST_CENTER|VJUST_CENTER),  LT_YELLOW);
	set_text_position(sps_xs[ver][p_num], stat_plqs_xys[ver][5][p_num][Y_VAL]-stat_plqs_yoff2[ver][!!is_low_res][5], 30.0F);
	oprintf("%dj%dc %d / 30 TEAMS", (HJUST_CENTER|VJUST_CENTER), WHITE, calc_tms_def(plr_rec->teams_defeated));

	// Show player rank awards
//	qcreate("plrawrds",PLAQ_AWARD_PID,plyr_plaques_awards,p_num,0,0,0);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
int pick_cpu_team( int defeated )
{
	int			tier,i,r;
	int			unbeaten[4],nu=0;
	
	for (tier=0; (tier < 9) && !nu; tier++)
	{
		// has the guy defeated every team on this tier
		nu = 0;
		for (i=0; team_ladder[tier][i] != -1; i++ )
		{
			if (!(defeated & (1<<team_ladder[tier][i])))
				unbeaten[nu++] = team_ladder[tier][i];
		}
	}

	if (nu)
		// pick from the unbeaten teams on this tier
		r = unbeaten[randrng(nu)];
	else
		// beat all - pick from the list of super tough teams
		r = beat_all_teams[randrng(4)];
	
	return r;
}

//---------------------------------------------------------------------------------------------------------------------------
//	This routine unloads and loads sound banks at EOQ
//  At coaching tips screen, and halftime
//---------------------------------------------------------------------------------------------------------------------------
static void load_tunes(void)
{
#ifndef NOAUDIO
	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info((void **)&cur_proc, suspend_self, resume_process);
#else
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

	coin_sound_holdoff(TRUE);

//	snd_bank_delete(qtr_tunes_tbl[game_info.game_quarter-1]);
	snd_bank_delete(taunt_bnk_str);
	snd_bank_delete(ancr_swaped_bnk_str);

	strcpy(eoq_tune_bnk_str, eoq_tunes_strs[game_info.game_quarter-1]);
	if (snd_bank_load_playing(eoq_tune_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	coin_sound_holdoff(FALSE);

	// Control will not reach this point until texture is loaded into TMU!
	// Process will wake up and then the beginobj can happen

	// Disable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif
#endif
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
static void load_gameover_tunes(void)
{
#ifndef NOAUDIO
	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info((void **)&cur_proc, suspend_self, resume_process);
#else
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

	coin_sound_holdoff(TRUE);

	snd_bank_delete(taunt_bnk_str);
	snd_bank_delete(ancr_swaped_bnk_str);

	if (snd_bank_load_playing("gameover") == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	coin_sound_holdoff(FALSE);

	// Disable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif
#endif
}

//---------------------------------------------------------------------------------------
