/****************************************************************************/
/*                                                                          */
/* trivia.c        Trivia Contest for Blitz 99								*/
/*                                                                          */
/* Written by:  Jeff "Japple" Johnson                                       */
/*                                                                          */
/* Copyright (c) 1998 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <goose/goose.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/support.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/sysfont.h"
#include "/Video/Nfl/Include/Record.h"
#include "/Video/Nfl/Include/plaq.h"

#define	IMAGEDEFINE	1
#include "/Video/Nfl/Include/trivia.h"


// "localset.mak" -D's looked at here:
//
//  DBG_TC - Debug Trivia Contest
//  CHK_TC - Chk all questions from 0 to MAX_QUESTIONS; superceded by DBG_TC

#ifdef DBG_TC
#define DBGCHK
#define D_QNUM
#undef  CHK_TC
#endif

#ifdef CHK_TC
#define DBGCHK
#define D_QNUM
#endif

#ifdef DEBUG
#define D_QNUM
#endif


// LOCAL DEFINES

#define	MAX_QUESTIONS	930
#define	MAX_ANSWERS		4
#define	MAX_LINELEN		350
#define	MAX_LINES		6

#define TIME_TOTAL		13
#define TIME_TIMER		5
#define TIME_X			(SPRITE_HRES/2.0f)
#define TIME_Y			(54.0f *SPRITE_VRES/384.0f)

#define FILE_NAME		"Q%02d.TXT"
#define FILE_ITEMS		100
#define	FILE_COUNT		((MAX_QUESTIONS+FILE_ITEMS-1)/FILE_ITEMS)

#define	LEN_VALID		(sizeof(szvalid)-1)
#define	LEN_INVALID		(sizeof(szinvalid)-1)
#define	LEN_CORRECT		(sizeof(szcorrect)-1)
#define	LEN_INCORRECT	(sizeof(szincorrect)-1)

///

#define BK_X		(0.0f)
#define BK_Y		(SPRITE_VRES)
#define BK_Z		(500.0f)

#define HDR_X		(50.0f  *SPRITE_HRES/512.0f)
#define HDR_Y		(363.0f *SPRITE_VRES/384.0f)
#define HDR_Z		(-1.0f  +BK_Z)


#define QUEST_X		(74.0f  *SPRITE_HRES/512.0f)
#define QUEST_YMID	(249.0f *SPRITE_VRES/384.0f)
#define QUEST_Z		(10.0f)
#define QUEST_W		(370.0f *SPRITE_HRES/512.0f)

#define OPTB_XOFF	(+SPRITE_HRES/2.0f)
//#define OPTB_XMIN	(188.0f *SPRITE_HRES/512.0f)
#define OPTB_Y		(183.0f *SPRITE_VRES/384.0f)
#define OPTB_Z		(HDR_Z)
#define OPTB_WOFF	(+32.0f)
//#define OPTB_WOFF	(+20.0f)
//#define OPTB_WMIN	(126.0f)
#define OPTB_H		(114.0f)

#define OPT1_XOFF	(0.5f    *OPTB_WOFF)
#define OPT1_Y		(-12.0f  +OPTB_Y)
#define OPT1_Z		(-200.0f +OPTB_Z)
#define OPT1_WMIN	(80.0f)
#define OPT1_C		(0xffc0ff30)//LT_YELLOW


#define CURS_XOFF	(2.0f)
//#define CURS_XOFF	(-6.0f)
#define CURS_Y		( 3.0f  +OPT1_Y)
#define CURS_Z		(-1.0f  +QUEST_Z)

///

#define Q_GOOD	0
#define Q_IVAL	1
#define Q_FERR	2

#define RGB_BLUE	0
#define RGB_GREEN	1
#define RGB_RED		2

#define P_U		(P1_UP|   P2_UP|   P3_UP|   P4_UP)
#define P_L		(P1_LEFT| P2_LEFT| P3_LEFT| P4_LEFT)
#define P_R		(P1_RIGHT|P2_RIGHT|P3_RIGHT|P4_RIGHT)
#define P_LR	(P_L|P_R)
#define P_RV	(~P_LR)
#define P_S		(P1_START_SW|P2_START_SW|P3_START_SW|P4_START_SW)


//typedef unsigned char BYTE;
typedef unsigned int  UINT;

// routine declartions

void test_trivia_contest(int min, int max);
int trivia_contest_init(void);
void do_trivia_sp_proc(int * args);
void trivia_contest(void);
static void trivia_cursor_proc(int *args);
static int select_question(void);
static void trivia_plaq_proc(int * args);


// global externs

extern plyr_record_t tmp_plyr_record;

extern float station_xys[MAX_PLYRS][2][2];
extern float japbx_cntr_xys[MAX_PLYRS][2][2];
extern short force_selection;
extern short exit_status;
extern short screen;

extern font_info_t bast13_font;

// static ram

static int allow_me;			// Flag bits of players to allow in challenge
static int award_me;			// Flag bits of players to award pts to

static int qnum = 0;
static int anum;
static int acnt;
static int amax;
static int pval;

static float que_yinc[] = { 18.0f, 12.0f };		// mid, low
static float ans_yinc[] = { 20.0f, 21.0f };		// mid, low

static char szvalid[]     = "##";
static char szinvalid[]   = "!!";
static char szcorrect[]   = "**";
static char szincorrect[] = "~~";

static char    szq[MAX_LINELEN];
static char    sza[MAX_LINELEN];
static char * pszq[MAX_LINES];
static char * psza[MAX_ANSWERS];

static tmu_ram_texture_t trivia_textures_tbl[] = {
 {"trivbk00.wms",BKGRND_TID},
 {"trivbk01.wms",BKGRND_TID},
 {"trivbk02.wms",BKGRND_TID},

 {"trivia00.wms",JAPPLE_BOX_TID},
 {"trivia01.wms",JAPPLE_BOX_TID},
 {"trivia02.wms",JAPPLE_BOX_TID},

 {"plqcrd00.wms",JAPPLE_BOX_TID},
 {"plqcrd01.wms",JAPPLE_BOX_TID},

 {"plqfsh.wms",JAPPLE_BOX_TID},
 {NULL,0}
};

static image_info_t * plaq_flsh_imgs[] = {
 &flshbx01, &flshbx01, &flshbx02, &flshbx03, &flshbx04,
 &flshbx05, &flshbx06, &flshbx07, &flshbx08, &flshbx09
};

static image_info_t * ii_trivia_curs[] = {
 &plyr1,
 &plyr2,
 &plyr3,
 &plyr4
};


#ifdef DBG_TC
static int testing;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void test_trivia_contest(int min, int max)
{
	int rec_num;

	int rec_me = record_me;
	int gq = game_info.game_quarter;
	char p0 = plyr_data_ptrs[0]->plyr_name[0];
	char p1 = plyr_data_ptrs[1]->plyr_name[0];
	char p2 = plyr_data_ptrs[2]->plyr_name[0];
	char p3 = plyr_data_ptrs[3]->plyr_name[0];
	int q0 = qtr_pstatus[0];
	int q1 = qtr_pstatus[1];
	int q2 = qtr_pstatus[2];
	int q3 = qtr_pstatus[3];

	record_me = 0;
	game_info.game_quarter = 4;
	plyr_data_ptrs[0]->plyr_name[0] = 0;
	plyr_data_ptrs[1]->plyr_name[0] = 0;
	plyr_data_ptrs[2]->plyr_name[0] = 0;
	plyr_data_ptrs[3]->plyr_name[0] = 0;
	qtr_pstatus[0] = 0;
	qtr_pstatus[1] = 0;
	qtr_pstatus[2] = 0;
	qtr_pstatus[3] = 0;

	// any switch to be there
	if (get_player_sw_current() & P1_SWITCH_MASK)
		qtr_pstatus[0] = 1<<(game_info.game_quarter-1);
	if (get_player_sw_current() & P2_SWITCH_MASK)
		qtr_pstatus[1] = 1<<(game_info.game_quarter-1);
	if (get_player_sw_current() & P3_SWITCH_MASK && four_plr_ver)
		qtr_pstatus[2] = 1<<(game_info.game_quarter-1);
	if (get_player_sw_current() & P4_SWITCH_MASK && four_plr_ver)
		qtr_pstatus[3] = 1<<(game_info.game_quarter-1);

	// any button to be initialed
	if (get_player_sw_current() & P1_ABCD_MASK) {
		record_me |= 1;
		clear_cmos_record((char *)&player1_data,sizeof(plyr_record_t));
		strcpy(player1_data.plyr_name, "PLYR A1111");
		rec_num = find_record_in_cmos(player1_data.plyr_name);
//fprintf(stderr,"P1 rec # %d\r\n",rec_num);
		if (rec_num >= 0)
			get_generic_record(PLYR_RECS_ARRAY, rec_num, &player1_data);
		else {
			get_new_record_into_ram(player1_data.plyr_name);
			player1_data = tmp_plyr_record;
		}
	}

	if (get_player_sw_current() & P2_ABCD_MASK) {
		record_me |= 2;
		clear_cmos_record((char *)&player2_data,sizeof(plyr_record_t));
		strcpy(player2_data.plyr_name, "PLYR B1111");
		rec_num = find_record_in_cmos(player2_data.plyr_name);
//fprintf(stderr,"P2 rec # %d\r\n",rec_num);
		if (rec_num >= 0)
			get_generic_record(PLYR_RECS_ARRAY, rec_num, &player2_data);
		else {
			get_new_record_into_ram(player2_data.plyr_name);
			player2_data = tmp_plyr_record;
		}
	}

	if (get_player_sw_current() & P3_ABCD_MASK && four_plr_ver) {
		record_me |= 4;
		clear_cmos_record((char *)&player3_data,sizeof(plyr_record_t));
		strcpy(player3_data.plyr_name, "PLYR C1111");
		rec_num = find_record_in_cmos(player3_data.plyr_name);
//fprintf(stderr,"P3 rec # %d\r\n",rec_num);
		if (rec_num >= 0)
			get_generic_record(PLYR_RECS_ARRAY, rec_num, &player3_data);
		else {
			get_new_record_into_ram(player3_data.plyr_name);
			player3_data = tmp_plyr_record;
		}
	}

	if (get_player_sw_current() & P4_ABCD_MASK && four_plr_ver) {
		record_me |= 8;
		clear_cmos_record((char *)&player4_data,sizeof(plyr_record_t));
		strcpy(player4_data.plyr_name, "PLYR D1111");
		rec_num = find_record_in_cmos(player4_data.plyr_name);
//fprintf(stderr,"P4 rec # %d\r\n",rec_num);
		if (rec_num >= 0)
			get_generic_record(PLYR_RECS_ARRAY, rec_num, &player4_data);
		else {
			get_new_record_into_ram(player4_data.plyr_name);
			player4_data = tmp_plyr_record;
		}
	}

	if (qtr_pstatus[0] ||
		qtr_pstatus[1] ||
		qtr_pstatus[2] ||
		qtr_pstatus[3])
	{
		int gs = game_info.game_state;
		game_info.game_state = GS_GAME_OVER;
		testing = 1;

		if (trivia_contest_init())
			trivia_contest();

		game_info.game_state = gs;
		testing = 0;
	}

	record_me = rec_me;
	game_info.game_quarter = gq;
	plyr_data_ptrs[0]->plyr_name[0] = p0;
	plyr_data_ptrs[1]->plyr_name[0] = p1;
	plyr_data_ptrs[2]->plyr_name[0] = p2;
	plyr_data_ptrs[3]->plyr_name[0] = p3;
	qtr_pstatus[0] = q0;
	qtr_pstatus[1] = q1;
	qtr_pstatus[2] = q2;
	qtr_pstatus[3] = q3;
}
#endif //DBG_TC

//-----------------------------------------------------------------------------
// Returns: !0 if we wanna do this & a question has been selected
//          =0 otherwise
//-----------------------------------------------------------------------------
int trivia_contest_init(void)
{
	int qmsk = 1 << (game_info.game_quarter - 1);
//	int gmsk = (qmsk << 1) - 1;
	int pnum;

	allow_me = 0;

	// Abort if unsuccessful
	if (!select_question())
		return 0;

	// Chk who & what wants to happen
	for (pnum = 0; pnum < (four_plr_ver ? MAX_PLYRS : 2); pnum++)
	{
		// Allow all finishing players
		if (qtr_pstatus[pnum] & qmsk)
		{
			allow_me |= 1 << pnum;
		}
	}
	allow_me |= record_me;

	return allow_me;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void do_trivia_sp_proc(int * args)
{
	sleep(30);
	snd_scall_bank(ancr_bnk_str,TRIVIA_SP,VOLUME3,127,LVL1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void trivia_contest(void)
{
	ostring_t * os_amax[MAX_ANSWERS];
	sprite_info_t * psprite;
	float fh, fw, fx, fy;
	int btns, q_lcnt, i_bw, iw, i;
	int fid13;
	int was_low_res = is_low_res;
	char * psz1;
	char * psz2;
	char * psz3;
	char * psz4;

#ifdef DBG_TC
	int sw = 0;
	int flg = 0;

	do {
	static int l = 0;
	static int r = 0;

	if (testing || flg)
		if (!select_question())
			return;
	flg = 1;
#endif

	// Freeze last page displayed
	draw_enable(0);

	// Preload texture files; abort if fails
	if (load_textures_into_tmu(trivia_textures_tbl))
	{
#ifdef DEBUG
		fprintf(stderr, "Couldn't load all trivia screen textures into tmu\r\n");
		lockup();
#endif
		draw_enable(1);
		goto trivia_abort;
	}

	// Draw background
	psprite = beginobj(&triviabg,    BK_X, BK_Y, BK_Z, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&triviabg_c1, BK_X, BK_Y, BK_Z, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&triviabg_c2, BK_X, BK_Y, BK_Z, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&triviabg_c3, BK_X, BK_Y, BK_Z, BKGRND_TID);
	psprite->id = 1;

	psprite = beginobj(&challeng,    HDR_X, HDR_Y, HDR_Z, JAPPLE_BOX_TID);
	psprite->id = 1;
	psprite = beginobj(&challeng_c1, HDR_X, HDR_Y, HDR_Z, JAPPLE_BOX_TID);
	psprite->id = 1;


	// Process the strings for bogus chars & trailing spaces
	psz1 = szq;
	for (i = 0; i <= amax; i++)
	{
		while (*psz1)
		{
			if (*psz1 < ' ' || *psz1 > 'z')
				*psz1 = ' ';
			psz1++;
		}
		while (*(--psz1) == ' ')
			*psz1 = '\0';

		if (i < MAX_ANSWERS)
			psz1 = psza[i];
	}


	// Do string setup stuff
	set_string_id(1);
	set_text_z_inc(0.1f);

	if (is_low_res)
	{
		fid13 = FONTID_BAST10LOW;
	}
	else
	{
		fid13 = FONTID_BAST13;
		if (!create_font(&bast13_font, fid13)) fid13 = FONTID_BAST10;
	}


	// Display question
	set_text_font(fid13);
	set_text_color(0xff60e0ff);//0xff60ffff);//LT_YELLOW);
	set_text_justification_mode(HJUST_LEFT|VJUST_TOP);

	q_lcnt = 0;
	psz1 = szq;
	psz2 = szq + strlen(szq);

	// Process while string start ptr is < end ptr
	while (psz1 < psz2 && q_lcnt < MAX_LINES)
	{
		// Always start scan from the end
		psz3 = psz2;
		while (get_string_width(psz1) > QUEST_W)
		{
			// Save last valid ending ptr
			psz4 = psz3;
			while ((psz1 < --psz3) && *psz3 != ' ');

			// If no spaces found, print the too-wide string to the
			// last valid ending; scan will continue from there
			if (psz1 == psz3)
			{
				psz3 = psz4;
				break;
			}
			// Zero the space, putting back the previous one,
			// making the string shorter (not as wide)
			*psz3 = '\0';
			*psz4 = ' ';
		}
		// Restore NULL to the true end of the string
		*psz2 = '\0';

		pszq[q_lcnt++] = psz1;

		psz1 = psz3 + 1;
	}

	is_low_res = 0;
	fy  = was_low_res ? (QUEST_YMID*2.0f/3.0f) : (QUEST_YMID);
	fy += que_yinc[was_low_res] * (float)(--q_lcnt) * 0.5f;
	for (i = 0; i <= q_lcnt; i++)
	{
		set_text_position(QUEST_X, fy, QUEST_Z);
		oprintf("%s", pszq[i]);
		fy -= que_yinc[was_low_res];
	}
	is_low_res = was_low_res;

#ifdef D_QNUM
	set_text_position(20, QUEST_YMID, OPT1_Z);
	oprintf("%d.", qnum+1);
#endif


	// Display answers
	set_text_font(fid13);
	set_text_color(OPT1_C);
	set_text_justification_mode(HJUST_LEFT|VJUST_TOP);

	// Set width of answer text
	for (i_bw = 0, i = 0; i < amax; i++)
	{
		iw = get_string_width(psza[i]);
		if (i_bw < iw) i_bw = iw; 
	}
	if (i_bw < OPT1_WMIN)
		i_bw = OPT1_WMIN;

	// Set width of answer box
	iw = i_bw + (int)(plyr1.w + CURS_XOFF + OPTB_WOFF);

	// Set answer box scales
	fh = trivbox.h;
	fw = trivbox.w;
	if (trivbox.ax == trivbox_c1.ax)
		fh += trivbox_c1.h;
	else
		fw += trivbox_c1.w;
	fh = OPTB_H / fh;
	fw = (float)iw / fw;

	fx = OPTB_XOFF - (float)(iw / 2);

//	// Set width of answer box
//	for (i_bw = 0, i = 0; i < amax; i++)
//	{
//		iw = get_string_width(psza[i]);
//		if (i_bw < iw) i_bw = iw; 
//	}
//	i_bw += OPTB_WOFF;
//	if (i_bw < OPTB_WMIN)
//		i_bw = OPTB_WMIN;
//
//	// Set width of answer "window"
//	iw = i_bw + (int)(plyr1.w + CURS_XOFF);
//
//	// Set answer box scales
//	fh = trivbox.h;
//	fw = trivbox.w;
//	if (trivbox.ax == trivbox_c1.ax)
//		fh += trivbox_c1.h;
//	else
//		fw += trivbox_c1.w;
//	fh = OPTB_H / fh;
//	fw = (float)i_bw / fw;
//
//	fx = OPTB_XOFF - (float)(i_bw - iw / 2);
//	if (fx > OPTB_XMIN)
//		fx = OPTB_XMIN;

	psprite = beginobj(&trivbox,    fx, OPTB_Y, OPTB_Z, JAPPLE_BOX_TID);
	psprite->id = 1;
	psprite->w_scale = fw;
	psprite->h_scale = fh;
	generate_sprite_verts(psprite);
	psprite = beginobj(&trivbox_c1, fx, OPTB_Y, OPTB_Z, JAPPLE_BOX_TID);
	psprite->id = 1;
	psprite->w_scale = fw;
	psprite->h_scale = fh;
	generate_sprite_verts(psprite);

	fw = fx +(float)(iw - i_bw) - OPT1_XOFF;
	fy = OPT1_Y;
	for (i = 0; i < amax; i++)
	{
		set_text_position(fw, fy, OPT1_Z);
		os_amax[i] = oprintf("%s", psza[i]);
		fy -= ans_yinc[is_low_res];
	}

	// Set cursor X
	fx += OPT1_XOFF;


	// Display point value (only if someone can tally points)
	if (record_me)
	{
		set_text_font(FONTID_BAST10);
		set_text_justification_mode(HJUST_CENTER|VJUST_TOP);

		set_text_position(OPTB_XOFF, fy - 3, OPT1_Z);
		oprintf("%dcVALUE: %dc%d PT%c", LT_GREEN, WHITE,
			pval, pval > 1 ? 'S':'\0');
	}


	// Reset string z-inc
	set_text_z_inc(0.0f);


	// Create cursor procs and japple boxes
	qcreate("cred_msg", CREDIT_PID, credit_msg,
		BUY_IN_SCRN,
		JAPPLE_BOX_TID,
		0,
		0);
	// Set anything(?) but definitely NOT BUY_IN_SCRN
	screen = HALFTIME_SCRN;
	qcreate("timer", TIMER_PID, timedown,
		(TIME_TOTAL - TIME_TIMER),
		TIME_TIMER,
		TIME_X,
		TIME_Y);
	btns = 0;
	for (i = 0; i < (four_plr_ver ? MAX_PLYRS : 2); i++)
	{
		if (allow_me & (1 << i))
		{
			btns |= (P_A|P_B|P_C) << (i << 3);

			// Show japple box
			qcreate("credbox", CREDIT_PID, plyr_credit_box,
				i,
				station_xys[i][four_plr_ver][X_VAL],
				station_xys[i][four_plr_ver][Y_VAL],
				JAPPLE_BOX_TID);

			// Do cursor proc
			qcreate("p_cur", PLYR_CURSOR_PID, trivia_cursor_proc,
				i,
				record_me & (1 << i),
				(int)fx,
				0);

			// Show name & current pts if valid initials
			if (record_me & (1 << i))
			{
				print_players_name(i,
					japbx_cntr_xys[i][four_plr_ver][X_VAL],
					japbx_cntr_xys[i][four_plr_ver][Y_VAL],
					plyr_data_ptrs[i]->plyr_name,
					1);
			}
		}
	}


#ifdef DBG_TC
	if (!testing)
#endif
	{
		snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);
		qcreate("trivsp", PLYR_CURSOR_PID, do_trivia_sp_proc, 0, 0, 0, 0);
	}

	draw_enable(1);

	// Init monitoring flags
	force_selection = 0;
	exit_status = 0;
	award_me = 0;


	// Loop till done
	do
	{
		sleep(1);
#ifdef DBG_TC
		sw = get_dip_coin_close() & P_S;
		if (sw && os_amax[anum]->state.color != (int)LT_RED)
		{
			os_amax[anum]->state.color = LT_RED;
			change_string_cc(os_amax[anum], LT_RED);
		}
		sw = get_dip_coin_current() & P_S;
		if (!sw && os_amax[anum]->state.color != (int)OPT1_C)
		{
			os_amax[anum]->state.color = OPT1_C;
			change_string_cc(os_amax[anum], OPT1_C);
		}
		if (sw) if ((sw = get_player_sw_close() & P_U)) break;
		sw = get_player_sw_current();
		if (sw & P_L) {--l; r = 4;} else l = 4;
		if (sw & P_R) {--r; l = 4;} else r = 4;
		if (sw & P_LR && !(sw & P_RV)) break;
#endif
	}
	while (exit_status);

	killall(TIMER_PID, -1);
	delete_multiple_strings(TIMER_MSG_ID, -1);

	i = 0;
#ifdef DBG_TC
	if (!exit_status)
#endif
	{
		int cnt = 40;
		int dly = 0;
		int col = os_amax[anum]->state.color;

		snd_scall_bank(plyr_bnk_str, (award_me ? TWO_PT_SND : NO_GOOD_SND),
			VOLUME4, 127, PRIORITY_LVL2);
		do
		{
			if (cnt && !dly--)
			{
				col = (UINT)col == LT_RED ? OPT1_C : LT_RED;
				os_amax[anum]->state.color = col;
				change_string_cc(os_amax[anum], col);
				cnt--;
				dly = 2;
			}
			sleep(1);

			if (get_player_sw_close() & btns && i > 60)
				break;
		} while (++i < (4 *57));
	}

#ifdef DBG_TC
	if (!exit_status && !testing)
#endif
trivia_abort:
	if (!record_me)
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
		snd_scall_bank(generic_bnk_str, 16, VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK0);

		// Do sound only if the transition finished, not the time-out
		if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);
	}

	// Clean out everything
	draw_enable(0);

	killall(CREDIT_PID, -1);
	killall(PLYR_CURSOR_PID, -1);

	del1c(1, -1);
	del1c(OID_JAPPLE_BOX, -1);
	delete_multiple_strings(0, 0);

	delete_multiple_textures(JAPPLE_BOX_TID, -1);
	delete_multiple_textures(BKGRND_TID, -1);

#ifdef DBG_TC
	if (sw & P_U) {
		qnum = randrng(MAX_QUESTIONS);
		snd_scall_bank(cursor_bnk_str,VS_GOT_CODE_SND,VOLUME3,127,PRIORITY_LVL1);
		sw = P_LR;
	} else {
		if (sw & P_L) {qnum -= (--l < 0) ? 10:1; r = 4;} else l = 4;
		if (sw & P_R) {qnum += (--r < 0) ? 10:1; l = 4;} else r = 4;
		if (qnum < 0)              qnum += MAX_QUESTIONS;
		if (qnum >= MAX_QUESTIONS) qnum -= MAX_QUESTIONS;
		if (sw & P_LR) snd_scall_bank(cursor_bnk_str,VS_CHANGE_ICON_SND,VOLUME3,127,PRIORITY_LVL1);
	}
	} while (sw & P_LR);
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void trivia_cursor_proc(int *args)
{
	sprite_info_t * psprite;
	float curs_x = (float)(args[2]);
	float curs_y = is_low_res ? CURS_Y+3.0f:CURS_Y;
	int pnum = args[0];
	int rcrd = args[1];
	int cur_answer = 0;
	int cnt = 0;
	int switches, buttons, i;

	psprite = beginobj(ii_trivia_curs[pnum], curs_x, curs_y,
		CURS_Z - (float)pnum, JAPPLE_BOX_TID);
	psprite->id = 1;

	// Mark that player is gonna answer a question
	exit_status |= (1<<pnum);

	// Do answer-select stuff
	while (1)
	{
		// Read INPUT
		switches = get_joy_val_down(pnum);
		buttons = get_but_val_down(pnum);

		// handle joystick input
		if (switches)
		{
			i = 0;
			if (switches == JOY_UP && cur_answer > 0)
				i = -1;
			else
			if (switches == JOY_DOWN && cur_answer < (acnt-1))
				i = 1;

			if (i)
			{
				cur_answer += i;
				psprite->y += ans_yinc[is_low_res] * (float)(-i);
				generate_sprite_verts(psprite);
				snd_scall_bank(cursor_bnk_str,VS_CHANGE_ICON_SND,VOLUME3,127,
					PRIORITY_LVL1);
			}
		}
		// handle button input
		if ((buttons && cnt > 30)
#ifndef DBG_TC
			|| force_selection
#endif
			)
		{
			snd_scall_bank(plyr_bnk_str,OVER_LOS_SND,VOLUME3,127,
				PRIORITY_LVL1);
			break;
		}
		sleep(1);

		cnt++;
	}

	flash_obj_white(psprite, 3, 2);
	
	// Mark that player is done
	exit_status &= ~(1<<pnum);

	// Chk to award points
	if (cur_answer == anum)
	{
		if (rcrd)
			plyr_data_ptrs[pnum]->trivia_pts += pval;
		award_me |= (1 << pnum);
	}

	// Wait for all players to answer question
	while (exit_status)
		sleep(1);

	// Show correct/wrong stuff
	qcreate("p_cur", PLYR_CURSOR_PID, trivia_plaq_proc,
		pnum,
		cur_answer == anum,
		0,
		0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int select_question(void)
{
	FILE * pfile;
	int fnum, qcnt, fc, qc;
	int ival[FILE_ITEMS];
	char fail[FILE_COUNT];
	char szfn[13];

	anum = 0;
	acnt = 0;
	amax = -1;

#ifdef DEBUG
	fprintf(stderr, "Entering select_question\r\n");
#endif
#ifdef CHK_TC
	qnum = 0;
#endif
	// Init file failure flags & go select a question
	for (fc = 0; fc < FILE_COUNT; fail[fc++] = FALSE);
	do
	{
#ifdef DBGCHK
		fnum = qnum / FILE_ITEMS; if (qnum < 0)
#endif
		// Loop till an unchecked file is selected; do it with MAX_QUESTIONS
		// instead of FILE_COUNT for even distribution
		do fnum = (qnum = randrng(MAX_QUESTIONS)) / FILE_ITEMS;
		while (fail[fnum]);

		// Open selected question file & skip the CRC
		if (sprintf(szfn, FILE_NAME, fnum))
		{
			if ((pfile = fopen(szfn,"r")) && !fseek(pfile, 4, 0))
			{
				// Set max number of questions current file can have
				qcnt = FILE_ITEMS;

#if MAX_QUESTIONS != (FILE_ITEMS * FILE_COUNT)
				if (fnum == (FILE_COUNT-1))
					qcnt = (MAX_QUESTIONS % FILE_ITEMS);
#endif
				// Init bad/invalid flags & go select a question
				for (qc = 0; qc < qcnt; ival[qc++] = Q_GOOD);
				do
				{
					register char * cp1;
					register char * cp2;
					register int num = -1;
					acnt = 0;

					// Did question go bad/invalid?
					if (ival[qnum % FILE_ITEMS] != Q_GOOD)
					{
						// Select new question number
						qnum = -1;
						for (num = randrng(qc); num >= 0; num--)
							while (ival[++qnum] != Q_GOOD);
						qnum += fnum * FILE_ITEMS;
					}
//{
//static int dnum = -1;
//if (dnum != qnum)
//fprintf(stderr,"===> %s, q/c %d/%d, q %d\r\n",szfn,qc,qcnt,qnum+1);
//dnum = qnum;
//}
					// Look for question number delim
					if (fgets(szq, sizeof(szq), pfile))
					{
						if (!strncmp(szq, szvalid, LEN_VALID))
						{	
							amax = MAX_ANSWERS;
							cp1 = (char *)LEN_VALID;
						}
						else
						if (!strncmp(szq, szinvalid, LEN_INVALID))
						{
							amax = -1;
							cp1 = (char *)LEN_INVALID;
						}
						else
							continue;

						// Found question number delim; skip any white-space
						// and process the number
						// Never use this number as an idx into anything!
						cp1 += (int)szq;
						num = 0;
						while (*cp1 == ' ' || *cp1 == '\t')
							cp1++;
						while (*cp1 >= '0' && *cp1 <= '9')
							num = num * 10 + (*cp1++ - '0');
						if (--num != qnum)
							continue;

						// Found selected question; chk if valid
						if (amax > 0)
						{
							if (fgets(szq, sizeof(szq), pfile))
							{
								if (fgets(sza, sizeof(sza), pfile))
								{
									if ((cp1 = strstr(sza, szcorrect)))
									{
										cp1 += LEN_CORRECT;
										if (!strstr(cp1, szcorrect))
										{
											cp2 = sza;
											num = 0;
											do
											{
												cp2 = strstr(cp2, szincorrect);
												if (cp1 && ((acnt+1) == amax ||
													cp1 < cp2))
												{
													num  = LEN_CORRECT;
													cp2  = cp1;
													cp1  = NULL;
													anum = acnt;
												}
												else if (cp2)
												{
													num  = LEN_INCORRECT;
													cp2 += num;
												}
												else continue;

												while (num)
													*(cp2 - num--) = '\0';

												psza[acnt++] = cp2;
											}
											while (cp2 && acnt < amax);
#ifdef CHK_TC
											if (qnum == (MAX_QUESTIONS - 1))
#endif
											if (acnt == amax)
												break;
										}
									}
								}
							}
						}
					}
					// Failed <fgets>, due to error or EOF before question was
					// found, or question was found but with bad/invalid data
					// Mark the question bad/invalid & retry
					ival[qnum % FILE_ITEMS] = (num == qnum && amax < 0) ? Q_IVAL: Q_FERR;
					rewind(pfile);
					qc--;
#ifdef DBGCHK
do {
static image_info_t * ii_n[] = {
	&bast18_0,&bast18_1,&bast18_2,&bast18_3,&bast18_4,
	&bast18_5,&bast18_6,&bast18_7,&bast18_8,&bast18_9};
sprite_info_t * psprite;
float x = SPRITE_HRES/2.0f;
int j = qnum % FILE_ITEMS;
int k = qnum + 1;
if (acnt != amax) fprintf(stderr,"Question %d is bad/invalid\r\n", k);
while (k) {
	x -= 2.0f + ii_n[k%10]->w;
	psprite = beginobj(ii_n[k%10], x, SPRITE_VRES/2.0f, CURS_Z, 5432);
	psprite->id = 9876;
	k /= 10;
}
if (acnt != amax) {
	x -= 10.0f + (ival[j] == Q_IVAL ? bast18exc.w : bast18_e.w);
	psprite = beginobj(ival[j] == Q_IVAL ? &bast18exc : &bast18_e, x, SPRITE_VRES/2.0f, CURS_Z, 5432);
	psprite->id = 9876;
}
sleep(1);
if (acnt != amax) {
#ifdef DBG_TC
	sleep(4);
#endif
	while (!(k = get_player_sw_current() & P_LR)) {
		if (get_dip_coin_current() & P_S
#ifdef DBG_TC
			&& get_player_sw_close() & P_U
#endif
			) {qnum = -1; qc = 0; break;}
		else sleep(1);
	}
	snd_scall_bank(cursor_bnk_str,(qnum < 0 ? VS_GOT_CODE_SND : VS_CHANGE_ICON_SND),VOLUME3,127,PRIORITY_LVL1);
}
del1c(9876,-1);
#ifdef DBG_TC
if (qnum < 0) break;
if (k & P_L) {j--; qnum--;} else
if (k & P_R) {j++; qnum++;}
if (j <  0)    {j += qcnt; qnum += qcnt;}
if (j >= qcnt) {j -= qcnt; qnum -= qcnt;}
} while (qc && ival[qnum % FILE_ITEMS] != Q_GOOD);
#else
} while (0);
if (qnum < 0 || get_dip_coin_current() & P_S)
	{snd_scall_bank(cursor_bnk_str,VS_GOT_CODE_SND,VOLUME3,127,PRIORITY_LVL1); fc = 1; qc = 0;}
else qnum++;
#endif
#endif
				}
				while (!ferror(pfile) && qc);

				// Either way, done with this file
				fclose(pfile);
#ifdef CHK_TC
				if (qnum == (MAX_QUESTIONS - 1))
#endif
				// Got a valid question if all the answers were there
				if (acnt == amax)
					break;
#ifdef DEBUG
#ifndef CHK_TC
fprintf(stderr,"Couldn't find a valid question in trivia file '%s'\r\n",szfn);
#endif
#endif
			}
#ifdef DEBUG
else fprintf(stderr,"Couldn't open trivia file '%s'\r\n",szfn);
#endif
		}
		// Mark the file failure & retry
		fail[fnum] = TRUE;
	}
	while (--fc);

	// Abort if couldn't open a file with a valid question
	if (!fc)
	{
#ifdef DEBUG
#ifndef CHK_TC
		fprintf(stderr, "Couldn't find a valid trivia file\r\n");
#endif
#endif
		return FALSE;
	}

#ifdef DEBUG
	fprintf(stderr, "Exiting select_question\r\n");
#endif

//fprintf(stderr,"%d. %s\r\n%c%s\r\n%c%s\r\n%c%s\r\n%c%s\r\n",qnum+1,szq,
//anum==0?'+':' ',psza[0],
//anum==1?'+':' ',psza[1],
//anum==2?'+':' ',psza[2],
//anum==3?'+':' ',psza[3]);

	// Set question point-value
	// Tie in with qnum so question will always have a known value
	// (of every 5 questions, 2 x 1pt, 2 x 2pt, 1 x 3pt)
	pval = (((qnum % 5) & 6) >> 1) + 1;

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------
// Drop the correct/incorrect plaqs
//
//   INPUT: player number, correct/wrong flag
// RETURNS: Nothing
//-------------------------------------------------------------------------------------------------------------------------
static void trivia_plaq_proc(int * args)
{
	sprite_info_t * plaq_obj;
	sprite_info_t * flsh_obj;
	int pnum = args[0];
	int i;

//	snd_scall_bank(cursor_bnk_str, FULLGM_APPEAR_SND, VOLUME3, 127,
//		PRIORITY_LVL3);

	plaq_obj = beginobj(args[1] ? &correct : &wrong,
		station_xys[pnum][four_plr_ver][X_VAL],
		station_xys[pnum][four_plr_ver][Y_VAL]-64.0F,
		1.18f - 0.005f * (float)pnum,
		JAPPLE_BOX_TID);
	plaq_obj->w_scale = 2.2f;
	plaq_obj->h_scale = 2.2f;
	plaq_obj->id = OID_JAPPLE_BOX;

	while (plaq_obj->w_scale > 1.0f)
	{
		plaq_obj->w_scale -= 0.10f;
		plaq_obj->h_scale -= 0.10f;
		generate_sprite_verts(plaq_obj);
		sleep(1);
	}

	snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME2, 127,
		PRIORITY_LVL1);

	if (args[1])
	{
		flsh_obj = beginobj(plaq_flsh_imgs[0],
			japbx_cntr_xys[pnum][four_plr_ver][X_VAL],
			japbx_cntr_xys[pnum][four_plr_ver][Y_VAL]+2.0f,
			1.15f - 0.005f * (float)pnum,
			JAPPLE_BOX_TID);
		flsh_obj->w_scale = 2.4f;
		flsh_obj->h_scale = 4.2f;
		flsh_obj->id = OID_JAPPLE_BOX;
		generate_sprite_verts(flsh_obj);

		for (i = 0; i < (int)(sizeof(plaq_flsh_imgs)/sizeof(image_info_t *) - 1); i++)
		{
			change_img_tmu(flsh_obj, plaq_flsh_imgs[i], JAPPLE_BOX_TID);
			sleep(2);
		}
		delobj(flsh_obj);
	}
}
