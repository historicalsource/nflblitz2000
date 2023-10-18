#if defined(VEGAS)

#define USE_DISK_CMOS
void update_other_cmos_to_disk(int sleep_allowed);

#endif

/******************************************************************************/
/*                                                                            */
/* vsscreen.c  - Versus screen code                                           */
/*                                                                            */
/* Written by:  DJT & JAPPLE.                                                 */
/* $Revision: 163 $                                                            */
/*                                                                            */
/* Copyright (c) 1996,1997 by Williams Electronics Games Inc.                 */
/* All Rights Reserved                                                        */
/*                                                                            */
/******************************************************************************/
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <string.h>

#include "/video/nfl/include/audits.h"
#include "/video/nfl/include/fontid.h"
#include "/video/nfl/include/game.h"
#include "/video/nfl/include/id.h"
#include "/video/nfl/include/player.h"
#include "/video/nfl/include/select.h"
#include "/video/nfl/include/sndcalls.h"
#include "/Video/Nfl/Include/Coin.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/Pinfo.h"
#include "/video/nfl/include/game_cfg.h"

#include "/video/nfl/include/flash.h"

#define IMAGEDEFINE 1
#include "/video/nfl/include/vsscreen.h"


//#define JASON

#ifdef JASON
#define PUP_DEBUG
#endif

//#if (MAX_PLYRS == 4)
//#undef  MAX_PLYRS
//#define MAX_PLYRS 2
//#endif


/*
** referenced data & function prototypes
*/
//extern void init_bog_monitor(void);

extern int	is_low_res;

extern short screen;
extern volatile int imgs_loaded;

extern float hres, vres;

extern char * announcer_strs[];
extern char * qtr_tunes_tbl[];
extern char * game_tunes_tbl[];
extern char * taunt_bnk_strs[];

char * get_ancr_str(char *);
char * get_taunt_str(char *);

void versus_speech_proc(void);

static void stick_stadium();

//
// global data & function prototypes
//
// NOT in yet!!!
int	pup_violence = 0;					// Set for violent moves

// Stick combo when exitting team select
int pup_field = 0;						// 0:newgrass,1:oldgrass,2:turf,3:dirt,4:snow
int	pup_stadium = 0;					// 0:day,1:night,2:snow,3:NO stadium
int	pup_showaudibles = 0;				// Set to show audible names
int	pup_playbook = 0;					// 0:extra plays,1:default,2:no custom

// RIGHT stick
int pup_pullback = 0;					//R1 Set for camera to pull back further
int pup_bighead = 0;					//R2 Set for player to have a big head
int pup_bigheadteam = 0;				//R3 Set for team to have big heads
int pup_bigplyrs = 0;					//R4 Set for team to have big players
int pup_babyplyrs = 0;					//R5 Set for team to have little players
int pup_bigball = 0;					//R6 Set for oversize football
int pup_rcvrname = 0;					//R7 Set to show receiver name on catch
//int pup_rain = 0;						//R8 Set to have rain
//int pup_altstadium = 0;					//R9 Set for alternate stadium
int pup_always_receiver = 0;			//R10 Player is never captain on offense

// DOWN stick (all rev 1.0 is here)
int pup_fog = 0;						//D1 Set for fog game
int pup_noassist = 0;					//D2 Set for no computer assistance
int pup_nofumbles = 0;					//D3 Set to eliminate fumbles (except onside kicks and spin abuse)
int pup_superdrones = 0;				//D4 Set drone team to be super-unbeatable
int pup_showfgper = 0;					//D5 Set to show fg percentage
int pup_tourney = 0;					//D6 Set for tournament mode
int pup_thickfog = 0;					//D7 Set for thick fog
//int pup_groundfog = 0;					//D8 Set for ground fog
int pup_secret_page = 0;				//D9 Set for secret page of offensive plays (old plays from blitz)
int pup_notarget = 0;					//D10 Set for no hiliting target receiver

// UP stick
int pup_offense = 0;					//U1 Set for powerup offense
int pup_defense = 0;					//U2 Set for powerup defense
int pup_drones = 0;						//U3 Set for powerup drones
int pup_blitzers = 0;					//U4 Set for super blitzers
int pup_infturbo = 0;					//U5 Set for infinite turbo
int pup_nopunt = 0;						//U6 Set for no punts
int pup_nointercept = 0;				//U7 Set for no interceptions
int pup_no1stdown = 0;					//U8 Set for no 1st downs
int pup_hugehead = 0;					//U9 Set for player to have a huge head
int pup_noalways = 0;					//U10 Set to cancel any all-time qb/wr on team
#ifdef Q_GAME
int pup_qgame = 0;						//U11 Set for quick game
#endif

// LEFT stick
int pup_nostadium = 0;					//L1 Set for no stadium
int pup_noplaysel = 0;					//L2 Set for no play select screen
int pup_fastpass = 0;					//L3 Set to always have fast passes
int pup_fastturborun = 0;				//L4 Set to have turbo run go faster
int pup_speed = 0;						//L5 Set for powerup speed (players run faster)
int pup_block = 0;						//L6 Set for powerup blocking
//int pup_noshowqb = 0;					//L7 Set to not show QB in MODE_QB
int pup_runob = 0;						//L8 Set to have player run as well as jump OB
int pup_superfg = 0;					//L9 Set for no range limit field goals
int pup_always_qb = 0;					//L10 Player is always captain on offense
int pup_perfect_fg = 0;					//L11 Set for perfect field goals every time


static struct tagPUPCODE pupcodes[] = {
//	{ 0x444, &pup_violence,   "VIOLENCE ON" },

	{ 0x021, JOY_RIGHT, PC_AGREE,					&pup_pullback,       "SHOW MORE FIELD",    "(IF TEAMS AGREE)" },
	{ 0x200, JOY_RIGHT, 0,							&pup_bighead,        "BIG HEAD",           NULL },
	{ 0x203, JOY_RIGHT, 0,							&pup_bigheadteam,    "TEAM BIG HEADS",     NULL },
	{ 0x141, JOY_RIGHT, PC_TRNY,					&pup_bigplyrs,       "TEAM BIG PLAYERS",   NULL },
	{ 0x310, JOY_RIGHT, PC_TRNY,					&pup_babyplyrs,      "TEAM TINY",          "PLAYERS" },
	{ 0x050, JOY_RIGHT, PC_TRNY,					&pup_bigball,        "BIG FOOTBALL",       NULL },
	{ 0x102, JOY_RIGHT, 0,							&pup_rcvrname,       "HIDE RECEIVER",      "NAME" },
//	{ 0x555, JOY_RIGHT, PC_TRNY,					&pup_rain,           "WEATHER: RAIN",      NULL },
//	{ 0x534, JOY_RIGHT, 0,							&pup_altstadium,     "SKY STADIUM",        NULL },
	{ 0x222, JOY_RIGHT,	PC_TCANCEL|PC_4PONLY,		&pup_always_receiver,"ALWAYS RECEIVER",    "(2 HUMANS TEAM)" },

	{ 0x030, JOY_DOWN,  0,							&pup_fog,            "FOG ON",             NULL },
	{ 0x012, JOY_DOWN,  PC_2P|PC_AGREE,				&pup_noassist,       "NO CPU ASSIST",      "(IF TEAMS AGREE)" },
	{ 0x423, JOY_DOWN,  PC_TRNY|PC_AGREE,			&pup_nofumbles,      "NO RANDOM FUMBLE",   "(IF TEAMS AGREE)" },
	{ 0x314, JOY_DOWN,  PC_TRNY|PC_1P,				&pup_superdrones,    "SMART CPU TEAM",     "(IN 1 TEAM GAME)" },
	{ 0x001, JOY_DOWN,  0,							&pup_showfgper,      "SHOW FIELD GOAL",    "PERCENTAGE" },
	{ 0x111, JOY_DOWN,  PC_2P,						&pup_tourney,        "TOURNAMENT MODE",    "(IN 2 TEAM GAME)" },
	{ 0x041, JOY_DOWN,  PC_TRNY,					&pup_thickfog,       "THICK FOG ON",       NULL },
	{ 0x321, JOY_DOWN,	PC_TRNY,					&pup_notarget,       "NO HILIGHTING",      "TARGET RECEIVER" },
//	{ 0x232, JOY_DOWN,  PC_TRNY,					&pup_groundfog,      "GROUND FOG ON",      NULL },
	{ 0x333, JOY_DOWN,  0,							&pup_secret_page,    "EXTRA PLAYS",        "FOR OFFENSE" },

	{ 0x312, JOY_UP,    PC_TRNY,					&pup_offense,        "POWERUP OFFENSE",    NULL },
	{ 0x421, JOY_UP,    PC_TRNY,					&pup_defense,        "POWERUP DEFENSE",    NULL },
	{ 0x233, JOY_UP,    PC_TRNY,					&pup_drones,         "POWERUP DRONE",      "TEAMMATES" },
	{ 0x045, JOY_UP,    PC_TRNY,					&pup_blitzers,       "SUPER BLITZING",     NULL },
	{ 0x514, JOY_UP,    PC_TRNY,					&pup_infturbo,       "INFINITE TURBO",     NULL },
	{ 0x151, JOY_UP,    PC_TRNY,					&pup_nopunt,         "NO PUNTING",         NULL },
	{ 0x344, JOY_UP,    PC_TRNY,					&pup_nointercept,    "NO",                 "INTERCEPTIONS" },
	{ 0x210, JOY_UP,    PC_TRNY,					&pup_no1stdown,      "NO FIRST DOWNS",     NULL },
	{ 0x040, JOY_UP,    PC_TRNY,					&pup_hugehead,       "HUGE HEAD",          NULL },
	{ 0x333, JOY_UP,    PC_4PONLY,					&pup_noalways,       "CANCEL ALWAYS",      "QB/RECEIVER" },
#ifdef Q_GAME
	{ 0x111, JOY_UP,    0,							&pup_qgame,          "QUICK GAME",         "(ONLY FOR TEST)" },
#endif

//	{ 0x500, JOY_LEFT,  PC_TRNY,					&pup_nostadium,      "TURN OFF STADIUM",   NULL },
	{ 0x115, JOY_LEFT,  PC_TRNY,					&pup_noplaysel,      "NO PLAY SELECT",     "(IF TEAMS AGREE)" },
	{ 0x250, JOY_LEFT,  PC_TRNY,					&pup_fastpass,       "FAST PASSES",        NULL },
	{ 0x002, JOY_LEFT,  0,							&pup_showaudibles,   "SHOW AUDIBLE",       "NAMES" },
	{ 0x032, JOY_LEFT,  PC_TRNY,					&pup_fastturborun,   "FAST TURBO",         "RUNNING" },
	{ 0x404, JOY_LEFT,  PC_TRNY|PC_AGREE,			&pup_speed,          "POWERUP SPEED",      "(IF TEAMS AGREE)" },
	{ 0x312, JOY_LEFT,  PC_TRNY,					&pup_block,          "POWERUP LINEMEN",    NULL },
//	{ 0x342, JOY_LEFT,  PC_TRNY,					&pup_noshowqb,       "INVISIBLE QB",       NULL },
	{ 0x211, JOY_LEFT,  PC_TRNY,					&pup_runob,          "ALLOW STEPPING",     "OUT OF BOUNDS" },
	{ 0x123, JOY_LEFT,  PC_TRNY,					&pup_superfg,        "SUPER FIELD",        "GOALS" },
	{ 0x222, JOY_LEFT,	PC_TCANCEL|PC_4PONLY,		&pup_always_qb,      "ALWAYS QB",          "(2 HUMANS TEAM)" },
	{ 0x333, JOY_LEFT,	PC_TRNY|PC_AGREE,			&pup_perfect_fg,     "100% FIELD GOALS",   "(IF TEAMS AGREE)" },
};


#define	Z_HELMET		100
#define Z_PLATES		1
//#define	NUM_PIECES		19

// Use this globally to track transition state
volatile int plates_on = PLATES_OFF;


// Can use this stuff globally for customization
char plate_name[20];

sprite_info_t *plate_obj[] = {NULL, NULL, NULL, NULL};

image_info_t plate_imgs[] = {
	{plate_name, 256.0F, 192.0F, 0.0F,      0.0F, 0.0F, 1.0F, 0.0F, 0.75F},	// BL
	{plate_name, 256.0F, 192.0F, 0.0F,   -192.0F, 0.0F, 1.0F, 0.0F, 0.75F},	// TL
	{plate_name, 256.0F, 192.0F, 256.0F,    0.0F, 0.0F, 1.0F, 0.0F, 0.75F},	// BR
	{plate_name, 256.0F, 192.0F, 256.0F, -192.0F, 0.0F, 1.0F, 0.0F, 0.75F}	// TR
};

//
// local data & function prototypes
//
#define Z_PUP_OVAL		90
#define Z_PUP_ICON		80
#define Z_PUP_TEXT		70
#define Z_SPARKLE		60


#define SPARKLE_IMGCNT	(sizeof(sparkle_imgs)/sizeof(void *))


#if 1
extern image_info_t *sparkle_imgs[10];
#else
static image_info_t *sparkle_imgs[] =
{
	&sparkc01,&sparkc02,&sparkc03,&sparkc04,&sparkc05,
	&sparkc06,&sparkc07,&sparkc08,&sparkc09				//,&sparkc10
};
#endif

//static image_info_t * pii_helmet_l[] = {
//	&vs_ari_l,&vs_atl_l,&vs_bal_l,&vs_buf_l,&vs_car_l,&vs_chi_l,
//	&vs_cin_l,&vs_dal_l,&vs_den_l,&vs_det_l,&vs_gre_l,&vs_ind_l,
//	&vs_jac_l,&vs_kan_l,&vs_mia_l,&vs_min_l,&vs_ne_l, &vs_no_l,
//	&vs_nyg_l,&vs_nyj_l,&vs_oak_l,&vs_phi_l,&vs_pit_l,&vs_sd_l,
//	&vs_sf_l, &vs_sea_l,&vs_stl_l,&vs_tam_l,&vs_ten_l,&vs_was_l
//};
//
//
//static image_info_t * pii_helmet_r[] = {
//	&vs_ari_r,&vs_atl_r,&vs_bal_r,&vs_buf_r,&vs_car_r,&vs_chi_r,
//	&vs_cin_r,&vs_dal_r,&vs_den_r,&vs_det_r,&vs_gre_r,&vs_ind_r,
//	&vs_jac_r,&vs_kan_r,&vs_mia_r,&vs_min_r,&vs_ne_r, &vs_no_r,
//	&vs_nyg_r,&vs_nyj_r,&vs_oak_r,&vs_phi_r,&vs_pit_r,&vs_sd_r,
//	&vs_sf_r, &vs_sea_r,&vs_stl_r,&vs_tam_r,&vs_ten_r,&vs_was_r
//};


static image_info_t * pii_pup_icon[] = {
//	&vs_pa,&vs_mdway,&vs_afc,&vs_nfc,&vs_nfl,&vs_play
//	&vs_nfl,&vs_mdway,&vs_afc,&vs_nfc,&vs_pa,&vs_play
	&vs_nfl,&vs_midway,&vs_pa,&vs_ball,&vs_yard,&vs_trophy
};


//static float x_pup_icon[][2][PUP_ICON_CNT] = {			// [plyr][2plr/4plr][x]
//	{{  0.0f,  0.0f,  0.0f},{111.0f,173.0f, 49.0f}},	// p1 {kit,ded}
//	{{  0.0f,  0.0f,  0.0f},{404.0f,466.0f,342.0f}}		// p2 {kit,ded}
//#if (MAX_PLYRS == 4)
//	,{{  0.0f,  0.0f,  0.0f},{  0.0f,  0.0f,  0.0f}}	// p3 {kit,ded}
//	,{{  0.0f,  0.0f,  0.0f},{  0.0f,  0.0f,  0.0f}} 	// p4 {kit,ded}
//#endif
//};

// X positions for icons
#define D	38

// Plaq center X's
//  plaqs are 118 pix wide

#define	PX1	 71
#define	PX2	195
#define	PX3	319
#define	PX4	443

static float x_pup_icon[2][MAX_PLYRS][PUP_ICON_CNT] = {		// [2/4 plyr][plyr][x]
	{{ 111, 111+D, 111-D }, 								// p1 (2 player version)
	 { 404, 404+D, 404-D }, 								// p2
	 {   0,     0,     0 }, 								// p3
	 {   0,     0,     0 }},								// p4

	{{ PX1, PX1+D, PX1-D },		 							// p1 (4 player version)
	 { PX2, PX2+D, PX2-D },									// p2
	 { PX3, PX3+D, PX3-D },									// p3
	 { PX4, PX4+D, PX4-D }}									// p4
};


//static float y_pup_icon[] = { 32, 32 };				// Y [2/4 plr version]
#define Y_PUP_ICON	32

static float y_pup_text[][2][4] = {					// [res][fourplyr][plyr#]
	{{ 90,  90,  0,   0 }, { 83, 108, 83, 108 }},
	{{ 62,  62,  0,   0 }, { 58,  67, 58,  67 }},
};

static float z_pup_icon[] = { Z_PUP_ICON + 1.0f, Z_PUP_ICON + 0.0f, Z_PUP_ICON + 2.0f };
static float z_pup_oval[] = { Z_PUP_OVAL + 1.0f, Z_PUP_OVAL + 0.0f, Z_PUP_OVAL + 2.0f };


//static char *tm_helmet_imgs[NUM_TEAMS][2] = {
//						{"arhlml.wms","arhlmr.wms"},
//						{"athlml.wms","athlmr.wms"},
//						{"bahlml.wms","bahlmr.wms"},
//						{"buhlml.wms","buhlmr.wms"},
//						{"cahlml.wms","cahlmr.wms"},
//						{"chhlml.wms","chhlmr.wms"},
//						{"cihlml.wms","cihlmr.wms"},
//						{"dahlml.wms","dahlmr.wms"},
//						{"dehlml.wms","dehlmr.wms"},
//						{"dlhlml.wms","dlhlmr.wms"},
//						{"grhlml.wms","grhlmr.wms"},
//						{"inhlml.wms","inhlmr.wms"},
//						{"jahlml.wms","jahlmr.wms"},
//						{"kahlml.wms","kahlmr.wms"},
//						{"mihlml.wms","mihlmr.wms"},
//						{"mnhlml.wms","mnhlmr.wms"},
//						{"nehlml.wms","nehlmr.wms"},
//						{"nohlml.wms","nohlmr.wms"},
//						{"nghlml.wms","nghlmr.wms"},
//						{"njhlml.wms","njhlmr.wms"},
//						{"oahlml.wms","oahlmr.wms"},
//						{"phhlml.wms","phhlmr.wms"},
//						{"pihlml.wms","pihlmr.wms"},
//						{"sdhlml.wms","sdhlmr.wms"},
//						{"sfhlml.wms","sfhlmr.wms"},
//						{"sehlml.wms","sehlmr.wms"},
//						{"sthlml.wms","sthlmr.wms"},
//						{"tahlml.wms","tahlmr.wms"},
//						{"tehlml.wms","tehlmr.wms"},
//						{"wahlml.wms","wahlmr.wms"} };

static image_info_t	*tm_helmet_imgs[NUM_TEAMS][2] = {
	{&aricarlf,&aricarrt},
	{&atlfallf,&atlfalrt},
	{&balravlf,&balravrt},
	{&bufbillf,&bufbilrt},
	{&carpanlf,&carpanrt},
	{&chibealf,&chibeart},
	{&cinbenlf,&cinbenrt},
	{&clebrwlf,&clebrwrt},
	{&dalcowlf,&dalcowrt},
	{&denbrolf,&denbrort},
	{&detliolf,&detliort},
	{&grepaclf,&grepacrt},
	{&indcollf,&indcolrt},
	{&jacjaglf,&jacjagrt},
	{&kanchilf,&kanchirt},
	{&miadollf,&miadolrt},
	{&minviklf,&minvikrt},
	{&ne_patlf,&ne_patrt},
	{&no_sailf,&no_sairt},
	{&ny_gialf,&ny_giart},
	{&ny_jetlf,&ny_jetrt},
	{&oakrailf,&oakrairt},
	{&phieaglf,&phieagrt},
	{&pitstelf,&pitstert},
	{&sd_chalf,&sd_chart},
	{&sf_49rlf,&sf_49rrt},
	{&seasealf,&seaseart},
	{&stlramlf,&stlramrt},
	{&tambuclf,&tambucrt},
	{&tentitlf,&tentitrt},
	{&wasredlf,&wasredrt},
};

static char *field_selected_msg[] = {
	"GRASS FIELD",
	"BLITZ FIELD",
	"ASTRO TURF FIELD",
	"DIRT FIELD",
	"SNOW FIELD",
	"ASPHALT FIELD",
	"MUD FIELD",
	NULL };

static char *stadium_selected_msg[] = {
	"DAY STADIUM",
	"NIGHT STADIUM",
	"SNOW STADIUM",
	"NO STADIUM",
	NULL };


//-------------------------------------------------------------------------------------------------
#define CHK_SLISTS(s)

extern sprite_node_t * alpha_sprite_node_list;
extern sprite_node_t * sprite_node_list;
extern sprite_node_t * free_sprite_node_list;


//-------------------------------------------------------------------------------------------------
#ifdef DEBUG
#if 0

#undef CHK_SLISTS
#define CHK_SLISTS(s)	chk_slists(s)

void chk_slists( char * sz_op )
{
	sprite_node_t * slist;
	int alpha_cnt = 0;
	int non_cnt = 0;
	int free_cnt = 0;

	slist = alpha_sprite_node_list;
	while (slist)
	{
		slist = slist->next;
		alpha_cnt++;
	}
	slist = sprite_node_list;
	while (slist)
	{
		slist = slist->next;
		non_cnt++;
	}
	slist = free_sprite_node_list;
	while (slist)
	{
		slist = slist->next;
		free_cnt++;
	}
	fprintf(stderr,
		"OPERATION: %48s  alpha: %8u  non: %8u  free: %8u\r\n",
		sz_op,
		alpha_cnt, non_cnt, free_cnt
	);
//	fflush(stdout);
}
#endif
#endif


//-------------------------------------------------------------------------------------------------
//  Just clear all the power-up flags NOT allowed in Tournament Mode
//-------------------------------------------------------------------------------------------------
void clear_tourney_pups(void)
{
	int	i;

	for( i = 0; i < (int)PUP_CODE_CNT; i++ )
	{
		if( pupcodes[i].control & PC_TRNY )
		{
#ifdef PUP_DEBUG
			if( pupcodes[i].flag[0] )
				fprintf( stderr, "PUP: %s cleared\n", pupcodes[i].text1 );
#endif
			pupcodes[i].flag[0] = 0;
		}
	}
}

//-------------------------------------------------------------------------------------------------
//  Just clear all the power-up flags... called from game over and vs scrn
//-------------------------------------------------------------------------------------------------
void clear_pups(void)
{
	int	i;

	for( i = 0; i < (int)PUP_CODE_CNT; i++ )
		pupcodes[i].flag[0] = 0;
	clear_user_config();
}

//-------------------------------------------------------------------------------------------------
// 	create_plates() - 
//-------------------------------------------------------------------------------------------------
void create_plates(void)
{
	CHK_SLISTS( "create_plates() entry" );
	// Create the plate objects
	plate_obj[0] = beginobj(&plate_imgs[0], 0.0f, 0.0f, Z_PLATES, 0);
	plate_obj[0]->id = 2;
	plate_obj[0]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	plate_obj[1] = beginobj(&plate_imgs[1], 0.0f, SPRITE_VRES, Z_PLATES, 0);
	plate_obj[1]->id = 2;
	plate_obj[1]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	plate_obj[2] = beginobj(&plate_imgs[2], SPRITE_HRES, 0.0f, Z_PLATES, 0);
	plate_obj[2]->id = 2;
	plate_obj[2]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	plate_obj[3] = beginobj(&plate_imgs[3], SPRITE_HRES, SPRITE_VRES, Z_PLATES, 0);
	plate_obj[3]->id = 2;
	plate_obj[3]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	CHK_SLISTS( "create_plates() exit" );
}

//-------------------------------------------------------------------------------------------------
// kill_plates() - 
//-------------------------------------------------------------------------------------------------
void kill_plates(void)
{
	int i = 3;

	CHK_SLISTS( "kill_plates() entry" );
	do
	{
		if (plate_obj[i])
		{
			delobj(plate_obj[i]);
			plate_obj[i] = NULL;
		}
	}
	while (i--);

	// Set plates flag
	plates_on = PLATES_OFF;
	CHK_SLISTS( "kill_plates() exit" );
}

//-------------------------------------------------------------------------------------------------
// transit_proc(int *args) - 
//
//  args[0] = transition tick count
//  args[1] = 0 or ptr to a float for custom velocity
//-------------------------------------------------------------------------------------------------
void transit_proc(int *args)
{
	float ang_vel;
	int i, tcount;
	int lft_rgt_doors;

	if (randrng(100) < 50)
		lft_rgt_doors = 1;
	else
		lft_rgt_doors = 0;
		
	// Set the angular velocity

//	if (args[1])
//	{
//		ang_vel = *(float *)args[1];
//	}
//	else
//	{
//		ang_vel = ((90.0f/180.0f) * 3.14159f) / (float)args[0];
//	}
	ang_vel  = lft_rgt_doors ? SPRITE_HRES/2.0f:(is_low_res ? SPRITE_VRES/3.0f:SPRITE_VRES/2.0f);
	ang_vel /= (float)args[0];

	// Do plate objs exist? Make'em if not
	// Something to do if at least one of'em doesn't exist
	if (!plate_obj[0] || !plate_obj[1] || !plate_obj[2] || !plate_obj[3])
	{
		// All of'em had better be NULL!!
		if (!plate_obj[0] && !plate_obj[1] && !plate_obj[2] && !plate_obj[3])
		{
			// Generate a random texture name for the transition plates
			sprintf(plate_name, "trans%d.wms", randrng(3) + 1);

			// Create the plate objects
			create_plates();

			// Make them be off screen
//			plate_obj[0]->z_angle = plate_obj[3]->z_angle = (90.0f/180.0f) * 3.14159f;
//			plate_obj[1]->z_angle = plate_obj[2]->z_angle = (-90.0f/180.0f) * 3.14159f;

			if (lft_rgt_doors)
			{
				plate_obj[0]->x = plate_obj[1]->x = -256.0f;
				plate_obj[2]->x = plate_obj[3]->x = 512.0f+256.0f;
			}
			else
			{
				plate_obj[1]->y = plate_obj[3]->y = SPRITE_VRES + (is_low_res ? 128.0f : 192.0f);
				plate_obj[0]->y = plate_obj[2]->y = 0.0f - (is_low_res ? 128.0f : 192.0f);
			}


			i = 3;
			do generate_sprite_verts(plate_obj[i]); while (i--);

			// Reverse the angular velocity
			ang_vel = -ang_vel;

			// Set plates flag
			plates_on = PLATES_COMING_ON;

			// Lets see them
			draw_enable(1);
			normal_screen();
		}
		else
		{
			// FIX!!
			// Freak-out & abort !!
			i = 3;
			do if (plate_obj[i]) delobj(plate_obj[i]); while (plate_obj[i] = NULL, i--);
#if DEBUG
			lockup();
#endif
			// Set plates flag
			plates_on = PLATES_OFF;

			return;
		}
	}
	else
	{
		// Set plates flag
		plates_on = PLATES_COMING_OFF;
	}

	// Initialize the tick counter
	tcount = 0;

	snd_scall_bank(plyr_bnk_str, SWOOSH2_SND, VOLUME3, 127, PRIORITY_LVL4);

	// Transition the plates
	while (1)
	{
		for(i = 0; i < 4; i++)
		{
			if (plates_on == PLATES_COMING_ON || plates_on == PLATES_COMING_OFF)
			{
				if (lft_rgt_doors)
				{
					plate_obj[i]->x += (i == 0 || i == 1) ? -ang_vel : ang_vel;
				}
				else
				{
					plate_obj[i]->y -= (i == 1 || i == 3) ? -ang_vel:ang_vel;
				}
			}
			generate_sprite_verts(plate_obj[i]);
		}

		// Are we done ?
		if (++tcount == args[0] || plates_on == PLATES_OFF)
		{
			// Yes; coming on, going away, or did we get shutdown?
			if (plates_on == PLATES_COMING_ON)
			{
				// Just came on - snap plates to final position
				i = 3;
				do 
				{
					if ((i == 0 || i == 1) && (lft_rgt_doors))
						plate_obj[i]->x = 0.0f;
					else if ((i == 2 || i == 3) && (lft_rgt_doors))
						plate_obj[i]->x = SPRITE_HRES;
					else if ((i == 1 || i == 3) && (!lft_rgt_doors))
						plate_obj[i]->y = SPRITE_VRES;
					else if ((i == 0 || i == 2) && (!lft_rgt_doors))
						plate_obj[i]->y = 0.0f;

//					plate_obj[i]->z_angle = 0.0f;
					generate_sprite_verts(plate_obj[i]);
				} while (i--);

				sleep(1);

				// Set plates flag
				plates_on = PLATES_ON;
			}
			else if (plates_on == PLATES_COMING_OFF)
			{
				// Just went off - snap plates to final position
				i = 3;
				do 
				{
					if(i == 0 || i == 1)
						plate_obj[i]->x = -256.0f;
					else
						plate_obj[i]->x = 512.0f+256.0f;

//					plate_obj[i]->z_angle = 0.0f;
					generate_sprite_verts(plate_obj[i]);
				} while (i--);

				sleep(1);

				// Set plates flag
				plates_on = PLATES_OFF;
				kill_plates();
			}
			else
			{
				// Just went away or got shutdown - delete the plate objs
				kill_plates();
			}
			break;
		}
		sleep(1);
	}
}

//-------------------------------------------------------------------------------------------------
// flash_code_proc() - assumes sparkle-texture preloaded with
//    id SCR_PLATE_TID (default; see <create_flash_texture> in nfl.c)
//  INPUT: x,y,z for sparkle
//-------------------------------------------------------------------------------------------------
void flash_code_proc(int *args)
{
	sprite_info_t * pobj;
	int i;

	CHK_SLISTS( "flash_code_proc() entry" );
	pobj = beginobj(sparkle_imgs[0],
			(float)args[0],
			(float)args[1],
			(float)args[2],
			SCR_PLATE_TID);
	pobj->id = OID_VERSUS;
	pobj->w_scale = 2.5f;
	pobj->h_scale = 2.5f;
	generate_sprite_verts(pobj);
	CHK_SLISTS( "flash_code_proc() sprite created" );

	for (i = 1; i < SPARKLE_IMGCNT; i++) {
		sleep(3);
		change_img_tmu(pobj,sparkle_imgs[i],SCR_PLATE_TID);
	}
	// Show the last frame
	sleep(3);
	CHK_SLISTS( "flash_code_proc() pre-delete" );
	delobj(pobj);
	CHK_SLISTS( "flash_code_proc() exit" );
}

//-------------------------------------------------------------------------------------------------
// vs_unhide_proc() - 
//-------------------------------------------------------------------------------------------------
void vs_unhide_proc(int *args)
{
	hide_sprite((sprite_info_t *)(args[1]));

	while (!(p_status & (1 << args[0])))
	{
		sleep(1);
	}

	unhide_sprite((sprite_info_t *)(args[1]));
}

//-------------------------------------------------------------------------------------------------
// 					vs_pups_proc() - 
//-------------------------------------------------------------------------------------------------
#ifdef JASON
#define VS_TICKS		(9999 * 57)					//5 // total ticks
#else
#define VS_TICKS		(7 * 57)					//5 // total ticks
#endif

void vs_pups_proc(int *args)
{
	int cnt;
	
#ifndef NOVS
	ostring_t * ostrings[MAX_PLYRS][2];
	int sw_curr;
	int sw_down;
	int sw_last = 0;
	sprite_info_t * obj_oval[MAX_PLYRS];
	sprite_info_t * obj_icon[MAX_PLYRS][PUP_ICON_CNT];
	int cur_icon[MAX_PLYRS][PUP_ICON_CNT];
	char sz[] = "123456789012345678901234567890";

	int i, j, k, n, s, t;
	float y;


	// Get length of longest text string
	n = PUP_CODE_CNT;
	j = 0;
	while (n--)
	{
		i = 0;
		if (pupcodes[n].text1)
			while (pupcodes[n].text1[i])
				i++;
		if (j < i)
			j = i;

		i = 0;
		if (pupcodes[n].text2)
			while (pupcodes[n].text2[i])
				i++;
		if (j < i)
			j = i;
	}
	if (j > sizeof(sz))
		j = sizeof(sz);
	sz[j] = '\0';

	// Init all the objects, keeping them hidden till later
	set_text_font(FONTID_BAST10);
	set_text_z_inc(0.01f);
	i = 0;
	y = (is_low_res) ? 10.0f : 14.0f;
	do {
		// create ICON backplate
		obj_oval[i] = beginobj(&vs_ibox,
				x_pup_icon[four_plr_ver][i][0],			// use center ICON as positioner
//				y_pup_icon[four_plr_ver],
				Y_PUP_ICON,
				z_pup_oval[0],
				BARS_PG_TID);
		obj_oval[i]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
		obj_oval[i]->id = OID_VERSUS;

		qcreate("vs_unhd", PRE_GAME_SCRNS_PID, vs_unhide_proc, i, (int)(obj_oval[i]), 0, 0);

		j = 0;
		do {

			obj_icon[i][j] = beginobj(pii_pup_icon[cur_icon[i][j] = 0],
					x_pup_icon[four_plr_ver][i][j],
//					y_pup_icon[four_plr_ver],
					Y_PUP_ICON,
					z_pup_icon[j],
					BARS_PG_TID);
			obj_icon[i][j]->id = OID_VERSUS;

			qcreate("vs_unhd", PRE_GAME_SCRNS_PID, vs_unhide_proc, i, (int)(obj_icon[i][j]), 0, 0);

		} while (++j < PUP_ICON_CNT);

		// create text ostrings
		set_string_id( 5 + i );
		for (j = 0; j < 2; j++)
		{
			set_text_position(
				x_pup_icon[four_plr_ver][i][0],
				y_pup_text[is_low_res][four_plr_ver][i] - (j ? y : 0.0f),
				Z_PUP_TEXT);
			ostrings[i][j] = oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, sz);
			update_ostring_string_nonalpha(ostrings[i][j], " ");
		}
	} while (++i < (four_plr_ver ? MAX_PLYRS : MAX_PLYRS-2));

	set_text_z_inc(0.0f);
#endif	//NOVS

	// Wait a bit before making input active
	sleep(6);

	// Loop here till its time to go
	cnt = VS_TICKS;
	do {
#ifndef NOVS
		sw_curr = get_player_sw_current();
		sw_down = (sw_curr ^ sw_last) & sw_curr;
		sw_last = sw_curr;
		s = 0;
		i = 0;
		do {
			if (p_status & (1 << i))
			{
				if ((k = (sw_down >> ((i<<3)+4) ) & ((P_A|P_B|P_C)>>4) ))
				{
					s |= 1;
					n = 0;
					do {
						if (k & 1)
						{
							if (++cur_icon[i][n] == PUP_ICON_IMGCNT)
								cur_icon[i][n] = 0;
						}
					} while (k >>= 1, ++n < PUP_ICON_CNT);
					// Set flag to do <change_img_tmu>
					k = 1;
				}
				if ((t = (sw_down >> (i << 3)) & P_RLDU_MASK))
				{
					// Reorder the counts to be buttons C,A,B
					j = (cur_icon[i][2] << 8) |
						(cur_icon[i][0] << 4) |
						(cur_icon[i][1]);
					// Always clr the counts
					cur_icon[i][0] = cur_icon[i][1] = cur_icon[i][2] = 0;
					// Always clr any old text if a non-000 was entered
					if (j)
					{
						CHK_SLISTS( "vs_pups_proc() PRE delete_multiple_strings()" );
						for (n = 0; n < 2; update_ostring_string_nonalpha(ostrings[i][n++], " "));
						CHK_SLISTS( "vs_pups_proc() POST delete_multiple_strings()" );
					}
					// See if code does something
					n = PUP_CODE_CNT;
					while (n--)
						if (pupcodes[n].code == j && pupcodes[n].stick == t)
							break;

					// ignore PC_4PONLY codes if two-player kit
					if ((pupcodes[n].control & PC_4PONLY) && !four_plr_ver)
						n = -1;

					if (n >= 0)
					{
						// Don't do multiple entries of the same code!
						if (!(pupcodes[n].flag[0] & (1 << i))) {
							// Do the valid code
							pupcodes[n].flag[0] |= (1 << i);
#ifdef JASON
							fprintf(stderr,"PUP: %s is now 0x%x\r\n",pupcodes[n].text1, pupcodes[n].flag[0]);
#endif
							qcreate("vs_sprk", PRE_GAME_SCRNS_PID, flash_code_proc,
//								x_pup_icon[four_plr_ver][i][0], y_pup_icon[four_plr_ver], Z_SPARKLE+1.0f, 0);
								x_pup_icon[four_plr_ver][i][0], Y_PUP_ICON, Z_SPARKLE+1.0f, 0);
							qcreate("vs_sprk", PRE_GAME_SCRNS_PID, flash_code_proc,
//								x_pup_icon[four_plr_ver][i][1], y_pup_icon[four_plr_ver], Z_SPARKLE+0.0f, 0);
								x_pup_icon[four_plr_ver][i][1], Y_PUP_ICON, Z_SPARKLE+0.0f, 0);
							qcreate("vs_sprk", PRE_GAME_SCRNS_PID, flash_code_proc,
//								x_pup_icon[four_plr_ver][i][2], y_pup_icon[four_plr_ver], Z_SPARKLE+2.0f, 0);
								x_pup_icon[four_plr_ver][i][2], Y_PUP_ICON, Z_SPARKLE+2.0f, 0);

							if (pupcodes[n].text2)
							{
								CHK_SLISTS( "vs_pups_proc() PRE double oprintf()" );
								update_ostring_string_nonalpha(ostrings[i][0], pupcodes[n].text1);
								update_ostring_string_nonalpha(ostrings[i][1], pupcodes[n].text2);
								CHK_SLISTS( "vs_pups_proc() POST double oprintf()" );
							}
							else
							{
								CHK_SLISTS( "vs_pups_proc() PRE single oprintf()" );
								update_ostring_string_nonalpha(ostrings[i][1], pupcodes[n].text1);
								CHK_SLISTS( "vs_pups_proc() POST single oprintf()" );
							}

							s |= 2;

							// SPECIAL:  If noalways, SET always_qb and always_wr for both members of team
							if (pupcodes[n].flag == &pup_noalways)
							{
								pup_always_qb |= TEAM_MASK(i/2);
								pup_always_receiver |= TEAM_MASK(i/2);
							}
								

							// SPECIAL:  If always_receiver, clear always_qb, and vice versa
							if (pupcodes[n].flag == &pup_always_qb)
								pup_always_receiver &= ~(1<<i);
							if (pupcodes[n].flag == &pup_always_receiver)
								pup_always_qb &= ~(1<<i);
						}
						else
						{
							s |= 4;
						}
					}
					else
					{
						if (j) s |= 4;
					}
					// Set flag to do <change_img_tmu>
					k = 1;
				}
				if (k)
				{
					obj_icon[i][0]->ii = pii_pup_icon[cur_icon[i][0]];
					generate_sprite_verts(obj_icon[i][0]);
//					change_img_tmu(obj_icon[i][0],
//							pii_pup_icon[cur_icon[i][0]],
//							BARS_PG_TID);
					obj_icon[i][1]->ii = pii_pup_icon[cur_icon[i][1]];
					generate_sprite_verts(obj_icon[i][1]);
//					change_img_tmu(obj_icon[i][1],
//							pii_pup_icon[cur_icon[i][1]],
//							BARS_PG_TID);
					obj_icon[i][2]->ii = pii_pup_icon[cur_icon[i][2]];
					generate_sprite_verts(obj_icon[i][2]);
//					change_img_tmu(obj_icon[i][2],
//							pii_pup_icon[cur_icon[i][2]],
//							BARS_PG_TID);
				}
			}
		} while (++i < (four_plr_ver ? MAX_PLYRS : MAX_PLYRS-2));

		if (s & 4)
			snd_scall_bank(plyr_bnk_str,VS_WRONG_CODE_SND,VOLUME3,127,PRIORITY_LVL3);	// wrong combo
		if (s & 2)
			snd_scall_bank(cursor_bnk_str,VS_GOT_CODE_SND,VOLUME3,127,PRIORITY_LVL4);	// got one!!
		if (s & 1)
			snd_scall_bank(cursor_bnk_str,VS_CHANGE_ICON_SND,VOLUME3,127,PRIORITY_LVL2);	// changing icon
#endif	//NOVS

		sleep(1);
	} while (cnt--);

	// process flags on powerups
	for( i = 0; i < PUP_CODE_CNT; i++ )
	{
		// clear any PC_AGREE powerups that haven't been set by both teams
		if ((pupcodes[i].control & PC_AGREE) &&
			(pupcodes[i].flag[0]))
		{
			if (four_plr_ver)
			{
				if ((!(pupcodes[i].flag[0] & 0x3)) ||
					(!(pupcodes[i].flag[0] & 0xC)))
				{
					pupcodes[i].flag[0] = 0;
					#ifdef PUP_DEBUG
					fprintf( stderr, "PUP: %s not agreed, cleared\n", pupcodes[i].text1 );
					#endif
				}
			}
			else
			{
				if (pupcodes[i].flag[0] != 3)
				{
					pupcodes[i].flag[0] = 0;
					#ifdef PUP_DEBUG
					fprintf( stderr, "PUP: %s not agreed, cleared\n", pupcodes[i].text1 );
					#endif
				}
			}
		}

		// clear any PC_TCANCEL powerups that were set by both teammates
		if ((pupcodes[i].control & PC_TCANCEL) &&
			(four_plr_ver))
		{
			if ((pupcodes[i].flag[0] & 0x03) == 0x03)
				pupcodes[i].flag[0] &= ~0x03;
			if ((pupcodes[i].flag[0] & 0x0C) == 0x0C)
				pupcodes[i].flag[0] &= ~0x0C;
		}
	}
	// Was <game_config> selected?
	if (user_config())
	{
		// Yes; do any overiding states from <game_config>
		i = pup_fog;
		j = pup_thickfog;
		apply_user_config( GC_FOG );
		pup_fog |= i;
		pup_thickfog |= j;

		k = pup_pullback;
		apply_user_config( GC_CAMERA );
		pup_pullback |= k;
	}
}

//-------------------------------------------------------------------------------------------------
//  rnd_field_stadium() -
//-------------------------------------------------------------------------------------------------

void rnd_field_stadium(void)
{
	int rnd = randrng(100);

	if (rnd < 35)
		pup_field = randrng(2);				// New grass (alt)
	else
	if (rnd < 60)
		pup_field = 2;						// Astro turf
	else
	if (rnd < 85)
		pup_field = randrng(2) ? 3 : 4;		// Dirt : Snow
	else
	if (rnd < 95)
		pup_field = 6;						// Mud
	else
		pup_field = 5;						// Asphalt

	pup_stadium = (pup_field == 4) ? 2 : randrng(2);
}

//-------------------------------------------------------------------------------------------------
//			 vs_screen() - 
//-------------------------------------------------------------------------------------------------
void vs_screen(void)
{
//	struct texture_node	*pnode;
//	struct texture_node	*pnode2;
	struct texture_node	* tn;
//	int 			sw;
//	obj3d_data_t	*phelm;
	sprite_info_t * bkgrnd_obj;
	image_info_t ** img;
	int i;

	// Init the pup flags
	i = 0;
	do {
		pupcodes[i].flag[0] = 0;
	} while (++i < PUP_CODE_CNT);

	CHK_SLISTS( "vs_screen() entry" );
	// set random field & stadium
	rnd_field_stadium();

	// now look for player override
	stick_stadium();

	// Init with default values
	pup_showaudibles = 0;			// hide audible names
	pup_playbook = 1;				// default playbook

	// Was <game_config> selected?
	if (user_config())
	{
		// Yes; set states from <game_config> here so powerup codes can
		//  still be active
		apply_user_config( GC_AUDIBLE );
		apply_user_config( GC_PLAYBOOK );
		apply_user_config( GC_CPU );		// CPU Assist
		apply_user_config( GC_SKILL );		// Skill level
	}

	screen = VS_SCRN;

	wipeout();
	sleep(1);
	CHK_SLISTS( "vs_screen() POST wipeout()" );
	
//	#ifdef DEBUG
//	init_bog_monitor();
//	#endif

#ifndef NOAUDIO
////////////////////////////////////////
// Load all the sound data
//
	// Kill teamsel music & start VS 
//	snd_stop_track(SND_TRACK_0);
	snd_stop_all();
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	// Play drum loop
//	snd_scall_bank(generic_bnk_str, 18, VOLUME2, 127, SND_PRI_SET|SND_PRI_TRACK0);
	snd_scall_bank(generic_bnk_str, 16, VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK0);

	coin_sound_holdoff(TRUE);

	// get rid of the name-entry tune
	snd_bank_delete(intro_bnk_str);
	snd_bank_delete(cursor_bnk_str);

	// load smaller cursor bank
	strcpy(cursor_bnk_str, "cursors2");
	if (snd_bank_load_playing(cursor_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// load game tune
//	strcpy(game_tune_bnk_str, game_tunes_tbl[3]);	// always L_MAIN -- for testing
	strcpy(game_tune_bnk_str, game_tunes_tbl[randrng(5)]);
	if (snd_bank_load_playing(game_tune_bnk_str) == 0xeeee)
//	if (snd_bank_load_playing(qtr_tunes_tbl[game_info.game_quarter]) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
		
	// load the team sound banks
	strncpy(team1_bnk_str, teaminfo[game_info.team[0]].name, 8);
	strncpy(team2_bnk_str, teaminfo[game_info.team[1]].name, 8);

	// load team 1 speech
	if (snd_bank_load_playing(team1_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
	
	// load team 2 speech (only if its a different team)
	if (game_info.team[0] != game_info.team[1])
	{
		if (snd_bank_load_playing(team2_bnk_str) == 0xeeee)
			increment_audit(SND_LOAD_FAIL_AUD);
	}

	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	coin_sound_holdoff(FALSE);
#endif

#ifdef USE_DISK_CMOS
	/* flush the CMOS cache */
	write_cmos_to_disk(TRUE);
	update_other_cmos_to_disk(TRUE);
#endif

	// Re-create the wipeout'd plate objects
	create_plates();

	// Re-enable draw
	draw_enable(1);
	normal_screen();

	iqcreate("vs_spch", PRE_GAME_SCRNS_PID+1, versus_speech_proc, 0, 0, 0, 0);

	// Make versus page background
	bkgrnd_obj = beginobj(&vsbg, 0, SPRITE_VRES, Z_HELMET, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;

	bkgrnd_obj = beginobj(&vsbg_c1, 0, SPRITE_VRES, Z_HELMET, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;

	bkgrnd_obj = beginobj(&vsbg_c2, 0, SPRITE_VRES, Z_HELMET, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;

	bkgrnd_obj = beginobj(&vsbg_c3, 0, SPRITE_VRES, Z_HELMET, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;


#if 0
	bkgrnd_obj = beginobj(&vsbg, SPRITE_HRES/2.0f, SPRITE_VRES, Z_HELMET, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;
	bkgrnd_obj->mode |= FLIP_TEX_H;
	generate_sprite_verts(bkgrnd_obj);


	bkgrnd_obj = beginobj(&vsbg_c1, SPRITE_HRES/2.0f, SPRITE_VRES, Z_HELMET, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;
	bkgrnd_obj->mode |= FLIP_TEX_H;
	generate_sprite_verts(bkgrnd_obj);
#endif

	// create helmet, team 1
	bkgrnd_obj = beginobj(tm_helmet_imgs[game_info.team[0]][0], 129.0f, SPRITE_VRES/2.0f-2.0f, Z_HELMET-30.0f, BARS_PG_TID);
	bkgrnd_obj->w_scale = 1.20f;
	bkgrnd_obj->h_scale = 1.20f;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj->id = OID_VERSUS+128;

	// create helmet, team 2
	bkgrnd_obj = beginobj(tm_helmet_imgs[game_info.team[1]][1], 383.0f, SPRITE_VRES/2.0f-2.0f, Z_HELMET-30.0f, BARS_PG_TID);
	bkgrnd_obj->w_scale = 1.20f;
	bkgrnd_obj->h_scale = 1.20f;
	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj->id = OID_VERSUS+128;

	// put up VS image
	bkgrnd_obj = beginobj(&vs_vs, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f+8.0f, Z_HELMET-1, BARS_PG_TID);
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
//	generate_sprite_verts(bkgrnd_obj);
	bkgrnd_obj->id = OID_VERSUS+128;

	// team 1 city name
	img = teaminfo[game_info.team[0]].pti;
	bkgrnd_obj = beginobj(img[0], 128.0f, SPRITE_VRES-28.0f, Z_HELMET-4.0f, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;

	// team 1 name logo
	bkgrnd_obj = beginobj(img[2], 128.0f, SPRITE_VRES-70.0f, Z_HELMET-2.0f, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;
	bkgrnd_obj->w_scale = .75f;
	bkgrnd_obj->h_scale = .75f;
	generate_sprite_verts(bkgrnd_obj);
	if (img[3])
	{
		bkgrnd_obj = beginobj(img[3], 128.0f, SPRITE_VRES-70.0f, Z_HELMET-2.0f, BARS_PG_TID);
		bkgrnd_obj->w_scale = .75f;
		bkgrnd_obj->h_scale = .75f;
		bkgrnd_obj->id = OID_VERSUS+128;
		generate_sprite_verts(bkgrnd_obj);
	}

	// team 2 city name
	img = teaminfo[game_info.team[1]].pti;
	bkgrnd_obj = beginobj(img[0], 384.0f, SPRITE_VRES-28.0f, Z_HELMET-4.0f, BARS_PG_TID);
	bkgrnd_obj->id = OID_VERSUS+128;

	// team 2 name logo
	bkgrnd_obj = beginobj(img[2], 384.0f, SPRITE_VRES-70.0f, Z_HELMET-2.0f, BARS_PG_TID);
	bkgrnd_obj->w_scale = .75f;
	bkgrnd_obj->h_scale = .75f;
	bkgrnd_obj->id = OID_VERSUS+128;
	generate_sprite_verts(bkgrnd_obj);
	if (img[3])
	{
		bkgrnd_obj = beginobj(img[3], 384.0f, SPRITE_VRES-70.0f, Z_HELMET-2.0f, BARS_PG_TID);
		bkgrnd_obj->w_scale = .75f;
		bkgrnd_obj->h_scale = .75f;
		bkgrnd_obj->id = OID_VERSUS+128;
		generate_sprite_verts(bkgrnd_obj);
	}
	CHK_SLISTS( "vs_screen() POST beginobj()" );

	// which field was selected text
	set_text_font(FONTID_BAST10);
	set_string_id(89);
	set_text_position(SPRITE_HRES/2.0f,SPRITE_VRES/2.0f+85.0f,1.5);
	// Was <game_config> selected?
	if (user_config())
	{
		// Yes; set states from <game_config>; powerup codes may step on this
		//  but will be reset again in "field.c"
		apply_user_config( GC_FIELD );
	}
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, field_selected_msg[pup_field]);
	set_text_position(SPRITE_HRES/2.0f,SPRITE_VRES/2.0f+60.0f-(is_low_res ? 1.0f : 0.0f),1.5);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, stadium_selected_msg[pup_stadium]);

	CHK_SLISTS( "vs_screen() POST oprintf()" );


#ifndef NOVS
	// Load the rest of the textures
	if (!create_texture(pii_pup_icon[0]->texture_name,
			BARS_PG_TID,
			0,
			CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP,
			GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR,
			GR_TEXTUREFILTER_BILINEAR))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",pii_pup_icon[0]->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
	// Sparkle texture is loaded as the initial/between qtr load with an id
	//  of SCR_PLATE_TID
	if (!(tn = create_flash_texture()))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",sparkle_imgs[0]->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}

	lock_texture(tn);
	lock_multiple_textures(BARS_PG_TID,0xffffffff);
#endif	//NOVS

	iqcreate("vs_pups", PRE_GAME_SCRNS_PID+1, vs_pups_proc, 0, 0, 0, 0);

	sleep(1);

	// Take away the plates 
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates
	sleep(PLATE_TRANSITION_TICKS+1);
	// Make sure plates are gone
	plates_on = PLATES_OFF;
}

//////////////////////////////////////////////////////////////////////////////
// read all the sticks & buttons looking for stadium powerups.  If more than
// one player has an opinion, pick one at random and listen to him.
static void stick_stadium( void )
{
	int		fields[4];
	int		stadiums[4];
	int		i, field, stadium, sw, ocnt = 0;

	for( i = 0; i < 4; i++ )
	{
		sw = get_player_sw_current() >> (8*i);
		field = -1;
		stadium = -1;

		if (!(p_status & (1<<i)))
			continue;

		if (sw & P1_C)
		{
			if (sw & P1_UP)
				field = 0;
			else if (sw & P1_DOWN)
				field = 1;
			else if (sw & P1_LEFT)
				field = 2;
			else if (sw & P1_RIGHT)
				field = 3;
		}

		if ((sw & P1_A) && (sw & P1_B))
		{
			stadium = 2;
			field = 4;
		}
		else if (sw & P1_A)
			stadium = 0;
		else if (sw & P1_B)
			stadium = 1;

		if ((field != -1) || (stadium != -1))
		{
			fields[ocnt] = field;
			stadiums[ocnt++] = stadium;
		}
	}

	if (ocnt == 0)
		return;

	i = randrng(ocnt);

	if (fields[i] != -1)
		pup_field = fields[i];
	if (stadiums[i] != -1)
		pup_stadium = stadiums[i];
}

//////////////////////////////////////////////////////////////////////////////
