//******************************************************************
//
// DESCRIPTION: Play select code for BLITZ
// AUTHOR: Jeff Johnson
// HISTORY: Started Sept 29. 1996
// $Revillon: 
// $Archive: /video/nfl/playsel.c $
// $Modtime: 10/05/99 12:09p $
// 
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

#include "/video/nfl/include/fontid.h"
#include "/video/nfl/include/id.h"
#include "/video/nfl/include/game.h"
#include "/video/nfl/include/ani3D.h"
#include "/video/nfl/include/sndcalls.h"
#include "/video/nfl/include/player.h"
#include "/video/nfl/include/select.h"
#include "/video/nfl/include/cap.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/pproto.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/PlayBook.h"
#include "/Video/Nfl/Include/PInfo.h"
#include "/Video/Nfl/Include/Record.h"
#include "/Video/Nfl/Include/PlaySel.h"
#include "/Video/Nfl/Include/PlayStuf.h"

#define IMAGEDEFINE	1
#include "/Video/Nfl/Include/Plays.h"

//
// #def's to include for debuging
//
#ifdef DEBUG
//	#define NOCLOCK
//	#define ADJ_OVERLAY_XY
#endif

// routine declarations
void cycle_play_arrows( int *args);
void wait_for_any_butn_press_active_team(int, int, int);
void plysel_draw_function(void *oi);
void cursor_draw_function(void *oi);
void cpu_pick_play(int);
void play_select_cntrl(int *);
void display_flip_proc(int *args);
void display_free_pt_proc(int *args);
void display_punt_proc(int *args);
void display_FG_proc(int *args);
void warning_snd_proc(int *args);
void do_off_play_percent_aud(int);
void do_def_play_percent_aud(int);
static void audit_play(play_t *);
//int check_for_secret_play(int plyr, int slot, int *sw_que);

static void PlaySel__ChoosePlay(int side, int page, int p_num, int cur_pos, int tm_num, int force_choose);

// must be variables...to account for kit
#define	QUE_SIZE			5

#define	PSEL_COLS			3
#define	PSEL_ROWS			3

extern PLAYSTUF__SLOT off_slots[];
extern PLAYSTUF__SLOT def_slots[];
extern PLAYSTUF__SLOT team_slots[][3];
extern PLAYSTUF__SLOT extra_slots[];

extern short exit_status;
extern short force_selection;							// !=0 force cursor to current choice

extern short screen;

extern image_info_t big1;
extern image_info_t big2;
extern image_info_t big3;
extern image_info_t big4;

extern LIMB limb_cursorb_obj;
extern LIMB limb_cursorr_obj;
extern LIMB limb_topbar_obj;
extern LIMB limb_triangle_obj;
extern LIMB limb_blank;
extern LIMB limb_kicks_punt;
extern LIMB limb_kicks_fieldgoal;

extern float logo_xys[SCREENS][2][2][2];

extern void extra_point_speech(void);
extern void one_wide_flash_proc(int *args);
extern image_info_t play_tab;
extern void	clock_low_sp(void);

extern image_info_t playresult;

//--------------------------------------
// these are for the play select textures

enum playsel_tex {
	PLAYSEL_TEX,
	PSARROW1_TEX,
//	PSARROW2_TEX,
//	PSARROW3_TEX,
//	PSARROW4_TEX,
//	PSARROW5_TEX,
//	PSARROW6_TEX,
//	PSARROW7_TEX,
//	PSARROW8_TEX,
	TMLOGO1_TEX,
	TMLOGO2_TEX,
	TMPLAYS1_TEX,
	TMPLAYS2_TEX,
	OFFPLAY1_TEX,
	DEFPLAY1_TEX,
	PSTOTAL_TEX
};

static struct texture_node * psel_decals[PSTOTAL_TEX];

static char * psel_decal_names[PSTOTAL_TEX] = {
	"playsel8.wms",
	"psarrow1.wms",
//	"psarrow2.wms",
//	"psarrow3.wms",
//	"psarrow4.wms",
//	"psarrow5.wms",
//	"psarrow6.wms",
//	"psarrow7.wms",
//	"psarrow8.wms",
	NULL,
	NULL,
	NULL,
	NULL,
	tp_off_tex_name,
	tp_def_tex_name,
};

#if 1
static char * teamspecific_textures[] = {
	tp_ari_tex_name,
	tp_atl_tex_name,
	tp_bal_tex_name,
	tp_buf_tex_name,
	tp_car_tex_name,
	tp_chi_tex_name,

	tp_buf_tex_name,
	tp_ari_tex_name,
	tp_atl_tex_name,
	tp_bal_tex_name,
	tp_buf_tex_name,
	tp_car_tex_name,
	tp_chi_tex_name,
	tp_buf_tex_name,
	tp_ari_tex_name,
	tp_atl_tex_name,
	tp_bal_tex_name,
	tp_buf_tex_name,
	tp_car_tex_name,
	tp_chi_tex_name,
	tp_buf_tex_name,
	tp_ari_tex_name,
	tp_atl_tex_name,
	tp_bal_tex_name,
	tp_buf_tex_name,
	tp_car_tex_name,
	tp_chi_tex_name,
	tp_buf_tex_name,
	tp_ari_tex_name,
	tp_atl_tex_name,
	tp_bal_tex_name,
};
#else
// SHIP CODE!!
static char *teamspecific_textures[] = {
	tp_ari_tex_name,
	tp_atl_tex_name,
	tp_bal_tex_name,
	tp_buf_tex_name,
	tp_car_tex_name,
	tp_chi_tex_name,
	tp_cin_tex_name,
	tp_cle_tex_name,
	tp_dal_tex_name,
	tp_den_tex_name,
	tp_det_tex_name,
	tp_gre_tex_name,
	tp_ind_tex_name,
	tp_jac_tex_name,
	tp_kan_tex_name,
	tp_mia_tex_name,
	tp_min_tex_name,
	tp_nen_tex_name,
	tp_nor_tex_name,
	tp_nyg_tex_name,
	tp_nyj_tex_name,
	tp_oak_tex_name,
	tp_phi_tex_name,
	tp_pit_tex_name,
	tp_snd_tex_name,
	tp_snf_tex_name,
	tp_sea_tex_name,
	tp_stl_tex_name,
	tp_tam_tex_name,
	tp_ten_tex_name,
	tp_was_tex_name,
};
#endif

//--------------------------------------
// these are for the play select 3d objects

static obj3d2_data_t	top_piece;
static obj3d2_data_t	info_piece;

static obj3d2_data_t	cursor1;
static obj3d2_data_t	cursor2;

static obj3d2_data_t	def_play1;
static obj3d2_data_t	def_play2;
static obj3d2_data_t	def_play3;
static obj3d2_data_t	def_play4;
static obj3d2_data_t	def_play5;
static obj3d2_data_t	def_play6;
static obj3d2_data_t	def_play7;
static obj3d2_data_t	def_play8;
static obj3d2_data_t	def_play9;

static obj3d2_data_t	off_play1;
static obj3d2_data_t	off_play2;
static obj3d2_data_t	off_play3;
static obj3d2_data_t	off_play4;
static obj3d2_data_t	off_play5;
static obj3d2_data_t	off_play6;
static obj3d2_data_t	off_play7;
static obj3d2_data_t	off_play8;
static obj3d2_data_t	off_play9;

static obj3d2_data_t * playsel_play_obj_ptrs[PLAYSTUF__PLAYS_PER_PAGE][2] = {
	{ &off_play1, &def_play1 },
	{ &off_play2, &def_play2 },
	{ &off_play3, &def_play3 },
	{ &off_play4, &def_play4 },
	{ &off_play5, &def_play5 },
	{ &off_play6, &def_play6 },
	{ &off_play7, &def_play7 },
	{ &off_play8, &def_play8 },
	{ &off_play9, &def_play9 },
};

//--------------------------------------

static int show_freept_idiot_box = 0;
int old_page[3] = {0,0,0};
int old_flip_plays = 0;
int hidden_off_cursor = 0;
int miss_kick = 0;

//GrColor_t	cursor_color;


//// used for secret plays on PLAY SELECT
//typedef struct _combo_data
//{
//	int		slot_num;					// where the cursor has to be to activate this secret play
//	int		joy_combo[5];				//4 max joysticks hits for combo + NULL
//	int		secret_play_num;
//} combo_data_t;
//
//#define	L	JOY_LEFT
//#define	R	JOY_RIGHT
//#define	U	JOY_UP
//#define	D	JOY_DOWN
//
//static combo_data_t secret_play_combos[] = {
//		{0,{L,L,L,NULL,NULL},0},
//		{0,{L,U,L,NULL,NULL},1},
//		{0,{U,L,U,NULL,NULL},2},
//		{2,{R,R,U,NULL,NULL},3},
//		{2,{R,U,R,NULL,NULL},4},
//		{2,{L,D,D,NULL,NULL},5},
//		{6,{L,D,D,D,   NULL},6},
//		{6,{D,L,D,D,   NULL},7},
//		{8,{R,D,D,NULL,NULL},8},
//		{8,{R,D,R,D,   NULL},9},
//		{-1,{NULL,NULL,NULL,NULL,NULL},-1} };
//
//
//static char *secret_play_txt[] = {"SECRET PLAY","SELECTED!!","0"};

static LIMB	*cursor_limb_ptrs[] = {&limb_cursorb_obj,&limb_cursorr_obj};


//////////////////////////////
// team play overlay sprites
static sprite_info_t * psi_plays[2][PLAYSTUF__MAX_PLAY_PAGES][PLAYSTUF__PLAYS_PER_PAGE];


//////////////////////////////
// placed on cursor in 4 player version of game
static image_info_t * arrow_imgs[] = {&big1,&big2,&big3,&big4};

static play_t *short_off_plays[] = {

//New plays
		&play_gen_1,
		&play_gen_2,
		&play_gen_3,
		&play_gen_4,
		&play_gen_5,
		&play_gen_6,
		&play_az_1,
		&play_az_2,
		&play_az_3,
		&play_at_1,
		&play_at_2,
		&play_at_3,
		&play_ba_1,
		&play_ba_2,
		&play_ba_3,
		&play_bu_1,
		&play_bu_2,
		&play_bu_3,
		&play_ca_1,
		&play_ca_2,
		&play_ca_3,
		&play_ch_1,
		&play_ch_2,
		&play_ch_3,
		&play_ci_1,
		&play_ci_2,
		&play_ci_3,


		&play_switch,
		&play_switch,
		&play_cruisn,
		&play_cruisn,
		&play_hurricane,
		&play_hurricane,
		&play_zigzag,
		&play_zigzag,
		&play_spider,
		&play_spider,
		&play_monkey,
		&play_monkey,
		&play_side_slide,
		&play_side_slide,
		&play_QBdeep,
		&play_QBdeep,
		&play_hooknladder,
		&play_hooknladder,

		&play_upper_cut,
		&play_blizzard,
		&play_criss_cross,
		&play_blitz_beater,
		&play_screen_rgt,
		&play_cross_slant,
		&play_sweep_rgt };

static play_t *long_off_plays[] = {

//New plays
		&play_gen_1,
		&play_gen_2,
		&play_gen_3,
		&play_gen_4,
		&play_gen_5,
		&play_gen_6,
		&play_az_1,
		&play_az_2,
		&play_az_3,
		&play_at_1,
		&play_at_2,
		&play_at_3,
		&play_ba_1,
		&play_ba_2,
		&play_ba_3,
		&play_bu_1,
		&play_bu_2,
		&play_bu_3,
		&play_ca_1,
		&play_ca_2,
		&play_ca_3,
		&play_ch_1,
		&play_ch_2,
		&play_ch_3,
		&play_ci_1,
		&play_ci_2,
		&play_ci_3,




		&play_switch,
		&play_switch,
		&play_cruisn,
		&play_cruisn,
		&play_hurricane,
		&play_hurricane,
		&play_zigzag,
		&play_zigzag,
		&play_spider,
		&play_spider,
		&play_monkey,
		&play_monkey,
		&play_side_slide,
		&play_side_slide,
		&play_QBdeep,
		&play_QBdeep,
		&play_hooknladder,
		&play_hooknladder,

		&play_hail_mary,
		&play_dawg_hook,
		&play_screen_rgt,
		&play_middle_pick,
		&play_utb_deep,
		&play_subzero,
		&play_razzle_dazzle,
		&play_da_bomb 
};

static play_t *short_def_plays[] = {


// New def plays
		&play_stuff_it,
		&play_stuff_it2,
		&play_fake_zone,
		&play_knock_down,
		&play_left_zone,
		&play_right_zone,
		&play_stop_run,
		&play_mix_it_up,
		&play_spread,



		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_near_zone,
		&play_near_zone,
		&play_medium_zone };
		
static play_t *short_def_plays2[] = {
// New def plays
		&play_stuff_it,
		&play_stuff_it2,
		&play_fake_zone,
		&play_knock_down,
		&play_left_zone,
		&play_right_zone,
		&play_stop_run,
		&play_mix_it_up,
		&play_spread,

		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_near_zone,
		&play_medium_zone };
		
static play_t *long_def_plays[] = {

// New def plays
		&play_stuff_it,
		&play_stuff_it2,
		&play_fake_zone,
		&play_knock_down,
		&play_left_zone,
		&play_right_zone,
		&play_stop_run,
		&play_mix_it_up,
		&play_spread,

		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_medium_zone,
		&play_deep_zone,
		&play_deep_zone };
		
static play_t *long_def_plays2[] = {

// New def plays
		&play_stuff_it,
		&play_stuff_it2,
		&play_fake_zone,
		&play_knock_down,
		&play_left_zone,
		&play_right_zone,
		&play_stop_run,
		&play_mix_it_up,
		&play_spread,

		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_medium_zone,
		&play_deep_zone,
		&play_deep_zone };
		
static play_t *blitz_def_plays[] = {
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_safe_cover,
		&play_stop_run,
		&play_stop_run,
		&play_2_man_blitz,
		&play_2_man_blitz,
		&play_2_man_blitz,
		&play_2_man_blitz,
		&play_knock_down,
		&play_knock_down,
		&play_knock_down,
//		&play_suicide_blitz,
		&play_3_man_blitz,
		&play_3_man_blitz,
		&play_3_man_blitz,
		&play_3_man_blitz };
		 
#define	NUM_MOVES		14.0F			// number of movements desired
#define	MOVE_DELTA		18.0F			// amount to move each movement
#define	CX1	3.0f						// start X of LEFT side plaque (upper left)
#define	CX2	260.0f						// start X of RIGHT side plaque (upper left)
#define	CX3	240.0f						// start X of RIGHT side plaque (upper left)
#define	CY1	294.0f
#define	CY2	294.0f
#define	CY3	306.0f
#define	XD	83.0f 						// size in X
#define	YD	96.0f						// size in Y
static float playsel_plays_xys[][PLAYSTUF__PLAYS_PER_PAGE][2] = {
 {
  // left side (blue)
  {CX1,CY1},      {CX1+XD,CY1},      {CX1+XD*2,CY1},
  {CX1,CY1-YD},   {CX1+XD,CY1-YD},   {CX1+XD*2,CY1-YD},
  {CX1,CY1-YD*2}, {CX1+XD,CY1-YD*2}, {CX1+XD*2,CY1-YD*2},
 },
 {
  // right side (red)
  {CX2,CY2},      {CX2+XD,CY2},      {CX2+XD*2,CY2},
  {CX2,CY2-YD},   {CX2+XD,CY2-YD},   {CX2+XD*2,CY2-YD},
  {CX2,CY2-YD*2}, {CX2+XD,CY2-YD*2}, {CX2+XD*2,CY2-YD*2},
 },
 {
  // right side (create a play)
  {CX3,CY3},      {CX3+XD,CY3},      {CX3+XD*2,CY3},
  {CX3,CY3-YD},   {CX3+XD,CY3-YD},   {CX3+XD*2,CY3-YD},
  {CX3,CY3-YD*2}, {CX3+XD,CY3-YD*2}, {CX3+XD*2,CY3-YD*2},
 },
};
 
 
static float xy_control[] = {0.0f, 0.0f, ((NUM_MOVES*MOVE_DELTA)*-1), 0.0f, (NUM_MOVES*MOVE_DELTA), 0.0f};  // none, left, right


static float plysel_text_xys[6][2][2] =
{
					{ {135.0F,358.0F}, {141.0F,356.0F} },		// team 1 score
					{ {387.0F,358.0F}, {378.0F,356.0F} },		// team 2 score
					{ {256.0F,356.0F}, {256.0F,356.0F} },		// down & yards msg
					{ {256.0F,343.0F}, {256.0F,343.0F} },		// quarter msg
					{ {256.0F,323.0F}, {256.0F,323.0F} },		// game clock
					{ {258.0F,299.0F}, {258.0F,299.0F} }		// play select timer
};

//static char *down_str[] = { "1ST", "2ND", "3RD", "4TH" };
extern char *downstr[];

static char *qtr_str[] = { "1ST QUARTER", "2ND QUARTER", "3RD QUARTER", "4TH QUARTER", "OVERTIME 1", "OVERTIME 2", "OVERTIME 3" };

//static char *pnum_str[2] = { "PLAYER 1", "PLAYER 2" };
//
//static float pnum_str_xys[2][2][2] = {										//[pnum][2plr/4plr][x,y]
//			{ {0.0,0.0}, {145.0f,328.0f} },
//			{ {0.0,0.0}, {370.0f,328.0f} } };

static float page_str_xys[][2][2] = {										//[side][res][x,y]
			{ {170, 305}, {170, 305} },
			{ {348, 305}, {348, 305} },
			{ {366, 309}, {366, 309} },
};
			
static int play_name_ids[] = {0x75, 0x76, 0x76};
static int page_str_ids[] =  {0x77, 0x78, 0x78};

// This is defined elsewhere also....  but nobody iis "Static"
//float idiot_box_xys[2][2] = { {0.0f,0.0f}, {85.0f,290.0f} };		// [2plr/4plr][x,y]

//static float idiot_box_xys3[2][2] = { {HRES/2.0f,VRES/2.0F}, {HRES/2.0f,VRES/2.0F} };		// [2plr/4plr][x,y] - used for kick was good

static int cursor_ids[] = { OID_P1CURSOR, OID_P2CURSOR };

static int cursor_edge_snds[] = {67,68};
//static int cursor_snds[] = {65,66};
static int cursor_snds[] = {63,64};
//static int cursor_snds[] = {P1_CURSR_SND,P2_CURSR_SND};

static float idiot_box1_xys[2][2] = {{384.0f,60.0f}, {384.0f,60.0f}};		// [2plr/4plr][x,y]
static float idiot_box2_xys[2][2] = {{128.0f,60.0f}, {128.0f,60.0f}};		// [2plr/4plr][x,y]

static int go_for_two( int );


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void load_play_select_textures(void)
{
	int i;
	char * psz;

	for ( i = 0; i < PSTOTAL_TEX; i++ )
	{
		psz = psel_decal_names[i];

		if (psz == NULL && game_info.game_state != GS_CREATE_PLAY)
		{
			if (i == TMLOGO1_TEX || i == TMLOGO2_TEX)
				psz = ((image_info_t *)teaminfo[game_info.team[i - TMLOGO1_TEX]].pii_logo)->texture_name;

			if (i == TMPLAYS1_TEX || i == TMPLAYS2_TEX)
				psz = teamspecific_textures[game_info.team[i - TMPLAYS1_TEX]];
		}
		if (psz != NULL)
		{
			psel_decals[i] = create_texture(psz,
				PLAY_SELECT_TID,
				0,
				CREATE_NORMAL_TEXTURE,
				GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

			if (game_info.game_state != GS_CREATE_PLAY)
				chk_game_proc_abort();

			if (psel_decals[i] == NULL)
			{
#ifdef DEBUG
				fprintf( stderr, "ERROR in <load_play_select_textures> loading: %s\r\n", psz );
#endif
				increment_audit( TEX_LOAD_FAIL_AUD );
			}
		}
	}
	lock_multiple_textures( PLAY_SELECT_TID, -1 );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void init_play_limbs(int off_side, int side)
{
	obj3d2_data_t * pobj;
	int i, j, k;

	// Init structures and create play limbs
	for (i = 0; i < PLAYSTUF__PLAYS_PER_PAGE; i++)
	{
		pobj = playsel_play_obj_ptrs[i][off_side != side];

		pobj->decal[0] = psel_decals[PLAYSEL_TEX]->texture_handle;
		pobj->ratio[0] = psel_decals[PLAYSEL_TEX]->texture_info.header.aspect_ratio;
		pobj->decal[1] = psel_decals[PSARROW1_TEX]->texture_handle;
		pobj->ratio[1] = psel_decals[PSARROW1_TEX]->texture_info.header.aspect_ratio;

		pobj->odata.x = playsel_plays_xys[side][i][X_VAL];
		pobj->odata.y = playsel_plays_xys[side][i][Y_VAL];
		pobj->limb_obj = &limb_blank;
		create_object("play", OID_AMODE|0x01, OF_NONE, DRAWORDER_PLAYSEL+1, (void *)pobj, plysel_draw_function);
	}

	// Init sprite overlays for default pages; this is only a problem if
	//  <current_pages> is built with an alternate play that also needs
	//  an overlay
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < PLAYSTUF__MAX_PLAY_PAGES; j++)
		{
			for (k = 0; k < PLAYSTUF__PLAYS_PER_PAGE; k++)
			{
				// Do limbs specify an overlay?
				if ((play_pages[i][j].slot_ptrs[k] != NULL) &&
					 play_pages[i][j].slot_ptrs[k]->limb_ptrs[0] != NULL &&
					 play_pages[i][j].slot_ptrs[k]->limb_ptrs[1] == NULL)
				{
					// Yes; create the overlay
					psi_plays[i][j][k] = beginobj(
						play_pages[i][j].slot_ptrs[k]->limb_ptrs[0],
						playsel_plays_xys[(side == 2) ? side : (off_side^i)][k][X_VAL]+1,
						playsel_plays_xys[(side == 2) ? side : (off_side^i)][k][Y_VAL]-1,
						1.6f,
						PLAY_SELECT_TID
						);		
					psi_plays[i][j][k]->id = PLAY_SELECT_ID+2;
					hide_sprite( psi_plays[i][j][k] );
				}
				else
					psi_plays[i][j][k] = NULL;
			}
		}
	}
	old_page[side] = 0;
	old_flip_plays = 0;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void clobber_playsel( void )
{
	int i = 3;

	killall( MESSAGE_PID,     0xfffffff0 );
	killall( MESSAGE2_PID,    0xfffffff0 );
	killall( MESSAGE3_PID,    0xfffffff0 );
	killall( MESSAGE4_PID,    -1 );
	killall( P1_PLAY_SEL_PID, -1 );
	killall( P2_PLAY_SEL_PID, -1 );
	killall( SPFX_PID,        -1 );

	del1c( OID_MESSAGE, -1 );
	delete_multiple_objects( OID_P1CURSOR, -1 );
	delete_multiple_objects( OID_P2CURSOR, -1 );
	delete_multiple_objects( OID_SMKPUFF,  -1 );

	delete_multiple_strings( PLAY_SEL_MSG2_ID, 0xfffffff0 );
	delete_multiple_strings( PLAY_SEL_MSG3_ID, -1 );

	while (i > 0)
	{
		i--;
		delete_multiple_strings( play_name_ids[i], -1 );
		delete_multiple_strings( page_str_ids[i],  -1 );
	}
	del1c( PLAY_SELECT_ID+2, -1 );
}

void clean_playsel( void )
{
	delete_multiple_objects( OID_AMODE, ~0xFF );
	delete_multiple_strings( PLAY_SEL_SCORE_MSG_ID, -1 );
	delete_multiple_strings( PLAY_SEL_MSG_ID,       -1 );
	del1c( PLAY_SELECT_ID, -1 );
}
	
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void play_select(void)
{
	static process_node_t	*pflasher;
	obj3d2_data_t	 		*pobj;
	sprite_info_t			*plysel_obj;
	sprite_info_t			*idiot_bx_obj;
	int i,rnd_wait,nlos;
	static ostring_t 		*pmsg[3];
	int tweak_id;
	
	// Also kill any big messages sprites/textures/process that may have appeared
	killall(MESSAGE_PID,0xFFFFFFF0);
	del1c(OID_MESSAGE,0xFFFFFFFF);
	del1c( 0x5678FACE, -1 );
	delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);

	// Fumble message
	delete_multiple_strings(0x34, 0xffffffff);

	PlayStuf__SetupCurrentPlayPages( TRUE, TRUE );

	// 'NO PLAY SELECT' power-up logic
	// any team with a human must agree
	if ((!(p_status & TEAM_MASK(0)) || (TEAM_MASK(0) & pup_noplaysel)) &&
		(!(p_status & TEAM_MASK(1)) || (TEAM_MASK(1) & pup_noplaysel)))
	{
//		if (ISDRONETEAM(&player_blocks[game_info.off_side]))
			cpu_pick_play(game_info.off_side);
//		else
//			game_info.team_play[game_info.off_side] = short_off_plays[randrng((sizeof(short_off_plays)/sizeof(play_t *)))];
	
//		if (ISDRONETEAM(&player_blocks[!game_info.off_side]))
			cpu_pick_play(!game_info.off_side);
//		else
//			game_info.team_play[!game_info.off_side] = short_def_plays[randrng((sizeof(short_def_plays)/sizeof(play_t *)))];

		// Exit with pages set for audibles (SPECIALS, NO EXTRA PT)
		//  FG & punt are locked-out elsewhere
		PlayStuf__SetupCurrentPlayPages( TRUE, FALSE );
		return;
	}
	
	// suspend all current processes.
	showhide_pinfo(0);
	showhide_status_box(0);
	// kill any flashing of player effects
	killall(PFLASH_PID,0xFFFFFFFF);
	// Suspend all but the indestructable processes (coin clickers, etc.)
	suspend_multiple_processes(0, 0x8000);
	resume_multiple_processes(SPFX_PID, -1);
	if ((pflasher = existp(AMODE_GRP_PID, -1)))
		resume_process(pflasher);
	pflasher = NULL;
	hide_all_3d_objects();
	
	// Tell string handler to keep all letters at the same z
	set_text_z_inc(0);
	
	screen = PLAY_SELECT_SCRN;
	
	// draw top part of screen
	pobj = &top_piece;
	pobj->decal[0] = psel_decals[PLAYSEL_TEX]->texture_handle;
	pobj->ratio[0] = psel_decals[PLAYSEL_TEX]->texture_info.header.aspect_ratio;
	pobj->limb_obj = &limb_topbar_obj;
	pobj->odata.x = 256;
	pobj->odata.y = 295;
	create_object("tpce", OID_AMODE|0x01, OF_NONE, DRAWORDER_PLAYSEL, (void *)pobj, plysel_draw_function);

	// create "trianglar" info piece
	pobj = &info_piece;
	pobj->decal[0] = psel_decals[PLAYSEL_TEX]->texture_handle;
	pobj->ratio[0] = psel_decals[PLAYSEL_TEX]->texture_info.header.aspect_ratio;
	pobj->limb_obj = &limb_triangle_obj;
	pobj->odata.x = 258;
	pobj->odata.y = 297;
	create_object("tpce", OID_AMODE|0x01, OF_NONE, DRAWORDER_PLAYSEL+3, (void *)pobj, plysel_draw_function);

	set_string_id(PLAY_SEL_SCORE_MSG_ID);
	set_text_font(FONTID_BAST25);

	// Create various team related items
	for( i = 0; i < 2; i++ )
	{
		// Create team logo
		plysel_obj = beginobj(teaminfo[game_info.team[i]].pii_logo,
				logo_xys[screen][i][four_plr_ver][X_VAL],
				logo_xys[screen][i][four_plr_ver][Y_VAL]+2,
				1.5F,
				PLAY_SELECT_TID);
		plysel_obj->id = PLAY_SELECT_ID;
		plysel_obj->w_scale /= 1.25f;
		plysel_obj->h_scale /= 1.25f;
		generate_sprite_verts(plysel_obj);

		if ((game_info.down == 4) && (game_info.off_side == i))
		{
			// Quickly flash logo if 4th down
			qcreate("flash",MESSAGE_PID,flash_obj_proc,(int)plysel_obj,12,4,0);
			qcreate("snd",MESSAGE_PID,warning_snd_proc,0,0,0,0);
		}
		// Turn on the page tab img
		plysel_obj = beginobj(&play_tab, (i ? 346.f:170.0f), 317.0f, 1.9f, SCR_PLATE_TID);
		plysel_obj->id = PLAY_SELECT_ID;

		// Print score
		set_text_position(plysel_text_xys[i][four_plr_ver][X_VAL], plysel_text_xys[i][four_plr_ver][Y_VAL], 1.1F);
		oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_score(i));
	}

	// down and yards message
	set_string_id(PLAY_SEL_MSG_ID);
	nlos = game_info.los;
	if (nlos > 50)
		nlos = 100 - nlos;
	set_text_position(plysel_text_xys[2][four_plr_ver][X_VAL], plysel_text_xys[2][four_plr_ver][Y_VAL]-2, 1.1F);
	set_text_font(FONTID_BAST10);
	if( game_info.play_type == PT_EXTRA_POINT )
	{
		set_text_position(plysel_text_xys[2][four_plr_ver][X_VAL], plysel_text_xys[2][four_plr_ver][Y_VAL]-1, 1.1F);
		oprintf( "%dj%dcEXTRA PT. ATTEMPT",
				(HJUST_CENTER|VJUST_BOTTOM),
				LT_YELLOW );

		// FIX!!! for lo res four_plr_ver placement
		if (game_info.off_side == 0)		// is player 1 offense ?
			qcreate("freept",MESSAGE3_PID,display_free_pt_proc,46,237,0,0);
		else
			qcreate("freept",MESSAGE3_PID,display_free_pt_proc,303,237,0,0);
	}
	else if( game_info.first_down == 100 )
	{
		oprintf("%dj%dc%s %dc& %dcG %dcON THE %dc%d",
				(HJUST_CENTER|VJUST_BOTTOM),
				LT_YELLOW,
				downstr[game_info.down-1],
				WHITE,LT_YELLOW,
				WHITE,LT_YELLOW,nlos );
	}
	else
	{
		oprintf("%dj%dc%s %dc& %dc%d %dcON THE %dc%d",
				(HJUST_CENTER|VJUST_BOTTOM),
				LT_YELLOW,
				downstr[game_info.down-1],
				WHITE,LT_YELLOW,
				game_info.first_down - game_info.los,
				WHITE,LT_YELLOW,nlos );
	}

	if (game_info.play_type != PT_EXTRA_POINT)
	{
	// Display FG message when that play is available!
		// FIX!!! for lo res four_plr_ver placement
		if ((game_info.los >= 50 || pup_superfg) &&
				(game_info.down == 4 ||
				(game_info.game_time < 0x00001000 &&
				(game_info.game_quarter == 1 || game_info.game_quarter >= 3))))
		{
//			if ((1<<pnum) & TEAM_MASK(game_info.off_side))
			if (game_info.off_side == 0)		// is player 1 offense ?
				qcreate("freept",MESSAGE3_PID,display_FG_proc,46,70,0,0);
			else
				qcreate("freept",MESSAGE3_PID,display_FG_proc,303,70,0,0);
		}
		else
		if ((game_info.los < 50 && !pup_nopunt) && game_info.down == 4)
		{
			if (game_info.off_side == 0)		// is player 1 offense ?
				qcreate("freept",MESSAGE3_PID,display_punt_proc,46,70,0,0);
			else
				qcreate("freept",MESSAGE3_PID,display_punt_proc,303,70,0,0);
		}
	}	


	// print quarter
	set_text_position(plysel_text_xys[3][four_plr_ver][X_VAL], plysel_text_xys[3][four_plr_ver][Y_VAL]+(is_low_res ? -2 : 0), 1.1F);
	oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, qtr_str[game_info.game_quarter]);

	// print game clock
	set_text_font(FONTID_BAST10);
	set_text_position(plysel_text_xys[4][four_plr_ver][X_VAL],plysel_text_xys[4][four_plr_ver][Y_VAL]+(is_low_res ? -2 : -1), 1.1F);
	
	// Less than 30 seconds left in half or game?
	if (((game_info.game_time & 0x00FFFF00) <= 0x00001f00) && (game_info.game_quarter == 1 || game_info.game_quarter == 3))
	{
		// We will flash the clock red/white if 30 seconds or less is left in half or game
		set_text_color(LT_RED);

		// x:09 thru x:00 seconds left?  If so, add a leading zero
		if ((game_info.game_time & 0x0000FF00) <= 0x00000900)
			pmsg[0] = oprintf( "%dj %d:0%d", (HJUST_CENTER|VJUST_CENTER), ((game_info.game_time & 0x00FF0000)>>16), ((game_info.game_time & 0x0000FF00)>>8));
		else	
			pmsg[0] = oprintf( "%dj %d:%d", (HJUST_CENTER|VJUST_CENTER), ((game_info.game_time & 0x00FF0000)>>16), ((game_info.game_time & 0x0000FF00)>>8));

		// Do some speech when only seconds remain in half/game
		clock_low_sp();
			
		qcreate("pflash", MESSAGE4_PID, flash_text_proc, (int)pmsg[0], 0, 0, 0);
	}
	else
	{
	// x:09 thru x:00 seconds left?  If so, add a leading zero
	if ((game_info.game_time & 0x0000FF00) <= 0x00000900)
		oprintf( "%dj%dc %d:0%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, ((game_info.game_time & 0x00FF0000)>>16), ((game_info.game_time & 0x0000FF00)>>8));
	else	
		oprintf( "%dj%dc %d:%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, ((game_info.game_time & 0x00FF0000)>>16), ((game_info.game_time & 0x0000FF00)>>8));
	}
	
	// "left side" plays (team 1)
	init_play_limbs(game_info.off_side, 0);

	// "right side" plays (team 2)
	init_play_limbs(game_info.off_side, 1);

	// Create team specific offensive play sprites; textures for these
	//  were loaded by <load_play_select_textures>
	for( i = 0; i < 3; i++)
	{
		// Do limbs specify an overlay?
		if (team_slots[game_info.team[game_info.off_side]][i].limb_ptrs[0] != NULL &&
			team_slots[game_info.team[game_info.off_side]][i].limb_ptrs[1] == NULL)
		{
			// Yes; create the overlay
			psi_plays[0][0][i] = beginobj(
				team_slots[game_info.team[game_info.off_side]][i].limb_ptrs[0],
				playsel_plays_xys[game_info.off_side][i][X_VAL]+1,
				playsel_plays_xys[game_info.off_side][i][Y_VAL]-1,
				1.6f,
				PLAY_SELECT_TID );
			psi_plays[0][0][i]->id = PLAY_SELECT_ID+2;
			hide_sprite( psi_plays[0][0][i] );
		}
	}
	
	PlayStuf__SetCurrentPlayPage(OFFENSE, 0);	//	Set offense play page to offense page 0
	PlayStuf__SetCurrentPlayPage(DEFENSE, 0);	//	Set defense play page to defense page 0

#ifndef NOCLOCK
	if (game_info.game_quarter)
		qcreate("timer",TIMER_PID,timedown_play_select,5,plysel_text_xys[5][four_plr_ver][X_VAL],plysel_text_xys[5][four_plr_ver][Y_VAL],0);						//wait 2 second cnt from 5
	else
		// FIX!!!
		// Perhaps we skip this if we have all humans in game with more than x games of experience
		// More seconds for 1st quarter of game
		qcreate("timer",TIMER_PID,timedown_play_select,9,plysel_text_xys[5][four_plr_ver][X_VAL],plysel_text_xys[5][four_plr_ver][Y_VAL],0);						//wait 2 second cnt from 5
#endif

	// Show idiot box (only for first so-many times into play select, skip if all humans have experience)
	if ((show_plysel_idiot_box) &&
		 (((plyr_data_ptrs[0]->games_played < 3) && (p_status & 0x1)) ||
		  ((plyr_data_ptrs[1]->games_played < 3) && (p_status & 0x2)) ||
		  ((plyr_data_ptrs[2]->games_played < 3) && (p_status & 0x4)) ||
		  ((plyr_data_ptrs[3]->games_played < 3) && (p_status & 0x8))))
	{
		show_plysel_idiot_box = show_plysel_idiot_box + 10;
		idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-6.0f, 1.4f, SCR_PLATE_TID);
		idiot_bx_obj->w_scale = 1.52f;
		idiot_bx_obj->h_scale = 1.9f;
		idiot_bx_obj->id = OID_MESSAGE;
		generate_sprite_verts(idiot_bx_obj);
		
		set_string_id(0x34);
		if (is_low_res)
			set_text_font(FONTID_BAST23);
		else	
			set_text_font(FONTID_BAST18);
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+38.0f, 1.1F);
		oprintf( "%dj%dcGENERAL INFO", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		if (is_low_res)
			set_text_font(FONTID_BAST10LOW);
		else	
			set_text_font(FONTID_BAST10);
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+8.0f + (is_low_res ? 3.0f : 0.0f), 1.1F);
		oprintf( "%dj%dcHIDE CURSOR BY QUICKLY TAPPING", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-4.0f, 1.1F);
		oprintf( "%dj%dcUP TWICE USING JOYSTICK", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-22.0f, 1.1F);
		oprintf( "%dj%dcBLUE BUTTON CHOOSES PLAY", (HJUST_CENTER|VJUST_CENTER), LT_BLUE);
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-40, 1.0F);
		oprintf( "%dj%dcRED BUTTON FLIPS PLAY", (HJUST_CENTER|VJUST_CENTER), LT_RED);
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-58.0f, 1.1F);
		oprintf( "%dj%dcWHITE BUTTON CHANGES PAGE", (HJUST_CENTER|VJUST_CENTER), WHITE);

		// Tell string handler to keep all letters at the same z
		set_text_z_inc(0);
		
		// print page number
		delete_multiple_strings(page_str_ids[game_info.off_side], 0xffffffff);
		set_text_font(FONTID_BAST10);
		set_text_position(page_str_xys[game_info.off_side][is_low_res][X_VAL], page_str_xys[game_info.off_side][is_low_res][Y_VAL], 1.8F);
		oprintf( "%dj%dcPAGE 1", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

                delete_multiple_strings(page_str_ids[game_info.off_side ^ 1], 0xffffffff);
		set_text_font(FONTID_BAST10);
                set_text_position(page_str_xys[game_info.off_side ^ 1][is_low_res][X_VAL], page_str_xys[game_info.off_side][is_low_res][Y_VAL], 1.8F);
		oprintf( "%dj%dcPAGE 1", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

		if (game_info.off_side)
		{	// right side is offense
			idiot_bx_obj = beginobj(&playresult, idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			// Allow each sequential letter to overlap the previous letter - turn off at end!
			if (is_low_res)
				set_text_font(FONTID_BAST23);
			else	
				set_text_font(FONTID_BAST18);
			set_string_id(0x34);
			set_text_position(idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.1F);

			set_text_color(LT_RED);
			//pmsg[0] = oprintf( "%djDEFENSE", (HJUST_CENTER|VJUST_CENTER));
			pmsg[0] = oprintf( "%djOFFENSE", (HJUST_CENTER|VJUST_CENTER));
			
			// Print defense over turbo bar
			idiot_bx_obj = beginobj(&playresult, idiot_box2_xys[four_plr_ver][X_VAL], idiot_box2_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			// Allow each sequential letter to overlap the previous letter - turn off at end!
			if (is_low_res)
				set_text_font(FONTID_BAST23);
			else	
				set_text_font(FONTID_BAST18);
			set_string_id(0x34);
			set_text_position(idiot_box2_xys[four_plr_ver][X_VAL], idiot_box2_xys[four_plr_ver][Y_VAL], 1.1F);
//			set_text_position(SPRITE_HRES/2.0f+172,(SPRITE_VRES/2.0f)+28.0f+127.0f-3-289, 1.0F);

			set_text_color(LT_GREEN);
			pmsg[1] = oprintf( "%djDEFENSE", (HJUST_CENTER|VJUST_CENTER));
//			pmsg[1] = oprintf( "%djOFFENSE", (HJUST_CENTER|VJUST_CENTER));
		}
		else
		{	// left side is offense
			idiot_bx_obj = beginobj(&playresult, idiot_box2_xys[four_plr_ver][X_VAL], idiot_box2_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			if (is_low_res)
				set_text_font(FONTID_BAST23);
			else	
				set_text_font(FONTID_BAST18);
			set_string_id(0x34);
			set_text_position(idiot_box2_xys[four_plr_ver][X_VAL], idiot_box2_xys[four_plr_ver][Y_VAL], 1.1F);

			set_text_color(LT_RED);
			pmsg[0] = oprintf( "%djOFFENSE", (HJUST_CENTER|VJUST_CENTER));

			// Print defense over turbo bar
			idiot_bx_obj = beginobj(&playresult, idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			// Allow each sequential letter to overlap the previous letter - turn off at end!
			if (is_low_res)
				set_text_font(FONTID_BAST23);
			else	
				set_text_font(FONTID_BAST18);
			set_string_id(0x34);
			set_text_position(idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.1F);

			set_text_color(LT_GREEN);
			pmsg[1] = oprintf( "%djDEFENSE", (HJUST_CENTER|VJUST_CENTER));
		}

		pflasher = qcreate("pflash", 0, flash_text_proc, (int)pmsg[0], (int)pmsg[1], 0, 0);
		
		// Only human controlled offensive player can whack out of this
		if (four_plr_ver)
		{
			if ((p_status & (1<<(game_info.off_side * 2))) ||
				(p_status & (1<<((game_info.off_side * 2) + 1))))
				wait_for_any_butn_press_active_team(40, tsec*11,game_info.off_side);
		}
		else
		{
			if (p_status & (game_info.off_side + 1))
				wait_for_any_butn_press_active_team(40, tsec*11,game_info.off_side);
			else	
				// this happens if CPU somehow got ball on first couple of plays
				wait_for_any_butn_press(40, tsec*11);
		}
		snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME1,127,PRIORITY_LVL4);
		
		kill(pflasher, 0);
		delete_multiple_strings(0x34, 0xffffffff);
		del1c(OID_MESSAGE,0xFFFFFFFF);
		
		// Don't show anymore when 0
		show_plysel_idiot_box = show_plysel_idiot_box - 11;
	}
	else
		// Needs to be zero for free extra point display
		show_plysel_idiot_box = 0;
	
//	// at start...no plays should be flipped.
//	old_flip_plays = 0;

	// update team 1's plays, page 1 - non-flipped
	update_play_page(0,0,0,0);

	// update team 2's plays, page 1 - non-flipped
	update_play_page(1,0,0,0);
	
	// OK...now select play!!
	
	exit_status = 0;
	force_selection = 0;
	show_freept_idiot_box = 0;
	game_info.off_flip = 0;			// dont show flipped plays...if flipped them last session!

	// set exit timer for a delay if in GS_ATTRACT_MODE
	rnd_wait = (game_info.game_state == GS_ATTRACT_MODE) ? (3.0 * 57) : 1;

	qcreate("tm1plysl",P1_PLAY_SEL_PID,play_select_cntrl,SIDE_0,0,0,0);
	qcreate("tm2plysl",P2_PLAY_SEL_PID,play_select_cntrl,SIDE_1,0,0,0);

	// cycle textures on arrows
//	qcreate("cycarrow",SPFX_PID,cycle_play_arrows,0,0,0,0);

	// sleep to let play select processes initialize.
	sleep(1);

	// drone teams on offense decide here if they'll go for two
	if (game_info.play_type == PT_EXTRA_POINT)
	{
		// is a drone team?
		if (((four_plr_ver) && !(p_status & (3<<(game_info.off_side*2)))) ||
			((!four_plr_ver) && !(p_status & (1<<game_info.off_side))))
		{
			if( !go_for_two( game_info.off_side ))
			{
				game_info.team_play[game_info.off_side] = &play_extra_point;
				show_freept_idiot_box = TRUE;
				force_selection = TRUE;
				rnd_wait = 1;
			}
		}
	}

	// wait until player(s) are done selecting plays
	do
	{
		sleep(1);
		
		if (!exit_status)
			// both teams picked...now just dec wait count
			rnd_wait = GREATER(rnd_wait-1,0);

	} while ((rnd_wait != 0) && (!force_selection));
	
	// let 'play_select_cntrl' processes finish...
	sleep(1);
		
	// if a NULL is returned from PLAY_SELECT...no play was chosen or first play was or no human
	if (game_info.team_play[0] == (play_t *)-1 )
		cpu_pick_play(0);
	else if (game_info.team_play[0] == &play_extra_point)
		show_freept_idiot_box = TRUE;
		
	if (game_info.team_play[1] == (play_t *)-1 )
		cpu_pick_play(1);
	else if (game_info.team_play[1] == &play_extra_point)
		show_freept_idiot_box = TRUE;

	// Audit the selected plays
	if (!four_plr_ver)
	{
		if (p_status & 0x01)
			audit_play(game_info.team_play[0]);
		if (p_status & 0x02)
			audit_play(game_info.team_play[1]);
	}
	else
	{
		if (p_status & 0x03)
			audit_play(game_info.team_play[0]);
		if (p_status & 0x0c)
			audit_play(game_info.team_play[1]);
	}
	
	// kill_countdown_timer();
	killall( TIMER_PID, -1 );
	delete_multiple_strings( TIMER_MSG_ID, -1 );

	// do clean-up
	clobber_playsel();

	if (four_plr_ver)
		// delete numbers over cursors
		del1c( PLAY_SELECT_ID+1, -1 );
	
	// Turn on free pt idiot box	
	if (show_freept_idiot_box)
	{
		// blank out the plays
		// "left side" plays (player 1)
		for (i=0; i < PLAYSTUF__PLAYS_PER_PAGE; i++)
		{
			pobj = playsel_play_obj_ptrs[i][0];
			pobj->limb_obj = &limb_blank;
		}

		// "right side" plays (player 2)
		for (i=0; i < PLAYSTUF__PLAYS_PER_PAGE; i++)
		{
			pobj = playsel_play_obj_ptrs[i][1];
			pobj->limb_obj = &limb_blank;
		}
	
		idiot_bx_obj = beginobj(&playresult, SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-6.0f, 1.4f, SCR_PLATE_TID);
		idiot_bx_obj->w_scale = 1.52f;
		idiot_bx_obj->h_scale = 1.9f;
		idiot_bx_obj->id = OID_MESSAGE;
		generate_sprite_verts(idiot_bx_obj);
		
		set_string_id(0x34);
		if (is_low_res)
			set_text_font(FONTID_BAST23);
		else
			set_text_font(FONTID_BAST18);
			
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)+32.0f, 1.1F);
	
		miss_kick = 0;		

		if (((p_status & (game_info.off_side + 1)) && !(four_plr_ver)) ||
			(((p_status & (1<<(game_info.off_side * 2))) || (p_status & (1<<((game_info.off_side * 2)+1)))) &&
			 (four_plr_ver)))
		{
			// Human team  just scored
			i = compute_score(game_info.off_side) - compute_score(game_info.off_side ^ 1);
			if ((i > 14) && (game_info.game_quarter > 2) && (randrng(100) < 2))
			{
				oprintf( "%dj%dcKICK WAS NO GOOD!", (HJUST_CENTER|VJUST_CENTER), LT_RED);
				miss_kick = 1;		
			}
			else
				oprintf( "%dj%dcKICK WAS GOOD!", (HJUST_CENTER|VJUST_CENTER), LT_GREEN);
		}
		else
		{
			// CPU player just scored
			i = compute_score(game_info.off_side) - compute_score(game_info.off_side ^ 1);
			if ((i > 8) && (game_info.game_quarter > 1) && (randrng(100) < 3))
			{
				oprintf( "%dj%dcKICK WAS NO GOOD!", (HJUST_CENTER|VJUST_CENTER), LT_RED);
				miss_kick = 1;		
			}
			else
				oprintf( "%dj%dcKICK WAS GOOD!", (HJUST_CENTER|VJUST_CENTER), LT_GREEN);
		}

		set_text_font(FONTID_BAST18);
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-5.0f, 1.1F);

		if (four_plr_ver)
		{
			if (game_info.off_side)
				oprintf( "%dj%dcTEAM 1", (HJUST_CENTER|VJUST_CENTER), WHITE);
			else
				oprintf( "%dj%dcTEAM 2", (HJUST_CENTER|VJUST_CENTER), WHITE);
		}
		else
		{
			if (game_info.off_side)
				oprintf( "%dj%dcPLAYER 1", (HJUST_CENTER|VJUST_CENTER), WHITE);
			else
				oprintf( "%dj%dcPLAYER 2", (HJUST_CENTER|VJUST_CENTER), WHITE);
		}
			
		set_text_position(SPRITE_HRES/2.0f,(SPRITE_VRES/2.0f)-35, 1.1F);
		oprintf( "%dj%dcGET READY FOR KICKOFF", (HJUST_CENTER|VJUST_CENTER), WHITE);

		// Tell string handler to keep all letters at the same z
		set_text_z_inc(0);
		
//		// print page number
//		delete_multiple_strings(page_str_ids[game_info.off_side], 0xffffffff);
//		set_text_font(FONTID_BAST10);
//		set_text_position(page_str_xys[game_info.off_side][four_plr_ver][X_VAL], page_str_xys[game_info.off_side][four_plr_ver][Y_VAL], 1.8F);
//		oprintf( "%dj%dcPAGE 1", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);

		// !!!FIX for 4 plyr kit
		// if right SIDE is offense
		if (game_info.off_side)
		{
			// Print offense over turbo bar
			idiot_bx_obj = beginobj(&playresult, idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			// Allow each sequential letter to overlap the previous letter - turn off at end!
			if (is_low_res)
				set_text_font(FONTID_BAST23);
			else	
				set_text_font(FONTID_BAST18);
				
			set_string_id(0x34);
			set_text_position(idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.1F);
//			set_text_position(SPRITE_HRES/2.0f-170,(SPRITE_VRES/2.0f)+28.0f+127.0f-3-289, 1.0F);

			set_text_color(LT_GREEN);
			pmsg[0] = oprintf( "%djOFFENSE", (HJUST_CENTER|VJUST_CENTER));
			
			// Print defense over turbo bar
			idiot_bx_obj = beginobj(&playresult, idiot_box2_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			// Allow each sequential letter to overlap the previous letter - turn off at end!
//			set_text_font(FONTID_BAST18);
//			set_string_id(0x34);
			set_text_position(idiot_box2_xys[four_plr_ver][X_VAL], idiot_box2_xys[four_plr_ver][Y_VAL], 1.0F);
//			set_text_position(SPRITE_HRES/2.0f+172,(SPRITE_VRES/2.0f)+28.0f+127.0f-3-289, 1.0F);

			set_text_color(LT_RED);
			pmsg[1] = oprintf( "%djDEFENSE", (HJUST_CENTER|VJUST_CENTER));
		}
		else
		{	
			// Print offense over turbo bar
			idiot_bx_obj = beginobj(&playresult, idiot_box2_xys[four_plr_ver][X_VAL], idiot_box2_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			// Allow each sequential letter to overlap the previous letter - turn off at end!
			if (is_low_res)
				set_text_font(FONTID_BAST23);
			else	
				set_text_font(FONTID_BAST18);
				
			set_string_id(0x34);
			set_text_position(idiot_box2_xys[four_plr_ver][X_VAL], idiot_box2_xys[four_plr_ver][Y_VAL], 1.1F);
//			set_text_position(SPRITE_HRES/2.0f+172,(SPRITE_VRES/2.0f)+28.0f+127.0f-3-289, 1.0F);

			set_text_color(LT_GREEN);
			pmsg[0] = oprintf( "%djOFFENSE", (HJUST_CENTER|VJUST_CENTER));

			// Print defense over turbo bar
			idiot_bx_obj = beginobj(&playresult, idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.4f, SCR_PLATE_TID);
			idiot_bx_obj->w_scale = 0.60f;
			idiot_bx_obj->h_scale = 0.60f;
			idiot_bx_obj->id = OID_MESSAGE;
			generate_sprite_verts(idiot_bx_obj);

			// Allow each sequential letter to overlap the previous letter - turn off at end!
//			set_text_font(FONTID_BAST18);
//			set_string_id(0x34);
			set_text_position(idiot_box1_xys[four_plr_ver][X_VAL], idiot_box1_xys[four_plr_ver][Y_VAL], 1.1F);
//			set_text_position(SPRITE_HRES/2.0f-170,(SPRITE_VRES/2.0f)+28.0f+127.0f-3-289, 1.0F);

			set_text_color(LT_RED);
			pmsg[1] = oprintf( "%djDEFENSE", (HJUST_CENTER|VJUST_CENTER));
		}
		pflasher = qcreate("pflash", 0, flash_text_proc, (int)pmsg[0], (int)pmsg[1], 0, 0);

		sleep(15);

		// Stop curent music
		snd_stop_track(SND_TRACK_0);
		snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
		// Kill extra point groove that might not have awakened yet
		killall(DELAY_PID,0xFFFFFFFF);
		
		if (miss_kick)
			// No Good!
			snd_scall_bank(ancr_swaped_bnk_str,NO_GOOD_SP,VOLUME3,127,LVL4);
		else
		{
			extra_point_speech();
			snd_scall_bank(plyr_bnk_str,TWO_PT_SND,FANFARE_VOL,127,PRIORITY_LVL4);
			// free extra point  score, then kickoff
#ifdef PLAY_DEBUG
			fprintf( stderr, "result: extra point\n" );
#endif
			// Add 1 point
			game_info.scores[game_info.off_side][game_info.game_quarter] += 1;
		}
		
		// delete old scores, print new scores
		delete_multiple_strings(PLAY_SEL_SCORE_MSG_ID, 0xffffffff);
		set_string_id(PLAY_SEL_SCORE_MSG_ID);
		set_text_font(FONTID_BAST25);
		set_text_position(plysel_text_xys[0][four_plr_ver][X_VAL], plysel_text_xys[0][four_plr_ver][Y_VAL], 1.1F);
		oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_score(0));
		set_text_position(plysel_text_xys[1][four_plr_ver][X_VAL], plysel_text_xys[1][four_plr_ver][Y_VAL], 1.1F);
		oprintf( "%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, compute_score(1));

		// For attract mode display of play select page
		if (p_status)
			wait_for_any_butn_press(40,tsec*5);
		else
			sleep( tsec*2 );
		snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME1,127,PRIORITY_LVL5);
		
		kill(pflasher, 0);
		delete_multiple_strings(0x34, 0xffffffff);
		del1c(OID_MESSAGE,0xFFFFFFFF);
	}

	// do clean-up
	clean_playsel();

	showhide_pinfo(1);
	showhide_status_box(1);
	resume_all();
	unhide_all_3d_objects();
	hide_multiple_objects( OID_REPLAY, -1 );

	// Exit with pages set for audibles (SPECIALS, NO EXTRA PT)
	//  FG & punt are locked-out elsewhere
	PlayStuf__SetupCurrentPlayPages( TRUE, FALSE );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//int check_for_secret_play(int plyr, int slot, int *sw_que)
//{
//	int i;
//
//	// scan list of valid combos
//	i = 0;
//	while (secret_play_combos[i].slot_num != -1)
//	{
//		if (secret_play_combos[slot].slot_num == slot)
//		{
//			
//		}
//		else
//			i++;
//	}
//	return 0;
//}

#define RANDOM_SELECT 1

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void play_select_cntrl(int *args)
{
	int				joy_sws,prev_joy_sws,butn_sws,page;
	int				flsh_cnt,p_num,cur_pos,flip_plays,tm_num;
	obj3d2_data_t 	*pcursr;
	int 			up_cnt, i;

#ifdef RANDOM_SELECT
	int left_cnt;
	int select_random;
	int ran_val;
	int max_ran;
	int max_flip = 4;
	int max_page = 6;
	int cnt_page = 0;
	int cnt_flip = 0;
#endif

	sprite_info_t 	*num_obj = NULL;
	int				color_inc;
	int				found_combo = 0;
	int				que_pos = 0;
	int				*joystick_que[QUE_SIZE];
	int				side;

	hidden_off_cursor = 0;			// 0=no offensive cursor is not hidden - used for "flip" message
	page = 0;
	flip_plays = 0;		// no
	up_cnt = 0;

#ifdef RANDOM_SELECT
	left_cnt = 0;
	select_random = 0;
	max_ran = 10;
	ran_val = 0;
#endif

	p_num = 0;
	tm_num = args[0];

	side = (tm_num == game_info.off_side) ? OFFENSE : DEFENSE;

#if FG_PUNT == 1
//	DEBUG MODE - FIELD GOAL EVERY TIME	

	if((side == OFFENSE) && !(p_status & TEAM_MASK(tm_num)))		// NON-HUMAN TEAM
	{
		game_info.team_play[tm_num] = &play_field_goal;	//	Auto-pick the field goal play
		sleep(1);
		die(0);
	}

#elif FG_PUNT == 2
//	DEBUG MODE - PUNT EVERY TIME
	if((side == OFFENSE) && !(p_status & TEAM_MASK(tm_num)))		// NON-HUMAN TEAM
	{
		game_info.team_play[tm_num] = &play_punt;	//	Auto-pick the punt play
		sleep(1);
		die(0);
	}

#else
	//	NORMAL GAME MODE

#endif

	// Init...play picked stuff... (***NOTE*** If this is unchanged...cpu will pick play)
	game_info.team_play[tm_num] = (play_t *)-1;

	cur_pos = 0;
	
	// only proceed if player is in game
	if (four_plr_ver)
	{
		while ( !(p_status & (1<<(tm_num*2))) && !(p_status & (1<<((tm_num*2)+1))) )
			sleep(1);
	}
	else
	{
		while (!(p_status & (1<<tm_num)))
			sleep(1);
	}

// NEW
	if (!four_plr_ver)
		p_num = args[0];		// in 2 player version -- side is player number 0 or 1
	else
	{
		i = tm_num + tm_num;
		if ((p_status & (3<<i)) == (3<<i))
			p_num = (side == OFFENSE) ? off_captain() : game_info.captain;
		else
			p_num = ((p_status >> i) & 0x3) - 1;
		p_num += i;
	}
// NEW
		
	// set players bit
	exit_status |= 1<<p_num;
	
	// Don't allow cursors to begin for awhile
	sleep (20);
	
	color_inc = 0x00020202;

	pcursr = (tm_num ? &cursor2 : &cursor1);
	// create player cursor
	pcursr->decal[0] = psel_decals[PLAYSEL_TEX]->texture_handle;
	pcursr->ratio[0] = psel_decals[PLAYSEL_TEX]->texture_info.header.aspect_ratio;
	pcursr->limb_obj = cursor_limb_ptrs[tm_num];
	pcursr->odata.x = playsel_plays_xys[tm_num][0][X_VAL];
	pcursr->odata.y = playsel_plays_xys[tm_num][0][Y_VAL];
	pcursr->move = p_num;
	create_object("cursor", cursor_ids[tm_num], OF_NONE, DRAWORDER_PLAYSEL+2, (void *)pcursr, cursor_draw_function);
//	create_object("cursor", cursor_ids[tm_num], OF_NONE, DRAWORDER_PLAYSEL+2, (void *)pcursr, plysel_draw_function);

	// create NUMBER on cursor (player picking plays)
	if (four_plr_ver)
	{
		num_obj = beginobj(arrow_imgs[p_num],
							pcursr->odata.x + 42.0f,
							pcursr->odata.y,
							1.2f,
							SCR_PLATE_TID);
		num_obj->id = PLAY_SELECT_ID+1;
	}


	// loop...reading joystick and buttons
	prev_joy_sws = -1;
	flsh_cnt = 0;
	pcursr->odata.flags &= ~PF_FLASHWHITE;
	while (force_selection != 1)
	{

#ifdef ADJ_OVERLAY_XY
		if((get_but_val_cur(p_num)) && !(get_but_val_down(p_num)))
		{
			float dx = 0;
			float dy = 0;
			int pages, plays;

			if(get_joy_val_down(p_num) & JOY_UP)    dy =  1;
			if(get_joy_val_down(p_num) & JOY_DOWN)  dy = -1;
			if(get_joy_val_down(p_num) & JOY_LEFT)  dx = -1;
			if(get_joy_val_down(p_num) & JOY_RIGHT) dx =  1;


			for (pages = 0; pages < PLAYSTUF__MAX_PLAY_PAGES; pages++)
			{
				for (plays = 0; plays < PLAYSTUF__PLAYS_PER_PAGE; plays++)
				{
					if (psi_plays[side][pages][plays])
					{
						psi_plays[side][pages][plays]->x += dx;
						psi_plays[side][pages][plays]->y += dy;
						generate_sprite_verts(psi_plays[side][pages][plays]);
					}
				}
			}
		}
		else
		{
#endif //ADJ_OVERLAY_XY

		joy_sws = get_joy_val_cur(p_num);

//		// record joystick movement
//		if (joy_sws)
//		{
//			if (que_pos == QUE_SIZE-1)
//			{
//				//slide all in que down one.
////				que_pos = QUE_SIZE-1;
//				for (i=0; i < QUE_SIZE-1; i++)
//					joystick_que[i] = joystick_que[i+1];
//				joystick_que[que_pos] = joy_sws;
//			}
//			else
//				joystick_que[que_pos] = joy_sws;

				
//			found_combo = check_for_secret_play(p_num,cur_pos,joystick_que);
//			que_pos++;
//		}

		// process joystick

#if RANDOM_SELECT
		if ((joy_sws != prev_joy_sws) || (select_random> 0))
#else
		if (joy_sws != prev_joy_sws)
#endif
		{
			prev_joy_sws = joy_sws;
#if RANDOM_SELECT
			if (select_random > 0)
			{
				joy_sws = 0;
				select_random += 1;
				ran_val = randrng(4);
				switch(ran_val)
				{
					case 0:
						joy_sws = JOY_UP;
						up_cnt = 0;
						break;
					case 1:
						joy_sws = JOY_RIGHT;
						break;
					case 2:
						joy_sws = JOY_LEFT;
						break;
					case 3:
						joy_sws = JOY_DOWN;
						break;
				}			
			}
#endif
			switch(joy_sws) 
			{
				case JOY_UP:
					if ((cur_pos == 0) && (++up_cnt == 2))			// Need 3 consecutive up pushes to hide cursor
					{
						hide_multiple_objects(cursor_ids[tm_num], 0xffffffff);
						if (four_plr_ver)
							hide_sprite(num_obj);
						snd_scall_bank(cursor_bnk_str,HIDE_CURSOR_SND,VOLUME4,127,PRIORITY_LVL2);
						// If offensive team, set flag telling update_play_page to not display "flip"
						if (tm_num == game_info.off_side)
							hidden_off_cursor = 1;
						
					}
					if (cur_pos > 2)
					{
						cur_pos-=3;
						pcursr->odata.x = playsel_plays_xys[tm_num][cur_pos][X_VAL];
						pcursr->odata.y = playsel_plays_xys[tm_num][cur_pos][Y_VAL];
						if (four_plr_ver)
						{

							num_obj->x = pcursr->odata.x + 42.0f;
							num_obj->y = pcursr->odata.y;
							generate_sprite_verts(num_obj);
						}
						snd_scall_bank(cursor_bnk_str,cursor_snds[tm_num],VOLUME4,127,PRIORITY_LVL2);
					}
					else if (hidden_off_cursor)
						snd_scall_bank(cursor_bnk_str,cursor_edge_snds[tm_num],VOLUME4,127,PRIORITY_LVL1);
					break;
					
				case JOY_DOWN:
					if (cur_pos < 6)
					{
						up_cnt = 0;
						cur_pos+=3;
						pcursr->odata.x = playsel_plays_xys[tm_num][cur_pos][X_VAL];
						pcursr->odata.y = playsel_plays_xys[tm_num][cur_pos][Y_VAL];
						if (four_plr_ver)
						{
							num_obj->x = pcursr->odata.x + 42.0f;
							num_obj->y = pcursr->odata.y;
							generate_sprite_verts(num_obj);
						}
						snd_scall_bank(cursor_bnk_str,cursor_snds[tm_num],VOLUME4,127,PRIORITY_LVL2);
					}
					else if (hidden_off_cursor)
						snd_scall_bank(cursor_bnk_str,cursor_edge_snds[tm_num],VOLUME4,127,PRIORITY_LVL1);
					break;
					
				case JOY_LEFT:
					if ((cur_pos != 0) && (cur_pos != 3) && (cur_pos != 6))
					{
						cur_pos--;
						pcursr->odata.x = playsel_plays_xys[tm_num][cur_pos][X_VAL];
						pcursr->odata.y = playsel_plays_xys[tm_num][cur_pos][Y_VAL];
						if (four_plr_ver)
						{
							num_obj->x = pcursr->odata.x + 42.0f;
							num_obj->y = pcursr->odata.y;
							generate_sprite_verts(num_obj);
						}
						snd_scall_bank(cursor_bnk_str,cursor_snds[tm_num],VOLUME4,127,PRIORITY_LVL2);
#if RANDOM_SELECT
					} else if ((cur_pos == 0) && (++left_cnt == 2) && (!select_random)) {
//						butn_sws = get_but_val_cur(p_num) & (A_BUTTON|B_BUTTON|TURBO_BUTTON);
//						if (butn_sws == TURBO_BUTTON)
//						{
							snd_scall_bank(cursor_bnk_str,HIDE_CURSOR_SND,VOLUME4,127,PRIORITY_LVL2);
							select_random = -1;
//							max_ran = 20 + randrng(10);
							max_ran = 30;
//						}
#endif
					} else if (hidden_off_cursor)
						snd_scall_bank(cursor_bnk_str,cursor_edge_snds[tm_num],VOLUME4,127,PRIORITY_LVL1);
					break;
							
				case JOY_RIGHT:
					if ((cur_pos != 2) && (cur_pos != 5) && (cur_pos != 8))
					{
						up_cnt = 0;
						cur_pos++;
						pcursr->odata.x = playsel_plays_xys[tm_num][cur_pos][X_VAL];
						pcursr->odata.y = playsel_plays_xys[tm_num][cur_pos][Y_VAL];
						if (four_plr_ver)
						{
							num_obj->x = pcursr->odata.x + 42.0f;
							num_obj->y = pcursr->odata.y;
							generate_sprite_verts(num_obj);
						}
						snd_scall_bank(cursor_bnk_str,cursor_snds[tm_num],VOLUME4,127,PRIORITY_LVL2);
					}
					else if (hidden_off_cursor)
						snd_scall_bank(cursor_bnk_str,cursor_edge_snds[tm_num],VOLUME4,127,PRIORITY_LVL1);
					break;
			}
		}
		butn_sws = get_but_val_down(p_num) & (A_BUTTON|B_BUTTON|TURBO_BUTTON);
		// On defense, all buttons pick play
		// On offense, turbo switches page, butn "a" flips play, butn "b" chooses play
#if RANDOM_SELECT
		if ((butn_sws) || (select_random> 0))
#else
		if (butn_sws)
#endif
		{

#if RANDOM_SELECT
			if (select_random > 0)
			{
				butn_sws = 0;
				select_random += 1;
				if (side == OFFENSE)
				{
					ran_val = randrng(2);
					if (ran_val == 1)
					{
						if (cnt_flip < max_flip)
						{
							butn_sws = A_BUTTON;		// Flip Play
							cnt_flip += 1;
						}
					} else if (ran_val == 0) {
						if (cnt_page < max_page)
						{
							butn_sws = TURBO_BUTTON;	// Change Page
							cnt_page += 1;
						}
//					} else {
//						select_random -= 1;			// Another stick move possibly
					}
				} else {
					ran_val = randrng(2);
					if (ran_val == 1)
					{
						if (cnt_page < max_page)
						{
							butn_sws = TURBO_BUTTON;
							cnt_page += 1;
						}
					}
				}
				if (select_random < 1)
					select_random = 1;
				// we done?
				if (select_random > max_ran)
				{
					select_random = -2;
					butn_sws = B_BUTTON;
				}
			}
#endif

//fprintf(stderr, "sel_ran = %d force = %d\n", select_random, force_selection);


#if 1
			switch (butn_sws)
			{
				case TURBO_BUTTON:
					if ( (select_random == -1) && (cur_pos == 0) && (joy_sws == JOY_LEFT))
					{
						select_random = 1;
						cur_pos = 5;
					} else {
						if (side == OFFENSE)
						{
							snd_scall_bank(cursor_bnk_str,SWITCH_SND,VOLUME4,127,PRIORITY_LVL3);

							page++;
							if((page == PLAYSTUF__OFF_PAGE_EXTRA) && (pup_playbook != 0) && (pup_secret_page == 0))
								page++;

							if((page == PLAYSTUF__OFF_PAGE_ID_CUSTOM) && ((pup_playbook == 2) || (game_info.has_custom[p_num] == FALSE)))
								page++;

							if (page >= PLAYSTUF__NUM_OFFENSE_PAGES)
								page = 0;
						} else {
							snd_scall_bank(cursor_bnk_str,SWITCH_SND,VOLUME4,127,PRIORITY_LVL3);

							if(++page >= PLAYSTUF__NUM_DEFENSE_PAGES)
								page = 0;
						}
						update_play_page(tm_num,page,flip_plays,cur_pos);
					}
					break;
				case B_BUTTON:
					PlaySel__ChoosePlay(side, page, p_num, cur_pos, tm_num, 0);
					break;
				case A_BUTTON:
					if (side == OFFENSE)
					{
						//	'A' button flips plays for offense
						snd_scall_bank(cursor_bnk_str,SWITCH2_SND,VOLUME4,127,PRIORITY_LVL2);

						flip_plays ^= 1;
						game_info.off_flip = flip_plays;

						update_play_page(tm_num,page,flip_plays,cur_pos);
					}
					break;
				default:
					break;
			}
#else
			if (butn_sws == TURBO_BUTTON)
			{
				if ((select_random == -1) && (cur_pos == 0) && (joy_sws == JOY_LEFT))
				{
					select_random = 1;
					cur_pos = 5;		// put it in the middle
				}

				// CHANGE page!!
				if(side == OFFENSE)
				{	// offense
					snd_scall_bank(cursor_bnk_str,SWITCH_SND,VOLUME4,127,PRIORITY_LVL3);

					page++;
					if((page == PLAYSTUF__OFF_PAGE_EXTRA) && (pup_playbook != 0) && (pup_secret_page == 0))
						page++;

					if((page == PLAYSTUF__OFF_PAGE_ID_CUSTOM) && ((pup_playbook == 2) || (game_info.has_custom[p_num] == FALSE)))
						page++;

					if (page >= PLAYSTUF__NUM_OFFENSE_PAGES)
						page = 0;
				}
				else
				{	// defense
					snd_scall_bank(cursor_bnk_str,SWITCH_SND,VOLUME4,127,PRIORITY_LVL3);

					if(++page >= PLAYSTUF__NUM_DEFENSE_PAGES)
						page = 0;
				}
				update_play_page(tm_num,page,flip_plays,cur_pos);
			}
			else
			if ((side == DEFENSE) || (butn_sws == B_BUTTON))
			{
//fprintf(stderr, "out here!!!\n");
				// SELECT play!!
				PlaySel__ChoosePlay(side, page, p_num, cur_pos, tm_num, 0);
			}
#if RANDOM_SELECT
			else if ((side == OFFENSE) && (butn_sws == A_BUTTON)) 
#else
			else
#endif
			{
				//	'A' button flips plays for offense
				snd_scall_bank(cursor_bnk_str,SWITCH2_SND,VOLUME4,127,PRIORITY_LVL2);

				flip_plays ^= 1;
				game_info.off_flip = flip_plays;

				update_play_page(tm_num,page,flip_plays,cur_pos);

			}
#endif


		}
#ifdef ADJ_OVERLAY_XY
		}
#endif //ADJ_OVERLAY_XY

		flsh_cnt++;
		if (flsh_cnt == 25)
			pcursr->odata.flags |= PF_FLASHWHITE;
		if (flsh_cnt >= 31)
		{	// only hold white for 6 ticks
			pcursr->odata.flags &= ~PF_FLASHWHITE;
			flsh_cnt = 0;
		}

//		cursor_color += color_inc;
	
		sleep(1);

#if RANDOM_SELECT
	if (select_random > 0)
		sleep(2);
#endif
	}

	if(game_info.team_play[tm_num] == (play_t *)-1)
	{
	//	Player didn't choose play before time ran out... so automatically choose play

		PlaySel__ChoosePlay(side, page, p_num, cur_pos, tm_num, 1);
	}
}


//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void cursor_draw_function(void *oi)
{
	obj_3d	*pobj = (obj_3d *)oi;
	obj3d2_data_t *pcursor = (obj3d2_data_t *)oi;

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);

	if (pcursor->move == 0)
		grConstantColorValue(0X000000FF);		// blue
	else if (pcursor->move == 1)
		grConstantColorValue(0X0000FF00);		// green
	else if (pcursor->move == 2)
		grConstantColorValue(0X00FF0000);		// red
	else
		grConstantColorValue(0X00FFFF00);		// yellow
	
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthMask(FXTRUE);

	grChromakeyMode(GR_CHROMAKEY_DISABLE);

	if (pobj->flags & PF_FLASHWHITE)
		guColorCombineFunction(GR_COLORCOMBINE_ONE);
	else
//		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);

//	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
		
	grAlphaBlendFunction(GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA);

	grDepthBufferFunction(GR_CMP_ALWAYS);

	render_plate(pcursor->limb_obj, &(pobj->x), &(xy_control[0]), pcursor->decal, pcursor->ratio);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
void plysel_draw_function( void *oi )
{
	obj_3d	*pobj = (obj_3d *)oi;
	obj3d2_data_t *pplysel = (obj3d2_data_t *)oi;

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);

	// Dim down screen if idiot box is being displayed
//	if (show_plysel_idiot_box == 2)
//		grConstantColorValue(0X80000000);
//	else
//		grConstantColorValue(0XFF000000);
//
	grConstantColorValue(0XFF000000);
	
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthMask(FXTRUE);

	grChromakeyMode(GR_CHROMAKEY_DISABLE);

	if (pobj->flags & PF_FLASHWHITE)
		guColorCombineFunction(GR_COLORCOMBINE_ONE);
	else
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
		
	grAlphaBlendFunction(GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA);

	grDepthBufferFunction(GR_CMP_ALWAYS);

	render_plate(pplysel->limb_obj, &(pobj->x), &(xy_control[0]), pplysel->decal, pplysel->ratio);

//	grDepthBufferFunction(GR_CMP_LESS);
//	grDepthMask(FXFALSE);
//	grDepthBiasLevel(0);
}


//-------------------------------------------------------------------------------------------------
//	This process plays warning snds on play select when it is 4th down
//
// 	INPUT:	Nothing
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void warning_snd_proc(int *args)
{
	snd_scall_bank(cursor_bnk_str,HELMET_SND,VOLUME2,127,PRIORITY_LVL5);
}

//-------------------------------------------------------------------------------------------------
//	This process turns on FLIP word on top of proper play upon being flipped
//
// 	INPUT:	x,y for FLIP word
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void display_flip_proc(int *args)
{
	int	i,curpos;
	float x,y;

	x = (float)args[0] + 43.0f;
	y = (float)args[1] - 28.0f;
	curpos = (int)args[2];

	killall(MESSAGE2_PID+curpos,0xFFFFFFFF);

	set_string_id(PLAY_SEL_MSG2_ID+curpos);
	set_text_font(FONTID_BAST18);
	set_text_position(x,y, 1.1F);
	oprintf( "%dj%dcFLIP",(HJUST_CENTER|VJUST_BOTTOM),LT_YELLOW );

	sleep(40);
	delete_multiple_strings(PLAY_SEL_MSG2_ID+curpos, 0xffffffff);
}


#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void cycle_play_arrows(int *args)
{
	int 			i,txnum;
	obj3d2_data_t	*pobj;

	while(1)
	{
		// number of overall textures to change-thru
		for (txnum = PSARROW1_TEX; txnum < PSARROW8_TEX; txnum++)
		{
			for (i = 0; i < PLAYSTUF__PLAYS_PER_PAGE; i++)
			{
				// offense plays
				pobj = playsel_play_obj_ptrs[i][game_info.off_side];
				pobj->decal[1] = psel_decals[txnum]->texture_handle;
				// defensive plays
				pobj = playsel_play_obj_ptrs[i][!game_info.off_side];
				pobj->decal[1] = psel_decals[txnum]->texture_handle;
			}
			sleep(10);
		}
	}
}
#endif


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void update_play_page(int scrn_side, int page, int flip, int curpos)
{
	PLAYSTUF__SLOT *slot_ptr = NULL;
	int		i;
	int		side;
	int		pnum;

	// is player on defense or offense
	if (game_info.game_state == GS_CREATE_PLAY)
	{
		side = scrn_side;
		scrn_side = 2;
	}
	else
	if (scrn_side == game_info.off_side)
		side = OFFENSE;
	else	
		side = DEFENSE;

	PlayStuf__SetCurrentPlayPage( side, page );

	// new page number?
	if (page != old_page[scrn_side])
	{
		//	Flash a "sparkle" on the page number
		qcreate("sparkle", MESSAGE_PID, one_wide_flash_proc,
			page_str_xys[scrn_side][is_low_res][X_VAL]-4,
			page_str_xys[scrn_side][is_low_res][Y_VAL],
			1,
			0);

		if (side == OFFENSE)
		{
			// Also kill any FLIP text messages/processes still alive
			killall( MESSAGE2_PID, 0xfffffff0 );
			delete_multiple_strings( PLAY_SEL_MSG2_ID, 0xfffffff0 );
			// Also kill any FREE text messages/processes still alive
			killall( MESSAGE3_PID, -1 );
			delete_multiple_strings( PLAY_SEL_MSG3_ID, -1 );
		}

		// Hide any overlays from old page
		for (i = 0; i < PLAYSTUF__PLAYS_PER_PAGE; i++)
		{
			if (psi_plays[side][old_page[scrn_side]][i] != NULL)
				hide_sprite( psi_plays[side][old_page[scrn_side]][i] );
		}
	}

	// delete play names from previous page
	delete_multiple_strings( play_name_ids[scrn_side], -1 );
	set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );
	set_text_z_inc( 0 );
	set_text_font( FONTID_BAST8T );
	set_string_id( play_name_ids[scrn_side] );

	// init for create a play
	pnum = 0;
	if (game_info.game_state != GS_CREATE_PLAY)
	{
		if (!four_plr_ver)
			pnum = scrn_side;
		else
		{
			i = scrn_side + scrn_side;
			if ((p_status & (3<<i)) == (3<<i))
				pnum = (side == OFFENSE) ? off_captain() : game_info.captain;
			else
				pnum = ((p_status >> i) & 0x3) - 1;
			pnum += i;
		}
	}

#ifdef DEBUG
	fprintf(stderr, "off_captain pnum: %d\r\n",pnum);
#endif

	// change plays (only the 'limbs')
	for (i=0; i < PLAYSTUF__PLAYS_PER_PAGE; i++)
	{
		slot_ptr = PlayStuf__GetSlot(side, PlayStuf__GetCurrentPlayPage(side), i);

		// Do limbs specify an overlay?
		if (slot_ptr->limb_ptrs[0] != NULL &&
			slot_ptr->limb_ptrs[1] == NULL)
		{
			// Yes; show the overlay on a blank limb
			if (flip)
				psi_plays[side][page][i]->mode |=  FLIP_TEX_H;
			else
				psi_plays[side][page][i]->mode &= ~FLIP_TEX_H;

			generate_sprite_verts( psi_plays[side][page][i] );
			unhide_sprite( psi_plays[side][page][i] );

			playsel_play_obj_ptrs[i][side]->limb_obj = &limb_blank;
		}
		else
			playsel_play_obj_ptrs[i][side]->limb_obj = slot_ptr->limb_ptrs[flip];

		// print play name
		set_text_position(
			playsel_plays_xys[scrn_side][i][X_VAL]+41.0f,
			playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res?88.0f:86.0f),
			1.5f);
		oprintf( "%dc%s", WHITE, slot_ptr->name);

		if (side == OFFENSE)
		{
//			// Not doing create a play?
//			if (scrn_side != 2)
//			{
//				// Doing one of the first three plays?
//				if (i < 3 )
//				{
//					if (flip)
//						psi_teamplays[i]->mode |= FLIP_TEX_H;
//					else
//						psi_teamplays[i]->mode &= ~FLIP_TEX_H;
//
//					if ((page == 0) && (game_info.play_type != PT_EXTRA_POINT || i != 0))
//					{
//						unhide_sprite( psi_teamplays[i] );
//						generate_sprite_verts(psi_teamplays[i]);
//					}
//					else
//						hide_sprite( psi_teamplays[i] );
//
//				}
//			}
			// Showing custom plays?
			if (page == PLAYSTUF__OFF_PAGE_ID_CUSTOM)
			{
				// display CUSTOM PAGE names
				if (custom_plays[pnum][i].flags & CPLT_EXISTS)
				{
					// print name of play from ram
					set_text_position(playsel_plays_xys[scrn_side][i][X_VAL]+41.0f, playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res ? 34.0f:33.0f), 1.5F);
					oprintf( "%dc%s", LT_YELLOW, custom_plays[pnum][i].name1);
					set_text_position(playsel_plays_xys[scrn_side][i][X_VAL]+41.0f, playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res ? 49.0f:48.0f), 1.5F);
					oprintf( "%dc%s", LT_YELLOW, custom_plays[pnum][i].name2);
					if (flip)
					{
						set_text_position(playsel_plays_xys[scrn_side][i][X_VAL]+41.0f, playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res ? 64.0f:63.0f), 1.5F);
						oprintf( "%dc%s", LT_CYAN, "(FLIPPED)");
					}
				}
				else
				if (custom_plays[pnum][i].flags & CPLT_UNUSED)
				{
					// print "available but unused"
					set_text_position(playsel_plays_xys[scrn_side][i][X_VAL]+41.0f, playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res ? 34.0f:33.0f), 1.5F);
					oprintf( "%dc%s", LT_CYAN, "AVAILABLE");
					set_text_position(playsel_plays_xys[scrn_side][i][X_VAL]+41.0f, playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res ? 49.0f:48.0f), 1.5F);
					oprintf( "%dc%s", LT_CYAN, "BUT UNUSED");
				}
				else
				{
					// print "not available"
					set_text_position(playsel_plays_xys[scrn_side][i][X_VAL]+41.0f, playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res ? 34.0f:33.0f), 1.5F);
					oprintf( "%dc%s", WHITE, "NOT");
					set_text_position(playsel_plays_xys[scrn_side][i][X_VAL]+41.0f, playsel_plays_xys[scrn_side][i][Y_VAL]-(is_low_res ? 49.0f:48.0f), 1.5F);
					oprintf( "%dc%s", WHITE, "AVAILABLE");
				}
			}
		}
//		else
//		// Defensive page 2?
//		if (page == 1)
//			// show play overlay
//			unhide_sprite( psi_def2plays[i]);
//		else
//			// hide play overlay
//			hide_sprite( psi_def2plays[i]);
	}

	if (side == OFFENSE)
	{
		if ((page != old_page[scrn_side]) && (game_info.game_state != GS_CREATE_PLAY))
		{
			// If going to page 0, and it is extra point time, display "free" word on first play
			if ((game_info.play_type == PT_EXTRA_POINT) && (page == 0))
				qcreate( "freept", MESSAGE3_PID, display_free_pt_proc,
					(scrn_side?305:46),
					237,
					0,
					0);

			if ((game_info.play_type != PT_EXTRA_POINT) && (page == 0))
			{
				// Display FG message when that play is available!
				if ((game_info.los >= 50 || pup_superfg) &&
					(game_info.down == 4 ||
						(game_info.game_time < 0x00001000 &&
							(game_info.game_quarter == 1 || game_info.game_quarter >= 3)
						)
					))
					qcreate("freept", MESSAGE3_PID, display_FG_proc,
						(scrn_side?303:46),
						70,
						0,
						0);
				else
				// Display punt message when that play is available!
				if ((game_info.los < 50 && !pup_nopunt) && game_info.down == 4)
					qcreate("freept", MESSAGE3_PID, display_punt_proc,
						(scrn_side?303:46),
						70,
						0,
						0);
			}
		}

		slot_ptr = PlayStuf__GetSlot(side, PlayStuf__GetCurrentPlayPage(side), curpos);

		// Don't show flip message if on free extra point box OR PUNT or FG
		if ((game_info.game_state == GS_CREATE_PLAY) ||
				(
				!((game_info.play_type == PT_EXTRA_POINT) &&
					(curpos == 0) &&
					(page == 0)) &&
				(slot_ptr->limb_ptrs[0] != &limb_kicks_punt) &&
				(slot_ptr->limb_ptrs[0] != &limb_kicks_fieldgoal)
				)
			)
		{
			// Go ahead and print a flip message on current play
			if (!hidden_off_cursor && (flip != old_flip_plays))
				qcreate("flip", MESSAGE2_PID+curpos, display_flip_proc,
					playsel_plays_xys[scrn_side][curpos][X_VAL],
					playsel_plays_xys[scrn_side][curpos][Y_VAL],
					curpos,
					0);
		}	
	}

	// print page number
	delete_multiple_strings( page_str_ids[scrn_side], -1 );
	set_string_id( page_str_ids[scrn_side] );
	set_text_font( FONTID_BAST10 );

	set_text_position(page_str_xys[scrn_side][is_low_res][X_VAL], page_str_xys[scrn_side][is_low_res][Y_VAL], 1.8F);
	oprintf( "%dj%dcPAGE %d", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW,
		page + (
			(side == DEFENSE) ||
			(page != PLAYSTUF__OFF_PAGE_ID_CUSTOM) ||
			(page == PLAYSTUF__OFF_PAGE_ID_CUSTOM && (pup_playbook == 0 || pup_secret_page != 0))
		)
	);

	old_page[scrn_side] = page;
	old_flip_plays = flip;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define CP_PUNT		0
#define CP_FG		1
#define CP_SHORT	2
#define CP_LONG		3
#define CP_FREE		4

#define CP_GOALLINE	5
#define CP_BLITZ	6

void cpu_pick_play(int pnum)
{
	int		down_by, up_by, fg_range, ytg, one_play;
	int		pick, periods_left;
	
	fg_range = (game_info.los >= 50);
	ytg = game_info.first_down - game_info.los;
	down_by = compute_score(!pnum) - compute_score(pnum);
	up_by = -down_by;
	one_play = (game_info.game_time < 0x0800);
	periods_left = 3 - game_info.game_quarter;
	if( periods_left < 0 )
		periods_left = 0;
	if( game_info.game_time >= 0x10000 )
		periods_left += 1;
	
	if (game_info.off_side == pnum)
	{
		// go for two.  we wouldn't even be here if we wanted the freebie
		if( game_info.play_type == PT_EXTRA_POINT )
			pick = CP_SHORT;
		else
		if( game_info.down == 4 )
		{
			// fourth down
			switch( game_info.game_quarter )
			{
				case 0:
				case 2:
					if(( down_by > 3 ) && ( ytg < 5 ))
						// down a little and it's an easy one.  go for it
						pick = CP_SHORT;
					else
					if(( down_by > periods_left * 7) && (game_info.los > 40))
						// desperate.  go for it
						pick = CP_LONG;
					else
					if( fg_range || pup_superfg )
						// take the field goal
						pick = CP_FG;
					else
						// not desperate, out of range.
						pick = CP_PUNT;
					
					break;

				case 1:
					if( fg_range || pup_superfg )
						// in fg range.  do it
						pick = CP_FG;
					else
						// too far. punt
						pick = CP_PUNT;
					break;
					
				default:
					if( fg_range )
					{
						if( down_by <= 3 )
							// fg ties or puts us on top.  do it
							pick = CP_FG;
						else
						{
							if( one_play || ( ytg > 10 ))
								pick = CP_LONG;
							else
								pick = CP_SHORT;
						}
					}
					else
					{
						if( up_by >= 0 && !pup_superfg )
							// we're winning.  punt
							pick = CP_PUNT;
						else
							// we're losing.  go for it
							if( one_play || ( ytg > 10 ))
								pick = CP_LONG;
							else
								pick = CP_SHORT;
					}
					break;
			}
		}
		else
		if((( game_info.game_quarter >= 3) || ( game_info.game_quarter == 1 )) && (one_play))
		{
			// time for one more play in half/game
			if(( fg_range ) && ( down_by <= 3 ))
				pick = CP_FG;
			else
			if( game_info.los < 93 )
				pick = CP_LONG;
			else
				pick = CP_SHORT;
		}
		else
		{
			// time for more plays this drive.
			switch( game_info.down )
			{
				case 1:
				case 2:
				case 3:
					pick = CP_SHORT;
					break;
				default:
#ifdef DEBUG
					fprintf( stderr, "problems in cpu play_select(1)\n" );
					lockup();
#endif
					pick = CP_LONG;
					break;
			}
		}

		switch( pick )
		{
			case CP_PUNT:
				// If we need 25 or fewer yards for first down, fake 10% of the time
				if ((ytg <= 25) && (!randrng(10)))
					game_info.team_play[pnum] = &play_fake_punt;
				else
					game_info.team_play[pnum] = &play_punt;
				break;
			case CP_FG:
				game_info.team_play[pnum] = &play_field_goal;
				break;
			case CP_SHORT:
				game_info.team_play[pnum] = short_off_plays[randrng((sizeof(short_off_plays)/sizeof(play_t *)))];
				if ((game_info.down == 4) && (randrng(2)))
					game_info.team_play[pnum] = &play_fake_punt;
				break;
			case CP_LONG:
				game_info.team_play[pnum] = long_off_plays[randrng((sizeof(long_off_plays)/sizeof(play_t *)))];
				break;
			case CP_FREE:
				game_info.team_play[pnum] = &play_extra_point;
				show_freept_idiot_box = TRUE;
				exit_status = 0;
				break;
		}
	}
	else
	{
		if( game_info.team_play[!pnum] == &play_punt )
			pick = CP_PUNT;
		else
		if( game_info.team_play[!pnum] == &play_field_goal )
			pick = CP_FG;
		else
		if( game_info.los >= 90 )
		{
			if( randrng(100) < 80 )
				pick = randrng(2) ? CP_GOALLINE : CP_SHORT;
			else
				pick = CP_BLITZ;
		}
		else
		if( game_info.los >= 80 )
		{
			if( randrng(100) < 75 )
				pick = CP_SHORT;
			else
				pick = CP_BLITZ;
		}
		else
		if( ytg >= (10*(5-game_info.down)))
			pick = CP_LONG;
		else
			pick = CP_SHORT;
		
		switch( pick )
		{
			case CP_PUNT:
				game_info.team_play[pnum] = &play_punt_return;
				break;
			case CP_FG:
				game_info.team_play[pnum] = &play_block_fg;
				break;
			case CP_SHORT:
				if( game_info.team_play[!pnum] == &custom_play )
					game_info.team_play[pnum] = short_def_plays2[randrng((sizeof(short_def_plays2)/sizeof(play_t *)))];
				else
					game_info.team_play[pnum] = short_def_plays[randrng((sizeof(short_def_plays)/sizeof(play_t *)))];
				break;
			case CP_LONG:
				if( game_info.team_play[!pnum] == &custom_play )
					game_info.team_play[pnum] = long_def_plays2[randrng((sizeof(long_def_plays2)/sizeof(play_t *)))];
				else
					game_info.team_play[pnum] = long_def_plays[randrng((sizeof(long_def_plays)/sizeof(play_t *)))];
				break;
			case CP_GOALLINE:
				game_info.team_play[pnum] = &play_goal_line;
				break;
			case CP_BLITZ:
				game_info.team_play[pnum] = blitz_def_plays[randrng((sizeof(blitz_def_plays)/sizeof(play_t *)))];
				break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static int go_for_two( int team )
{
	// probs[] is chance in ten of going for a two-point conversion
	// down by:     8  7  6  5  4  3  2  1  0
	// up by:								0  1  2  3  4  5  6  7  8
	int	probs[] = { 0, 2, 2,10, 0, 0,10, 0, 0,10, 0, 0, 8,10, 0, 0, 9 };
	int	down_by, prob;
	
	down_by = compute_score(!team) - compute_score(team);

#ifdef DEBUG
	fprintf( stderr, "go_for_two, down by %d, p(go) = %d\n",
		down_by, probs[GREATER(LESSER(8-down_by,16),0)] );
#endif
	
	if( down_by <= -9 )
		return FALSE;
		
	if( down_by >= 9 )
		return TRUE;
		
	prob = probs[8-down_by];
	
	if( randrng(10) < prob )
		return TRUE;
	else
		return FALSE;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void audit_play(play_t *play)
{
	PLAYSTUF__SLOT * slot;
	unsigned i;

	if( play->flags & PLF_CUSTOM )
		return;

	slot = off_slots;
	while( slot->play_ptr )
	{
		if(( slot->aud_num != 0 ) && ( play == slot->play_ptr ))
		{
			do_off_play_percent_aud( slot->aud_num );
			return;
		}
		slot++;
	}

	slot = def_slots;
	while( slot->play_ptr )
	{
		if(( slot->aud_num != 0 ) && ( play == slot->play_ptr ))
		{
			do_def_play_percent_aud( slot->aud_num );
			return;
		}
		slot++;
	}

	slot = &team_slots[0][0];
	while( slot->play_ptr )
	{
		if(( slot->aud_num != 0 ) && ( play == slot->play_ptr ))
		{
			do_off_play_percent_aud( slot->aud_num );
			return;
		}
		slot++;
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void PlaySel__ChoosePlay(int side, int page, int p_num, int cur_pos, int tm_num, int force_choose)
{
	play_t * play_ptr = PlaySel__MakeChoice( side, page, p_num, cur_pos, tm_num, force_choose );

	if( play_ptr != NULL )
	{
		game_info.team_play[tm_num] = play_ptr;

		snd_scall_bank( cursor_bnk_str, cursor_snds[tm_num], VOLUME4, 127, PRIORITY_LVL2 );

		// clear players bit (he picked)
		exit_status &= ~(1<<p_num);

		// If free extra point was picked, exit right away so we can display idiot box
		if( game_info.team_play[tm_num] == &play_extra_point )
		{
			exit_status = 0;
			show_freept_idiot_box = 1;
		}
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
play_t * PlaySel__MakeChoice(int side, int page, int p_num, int cur_pos, int tm_num, int force_choose)
{
	PLAYSTUF__SLOT * slot_ptr;
	play_t * play_ptr = NULL;

	if( (side == OFFENSE) && (page == PLAYSTUF__OFF_PAGE_ID_CUSTOM) )
	{
		//	On custom page...
		if( custom_plays[p_num][cur_pos].flags & CPLT_EXISTS )
		{	
			//	Current custom play choice exists
			//	Decompress (and choose) the custom play
			convert_play( &(custom_plays[p_num][cur_pos]) );
			play_ptr = &custom_play;
		}
		else
		{
			//	Current custom play choice does not exist
			if( force_choose )
			{
				//	Cursor is highlighting a custom play that does not exist,
				//	but	caller wants to force a play to be chosen (i.e. the time ran out before a play was chosen),
				//	So either find the first existing custom play on the custom page, 
				//	or choose a random play from the first play page.
				for( cur_pos = 0; cur_pos < 9; cur_pos++ )
				{
					//	find the first custom play that exists and do that one
					if( custom_plays[p_num][cur_pos].flags & CPLT_EXISTS )
					{
						//	Decompress (and choose) the custom play
						convert_play( &(custom_plays[p_num][cur_pos]) );
						play_ptr = &custom_play;
						break;
					}
				}
				if( play_ptr == NULL )
				{
					//	No custom plays found on the page... choose a random play from first main page
					slot_ptr = PlayStuf__GetSlot( side, 0, randrng(9) );
					play_ptr = slot_ptr->play_ptr;
				}
			}
		}	
	}
	else
	{
		//	Choose the currently selected play
		slot_ptr = PlayStuf__GetSlot( side, page, cur_pos );
		play_ptr = slot_ptr->play_ptr;
	}
	return play_ptr;
}

//-------------------------------------------------------------------------------------------------
