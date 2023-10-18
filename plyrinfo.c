/******************************************************************************/
/*                                                                            */
/* plyrinfo.c  - Player info code (arrows, turbo, "insert", "press", etc.)    */
/*                                                                            */
/* Written by:  DJT                                                           */
/* $Revision: 407 $                                                           */
/*                                                                            */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                      */
/* All Rights Reserved                                                        */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "/Video/Nfl/Include/Anim.h"
#include "/Video/Nfl/Include/Game.h"
#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/coin.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/pproto.h"
#include "/Video/Nfl/Include/CamData.h"
#include "/Video/Nfl/Include/CamBot.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/PlaySel.h"
#include "/Video/Nfl/Include/PInfo.h"
#include "/Video/Nfl/Include/Select.h"
#include "/Video/Nfl/Include/Record.h"

#include "/Video/Nfl/Include/flash.h"

#define	IMAGEDEFINE	1
#include "/Video/Nfl/Include/plyrinfo.h"
#include "/Video/Nfl/Include/winmsg.h"

//#if (MAX_PLYRS == 4)
//#undef  MAX_PLYRS
//#define MAX_PLYRS 2
//#endif

#define	X_VAL	0
#define	Y_VAL	1
#define TRNSW_ADJ	(29.0f)				// Transparent shading width scale
//#define TRNSW_ADJ	(30.0f)				// Transparent shading width scale
#define TRNSH_ADJ	(21.0f/16.0f)		// Transparent shading height scale

/*
** referenced data & function prototypes
*/
extern font_info_t bast8t_font;

extern sprite_node_t * alpha_sprite_node_list;
extern sprite_node_t * sprite_node_list;

//extern struct texture_node * ptn_gndarrow;

extern void wait_for_any_butn_press_active_plyr(int, int, int);
extern void idiot_speech(void);
extern void crowd_cheer_snds(void);
extern void first_down_speech(void);
extern void intercepted_ball_sp(void);
extern void	safety_sp(void);
extern void field_goal_speech(void);

extern void incomplete_pass_sp(void);

extern image_info_t jplplq_l;


// Prototypes
void wait_for_any_butn_press_active_team(int, int, int);
void show_end_quarter(void);

/*
** global data & function prototypes
*/
ARROWINFO arrowinfo[MAX_PLYRS];
//ARROWINFO arrowinfo[2];

/*
** local data & function prototypes
*/
#define JOININ_TIMER	70					// ticks for info cycle
#define CREDIT_TIMER	120					// ticks for credits
#define COININ_TIMER	300					// ticks for new credits
#define CREDIT_CYCLE	4					// should always be even

#define	TBCRED_DIGCNT	3
#define	TBCRED_XOFF		24
#define	TBCRED_Z		1.1f

#define CREDSHOW_KILL(csid)	tbcred_hide(csid)
#define CREDSHOW_MAKE(csid)	tbcred_show(csid)

//#define CREDSHOW_FONT		set_text_font(FONTID_BAST8T)
//#define CREDSHOW_KILL(csid)	delete_multiple_strings(\
//								CREDIT_MSGS_ID + (int)(csid), 0xffffffff)
//#define CREDSHOW_MAKE(csid)	{\
//								set_string_id(\
//									CREDIT_MSGS_ID + (int)(csid));\
//								set_text_position(\
//									(float)(24+(int)pintx_turbo[four_plr_ver][(int)(csid)]),\
//									(float)(credit_msg_ys[!!is_low_res]),\
//									TBCRED_Z);\
//								{\
//									int ilr=is_low_res;\
//									is_low_res=0;\
//									oprintf("%dj%dc %d",\
//										(HJUST_CENTER | VJUST_BOTTOM),\
//										LT_YELLOW,\
//										get_total_credits());\
//									is_low_res=ilr;\
//								}\
//							}

int		no_result_box;

static sprite_info_t * psi_tbcred[MAX_PLYRS][TBCRED_DIGCNT];

static font_info_t * pfi_tbcred[] = {					// [res]
	&bast8t_font, &bast8t_font
};

static int pintx_turbo[2][MAX_PLYRS] = {
			 {BARX1, BARX2, 0, 0},						// X's for 2 player version
			 {BARX1_K, BARX2_K, BARX3_K, BARX4_K}};		// X's for 4 player version

static int pinty_turbo[] = {BARY, BARY_LR};				// Y for plyr turbo bars [res]
static int tbempty_msg_ys[] = {BARY, BARY_LR};			// Y for plyr turbo bars [res]
static int credit_msg_ys[] = {BARY-6.0f, BARY_LR-6.0f};	// Y for plyr turbo bars [res]

// *** NOTE *** low res...seems to select 1st entry of above tables

static image_info_t * pii_turbo[][MAX_PLYRS] = {		// plyr turbo bar imgs [res][plyr#]
	{&tb_blue, &tb_green, &tb_red, &tb_yellow},
	{&tb_blue, &tb_green, &tb_red, &tb_yellow}
};

#define	FIRE_FRAMES 15
#define	FIRE_FDELAY 3
static image_info_t *trb_bar_fire_imgs[FIRE_FRAMES] = {&firbar01,&firbar02,&firbar03,&firbar04,&firbar05,&firbar06,&firbar07,
														&firbar08,&firbar09,&firbar10,&firbar11,&firbar12,&firbar13,&firbar14,&firbar15};

static image_info_t ii_turbo[MAX_PLYRS];	// dynamic copies of turbo bar img info
static image_info_t ii_arr[MAX_PLYRS];		// dynamic copies of arrow img info
static image_info_t ii_num[MAX_PLYRS];		// dynamic copies of arrow # img info

static process_node_t *		pflasher = NULL;
static process_node_t *		pflasher2 = NULL;
static process_node_t *		pflasher3 = NULL;
static process_node_t *		pflasher_kick_meter = NULL;
static process_node_t *		pflasher_punt_meter = NULL;
static ostring_t *			pmsg[4];

// Prototypes
void tbcred_hide(int p);
void tbcred_show(int p);

void kick_meter_exit(struct process_node *, int);
void punt_meter_exit(struct process_node *, int);
int on_offense(int pnum);

void update_arrows(void);

static void flash_plyr_proc2(int *args);
void score_plate_proc(int *);

void win_proc(int *args);
void clock_low_proc(int *args);
void one_wide_flash_proc(int *);
void one_flash_proc(int *);
void show_message_proc(int *);
void display_free_pt_proc(int *);
void display_punt_proc(int *);
void display_FG_proc(int *);
void ko_idiot_box_proc(int *args);
static void plr_ko_idiot_box_proc(int *args);
void show_plyrs_mugshot_proc(int *args);
//void offense_idiot_box_proc(int *args);
//void defense_idiot_box_proc(int *args);
void flash_plyr_proc(int *args);
void delay_groove_proc(int *args);

// variables
sprite_info_t	*status_box_obj;

int punt_strength;							// Strength for punt from punt meter
int showkmtr_flag = 0;						// Hide-state for kick meter
int showhide_flag;							// Hide-state for turbo/msgs/arrows

int	update_status_box;
int	hide_status_box;

static image_info_t *status_box_imgs[] = {&gameclock, &gameclock};
static image_info_t *qtr_imgs[][2] = {
	{&sp_1st, &sp_1st_lr},
	{&sp_2nd, &sp_2nd_lr},
	{&sp_3rd, &sp_3rd_lr},
	{&sp_4th, &sp_4th_lr},
	{&sp_ot,  &sp_ot_lr},
	{&sp_ot,  &sp_ot_lr},
	{&sp_ot,  &sp_ot_lr}
};
static image_info_t * yrdplt_imgs[][2] = {
	{&sp_end,   	&sp_end},
	{&sp_bar,		&sp_bar},
//	{&sp_bar,   	&sp_bar},
	{&sp_bar,   	&sp_bar},
	{&sp_end,   	&sp_end}

//	{&sp_logoback,	&sp_end_lr},
//	{&sp_bar,		&sp_bar_lr},
//	{&sp_bar,   	&sp_bar_lr},
//	{&sp_bar,   	&sp_bar_lr},
//	{&sp_end,   	&sp_end_lr},
//	{&sp_clock, 	&sp_clock_lr},
//	{&sp_qtr,   	&sp_qtr_lr}
};


static image_info_t * sp_imgs[][2] = {
	{&sp_logoback,	&sp_end},
	{&sp_bar,		&sp_bar},
	{&sp_bar,   	&sp_bar},
	{&sp_bar,   	&sp_bar},
	{&sp_end,   	&sp_end},
	{&sp_clock, 	&sp_clock},
	{&sp_qtr,   	&sp_qtr}

//	{&sp_logoback,	&sp_end_lr},
//	{&sp_bar,		&sp_bar_lr},
//	{&sp_bar,   	&sp_bar_lr},
//	{&sp_bar,   	&sp_bar_lr},
//	{&sp_end,   	&sp_end_lr},
//	{&sp_clock, 	&sp_clock_lr},
//	{&sp_qtr,   	&sp_qtr_lr}
};

static float qb_msg_xys[MAX_PLYRS][2] = {				// [plyr_num][2/4 plyr -> x]
	{128.0F, 64.0f}, 		 						// player 1
	{384.0F,190.0f},  								// player 2
	{  0.0F,322.0F},  								// player 3
	{  0.0F,448.0F}	  								// player 4
	};


#define STBX_X		75.0f //437
#define STBX_Y		349.0F

#define	MSG_Z			1.75F
#define	X 				0
#define	Y 				1
static float status_box_xys[7][2][2] = {						// [# of msgs][2plr/4plr][x, y]
		{ {STBX_X+ 0.0F,STBX_Y+ 0.0F}, {STBX_X+ 0.0F,STBX_Y+ 0.0F} },				// status box itself
		{ {STBX_X+ 9.0F,STBX_Y+ 6.0F}, {STBX_X+ 9.0F,STBX_Y+ 6.0F} },				// team 1 name msg
		{ {STBX_X+31.0F,STBX_Y+ 6.0F}, {STBX_X+31.0F,STBX_Y+ 6.0F} },				// team 1 score
		{ {STBX_X+ 9.0F,STBX_Y- 7.0F}, {STBX_X+ 9.0F,STBX_Y- 7.0F} },				// team 2 name msg
		{ {STBX_X+31.0F,STBX_Y- 7.0F}, {STBX_X+31.0F,STBX_Y- 7.0F} },				// team 2 score
		{ {STBX_X-26.0F,STBX_Y- 7.0F}, {STBX_X-26.0F,STBX_Y- 7.0F} },				// quarter number str.
		{ {STBX_X-29.0F,STBX_Y+ 6.0F}, {STBX_X-29.0F,STBX_Y+ 6.0F} } };			// game time

static float scrplt_xys[][2][2] = {						// [# of objs][res][x, y]
		{ {  9.0f, 151.0f}, {  9.0f, 78.0f} },				// 0 bar end piece
		{ { 91.0f, 102.0f}, { 91.0f, 78.0f} },				// 1 bar piece 2
		{ {197.0f, 102.0f}, {197.0f, 78.0f} },				// 2 bar piece 3
		{ {303.0f, 102.0f}, {303.0f, 78.0f} },				// 3 bar piece 4
		{ {409.0f, 102.0f}, {409.0f, 78.0f} },				// 4 bar end piece 
		{ {400.0f,  89.0f}, {425.0f, 65.0f} },				// 5 clock piece
		{ {170.0f,  92.0f}, {220.0f, 68.0f} },				// 6 'quarter'
		{ {115.0f,  92.0f}, {176.0f, 68.0f} },				// 7 1st, 2nd, 3rd, 4th or OT (image)
		{ {372.0f,  87.0f}, {399.0f, 94.0f} },				// 8 game time minute
		{ {415.0f,  87.0f}, {438.0f, 94.0f} },				// 9 game time seconds
		{ { 65.0f,  85.0f}, { 68.0f, 65.0f} },				//10 nfl logo
		{ { 20.0f,  76.0f}, { 10.0f, 76.0f} },				//11 trans background piece
		{ {472.0f,  76.0f}, {472.0f, 76.0f} },				//12 trans background edge piece
		{ {120.0f,  56.0f}, {140.0f, 56.0f} },				//13 team 1 city name
		{ {462.0f,  56.0f}, {452.0f, 56.0f} },				//14 team 1 score
		{ {120.0f,  25.0f}, {140.0f, 25.0f} },				//15 team 2 city name
		{ {462.0f,  25.0f}, {452.0f, 25.0f} },				//16 team 2 score
		{ {  9.0f,  87.0f}, {  0.0f, 62.0f} }};				//17 left bottom piece

static float yrdplt_xys[][2][2] = {						// [# of objs][res][x, y]
//		{ {  9.0f, 151.0f}, {  9.0f, 78.0f} },				// 0 bar end piece
		{ { 69.0f, 75.0f}, { 69.0f, 50.0f} },				// 1 bar piece 2
		{ {150.0f, 75.0f}, {150.0f, 50.0f} },				// 2 bar piece 3
		{ {256.0f, 75.0f}, {256.0f, 50.0f} },				// 3 bar piece 4
		{ {362.0f, 75.0f}, {362.0f, 50.0f} },				// 4 bar end piece 
//		{ {400.0f,  89.0f}, {425.0f, 65.0f} },				// 5 clock piece
//		{ {210.0f,  92.0f}, {220.0f, 68.0f} },				// 6 'quarter'
//		{ {158.0f,  92.0f}, {176.0f, 68.0f} },				// 7 1st, 2nd, 3rd, 4th or OT (image)
//		{ {372.0f,  87.0f}, {399.0f, 94.0f} },				// 8 game time minute
//		{ {415.0f,  87.0f}, {438.0f, 94.0f} },				// 9 game time seconds
//		{ { 65.0f,  80.0f}, { 68.0f, 65.0f} },				//10 team logo
//		{ { 20.0f,  76.0f}, { 10.0f, 76.0f} },				//11 trans background piece
//		{ {472.0f,  76.0f}, {472.0f, 76.0f} },				//12 trans background edge piece
		{ {256.0f,  62.0f}, {256.0f, 53.0f} },				//13 text line 1
		{ {256.0f,  62.0f}, {256.0f, 53.0f} }};				//14 text line 2
//		{ {  9.0f,  87.0f}, {  0.0f, 62.0f} }};				//15 left bottom piece

//		{ {  9.0f, 151.0f}, {  9.0f, 78.0f} },				// 0 bar end piece
//		{ { 91.0f, 102.0f}, { 91.0f, 78.0f} },				// 1 bar piece 2
//		{ {197.0f, 102.0f}, {197.0f, 78.0f} },				// 2 bar piece 3
//		{ {303.0f, 102.0f}, {303.0f, 78.0f} },				// 3 bar piece 4
//		{ {409.0f, 102.0f}, {409.0f, 78.0f} },				// 4 bar end piece 
//		{ {400.0f,  89.0f}, {425.0f, 65.0f} },				// 5 clock piece
//		{ {210.0f,  92.0f}, {220.0f, 68.0f} },				// 6 'quarter'
//		{ {158.0f,  92.0f}, {176.0f, 68.0f} },				// 7 1st, 2nd, 3rd, 4th or OT (image)
//		{ {372.0f,  87.0f}, {399.0f, 94.0f} },				// 8 game time minute
//		{ {415.0f,  87.0f}, {438.0f, 94.0f} },				// 9 game time seconds
//		{ { 65.0f,  80.0f}, { 68.0f, 65.0f} },				//10 team logo
//		{ { 20.0f,  76.0f}, { 10.0f, 76.0f} },				//11 trans background piece
//		{ {472.0f,  76.0f}, {472.0f, 76.0f} },				//12 trans background edge piece
//		{ {256.0f,  55.0f}, {256.0f, 55.0f} },				//13 text line 1
//		{ {256.0f,  23.0f}, {256.0f, 23.0f} },				//14 text line 2
//		{ {  9.0f,  87.0f}, {  0.0f, 62.0f} }};				//15 left bottom piece


char *downstr[] = { "1ST", "2ND", "3RD", "4TH" };
char *qtrstr[] = { "1ST", "2ND", "3RD", "4TH", "OT1", "OT2", "OT3" };

static image_info_t *message_imgs[] =
{
	&msg_tdn,&msg_tdn_c1,						// 0	MS_TOUCHDOWN	
	&msg_trn,&msg_trn_c1,						// 1	MS_TURNOVER		
	&msg_saf,0,									// 2	MS_SAFETY		
	&msg_2pt,&msg_2pt_c1,						// 3	MS_2POINTS   	
	&msg_4d2,&msg_4d2_c1,						// 4	MS_4THDOWN     
	&msg_gd,&msg_gd_c1,							// 5	MS_ITSGOOD   	
	&msg_1dn,&msg_1dn_c1,						// 6	MS_1STDOWN    	
	&msg_fum,&msg_fum_c1,						// 7	MS_FUMBLE    	
	&msg_int,&msg_int_c1,						// 8	MS_INTERCEPTION
	&msg_ngd,&msg_ngd_c1,						// 9	MS_NOGOOD
	&msg_ond,0,									// 10	BLANK
	&msg_ond,0,									// 11	BLANK
	&msg_ond,0,									// 12	MS_ON_DOWNS
	&msg_trn,&msg_trn_c1,						// 13	MS_TURNOVER2		
	&msg_blk,&msg_blk_c1,						// 14	MS_BLOCKED
	0,0											// 15	MS_PUNTRETURN
};

static char *message_tex[] =
{
	msg_tdn_tex_name,							// 0	MS_TOUCHDOWN	
	msg_trn_tex_name,							// 1	MS_TURNOVER		
	msg_saf_tex_name,							// 2	MS_SAFETY		
	msg_2pt_tex_name,							// 3	MS_2POINTS   	
	msg_4dn_tex_name,							// 4	MS_4THDOWN     
	msg_gd_tex_name,							// 5	MS_ITSGOOD   	
	msg_1dn_tex_name,							// 6	MS_1STDOWN    	
	msg_fum_tex_name,							// 7	MS_FUMBLE    	
	msg_int_tex_name,							// 8	MS_INTERCEPTION
	msg_ngd_tex_name,							// 9	MS_NOGOOD		
	msg_ngd_tex_name,							// 10	BLANK
	msg_ngd_tex_name,							// 11	BLANK
	msg_trn_tex_name,							// 12	MS_ON_DOWNS
	msg_trn_tex_name,							// 13	MS_TURNOVER2		
	msg_blk_tex_name,							// 14	MS_BLOCKED
	""											// 15	MS_PUNTRETURN
};

static char *message_win_tex[] =
{	
	"win_car.wms",								// Arizona Cardinals
	"win_fal.wms",								// Altlanta Falcons
	"win_rav.wms",								// Baltimore Ravens
	"win_bil.wms",								// Buffalo Bills
	"win_pan.wms",								// Carolina Panthers
	"win_bea.wms", 								// Chicago Bears
	"win_ben.wms",								// Cincinnati Bengals
	"win_brw.wms",								// Cleveland Browns
	"win_cow.wms",								// Dallas Cowboys
	"win_bro.wms",								// Denver Broncos
	"win_lio.wms",								// Detroit Lions
	"win_pac.wms",								// Green Bay Packers
	"win_col.wms",								// Indianapolis Colts
	"win_jag.wms",								// Jacksonville Jaguars
	"win_chi.wms",								// Kansas City Chiefs
	"win_dol.wms",								// Miami Dolphins
	"win_vik.wms",								// Minnesota Vikings
	"win_pat.wms",								// New England Patriots
	"win_sai.wms",								// New Orleans Saints
	"win_gia.wms", 								// New York Giants
	"win_jet.wms",								// New York Jets
	"win_rai.wms",								// Oakland Raiders
	"win_eag.wms",								// Philadelphia Eagles
	"win_ste.wms",								// Pittsburgh Steelers
	"win_cha.wms",								// San Diego Chargers
	"win_49r.wms",								// San Francisco 49'ers
	"win_sea.wms", 								// Seattle Seahawks
	"win_ram.wms",								// St. Louis Rams
	"win_buc.wms",								// Tampa Bay Bucs
	"win_tit.wms",								// Tennessee Titans
	"win_red.wms",								// Washington Redskins
};

static image_info_t *message_win_imgs[] =
{
	&win_car,&win_car_c1,					// Arizona Cardinals	
	&win_fal,&win_fal_c1,					// Altlanta Falcons	
	&win_rav,0,								// Baltimore Ravens	
	&win_bil,0,								// Buffalo Bills	
	&win_pan,&win_pan_c1,					// Carolina Panthers	
	&win_bea,0,								// Chicago Bears	
	&win_ben,&win_ben_c1,					// Cincinnati Bengals	
	&win_brw,0,								// Cleveland Browns
	&win_cow,&win_cow_c1,					// Dallas Cowboys	
	&win_bro,&win_bro_c1,					// Denver Broncos	
	&win_lio,0,								// Detroit Lions	
	&win_pac,&win_pac_c1,					// Green Bay Packers	
	&win_col,0,								// Indianapolis Colts	
	&win_jag,&win_jag_c1,					// Jacksonville Jaguars	
	&win_chi,0,								// Kansas City Chiefs
	&win_dol,&win_dol_c1,					// Miami Dolphins
	&win_vik,0,								// Minnesota Vikings
	&win_pat,&win_pat_c1,					// New England Patriots
	&win_sai,0,								// New Orleans Saints
	&win_gia,0,								// New York Giants
	&win_jet,0,								// New York Jets
	&win_rai,&win_rai_c1,					// Oakland Raiders
	&win_eag,0,								// Philadelphia Eagles
	&win_ste,&win_ste_c1,					// Pittsburgh Steelers
	&win_cha,&win_cha_c1,					// San Diego Chargers
	&win_49r,0,								// San Francisco 49'ers
	&win_sea,&win_sea_c1,					// Seattle Seahawks
	&win_ram,0,								// St. Louis Rams
	&win_buc,&win_buc_c1,					// Tampa Bay Bucs
	&win_tit,0,								// Tennessee Titans
	&win_red,&win_red_c1,					// Washington Redskins
};



static image_info_t *qtr_tbl_imgs[] =
{
	&msg_1st,&msg_2nd,&msg_3rd
};

#if 1
extern image_info_t *sparkle_imgs[10];
#else
static image_info_t *sparkle_imgs[] =
{
	&sparkc01,&sparkc02,&sparkc03,&sparkc04,&sparkc05,
	&sparkc06,&sparkc07,&sparkc08,&sparkc09,&sparkc10
};
#endif
/*****************************************************************************/
//
void change_ostring_string(sprite_node_t * psn, ostring_t * string, char * psz, float xpos, int justify)
{
	register font_info_t * pfi = string->state.font_node->font_info;
	register image_info_t ** ppii = pfi->characters;
	register sprite_info_t * psi;
	register float fax;
	register int wch = 0;
	register int wst = 0;
	register int cnt = 0;
	register int len;
	register char val;
	sprite_node_t * psn_o = psn;
	int	was_low_res = is_low_res;

	// Ensure parms are useable
	justify &= HJUST_MODE_MASK;

	// Do new string length
	while (psz[cnt])
	{
		cnt++;
	}
	len = cnt;

	// Only go into loop if we know something has to be displayed
	while (cnt)
	{
		if (justify == HJUST_RIGHT)
		{
			cnt--;
			val = psz[cnt];
		}
		else
		{
			val = psz[len - cnt];
			cnt--;
		}

		// Make sure character exists in the font
		psi = NULL;
		fax = 0;
		if ((val >= pfi->start_character && val <= pfi->end_character) &&
			ppii[val - pfi->start_character] != NULL)
		{
			// Walk sprite list to find next character
			while (psn)
			{
				psi = psn->si;

				// Get the next node pointer
				psn = psn->next;

				// Node we are looking for?
				if (psi->parent == string)
				{
					// Yes; setup character for display & break the find
					psi->ii = ppii[val - pfi->start_character];
					wch = (int)psi->ii->w + pfi->char_space;
					fax = psi->ii->ax;

					unhide_sprite(psi);
					break;
				}
				psi = NULL;
			}
		}
		else
		{
			// Not in the font, but need to handle if its a space
			wch = (val == ' ') ? pfi->space_width : 0;
		}

		// Set position per justification mode
		if (justify == HJUST_RIGHT)
		{
			xpos -= wch;
			if (psi)
				psi->x = xpos - fax;
		}
		else
		{
			if (psi)
				psi->x = xpos - fax;
			xpos += wch;
		}
		wst += wch;

		// Abort if none left
		if (!psn) break;
	}

	// Walk sprite list & hide any remaining characters
	while (psn)
	{
		// Node we are looking for?
		if (psn->si->parent == string)
			hide_sprite(psn->si);

		// Get the next node pointer
		psn = psn->next;
	}

	// Do verts & chk if wants to be center-justified
	psn = psn_o;
	wch = wst >> 1;

	// Walk sprite list to find next character
	is_low_res = 0;
	while (psn)
	{
		// Node we are looking for?
		if (psn->si->parent == string &&
			!(psn->mode & HIDE_SPRITE))
		{
			if (justify == HJUST_CENTER)
				psn->si->x -= wch;

			generate_sprite_verts(psn->si);
		}
		// Get the next node pointer
		psn = psn->next;
	}
	is_low_res = was_low_res;
}

/*****************************************************************************/
//
void update_ostring_string(ostring_t * string, char * psz)
{
	change_ostring_string(alpha_sprite_node_list, string, psz, string->state.x, string->state.justification_mode);
}

/*****************************************************************************/
//
void update_ostring_string_nonalpha(ostring_t * string, char * psz)
{
	change_ostring_string(sprite_node_list, string, psz, string->state.x, string->state.justification_mode);
}

/******************************************************************************
** tbcred_hide(int p)
*/
void tbcred_hide(int p)
{
	int i = TBCRED_DIGCNT;
	while (i--)
		hide_sprite(psi_tbcred[p][i]);
}

/******************************************************************************
** tbcred_show(int p)
*/
void tbcred_show(int p)
{
	register font_info_t * pfi = pfi_tbcred[!!is_low_res];
	image_info_t ** ppii = pfi->characters;
	int s_c = '0' - pfi->start_character;
	int c_s = pfi->char_space;
	float fx = pintx_turbo[four_plr_ver][p] + TBCRED_XOFF;
	float fy = credit_msg_ys[!!is_low_res];
	int was_low_res = is_low_res;
	int crd = get_total_credits();
	int i, j, tmp;

	i = j = TBCRED_DIGCNT - 1;
	do {
		if ((tmp = crd % 10))
			j = i;
		crd /= 10;
		psi_tbcred[p][i]->parent = ppii[tmp + s_c];
	} while (i--);
	tbcred_hide(p);
	is_low_res = 0;
	do {
		register sprite_info_t * psi = psi_tbcred[p][j];
		if (psi->ii != psi->parent) {
			psi->ii = psi->parent;
			psi->x  = fx - psi->ii->ax;
			psi->y  = fy - psi->ii->ay;
			generate_sprite_verts(psi);
		}
		fx += psi->ii->w + c_s;
		unhide_sprite(psi);
	} while (++j < TBCRED_DIGCNT);
	is_low_res = was_low_res;
}

/******************************************************************************
** kick_meter_exit() - 
*/
void kick_meter_exit(struct process_node * proc, int cause)
{
	del1c( OID_FG_METER, -1 );
	delete_multiple_strings( SID_FG_METER, -1 );
#ifdef DEBUG
	delete_multiple_strings(0xdabbedad,-1);
#endif

	if (pflasher_kick_meter) kill( pflasher_kick_meter, 0 );

	showkmtr_flag = 0;
}

/******************************************************************************
** punt_meter_exit() - 
*/
void punt_meter_exit(struct process_node * proc, int cause)
{
	del1c( OID_PUNT_METER, -1 );
	delete_multiple_strings( SID_PUNT_METER, -1 );

	if (pflasher_punt_meter) kill( pflasher_punt_meter, 0 );

	showkmtr_flag = 0;
}

/******************************************************************************
** kick_meter_proc() - 
*/
//#define VELBASE	(5.5f)
#define VELBASE	(7.5f)

float kmtr_per;								// Accuracy % for kick meter

void kick_meter_proc(int *args)
{
	static float att_vel[] =
//		{ 2.00f *VELBASE, 1.66f *VELBASE, 1.33f *VELBASE, 1.00f *VELBASE, 0.66f *VELBASE, 0.33f *VELBASE };
		{ 1.15f *VELBASE, 1.10f *VELBASE, 1.05f *VELBASE, 1.00f *VELBASE, 0.95f *VELBASE, 0.90f *VELBASE };

	// Only do meter if FG-type play, human, and not in attract mode
	if ((game_info.team_play[game_info.off_side]->flags & (PLF_FIELD_GOAL | PLF_FAKE_FG)) &&
		(game_info.game_state != GS_ATTRACT_MODE) &&
			((!four_plr_ver && (p_status & (1 << (game_info.off_side  )))) ||
			 ( four_plr_ver && (p_status & (3 << (game_info.off_side*2))))))
	{
		static image_info_t ii_windarrow;
		unsigned char old_color_combiner_function;
		sprite_info_t * box_obj;
		sprite_info_t * meter_obj;
		sprite_info_t * marker_obj;
		sprite_info_t * kball_obj;
		sprite_info_t * windtext_obj = NULL;
		sprite_info_t * windarrow_obj = NULL;
		int mark_lx, mark_rx, pos, vel, wo2, xoff;
		float wind;
		float x;

		x = (game_info.off_side) ? BARX2 - 20 : BARX1 + 20;

		box_obj =    beginobj( &k_box,		x, (BARY + 30), 9.0f, SCR_PLATE_TID);
		meter_obj =  beginobj( &k_bar,		x, (BARY + 30), 8.9f, SCR_PLATE_TID);
		marker_obj = beginobj( &k_marker,	x, (BARY + 30), 8.0f, SCR_PLATE_TID);
		kball_obj =  beginobj( &k_ball,		x, (BARY + 30), 7.0f, SCR_PLATE_TID);

		box_obj->id = OID_FG_METER;
		meter_obj->id = OID_FG_METER;
		kball_obj->id = OID_FG_METER;
		marker_obj->id = OID_FG_METER;

		// Get rnd 0.0 to 1.0 in 10ths
		wind = (float)randrng(11) * 0.1f;
//		wind = 0.1f;
		if (wind != 0.0f)
		{
			float s = randrng(2) ? -1.0f : 1.0f;
//			static int tog = 0;
//			s = (tog++ & 2) ? 1.0f : -1.0f;

			windtext_obj =  beginobj( &k_windtext,  x,	(BARY + 30),	7.9f, SCR_PLATE_TID);
			windarrow_obj = beginobj( &k_windarrow,	x,	(BARY + 30),	7.8f, SCR_PLATE_TID);
			windtext_obj->id =  OID_FG_METER;
			windarrow_obj->id = OID_FG_METER;

			windtext_obj->x  += 40.0f * s;
			windarrow_obj->x -= 45.0f * s;

			ii_windarrow = *windarrow_obj->ii;
			ii_windarrow.w = 30.0f + ((ii_windarrow.w - 30.0f) * wind);
			ii_windarrow.s_start = ii_windarrow.s_end - (ii_windarrow.w/256.0f);

			if (s > 0.0f)
			{
				windarrow_obj->mode = FLIP_TEX_H;
				windarrow_obj->x += windarrow_obj->ii->w - ii_windarrow.w;
				wind = -wind;
			}
			windarrow_obj->ii = &ii_windarrow;

			generate_sprite_verts(windtext_obj);
			generate_sprite_verts(windarrow_obj);
		}
		// Times 10 for wind being 10ths, then times pixels/step
		xoff = (wind * (-10.0f * 4.0f));
		marker_obj->x += xoff;
		pos = ((int)marker_obj->ii->w + 1) >> 1;
		mark_lx = (xoff - pos) * 10000;
		mark_rx = (xoff + pos) * 10000;
		xoff *= 10000;
		generate_sprite_verts(marker_obj);

		old_color_combiner_function = kball_obj->state.color_combiner_function;
		showkmtr_flag = 1;

		((struct process_node *)cur_proc)->process_exit = kick_meter_exit;
		pflasher_kick_meter = NULL;

		pos = 0;
		vel = (int)(att_vel[teaminfo[game_info.team[game_info.off_side]].attr[TA_SPTEAMS]] * 10000.0f);
		wo2 = (int)(meter_obj->ii->w * 10000.0f * 0.5f);

		while (game_info.game_mode == GM_PRE_SNAP &&
				!(game_info.game_flags & GF_HIKING_BALL))
		{
			if (showhide_flag)
			{
				unhide_sprite(meter_obj);
				unhide_sprite(kball_obj);
				unhide_sprite(marker_obj);
				if (windtext_obj)
				{
					unhide_sprite(windtext_obj);
					unhide_sprite(windarrow_obj);
				}
			}
			else
			{
				hide_sprite(meter_obj);
				hide_sprite(kball_obj);
				hide_sprite(marker_obj);
				if (windtext_obj)
				{
					hide_sprite(windtext_obj);
					hide_sprite(windarrow_obj);
				}
			}

#ifdef DEBUG
			if (!halt || (halt && get_player_sw_close() & P3_DOWN))
#endif
			pos += vel;

			// Gone beyond an end of the meter bar?
			if ((pos >  wo2 && vel > 0) ||
				(pos < -wo2 && vel < 0))
			{
				// Beyond meter bar end; flip velocity & cap position
				pos = (vel > 0) ? wo2 : -wo2;
				vel = -vel;

				snd_scall_bank(cursor_bnk_str,P1_CURSR_SND,VOLUME3,127,PRIORITY_LVL5);
			}

			// Do color combiner at middle or one step to either side
			if (pos >= mark_lx && pos <= mark_rx)
			{
				kball_obj->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA;
				kball_obj->state.constant_color = 0xc0000000;
			}
			else
			{
				kball_obj->state.color_combiner_function = old_color_combiner_function;
				kball_obj->state.constant_color = 0xff000000;
			}

			kball_obj->x = x + pos / 10000;
			generate_sprite_verts(kball_obj);
#if 0
#ifdef DEBUG
			delete_multiple_strings(0xdabbedad,-1);
			set_string_id(0xdabbedad);
			set_text_font(FONTID_BAST8T);
			set_text_position(x,BARY+60,1);
			oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_CYAN,
				vel );
			set_text_position(x,BARY+72,1);
			oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_CYAN,
				pos );
			set_text_position(x,BARY+84,1);
			oprintf("%dj%dc%f", (HJUST_CENTER|VJUST_CENTER), LT_CYAN,
				((kball_obj->x - marker_obj->x) * 200.0f / meter_obj->ii->w) );
#endif
#endif
			sleep(1);
		}
		// Do accuracy % and leave it up for a bit if it wasn't a fake
		if (game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL)
		{
			if (game_info.auto_hike_cntr == 0)
			{
				kmtr_per = (55 + randrng(35)) * ((randrng(2)<<1)-1);

				del1c( OID_FG_METER, -1 );

				// Show auto-hike timeout message
				box_obj = beginobj( &playresult, x, BARY+10 , 1.4f, SCR_PLATE_TID );
				box_obj->w_scale = 0.9f;
				box_obj->h_scale = 0.55f;
				box_obj->id = OID_FG_METER;
				generate_sprite_verts( box_obj );

				set_text_transparency_mode( TRANSPARENCY_ENABLE );
				set_text_font( FONTID_BAST18 );
				set_string_id( SID_FG_METER );
				set_text_position( x, BARY+10, 1.2f );
				set_text_color(LT_CYAN);

				pmsg[0] = oprintf( "%djTIME EXPIRED", (HJUST_CENTER|VJUST_CENTER) );

				pflasher_kick_meter = qcreate( "ftp18", 0, flash_text_proc, (int)pmsg[0], 0, 0, 0 );

				// Wait till play is over
				while (game_info.game_mode != GM_PLAY_OVER) sleep(1);

				// added for '99
				kill(pflasher_kick_meter, 0);
			}
			else
			{
				// Do accuracy % and leave meter up for a bit
				// Convert the percentage; sprites may be deleted but ptrs still work
				//  Constant is percentage of meter from end to end
				kmtr_per = (kball_obj->x - marker_obj->x) * 200.0f / meter_obj->ii->w;
#ifdef DEBUG
				fprintf(stderr," kball_obj->x: %f\r\n", kball_obj->x);
				fprintf(stderr,"marker_obj->x: %f\r\n", marker_obj->x);
				fprintf(stderr,"meter: %d%%\r\n", (int)kmtr_per);
#endif
				sleep(30);
			}
		}
		die(0);
	}
}

void punt_meter_proc(int *args)
{
	static float att_vel[] =
		{ 1.15f *VELBASE, 1.10f *VELBASE, 1.05f *VELBASE, 1.00f *VELBASE, 0.95f *VELBASE, 0.90f *VELBASE };

	punt_strength = 50 + randrng(50);

	// Only do meter if FG-type play, human, and not in attract mode
	if ((game_info.team_play[game_info.off_side]->flags & (PLF_PUNT | PLF_FAKE_PUNT)) &&
		(game_info.game_state != GS_ATTRACT_MODE) &&
			((!four_plr_ver && (p_status & (1 << (game_info.off_side  )))) ||
			 ( four_plr_ver && (p_status & (3 << (game_info.off_side*2))))))
	{
		sprite_info_t * bx_obj;
		unsigned char old_color_combiner_function;

		register int pos;
		register int vel;
		register int wo2;
		register int x = (game_info.off_side) ? BARX2 + 64 : BARX1 - 64;
		register int y = BARY + 75;
		register sprite_info_t * kbobj = beginobj( &p_meter, x, y , 9.0f, SCR_PLATE_TID);
		register sprite_info_t * kmobj = beginobj( &k_ball,  x, y , 8.0f, SCR_PLATE_TID);

		kbobj->id = OID_PUNT_METER;
		kmobj->id = OID_PUNT_METER;

		old_color_combiner_function = kmobj->state.color_combiner_function;

		showkmtr_flag = 1;

		((struct process_node *)cur_proc)->process_exit = punt_meter_exit;
		pflasher_punt_meter = NULL;

		pos = 0;
		vel = (int)(att_vel[teaminfo[game_info.team[game_info.off_side]].attr[TA_SPTEAMS]] * 10000.0f);
		wo2 = (int)(kbobj->ii->h * (0.40f * 10000.0f));

		while (game_info.game_mode == GM_PRE_SNAP &&
				!(game_info.game_flags & GF_HIKING_BALL))
		{
			if (showhide_flag)
			{
				unhide_sprite(kbobj);
				unhide_sprite(kmobj);
			}
			else
			{
				hide_sprite(kbobj);
				hide_sprite(kmobj);
			}

#if DEBUG
			if (!halt || (halt && get_player_sw_close() & P3_DOWN))
#endif //DEBUG
			pos += vel;

			if (pos >  wo2 || pos < -wo2)
			{
				vel = -vel;
				snd_scall_bank(cursor_bnk_str,P1_CURSR_SND,VOLUME3,127,PRIORITY_LVL5);
			}

			// Do color combiner at middle or one step to either side
			if ((vel > 0 && pos <= vel && pos >= -vel) || (vel < 0 && pos <= -vel && pos >= vel))
			{
				kmobj->state.color_combiner_function = GR_COLORCOMBINE_DECAL_TEXTURE_ADD_CCALPHA;
				kmobj->state.constant_color = 0xc0000000;
			}
			else
			{
				kmobj->state.color_combiner_function = old_color_combiner_function;
				kmobj->state.constant_color = 0xff000000;
			}

			kmobj->y = y + pos / 10000;
			generate_sprite_verts(kmobj);

			sleep(1);
		}

		// Do accuracy % and leave it up for a bit if it wasn't a fake
		if (game_info.team_play[game_info.off_side]->flags & PLF_PUNT)
		{
			if (game_info.auto_hike_cntr == 0)
			{
				del1c( OID_PUNT_METER, -1 );

				// Show auto-hike timeout message
				bx_obj = beginobj( &playresult, x, BARY+10 , 1.4f, SCR_PLATE_TID );
				//bx_obj = beginobj( &playresult, x-104, BARY+42 , 1.4f, SCR_PLATE_TID );
				bx_obj->w_scale = 0.9f;
				bx_obj->h_scale = 0.55f;
				bx_obj->id = OID_PUNT_METER;
				generate_sprite_verts( bx_obj );

				set_text_transparency_mode( TRANSPARENCY_ENABLE );
				set_text_font( FONTID_BAST18 );
				set_string_id( SID_PUNT_METER );
				set_text_position( x, BARY+10, 1.2f );
				set_text_color(LT_CYAN);

                                pmsg[0] = oprintf( "%djTIME EXPIRED", (HJUST_CENTER|VJUST_CENTER) );

				pflasher_punt_meter = qcreate( "ftp1", 0, flash_text_proc, (int)pmsg[0], 0, 0, 0 );

				// Wait till play is over
				while (game_info.game_mode != GM_PLAY_OVER) sleep(1);

				// added for '99
				kill(pflasher_punt_meter, 0);
			}
			else
			{
			// Do accuracy % and leave meter up for a bit
			
				//	If ball-sprite ended up beyond the edges of the kick meter
				//	Cap it back inside (to the appropriate edge)
				if(pos < -wo2)
				{
				//	Ball-sprite is beyond the left edge of the kick meter
				//	Bump it back to the left edge of the kick meter
					pos = -wo2;
				}
				else if(pos >  wo2)
				{
				//	Ball-sprite is beyond the right edge of the kick meter
				//	Bump it back to the right edge of the kick meter
					pos =  wo2;
				}

//				punt_strength = (50 * pos) / wo2 + 50;
//fprintf(stderr, "old = %d\n", punt_strength);

				punt_strength = (65 * (wo2 - abs(pos))/wo2) + 25;
//fprintf(stderr, "wo2 = %d\npos = %d\n", wo2, abs(pos));
//fprintf(stderr, "new = %d\n", punt_strength);
				sleep(30);
			}
		}
		die(0);
	}
}

//---------------------------------------------------------------------------------------------------------------------------
//	delay_kicker_cam_proc()
//---------------------------------------------------------------------------------------------------------------------------
//void delay_kicker_cam_proc(int *);
//
//void delay_kicker_cam_proc(int *args)
//{
//	sleep(40);
//
//	select_csi_item( csi_zm_endplayfg );
//	cambot.pmodeobj = (void *)(args[0]);
//}

//---------------------------------------------------------------------------------------------------------------------------
//	This process delays the extra point groove
//
// 	INPUT:	action flag
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
void delay_groove_proc(int *args)
{
	sleep(220);		//190
	snd_scall_bank(generic_bnk_str,19,VOLUME4,127,SND_PRI_SET|SND_PRI_TRACK0);
}

//---------------------------------------------------------------------------------------------------------------------------
//	This function displays the play result after each play
//
// 	INPUT:	action flag
//	OUTPUT:	TRUE to have caller show screen longer (typically showing a scoreplate), FALSE otherwise
//---------------------------------------------------------------------------------------------------------------------------
int result_box( int action )
{
static sprite_info_t		*idiot_bx_obj;
float idiot_box_xys[2][2] =  {{315.0f,330.0f}, {315.0f,330.0f}};		// [2plr/4plr][x,y]
float idiot_box_xys2[2][2] = {{315.0f,330.0f}, {315.0f,330.0f}};		// [2plr/4plr][x,y] - used for kickoff return
int carrier = (game_info.ball_carrier < 0) ? game_info.last_carrier : game_info.ball_carrier;
int pnum = 0, yards;

//	if ((no_result_box) && (!game_info.off_turnover))
//		return FALSE;


	switch( game_info.play_result )
	{
		case PR_TOUCHDOWN:
			if( action )
			{
				qcreate("msg1", MESSAGE_PID, show_message_proc, MS_TOUCHDOWN, 0, 0, 0);

				qcreate("mugshot", PLYRINFO_PID, show_plyrs_mugshot_proc, 0, 0, 0, 0);

				do select_csi_item( csi_cd_endplay );
				while (cambot.csi.mode >= CAM_MODECNT);									// reselect camseq if its a NOP
				cambot.pmodeobj = &(player_blocks[carrier].odata);

//				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );
				qcreate( "delay", DELAY_PID, delay_groove_proc, 0, 0, 0, 0 );

				snd_stop_track(SND_TRACK_0);												// stop curent music
				snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
				snd_scall_bank(plyr_bnk_str,TOUCHDOWN_SND,FANFARE_VOL,127,SND_PRI_SET|SND_PRI_TRACK0);		// start touchdown ditty
				snd_scall_bank(ancr_swaped_bnk_str,TOUCHDOWN_SP,VOLUME4,127,LVL5);					// say 'TOUCHDOWN!'
				crowd_cheer_snds();
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;

		case PR_SAFETY:
		case PR_PAT_SAFETY:
			if( action )
			{
				qcreate("msg2", MESSAGE_PID, show_message_proc, MS_SAFETY, 0, 0, 0);

				select_csi_item( csi_cd_endplay );
				cambot.pmodeobj = &(player_blocks[ carrier ].odata);

				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );

				snd_scall_bank(plyr_bnk_str,NO_GOOD_SND,FANFARE_VOL,127,PRIORITY_LVL5);			// start wha wha wha ditty
				safety_sp();
				crowd_cheer_snds();
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;

		case PR_FAILED_CONVERSION:
			if( action )
			{
				qcreate("msg3", MESSAGE_PID, show_message_proc, MS_NOGOOD, 0, 0, 0);

				select_csi_item( csi_cd_endplay );
				if ((pnum = game_info.ball_carrier) < 0)
				{
					if ((pnum = ball_obj.int_receiver) < 0 || (ball_obj.type != LB_PASS && ball_obj.type != LB_LATERAL))
					{
						if ((pnum = ball_obj.who_threw) < 0)
						{
							pnum = carrier;
						}
					}
				}
				cambot.pmodeobj = &(player_blocks[ pnum ].odata);

				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );

				snd_stop_track(SND_TRACK_0);												// stop curent music
				snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
				snd_scall_bank(plyr_bnk_str,NO_GOOD_SND,FANFARE_VOL,127,PRIORITY_LVL5);			// start wha wha wha ditty
				snd_scall_bank(ancr_swaped_bnk_str,NO_GOOD_SP,VOLUME4,127,LVL5);					// say 'NO GOOD!'
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;

		case PR_CONVERSION:
			if( action )
			{
				qcreate("msg4", MESSAGE_PID, show_message_proc, MS_2POINTS, 0, 0, 0);

				select_csi_item( csi_cd_endplay );
				cambot.pmodeobj = &(player_blocks[ carrier ].odata);

				qcreate("mugshot", PLYRINFO_PID, show_plyrs_mugshot_proc, 0, 0, 0, 0);
//				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );

				snd_stop_track(SND_TRACK_0);												// stop curent music
				snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
				snd_scall_bank(plyr_bnk_str,ITS_GOOD_SND,FANFARE_VOL,127,PRIORITY_LVL5);		// start touchdown ditty
				snd_scall_bank(ancr_swaped_bnk_str,AND_IT_GOOD_SP,VOLUME4,127,LVL5);				// say 'AND ITS GOOD!'
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;

		case PR_FIELD_GOAL:
			if( action )
			{
				qcreate("msg5", MESSAGE_PID, show_message_proc, MS_ITSGOOD, 0, 0, 0);

				//select_csi_item( csi_zm_endplayfg );
				//cambot.pmodeobj = &(player_blocks[ carrier ].odata);
				//qcreate( "kckrcam", PLYRINFO_PID, delay_kicker_cam_proc, (int)(&(player_blocks[ carrier ].odata)), 0, 0, 0 );

				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );

				snd_scall_bank(plyr_bnk_str,ITS_GOOD_SND,FANFARE_VOL,127,PRIORITY_LVL4);		// start touchdown ditty
				field_goal_speech();
				crowd_cheer_snds();
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;

		case PR_MISSED_KICK:
			if( action )
			{
				qcreate("msg6", MESSAGE_PID, show_message_proc, MS_NOGOOD, 0, 0, 0);

				//select_csi_item( csi_zm_endplayfg );
				//cambot.pmodeobj = &(player_blocks[ carrier ].odata);
				//qcreate( "kckrcam", PLYRINFO_PID, delay_kicker_cam_proc, (int)(&(player_blocks[ carrier ].odata)), 0, 0, 0 );

				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );

				snd_scall_bank(plyr_bnk_str,NO_GOOD_SND,FANFARE_VOL,127,PRIORITY_LVL5);			// start wha wha wha ditty
				snd_scall_bank(ancr_swaped_bnk_str,NO_GOOD_SP,VOLUME4,127,LVL5);					// say 'NO GOOD!'
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;
		
		case PR_KICKOFF_OOB:
		case PR_PUNT_OOB:
			// Sometimes show score box instead of play result - based on play result
			if( action )
			{
				select_csi_item( csi_cd_endplay );
				cambot.pmodeobj = &(player_blocks[ (game_info.off_side) ? 13 : 6 ].odata);

				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;

		case PR_DOWNED_PUNT:
			// Sometimes show score box instead of play result - based on play result
			if( action )
			{
				select_csi_item( csi_cd_endplay );
				cambot.pmodeobj = &(ball_obj.odata);

				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;

		case PR_TOUCHBACK:
		
			if( action )
			{
				// Show result box - no big messages

				// Delete the Fumble! message that might be on screen!
				delete_multiple_strings(0x34, 0xffffffff);
				killall(MESSAGE_PID,0xFFFFFFF0);
				del1c(OID_FUMBLE_MSG,0xFFFFFFFF);

				// Put up the text messagebox
				idiot_bx_obj = beginobj(&playresult, idiot_box_xys[four_plr_ver][X_VAL], idiot_box_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
				idiot_bx_obj->id = OID_MESSAGE;
				idiot_bx_obj->w_scale = 1.32f;
				generate_sprite_verts(idiot_bx_obj);

				set_text_font(FONTID_BAST25);
				set_string_id(0x34);
				set_text_position(SPRITE_HRES/2.0f+63.0f,(SPRITE_VRES/2.0f)+28.0f+117.0f, 1.001F);

				set_text_color(LT_BLUE);
				pmsg[0] = oprintf( "%djTOUCHBACK", (HJUST_CENTER|VJUST_CENTER));
				
				if(is_low_res)
				{
					set_text_font(FONTID_BAST10LOW);
					set_text_position(SPRITE_HRES/2.0f+65.0f,(SPRITE_VRES/2.0f)-11.0f+127.0f-4, 1.001F);
				}
				else	
				{
					set_text_font(FONTID_BAST10);
					set_text_position(SPRITE_HRES/2.0f+65.0f,(SPRITE_VRES/2.0f)-11.0f+127.0f, 1.001F);
				}


				oprintf("%dj%dcBALL GOES TO THE 20 YARD LINE",
							(HJUST_CENTER|VJUST_CENTER),
							LT_YELLOW);

				pflasher = qcreate("ftp1", 0, flash_text_proc, (int)pmsg[0], 0, 0, 0);

				// Quickly flash idiot box white two times
				qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx_obj,3,2,0);
			}
			else
			{
				delete_multiple_strings(0x34, 0xffffffff);
				delobj(idiot_bx_obj);
				kill(pflasher, 0);

				// Also kill any big messages sprites/textures/process that may have appeared
				killall(MESSAGE_PID,0xFFFFFFF0);
				del1c(OID_MESSAGE,0xFFFFFFFF);
				delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);
			}
			break;
		
		case PR_EXTRA_POINT:
			// Sometimes show score box instead of play result - based on play result
			if( action )
			{
				select_csi_item( csi_cd_endplay );
				if ((pnum = game_info.ball_carrier) < 0)
				{
					if ((pnum = ball_obj.int_receiver) < 0 || (ball_obj.type != LB_PASS && ball_obj.type != LB_LATERAL))
					{
						if ((pnum = ball_obj.who_threw) < 0)
						{
							pnum = carrier;
						}
					}
				}
				cambot.pmodeobj = &(player_blocks[ pnum ].odata);

				qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );
			}
			else
			{
				del1c(OID_SCRPLATE,0xffffffff);
				delete_multiple_strings(0x09, 0xffffffff);
			}
			break;
			
		case PR_INCOMPLETE:
			if (action)
				incomplete_pass_sp();

		case PR_DOWN:
		case PR_FUMBLED_OOB:
			if( action )
			{
				select_csi_item( csi_cd_endplay );
				switch( game_info.play_result )
				{
					case PR_INCOMPLETE:
						// Will be either valid carrier/receiver or -1 to flag
						//  watching the ball
						if (ball_obj.flags & BF_DEFLECTED)
							pnum = game_info.ball_carrier;
						else
							pnum = ball_obj.int_receiver;
						break;

					case PR_DOWN:
						if ((pnum = game_info.ball_carrier) < 0)
						{
							if ((pnum = ball_obj.int_receiver) < 0 || (ball_obj.type != LB_PASS && ball_obj.type != LB_LATERAL))
							{
								if ((pnum = ball_obj.who_threw) < 0)
								{
									pnum = carrier;
								}
							}
						}
						break;

					case PR_FUMBLED_OOB:
						if ((pnum = ball_obj.who_threw) < 0)
							pnum = carrier;
						break;
				}
				cambot.pmodeobj = (pnum < 0) ? &(ball_obj.odata) : &(player_blocks[ pnum ].odata);
			}
			// Chk if it was a turnover on downs
			if (game_info.off_turnover)
			{
				if( action )
				{
					qcreate("msg7a", MESSAGE_PID, show_message_proc, MS_TURNOVER, 0, 0, 0);
					qcreate("msg7b", MESSAGE_PID, show_message_proc, MS_ON_DOWNS, 0, 0, 0);

					snd_scall_bank(plyr_bnk_str,NO_GOOD_SND,FANFARE_VOL,127,PRIORITY_LVL5);		// start wha wha wha ditty
					idiot_speech();
				}
				else
				{
					del1c(OID_SCRPLATE,0xffffffff);
					delete_multiple_strings(0x09, 0xffffffff);

					game_info.off_turnover = 0;
				}
			}
			// Not a turnover on downs
			else if( action )
			{
				// Show a first down message instead of play result box?
				if (game_info.just_got_1st)
				{
					// Is the SACK message currently being displayed?
					// If so, wait until it has been up the proper amount of time, then delete SACK text,
					// skip redrawing box, and put up new text
					if (existp(MESSAGE5_PID,0xFFFFFFFF))
					{
						while (existp(MESSAGE5_PID,0xFFFFFFFF))
							sleep (1);
					}

					// If no time on clock, skip first down message
					if (game_info.game_time)
					{
						qcreate("msg8", MESSAGE_PID, show_message_proc, MS_1STDOWN, 0, 0, 0);
						snd_scall_bank(cursor_bnk_str,FIRSTDOWN_SND,VOLUME3,127,PRIORITY_LVL5);
						if (randrng(100) < 40)
							snd_scall_bank(ancr_bnk_str,GETS_THE_FIRST_DOWN_SP,VOLUME3,127,LVL4);
						else
							snd_scall_bank(ancr_bnk_str,FIRST_DWN_SP,VOLUME3,127,LVL4);
						crowd_cheer_snds();
//						first_down_speech();
					}
				}
				// Not showing first down message
				else
				{
					// Show result box - no big messages

					// Is the SACK message currently being displayed?
					// If so, wait until it has been up the proper amount of time, then delete SACK text,
					// skip redrawing box, and put up new text
					if (existp(MESSAGE5_PID,0xFFFFFFFF))
					{
						while (existp(MESSAGE5_PID,0xFFFFFFFF))
							sleep (1);
					}

					// Delete the Fumble! message that might be on screen!
					delete_multiple_strings(0x34, 0xffffffff);
					killall(MESSAGE_PID,0xFFFFFFF0);
					del1c(OID_FUMBLE_MSG,0xFFFFFFFF);

					// Put up the text messagebox
					idiot_bx_obj = beginobj(&playresult, idiot_box_xys[four_plr_ver][X_VAL], idiot_box_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
					idiot_bx_obj->id = OID_MESSAGE;
					idiot_bx_obj->w_scale = 1.32f;
					generate_sprite_verts(idiot_bx_obj);

					set_text_font(FONTID_BAST25);
					set_string_id(0x34);
					set_text_position(SPRITE_HRES/2.0f+63.0f,(SPRITE_VRES/2.0f)+28.0f+117.0f, 1.001F);

					yards = game_info.los - game_info.old_los;
					
					if (game_info.play_result == PR_INCOMPLETE)
						yards = 0;
						
					if (!yards)
					{
						set_text_color(LT_BLUE);
						if( game_info.play_result == PR_INCOMPLETE)
							pmsg[0] = oprintf( "%djINCOMPLETE", (HJUST_CENTER|VJUST_CENTER));
						else
							pmsg[0] = oprintf( "%djNO GAIN", (HJUST_CENTER|VJUST_CENTER));
					}				
					else if (yards > 0)
					{
						set_text_color(LT_GREEN);
						if (game_info.game_flags & GF_FWD_PASS)
							pmsg[0] = oprintf( "%dj%d YARD PASS", (HJUST_CENTER|VJUST_CENTER), yards);
						else
							pmsg[0] = oprintf( "%dj%d YARD RUN", (HJUST_CENTER|VJUST_CENTER), yards);
					}
					else
					{
						set_text_color(LT_RED);
						pmsg[0] = oprintf( "%dj%d YARD LOSS", (HJUST_CENTER|VJUST_CENTER), -yards);
					}				
					
					if(is_low_res)
					{
						set_text_font(FONTID_BAST10LOW);
						set_text_position(SPRITE_HRES/2.0f+65.0f,(SPRITE_VRES/2.0f)-11.0f+127.0f-4, 1.001F);
					}
					else
					{
						set_text_font(FONTID_BAST10);
						set_text_position(SPRITE_HRES/2.0f+65.0f,(SPRITE_VRES/2.0f)-11.0f+127.0f, 1.001F);
					}

					if( game_info.first_down == 100 )
					{
						oprintf("%dj%dc%s AND GOAL",
								(HJUST_CENTER|VJUST_CENTER),
								LT_YELLOW,
								downstr[game_info.down-1] );
					}
					else
					{
						oprintf("%dj%dc%s DOWN AND %d",
								(HJUST_CENTER|VJUST_CENTER),
								LT_YELLOW,
								downstr[game_info.down-1],
								game_info.first_down - game_info.los);
					}

					pflasher = qcreate("ftp3", 0, flash_text_proc, (int)pmsg[0], 0, 0, 0);

					// Quickly flash idiot box white two times
					qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx_obj,3,2,0);
				}	
			}
			else
			{
				// If it wasn't a big first down message, delete result crap
				if (!game_info.just_got_1st)
				{	
					delete_multiple_strings(0x34, 0xffffffff);
					delobj(idiot_bx_obj);
					kill(pflasher, 0);

					// Also kill any big messages sprites/textures/process that may have appeared
					killall(MESSAGE_PID,0xFFFFFFF0);
					del1c(OID_MESSAGE,0xFFFFFFFF);
					delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);
				}
			}
			return FALSE;	// show 4th down turnover or first down or result box message

		case PR_NEW_POSSESSION:
			// If kick return & no fumble happened, print result
			if ((game_info.last_play_type == PT_KICKOFF || game_info.last_play_type == PT_PUNT) && !(game_info.game_flags & GF_FUMBLE))
//			if ((game_info.last_play_type == PT_KICKOFF) && !(game_info.game_flags & GF_FUMBLE) !(game_info.game_flags & GF_FUMBLE))
//			if ((game_info.last_play_type == PT_KICKOFF) && !(game_info.game_flags & GF_POSS_CHANGE))
			{
				if (action)
				{
					if ((pnum = game_info.ball_carrier) < 0)
					{
						if ((pnum = ball_obj.int_receiver) < 0 || (ball_obj.type != LB_PASS && ball_obj.type != LB_LATERAL))
						{
							if ((pnum = ball_obj.who_threw) < 0)
							{
								pnum = carrier;
							}
						}
					}
					cambot.pmodeobj = &(player_blocks[ pnum ].odata);

					// Delete the Fumble! message that might be on screen!
					delete_multiple_strings(0x34, 0xffffffff);
					killall(MESSAGE_PID,0xFFFFFFF0);
					del1c(OID_FUMBLE_MSG,0xFFFFFFFF);

					// Put up the text messagebox
					idiot_bx_obj = beginobj(&playresult, idiot_box_xys2[four_plr_ver][X_VAL], idiot_box_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
					idiot_bx_obj->id = OID_MESSAGE;
					idiot_bx_obj->w_scale = 1.60f;
					generate_sprite_verts(idiot_bx_obj);

					set_text_font(FONTID_BAST25);
					set_string_id(0x34);
					set_text_position(SPRITE_HRES/2.0f+63.0f,(SPRITE_VRES/2.0f)+28.0f+117.0f, 1.001F);

					set_text_color(LT_GREEN);
					pmsg[0] = oprintf( "%dj%d YARD RETURN", (HJUST_CENTER|VJUST_CENTER), game_info.los - game_info.ko_catch);
					
					if(is_low_res)
					{
						set_text_font(FONTID_BAST10LOW);
						set_text_position(SPRITE_HRES/2.0f+65.0f,(SPRITE_VRES/2.0f)-11.0f+127.0f-4, 1.001F);
					}
					else	
					{
						set_text_font(FONTID_BAST10);
						set_text_position(SPRITE_HRES/2.0f+65.0f,(SPRITE_VRES/2.0f)-11.0f+127.0f, 1.001F);
					}				

//					set_text_position(SPRITE_HRES/2.0f+65.0f,(SPRITE_VRES/2.0f)-11.0f+127.0f, 1.001F);

					if( game_info.first_down == 100 )
					{
						oprintf("%dj%dc%s AND GOAL",
								(HJUST_CENTER|VJUST_CENTER),
								LT_YELLOW,
								downstr[game_info.down-1] );
					}
					else
					{
						oprintf("%dj%dc%s DOWN AND %d",
								(HJUST_CENTER|VJUST_CENTER),
								LT_YELLOW,
								downstr[game_info.down-1],
								game_info.first_down - game_info.los);
					}

					pflasher = qcreate("ftp4", 0, flash_text_proc, (int)pmsg[0], 0, 0, 0);

					// Quickly flash idiot box white two times
					qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx_obj,3,2,0);
				}
				else
				{
					delete_multiple_strings(0x34, 0xffffffff);
					delobj(idiot_bx_obj);
					kill(pflasher, 0);

					// Also kill any big messages sprites/textures/process that may have appeared
					killall(MESSAGE_PID,0xFFFFFFF0);
					del1c(OID_MESSAGE,0xFFFFFFFF);
					delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);
				}
			}
			return FALSE;
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------
//	This function displays the clock low message
//
// 	INPUT:	action flag
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
void clock_low_proc(int *args)
{
	int i;
	static sprite_info_t		*idiot_bx_obj;
	float idiot_box_xys[2][2] = { {285.0f,330.0f}, {315.0f,330.0f} };		// [2plr/4plr][x,y]
	process_node_t	*pflsh = NULL;

	snd_scall_bank(cursor_bnk_str,WARNING_SND,VOLUME3,127,PRIORITY_LVL5);
	
	idiot_bx_obj = beginobj(&playresult, idiot_box_xys[four_plr_ver][X_VAL], idiot_box_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
	idiot_bx_obj->id = OID_MESSAGE2;
	idiot_bx_obj->w_scale = 1.32f;
	generate_sprite_verts(idiot_bx_obj);

	// Allow each sequential letter to overlap the previous letter - turn off at end!
	if (is_low_res)
	{
		set_text_font(FONTID_BAST23LOW);
		set_text_position(SPRITE_HRES/2.0f+63.0f,(SPRITE_VRES/2.0f)+28.0f+112.0f-5, 1.001F);
	}
	else
	{
		set_text_font(FONTID_BAST25);
		set_text_position(SPRITE_HRES/2.0f+63.0f,(SPRITE_VRES/2.0f)+28.0f+112.0f, 1.001F);
	}

	set_string_id(0x34);

	set_text_color(LT_RED);
	pmsg[0] = oprintf( "%djCLOCK LOW", (HJUST_CENTER|VJUST_CENTER));

	pflsh = qcreate("ftp5", 0, flash_text_proc, (int)pmsg[0], 0, 0, 0);

	// Quickly flash idiot box white two times
	qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx_obj,3,2,0);

	for( i = 0; i < tsec*3; i++ )
	{
		sleep(1);

		if (i == 55)
			snd_scall_bank(cursor_bnk_str,WARNING_SND,VOLUME3,127,PRIORITY_LVL5);

		if( game_info.game_mode == GM_IN_PLAY )
			break;
	}

	delete_multiple_strings(0x34, 0xffffffff);
	delobj(idiot_bx_obj);
	kill(pflsh, 0);
}

/******************************************************************************
** plyrinfo_proc() - Top-level player info process
*/
static void plyrinfo_proc(int *parg)
{
	int	old_creds = get_total_credits();
	int	cyc_creds = CREDIT_CYCLE;
	int message_timer = 0;
	int was_low_res = is_low_res;

	// Top of proc loop
	while (1)
	{
		register int free_play      = coin_check_freeplay();
		register int cur_creds      = get_total_credits();
		register int can_start      = check_credits_to_start();
		register pdata_t * ppb      = pdata_blocks;
		register image_info_t * pii = ii_turbo;
		register image_info_t * piitmsg;
		register float f,g;
		register int i;

		i = 0;
		do {
			// Chk if player active
			if (p_status & (1 << i))
			{
				// Player active
				// Check turbo button action
				if ((game_info.plyr_control[i] >= 0) &&
					(game_info.game_mode == GM_IN_PLAY) &&
					!(game_info.team_fire[PLYRTEAM(i)]) &&
					(!(pup_infturbo & (1<<i))))
				{		// 0 or 1 if human, -1 if drone
					if ((get_player_sw_current() >> (i<<3)) & P_C)
					{
						ppb->turbo -= 0.35f;				// .4
						if (ppb->turbo < 0)
							ppb->turbo = 0;
					}
					else
					{
						// Replenish turbo
						if ((ppb->turbo += 0.35f) > 100.0f)		// .7
							ppb->turbo = 100.0f;

						// more for defense
						if ((player_blocks[game_info.plyr_control[i]].team != game_info.off_side) &&
							((ppb->turbo += 0.15f) > 100.0f))
							ppb->turbo = 100.0f;
					}
				}

				// Kill any message sprites & show turbo bar
//				if (ppb->si_fmsg) {
//					delobj(ppb->si_fmsg);
//					ppb->si_fmsg = NULL;
//				}
				if (ppb->credits_show)
				{
					CREDSHOW_KILL(i);
					ppb->credits_show = 0;
				}
				if (ppb->si_tmsg)
				{
					delobj(ppb->si_tmsg);
					ppb->si_tmsg = NULL;
				}
				if (showkmtr_flag)	// && game_info.off_side == i)
				{
					hide_sprite(ppb->si_fframe);
					hide_sprite(ppb->si_frame);
					hide_sprite(ppb->si_turbo);
				}
				else if (showhide_flag)
				{
					if (game_info.team_fire[PLYRTEAM(i)])
						unhide_sprite(ppb->si_fframe);
					unhide_sprite(ppb->si_frame);
					unhide_sprite(ppb->si_turbo);
				}

				// Chk if turbo bar needs updating
				if ((f = ppb->turbo) != ppb->turbo_last)
				{
					ppb->turbo_last = f;
					pii->w = g = ppb->turbo_w * f / 100.0f;
					pii->s_end = pii->s_start + g / 256.0f;		// !!! Watch This !!!	128

					// Make sure you put this back before sleeping!!!
					is_low_res = 0;

					generate_sprite_verts(ppb->si_turbo);

					// Back it goes!!!
					is_low_res = was_low_res;
				}

				// if team (player) on fire... animate fire behind turbo bar!
				if ((game_info.plyr_control[i] >= 0) &&
					(game_info.team_fire[player_blocks[game_info.plyr_control[i]].team]))
				{
					if (--ppb->fdelay <= 0)
					{
						ppb->fdelay = FIRE_FDELAY;
						if (++ppb->fframe >= FIRE_FRAMES)
							ppb->fframe = 0;
						// change fire frame
						ppb->si_fframe->ii = trb_bar_fire_imgs[ppb->fframe];
						generate_sprite_verts(ppb->si_fframe);
					}
				}
				
			}
			else
			{
				// Player inactive
				// Hide turbo bar & make message frame
				hide_sprite(ppb->si_fframe);				// hide fire behind turbo bar
				hide_sprite(ppb->si_turbo);
				if (!showhide_flag || showkmtr_flag)
					hide_sprite(ppb->si_frame);
				else
					unhide_sprite(ppb->si_frame);

				// if drone team on fire...show fire behind turbo message bars
//				if (game_info.team_fire[ (i < (four_plr_ver ? 2 : 1) ? 0 : 1) ])
//					unhide_sprite(ppb->si_fframe);

				// Chk for message change
				piitmsg = NULL;
				if (showkmtr_flag)
				{
					hide_sprite(ppb->si_fframe);

					if (ppb->credits_show)
					{
						CREDSHOW_KILL(i);
						ppb->credits_show = 0;
					}
					if (ppb->si_tmsg)
					{
						delobj(ppb->si_tmsg);
						ppb->si_tmsg = NULL;
					}
				}
				else if (old_creds != cur_creds || !cyc_creds)
				{
					if (old_creds != cur_creds || !message_timer)
					{
						if (free_play)
							piitmsg = &tb_free;
						else
						{
							piitmsg = &tb_credits;

							CREDSHOW_KILL(i);
							ppb->credits_show = 0;

							if (showhide_flag)
							{
								//CREDSHOW_FONT;
								CREDSHOW_MAKE(i);
							}
							ppb->credits_show = 1;
						}
					}
				}
				// Chk please wait status
				else if (ppb->please_wait)
				{
					if (!(ppb->si_tmsg) || ppb->si_tmsg->ii != &tb_please)
						piitmsg = &tb_please;
				}
				// Do other toggling messages
				else if (!message_timer)
				{
					if (cyc_creds & 1)
						piitmsg = (can_start) ? &tb_press : &tb_insert;
					else
						piitmsg = &tb_join;
				}
				if (piitmsg)
				{
					if (piitmsg != &tb_credits)
					{
						CREDSHOW_KILL(i);
						ppb->credits_show = 0;
					}
					if (ppb->si_tmsg) delobj(ppb->si_tmsg);

					// Make sure you put this back before sleeping!!!
					is_low_res = 0;

					generate_sprite_verts(ppb->si_tmsg = beginobj(piitmsg,(float)((int)pintx_turbo[four_plr_ver][i]), (float)(tbempty_msg_ys[!!was_low_res]), 10.0f, SCR_PLATE_TID));

					// Back it goes!!!
					is_low_res = was_low_res;

					ppb->si_tmsg->id = OID_TURBARROW;
					if (!showhide_flag)
					{
						hide_sprite(ppb->si_tmsg);
						hide_sprite(ppb->si_fframe);
					}
				}

				// if drone team on fire...animate fire behind turbo bar!
				if ((game_info.team_fire[ (i < (four_plr_ver ? 2 : 1) ? 0 : 1) ]) && (showhide_flag) && (!showkmtr_flag))

				{
					unhide_sprite(ppb->si_fframe);
					if (--ppb->fdelay <= 0)
					{
						ppb->fdelay = FIRE_FDELAY;
						if (++ppb->fframe >= FIRE_FRAMES)
							ppb->fframe = 0;
						// change fire frame
						ppb->si_fframe->ii = trb_bar_fire_imgs[ppb->fframe];
						generate_sprite_verts(ppb->si_fframe);
					}
				}
			}
		} while (++ppb, ++pii, ++i < (four_plr_ver ? MAX_PLYRS : MAX_PLYRS-2));

		if (showkmtr_flag)
		{
			message_timer = 0;
			cyc_creds = 0;
		}
		else
		{
			if (old_creds != cur_creds)
			{
				message_timer = COININ_TIMER;
				cyc_creds = 0;
			}
			old_creds = cur_creds;

			if (--message_timer <= 0)
			{
				if (!message_timer)
				{
					if (--cyc_creds < 0)
						cyc_creds = CREDIT_CYCLE;
				}
				else
					message_timer = (cyc_creds) ? JOININ_TIMER : CREDIT_TIMER;
			}
		}
		sleep(1);
	}
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
#if 0
int on_offense(int pnum)
{
	if (four_plr_ver)
	{
		if ((game_info.off_side) && (pnum >= 2))
			return(1);							// offense
		if ((!game_info.off_side) && (pnum < 2))
			return(1);							// offense
	}
	else
	{
		if (game_info.off_side == pnum)
			return(1);							// offense
	}

	return(0);									// defense
}

#endif


/******************************************************************************
** showhide_pinfo() - Show|hide the turbo bars/messages/arrows
** Pass non-zero to show, zero to hide
*/
void showhide_pinfo(int flag)
{
	register ARROWINFO * pai = &arrowinfo[0];
	register pdata_t *   ppb = &pdata_blocks[0];
	
	register void (*func)(sprite_info_t *) = (flag) ? unhide_sprite : hide_sprite;
	register int i = 0;

	// Set font for credits_show
	//CREDSHOW_FONT;

	do {
		if (!flag)
		{
			hide_sprite(pai->arr_obj);
			hide_sprite(pai->num_obj);
			if (ppb->si_turbo)
				hide_sprite(ppb->si_turbo);
		}
		else
		{
			if (pai->show > 0)
			{
				unhide_sprite(pai->arr_obj);
				unhide_sprite(pai->num_obj);
			}
			if (p_status & (1 << i) && ppb->si_turbo)
				unhide_sprite(ppb->si_turbo);
		}

		if ((game_info.plyr_control[i] >= 0) &&
				game_info.team_fire[player_blocks[game_info.plyr_control[i]].team] &&
				flag)
			unhide_sprite(ppb->si_fframe);
		else
			hide_sprite(ppb->si_fframe);

		if (ppb->si_frame)
			func(ppb->si_frame);
		if (ppb->si_tmsg)
			func(ppb->si_tmsg);

		CREDSHOW_KILL(i);

		if (ppb->credits_show && flag)
			CREDSHOW_MAKE(i);

	} while (++pai,++ppb,++i < (four_plr_ver ? MAX_PLYRS : MAX_PLYRS-2));
	showhide_flag = flag;
}

/******************************************************************************
** update_arrows() - Update player off-screen arrows
*/

// Same math works for mid- & low-res; overlay global <vres> refs with
// a #def = 384; undef'd at end of <update_arrows>
#define vres (384.0f)

#define P_ARRHIDE	(0.96f)
#define P_ARRSHOW	(1.05f)

#define CNT_PRESNAP_Q1	(57 * 3)
#define CNT_KICKOFF_Q1	(57 * 12)
#define CNT_PSWITCH_Q1	(57 * 3)
#define CNT_POFFON_Q1	(57 * 3)

#define CNT_PRESNAP		(200)
#define CNT_KICKOFF		(57 * 9)
#define CNT_PSWITCH		(57 * 2)
#define CNT_POFFON		(57 * 2)

#define OFF_ARR_DX	(8.0f)
#define OFF_ARR_DY	(10.0f)
#define DEF_ARR_DX	(0.0f)
#define DEF_ARR_DY	(22.0f)

#define XBORD	(30)
#define YBORD	(30)
#define SK		((vres/2.0f)/hres)

static void update_arrows_proc(int *parg)
{
	int gstart;		// Initial kickoff flag for arrows to stay on

	// Wait for other guys to start their asses
//	sleep(9);
	while(game_info.game_mode == GM_INITIALIZING)
	{
		sleep(1);
	}

	// Use <game_quarter> so it'll only be 0 at start of game
	gstart = game_info.game_quarter;

/// Loop here till dead
	while (1) {
		register ARROWINFO * pai = &arrowinfo[0];
		register sprite_info_t * arr_obj;
		register sprite_info_t * num_obj;
		register fbplyr_data * ppdata;
		register float psx,psy,psz,sx,sy;
		float distance = cambot.d;
		int cnt, i;

		// Set initial kickoff flag after 1st play is over
		if (game_info.game_mode == GM_PLAY_OVER)
			gstart = 1;

		// Set timer cnt per mode
		switch (game_info.game_mode) {
			case GM_LINING_UP:
			case GM_PRE_SNAP:
				cnt = (game_info.game_quarter) ? CNT_PRESNAP : CNT_PRESNAP_Q1;

				// Clr arrows-off flags
				pai[0].ai_flags &= ~AF_INACTIVE;
				pai[1].ai_flags &= ~AF_INACTIVE;
				pai[2].ai_flags &= ~AF_INACTIVE;
				pai[3].ai_flags &= ~AF_INACTIVE;
				break;

			case GM_PRE_KICKOFF:
				cnt = (game_info.game_quarter) ? CNT_KICKOFF : CNT_KICKOFF_Q1;
				break;

			case GM_PLAY_OVER:
			case GM_GAME_OVER:
				cnt = -1;
				break;

			default:
				cnt = 0;
				break;
		}
//{
//	static fuckyou=0;
//	if (get_dip_coin_current() & SLAM_SW) {
//		if (cambot.was_playover)
//		{
//			printf("was_playover: %d\n",cambot.was_playover);
//			fuckyou=1;
//		}
//		if (fuckyou)
//		{
//			while(!(get_dip_coin_close() & SERVICE_CREDIT_SW));
//		}
//	}
//	else
//		fuckyou=0;
//}

//Temp!!!
//	if (get_player_sw_current() & P1_ABCD_MASK)
//		showhide_pinfo(0);
//	else	
//		showhide_pinfo(1);
//End Temp!!!
//#if 0
//	delete_multiple_strings(0xbeef, 0xffffffff);
//	set_text_font(FONTID_BAST10);
//	set_text_transparency_mode(TRANSPARENCY_ENABLE);
//	set_string_id(0xbeef);
//	set_text_position(256, 80, 1.2f); oprintf("%dj%dc%f", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, distance);
//#endif
//#if 0
//		{
//			static int crap = GM_PLAY_OVER;
//
//			switch (crap) {
//				case GM_PRE_SNAP:					// waiting
//
//					if (game_info.game_mode == GM_IN_PLAY)
//					{
//						crap = GM_IN_PLAY;
//					}
//					break;
//
//				case GM_IN_PLAY:					// play, no pup
//
//					if ((((i = pai[0].pnum) >= 0) &&
//							player_blocks[i].odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER)) ||
//							(((i = pai[1].pnum) >= 0) &&
//							player_blocks[i].odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER)))
//					{
//						crap = -GM_IN_PLAY;
//					}
//					if (game_info.game_mode == GM_PLAY_OVER)
//					{
//						if (crap == GM_IN_PLAY)		// play, no pup
//						{
//							crap = GM_PLAY_OVER;	// over, no pup
//						}
//						else
//						{
//							crap = -GM_PLAY_OVER;	// over, pup
//						}
//					}
//					break;
//
//				case -GM_IN_PLAY:					// play, pup
//
//					if (!((((i = pai[0].pnum) >= 0) &&
//							player_blocks[i].odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER)) ||
//							(((i = pai[1].pnum) >= 0) &&
//							player_blocks[i].odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER))))
//					{
//#ifdef DEBUG
//						fprintf(stderr,"----\r\nCrap hits the fan--  P0: %d  P1: %d\r\n----\r\n",pai[0].pnum,pai[1].pnum);
//#endif
//
//						crap = GM_IN_PLAY;			// play, no pup
//					}
//					if (game_info.game_mode == GM_PLAY_OVER)
//					{
//						if (crap == -GM_IN_PLAY)	// play, pup
//						{
//							crap = -GM_PLAY_OVER;	// over, pup
//						}
//						else
//						{
//							crap = GM_PLAY_OVER;	// over, no pup
//						}
//					}
//					break;
//
//				case -GM_PLAY_OVER:					// over, pup
//				case GM_PLAY_OVER:					// over, no pup
//
//					if (game_info.game_mode == GM_IN_PLAY)
//					{
//#ifdef DEBUG
//						fprintf(stderr,"----\r\nPlaying with the crap again\r\n----\r\n");
//#endif
//
//						if (crap == GM_PLAY_OVER)	// over, no pup
//						{
//							crap = GM_IN_PLAY;		// play, no pup
//						}
//						else
//						{
//							crap = -GM_IN_PLAY;		// play, pup
//						}
//					}
//					if (game_info.game_mode == GM_PRE_SNAP)
//					{
//						crap = GM_PRE_SNAP;
//					}
//					break;
//			}
//		}
//#endif
		
#if 0
		// Arrows go away if either player is in puppet/puppeteer as long as one
		//  of them is the ball carrier
		if  (  ((pai[0].pnum >= 0 && pai[0].pnum == game_info.ball_carrier) ||
				(pai[1].pnum >= 0 && pai[1].pnum == game_info.ball_carrier)) &&
			   ((pai[0].pnum >= 0 &&
				((fbplyr_data *)pai[0].ppdata)->odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER)) ||
				(pai[1].pnum >= 0 &&
				((fbplyr_data *)pai[1].ppdata)->odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER)))
			)
		{
			// Set arrows-off flags
			pai[0].ai_flags |= AF_INACTIVE;
			pai[1].ai_flags |= AF_INACTIVE;
		}

		if  (  ((pai[2].pnum >= 0 && pai[2].pnum == game_info.ball_carrier) ||
				(pai[3].pnum >= 0 && pai[3].pnum == game_info.ball_carrier)) &&
			   ((pai[2].pnum >= 0 &&
				((fbplyr_data *)pai[2].ppdata)->odata.adata[2].animode & (MODE_PUPPET|MODE_PUPPETEER)) ||
				(pai[3].pnum >= 0 &&
				((fbplyr_data *)pai[3].ppdata)->odata.adata[3].animode & (MODE_PUPPET|MODE_PUPPETEER)))
			)
		{
			// Set arrows-off flags
			pai[2].ai_flags |= AF_INACTIVE;
			pai[3].ai_flags |= AF_INACTIVE;
		}
#endif

		// OK so far, go ahead and do'em
		i = 0;
		do {
			arr_obj = pai->arr_obj;
			num_obj = pai->num_obj;
			ppdata  = pai->ppdata;

			// Assume we don't wanna see'em
			pai->show = -1;

#ifdef SHOW3DARROWS
			// Assume we don't wanna see'em
			pai->show3d = 0;
#endif
			// Do the timer-stuff
			if (!gstart)
				pai->timer = 1;
			else if (pai->timer < cnt)
				pai->timer = cnt;
			else if (pai->timer)
				pai->timer--;

			if (pai->timer < 0 || cnt < 0)
				pai->timer = 0;

			// Is it a human player? Show arrows only if so & play didn't just end
			if (pai->pnum >= 0 && !cambot.was_playover && !(pai->ai_flags & AF_INACTIVE) &&
					idiot_boxes_shown >= 0)
			{
				// Do player-switched timer-stuff
				if ((pai->pnum_last != pai->pnum && pai->pnum >=0) ||
						(ppdata->team == game_info.off_side &&
						game_info.game_mode == GM_IN_PLAY &&
						ball_obj.type == LB_PASS))
				{
					int j = (game_info.game_quarter) ? CNT_PSWITCH : CNT_PSWITCH_Q1;
					if (pai->timer < j)
						pai->timer = j;
				}

				// Get positioning data for off-screen arrow
				sx = fabs(psx = -pai->plyr_sx)*(2.0f*FOCFAC);
				sy = fabs(psy =  pai->plyr_sy)*(2.0f*FOCFAC*hres/vres);
				psz = pai->plyr_sz;
//#if 0
//{
//	float tsx, tsy, tsz;
//	int xpsx, xpsy, xpsz, xtsz, xsx, xsy;
//	int cpsx, cpsy, cpsz, ctsz, csx, csy;
//	xpsx = xpsy = xpsz = xtsz = xsx = xsy = 60+i*380;
//	cpsx = cpsy = cpsz = ctsz = csx = csy = LT_YELLOW;
//
//	if (   sx > (psz*P_ARRHIDE)) csx  = LT_RED;
//	if (   sy > (psz*P_ARRHIDE)) csy  = LT_RED;
//	if (10.0f > (psz*P_ARRHIDE)) cpsz = LT_RED;
//
//	if (   sx > (psz*P_ARRSHOW)) csx  = LT_GREEN;
//	if (   sy > (psz*P_ARRSHOW)) csy  = LT_GREEN;
//	if (10.0f > (psz*P_ARRSHOW)) cpsz = LT_GREEN;
//
//	tsz = psz;
//
//	if (sy > psz) {
//		if (psy > 0) {
//			xpsy -= 16;
//			cpsy = LT_GREEN;
//			tsz = (psy * distance) / (psy + SK * (distance - psz));
//		}
//		else {
//			xpsy += 16;
//			cpsy = LT_GREEN;
//			tsz = (psy * distance) / (psy - SK * (distance - psz));
//		}
//	}
//
//	tsx = psx/tsz*hres;
//	tsy = psy/tsz*hres;
//
//	set_text_position(xpsx, 104, 1.2f); oprintf("%dj%dc%i.", (HJUST_RIGHT|VJUST_CENTER), cpsx, (int)psx);
//	set_text_position(xpsx, 104, 1.2f); oprintf("%dj%dc%i",  (HJUST_LEFT| VJUST_CENTER), cpsx, (int)fabs(1000000.0f*(psx-(float)(int)psx)));
//	set_text_position(xpsy, 92,  1.2f); oprintf("%dj%dc%i.", (HJUST_RIGHT|VJUST_CENTER), cpsy, (int)psy);
//	set_text_position(xpsy, 92,  1.2f); oprintf("%dj%dc%i",  (HJUST_LEFT| VJUST_CENTER), cpsy, (int)fabs(1000000.0f*(psy-(float)(int)psy)));
//	set_text_position(xpsz, 80,  1.2f); oprintf("%dj%dc%i.", (HJUST_RIGHT|VJUST_CENTER), cpsz, (int)psz);
//	set_text_position(xpsz, 80,  1.2f); oprintf("%dj%dc%i",  (HJUST_LEFT| VJUST_CENTER), cpsz, (int)fabs(1000000.0f*(psz-(float)(int)psz)));
//	set_text_position(xtsz, 66,  1.2f); oprintf("%dj%dc%i.", (HJUST_RIGHT|VJUST_CENTER), ctsz, (int)tsz);
//	set_text_position(xtsz, 66,  1.2f); oprintf("%dj%dc%i",  (HJUST_LEFT| VJUST_CENTER), ctsz, (int)fabs(1000000.0f*(tsz-(float)(int)tsz)));
//	set_text_position(xsx,  52,  1.2f); oprintf("%dj%dc%i.", (HJUST_RIGHT|VJUST_CENTER), csx,  (int)sx);
//	set_text_position(xsx,  52,  1.2f); oprintf("%dj%dc%i",  (HJUST_LEFT| VJUST_CENTER), csx,  (int)fabs(1000000.0f*(sx-(float)(int)sx)));
//	set_text_position(xsy,  40,  1.2f); oprintf("%dj%dc%i.", (HJUST_RIGHT|VJUST_CENTER), csy,  (int)sy);
//	set_text_position(xsy,  40,  1.2f); oprintf("%dj%dc%i",  (HJUST_LEFT| VJUST_CENTER), csy,  (int)fabs(1000000.0f*(sy-(float)(int)sy)));
//}
//#endif
				// Can we show off-screen arrow?
				// Can if:
				//     arrows are showing and GM_IN_PLAY and
				//       ball is not a LB_KICKOFF or
				//       player's team is not <off_init> or
				//       there is a ball carrier
				//   or
				//     active timer and
				//       not game start or
				//       ball is a LB_KICKOFF and
				//         ball Y is <= 10
				// and
				//   X or Y or Z are outside the arrow-hide perimeter
				if (( (showhide_flag && game_info.game_mode == GM_IN_PLAY &&
								(ball_obj.type != LB_KICKOFF ||
								ppdata->team != game_info.off_init ||
								game_info.ball_carrier >= 0)
							)
						||
							(pai->timer &&
								(gstart ||
									(ball_obj.type == LB_KICKOFF &&
									ball_obj.odata.y <= 10.0f)
								)
							)
						) && (
							(psz * P_ARRHIDE) < sx ||
							(psz * P_ARRHIDE) < sy ||
							(psz * P_ARRHIDE) < 10.0f)
						) {
					// Show arrow if X, Y, or Z are beyond the arrow-show perimeter
					//   else leave arrow as it was, shown or hidden
//if (get_player_sw_current() & P4_D)
//fprintf(stderr,"showing arrow %d	",i);
					if ((pai->show = (int)
							((psz * P_ARRSHOW) < sx ||
							 (psz * P_ARRSHOW) < sy ||
							 (psz * P_ARRSHOW) < 10.0f) ))
						// Flag that it was shown
						pai->ai_flags |= AF_OFFSCREENARROW;

					// Invalid X and/or Y; check Y
					if (sy > psz) {
						// Invalid Y, maybe X too; set Z limit
						if (psy > 0)
							// Top side; set maximum Z; <fabs> is to stop new <psz>
							//  from going negative (this would be bad); <psz> gets
							//  flaky near the top of the screen where the divisor
							//  tends to get a little small
							psz = fabs((psy * distance) / (psy + SK * (distance - psz)));
						else
							// Bottom side; set minimum Z; <fabs> is to stop new <psz>
							//  from going negative (this would be bad); <psz> gets
							//  flaky near the top of the screen where the divisor
							//  tends to get a little small
							psz = fabs((psy * distance) / (psy - SK * (distance - psz)));
					}

					// Do & check coordinate ranges to determine any arrow rotation
					psx = psx/psz*hres;
					psy = psy/psz*hres;

					if (fabs(psy) > fabs(psx)) {
						// Bad Y only; not past left or right side
						arr_obj->z_angle = (sx = psx/psy) * DG2RD( 45.0f);

						if (psy < 0 ) {
							// Off the bottom only
							arr_obj->x = (hres/2) - (vres/2-YBORD) * sx;
//#ifdef DAN
//							if (arr_obj->y > (YBORD)) {
//								printf("[[-- Check this --]]\n");
//								//lockup();
//							}
//#endif
							arr_obj->y = (YBORD);
						//	arr_obj->z_angle += (0);
						}
						else {
							// Off the top only
							arr_obj->x = (hres/2) + (vres/2-YBORD) * sx;
//#ifdef DAN
//							if (arr_obj->y < (vres-YBORD)) {
//								printf("[[-- Check this --]]\n");
//								//lockup();
//							}
//#endif
							arr_obj->y = (vres-YBORD);
							arr_obj->z_angle += DG2RD(180.0f);
						}
					}
					else {
						// Bad X & maybe Y as well
						arr_obj->z_angle = (sx = psy/psx) * DG2RD(-45.0f);

						if (psx < 0 ) {
							// Off the left at least
							arr_obj->x = (XBORD);
							arr_obj->y = (vres/2) - (hres/2-XBORD) * sx;
							arr_obj->z_angle += DG2RD( 90.0f);
						}
						else {
							// Off the right at least
							arr_obj->x = (hres-XBORD);
							arr_obj->y = (vres/2) + (hres/2-XBORD) * sx;
							arr_obj->z_angle += DG2RD(270.0f);
						}

						if (arr_obj->y < (YBORD) || arr_obj->y > (vres-YBORD)) {
							if (psy < 0 ) {
								// Off the bottom as well
								arr_obj->x = (hres/2) - (vres/2-YBORD) * psx/psy;
//#ifdef DAN
//							if (arr_obj->y > (YBORD)) {
//								printf("[[-- Check this --]]\n");
//								//lockup();
//							}
//#endif
								arr_obj->y = (YBORD);
							}
							else {
								// Off the top as well
								arr_obj->x = (hres/2) + (vres/2-YBORD) * psx/psy;
//#ifdef DAN
//							if (arr_obj->y < (vres-YBORD)) {
//								printf("[[-- Check this --]]\n");
//								//lockup();
//							}
//#endif
								arr_obj->y = (vres-YBORD);
							}
						}
					}
					// Adjust ani pt
					if (!(arr_obj->ii->ay)) {
						arr_obj->ii->ay = pai->arr_ay;
						num_obj->ii->ax = pai->num_ax;
						num_obj->ii->ay = pai->num_ay;
					}
				}
				// Can we show above head arrow?
				// Can if the player is on-screen
				//   and active timer
				//   or GM_PRE_SNAP or GM_LININIG_UP or GM_PRE_KICKOFF
				else if (pai->ai_flags & AF_ONSCREEN && (
						 pai->ai_flags & AF_OFFSCREENARROW ||
						 pai->timer ||
						 game_info.game_mode == GM_PRE_SNAP ||
						 game_info.game_mode == GM_LINING_UP ||
						 game_info.game_mode == GM_PRE_KICKOFF))
				{
					// Set timer to show arrow if player just came back on-screen
					if (pai->ai_flags & AF_OFFSCREENARROW)
					{
						int j = (game_info.game_quarter) ? CNT_POFFON : CNT_POFFON_Q1;
						if (pai->timer < j)
							pai->timer = j;

						pai->ai_flags &= ~AF_OFFSCREENARROW;
					}

#ifdef SHOW3DARROWS
					// We wanna see'em
					pai->show3d = 1;
#else
					// We wanna see'em
					pai->show = 1;

					// Adjust & reposition the arrow sprite
					if (!(game_info.ball_carrier < 0 &&
							game_info.game_mode == GM_IN_PLAY &&
							ball_obj.type == LB_PASS &&
							FIELDX(ball_obj.tx) >= game_info.los) &&
							!(game_info.game_flags & GF_FWD_PASS) &&
							ppdata->team ==
							((!gstart) ? game_info.off_init ^ 1:game_info.off_side))
					{
						// Offense arrow
						if ((!gstart) ? game_info.off_init ^ 1:game_info.off_side)
						{
							arr_obj->z_angle = DG2RD(90);
							num_obj->ii->ax = pai->num_ax + pai->arr_ay;
							psx += OFF_ARR_DX;
						}
						else
						{
							arr_obj->z_angle = DG2RD(270);
							num_obj->ii->ax = pai->num_ax - pai->arr_ay;
							psx -= OFF_ARR_DX;
						}
						arr_obj->ii->ay = 0.0f;
						num_obj->ii->ay = pai->num_ay;
						psy += OFF_ARR_DY;
					}
					else
					{
						// Defense arrow
						arr_obj->z_angle = 0;
						arr_obj->ii->ay = 0.0f;
						num_obj->ii->ax = pai->num_ax;
						num_obj->ii->ay = pai->num_ay - pai->arr_ay;
						psy += DEF_ARR_DY;
					}
					arr_obj->x = (hres/2) + psx/psz*hres*FOCFAC;
					arr_obj->y = (vres/2) + (psy + ppdata->odata.y)/psz*hres*FOCFAC;
#endif
				}
			}

			if (pai->show < 0)
			{
				// Don't show the sprite arrow
				hide_sprite(arr_obj);
				hide_sprite(num_obj);
//				continue;
			}
			else if (pai->show > 0)
			{
				// Show the sprite arrow
				unhide_sprite(arr_obj);
				unhide_sprite(num_obj);
			}

			// Player # goes with arrow
			num_obj->x = arr_obj->x = (float)(int)arr_obj->x;
			num_obj->y = arr_obj->y = (float)(int)arr_obj->y;

			if (is_low_res)
			{
				switch ((int)(arr_obj->y) % 3)
				{
					case 0:
						num_obj->y = (arr_obj->y += 0.5f);	//  0.0 + 0.5
						break;

					case 1:
						num_obj->y = (arr_obj->y -= 0.5f);	// -1.0 + 0.5
						break;

					case 2:
						//num_obj->y = (arr_obj->y -= 0.0f);	// -0.5 + 0.5
						break;
				}
			}

			// Update positions
			generate_sprite_verts(arr_obj);
			generate_sprite_verts(num_obj);
		} while ((pai->pnum_last = pai->pnum), ++pai, ++i < (four_plr_ver ? MAX_PLYRS : MAX_PLYRS-2));
//if (get_player_sw_current() & P4_D)
//fprintf(stderr,"\r\n");

		// Go let other guys play now
		sleep(1);
	}
}

// This undef MUST be done here!
#undef vres


//	if (!presnap)
//		ifdef stuff
//		if (!inplay)
//			hide'em
//			continue;
//		inplay
//	else
//		presnap
//
//	update'em
/*
#define DIVITA	1
			// Invalid X and/or Y; check Y
			if (sy <= psz) {
				// Valid Y; set default angle
				if (psx > 0)
					// Right side
					arr_obj->z_angle = DG2RD(270.0f);
				else
					// Left side
					arr_obj->z_angle = DG2RD( 90.0f);
			}
			// Invalid Y, maybe X too; set Z limit & default angle
			else if (psy > 0) {
				// Top side; set maximum Z; <fabs> is to stop new <psz>
				//  from going negative (this would be bad); <psz> gets
				//  flaky near the top of the screen where the divisor
				//  tends to get a little small
				psz = fabs((psy * distance) / (psy + SK * (distance - psz)));
				arr_obj->z_angle = DG2RD(180.0f);
			}
			else {
				// Bottom side; set minimum Z; <fabs> is to stop new <psz>
				//  from going negative (this would be bad); <psz> gets
				//  flaky near the top of the screen where the divisor
				//  tends to get a little small
				psz = fabs((psy * distance) / (psy - SK * (distance - psz)));
				arr_obj->z_angle = 0;
			}

			// Do & check coordinate ranges to determine any arrow rotation
			arr_obj->x = psx/psz*hres + (hres/2);
			arr_obj->y = psy/psz*hres + (vres/2);

			sx = 0;
			if ((psx = arr_obj->x - XBORD) < 0) {
				sx = psx;
				arr_obj->x = XBORD;
			}
			if ((psx = arr_obj->x - (hres-XBORD)) > 0) {
				sx = psx;
				arr_obj->x = (hres-XBORD);
			}

			sy = 0;
			if ((psy = arr_obj->y - YBORD) < 0) {
				sy = psy;
				arr_obj->y = YBORD;
			}
			if ((psy = arr_obj->y - (vres-YBORD)) > 0) {
				sy = psy;
				arr_obj->y = (vres-YBORD);
			}

			if (sx && sy) {
				// Need to rotate the arrow
				if (fabs(sx) >= fabs(sy)) {
					arr_obj->z_angle = (M_PI/2) - (sy / sx) * DG2RD( 45.0f);
					if (sx > 0)
						arr_obj->z_angle += DG2RD(180.0f);
				}
				else {
					arr_obj->z_angle =   (M_PI) + (sx / sy) * DG2RD( 45.0f);
					if (sy < 0)
						arr_obj->z_angle += DG2RD(180.0f);
				}
			}
*/

/******************************************************************************
** plyrinfo_init() - Starts player info bars
*/

//image_info_t * arrow_imgs[] = {&arrow_blue,&arrow_num1,&arrow_red,&arrow_num2};

void plyrinfo_init(void)
{
	int was_low_res = is_low_res;
	register int i = 0;
	register int j;

	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	do {
		register pdata_t *      ppb = &pdata_blocks[i];
		register image_info_t * pii = &ii_turbo[i];

		// misc inits; probably dispose of or at least move
		ppb->turbo = 100.0f;				// should(?) be 0% to 100% left
		ppb->turbo_last = -1.0f;			// ensure turbo bar update

		// init
		ppb->si_tmsg = NULL;

		// make turbo bar sprites
		ppb->si_frame = beginobj(&tb_empty,
			pintx_turbo[four_plr_ver][i], pinty_turbo[!!was_low_res], 11.0f, SCR_PLATE_TID);

		ppb->si_turbo = beginobj(pii_turbo[!!was_low_res][i],
			pintx_turbo[four_plr_ver][i], pinty_turbo[!!was_low_res], 10.0f, SCR_PLATE_TID);

		// make FIRE images under turbo bar
		ppb->fframe = 0;									// img nbr.
		ppb->fdelay = FIRE_FDELAY;									// 2 ticks per frame
		ppb->si_fframe = beginobj(trb_bar_fire_imgs[ppb->fframe],
			pintx_turbo[four_plr_ver][i], pinty_turbo[!!was_low_res]+(was_low_res ? 7.0f : 0.0f), 11.5f + i, SCR_PLATE_TID);
		ppb->si_fframe->w_scale = 2.0f;
		ppb->si_fframe->h_scale = 2.0f;
		generate_sprite_verts(ppb->si_fframe);

		ppb->si_frame->id = OID_TURBARROW;
		ppb->si_turbo->id = OID_TURBARROW;
		ppb->si_fframe->id = OID_TURBARROW;

		// create & substitute the dynamic image info
		*pii = *pii_turbo[!!was_low_res][i];
		ppb->si_turbo->ii = pii;
		ppb->turbo_w = pii->w;

		// assume need to be hidden
		hide_sprite(ppb->si_frame);
		hide_sprite(ppb->si_turbo);
		hide_sprite(ppb->si_fframe);

		//make turbo bar credits sprites
		pii = pfi_tbcred[!!was_low_res]->characters[0];
		j = TBCRED_DIGCNT;
		while (j--)
		{
			psi_tbcred[i][j] = beginobj(pii, 0.0f, 0.0f, TBCRED_Z, 0);
			psi_tbcred[i][j]->id = OID_TURBARROW;
			hide_sprite(psi_tbcred[i][j]);
		}
	} while (++i < (four_plr_ver ? MAX_PLYRS : MAX_PLYRS-2));

	// Back it goes!!!
	is_low_res = was_low_res;

	//make the off-screen arrow objs
	// player 1
	arrowinfo[0].arr_obj = beginobj(&arrow_blue, 0.0f, 50.0f, 2.12f, SCR_PLATE_TID);
	ii_arr[0] = *((sprite_info_t *)arrowinfo[0].arr_obj)->ii;
	((sprite_info_t *)arrowinfo[0].arr_obj)->ii = &ii_arr[0];
	((sprite_info_t *)arrowinfo[0].arr_obj)->id = OID_TURBARROW;
	arrowinfo[0].arr_ax = ii_arr[0].ax;
	arrowinfo[0].arr_ay = ii_arr[0].ay;

	arrowinfo[0].num_obj = beginobj(&arrow_num1, 0.0f, 50.0f, 2.11f, SCR_PLATE_TID);
	ii_num[0] = *((sprite_info_t *)arrowinfo[0].num_obj)->ii;
	((sprite_info_t *)arrowinfo[0].num_obj)->ii = &ii_num[0];
	((sprite_info_t *)arrowinfo[0].num_obj)->id = OID_TURBARROW;
	arrowinfo[0].num_ax = ii_num[0].ax;
	arrowinfo[0].num_ay = ii_num[0].ay;

	arrowinfo[0].pnum = arrowinfo[0].pnum_last = arrowinfo[0].show = -1;
	arrowinfo[0].ai_flags = 0;

	// player 2
	arrowinfo[1].arr_obj = beginobj(&arrow_green,  0.0f, 50.0f, 2.16f, SCR_PLATE_TID);
	ii_arr[1] = *((sprite_info_t *)arrowinfo[1].arr_obj)->ii;
	((sprite_info_t *)arrowinfo[1].arr_obj)->ii = &ii_arr[1];
	((sprite_info_t *)arrowinfo[1].arr_obj)->id = OID_TURBARROW;
	arrowinfo[1].arr_ax = ii_arr[1].ax;
	arrowinfo[1].arr_ay = ii_arr[1].ay;

	arrowinfo[1].num_obj = beginobj(&arrow_num2, 0.0f, 50.0f, 2.15f, SCR_PLATE_TID);
	ii_num[1] = *((sprite_info_t *)arrowinfo[1].num_obj)->ii;
	((sprite_info_t *)arrowinfo[1].num_obj)->ii = &ii_num[1];
	((sprite_info_t *)arrowinfo[1].num_obj)->id = OID_TURBARROW;
	arrowinfo[1].num_ax = ii_num[1].ax;
	arrowinfo[1].num_ay = ii_num[1].ay;

	arrowinfo[1].pnum = arrowinfo[1].pnum_last = arrowinfo[1].show = -1;
	arrowinfo[1].ai_flags = 0;

	if (four_plr_ver)
	{
		// player 3
		arrowinfo[2].arr_obj = beginobj(&arrow_red,  0.0f, 50.0f, 2.14f, SCR_PLATE_TID);
		ii_arr[2] = *((sprite_info_t *)arrowinfo[2].arr_obj)->ii;
		((sprite_info_t *)arrowinfo[2].arr_obj)->ii = &ii_arr[2];
		((sprite_info_t *)arrowinfo[2].arr_obj)->id = OID_TURBARROW;
		arrowinfo[2].arr_ax = ii_arr[2].ax;
		arrowinfo[2].arr_ay = ii_arr[2].ay;

		arrowinfo[2].num_obj = beginobj(&arrow_num3, 0.0f, 50.0f, 2.13f, SCR_PLATE_TID);
		ii_num[2] = *((sprite_info_t *)arrowinfo[2].num_obj)->ii;
		((sprite_info_t *)arrowinfo[2].num_obj)->ii = &ii_num[2];
		((sprite_info_t *)arrowinfo[2].num_obj)->id = OID_TURBARROW;
		arrowinfo[2].num_ax = ii_num[2].ax;
		arrowinfo[2].num_ay = ii_num[2].ay;

		arrowinfo[2].pnum = arrowinfo[2].pnum_last = arrowinfo[2].show = -1;
		arrowinfo[2].ai_flags = 0;


		// player 4
		arrowinfo[3].arr_obj = beginobj(&arrow_yellow,  0.0f, 50.0f, 2.18f, SCR_PLATE_TID);
		ii_arr[3] = *((sprite_info_t *)arrowinfo[3].arr_obj)->ii;
		((sprite_info_t *)arrowinfo[3].arr_obj)->ii = &ii_arr[3];
		((sprite_info_t *)arrowinfo[3].arr_obj)->id = OID_TURBARROW;
		arrowinfo[3].arr_ax = ii_arr[3].ax;
		arrowinfo[3].arr_ay = ii_arr[3].ay;

		arrowinfo[3].num_obj = beginobj(&arrow_num4, 0.0f, 50.0f, 2.17f, SCR_PLATE_TID);
		ii_num[3] = *((sprite_info_t *)arrowinfo[3].num_obj)->ii;
		((sprite_info_t *)arrowinfo[3].num_obj)->ii = &ii_num[3];
		((sprite_info_t *)arrowinfo[3].num_obj)->id = OID_TURBARROW;
		arrowinfo[3].num_ax = ii_num[3].ax;
		arrowinfo[3].num_ay = ii_num[3].ay;

		arrowinfo[3].pnum = arrowinfo[3].pnum_last = arrowinfo[3].show = -1;
		arrowinfo[3].ai_flags = 0;
	}

// Tell the gnd arrows where to look
//	ptn_gndarrow = ((sprite_info_t *)arrowinfo[0].arr_obj)->tn;

	// Init various things
	showhide_flag = 1;

	// start the main line & arrow display procs
	qcreate( "plrinfo", PLYRINFO_PID, plyrinfo_proc, 0, 0, 0, 0 );
	qcreate( "parrows", PLYRINFO_PID, update_arrows_proc, 0, 0, 0, 0 );
}

//-------------------------------------------------------------------------------------------------
// 					This process displays the score plate at the bottom of screen after each score and end
//					of periods
//-------------------------------------------------------------------------------------------------
void score_plate_proc(int *args)
{
	sprite_info_t *scrplt_obj;
	float fz;
	int i;
	int was_low_res = is_low_res;


	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	for (i = 0, fz = 1.5f; i <= 6; i++, fz -= 0.01f)
	{
		if (i < 6 || game_info.game_quarter < 4)
		{
			scrplt_obj = beginobj(
				sp_imgs[i][!!was_low_res],
				scrplt_xys[i][!!was_low_res][X_VAL],
				scrplt_xys[i][!!was_low_res][Y_VAL],
				fz,
				SCR_PLATE_TID);
			scrplt_obj->id = OID_SCRPLATE;

			if (i == 0)			// only first piece is flipped
			{
				scrplt_obj = beginobj(
					sp_imgs[0][!!was_low_res],
					scrplt_xys[17][!!was_low_res][X_VAL],
					scrplt_xys[17][!!was_low_res][Y_VAL],
					fz,
					SCR_PLATE_TID);
				scrplt_obj->mode = FLIP_TEX_V;
				generate_sprite_verts(scrplt_obj);
				scrplt_obj->id = OID_SCRPLATE;
			}
//			scrplt_obj->id = OID_SCRPLATE;
		}
	}
	scrplt_obj = beginobj(
		qtr_imgs[game_info.game_quarter][!!was_low_res],
		scrplt_xys[7][!!was_low_res][X_VAL],
		scrplt_xys[7][!!was_low_res][Y_VAL],
		fz,
		SCR_PLATE_TID);
	scrplt_obj->id = OID_SCRPLATE;

	// Back it goes!!!
	is_low_res = was_low_res;

	// print game time
	set_text_font(FONTID_CLOCK12);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_string_id(0x09);

	set_text_position(
		scrplt_xys[8][!!was_low_res][X_VAL],
		scrplt_xys[8][!!was_low_res][Y_VAL],
		1.2f);
	oprintf( "%dj%dc %d", (HJUST_CENTER|VJUST_CENTER), WHITE, ((game_info.game_time & 0x00FF0000)>>16));

	set_text_position(
		scrplt_xys[9][!!was_low_res][X_VAL],
		scrplt_xys[9][!!was_low_res][Y_VAL],
		1.2f);
	oprintf( "%dj%dc %02d", (HJUST_CENTER|VJUST_CENTER), WHITE, ((game_info.game_time & 0x0000FF00)>>8));

	// Back it goes!!!
//	is_low_res = was_low_res;


	// draw NFL logo
	scrplt_obj = beginobj(
		&sp_logo,
		scrplt_xys[10][!!was_low_res][X_VAL],
		scrplt_xys[10][!!was_low_res][Y_VAL],
		1.3f,
		SCR_PLATE_TID);
	scrplt_obj->id = OID_SCRPLATE;

	// draw shadow backdrops
	scrplt_obj = beginobj(
		&sp_trns,
		scrplt_xys[11][!!was_low_res][X_VAL],
		scrplt_xys[11][!!was_low_res][Y_VAL],
		1.7f,
		SCR_PLATE_TID);
	scrplt_obj->id = OID_SCRPLATE;
	scrplt_obj->w_scale = TRNSW_ADJ;		// Stretch the width
	scrplt_obj->h_scale = TRNSH_ADJ;		// Stretch the height
	generate_sprite_verts(scrplt_obj);

	scrplt_obj = beginobj(
		&sp_trnsend,
		scrplt_xys[12][!!was_low_res][X_VAL],
		scrplt_xys[12][!!was_low_res][Y_VAL],
		1.7f,
		SCR_PLATE_TID);
	scrplt_obj->id = OID_SCRPLATE;
	scrplt_obj->h_scale = TRNSH_ADJ;		// Stretch the height
	generate_sprite_verts(scrplt_obj);
	
	// draw city names & scores
	set_text_font(FONTID_BAST25);

	set_text_position(
		scrplt_xys[13][!!was_low_res][X_VAL],
		scrplt_xys[13][!!was_low_res][Y_VAL],
		1.2f);
//	cty_name1 = 
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, teaminfo[game_info.team[0]].home);

	set_text_position(
		scrplt_xys[15][!!was_low_res][X_VAL],
		scrplt_xys[15][!!was_low_res][Y_VAL],
		1.2f);
//	cty_name2 = 
	oprintf("%dj%dc%s", (HJUST_LEFT|VJUST_CENTER), LT_YELLOW, teaminfo[game_info.team[1]].home);

	set_text_position(
		scrplt_xys[14][!!was_low_res][X_VAL],
		scrplt_xys[14][!!was_low_res][Y_VAL],
		1.2f);
//	cty_scr1 = 
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, compute_score(0));

	set_text_position(
		scrplt_xys[16][!!was_low_res][X_VAL],
		scrplt_xys[16][!!was_low_res][Y_VAL],
		1.2f);
//	cty_scr2 = 
	oprintf("%dj%dc%d", (HJUST_RIGHT|VJUST_CENTER), LT_YELLOW, compute_score(1));

	showhide_pinfo(0);
}

//-------------------------------------------------------------------------------------------------
// 					This process shows the yardage info at the begining of a play.
//-------------------------------------------------------------------------------------------------
void yardage_info_plate(int *args)
{
	sprite_info_t *scrplt_obj;
//	ostring_t *yard_info1;
//	ostring_t *yard_info2;
//	float fz;
	int i;
	int was_low_res = is_low_res;

	if (game_info.team_play[game_info.off_side]->flags & (PLF_PUNT | PLF_FIELD_GOAL | PLF_FAKE_FG | PLF_FAKE_PUNT))
		return;


	// Make sure you put this back before sleeping!!!
	is_low_res = 0;

	scrplt_obj = beginobj(
		yrdplt_imgs[0][!!was_low_res],
		yrdplt_xys[0][!!was_low_res][X_VAL],
		yrdplt_xys[0][!!was_low_res][Y_VAL],
		1.2f,
		SCR_PLATE_TID);
	scrplt_obj->mode = FLIP_TEX_H;
	generate_sprite_verts(scrplt_obj);
	scrplt_obj->id = OID_SCRPLATE;

	for (i = 1; i < 4; i++)
	{
		scrplt_obj = beginobj(
				yrdplt_imgs[i][!!was_low_res],
				yrdplt_xys[i][!!was_low_res][X_VAL],
				yrdplt_xys[i][!!was_low_res][Y_VAL],
				1.2f,
				SCR_PLATE_TID);
		scrplt_obj->id = OID_SCRPLATE;
	}

//	scrplt_obj = beginobj(
//			qtr_imgs[game_info.game_quarter][!!was_low_res],
//			yrdplt_xys[7][!!was_low_res][X_VAL],
//			yrdplt_xys[7][!!was_low_res][Y_VAL],
//			fz,
//			SCR_PLATE_TID);
//	scrplt_obj->id = OID_SCRPLATE;

	// Back it goes!!!
	is_low_res = was_low_res;

//	// print game time
//	set_text_font(FONTID_CLOCK12);
//	set_text_transparency_mode(TRANSPARENCY_ENABLE);
//	set_string_id(0x09);

//	set_text_position(
//			yrdplt_xys[8][!!was_low_res][X_VAL],
//			yrdplt_xys[8][!!was_low_res][Y_VAL],
//			1.2f);
//	oprintf( "%dj%dc %d", (HJUST_CENTER|VJUST_CENTER), WHITE, ((game_info.game_time & 0x00FF0000)>>16));

//	set_text_position(
//			yrdplt_xys[9][!!was_low_res][X_VAL],
//			yrdplt_xys[9][!!was_low_res][Y_VAL],
//			1.2f);
//	oprintf( "%dj%dc %02d", (HJUST_CENTER|VJUST_CENTER), WHITE, ((game_info.game_time & 0x0000FF00)>>8));

//	// draw TEAM logo
//	scrplt_obj = beginobj(
//			teaminfo[game_info.team[game_info.off_side]].pii_logo,
//			yrdplt_xys[10][!!was_low_res][X_VAL],
//			yrdplt_xys[10][!!was_low_res][Y_VAL],
//			1.4F,
//			PLAY_SELECT_TID);
//	scrplt_obj->id = OID_SCRPLATE;
//	generate_sprite_verts(scrplt_obj);
	
//	// draw shadow backdrops
//	scrplt_obj = beginobj(
//			&sp_trns,
//			yrdplt_xys[11][!!was_low_res][X_VAL],
//			yrdplt_xys[11][!!was_low_res][Y_VAL],
//			1.7f,
//			SCR_PLATE_TID);
//	scrplt_obj->id = OID_SCRPLATE;
//	scrplt_obj->w_scale = TRNSW_ADJ;		// Stretch the width
//	scrplt_obj->h_scale = TRNSH_ADJ;		// Stretch the height
//	generate_sprite_verts(scrplt_obj);

//	scrplt_obj = beginobj(
//			&sp_trnsend,
//			yrdplt_xys[12][!!was_low_res][X_VAL],
//			yrdplt_xys[12][!!was_low_res][Y_VAL],
//			1.7f,
//			SCR_PLATE_TID);
//	scrplt_obj->id = OID_SCRPLATE;
//	scrplt_obj->h_scale = TRNSH_ADJ;		// Stretch the height
//	generate_sprite_verts(scrplt_obj);
	
	// print down & to-go
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_string_id(0x09);

	if (is_low_res)
		set_text_font(FONTID_BAST23);
	else
		set_text_font(FONTID_BAST18);

	set_text_position(
			yrdplt_xys[4][!!was_low_res][X_VAL],
			yrdplt_xys[4][!!was_low_res][Y_VAL],
			1.19f);
	if (game_info.first_down == 100)
		oprintf("%dj%dc%s AND GOAL",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,downstr[game_info.down-1]);
	else
		oprintf("%dj%dc%s AND %d",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,downstr[game_info.down-1],game_info.first_down - game_info.los);

	// draw current los
//	set_text_font(FONTID_BAST18);

//	set_text_position(
//			yrdplt_xys[14][!!was_low_res][X_VAL],
//			yrdplt_xys[14][!!was_low_res][Y_VAL],
//			1.2f);
//	if ((i = game_info.los) > 50) i = 100 - i;
//	oprintf("%dj%dcON THE %d",
//			(HJUST_CENTER|VJUST_CENTER),
//			LT_YELLOW,
//			i);

	showhide_pinfo(0);
	for( i = 0; i < tsec*4; i++ )
	{
		sleep(1);
		if (game_info.game_mode == GM_IN_PLAY)
			break;
	}
	showhide_pinfo(1);
	
	del1c(OID_SCRPLATE,0xffffffff);
	delete_multiple_strings(0x09, 0xffffffff);

//	qcreate("message", MESSAGE_PID, show_end_quarter, 0, 0, 0, 0);
	
//	qcreate("message", MESSAGE_PID, show_message_proc, MS_TOUCHDOWN, 0, 0, 0);
//	qcreate("message", 0, display_fumble_proc, 0, 0, 0, 0);
}


ostring_t	*pclock = (ostring_t *)0;
ostring_t	*pteam0 = (ostring_t *)0;
ostring_t	*pteam1 = (ostring_t *)0;
ostring_t	*pscore0 = (ostring_t *)0;
ostring_t	*pscore1 = (ostring_t *)0;
ostring_t	*pqtr = (ostring_t *)0;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void showhide_status_box(int flag)
{
	if (!flag)
	{
		hide_status_box = 1;
		hide_sprite(status_box_obj);
		hide_string( pclock );
		hide_string( pteam0 );
		hide_string( pteam1 );
		hide_string( pscore0 );
		hide_string( pscore1 );
		hide_string( pqtr );
//		delete_multiple_strings( SID_TLCLOCK, 0xffffffff );
//		delete_multiple_strings( SID_TLSCORES, 0xffffffff );
	}
	else
	{
		hide_status_box = 0;
		unhide_sprite(status_box_obj);
		unhide_string( pclock );
		unhide_string( pteam0 );
		unhide_string( pteam1 );
		unhide_string( pscore0 );
		unhide_string( pscore1 );
		unhide_string( pqtr );
//		update_status_box = 1;
	}
}

//-------------------------------------------------------------------------------------------------
// 					This process updates the status display at the top of the screen
//-------------------------------------------------------------------------------------------------
void score_status_box(int *args)
{
	int		tmp,color,delay;
	int		last_time;
	int		team_hilite = 0;
	int		dscore0 = -1, dscore1 = -1;
	int		color0 = -1, color1 = -1, tc0, tc1;

	pclock = (ostring_t *)0;
	pteam0 = (ostring_t *)0;
	pteam1 = (ostring_t *)0;
	pscore0 = (ostring_t *)0;
	pscore1 = (ostring_t *)0;
	pqtr = (ostring_t *)0;

	hide_status_box = 0;						// dont hide...yet
	update_status_box = 1;						// print everything first time through

	// create the STATUS BOX object
	status_box_obj = beginobj(status_box_imgs[four_plr_ver],status_box_xys[0][four_plr_ver][X],status_box_xys[0][four_plr_ver][Y],MSG_Z+.02f,SCR_PLATE_TID);
	status_box_obj->id = OID_INFOBOX;		// 5

	if(is_low_res)
		{
			status_box_obj->h_scale = 1.10f;
			generate_sprite_verts(status_box_obj);
		}


	set_text_font(FONTID_BAST8T);
	set_string_id(SID_TLNAMES);

	// create the team names
	set_text_position(status_box_xys[1][four_plr_ver][X],
			status_box_xys[1][four_plr_ver][Y], MSG_Z);
	pteam0 = oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW,
			teaminfo[game_info.team[0]].abbrev);

	set_text_position( status_box_xys[3][four_plr_ver][X],
			status_box_xys[3][four_plr_ver][Y] - (is_low_res ? 2 : 0), MSG_Z);
	pteam1 = oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE,
			teaminfo[game_info.team[1]].abbrev);

	// create the quarter indicator
	// this process is always deleted between quarters, so this one never needs to be updated
	set_text_position(status_box_xys[5][four_plr_ver][X],
					  status_box_xys[5][four_plr_ver][Y] - (is_low_res ? 2 : 0),
					  MSG_Z);
	pqtr = oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, qtrstr[game_info.game_quarter]);

	
	// start the main loop
	delay = 8;
	color = 0;
	last_time = -1;
	while (1)
	{
		if (!hide_status_box)
		{	// new clock digits?
			if ((last_time & ~0xff) != (game_info.game_time & ~0xff))
			{	// wipe & redraw clock string
	  			set_text_font(FONTID_BAST8T);
//	  			delete_multiple_strings(SID_TLCLOCK, 0xFFFFFFFF);
				if (pclock) delete_string( pclock );
	  			set_string_id(SID_TLCLOCK);
				last_time = game_info.game_time;

	  			set_text_position(status_box_xys[6][four_plr_ver][X], status_box_xys[6][four_plr_ver][Y], MSG_Z);
				pclock = oprintf( "%dj%dc %d%s%d",
					(HJUST_CENTER|VJUST_CENTER),
					color ? LT_RED : WHITE,
					((game_info.game_time & 0x00FF0000)>>16),
					((game_info.game_time & 0x0000ff00) <= 0x0900) ? ":0" : ":",
					((game_info.game_time & 0x0000FF00)>>8));
			}

			// change color?
			if (((game_info.game_time & 0x00FFFF00) <= 0x00001f00) &&
				((game_info.game_quarter == 1) ||
				(game_info.game_quarter >= 3)))
			{
				if (delay-- == 0)
				{
					color ^=1;
					delay = 8;
					if( pclock )
						change_string_cc( pclock, color ? LT_RED : WHITE );
				}
			}
			else if (color)
			{
				color = 0;
				if( pclock )
					change_string_cc( pclock, WHITE );
			}

			// if team_hilite doesn't match off_side, update the team name & score colors
			tc0 = (game_info.play_type == PT_KICKOFF) ? game_info.off_side : !game_info.off_side;
			tc1 = !tc0;

			if ((tc0 != color0) || (tc1 != color1))
			{
				color0 = tc0;
				color1 = tc1;
				change_string_cc( pteam0, tc0 ? LT_YELLOW : WHITE );
				change_string_cc( pscore0, tc0 ? LT_YELLOW : WHITE );
				change_string_cc( pteam1, tc1 ? LT_YELLOW : WHITE );
				change_string_cc( pscore1, tc1 ? LT_YELLOW : WHITE );
			}

			// if dscores don't match current scores, delete & re-create score digits
			if (dscore0 != compute_score(0))
			{
				dscore0 = compute_score(0);
				if (pscore0) delete_string( pscore0 );
	  			set_text_font(FONTID_BAST8T);
  				set_string_id(SID_TLSCORES);
				set_text_position(status_box_xys[2][four_plr_ver][X], status_box_xys[2][four_plr_ver][Y], MSG_Z);
				pscore0 = oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), tc0 ? LT_YELLOW : WHITE, dscore0);
			}

			if (dscore1 != compute_score(1))
			{
				dscore1 = compute_score(1);
				if (pscore1) delete_string( pscore1 );
	  			set_text_font(FONTID_BAST8T);
  				set_string_id(SID_TLSCORES);
				tmp = (game_info.play_type == PT_KICKOFF) ? !team_hilite : team_hilite;
				set_text_position(status_box_xys[4][four_plr_ver][X],
					status_box_xys[4][four_plr_ver][Y] - (is_low_res ? 2 : 0), MSG_Z);
				pscore1 = oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), tc1 ? LT_YELLOW : WHITE, dscore1);
			}
#if 0				
  			// only update scores and qtr. nbr when told to
  			if (update_status_box)
  			{
	  			set_text_font(FONTID_BAST8T);
  				update_status_box = 0;
  		
  				delete_multiple_strings(SID_TLSCORES, 0xffffffff);
  				set_string_id(SID_TLSCORES);
  				
  				// print team names and scores
	
					tmp =	game_info.off_side;
					if (game_info.play_type == PT_KICKOFF)
						tmp = tmp ^ 1;
  				if (tmp)
  				{
  					// print team names (team 1 has ball)
  					set_text_position(status_box_xys[1][four_plr_ver][X], status_box_xys[1][four_plr_ver][Y], MSG_Z);
  					oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[game_info.team[0]].abbrev);

						if(is_low_res)
	  					set_text_position(status_box_xys[3][four_plr_ver][X], status_box_xys[3][four_plr_ver][Y]-2, MSG_Z);
  					else
	  					set_text_position(status_box_xys[3][four_plr_ver][X], status_box_xys[3][four_plr_ver][Y], MSG_Z);
						
						oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, teaminfo[game_info.team[1]].abbrev);
  					// print scores
  					set_text_position(status_box_xys[2][four_plr_ver][X], status_box_xys[2][four_plr_ver][Y], MSG_Z);
  					oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), WHITE, compute_score(0));
						if(is_low_res)
	  					set_text_position(status_box_xys[4][four_plr_ver][X], status_box_xys[4][four_plr_ver][Y]-2, MSG_Z);
  					else
	  					set_text_position(status_box_xys[4][four_plr_ver][X], status_box_xys[4][four_plr_ver][Y], MSG_Z);
  
						oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_score(1));
  				}
  				else	
  				{
  					// print team names (team 0 has ball)
  					set_text_position(status_box_xys[1][four_plr_ver][X], status_box_xys[1][four_plr_ver][Y], MSG_Z);
  					oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, teaminfo[game_info.team[0]].abbrev);

						if(is_low_res)
	  					set_text_position(status_box_xys[3][four_plr_ver][X], status_box_xys[3][four_plr_ver][Y]-2, MSG_Z);
						else
	  					set_text_position(status_box_xys[3][four_plr_ver][X], status_box_xys[3][four_plr_ver][Y], MSG_Z);
							
  					oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, teaminfo[game_info.team[1]].abbrev);
  					// print score
  					set_text_position(status_box_xys[2][four_plr_ver][X], status_box_xys[2][four_plr_ver][Y], MSG_Z);
  					oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_score(0));
						if(is_low_res)
	  					set_text_position(status_box_xys[4][four_plr_ver][X], status_box_xys[4][four_plr_ver][Y]-2, MSG_Z);
						else
	  					set_text_position(status_box_xys[4][four_plr_ver][X], status_box_xys[4][four_plr_ver][Y], MSG_Z);
							
  					oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), WHITE, compute_score(1));
  				}
  				
  				// print quarter number
					if(is_low_res)
	  				set_text_position(status_box_xys[5][four_plr_ver][X], status_box_xys[5][four_plr_ver][Y]-2, MSG_Z);
					else
	  				set_text_position(status_box_xys[5][four_plr_ver][X], status_box_xys[5][four_plr_ver][Y], MSG_Z);
						
  				oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, qtrstr[game_info.game_quarter]);
  			}			
#endif
		}
		sleep(1);
	}
}

//-------------------------------------------------------------------------------------------------
//	This process turns on sparkle on top of page # on play select
//
// 	INPUT:	x,y for sparkle
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void one_wide_flash_proc(int *args)
{
	sprite_info_t	*flsh_obj;
	int	i;
	float x,y,z;
	// args are always ints	 (MARK...just a note..remember to not pass in floats (always ints) then cast to floats,
	// 			if need to be used as floats
	x = (float)args[0];
	y = (float)args[1];
	z = (float)args[2];

	// All these sparkles are part of ingame00.wms texture
	flsh_obj = beginobj(sparkle_imgs[0],x,y,1.20f, SCR_PLATE_TID);
	flsh_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	flsh_obj->state.constant_color = 0Xff000000;
	flsh_obj->id = OID_MESSAGE;
	flsh_obj->w_scale = 6.0f;
	flsh_obj->h_scale = 3.0f;
	generate_sprite_verts(flsh_obj);

	for (i=0; i < (int)((sizeof(sparkle_imgs)/sizeof(image_info_t *))-1); i++)
	{
		change_img_tmu(flsh_obj,sparkle_imgs[i],SCR_PLATE_TID);
		sleep(3);
	}

	delobj(flsh_obj);
}

//-------------------------------------------------------------------------------------------------
//	This process turns on sparkles on big text messages
//
// 	INPUT:	x,y,z for sparkle
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void one_flash_proc(int *args)
{
	sprite_info_t	*flsh_obj;
	int	i,x,y;
	float z;
	// args are always ints
	x = args[0];
	y = args[1];
	z = (float)args[2];

	// All these sparkles are part of ingame00.wms texture
	if(is_low_res)
	{
		y *= 1.5f;
	}
	flsh_obj = beginobj(sparkle_imgs[0],x,y,z, SCR_PLATE_TID);
//	flsh_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
//	flsh_obj->state.constant_color = 0XFF000000;
	flsh_obj->id = OID_MESSAGE;
	flsh_obj->w_scale = 2.5f;
	flsh_obj->h_scale = 2.5f;
	generate_sprite_verts(flsh_obj);

	for (i=0; i < (int)((sizeof(sparkle_imgs)/sizeof(image_info_t *))-1); i++)
	{
		change_img_tmu(flsh_obj,sparkle_imgs[i],SCR_PLATE_TID);
		sleep(3);
	}

	delobj(flsh_obj);
}

//-------------------------------------------------------------------------------------------------
//	This process turns on a big message & scales it in
//
// 	INPUT:	Message # to display
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void show_message_proc(int *args)
{
	sprite_info_t *message_obj = NULL;
	sprite_info_t *message2_obj = NULL;
	texture_node_t *tex_node;
	int	columns,start_x,start_y,t,i,x,y;
	float z_flsh;

//	// Big message could be on screen
//	while (existp(MESSAGE_PID,0xFFFFFFFF))
//		sleep (1);
	
	t = args[0];
	// MS_ON_DOWNS is in same texture as MS_TURNOVER
	// These are the messages kept in memory all the time
	if (t != MS_ON_DOWNS && t != MS_PUNTRETURN && t != MS_1STDOWN && t != MS_INTERCEPTION &&
			t != MS_TURNOVER && t != MS_TOUCHDOWN && t != MS_4THDOWN && t != MS_TURNOVER2)
	{
		// Enable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info((void **)&cur_proc, suspend_self, resume_process);
#else
		dio_mode(DIO_MODE_NO_BLOCK);
#endif

		// Load in the mugshots for this team
		tex_node = create_texture(message_tex[t],
								MESSAGE_TID,
								0,
								CREATE_NORMAL_TEXTURE,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTUREFILTER_BILINEAR,
								GR_TEXTUREFILTER_BILINEAR);

#ifdef DEBUG
		fprintf(stderr, "loaded tex:%s  handle:%d\r\n" ,message_tex[t], (int)tex_node->texture_handle);
#endif

		if (!tex_node)
		{
#ifdef DEBUG
			lockup();
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}

		// Control will not reach this point until texture is loaded into TMU!
		// Process will wake up and then the beginobj can happen

		// Disable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif
	}

	if (t == MS_1STDOWN || t == MS_INTERCEPTION || t == MS_TURNOVER2 || t == MS_PUNTRETURN)
	{
		if (t != MS_PUNTRETURN)
		{
			message_obj = beginobj(message_imgs[t*2],SPRITE_HRES/2, SPRITE_VRES/2+55, 91.0F, MESSAGE_TID);
			message_obj->id = (t == MS_INTERCEPTION) ? 0x5678FACE : OID_MESSAGE;
			qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message_obj,4,4,0);

			message2_obj = beginobj(message_imgs[t*2+1],SPRITE_HRES/2, SPRITE_VRES/2+55, 91.0F, MESSAGE_TID);
			message2_obj->id = (t == MS_INTERCEPTION) ? 0x5678FACE : OID_MESSAGE;
			qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message2_obj,4,4,0);
		}
		if (t == MS_INTERCEPTION)
			intercepted_ball_sp();

//		snd_scall_bank(cursor_bnk_str,HELMET_SND,VOLUME2,127,PRIORITY_LVL5);

//		sleep(1);
//
//		for( i = 0; i < tsec*2; i++ )
//		{
//			sleep(1);
//			if (game_info.game_mode == GM_IN_PLAY)
//				break;
//		}
//		
		if (t == MS_INTERCEPTION || t == MS_TURNOVER2 || t == MS_PUNTRETURN)
		{
			sleep(100);

			if (t != MS_PUNTRETURN)
			{
				// Cleanup also happens at top of play_select in playsel.c	
				del1c( (t == MS_INTERCEPTION) ? 0x5678FACE : OID_MESSAGE, -1 );
				delete_multiple_textures( MESSAGE_TID, -1 );
			}
		}
		
		die(0);
	}
	if (t == MS_4THDOWN)
	{
		message_obj = beginobj(message_imgs[t*2],SPRITE_HRES/2, SPRITE_VRES/2+55, 91.0F, MESSAGE_TID);
		message_obj->id = OID_MESSAGE;
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message_obj,4,4,0);

		message2_obj = beginobj(message_imgs[t*2+1],SPRITE_HRES/2, SPRITE_VRES/2+55, 91.0F, MESSAGE_TID);
		message2_obj->id = OID_MESSAGE;
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message2_obj,4,4,0);

		snd_scall_bank(cursor_bnk_str,HELMET_SND,VOLUME2,127,PRIORITY_LVL5);

		sleep(1);

		for( i = 0; i < tsec*2; i++ )
		{
			sleep(1);
			if (game_info.game_mode == GM_IN_PLAY)
				break;
		}
		
		// Cleanup also happens at top of play_select in playsel.c	
		del1c(OID_MESSAGE,0xFFFFFFFF);
		delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);
		die(0);
	}
	else	
	{
		message_obj = beginobj(message_imgs[t*2],SPRITE_HRES/2, SPRITE_VRES/2+70, 91.0F, MESSAGE_TID);
		message_obj->w_scale = 0.2f;
		message_obj->h_scale = 0.2f;
		
		message_obj->id = OID_MESSAGE;
		
		if( message_imgs[t*2+1])
		{
			if (t == MS_TURNOVER)
				message2_obj = beginobj(message_imgs[t*2+1],SPRITE_HRES/2-1, SPRITE_VRES/2+70, 91.0F, MESSAGE_TID);
			else
				message2_obj = beginobj(message_imgs[t*2+1],SPRITE_HRES/2, SPRITE_VRES/2+70, 91.0F, MESSAGE_TID);
				
			message2_obj->w_scale = 0.20f;
			message2_obj->h_scale = 0.20f;
			message2_obj->id = OID_MESSAGE;
		}

		while (message_obj->w_scale < 1.8F)
		{
			message_obj->w_scale += .06f;
			message_obj->h_scale += .06f;
			generate_sprite_verts(message_obj);
			
			if( message2_obj)
			{
				message2_obj->w_scale += .06f;
				message2_obj->h_scale += .06f;
				generate_sprite_verts(message2_obj);
			}
			sleep(1);
		}

			while (message_obj->w_scale > 1.20F)
			{
				message_obj->w_scale -= .06f;
				message_obj->h_scale -= .06f;
				generate_sprite_verts(message_obj);
				
				if( message2_obj)
				{
					message2_obj->w_scale -= .06f;
					message2_obj->h_scale -= .06f;
					generate_sprite_verts(message2_obj);
				}
				sleep(1);
			}

		// Show sparkles?		
		t = args[0];
//		if ((t == MS_TOUCHDOWN) || (t == MS_2POINTS) || (t == MS_ITSGOOD))
		if (1)
		{
			columns = message_obj->verts[2].x - message_obj->verts[3].x;
			if( message2_obj)
				columns = columns + (message2_obj->verts[2].x - message2_obj->verts[3].x);
//			columns = (columns / 20); //45 15);
			columns = (columns / 30); //45 15);

			start_x = message_obj->verts[3].x;
			start_y = message_obj->verts[0].y;

			z_flsh = 90.0F;
				
			for (t=0; t < columns; t++)
			{
				for (i=0; i < 2; i++)
				{
					z_flsh = z_flsh -0.80F;
					x = start_x + randrng(15);
					y = start_y + randrng((message_obj->verts[3].y + 0) - message_obj->verts[0].y);
					qcreate("sparkle",MESSAGE_PID,one_flash_proc,x,y,(int)z_flsh,0);
				}
//				start_x = start_x + 20;
				start_x = start_x + 30;
				sleep(3);
			}
		}
		
		if (args[0] == MS_ON_DOWNS)
			sleep(120);
		else	
			sleep(60);

		// Message go away	
		
		while (message_obj->w_scale > 0.05F)
		{
	//		message_obj->z_angle = message_obj->z_angle + 0.05F;

	//		message_obj->y = message_obj->y + y_sweep;
	//		y_sweep = y_sweep,y_sweep

			message_obj->w_scale -= .10f;
			message_obj->h_scale -= .10f;
			generate_sprite_verts(message_obj);
			
			if( message2_obj)
			{
	//			message2_obj->z_angle = message2_obj->z_angle + 0.05F;
				message2_obj->w_scale -= .10f;
				message2_obj->h_scale -= .10f;
				generate_sprite_verts(message2_obj);
			}
			sleep(1);
		}

		// Cleanup also happens at top of play_select in playsel.c	
		del1c(OID_MESSAGE,0xFFFFFFFF);
		delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);
	}	
}

//---------------------------------------------------------------------------------------------------------------------------
//	This process prints the message fumble on screen and cleans it up
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
void display_fumble_proc(int *args)
{
	static sprite_info_t		*idiot_bx_obj;
	static float idiot_box_xys[2][2] = { {SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+152.0f},
										 {SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+152.0f} };		// [2plr/4plr][x,y]
	process_node_t * pflsh;
	process_node_t * pflsh1;
	int i;
	float x,y;

	// FRATRICIDE!  Kill any other fumble stuff
	del1c(OID_FUMBLE_MSG,0xFFFFFFFF);
	killall( MSG_FUMBLE_PID, 0xffffffff );
	delete_multiple_strings(FUMBLE_SID, 0xffffffff);
	
	x = idiot_box_xys[four_plr_ver][X_VAL];
	y = idiot_box_xys[four_plr_ver][Y_VAL];

	idiot_bx_obj = beginobj(&playresult, x, y, 1.4f, SCR_PLATE_TID);
	idiot_bx_obj->w_scale = 0.72f*1.10f;
	idiot_bx_obj->h_scale = 0.48f*1.20f;
	idiot_bx_obj->id = OID_FUMBLE_MSG;
	generate_sprite_verts(idiot_bx_obj);

	// Allow each sequential letter to overlap the previous letter - turn off at end!
	set_text_font(FONTID_BAST25);
	set_string_id(FUMBLE_SID);
	set_text_position(x, y, 1.001F);

	set_text_color(LT_RED);
	pmsg[0] = oprintf( "%djFUMBLE", (HJUST_CENTER|VJUST_CENTER));

	pflsh = qcreate("ftp6", MSG_FUMBLE_PID, flash_text_proc, (int)pmsg[0], 0, 0, 0);

	// Quickly flash idiot box white two times
	pflsh1 = qcreate("flash",MSG_FUMBLE_PID,flash_obj_proc,(int)idiot_bx_obj,3,2,0);

//		flash_obj_white(idiot_bx_obj,3);

//		sleep(2*tsec);

	// 	Wait until someone gets ball or it goes OB causing a play over or this times out
	i = tsec * 7;
	while (((game_info.ball_carrier == NOBALLCARRIER) && (game_info.game_mode != GM_PLAY_OVER)) &&
				(--i))
		sleep (1);

	delete_multiple_strings(FUMBLE_SID, 0xffffffff);
	delobj(idiot_bx_obj);
	del1c(OID_FUMBLE_MSG,0xFFFFFFFF);
	killall(MSG_FUMBLE_PID,0xFFFFFFFF);
//	kill(pflsh1, 0xaced);
//	kill(pflsh, 0xaced);
}

//---------------------------------------------------------------------------------------------------------------------------
//	This process prints the sack/double sack message on the result box
//
// 	INPUT:	# of consecutive sacks
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
#if 0
void show_sack_proc(int *args)
{
	static sprite_info_t		*idiot_bx_obj;
	static float idiot_box_xys[2][2] = {{285.0f,330.0f}, {315.0f,330.0f}};		// [2plr/4plr][x,y]
	int sack_cnt;
	process_node_t	*pflsh = NULL;
	
	sack_cnt = args[0];

	snd_scall_bank(plyr_bnk_str,SACK_SND,VOLUME2,127,PRIORITY_LVL3);

	if ((sack_cnt >= 4) || (game_info.down == 4) || (game_info.play_type == PT_EXTRA_POINT))
		die(0);

	// Code taken from NFL.c sack audit
	if (!( game_info.game_flags & GF_FWD_PASS) &&
			(game_info.off_side == game_info.off_init) &&
			!(game_info.game_flags & GF_BALL_PUNTED) &&
			(game_info.ball_carrier %7 == 3) &&
			!(game_info.game_flags & GF_BALL_CROSS_LOS))
	
	{
		// Sack
		idiot_bx_obj = beginobj(&playresult, idiot_box_xys[four_plr_ver][X_VAL], idiot_box_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
		idiot_bx_obj->id = OID_MESSAGE;
		idiot_bx_obj->w_scale = 1.32f;
		generate_sprite_verts(idiot_bx_obj);

		// Allow each sequential letter to overlap the previous letter - turn off at end!
		if (is_low_res)
			set_text_font(FONTID_BAST23LOW);
		else
			set_text_font(FONTID_BAST25);
			
		set_string_id(0x34);
		set_text_position(SPRITE_HRES/2.0f+60.0f,(SPRITE_VRES/2.0f)+28.0f+103.0f, 1.001F);
		set_text_color(LT_RED);

		if (sack_cnt == 1)
			pmsg[0] = oprintf( "%djQB SACK", (HJUST_CENTER|VJUST_CENTER));
		else if (sack_cnt == 2)
			pmsg[0] = oprintf( "%djDOUBLE SACK", (HJUST_CENTER|VJUST_CENTER));
		else if (sack_cnt == 3)
			pmsg[0] = oprintf( "%djTRIPLE SACK", (HJUST_CENTER|VJUST_CENTER));
	//	else if (sack_cnt >= 4)
	//		pmsg[0] = oprintf( "%djSUPER SACK", (HJUST_CENTER|VJUST_CENTER));


		pflsh = qcreate("ftp7", 0, flash_text_proc, (int)pmsg[0], 0, 0, 0);

		// Quickly flash idiot box white two times
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx_obj,3,2,0);

		wait_for_all_buttons(30, 90);
	//	sleep (110-20);

		delete_multiple_strings(0x34, 0xffffffff);
		delobj(idiot_bx_obj);
		kill(pflsh, 0);
	}
}
#endif

//---------------------------------------------------------------------------------------------------------------------------
//	This process prints the opening kickoff message
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
void opening_msg_proc(int *args)
{
	register int ireg;
	register char creg;
	register float freg;

	process_node_t	*pflsh = NULL;
	sprite_info_t * idiot_bx_obj = NULL;
	sprite_info_t * idiot_bx2_obj = NULL;
	sprite_info_t * idiot_bx3_obj = NULL;
	static float idiot_box_xys[2][2][2] = {				// [team][2plr/4plr][x,y]
		{ {128.0f,60.0f}, {128.0f,60.0f} },
		{ {384.0f,60.0f}, {384.0f,60.0f} }};

#ifdef DBG_KO
	return;
#endif //DBG_KO

	while(game_info.game_mode == GM_INITIALIZING)
	{
		sleep(1);
	}

	idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2, SPRITE_VRES/2+40, 1.4f, SCR_PLATE_TID);
//	idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2-118, SPRITE_VRES/2+119, 1.4f, SCR_PLATE_TID);
	idiot_bx_obj->w_scale = 1.00f;
	idiot_bx_obj->h_scale = 1.35f;
	idiot_bx_obj->id = OID_MESSAGE;
	generate_sprite_verts(idiot_bx_obj);

	// Display tournament mode message if needed
	if (pup_tourney)	// && p_status == 3)
	{
		snd_scall_bank(plyr_bnk_str,INTERCEPT_SND,VOLUME2,127,PRIORITY_LVL5);

		if (is_low_res)
			idiot_bx3_obj = beginobj(&playresult, SPRITE_HRES/2, SPRITE_VRES/2-55.0f, 1.3f, SCR_PLATE_TID);
		else
			idiot_bx3_obj = beginobj(&playresult, SPRITE_HRES/2, SPRITE_VRES/2-55.0f, 1.3f, SCR_PLATE_TID);
			
		idiot_bx3_obj->w_scale = 2.10f;
		idiot_bx3_obj->h_scale = 1.00f;
		idiot_bx3_obj->id = OID_MESSAGE;
		generate_sprite_verts(idiot_bx3_obj);

		// Quickly flash idiot box white two times
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx3_obj,5,3,0);

		// Allow each sequential letter to overlap the previous letter - turn off at end!
		if(is_low_res)
			set_text_font(FONTID_BAST23);		// Actually, bast23 equates to the real bast18
		else
			set_text_font(FONTID_BAST18);
			
		set_string_id(0x34);
		set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +69-112, 1.1f);								//1.0

		set_text_color(LT_RED);

 		oprintf( "%djTOURNAMENT MODE", (HJUST_CENTER|VJUST_CENTER));

		if(is_low_res)
		{
			set_text_font(FONTID_BAST10LOW);
			set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) -31+80-4-116, 1.1f);								//1.0
		}
		else
		{
			set_text_font(FONTID_BAST10);
			set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) -31+80-116, 1.1f);								//1.0
		}
			
		oprintf( "%djNO CPU ASSISTANCE, POWERUPS OR SECRETS ALLOWED", (HJUST_CENTER|VJUST_CENTER));

	}
	

	// Allow each sequential letter to overlap the previous letter - turn off at end!
	if(is_low_res)
		set_text_font(FONTID_BAST23);		// Actually, bast23 equates to the real bast18
	else
		set_text_font(FONTID_BAST18);
		
	set_string_id(0x34);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +69, 1.1f);								//1.0

	set_text_color(LT_GREEN);

	creg = '1';
	freg = -170.0f;
	ireg = game_info.off_side ^ 1;
	if (ireg)
	{
		creg = '2';
		freg = -freg;
	}

	if (four_plr_ver)
	 	pmsg[0] = oprintf( "%djTEAM %c", (HJUST_CENTER|VJUST_CENTER), creg);
	else
	 	pmsg[0] = oprintf( "%djPLAYER %c", (HJUST_CENTER|VJUST_CENTER), creg);

	if(is_low_res)
		set_text_font(FONTID_BAST10LOW);
	else
		set_text_font(FONTID_BAST10);
		
	if (is_low_res)
		set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) -31+80-8, 1.1f);								//1.0
	else
		set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) -31+80, 1.1f);								//1.0
	
	pmsg[1] = oprintf("%dj(%s)", (HJUST_CENTER|VJUST_CENTER), teaminfo[game_info.team[game_info.off_side ^ 1]].name);

	if (is_low_res)
		idiot_bx2_obj = beginobj(&playresult,
				idiot_box_xys[ireg][four_plr_ver][X_VAL],
				idiot_box_xys[ireg][four_plr_ver][Y_VAL]+14,
				1.4f,
				SCR_PLATE_TID);
	else
		idiot_bx2_obj = beginobj(&playresult,
				idiot_box_xys[ireg][four_plr_ver][X_VAL],
				idiot_box_xys[ireg][four_plr_ver][Y_VAL],
				1.4f,
				SCR_PLATE_TID);
			
	idiot_bx2_obj->w_scale = 0.60f;
	idiot_bx2_obj->h_scale = 0.60f;
	idiot_bx2_obj->id = OID_MESSAGE;
	generate_sprite_verts(idiot_bx2_obj);

	// Allow each sequential letter to overlap the previous letter - turn off at end!
	if (is_low_res)
		set_text_font(FONTID_BAST23);		// Actually, bast23 equates to the real bast18
	else
		set_text_font(FONTID_BAST18);

	set_string_id(0x34);
	if (is_low_res)
		set_text_position(idiot_box_xys[ireg][four_plr_ver][X_VAL], idiot_box_xys[ireg][four_plr_ver][Y_VAL]+13.0f, 1.1f);	//1.0
	else
		set_text_position(idiot_box_xys[ireg][four_plr_ver][X_VAL], idiot_box_xys[ireg][four_plr_ver][Y_VAL], 1.1f);	//1.0

	set_text_color(LT_GREEN);
	pmsg[3] = oprintf( "%djOFFENSE", (HJUST_CENTER|VJUST_CENTER));


	if (is_low_res)
		set_text_font(FONTID_BAST23);		// Actually, bast23 equates to the real bast18
	else
		set_text_font(FONTID_BAST18);

	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) -70+85, 1.1f);								//1.0
	pmsg[2] = oprintf( "%djRECEIVES BALL", (HJUST_CENTER|VJUST_CENTER));

	pflsh = qcreate("ftp8", 0, flash_text_proc, (int)pmsg[0], (int)pmsg[1], (int)pmsg[2], (int)pmsg[3]);

	// Quickly flash idiot box white two times
	qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx2_obj,8,4,0);

	wait_for_all_buttons(60, 5 * tsec);

#ifdef DEBUG
//	while (halt)
//		sleep (1);
#endif //DEBUG

	delete_multiple_strings(0x34, 0xffffffff);
	delobj(idiot_bx_obj);
	delobj(idiot_bx2_obj);
	if (idiot_bx3_obj)
		delobj(idiot_bx3_obj);
	kill(pflsh, 0);
	// added for '99
	killall(MESSAGE_PID,0xFFFFFFF0);
}

//-------------------------------------------------------------------------------------------------
//	This process turns on FREE word on top of extra point play at start of page 1
//
// 	INPUT:	x,y for FREE word
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void display_free_pt_proc(int *args)
{
	int	i;
	float x,y;

	if (!show_plysel_idiot_box)
	{
		x = (float)args[0];
		y = (float)args[1];

		killall(MESSAGE3_PID,0xFFFFFFFF);

		set_string_id(PLAY_SEL_MSG3_ID);
		set_text_font(FONTID_BAST18);
		if (is_low_res)
			set_text_position(x,y+5, 1.001F);
		else	
			set_text_position(x,y, 1.001F);
		set_text_color(LT_RED);
		pmsg[0] =	oprintf( "%djFREE",(HJUST_CENTER|VJUST_BOTTOM));

		pflasher = qcreate("ftp9", MESSAGE3_PID, flash_text_proc, (int)pmsg[0], 0, 0, 0);
	}
}

//-------------------------------------------------------------------------------------------------
//	This process turns on FG word on top of field goal play at start of page 1
//
// 	INPUT:	x,y for FG word
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void display_FG_proc(int *args)
{
	int	i;
	float x,y;

	x = (float)args[0];
	y = (float)args[1];

	killall(MESSAGE3_PID,0xFFFFFFFF);

	set_string_id(PLAY_SEL_MSG3_ID);
	set_text_font(FONTID_BAST18);
	if (is_low_res)
		set_text_position(x,y+5, 1.001F);
	else	
		set_text_position(x,y, 1.001F);
	set_text_color(LT_GREEN);
	pmsg[0] =	oprintf( "%dj3 PTS",(HJUST_CENTER|VJUST_BOTTOM));

	pflasher = qcreate("ftp10", MESSAGE3_PID, flash_text_proc, (int)pmsg[0], 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------
//	This process turns on PUNT word on top of punt play at start of page 1
//
// 	INPUT:	x,y for PUNT word
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void display_punt_proc(int *args)
{
	int	i;
	float x,y;

	x = (float)args[0];
	y = (float)args[1];

	killall(MESSAGE3_PID,0xFFFFFFFF);

	set_string_id(PLAY_SEL_MSG3_ID);
	set_text_font(FONTID_BAST18);
	if (is_low_res)
		set_text_position(x,y+5, 1.001F);
	else	
		set_text_position(x,y, 1.001F);
	set_text_color(LT_GREEN);
	pmsg[0] =	oprintf( "%djPUNT",(HJUST_CENTER|VJUST_BOTTOM));

	pflasher = qcreate("ftp11", MESSAGE3_PID, flash_text_proc, (int)pmsg[0], 0, 0, 0);
}


//-------------------------------------------------------------------------------------------------
//	This routine displays end of quarter message, score plate
//  and waits for buttons to be pressed or a timeout before returning
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void show_end_quarter_proc(int *args)
{
	ostring_t * pmesg;
	sprite_info_t *message_obj = NULL;
	sprite_info_t *message2_obj = NULL;
	sprite_info_t *message3_obj = NULL;
	sprite_info_t *message4_obj = NULL;
	texture_node_t *tex_node = NULL;
	int t = 0;
	int	columns,start_x,start_y,i,x,y;
	float z_flsh;

#ifndef NOSETCAM
//	extern CAMSEQINFO csi_zm_endquarter[];
//	select_csi_item( csi_zm_endquarter );

	select_csi_item( csi_fs_endqtr );
	if ((i = game_info.ball_carrier) < 0)
		 i = game_info.last_carrier;
	cambot.pmodeobj = &player_blocks[i].odata;
#endif

	// Big message could be on screen - timeout after 7 seconds - we've seen bug where period never ends!
	i = tsec * 6;
	while ((existp(MESSAGE_PID,0xFFFFFFFF)) && (--i))
		sleep(1);

	// Stop curent music
	snd_stop_track(SND_TRACK_0);
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	if (game_info.game_quarter == 1 || (game_info.game_quarter >= 3 && check_scores()) || (game_info.game_quarter == 6))
		// Play EOQ music that doesn't go into drum roll automatically
		snd_scall_bank(generic_bnk_str,EOQ_MUSIC_SND,VOLUME4,127,SND_PRI_SET|SND_PRI_TRACK0);
	else
		// Play EOQ music that does go into drum roll automatically
		snd_scall_bank(generic_bnk_str,EOQ_MUSIC2_SND,VOLUME4,127,SND_PRI_SET|SND_PRI_TRACK0);
	
	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info((void**)&cur_proc, suspend_self, resume_process);
#else
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

	// Somebody win?
	if ((game_info.game_quarter >= 3 && check_scores()))	// || (game_info.game_quarter == 6))
	{
		// Which team won?
		if (compute_score(0) > compute_score(1))
			t = game_info.team[0];
		else
//		if (compute_score(0) < compute_score(1))
			t = game_info.team[1];
//		else
//		if (tdata_blocks[0].total_yards > tdata_blocks[1].total_yards)
//			// FIX!!!!  If tied after 3 OT's, team with most yardage is awarded
//			// the win! Unless that is the same too which means team 2 wins
//			// anyhow! Tie it up?  Do we then save stats?  I think the save
//			// player record stuff requires a winner...
//			t = game_info.team[0];
//		else
//			t = game_info.team[1];

		tex_node = create_texture(message_win_tex[t],
								MESSAGE_TID,
								0,
								CREATE_NORMAL_TEXTURE,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTUREFILTER_BILINEAR,
								GR_TEXTUREFILTER_BILINEAR);

#ifdef DEBUG
		fprintf(stderr, "loaded tex:%s  handle:%d\r\n" ,message_tex[t], (int)tex_node->texture_handle);
#endif

		if (!tex_node)
		{
#ifdef DEBUG
			lockup();
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}

		tex_node = create_texture(win_win.texture_name,
								MESSAGE_TID,
								0,
								CREATE_NORMAL_TEXTURE,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTUREFILTER_BILINEAR,
								GR_TEXTUREFILTER_BILINEAR);

#ifdef DEBUG
		fprintf(stderr, "loaded tex:%s  handle:%d\r\n" , "msg_ovr.wms", (int)tex_node->texture_handle);
#endif

		if (!tex_node)
		{
#ifdef DEBUG
			lockup();
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}

		// Control will not reach this point until texture is loaded into TMU!
		// Process will wake up and then the beginobj can happen

		// Disable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif

		// Do a beginobj to display team wins

		message_obj = beginobj(message_win_imgs[t*2],SPRITE_HRES/2, SPRITE_VRES/2+70, 91.0F, MESSAGE_TID);
		message_obj->id = OID_MESSAGE;
		message_obj->w_scale = 0.2f;
		message_obj->h_scale = 0.2f;

	 	if (message_win_imgs[t*2+1])
		{
			message2_obj = beginobj(message_win_imgs[t*2+1],SPRITE_HRES/2, SPRITE_VRES/2+70, 91.0F, MESSAGE_TID);
			message2_obj->w_scale = 0.2f;
			message2_obj->h_scale = 0.2f;
			message2_obj->id = OID_MESSAGE;
		}
	 
	 	qcreate("wins",MESSAGE_PID,win_proc,0,0,0,0);
		
//		message3_obj = beginobj(&win_win,SPRITE_HRES/2, SPRITE_VRES/2, 1.001F, MESSAGE_TID);
//		message3_obj->id = OID_MESSAGE;
//		message3_obj->w_scale = 0.2f;
//		message3_obj->h_scale = 0.2f;

		while (message_obj->w_scale < 1.8F)
		{
			message_obj->w_scale += .06f;
			message_obj->h_scale += .06f;
			generate_sprite_verts(message_obj);
			
//			message3_obj->w_scale += .06f;
//			message3_obj->h_scale += .06f;
//			generate_sprite_verts(message3_obj);
			
			if( message2_obj)
			{
				message2_obj->w_scale += .06f;
				message2_obj->h_scale += .06f;
				generate_sprite_verts(message2_obj);
			}
			sleep(1);
		}

		while (message_obj->w_scale > 1.20F)
		{
			message_obj->w_scale -= .06f;
			message_obj->h_scale -= .06f;
			generate_sprite_verts(message_obj);
			
//			message3_obj->w_scale -= .06f;
//			message3_obj->h_scale -= .06f;
//			generate_sprite_verts(message3_obj);
			
			if( message2_obj)
			{
				message2_obj->w_scale -= .06f;
				message2_obj->h_scale -= .06f;
				generate_sprite_verts(message2_obj);
			}
			sleep(1);
		}

		// Show sparkles
		if (1)
		{
			columns = message_obj->verts[2].x - message_obj->verts[3].x;
			if( message2_obj)
				columns = columns + (message2_obj->verts[2].x - message2_obj->verts[3].x);
			columns = (columns / 15);

			start_x = message_obj->verts[3].x;
			start_y = message_obj->verts[0].y;

			z_flsh = 90.0F;
				
			for (t=0; t < columns; t++)
			{
				for (i=0; i < 2; i++)
				{
					z_flsh = z_flsh -0.80F;
					x = start_x + randrng(15);
					y = start_y + randrng((message_obj->verts[3].y + 0) - message_obj->verts[0].y);
					qcreate("sparkle",MESSAGE_PID,one_flash_proc,x,y,(int)z_flsh,0);
				}
				start_x = start_x + 15;
				sleep(3);
			}
		}
	}	
	// If tied & game is over after 3 OT...
	else if (!check_scores() && game_info.game_quarter == 6)
	{
		set_text_transparency_mode( TRANSPARENCY_ENABLE );
		set_text_font( FONTID_BAST25 );
		set_string_id( 0X7654 );

		set_text_color(MED_YELLOW);
		set_text_position( SPRITE_HRES/2, SPRITE_VRES/2+25, 1.2f );
		pmesg = oprintf( "%djTIE GAME!", (HJUST_CENTER|VJUST_CENTER) );

		pflasher3 = qcreate( "ftp12", MESSAGE6_PID, flash_text_proc, (int)pmesg, 0, 0, 0 );

		// Disable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif

	}
	// If tie game...
	else if (game_info.game_quarter >= 3)
	{
		tex_node = create_texture("msg_ovr.wms",
								MESSAGE_TID,
								0,
								CREATE_NORMAL_TEXTURE,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTUREFILTER_BILINEAR,
								GR_TEXTUREFILTER_BILINEAR);

#ifdef DEBUG
		fprintf(stderr, "loaded tex:%s  handle:%d\r\n" , "msg_ovr.wms", (int)tex_node->texture_handle);
#endif

		if (!tex_node)
		{
#ifdef DEBUG
			lockup();
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}

		// Control will not reach this point until texture is loaded into TMU!
		// Process will wake up and then the beginobj can happen

		// Disable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif

		message_obj = beginobj(&msg_ovr,SPRITE_HRES/2, SPRITE_VRES/2+50, 1.001F, MESSAGE_TID);
		message_obj->id = OID_MESSAGE;
		message_obj->w_scale = 1.20f;
		message_obj->h_scale = 1.20f;
		generate_sprite_verts(message_obj);
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message_obj,4,4,0);

		message2_obj = beginobj(&msg_ovr_c1,SPRITE_HRES/2, SPRITE_VRES/2+50, 1.001F, MESSAGE_TID);
		message2_obj->id = OID_MESSAGE;
		message2_obj->w_scale = 1.20f;
		message2_obj->h_scale = 1.20f;
		generate_sprite_verts(message2_obj);
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message2_obj,4,4,0);

	}
	else
	// This is end of 1st, 2nd, or 3rd periods
	{
label:
		tex_node = create_texture("m_end.wms",
								MESSAGE_TID,
								0,
								CREATE_NORMAL_TEXTURE,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTURECLAMP_CLAMP,
								GR_TEXTUREFILTER_BILINEAR,
								GR_TEXTUREFILTER_BILINEAR);

#ifdef DEBUG
		fprintf(stderr, "loaded tex:%s  handle:%d\r\n" , "m_end.wms", (int)tex_node->texture_handle);
#endif

		if (!tex_node)
		{
#ifdef DEBUG
			lockup();
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}

//		tex_node = create_texture("m_end01.wms",
//								MESSAGE_TID,
//								0,
//								CREATE_NORMAL_TEXTURE,
//								GR_TEXTURECLAMP_CLAMP,
//								GR_TEXTURECLAMP_CLAMP,
//								GR_TEXTUREFILTER_BILINEAR,
//								GR_TEXTUREFILTER_BILINEAR);
//
//#ifdef DEBUG
//		fprintf(stderr, "loaded tex:%s  handle:%d\r\n" , "m_end01.wms", (int)tex_node->texture_handle);
//#endif
//
//		if (!tex_node)
//		{
//#ifdef DEBUG
//			lockup();
//#endif
//			increment_audit(TEX_LOAD_FAIL_AUD);
//		}

		// Control will not reach this point until texture is loaded into TMU!
		// Process will wake up and then the beginobj can happen

		// Disable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif

		message_obj = beginobj(&msg_end,SPRITE_HRES/2, SPRITE_VRES/2+70, 1.001F, MESSAGE_TID);
		message_obj->id = OID_MESSAGE;
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message_obj,4,4,0);

		message2_obj = beginobj(qtr_tbl_imgs[game_info.game_quarter],SPRITE_HRES/2-150, SPRITE_VRES/2+5, 1.001F, MESSAGE_TID);
		message2_obj->id = OID_MESSAGE;
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message2_obj,4,4,0);

		message3_obj = beginobj(&msg_qtr,SPRITE_HRES/2+60, SPRITE_VRES/2, 1.001F, MESSAGE_TID);
		message3_obj->id = OID_MESSAGE;
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message3_obj,4,4,0);

		message4_obj = beginobj(&msg_qtr_c1,SPRITE_HRES/2+60, SPRITE_VRES/2, 1.001F, MESSAGE_TID);
		message4_obj->id = OID_MESSAGE;
		qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)message4_obj,4,4,0);
	}

	wait_for_all_buttons(40, 4*tsec);

	// Cleanup also happens at top of play_select in playsel.c	
	del1c(OID_MESSAGE,0xFFFFFFFF);
	delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);
	delete_multiple_strings(0x7654, 0xffffffff);
	killall(MESSAGE6_PID,0xFFFFFFFF);		// Kill TIE GAME flasher process
	// added for '99
	killall(MESSAGE_PID,0xFFFFFFF0);		// Kill TIE GAME flasher process
//	kill(pflasher3, 0);		// BUG...
}

//-------------------------------------------------------------------------------------------------
//
// Show opening kickoff idiot boxes
//
//-------------------------------------------------------------------------------------------------
void ko_idiot_box_proc(int *args)
{
	int	i;


	idiot_boxes_shown = 1;

	qcreate("idiots",IDIOT_BOX_PID,plr_ko_idiot_box_proc,0,0,0,0);
	qcreate("idiots",IDIOT_BOX_PID,plr_ko_idiot_box_proc,1,0,0,0);
	
	if (four_plr_ver)
	{
		qcreate("idiots",IDIOT_BOX_PID,plr_ko_idiot_box_proc,2,0,0,0);
		qcreate("idiots",IDIOT_BOX_PID,plr_ko_idiot_box_proc,3,0,0,0);
	}

	suspend_multiple_processes(0, 0x8000);
	resume_multiple_processes(IDIOT_BOX_PID, -1);

	// Wait until both boxes are gone
	while (existp(IDIOT_BOX_PID,0xFFFFFFFF))
		sleep (1);

	// here we just clear the button bits... (ie.. to stop lateraling after whack thru..etc...and spin... etc)

	resume_all();

	for (i=0; i < MAX_PLYRS; i++)
	{
		if ( (p_status & (1<<i)) && (game_info.plyr_control[i] >= 0) )
		{
//		player_blocks[game_info.plyr_control[i]].but_new = 0;
			player_blocks[game_info.plyr_control[i]].but_cur |= (P_A|P_B);
			player_blocks[game_info.plyr_control[i]].but_old |= (P_A|P_B);
		}
	}

}

#define LOW_LFT_X 20	
#define LOW_RGT_X 300
#define UP_RGT_X 300
#define UP_LFT_X 20

#define LOW_LFT_Y 170	
#define UP_RGT_Y 330
#define LOW_RGT_Y 170
#define UP_LFT_Y 330

#define P1_ID 11
#define P2_ID 22

//	static float idiot_box_LL_xys[2][2] = { {0.0f,0.0f}, {LOW_LFT_X,LOW_LFT_Y} };	// [2plr/4plr][x,y]
//	static float idiot_box_LR_xys[2][2] = { {0.0f,0.0f}, {LOW_RGT_X,LOW_RGT_Y} };	// [2plr/4plr][x,y]
//	static float idiot_box_UL_xys[2][2] = { {0.0f,0.0f}, {UP_LFT_X,UP_LFT_Y} };		// [2plr/4plr][x,y]
//	static float idiot_box_UR_xys[2][2] = { {0.0f,0.0f}, {UP_RGT_X,UP_RGT_Y} };		// [2plr/4plr][x,y]

static	float ko_idiot_box_xys[MAX_PLYRS][2][2] = {							// [plyr][2/4 plyr ver][x,y]
								{{BARX1,150.0f},  {64.0f,150.0f}},		// player 1
								{{BARX2,150.0f}, {192.0f,180.0f}},		// player 2
								{{ 0.0f,150.0f}, {320.0f,150.0f}},		// player 3
								{{ 0.0f,150.0f}, {448.0f,180.0f}} };		// player 4

static	int text_colors[MAX_PLYRS][2] = {				// [plyr][2/4 plyr ver][x,y]
								{LT_BLUE,LT_BLUE},		// player 1
								{LT_RED,LT_GREEN},		// player 2
								{0,LT_RED},				// player 3
								{0,LT_YELLOW} };		// player 4

//-------------------------------------------------------------------------------------------------
// 						Show opening kickoff idiot box for each player in game
//-------------------------------------------------------------------------------------------------
static void plr_ko_idiot_box_proc(int *args)
{
	int 	pnum;
	static	sprite_info_t		*idiot_bx_obj;
	float	box_x,box_y;

#ifdef DBG_KO
	return;
#endif //DBG_KO

	pnum = args[0];
	box_x = ko_idiot_box_xys[pnum][four_plr_ver][X_VAL];
	box_y = ko_idiot_box_xys[pnum][four_plr_ver][Y_VAL];

	// If player isnt in game, quit
	if (!(p_status & (1<<pnum)))
		die(0);

	// If player is experienced, quit
	if (plyr_data_ptrs[pnum]->games_played > 2)
		die(0);
//	if ((plyr_data_ptrs[game_info.off_side*2]->games_played > 2) &&
//		(plyr_data_ptrs[game_info.off_side*2+1]->games_played > 2))
//		die(0);

	// Start a flash player white process
//	qcreate ("flash",0,flash_plyr_proc2,game_info.plyr_control[1],8,P2_ID,0);
	qcreate ("flash",0,flash_plyr_proc2,pnum,8,P2_ID,0);

	// Tell string handler to keep all letters at the same z
	set_text_z_inc(0);

	// create the players IDIOT BOX
	idiot_bx_obj = beginobj(&playresult, box_x, box_y, 1.4f, SCR_PLATE_TID);
	idiot_bx_obj->z_angle = 256.04f;
	idiot_bx_obj->h_scale = 1.45f;
	idiot_bx_obj->id = OID_IDIOTBOX+pnum;
	generate_sprite_verts(idiot_bx_obj);

	set_text_font(FONTID_BAST10);
	set_string_id(0x34+pnum);
	set_text_color(text_colors[pnum][four_plr_ver]);
	set_text_position(box_x-5.0f,box_y+90.0f, 1.0F);
	oprintf( "%djPLAYER %d", (HJUST_CENTER|VJUST_CENTER), pnum+1);

	// if Player has BALL 
	if (game_info.ball_carrier == game_info.plyr_control[pnum])
	{
		set_text_font(FONTID_BAST10);
		set_text_color(LT_CYAN);
		set_text_position(box_x, box_y+75.0f, 1.0F);
		oprintf( "%djHAS BALL", (HJUST_CENTER|VJUST_CENTER));

		set_text_font(FONTID_BAST8T);
		set_text_color(LT_YELLOW);
		set_text_position(box_x,box_y+55.0f, 1.0F);
		oprintf( "%djUSE TURBO", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+43.0f + (is_low_res ? -2.0f : 0.0f), 1.0F);
		oprintf( "%djTO RUN FASTER ", (HJUST_CENTER|VJUST_CENTER));

		set_text_position(box_x,box_y+23.0f, 1.0F);
		oprintf( "%djDOUBLE TAP", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+11.0f + (is_low_res ? -1.0f : 0.0f), 1.0F);
		oprintf( "%djTURBO BUTTON", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-1.0f + (is_low_res ? -3.0f : 0.0f), 1.0F);
		oprintf( "%djTO SPIN.", (HJUST_CENTER|VJUST_CENTER));

		set_text_position(box_x,box_y-21.0f, 1.0F);
		oprintf( "%djTURBO + BLOCK", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-33.0f + (is_low_res ? -1.0f : 0.0f), 1.0F);
		oprintf( "%djTO STIFF ARM", (HJUST_CENTER|VJUST_CENTER));

		set_text_position(box_x,box_y-53.0f, 1.0F);
		oprintf( "%djTURBO + JUMP", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-65.0f + (is_low_res ? -2.0f : 0.0f), 1.0F);
		oprintf( "%djTO HURDLE", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-77.0f + (is_low_res ? -3.0f : 0.0f), 1.0F);
		oprintf( "%djOPPONENT.", (HJUST_CENTER|VJUST_CENTER));
	}
	//FIX!!!???
	// No real harm here if <plyr_control> less than 0; he'll just get the defensive box
	else if (player_blocks[game_info.plyr_control[pnum]].team == game_info.off_side)
	{
		// on offense but doesnt have ball
		set_text_font(FONTID_BAST8T);
		set_text_color(LT_YELLOW);
		set_text_position(box_x,box_y+55.0f + (is_low_res ? 1.0f : 0.0f), 1.0F);
		oprintf( "%djTURBO + JUMP", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+43.0f + (is_low_res ? -1.0f : 0.0f), 1.0F);
		oprintf( "%djDIVES AT", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+31.0f + (is_low_res ? -2.0f : 0.0f), 1.0F);
		oprintf( "%djDEFENDERS. USE", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+19.0f + (is_low_res ? -3.0f : 0.0f), 1.0F);
		oprintf( "%djTHIS TO HELP", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+7.0f + (is_low_res ? -4.0f : 0.0f), 1.0F);
		oprintf( "%djYOUR TEAMMATE", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-5.0f + (is_low_res ? -6.0f : 0.0f), 1.0F);
		oprintf( "%djGAIN EXTRA", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-17.0f + (is_low_res ? -7.0f : 0.0f), 1.0F);
		oprintf( "%djYARDS.", (HJUST_CENTER|VJUST_CENTER));

		set_text_position(box_x,box_y-37.0f + (is_low_res ? -6.0f : 0.0f), 1.0F);
		oprintf( "%djTRY TO", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-49.0f + (is_low_res ? -7.0f : 0.0f), 1.0F);
		oprintf( "%djGET INTO", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-61.0f + (is_low_res ? -8.0f : 0.0f), 1.0F);
		oprintf( "%djPOSITION FOR", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-73.0f + (is_low_res ? -10.0f : 0.0f), 1.0F);
		oprintf( "%djA LATERAL.", (HJUST_CENTER|VJUST_CENTER));
	}
	else
	{
		// on defense
		set_text_font(FONTID_BAST8T);
		set_text_color(LT_YELLOW);
		set_text_position(box_x,box_y+55.0f + (is_low_res ? 1.0f : 0.0f), 1.0F);
		oprintf( "%djPRESS RED", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+43.0f + (is_low_res ? -1.0f : 0.0f), 1.0F);
		oprintf( "%djBUTTON TO", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+31.0f + (is_low_res ? -2.0f : 0.0f), 1.0F);
		oprintf( "%djTAKE CONTROL", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+19.0f + (is_low_res ? -3.0f : 0.0f), 1.0F);
		oprintf( "%djOF THE CLOSET", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y+7.0f + (is_low_res ? -5.0f : 0.0f), 1.0F);
		oprintf( "%djDEFENDER TO", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-5.0f + (is_low_res ? -6.0f : 0.0f), 1.0F);
		oprintf( "%djTHE BALL", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-17.0f + (is_low_res ? -8.0f : 0.0f), 1.0F);
		oprintf( "%djCARRIER.", (HJUST_CENTER|VJUST_CENTER));

		set_text_position(box_x,box_y-37.0f + (is_low_res ? -7.0f : 0.0f), 1.0F);
		oprintf( "%djTURBO + JUMP", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-49.0f + (is_low_res ? -9.0f : 0.0f), 1.0F);
		oprintf( "%djDIVE TACKLES", (HJUST_CENTER|VJUST_CENTER));
		set_text_position(box_x,box_y-61.0f + (is_low_res ? -10.0f : 0.0f), 1.0F);
		oprintf( "%djOPPONENT.", (HJUST_CENTER|VJUST_CENTER));
	}

//	pflasher = qcreate("pflash",P1_ID, flash_text_proc, (int)pmsg[1], 0, 0, 0);
	wait_for_any_butn_press_active_plyr(40, 6*tsec, pnum);
	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME1,127,PRIORITY_LVL4);
	
	delete_multiple_strings(0x34+pnum, 0xffffffff);
	del1c(OID_IDIOTBOX+pnum,0xFFFFFFFF);
}



//-------------------------------------------------------------------------------------------------
//									 Show mugshot of player
//-------------------------------------------------------------------------------------------------
void show_plyrs_mugshot_proc(int *args)
{
	int 			team,yards,scr_dif,ppos;
	float			box_x,box_y,mug_y,mug_x;
	sprite_info_t	*mug_bx_obj;
	sprite_info_t	*mug_obj;


	// 40 percent of time... show mugshot on touchdowns and two points
	if (randrng(100) > 40)
	{
		qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );
		die(0);
	}

	// which SIDE scored ?
	team = player_blocks[game_info.ball_carrier].team;

	// make sure player has a mugshot
/*
	if (!(player_blocks[game_info.ball_carrier].static_data->pii_mugshot))
	{
		// no mugshot for player...just show score_plate
		qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );
		die(0);
	}
*/
	box_x = 270.0f;
	box_y = 55.0f;
	mug_x = 75.0f;
	mug_y = 115.0f;

	// create the mugshot backdrop
	mug_bx_obj = beginobj(&playresult, box_x, box_y, 1.4f, SCR_PLATE_TID);
	mug_bx_obj->w_scale = 1.7f;
	mug_bx_obj->mode = FLIP_TEX_H;
	mug_bx_obj->id = OID_SCRPLATE;
	generate_sprite_verts(mug_bx_obj);

#if 1
	mug_obj = beginobj(&sp_logo, mug_x, mug_y, 1.3f, SCR_PLATE_TID);
	mug_obj->id = OID_SCRPLATE;
#else
	// create mugshot
//	mug_obj = beginobj(teaminfo[game_info.team[team]].static_data[ppos].pii_mugshot,
	mug_obj = beginobj(player_blocks[game_info.ball_carrier].static_data->pii_mugshot,
						mug_x,
						mug_y,
						1.3f,
						SCR_PLATE_TID);
	mug_obj->id = OID_SCRPLATE;
	if (!mug_obj)
	{
		// ERROR... just show score plate
		delete_multiple_strings(0x34, 0xffffffff);
		del1c(OID_SCRPLATE,0xFFFFFFFF);
		qcreate( "scores", PLYRINFO_PID, score_plate_proc, 0, 0, 0, 0 );
		die(0);					// couldnt get texture...or something
	}
#endif

	yards = game_info.los - game_info.old_los;
	if (yards < 0)
		yards *= -1;			// make positive

	scr_dif = compute_score(team) - compute_score(!team);

	set_text_z_inc(0);
	set_text_font(FONTID_BAST10);
	set_string_id(0x09);			// same ID as score plate text, so it will be deleted
	set_text_color(LT_YELLOW);
	set_text_position(box_x-125.0f, box_y+10.0f, 1.001F);
	set_text_justification_mode(HJUST_LEFT|VJUST_CENTER);

	if ((yards <= 0) && (game_info.play_result == PR_TOUCHDOWN))
		oprintf( "%s WITH THE TOUCHDOWN",
			player_blocks[game_info.ball_carrier].static_data->lastname );
	else
	if ((yards <= 0) && (game_info.play_result == PR_CONVERSION))
		oprintf( "%s WITH THE CONVERSION",
			player_blocks[game_info.ball_carrier].static_data->lastname );
	else
	if (game_info.game_flags & GF_FWD_PASS)
		oprintf( "%s WITH A %d YARD RECEPTION",
			player_blocks[game_info.ball_carrier].static_data->lastname, yards );
	else
		oprintf( "%s WITH A %d YARD RUN",
			player_blocks[game_info.ball_carrier].static_data->lastname, yards );

	set_text_position(box_x-125.0f, box_y-10.0f, 1.001F);
	if (scr_dif == 0)
		oprintf( "TIES THE GAME AT %d",
			compute_score(team));
	else if (scr_dif > 0)
		oprintf( "GIVES THE %s A %d POINT LEAD",
			teaminfo[game_info.team[team]].name, scr_dif );
	else
		oprintf( "BRINGS THE %s WITHIN %d",
			teaminfo[game_info.team[team]].name, -scr_dif );

//	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME1,127,PRIORITY_LVL4);
	showhide_pinfo(0);
}


//-------------------------------------------------------------------------------------------------
//
//  		This process flashes player white several times
//
//			INPUT:  pobj of player to flash, sleep time, id
//			OUTPUT:	Nothing
//
//-------------------------------------------------------------------------------------------------
static void flash_plyr_proc2(int *args)
{
	obj_3d *pobj;
	fbplyr_data *pplyr;
	int id,delay,x;

	pplyr = &player_blocks[args[0]];
	pobj = (obj_3d *)pplyr;
	delay = args[1];
	id = args[2];

	do
	{
		pobj->flags |= PF_FLASHWHITE;
		sleep(delay);
		pobj->flags &= ~PF_FLASHWHITE;
		sleep(delay);
	} while (existp(id,0xFFFFFFFF));
}


//-------------------------------------------------------------------------------------------------
//							Show which player is currently the WB
//-------------------------------------------------------------------------------------------------
void whois_qb_idiot_box_proc(int *args)
{
	int 	pnum,i;
	static	sprite_info_t		*idiot_bx_obj;
	float	box_x,box_y;

	pnum = player_blocks[(game_info.off_side ? 10 : 3)].station;

	// if Im not in game...dont bother
	if ( (pnum < 0) || !(p_status & (1<<pnum)) )
		die(0);					// player not in game...no need for message

	// quit right now if I'm not human
//	if (ISDRONE(player_blocks+pnum))
//		die(0);

	// if teammate isnt in game... dont show
	if ((pnum == 0) && (!(p_status & 2)))
		die(0);
	else if ((pnum == 1) && (!(p_status & 1)))
		die(0);
	else if ((pnum == 2) && (!(p_status & 8)))
		die(0);
	else if ((pnum == 3) && (!(p_status & 4)))
		die(0);

	box_x = qb_msg_xys[pnum][four_plr_ver];
	box_y = (SPRITE_HRES/2.0f)+30.0f;

//	box_x = (SPRITE_HRES/2.0f)+80.0f;
//	box_y = (SPRITE_HRES/2.0f)-140.0f;

	// If player is experienced, quit
//	if (plyr_data_ptrs[pnum]->games_played > 2)
//		die(0);

	// Start a flash player white process
//	qcreate ("flash",0,flash_plyr_proc2,pnum,8,P2_ID,0);

	// Tell string handler to keep all letters at the same z
	set_text_z_inc(0);

	// create the players IDIOT BOX
	idiot_bx_obj = beginobj(&gameclock, box_x, box_y, 1.2f, SCR_PLATE_TID);
//	idiot_bx_obj->w_scale = 1.45f;
//	idiot_bx_obj->h_scale = 1.45f;
//	generate_sprite_verts(idiot_bx_obj);

	set_text_color(text_colors[pnum][four_plr_ver]);
	set_text_font(FONTID_BAST10);
	set_string_id(0xeef);
	set_text_position(box_x,box_y+8.0f, 1.001F);
	
	pmsg[0] = oprintf( "%djPLAYER %d", (HJUST_CENTER|VJUST_CENTER), pnum+1);
	set_text_position(box_x,box_y-8.0f, 1.001F);
	pmsg[1] = oprintf( "%djIS QB", (HJUST_CENTER|VJUST_CENTER));

	pflasher = qcreate("ftp13",P1_ID, flash_text_proc, (int)pmsg[0], 0, 0, 0);
	pflasher2 = qcreate("ftp14",P1_ID, flash_text_proc, (int)pmsg[1], 0, 0, 0);

	// once ball is HIKED...die.. else time out after 3 seconds.
	for (i=0; i < 4*tsec; i++)
	{
		if (game_info.game_mode == GM_IN_PLAY)
			break;
		sleep(1);
	}

//	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME1,127,PRIORITY_LVL4);
	kill(pflasher, 0);
	kill(pflasher2, 0);
	delete_multiple_strings(0xeef, 0xffffffff);
	delobj(idiot_bx_obj);
}

//---------------------------------------------------------------------------------------------------------------------------
//	This process prints the opening play instructions
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
void instructions_proc(int *args)
{
	static process_node_t	*pflasher4 = NULL;
	static sprite_info_t * idiot_bx_obj;
	static float idiot_box_xys[][2][2] = {				// [p#][2plr/4plr][x,y]
			{ {0.0f,0.0f}, {326.0f-164-146,84.0f} },
			{ {0.0f,0.0f}, {326.0f-164+196,84.0f} }};

	idiot_boxes_shown = -1;		// flag for showing instructions

	sleep(2);

	suspend_multiple_processes(0, 0x8000);
	resume_multiple_processes(IDIOT_BOX2_PID, -1 );
	resume_multiple_processes(POST_PLAYER_PID, -1 );
	resume_multiple_processes(CAMBOT_PID, -1 );

	// Gets here when QB lines up for first snap of game

 	idiot_bx_obj = beginobj(&playresult,(float)(int)(SPRITE_HRES/2.0f),(float)(int)(SPRITE_VRES/2.0f+10), 2.5f, SCR_PLATE_TID);
	idiot_bx_obj->w_scale = (308.0f / 212.0f);	//1.45f;
	idiot_bx_obj->h_scale = (190.0f / 79.0f);	//2.15f;
	idiot_bx_obj->id = OID_MESSAGE;
	generate_sprite_verts(idiot_bx_obj);

	// Allow each sequential letter to overlap the previous letter - turn off at end!
	if (is_low_res)
		set_text_font(FONTID_BAST23);
	else	
		set_text_font(FONTID_BAST18);
	set_string_id(0x34);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +76, 1.0f);

	set_text_color(LT_RED);
 	pmsg[0] = oprintf("%djRULES:", (HJUST_CENTER|VJUST_CENTER));

	set_text_color(LT_RED);
	if (is_low_res)
		set_text_font(FONTID_BAST10LOW);
	else	
		set_text_font(FONTID_BAST10);
	set_text_position((SPRITE_HRES/2)-128, (SPRITE_VRES/2) +47, 1.0f);
	pmsg[1] =	oprintf("%dj30 YARDS", (HJUST_LEFT|VJUST_CENTER));
	set_text_color(WHITE);
	set_text_position((SPRITE_HRES/2)+40, (SPRITE_VRES/2) +47, 1.0f);
	oprintf("%djNEEDED FOR FIRST DOWN", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +47-22, 1.0f);
	set_text_color(LT_YELLOW);
 	oprintf("%djCLOCK STOPS AFTER EACH PLAY", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +47-44, 1.0f);
	set_text_color(WHITE);
	oprintf("%djFIELD GOALS & PUNTING ALLOWED", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +47-66, 1.0f);
	set_text_color(LT_YELLOW);
	oprintf("%djPASS INTERFERENCE IS OKAY", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +47-88, 1.0f);
	set_text_color(WHITE);
	oprintf("%djNO PENALTIES", (HJUST_CENTER|VJUST_CENTER));


	pflasher4 = qcreate("ftp15", 0, flash_text_proc, (int)pmsg[0], (int)pmsg[1],0,0);

	sleep(2);
	
	// Quickly flash idiot box white two times
	qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)idiot_bx_obj,2,3,0);

	wait_for_any_butn_press(10, 9 * tsec);

	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);

#ifdef DEBUG
//	while (halt)
//		sleep (1);
#endif //DEBUG

	delete_multiple_strings(0x34, 0xffffffff);
	kill(pflasher4, 0);

	// Do second page of text messages
	if (is_low_res)
		set_text_font(FONTID_BAST23);
	else
		set_text_font(FONTID_BAST18);
	set_string_id(0x34);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +76, 1.0f);

	set_text_color(LT_RED);
 	pmsg[0] = oprintf("%djHOW TO PASS:", (HJUST_CENTER|VJUST_CENTER));

	set_text_color(LT_YELLOW);
	if (is_low_res)
		set_text_font(FONTID_BAST10LOW);
	else	
		set_text_font(FONTID_BAST10);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49, 1.0f);
	oprintf("%djPUSH STICK LEFT TO HIGHLIGHT", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-17, 1.0f);
 	oprintf("%djLEFT MOST RECEIVER ON FIELD", (HJUST_CENTER|VJUST_CENTER));
	set_text_color(WHITE);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-41, 1.0f);
	oprintf("%djPUSH STICK RIGHT TO HIGHLIGHT", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-58, 1.0f);
	oprintf("%djRIGHT MOST RECEIVER ON FIELD", (HJUST_CENTER|VJUST_CENTER));
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-82, 1.0f);
	oprintf("%djPUSH STICK UP/DOWN TO HIGHLIGHT", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-99, 1.0f);
	oprintf("%djCENTER RECEIVER ON FIELD", (HJUST_CENTER|VJUST_CENTER));

	pflasher4 = qcreate("ftp16", 0, flash_text_proc, (int)pmsg[0], 0,0,0);

	sleep(2);
	
	wait_for_any_butn_press(10,9 * tsec);

	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);

#ifdef DEBUG
//	while (halt)
//		sleep (1);
#endif //DEBUG

	delete_multiple_strings(0x34, 0xffffffff);
	kill(pflasher4, 0);

	// Do second page of text messages
	if (is_low_res)
		set_text_font(FONTID_BAST23);
	else
		set_text_font(FONTID_BAST18);
	set_string_id(0x34);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +76, 1.0f);

	set_text_color(LT_RED);
 	pmsg[0] = oprintf("%djHOW TO AUDIBLE:", (HJUST_CENTER|VJUST_CENTER));

	set_text_color(LT_YELLOW);
	if (is_low_res)
		set_text_font(FONTID_BAST10LOW);
	else	
		set_text_font(FONTID_BAST10);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49, 1.0f);
	oprintf("%djDOUBLE TAP THE TURBO", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-17, 1.0f);
 	oprintf("%djBUTTON TO ACTIVATE", (HJUST_CENTER|VJUST_CENTER));
	set_text_color(WHITE);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-41, 1.0f);
	oprintf("%djPUSH STICK UP OR DOWN TO", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-58, 1.0f);
	oprintf("%djSELECT DIFFERENT PLAYS", (HJUST_CENTER|VJUST_CENTER));
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-82, 1.0f);
	oprintf("%djAUDIBLES CAN BE SET IN", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-99, 1.0f);
	oprintf("%djCREATE A PLAY", (HJUST_CENTER|VJUST_CENTER));

	pflasher4 = qcreate("ftp16", 0, flash_text_proc, (int)pmsg[0], 0,0,0);

	sleep(2);
	
	wait_for_any_butn_press(10,9 * tsec);

	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);

#ifdef DEBUG
//	while (halt)
//		sleep (1);
#endif //DEBUG

	delete_multiple_strings(0x34, 0xffffffff);
	kill(pflasher4, 0);

	// Do third page of text messages
	if (is_low_res)
		set_text_font(FONTID_BAST23);
	else	
		set_text_font(FONTID_BAST18);
	set_string_id(0x34);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +76, 1.0f);

	set_text_color(LT_RED);
 	pmsg[0] = oprintf("%djGENERAL HINTS:", (HJUST_CENTER|VJUST_CENTER));

	set_text_color(LT_YELLOW);
	if (is_low_res)
		set_text_font(FONTID_BAST10LOW);
	else	
		set_text_font(FONTID_BAST10);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49, 1.0f);
	oprintf("%djWAIT FOR A RECEIVER TO GET OPEN", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-17, 1.0f);
 	oprintf("%djBEFORE PASSING TO HIM", (HJUST_CENTER|VJUST_CENTER));
	set_text_color(WHITE);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-41, 1.0f);
	oprintf("%djUSE CHANGE PLAYER BUTTON, THEN", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-58, 1.0f);
	oprintf("%djPRESS THE TACKLE BUTTON", (HJUST_CENTER|VJUST_CENTER));
	set_text_color(LT_YELLOW);
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-82, 1.0f);
	oprintf("%djKNOCK RECEIVER DOWN JUST AS HE", (HJUST_CENTER|VJUST_CENTER));
	set_text_position((SPRITE_HRES/2), (SPRITE_VRES/2) +49-99, 1.0f);
	oprintf("%djCATCHES BALL TO CAUSE A BOBBLE", (HJUST_CENTER|VJUST_CENTER));

	pflasher4 = qcreate("ftp17", 0, flash_text_proc, (int)pmsg[0], 0,0,0);

	sleep(2);
	
	wait_for_any_butn_press(10, 9 * tsec);

	snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);

	delete_multiple_strings(0x34, 0xffffffff);
	kill(pflasher4, 0);

	delobj(idiot_bx_obj);

	resume_all();

	sleep(10);	//15

	idiot_boxes_shown = 2;		// flag for boxes were shown
}

//-------------------------------------------------------------------------------------------------
//
// Show win word under team name at end of game
//
//-------------------------------------------------------------------------------------------------
void win_proc(int *args)
{
	sprite_info_t *message_obj = NULL;
	texture_node_t *tex_node = NULL;
	int t = 0;
	int	columns,start_x,start_y,i,x,y;
	float z_flsh;
	
	sleep(35);
	
	message_obj = beginobj(&win_win,SPRITE_HRES/2, SPRITE_VRES/2-15, 91.0F, MESSAGE_TID);
	message_obj->id = OID_MESSAGE;
	message_obj->w_scale = 0.2f;
	message_obj->h_scale = 0.2f;

	while (message_obj->w_scale < 1.8f)
	{
		message_obj->w_scale += .06f;
		message_obj->h_scale += .06f;
		generate_sprite_verts(message_obj);
		sleep(1);
	}

	while (message_obj->w_scale > 1.20F)
	{
		message_obj->w_scale -= .06f;
		message_obj->h_scale -= .06f;
		generate_sprite_verts(message_obj);
		sleep(1);
	}

	// Show sparkles
	if (1)
	{
		columns = message_obj->verts[2].x - message_obj->verts[3].x;
		columns = (columns / 15);

		start_x = message_obj->verts[3].x;
		start_y = message_obj->verts[0].y;

		z_flsh = 90.0F;
			
		for (t=0; t < columns; t++)
		{
			for (i=0; i < 2; i++)
			{
				z_flsh = z_flsh -0.80F;
				x = start_x + randrng(15);
				y = start_y + randrng((message_obj->verts[3].y + 0) - message_obj->verts[0].y);
				qcreate("sparkle",MESSAGE_PID,one_flash_proc,x,y,(int)z_flsh,0);
			}
			start_x = start_x + 15;
			sleep(3);
		}
	}
}	
/*****************************************************************************/
