#if defined(VEGAS)

#define USE_DISK_CMOS
void update_other_cmos_to_disk(int sleep_allowed);

#endif

/****************************************************************************/
/*                                                                          */
/* attract.c - Main attract mode loop.                                      */
/*                                                                          */
/* Written by:  Jeff Johnson and Michael J. Lynch                           */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Right Reserved                                                       */
/*                                                                          */
/* $Revision: 334 $                                                         */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <string.h>
#include <dir.h>

#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/stream.h"
#include "/Video/Nfl/Include/movie.h"
#include "/Video/Nfl/Include/sweeps.h"
#include "/Video/Nfl/Include/Record.h"
#include "/Video/Nfl/Include/PlaySel.h"
#include "/Video/Nfl/Include/Coin.h"
#include "/Video/Nfl/Include/Attract.h"
#include "/video/nfl/include/game_sel.h"
#include "/video/nfl/include/gameadj.h"
#include "/video/nfl/include/game_cfg.h"

#define IMAGEDEFINE	1
#include "/video/nfl/include/gamesel.h"
#include "include/general.h"
#include "include/flash.h"
#include "include/credits.h"
#include "include/newfeatr.h"
//#include "include/ending.h"


#define LOOPS_UNTIL_REBOOT	3
#define HELMET_LIMBS		10

#ifdef DEBUG
//	#define TEST_EXEC			//Include for immediate EXEC execution
//	#define STICK_HACK			//Include for joystick debugging to screen & hyperterm
//	#define DBG_GC				//Include to test grand champ movie
//	#define DBG_TC				//Include to test trivia contest
#endif


int set_coin_hold(int flag);
typedef struct popular_team_info
{
	int		team_number;
	int		times_choosen;
} popular_team_info_t;


extern LIMB *helmet_limbs[];
extern void launch_game_proc(void);

struct texture_node	*pnode;

static obj3d_data_t	h1_limb1;
static obj3d_data_t	h1_limb2;
static obj3d_data_t	h1_limb3;
static obj3d_data_t	h1_limb4;
static obj3d_data_t	h1_limb5;
static obj3d_data_t	h1_limb6;
static obj3d_data_t	h1_limb7;
static obj3d_data_t	h1_limb8;
static obj3d_data_t	h1_limb9;
static obj3d_data_t	h1_limb10;


////////////////////////////////////////
// Grand champ screen stuff

static char * gc_tex_tbl[] = {
	NULL,
};

static image_info_t * pii_gc_tbl[] = {
	(image_info_t *) 0,
	&hiscor1,
	&hiscor1_c1,
	&hiscor1_c2,
	&hiscor1_c3,
	(image_info_t *) -1
};

static char	* gc_movie[] = {
	"dnce", 0
};
static char gc_snd_bnk_str[] = "winall";

enum {
	SCRIPTGC = 0x10000,

	LOADTEX,			// tid,v:*
	LOCKTEX,			// tid
	KILLTEX,			// tid

	LOADBNK,			// c:*
	CALLSND,			// c:*,snd,vol,trk
	STOPSND,			// trk
	KILLBNK,			// c:*

	INITMOV,			// f:z,fps
	PLAYMOV,			// v:*,last

	SETDRAW,			// flag
	SETFADE,			// f:%,fcnt
	DOTRANS,			// flag

	TBL_OBJ,			// tid,v:*,f:z
	SHO_OBJ,			// oid#
	HID_OBJ,			// oid#
	CUR_OBJ,			// oid#
	FAD_OBJ,			// f:%,ticks
	POS_OBJ,			// f:x,f:y
	VEL_OBJ,			// f:xv,f:yv

	COL_STR,			// color
	DEL_STR,			// sid#
	FAD_STR,			// f:%,ticks
	FON_STR,			// font
	JUS_STR,			// justify
	NAM_STR,			// c:*,f:yinc
	OUT_STR,			// c:*,f:yinc
	POS_STR,			// f:x,f:y
	SID_STR,			// sid#
	VEL_STR,			// f:xv,f:yv
};

union IFCV {
	int		i;
	float	f;
	char *	c;
	void *	v;
};

#define BG_W	800.0f
#define BG_D	(BG_W-SPRITE_HRES)

#define FG0_W	334.0f
#define FG1_W	373.0f

#undef HRES
#undef VRES
#define HRES	(SPRITE_HRES)
#define VRES	(SPRITE_VRES)
#define HRAT	(SPRITE_HRES/512.0f)
#define VRAT	(SPRITE_VRES/384.0f)

#define HJ_L	(HJUST_LEFT)
#define HJ_C	(HJUST_CENTER)
#define HJ_R	(HJUST_RIGHT)
#define VJ_T	(VJUST_TOP)
#define VJ_C	(VJUST_CENTER)
#define VJ_B	(VJUST_BOTTOM)

union IFCV gc_script[] = {
	{SETFADE},	{f:1.0},					{1},
	{DOTRANS},	{0},

//	{LOADTEX},	{BKGRND_TID},				{v:gc_tex_tbl},
//	{LOCKTEX},	{BKGRND_TID},
//	{LOCKTEX},	{0},

	{TBL_OBJ},	{BKGRND_TID},				{v:pii_gc_tbl},				{f:501.0},
	{SHO_OBJ},	{0},
	{POS_OBJ},	{f:0.0},					{f:VRES},
//	{FAD_OBJ},	{f:0.0},					{0},

	{LOADBNK},	{c:gc_snd_bnk_str},
	{CALLSND},	{c:gc_snd_bnk_str},			{0},		{VOLUME4},		{SND_PRI_SET|SND_PRI_TRACK0},

	{FON_STR},	{FONTID_BAST23SCALE},
	{JUS_STR},	{HJ_C|VJ_B},

	{SID_STR},	{1},
	{COL_STR},	{LT_YELLOW},
	{POS_STR},	{f:HRES*0.5},				{f:342.0 *VRAT},
	{OUT_STR},	{c:"new grand champion"},	{f:0.0},
//	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{COL_STR},	{LT_GREEN},
	{POS_STR},	{f:HRES*0.5},				{f:240.0 *VRAT},
	{OUT_STR},	{c:"the game is over."},	{f:0.0},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{3},
	{COL_STR},	{LT_GREEN},
	{POS_STR},	{f:HRES*0.5},				{f:174.0 *VRAT},
	{OUT_STR},	{c:"the field is clear."},	{f:0.0},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{4},
	{COL_STR},	{LT_RED2},
	{POS_STR},	{f:HRES*0.5},				{f:108.0 *VRAT},
	{OUT_STR},	{c:"well, why not?"},		{f:-30.0},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{5},
	{OUT_STR},	{c:"you did win."},			{f:0.0},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{6},
	{COL_STR},	{LT_YELLOW},
	{POS_STR},	{f:180.0 *HRAT},			{f:-30.0 *VRAT},
	{OUT_STR},	{c:"do the cabbage"},		{f:-30.0 *VRAT},
	{OUT_STR},	{c:"patch hip-hop."},		{f:-48.0 *VRAT},
	{COL_STR},	{LT_YELLOW},
	{OUT_STR},	{c:"celebrate! you've"},	{f:-30.0 *VRAT},
	{OUT_STR},	{c:"earned it."},			{f:-48.0 *VRAT},
	{COL_STR},	{LT_YELLOW},
	{OUT_STR},	{c:"no one will"},			{f:-30.0 *VRAT},
	{OUT_STR},	{c:"forget this win."},		{f:0.0},
	{15},
//15
//	{FAD_OBJ},	{f:1.0},					{25},
//	{SID_STR},	{1},
//	{FAD_STR},	{f:1.0},					{25},
	{105},
//120
	{SID_STR},	{2},
	{FAD_STR},	{f:1.0},					{40},
	{160},
//280
	{SID_STR},	{3},
	{FAD_STR},	{f:1.0},					{40},
	{160},
//440
	{SID_STR},	{4},
	{FAD_STR},	{f:1.0},					{40},
	{80},
//520
	{SID_STR},	{5},
	{FAD_STR},	{f:1.0},					{40},
	{290},
//810
	{SETFADE},	{f:0.0},					{120},
	{120},
//930
	{HID_OBJ},	{0},
	{DEL_STR},	{1},
	{DEL_STR},	{2},
	{DEL_STR},	{3},
	{DEL_STR},	{4},
	{DEL_STR},	{5},
	{70},
//1000
	{INITMOV},	{f:500.0},					{30},
	{PLAYMOV},	{v:gc_movie},				{2590},
	{SETFADE},	{f:1.0},					{1},
	{480},
//1480
	{SID_STR},	{6},
	{FAD_STR},	{f:0.8},					{0},
	{VEL_STR},	{f:0.0},					{f:1.0},
	{300},
//1780
	{VEL_STR},	{f:0.0},					{f:0.0},
	{310},
//2090
	{FAD_STR},	{f:0.0},					{50},
	{50},
//2140
	{DEL_STR},	{6},
	{SID_STR},	{1},
	{COL_STR},	{WHITE},
	{POS_STR},	{f:180.0 *HRAT},			{f:240.0 *VRAT},
	{OUT_STR},	{c:"it was beating"},		{f:-30.0},
	{OUT_STR},	{c:"all of the teams."},	{f:0.0},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{COL_STR},	{LT_MAGNETA},
	{POS_STR},	{f:180.0 *HRAT},			{f:117.0 *VRAT},
	{OUT_STR},	{c:"it was in the"},		{f:-30.0 *VRAT},
	{OUT_STR},	{c:"new millenium!"},		{f:0.0},
	{FAD_STR},	{f:0.0},					{0},
	{70},
//2210
	{SID_STR},	{1},
	{FAD_STR},	{f:0.8},					{50},
	{220},
//2430
	{SID_STR},	{2},
	{FAD_STR},	{f:0.8},					{50},
	{320},
//2750
	{FAD_STR},	{f:0.0},					{50},
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{50},
	{120},
//2870
	{DEL_STR},	{1},
	{DEL_STR},	{2},

	{SID_STR},	{1},
	{POS_STR},	{f:140.0 *HRAT},			{f:312.0 *VRAT},
	{COL_STR},	{WHITE},
	{OUT_STR},	{c:"it was the"},			{f:-75.0 *VRAT},

	{FON_STR},	{FONTID_BAST75},

	{SID_STR},	{2},
	{COL_STR},	{0xFF1880ff},
	{OUT_STR},	{c:"nfl"},					{f:-66.0 *VRAT},
	{OUT_STR},	{c:"blitz"},				{f:-66.0 *VRAT},
	{COL_STR},	{0xFFffff40},
	{OUT_STR},	{c:"2000"},					{f:-75.0 *VRAT},

	{JUS_STR},	{HJ_L|VJ_B},

	{SID_STR},	{3},
	{POS_STR},	{f:24.0 *HRAT},				{f:30.0 *VRAT},
	{COL_STR},	{LT_RED2},
	{OUT_STR},	{c:"superbowl"},			{f:0.0},
	{CALLSND},	{c:ancr_bnk_str},			{231},		{80},			{LVL2},

	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.0}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.0}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.0}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.0}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.0}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.0}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.0}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.0}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.0}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.0}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.0}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.0}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.0}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.0}, {0}, {4},
	{SID_STR},	{2}, {FAD_STR}, {f:0.8}, {0}, {SID_STR}, {3}, {FAD_STR}, {f:0.8}, {0}, {4},
//2930
	{270},
//3200
	{SID_STR},	{1},
	{VEL_STR},	{f:0.0},					{f:16.0},
	{SID_STR},	{2},
	{VEL_STR},	{f:-16.0},					{f:0.0},
	{SID_STR},	{3},
	{VEL_STR},	{f:0.0},					{f:-16.0},
	{CALLSND},	{c:plyr_bnk_str},			{131},		{VOLUME4},		{PRIORITY_LVL1},
	{30},
//3230
	{DEL_STR},	{1},
	{DEL_STR},	{2},
	{DEL_STR},	{3},

	{FON_STR},	{FONTID_BAST23SCALE},
	{JUS_STR},	{HJ_C|VJ_B},

	{SID_STR},	{1},
	{POS_STR},	{f:180.0 *HRAT},			{f:240.0 *VRAT},
	{COL_STR},	{WHITE},
	{OUT_STR},	{c:"congratulations!"},		{f:-48.0 *VRAT},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{2},
	{COL_STR},	{LT_YELLOW},
	{OUT_STR},	{c:"you have entered"},		{f:-30.0 *VRAT},
	{OUT_STR},	{c:"the elite ranks"},		{f:-30.0 *VRAT},
	{OUT_STR},	{c:"of the blitz 2000"},	{f:-33.0 *VRAT},
	{COL_STR},	{LT_MAGNETA},
	{OUT_STR},	{c:"grand champions!"},		{f:0.0},
	{FAD_STR},	{f:0.0},					{0},
	{SID_STR},	{3},
	{POS_STR},	{f:180.0 *HRAT},			{f:210.0 *VRAT},
	{COL_STR},	{LT_ORANGE},
	{OUT_STR},	{c:"midway games"},			{f:-30.0 *VRAT},
	{OUT_STR},	{c:"thanks you for"},		{f:-30.0 *VRAT},
	{OUT_STR},	{c:"playing."},				{f:0.0},
	{FAD_STR},	{f:0.0},					{0},
	{60},
//3290
	{SID_STR},	{1},
	{FAD_STR},	{f:0.8},					{50},
	{140},
//3430
	{SID_STR},	{2},
	{FAD_STR},	{f:0.8},					{50},
	{360},
//3790
	{SID_STR},	{1},
	{FAD_STR},	{f:0.0},					{50},
	{SID_STR},	{2},
	{FAD_STR},	{f:0.0},					{50},
	{50},
//3840
	{DEL_STR},	{1},
	{DEL_STR},	{2},

	{SID_STR},	{3},
	{FAD_STR},	{f:0.8},					{50},
	{340},
//4180
	{DOTRANS},	{1},
//	{SETFADE},	{f:0.0},					{1},
	{DEL_STR},	{3},
//	{1},
//	{SETDRAW},	{0},
	{20},
//

	{STOPSND},	{SND_TRACK_0},
	{KILLBNK},	{c:gc_snd_bnk_str},

	{KILLTEX},	{BKGRND_TID},

{SCRIPTGC}};

//new grand champion
//the game is over. the field is clear. why not? you did win.
//do the cabbage patch hip-hop.
//celebrate! you've earned it.
//no one will forget this win.
//it was beating all the teams.
//it was in the new millenium!
//it was the blitz 2000 superbowl
//congratulations!
//you have entered the elite ranks of the blitz 2000 grand champions!
//midway games thanks you for playing.


//union IFCV gc_script[] = {
//	{SETDRAW},	{1},
//	{DOTRANS},	{0},
//
//	{SHOW_BG},	{0},
//	{POSI_BG},	{f:-BG_D},					{f:VRES},
//	{FADE_BG},	{f:0.0},					{0},
//	{FADE_BG},	{f:1.0},					{70},
//	{VELI_BG},	{f: BG_D /2291.0},			{f:0.0},
//
//	{SID_STR},	{1},
//	{COL_STR},	{LT_ORANGE},
//	{FON_STR},	{FONTID_BAST23SCALE},
//	{JUS_STR},	{HJ_C|VJ_B},
//	{POS_STR},	{f:HRES/2.0},				{f:-165.0 *VRAT},
//	{OUT_STR},	{c:"still pumped with"},	{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"getting to chalk one"},	{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"in the win column,"},	{f:-48.0 *VRAT},
//	{COL_STR},	{LT_RED},
//	{OUT_STR},	{c:"a reality-check slams"},{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"your face."},			{f:-30.0 *VRAT},
//	{FAD_STR},	{f:0.8},					{0},
//	{VEL_STR},	{f:0.0},					{f:1.0},
//	{415},
////415
//	{VEL_STR},	{f:0.0},					{f:0.0},
//	{345},
////760
//	{DEL_STR},	{1},
//	{COL_STR},	{LT_ORANGE},
//	{POS_STR},	{f:HRES/2.0},				{f:250.0 *VRAT},
//	{OUT_STR},	{c:"this was the"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"big one."},				{f:-48.0 *VRAT},
//	{FAD_STR},	{f:0.0},					{0},
//	{SID_STR},	{2},
//	{OUT_STR},	{c:"this was what"},		{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"all the hard work"},	{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"was about."},			{f:-30.0 *VRAT},
//	{FAD_STR},	{f:0.0},					{0},
//	{85},
////845
//	{SID_STR},	{1},
//	{FAD_STR},	{f:0.8},					{60},
//	{135},
////980
//	{SID_STR},	{2},
//	{FAD_STR},	{f:0.8},					{60},
//	{275},
////1255
//	{DEL_STR},	{1},
//	{DEL_STR},	{2},
//	{FON_STR},	{FONTID_BAST75},
//	{COL_STR},	{MED_RED},
//	{POS_STR},	{f:HRES/2.0},				{f:250.0 *VRAT},
//	{OUT_STR},	{c:"nfl blitz"},			{f:-100.0 *VRAT},
//	{COL_STR},	{MED_GREEN},
//	{OUT_STR},	{c:"superbowl"},			{f:-100.0 *VRAT},
//	{OUT_STR},	{c:"champion"},				{f:-100.0 *VRAT},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
//	{FAD_STR},	{f:0.0},					{0},						{8},
//	{FAD_STR},	{f:0.8},					{0},						{8},
////1391
//	{FAD_STR},	{f:0.0},					{120},
//	{SID_STR},	{1},
//	{FON_STR},	{FONTID_BAST23SCALE},
//	{COL_STR},	{LT_ORANGE},
//	{JUS_STR},	{HJ_R|VJ_B},
//	{POS_STR},	{f:480.0 *HRAT},			{f:-110.0 *VRAT},
//	{OUT_STR},	{c:"and then the"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"superbowl trophy"},		{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"is awarded."},			{f:-48.0 *VRAT},
//	{OUT_STR},	{c:"it feels so right,"},	{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"so good, that it"},		{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"takes you to"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"the top of"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"the world."},			{f:-30.0 *VRAT},
//	{FAD_STR},	{f:0.8},					{0},
//	{VEL_STR},	{f:0.0},					{f:1.0},
//	{SHOW_FG},	{0},
//	{POSI_FG},	{f:-FG0_W - 50.0},			{f:VRES},
//	{VELI_FG},	{f: FG0_W /900.0},			{f:0.0},
//	{120},
////1511
//	{DEL_STR},	{2},
//	{310},
////1821
//	{VEL_STR},	{f:0.0},					{f:0.0},
//	{470},
////2291
//	{VELI_BG},	{f:0.0},					{f:0.0},
//	{VELI_FG},	{f:0.0},					{f:0.0},
//	{370},
////2661
//	{FAD_STR},	{f:0.0},					{90},
//	{45},
////2706
//	{FADE_BG},	{f:0.0},					{90},
//	{FADE_FG},	{f:0.0},					{90},
//	{SID_STR},	{2},
//	{POS_STR},	{f:480.0 *HRAT},			{f:-110.0 *VRAT},
//	{OUT_STR},	{c:"but you know"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"the other"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"coaches and"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"players want"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"to pull you"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"down and"},				{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"take your"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"place."},				{f:-30.0 *VRAT},
//	{FAD_STR},	{f:0.8},					{0},
//	{VEL_STR},	{f:0.0},					{f:1.0},
//	{SHOW_BG},	{1},
//	{SHOW_FG},	{1},
//	{POSI_BG},	{f:-BG_D},					{f:VRES},
//	{POSI_FG},	{f:-FG1_W - 20.0f},			{f:VRES},
//	{VELI_BG},	{f: BG_D /2291.0},			{f:0.0},
//	{VELI_FG},	{f: FG1_W /900.0},			{f:0.0},
//	{45},
////2751
//	{DEL_STR},	{1},
//	{45},
////2796
//	{HIDE_BG},	{0},
//	{HIDE_FG},	{0},
//	{350},
////3146
//	{VEL_STR},	{f:0.0},					{f:0.0},
//	{350},
////3496
//	{CURR_BG},	{1},
//	{CURR_FG},	{1},
//	{VELI_BG},	{f:0.0},					{f:0.0},
//	{VELI_FG},	{f:0.0},					{f:0.0},
//	{200},
////3696
//	{FAD_STR},	{f:0.0},					{90},
//	{SID_STR},	{1},
//	{POS_STR},	{f:(480.0+340.0) *HRAT},	{f:330.0 *VRAT},
//	{OUT_STR},	{c:"and now you"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"learn of the"},			{f:-30.0 *VRAT},
//	{COL_STR},	{LT_RED},
//	{OUT_STR},	{c:"secret new"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"strengths and"},		{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"power-ups"},			{f:-30.0 *VRAT},
//	{COL_STR},	{LT_ORANGE},
//	{OUT_STR},	{c:"that your"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"opponents"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"will be"},				{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"bringing to"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"the field."},			{f:-30.0 *VRAT},
//	{FAD_STR},	{f:0.8},					{0},
//	{VEL_STR},	{f:-1.0},					{f:0.0},
//	{90},
////3786
//	{DEL_STR},	{2},
//	{250},
////4036
//	{VEL_STR},	{f:0.0},					{f:0.0},
//	{550},
////4586
//	{FAD_STR},	{f:0.0},					{90},
//	{45},
////4631
//	{FADE_BG},	{f:0.0},					{90},
//	{FADE_FG},	{f:0.0},					{90},
//	{SID_STR},	{2},
//	{POS_STR},	{f:480.0 *HRAT},			{f:330.0 *VRAT},
//	{OUT_STR},	{c:"congratulations to"},	{f:0.0},
//	{COL_STR},	{MED_YELLOW},
//	{JUS_STR},	{HJ_C|VJ_B},
//	{POS_STR},	{f:330.0 *HRAT},			{f:300.0 *VRAT},
//	{NAM_STR},	{c:""},						{f:0.0},
//	{COL_STR},	{LT_ORANGE},
//	{JUS_STR},	{HJ_R|VJ_B},
//	{POS_STR},	{f:480.0 *HRAT},			{f:270.0 *VRAT},
//	{OUT_STR},	{c:"& louise at the"},		{f:-30.0},
//	{OUT_STR},	{c:"top of it all!"},		{f:-30.0 *VRAT},
//	{COL_STR},	{MED_GREEN},
//	{OUT_STR},	{c:"midway games"},			{f:-30.0 *VRAT},
//	{COL_STR},	{LT_ORANGE},
//	{OUT_STR},	{c:"thanks you"},			{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"for playing"},			{f:(-69.0-150.0) *VRAT},
//	{FAD_STR},	{f:0.0},					{0},
//	{SID_STR},	{3},
//	{FON_STR},	{FONTID_BAST75},
//	{COL_STR},	{LT_RED},
//	{OUT_STR},	{c:"nfl     "},				{f:-66.0 *VRAT},
//	{COL_STR},	{LT_BLUE},
//	{OUT_STR},	{c:"blitz     "},			{f:0.0},
//	{COL_STR},	{0xffc0c0c0},
//	{OUT_STR},	{c:"99"},					{f:0.0},
//	{FAD_STR},	{f:0.8},					{0},
//	{SHOW_BG},	{2},
//	{POSI_BG},	{f:0.0},					{f:VRES},
//	{45},
////4676
//	{DEL_STR},	{1},
//	{45},
////4721
//	{HIDE_BG},	{1},
//	{HIDE_FG},	{1},
//	{40},
////4761
//	{SID_STR},	{2},
//	{FAD_STR},	{f:0.8},					{60},
//	{250},
////5011
//	{SID_STR},	{3},
//	{VEL_STR},	{f:0.0},					{f:10.0},
//	{150/10},
////5041
//	{VEL_STR},	{f:0.0},					{f:0.0},
//	{FAD_STR},	{f:0.0},					{0},					{2},
//	{FAD_STR},	{f:0.8},					{0},					{2},
//	{FAD_STR},	{f:0.0},					{0},					{2},
//	{FAD_STR},	{f:0.8},					{0},					{2},
//	{FAD_STR},	{f:0.0},					{0},					{2},
//	{FAD_STR},	{f:0.8},					{0},					{2},
//	{FAD_STR},	{f:0.0},					{0},					{2},
//	{FAD_STR},	{f:0.8},					{0},					{2},
//	{FAD_STR},	{f:0.0},					{0},					{2},
//	{FAD_STR},	{f:0.8},					{0},					{2},
//	{FAD_STR},	{f:0.0},					{0},					{2},
//	{FAD_STR},	{f:0.8},					{0},					{2},
//	{FAD_STR},	{f:0.0},					{0},					{2},
//	{FAD_STR},	{f:0.8},					{0},					{2},
//	{FAD_STR},	{f:0.0},					{0},					{2},
////5071
//	{FAD_STR},	{f:0.8},					{0},
//	{240},
////5311
//	{SID_STR},	{2},
//	{FAD_STR},	{f:0.0},					{60},
//	{70},
////5381
//	{DEL_STR},	{2},
//	{COL_STR},	{LT_ORANGE},
//	{FON_STR},	{FONTID_BAST23SCALE},
//	{POS_STR},	{f:(480.0) *HRAT},			{f:300.0 *VRAT},
//	{OUT_STR},	{c:"and don't forget:"},	{f:-48.0 *VRAT},
//	{FAD_STR},	{f:0.0},					{0},
//	{FAD_STR},	{f:0.8},					{60},
//	{SID_STR},	{1},
//	{COL_STR},	{LT_RED},
//	{OUT_STR},	{c:"things only get"},		{f:-30.0 *VRAT},
//	{OUT_STR},	{c:"harder now!"},			{f:-30.0 *VRAT},
//	{FAD_STR},	{f:0.0},					{0},
//	{200},
////5581
//	{FAD_STR},	{f:0.8},					{60},
//	{500},
////6051
//	{FAD_STR},	{f:0.0},					{60},
//	{SID_STR},	{2},
//	{FAD_STR},	{f:0.0},					{60},
//	{200},
////6251
//	{DOTRANS},	{1},
//	{PLATE_TRANSITION_TICKS+1},
////
//	{SETDRAW},	{0},
//
//{SCRIPTGC}};
//
//still pumped with getting to chalk one in the win column,
//a reality-check slams your face.
//this was the big one.
//this was what all the hard work was about.
//nfl blitz superbowl champion
//and then the superbowl trophy is awarded.
//it feels so right, so good, that it takes you to the top of the world.
//but you know the other coaches and players want to
//pull you down and take your place.
//and now you learn of the secret new strengths and power-ups
//that your opponents will be bringing to the field.
//from the top of the world with kromm & louise,
//midway games thanks you for playing nfl blitz 99
//and don't forget: things only get harder now!


//int grand_champ_msg[] = {
//FONTID_BAST23SCALE, LT_YELLOW,  HJUST_CENTER|VJUST_BOTTOM,    0, -228,
//	(int)"Congratulations!",
//
//FONTID_BAST23SCALE, 0xff40ff40, HJUST_CENTER|VJUST_BOTTOM,    0, -144,
//	(int)"You have defeated",
//FONTID_BAST23SCALE, 0xff40ff40, HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
//	(int)"all 30 teams to take",
//FONTID_BAST23SCALE, WHITE,      HJUST_LEFT  |VJUST_BOTTOM, -120,  -36,
//	(int)"the",
//FONTID_BAST23SCALE, LT_RED2,    HJUST_LEFT  |VJUST_BOTTOM,   78,    0,
//	(int)"nfl",
//FONTID_BAST23SCALE, LT_BLUE2,   HJUST_LEFT  |VJUST_BOTTOM,   70,    0,
//	(int)"blitz",
//FONTID_BAST75, LT_YELLOW,  HJUST_CENTER|VJUST_BOTTOM,  -28,  -78,
//	(int)"super",
//FONTID_BAST75, LT_YELLOW,  HJUST_CENTER|VJUST_BOTTOM,    0,  -70,
//	(int)"bowl!",
//
//FONTID_BAST23SCALE, 0xff40ff40, HJUST_CENTER|VJUST_BOTTOM,    0, -144,
//	(int)"but just 'cuz you're",
//FONTID_BAST23SCALE, 0xff40ff40, HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
//	(int)"the champ doesn't",
//FONTID_BAST23SCALE, 0xff40ff40, HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
//	(int)"make you the best",
//FONTID_BAST23SCALE, LT_BLUE2,   HJUST_LEFT  |VJUST_BOTTOM, -120,  -36,
//	(int)"blitz'er",
//FONTID_BAST23SCALE, 0xff40ff40, HJUST_LEFT  |VJUST_BOTTOM,  150,    0,
//	(int)"ever!",
//
//FONTID_BAST23SCALE, 0xffff80ff, HJUST_CENTER|VJUST_BOTTOM,  -30, -144,
//	(int)"now the coaches and",
//FONTID_BAST23SCALE, 0xffff80ff, HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
//	(int)"players are pumped",
////FONTID_BAST23SCALE, 0xffff80ff, HJUST_LEFT  |VJUST_BOTTOM, -168,  -36,
////	(int)"players are",
////FONTID_BAST23SCALE, LT_YELLOW,  HJUST_LEFT  |VJUST_BOTTOM,  229,    0,
////	(int)"pumped",
////FONTID_BAST23SCALE, 0xffff80ff, HJUST_CENTER|VJUST_BOTTOM,  -61,  -36,
//FONTID_BAST23SCALE, 0xffff80ff, HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
//	(int)"and ready to take",
////FONTID_BAST23SCALE, 0xffff98ff, HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
////	(int)"you down!",
//FONTID_BAST23SCALE, 0xffff80ff, HJUST_LEFT  |VJUST_BOTTOM,  -91,  -36,
//	(int)"you",
//FONTID_BAST23SCALE, LT_RED2,    HJUST_LEFT  |VJUST_BOTTOM,   79,    0,
//	(int)"down!",
//
//FONTID_BAST23SCALE, LT_YELLOW,  HJUST_CENTER|VJUST_BOTTOM,   12, -180,
//	(int)"thanks for playing...",
//
//FONTID_BAST23SCALE, LT_RED2,    HJUST_CENTER|VJUST_BOTTOM,    0, -144,
//	(int)"didn't think you",
//FONTID_BAST23SCALE, LT_RED2,    HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
//	(int)"were done,",
//FONTID_BAST23SCALE, LT_RED2,    HJUST_CENTER|VJUST_BOTTOM,    0,  -36,
//	(int)"did ya?",
//-1
//};
//
// congratulations!
// you have defeated all 30 teams to take the nfl blitz super bowl!
// but just 'cuz you're the champ doesn't make you the best blitz'er ever!
// now the coaches and players are pumped and ready to take you down!
// thanks for playing...
// didn't think you were done, did ya?


static popular_team_info_t	pop_team[NUM_TEAMS] = {
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0}
};


extern image_info_t teamsel;
extern image_info_t teamsel_c1;

			 
extern image_info_t selback;
//extern image_info_t selbck2;
//extern image_info_t selbck2_c1;
extern image_info_t flshbx01;
extern image_info_t flshbx02;
extern image_info_t flshbx03;
extern image_info_t flshbx04;
extern image_info_t flshbx05;
extern image_info_t flshbx06;
extern image_info_t flshbx07;
extern image_info_t flshbx08;
extern image_info_t flshbx09;
extern image_info_t flshbx10;
//extern image_info_t blitzlog;
//extern image_info_t blitzlog_c1;
extern image_info_t blitzl_s;
extern unsigned int	start_crc32;


////////////////////////////////////////
// Function prototypes found elsewhere
void do_operator_message(int, int);
//int movie_get_state(void);
//void movie_abort(void);
//void movie_pause(void);
void draw_backdrop_flip(void);
void start_enable(int);
void backdoor_proc(int *);
unsigned int	get_text_addr(void);
unsigned int	get_text_end_addr(void);
void test_trivia_contest(int min, int max);

//void movie_proc(int *args);
//void movie_config_player(float w, float h, float xpos, float ypos, float z, int fps);


////////////////////////////////////////
// Referenced data
extern sprite_node_t * alpha_sprite_node_list;

extern plyr_record_t	*rec_sort_ram;			// pointer to the array of PLAYER RECORDs from cmos into ram
extern world_record_t	tmp_world_rec;

extern int	cmos_status;
extern int	texture_debug;

//extern void show_logo_proc(int *args);
extern font_info_t bast75_font;
extern font_info_t bast23_font;
extern font_info_t bast23scale_font;
extern font_info_t bast10_font;
extern font_info_t bast16_font;


////////////////////////////////////////
// Structure definition of attract mode functions to run
typedef struct att_func_info
{
	void  (*func)(int, int);				// Function name to call
	int   loop_count;						// Loops before function is called
	float min_time;							// Minimum time to display (seconds)
	float timeout;							// Maximum time to display (seconds)
} att_func_info_t;


////////////////////////////////////////
// Function prototypes for attract mode functions
void cheer_page(int min_time, int max_time);
void stick_crap(int, int);
void time_proc(int *);

void title_flash(int *args);
void load_title_flash(int tex, int snd);
void free_title_flash(void);

void show_b2k_movie(int min_time, int max_time);
void show_blitz_movie(int min_time, int max_time);
void show_attract_movie(char **movie, sndinfo *sounds, int allow_btn, int use_chroma_key, int ckey, int sound_delay, int wait_for_start, int kill_sound);
void new_feature_page(int, int);

void draw_bobj_backgrnd2(float, int);
void credit_page(int, int);
void credit_page_proc( int *args);
void color_bars_page(int, int);
void wait_for_any_butn_press_active_team(int, int, int);
void wait_for_any_butn_press_active_plyr(int, int, int);
void test_grand_champ_movie(int, int);
void show_movies(int, int);
//void show_nfl_movie(int, int);
//void show_nfl_movie2(int, int);
void show_midway_logo_movie(int, int);
void show_nfl_logo_movie(int, int);
//void show_big_font(int, int);
void show_stats_page(int, int);
void title_screen(int, int);
//void trading_card_screen(int, int);
void logo_3DFX_screen(int, int);
void NFLPA_screen(int, int);
void show_coaching_tips(int, int);
void do_amode_play(int,int);
void jason_screen( int, int );

void midway_team_page(int, int);
void show_team_info_pg(int, int);
void show_mugshot_proc(int *);
void one_sparkle_proc(int *);
void spawn_sparkles_proc(int *);
void nfl_logo(int, int);

extern float pass_rating(int, int, int, int, int);

void show_root_movie(int min_time, int max_time);

//void parental_advisory(int, int);
//void dcs_screen(int, int);
//void williams_legal_screen(int, int);
//void att_credits(int, int);
//void start_page(int, int);
//void nfl_legal_screen(int, int);
//void design_team_screen(int, int);
//void attract_movie(int, int);
//void attract_runner(int, int);


struct process_node *coin_insert_proc;
struct process_node *coin_credits_proc;
int	credit_pg_on;

#define APTIME	90.0f

#ifdef JASON
#undef APTIME
#define APTIME	10000.0f
//#define JTEST
#endif

#ifdef LONG_APLAY
#undef APTIME
#define APTIME	10000.0f
#endif

////////////////////////////////////////
// Functions to call during attract mode
//static att_func_info_t	att_funcs[] = {
//       {midway_team_page,	1,  2.0,      0 },	// only uses min
//	{test_grand_champ_movie,1,    0,      0 },
//};
static att_func_info_t	att_funcs[] = {
//	{ show_hiscore_tables,	1,    0,      0 },	// hiscore doesn't use timers
//	{ credit_page,			1, 12.0,      0 },	// min is also max
//	{ show_root_movie,		1,    0,      0 },
//	{ show_movies,			1,    0,      0 },	// movie doesn't use timers
#ifdef DBG_TC
	{ test_trivia_contest,	1,    0,      0 },
#endif
#ifdef DBG_GC
	{ test_grand_champ_movie,1,    0,      0 },
#endif
#ifdef STICK_HACK
	{ stick_crap,			1,10000,  10000 },
#endif
#ifdef TEST_SCREENS
	{ color_bars_page,		1,    0,      0 },
#endif
#ifdef JTEST
	{ jason_screen,			1,  0.3, 1000.0 },
#endif

	{ title_screen,			1,  0.3,    6.0 },

#ifndef NOMOVIES
	{ show_b2k_movie,		1,    0,      0 },
#endif

	{ new_feature_page,		1,  0.3,    9.0 },	// only uses min
	{ sweeps_attract,		1,  0.3,   10.0 },

#ifndef NOMOVIES
	{ show_midway_logo_movie,1,    0,      0 },	// movie doesn't use timers
#endif

	{ do_amode_play,		1,  1.0, APTIME },
	{ show_stats_page,		1,  1.0,   14.0 },
	{ credit_page,			1, 12.0,      0 },	// min is also max
	{ sweeps_attract,		1,  0.3,    6.0 },

	{ show_team_info_pg,	1,    0,      0 },
	{ NFLPA_screen,			1,  0.3,    4.0 },

#ifndef NOMOVIES
	{ show_movies,			1,    0,      0 },	// movie doesn't use timers
#endif

	{ do_amode_play,		1,  1.0, APTIME },
	{ show_stats_page,		1,  1.0,   14.0 },
	{ credit_page,			1, 12.0,      0 },	// min is also max
	{ sweeps_attract,		1,  0.3,   10.0 },

#ifndef NOMOVIES
	{ show_nfl_logo_movie,	1,    0,      0 },	// movie doesn't use timers
#endif

	{ show_hiscore_tables,	1,    0,      0 },	// hiscore doesn't use timers

	{ do_amode_play,		1,  1.0, APTIME },
	{ show_stats_page,		1,  1.0,   14.0 },
	{ credit_page,			1, 12.0,      0 },	// min is also max
	{ sweeps_attract,		1,  0.3,   10.0 },

#ifndef NOMOVIES
	{ show_blitz_movie,		1,    0,      0 },
#endif

	{ midway_team_page,		4,  3.0,      0 },	// only uses min

	{ cheer_page,			1,  0.3,    7.0 },
	{ nfl_logo,				1,  0.3,      0 },	// only uses min
	{ logo_3DFX_screen,		2,  0.3,    4.0 },

// {attract_runner, 1, 10, 30},
// {show_big_font,1,4,8},
// {start_page, 1, 4, 8},
// {nfl_logo, 1, 4, 8},
// {create_and_anim_midway_logo, 1, 5, 10},
// {show_world_records, 1, 4, 8},					// in 'record.c'
// {nfl_legal_screen, 1, 4, 8}
// {design_team_screen, 1, 1, 5},
// {all_time_high_scores, 1, 1, 5},
// {parental_advisory, 5, 1, 4},
// {daily_high_scores, 1, 1, 5},
// {att_credits, 1, 1, 5},
// {dcs_screen, 4, 1, 4}
// {show_coaching_tips,  1,  0.3,  6.0},
};
#define ATT_FUNC_CNT	((int)(sizeof(att_funcs) / sizeof(att_func_info_t)))


////////////////////////////////////////
// global data
unsigned int coin_snd_drop_calln;
unsigned int coin_snd_paid_calln;


////////////////////////////////////////
// static local data
static int att_keeper;				// Make attract function leave it up
static int team_index = NUM_TEAMS-1;

static char	*nflf_movie[] = {
"nflf",
0
};

static char	*end_movie[] = {
"end_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
"cyc_",
0
};

static char *tm_helmet_textures[] = {
									"carvs.wms",
									"falvs.wms",
									"ravvs.wms",
									"bilvs.wms",
									"panvs.wms",
									"beavs.wms",
									"benvs.wms",
									"brwvs.wms",
									"cowvs.wms",
									"brovs.wms",
									"liovs.wms",
									"pacvs.wms",
									"colvs.wms",
									"jagvs.wms",
									"chivs.wms",
									"dolvs.wms",
									"vikvs.wms",
									"patvs.wms",
									"saivs.wms",
									"giavs.wms",
									"jetvs.wms",
									"raivs.wms",
									"eagvs.wms",
									"stevs.wms",
									"chavs.wms",
									"forvs.wms",
									"seavs.wms",
									"ramvs.wms",
									"bucvs.wms",
									"titvs.wms",
									"redvs.wms",
};


static obj3d_data_t *helmet_limb_ptrs[HELMET_LIMBS] = {
									&h1_limb1,
									&h1_limb2,
									&h1_limb3,
									&h1_limb4,
									&h1_limb5,
									&h1_limb6,
									&h1_limb7,
									&h1_limb8,
									&h1_limb9,
									&h1_limb10};



static image_info_t * midway_team_imgs[] = {
	&mugdanf, &mugdant, &mugjason,&mugjeffj,&mugjenh, &mugjima,&mugjimg,
	&mugjohnr,&mugluism,&mugmarkg,&mugmarkt,&mugmikel,&mugsald
};

static char * midway_team_nms[][2] = {
	{"DAN FORDEN",""},
	{"DANIEL","THOMPSON"},
	{"JASON","SKILES"},
	{"JEFF","JOHNSON"},
	{"JENNIFER","HEDRICK"},
	{"JIM","ANDERSON"},
	{"JIM","GENTILE"},
	{"JOHN","ROOT"},
	{"LUIS","MANGUBAT"},
	{"MARK","GUIDARELLI"},
	{"MARK","TURMELL"},
	{"MIKE","LYNCH"},
	{"SAL","DIVITA"}
};

//static image_info_t * midway_team_logo_imgs[] = {
//	&midway01,&midway02,&midway03,&midway04,&midway05,
//	&midway06,&midway07,&midway08,&midway09,&midway10,
//	&midway11,&midway12,&midway13,&midway14,&midway15,
//	&midway16,&midway17,&midway18,&midway19,&midway20,
//	&midway21,&midway22,&midway23,&midway24,&midway25,
//	&midway26,&midway27,&midway28,&midway29,&midway30,
//	&midway31,&midway32,&midway33,&midway34,&midway35,
//	&midway36,&midway37,&midway38,&midway39
//};

#if 0
// images for NEW FEATURE page
static image_info_t *new_feature_imgs[] = {
//	&nf_4,		&nf_plyrs,
	&nf_creat,	&nf_play,
	&nf_on,		&nf_fire,
//	&nf_insta,	&nf_repla,
	&nf_imprv,	&nf_gmply,
	&nf_more,	&nf_plays,
	&nf_more2,	&nf_stadi,
	&nf_updat,	&nf_rostr,
//	&nf_trivi,	&nf_game,
	NULL,
};


static float new_feature_xys[][2] = {
	{316.0f *SPRITE_HRES/512.0f,  60.0f *SPRITE_VRES/384.0f},
	{316.0f *SPRITE_HRES/512.0f, 100.0f *SPRITE_VRES/384.0f}
};
#endif

static tmu_ram_texture_t midway_team_textures_tbl[] =
 {
	 {"credbg00.wms",NFL_LOGO_TID},		// 256 * 256 * 16bits
	 {"credbg01.wms",NFL_LOGO_TID},
	 {"credbg02.wms",NFL_LOGO_TID},
	 {"creds00.wms",NFL_LOGO_TID},
	 {"creds01.wms",NFL_LOGO_TID},
	 {"creds02.wms",NFL_LOGO_TID},
	 {"flash.wms",NFL_LOGO_TID},
	 {NULL,0}
 };

#if 0
static tmu_ram_texture_t nfl_logo_textures_tbl[] =
 {
	 {"nflogo00.wms",NFL_LOGO_TID},		// 256 * 256 * 16bits
	 {"nflogo01.wms",NFL_LOGO_TID},
	 {"nflogo02.wms",NFL_LOGO_TID},
	 {"nflogo03.wms",NFL_LOGO_TID},
	 {"nflogo04.wms",NFL_LOGO_TID},
	 {"nflogo05.wms",NFL_LOGO_TID},
	 {"nflogo06.wms",NFL_LOGO_TID},
	 {NULL,0}
 };
#endif

//// Player position icons to display on mugshot frames
////   see Player Position attributes in <player.h>
//static image_info_t * icon_pos_imgs[] =
//	{&ti_qb,&ti_wr,&ti_rb,&ti_rb,0,0,
//	 &ti_lb,&ti_lb,&ti_db,&ti_db};
//
//// These are for positioning the icons on top of mugshots
//// [obj#][lores,medres][x,y]
//static float icon_xy[][2][2] =
//{
//	{ {440.0f, 300.0f}, { 20.0f, 195.0f} },
//	{ {330.0f, 250.0f}, { 83.0f, 195.0f} },
//	{ {330.0f, 100.0f}, {146.0f, 195.0f} },
//	{ {440.0f,  70.0f}, {209.0f, 195.0f} },
//};
//
//// These are for positioning the flashes on top of mugshots
//// [obj#][lores,medres][x,y]
//static float flsh_xy[][2][2] =
//{
//	{ {0.0f, 0.0f}, { 66.0f, 140.0f} },
//	{ {0.0f, 0.0f}, {129.0f, 140.0f} },
//	{ {0.0f, 0.0f}, {192.0f, 140.0f} },
//	{ {0.0f, 0.0f}, {255.0f, 140.0f} },
//	{ {0.0f, 0.0f}, {318.0f, 140.0f} },
//	{ {0.0f, 0.0f}, {381.0f, 140.0f} },
//	{ {0.0f, 0.0f}, {444.0f, 140.0f} }
//};

// These are for positioning offsets for the sparkles on top of Blitz logo
static float sparkle_xy[][2] =							// [# of objs][res][x, y]
{
	{ -75.0f,171.0f},
	{ -65.0f,165.0f},
	{ -58.0f,157.0f},
	{ -42.0f,143.0f},
	{ -37.0f,139.0f},
	{ -33.0f,134.0f},
	{ -28.0f,129.0f},
	{ -20.0f,123.0f},
	{  -7.0f,116.0f},
	{  -1.0f,112.0f},

	{  75.0f,171.0f},
	{  65.0f,165.0f},
	{  58.0f,157.0f},
	{  42.0f,143.0f},
	{  37.0f,139.0f},
	{  33.0f,134.0f},
	{  28.0f,129.0f},
	{  20.0f,123.0f},
	{ -15.0f,173.0f},
	{  10.0f,172.0f},

};

#ifdef TEST_SCREENS
static tmu_ram_texture_t color_bars_textures_tbl[] =
{
	{"colbar00.wms",BARS_PG_TID},
	{"colbar01.wms",BARS_PG_TID},
	{"colbar02.wms",BARS_PG_TID},
	{"colbar03.wms",BARS_PG_TID},
	{"colbar04.wms",BARS_PG_TID},
	{NULL,0}
};
#endif

#if 0
static image_info_t *stat_flsh_imgs[] =
{
	&flshbx01,&flshbx01,&flshbx02,&flshbx03,&flshbx04,&flshbx05,
	&flshbx06,&flshbx07,&flshbx08,&flshbx09,&flshbx10
};
#endif

#if 0
static image_info_t *mug_flsh_imgs[] =
{
	&mflash01,&mflash01,&mflash02,&mflash03,&mflash04,&mflash05,
	&mflash06,&mflash07,&mflash08,&mflash09,&mflash10
};
#endif

//static image_info_t *sparkle_imgs[] =
//{
//	&sparka01,&sparka01,&sparka02,&sparka03,&sparka04,&sparka05,
//	&sparka06,&sparka07,&sparka08,&sparka09,&sparka10
//};

image_info_t *sparkle_imgs[10] =
{
	&sparkc01,&sparkc02,&sparkc03,&sparkc04,&sparkc05,
	&sparkc06,&sparkc07,&sparkc08,&sparkc09,&sparkc10
};

//static image_info_t *flash_imgs[] =
//{
//	&flashb01,&flashb01,&flashb02,&flashb03,&flashb04,&flashb05,
//	&flashb06,&flashb07,&flashb08,&flashb09,&flashb10
//};

static tmu_ram_texture_t hiscore_textures_tbl[] =
{
// {"hiscor09.wms",HISCORE_PG_TID},
// {"hiscor10.wms",HISCORE_PG_TID},
// {"hiscor11.wms",HISCORE_PG_TID},
// {"bkgnd2.wms",HISCORE_PG_TID},
 {"hiscr00.wms",HISCORE_PG_TID},
 {"hiscr01.wms",HISCORE_PG_TID},
// {"hiscr02.wms",HISCORE_PG_TID},
// {"hiscr03.wms",HISCORE_PG_TID},
 {NULL,0}
};

static char *world_record_titles[] = {
		"MOST SACKS",
		"MOST TOUCHDOWNS",
		"MOST TACKLES",
		"MOST INTERCEPTIONS",
		"MOST POINTS",
		"LARGEST VICTORY MARGIN" };

static float wrld_rec_xs[NUM_WORLD_RECORDS][2] = {  // [world rec][2plr/4plr]
						{118.0f,118.0f},
						{118.0f,118.0f},
						{118.0f,118.0f},
						{374.0f,374.0f},
						{374.0f,374.0f},
						{374.0f,374.0f} };
		
static float wrld_rec_title_ys[NUM_WORLD_RECORDS][2] = {  // [world rec][2plr/4plr]
						{285.0f,285.0f},
						{200.0f,200.0f},
						{115.0f,115.0f},
						{285.0f,285.0f},
						{200.0f,200.0f},
						{115.0f,115.0f} };

static float wrld_rec_ys[NUM_WORLD_RECORDS][2] = {  // [world rec][2plr/4plr]
						{260.0f,260.0f},
						{175.0f,175.0f},
						{90.0f,90.0f},
						{260.0f,260.0f},
						{175.0f,175.0f},
						{90.0f,90.0f} };

static float hiscr_ys[HI_SCORE_ENTRIES][2] = {  // [y][2plr/4plr]
						{305.0f,305.0f},
						{273.0f,273.0f},
						{241.0f,241.0f},
						{209.0f,209.0f},
						{177.0f,177.0f},
						{145.0f,145.0f},
						{113.0f,113.0f},
						{ 81.0f, 81.0f},
						{ 49.0f, 49.0f},
						{ 17.0f, 17.0f} };


static char *hi_scr_subhdrs[] = {
		"(DEFEATED ALL 31 NFL TEAMS)",
		"(BEST WIN PERCENTAGE)",
		"(MOST GAMES WON)",
		"(POINTS SCORED PER GAME)",
		"(POINTS ALLOWED PER GAME)",
		"(YARDS PER GAME)",
		"(YARDS PER GAME)",
		"(CONSECUTIVE WINS)",
		"(MOST GAMES PLAYED)",
		"(BEST OVERALL QUARTERBACK)",
		"(BEST SINGLE GAME RECORDS)",
		"(MOST TRIVIA POINTS)",
		"(BASED ON THIS LOCATION)" };

static float sub_hdr_xys[2][2] = { {256.0f,330.0f}, {256.0f,330.0f} };

static image_info_t *hiscr_hdr_imgs[] = {
										 &hs_grndch,&hs_grndch_c1,
										 &hs_grtplr,&hs_grtplr_c1,
										 &hs_bigwin,&hs_bigwin_c1,
										 &hs_bstoff,&hs_bstoff_c1,
										 &hs_bstdef,&hs_bstdef_c1,
										 &hs_bstrus,&hs_bstrus_c1,
										 &hs_bstpas,&hs_bstpas_c1,
										 &hs_curwin,&hs_curwin_c1,
										 &hs_expplr,&hs_expplr_c1,
										 &hs_qbrate,&hs_qbrate_c1,
										 &hs_wrldrc,&hs_wrldrc_c1,
										 &hs_trivia,&hs_trivia_c1,
										 &hs_poptms,&hs_poptms_c1 };


//static image_info_t *nfl_logo_imgs[] =
//{
//	&nfl37,&nfl81,&nfl80,&nfl79,&nfl78,&nfl77,&nfl76,
//	&nfl75,&nfl74,&nfl73,&nfl72,&nfl71,&nfl70,&nfl69,&nfl68,
//	&nfl67,&nfl66,&nfl65,&nfl64,&nfl63,&nfl62,&nfl61,&nfl60,
//	&nfl59,&nfl58,&nfl57,&nfl56,&nfl55,&nfl54,&nfl53,&nfl52,
//	&nfl51,&nfl50,&nfl49,&nfl48,&nfl47,&nfl46,&nfl45,&nfl44,
//	&nfl43,&nfl42,&nfl41,&nfl40,&nfl39,&nfl38
//};
	
/*
static float colors[16] = {			 
//	255.0f,	255.0f,	0.0f,	0.0f,
//	255.0f,	128.0f,	0.0f,	0.0f, 
//	255.0f,	0.0f,	255.0f,	0.0f,
//	255.0f,	255.0f,	0.0f,	0.0f
	255.0f,	127.0f, 	0.0f,	0.0f,
	217.0f,	 64.0f,  	0.0f,	0.0f,
	127.0f,	  0.0f,	127.0f,	0.0f,
	127.0f,	 64.0f,   0.0f,	0.0f
};
*/

static int pop_team_sort(const void *a, const void *b)
{
	popular_team_info_t	*a1;
	popular_team_info_t	*b1;

	a1 = (popular_team_info_t *)a;
	b1 = (popular_team_info_t *)b;

	return(b1->times_choosen - a1->times_choosen);
//	return(a1->times_choosen - b1->times_choosen);
}

#ifdef STICK_HACK
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void stick_crap(int min_time, int max_time)
{
	int cursw = 12345678;
	int cur49 = 12345678;
	int oldsw;
	int old49;
	char msg[200];

	set_string_id(5);

	while (--min_time) {
		cursw = get_player_sw_current();
		cur49 = get_player_49way_current();
		if (cursw != oldsw || cur49 != old49) {
			sprintf(msg,"cursw %08x   P1-cur49 %02i   P2-cur49 %02i\n",
				cursw,cur49&255,(cur49>>8)&255);

			printf("%s",msg);

			delete_multiple_strings(5, 0xffffffff);
			set_text_position(SPRITE_HRES/2, SPRITE_VRES/2+40.0F, 6.0F);
			set_text_font(FONTID_BAST10);

			oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, msg);
		}
		oldsw = cursw;
		old49 = cur49;
		sleep (1);
	}

	delete_multiple_strings(5, 0xffffffff);
}
#endif

#ifdef TIME_PROC
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void time_proc(int *args)
{
	struct time	tm;
	ostring_t		*str_ptr = (ostring_t *)0;

	tm.ti_sec = 0;
	tm.ti_min = 0;
	tm.ti_hour = 0;
	while(1)
	{
		if(str_ptr)
		{
			delete_string(str_ptr);
		}
		set_text_position(10, 20, 1);
		set_text_justification_mode(HJUST_LEFT|VJUST_BOTTOM);
		set_text_color(WHITE);
		set_text_id(0x01234567);
		set_text_font(FONTID_BAST7T);
		str_ptr = oprintf("%02d:%02d:%02d", tm.ti_hour, tm.ti_min, tm.ti_sec);

		sleep(57);

		++tm.ti_sec;
		if(tm.ti_sec > 59)
		{
			tm.ti_sec = 0;
			tm.ti_min++;
		}
		if(tm.ti_min > 59)
		{
			tm.ti_min = 0;
			tm.ti_hour++;
		}
	}
}
#endif

//---------------------------------------------------------------------------------------------------------------------------
// 			THIS is the PROCESS that CONTROLS the ATTRACT MODE
//
// INPUT: none
// 
// called from MAIN.C
//---------------------------------------------------------------------------------------------------------------------------
void attract_loop(int *args)
{
	unsigned int	start_addr;
	unsigned int	num_bytes;
	unsigned long	crc32;
	void * att_next;
	float ticks_per_second;
	int count, fn;
	int joy, joy1, joy2, joy3, joy4;
// texture_debug = 1;

	// Set the game state
#ifdef TIME_PROC
	qcreate("time", 0x8000, time_proc, 0, 0, 0, 0);
#endif

	if (!existp(BACKDOOR_PID,0xFFFFFFFF)) {
		// Create the backdoor to the diagnostics
		qcreate("bdoor", BACKDOOR_PID, backdoor_proc, 0, 0, 0, 0);
	}

	wipeout();

	// Make start buttons active
	start_enable(TRUE);

	/* set attract mode master volume level */
	snd_master_volume(game_info.attract_mode_volume);
	/* potentially disable attract mode sounds */
	get_adjustment(ATTRACT_SOUND_ADJ, &fn);
	snd_bank_lockout(!fn);

	count = snd_get_first_sound(cursor_bnk_str);

	tick_sndn = count + TICK_SND;
	coin_snd_drop_calln = count + COIN_SND_DROP_CALL;
	coin_snd_paid_calln = count + COIN_SND_PAID_CALL;

	// Set game state
	game_info.game_state = GS_ATTRACT_MODE;

// snd bank nedded at game start is loaded at game_over in nfl.c
// load in the needed sounds
//#ifndef NOAUDIO
//	load_permanent_snds();
//#endif

	// Initialize the attract mode switch handlers
#ifdef DEBUG
	printf("ATTRACT mode started with game_state = %d \n", game_info.game_state);
#endif

	credit_pg_on = 0;
	count = 0;
	att_keeper = 0;
	ticks_per_second = (float)get_tsec();
	draw_enable(1);

	// Are we supposed to show the hiscore tables first
	if(show_them_hiscore_tables_first)
	{
		// YES
		show_hiscore_tables(0, 0);
	}

	while(game_info.game_state == GS_ATTRACT_MODE)
	{
		// free up any heap that's sitting on the free process list
		clear_free_process_list();

		// Generate the crc for the executable in RAM
		start_addr = get_text_addr();
		num_bytes = get_text_end_addr();
		num_bytes -= start_addr;
		crc32 = crc((unsigned char *)start_addr, num_bytes);
		if(crc32 != start_crc32)
		{
#ifdef DEBUG
			fprintf(stderr, "Text section is trashed\r\n");
			lockup();
#else
			int	restart_args[] = {0, 0, 0, 0};

			increment_audit(RESTART_AUD);
			increment_audit(WATCHDOG_AUDIT_NUM);

#ifdef USE_DISK_CMOS
			write_cmos_to_disk(FALSE);
			update_other_cmos_to_disk(FALSE);
#endif
			exec("game.exe", 0xdeadbeef, restart_args);
#endif
		}

#ifdef USE_DISK_CMOS
		write_cmos_to_disk(TRUE);
		update_other_cmos_to_disk(TRUE);
#endif

#ifndef TEST_EXEC
		// Loop through all of the attract mode screens
		for (fn = 0; fn < ATT_FUNC_CNT; fn++)
		{
			att_next = att_funcs[(fn == (ATT_FUNC_CNT-1)) ? 0 : (fn+1)].func;

			// only do if in attract mode
			if (game_info.game_state != GS_ATTRACT_MODE)
				break;
		
			// Check to see if it's time to draw this screen
			if (!(count % att_funcs[fn].loop_count))
			{
#ifndef TEX_LOCK
#if 0
				// Reset the texturing memory
				grTexMemReset();
#endif
#endif
				att_keeper = (int)(att_next == do_amode_play);

				// Call the function to draw this screen
				//fprintf(stderr,"Func %d/%d   att_keeper %d where att_next was %X & do_amode_play is %X\r\n",
				//		fn,ATT_FUNC_CNT,att_keeper,(int)att_next,(int)do_amode_play);
				att_funcs[fn].func(
					(int)(ticks_per_second * att_funcs[fn].min_time),
					(int)(ticks_per_second * 
					(att_funcs[fn].timeout - att_funcs[fn].min_time)));
					
				// if credit page is up...wait until off before goto next page
				while (credit_pg_on)
					sleep(2);
// Sweeps Check!
//fprintf(stderr, "Check Sweeps!\n");
				if ((get_joy_val_cur(0) & JOY_DOWN) ||
					(get_joy_val_cur(1) & JOY_DOWN) ||
					(get_joy_val_cur(2) & JOY_DOWN) ||
					(get_joy_val_cur(3) & JOY_DOWN))
				{
//fprintf(stderr, "Sweeps triggered!\n");
					qcreate( "sweeps", AMODE_GRP_PID, sweeps_proc, 0, 0, 0, 0 );			
					die (0);
				}
// End sweeps check

			}
		}

//fprintf(stderr, "ATTRACT> count = %d fn = %d\n", count, fn);
		count++;

#else //TEST_EXEC
		fprintf(stderr,"--> Setting count = %d to do an EXEC\r\n",LOOPS_UNTIL_REBOOT);
		count = LOOPS_UNTIL_REBOOT;
		sleep(1);
#endif //TEST_EXEC

#if defined(VEGAS)
		// Check game select & game lock DIPs
		if (((get_dip_coin_current() & DIP_BIT_6) == 0 ||	// 0 if no lock, 1 if lock
			(get_dip_coin_current() & DIP_BIT_7) == 1) &&	// 0 if NFL, 1 if NBA
			(game_info.game_state == GS_ATTRACT_MODE))
		{

			if (count == LOOPS_UNTIL_REBOOT)
			{
				// REBOOT PLEASE!!!!
				#ifdef DEBUG
					fprintf( stderr,"ATTRACT> REBOOT! count = %d of %d\n", count, LOOPS_UNTIL_REBOOT);
					if (game_info.game_state != GS_ATTRACT_MODE)
						fprintf(stderr, "ATTRACT> YOU FUCKING DUMBASS!\n");
				//	while(1) sleep(1);
				#endif

				{
					int sargs[4] = { 2, 0, 0, 0};

#ifdef USE_DISK_CMOS
					write_cmos_to_disk(FALSE);
					update_other_cmos_to_disk(FALSE);
#endif
					set_coin_hold(TRUE);
					snd_master_volume(0);
					setdisk(0);				// Set NBA Partition
					write_game_info();		// Write out valuable info
					exec("GAME.EXE", 0xfeedbeef, sargs);
				}
			}
		}
#endif
	}
}

//---------------------------------------------------------------------------------------------------------------------------
// draw_bobj_backgrnd2() -
// 
//---------------------------------------------------------------------------------------------------------------------------
void draw_bobj_backgrnd2(float z, int tid)
{
	sprite_info_t *bkgrnd_obj;
	bkgrnd_obj = beginbobj(&hiscor2,    0.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginbobj(&hiscor2_c1, 0.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginbobj(&hiscor2_c2, 0.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginbobj(&hiscor2_c3, 0.0F, SPRITE_VRES, z, tid);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
		
//	bkgrnd_obj = beginbobj(&selbck2, 0.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);
//	bkgrnd_obj = beginbobj(&selbck2, 154.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);
//	bkgrnd_obj = beginbobj(&selbck2, 308.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);
//	bkgrnd_obj = beginbobj(&selbck2, 462.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);

//	bkgrnd_obj = beginbobj(&selbck2_c1, -1.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);
//	bkgrnd_obj = beginbobj(&selbck2_c1, 153.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);
//	bkgrnd_obj = beginbobj(&selbck2_c1, 307.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);
//	bkgrnd_obj = beginbobj(&selbck2_c1, 461.0F, SPRITE_VRES, z, tid);
//	bkgrnd_obj->w_scale = 1.203125F;
//	bkgrnd_obj->h_scale = 1.203125F;
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
//	generate_sprite_verts(bkgrnd_obj);
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void coin_credits_exit_func(process_node_t *, int);

void credit_page(int min_time, int max_time)
{
	int prev_cred;
	sprite_info_t *bkgrnd_obj;
	
//	stream_stop();
//	do { sleep (1); } while (stream_check_active());
//	stream_release_key();

	wipeout();

#if defined(SEATTLE)
	snd_bank_delete("intro1");
	snd_bank_delete("mdwylogo");
	snd_bank_delete("movie1");
#endif
	
	if (rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}

	// create the insert coins message proc
	coin_insert_proc = qcreate("coinmesg",ZERO_PID,coin_show_insert_proc,0,0,0,0);

	// create the show player credits message proc
	coin_credits_proc = qcreate("credmesg",ZERO_PID,coin_show_credits_proc,0,0,0,0);
	coin_credits_proc->process_exit = coin_credits_exit_func;

	// let procs do disk loads so fade won't be interrupted or text won't
	//  appear after background
	sleep(1);

	draw_enable(1);

	credit_pg_on = 1;

	bkgrnd_obj = beginobj(&coin,    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&coin_c1, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&coin_c2, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bkgrnd_obj = beginobj(&coin_c3, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
//	bkgrnd_obj = beginobj(&cheerpg1,    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
//	bkgrnd_obj = beginobj(&cheerpg1_c1, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
//	bkgrnd_obj = beginobj(&cheerpg1_c2, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
//	bkgrnd_obj = beginobj(&cheerpg1_c3, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	
	fade(1.0, 10, NULL);
	
	do
	{
		prev_cred = get_total_credits();

		wait_for_any_butn_press(76, min_time);
//		sleep (min_time);

	}	while (prev_cred != get_total_credits());
		
	// clean_up:
	delete_all_sprites();

	kill(coin_insert_proc,0);
	kill(coin_credits_proc,0);
	delete_multiple_strings(5, 0xffffffff);

	// delete background
	del1c(0,0xFFFFFFFF);
//	del1c(1,0xFFFFFFFF);
//	delete_all_background_sprites();

	credit_pg_on = 0;
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
extern int texture_load_in_progress;

void credit_page_proc( int *args)
{
	if (game_info.game_state == GS_ATTRACT_MODE && !credit_pg_on)
	{
		credit_pg_on = 1;

		start_enable(TRUE);		// Problem during rebooting

		// Make sure <game_proc> finishes loads &| dies
		if (existp(GAME_PROC_PID, 0xFFFFFFFF) && texture_load_in_progress)
			game_info.attract_coin_in = 1;

		// Make sure the movie player is shut down if it is running
		if(movie_get_state() > 1)
		{
			movie_abort();
		}

		// Don't kill any disk loads
		while (texture_load_in_progress) sleep(1);
		// Make sure the disk is in normal mode
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif
		//sleep(1);

		// Make sure the we fade back to normal if we came here from
		// a screen that faded.
		normal_screen();

		// Make sure any transitions go away
		delete_process_id(TRANSIT_PID);
		kill_plates();

		// Show the page
		credit_page(12*get_tsec(),0);

		// Restart attract mode
		iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
	}
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//void nfl_legal_screen(int min_time, int max_time)
//{
//	set_string_id(5);
//	set_text_position(HRESf/2, VRESf/2+20.0F, 6.0F);
//	set_text_font(FONTID_BAST10);
//	oprintf("%dj%dc LEGAL SCREEN", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
//	sleep (min_time);
//	delete_multiple_strings(5, 0xffffffff);
//}

#ifdef TEST_SCREENS
//-------------------------------------------------------------------------------------------------
//  		This process displays the color bars & cross hatch pattern
//-------------------------------------------------------------------------------------------------
void color_bars_page(int min_time, int max_time)
{

	#define	PASS_BUTTON		1		// red
	#define	BLOCK_BUTTON	2		// blue
	#define	TURBO_BUTTON	4		// white

	float		bars_z;
	sprite_info_t *bkgrnd_obj;

	wipeout();

	if (load_textures_into_tmu(color_bars_textures_tbl)) 
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all COLOR BAR textures into tmu\r\n");
		lockup();
#else
		return;
#endif
	}
	lock_multiple_textures(BARS_PG_TID,0xFFFFFFFF);

	bars_z = 5.0F;

	do
	{
		bkgrnd_obj = beginobj(&bars, 0, SPRITE_VRES, bars_z, BARS_PG_TID);
		bkgrnd_obj = beginobj(&bars_c1, 0, SPRITE_VRES, bars_z, BARS_PG_TID);
		bkgrnd_obj = beginobj(&bars_c2, 0, SPRITE_VRES, bars_z, BARS_PG_TID);
		bkgrnd_obj = beginobj(&bars_c3, 0, SPRITE_VRES, bars_z, BARS_PG_TID);

		wait_for_any_butn_press(20,tsec*6);
		delete_all_sprites();

		bkgrnd_obj = beginobj(&hatch,		0, SPRITE_VRES, bars_z+1, BARS_PG_TID);
		bkgrnd_obj = beginobj(&hatch_c1, 0, SPRITE_VRES, bars_z+1, BARS_PG_TID);
		bkgrnd_obj = beginobj(&hatch,		256, SPRITE_VRES, bars_z+1, BARS_PG_TID);
		bkgrnd_obj = beginobj(&hatch_c1, 256, SPRITE_VRES, bars_z+1, BARS_PG_TID);

		wait_for_any_butn_press(20,tsec*6);
		delete_all_sprites();

	}	while (get_but_val_cur(1) == TURBO_BUTTON);


	unlock_multiple_textures(BARS_PG_TID,0xFFFFFFFF);
	delete_multiple_textures(BARS_PG_TID,0xFFFFFFFF);

}
#endif

//---------------------------------------------------------------------------------------------------------------------------
// Wait for any butn press of an active player on a team
// Used for player specific bozo boxes
// Checks to make sure guy is in game also
//---------------------------------------------------------------------------------------------------------------------------
void wait_for_any_butn_press_active_team(int min, int max, int pnum)
{
	int ticks = 0, done = FALSE;

	do
	{
		sleep(1);
		ticks++;
		if (pnum == 0)
		{
			if ((p_status & 0x1) && (get_player_sw_current() & (P1_A|P1_B|P1_C)) && (ticks >= min))
				done = TRUE;
			if (four_plr_ver)
			{
				if ((p_status & 0x2) && (get_player_sw_current() & (P2_A|P2_B|P2_C)) && (ticks >= min))
					done = TRUE;
			}
		}
		else if (pnum == 1)
		{
			if (four_plr_ver)
			{
				if ((p_status & 0x4) &&	(get_player_sw_current() & (P3_A|P3_B|P3_C)) && (ticks >= min))
					done = TRUE;
				if ((p_status & 0x8) &&	(get_player_sw_current() & (P4_A|P4_B|P4_C)) && (ticks >= min))
					done = TRUE;
			}
			else
			{
				if ((p_status & 0x2) && (get_player_sw_current() & (P2_A|P2_B|P2_C)) && (ticks >= min))
					done = TRUE;
			}
		}
	} while (ticks < max && !done);
}

void wait_for_any_butn_press_active_plyr(int min, int max, int pnum)
{
	int ticks = 0, done = FALSE;

	do
	{
		sleep(1);
		ticks++;

		if ((p_status & (1<<pnum)) && ((get_player_sw_current()>>(8*pnum)) & (P1_A|P1_B|P1_C)) && (ticks >= min))
			done = TRUE;

	} while (ticks < max && !done);
}

#ifdef DBG_GC
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void test_grand_champ_movie(int min_time, int max_time)
{
	int msk = 0;

	while (get_player_sw_current() & ALL_ABCD_MASK)
	{
		wipeout();

		if (plates_on == PLATES_ON || plates_on == PLATES_COMING_ON)
			create_plates();

		if (!msk)
			msk = 0x00c38421;	//1100 0011 1000 0100 0010 0001

		do_grand_champ = msk & 0xf;

		show_grand_champ_movie(min_time, max_time);

		msk >>= 4;

		//draw_enable(1);
		//
		//// Only remove plates if they're there (should be!)
		//if (plates_on == PLATES_ON)
		//{
		//	// Take away the plates
		//	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		//
		//	// Wait for plates
		//	sleep(PLATE_TRANSITION_TICKS+1);
		//
		//	// Make sure plates are gone
		//	plates_on = PLATES_OFF;
		//}
	}
	do_grand_champ = 0;
}
#endif

#if 0
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void show_grand_champ_movie(int min_time, int max_time)
{

	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 2.0f, 30);
	qcreate("mplay", 0, movie_proc, (int)&gc_movie, 0, 0, 0);

	fade(1.0, 20, NULL);

	// Make 'em watch it for at least 1/2 second
	sleep(30);

	// As long as the movie is playing and the player has NOT pressed a button
	// do nothing
	while((movie_get_state() > 1)
		&& !(get_player_sw_current() & ALL_ABCD_MASK)
		&& !(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW))
		)
	{
		sleep(1);
	}

	// Fade the last frame shown
	fade(0.0, 5, NULL);
	sleep(5);

	// Make a beep if aborted
	if(movie_get_state() > 1)
	{
		snd_scall_direct(tick_sndn,VOLUME1,127,1);
	}

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
	{
		movie_abort();
	}
}
#endif

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void show_grand_champ_movie(int min_time, int max_time)
{
	register union IFCV * script;
	texture_node_t * ptn;
	sprite_node_t * slist;
	sprite_info_t * psi;
	sprite_info_t ** ppsi;
	image_info_t ** ppii;
	float fp;
	float fx = 0;
	float fy = 0;
	int cur_oid = 0;
	int fid = 0;
	int sid = 0;
	int tid = -1;
	int fid23, fid75;
	int icur, inxt, inc;
	int i = 0, j = 0, p, q;
	char ** ppsz;
	char sz_nms[LETTERS_IN_NAME*2+3+1];	// +3 for " & ", +1 for null terminator

	// Make sure fonts are available (at least something!)
	fid23 = FONTID_BAST23SCALE;
	fid75 = FONTID_BAST75;
	if (!create_font(&bast23scale_font, fid23)) fid23 = FONTID_BAST25;
	if (!create_font(&bast75_font, fid75)) fid75 = FONTID_BAST25;

	// Setup text
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_text_z_inc(-0.1f);

	script = gc_script;
	while ((icur = script->i) != SCRIPTGC)
	{
		script++;
		inxt = script->i;
		inc = 1;

		switch (icur)
		{
			case LOADTEX:											// tid,v:*
				ppsz = script[1].v;
				while (*ppsz != NULL)
				{
					ptn = create_texture(*ppsz, inxt, 0,
						CREATE_NORMAL_TEXTURE,
						GR_TEXTURECLAMP_CLAMP,
						GR_TEXTURECLAMP_CLAMP,
						GR_TEXTUREFILTER_BILINEAR,
						GR_TEXTUREFILTER_BILINEAR);

					if (ptn == NULL)
					{
						increment_audit(TEX_LOAD_FAIL_AUD);
					}
					ppsz++;
				}
				inc = 2;
				break;

			case LOCKTEX:											// tid
				lock_multiple_textures(inxt, -1);
				break;

			case KILLTEX:											// tid
				unlock_multiple_textures(inxt, -1);
				delete_multiple_textures(inxt, -1);
				break;


			case LOADBNK:											// c:*
#ifndef NOAUDIO
				// Disallow coin-in sounds while loading
				coin_sound_holdoff(TRUE);

				if (snd_bank_load_playing((char *)inxt) == 0xeeee)
					increment_audit(SND_LOAD_FAIL_AUD);

				// 0 is tune, 1 is audience, 5 is announcer
				snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
				coin_sound_holdoff(FALSE);
#endif
				break;

			case CALLSND:											// c:*,snd,vol,trk
#ifndef NOAUDIO
				snd_scall_bank((char *)inxt, script[1].i, script[2].i,
					127, script[3].i);
#endif
				inc = 4;
				break;

			case STOPSND:											// trk
#ifndef NOAUDIO
				snd_stop_track(inxt);
#endif
				break;

			case KILLBNK:											// c:*
#ifndef NOAUDIO
				snd_bank_delete((char *)inxt);
#endif
				break;


			case INITMOV:											// f:z,fps
				fp = script[0].f;
				i  = script[1].i;
				movie_config_player(512, 384, 0, 0, fp, i);
				inc = 2;
				break;

			case PLAYMOV:											// v:*,last
				i = script[1].i;
				qcreate("mplay", 0, movie_proc, inxt, 0, i, 0);
				inc = 2;
				break;


			case SETDRAW:											// flag
				draw_enable(inxt);
				break;

			case SETFADE:											// f:%,fcnt
				fp = script[0].f;
				i  = script[1].i;
				fade(fp, i, NULL);
				inc = 2;
				break;

			case DOTRANS:											// flag
				if ((!inxt && plates_on == PLATES_ON) ||
					( inxt && plates_on == PLATES_OFF))
				{
					iqcreate("transit", TRANSIT_PID, transit_proc,
						PLATE_TRANSITION_TICKS, 0, 0, 0);
				}
				break;


			case TBL_OBJ:											// tid,v:*,f:z
				ppii = script[1].v;
				fp = script[2].f;
				while ((int)(*ppii) != (int)-1)
				{
					if ((int)(*ppii) >= (int)0)
					{
						i = (int)(*ppii++);
						continue;
					}
					psi = beginobj(*ppii, 0, 0, fp + (float)i, inxt);
					if (psi)
					{
						if (psi->state.color_combiner_function ==
							GR_COLORCOMBINE_DECAL_TEXTURE)
						{
							change_sprite_alpha_state(psi, TRUE);
						}
						psi->id = i;
						set_veladd_mode(psi, TRUE);
						hide_sprite(psi);
					}
					ppii++;
				}
				inc = 3;
				break;

			case SHO_OBJ:											// bg#
				cur_oid = inxt;
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == cur_oid)
						 unhide_sprite(slist->si);
					slist = slist->next;
				}
				break;

			case HID_OBJ:											// bg#
				cur_oid = inxt;
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == cur_oid)
					{
						 hide_sprite(slist->si);
					}
					slist = slist->next;
				}
				break;

			case CUR_OBJ:											// bg#
				cur_oid = inxt;
				break;

			case POS_OBJ:											// f:x,f:y
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == cur_oid)
					{
						slist->si->x = script[0].f;
						slist->si->y = script[1].f;
					}
					slist = slist->next;
				}
				inc = 2;
				break;

			case VEL_OBJ:											// f:x,f:y
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == cur_oid)
					{
						slist->si->x_vel = script[0].f;
						slist->si->y_vel = script[1].f;
					}
					slist = slist->next;
				}
				inc = 2;
				break;


			case COL_STR:											// color
				set_text_color(inxt);
				break;

			case DEL_STR:											// id#
				delete_multiple_strings(inxt, -1);
				break;

			case FAD_OBJ:											// f:%,ticks
				inxt = cur_oid;
			case FAD_STR:											// f:%,ticks
				if (icur == FAD_STR)
					inxt = sid;
				fp= script[0].f;
				q = script[1].i;
				if (fp > 1.0f) fp = 1.0f;
				if (fp < 0.0f) fp = 0.0f;
				if (q < 0) q = 0;

				p = ((int)(255.0f * fp)) & 255;
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == inxt)
					{
						if (q)
						{
							i = (slist->si->state.constant_color >> 24) & 255;
							j = p - i;
							i *= q;
							*(int *)(&slist->si->x_angle_end) = (p << 16) + q;
							*(int *)(&slist->si->y_angle_end) = i;
							*(int *)(&slist->si->z_angle_end) = j;
						}
						else
							slist->si->state.constant_color =
								(slist->si->state.constant_color & 0xffffff) |
								(p << 24);
					}
					slist = slist->next;
				}
				inc = 2;
				break;

			case FON_STR:											// font
				if (inxt == FONTID_BAST23SCALE) fid = fid23;
				else
				if (inxt == FONTID_BAST75) fid = fid75;
				else
				fid = inxt;
				set_text_font(fid);
				break;

			case JUS_STR:											// justify
				set_text_justification_mode(inxt);
				break;

			case NAM_STR:											// c:*,f:yinc
				*sz_nms = '\0';
				i = 0;
				p = 0;
				q = do_grand_champ;
				while (q)
				{
					if (q & 1)
					{
						j = LETTERS_IN_NAME;
						while (--j >= 0)
							if (plyr_data_ptrs[p]->plyr_name[j] != ' ')
								break;

						if (j < 0)
							sprintf(sz_nms,"%s%s(PLR%d)",
								*sz_nms ? sz_nms:"",
								*sz_nms ?  " & ":"",
								p+1);
						else
						{
							register char c = plyr_data_ptrs[p]->plyr_name[++j];
							plyr_data_ptrs[p]->plyr_name[j] = '\0';
							sprintf(sz_nms,"%s%s%s",
								*sz_nms ? sz_nms:"",
								*sz_nms ?  " & ":"",
								plyr_data_ptrs[p]->plyr_name);
							plyr_data_ptrs[p]->plyr_name[j] = c;
						}

						// If i=2, q should be going 0, but ...
						if (++i == 2) break;
					}
					q >>= 1;
					p++;
				}
				set_string_id(sid);
				oprintf("%s%s", sz_nms, script[0].c);
				fy += script[1].f;
				set_text_position(fx, fy, 100.0f);
				inc = 2;
				break;

			case OUT_STR:											// c:*,f:yinc
				set_string_id(sid);
				oprintf(script[0].c);
				fy += script[1].f;
				set_text_position(fx, fy, 100.0f);
				inc = 2;
				break;

			case POS_STR:											// f:x,f:y
				fx = script[0].f;
				fy = script[1].f;
				set_text_position(fx, fy, 100.0f);
				inc = 2;
				break;

			case SID_STR:											// id#
				set_string_id(sid = inxt);
				break;

			case VEL_STR:											// f:xv,f:yv
				slist = alpha_sprite_node_list;
				while (slist)
				{
					if (slist->si->id == sid)
					{
						slist->si->x_vel = script[0].f;
						slist->si->y_vel = script[1].f;

						set_veladd_mode(slist->si, TRUE);
					}
					slist = slist->next;
				}
				inc = 2;
				break;


			default:
				while (icur)
				{
					slist = alpha_sprite_node_list;
					while (slist)
					{
						p = *(int *)(&slist->si->x_angle_end);
						if (p)
						{
							q = p & 65535;
							p = (p >> 16) & 255;
							i  = *(int *)(&slist->si->y_angle_end);
							i += *(int *)(&slist->si->z_angle_end);
							j = i / q;
							if (j == p)
							{
								*(int *)(&slist->si->x_angle_end) = 0;
								*(int *)(&slist->si->y_angle_end) = 0;
								*(int *)(&slist->si->z_angle_end) = 0;
							}
							else
								*(int *)(&slist->si->y_angle_end) = i;

							slist->si->state.constant_color =
								(slist->si->state.constant_color & 0xffffff) |
								(j << 24);
						}
						slist = slist->next;
					}
					i = plates_on;
					sleep(1);
					if (i == PLATES_COMING_ON && plates_on == PLATES_ON)
						snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
#ifdef DEBUG
	if (get_player_sw_close() & ALL_ABCD_MASK) break;
#endif
					icur--;
				}
				inc = 0;
		}
		script += inc;
	}

	// Make sure the movie player is shut down if it is running
	if (movie_get_state() > 1)
		movie_abort();

	// Clean out everything, remaking the plates it they were there
	wipeout();

	if (plates_on == PLATES_ON || plates_on == PLATES_COMING_ON)
		create_plates();
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//void test_grand_champ_movie(int min_time, int max_time)
//{
//	wipeout();
//
//	show_grand_champ_movie(min_time, max_time);
//
//	draw_enable(1);
//
//	// Only remove plates if they're there (should be!)
//	if (plates_on == PLATES_ON)
//	{
//		// Take away the plates
//		iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
//
//		// Wait for plates
//		sleep(PLATE_TRANSITION_TICKS+1);
//
//		// Make sure plates are gone
//		plates_on = PLATES_OFF;
//	}
//}
//
//void show_grand_champ_movie(int min_time, int max_time)
//{
//	ostring_t * pos;
//	ostring_t * postmp;
//	sprite_node_t * snl;
//	float yvel_msg;
//	int fid23, fid75;
//	int i, j, x, y;
//	int * msg;
//
//#ifndef NOAUDIO
//	// Disallow coin-in sounds while loading
//	coin_sound_holdoff(TRUE);
//
////	snd_report_banks();
//	if (snd_bank_load_playing("gameover") == 0xeeee)
//		increment_audit(SND_LOAD_FAIL_AUD);
////	snd_report_banks();
//
//	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
//	snd_set_reserved (SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
//
//	snd_scall_bank("gameover", 0, VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK0);
//
//	coin_sound_holdoff(FALSE);
//#endif
//
//	// Only remove plates if they're there (should be!)
//	if (plates_on == PLATES_ON)
//	{
//		// Take away the plates
//		iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
//
//		// Wait for plates
//		sleep(PLATE_TRANSITION_TICKS+1);
//
//		// Make sure plates are gone
//		plates_on = PLATES_OFF;
//	}
//
//	// Make 23 & 75 pt fonts available
//	fid23 = FONTID_BAST23SCALE;
//	fid75 = FONTID_BAST75;
//	if (!create_font(&bast23scale_font, fid23)) fid23 = FONTID_BAST25;
//	if (!create_font(&bast75_font, fid75)) fid75 = FONTID_BAST25;
////fprintf(stderr,"fonts in\r\n");
//
//	// Setup text
//	set_text_z_inc(-0.01f);
//	set_string_id(1);
//
//	msg = grand_champ_msg;
//	x = SPRITE_HRES / 2;
//	y = SPRITE_VRES / 2;
//	while (*msg != -1)
//	{
//		if (*msg == FONTID_BAST23SCALE) set_text_font(fid23);
//		else
//		if (*msg == FONTID_BAST75) set_text_font(fid75);
//		else
//		set_text_font(*msg);
//		msg++;
//
//		set_text_color(*msg++);
//
//		set_text_justification_mode(*msg++);
//
//		x += *msg++;
//		y += *msg++;
//
//		set_text_position(x, y, 4.0f);
//
//		pos = oprintf((char *)(*msg));
////fprintf(stderr,"%s\r\n",(char *)(*msg));
//
//		msg++;
//	}
////fprintf(stderr,"text done\r\n");
//
//	// Setup the movie
//	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 5.0f, 30);
//	qcreate("mplay", 0, movie_proc, (int)&end_movie, 0, PLATE_TRANSITION_TICKS, 0);
//
//	// Show some of the movie before scrolling text
//	i = 800;
//	do {
//		sleep(1);
//	} while (--i);
//
//	// Show the rest of the movie while scrolling text
//	yvel_msg = (is_low_res) ? 0.66666666f : 1.0f;
//	do {
//		snl = alpha_sprite_node_list;
//		if ((i < 460) || (i > 650 && i < 1525))
//		{
//			while (snl)
//			{
//				if (snl->si->parent)
//				{
//					snl->si->verts[0].y += yvel_msg;
//					snl->si->verts[1].y += yvel_msg;
//					snl->si->verts[2].y += yvel_msg;
//					snl->si->verts[3].y += yvel_msg;
//				}
//				snl = snl->next;
//			}
//		}
//		j = i - (1525 + 6 * 57);
//		if (j >= 0)
//		{
//			j = 255 - (j * 3);
//			if (j < 0) j = 0;
//
//			postmp = pos;
//			fade_string(postmp, j);
//			postmp = postmp->next;
//			fade_string(postmp, j);
//			postmp = postmp->next;
//			fade_string(postmp, j);
//			postmp = postmp->next;
//			fade_string(postmp, j);
//		}
//		sleep(1);
//		i++;
//	} while (movie_get_state() > MOVIE_STATE_IDLE && movie_get_state() != MOVIE_STATE_HOLD);
//
//	movie_pause();
//	sleep(1);
//
//	// Turn on the plates
//	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
//	// Wait for plates; set time-out so we don't hang
//	i = PLATE_TRANSITION_TICKS * 2;
//	// Do it this way so we can sync-up the sound
//	while (plates_on != PLATES_ON && --i) sleep(1);
//	// Do sound only if the transition finished, not the time-out
//	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
//
//	// Delete all the text
//	delete_multiple_strings(1, -1);
//
//	// Make sure the movie player is shut down if it is running
//	if(movie_get_state() > MOVIE_STATE_IDLE)
//	{
//		movie_abort();
//	}
//
//	sleep(20);
//
//	// Do the text
////	draw_bobj_backgrnd2(50.0f, HISCORE_PG_TID);
//
//#ifndef NOAUDIO
//	// Kill game over music
//	snd_stop_track(SND_TRACK_0);
//
//	snd_bank_delete("gameover");
//#endif
//
//	draw_enable(0);
//
//	// delete background
////	delete_all_background_sprites();
////	del1c(0,0xFFFFFFFF);
//}

//---------------------------------------------------------------------------------------------------------------------------
//	Test big font
//	
//	INPUT:	Nothing
//
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
//void show_big_font(int min_time, int max_time)
//{
//	draw_backdrop();
//
//	if(!create_font(&bast75_font, FONTID_BAST75))
//	{
//#ifdef DEBUG
//		lockup();
//#endif
//		exit(0);
//	}
//
//	if(!create_font(&bast23_font, FONTID_BAST23))
//	{
//#ifdef DEBUG
//		lockup();
//#endif
//		exit(0);
//	}
//
//	set_string_id(5);
//	set_text_font(FONTID_BAST75);
//
//	set_text_z_inc(-0.10F);
//
//	set_text_position(SPRITE_HRES/2,SPRITE_VRES/2+100, 10.0F);
//	oprintf("%dj%dc FUMBLE!", (HJUST_CENTER|VJUST_CENTER), 0);
//
//	set_text_position(SPRITE_HRES/2,SPRITE_VRES/2, 10.0F);
//	oprintf("%dj%dc IT'S GOOD!", (HJUST_CENTER|VJUST_CENTER), LT_RED);
//	
//	set_text_font(FONTID_BAST23);
//
//	set_text_position(SPRITE_HRES/2,100, 10.0F);
//	oprintf("%dj%dc ABCDEFGHIJKLMNOP'QRST", (HJUST_CENTER|VJUST_CENTER), LT_BLUE);
//	set_text_position(SPRITE_HRES/2,60, 10.0F);
//	oprintf("%dj%dc UVWXYZ!?.", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW);
//
//	wait_for_any_butn_press(1,tsec*6);
//
//	delete_multiple_strings(5, 0xffffffff);
//	delete_multiple_textures(FONTID_BAST75,0xFFFFFFFF);
//	delete_multiple_textures(FONTID_BAST23,0xFFFFFFFF);
//
//	// delete background
//	del1c(0,0xFFFFFFFF);
//	delete_all_background_sprites();
//
//	set_text_z_inc(0);
//}

//-------------------------------------------------------------------------------------------------
//  		This process displays any valid last-game final stats
//-------------------------------------------------------------------------------------------------
void show_stats_page(int min_time, int max_time)
{
	// No stats to show if a valid game wasn't completed
	if (game_info_last.game_quarter < 3)
		return;

	// Disable drawing
	draw_enable(0);

	wipeout();

	// Setup stats page
	team_stats_page();

	// Enable drawing
	draw_enable(1);

	black_screen();

	sleep(2);
	fade(1.0f, 30, 0);		//1=full on 30=ticks

	wait_for_any_butn_press(min_time,max_time);

	if (!att_keeper)
	{
		fade(0.0f, 30, 0);		//0=full off
		sleep(30);

		wipeout();

		sleep(1);
	}
}

//-------------------------------------------------------------------------------------------------
//  		This process displays the title page
//-------------------------------------------------------------------------------------------------
void title_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;
	int i;
	
	// Disable drawing
	draw_enable(0);

	// do END-OF-GAME clean up
	wipeout();

	load_title_flash(TRUE,TRUE);

	bkgrnd_obj = beginobj(&blitztit, 0, SPRITE_VRES, 5.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&blitztit_c1, 0, SPRITE_VRES, 5.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&blitztit_c2, 0, SPRITE_VRES, 5.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&blitztit_c3, 0, SPRITE_VRES, 5.0f, BARS_PG_TID);

	// Take care of proper handling of transition
	if (plates_on != PLATES_OFF)
	{
		// Re-create the wipeout'd plate objects
		create_plates();

		// Enable drawing since we know something is being or will be drawn
		draw_enable(1);

		if (plates_on != PLATES_ON)
		{
			int j = (plates_on == PLATES_COMING_ON) ? PLATES_ON : PLATES_OFF;
			// Wait for plates; set time-out so we don't hang
			i = PLATE_TRANSITION_TICKS * 2;
			// Do it this way so we can sync-up the sound
			while (plates_on != j && --i) sleep(1);
			// Do sound only if the transition finished, not the time-out
			if (i && j == PLATES_ON)
//				snd_scall_bank(plyr_bnk_str, P_HARD_HIT2_SND, VOLUME4, 127, PRIORITY_LVL5);
				snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
			// Make sure plates are on
			plates_on = j;
			sleep(1);
		}
		if (plates_on == PLATES_ON)
		{	
			// Turn off the plates
			iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		}
	}
	else
	{	
		// Enable drawing
		draw_enable(1);

		black_screen();

		sleep(2);
		fade(1.0f, 30, 0);		//1=full on 30=ticks
	}
	sleep(30);
	qcreate("flash", ROTATE_LOGO_PID, title_flash, 384, 132, 200, 0);

	wait_for_any_butn_press(min_time,max_time);

	killall(ROTATE_LOGO_PID, -1);

	if (!att_keeper)
	{
		fade(0.0f, 30, 0);		//0=full off
		sleep(30);

		delete_all_sprites();

		sleep(1);

		draw_enable(0);
	}
	free_title_flash();
}

//-------------------------------------------------------------------------------------------------
//  		This process displays the title page
//-------------------------------------------------------------------------------------------------
#define K_LITE	6								// # of times to flash on/off
#define K_LDLY	2								// ticks between flash on/off

#define K_ZOOM	40								// ticks for zoom
#define K_ZDLY	(K_ZOOM+(K_LITE*K_LDLY)+ 15)	// ticks to pause

#define K_FADE	24								// ticks for fade
#define K_FDLY	(K_FADE+ 10)					// ticks to pause

void new_feature_page(int min_time, int max_time)
{
//	image_info_t ** ppii = new_feature_imgs;
	sprite_info_t * psi_msg[2];
	int ccf_msg[2];
	int t_lite = 0;
	int t_zoom = 0;
	int t_fade = 0;
	int i, j, k;

	// put up background
	beginobj(&nf_bg,    0, SPRITE_VRES, 5.0f, BARS_PG_TID);
	beginobj(&nf_bg_c1, 0, SPRITE_VRES, 5.0f, BARS_PG_TID);
	beginobj(&nf_bg_c2, 0, SPRITE_VRES, 5.0f, BARS_PG_TID);
	beginobj(&nf_bg_c3, 0, SPRITE_VRES, 5.0f, BARS_PG_TID);

	fade(1.0f, 30, 0);		//1=full on 30=ticks
	normal_screen();
	sleep(30);


#if 1
	wait_for_any_butn_press(min_time,max_time);
#else	
	i = 0;
	while (1)
	{
		if (!t_zoom && !t_fade)
		{
			j = 2;
			while (j--)
			{
				psi_msg[j] = beginobj(*ppii,
					new_feature_xys[j][0],
					new_feature_xys[j][1],
					(float)(3 + j),
					BARS_PG_TID);
				psi_msg[j]->w_scale      = 1.5f;
				psi_msg[j]->h_scale      = 1.5f;
				psi_msg[j]->w_scale_end  = 1.0f;
				psi_msg[j]->h_scale_end  = 1.0f;
				psi_msg[j]->w_scale_vel  = (1.0f - 1.5f) / (float)K_ZOOM;
				psi_msg[j]->h_scale_vel  = (1.0f - 1.5f) / (float)K_ZOOM;
				psi_msg[j]->notify_modes = NOTIFY_W_SCALE|NOTIFY_H_SCALE;
				set_veladd_mode(psi_msg[j], TRUE);
				ccf_msg[j] = psi_msg[j]->state.color_combiner_function;
				ppii++;
			}
			t_lite = K_LITE;
			t_zoom = K_ZDLY;
			t_fade = K_FDLY;
		}

		sleep(1);

		if (++i > min_time && (get_player_sw_current() & ALL_ABCD_MASK))
			break;

		if (t_zoom)
		{
			t_zoom--;

			if (t_lite)
			{
				k = t_zoom - (K_ZDLY - K_ZOOM);
				if (k <= 0 && !(k % K_LDLY))
				{
					j = 2;
					while (j--)
					{
						psi_msg[j]->state.color_combiner_function =
							(t_lite & 1) ?
								ccf_msg[j] : GR_COLORCOMBINE_ONE;
						psi_msg[j]->state.chroma_key_mode =
							(t_lite & 1) ?
								GR_CHROMAKEY_DISABLE : GR_CHROMAKEY_ENABLE;
					}
					t_lite--;
				}
			}
			continue;
		}

		t_fade--;

		if (!ppii[0])
		{
			if (t_fade)
				continue;
			else
				break;
		}

		k = t_fade - (K_FDLY - K_FADE);
		if (k >= 0)
		{
			j = 2;
			while (j--)
				psi_msg[j]->state.constant_color =
					(psi_msg[j]->state.constant_color & 0xffffff) |
					(((k * 255) / K_FADE) << 24); 
		}

		if (!t_fade)
		{
			j = 2;
			while (j--)
				delobj(psi_msg[j]);

		}
	}
#endif
	fade(0.0f, 30, 0);		//0=full off
	sleep(30);

	delete_all_sprites();
}

#if 0	// NO deal with trading card people!
//-------------------------------------------------------------------------------------------------
//  		This process displays the Pacific Trading card logo
//-------------------------------------------------------------------------------------------------
void trading_card_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;

	wipeout();
	
	bkgrnd_obj = beginobj(&pacific,    0, SPRITE_VRES, 50.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&pacific_c1, 0, SPRITE_VRES, 50.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&pacific_c2, 0, SPRITE_VRES, 50.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&pacific_c3, 0, SPRITE_VRES, 50.0f, BARS_PG_TID);

//	set_string_id(5);
//	set_text_font(FONTID_BAST10);
//	set_text_z_inc(0.0f);
	
//	if(is_low_res)
//		set_text_position(SPRITE_HRES/2,80-3+1, 2.0F);
//	else
//		set_text_position(SPRITE_HRES/2,80-3, 2.0F);
//	oprintf("%dj%dcWWW.PACIFICTRADINGCARDS.COM", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2,66-3, 2.0F);
//	oprintf("%dj%dcPACIFIC AND THE CROWN LOGO ARE TRADEMARKS OF", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2,52-3, 2.0F);
//	oprintf("%dj%dcPACIFIC TRADING CARDS, INC.", (HJUST_CENTER|VJUST_CENTER), WHITE);

	black_screen();

	sleep(2);
	fade(1.0f, 30, 0);		//1=full on 20=ticks

	wait_for_any_butn_press(min_time,max_time);

	fade(0.0f, 30, 0);		//0=full off
	sleep(30);

	delete_multiple_strings(5, 0xffffffff);
	delete_all_sprites();
}
#endif

#ifdef JTEST
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#include "geertest.out"

void draw_test( void *oi );

#ifdef SEATTLE
GrMipMapId_t	test_textures[12];
#else
Texture_node_t	*test_textures[12];
#endif
int				test_ratios[12];
int				trotate = 0;

void jason_screen(int min_time, int max_time)
{
	int			i;

	wipeout();

	load_textures( geertest_textures, test_ratios, test_textures, 12, TXID_PLAYER,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );

	for( i = 0; i < 12; i++ )
		fprintf( stderr, "%ld\t%d\n", test_textures[i], test_ratios[i] );

	create_object( "jtest", OID_PLAYER, OF_NONE, DRAWORDER_PLAYER, NULL, draw_test );

	sleep( 600 );

	wait_for_any_butn_press(min_time,max_time);
}

void draw_test( void *oi )
{
	float	cm[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, -8.0f,
						0.0f, 0.0f, 1.0f, 30.0f };

	float	dm[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f };

	float	fm[12];

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_LESS );
	grDepthMask( FXTRUE );
	grDepthBiasLevel( 0 );
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );
	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	trotate += 1;

	roty( trotate, dm );

	mxm( cm, dm, fm );

	render_limb_noclip( &limb_geertest, fm, test_textures, test_ratios );
}
#endif

//-------------------------------------------------------------------------------------------------
//  		This process displays the 3DFX logo
//-------------------------------------------------------------------------------------------------
void logo_3DFX_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;

	wipeout();
	
//	coaching_tip_scrn();
//	wait_for_any_butn_press(20,tsec*1114);

	bkgrnd_obj = beginobj(&logo3dfx, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&logo3dfx_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	if(is_low_res)
		bkgrnd_obj = beginobj(&logo3dfx_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	else	
		bkgrnd_obj = beginobj(&logo3dfx_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&logo3dfx_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);

//	set_string_id(5);
//	set_text_font(FONTID_BAST10);
//
//	set_text_position(SPRITE_HRES/2,80-3, 10.0F);
//	oprintf("%dj%dcGLIDE COPYRIGHT (c) 1997 3DFX INTERACTIVE, INC.", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2,66-3, 10.0F);
//	oprintf("%dj%dcTHE 3DFX INTERACTIVE LOGO IS A TRADEMARK OF", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2,52-3, 10.0F);
//	oprintf("%dj%dc3DFX INTERACTIVE, INC.", (HJUST_CENTER|VJUST_CENTER), WHITE);

	black_screen();

	sleep(2);
	fade(1.0f, 30, 0);		//1=full on 20=ticks

	wait_for_any_butn_press(min_time,max_time);

	fade(0.0f, 30, 0);		//0=full off
	sleep(30);

	delete_multiple_strings(5, 0xffffffff);
	delete_all_sprites();
}
//-------------------------------------------------------------------------------------------------
//  		This process displays the NFLPA logo
//-------------------------------------------------------------------------------------------------
void NFLPA_screen(int min_time, int max_time)
{
	sprite_info_t *bkgrnd_obj;

	wipeout();
	
	bkgrnd_obj = beginobj(&nflpabg, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&nflpabg_c1, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&nflpabg_c2, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);
	bkgrnd_obj = beginobj(&nflpabg_c3, 0, SPRITE_VRES, 500.0f, BARS_PG_TID);

//	set_string_id(5);
//	set_text_font(FONTID_BAST10);

//	set_text_position(SPRITE_HRES/2,80-3, 10.0F);
//	oprintf("%dj%dcGLIDE COPYRIGHT (c) 1997 3DFX INTERACTIVE, INC.", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2,66-3, 10.0F);
//	oprintf("%dj%dcTHE 3DFX INTERACTIVE LOGO IS A TRADEMARK OF", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2,52-3, 10.0F);
//	oprintf("%dj%dc3DFX INTERACTIVE, INC.", (HJUST_CENTER|VJUST_CENTER), WHITE);

	black_screen();

	sleep(2);
	fade(1.0f, 30, 0);		//1=full on 20=ticks

	wait_for_any_butn_press(min_time,max_time);

	fade(0.0f, 29, 0);		//0=full off
	sleep(30);

	delete_all_sprites();

	// Wait for both buffers to clear
	sleep(2);
}
//-------------------------------------------------------------------------------------------------
//  		This process displays coaching tips
//-------------------------------------------------------------------------------------------------
void show_coaching_tips(int min_time, int max_time)
{

	wipeout();

	coaching_tip_scrn();

	delete_multiple_strings(5, 0xffffffff);
	delete_all_sprites();
}

//---------------------------------------------------------------------------------------------------------------------------
//	Turn on the Team Info page with mugshots and animate them
//	Each time through, it shows the next team
//	
//	INPUT:	Nothing
//
//	OUTPUT:	Nothing
//---------------------------------------------------------------------------------------------------------------------------
extern volatile int lg_loaded;

void show_team_info_pg(int min_time, int max_time)
{
	const struct st_plyr_data * pspd;
	const struct fbteam_data * pftd;
	image_info_t ** ppii;
	float fz;
	int	mcnt, i, t;
	char * pstr;
	sprite_info_t 	*bkgrnd_obj;
	obj3d_data_t	*phelm;
	image_info_t	**img;

	if (++team_index >= NUM_TEAMS)
		team_index = 0;

	wipeout();
	
	// Count # of mugs to show; max is 4
#if 0
	mcnt = 0;
	i = NUM_PERTEAM_SPD;
	pspd = teaminfo[team_index].static_data;
	while (i-- && mcnt < 4)
	{
		// Find a mug?
		if (pspd->pii_mugshot)
		{
			// Get its texture name & load if hasn't been
			pstr = ((image_info_t *)pspd->pii_mugshot)->texture_name;
			if (!find_texture(pstr, MUGSHOT2_TID)) {
				if (!create_texture(pstr,
						MUGSHOT2_TID,
						0,
						CREATE_NORMAL_TEXTURE,
						GR_TEXTURECLAMP_CLAMP,
						GR_TEXTURECLAMP_CLAMP,
						GR_TEXTUREFILTER_BILINEAR,
						GR_TEXTUREFILTER_BILINEAR)) {
#ifdef DEBUG
					lockup();
#else
					while(1) ;
#endif
				}
			}
			mcnt++;
		}
		pspd++;
	}

	// Only continue if we found at least one
	if (mcnt)
#endif
	{
#if 0
		// Set ptr to team data & get textures for team info imgs
		pftd = teaminfo + team_index;
		ppii = pftd->pti;

		while (*ppii)
		{
			pstr = ppii[0]->texture_name;
			if (!find_texture(pstr, MUGSHOT2_TID))
			{
				if (!create_texture(pstr,MUGSHOT2_TID,0,CREATE_NORMAL_TEXTURE,
						GR_TEXTURECLAMP_CLAMP,GR_TEXTURECLAMP_CLAMP,
						GR_TEXTUREFILTER_BILINEAR,GR_TEXTUREFILTER_BILINEAR))
				{
#ifdef DEBUG
					lockup();
#else
					while(1) ;
#endif
				}
			}
			ppii++;
		}
#endif

		// Load in flash images for mugshots
//		if (!create_texture("mflash.wms",
//				MUGSHOT2_TID,
//				0,
//				CREATE_NORMAL_TEXTURE,
//				GR_TEXTURECLAMP_CLAMP,
//				GR_TEXTURECLAMP_CLAMP,
//				GR_TEXTUREFILTER_BILINEAR,
//				GR_TEXTUREFILTER_BILINEAR))
//#ifdef DEBUG
//			lockup();
//#else
//			while(1) ;
//#endif

#if 0
		// Load in fade piece, player position icons
		if (!create_texture("tminfo00.wms",MUGSHOT2_TID,0,CREATE_NORMAL_TEXTURE,
				GR_TEXTURECLAMP_CLAMP,GR_TEXTURECLAMP_CLAMP,
				GR_TEXTUREFILTER_BILINEAR,GR_TEXTUREFILTER_BILINEAR))
#ifdef DEBUG
			lockup();
#else
			while(1) ;
#endif
#endif

//		if (!create_texture("tminfo01.wms",MUGSHOT2_TID,0,CREATE_NORMAL_TEXTURE,
//				GR_TEXTURECLAMP_CLAMP,GR_TEXTURECLAMP_CLAMP,
//				GR_TEXTUREFILTER_BILINEAR,GR_TEXTUREFILTER_BILINEAR))
//#ifdef DEBUG
//			lockup();
//#else
//			while(1) ;
//#endif

		// Load in flash images for mugshots
//		if (!create_texture("plqfsh.wms",
//				MUGSHOT2_TID,
//				0,
//				CREATE_NORMAL_TEXTURE,
//				GR_TEXTURECLAMP_CLAMP,
//				GR_TEXTURECLAMP_CLAMP,
//				GR_TEXTUREFILTER_BILINEAR,
//				GR_TEXTUREFILTER_BILINEAR))
//#ifdef DEBUG
//			lockup();
//#else
//			while(1) ;
//#endif

		// Abort if we can't setup the font
		if(!create_font(&bast10_font, FONTID_BAST10))
		{
#ifdef DEBUG
			lockup();
#endif
			exit(0);
		}

#if 0
		lock_multiple_textures(MUGSHOT2_TID,0xFFFFFFFF);
#endif

		// draw background
//		bkgrnd_obj = beginobj(&teamsel,    0.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//		bkgrnd_obj->id = 1;
//		bkgrnd_obj = beginobj(&teamsel_c1, 0.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//		bkgrnd_obj->id = 1;
//
//		bkgrnd_obj = beginobj(&teamsel,    SPRITE_HRES/2.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//		bkgrnd_obj->id = 1;
//		bkgrnd_obj->mode = FLIP_TEX_H;
//		generate_sprite_verts(bkgrnd_obj);
//
//		bkgrnd_obj = beginobj(&teamsel_c1, SPRITE_HRES/2.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//		bkgrnd_obj->id = 1;
//		bkgrnd_obj->mode = FLIP_TEX_H;
//		generate_sprite_verts(bkgrnd_obj);

		bkgrnd_obj = beginobj(&helmet,    0, SPRITE_VRES, 500, BKGRND2_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&helmet_c1, 0, SPRITE_VRES, 500, BKGRND2_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&helmet_c2, 0, SPRITE_VRES, 500, BKGRND2_TID);
		bkgrnd_obj->id = 1;
		bkgrnd_obj = beginobj(&helmet_c3, 0, SPRITE_VRES, 500, BKGRND2_TID);
		bkgrnd_obj->id = 1;

		//	3D helmet logic
		// Load the helmet texture for TEAM 1
		pnode = create_texture(tm_helmet_textures[team_index], HELMET_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);

		if (pnode == NULL)
			fprintf(stderr, "Error loading %s\r\n", tm_helmet_textures[team_index]);
		else
		{
			for(i=0; i < HELMET_LIMBS; i++)
			{
				phelm = helmet_limb_ptrs[i];
				phelm->decal = pnode->texture_handle;
				phelm->ratio = pnode->texture_info.header.aspect_ratio;
				phelm->limb_obj = helmet_limbs[i];
				phelm->odata.x = -1.3F;		//0.0F 258.0F
				phelm->odata.y = -0.9F;		//-1.0F -0.5F
				phelm->odata.z = 10.0F;
				phelm->odata.fwd = 655;
				create_object("hlm1limb", OID_P1HELMET, 0, 0, (void *)phelm, helmet_draw_function);
			}
		}
		qcreate("helmet1",HELMET1_PID,helmet_proc,0,1,0,0);

		img = teaminfo[team_index].pti;

		// draw city name
		beginobj(img[0], 340, SPRITE_VRES-33, 2.0f, MUGSHOT2_TID);	//256

		// draw team logo
		beginobj(img[2], 340, SPRITE_VRES-90, 2.5f, MUGSHOT2_TID);	//256
		if (img[3])
			beginobj(img[3], 340, SPRITE_VRES-90, 2.5f, MUGSHOT2_TID);	//256

//		bkgrnd_obj->id = OID_VERSUS;
//		bkgrnd_obj->w_scale = .75f;
//		bkgrnd_obj->h_scale = .75f;

		// draw team logo
//		qcreate("logo", ROTATE_LOGO_PID, show_logo_proc, team_index, 0, 0, 0);

		// Turn on team logo stuff
//		beginobj(&ti_fade,   13,SPRITE_VRES-5, 15.0f, MUGSHOT2_TID);
//		beginobj(&ti_fade_c1,13,SPRITE_VRES-5, 15.0f, MUGSHOT2_TID);

//		ppii = pftd->pti;
//		fz = 10.0f;
//		while (*ppii)
//		{
//			beginobj(*ppii, SPRITE_VRES/2.0f, SPRITE_VRES-40.0f, fz, MUGSHOT2_TID);
//			fz -= 0.1f;
//			ppii++;
//		}

		black_screen();

		draw_enable(1);

		sleep(3);
		fade(1.0f, 20, 0);			//1=full on 20=ticks

#ifdef SHOW_MUGSHOT
		for (t = 0; t < mcnt; t++)
		{
			qcreate("mugshot", ROTATE_LOGO_PID, show_mugshot_proc, t, mcnt, 0, 0);

			for (i = 0; i < (28 + 15); i++)
			{
				if (t > 0 || i > 20)		// # for tick check should be less than <i> max
				{
					if (get_player_sw_current() & ALL_ABCD_MASK)
					{
						snd_scall_direct(tick_sndn,VOLUME1,127,1);
						//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
						goto clean_up;
					}
				}
				sleep(1);
			}
//			sleep(15);
		}
#endif
		sleep(60);

		wait_for_any_butn_press(1, tsec*5);

clean_up:
		if (!att_keeper)
		{
			fade(0.0f, 20, 0);		//0=full off
			sleep(20);

			delete_all_sprites();

			// Delete mugshot textures
			unlock_multiple_textures( MUGSHOT2_TID, -1 );
			delete_multiple_textures( MUGSHOT2_TID, -1 );

			// Delete background
			del1c( 0, -1 );
			delete_all_background_sprites();
#ifdef SHOW_MUGSHOT
			// Kill logo sprite
			delete_multiple_textures( ROTATE_LOGO_TID, -1 );
#endif
			// delete helmet object and texture
			delete_multiple_textures( HELMET_TID, -1 );
			delete_multiple_objects( OID_P1HELMET, -1 );
			
		}
#ifdef SHOW_MUGSHOT
		// Kill logo proc
		killall( ROTATE_LOGO_PID, -1 );
#endif
		// kill 3d helmet proc
		killall( HELMET1_PID, -1 );

		sleep(10);
	}
	normal_screen();
}
	

//-------------------------------------------------------------------------------------------------------
// 	helmet_proc()
//
//		arg1 is SIDE (left or right)
//		arg2 is TIME (in ticks) to wait before slide down
//-------------------------------------------------------------------------------------------------------
void helmet_proc(int *args)
{
	int				side,i,done,wait;
	obj3d_data_t	*pobj;


//	side = args[0];						// team (0 or 1)
//	wait = args[1];						// time to wait before falling
//
//	while (wait--)
//		sleep(1);
//	
//	done = 0;
	while (1)
	{
		for (i=0; i < HELMET_LIMBS; i++)
		{
			pobj = helmet_limb_ptrs[i];
			pobj->odata.fwd += 10;
		}
		sleep(2);
	}
}

//-------------------------------------------------------------------------------------------------------
//	helmet_draw_function() - 
//-------------------------------------------------------------------------------------------------------
void helmet_draw_function(void *oi)
{
	float	xlate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
	 		  			  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.0f };

	float	rotate[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, 1.0f, 0.0f, 0.0f,
						   0.0f, 0.0f, 1.0f, 0.0f };

	float	cm[12];

	obj_3d	*pobj = (obj_3d *)oi;
	obj3d_data_t *phlmt = (obj3d_data_t *)oi;

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

	render_limb(phlmt->limb_obj, cm, &(phlmt->decal), &(phlmt->ratio));
}

#ifdef SHOW_MUGSHOT
//-------------------------------------------------------------------------------------------------
//	This process turns on a mugshot and scales it in
//
// 	INPUT:	mugshot # (0-3)  
//          total # (1-4)
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void show_mugshot_proc(int *args)
{
	const struct st_plyr_data * pspd;
	sprite_info_t *p_mugshot;
	sprite_info_t *flsh_obj;
	sprite_info_t *icon_obj;
	register float fx, fy, fh;
	register int mug_num;
	register int i = args[0];
	register int j = args[1];

	sleep(30);

	// Set ptr to spec'd player data
	pspd = teaminfo[team_index].static_data;

	mug_num = i;
	while (1)
	{
		if (pspd->pii_mugshot)
		{
			if (--i < 0)
				break;
		}
		pspd++;
	}

	i = mug_num;
	j--;
	if (!(j & 1) && i == j)
		fx = 385.0f;
	else
	{
		i ^= (i >> 1);
		fx = i & 1 ? 330.0f : 440.0f;
	}
	if (j < 2)
		fy = 189.0f;
	else
		fy = i < 2 ? 276.0f : 102.0f;

//	// Do idx for pos tables
//	i = 4 - args[1] + args[0] + args[0];

	p_mugshot = beginobj(pspd->pii_mugshot,
			fx,
			fy,
			10.0f+mug_num,
			MUGSHOT2_TID);
	p_mugshot->w_scale *= 0.8f;
	p_mugshot->h_scale *= 0.8f;
	generate_sprite_verts(p_mugshot);

	// Display player name & position - 2 lines of info per player
	set_text_font(i = is_low_res ? FONTID_BAST8T : FONTID_BAST10);
	set_text_justification_mode(HJUST_CENTER|VJUST_TOP);

	fy -= 57.0f;
	set_text_position(fx, fy, 10.0f);
	oprintf("%dc%s", WHITE, pspd->frstname);
	set_text_position(fx+1.0f, fy-1.0f, 11.0f);
	oprintf("%dc%s", DK_GRAY, pspd->frstname);

	fy -= (float)get_font_height(i) + (is_low_res ? 6.0f : 3.0f);
	set_text_position(fx, fy, 10.0f);
	oprintf("%dc%s", WHITE, pspd->lastname);
	set_text_position(fx+1.0f, fy-1.0f, 11.0f);
	oprintf("%dc%s", DK_GRAY, pspd->lastname);


// 	flsh_obj = beginobj(mug_flsh_imgs[0],
// 			flsh_xy[i][four_plr_ver][X],
// 			flsh_xy[i][four_plr_ver][Y],
// 			7.0f,
// 			MUGSHOT2_TID);
//	flsh_obj->w_scale = 3.5f;
//	flsh_obj->h_scale = 4.0f;
//	generate_sprite_verts(flsh_obj);

//	for (j = 0; j < 10; j++)
//	{
//		change_img_tmu(flsh_obj, mug_flsh_imgs[j], JAPPLE_BOX_TID);
//		sleep(3);
//	}
//	delobj(flsh_obj);

	// Only do position icon if there is one!
//	if (icon_pos_imgs[pspd->position])
//	{
//		icon_obj = beginobj(icon_pos_imgs[pspd->position],
//				icon_xy[i][is_low_res][X],
//				icon_xy[i][is_low_res][Y],
//				10.0f,
//				MUGSHOT2_TID);
//		flsh_obj = beginobj(stat_flsh_imgs[0],
//				icon_xy[i][is_low_res][X],
//				icon_xy[i][is_low_res][Y]+5,
//				9.0f,
//				MUGSHOT2_TID);
//		flsh_obj->w_scale = 0.6f;
//		flsh_obj->h_scale = 2.4f;
//		generate_sprite_verts(flsh_obj);

//		for (j=0; j < (sizeof(stat_flsh2_imgs)/sizeof(image_info_t *))-1; j++)
//		for (j = 0; j < 11; j++)
//		{
//			change_img_tmu(flsh_obj, stat_flsh_imgs[j], JAPPLE_BOX_TID);
//			sleep(3);
//		}
//		delobj(flsh_obj);
//	}
}
#endif

//-------------------------------------------------------------------------------------------------
//	This process turns on sparkles on top of Blitz logo
//
// 	INPUT:	x,y of Blitz logo
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void one_sparkle_proc(int *args)
{
	sprite_info_t	*flsh_obj;
	int	i,x,y;

	// args are always ints
	x = args[0];
	y = args[1];

	i = randrng( sizeof(sparkle_xy)/(sizeof(float)*4) );
	x = sparkle_xy[i][0]+x;
	y = sparkle_xy[i][1]+y;

//	flsh_obj = beginobj(sparkle_imgs[0],x,y,9.0F, NFL_LOGO_TID);
	flsh_obj = beginobj(sparkle_imgs[0],x,y,9.0F, JAPPLE_BOX_TID);
	flsh_obj->w_scale = 2.0f;	// 3.0f
	flsh_obj->h_scale = 2.0f;	// 3.0f
	generate_sprite_verts(flsh_obj);

	sleep(3);

	for (i=0; i < (int)(sizeof(sparkle_imgs)/sizeof(image_info_t *))-1; i++)
	{
//		change_img_tmu(flsh_obj,sparkle_imgs[i],NFL_LOGO_TID);
		change_img_tmu(flsh_obj,sparkle_imgs[i],JAPPLE_BOX_TID);
		sleep(3);
	}

	delobj(flsh_obj);
	die(0);
}

//-------------------------------------------------------------------------------------------------
//	This process turns on sparkles on top of Blitz logo
//
// 	INPUT:	x,y of Blitz logo
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void spawn_sparkles_proc(int *args)
{
	int	t,x,y;

	// args are always ints
	x = args[0];
	y = args[1];

	if (!create_texture("flash.wms", JAPPLE_BOX_TID, 0, CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
		GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR))
	{
#ifdef DEBUG
			fprintf( stderr,"Error loading flash.wms' texture \n");
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
			return;
	}

//	lock_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);
	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);

	for (t=0; t < 100; t++)
	{
		sleep(randrng(230)+60);
		qcreate("sparkle",ROTATE_LOGO_PID,one_sparkle_proc,x,y,0,0);
	}
	sleep(35);
	die(0);
}

//-------------------------------------------------------------------------------------------------
//	This process does a sparkles on the design team credits
//
// 	INPUT:	screen x,screen y
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void midway_team_sparkle_proc(int *);
void midway_team_sparkle_proc(int *args)
{
	sprite_info_t	*flsh_obj;
	int	i,x,y;

	// args are always ints
	x = args[0];
	y = args[1];

	flsh_obj = beginobj(sparkle_imgs[0],x,y,9.0F, NFL_LOGO_TID);
	flsh_obj->w_scale = 2.0f;
	flsh_obj->h_scale = 2.0f;
	generate_sprite_verts(flsh_obj);

	sleep(3);

	for (i=0; i < (int)(sizeof(sparkle_imgs)/sizeof(void *)-1); i++)
	{
		change_img_tmu(flsh_obj,sparkle_imgs[i],NFL_LOGO_TID);
		sleep(3);
	}

	delobj(flsh_obj);
	die(0);
}

//---------------------------------------------------------------------------------------------------------------------------
//			Do Midway team credits page
//---------------------------------------------------------------------------------------------------------------------------
#define ROTATECNT 18

void midway_team_page(int min_time, int max_time)
{
	int	i, r, t, x, y, ly = -1;
	float tx, ty, dy;
//	sprite_info_t *r_logo;

	wipeout();
	
	// load & lock
	if (load_textures_into_tmu(midway_team_textures_tbl))
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all MIDWAY TEAM textures into tmu\r\n");
		lockup();
#else
		return;
#endif
	}
	lock_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);

//	draw_backdrop();
	beginobj(&crdts_bg,    0, SPRITE_VRES, 13, NFL_LOGO_TID);
	beginobj(&crdts_bg_c1, 0, SPRITE_VRES, 13, NFL_LOGO_TID);
	beginobj(&crdts_bg_c2, 0, SPRITE_VRES, 13, NFL_LOGO_TID);
	beginobj(&crdts_bg_c3, 0, SPRITE_VRES, 13, NFL_LOGO_TID);

	// create Midway Team page header
//	beginobj(&fadetm,      0.0f, SPRITE_VRES, 13.0f, NFL_LOGO_TID);
//	beginobj(&fadetm_c1,   0.0f, SPRITE_VRES, 13.0f, NFL_LOGO_TID);
//	beginobj(&midwaytm,    0.0f, SPRITE_VRES, 12.0f, NFL_LOGO_TID);
//	beginobj(&midwaytm_c1, 0.0f, SPRITE_VRES, 12.0f, NFL_LOGO_TID);
//	beginobj(&blitztm,     0.0f, SPRITE_VRES, 11.0f, NFL_LOGO_TID);
//	beginobj(&blitztm_c1,  0.0f, SPRITE_VRES, 11.0f, NFL_LOGO_TID);

	// setup text font
	set_string_id(5);
	set_text_font(is_low_res ? FONTID_BAST7T : FONTID_BAST8T);
	dy = (float)(get_font_height(is_low_res ? FONTID_BAST7T : FONTID_BAST8T) + ((is_low_res) ? 5.0f : 2.0f));
	set_text_z_inc(0.1f);

	// create Midway Team mugs
	for (i = 0; i < (int)(sizeof(midway_team_imgs)/sizeof(void *)); i++)
	{
		beginobj(midway_team_imgs[i], 0, SPRITE_VRES, 10.0F, NFL_LOGO_TID);

		tx = 0.0f - midway_team_imgs[i]->ax + midway_team_imgs[i]->w / 2.0f - 4.0F;
		ty = SPRITE_VRES + midway_team_imgs[i]->ay + ((is_low_res) ? 1.0f : 3.0f);

		set_text_position(tx, ty-2, 8.0f);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, midway_team_nms[i][0]);
		set_text_position(tx, ty - dy-1, 6.0f);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, midway_team_nms[i][1]);
	}
	set_text_z_inc(0.0f);

//	// create rotating logo obj
//	r_logo = beginobj(midway_team_logo_imgs[0], 110.0f, (SPRITE_VRES - 54.0f), 8.5f, NFL_LOGO_TID);

	black_screen();

	draw_enable(1);

	sleep(3);
	fade(1.0f, 20, 0);			//1=full on 20=ticks

	r = t = 0;
	while (r < ROTATECNT)
	{
		i = 0;
//		while (i < (int)(sizeof(midway_team_logo_imgs)/sizeof(void *)))
		while (i < 39)
		{
			if (t > min_time && (get_player_sw_current() & ALL_ABCD_MASK))
			{
				r = ROTATECNT;
				snd_scall_direct(tick_sndn,VOLUME1,127,1);
				break;
			}
			if (!(t % 2))
			{
//				change_img_tmu(r_logo, midway_team_logo_imgs[i], NFL_LOGO_TID);
				i++;
			}
			t++;

			if (!(t % 50))
			{
				y = randrng((int)(sizeof(midway_team_imgs)/sizeof(void *)));
				x = randrng(4);

				if (y == ly)
					y = (y + 1) % (int)(sizeof(midway_team_imgs)/sizeof(void *));
				ly = y;

//				{char * fuck[]={"BL","TL","TR","BR"};fprintf(stderr,"C:%s  P:%s %s\r\n",fuck[x],midway_team_nms[y][0],midway_team_nms[y][1]);}
				switch (x)
				{
					case 0:
						x = -midway_team_imgs[y]->ax + 6.0f - 3.0f;
						y = SPRITE_VRES + midway_team_imgs[y]->ay + 15.0f - 4.0f;
						break;

					case 1:
						x = -midway_team_imgs[y]->ax + 6.0f - 3.0f;
						y = SPRITE_VRES + midway_team_imgs[y]->ay + midway_team_imgs[y]->h - 6.0f + 4.0f;
						break;

					case 2:
						x = -midway_team_imgs[y]->ax + midway_team_imgs[y]->w - 16.0f + 6.0f;
						y = SPRITE_VRES + midway_team_imgs[y]->ay + midway_team_imgs[y]->h - 6.0f + 4.0f;
						break;

					case 3:
						x = -midway_team_imgs[y]->ax + midway_team_imgs[y]->w - 16.0f + 6.0f;
						y = SPRITE_VRES + midway_team_imgs[y]->ay + 15.0f - 4.0f;
						break;
				}
				qcreate("sparkle",ROTATE_LOGO_PID,midway_team_sparkle_proc,x,y,0,0);
			}

			sleep(1);
		}
		r++;
	}

	// Kill flash procs
	killall(ROTATE_LOGO_PID,0xFFFFFFFF);

	fade(0.0f, 20, 0);		//0=full off
	sleep(20);
	
	unlock_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);
	delete_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);
	delete_multiple_strings(5, 0xffffffff);

	// kill all objs & background
	del1c(0,0xFFFFFFFF);
	delete_all_background_sprites();
} 

//---------------------------------------------------------------------------------------------------------------------------
//			Turn on the NFL logo and animate it
//---------------------------------------------------------------------------------------------------------------------------
//void fade(float, int, void (*)(void));

void nfl_logo(int min_time, int max_time)
{

	int	i,j,t;
	sprite_info_t	*p_nfl_logo;
	sprite_info_t	*b_logo;
	sprite_info_t	*bkgrnd_obj;

	wipeout();
//	
//	if (load_textures_into_tmu(nfl_logo_textures_tbl))
//	{
//#ifdef DEBUG
//		fprintf(stderr, "Couldn't load all NFL LOGO textures into tmu\r\n");
//		lockup();
//#else
//		return;
//#endif
//	}

	// put up legal background
	bkgrnd_obj = beginbobj(&nfllegal,   0.0F, SPRITE_VRES, 500.0F, JAPPLE_BOX_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginbobj(&nfllegal_c1, 0.0F, SPRITE_VRES, 500.0F, JAPPLE_BOX_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginbobj(&nfllegal_c2, 0.0F, SPRITE_VRES, 500.0F, JAPPLE_BOX_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	bkgrnd_obj = beginbobj(&nfllegal_c3, 0.0F, SPRITE_VRES, 500.0F, JAPPLE_BOX_TID);
	bkgrnd_obj->id = 1;
	bkgrnd_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

//	draw_backdrop();

	// create BLITZ logo
//	b_logo = beginobj(&blitzlog, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f-20, 10.0F, TM_SELECT_TID);
//	b_logo = beginobj(&blitzlog_c1, SPRITE_HRES/2.0f, SPRITE_VRES/2.0f-20, 10.0F, TM_SELECT_TID);
//	b_logo = beginobj(&blitzl_s, SPRITE_HRES/2.0f+3, SPRITE_VRES-75, 11.0F, TM_SELECT_TID);
//	b_logo->w_scale = 2.0f;
//	b_logo->h_scale = 2.0f;
//	generate_sprite_verts(b_logo);

//	qcreate("sparkle",ROTATE_LOGO_PID,spawn_sparkles_proc,SPRITE_HRES/2.0f, SPRITE_VRES/2.0f-20,0,0);

//	set_string_id(5);
//	set_text_font(FONTID_BAST10);
//	i = get_font_height(FONTID_BAST10) + 2;
//	j = (is_low_res) ? 135 : 140;
//	if(is_low_res)
//		i = i + 3;
//	
//	set_text_position(SPRITE_HRES/2, j, 10.0F); j -= i;
//	oprintf("%dj%dc(c) 1997 MIDWAY GAMES INC.  ALL RIGHTS RESERVED.", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2, j, 10.0F); j -= i; j -= 6;
//	oprintf("%dj%dcBLITZ AND MIDWAY ARE TRADEMARKS OF MIDWAY GAMES INC.", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2, j, 10.0F); j -= i;
//	oprintf("%dj%dc(c) 1997 NFLP.  TEAM NAMES AND LOGOS ARE TRADEMARKS", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2, j, 10.0F); j -= i;
//	oprintf("%dj%dcOF THE TEAMS INDICATED.  THE NFL SHIELD, SUPER BOWL", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2, j, 10.0F); j -= i;
//	oprintf("%dj%dcAND PRO BOWL ARE TRADEMARKS OF THE", (HJUST_CENTER|VJUST_CENTER), WHITE);

//	if (is_low_res)
//	{
//		set_text_position(SPRITE_HRES/2, j-1, 10.0F);
//		j -= i;
//		j -= 7;
//	}
//	else	
//	{
//		set_text_position(SPRITE_HRES/2, j, 10.0F);
//		j -= i;
//		j -= 6;
//	}

//	oprintf("%dj%dcNATIONAL FOOTBALL LEAGUE.", (HJUST_CENTER|VJUST_CENTER), WHITE);
////?	set_text_position(SPRITE_HRES/2,22-3+32, 10.0F);
////?	oprintf("%dj%dcOFFICIALLY LICENSED PRODUCT OF THE", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	set_text_position(SPRITE_HRES/2, j, 10.0F); j -= i;
//	oprintf("%dj%dcOFFICIALLY LICENSED PRODUCT OF THE", (HJUST_CENTER|VJUST_CENTER), WHITE);
//	if (is_low_res)
//		set_text_position(SPRITE_HRES/2, j-1, 10.0F);
//	else
//		set_text_position(SPRITE_HRES/2, j, 10.0F);
////	j -= i;
//	oprintf("%dj%dcNATIONAL FOOTBALL LEAGUE PLAYERS.", (HJUST_CENTER|VJUST_CENTER), WHITE);

//	p_nfl_logo = beginobj(&nfl37, SPRITE_HRES/2, SPRITE_VRES/2-20+34, 10.0F, NFL_LOGO_TID);
//	p_nfl_logo->w_scale = 0.70;
//	p_nfl_logo->h_scale = 0.70;
//	generate_sprite_verts(p_nfl_logo);

	black_screen();

	sleep(3);
	fade(1.0f, 20, 0);			//1=full on 20=ticks

//	wait_for_any_butn_press(min_time,max_time);
	wait_for_any_butn_press(120,300);

//	j = 0;
//	for (t=0; t < 9; t++)
//	{
//		for (i=0; i < (int)(sizeof(nfl_logo_imgs)/sizeof(image_info_t *)); i++)
//		{
//			change_img_tmu(p_nfl_logo,nfl_logo_imgs[i],NFL_LOGO_TID);
//			if (j > min_time)
//			{
//				if (get_player_sw_current() & ALL_ABCD_MASK)
//				{
//					snd_scall_direct(tick_sndn,VOLUME1,127,1);
//					//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
//					goto clean_up;
//				}
//			}
//			sleep(2); j += 2;		// constant wants to be the same for both
//		}
//	}
//
//clean_up:
//	// Kill flash procs
//	killall(ROTATE_LOGO_PID,0xFFFFFFFF);

	fade(0.0f, 20, 0);		//0=full off
	sleep(20);
	
//	delobj(p_nfl_logo);
//	unlock_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);
//	delete_multiple_textures(NFL_LOGO_TID,0xFFFFFFFF);
	delete_multiple_strings(5, 0xffffffff);

////	unlock_multiple_textures(BARS_PG_TID,0xFFFFFFFF);
////	delete_multiple_textures(BARS_PG_TID,0xFFFFFFFF);

	del1c(0,0xFFFFFFFF);												// delete background
	delete_all_background_sprites();
} 


//---------------------------------------------------------------------------------------------------------------------------
//
//  High Score Pages Stuff
//
//---------------------------------------------------------------------------------------------------------------------------

static ostring_t * hstrings[3][HI_SCORE_ENTRIES];

static int simple_cycle_state = 0;
static int show_hilite;

static int simple_cycle_table[] = {
0xffffff00,
0xffffff1f,
0xffffff3f,
0xffffff5f,
0xffffff7f,
0xffffff9f,
0xffffffbf,
0xffffffdf,
0xffffffff,
0xffffffdf,
0xffffffbf,
0xffffff9f,
0xffffff7f,
0xffffff5f,
0xffffff3f,
0xffffff1f
};

static int simple_hilite_table[] = {
0xff0000ff,
0xff1f1fff,
0xff3f3fff,
0xff5f5fff,
0xff7f7fff,
0xff9f9fff,
0xffbfbfff,
0xffdfdfff,
0xffffffff,
0xffdfdfff,
0xffbfbfff,
0xff9f9fff,
0xff7f7fff,
0xff5f5fff,
0xff3f3fff,
0xff1f1fff
};

#define RATE1	5
#define RATE2	3

static void simple_cycle_proc(int *args)
{
	int	i, j, m, n, c;
	char hilite[HI_SCORE_ENTRIES];

	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		hilite[i] = 0;
		if (show_hilite && !args[1])
		{
			for (j = 0; j < (four_plr_ver ? MAX_PLYRS:2); j++)
			{
				if (!(record_me & (1 << j)))
					continue;
				if (plyr_data_ptrs[j]->plyr_name[0] == '\0' ||
						plyr_data_ptrs[j]->plyr_name[0] == '-')
					continue;
				if (strncmp(rec_sort_ram[i].plyr_name,
							plyr_data_ptrs[j]->plyr_name,
							LETTERS_IN_NAME))
					continue;
				if (strncmp(rec_sort_ram[i].plyr_pin_nbr,
							plyr_data_ptrs[j]->plyr_pin_nbr,
							PIN_NUMBERS))
					continue;
				hilite[i] = 1;
			}
		}
	}

	i = 0;
	j = HI_SCORE_ENTRIES - 1;
	while(1)
	{
		if(!args[0])
		{
			for (m = 0; m < HI_SCORE_ENTRIES; m++)
			{
				for (n = 0; n < 3; n++)
				{
					if (hstrings[n][m])
					{
						c = hilite[m] ?
							simple_hilite_table[(i/RATE2)%15] : WHITE;
						if (m == j)
						{
							if (j)
								c = LT_YELLOW;
							else if (c == (int)WHITE)
								c = simple_cycle_table[(i/RATE2)%15];
						}
						hstrings[n][m]->state.color = c;
						change_string_state(hstrings[n][m]);
					}
				}
			}
		}

		i++;
		if (j)
		{
			if (!(i % RATE1))
			{
				j--;
				if (!j)
				{
					i = 0;
					simple_cycle_state = 1;
				}
			}
		}
		else if (i > (40 * RATE2))
			simple_cycle_state = 2;

		sleep(1);
	}
}

static void do_simple_cycle(int sleep_time, int no_cyc, int no_hilite, int no_del)
{
	simple_cycle_state = 0;
	qcreate("scycle", SIMPLE_CYCLE_PROC_ID, simple_cycle_proc, no_cyc, no_hilite, 0, 0);

	// Don't button-out till state changes
	while(simple_cycle_state < 1)
		sleep(1);

	while(sleep_time && !(get_player_sw_current() & ALL_ABCD_MASK))
	{
		sleep(1);
		sleep_time--;
	}

	if(sleep_time)
		snd_scall_direct(tick_sndn,VOLUME1,127,1);
		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);

	killall(SIMPLE_CYCLE_PROC_ID, -1);

	if (!no_del)
	{
		del1c(2, -1);
		delete_multiple_strings(5, -1);
	}
}

	
//-------------------------------------------------------------------------------------------------
//			This routine prints all the hi-score tables
//
// called from ATTRACT.C
//-------------------------------------------------------------------------------------------------

#define HISCORE_TICKS	(7 * 57)

void show_hiscore_tables(int min_time, int max_time)
{
	plyr_record_t *rec_ram;
	sprite_info_t *hiscr_hdr_obj;
	ostring_t *strobj;
	char *tmp;
	char *tmp1;
	float x1, x2, wid_s;
	short i;
	char buffer[80];
	int fid;

	show_hilite = show_them_hiscore_tables_first;
	show_them_hiscore_tables_first = 0;

	if(cmos_status == 2)
	{
		return;
	}

	// Disable drawing
	draw_enable(0);

	wipeout();
	
	// Enable drawing
	draw_enable(1);

	if (load_textures_into_tmu(hiscore_textures_tbl)) 
	{
		// Not to worry, the wipeout killed the plates if they even existed
		printf("Couldn't load all HI-SCORE page textures into tmu\n");
		return;
	}
	
	get_all_records();

	if(!rec_sort_ram)
	{
		// Not to worry, the wipeout killed the plates if they even existed
		return;
	}

	// Disable drawing
	draw_enable(0);

	draw_bobj_backgrnd2(50.0f, HISCORE_PG_TID);

	// Set step value for left-to-right text overlap
	set_text_z_inc(0.1f);

	// Make sure we are at full intensity
	normal_screen();

	// Make 16 pt font available
	fid = FONTID_BAST23;
	if (is_low_res)
		if (create_font(&bast16_font, FONTID_BAST16))
			fid = FONTID_BAST16;
	{
		font_node_t * pfn;
		image_info_t * pii;
		wid_s = 0.0f;
		if ((pfn =  get_font_handle(fid)))
		{
			if ((pii = get_char_image_info(fid, 'S')))
				wid_s = pii->w + (float)(pfn->font_info->char_space);
		}
	}

	//
	// Hi-score page 1 (grand champions)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[0], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[1], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[0]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_teams_defeated);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	if(create_font(&bast75_font, FONTID_BAST75))
		set_text_font(FONTID_BAST75);
	else
		set_text_font(FONTID_BAST23);

//	x1 = 0.0f;
//	x2 = 0.0f;
//	for(i = 0; i < 3; i++)
//	{
//		if(get_string_width(buffer) > x1)
//			x1 = get_string_width(buffer);
//		sprintf(buffer, "%s", rec_ram[i].plyr_name);
//		if(get_string_width(buffer) > x2)
//			x2 = get_string_width(buffer);
//	}
//	x2 = (SPRITE_HRES * 0.9f) - x2;
//	x1 = (SPRITE_HRES * 0.1f) + x1;
//	x1 += x2;
//	x1 /= 2.0f;

////	fprintf(stderr,"%s\r\n",rec_sort_ram[0].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[1].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[2].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[3].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[4].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[5].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[6].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[7].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[8].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[9].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[10].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[11].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[12].plyr_name);
////	fprintf(stderr,"%s\r\n",rec_sort_ram[13].plyr_name);

//	set_text_position(x1, hiscr_ys[1][four_plr_ver], 3.0F);
	set_text_position(SPRITE_HRES / 2, hiscr_ys[1][four_plr_ver], 3.0F);
	if (calc_tms_def(rec_ram[0].teams_defeated) < NUM_TEAMS)	// && 0)
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
	else
	{
//		oprintf("%dj%dc%1.*s", (HJUST_CENTER|VJUST_CENTER), WHITE, LETTERS_IN_NAME, rec_ram[0].plyr_name);
		i = LETTERS_IN_NAME;
		while (i--) buffer[i] = rec_ram[0].plyr_name[i];		// rec_sort_ram[8].plyr_name[i]
		i = LETTERS_IN_NAME;
		do buffer[i] = 0; while (buffer[--i] == ' ');
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_GREEN, buffer);
	}

//	set_text_position(x1, hiscr_ys[4][four_plr_ver], 3.0F);
	set_text_position(SPRITE_HRES / 2, hiscr_ys[4][four_plr_ver], 3.0F);
	if (calc_tms_def(rec_ram[1].teams_defeated) < NUM_TEAMS)	// && 0)
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
	else
	{
//		oprintf("%dj%dc%1.*s", (HJUST_CENTER|VJUST_CENTER), WHITE, LETTERS_IN_NAME, rec_ram[1].plyr_name);
		i = LETTERS_IN_NAME;
		while (i--) buffer[i] = rec_ram[1].plyr_name[i];		// rec_sort_ram[12].plyr_name[i]
		i = LETTERS_IN_NAME;
		do buffer[i] = 0; while (buffer[--i] == ' ');
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, buffer);
	}

//	set_text_position(x1, hiscr_ys[7][four_plr_ver], 3.0F);
	set_text_position(SPRITE_HRES / 2, hiscr_ys[7][four_plr_ver], 3.0F);
	if (calc_tms_def(rec_ram[2].teams_defeated) < NUM_TEAMS)	// && 0)
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, ".  .  .  .  .  .");
	else
	{
//		oprintf("%dj%dc%1.*s", (HJUST_CENTER|VJUST_CENTER), WHITE, LETTERS_IN_NAME, rec_ram[2].plyr_name);
		i = LETTERS_IN_NAME;
		while (i--) buffer[i] = rec_ram[2].plyr_name[i];		// rec_sort_ram[13].plyr_name[i]
		i = LETTERS_IN_NAME;
		do buffer[i] = 0; while (buffer[--i] == ' ');
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, buffer);
	}

	// Take care of proper handling of transition
	if (plates_on != PLATES_OFF)
	{
		// Re-create the wipeout'd plate objects
		create_plates();

		// Enable drawing since we know something is being or will be drawn
		draw_enable(1);

		if (plates_on != PLATES_ON)
		{
			int j = (plates_on == PLATES_COMING_ON) ? PLATES_ON : PLATES_OFF;
			// Wait for plates; set time-out so we don't hang
			i = PLATE_TRANSITION_TICKS * 2;
			// Do it this way so we can sync-up the sound
			while (plates_on != j && --i) sleep(1);
			// Do sound only if the transition finished, not the time-out
			if (i && j == PLATES_ON)
//				snd_scall_bank(plyr_bnk_str, P_HARD_HIT2_SND, VOLUME4, 127, PRIORITY_LVL5);
				snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
			// Make sure plates are on
			plates_on = j;
			sleep(1);
		}
		if (plates_on == PLATES_ON)
		{	
			// Turn off the plates
			iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		}
	}

	// Enable drawing
	draw_enable(1);

	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 1, 1, 0);
	
	//
	// Hi-score page 2 (greatest players...win %)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[2], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[3], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[1]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_percent);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%d WINS", rec_ram[i].games_won);
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i][four_plr_ver], 3.0F);
		if (rec_ram->games_played == 0)
			hstrings[2][i] = oprintf("%dj.%d", (HJUST_RIGHT|VJUST_CENTER), ((rec_ram->games_won * 1000)/50));	// make low
		else if (rec_ram->games_won != rec_ram->games_played)	
			hstrings[2][i] = oprintf("%dj.%d", (HJUST_RIGHT|VJUST_CENTER), ((rec_ram->games_won * 1000)/rec_ram->games_played));
		else	
			hstrings[2][i] = oprintf("%dj1.000", (HJUST_RIGHT|VJUST_CENTER));
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
	//
	// Hi-score page 3 (biggest winners - most games won)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[4], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[5], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[2]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_wins);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%d WINS", rec_ram[i].games_won);
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position((rec_ram->games_won == 1 ? (SPRITE_HRES * 0.9f - wid_s):(SPRITE_HRES * 0.9f)), hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%d WIN%c", (HJUST_RIGHT|VJUST_CENTER), rec_ram->games_won, (rec_ram->games_won == 1 ? '\0':'S'));
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);

	//
	// Hi-score page 4 (best offensive players)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[6], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[7], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[3]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_scored);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%.2f PTS", (float)rec_ram[i].points_scored/(float)rec_ram[i].games_played);	// 3.2f
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%.2f PTS", (HJUST_RIGHT|VJUST_CENTER), (float)rec_ram->points_scored/(float)rec_ram->games_played);	//3.2f
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
	//
	// Hi-score page 5 (best defensive players)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[8], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[9], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[4]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_points_allowed);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%.2f PTS", (float)rec_ram[i].points_allowed/(float)rec_ram[i].games_played);	//d%d ?
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%.2f PTS", (HJUST_RIGHT|VJUST_CENTER), (float)rec_ram->points_allowed/(float)rec_ram->games_played);	//d%d ?
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
	//
	// Hi-score page 6 (best rushers)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[10], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[11], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[5]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_rushing_yds);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%.2f YDS", (float)rec_ram[i].rushing_yds/(float)rec_ram[i].games_played);	// d ?
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%.2f YDS", (HJUST_RIGHT|VJUST_CENTER), (float)rec_ram->rushing_yds/(float)rec_ram->games_played);	// d ?
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
	//
	// Hi-score page 7 (best passers)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[12], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[13], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[6]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_passing_yds);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%.2f YDS", (float)rec_ram[i].passing_yds/(float)rec_ram[i].games_played);
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%.2f YDS", (HJUST_RIGHT|VJUST_CENTER), (float)rec_ram->passing_yds/(float)rec_ram->games_played);
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);

	//
	// Hi-score page 8 (consecutive wins)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[14], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[15], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[7]);

	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_win_streaks);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%d WINS", rec_ram[i].winstreak);
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position((rec_ram->winstreak == 1 ? (SPRITE_HRES * 0.9f - wid_s):(SPRITE_HRES * 0.9f)), hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%d WIN%c", (HJUST_RIGHT|VJUST_CENTER), rec_ram->winstreak, (rec_ram->winstreak == 1 ? '\0':'S'));
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);

	//
	// Hi-score page 9 (most games played)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[16], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[17], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[8]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_games_played);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%d GAMES", rec_ram[i].games_played);
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%d GAMES", (HJUST_RIGHT|VJUST_CENTER), rec_ram->games_played);
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
	//
	// Hi-score page 10 (QB rating)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[18], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[19], 0.0f-1, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[9]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_qb_rating);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%.1f",
			 pass_rating(rec_ram[i].pass_attempts, rec_ram[i].pass_completions, rec_ram[i].passing_yds,
				 rec_ram[i].pass_touchdowns, rec_ram[i].intercepted) );
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
	
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position(SPRITE_HRES * 0.9f, hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%.1f", (HJUST_RIGHT|VJUST_CENTER),
			 pass_rating(rec_ram->pass_attempts, rec_ram->pass_completions, rec_ram->passing_yds,
				 rec_ram->pass_touchdowns, rec_ram->intercepted) );
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);
	
	//
	// Hi-score page 11 (world records)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[20], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[21], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[10]);
	
	// print world records
	set_text_font(FONTID_BAST10);
	for (i=0; i < NUM_WORLD_RECORDS; i++)
	{
		set_text_position(wrld_rec_xs[i][four_plr_ver], wrld_rec_title_ys[i][four_plr_ver], 3.0F);
		strobj = oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, world_record_titles[i]);
	}	
	set_text_font(fid);	//FONTID_BAST23);
	for (i=0; i < NUM_WORLD_RECORDS; i++)
	{
		get_generic_record(WORLD_RECS_ARRAY,i,&tmp_world_rec);							// get cmos record into ram
		set_text_position(wrld_rec_xs[i][four_plr_ver]-30.0f, wrld_rec_ys[i][four_plr_ver], 3.0F);
		oprintf("%dj%dc%1.*s", (HJUST_CENTER|VJUST_CENTER), WHITE, LETTERS_IN_NAME, tmp_world_rec.plyr_name);
		set_text_position(wrld_rec_xs[i][four_plr_ver]+70.0f, wrld_rec_ys[i][four_plr_ver], 3.0F);
		oprintf("%dj%dc%d", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, tmp_world_rec.value);
	}

	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 1, 1, 0);

	//
	// Hi-score page 12 (trivia masters)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[22], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[23], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[11]);
	
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	qsort(rec_ram,NUM_PLYR_RECORDS,sizeof(plyr_record_t),(int (*)(const void *, const void *))sort_trivia);
	rec_ram = rec_sort_ram;											// point to begining of allocated 'record sort' ram
	
	set_text_font(fid);	//FONTID_BAST23);
	x1 = 0.0f;
	x2 = 0.0f;
	for(i = 0; i < HI_SCORE_ENTRIES; i++)
	{
		sprintf(buffer, "#%d", i+1);
		if(get_string_width(buffer) > x1)
		{
			x1 = get_string_width(buffer);
		}
		sprintf(buffer, "%d PTS", rec_ram[i].trivia_pts);
		if(get_string_width(buffer) > x2)
		{
			x2 = get_string_width(buffer);
		}
	}
	x2 = (SPRITE_HRES * 0.9f) - x2;
	x1 = (SPRITE_HRES * 0.1f) + x1;
	x1 += x2;
	x1 /= 2.0f;
	for (i=0; i < HI_SCORE_ENTRIES; i++,rec_ram++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.1f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);
		
		// print name
		set_text_position(x1, hiscr_ys[i][four_plr_ver], 3.0F);
		sprintf(buffer, "%1.*s", LETTERS_IN_NAME, rec_ram->plyr_name);
		tmp = buffer + (LETTERS_IN_NAME - 1);
		while(*tmp == ' ' && tmp >= buffer)
		{
			--tmp;
		}
		if(tmp != &buffer[LETTERS_IN_NAME-1])
		{
			*(tmp+1) = 0;
		}
		hstrings[1][i] = oprintf("%dj%1.*s", (HJUST_CENTER|VJUST_CENTER), LETTERS_IN_NAME, buffer);
		
		// print value
		set_text_position((rec_ram->trivia_pts == 1 ? (SPRITE_HRES * 0.9f - wid_s):(SPRITE_HRES * 0.9f)), hiscr_ys[i][four_plr_ver], 3.0F);
		hstrings[2][i] = oprintf("%dj%d PT%c", (HJUST_RIGHT|VJUST_CENTER), rec_ram->trivia_pts, (rec_ram->trivia_pts == 1 ? '\0':'S'));
	}	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 0, 0);

	//
	// Hi-score page 13 (most popular teams)
	//
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[24], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	hiscr_hdr_obj = beginobj(hiscr_hdr_imgs[25], 0.0f, SPRITE_VRES, 49.0f, HISCORE_PG_TID);
	hiscr_hdr_obj->id = 2;
	
	set_string_id(5);
	set_text_font(FONTID_BAST10);
	set_text_position(sub_hdr_xys[four_plr_ver][0], sub_hdr_xys[four_plr_ver][1], 3.0F);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, hi_scr_subhdrs[12]);

	// Fill in the popular team info structure
	for(i = 0; i < NUM_TEAMS; i++)
	{
		pop_team[i].team_number = i;
		if (get_audit(ARIZONA_CARDINALS_AUD+i, &pop_team[i].times_choosen))
			pop_team[i].times_choosen = 0;
	}

	// Sort the popular teams array
	qsort(pop_team, NUM_TEAMS, sizeof(popular_team_info_t), pop_team_sort);
	
	set_text_font(fid);	//FONTID_BAST23);

	// Show the first 10 popular teams
	for(i = 0; i < 10; i++)
	{
		// print rank
		set_text_position(SPRITE_HRES * 0.02f, hiscr_ys[i][four_plr_ver], 3.0f);
		hstrings[0][i] = oprintf("%dj#%d", (HJUST_LEFT|VJUST_CENTER), i+1);

		// build team name
		tmp = buffer;

		tmp1 = (char *)(teaminfo[pop_team[i].team_number].home);
		do *tmp++ = *tmp1; while (*tmp1++);
		*(tmp - 1) = ' ';

		tmp1 = (char *)(teaminfo[pop_team[i].team_number].name);
		do *tmp++ = *tmp1; while (*tmp1++);

		// print team name
		set_text_position(SPRITE_HRES / 2.0f, hiscr_ys[i][four_plr_ver], 3.0F);
//		hstrings[1][i] = oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), hs_team_names[pop_team[i].team_number]);
		hstrings[1][i] = oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), buffer);
		hstrings[2][i] = NULL;
	}
	
	// (ticks, no_cyc, no_hilite, no_del)
	do_simple_cycle(HISCORE_TICKS, 0, 1, 1);

	// Done. Reset & cleanup
	set_text_z_inc(0.0f);

	if (!att_keeper)
		del1c(1,0xffffffff);			// delete background
	
	JFREE(rec_sort_ram);				// free the memory used for the sorting of records
	rec_sort_ram = NULL;
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void rnd_field_stadium(void);

void do_amode_play( int min_time, int max_time )
{
	int i;

	// clear user game configuration
	clear_user_config();

	// set random field & stadium
	rnd_field_stadium();

	// clear heads
	game_info.team_head[0] = -1;
	game_info.team_head[1] = -1;
	game_info.team_head[2] = -1;
	game_info.team_head[3] = -1;

	// Turn on the plates 
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way so we can sync-up the sound
	while (plates_on != PLATES_ON && --i) sleep(1);
	// Do sound only if the transition finished, not the time-out
//	if (i) snd_scall_bank(plyr_bnk_str, P_HARD_HIT2_SND, VOLUME3, 127, PRIORITY_LVL5);
	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

	// Disable here to go around the wipeout; <game_proc> does an enable
	draw_enable(0);

	wipeout();

	// Re-create the wipeout'd plate objects
	create_plates();

	game_info.team[0] = team_index;
	game_info.team[1] = randrng(NUM_TEAMS-1);
	if( game_info.team[1] >= game_info.team[0] )
		game_info.team[1] += 1;


	if (game_info.game_state != GS_ATTRACT_MODE)
		die(0);

	launch_game_proc();

	// Don't do anything till <game_proc> says its done initing; first wait till it starts
	while (game_info.game_mode != GM_INITIALIZING) sleep(1);
	while (game_info.game_mode == GM_INITIALIZING) sleep(1);

	i = wait_for_any_butn_press(min_time,max_time);

	// Timer done - now just wait for a play to end but not on a score
	while (!i)
	{
		while (game_info.game_mode != GM_IN_PLAY && !i)
			i = wait_for_any_butn_press(0, 1);

		while (game_info.game_mode != GM_PLAY_OVER && !i)
			i = wait_for_any_butn_press(0, 15);

		if (	game_info.play_result != PR_TOUCHDOWN  &&
				game_info.play_result != PR_SAFETY     &&
				game_info.play_result != PR_PAT_SAFETY &&
				game_info.play_result != PR_FIELD_GOAL &&
				game_info.play_result != PR_EXTRA_POINT)
			break;
	}

	fade(0.0f, 30, 0);		//0=full off
	sleep(30);

	wipeout();
	snd_stop_all();

	sleep(1);
}

//-------------------------------------------------------------------------------------------------
//  		This process displays the NFL BLITZ logo and says please wait
//-------------------------------------------------------------------------------------------------
/*
void start_page(int min_time, int max_time)
{
float		bars_z; 
sprite_info_t *nfl_obj;
sprite_info_t *blitz_obj;

	if (load_textures_into_tmu(start_textures_tbl)) 
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all START GAME textures into tmu\r\n");
		lockup();
#else
		while(1) ;
#endif
	}
	
	bars_z = 5.0F;

	nfl_obj = beginobj(&nfl37, SPRITE_HRES/2, SPRITE_VRES-100, 10.0F, BARS_PG_TID);
	blitz_obj = beginobj(&blitz, SPRITE_HRES/2, SPRITE_VRES/2-30, 10.0F, BARS_PG_TID);

	snd_scall_direct(tick_sndn,VOLUME1,127,1);
	//snd_scall_bank(cursor_bnk_str,TICK_SND,255,0,1);

//	qcreate("cred_msg",CREDIT_PID,credit_msg,screen,0,0,0);
 	flash_obj_white(blitz_obj);

	wait_for_any_butn_press(20,tsec*20);
	delete_all_sprites();
	
	delete_multiple_textures(BARS_PG_TID,0xFFFFFFFF);
}
*/


//***************************************************************************//
//***************************************************************************//
//
//  The Movie Section
//
//***************************************************************************//
//***************************************************************************//

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void show_movies(int min_time, int max_time)
{
	static char	* firerun_movie[] = {
		"run_", 0
	};
	static char	* cheerleader_movie[] = {
		"chee", 0
	};
	static char	* hike_movie[] = {
		"hut_", 0
	};
	static char	** movies[] = {
		firerun_movie,
		cheerleader_movie,
		hike_movie,
	};
	char ** movie_to_play;

#if 0
#if defined(SEATTLE)
	// Movie test
	static char	* mov[] = {
		"chee", "dnce", "hut_", "in__",
		"mway", "nflf", "nfl_", "run_",
		0
	};
#else
	// Movie test
	static char	* mov[] = {
		"atts", "chee", "cyc_", "dnce", "end_", "eog1", "htim", "hut_", "in__", "mdwy",
		"mway", "nbcs", "nflf", "nfl_", "qtr1", "qtr2", "qtr3", "qtr4", "run_", "shwt",
		"star", "tphy", "tra1", 0
	};
#endif
	movie_to_play = mov;
	movie_to_play--;
	while (*++movie_to_play)
	{
		fade(1, 1, 0);
#else
	// Pick random selection to play; skip if 0
	movie_to_play = movies[randrng(sizeof(movies)/sizeof(void *))];

	if (movie_to_play)
	{
#endif
		normal_screen();

		if(movie_to_play == cheerleader_movie)
		{
			movie_config_player(573.0f, 384.0f, -30.0f, 0.0f, 2.0f, 37);
		}
		else
		{
			movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 2.0f, 37);
		}
		qcreate("mplay", 0, movie_proc, (int)movie_to_play, 0, 0, 0);

		// Make 'em watch it for at least 1/2 second
		sleep(30);

		// As long as the movie is playing and the player has NOT pressed a button
		// do nothing
		while(!(get_player_sw_current() & ALL_ABCD_MASK) &&
			!(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) &&
			(movie_get_state() > 1))
		{
			sleep(1);
		}

		// Fade the movie
		fade(0.0f, 5, NULL);
		sleep(5);

		// Make a beep if aborted
		if(movie_get_state() > 1)
		{
			snd_scall_direct(tick_sndn,VOLUME1,127,1);
			//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
		}

		// Make sure the movie player is shut down if it is running
		// This happens when the player whacks through the movie
		if(movie_get_state() > 1)
		{
			movie_abort();
		}
	}
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
//void show_nfl_movie(int min_time, int max_time)
//{
//	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 2.0f, 30);
//	qcreate("mplay", 0, movie_proc, (int)&nflf_movie, 0, 0, 0);
//
//	fade(1.0, 20, NULL);
//
//	// Make 'em watch it for at least 1/2 second
//	sleep(30);
//
//	// As long as the movie is playing and the player has NOT pressed a button
//	// do nothing
//	while(!(get_player_sw_current() & ALL_ABCD_MASK) &&
//			!(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) &&
//			(movie_get_state() > 1))
//	{
//		sleep(1);
//	}
//
//	// Fade the last frame shown
//	fade(0.0f, 5, NULL);
//	sleep(5);
//
//	// Make a beep if aborted
//	if(movie_get_state() > 1)
//	{
//		snd_scall_direct(tick_sndn,VOLUME1,127,1);
//		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
//	}
//
//	// Make sure the movie player is shut down if it is running
//	// This happens when the player whacks through the movie
//	if(movie_get_state() > 1)
//	{
//		movie_abort();
//	}
//}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
//void show_nfl_movie2(int min_time, int max_time)
//{
//	int		asound;
//
//	// kill all sound here
//	snd_stop_all();
//
//	get_adjustment(ATTRACT_SOUND_ADJ, &asound);
//
//	/* init the streaming audio */
//	/* this also pre-loads the audio buffers */
//	if (asound)
//	{
//		stream_set_volume (255);
//		stream_init ("blitz99.snd");
//	}
//
//	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 2.0f, 30);
//
//	qcreate("mplay", 0, movie_proc, (int)&nfl_movie2, 0, 0, 0);
//
//	sleep (1);
//
//	// start the audio
//	if (asound)
//		stream_start();
//
//	sleep (9);
//
//	/* the movie player will return a 3 when it is preloaded */
//	/* and actually sending frames to the screen */	
//	while (movie_get_state() != 3)
//	{
//
//		if (movie_get_state() == MOVIE_STATE_DONE)
//			break;
//
//		if (game_info.game_mode != GS_ATTRACT_MODE) 
//			break;
//
//		sleep (1);
//
//	}
//
//	fade(1.0, 10, NULL);
//
//	// Make 'em watch it for at least 1/2 second
//	sleep(30);
//
//	// As long as the movie is playing and the player has NOT pressed a button
//	// do nothing
//	while(!(get_player_sw_current() & ALL_ABCD_MASK) &&
//		!(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) &&
//		(movie_get_state() > 1))
//		sleep(1);
//
//	// Fade the last frame shown
//	fade(0.0f, 5, NULL);
//	sleep(5);
//
//	// Make a beep if aborted
//	if (movie_get_state() > 1)
//	{
//		snd_scall_direct(tick_sndn,VOLUME1,127,1);
//		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
//	}
//
//	// Make sure the movie player is shut down if it is running
//	// This happens when the player whacks through the movie
//	if (movie_get_state() > 1)
//	{
//		movie_abort();
//	}
//
//	/* stop the streaming audio */
//	if (asound)
//	{
//		stream_stop();
//		/* wait for all streaming to be done */
//		// 11 June 98 MVB
//		// make sure we at least sleep 1 here
//		do { sleep (1); } while (stream_check_active());
//		stream_release_key();
//	}
//}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
//void show_midway_logo_movie(int min_time, int max_time)
//{
//	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 2.0f, 30);
//	qcreate("mplay", 0, movie_proc, (int)&midway_logo_movie, 0, 0, 0);
//
//	fade(1.0, 20, NULL);
//
//	draw_enable(1);
//
//	// Make 'em watch it for at least 1/2 second
//	sleep(30);
//
//	// As long as the movie is playing and the player has NOT pressed a button
//	// do nothing
//	while(!(get_player_sw_current() & ALL_ABCD_MASK) &&
//			!(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) &&
//			(movie_get_state() > 1))
//	{
//		sleep(1);
//	}
//
//	// Fade the last frame shown
//	fade(0.0f, 5, NULL);
//	sleep(5);
//
//	// Make a beep if aborted
//	if(movie_get_state() > 1)
//	{
//		snd_scall_direct(tick_sndn,VOLUME1,127,1);
//		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
//	}
//
//	// Make sure the movie player is shut down if it is running
//	// This happens when the player whacks through the movie
//	if(movie_get_state() > 1)
//	{
//		movie_abort();
//	}
//}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
//void show_nfl_logo_movie(int min_time, int max_time)
//{
//	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 2.0f, 30);
//	qcreate("mplay", 0, movie_proc, (int)&nfl_logo_movie, 0, 0, 0);
//
//	fade(1.0, 20, NULL);
//
//	// Make 'em watch it for at least 1/2 second
//	sleep(30);
//
//	// As long as the movie is playing and the player has NOT pressed a button
//	// do nothing
//	while(!(get_player_sw_current() & ALL_ABCD_MASK) &&
//			!(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) &&
//			(movie_get_state() > 1))
//	{
//		sleep(1);
//	}
//
//	// Fade the last frame shown
//	fade(0.0f, 5, NULL);
//	sleep(5);
//
//	// Make a beep if aborted
//	if(movie_get_state() > 1)
//	{
//		snd_scall_direct(tick_sndn,VOLUME1,127,1);
//		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
//	}
//
//	// Make sure the movie player is shut down if it is running
//	// This happens when the player whacks through the movie
//	if(movie_get_state() > 1)
//	{
//		movie_abort();
//	}
//}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void show_nfl_logo_movie(int min_time, int max_time)
{
	static char	* nfl_logo_movie[] = {
		"nfl_", 0
	};
	show_attract_movie(nfl_logo_movie,  NULL, 1, FALSE, 0x000000, 0, FALSE, TRUE);
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void show_midway_logo_movie(int min_time, int max_time)
{
	static char	* midway_logo_movie[] = {
		"mway", 0
	};
	static char midway_logo_movie_snd_bnk_str[] = "mdwylogo";

	static sndinfo midway_logo_movie_snds[] = {
		{midway_logo_movie_snd_bnk_str, 0, 255, 127, PRIORITY_LVL5},
		{NULL}
	};
	show_attract_movie(midway_logo_movie,  midway_logo_movie_snds, 1, FALSE, 0x000000, 12, FALSE, TRUE);
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
//void show_root_movie(int min_time, int max_time)
//{
//	static char	* root_movie[] = {
//		"rd__", 0
//	};
//	static char root_movie_snd_bnk_str[] = "movie";
//
//	static sndinfo root_movie_snds[] = {
//		{root_movie_snd_bnk_str, 0, VOLUME4, 127, PRIORITY_LVL5},
//		{NULL}
//	};
////	show_attract_movie(b2k_movie, b2k_movie_snds, 1, FALSE, 0x000000, 140 /*150 / 120*/, FALSE, TRUE);
////	show_attract_movie(b2k_movie, b2k_movie_snds, 1, FALSE, 0x000000, 10 /*150 / 120*/, FALSE, TRUE);
//	show_attract_movie(root_movie, root_movie_snds, 1, FALSE, 0x000000, 10 /*150 / 120*/, FALSE, TRUE);
//}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void show_b2k_movie(int min_time, int max_time)
{
	static char	* b2k_movie[] = {
		"in__", 0
	};
#if defined(VEGAS)
	static char b2k_movie_snd_bnk_str[] = "movie";
#else
	static char b2k_movie_snd_bnk_str[] = "movie1";
#endif

	static sndinfo b2k_movie_snds[] = {
		{b2k_movie_snd_bnk_str, 0, VOLUME4, 127, PRIORITY_LVL5},
		{NULL}
	};
//	show_attract_movie(b2k_movie, b2k_movie_snds, 1, FALSE, 0x000000, 140 /*150 / 120*/, FALSE, TRUE);
//fprintf(stderr, "MOVIE> in\n");
	show_attract_movie(b2k_movie, b2k_movie_snds, 1, FALSE, 0x000000, 10 /*150 / 120*/, FALSE, TRUE);
//fprintf(stderr, "MOVIE> out\n");
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void show_blitz_movie(int min_time, int max_time)
{
	static char	* blitz_movie[] = {
		"nflf", 0
	};
//	static char b2k_movie_snd_bnk_str[] = "movie";

//	static sndinfo b2k_movie_snds[] = {
//		{b2k_movie_snd_bnk_str, 0, VOLUME4, 127, PRIORITY_LVL5},
//		{NULL}
//	};
	show_attract_movie(blitz_movie, NULL/*b2k_movie_snds*/, 1, FALSE, 0x000000, 0, FALSE, TRUE);
}

//-------------------------------------------------------------------------------------------------
// show_attract_movie - 
//-------------------------------------------------------------------------------------------------
void show_attract_movie(char **movie, sndinfo *sounds, int allow_btn, int use_chroma_key, int ckey, int sound_delay, int wait_for_start, int kill_sound)
{
	int	wait_time = 120;		// 2 seconds
	int hold_count;
	int movie_sound, attract_sound;
	
	get_adjustment(MOVIE_SND_ADJ, &movie_sound);
	if (movie_sound) {
		get_adjustment(ATTRACT_SOUND_ADJ, &attract_sound);
		if (!attract_sound) {
			snd_bank_lockout(FALSE);
			movie_sound = -1;
		}
	}
		
	hold_count = use_chroma_key ? 0 : 30;
	movie_config_player(512.0f, 384.0f, 0.0f, 0.0f, 1.0f, 30);

	qcreate("mplay", 0, movie_proc, (int)movie, 0, hold_count, FALSE);

	fade(1.0, 20, NULL);

	if (use_chroma_key || wait_for_start) {
		draw_enable(0);
		while (--wait_time > 0) {
			if (movie_get_state() == MOVIE_STATE_RUN) {
				if (use_chroma_key) {
					movie_set_chromakey_mode(TRUE); 
					movie_set_chromakey(ckey);
				}
				break;
			}
			sleep(1);
		}
		draw_enable(1);
	}

	// Don't play sound if movie sound is disabled or attract volume = 0
	if (movie_sound != 0) {
		get_adjustment(ATTRACT_VOLUME_LEVEL_ADJ, &attract_sound);
		if (sounds && sounds->bank_name && attract_sound) {
			if (snd_bank_load(sounds->bank_name) == 0xeeee)
				increment_audit(SND_LOAD_FAIL_AUD);
			snd_set_reserved(SND_TRACK_0);
			if (sound_delay > 0)
				sleep(sound_delay);
			snd_scall_bank(sounds->bank_name,
				sounds->scall,
			    sounds->volume,
			    sounds->pan,
			    sounds->priority);
		}
	}

	// Make 'em watch it for at least 1/2 second
	draw_enable(1);
	sleep(30);

	// As long as the movie is playing and the player has NOT pressed a button
	// do nothing
	if (!allow_btn)
	{
		while(movie_get_state() > 1)
		{
			if (movie_get_state() == MOVIE_STATE_HOLD) {
				fade(0.0f, 30, NULL);
				sleep(30);
			}
			sleep(1);
		}
	}
	else
	{
		while(!(get_player_sw_current() & (P1_ABCD_MASK|P2_ABCD_MASK|P3_ABCD_MASK|P4_ABCD_MASK)) &&
				!(get_dip_coin_current() & (P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)) &&
				(movie_get_state() > 1))
		{
			if (movie_get_state() == MOVIE_STATE_HOLD) {
				fade(0.0f, 30, NULL);
				sleep(30);
			}
			sleep(1);
		}
	}

	if (((movie_get_state() > 1) || kill_sound) && sounds && sounds->bank_name)
	{
		snd_stop_all();
		snd_set_reserved(SND_TRACK_0);
		snd_bank_delete(sounds->bank_name);
	}

	// Make a beep if aborted
	if(movie_get_state() > 1)
	{
		snd_scall_direct(tick_sndn,VOLUME2,127,1);
		//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
	}

	// Make sure the movie player is shut down if it is running
	// This happens when the player whacks through the movie
	if(movie_get_state() > 1)
	{
		movie_abort();
	}

	if (movie_sound == -1)
		snd_bank_lockout(TRUE);
}
//***************************************************************************//
//***************************************************************************//

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void cheer_page(int min_time, int max_time)
{
	sprite_info_t *bg;

	wipeout();

	if (rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}
	bg = beginobj(&cheer,    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bg = beginobj(&cheer_c1, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bg = beginobj(&cheer_c2, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bg = beginobj(&cheer_c3, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);

	draw_enable(1);	

	fade(1.0, 20, NULL);

	wait_for_any_butn_press(min_time, max_time);


	fade(0.0f, 30, 0);		//0=full off
	sleep(30);

//	draw_enable(0);			
	// clean_up:
	delete_all_sprites();

	// delete background
	del1c(0,0xFFFFFFFF);

}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void load_title_flash(int tex, int snd)
{
	if (snd)
	{
		coin_sound_holdoff(TRUE);

		// load flash sound
		if (snd_bank_load_playing("intro1") == 0xeeee)
			increment_audit(SND_LOAD_FAIL_AUD);

		// track 0 is the tune, track 1 is the audience, track 5 is the announcer
		snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

		coin_sound_holdoff(FALSE);
	}

	if (tex)
	{
		static tmu_ram_texture_t flash_texture_tbl[] = {
			{"gs_fl00.wms", JAPPLE_BOX_TID},
			{"gs_fl01.wms", JAPPLE_BOX_TID},
			{NULL, 0},
		};

		// Enable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif

		if (load_textures_into_tmu(flash_texture_tbl))
		{
			fprintf(stderr, "Couldn't load all sweeps textures into tmu\r\n");
#ifdef DEBUG
			lockup();
#endif
		}

		// Disable the disk interrupt callback
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void free_title_flash(void)
{
	snd_bank_delete("intro1");
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void title_flash(int *args)
{
	sprite_info_t *fl_obj;
	image_info_t *flash_imgs[] = {
		&gselfl01, &gselfl02, &gselfl03, &gselfl04, &gselfl05,
		&gselfl06, &gselfl07, &gselfl08, &gselfl09, &gselfl10,
		&gselfl11, &gselfl12, &gselfl13, &gselfl14, &gselfl15,
	};
	int cnt;
	int img_cnt;
	int x, y;
	float scale;


	snd_scall_bank("intro1",2,VOLUME4, 127,PRIORITY_LVL3);


	x = args[0];
	y = args[1];
	scale = args[2] / 100.0f;

	img_cnt = (sizeof(flash_imgs) / sizeof(image_info_t *));

	fl_obj = beginobj(flash_imgs[0], x, y, 1.0f, JAPPLE_BOX_TID);
	fl_obj->id = 1;
	fl_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	fl_obj->state.texture_filter_mode = GR_TEXTUREFILTER_BILINEAR;
	fl_obj->w_scale = scale;
	fl_obj->h_scale = scale;
	generate_sprite_verts(fl_obj);

	for (cnt=0; cnt < img_cnt; cnt++)
	{
		change_img_tmu(fl_obj, flash_imgs[cnt], JAPPLE_BOX_TID);
		if (cnt & 1)
			sleep(3);
		else 
			sleep(2);		
	}
	delobj(fl_obj);
}

//--------------------------------------------------------------------------------------------[EOF]