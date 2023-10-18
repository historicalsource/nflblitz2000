#if defined(VEGAS)

#define USE_DISK_CMOS
void update_other_cmos_to_disk(int sleep_allowed);

#endif

#if defined(VEGAS)
#define PLAY_PARTITION		4			/* for NBA On NBC/Blitz 99 dual boot */
#else
#define PLAY_PARTITION		3			/* for tradtional Blitz 99 */
#endif

#ifdef DEBUG
//#define NO_CAPTIMER						/* disables timer in DEBUG only */
#endif

/****************************************************************************/
/*                                                                          */
/* cap.c        create-a-play editor										*/
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* Copyright (c) 1998 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include	<math.h>
#include <stdlib.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <goose/goose.h>
#include <goose/ostrings.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/general.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/cap.h"
#include "/Video/Nfl/Include/support.h"
#include "/Video/Nfl/Include/ani3d.h"
#include "/Video/Nfl/Include/plyrseq.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/PlayStuf.h"
#include "/Video/Nfl/Include/Attract.h"
#include "/Video/Nfl/Include/playsel.h"
#include "/Video/Nfl/Include/pup.h"

#define	IMAGEDEFINE	1
#include "include/makeplay.h"

#define CAP_TEXT_ID		0x500

#define CERR_NOSEL		3
#define CERR_NOCARD		4
#define CERR_NOFILE		5
#define CERR_NOPLAYS	6

/////////////////////////////////////////////////////////////////////////////
// referenced data

void purchase_flasher(int *args);

// this is the RAM copy of the players record (from CMOS or New record)
extern plyr_record_t		tmp_plyr_record;

extern font_info_t bast75_font;
extern font_info_t bast23_font;
extern font_info_t bast23scale_font;
extern font_info_t bast10_font;
extern int dir49_table[];
extern int ds_none[];
extern int pr_QB_block[];

extern image_info_t mbg,mbg_c1,mbg_c2,mbg_c3;
extern image_info_t gridbox;

/////////////////////////////////////////////////////////////////////////////
// local functions
static void create_play_exit_proc( int *args );

static void update_help_box( int );
static void activate_lcb( int b );
static void deactivate_lcb( int b );
static void hilite_lcb( int b );
static void unhilite_lcb( int b );
static void lcb_move_up( void );
static void lcb_move_down( void );

static void cap_timer( int * );

static void get_name( void );
static void get_pin( void );
static void show_record_status_msg(int,int);


static void lcb_create_edit( void );
#ifndef NO_CARD_READER
static void lcb_load_plays( int );
#endif
static void lcb_formation( int );
//static void lcb_qb( void );
static void lcb_edit_route( int );
static void lcb_choose_setreceiver(void);
static void lcb_choose_audibles(void);
static void lcb_name_play( void );
//static void lcb_save( void );
//static void lcb_abort( void );
static void lcb_exit( void );


static void show_cap_info_screen(int,int);

static void update_help_text( char ** );

static void write_input_name( char instring[2][CP_NAMELEN] );
static void print_string( char *, int );

static void flash_lcb(int, int);

static void copy_play( cap_play_t *, cap_play_t * );
static void update_play_display( int );
static void update_audibles_display( void );
static void _draw_wpn( int );
static int _node_chain_blocked( cap_point_t *, int, int );
static void _move_downstream( cap_point_t *, int, int, int );
static void find_new_point( cap_point_t *, int, cap_point_t * );
static int _is_valid_point( cap_point_t *, int, int, int );
static int _root_node_blocked( int, int, int );

static int prompt_yn( char ** );
static int prompt_off_def(void);
static void sprite_blinker( int * );
static void draw_line( void * );

static int er_menu( cap_point_t *, int );
static void get_menu_pos( float *, float *, int, int );

static void count_specials_used( cap_play_t * );

#ifdef CAP_DEBUG
static void dump_play( cap_play_t *, int );
static void dump_route( int *route );
#endif

static void wait_for_any_butn_press_plyr(int, int, int);

/////////////////////////////////////////////////////////////////////////////
// play conversion routines
static int convert_route( cap_point_t *, int * );
static int angle_from( cap_point_t *, cap_point_t * );
static int dist_from( cap_point_t *, cap_point_t * );

/////////////////////////////////////////////////////////////////////////////
// play validation routines
static int validate_play( cap_play_t * );
static int points_overlap( cap_point_t *, cap_point_t * );
static int legal_control_point( int, int );
static int legal_start_pos( int, int, int );
static int check_play_name( char * );
static void draw_enter_name_backplate(void);
short  _SecWrites( int, int *, int );
short _SecReads( int, int *, int );

/////////////////////////////////////////////////////////////////////////////
// disk routines
static int write_plays_hd( int, cap_play_t *, audible_ids_t * );
int read_plays_hd( int, int, cap_play_t *, audible_ids_t * );
static int update_stored_plays( void );

/////////////////////////////////////////////////////////////////////////////
// local state variables
//static int slower_clock = 0;		// NO
int cap_pnum;						// active control panel station
static int active_play = -1;		// current play being edited
static int lcb_focus = 0;			// focus button on left column
//static int play_changed = FALSE;
static int route_flags[3];
static int lq_dim;
static int cur_form = 0;
static int er_point;				// focus node in route being edited
static int specials_used[4] = {0,0,0};		// juke,spin,turbo
static int mirror_dir;				// -1 if moving right-to-left causes
									// route flip, +1 if left-to-right
static int force_create;

#ifndef NO_CARD_READER
static int reader_exists;
static int open2_on_exit = FALSE;	// should I do an OPEN2 when timeout/quit?
#endif

static int current_page_index;
static int current_page_total;
static int usable_slots = 1;
static int wins_needed[] = {
	 0,  0,  0,
	 5, 10, 15,
	20, 25, 30,
};

/////////////////////////////////////////////////////////////////////////////
// Custom plays

play_t			custom_play;
cap_play_t		custom_plays[MAX_PLYRS][PLAYSTUF__PLAYS_PER_PAGE];
cap_play_t *	convert_play_last;
audible_ids_t	audible_ids[MAX_PLYRS];

static cap_play_t *		current_plays;
static audible_ids_t *	current_ids;

static cap_play_t	cur_play;
static fline_t		custom_form[7];
static int			custom_routes[512];

static cap_line_t	cap_lines[32];
static int			line_cnt;

#ifdef SEATTLE
static GrMipMapId_t	line_tex;
#else
static Texture_node_t *line_tex;
#endif


/////////////////////////////////////////////////////////////////////////////
// global copy of players NAME and PIN number
static char 	name_pin[LETTERS_IN_NAME+PIN_NUMBERS];

partition_table_t *ide_get_partition_table( void );

// splash screen text
static char *splash_info_msg[] = {
	"YOUR PLAYS CAN BE RETRIEVED USING YOUR NAME",
	"AND PIN NUMBER.",
	" ",
	"BUILD AN ARSENAL OF PLAYS TO OUTSMART",
	"YOUR OPPONENT.",
	" ",
	"THE MORE GAMES YOU WIN, THE MORE PLAYS",
	"YOU CAN DESIGN.",
	NULL
};
static char *splash_info_msg1[] = {
	"YOU MUST ENTER YOUR NAME AND A PIN NUMBER",
	"TO CREATE YOUR OWN CUSTOM PLAYS.",
	" ",
	"YOU WOULD THEN BE ABLE TO BUILD AN ARSENAL",
	"OF PLAYS TO OUTSMART YOUR OPPONENT.",
	" ",
	"AS YOU WIN MORE GAMES, MORE CUSTOM PLAYS",
	"WILL BE UNLOCKED FOR YOU TO USE.",
	NULL
};
//	"YOU CAN ALSO DESIGN PLAYS AT HOME WITH NFL BLITZ",
//	"FOR N64. BRING IN YOUR N64 CONTROLLER PAK",
//	"WITH SAVED PLAYS, ENTER CREATE PLAY AND CHOOSE",
//	"LOAD N64 OPTION.",


// record FOUND text
static char *rec_status_msgs[] = {
	"NO RECORD!",
	"NEW ONE CREATED",
	"RECORD FOUND!",
	"DATA READ IN",
	NULL };

// edit route action flags
static int er_action_flags[] = {0,CPPT_JUKE,CPPT_SPIN,CPPT_TURBO,0,CPPT_LAST};

// control point menu help texts
static char *er_ht_none[] = {
	"no special item",
	NULL };

static char *er_ht_juke[] = {
	"receiver performs a juke",
	"move to fake out defenders",
	NULL };

static char *er_ht_spin[] = {
	"receiver spins to",
	"evade defenders",
	NULL };

static char *er_ht_turbo[] = {
	"gives receiver a short",
	"burst of speed",
	NULL };

static char *er_ht_delete[] = {
	"delete current node",
	NULL };

static char *er_ht_done[] = {
	"done editing route",
	NULL };

static char **er_help_texts[ER_MENU_LENGTH] = {
	(char **)&er_ht_none,
	(char **)&er_ht_juke,
	(char **)&er_ht_spin,
	(char **)&er_ht_turbo,
	(char **)&er_ht_delete,
	(char **)&er_ht_done
};


/////////////////////////////////////////////////////////////////////////////
// name input block
static image_info_t *letter_block[] = {
	&mp_a,&mp_b,&mp_c,&mp_d,
	&mp_e,&mp_f,&mp_g,&mp_h,
	&mp_i,&mp_j,&mp_k,&mp_l,
	&mp_m,&mp_n,&mp_o,&mp_p,
	&mp_q,&mp_r,&mp_s,&mp_t,
	&mp_u,&mp_v,&mp_w,&mp_x,
	&mp_y,&mp_z,&mp_dsh,&mp_ast,
	&mp_exc,&mp_spc,&mp_del,&mp_end };

static char letter_trans[] = {
	CH_A,CH_B,CH_C,CH_D,
	CH_E,CH_F,CH_G,CH_H,
	CH_I,CH_J,CH_K,CH_L,
	CH_M,CH_N,CH_O,CH_P,
	CH_Q,CH_R,CH_S,CH_T,
	CH_U,CH_V,CH_W,CH_X,
	CH_Y,CH_Z,CH_DSH,CH_AST,
	CH_EXC,CH_SPC,CH_DEL,CH_END};


// pin number input block
static image_info_t *pin_nbrs_block[] = {
	&cp_1,&cp_2,&cp_3,
	&cp_4,&cp_5,&cp_6,
	&cp_7,&cp_8,&cp_9,
	&cp_spc,&cp_0,&cp_del };

static char pin_nbrs_trans[] = {
	CH_1,CH_2,CH_3,
	CH_4,CH_5,CH_6,
	CH_7,CH_8,CH_9,
	CH_SPC,CH_0,CH_DEL};


extern image_info_t *stat_flsh_imgs[10];


/////////////////////////////////////////////////////////////////////////////
// route display icons
static image_info_t *route_icons[5][3] = {
	{&cpoint,	&cpointf,	&cpointg},
	{&sym_j,	&sym_jf,	&sym_jg},
	{&sym_s,	&sym_sf,	&sym_sg},
	{&cpoint,	&cpointf,	&cpointg},
	{&sym_t,	&sym_tf,	&sym_tg}};

static image_info_t *startpos_icons[5][3] = {
	{&sym_r1,	&sym_r1f,	&sym_r1g},
	{&sym_r2,	&sym_r2f,	&sym_r2g},
	{&sym_r3,	&sym_r3f,	&sym_r3g},
	{&sym_qb,	&sym_qbf,	&sym_qbg},
	{&mpo,		&mpof,		&mpog}};

/////////////////////////////////////////////////////////////////////////////
// edit route menu digits
static image_info_t	*erm_digits[2][4] = {{&drop_0g,&drop_1,&drop_2,&drop_3}, {&drp_0glr,&drp_1lr,&drp_2lr,&drp_3lr}};

/////////////////////////////////////////////////////////////////////////////
// left-column button positions
static float lcb_xys[LCB_COUNT][2] = {
	{LCB_X, LCB_Y_TOP},
	{LCB_X, LCB_Y_TOP-1*LCB_Y_INC},
	{LCB_X, LCB_Y_TOP-2*LCB_Y_INC},
	{LCB_X, LCB_Y_TOP-3*LCB_Y_INC},
	{LCB_X, LCB_Y_TOP-4*LCB_Y_INC},
	{LCB_X, LCB_Y_TOP-5*LCB_Y_INC},
	{LCB_X, LCB_Y_TOP-6*LCB_Y_INC},
	{LCB_X, LCB_Y_TOP-7*LCB_Y_INC},
	{LCB_X, LCB_Y_TOP-8*LCB_Y_INC},
};

// left-column button texture names
static image_info_t *lcb_names[LCB_COUNT][2] = {
	{&create,	&createg},
	{&form,		&formg},
	{&r1,		&r1g},
	{&r2,		&r2g},
	{&r3,		&r3g},
	{&setrcvr,	&setrcvrg},
	{&name,		&nameg},
	{&setaud,	&setaudg},
	{&mpexit,	&mpexitg}
};

// help text for ENTER NAME ANS PIN
static char *ht_enter_name[] = {
	"enter name and pin number",
	"of record to retrieve",
	NULL
};

// left-column button objects
sprite_info_t *lcb_buttons[LCB_COUNT];

// left-column button texts
sprite_info_t *lcb_texts[LCB_COUNT];

// left-column button help texts
static char *lcb_ht_create[] = {
	"pick play to edit or",
	"start editing from scratch",
	NULL };

static char *lcb_ht_formation[] = {
	"choose formation",
	"(clears existing pass routes)",
	NULL };

//static char *lcb_ht_qb[] = {
//	"set quarterback",
//	"starting position",
//	NULL };

static char *lcb_ht_r1[] = {
	"create/edit route for",
	"receiver 1",
	NULL };

static char *lcb_ht_r2[] = {
	"create/edit route for",
	"receiver 2",
	NULL };

static char *lcb_ht_r3[] = {
	"create/edit route for",
	"receiver 3",
	NULL };

static char *lcb_ht_setreceiver[] = {
	"Select the primary receiver",
	NULL,
	NULL };

static char *lcb_ht_audibles[] = {
	"Select your audible plays",
	NULL,
	NULL };

static char *lcb_ht_name[] = {
	"change the name of",
	"the current play",
	NULL };

//static char *lcb_ht_save[] = {
//	"save current play",
//	NULL,
//	NULL };

//static char *lcb_ht_abort[] = {
//	"quit editing play",
//	"without saving",
//	NULL };

static char *lcb_ht_exit[] = {
	"saves progress and exits",
	NULL,
	NULL };


// editing route info messages
//static char *stage_title_ht[] = {
//	"read these instructions",
//	NULL };

static char * stage1_ht[] = {
	"select a starting position",
	"for the receiver.",
	"JOYSTICK: moves receiver",
	"BUTTONS: select position",
	NULL
};

static char * stage2_ht[] = {
	"Next, set the first point that",
	"the receiver will run to.",
	"JOYSTICK: moves position",
	"BUTTONS: select position",
	NULL
};

static char * stage3_ht[] = {
	"Next, choose an ACTION",
	"for this route point.",
	"JOYSTICK: change action",
	"BUTTONS: select action",
	NULL
};

static char * stage1_pr[] = {
	"select the primary receiver",
	"for this play.",
	"JOYSTICK: change",
	"BUTTONS: select",
};


static char **lcb_help_texts[LCB_COUNT] =
{
	(char **)&lcb_ht_create,
	(char **)&lcb_ht_formation,
	(char **)&lcb_ht_r1,
	(char **)&lcb_ht_r2,
	(char **)&lcb_ht_r3,
	(char **)&lcb_ht_setreceiver,
	(char **)&lcb_ht_name,
	(char **)&lcb_ht_audibles,
	(char **)&lcb_ht_exit
};

#ifndef NO_CARD_READER
static char *card_reader_info[] = {
		"This option allows",
		"you to load in 9 plays",
		"that were created on the",
		"nintendo 64 home version",
		"of nfl blitz.",
		NULL};

static char *card_reader_question[] = {
		"load from N64",
		"controller pak ?"};

//static char *card_reader_status[] = {
//		"N64 MEMORY CARD",
//		"READER FOUND!!"};
#endif

static char *formation_info[] = {
		"UP/DWN: CHANGE",
		"BUTTON: SELECTS" };

// weapon / route display colors
//  wpn 1,    wpn 2,    wpn 3,    active,   dim
static int wpn_color[] = {
	0x00FF00, 0x0000FF, 0xFF0000, 0xFFA000, 0x969696, 
};

/////////////////////////////////////////////////////////////////////////////
// miscellaneous help texts

char *edit_route_text_root[] = {
	"set receiver start position",
	NULL };

char *edit_route_text_node[] = {
	"set next action point",
	NULL };

/////////////////////////////////////////////////////////////////////////////
// various yes/no prompt texts
char *delete_route_text[] = {
	"this receiver",
	"has a route.",
	" ",
	"delete route ?",
	NULL }; 

//char *abort_text[] = {
//	"you have made changes.",
//	"aborting now will discard",
//	"these changes. continue?",
//	NULL };

char *formation_text[] = {
	"changing the",
	"formation",
	"clears all",
	"pass routes.",
	"continue?",
	NULL };

char *name_play_text[] = {
	"are you sure",
	"you want to",
	"rename the",
	"current play ?",
	NULL };

char *exit_text2[] = {
	"no routes",
	"edited. play",
	"wont be saved!",
	"exit anyway ?",
	NULL };

char *exit_text[] = {
	"this will",
	"end your",
	"editing session.",
	"exit now ?",
	NULL };

/////////////////////////////////////////////////////////////////////////////
// textures to load
char *cap_textures[] = { "capa00.wms", "capa01.wms", "capa02.wms", "capa03.wms", "capa04.wms",
						 "capb00.wms", "capb01.wms", "capb02.wms", "capb03.wms",
						 "plqcrd00.wms", "plqcrd01.wms", "plqfsh.wms", NULL };

/////////////////////////////////////////////////////////////////////////////
// default formations
static int cap_form_2plus1[4][2] =
{	{20-11,10-2},
	{20-8,10-5},
	{20+8,10-2},
	{FALSE,1}};

static int cap_form_1plus1plus1[4][2] =
{	{20-9,10-3},
	{20-4,10-9},
	{20+8,10-2},
	{TRUE,0}};

static int cap_form_trips[4][2] =
{	{20+7,10-3},
	{20+9,10-2},
	{20+12,10-2},
	{TRUE,0}};

static int cap_form_1wr2back[4][2] =
{	{20-4,10-6},
	{20+4,10-6},
	{20+9,10-5},
	{FALSE,2}};

static int cap_form_4lineman[4][2] =
{	{20-10,10-5},
	{20-4,10-6},
	{20+5,10-2},
	{FALSE,0}};

int (*formations[])[4][2] =
{	&cap_form_2plus1,
	&cap_form_1plus1plus1,
	&cap_form_trips,
	&cap_form_1wr2back,
	&cap_form_4lineman };


/////////////////////////////////////////////////////////////////////////////
static void update_help_box( int help_box )
{
	PLAYSTUF__SLOT * slot_ptr;
	sprite_info_t * box_obj;
	float			fx, fy1, fy2;
	signed short	flag, flip, page, play;
	int				i, j;

	delete_multiple_strings( INFO_SID, -1 );
	del1c( OID_CAP_BOX+1, -1 );

	if ( help_box )
	{
		if ( lcb_focus == 5 )
		{
			// make primary receiver box
			box_obj = beginobj_with_id( &form_box, ER_HELP_BOX1_X, ER_HELP_BOX1_Y, ER_HELP_BOX_Z, CAP_TID ,OID_CAP_BOX+1, 0);
			box_obj->w_scale = 1.5f;
			box_obj->h_scale = 0.75f;
			generate_sprite_verts( box_obj );

			set_string_id( INFO_SID );
			set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );

			set_text_font( FONTID_BAST8T );
			set_text_position( ER_HELP_BOX1_X, ER_HELP_BOX1_Y+(is_low_res ?  7 :  9), ER_HELP_BOX_Z-1);
			oprintf("%dccurrent primary receiver:", LT_CYAN);

			set_text_font( FONTID_BAST10 );
			set_text_position( ER_HELP_BOX1_X, ER_HELP_BOX1_Y+(is_low_res ? -8 : -7), ER_HELP_BOX_Z-1);
			if ( cur_play.flags & CPLT_PRI_RCVRS )
			{
				if ( cur_play.flags & CPLT_PRI_RCVR1 )
					i = 0;
				else
				if ( cur_play.flags & CPLT_PRI_RCVR2 )
					i = 1;
				else
					i = 2;

				oprintf("%dc%d", wpn_color[i], i+1);
			}
			else
				oprintf("%dcNONE", LT_CYAN);
		}
		else
		if ( lcb_focus == 7 )
		{
			static float box_xy[][2][2] = {	//[hires,lores][lft,rgt][x,y]
				{ { AUD_BOX_XL, AUD_BOX_YL }, { AUD_BOX_XR, AUD_BOX_YR   } },
				{ { AUD_BOX_XL, AUD_BOX_YL }, { AUD_BOX_XR, AUD_BOX_YR-1 } }
			};

			fx  = box_xy[is_low_res][help_box>0][X_VAL];
			fy1 = box_xy[is_low_res][help_box>0][Y_VAL];

			// make current audible plays box
			box_obj = beginobj_with_id(
				&form_box,
				fx,
				fy1,
				ER_HELP_BOX_Z,
				CAP_TID,
				OID_CAP_BOX+1,
				0);
			box_obj->w_scale = 1.5f;
			box_obj->h_scale = 3.3f;
			generate_sprite_verts( box_obj );

			set_string_id( INFO_SID );
			set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );

			set_text_font( FONTID_BAST8T );
			fy2 = fy1 + (is_low_res ? -12 : -12);
			fy1 = fy1 + (is_low_res ?  54 :  54);
			set_text_position( fx, fy1, ER_HELP_BOX_Z-1);
			oprintf("%dccurrent offense audibles:", LT_CYAN);
			set_text_position( fx, fy2, ER_HELP_BOX_Z-1);
			oprintf("%dccurrent defense audibles:", LT_CYAN);

			set_text_font( FONTID_BAST10 );
			for (i = 0; i < 2; i++)
			{
				for (j = 0, flag = 0; j < PLAYSTUF__AUDIBLES_PER_PLAYER; flag |= current_ids->ids[i][j++]);
				for (j = 0; j < PLAYSTUF__AUDIBLES_PER_PLAYER; j++)
				{
					fy1 += (is_low_res ? -15 : -15);
					set_text_position( fx, fy1, ER_HELP_BOX_Z-1);

					page = current_ids->ids[i][j];
					if ( page == 0 )
					{
						oprintf("%dcNONE", LT_CYAN);
						if ( !flag ) break;
					}
					else
					{
						flip = page < 0;
						if ( flip )
							page = -page;
						play = (page - 1) % 9;
						page = (page - 1) / 9;

						slot_ptr = PlayStuf__GetSlot( i, page, play );

						if (( i == OFFENSE ) && (page == PLAYSTUF__OFF_PAGE_ID_CUSTOM))
						{
							// display CUSTOM PAGE names
							if (current_plays[play].flags & CPLT_EXISTS)
								// print name of play from ram
								oprintf("%dc%s %s%dc%s", (i ? LT_RED:LT_GREEN),
									current_plays[play].name1,
									current_plays[play].name2,
									LT_YELLOW,
									flip ? " :F" : "" );
							else
							if (current_plays[play].flags & CPLT_UNUSED)
								// print "available but unused"
								oprintf( "%dcUNUSED", WHITE );
							else
								// print "not available"
								oprintf( "%dcNOT AVAILABLE", WHITE );
						}
						else
						{
							oprintf("%dc%s%dc%s", (i ? LT_RED:LT_GREEN),
								slot_ptr->name,
								LT_YELLOW,
								flip ? " :F" : "" );
						}
					}
				}
				fy1 = fy2;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void create_play_exit_proc( int *args )
{
	int		bank;

	force_create = 0;

	while( force_create == 0 )
		sleep(1);

	// if we're currently editing a play, write it
	if( active_play != -1 )
	{
		cur_play.flags |= CPLT_EXISTS;
		cur_play.flags &= ~CPLT_UNUSED;
		copy_play( current_plays + active_play, &cur_play );
		active_play = -1;
	}

	if (tmp_plyr_record.cmos_rec_num != -1)
	{
		// update disk/cmos
		bank = update_stored_plays();

#ifdef DEBUG
		game_info.team_head[0] = -1;
		fprintf( stderr, "Plays written to bank %d\n", bank );
#endif
	}

	// kill any processes
	killall( CREATE_PLAY_PID, -1 );
	killall( DISPLAY_BLINKER_PID, -1 );

	show_cap_info_screen( cap_pnum, 0 );

	// unlock the cap textures
	unlock_multiple_textures( CAP_TID, -1 );
	delete_multiple_textures( CAP_TID, -1 );

#ifndef NO_CARD_READER
	// release the door, maybe
	if ( open2_on_exit )
		joyplay_do_function( JPFUNC_DOOR_OPEN2 );
#endif

// Don't need to do this here since it gets done at the top of Attract!
//
//#ifdef USE_DISK_CMOS
//	/* update CMOS */
//	write_cmos_to_disk(FALSE);
//	update_other_cmos_to_disk(FALSE);
//#endif

	// kill the CAP tune
	snd_stop_track( SND_TRACK_0 );
	snd_set_reserved( SND_TRACK_0|SND_TRACK_1|SND_TRACK_5 );

	iqcreate( "attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0 );
	die( 0 );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void create_play_proc( int *args )
{
	struct texture_node * pnode;
	ostring_t *	str_ptr = (ostring_t *)NULL;
	pdata_t *	pdata;
	char **		pc;
	int			rec_num, defeated, i;
	int			was_low_res = is_low_res;

	draw_enable(0);
	wipeout();
	sleep(1);

	// clear for next time
	bought_in_first = 0;

#ifndef NO_CARD_READER
	// look for N64 reader
	reader_exists = joyplay_init();
#endif

	// chalk the audit
	increment_audit( CREATE_PLAY_AUD );
	decrement_audit( GAME_START_AUD );

	// clear some credit info
	pdata = pdata_blocks + cap_pnum;
#ifndef CAP_FIX		//FIX!!! incomplete
	pdata->quarters_purchased = 0;
	pdata->credits_paid = 0;
	game_purchased = 0;
	p_status = 0;
	p_status2 = 0;
#endif

	// Set for default playbook
	pup_playbook = 1;
	pup_secret_page = 0;

	// set current pointers
	current_plays = custom_plays[0];
	current_ids   = audible_ids;

	// kill tune and start new one
	snd_stop_track(SND_TRACK_0);
	snd_set_reserved (SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	snd_scall_bank(intro_bnk_str,10,VOLUME4,127,SND_PRI_SET|SND_PRI_TRACK0);

	game_info.game_state = GS_CREATE_PLAY;

	// load the create play textures
	pc = cap_textures;
	do
	{
		if (!create_texture( *pc, CAP_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR))
		{
#ifdef DEBUG
			fprintf( stderr,"Error loading %s texture \n", *pc );
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}
		pc++;
	} while( *pc );

	// lock the cap textures
	lock_multiple_textures( CAP_TID, 0xFFFFffff );

	// load the 3d line texture
	pnode = create_texture( "mpline.wms", CAP_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
	line_tex = pnode->texture_handle;

	// put up background
	if (is_low_res)
	{
		is_low_res = 0;
		beginobj(&bckgrndlr,		0, 256.0f, CAP_BACK_Z, CAP_BACK_TID);
		beginobj(&bckgrndlr_c1,		0, 256.0f, CAP_BACK_Z, CAP_BACK_TID);
		is_low_res = was_low_res;
	}
	else
	{
		beginobj(&bckgrnd,	  0, SPRITE_VRES, CAP_BACK_Z, CAP_BACK_TID);
		beginobj(&bckgrnd_c1, 0, SPRITE_VRES, CAP_BACK_Z, CAP_BACK_TID);
		beginobj(&bckgrnd_c2, 0, SPRITE_VRES, CAP_BACK_Z, CAP_BACK_TID);
		beginobj(&bckgrnd_c3, 0, SPRITE_VRES, CAP_BACK_Z, CAP_BACK_TID);
	}

	// create the left side buttons
	for( i = LCB_COUNT-1; i >= 0; i-- )
	{
		lcb_buttons[i] = beginobj( &button_off,
			lcb_xys[i][0],
			lcb_xys[i][1],
			LCB_BUTTON_Z - (float)i * 0.1f,
			CAP_TID );

		lcb_buttons[i]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

		lcb_texts[i] = beginobj( lcb_names[i][0],
			lcb_xys[i][0],
			lcb_xys[i][1],
			LCB_TEXT_Z,
			CAP_TID );
	}

	// create the PLAY NAME box
//	beginobj(&play_box, PLAY_NAME_BOX_X, PLAY_NAME_BOX_Y, PLAY_NAME_BOX_Z, CAP_TID);

#ifndef NO_CAPTIMER
	// start the timer
	qcreate( "captmr", CAP_TIMER_PID, cap_timer, cap_pnum, 0, 0, 0 );
#endif

	// Set current button
	lcb_focus = 0;

	// Init these before anything happens so things won't crash if they let it
	//  time-out while entering initials/pin
	active_play = -1;
	tmp_plyr_record.cmos_rec_num = -1;

	for(i=0; i < LCB_COUNT; i++)
		deactivate_lcb( i );

	update_help_text(ht_enter_name);
	update_play_display(0);

	// Re-create the wipeout'd plate objects
	create_plates();

	// put up backplate for entering NAME (done here to avoid glitch)
	draw_enter_name_backplate();
	// draw box that shows name letters as entering them
	beginobj_with_id(&form_box, NAME_X, NAME_Y-150.0f, NAME_Z-2, CAP_TID ,OID_CAP_NAME, 0);

	// Take away the plates
	iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);

	// Make the exit process
	iqcreate("cap_exit", CREATE_PLAY_PID, create_play_exit_proc, 0, 0, 0, 0);

	sleep(1);

	// Lets see them
	draw_enable(1);

	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way since we can't know how many ticks have lapsed since transition began
	while (plates_on != PLATES_OFF && --i) sleep(1);
	// Make sure plates are gone
	plates_on = PLATES_OFF;

	//
	// first the player must enter his/her name and pin number
	//
//	slower_clock = 1;
	snd_scall_bank(intro_bnk_str,12,VOLUME3,127,SND_PRI_SET|SND_PRI_TRACK5);
	get_name();
	get_pin();
//	slower_clock = 0;

//fprintf(stderr,"********* name and pin  = %s\r\n",name_pin);

	rec_num = find_record_in_cmos(name_pin);	// puts found record into 'tmp_plyr_record'

	if (rec_num < 0)
	{	// record NOT FOUND
		get_new_record_into_ram(name_pin);		// puts data into 'tmp_plyr_record'
		show_record_status_msg(0,cap_pnum);				// not found
	}
	else
		show_record_status_msg(1,cap_pnum);				// found


	// get this guy's plays
	if( tmp_plyr_record.disk_bank == -1 )
	{
#ifdef DEBUG
		fprintf( stderr, "%s has no plays on disk.  Starting from scratch.\n", name_pin );
#endif
		memset( (void *)current_plays, 0, 9*sizeof(cap_play_t));
		memset( (void *)current_ids, 0, sizeof(audible_ids_t));
	}
	else
		read_plays_hd( tmp_plyr_record.cmos_rec_num, tmp_plyr_record.disk_bank, current_plays, current_ids );

	// update plays_allowed
	defeated = GREATER( 0, tmp_plyr_record.games_won );
	usable_slots = 0;
	while (usable_slots < 9)
	{
		if ( defeated >= wins_needed[usable_slots] )
			usable_slots++;
		else
			break;
	}
	usable_slots = GREATER( tmp_plyr_record.plays_allowed, usable_slots );

	// update availability flags
	for( i = 0; i < usable_slots; i++ )
		if ( !(current_plays[i].flags & CPLT_EXISTS) )
			current_plays[i].flags = CPLT_UNUSED;
	while (i < 9)
		current_plays[i++].flags = 0;

	// initialize display
	hilite_lcb( lcb_focus );

	for( i = 0; i < LCB_COUNT; i++ )
	{
		if (i < LCB_GRAY_TOP || i > LCB_GRAY_BOT)
			activate_lcb( i );
		else
			deactivate_lcb( i );
	}

	PlayStuf__SetupCurrentPlayPages( FALSE, FALSE );
	update_help_text( lcb_help_texts[lcb_focus] );
	update_play_display(0);

	// top create loop
	while( 1 )
	{
		static int stick_cur, stick_new, but, stick_time=0;

		// look for stick
		stick_cur = (get_player_sw_current()>>(cap_pnum<<3)) & (P_UP|P_DOWN);
		stick_new = (get_player_sw_close()>>(cap_pnum<<3)) & (P_UP|P_DOWN);
		stick_time = stick_new ? 0 : stick_time + 1;

		// look for button
		but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);

		// move down?
		if ((stick_new & P_DOWN) ||
			((stick_cur & P_DOWN) &&
			(stick_time > LCB_REPEAT_TIME) &&
			(stick_time % LCB_REPEAT_INC == 0)))
		{
			lcb_move_down();

			// update primary receiver box
			update_help_box( 1 );
		}

		// move up?
		if ((stick_new & P_UP) ||
			((stick_cur & P_UP) &&
			(stick_time > LCB_REPEAT_TIME) &&
			(stick_time % LCB_REPEAT_INC == 0)))
		{
			lcb_move_up();

			// update primary receiver box
			update_help_box( 1 );
		}

		// select?
		if (but)
		{
			snd_scall_bank(cursor_bnk_str,CAP_LCB_SEL_SND,VOLUME2,127,PRIORITY_LVL2);				// play sound
			flash_lcb(lcb_focus, CAP_TID);

			// kill primary receiver box
			update_help_box( 0 );

			switch (lcb_focus)
			{
				case 0:
					lcb_create_edit();
					break;
				case 1:
					lcb_formation(FALSE);
					break;
				case 2:
				case 3:
				case 4:
					lcb_edit_route(lcb_focus-2);
					break;
				case 5:
					lcb_choose_setreceiver();
					break;
				case 6:
					lcb_name_play();
					break;
				case 7:
					update_help_box( -1 );
					lcb_choose_audibles();
					break;
				case 8:
					lcb_exit();
					break;
			}
			// update primary receiver box
			update_help_box( 1 );
		}
		update_help_text( lcb_help_texts[lcb_focus] );

		sleep( 1 );
	}
}

/////////////////////////////////////////////////////////////////////////////
static void lcb_move_up( void )
{
	unhilite_lcb( lcb_focus );
	lcb_focus = (lcb_focus == 0) ? LCB_COUNT-1 : lcb_focus-1;
	if ((active_play == -1) && (lcb_focus == LCB_GRAY_BOT))
		lcb_focus = LCB_GRAY_TOP-1;
	hilite_lcb( lcb_focus );
	snd_scall_bank(cursor_bnk_str,CAP_LCB_CUR_SND,VOLUME4,127,PRIORITY_LVL1);
}

/////////////////////////////////////////////////////////////////////////////
static void lcb_move_down( void )
{
	unhilite_lcb( lcb_focus );
	lcb_focus = (lcb_focus == LCB_COUNT-1) ? 0 : lcb_focus+1;
	if ((active_play == -1) && (lcb_focus == LCB_GRAY_TOP))
		lcb_focus = LCB_GRAY_BOT+1;
	hilite_lcb( lcb_focus );
	snd_scall_bank(cursor_bnk_str,CAP_LCB_CUR_SND,VOLUME4,127,PRIORITY_LVL1);
}


/////////////////////////////////////////////////////////////////////////////
#define INIT_POS	0
#define MAX_POS		31
#define MIN_UP		4
#define MAX_DOWN	27

static void lcb_name_play( void )
{
	sprite_info_t	*psprite, *focus;
	int				stick_cur,stick, but, opos;
	int				pos = INIT_POS, done = FALSE;
	int				stick_time, stick_old;
	char			instring[2][CP_NAMELEN];
	int				in_word = 0, in_pos = 0, i;


	// first ask if player really wants to change the play name
	if ((cur_play.name1[0] != 'n') &&
		(cur_play.name1[1] != 'e') &&
		(cur_play.name1[2] != 'w') &&
		(cur_play.name2[0] != 'p') &&
		(cur_play.name2[1] != 'l') &&
		(cur_play.name2[2] != 'a') &&
		(cur_play.name2[3] != 'y'))
	{
		if (prompt_yn(name_play_text) == FALSE)
			return;
	}

	// initialize
	for (i = 0; i < CP_NAMELEN; i++)
		instring[0][i] = 0, instring[1][i] = 0;

	instring[0][0] = pos+1;
//	instring[0][1] = 0;
//	instring[1][0] = 0;

	// dim route
	lq_dim = TRUE;
	for( i = 0; i < 3; i++ )
		route_flags[i] = RF_DIM;
	update_play_display(0);

	// put up backplate
	draw_enter_name_backplate();

	// draw box that shows name letters as entering them
	beginobj_with_id(&form_box, NAME_X, NAME_Y-150.0f, NAME_Z-2, CAP_TID ,OID_CAP_NAME, 0);

	// draw letters
	psprite = beginobj_with_id( &letters,
		NAME_X, NAME_Y, NAME_Z,
		CAP_TID, OID_CAP_NAME, 0);
	psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	// draw hilite
	focus = beginobj_with_id( letter_block[pos],
		NAME_X, NAME_Y, NAME_CURSOR_Z,
		CAP_TID, OID_CAP_NAME, 0);

	stick = 0;
	stick_old = 0;
	stick_time = 0;

	write_input_name(instring);

	while(!done)
	{
		// wait for stick or button
		do
		{
			sleep(1);
			stick_cur = (get_player_sw_current()>>(cap_pnum<<3)) & (P_RLDU_MASK);
			stick = (get_player_sw_close()>>(cap_pnum<<3)) & (P_RLDU_MASK);
			but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);

			if (stick_old == stick_cur)
			{
				stick_time++;
			}
			else
			{
				stick_time = 0;
				stick_old = stick_cur;
			}

			if ((stick_time > NAME_REPEAT_TIME) &&
				(stick_time % NAME_REPEAT_RATE == 0))
				stick |= stick_cur;

			// no diagonal movement.  If the vertical part is legal,
			// mask off the horizontal.  Otherwise mask off the vertical.
			if ((stick & (P_UP|P_DOWN)) && (stick & (P_LEFT|P_RIGHT)))
			{
				if ((stick & P_UP) && (pos < MIN_UP ))
				{	// illegal up
					stick &= (P_LEFT|P_RIGHT);
				}
				else if ((stick & P_DOWN) && (pos > MAX_DOWN))
				{	// illegal down
					stick &= (P_LEFT|P_RIGHT);
				}
				else
				{	// legal vertical
					stick &= (P_UP|P_DOWN);
				}
			}

		} while (!stick && !but);

		opos = pos;

		if (stick & P_UP)
			pos -= 4;
		else if (stick & P_DOWN)
			pos += 4;
		else if (stick & P_LEFT)
			pos -= 1;
		else if (stick & P_RIGHT)
			pos += 1;

		if ((pos >= 0) && (pos <= MAX_POS))
		{	// legal move
			instring[in_word][in_pos] = pos+1;
			instring[in_word][in_pos+1] = 0;
		 	snd_scall_bank(cursor_bnk_str,CAP_LET_CUR_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound

		}
		else
		{	// illegal move
			pos = opos;
		}

		if( but & (P_A|P_B))
		{
			if (pos == MAX_POS)
			{	// end key
				instring[in_word][in_pos] = 0;
				if (in_word == 0)
				{
					in_word = 1;
					in_pos = 0;
					instring[in_word][in_pos] = pos+1;
				}
				else
				{
					done = TRUE;
				}
			}
			else if (pos == MAX_POS-1)
			{	// delete key
				instring[in_word][in_pos] = 0;
				if (in_pos == 0)
				{
					if (in_word)
					{	// back up to first word and find last character
						in_word = 0;
						for (i = 0; (i < 10) && (instring[0][i]); i++);
						in_pos = GREATER(0,i-1);
					}
				}
				else
				{
					in_pos--;
				}
			}
			else
			{	// regular key
				instring[in_word][in_pos] = pos+1;
				instring[in_word][in_pos+1] = 0;
				if (++in_pos == CP_NAMELEN-1)
				{
					if( in_word == 0 )
					{
						in_word = 1;
						in_pos = 0;
					}
					else
					{
						done = TRUE;
					}
				}
			}
		 	snd_scall_bank(cursor_bnk_str,CAP_LET_SEL_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
			flash_cursor(focus, CAP_TID);
		}

		// update cursor
		change_img_tmu(focus, letter_block[pos], CAP_TID);

		// update display
		write_input_name(instring);
	}

	// write play name
	for( i = 0; (i < CP_NAMELEN-1) && instring[0][i]; i++ )
		cur_play.name1[i] = letter_trans[instring[0][i]-1];
	for( ; i < CP_NAMELEN; i++ )
		cur_play.name1[i] = '\0';
	for( i = 0; (i < CP_NAMELEN-1) && instring[1][i]; i++ )
		cur_play.name2[i] = letter_trans[instring[1][i]-1];
	for( ; i < CP_NAMELEN; i++ )
		cur_play.name2[i] = '\0';

	// set the change flag
//	play_changed = TRUE;
	copy_play( current_plays + active_play, &cur_play );

	// delete my stuff
	del1c( OID_CAP_NAME, 0xFFFFffff);
	del1c( OID_CAP_BACK, 0xFFFFffff);
	delete_multiple_strings( NAME_TEXT_SID,0xFFFFffff );

	// un-dim route
	lq_dim = FALSE;
	for( i = 0; i < 3; i++ )
		route_flags[i] &= ~RF_DIM;
	update_play_display(0);

	// quit
}

/////////////////////////////////////////////////////////////////////////////
static void get_name( void )
{
	sprite_info_t	*psprite, *focus;
	int				switches, opos;
	int				pos = 0, done = FALSE;
//	char			instring[LETTERS_IN_NAME+1] = {CH_SPC,CH_SPC,CH_SPC,CH_SPC,CH_SPC,CH_SPC,0};
	char			instring[LETTERS_IN_NAME+1] = {CH_DSH,CH_DSH,CH_DSH,CH_DSH,CH_DSH,CH_DSH,0};
	int				in_pos = 0, i;


	// initialize
	instring[in_pos] = letter_trans[pos];	// start showing 'A'

	// put up backplate (done outside this routine to avoid glitch)
//	draw_enter_name_backplate();

	// draw letters
	psprite = beginobj_with_id( &letters,
		NAME_X, NAME_Y, NAME_Z,
		CAP_TID, OID_CAP_NAME, 0);
	psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	// draw hilite
	focus = beginobj_with_id( letter_block[pos],
		NAME_X, NAME_Y, NAME_CURSOR_Z,
		CAP_TID, OID_CAP_NAME, 0);

	print_string( instring, LETTERS_IN_NAME );

	while(!done)
	{
		if (get_but_val_down(cap_pnum))
		{
			if ((pos == MAX_POS) && (in_pos != 0))
			{	// end key (done entering NAME...now get PIN number)
				while (in_pos < LETTERS_IN_NAME)
					instring[in_pos++] = CH_SPC;
				done = TRUE;
			}
			else if (pos == MAX_POS-1)
			{	// delete key
				instring[in_pos] = CH_DSH;
//				instring[in_pos] = CH_SPC;
				if( in_pos > 0 )
					in_pos--;
			}
			else if ((pos != MAX_POS) && (in_pos >= 0))
			{	// regular key
				instring[in_pos] = letter_trans[pos];
				if (++in_pos == LETTERS_IN_NAME)
					done = TRUE;
				else
					instring[in_pos] = letter_trans[pos];		// echo current char in next slot

			}
			snd_scall_bank(cursor_bnk_str,CAP_LET_SEL_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
			flash_cursor(focus, CAP_TID);
		}
		switches = get_joy_val_down(cap_pnum);
		if (switches)
		{
			opos = pos;
			switch(switches)
			{
				case JOY_UP:
					if (pos > 3)		// below first row ?
						pos -= 4;		// yep, move up a row
					break;
				case JOY_DOWN:
					if (pos < 28)		// not on last row ?
						pos += 4;		// nope, move down a row
					break;
				case JOY_LEFT:
				case JOY_DWN_LFT:
				case JOY_UP_LFT:
					if (pos > 0)
						pos -= 1;
					break;
				case JOY_RIGHT:
				case JOY_DWN_RGT:
				case JOY_UP_RGT:
					if (pos < MAX_POS)
						pos += 1;
					break;
			}
			if (opos != pos)
			{
				opos = pos;
			 	snd_scall_bank(cursor_bnk_str,CAP_LET_CUR_SND,VOLUME2,127,PRIORITY_LVL1);
				instring[in_pos] = letter_trans[pos];
				// update cursor
				change_img_tmu(focus, letter_block[pos], CAP_TID);
			}
		}
		// update display
		print_string(instring, LETTERS_IN_NAME);
		sleep(1);
	}

	// copy NAME to global string for later use
	for (i=0; i < LETTERS_IN_NAME; i++)
	{
//		if (instring[i] == CH_DSH)
//			name_pin[i] = CH_SPC;
//		else
			name_pin[i] = instring[i];
	}


	// delete stuff
	del1c( OID_CAP_NAME, 0xFFFFffff);
	del1c( OID_CAP_BACK, 0xFFFFffff);
	delete_multiple_strings( NAME_TEXT_SID,0xFFFFffff );
}


/////////////////////////////////////////////////////////////////////////////
void show_record_status_msg(int status, int plyr)
{
	sprite_info_t	*psprite;

	draw_enter_name_backplate();

	// draw info box
	beginobj_with_id(&form_box, NAME_X, NAME_Y, NAME_Z, CAP_TID ,OID_CAP_NAME, 0);

	set_text_font(FONTID_BAST10);
	set_string_id(NAME_TEXT_SID);
	set_text_position(NAME_X, NAME_Y+8.0f, NAME_Z-1);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, rec_status_msgs[status * 2] );
	set_text_position(NAME_X, NAME_Y-8.0f, NAME_Z-1);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, rec_status_msgs[(status * 2) + 1] );

	if (status)
		snd_scall_bank(cursor_bnk_str,RECORD_FOUND_SND,VOLUME3,127,PRIORITY_LVL3);				// play souund

	wait_for_any_butn_press_plyr(60,180,plyr);

	// kill button downs
	sleep(1);

	// delete stuff
	del1c( OID_CAP_NAME, 0xFFFFffff);
	del1c( OID_CAP_BACK, 0xFFFFffff);
	delete_multiple_strings( NAME_TEXT_SID,0xFFFFffff );
}


/////////////////////////////////////////////////////////////////////////////
//#define INIT_POS	0
#define P_MAX_POS		11
#define P_MIN_UP		3
#define P_MAX_DOWN		8

static void get_pin( void )
{
	sprite_info_t	*psprite, *focus;
	int				switches, but, opos;
	int				pos = 0, done = FALSE;
	char			instring[PIN_NUMBERS+1] = {CH_DSH,CH_DSH,CH_DSH,CH_DSH,0};
//	char			instring[PIN_NUMBERS+1] = {0,0,0,0,0};
	int				in_pos = 0, i,j;

	// initialize
	instring[in_pos] = pin_nbrs_trans[pos];		// start showing '1'

	// put up backplate
	draw_enter_name_backplate();

	// draw box that shows name letters as entering them
	beginobj_with_id(&form_box, NAME_X, NAME_Y-150.0f, NAME_Z-2, CAP_TID ,OID_CAP_NAME, 0);

	// draw letters
	psprite = beginobj_with_id( &mpin,
		PIN_X, PIN_Y, PIN_Z,
		CAP_TID, OID_CAP_NAME, 0);
	psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	// draw hilite
	focus = beginobj_with_id( pin_nbrs_block[pos],
		PIN_X, PIN_Y, NAME_CURSOR_Z,
		CAP_TID, OID_CAP_NAME, 0);

	print_string(instring, PIN_NUMBERS);

	while(!done)
	{
		if (get_but_val_down(cap_pnum))
		{
			if (pos == P_MAX_POS)
			{	// delete key
				instring[in_pos] = CH_DSH;
//				instring[in_pos] = 0;
				if (in_pos > 0)
					in_pos--;
			}
			else
			{	// regular key
				instring[in_pos] = pin_nbrs_trans[pos];
				if (++in_pos == PIN_NUMBERS)
					done = TRUE;
				else
					instring[in_pos] = pin_nbrs_trans[pos];	// echo current char. in next position

			}
			snd_scall_bank(cursor_bnk_str,CAP_LET_SEL_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
			flash_cursor(focus, CAP_TID);
		}
		switches = get_joy_val_down(cap_pnum);
		if (switches)
		{
			opos = pos;
			switch(switches)
			{
				case JOY_UP:
					if (pos > 2)		// below first row ?
						pos -= 3;		// yep, move up a row
					break;
				case JOY_DOWN:
					if (pos < 9)		// not on last row ?
						pos += 3;		// nope, move down a row
					break;
				case JOY_LEFT:
				case JOY_DWN_LFT:
				case JOY_UP_LFT:
					if (pos > 0)
						pos -= 1;
					break;
				case JOY_RIGHT:
				case JOY_DWN_RGT:
				case JOY_UP_RGT:
					if (pos < 11)
						pos += 1;
					break;
			}
			if (opos != pos)
			{
				opos = pos;
			 	snd_scall_bank(cursor_bnk_str,CAP_LET_CUR_SND,VOLUME2,127,PRIORITY_LVL1);
				instring[in_pos] = pin_nbrs_trans[pos];
				// update cursor
				change_img_tmu( focus, pin_nbrs_block[pos], CAP_TID );
			}
		}
		// update display
		print_string( instring, PIN_NUMBERS );
		sleep(1);
	}

	// copy PIN NUMBER to global string for later use
	for (i=LETTERS_IN_NAME,j=0; i < LETTERS_IN_NAME+PIN_NUMBERS; i++,j++)
		name_pin[i] = instring[j];

//fprintf(stderr,"********* PLAYERS name and pin = %s\r\n",name_pin);

	// delete stuff
	del1c( OID_CAP_NAME, 0xFFFFffff);
	del1c( OID_CAP_BACK, 0xFFFFffff);
	delete_multiple_strings( NAME_TEXT_SID,0xFFFFffff );
}

/////////////////////////////////////////////////////////////////////////////
#define CE_X			240.0f
#define CE_Y			306.0f
#define CE_PLATE_Z		4.0f
#define CE_TEXT_Z		3.9f
#define CE_CURSOR_Z		1.4f
#define CE_XINC			83.0f
#define CE_YINC			-96.0f
#define CE_TEXT_X		42.0f
#define CE_TEXT_Y1		-27.0f
#define CE_TEXT_Y15		-36.0f
#define CE_TEXT_Y2		-42.0f
#define CE_TEXT_Y3		-57.0f

static void lcb_create_edit( void )
{
	int				i, selected_play;
	int				but,stick, done = FALSE;
	sprite_info_t	*ce_cursor;
	sprite_info_t	*plt_obj;
	int				sel_pos = 0, old_pos;


	// if we're currently editing a play, write it
	if( active_play != -1 )
	{
		copy_play( current_plays + active_play, &cur_play );
		active_play = -1;
	}

	// put up display plates & names
	set_text_font( FONTID_BAST8T );
	set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );
	set_string_id( CHOOSE_PLAY_SID );
	for( i = 0; i < 9; i++ )
	{
		plt_obj = beginobj_with_id( &mp_plateg,
			CE_X + (float)(i%3) * CE_XINC,
			CE_Y + (float)(i/3) * CE_YINC,
			CE_PLATE_Z,
			CAP_TID, OID_CAP_CREATE, 0);
		plt_obj->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

		if( i < usable_slots )
		{
			if( current_plays[i].flags & CPLT_EXISTS )
			{
				set_text_position(
					CE_X + CE_TEXT_X + (float)(i%3) * CE_XINC,
					CE_Y + ((current_plays[i].name2[0]) ? CE_TEXT_Y1-(!!is_low_res) : CE_TEXT_Y15-(!!is_low_res)) + (float)(i/3) * CE_YINC,
					CE_TEXT_Z );
				oprintf("%dc%s", LT_YELLOW, current_plays[i].name1 );
				set_text_position(
					CE_X + CE_TEXT_X + (float)(i%3) * CE_XINC,
					CE_Y + ((current_plays[i].name1[0]) ? CE_TEXT_Y2-(!!is_low_res) : CE_TEXT_Y15-(!!is_low_res)) + (float)(i/3) * CE_YINC,
					CE_TEXT_Z );
				oprintf("%dc%s", LT_YELLOW, current_plays[i].name2 );
			}
			else
			{
				set_text_position(
					CE_X + CE_TEXT_X + (float)(i%3) * CE_XINC,
					CE_Y + CE_TEXT_Y15 + (float)(i/3) * CE_YINC,
					CE_TEXT_Z );
				oprintf("%dc%s", LT_CYAN, "UNUSED" );
			}
		}
		else
		{
			set_text_position(
				CE_X + CE_TEXT_X + (float)(i%3) * CE_XINC,
				CE_Y + CE_TEXT_Y1 + (float)(i/3) * CE_YINC,
				CE_TEXT_Z );
			oprintf("%dc%s", LT_YELLOW, "AVAILABLE" );
			set_text_position(
				CE_X + CE_TEXT_X + (float)(i%3) * CE_XINC,
				CE_Y + CE_TEXT_Y2 + (float)(i/3) * CE_YINC,
				CE_TEXT_Z );
			oprintf("%dc%s", LT_YELLOW, "AFTER" );
			set_text_position(
				CE_X + CE_TEXT_X + (float)(i%3) * CE_XINC,
				CE_Y + CE_TEXT_Y3 + (float)(i/3) * CE_YINC,
				CE_TEXT_Z );
			oprintf("%dc%d%dc%s", WHITE, wins_needed[i], LT_YELLOW, " WINS" );

		}
	}

	ce_cursor = beginobj_with_id( &mp_cursor,
			CE_X + (float)(sel_pos%3) * CE_XINC,
			CE_Y + (float)(sel_pos/3) * CE_YINC,
			CE_CURSOR_Z,
			CAP_TID, OID_CAP_CREATE, 0);

	// wait for stick or button
	while( !done )
	{
		do
		{
			sleep( 1 );
			stick = (get_player_sw_close()>>(cap_pnum<<3)) & (P_RLDU_MASK);
			but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
		} while( !but && !stick );

		if( but )
		{
			done = TRUE;
		 	snd_scall_bank(cursor_bnk_str,CAP_SEL_PLAY_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
		}
		else if( stick )
		{
			old_pos = sel_pos;

			if ((stick & P_UP) && (sel_pos > 2))
				sel_pos -= 3;

			if ((stick & P_DOWN) && (sel_pos < 6))
				sel_pos += 3;

			if ((stick & P_LEFT) && ((sel_pos % 3) > 0))
				sel_pos -= 1;

			if ((stick & P_RIGHT) && ((sel_pos % 3) < 2))
				sel_pos += 1;

			if( sel_pos >= usable_slots )
			{	// tried to move to unusable play
				sel_pos = old_pos;
			}

			if (old_pos != sel_pos)
			{
			 	snd_scall_bank(cursor_bnk_str,CAP_CUR_PLAY_SND,VOLUME2,127,PRIORITY_LVL1);

				// adjust cursor
				ce_cursor->x = CE_X + (float)(sel_pos%3) * CE_XINC;
				ce_cursor->y = CE_Y + (float)(sel_pos/3) * CE_YINC;
				generate_sprite_verts( ce_cursor );
			}
		}
	}

	flash_obj_white(ce_cursor,3,2);

	del1c(OID_CAP_CREATE, 0xFFFFffff);
	delete_multiple_strings(CHOOSE_PLAY_SID, 0xFFFFffff);

	// which play?
	selected_play = sel_pos;

	// initialize state vars
//	play_changed = FALSE;
	for( i = 0; i < 3; i++ )
		route_flags[i] = 0;
	lq_dim = FALSE;

	// new or edit old?
	if( current_plays[selected_play].flags & CPLT_EXISTS )
	{	// edit existing play
		copy_play( &cur_play, current_plays + selected_play );
	}
	else
	{	// new play.  initialize
		lcb_formation( TRUE );

		// set the play name
		sprintf(cur_play.name1, "new");
		sprintf(cur_play.name2, "play %d", sel_pos+1);

		// always default to shotgun on new plays
		cur_play.flags |= CPLT_QB_FAR;			// shotgun
	}

	// set exists bit
	cur_play.flags |= CPLT_EXISTS;
	cur_play.flags &= ~CPLT_UNUSED;

	active_play = selected_play;
	for( i = LCB_GRAY_TOP; i <= LCB_GRAY_BOT; i++ )
		activate_lcb(i);

	update_play_display(0);

}

/////////////////////////////////////////////////////////////////////////////
static void lcb_choose_setreceiver(void)
{
	sprite_info_t *	box_obj;
	int				stk, but;
	int				i, m, done;

	// first help box
	box_obj = beginobj_with_id( &form_box, ER_HELP_BOX1_X, ER_HELP_BOX1_Y, ER_HELP_BOX_Z, CAP_TID ,OID_CAP_BOX+1, 0);
	box_obj->w_scale = 1.5f;
	box_obj->h_scale = 1.5f;
	generate_sprite_verts( box_obj );

	set_text_font( FONTID_BAST8T );
	set_string_id( INFO_SID );
	set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );

	set_text_position( ER_HELP_BOX1_X, ER_HELP_BOX1_Y+(is_low_res ?  22 :  24), ER_HELP_BOX_Z-1);
	oprintf("%dc%s", LT_CYAN, stage1_pr[0]);
	set_text_position( ER_HELP_BOX1_X, ER_HELP_BOX1_Y+(is_low_res ?   8 :  12), ER_HELP_BOX_Z-1);
	oprintf("%dc%s", LT_CYAN, stage1_pr[1]);

	set_text_position( ER_HELP_BOX1_X, ER_HELP_BOX1_Y+(is_low_res ? -11 : -14), ER_HELP_BOX_Z-1);
	oprintf("%dc%s", LT_YELLOW, stage1_pr[2]);
	set_text_position( ER_HELP_BOX1_X, ER_HELP_BOX1_Y+(is_low_res ? -25 : -25), ER_HELP_BOX_Z-1);
	oprintf("%dc%s", LT_YELLOW, stage1_pr[3]);

	// dim qb, linemen
	lq_dim = TRUE;

	// mask out receiver bits
	m = cur_play.flags & CPLT_PRI_RCVRS;

	er_point = 0;
	done = FALSE;

	// main edit loop
	while (!done)
	{
		// hilite selected receiver
		for( i = 0; i < 3; i++ )
			route_flags[i] = (m & (CPLT_PRI_RCVR1<<i)) ? RF_PRIMARY : RF_DIM;

		update_play_display(0);

		do
		{
			sleep( 1 );

			stk = get_player_sw_close()>>(cap_pnum<<3);
			but = stk & (P_A|P_B|P_C);
			stk = stk & (P_LEFT|P_RIGHT);

			if( stk )
				snd_scall_bank(cursor_bnk_str,CAP_NODE_MENU_SND,VOLUME4,127,PRIORITY_LVL1);
			if( stk & P_LEFT )
				m = (m) ? ((m >> 1) & CPLT_PRI_RCVRS) : CPLT_PRI_RCVR3;
			if( stk & P_RIGHT )
				m = (m) ? ((m << 1) & CPLT_PRI_RCVRS) : CPLT_PRI_RCVR1;

			if( but & P_C )
			{
				// escape
				snd_scall_bank(cursor_bnk_str,CAP_DROP_NODE_SND,VOLUME4,127,PRIORITY_LVL1);
				done = TRUE;
			}
			else
			if( but )
			{
				// select
				cur_play.flags &= ~CPLT_PRI_RCVRS;
				cur_play.flags |= m;

				snd_scall_bank(cursor_bnk_str,CAP_PICK_MENU_SND,VOLUME4,127,PRIORITY_LVL1);
				done = TRUE;
			}
		} while (!stk && !but);
	}

	er_point = -1;

	delete_multiple_strings(INFO_SID, 0xFFFFFFFF);			// get rid of first info box
	del1c(OID_CAP_BOX+1, 0xFFFFFFFF);

	// restore original display settings
	for( i = 0; i < 3; i++ )
		route_flags[i] = 0;
	lq_dim = FALSE;

	update_play_display(0);
}

/////////////////////////////////////////////////////////////////////////////

static void lcb_choose_audibles(void)
{
	sprite_info_t *	ce_cursor;
	int				i;
	int				selected_play = active_play;
	int				stk, but;
	int				done = FALSE;
	int				sel_pos = 0;
	int				old_pos;
	int				cur_audible_index;
	int				off_def;
	int				flip = 0;

	off_def = prompt_off_def();

	// Abort if escape (TURBO) button was hit
	if( off_def < 0 )
		return;

	current_page_total = (off_def == OFFENSE) ?
		PLAYSTUF__NUM_OFFENSE_PAGES:
		PLAYSTUF__NUM_DEFENSE_PAGES;

	load_play_select_textures();

	current_page_index = 0;

	// if we're currently editing a play, write it
	if( active_play != -1 )
	{
		copy_play( current_plays + active_play, &cur_play );
		active_play = -1;
	}

	ce_cursor = beginobj_with_id( &mp_cursor,
			CE_X + (float)(sel_pos%3) * CE_XINC,
			CE_Y + (float)(sel_pos/3) * CE_YINC,
			CE_CURSOR_Z,
			CAP_TID, OID_CAP_AUDIBLE_CURSOR, 0);

	init_play_limbs((off_def == OFFENSE ? 2 : 1), 2);

	for(cur_audible_index = 0; cur_audible_index < PLAYSTUF__AUDIBLES_PER_PLAYER; cur_audible_index++)
	{
		update_help_box( -1 );
		update_play_display( cur_audible_index+1 );
		done = FALSE;

		// wait for stick or button
		while( !done )
		{
			//	Set the audible play
			current_ids->ids[off_def][cur_audible_index] =
				(1 + sel_pos + current_page_index * 9) * (flip ? -1 : 1);

			update_help_box( -1 );
			update_play_page(off_def, current_page_index, flip, sel_pos);

			do
			{
				sleep( 1 );
				stk = get_player_sw_close()>>(cap_pnum<<3);
				but = stk & (P_A|P_B|P_C);
				stk = stk & (P_RLDU_MASK);
			} while( !but && !stk );

			if( but )
			{
				if(but & P_C)
				{
					current_page_index++;

					if(( current_page_index == PLAYSTUF__OFF_PAGE_EXTRA ) && ( off_def == OFFENSE ))
						current_page_index++;

					if ( current_page_index == current_page_total )
						current_page_index = 0;

					snd_scall_bank(cursor_bnk_str,SWITCH_SND,VOLUME4,127,PRIORITY_LVL3);
				}
				else
				if((off_def == DEFENSE) || (but & P_B))
				{
					if ((off_def == OFFENSE) &&
						(current_page_index == PLAYSTUF__OFF_PAGE_ID_CUSTOM) &&
						!(current_plays[sel_pos].flags & CPLT_EXISTS))
					{
						snd_scall_bank(cursor_bnk_str,CAP_DROP_NODE_SND,VOLUME4,127,PRIORITY_LVL1);
					}
					else
					{
			 			snd_scall_bank(cursor_bnk_str,CAP_SEL_PLAY_SND,VOLUME2,127,PRIORITY_LVL1);
						done = TRUE;
					}
				}
				else
				{
					//	'A' button flips plays for offense
					flip ^= 1;
					snd_scall_bank(cursor_bnk_str,SWITCH2_SND,VOLUME4,127,PRIORITY_LVL2);
				}
			}
			else
			if( stk )
			{
				old_pos = sel_pos;

				if((stk & P_UP) && (sel_pos > 2))
					sel_pos -= 3;

				if((stk & P_DOWN) && (sel_pos < 6))
					sel_pos += 3;

				if((stk & P_LEFT) && ((sel_pos % 3) > 0))
					sel_pos -= 1;

				if((stk & P_RIGHT) && ((sel_pos % 3) < 2))
					sel_pos += 1;

				if(sel_pos != old_pos)
				{
			 		snd_scall_bank(cursor_bnk_str,CAP_CUR_PLAY_SND,VOLUME2,127,PRIORITY_LVL1);
					// adjust cursor
					ce_cursor->x = CE_X + (float)(sel_pos%3) * CE_XINC;
					ce_cursor->y = CE_Y + (float)(sel_pos/3) * CE_YINC;
					generate_sprite_verts( ce_cursor );
				}
			}
		}
		flash_obj_white(ce_cursor,3,2);
	}

	del1c( OID_CAP_CREATE, -1 );
	del1c( OID_CAP_AUDIBLE_CURSOR, -1 );
	delete_multiple_strings( CHOOSE_PLAY_SID, -1 );
	delete_multiple_strings( NO_ACTIVE_SID, -1 );

	clobber_playsel();
	clean_playsel();

	unlock_multiple_textures( PLAY_SELECT_TID, -1 );
	delete_multiple_textures( PLAY_SELECT_TID, -1 );

	active_play = selected_play;

	if( active_play != -1 )
	{
		// restore play being edited
		copy_play( &cur_play, current_plays + active_play );

		// initialize state vars
		for( i = 0; i < 3; i++ )
			route_flags[i] = 0;

		lq_dim = FALSE;
//		play_changed = FALSE;
	}

	update_play_display( 0 );
}

/////////////////////////////////////////////////////////////////////////////
#ifndef NO_CARD_READER
static void lcb_load_plays( int plyr )
{
	float	ypos;
	int		i,stick,but;
	unsigned char port_status;
	int		cur_sel, tries;
	unsigned char status1, status2;
	int		done, ll_abort, fh, timeout;
	int		wait_ticks, cerror = 0;
	char	*lname, *lpin;
	sprite_info_t	*hilite = (sprite_info_t *)NULL;
	PlayBook	n64_book;

	// return if no reader present
	if (!reader_exists)
		return;

	// initialize lname and lpin
	lname = name_pin;
	lpin = name_pin + 6;

	draw_enter_name_backplate();
	
	// card_reader = get_card_reader_status();
	set_text_font(FONTID_BAST10);
	set_string_id(NAME_TEXT_SID);
	ypos = 290.0f;
	for (i=0; card_reader_info[i]; i++)
	{
		set_text_position(NAME_X, ypos-(15 * i), NAME_Z-1);
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, card_reader_info[i]);
	}

	set_text_font(FONTID_BAST18);
	set_text_position(NAME_X, 180.0f, NAME_Z-1);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, card_reader_question[0]);
	set_text_font(FONTID_BAST18);
	set_text_position(NAME_X, 155.0f, NAME_Z-1);
	oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, card_reader_question[1]);

	// ask yes/no
	
	beginobj_with_id(&myn2, NAME_X, 100.0f, YESNO_Z2, CAP_TID, OID_CAP_YESNO, 0);
	hilite = beginobj_with_id(&mno2, NAME_X, 100.0f, YESNO_Z2-.5f, CAP_TID, OID_CAP_YESNO, 0);
	
	done = FALSE;
	wait_ticks = 480;
	cur_sel = 1;			// default to no
	while ((!done) && (wait_ticks >= 0))
	{
		stick = (get_player_sw_close()>>(plyr<<3)) & (P_UP|P_DOWN);
		but = (get_player_sw_close()>>(plyr<<3)) & (P_A|P_B|P_C);
	
		wait_ticks--;
		if (wait_ticks == 0)
		{
			stick |= P_DOWN;
			but |= P_B;
		}
			
		if ((stick & P_UP) && (cur_sel == 1))
		{
			cur_sel = 0;
			snd_scall_bank(cursor_bnk_str,CAP_YESNO_CUR_SND,VOLUME4,127,PRIORITY_LVL2);
			change_img_tmu(hilite, &myes2, CAP_TID );
		}
		else if ((stick & P_DOWN) && (cur_sel == 0))
		{
			cur_sel = 1;
			snd_scall_bank(cursor_bnk_str,CAP_YESNO_CUR_SND,VOLUME4,127,PRIORITY_LVL2);
			change_img_tmu(hilite, &mno2, CAP_TID );
		}
		if (but)
		{
			snd_scall_bank(cursor_bnk_str,CAP_YESNO_SEL_SND,VOLUME3,127,PRIORITY_LVL2);
			done = TRUE;
		}
		sleep(1);
	}

	del1c(OID_CAP_YESNO, 0xFFFFffff);
	delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);

	if (cur_sel)	// yes selected?
	{
		cerror = CERR_NOSEL;
		del1c(OID_CAP_YESNO, 0xFFFFffff);
		del1c(OID_CAP_NAME, 0xFFFFffff);
		del1c(OID_CAP_BACK, 0xFFFFffff);
		delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);
		return;
	}

	// open the door
	joyplay_do_function(JPFUNC_DOOR_OPEN);
		sleep(OPEN_WAIT);
	open2_on_exit = TRUE;
	jp_status(&port_status);

	tries = 2;
	while ((port_status & JP_OPTO_CLOSE) &&
			!(port_status & JP_OPTO_OPEN) &&
			(tries))
	{
		tries--;
		joyplay_do_function(JPFUNC_DOOR_CLOSE2);
		sleep(CLOSE_WAIT);
		joyplay_do_function(JPFUNC_DOOR_OPEN);
		open2_on_exit = TRUE;
		sleep(OPEN_WAIT);
		jp_status(&port_status);
	}

	// prompt for card
	set_text_font(FONTID_BAST18);
	set_string_id(NAME_TEXT_SID);
	set_text_position(NAME_X, 180.0f, NAME_Z-1.0f);
	oprintf("%dj%dcINSERT PAK", (HJUST_CENTER|VJUST_CENTER), WHITE );
	sleep(1);

	// wait for card or abort
	do
	{
		sleep(1);
		ll_abort = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
	} while (!joyplay_check_card() && !ll_abort);

	// release door
	joyplay_do_function(JPFUNC_DOOR_OPEN2);
	sleep(OPEN2_WAIT);
	open2_on_exit = FALSE;

	// read the card
	del1c(OID_CAP_NAME, 0xFFFFffff);
	delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);
	set_text_font(FONTID_BAST18);
	set_string_id(NAME_TEXT_SID);
	set_text_position(NAME_X, 180.0f, NAME_Z-1.0f);
	oprintf("%dj%dcREADING...", (HJUST_CENTER|VJUST_CENTER), WHITE );
	sleep(1);

	set_text_font(FONTID_BAST18);
	set_string_id(NAME_TEXT_SID);

	if (!cerror)	// card present?
		if ((cerror = joyplay_check_card() ? 0 : CERR_NOCARD))
		{	// no card
			delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);
			fprintf( stderr, "cap: No N64 controller pak found\n" );
			set_text_position(NAME_X, 180.0f, NAME_Z-1.0f);
			oprintf("%dj%dcNO CONTROLLER", (HJUST_CENTER|VJUST_CENTER), LT_RED);
			set_text_position(NAME_X, 155.0f, NAME_Z-1.0f);
			oprintf("%dj%dcPAK FOUND", (HJUST_CENTER|VJUST_CENTER), LT_RED);
			wait_for_any_butn_press_plyr(60,240,plyr);
		}

	if (!cerror)	// Blitz file present?
	{
		// Inc N64CARD_PLUGINS_AUD
		add_to_audit(N64CARD_PLUGINS_AUD, 1);

		if ((cerror = ((fh = joyplay_check_file()) != -1) ? 0 : CERR_NOFILE))
		{	// no Blitz file
			delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);
			fprintf( stderr, "cap: No Blitz play file on pak\n" );
			set_text_position(NAME_X, 180.0f, NAME_Z-1.0f);
			oprintf("%dj%dcNO BLITZ", (HJUST_CENTER|VJUST_CENTER), LT_RED);
			set_text_position(NAME_X, 155.0f, NAME_Z-1.0f);
			oprintf("%dj%dcFILE FOUND", (HJUST_CENTER|VJUST_CENTER), LT_RED);
			wait_for_any_butn_press_plyr(60,240,plyr);
		}
	}

	if (!cerror)	// Plays for this guy?
		if ((cerror = joyplay_get_playbook(fh,lname,lpin,&n64_book) ? 0 : CERR_NOPLAYS))
		{	// no plays
			delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);
			fprintf( stderr, "cap: No plays found for %s\n", name_pin );
			set_text_position(NAME_X, 180.0f, NAME_Z-1.0f);
			oprintf("%dj%dcNO PLAYS MATCH", (HJUST_CENTER|VJUST_CENTER), LT_RED);
			set_text_position(NAME_X, 155.0f, NAME_Z-1.0f);
			oprintf("%dj%dcNAME AND PIN", (HJUST_CENTER|VJUST_CENTER), LT_RED);
			wait_for_any_butn_press_plyr(60,240,plyr);
		}

	if (!cerror)
	{
		// Inc N64CARD_BLITZED_AUD
		add_to_audit(N64CARD_BLITZED_AUD, 1);

		// all clear & plays read
//		del1c(OID_CAP_YESNO, 0xFFFFffff);
		delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);
		
		set_text_font(FONTID_BAST18);
		set_string_id(NAME_TEXT_SID);
		set_text_position(NAME_X, 180.0f, NAME_Z-1.0f);
		oprintf("%dj%dcPLAYS LOADED", (HJUST_CENTER|VJUST_CENTER), WHITE);
		set_text_position(NAME_X, 155.0f, NAME_Z-1.0f);
		oprintf("%dj%dcSUCCESSFULLY", (HJUST_CENTER|VJUST_CENTER), WHITE);

//		fprintf( stderr, "cap: Plays read successfully\n" );

		// copy any N64 plays found into their corresponding slots
		for( i = 0; i < 9; i++ )
		{
			if (n64_book.play[i].flags & CPLT_EXISTS)
			{
				if (!validate_play(n64_book.play + i))
				{
					copy_play( current_plays + i, n64_book.play + i );
					// Inc N64CARD_PLAY_OK_AUD
					add_to_audit(N64CARD_PLAY_OK_AUD, 1);
				}
				else
				{
					// Inc N64CARD_PLAY_BD_AUD
					add_to_audit(N64CARD_PLAY_BD_AUD, 1);
				}
			}
		}

		// guy can now use all nine slots
		usable_slots = 9;

		// show msg for a little bit
		wait_for_any_butn_press_plyr(60,240,plyr);
		unhilite_lcb( lcb_focus );
		lcb_focus = 0;
		hilite_lcb( lcb_focus );
		active_play = -1;
//		force_create = TRUE;
	}

	// wait 2 seconds or for button, then get rid of card
	timeout = 114;
	do
	{
		sleep(1);
		ll_abort = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
	} while (--timeout && !ll_abort);

	delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);

	set_text_font(FONTID_BAST18);
	set_string_id(NAME_TEXT_SID);
	set_text_position(NAME_X, 180.0f, NAME_Z-1.0f);
	oprintf("%dj%dcREMOVE PAK", (HJUST_CENTER|VJUST_CENTER), LT_RED);

	// optos say door is open, prompt for remove card
	// and wait indefinitely.  If they don't, wait only
	// five seconds.
	jp_status(&port_status);

	if ((port_status & (JP_OPTO_CLOSE|JP_OPTO_OPEN)) == (JP_OPTO_OPEN))
	{
//		printf( "optos look good\n" );
		do {
			sleep(1);
			jp_status(&port_status);
		} while(!(port_status & JP_OPTO_CLOSE));
	}
	else
	{
//		printf( "optos look bad\n" );
		timeout = 5*57;
		do
		{
			sleep(1);
			ll_abort = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
		} while (--timeout && !ll_abort);
	}


	// close motor
	joyplay_do_function(JPFUNC_DOOR_CLOSE);
	sleep(CLOSE_WAIT);

	// delete stuff
	del1c(OID_CAP_YESNO, 0xFFFFffff);
	del1c(OID_CAP_NAME, 0xFFFFffff);
	del1c(OID_CAP_BACK, 0xFFFFffff);
	delete_multiple_strings(NAME_TEXT_SID,0xFFFFffff);
}
#endif	//	NO_CARD_READER
//	END lcb_load_plays
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// clear the old input string (if any) and write the new one
static void print_string( char *string , int len)
{
	// delete old
	delete_multiple_strings( NAME_TEXT_SID,0xFFFFffff );

	set_text_font(FONTID_BAST10);
	set_string_id(NAME_TEXT_SID);

	set_text_position( NAME_TEXT_X,NAME_TEXT_Y1,NAME_TEXT_Z );

	oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,string);
}

/////////////////////////////////////////////////////////////////////////////
// clear the old input string (if any) and write the new one
static void write_input_name( char instring[2][CP_NAMELEN] )
{
	char		outstring[11];
	int			i;

	// delete old
	delete_multiple_strings( NAME_TEXT_SID,0xFFFFffff );

	set_text_font(FONTID_BAST10);
	set_string_id(NAME_TEXT_SID);

//	for( i = 0; (i < 10) && (instring[0][i]); i++ )
//		outstring[i] = letter_trans[(int)(instring[0][i])-1];
	for (i = 0; i < CP_NAMELEN-1; i++ )
	{
		if (instring[0][i])
			outstring[i] = letter_trans[(int)(instring[0][i])-1];
		else
			outstring[i] = CH_DSH;
			
	}
	outstring[i] = '\0';

	set_text_position( NAME_TEXT_X,NAME_TEXT_Y1,NAME_TEXT_Z );
	oprintf("%dj%dc%s",
		(HJUST_CENTER|VJUST_CENTER),
		LT_YELLOW,
		outstring );

//	for( i = 0; (i < 10) && (instring[1][i]); i++ )
//		outstring[i] = letter_trans[(int)(instring[1][i])-1];
	for (i = 0; i < CP_NAMELEN-1; i++ )
	{
		if (instring[1][i])
			outstring[i] = letter_trans[(int)(instring[1][i])-1];
		else
			outstring[i] = CH_DSH;
			
	}
	outstring[i] = '\0';

	set_text_position( NAME_TEXT_X,NAME_TEXT_Y2,NAME_TEXT_Z );
	oprintf("%dj%dc%s",
		(HJUST_CENTER|VJUST_CENTER),
		LT_YELLOW,
		outstring );
}

/////////////////////////////////////////////////////////////////////////////
// if flag is TRUE, then we've been called from create-edit and
// should simply copy the default formation without any kind of
// prompting
static void lcb_formation( int flag )
{
	int			i,j, active_route = FALSE;
	int			but,stick, done = FALSE;
	int			old_form;
	sprite_info_t	*pcursor;

	// does any receiver have a route?
	for( i = 0; i < 3; i++ )
		if( cur_play.wpn[i][1].x )
			active_route = TRUE;

	// is there a primary receiver?
	if( !flag && (cur_play.flags & CPLT_PRI_RCVRS) )
		active_route = TRUE;

	// if any routes, prompt a warning
	if(( active_route ) &&
		(!flag) &&
		(prompt_yn(formation_text) == FALSE))
		return;

	// if being called from lcb_create_edit, just do stuff and exit

	// clear primary receiver
	cur_play.flags &= ~CPLT_PRI_RCVRS;

	// clear out old pass routes
	for( i = 0; i < 3; i++ )
	{
		for( j = 1; j < CP_COUNT; j++ )
		{
			cur_play.wpn[i][j].x = 0;
			cur_play.wpn[i][j].y = 0;
			cur_play.wpn[i][j].flags = 0;
		}
	}

	// copy new positions
	for(i = 0; i < 3; i++)
	{
		cur_play.wpn[i][0].x = (*formations[cur_form])[i][0];
		cur_play.wpn[i][0].y = (*formations[cur_form])[i][1];
		cur_play.wpn[i][0].flags = CPPT_LAST;
	}

	// set qb position
	if((*formations[cur_form])[3][0])
		cur_play.flags |= CPLT_QB_FAR;
	else
		cur_play.flags &= ~CPLT_QB_FAR;

	// set the motion index
	cur_play.motion = (*formations[cur_form])[3][1];

	// set the change flag
//	play_changed = TRUE;

	// update display and exit
	update_play_display(0);

	if (flag)
		return;

	// player wants a NEW formation
	old_form = cur_form;

	// delete old copy of info box
	delete_multiple_strings(INFO_SID,0xFFFFffff);
	del1c( OID_CAP_BOX, 0xFFFFffff );

	// create box and cursor
	beginobj_with_id(&fbox, FORM_SEL_BOX_X, FORM_SEL_BOX_Y, FORM_SEL_BOX_Z, CAP_TID ,OID_CAP_BOX, 0);
	beginobj_with_id(&ftext, FORM_SEL_BOX_X, FORM_SEL_BOX_Y, FORM_SEL_TXT_Z, CAP_TID ,OID_CAP_BOX, 0);
	pcursor = beginobj_with_id(&fcursor,
								FORM_SEL_BOX_X,
								FORM_CUR_Y - (float)(cur_form * FORM_CUR_YINC),
								FORM_CUR_Z,
								CAP_TID,
								OID_CAP_BOX, 0);

	// put up info box
	beginobj_with_id(&form_box, FORM_BOX_X, FORM_BOX_Y, FORM_BOX_Z, CAP_TID ,OID_CAP_BOX, 0);
	set_text_font(FONTID_BAST10);
	set_string_id(INFO_SID);
	set_text_position( FORM_BOX_X,FORM_BOX_Y+8.0f,FORM_BOX_Z-1 );
	oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,formation_info[0]);
	set_text_position( FORM_BOX_X,FORM_BOX_Y-8.0f,FORM_BOX_Z-1 );
	oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,formation_info[1]);

	while(!done)
	{
		do
		{
			sleep(1);
			stick = (get_player_sw_close()>>(cap_pnum<<3)) & (P_RLDU_MASK);
			but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
		} while( !but && !stick );

		if (but)
		{
			done = TRUE;
			snd_scall_bank(cursor_bnk_str,CAP_FRM_SEL_SND,VOLUME3,127,PRIORITY_LVL2);
		}

		// which formation do we want?
		if ((stick & P_UP) && (cur_form > 0))
			cur_form = (cur_form-1) % 5;
		else if (stick & P_DOWN)
			cur_form = (cur_form+1) % 5;

		if (old_form != cur_form)
		{
			old_form = cur_form;

			// sound for cursor moving
			snd_scall_bank(cursor_bnk_str,CAP_FRM_CUR_SND,VOLUME4,127,PRIORITY_LVL1);

			// adjust cursor
			pcursor->y = FORM_CUR_Y - (float)(cur_form * FORM_CUR_YINC);
			generate_sprite_verts(pcursor);

			// copy new positions
			for(i = 0; i < 3; i++)
			{
				cur_play.wpn[i][0].x = (*formations[cur_form])[i][0];
				cur_play.wpn[i][0].y = (*formations[cur_form])[i][1];
			}

			// set qb position
			if((*formations[cur_form])[3][0])
				cur_play.flags |= CPLT_QB_FAR;
			else
				cur_play.flags &= ~CPLT_QB_FAR;

			// set the motion index
			cur_play.motion = (*formations[cur_form])[3][1];

			// set the change flag
//			play_changed = TRUE;

			// update display and exit
			update_play_display(0);
		}
	}
	// delete old copy of info box
	delete_multiple_strings(INFO_SID,0xFFFFffff);
	del1c( OID_CAP_BOX, 0xFFFFffff );
}

#if 0
/////////////////////////////////////////////////////////////////////////////
static void lcb_qb( void )
{
	int			but,stick, done = FALSE;
	int			pos, old_pos;
	sprite_info_t	*pcursor;


	if (cur_play.flags & CPLT_QB_FAR)
		pos = 1;
	else
		pos = 0;
	old_pos = pos;

	// delete old copy of info box
	delete_multiple_strings(INFO_SID,0xFFFFffff);
	del1c( OID_CAP_BOX, 0xFFFFffff );

	// create box and cursor
	beginobj_with_id(&qbox, QB_BOX_X, QB_BOX_Y, QB_BOX_Z, CAP_TID ,OID_CAP_BOX, 0);
	beginobj_with_id(&qtext, QB_BOX_X, QB_BOX_Y, QB_TXT_Z, CAP_TID ,OID_CAP_BOX, 0);
	pcursor = beginobj_with_id(&qcursor,
								QB_BOX_X,
								QB_CUR_Y - (float)(pos * QB_CUR_YINC),
								QB_CUR_Z,
								CAP_TID,
								OID_CAP_BOX, 0);

	// put up info box
	beginobj_with_id(&form_box, QB_INFO_BOX_X, QB_INFO_BOX_Y, QB_INFO_BOX_Z, CAP_TID ,OID_CAP_BOX, 0);
	set_text_font(FONTID_BAST10);
	set_string_id(INFO_SID);
	set_text_position(QB_INFO_BOX_X, QB_INFO_BOX_Y+8.0f, QB_INFO_BOX_Z-1 );
	oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,formation_info[0]);
	set_text_position(QB_INFO_BOX_X, QB_INFO_BOX_Y-8.0f, QB_INFO_BOX_Z-1 );
	oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,formation_info[1]);

	while(!done)
	{
		do
		{
			sleep(1);
			stick = (get_player_sw_close()>>(cap_pnum<<3)) & (P_RLDU_MASK);
			but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
		} while( !but && !stick );

		if (but)
		{
			done = TRUE;
			snd_scall_bank(cursor_bnk_str,CAP_QB_SEL_SND,VOLUME3,127,PRIORITY_LVL2);
		}

		// which qb position do we want?
		if ((stick & P_UP) && (pos > 0))
		{
			pos--;
			cur_play.flags &= ~CPLT_QB_FAR;			// regular set
		}
		else if ((stick & P_DOWN) && (pos < 1))
		{
			pos++;
			cur_play.flags |= CPLT_QB_FAR;			// shotgun
		}

		// update cursor ?
		if (old_pos != pos)
		{
			old_pos = pos;

			// sound for cursor moving
			snd_scall_bank(cursor_bnk_str,CAP_QB_CUR_SND,VOLUME4,127,PRIORITY_LVL1);

			// adjust cursor
			pcursor->y = QB_CUR_Y - (float)(pos * QB_CUR_YINC);
			generate_sprite_verts(pcursor);

			// set the change flag
			play_changed = TRUE;

			// update display and exit
			update_play_display(0);
		}
	}
	// delete old copy of info box
	delete_multiple_strings(INFO_SID,0xFFFFffff);
	del1c( OID_CAP_BOX, 0xFFFFffff );
}
#endif

/////////////////////////////////////////////////////////////////////////////
static void lcb_edit_route( int rec )
{
	int				i, done = FALSE;
	int				active_route = FALSE;
	int				stick,but, dx,dy;
	int				blocked_mask, astick;
	int				s49,old49,stick_time;
	float			fx,fy;
	cap_point_t		*cur_route;
	int				action;
	sprite_info_t	*overlay1 = (sprite_info_t *)NULL;
	sprite_info_t	*overlay2 = (sprite_info_t *)NULL;
	sprite_info_t	*box_obj;
	int				was_low_res = is_low_res;

	// set cur_route
	cur_route = cur_play.wpn[rec];
	er_point = 0;

	// dim qb, linemen, and other receivers
	for( i = 0; i < 3; i++ )
		route_flags[i] = (i == rec) ? RF_ACTIVE : RF_DIM;
	lq_dim = TRUE;
	
	// deactivate all lcb buttons
//	for(i=0; i < LCB_COUNT; i++)
//		deactivate_lcb(i);

	update_play_display(0);

	s49 = 24;
	stick_time = 0;
	fx = 0.0f;
	fy = 0.0f;

	mirror_dir = 0;

	// does any receiver have a route?
	for (i=0; i < 3; i++)
	{
		if (cur_play.wpn[i][1].x)
			active_route = TRUE;
	}

	// if route exists...ask if want to delete it.
	if (cur_play.wpn[rec][1].x)
	{
		if (prompt_yn(delete_route_text) == FALSE)
		{
			active_route = TRUE;
		}
		else
		{
			// delete route...and continue
			for (i=1; i < CP_COUNT; i++)
			{
				cur_play.wpn[rec][i].x = 0;
				cur_play.wpn[rec][i].y = 0;
				cur_play.wpn[rec][i].flags = 0;
			}
			// set new (default) position
			cur_play.wpn[rec][0].flags = CPPT_LAST;
			update_play_display(0);
		}
	}

	if (active_route == FALSE)
	{
		// first help box
		box_obj = beginobj_with_id(&form_box, ER_HELP_BOX1_X, ER_HELP_BOX1_Y, ER_HELP_BOX_Z, CAP_TID ,OID_CAP_BOX+1, 0);
		box_obj->w_scale = 1.5f;
		box_obj->h_scale = 1.5f;
		generate_sprite_verts(box_obj);
	
		set_text_font(FONTID_BAST8T);
		set_string_id(INFO_SID);
		set_text_position(ER_HELP_BOX1_X,ER_HELP_BOX1_Y+24.0f-(is_low_res ? 2.0f : 0.0f),ER_HELP_BOX_Z-1);
		oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage1_ht[0]);
		set_text_position(ER_HELP_BOX1_X,ER_HELP_BOX1_Y+12.0f-(is_low_res ? 5.0f : 0.0f),ER_HELP_BOX_Z-1);
		oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage1_ht[1]);

		set_text_position(ER_HELP_BOX1_X,ER_HELP_BOX1_Y-12.0f,ER_HELP_BOX_Z-1);
		oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage1_ht[2]);
		set_text_position(ER_HELP_BOX1_X,ER_HELP_BOX1_Y-25.0f,ER_HELP_BOX_Z-1);
		oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage1_ht[3]);
	}

	// main edit loop
	while (!done)
	{
		if (er_point)
			update_help_text( (char **)edit_route_text_node );
		else
			update_help_text( (char **)edit_route_text_root );

		is_low_res = 0;
		// create overlay if needed
		if( er_point == 0 )
		{
			beginobj_with_id(
				(rec == cur_play.motion) ? (was_low_res ? &b_thicklr : &border_thick) : (was_low_res ? &b_thinlr : &border_thin),
//				(rec == cur_play.motion) ? &border_thick : &border_thin,
				ER_OVERLAY_X,
				ER_OVERLAY_Y - (was_low_res ? 128.0f : 0.0f),
				ER_OVERLAY_Z,
				CAP_TID, OID_ER_OVERLAY, 0 );
			beginobj_with_id(
				(rec == cur_play.motion) ? (was_low_res ? &b_thicklr_c1 : &border_thick_c1) : (was_low_res ? &b_thinlr_c1 : &border_thin_c1),
				ER_OVERLAY_X,
				ER_OVERLAY_Y - (was_low_res ? 128.0f : 0.0f),
				ER_OVERLAY_Z,
				CAP_TID, OID_ER_OVERLAY, 0 );
		}

		is_low_res = was_low_res;

		// update special count
		count_specials_used( &cur_play );

		// wait for stick or button
		do
		{
			sleep( 1 );
			old49 = s49;
			s49 = (get_player_49way_current() >> (cap_pnum<<3)) & P_SWITCH_MASK;

			if (s49 == 24)
			{
				stick_time = 0;
				fx = 0.0f;
				fy = 0.0f;
			}
			else
			{
				if (old49 == 24)
				{
					fy =  1.5f * icos( dir49_table[s49] );
					fx = -1.5f * isin( dir49_table[s49] );
					stick_time = 0;
				}
				else
				{
					stick_time++;
				}
			}

			if ((stick_time > ER_STICK_REPEAT_TIME) &&
				(stick_time % ER_STICK_REPEAT_RATE == 0))
			{
				fy +=  icos( s49 != 24 ? dir49_table[s49] : 0 );
				fx += -isin( s49 != 24 ? dir49_table[s49] : 0 );
			}

			stick = 0;
			if( fx >= 1.0f )
			{
				fx -= 1.0f;
				stick |= P_RIGHT;
			}
			if( fx <= -1.0f )
			{
				fx += 1.0f;
				stick |= P_LEFT;
			}
			if( fy >= 1.0f )
			{
				fy -= 1.0f;
				stick |= P_UP;
			}
			if( fy <= -1.0f )
			{
				fy += 1.0f;
				stick |= P_DOWN;
			}

			but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
		} while( !stick && !but );

		if( but & P_C )
		{	// escape
			if( !er_point )
			{	// on root note -- quit

				delete_multiple_strings(INFO_SID, 0xFFFFFFFF);			// get rid of first info box
				del1c(OID_CAP_BOX+1, 0xFFFFFFFF);

				done = TRUE;
			}
			else
			{	// back up one node
				er_point -= 1;
			}
		}
		else if( but )
		{	// plant current point
			// prompt for special
			if (er_point)
			{
				snd_scall_bank(cursor_bnk_str,CAP_DROP_NODE_SND,VOLUME4,127,PRIORITY_LVL1);
				action = er_menu( cur_route, er_point );
			}
			else
			{
				action = ER_ACTION_NONE;
				cur_route[er_point].flags = 0;

				if (active_route == FALSE)
				{
					// get rid of first info box
					delete_multiple_strings(INFO_SID, 0xFFFFFFFF);
					del1c(OID_CAP_BOX+1, 0xFFFFFFFF);

					// next stage of instructions
					box_obj = beginobj_with_id(&form_box, ER_HELP_BOX2_X, ER_HELP_BOX2_Y, ER_HELP_BOX_Z, CAP_TID ,OID_CAP_BOX+1, 0);
					box_obj->w_scale = 1.5f;
					box_obj->h_scale = 1.5f;
					generate_sprite_verts(box_obj);

					set_text_font(FONTID_BAST8T);
					set_string_id(INFO_SID);
					set_text_position(ER_HELP_BOX2_X,ER_HELP_BOX2_Y+24.0f-(is_low_res ? 3.0f : 0.0f),ER_HELP_BOX_Z-1);
					oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage2_ht[0]);
					set_text_position(ER_HELP_BOX2_X,ER_HELP_BOX2_Y+12.0f-(is_low_res ? 6.0f : 0.0f),ER_HELP_BOX_Z-1);
					oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage2_ht[1]);

					set_text_position(ER_HELP_BOX2_X,ER_HELP_BOX2_Y-12.0f,ER_HELP_BOX_Z-1);
					oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage2_ht[2]);
					set_text_position(ER_HELP_BOX2_X,ER_HELP_BOX2_Y-26.0f,ER_HELP_BOX_Z-1);
					oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage2_ht[3]);
				}
			}

			if (action == ER_ACTION_ABORT)
			{	// do nothing at all
			}
			else if (action == ER_ACTION_DELETE)
			{	// kill current node, shuffle later ones down
				for( i = er_point+1; i < CP_COUNT; i++ )
					memcpy( cur_route+i-1, cur_route+i, sizeof( cap_point_t ));

				cur_route[i-1].x = 0;
				cur_route[i-1].y = 0;
				cur_route[i-1].flags = 0;

				// if new current node doesn't exist, back up one
				if( cur_route[er_point].x == 0 )
					er_point--;
			}
			else
			{	// write node and advance
				cur_route[er_point].flags |= er_action_flags[action];

				er_point++;

				if (er_point == CP_COUNT)
				{	// was last node
					cur_route[er_point].flags |= CPPT_LAST;
					done = TRUE;
				}
				else if (action == ER_ACTION_DONE)
				{	// wasn't last node, but chose done anyway
					// if er_point is valid, clear DONE flag on previous
					if (cur_route[er_point].x)
						cur_route[er_point-1].flags &= ~CPPT_LAST;
					done = TRUE;
				}
				else if( cur_route[er_point].x == 0 )
				{	// adding a new node
					find_new_point( cur_route, er_point-1, cur_route + er_point );
				}
			}
		}
		else
		{	// stick
			// to avoid illegal diagonal motions, do all horiz, then all vert
			// vert first
			blocked_mask = _node_chain_blocked( cur_route + er_point, er_point, rec );

			astick = stick & ~blocked_mask;

			dx = 0;
			dy = 0;
			if( astick & P_LEFT )
			{
				dx = -1;
				snd_scall_bank(cursor_bnk_str,CAP_JOY_REPEAT_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
			}
			if( astick & P_RIGHT )
			{
				dx = 1;
				snd_scall_bank(cursor_bnk_str,CAP_JOY_REPEAT_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
			}

			_move_downstream( cur_route + er_point, er_point, dx, dy );

			// now do horz
			blocked_mask = _node_chain_blocked( cur_route + er_point, er_point, rec );

			astick = stick & ~blocked_mask;

			dx = 0;
			dy = 0;
			if( astick & P_UP )
			{
				dy = 1;
				snd_scall_bank(cursor_bnk_str,CAP_JOY_REPEAT_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound

				if (cur_route[er_point].y == 15)
				{
					// get rid of stage 2 info box (if move cursor over it)
					delete_multiple_strings(INFO_SID, 0xFFFFFFFF);
					del1c(OID_CAP_BOX+1, 0xFFFFFFFF);
				}

			}
			if( astick & P_DOWN )
			{
				dy = -1;
				snd_scall_bank(cursor_bnk_str,CAP_JOY_REPEAT_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
			}

			_move_downstream( cur_route + er_point, er_point, dx, dy );
		}

		update_play_display(0);

		// delete any overlay
		del1c( OID_ER_OVERLAY, 0xFFFFffff );
	}

	// restore original display settings
	for( i = 0; i < 3; i++ )
		route_flags[i] = 0;
	lq_dim = FALSE;

	// activate all lcb buttons
//	for(i=0; i < LCB_COUNT; i++)
//		activate_lcb(i);

	update_play_display(0);
}

#if 0
/////////////////////////////////////////////////////////////////////////////
static void lcb_save( void )
{
	int			i;

	// copy scratchpad play to bank of 9
	copy_play( current_plays + active_play, &cur_play );

	// restore lcb focus to top
	unhilite_lcb( lcb_focus );
	active_play = -1;
	lcb_focus = 0;
	hilite_lcb( lcb_focus );
	update_help_text( lcb_help_texts[lcb_focus] );

	// deactivate middle chunk of menu
	for( i = LCB_GRAY_TOP; i <= LCB_GRAY_BOT; i++ )
		deactivate_lcb( i );

	// get rid of play display
	update_play_display(0);
}

/////////////////////////////////////////////////////////////////////////////
static void lcb_abort( void )
{
	int			i;

	if ((play_changed) &&
		(prompt_yn(abort_text) == FALSE))
		return;

	active_play = -1;
	unhilite_lcb( lcb_focus );
	lcb_focus = 0;
	hilite_lcb( lcb_focus );
	for( i = LCB_GRAY_TOP; i <= LCB_GRAY_BOT; i++ )
		deactivate_lcb( i );
	update_help_text( lcb_help_texts[lcb_focus] );
	update_play_display(0);
}
#endif

/////////////////////////////////////////////////////////////////////////////
static void lcb_exit( void )
{
	int		bank;

	if ( prompt_yn(exit_text) == FALSE )
		return;

	force_create = 1;
}


/////////////////////////////////////////////////////////////////////////////
static void show_cap_info_screen(int plyr, int timed_out)
{
	float fy;
	char ** psplash;
	sprite_info_t *ps;

	// delete all of CREATE PLAY graphics
	killall( CAP_TIMER_PID, -1 );
	delete_multiple_objects( OID_CAP_PDISPLAY, -1 );
	delete_all_sprites();

	// delete old help text
	delete_multiple_strings( HELP_TEXT_SID, -1 );

	// put up background
	// draw background
	beginobj( &mbg,    0, SPRITE_VRES, 500, BKGRND_TID );
	beginobj( &mbg_c1, 0, SPRITE_VRES, 500, BKGRND_TID );
	beginobj( &mbg_c2, 0, SPRITE_VRES, 500, BKGRND_TID );
	beginobj( &mbg_c3, 0, SPRITE_VRES, 500, BKGRND_TID );

	ps = beginobj(&gridbox,
			256,
			94,
			200,
			TM_SELECT_TID);
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	ps->state.constant_color = 0xff6400ff;
	ps->w_scale = 3.75f;
	ps->h_scale = 0.75f;
	generate_sprite_verts(ps);

	ps = beginobj(&gridbox,
			256,
			94 + 97,
			200,
			TM_SELECT_TID);
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	ps->state.constant_color = 0xff6400ff;
	ps->mode = FLIP_TEX_V;
	ps->w_scale = 3.75f;
	ps->h_scale = 0.75f;
	generate_sprite_verts(ps);

	set_string_id( NAME_TEXT_SID );
	set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );

	// print title
	set_text_font( FONTID_BAST18 );
	set_text_position( 256, SPRITE_VRES - 170, 1.1f );
	oprintf( "%dcCREATE PLAY INFO", WHITE );


	if( timed_out )
	{
		set_text_font( FONTID_BAST10 );
		set_text_position( 256, SPRITE_VRES - 197, 1.2f );

		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, "TIMER EXPIRED, BUT ALL EDITING INFO WAS SAVED!" );
//		oprintf( "%dcTIMER EXPIRED, BUT ALL EDITING INFO WAS SAVED!", LT_CYAN );
	}

	// print general info msg
	set_text_font( FONTID_BAST10 );
	set_text_justification_mode( HJUST_LEFT|VJUST_CENTER );

	if (tmp_plyr_record.cmos_rec_num != -1)
		psplash = splash_info_msg;
	else
		psplash = splash_info_msg1;

	fy = SPRITE_VRES - 217;
	while( *psplash )
	{
		set_text_position( 70, fy, 1.2f);
		oprintf( "%dc%s", LT_YELLOW, *psplash );
		fy -= 15;
		psplash++;
	}

	wait_for_any_butn_press_plyr( 240, (tmp_plyr_record.cmos_rec_num != -1 ? 420 : 600), plyr );

	// delete stuff
	del1c( 0, -1 );
	delete_multiple_strings( NAME_TEXT_SID, -1 );
	delete_multiple_textures( BKGRND_TID, -1 );
}


/////////////////////////////////////////////////////////////////////////////
static void cap_timer( int *args )
{
	int			timer_val,min,sec,mod;
	int			old_sec,p_num;
	ostring_t	*timer_string = (ostring_t*)NULL;

	timer_val = (57*150);
	old_sec = -1;
	p_num = args[0];

	while( timer_val )
	{
		timer_val--;
		min = (timer_val+30) / (57 * 60);
		sec = ((timer_val+30) / 57) % 60;
		mod = (timer_val+30) % 57;

		if ((min < 1) &&
			((sec <= 20 && old_sec != sec) ||
			 (sec <= 10 && mod == 35)))
		{
			old_sec = sec;
			snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME4,127,PRIORITY_LVL3);
		}
			
		// delete old timer digits
		if( timer_string )
			delete_string( timer_string );

		// draw new timer digits
		set_text_font(FONTID_BAST18);
		set_text_id(CAP_TIMER_TID);
		set_string_id(TIMER_SID);

		if ((min < 1) && (sec <= 20))
			set_text_color(LT_RED);
		else
			set_text_color(LT_YELLOW);

		if (is_low_res)
			set_text_position(TIMER_X,TIMER_Y,TIMER_Z );
		else
			set_text_position(TIMER_X,TIMER_Y,TIMER_Z );
		timer_string = oprintf("%dj%d:%02d",
			(HJUST_CENTER|VJUST_CENTER),
			min, sec);

//		if (slower_clock)
//			sleep(2);
//		else
			sleep(1);
	}
	force_create = 1;
	snd_scall_bank(cursor_bnk_str,CAP_LCB_SEL_SND,VOLUME2,127,PRIORITY_LVL2);				// play sound
}

/////////////////////////////////////////////////////////////////////////////
static void activate_lcb( int b )
{
	assert((b >= 0) && (b < LCB_COUNT));
	change_img_tmu( lcb_texts[b], lcb_names[b][0], CAP_TID );
}

/////////////////////////////////////////////////////////////////////////////
static void deactivate_lcb( int b )
{
	assert((b >= 0) && (b < LCB_COUNT));
	change_img_tmu( lcb_texts[b], lcb_names[b][1], CAP_TID );
}

/////////////////////////////////////////////////////////////////////////////
static void hilite_lcb( int b )
{
	assert((b >= 0) && (b < LCB_COUNT));
	change_img_tmu( lcb_buttons[b], &button_on, CAP_TID );
}

/////////////////////////////////////////////////////////////////////////////
static void unhilite_lcb( int b )
{
	assert((b >= 0) && (b < LCB_COUNT));
	change_img_tmu( lcb_buttons[b], &button_off, CAP_TID );
}

/////////////////////////////////////////////////////////////////////////////
static void update_help_text( char **text )
{
	// delete old help text
	delete_multiple_strings( HELP_TEXT_SID, 0xFFFFFFFF );

	// write new help text
	set_text_font( FONTID_BAST10 );
	set_string_id( HELP_TEXT_SID );

	if( text[1] )
	{	// two lines
		set_text_position( HELP_TEXT_X, HELP_TEXT_Y1+(is_low_res ? 1.0f : 0.0f), HELP_TEXT_Z );
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, text[0] );
		set_text_position( HELP_TEXT_X, HELP_TEXT_Y2-(is_low_res ? 1.0f : 0.0f), HELP_TEXT_Z );
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, text[1] );
	}
	else
	{	// one line
		set_text_position( HELP_TEXT_X, HELP_TEXT_Y15, HELP_TEXT_Z );
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_YELLOW, text[0] );
	}
}

/////////////////////////////////////////////////////////////////////////////
static int er_menu( cap_point_t *cur_route, int node )
{
	sprite_info_t	*menu_bak, *menu_text, *menu_hilite;
	float			mx,my;
	int				done = FALSE, action = ER_ACTION_NONE, i;
	int				menu_pos;
	int				stick,but;
	int				oflags;
	int				jukes, spins, turbos;
	int				menu_blocked[ER_MENU_LENGTH];
	char			*custom_help_text[4];
	sprite_info_t	*box_obj;
	int				was_low_res = is_low_res;

	// save original flags
	oflags = cur_route[node].flags;

	// initialize menu_blocked
	for( i = 0; i < ER_MENU_LENGTH; i++ )
		menu_blocked[i] = FALSE;

	// update the specials count, NOT counting current node
	cur_route[node].flags = 0;
	count_specials_used( &cur_play );
	cur_route[node].flags = oflags;

	// set starting menu_pos
	menu_pos = 0;
	if( cur_route[node].flags & CPPT_JUKE )
		menu_pos = ER_ACTION_JUKE;
	if( cur_route[node].flags & CPPT_SPIN )
		menu_pos = ER_ACTION_SPIN;
	if( cur_route[node].flags & CPPT_TURBO )
		menu_pos = ER_ACTION_TURBO;
	if( cur_route[node].flags & CPPT_LAST )
		menu_pos = ER_ACTION_DONE;


	get_menu_pos( &mx, &my, cur_route[node].x,
			cur_route[node].y );

	is_low_res = 0;

	menu_bak = beginobj_with_id( (was_low_res ? &d_boxlr : &drop_box),
								mx,
								my,
								ER_MENU_BAK_Z,
								CAP_TID,
								OID_ER_MENU,
								0);
	menu_text = beginobj_with_id( (was_low_res ? &dtextlr : &droptext),
								mx,
								my,
								ER_MENU_TEXT_Z,
								CAP_TID,
								OID_ER_MENU,
								0);
	menu_hilite = (sprite_info_t *)NULL;

	is_low_res = was_low_res;

	if (node == 1)
	{
		// get rid of second info box
		delete_multiple_strings(INFO_SID, 0xFFFFFFFF);
		del1c(OID_CAP_BOX+1, 0xFFFFFFFF);

		if ((my+90.0f < 250.0f) && (!was_low_res))
		{
			// last stage of instructions
			box_obj = beginobj_with_id(&form_box, mx, my+90.0f, ER_HELP_BOX_Z, CAP_TID ,OID_CAP_BOX+1, 0);
			box_obj->w_scale = 1.5f;
			box_obj->h_scale = 1.5f;
			generate_sprite_verts(box_obj);

			set_text_font(FONTID_BAST8T);
			set_string_id(INFO_SID);
			set_text_position(mx,my+90.0f+24.0f-(is_low_res ? 3.0f : 0.0f),ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage3_ht[0]);
			set_text_position(mx,my+90.0f+12.0f-(is_low_res ? 6.0f : 0.0f),ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage3_ht[1]);

			set_text_position(mx,my+90.0f-12.0f,ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage3_ht[2]);
			set_text_position(mx,my+90.0f-26.0f,ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage3_ht[3]);
		}
		else if (!was_low_res)
		{
			// last stage of instructions
			box_obj = beginobj_with_id(&form_box, mx-150.0f, my, ER_HELP_BOX_Z, CAP_TID ,OID_CAP_BOX+1, 0);
			box_obj->w_scale = 1.5f;
			box_obj->h_scale = 1.5f;
			generate_sprite_verts(box_obj);

			set_text_font(FONTID_BAST8T);
			set_string_id(INFO_SID);
			set_text_position(mx-150.0f,my+24.0f-(is_low_res ? 3.0f : 0.0f),ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage3_ht[0]);
			set_text_position(mx-150.0f,my+12.0f-(is_low_res ? 6.0f : 0.0f),ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_CYAN,stage3_ht[1]);

			set_text_position(mx-150.0f,my-12.0f,ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage3_ht[2]);
			set_text_position(mx-150.0f,my-26.0f,ER_HELP_BOX_Z-1);
			oprintf("%dj%dc%s",(HJUST_CENTER|VJUST_CENTER),LT_YELLOW,stage3_ht[3]);
		}

	}

	is_low_res = 0;

	// create digits & gray overlays
	jukes = CAP_JUKE_MAX - specials_used[0];
	spins = CAP_SPIN_MAX - specials_used[1];
	turbos = CAP_TURBO_MAX - specials_used[2];

	jukes = LESSER(3,jukes);	jukes = GREATER(0,jukes);
	spins = LESSER(3,spins);	spins = GREATER(0,spins);
	turbos = LESSER(3,turbos);	turbos = GREATER(0,turbos);

//	is_low_res = 0;

	if (!jukes)
	{
		menu_blocked[1] = TRUE;
		beginobj_with_id( (was_low_res ? &d_jukglr : &drop_jukeg),
						  mx, my,
						  ER_MENU_OVERLAY_Z,
						  CAP_TID, OID_ER_MENU, 0 );
	}

	if (!spins)
	{
		menu_blocked[2] = TRUE;
		beginobj_with_id( (was_low_res ? &d_spnglr : &drop_sping),
						  mx, my,
						  ER_MENU_OVERLAY_Z,
						  CAP_TID, OID_ER_MENU, 0 );
	}

	if (!turbos)
	{
		menu_blocked[3] = TRUE;
		beginobj_with_id( (was_low_res ? &d_trbglr : &drop_turbog),
						  mx, my,
						  ER_MENU_OVERLAY_Z,
						  CAP_TID, OID_ER_MENU, 0 );
	}

	beginobj_with_id( erm_digits[was_low_res][jukes],
					  mx,
					  my - (0 * (was_low_res ? ER_MENU_ITEM_HEIGHT_LR : ER_MENU_ITEM_HEIGHT)),
					  ER_MENU_DIGIT_Z,
					  CAP_TID, OID_ER_MENU, 0 );

	beginobj_with_id( erm_digits[was_low_res][spins],
					  mx,
					  my - (1 * (was_low_res ? ER_MENU_ITEM_HEIGHT_LR : ER_MENU_ITEM_HEIGHT)),
					  ER_MENU_DIGIT_Z,
					  CAP_TID, OID_ER_MENU, 0 );

	beginobj_with_id( erm_digits[was_low_res][turbos],
					  mx,
					  my - (2  * (was_low_res ? ER_MENU_ITEM_HEIGHT_LR : ER_MENU_ITEM_HEIGHT)),
					  ER_MENU_DIGIT_Z,
					  CAP_TID, OID_ER_MENU, 0 );

	is_low_res = was_low_res;

	// wait for stick or button
	while( !done )
	{
		// update hilite
		if( menu_hilite )
			delobj( menu_hilite );
		menu_hilite = (sprite_info_t *)NULL;

		is_low_res = 0;
		menu_hilite = beginobj_with_id( (was_low_res ? &d_onlr : &drop_on),
										mx,
										my - (menu_pos * (was_low_res ? ER_MENU_ITEM_HEIGHT_LR : ER_MENU_ITEM_HEIGHT)),
										ER_MENU_HILITE_Z,
										CAP_TID,
										OID_ER_MENU,
										0);

		is_low_res = was_low_res;

		// make custom help text
		for( i = 0; er_help_texts[menu_pos][i]; i++ )
			custom_help_text[i] = er_help_texts[menu_pos][i];

		// update help text
		update_help_text( er_help_texts[menu_pos] );

		do
		{
			sleep( 1 );
			stick = (get_player_sw_close()>>(cap_pnum<<3)) & (P_UP|P_DOWN);
			but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
		} while( !stick && !but );

		if( but & P_C )
		{
			done = TRUE;
			action = ER_ACTION_ABORT;
			// restore original flags
			cur_route[node].flags = oflags;
		}
		else if( but & (P_A|P_B))
		{
			snd_scall_bank(cursor_bnk_str,CAP_PICK_MENU_SND,VOLUME4,127,PRIORITY_LVL1);
			done = TRUE;
			action = menu_pos;
		}
		else if( stick & P_UP )
		{
			snd_scall_bank(cursor_bnk_str,CAP_NODE_MENU_SND,VOLUME4,127,PRIORITY_LVL1);
			if ((menu_pos -= 1) < 0)
				menu_pos = ER_MENU_LENGTH-1;

			while( menu_blocked[menu_pos] )
				menu_pos -= 1;
		}
		else if( stick & P_DOWN )
		{
			snd_scall_bank(cursor_bnk_str,CAP_NODE_MENU_SND,VOLUME4,127,PRIORITY_LVL1);
			if ((menu_pos += 1) >= ER_MENU_LENGTH)
				menu_pos = 0;

			while( menu_blocked[menu_pos] )
				menu_pos += 1;
		}

		// set current point flags & update route display
		cur_route[node].flags = er_action_flags[menu_pos];
		update_play_display(0);
	}

	// kill off the menu
	del1c( OID_ER_MENU, 0xFFFFffff );

	if ((node == 1) && (!was_low_res))
	{
		// get rid of stage 3 info box
		delete_multiple_strings(INFO_SID, 0xFFFFFFFF);
		del1c(OID_CAP_BOX+1, 0xFFFFFFFF);
	}


	return action;
}

/////////////////////////////////////////////////////////////////////////////
static void copy_play( cap_play_t *dest, cap_play_t *src )
{
	memcpy( (void *)dest, (void *)src, sizeof( cap_play_t ));
}

/////////////////////////////////////////////////////////////////////////////
static void update_play_display( int audible )
{
	int			i;
	char		play_name[20];

	// wipe out any existing text & play data
	delete_multiple_strings( NO_ACTIVE_SID,0xFFFFffff );
	del1c( OID_CAP_PDISPLAY, 0xFFFFffff );
	delete_multiple_objects( OID_CAP_PDISPLAY, 0xFFFFffff );
	line_cnt = 0;

	// kill any blinker processes
	killall( DISPLAY_BLINKER_PID, 0xFFFFffff );

	// write new
	if( active_play == -1 )
	{	// no active play
		set_string_id( NO_ACTIVE_SID );
		set_text_font( FONTID_BAST10 );
		set_text_position( PLAY_NAME_X, PLAY_NAME_Y, NO_ACTIVE_Z );
		set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );
		if (!audible)
			oprintf("%dcNO PLAY SELECTED", WHITE );
		else
			oprintf("%dcCHOOSE AUDIBLE %d", WHITE, audible );
	}
	else
	{	// editing a play
		// print name of play
		sprintf( play_name, "%s %s", cur_play.name1, cur_play.name2 );
		set_text_font( FONTID_BAST10 );
		set_string_id( NO_ACTIVE_SID );
		set_text_position( PLAY_NAME_X, PLAY_NAME_Y, NO_ACTIVE_Z );
		oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), LT_CYAN, play_name );

		// draw linemen
		beginobj_with_id( lq_dim ? &mpog : &mpo,
			GX2W(20.0f)-17.0f, GY2WNS(10.0f)-(is_low_res ? 10.0f : 7.0f), ER_LINEMAN_Z,
			CAP_TID, OID_CAP_PDISPLAY, 0);
		beginobj_with_id( lq_dim ? &mpog : &mpo,
			GX2W(20.0f), GY2WNS(10.0f)-(is_low_res ? 10.0f : 7.0f), ER_LINEMAN_Z,
			CAP_TID, OID_CAP_PDISPLAY, 0);
		beginobj_with_id( lq_dim ? &mpog : &mpo,
			GX2W(20.0f)+17.0f, GY2WNS(10.0f)-(is_low_res ? 10.0f : 7.0f), ER_LINEMAN_Z,
			CAP_TID, OID_CAP_PDISPLAY, 0);

		// draw QB
		beginobj_with_id( lq_dim ? &sym_qbg : &sym_qb,
			GX2W(20.0f),
			GY2WNS(10.0f) - ((cur_play.flags & CPLT_QB_FAR) ? 38.0f : 24.0f),
			lq_dim ? ER_GRAYQB_Z : ER_QB_Z,
			CAP_TID, OID_CAP_PDISPLAY, 0);

		// draw wpns
		for( i = 0; i < 3; i++ )
			_draw_wpn( i );
	}
}

/////////////////////////////////////////////////////////////////////////////
static void _draw_wpn( int n )
{
	int					i;
	sprite_info_t		*psprite;
	cap_line_t			*rline;
	image_info_t		**pimage;

	// make receiver-in-motion arrows
	if( n == cur_play.motion )
	{
		beginobj_with_id( (route_flags[n] & RF_DIM) ? &arrow_motgl : &arrow_motl,
			GX2W(cur_play.wpn[n][0].x),
			GY2WNS(cur_play.wpn[n][0].y),
			ER_ARROW_Z,
			CAP_TID, OID_CAP_PDISPLAY, 0);
		beginobj_with_id( (route_flags[n] & RF_DIM) ? &arrow_motgr : &arrow_motr,
			GX2W(cur_play.wpn[n][0].x),
			GY2WNS(cur_play.wpn[n][0].y),
			ER_ARROW_Z,
			CAP_TID, OID_CAP_PDISPLAY, 0);
	}

	// make receiver icon
	pimage = startpos_icons[n];
	psprite = beginobj_with_id( (route_flags[n] & RF_DIM) ? pimage[2] : pimage[0],
		GX2W(cur_play.wpn[n][0].x),
		GY2WNS(cur_play.wpn[n][0].y),
		(route_flags[n] & RF_DIM) ? ER_GRAYROOT_Z : ER_NODE_Z,
		CAP_TID, OID_CAP_PDISPLAY, 0);

	// blink receiver icon if active
	if( (route_flags[n] & (RF_ACTIVE|RF_PRIMARY)) && (er_point == 0) )
		qcreate( "blinker", DISPLAY_BLINKER_PID, sprite_blinker,
				(int)(psprite),
				(int)(pimage[0]),
				(int)(pimage[1]),
				0x00040008 );

	// make receiver route icon(s)
	for( i = 1; (i < CP_COUNT) && (cur_play.wpn[n][i].x); i++ )
	{
		pimage = route_icons[cur_play.wpn[n][i].flags & CPPT_JST_MASK];
		psprite = beginobj_with_id( (route_flags[n] & RF_DIM) ? pimage[2] : pimage[0],
			GX2W(cur_play.wpn[n][i].x),
			GY2WNS(cur_play.wpn[n][i].y),
			(route_flags[n] & RF_DIM) ? ER_GRAYNODE_Z : ER_NODE_Z,
			CAP_TID, OID_CAP_PDISPLAY, 0);

		// blink receiver route icon(s) if active
		if( (route_flags[n] & RF_ACTIVE) && (er_point == i) )
			qcreate( "blinker", DISPLAY_BLINKER_PID, sprite_blinker,
					(int)(psprite),
					(int)(pimage[0]),
					(int)(pimage[1]),
					0x00040008 );

		// make receiver route line
		rline = cap_lines + line_cnt++;
		rline->x1 = cur_play.wpn[n][i-1].x;
		rline->x2 = cur_play.wpn[n][i].x;
		rline->y1 = cur_play.wpn[n][i-1].y;
		rline->y2 = cur_play.wpn[n][i].y;
		rline->z = (route_flags[n] & RF_DIM) ? ER_GRAYLINE_Z : ER_LINE_Z;

		if( route_flags[n] & RF_ACTIVE )
			rline->color = wpn_color[3];
		else
		if (route_flags[n] & RF_DIM)
			rline->color = wpn_color[4];
		else
			rline->color = wpn_color[n];

		create_object( "line", OID_CAP_PDISPLAY, OF_NONE, DRAWORDER_PLAYER, (void *)rline, draw_line );
	}
}

/////////////////////////////////////////////////////////////////////////////
// returns 'OFFENSE' or 'DEFENSE' (both defined constants)
static int prompt_off_def(void)
{
	ostring_t * pos_off;
	ostring_t * pos_def;
	sprite_info_t * psi;
	float	ypos;
	int		stk, but;
	int		cur_sel = OFFENSE;
	int		done = FALSE;

	// put up the yes/no box
	psi = beginobj_with_id(&ynbox,
		YESNO_X,
		YESNO_Y,
		YESNO_Z,
		CAP_TID,
		OID_CAP_YESNO,
		0
	);
	psi->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	psi = beginobj_with_id(&lback,
		YESNO_X,
		YESNO_Y-87,
		YESNO_Z-0.05f,
		CAP_TID,
		OID_CAP_YESNO,
		0
	);
	psi->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	psi->w_scale = 0.40f;
	psi->h_scale = 0.35f;
	generate_sprite_verts( psi );

	// write the text
	set_string_id( YESNO_TEXT_SID );
	set_text_justification_mode( HJUST_CENTER|VJUST_CENTER );

	set_text_font( FONTID_BAST10 );

	ypos = YESNO_LINE1_Y + 15;
	set_text_position(YESNO_X, ypos, YESNO_Z2);
	oprintf("%dcCHOOSE THE", WHITE);
	ypos -= 15;
	set_text_position(YESNO_X, ypos, YESNO_Z2);
	oprintf("%dcAUDIBLES FOR", WHITE);
	ypos -= 48;
	set_text_position(YESNO_X, ypos, YESNO_Z2);
	oprintf("%dcOR", MED_GRAY);
	ypos -= 48;
	set_text_position(YESNO_X, ypos, YESNO_Z2);
	oprintf("%dcOR TURBO", WHITE);
	ypos -= 15;
	set_text_position(YESNO_X, ypos, YESNO_Z2);
	oprintf("%dcTO CANCEL", WHITE);

	set_text_font( FONTID_BAST18 );

	ypos = YESNO_LINE1_Y - 27;
	set_text_position(YESNO_X, ypos, YESNO_Z2);
	pos_off = oprintf("OFFENSE");
	ypos -= 42;
	set_text_position(YESNO_X, ypos, YESNO_Z2);
	pos_def = oprintf("DEFENSE");

	// input loop
	while( !done )
	{
		// change highlight
		pos_off->state.color = (cur_sel == OFFENSE ? LT_CYAN : MED_GRAY);
		pos_def->state.color = (cur_sel == DEFENSE ? LT_CYAN : MED_GRAY);
		change_string_state( pos_off );
		change_string_state( pos_def );
		killall( DISPLAY_BLINKER_PID, -1 );
		iqcreate( "pflash", DISPLAY_BLINKER_PID, purchase_flasher,
			(int)(cur_sel == OFFENSE ? pos_off : pos_def), 0, 0, 5 );

		// wait for stick or button
		do
		{
			sleep( 1 );
			stk = get_player_sw_close()>>(cap_pnum<<3);
			but = stk & (P_A|P_B|P_C);
			stk = stk & (P_UP|P_DOWN);
		} while( !stk && !but );

		if( but )
		{
			if( but & P_C )
			{
				// escape
				snd_scall_bank(cursor_bnk_str,CAP_DROP_NODE_SND,VOLUME4,127,PRIORITY_LVL1);
				cur_sel = -1;
			}
			else
			{
				// select
				snd_scall_bank(cursor_bnk_str,CAP_YESNO_SEL_SND,VOLUME3,127,PRIORITY_LVL2);
			}
			done = TRUE;
		}
		else
		if( (stk & P_UP   && cur_sel == DEFENSE) ||
			(stk & P_DOWN && cur_sel == OFFENSE) )
		{
			// sound for cursor moving
			snd_scall_bank(cursor_bnk_str,CAP_YESNO_CUR_SND,VOLUME4,127,PRIORITY_LVL2);

			// move
			if( cur_sel == OFFENSE)
				cur_sel = DEFENSE;
			else
				cur_sel = OFFENSE;
		}
	}

	// delete all our crap
	killall( DISPLAY_BLINKER_PID, -1 );
	delete_multiple_strings( YESNO_TEXT_SID, -1 );
	del1c( OID_CAP_YESNO, -1 );

	return cur_sel;
}

/////////////////////////////////////////////////////////////////////////////
// returns TRUE if yes selected, FALSE if no or esc
static int prompt_yn( char **text )
{
	int				cur_sel, done, retval = FALSE;
	int				stick,but,i;
	sprite_info_t	*hilite = (sprite_info_t *)NULL;
	sprite_info_t	*backplate;
	float			ypos;

	// initialize
	cur_sel = 1;				// default to NO!!!
	done = FALSE;

	// put up the yes/no box
	backplate = beginobj_with_id(&ynbox, YESNO_X, YESNO_Y, YESNO_Z, CAP_TID, OID_CAP_YESNO, 0);
	backplate->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	hilite = beginobj_with_id(&no, YESNO_X, YESNO_Y, YESNO_Z2, CAP_TID, OID_CAP_YESNO, 0);

	// write the text
	set_text_font(FONTID_BAST10);
	set_string_id(YESNO_TEXT_SID);

	ypos = YESNO_LINE1_Y;
	// print msg in yes/no box
	for (i=0; text[i]; i++)
	{
		set_text_position(YESNO_X, ypos-(15 * i), YESNO_Z2);
                oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, text[i]);
	}

//	if( text[1] && text[2] )
//	{	// three lines
//		set_text_position( YESNO_X, YESNO_LINE1_Y, YESNO_Z2 );
//              oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, text[0] );
//		set_text_position( YESNO_X, YESNO_LINE2_Y, YESNO_Z2 );
//              oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, text[1] );
//		set_text_position( YESNO_X, YESNO_LINE3_Y, YESNO_Z2 );
//              oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, text[2] );
//	}
//	else if( text[1] )
//	{	// two lines
//		set_text_position( YESNO_X, YESNO_LINE15_Y, YESNO_Z2 );
//              oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, text[0] );
//		set_text_position( YESNO_X, YESNO_LINE25_Y, YESNO_Z2 );
//              oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, text[1] );
//	}
//	else
//	{	// one line
//		set_text_position( YESNO_X, YESNO_LINE2_Y, YESNO_Z2 );
//              oprintf("%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, text[0] );
//	}

	// input loop
	while( !done )
	{
		// change highlight
		change_img_tmu( hilite, cur_sel ? &no : &yes, CAP_TID );

		// wait for stick or button
		do
		{
			sleep( 1 );
			stick = (get_player_sw_close()>>(cap_pnum<<3)) & (P_UP|P_DOWN);
			but = (get_player_sw_close()>>(cap_pnum<<3)) & (P_A|P_B|P_C);
		} while( !stick && !but );

		if( but )
		{
			// select
			snd_scall_bank(cursor_bnk_str,CAP_YESNO_SEL_SND,VOLUME3,127,PRIORITY_LVL2);

			done = TRUE;
			if (cur_sel == 1)
				retval = FALSE;
			else
				retval = TRUE;
		}
		else if( stick & P_UP )
		{
			// sound for cursor moving
			snd_scall_bank(cursor_bnk_str,CAP_YESNO_CUR_SND,VOLUME4,127,PRIORITY_LVL2);

			// move up (yes)
			if( cur_sel )
				cur_sel = 0;
		}
		else
		{
			// sound for cursor moving
			snd_scall_bank(cursor_bnk_str,CAP_YESNO_CUR_SND,VOLUME4,127,PRIORITY_LVL2);

			// move down (no)
			if( !cur_sel )
				cur_sel = 1;
		}
	}

	// delete all our crap
	delete_multiple_strings( YESNO_TEXT_SID, 0xFFFFffff );
	del1c( OID_CAP_YESNO, 0xFFFFffff);

	return retval;
}

/////////////////////////////////////////////////////////////////////////////
// blinks the texture on a sprite
// args[0] = sprite pointer
// args[1] = image 1
// args[2] = image 2
// args[3] = [img 1 time][img 2 time]
static void sprite_blinker( int *args )
{
	sprite_info_t		*psprite;
	image_info_t		*img1, *img2;
	int					time1, time2;

	// extract args
	psprite = (sprite_info_t *)(args[0]);
	img1 = (image_info_t *)(args[1]);
	img2 = (image_info_t *)(args[2]);
	time1 = args[3] & 0xffff;
	time2 = (args[3]>>16) & 0xffff;

	while( TRUE )
	{
		sleep( time1 );
		change_img_tmu( psprite, img2, CAP_TID );
		sleep( time2 );
		change_img_tmu( psprite, img1, CAP_TID );
	}
}

/////////////////////////////////////////////////////////////////////////////
// returns P_RLDU mask of directions in which route node "base" or nodes
// downstream can't move because they're up against a wall.  cpos is
// the index of the base node.  Also sets block mask if base node is up
// against los and is node 0.
// also, set block mask for directions in which we can't move because
// it would cause base or downstream nodes to overlap an upstream node.
// rec is the current receiver number
static int _node_chain_blocked( cap_point_t *base, int cpos, int rec )
{
	int				i,j, blocked = 0;
	int				motion;
	cap_point_t		*root;

	if (cpos == 0)
		blocked |= _root_node_blocked( base[0].x, base[0].y, rec );

	motion = (rec == cur_play.motion);

	root = base - cpos;

	for( i = 0; (i + cpos < CP_COUNT) && (base[i].x); i++ )
	{
		if( base[i].x <= GRID_MIN_X )
			blocked |= P_LEFT;
		if( base[i].x >= GRID_MAX_X )
			blocked |= P_RIGHT;
		if( base[i].y <= GRID_MIN_Y )
			blocked |= P_DOWN;
		if( base[i].y >= GRID_MAX_Y )
			blocked |= P_UP;

		// loop through upstream nodes looking for overlap
		for( j = 0; j < cpos; j++ )
		{
			if (abs(base[i].x - root[j].x) <= 1)
			{	// in vertical band - check for up/down blockage
				if (base[i].y == (root[j].y - 2))
					blocked |= P_UP;
				if (base[i].y == (root[j].y + 2))
					blocked |= P_DOWN;
			}

			if (abs(base[i].y - root[j].y) <= 1)
			{	// in horizontal band - check for left/right blockage
				if (base[i].x == (root[j].x - 2))
					blocked |= P_RIGHT;
				if (base[i].x == (root[j].x + 2))
					blocked |= P_LEFT;
			}
		}
	}

	return blocked;
}

/////////////////////////////////////////////////////////////////////////////
// returns P_RLDU mask of directions in which a root node of
// a receiver can't move because of qb/lineman restrictions.
// motion = is this the motion guy?
static int _root_node_blocked( int x, int y, int rec )
{
	int		blocked = 0;
	int		motion = (rec == cur_play.motion);
	int		i, dx,dy;

	// check against LOS
	if (y >= GRID_LOS_Y)
		blocked |= P_UP;
	if ((y >= GRID_LOS_Y-1) && motion)
		blocked |= P_UP;

	if ((x == 16) && (y <= GRID_LOS_Y) && (y >= (GRID_LOS_Y-1)))
		blocked |= P_RIGHT;
	if ((x == 24) && (y <= GRID_LOS_Y) && (y >= (GRID_LOS_Y-1)))
		blocked |= P_LEFT;
	if ((x == 18) && (y <= (GRID_LOS_Y-2)) && (y >= (GRID_LOS_Y-4)))
		blocked |= P_RIGHT;
	if ((x == 22) && (y <= (GRID_LOS_Y-2)) && (y >= (GRID_LOS_Y-4)))
		blocked |= P_LEFT;
	if ((y == GRID_LOS_Y-2) && (x >= 17) && (x <= 23))
		blocked |= P_UP;
	if ((y == GRID_LOS_Y-5) && (x >= 19) && (x <= 21))
		blocked |= P_UP;

	if (x <= REC_START_MIN_X)
		blocked |= P_LEFT;
	if (x >= REC_START_MAX_X)
		blocked |= P_RIGHT;

	// check against other receivers
	for( i = 0; i < 3; i++ )
	{
		if (i == rec)
			continue;

		dx = cur_play.wpn[i][0].x - x;
		dy = cur_play.wpn[i][0].y - y;

		if ((dy == 2) && (abs(dx) <= 1))
			blocked |= P_UP;
		if ((dy == -2) && (abs(dx) <= 1))
			blocked |= P_DOWN;
		if ((dx == 2) && (abs(dy) <= 1))
			blocked |= P_RIGHT;
		if ((dx == -2) && (abs(dy) <= 1))
			blocked |= P_LEFT;
	}

	return blocked;
}

/////////////////////////////////////////////////////////////////////////////
// move route node "base" nodes downstream by X & Y, capping motion against
// the walls.  cpos is the index of the base node.

static void _move_downstream( cap_point_t *base, int cpos, int x, int y )
{
	int			i;
	int			mirror = FALSE;

	if ((cpos == 0) && (base[0].x == 20) && (mirror_dir == x))
	{
		mirror = TRUE;
		mirror_dir = -mirror_dir;
	}

	if ((mirror_dir == 0) && (base[0].x == 20) && x)
		mirror_dir = -x;

	for( i = 0; (i + cpos < CP_COUNT) && (base[i].x); i++ )
	{
		if(mirror && i)
			base[i].x = 40 - base[i].x;

		base[i].x += x;
		base[i].y += y;

		base[i].x = GREATER(base[i].x, GRID_MIN_X);
		base[i].x = LESSER(base[i].x, GRID_MAX_X);
		base[i].y = GREATER(base[i].y, GRID_MIN_Y);
		base[i].y = LESSER(base[i].y, GRID_MAX_Y);

	}
}

/////////////////////////////////////////////////////////////////////////////
// Find a valid grid coordinate for a new point to be placed after 'node'
// write the x and y to dest
// start with a point 3 above node, then spiral outward, searching for a
// valid point.  up 1, right 1, d2, l2, u3, r3, d4, l4, etc
static void find_new_point( cap_point_t *route, int node, cap_point_t *dest )
{
	int		direction = 0;		// up=0, right=1, down=2, left=3
	int		length = 1;			// number of steps to take in current leg
	int		to_go = 1;			// steps remaining in current leg
	int		x,y;				// current x & y
	int		moves[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};	// up, right, down, left

	x = route[node].x;
	y = LESSER(route[node].y+3,GRID_MAX_Y);

	while( TRUE )
	{
		if( _is_valid_point( route, node+1, x, y ))
		{
			dest->x = x;
			dest->y = y;
			return;
		}

		if (to_go)
		{
			to_go -= 1;
			x += moves[direction][0];
			y += moves[direction][1];
		}
		else
		{
			direction = (direction + 1) % 4;
			if ((direction == 0) || (direction == 2))
				length += 1;

			to_go = length-1;
			x += moves[direction][0];
			y += moves[direction][1];
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// returns TRUE if point x,y doesn't collide with any of the first n nodes
// of route.
static int _is_valid_point( cap_point_t *route, int n, int x, int y )
{
	int		i;

	if ((x > GRID_MAX_X) || (x < GRID_MIN_X) ||
		(y > GRID_MAX_Y) || (y < GRID_MIN_Y))
		return FALSE;

	for( i = 0; i < n; i++ )
	{
		if ((abs(route[i].x - x) < 2) &&
			(abs(route[i].y - y) < 2))
			return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
#define DL_LENGTH		0.625f
#define DL_WIDTH		1.5f
static void draw_line( void *oi )
{
	cap_line_t			*pline;
	MidVertex			va,vb,vc,vd;
	float				abx,aby,slope;
	float				px,py,pslope;
	float				x1,y1,x2,y2;
	float				tmp;

	pline = (cap_line_t *)oi;

	// go home right now if start and end point are the same
	if ((pline->x1 == pline->x2) &&
		(pline->y1 == pline->y2))
		return;

	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBufferFunction( GR_CMP_LESS );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	guColorCombineFunction(GR_COLORCOMBINE_CCRGB);
	grConstantColorValue( pline->color );
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );
	guTexSource( line_tex );

	// convert from grid to screen coordinates
	x1 = GX2W(pline->x1) + 0.5f;
	x2 = GX2W(pline->x2) + 0.5f;
	y1 = GY2W(pline->y1) - 0.5f;
	y2 = GY2W(pline->y2) - 0.5f;

	// get slope & inverse slope
	if (y2 < y1)
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	slope = (y2-y1) / (x2-x1);
	pslope = -1.0f / slope;

	// get ab and perp vectors
	// unit vector for slope m
	// m = y/x, so y = mx
	// x^2 + y^2 = 1
	// x^2 + m^2*x^2 = 1
	// x^2(1+m^2) = 1
	// x = sqrt(1/(1+m^2))
	// y = m * x
	abx = fsqrt(1.0f / (1.0f + slope*slope));
	aby = slope * abx;

	px = fsqrt(1.0f / (1.0f + pslope*pslope));
	py = pslope * px;

	va.x = x1 - (abx*DL_LENGTH) + (px*DL_WIDTH);
	va.y = y1 - (aby*DL_LENGTH) + (py*DL_WIDTH);
	vb.x = x2 + (abx*DL_LENGTH) + (px*DL_WIDTH);
	vb.y = y2 + (aby*DL_LENGTH) + (py*DL_WIDTH);
	vc.x = x2 + (abx*DL_LENGTH) - (px*DL_WIDTH);
	vc.y = y2 + (aby*DL_LENGTH) - (py*DL_WIDTH);
	vd.x = x1 - (abx*DL_LENGTH) - (px*DL_WIDTH);
	vd.y = y1 - (aby*DL_LENGTH) - (py*DL_WIDTH);
	va.oow = 1.0f / pline->z;
	vb.oow = va.oow;
	vc.oow = va.oow;
	vd.oow = va.oow;

	va.sow = 0.0f;
	va.tow = 0.0f;
	vb.sow = 0.0f;
	vb.tow = 255.0f * vb.oow;
	vc.sow = 255.0f * vc.oow;
	vc.tow = 255.0f * vc.oow;
	vd.sow = 255.0f * vc.oow;
	vd.tow = 0.0f;

	grDepthBiasLevel( 0L );
	grDrawTriangleDma( &va, &vb, &vc, 0 );
	grDrawTriangleDma( &vc, &vd, &va, 0 );
}

/////////////////////////////////////////////////////////////////////////////
static void get_menu_pos( float *x, float *y, int sx, int sy )
{
	*x = GX2W( sx );
	*x += (sx > 20) ? -45.0f : 45.0f;

	sy = LESSER( sy, GRID_MAX_Y-5 );
	sy = GREATER( sy, GRID_MIN_Y+5 );

	*y = GY2W( sy );
}

/////////////////////////////////////////////////////////////////////////////
static void count_specials_used( cap_play_t *pplay )
{
	int		i,j,k;

	specials_used[0] = 0;
	specials_used[1] = 0;
	specials_used[2] = 0;

	for( i = 0; i < 3; i++ )
	{
		for( j = 0; (j < CP_COUNT) && (pplay->wpn[i][j].x); j++ )
		{
			if( pplay->wpn[i][j].flags & CPPT_JUKE )
				specials_used[0] += 1;
			if( pplay->wpn[i][j].flags & CPPT_SPIN )
				specials_used[1] += 1;
			if( pplay->wpn[i][j].flags & CPPT_TURBO )
				specials_used[2] += 1;
		}
	}
}

#ifdef CAP_DEBUG
/////////////////////////////////////////////////////////////////////////////
static void dump_play( cap_play_t *pplay, int unk )
{
	int			i,j;

	if( unk == -1 )
	{
		fprintf( stderr, "----------\n%s\n%s\n", pplay->name1, pplay->name2 );
		fprintf( stderr, "flags: (%x) %s%s%s%s\n", pplay->flags,
			(pplay->flags & CPLT_EXISTS) ? "CPLT_EXISTS " : "",
			(pplay->flags & CPLT_QB_FAR) ? "CPLT_QB_FAR " : "",
			(pplay->flags & CPLT_PROTECT) ? "CPLT_PROTECT " : "",
			(pplay->flags) ? "" : "none" );
	}

	for( i = 0; i < 3; i++ )
	{
		if ((unk != -1) && (unk != i))
			continue;

		fprintf( stderr, "wpn %d starts at %d,%d\n",
			i, pplay->wpn[i][0].x,
			pplay->wpn[i][0].y );
		for( j = 1; j < CP_COUNT; j++ )
		{
			fprintf( stderr, " %d.  %2d, %2d", j,
				pplay->wpn[i][j].x,
				pplay->wpn[i][j].y );

			if( pplay->wpn[i][j].flags & CPPT_JUKE )
				fprintf( stderr, " JUKE" );
			if( pplay->wpn[i][j].flags & CPPT_SPIN )
				fprintf( stderr, " SPIN" );
			if( pplay->wpn[i][j].flags & CPPT_TURBO )
				fprintf( stderr, " TURBO" );
			if( pplay->wpn[i][j].flags & CPPT_LAST )
				fprintf( stderr, " LAST" );

			fprintf( stderr, "\n" );
		}
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
void convert_play(cap_play_t *pplay)
{
	int		i;
	float	lineman_z[] = {-17.5f, 0.0f, 17.5f };
	int		player_flags[] = {LU_LEFT,LU_CENTER,LU_RIGHT};
	int		rt2,rt3, rend;

	// Save * to last play converted; do this for "Original Play" in
	//  the audibles menu
	convert_play_last = pplay;

	// set linemen
	for( i = 0; i < 3; i++ )
	{
		custom_form[i].x = -12.0f;
		custom_form[i].z = lineman_z[i];
		custom_form[i].seq = (i==1) ? center_set_anim : stance3pt_anim;
		custom_form[i].mode = MODE_LINEPS;
		custom_form[i].control = 0;
	}

	// qb
	custom_form[3].x = (pplay->flags & CPLT_QB_FAR) ? -38.0f : -18.0f;
	custom_form[3].z = 0.0f;
	custom_form[3].seq = (pplay->flags & CPLT_QB_FAR) ? shotgun_anim : qbset_anim;
	custom_form[3].mode = MODE_QBPS;
	custom_form[3].control = LU_CAPN;

	// receivers
	for( i = 0; i < 3; i++ )
	{
		custom_form[i+4].x = GY2FX(pplay->wpn[i][0].y );
		custom_form[i+4].z = GX2FZ(pplay->wpn[i][0].x );

		if (pplay->wpn[i][0].x == 20)
			custom_form[i+4].z += 2.5f;
			
		custom_form[i+4].seq = wr_set_anim;
		custom_form[i+4].mode = MODE_WPNPS;
		custom_form[i+4].control = player_flags[i];
		if( pplay->motion == i)
			custom_form[i+4].control |= (LU_PLYR2|LU_MOTION);
	}

	// convert routes
	rt2 = convert_route( pplay->wpn[0], custom_routes );
	rt3 = rt2 + convert_route( pplay->wpn[1], custom_routes + rt2 );
	rend = rt3 + convert_route( pplay->wpn[2], custom_routes + rt3 );

	custom_play.formation = custom_form;
	custom_play.flags = PLF_CUSTOM;
	custom_play.routes[0] = ds_none;
	custom_play.routes[1] = ds_none;
	custom_play.routes[2] = ds_none;
	custom_play.routes[3] = pr_QB_block;
	custom_play.routes[4] = custom_routes;
	custom_play.routes[5] = custom_routes + rt2;
	custom_play.routes[6] = custom_routes + rt3;

	if (pplay->flags & CPLT_PRI_RCVR1)
		custom_play.flags |= PLF_PRI_RCVR1;
	else
	if (pplay->flags & CPLT_PRI_RCVR2)
		custom_play.flags |= PLF_PRI_RCVR2;
	else
	if (pplay->flags & CPLT_PRI_RCVR3)
		custom_play.flags |= PLF_PRI_RCVR3;

#ifdef DEBUG
	fprintf(stderr,"  convert_play() setting Primary Receiver %s\r\n",
		((pplay->flags & CPLT_PRI_RCVR1) ? "1" :
			((pplay->flags & CPLT_PRI_RCVR2) ? "2" :
				((pplay->flags & CPLT_PRI_RCVR3) ? "3" : "NONE"))));
#endif

#ifdef CAP_DEBUG
	fprintf( stderr, "route 1\n" );
	dump_play( pplay, 0 );
	dump_route( custom_play.routes[4] );
	fprintf( stderr, "route 2\n" );
	dump_play( pplay, 1 );
	dump_route( custom_play.routes[5] );
	fprintf( stderr, "route 3\n" );
	dump_play( pplay, 2 );
	dump_route( custom_play.routes[6] );
#endif
}

/////////////////////////////////////////////////////////////////////////////
// converts play-editor format *route into drone script format *script
// returns number of words in script
static int convert_route( cap_point_t *route, int *script )
{
	int		i = 0;
	int		nangle, ndist, change, fake_dir;
	int		flip, juke_used = FALSE;
	int		turbo_left = 0, turbo_used = FALSE;
	int		last_face = 1024;

	// set flip if recvr starts left of ball
	flip = (route[0].x < 20) ? TRUE : FALSE;

	// always start with wait_snap
	script[i++] = DS_WAIT_SNAP;

	while(!( route->flags & CPPT_LAST ))
	{
		// get angle to next node
		nangle = angle_from( route, route + 1 );
		if (flip)
			nangle = -nangle;

		// compute change in direction
		change = (nangle - last_face) & 0x3FF;
		if (change > 512)
			change -= 1024;

		// compute fake direction
		if (abs(change) <= 256)
			fake_dir = (last_face - change) & 0x3FF;
		else
			fake_dir = last_face;

		if (fake_dir > 512)
			fake_dir -= 1024;

		// get distance to next node
		ndist = dist_from( route, route + 1 );

		// specials
		if( route->flags & CPPT_TURBO )
		{
			if( !turbo_left )
				script[i++] = DS_TURBO_ON;

			turbo_left += (turbo_used) ? 6 : 10;
			turbo_used = TRUE;
		}
		else if( route->flags & CPPT_JUKE )
		{
			int		jf_prob, jf_force;

			if (abs(change) >= 256)
				jf_prob = juke_used ? 45 : 70;		//35 : 55;
			else if (abs(change) >= 128)				//128)
				jf_prob = juke_used ? 45 : 55;		//30 : 40;
			else
				jf_prob = juke_used ? 20 : 35;		//14 : 20;

			jf_force = juke_used ? 30 : 45;			//10 : 20;

			juke_used = TRUE;

			script[i++] = DS_JUKE;
			script[i++] = DS_FAKE_OUT;
			script[i++] = fake_dir;
			script[i++] = jf_prob;
			script[i++] = jf_force;
		}
		else if( route->flags & CPPT_SPIN )
		{
			int		jf_prob, jf_force;

			jf_prob = 25;
			jf_force = 30;

			script[i++] = DS_SPIN;
			script[i++] = DS_FAKE_OUT;
			script[i++] = fake_dir;
			script[i++] = jf_prob;
			script[i++] = jf_force;
		}
		else if (i > 1)
		{	// no special & not on first node
			if (abs(change) >= 64)
			{
				script[i++] = DS_FAKE_OUT;
				script[i++] = fake_dir;
				script[i++] = (change >= 256) ? 45 : 30;		//35 : 20;
				script[i++] = 0;
			}
		}

		// turn
		// write only if it's changed, but always write first one
		if (change || (i==1))
		{
			script[i++] = DS_FACE;
			script[i++] = nangle;
			last_face = nangle;
		}

		// in turbo mode?
		if (turbo_left)
		{
			// does turbo run out during this leg?
			if (turbo_left <= ndist)
			{
				script[i++] = DS_RUN_YDS;
				script[i++] = turbo_left;
				script[i++] = DS_TURBO_OFF;

				if( ndist - turbo_left )
				{
					script[i++] = DS_RUN_YDS;
					script[i++] = ndist - turbo_left;
				}

				turbo_left = 0;
			}
			else
			{
				turbo_left -= ndist;
				script[i++] = DS_RUN_YDS;
				script[i++] = ndist;
			}
		}
		else
		{
			script[i++] = DS_RUN_YDS;
			script[i++] = ndist;
		}

		route++;
	}

	if( turbo_left )
		script[i++] = DS_TURBO_OFF;

	script[i++] = DS_IMPROVISE;

	return i;
}

/////////////////////////////////////////////////////////////////////////////
static int dist_from( cap_point_t *src, cap_point_t *dest  )
{
	float	dx,dy,mag,idist;

	dx = (float)(dest->x - src->x);
	dy = (float)(dest->y - src->y);

	mag = fsqrt( dx*dx + dy*dy );

	mag = mag / 1.1f;

	idist = (int)(mag + 0.5f);

	return idist;
}

/////////////////////////////////////////////////////////////////////////////
static int angle_from( cap_point_t *src, cap_point_t *dest  )
{
	float	dx,dy,mag,cosine;
	int		angle;

	dx = (float)(dest->x - src->x);
	dy = (float)(dest->y - src->y);

	mag = fsqrt( dx*dx + dy*dy );

	if( mag < 0.01f )
		return 0;

	cosine = dy / mag;

	angle = iacos( cosine );

	if( dx > 0.0f )
		angle *= -1;

	angle = angle & 0x3FF;

	if( angle > 512 )
		angle -= 1024;

	return angle;
}

#ifdef CAP_DEBUG
/////////////////////////////////////////////////////////////////////////////
static void dump_route( int *route )
{
	int			done = FALSE;

	while( !done )
	{
		switch( *route )
		{
			case DS_WAIT_SNAP:
				printf( "DS_WAIT_SNAP,\n" );
				route++;
				break;
			case DS_FACE:
				printf( "DS_FACE,%d,\n", route[1] );
				route+=2;
				break;
			case DS_GOTO_LOS_PLUS:
				printf( "DS_GOTO_LOS_PLUS,%d,\n", route[1] );
				route+=2;
				break;
			case DS_RUN_YDS:
				printf( "DS_RUN_YDS,%d,\n", route[1] );
				route+=2;
				break;
			case DS_IMPROVISE:
				printf( "DS_IMPROVISE\n" );
				done = TRUE;
				break;
			case DS_BLOCK:
				printf( "DS_BLOCK\n" );
				route++;
				break;
			case DS_JUKE:
				printf( "DS_JUKE,\n" );
				route++;
				break;
			case DS_FAKE_OUT:
				printf( "DS_FAKE_OUT,%d,%d,%d,\n",
					route[1], route[2], route[3] );
				route+=4;
				break;
			case DS_TURBO_ON:
				printf( "DS_TURBO_ON\n" );
				route++;
				break;
			case DS_TURBO_OFF:
				printf( "DS_TURBO_OFF\n" );
				route++;
				break;
			case DS_WAVE_ARM:
				printf( "DS_WAVE_ARM\n" );
				route++;
				break;
			case DS_SPIN:
				printf( "DS_SPIN\n" );
				route++;
				break;
			case DS_MOVE_DEFENDER:
				printf( "DS_MOVE_DEFENDER,%d,%d,\n",
					route[1], route[2] );
				route += 3;
				break;
		}
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// check play for validity.  Make minor edits.  Return FALSE if play
// was good or salvageable.  Returns non-zero error code otherwise.
static int validate_play( cap_play_t *play )
{
	char	*cp;
	int		edit = FALSE, i,j,k;

	// check name1 and name2
	check_play_name( play->name1 );
	check_play_name( play->name2 );

	// check motion
	if (play->motion == 3)
		return 1;

	// check flags
	// there's no invalid value for flags

	// check routes
	for( i = 0; i < 3; i++ )
	{
		// check base node start
		if (!legal_start_pos( play->wpn[i][0].x, play->wpn[i][0].y, (int)play->motion == i))
			return 2;

		// make sure base node has no jump, spin, or turbo
		play->wpn[i][0].flags &= CPPT_LAST;

		// check against previous base nodes
		for( j = 0; j < i; j++ )
		{
			if (points_overlap( play->wpn[i], play->wpn[j]))
				return 3;
		}

		// check later nodes
		// last node in chain is first node with CPPT_LAST set,
		// OR last node before a node with an x-coordinate of zero.
		for( j = 1; (j < CP_COUNT) && !(play->wpn[i][j-1].flags & CPPT_LAST) && (play->wpn[i][j].x); j++ )
		{
			if (!legal_control_point( play->wpn[i][j].x, play->wpn[i][j].y))
				return 4;

			for( k = 0; k < j; k++ )
			{
				if (points_overlap( play->wpn[i] + j, play->wpn[i] + k ))
					return 5;
			}
		}

		// make sure last node has CPPT_LAST flag set
		play->wpn[i][j-1].flags |= CPPT_LAST;

		// clear out any nodes after last
		for( ;j < CP_COUNT; j++ )
		{
			play->wpn[i][j].x = 0;
			play->wpn[i][j].y = 0;
			play->wpn[i][j].flags = 0;
		}
	}

	count_specials_used( play );

	if ((specials_used[0] > CAP_JUKE_MAX) ||
		(specials_used[1] > CAP_SPIN_MAX) ||
		(specials_used[2] > CAP_TURBO_MAX))
		return 6;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// returns TRUE if points *pt1 and *pt2 are too close
static int points_overlap( cap_point_t *pt1, cap_point_t *pt2 )
{
	if ((abs(pt1->x - pt2->x) < 2) &&
		(abs(pt1->y - pt2->y) < 2))
		return TRUE;

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// returns TRUE if x,y is a legal control point position.
static int legal_control_point( int x, int y )
{
	if ((x < GRID_MIN_X) ||
		(x > GRID_MAX_X) ||
		(y < GRID_MIN_Y) ||
		(y > GRID_MAX_Y))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// returns TRUE if x,y is a legal base node position.  motion is TRUE if
// this is the motion receiver.
static int legal_start_pos( int x, int y, int motion )
{
	if (y > GRID_LOS_Y)
		return FALSE;
	if ((y > GRID_LOS_Y-1) && motion)
		return FALSE;
	if (y == 0)
		return FALSE;

	if (x < REC_START_MIN_X)
		return FALSE;
	if (x > REC_START_MAX_X)
		return FALSE;

	if ((abs(x-20) <= 1) && (y > 3))
		return FALSE;

	if ((abs(x-20) <= 3) && (y > 6))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// makes sure a play name is valid.  returns TRUE if edits were needed.
static int check_play_name( char *pname )
{
	int		i,j, word_end = FALSE, valid, edit = FALSE;

	// names should be letters, !, space, or *, null-terminated
	for( i = 0; i < CP_NAMELEN; i++ )
	{
		if ((word_end) || (i == CP_NAMELEN-1))
		{
			if (pname[i])
			{
				pname[i] = '\0';
				edit = TRUE;
			}
		}
		else
		{
			valid = FALSE;

			if (pname[i] == '\0')
			{
				word_end = TRUE;
				valid = TRUE;
			}

			if( isalnum( (int)(pname[i])))
				valid = TRUE;

			if ((pname[i] == ' ') || (pname[i] == '!') || (pname[i] == '*') || (pname[i] == '-'))
				valid = TRUE;

			if (!valid)
			{
				pname[i] = '\0';
				word_end = TRUE;
				edit = TRUE;
			}
		}
	}

	return edit;
}


/////////////////////////////////////////////////////////////////////////////
static void draw_enter_name_backplate(void)
{
	sprite_info_t	*psprite;

	psprite = beginobj_with_id(&lback,	NAME_X, BCKPLTE_Y, NAME_Z+1, CAP_TID, OID_CAP_BACK, 0);
	psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	psprite = beginobj_with_id(&lback_c1, NAME_X, BCKPLTE_Y, NAME_Z+1, CAP_TID, OID_CAP_BACK, 0);
	psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	psprite = beginobj_with_id(&lback_c2, NAME_X, BCKPLTE_Y, NAME_Z+1, CAP_TID, OID_CAP_BACK, 0);
	psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;

	psprite = beginobj_with_id(&lback_c3, NAME_X, BCKPLTE_Y, NAME_Z+1, CAP_TID, OID_CAP_BACK, 0);
	psprite->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
}


/////////////////////////////////////////////////////////////////////////////
// returns bank written to, -1 if fail
static int write_plays_hd( int record, cap_play_t * pplays, audible_ids_t * paudibles )
{
	partition_table_t * part_table;
	void *			b1;
	void *			b2;
	cap_block_t *	buffer1;
	cap_block_t *	buffer2;
	int				base_block, num_banks, write_block, i;
	int				bank = 0, bank_retry = FALSE, done = FALSE;

#ifdef DEBUG
	if ( sizeof(cap_block_t) > (HD_RECORD_BLOCKS*512) )
	{
		fprintf( stderr, "<cap_block_t> structure too large!  (%d > %d)\n",
			sizeof(cap_block_t), (HD_RECORD_BLOCKS*512) );
		lockup();
	}
#endif

	// allocate buffers
	b1 = JMALLOC( 32 + HD_RECORD_BLOCKS*512 );
	b2 = JMALLOC( 32 + HD_RECORD_BLOCKS*512 );

	if( !b1 || !b2 )
	{
		fprintf( stderr, "Error allocating buffers, can't save plays.\n");
		if( b1 )
			JFREE( b1 );
		if( b2 )
			JFREE( b2 );
		return -1;
	}

	// make sure our buffers are on 32-byte boundaries
	buffer1 = (cap_block_t *)(((int)b1 + 31) & ~31);
	buffer2 = (cap_block_t *)(((int)b2 + 31) & ~31);

	// get partition info
	part_table = ide_get_partition_table();

#ifdef CAP_DEBUG
	fprintf( stderr, "\r\n"
					 "write_plays_hd() - num_partitions: %d\r\n"
					 "Partition %d info:\r\n"
					 "                  starting_block: %d\r\n"
					 "                      num_blocks: %d\r\n"
					 "                  partition_type: %d\r\n"
					 "                       num_banks: %d\r\n",
		part_table->num_partitions,
		PLAY_PARTITION,
		part_table->partition[PLAY_PARTITION].starting_block,
		part_table->partition[PLAY_PARTITION].num_blocks,
		part_table->partition[PLAY_PARTITION].partition_type,
		part_table->partition[PLAY_PARTITION].num_blocks / (HD_RECORD_BLOCKS * NUM_PLYR_RECORDS) );
#endif

	// get first block of partition 3/4
	if( part_table->partition[PLAY_PARTITION].partition_type != 0 )
	{
#ifdef DEBUG
		fprintf( stderr, "ERROR: Partition %d (play storage) is wrong type.\n", PLAY_PARTITION );
#endif
		JFREE( b1 );
		JFREE( b2 );
		return -1;
	}

	base_block = part_table->partition[PLAY_PARTITION].starting_block;

	num_banks = part_table->partition[PLAY_PARTITION].num_blocks / (HD_RECORD_BLOCKS * NUM_PLYR_RECORDS);

	// write plays/audibles to buffer & generate crc
	memcpy( &buffer1->custom_plays, pplays, 9*sizeof(cap_play_t) );
	memcpy( &buffer1->audible_ids, paudibles, sizeof(audible_ids_t));
	buffer1->crc = crc( (unsigned char *)buffer1->custom_plays, ((9*sizeof(cap_play_t))+sizeof(audible_ids_t)) );

	// write the block
	while( !done )
	{
		// compute write block
		write_block = base_block + (record * HD_RECORD_BLOCKS) + (bank * NUM_PLYR_RECORDS);

		// write & verify loop
		for( i = 0; (i < 2) && (!done); i++ )
		{
			// write, read back & verify
			_SecWrites( write_block, (int *)buffer1, HD_RECORD_BLOCKS );
			_SecReads ( write_block, (int *)buffer2, HD_RECORD_BLOCKS );

			if( !memcmp( (const char *)buffer1, (const char *)buffer2, sizeof(cap_block_t) ) )
				done = TRUE;
			else
				fprintf( stderr, "Verify error (pass %d) saving to record %d of bank %d\n",
					i, record, bank );
		}

		// maybe increment block
		if( !done )
		{
			if( bank < (num_banks-1) )
			{
				bank++;
				fprintf( stderr, "Error writing to record %d in bank %d, moving to bank %d\n",
					record, bank-1, bank );
			}
			else
			// all out of banks
			if( bank_retry )
			{	// and we just checked all the other possibilities, so just forget it
				fprintf( stderr, "All banks re-checked.  Can't save this guy's stuff.\n" );
				JFREE( b1 );
				JFREE( b2 );
				return -1;
			}
			else
			{	// go back to bank 0 and try again.
				fprintf( stderr, "No further banks available, retrying from bank 0\n" );
				bank_retry = TRUE;
				bank = 0;
			}
		}
	}

	JFREE( b1 );
	JFREE( b2 );

#ifdef CAP_DEBUG
	fprintf( stderr, "%s plays written to bank:record %d:%d\n", name_pin, bank, record );
#endif

	return bank;
}

/////////////////////////////////////////////////////////////////////////////
// returns number of plays successfully read, -1 on total failure
int read_plays_hd( int record, int bank, cap_play_t * pplays, audible_ids_t * paudibles )
{
	partition_table_t * part_table;
	void *			b1;
	cap_block_t *	buffer1;
	unsigned int	generated_crc;
	int				base_block, read_block;
	int				i, j;
	int				valid_plays = 0;
	signed short	page, play;
	unsigned char	flags;

#ifdef DEBUG
	if( sizeof(cap_block_t) > (HD_RECORD_BLOCKS*512) )
	{
		fprintf( stderr, "<cap_block_t> structure too large!  (%d > %d)\n",
			sizeof(cap_block_t), (HD_RECORD_BLOCKS*512) );
		lockup();
	}
	fprintf( stderr, "<cap_block_t> structure: %d\n", sizeof(cap_block_t) );
	fprintf( stderr, "Trying to read from bank:record %d:%d\n", bank, record );
#endif

	// allocate buffer
	b1 = JMALLOC( 32 + HD_RECORD_BLOCKS*512 );

	if( !b1 )
	{
		fprintf( stderr, "Error allocating buffers, can't load plays\n" );
		return -1;
	}

	// make sure buffer is on 32-byte boundary
	buffer1 = (cap_block_t *)(((int)b1 + 31) & ~31);

	// get partition info
	part_table = ide_get_partition_table();

#ifdef CAP_DEBUG
	fprintf( stderr, "\r\n"
					 "read_plays_hd() - num_partitions: %d\r\n"
					 "Partition %d info:\r\n"
					 "                  starting_block: %d\r\n"
					 "                      num_blocks: %d\r\n"
					 "                  partition_type: %d\r\n"
					 "                       num_banks: %d\r\n",
		part_table->num_partitions,
		PLAY_PARTITION,
		part_table->partition[PLAY_PARTITION].starting_block,
		part_table->partition[PLAY_PARTITION].num_blocks,
		part_table->partition[PLAY_PARTITION].partition_type,
		part_table->partition[PLAY_PARTITION].num_blocks / (HD_RECORD_BLOCKS * NUM_PLYR_RECORDS) );
#endif

	// get first block of partition 3/4
	if( part_table->partition[PLAY_PARTITION].partition_type != 0 )
	{
#ifdef DEBUG
		fprintf( stderr, "ERROR: Partition %d (play storage) is wrong type.\n", PLAY_PARTITION );
#endif
		JFREE( b1 );
		return -1;
	}

	base_block = part_table->partition[PLAY_PARTITION].starting_block;
	read_block = base_block + (record * HD_RECORD_BLOCKS) + (bank * NUM_PLYR_RECORDS);

	// read file & verify crc
	_SecReads( read_block, (int *)buffer1, HD_RECORD_BLOCKS );

	generated_crc = crc( (unsigned char *)buffer1->custom_plays, ((9*sizeof(cap_play_t))+sizeof(audible_ids_t)) );

	if (buffer1->crc != generated_crc)
	{
		fprintf( stderr, "Error: bad CRC on play file.  %x %x\n", (int)buffer1->crc, generated_crc );
		memset( (void *)pplays, 0, 9*sizeof(cap_play_t));
		memset( (void *)paudibles, 0, sizeof(audible_ids_t));
		JFREE( b1 );
		return -1;
	}

	// clear out any bogus plays
	for( i = 0; i < 9; i++ )
	{
		if( ( buffer1->custom_plays[i].flags != CPLT_UNUSED ) &&
			( validate_play( &buffer1->custom_plays[i] ) ) )
			memset( (void *)(&buffer1->custom_plays[i]), 0, sizeof(cap_play_t) );
		else
			valid_plays++;
	}

	// clear out any bogus offense audibles
	for( i = 0, j = 0; i < PLAYSTUF__AUDIBLES_PER_PLAYER; i++ )
	{
		page = buffer1->audible_ids.ids[0][i];
		if( ( page > ( 9 * PLAYSTUF__NUM_OFFENSE_PAGES) ) ||
			( page < (-9 * PLAYSTUF__NUM_OFFENSE_PAGES) ) )
			break;
		if( page == 0 )
			j |= 1;
		else
		{
			j |= 2;
			if( page < 0 ) page = -page;
			play = (page - 1) % 9;
			page = (page - 1) / 9;
			if( page == PLAYSTUF__OFF_PAGE_ID_CUSTOM )
			{
				flags = buffer1->custom_plays[play].flags;
				if( ( flags & CPLT_EXISTS ) == 0 ||
					( flags & CPLT_UNUSED ) != 0 ||
					( flags & ~CPLT_VALID ) != 0 ||
						(( flags & CPLT_PRI_RCVRS ) != 0 &&
						 ( flags & CPLT_PRI_RCVRS ) != CPLT_PRI_RCVR1 &&
						 ( flags & CPLT_PRI_RCVRS ) != CPLT_PRI_RCVR2 &&
						 ( flags & CPLT_PRI_RCVRS ) != CPLT_PRI_RCVR3
						)
					)
				{
					break;
				}
			}
		}
	}
	// clear offense audibles if one was out of range or there was a zero/non-zero mix
	if(( i < PLAYSTUF__AUDIBLES_PER_PLAYER ) || ( j == 3 ))
		for( i = 0; i < PLAYSTUF__AUDIBLES_PER_PLAYER; buffer1->audible_ids.ids[0][i++] = 0 );

	// clear out any bogus defense audibles
	for( i = 0, j = 0; i < PLAYSTUF__AUDIBLES_PER_PLAYER; i++ )
	{
		page = buffer1->audible_ids.ids[1][i];
		if( ( page > ( 9 * PLAYSTUF__NUM_DEFENSE_PAGES) ) ||
			( page < (-9 * PLAYSTUF__NUM_DEFENSE_PAGES) ) )
			break;
		if( page == 0 )
			j |= 1;
		else
			j |= 2;
	}
	// clear defense audibles if one was out of range or there was a zero/non-zero mix
	if(( i < PLAYSTUF__AUDIBLES_PER_PLAYER ) || ( j == 3 ))
		for( i = 0; i < PLAYSTUF__AUDIBLES_PER_PLAYER; buffer1->audible_ids.ids[1][i++] = 0 );

	memcpy( pplays, &buffer1->custom_plays, 9*sizeof(cap_play_t) );
	memcpy( paudibles, &buffer1->audible_ids, sizeof(audible_ids_t) );

	JFREE( b1 );

#ifdef CAP_DEBUG
	fprintf( stderr, "%s plays read from bank:record %d:%d\n", name_pin, bank, record );
#endif

	return valid_plays;
}

/////////////////////////////////////////////////////////////////////////////
static int update_stored_plays( void )
{
	int		bank;

	if (tmp_plyr_record.cmos_rec_num != -1)
	{
		bank = write_plays_hd( tmp_plyr_record.cmos_rec_num, current_plays, current_ids );

		// if his bank address has changed, update the CMOS record
		// also update if usable_slots doesn't match his cmos value
		if ((tmp_plyr_record.disk_bank != bank) ||
			(usable_slots != tmp_plyr_record.plays_allowed))
		{
			tmp_plyr_record.disk_bank = bank;
			tmp_plyr_record.plays_allowed = usable_slots;
			set_generic_record(PLYR_RECS_ARRAY,tmp_plyr_record.cmos_rec_num,&tmp_plyr_record);

#ifdef CAP_DEBUG
			fprintf( stderr, "%s plays MOVED to bank %d\n", name_pin, bank );
#endif
		}
		return bank;
	}
	return -1;
}


//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
void flash_lcb(int cur_pos, int tid)
{
	sprite_info_t 	*burst_obj;
	int		i;

	burst_obj = beginobj(stat_flsh_imgs[0],
						lcb_xys[cur_pos][0],
						lcb_xys[cur_pos][1],
						LCB_TEXT_Z+2.0f,
						tid);

	burst_obj->w_scale = 2.0f;
	burst_obj->h_scale = 2.4f;
//	burst_obj->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	generate_sprite_verts(burst_obj);

	sleep(1);

	for (i=0; i < 10; i++)
	{
		change_img_tmu(burst_obj,stat_flsh_imgs[i],tid);
		sleep(2);
	}
	delobj(burst_obj);
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
static void wait_for_any_butn_press_plyr(int min, int max, int plr)
{
	int ticks = 0, done = FALSE;

	do
	{
		sleep(1);
		ticks++;

		if (((get_player_sw_current()>>(8*plr)) & (P1_A|P1_B|P1_C)) && (ticks >= min))
			done = TRUE;

	} while (ticks < max && !done);

	snd_scall_bank(cursor_bnk_str,CAP_LET_CUR_SND,VOLUME3,127,PRIORITY_LVL2);
}


/////////////////////////////////////////////////////////////////////////////
