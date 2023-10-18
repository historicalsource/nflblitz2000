#if defined(VEGAS)

#define USE_DISK_CMOS
void update_other_cmos_to_disk(int sleep_allowed);

#endif

/****************************************************************************/
/*                                                                          */
/* nfl.c - Game control process & associated functions                      */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <math.h>
#include <string.h>
#include <ioctl.h>

#include "/Video/Nfl/Include/plyrseq.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Anim/genseq.h"
#include "/Video/Nfl/Include/playbook.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/gameadj.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/pproto.h"
#include "/Video/Nfl/Include/handicap.h"
#include "/Video/Nfl/Include/cap.h"
#include "/Video/Nfl/Include/movie.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/Result.h"
#include "/Video/Nfl/Include/PlaySel.h"
#include "/Video/Nfl/Include/PInfo.h"
#include "/Video/Nfl/Include/Record.h"
#include "/Video/Nfl/Include/PlayStuf.h"
#include "/Video/Nfl/Include/Drone.h"
#include "/Video/Nfl/Include/Sounds.h"
#include "/Video/Nfl/Include/ShowPnam.h"
#include "/Video/Nfl/Include/Cambot.h"
#include "/Video/Nfl/Include/Attract.h"
#include "/Video/Nfl/Include/TeamData.h"
#include "/Video/Nfl/Include/Field.h"
#include "/Video/Nfl/Include/Detect.h"
#include "/Video/Nfl/Include/FreeGame.h"
#include "/Video/Nfl/Include/Coin.h"
#include "/Video/Nfl/Include/Nfl.h"
#include "/video/nfl/include/game_cfg.h"

#ifdef JASON
//#define FG_DEBUG
//#define STATS_DEBUG
//#define ONSIDE_KICK_DEBUG
#endif

#ifdef NODRONES
#undef NOKICKOFF
#define NOKICKOFF
#endif

//#define BOG_METER


void do_game_over_screens(void);
void show_final_player_stats(void);
void start_enable(int);
void clear_random_control_flags(void);

//
// referenced data
//

extern char *play_type_names[];
extern char *down_names[];
extern char *play_end_cause_names[];

//extern image_info_t bast7_up,bast7_dn;
extern image_info_t playresult;

extern char msg_tdn_tex_name[];
extern char msg_4dn_tex_name[];
extern char msg_1dn_tex_name[];
extern char msg_trn_tex_name[];
extern char msg_int_tex_name[];
extern char ingame00_tex_name[];
extern char ingame01_tex_name[];
extern char flash_tex_name[];
extern char arrows_tex_name[];

//
// Global data
//
int flg_hhteam;									// Set per case at start of each play
int flg_pullback;								// Set per case at start of each play

int		fpend;
int		forward_catcher;

//GrMipMapId_t	playsel_decals[2];
//int				playsel_ratios[2];

char * game_tunes_tbl[]  =
{
	"main1","main3","main2","main4","otime"
};

#ifdef DBG_KO
#define DBG_KO_BOOL	!dbg_ko
int dbg_ko = FALSE;
#else
#define DBG_KO_BOOL	1
#endif

static int	free_game_count[2] = {0,0};		// Team-based
int	old_game_purchased = 0;

//process_node_t player_procs[NUM_PLAYERS];

// table of decals for SPECIAL EFFECT images
struct texture_node	*smkpuf_decals[NUM_SMOKE_TXTRS];
struct texture_node	*dustcld_decals[NUM_DUST_TXTRS];
//struct texture_node	*blood_decals[NUM_BLOOD_TXTRS];
//struct texture_node	*spark_decals[NUM_SPARK_TXTRS];
struct texture_node	*bsmkpuf_decals[NUM_BSMOKE_TXTRS];

//
// function prototypes
//
static void init_bog_monitor(void);
static cpu_do_onside( void );
static void game_clock_proc(int *args);
static void start_new_quarter(void);
static void game_over(void);
static void eoq_handling(void);
static void load_player_textures(void);
static int match_receiver( int pos );
static int strong_left( void );
static void verify_loaded_snd_banks(void);
static void reinit_offense_players(void);
static void reinit_defense_players(void);
static void lineup_players( int side );
static void start_off_proc(int *);
static void paid_credits_deduct(void);
static int InitGameProc(void);
static void snd_board_error_recovery(void);
//static void InitAudibles(void);

#if 0
static void reset_game_proc( int * );
#endif

//	FIX ME: AUDIBLES
//	Move the contents of this function to wherever save/load of the player record file is
//	Then remove the call to 'InitAudibles' from 'InitGameProc()' in NFL.C
//	And remove the prototype of 'InitAudibles' from NFL.C
//void InitAudibles(void)
//{
//	int i;
//
//	for(i = 0; i < MAX_PLYRS; i++)
//	{
//	//	4 players... each has 3 offensive audibles and 3 defensive audibles
//
//		//	OFFENSIVE AUDIBLES
//		PlayStuf__SetAudiblePlay(i, OFFENSE, 0, NULL);				//	placeholder for the play chosen in-game from the play select screen
//		PlayStuf__SetAudiblePlay(i, OFFENSE, 1, &play_zigzag);		//	AUDIBLE 1 (these should be loaded from player record)
//		PlayStuf__SetAudiblePlay(i, OFFENSE, 2, &play_spider);		//	AUDIBLE 2 (these should be loaded from player record)
//		PlayStuf__SetAudiblePlay(i, OFFENSE, 3, &play_monkey);		//	AUDIBLE 3 (these should be loaded from player record)
//
//		//	DEFENSIVE AUDIBLES
//		PlayStuf__SetAudiblePlay(i, DEFENSE, 0, NULL);				//	placeholder for the play chosen in-game from the play select screen
//		PlayStuf__SetAudiblePlay(i, DEFENSE, 1, &play_near_zone);	//	AUDIBLE 1 (these should be loaded from player record)
//		PlayStuf__SetAudiblePlay(i, DEFENSE, 2, &play_goal_line);	//	AUDIBLE 2 (these should be loaded from player record)
//		PlayStuf__SetAudiblePlay(i, DEFENSE, 3, &play_deep_zone);	//	AUDIBLE 3 (these should be loaded from player record)
//	}
//}

static void start_off_proc(int *args)
{
	while((game_info.game_time & 0x00ffffff) > 0x00011d18)	// 01:29:23
	{
		sleep(1);
	}
	start_enable(FALSE);
}

#ifdef DEBUG
#if 0
/****************************************************************************/
static void reset_game_proc( int *args)
{
	int cnt = 10;

	do {
		if (get_player_sw_current() != P3_LEFT) {
			cnt = 10;
			sleep (5);
		}
		else {
			cnt--;
			sleep (1);
		}
	} while (cnt);

	wipeout();

	// a fix for the debug restart button (seems to work)
#ifndef NOAUDIO
	snd_stop_all();
	
	coin_sound_holdoff(TRUE);

//	snd_bank_delete(qtr_tunes_tbl[game_info.game_quarter]);
	snd_bank_delete(taunt_bnk_str);
	snd_bank_delete(ancr_swaped_bnk_str);
	snd_bank_delete(team2_bnk_str);
	snd_bank_delete(team1_bnk_str);
	snd_bank_delete(game_tune_bnk_str);
	snd_bank_delete(cursor_bnk_str);

	strcpy(cursor_bnk_str, "cursors");
	if (snd_bank_load(cursor_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
	if (snd_bank_load(intro_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved (SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	coin_sound_holdoff(FALSE);
#endif

	// reset state ram....and stuff
	game_info.game_state = GS_GAME_OVER;
	game_info.game_quarter = 0;

	old_game_purchased = game_purchased;
	game_purchased = 0;

	halt = 0;
	p_status = 0;
	p_status2 = 0;
	qtr_pstatus[0] = 0;
	qtr_pstatus[1] = 0;
	qtr_pstatus[2] = 0;
	qtr_pstatus[3] = 0;

	iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
}
#endif
#endif //DEBUG

/****************************************************************************/
#ifdef DEBUG
void game_proc_debug( int *parg )
{

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
	{
		movie_abort();
	}

	/* cleanup */
	wipeout();

	launch_game_proc();
}
#endif

static int InitGameProc(void)
{
	struct texture_node	* tn;
	char * pstruct;
//	int	failed;
	int	i;

//	show_player_free_game(3);

	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
		movie_abort();

	// free up any heap that's sitting on the free process list
	clear_free_process_list();
	

	// Tell string handler to keep all letters at the same z
	set_text_z_inc(0);

	// audit full games
	for( i = 0; i < (four_plr_ver ? 4 : 2); i++)
		if (game_purchased & (1<<i))
			do_percent_aud(FULL_GAME_PURCHASE_AUD, GAME_START_AUD, FULL_GAME_PURCHASE_PCT_AUD);

	// audit initials entered
	for( i = 0; i < (four_plr_ver ? 4 : 2); i++)
		if ((p_status & (1<<i)) &&
			(plyr_data_ptrs[i]->plyr_name[0] != '-') &&
			(plyr_data_ptrs[i]->plyr_name[0] != 0))
			do_percent_aud(INITIALS_ENTERED_AUD, GAME_START_AUD, INITIALS_ENTERED_PERCENT_AUD);

//$	// Were any full games purchased ?
//$	if(game_purchased & 3)
//$	{
//$		if(game_purchased & 1)
//$		{
//$			// Audit full game purchases
//$			do_percent_aud(FULL_GAME_PURCHASE_AUD, GAME_START_AUD, FULL_GAME_PURCHASE_PCT_AUD);
//$		}
//$
//$		if(game_purchased & 2)
//$		{
//$			// Audit full game purchases
//$			do_percent_aud(FULL_GAME_PURCHASE_AUD, GAME_START_AUD, FULL_GAME_PURCHASE_PCT_AUD);
//$		}
//$	}
//$	else
//$	{
//$		recalc_percent_aud(FULL_GAME_PURCHASE_AUD, GAME_START_AUD, FULL_GAME_PURCHASE_PCT_AUD);
//$	}

////////////////////////////////////////
// Initialize game_info
//
	if (HUMAN_TEAMS == 1)
	{
		if (p_status & TEAM1_MASK)
			game_info.first_pos = 1;
		else
			game_info.first_pos = 0;
	}
	else
	{
		game_info.first_pos = randrng(2);
	}

// DEBUG USED FOR TESTING OF PLAYS
//#if DEBUG
//	game_info.first_pos = 1;	// Force team 2 to start
//#endif

	game_info.off_side = game_info.first_pos;

	qtr_pstatus[0] = 0;
	qtr_pstatus[1] = 0;
	qtr_pstatus[2] = 0;
	qtr_pstatus[3] = 0;
	do_grand_champ = 0;

	record_me = 0;

	game_info.down         = 1;
	if( game_info.game_state != GS_ATTRACT_MODE )
	{
		//Not in attract mode; set game state & clear last game stats
		game_info.game_state = GS_GAME_MODE;
		game_info_last.game_quarter = 0;
	}
	game_info.game_flags   = 0;
	game_info.off_turnover = 0;
	game_info.game_mode    = GM_INITIALIZING;
	game_info.play_result  = PR_TOUCHDOWN;		// for the benefit of <do_amode_proc>
	if (p_status & 15)
		game_info.captain = 0;					// player 1 and 3 always start as QB...
	else
		game_info.captain = randrng(2);

#ifdef NOKICKOFF
	game_info.play_type    = PT_NORMAL;
	game_info.los          = 95;



// DEBUG USED FOR TESTING OF PLAYS
//#if DEBUG
//	game_info.los          = 50;
//	game_info.team[0] = 0;
//	game_info.team[1] = 0;
//#endif



	game_info.first_down   = 100;
#else
	game_info.play_type    = PT_KICKOFF;
#endif

	// set to some default value so that players can buyin on vs. page
	game_info.game_time = QUARTER_TIME;

	// Make sure clock is stopped for opening kickoff
	clock_active = 0;

	// FIX!!  Don't set this if all human players have experience
	if( p_status )
	{
		show_plysel_idiot_box = 2;
		idiot_boxes_shown = 0;
	}
	else
	{
		show_plysel_idiot_box = 0;
		idiot_boxes_shown = 2;
	}

	// clear scores
	for (i=0; i < MAX_QUARTERS; i++)
	{
		game_info.scores[0][i] = 0;
		game_info.scores[1][i] = 0;
	}

	// zero out the team bookeeping structures
	pstruct = (char *)&tdata_blocks[0];
	for (i=0; i < (int)sizeof(tdata_t); i++)
		*pstruct++ = 0;
	pstruct = (char *)&tdata_blocks[1];
	for (i=0; i < (int)sizeof(tdata_t); i++)
		*pstruct++ = 0;

	// zero out "ON FIRE" counts
	reset_onfire_values();

#if 0
	// Process that reads debug butn #3 and will send the game back into attract mode
	qcreate ("gmereset", 0, reset_game_proc,0, 0, 0, 0);
#endif

////////////////////////////////////////
// Enable the disk interrupt callback
//
	game_info.attract_coin_in = 0;
#if defined(SEATTLE)
	install_disk_info((void **)&cur_proc, suspend_self, resume_process);
#else
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

////////////////////////////////////////
// Load player animation sequences
//
//	failed = load_anim_block( ASB_GENSEQ, "GENSEQ.ANI", GENSEQ_VERSION );
	chk_game_proc_abort();

//	if (failed)
//#ifdef DEBUG
//		lockup();		// lockup if in debug
//#else
//		while (1);		// infinite loop if not in debug -- FIX!!!???
//#endif

////////////////////////////////////////
// Load game/player/play select texture maps
//
// NOTE: The order in which this stuff is loaded is crucial to keeping fragmentation
// at a minimum.  Don't change the order of any of these loads unless you're sure you
// know precisely what kind of impact it's having on the fragmentation.
//

	load_play_select_textures();

	load_player_textures();

	// Load the SPECIAL EFFECTS textures
	for (i=0; i < NUM_SMOKE_TXTRS; i++)
	{
		sprintf( spfx_texture, "TRAIL%d.WMS", i+1);

		smkpuf_decals[i] = create_texture(spfx_texture, SPFX_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
		chk_game_proc_abort();

		if (!smkpuf_decals[i])
		{
#ifdef DEBUG
			fprintf( stderr,"Error loading special effect texture smoke%d, \r\n",i+1);
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}
	}

	// Load DUST textures
	for (i=0; i < NUM_DUST_TXTRS; i++)
	{
		sprintf( spfx_texture, "DIRT%d.WMS", i+1);

		dustcld_decals[i] = create_texture(spfx_texture, SPFX_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
		chk_game_proc_abort();

		if (!dustcld_decals[i])
		{
#ifdef DEBUG
			fprintf( stderr,"Error loading special effect texture dirt%d, \r\n",i+1);
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}
	}

#if defined(VEGAS)
	// Load transient (discarded & reloaded as needed) in-game textures
	load_ingame_textures();
#endif

	// Load field/stadium textures
	field_setup();

////////////////////////////////////////
// Disable the disk interrupt callback
//
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

////////////////////////////////////////
// Kill the versus screen
//
#ifndef NO_GAME_PLATES
	// Turn on transition here to kill vs screen; attract-mode does it there
	if (game_info.game_state != GS_ATTRACT_MODE)
	{
		while (existp(PRE_GAME_SCRNS_PID+1,0xFFFFFFFF)) sleep(1);
		killall(PRE_GAME_SCRNS_PID, -1);

		// Turn on the plates
		iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		// Wait for plates; set time-out so we don't hang
		i = PLATE_TRANSITION_TICKS * 2;
		// Do it this way since we can't know how many ticks have lapsed since transition began
		while (plates_on != PLATES_ON && --i) sleep(1);
		// Do sound only if the transition finished, not the time-out
		if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME3, 127, PRIORITY_LVL5);

		coin_sound_holdoff(TRUE);
		// load speech bank
		if (snd_bank_load_playing(get_ancr_str(ancr_swaped_bnk_str)) == 0xeeee)
			increment_audit(SND_LOAD_FAIL_AUD);

		// load player taunts bank
		if (snd_bank_load_playing(get_taunt_str(taunt_bnk_str)) == 0xeeee)
			increment_audit(SND_LOAD_FAIL_AUD);

		// load current (1st) quarter tune
//		if (snd_bank_load_playing(qtr_tunes_tbl[game_info.game_quarter]) == 0xeeee)
//			increment_audit(SND_LOAD_FAIL_AUD);

		// track 0 is the tune, track 1 is the audience, track 5 is the announcer
		snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

		coin_sound_holdoff(FALSE);
	}
#endif


	// Clean out the versus page objects/textures
//	killall(HELMET1_PID, 0xFFFFFFFF);
//	killall(HELMET2_PID, 0xFFFFFFFF);
//	delete_multiple_objects( OID_P1HELMET, 0xFFFFFFFF );
//	delete_multiple_objects( OID_P2HELMET, 0xFFFFFFFF );
//	delete_multiple_textures(HELMET_TID,0xFFFFFFFF);

	unlock_multiple_textures(BARS_PG_TID,0xFFFFFFFF);

	del1c(OID_VERSUS,0xffffff00);
	delete_multiple_strings( 89, -1);		// stadium/field text
	delete_multiple_strings( SID_TLCLOCK, -1);
	delete_multiple_strings( SID_TLSCORES, -1);
	delete_multiple_strings( SID_TLNAMES, -1);
	if (four_plr_ver)
	{
		delete_multiple_strings( 7, -1);
		delete_multiple_strings( 8, -1);
	}
	delete_multiple_textures(BARS_PG_TID,0xFFFFFFFF);

#ifndef NOAUDIO
	// Kill VS music & start game tune
	snd_stop_track(SND_TRACK_0);
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
//	snd_scall_bank(qtr_tunes_tbl[game_info.game_quarter], 0, VOLUME0, 127, SND_PRI_SET|SND_PRI_TRACK0);
	snd_scall_bank(game_tune_bnk_str, 0, MUSIC_VOL, 127, SND_PRI_SET|SND_PRI_TRACK0);
	snd_scall_bank(generic_bnk_str, 1, VOLUME1, 127, SND_PRI_SET|SND_PRI_TRACK1);
#endif

////////////////////////////////////////
// Chk for Tournament Mode; clean out the undesirables and disable
//  computer assistance if set (on-screen text happens elsewhere)
//
	if ((((!get_adjustment(TOURNAMENT_MODE_ADJ, &i)) && i) || pup_tourney) &&
		(HUMAN_TEAMS == 2))
	{
		clear_tourney_pups();
		pup_tourney = 1;
		pup_noassist = 1;
	}
	else
		pup_tourney = 0;

	if (HUMAN_TEAMS == 1)
	{
		// Assume 2 plyr to avoid compile warning; probably smaller code too
		register plyr_record_t * prec1 = p_status & 1 ? &player1_data:&player2_data;
		register plyr_record_t * prec2 = prec1;
		register int td1 = 0;
		register int td2 = 0;
		register int ix  = 1 << (NUM_TEAMS-1);

		if (four_plr_ver)
		{
			prec1 = (p_status & 3) ? &player1_data:&player3_data;
			prec2 = (p_status & 3) ? &player2_data:&player4_data;
		}
		while (ix)
		{
			if ((prec1->plyr_name[0] != NULL && prec1->plyr_name[0] != '-') &&
					prec1->teams_defeated & ix)
				td1++;
			if ((prec2->plyr_name[0] != NULL && prec2->plyr_name[0] != '-') &&
					prec2->teams_defeated & ix)
				td2++;
			ix >>= 1;
		}
		if (td1 >= 20 || td2 >= 20)
		{
			clear_tourney_pups();
			pup_tourney = 1;
			pup_noassist = 1;
		}
	}

////////////////////////////////////////
// Load remaining textures & launch everybody else
//

#if defined(SEATTLE)
	// Load transient (discarded & reloaded as needed) in-game textures
	load_ingame_textures();
#endif

	// Load one of the message textures here, as it will take up the space left behind by the VS icons
	// 256 x 128
	if (!(tn = create_texture(msg_4dn_tex_name, MESSAGE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",msg_4dn_tex_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	fprintf( stderr,"Loaded %s\r\n",msg_4dn_tex_name);
#endif
	lock_texture(tn);

	// wipe the player data ram
	memset( (void *)player_blocks, 0, 14 * sizeof(fbplyr_data));

	// Go create the field draw procs
	do_field_procs();
	if (user_config())
	{
		do_user_fog();
	} else {
		do_vs_fog();
	}

#ifndef CATCH_DEBUG
	// create the instant replay recorder proc
//N_RPLAY	set_process_run_level( qcreate( "replay", REPLAY_PID, replay_proc, 0, 0, 0, 0), RL_REPLAY );
#endif

	// create the ball
	set_process_run_level( qcreate( "ball", BALL_PID, ball_proc, 0, 0, 0, 0 ), RL_BALL );

	// create the player-to-player collision detect proc
	set_process_run_level( qcreate( "detp2p", COLLIS_PID, player_detect_proc, 0, 0, 0, 0), RL_COLLIS );

	// create the camera
	set_process_run_level( iqcreate( "cambot", CAMBOT_PID, cambot_proc, 0, 0, 0, 0 ), RL_CAMBOT );

	// create the players -- use iqcreate because we want the game process
	// to happen before the player processes every tick.
	//								plyrnum, team, position, unused
	set_process_run_level( qcreate( "plyrA", PRE_PLAYER_PID,  pre_player_proc,  0, 0, 0, 0 ), RL_PLAYER-1);
	set_process_run_level( qcreate( "plyrZ", POST_PLAYER_PID, last_player_proc, 0, 0, 0, 0 ), RL_PLAYER+14);
	set_process_run_level( qcreate( "plyr1a", PLAYER_PID, player_proc,  0, 0, PPOS_OLINE1, game_info.team[0] * NUM_PERTEAM_SPD +  0 + (game_info.off_side * 7)), RL_PLAYER);
	set_process_run_level( qcreate( "plyr1b", PLAYER_PID, player_proc,  1, 0, PPOS_OLINE2, game_info.team[0] * NUM_PERTEAM_SPD +  1 + (game_info.off_side * 7)), RL_PLAYER+1);
	set_process_run_level( qcreate( "plyr1c", PLAYER_PID, player_proc,  2, 0, PPOS_OLINE3, game_info.team[0] * NUM_PERTEAM_SPD +  2 + (game_info.off_side * 7)), RL_PLAYER+2);
	set_process_run_level( qcreate( "plyr1d", PLAYER_PID, player_proc,  3, 0, PPOS_QBACK,  game_info.team[0] * NUM_PERTEAM_SPD +  3 + (game_info.off_side * 7)), RL_PLAYER+3);
	set_process_run_level( qcreate( "plyr1e", PLAYER_PID, player_proc,  4, 0, PPOS_WPN1,   game_info.team[0] * NUM_PERTEAM_SPD +  4 + (game_info.off_side * 7)), RL_PLAYER+4);
	set_process_run_level( qcreate( "plyr1f", PLAYER_PID, player_proc,  5, 0, PPOS_WPN2,   game_info.team[0] * NUM_PERTEAM_SPD +  5 + (game_info.off_side * 7)), RL_PLAYER+5);
	set_process_run_level( qcreate( "plyr1g", PLAYER_PID, player_proc,  6, 0, PPOS_WPN3,   game_info.team[0] * NUM_PERTEAM_SPD +  6 + (game_info.off_side * 7)), RL_PLAYER+6);

	set_process_run_level( qcreate( "plyr2a", PLAYER_PID, player_proc,  7, 1, PPOS_DLINE1, game_info.team[1] * NUM_PERTEAM_SPD +  7 - (game_info.off_side * 7)), RL_PLAYER+7);
	set_process_run_level( qcreate( "plyr2b", PLAYER_PID, player_proc,  8, 1, PPOS_DLINE2, game_info.team[1] * NUM_PERTEAM_SPD +  8 - (game_info.off_side * 7)), RL_PLAYER+8);
	set_process_run_level( qcreate( "plyr2c", PLAYER_PID, player_proc,  9, 1, PPOS_DLINE3, game_info.team[1] * NUM_PERTEAM_SPD +  9 - (game_info.off_side * 7)), RL_PLAYER+9);
	set_process_run_level( qcreate( "plyr2d", PLAYER_PID, player_proc, 10, 1, PPOS_LBACK1, game_info.team[1] * NUM_PERTEAM_SPD + 10 - (game_info.off_side * 7)), RL_PLAYER+10);
	set_process_run_level( qcreate( "plyr2e", PLAYER_PID, player_proc, 11, 1, PPOS_LBACK2, game_info.team[1] * NUM_PERTEAM_SPD + 11 - (game_info.off_side * 7)), RL_PLAYER+11);
	set_process_run_level( qcreate( "plyr2g", PLAYER_PID, player_proc, 13, 1, PPOS_DBACK2, game_info.team[1] * NUM_PERTEAM_SPD + 13 - (game_info.off_side * 7)), RL_PLAYER+13);
	set_process_run_level( qcreate( "plyr2f", PLAYER_PID, player_proc, 12, 1, PPOS_DBACK1, game_info.team[1] * NUM_PERTEAM_SPD + 12 - (game_info.off_side * 7)), RL_PLAYER+12);

	// create the cursors
	set_process_run_level( qcreate( "pcursr", PLYR_CURSOR_PID, cursor_proc, 0, 0, 0, 0 ), RL_CURSORS );

	#ifdef BOG_METER
	// create the bog monitor
	init_bog_monitor();
	#endif

	// initialize player info displays
	plyrinfo_init();

	// create the GAME CLOCK process
	qcreate("gmclock",GAME_CLOCK_PID,game_clock_proc,0,0,0,0);

	// create 'upper-left corner' status box
	qcreate( "score", STATUS_BOX_PID, score_status_box, 0, 0, 0, 0 );

#ifndef NOKICKOFF
	// print the opening messages - start idiot boxes
	qcreate("message", MESSAGE5_PID, opening_msg_proc, 0, 0, 0, 0);
#endif

	// create the debug switches
	// FIX!! do we still need this?
	#ifdef DEBUG
	iqcreate( "debug", 0, debug_switch_proc, 0, 0, 0, 0 );
	#endif

	// let everyone get initialized before we proceed
	sleep( 10 );

//#ifndef NO_GAME_PLATES
//////////////////////////////////////////
//// Take away the plates
////
//	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
//#endif

	// Enable drawing
	draw_enable(1);

	// Make sure we are at full intensity if a fade was happening
	// when this got started
	normal_screen();

	// Make sure starts are enabled
	start_enable(TRUE);
	if( game_info.game_state != GS_ATTRACT_MODE )
		qcreate("soff", 0, start_off_proc, 0, 0, 0, 0);

//	InitAudibles();

	return 0;	//	success
}

/****************************************************************************/
void launch_game_proc(void)
{
	qcreate("game", GAME_PROC_PID, game_proc, 0, 0, 0, 0);
}

/****************************************************************************/
short ot_kickoff = 0;

void game_proc( int *parg )
{
//	struct process_node *	kick_meter_process = NULL;
//	struct process_node *	punt_meter_process = NULL;
	int		first_kickoff = TRUE;
	int		do_onside_kick;
	int		half_kickoff = FALSE;
	int		dont_update_los;
	int		active_players;
	int		hvh;
	int		vaud;
	int		gaud;
	int		pnum;
	int		val;
	int		i;

//	Init everything before the main play loop
	if(InitGameProc() != 0)
	{
	//	error
	}

	ot_kickoff = -1;

/*****************************************************************************/
/* main play loop ************************************************************/
/*****************************************************************************/
	while(( game_info.game_state == GS_GAME_MODE ) ||
		(( game_info.game_state == GS_ATTRACT_MODE ) && (1)))
	{

#ifndef NO_GAME_PLATES
		if (plates_on == PLATES_ON)
		{
			// Take away the plates
			iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
		}
#endif

		#ifdef PLAY_DEBUG
		fprintf( stderr, "\n----------\n" );
		#else
		#ifdef FIRE_DEBUG
		fprintf( stderr, "\n----------\n" );
		#endif
		#endif

		// clear game flags (really play flags)
		game_info.game_flags = 0;

		// set GF_OPENING_KICK if needed
		if ((game_info.game_quarter %2 == 0) &&
			(game_info.play_type == PT_KICKOFF) &&
			(game_info.game_time == QUARTER_TIME))
			game_info.game_flags |= GF_OPENING_KICK;

		// store team number who started with the ball
		game_info.off_init = game_info.off_side;

		// set team number who last had control of the ball
		game_info.last_pos = game_info.off_side;

		// inc the play counter
		if( first_kickoff )
			tdata_blocks[!game_info.off_side].off_plays += 1;
		else
			tdata_blocks[game_info.off_side].off_plays += 1;


		// Set flags here in case p_status changes; also done here for
		//  a centralized place to change the pup rules if needed;
		// Set flag for camera pullback
		flg_pullback = !!(pup_pullback);

		// Set flag for a HH team
		flg_hhteam = !!(four_plr_ver &&
			(((p_status & 0x3) == 0x3) || ((p_status & 0xC) == 0xC)));


		// kill the HELMET POPPING OFF shit!!
		killall(LOOSE_HELMET_PID,0xFFFFFFFF);
		delete_multiple_objects(OID_LOOSE_HELMET, 0xFFFFFFFF);

		// play select
		switch( game_info.play_type )
		{
			int but, up_by;
			
			case PT_EXTRA_POINT:
				// skip extra point if it won't make any difference
				up_by = compute_score(game_info.off_side) - compute_score(!game_info.off_side);
				if ((game_info.game_time == 0) &&
					(game_info.game_quarter > 2) &&
					(up_by > 0))
				{
					game_info.play_type = PT_NORMAL;
					goto eoq;
				}
					
				snd_scall_bank(cursor_bnk_str,SWITCH_SND,VOLUME2,127,PRIORITY_LVL5);
				play_select();
				if (game_info.team_play[game_info.off_side] != &play_extra_point)
					break;

				game_info.off_side = game_info.off_init;
				game_info.play_type = PT_KICKOFF;

				#ifdef PLAY_DEBUG
				fprintf( stderr, "  free extra point taken by team %d.\n", game_info.off_side );
				fprintf( stderr, "\n----------\n" );
				#endif

				// if time is up, skip to eoq stuff
				if( game_info.game_time == 0 )
					goto eoq;

				// fall through

			case PT_KICKOFF:
//				reset_onfire_values();
				if( first_kickoff )
				{
#ifdef PLAY_DEBUG
					fprintf( stderr, "  first kickoff, off_side %d\n", game_info.off_side );
#endif
					if (game_info.off_side)
						game_info.team_play[game_info.off_side] = &play_kickoff2_313;
					else
						game_info.team_play[game_info.off_side] = &play_kickoff_313;

					game_info.team_play[!game_info.off_side] = &play_kick_return;
					game_info.los = FIRSTKO_LOS;
					game_info.first_down = 100;
					game_info.line_pos = 0.0f;
					ball_obj.odata.fwd = 256;
					ball_obj.type = LB_KICKOFF;
					break;
				}

				// maybe toggle captain
				if( game_info.off_side != game_info.first_pos )
				{
					game_info.captain = !game_info.captain;
				}


				// FIX!!!  Need to unlock all players on receiving team, allow change plyr, etc.
				// look for onside kick
				do_onside_kick = FALSE;
				if (p_status & TEAM_MASK(game_info.off_side))
				{	// human-controlled team
					if (four_plr_ver)
					{
						but = get_player_sw_current() >> ((game_info.off_side*2)<<3) & (P_A|P_B|P_C);
						if ((but == (P_A|P_B|P_C)) &&
							(get_joy_val_cur(game_info.off_side*2) == JOY_UP))
							do_onside_kick = TRUE;

						but = get_player_sw_current() >> ((game_info.off_side*2+1)<<3) & (P_A|P_B|P_C);
						if ((but == (P_A|P_B|P_C)) &&
							(get_joy_val_cur(game_info.off_side*2+1) == JOY_UP))
							do_onside_kick = TRUE;
					}
					else
					{
						but = get_player_sw_current() >> (game_info.off_side<<3) & (P_A|P_B|P_C);
						if ((but == (P_A|P_B|P_C)) &&
							(get_joy_val_cur(game_info.off_side) == JOY_UP))
							do_onside_kick = TRUE;
					}
				}
				else
				{	// drone-controlled team
					do_onside_kick = cpu_do_onside();
				}

				// no onside kicks to open a half
				if (first_kickoff || half_kickoff)
					do_onside_kick = FALSE;


#ifdef ONSIDE_KICK_DEBUG
#endif
				if (do_onside_kick)
				{
					// onside kick
					game_info.team_play[game_info.off_side] = &play_onside_313;
					game_info.team_play[!game_info.off_side] = &play_onside_kick_return;
					game_info.los = 30;
					game_info.first_down = 100;
					game_info.line_pos = 0.0f;
					ball_obj.odata.fwd = 256;
					ball_obj.type = LB_KICKOFF;
					game_info.game_flags |= GF_ONSIDE_KICK;

					// Kill VS music & start game tune
					snd_stop_track(SND_TRACK_0);
					snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
					snd_scall_bank(game_tune_bnk_str, 0, MUSIC_VOL, 127, SND_PRI_SET|SND_PRI_TRACK0);
				}
				else
				{
					// regular (quick) kick
					game_info.off_init = !game_info.off_init;
					game_info.off_side = game_info.off_init;
					game_info.last_pos = game_info.off_init;

					game_info.team_play[game_info.off_side] = &play_kick_return_short;
					game_info.team_play[!game_info.off_side] = &play_kickoff_short;
					game_info.los = 40;
					game_info.first_down = 100;
					game_info.line_pos = 0.0f;
					game_info.game_flags |= GF_QUICK_KICK;
					ball_obj.odata.fwd = 256;
					ball_obj.type = LB_KICKOFF;

					// Kill VS music & start game tune
					snd_stop_track(SND_TRACK_0);
					snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
					snd_scall_bank(game_tune_bnk_str, 0, MUSIC_VOL, 127, SND_PRI_SET|SND_PRI_TRACK0);
//					snd_scall_bank(qtr_tunes_tbl[game_info.game_quarter], 0, VOLUME0, 127, SND_PRI_SET|SND_PRI_TRACK0);

#ifdef PLAY_DEBUG
					fprintf( stderr, "  regular (quick) kickoff for team %d\n", game_info.off_side );
#endif
				}
				showhide_pinfo(1);						// show turbo bars
				break;
			default:
				snd_scall_bank(cursor_bnk_str,SWITCH_SND,VOLUME2,127,PRIORITY_LVL5);
				play_select();
				
				if ((game_info.down == 4) &&
					(game_info.team_play[game_info.off_side] != &play_punt) &&
					(game_info.team_play[game_info.off_side] != &play_field_goal))
					tdata_blocks[game_info.off_side].forthdwn_attempts += 1;

				// Do opening idiot box - big instruction box
				// If all human players in game have > 3 games of experience, skip box
				if ((idiot_boxes_shown == 1) &&
					 (((plyr_data_ptrs[0]->games_played < 3) && (p_status & 0x1)) ||
					  ((plyr_data_ptrs[1]->games_played < 3) && (p_status & 0x2)) ||
					  ((plyr_data_ptrs[2]->games_played < 3) && (p_status & 0x4)) ||
					  ((plyr_data_ptrs[3]->games_played < 3) && (p_status & 0x8))))
					qcreate("rules", IDIOT_BOX2_PID, instructions_proc, 0, 0, 0, 0);
				break;
		}

		reinit_offense_players();
		reinit_defense_players();
		
		// set handicaps
		set_handicaps();
		
#ifdef PLAY_DEBUG
		fprintf( stderr, "  hcap: %d %d\n", game_info.team_handicap[0], game_info.team_handicap[1] );
#endif
#ifdef MARK
		fprintf( stderr, "  hcap: %d %d\n", game_info.team_handicap[0], game_info.team_handicap[1] );
#endif
#ifdef JASON
		fprintf( stderr, "  hcap: %d %d\n", game_info.team_handicap[0], game_info.team_handicap[1] );
#endif
#if 0
		fprintf( stderr, "   att   cmp   yds    TD   INT  rtng\n" );
		for (i = 0; i < 2; i++ )
		
		fprintf( stderr, "%6d%6d%6d%6d%6d%6.1f\n",
			tdata_blocks[i].passes,
			tdata_blocks[i].completions,
			tdata_blocks[i].passing_yards,
			tdata_blocks[i].pass_touchdowns,
			tdata_blocks[!i].interceptions,
			pass_rating( tdata_blocks[i].passes,
				tdata_blocks[i].completions,
				tdata_blocks[i].passing_yards,
				tdata_blocks[i].pass_touchdowns,
				tdata_blocks[!i].interceptions ));
#endif

		// set show_fd and show_los
		if( game_info.play_type == PT_KICKOFF )
		{
			game_info.show_los = 30;
			game_info.show_fd = -1;
		}
		else
		{
			game_info.show_los = game_info.los;
			game_info.show_fd = game_info.first_down;
		}

		// start the replay recorder
//N_RPLAY		set_recorder_mode( RR_RESTART );

		// line up & initialize the players & drones
		init_drones_prelineup( -1 );		// before lineup
		lineup_players( -1 );
		init_drones_postlineup( -1 );		// after lineup

		qcreate("anc_scr",0,pre_snap_speech_proc,0,0,0,0);

		// misc initialization
		game_info.ball_carrier = game_info.off_side ? 8 : 1;
		ball_obj.flags = 0;
		game_info.spin_cnt[0] = 0;
		game_info.spin_cnt[1] = 0;
		game_info.hurdle_cnt[0] = 0;
		game_info.hurdle_cnt[1] = 0;
		game_info.fumbles_on_play = 0;
		forward_catcher = -1;

		if( game_info.play_type == PT_KICKOFF )
		{
			// This will kill any messages that have appeared at end of last play (ex: safety!, No Good!)
			// Normally, play select clears them up, but on kickoffs we don't go to play select
			del1c(OID_MESSAGE,0xFFFFFFF0);
			del1c( 0x5678FACE, -1 );
			killall(MESSAGE_PID,0xFFFFFFF0);
			delete_multiple_textures(MESSAGE_TID,0xFFFFFFFF);

			// NEW - Kill any special-effects type crap too
			delete_multiple_objects( OID_SMKPUFF, 0xFFFFFFFF );
			killall(SPFX_PID,0xFFFFFFFF);

			if( game_info.game_flags & GF_QUICK_KICK )
			{
				obj_3d * pobj = (obj_3d *)&player_blocks[3 + 7 * game_info.off_side];

				// Tweak this for position of intended receiver on quick kickoffs - ball will fall on him
				pobj->x = WORLDX(min_ko_catch_ht[OFFH] +
									randrng(1 + max_ko_catch_ht[OFFH] - min_ko_catch_ht[OFFH]));
				pobj->z = (float)randrng((int)(40.0f*YD2UN)) - (20.0f*YD2UN);

				ball_obj.odata.vx = (pobj->x + (game_info.off_side ? -20.0f : 20.0f)*YD2UN) / 150.0f;
				ball_obj.odata.vy = -2.8f;
				ball_obj.odata.vz = pobj->z / 150.0f;

				ball_obj.odata.x = pobj->x - 40.0f * ball_obj.odata.vx;
				ball_obj.odata.z = pobj->z - 40.0f * ball_obj.odata.vz;
				ball_obj.odata.y = 176.0f;
				ball_obj.flags = BF_FREEZE;
				ball_obj.pitch = 256;
				ball_obj.tx = pobj->x;
				ball_obj.tz = pobj->z;
				game_info.ball_carrier = -1;
			}
			else
			{
				ball_obj.odata.x = WORLDX(game_info.los);
				ball_obj.tx = ball_obj.odata.x;
				ball_obj.odata.y = 0.0f;
				ball_obj.odata.z = 0.0f;
				ball_obj.odata.vz = 0.0f;
				ball_obj.odata.vy = 0.0f;
				ball_obj.odata.vx = 0.0f;
				ball_obj.tz = 0.0f;
				ball_obj.flags = 0;
				ball_obj.pitch = 256;
				game_info.ball_carrier = -1;
			}
		}


		// Start player names display proc
		// FIX!!!
		qcreate( "plyrnms", PLYRINFO_PID, show_player_names_proc, 0, 0, 0, 0 );

		// go into pre-snap mode
		game_info.game_mode = GM_PRE_SNAP;

		// auto hike ball when counter reaches 0
		if( game_info.play_type == PT_KICKOFF)
		{
			if (first_kickoff)
			{
				// Time opening kickoff to look good for camera path
				game_info.auto_hike_cntr = INITIAL_KOFF_TIME;
				first_kickoff = FALSE;
			}
			else
				game_info.auto_hike_cntr = MAX_KOFF_TIME;
		}
		else
			game_info.auto_hike_cntr = MAX_HIKE_TIME;
//			game_info.auto_hike_cntr = 1000000;

		// QB yelling & yardage info plate
		switch( game_info.play_type )
		{
			case PT_NORMAL:
				qcreate("yrdplt",SCR_PLATE_PID,yardage_info_plate,0,0,0,0);
				qcreate("qbyell",QBYELL_PID,yell_signals_proc,0,0,0,0);
				if (four_plr_ver)
					qcreate("whoisqb",IDIOT_BOX_PID,whois_qb_idiot_box_proc,0,0,0,0);
				break;
			case PT_EXTRA_POINT:
			case PT_KICKOFF:
				break;
		}

		game_info.old_los = game_info.los;
		game_info.play_end_pos = WORLDX(-100);

		PlayStuf__ResetAudibles();

//AUDIBLE_RESTART:

		// Start kick meter proc in case its needed (goes away if not)
//		kick_meter_process = 
		qcreate( "kickmtr", PLYRINFO_PID, kick_meter_proc, 0, 0, 0, 0 );

		// Start punt meter proc in case its needed (goes away if not)
//		punt_meter_process = 
		qcreate( "puntmtr", PLYRINFO_PID, punt_meter_proc, 0, 0, 0, 0 );


		// DJT: Restart changed to avoid play/lineup/kickmeter/etc confusion
		//  Audibles are locked out in "player.c" for FG & Punt plays
AUDIBLE_RESTART:

#ifdef PLAY_DEBUG
		fprintf( stderr, "  play type: %s\n", play_type_names[game_info.play_type] );
		fprintf( stderr, "  %s down and %d on %d\n",
			down_names[1+game_info.down],
			game_info.first_down - game_info.los,
			game_info.los );
#else
#ifdef FIRE_DEBUG
		fprintf( stderr, "  play type: %s\n", play_type_names[game_info.play_type] );
		fprintf( stderr, "  %s down and %d on %d\n",
			down_names[1+game_info.down],
			game_info.first_down - game_info.los,
			game_info.los );
#endif
#endif
		// Reset turbo meters for both players (should this be here or above the AUDIBLE_RESTART label?)
		pdata_blocks[0].turbo = 100.0f;
		pdata_blocks[1].turbo = 100.0f;
		pdata_blocks[2].turbo = 100.0f;
		pdata_blocks[3].turbo = 100.0f;

		// init plyr_controls
		game_info.plyr_control[0] = -1;
		game_info.plyr_control[1] = -1;
		game_info.plyr_control[2] = -1;
		game_info.plyr_control[3] = -1;

		for( i = 0; i < 7; i++ )
		{
			if( p_status & TEAM1_MASK )
			{	// someone from team 0 is human
				if (((p_status & TEAM1_MASK) == TEAM1_MASK) &&
					(four_plr_ver))
				{	// 2 players on team
					int		lcaptain = game_info.off_side ? game_info.captain : off_captain();

					if( game_info.team_play[0]->formation[i].control & LU_CAPN )
						game_info.plyr_control[lcaptain] = i;

					if( game_info.team_play[0]->formation[i].control & LU_PLYR2 )
						game_info.plyr_control[!lcaptain] = i;
				}
				else
				{	// 1 player on team
					if( game_info.team_play[0]->formation[i].control & LU_CAPN )
						game_info.plyr_control[four_plr_ver ? ((p_status&0x3)-1) : 0] = i;
				}
			}

			if( p_status & TEAM2_MASK )
			{	// someone from team 1 is human
				if((( p_status & TEAM2_MASK) == TEAM2_MASK ) &&
					(four_plr_ver))
				{	// 2 players on team
					int		lcaptain = game_info.off_side ? off_captain() : game_info.captain;

					if( game_info.team_play[1]->formation[i].control & LU_CAPN )
						game_info.plyr_control[lcaptain+2] = i+7;

					if( game_info.team_play[1]->formation[i].control & LU_PLYR2 )
						game_info.plyr_control[!lcaptain+2] = i+7;
				}
				else
				{	// 1 player on team
					if( game_info.team_play[1]->formation[i].control & LU_CAPN )
						game_info.plyr_control[four_plr_ver ? (((p_status>>2)&0x3)+1) : 1] = i+7;
				}
			}
		}

		// get the player block station handles set
		update_stations();

		// wait for play to start
		while( game_info.game_mode == GM_PRE_SNAP && DBG_KO_BOOL )
		{
			if( ( game_info.audible_flags[OFFENSE] == 1 ) ||
				( game_info.audible_flags[DEFENSE] == 1 ) )
			{
				// new play called, setup play called structure

				//	Make sure that any dust/fire is killed before we re-init everything
				delete_multiple_objects( OID_SMKPUFF, -1 );
				killall( SPFX_PID, -1 );

//				// Make sure the kick & punt meters are deleted
//				//  they have exit procs to clean themselves up
//				kill(kick_meter_process, 0);
//
//				kill(punt_meter_process, 0);

				// call a sound effect

				// Reset the "audible was called" flags for both sides
				//  only reset the flag if the player did the audible
				//  that way team 1 can't stop team 2 from doing an audible as well
				if( game_info.audible_flags[OFFENSE] == 1 )
				{
					// Reset offensive line
					reinit_offense_players();

					init_drones_prelineup( game_info.off_side );
					lineup_players( game_info.off_side );
					init_drones_postlineup( game_info.off_side );

					// Flag a coordinate recalc
					i = game_info.off_side ? NUM_PERTEAM : 0;
					do showpnam_upd[i++] = -1;
					while( i != NUM_PERTEAM && i != NUM_PLAYERS );

					game_info.audible_flags[OFFENSE] = 0;
				}
				if( game_info.audible_flags[DEFENSE] == 1 )
				{
					// Reset defensive line
					reinit_defense_players();

					init_drones_prelineup( !game_info.off_side );
					lineup_players( !game_info.off_side );
					init_drones_postlineup( !game_info.off_side );

					// Flag a coordinate recalc
					i = game_info.off_side ? 0 : NUM_PERTEAM;
					do showpnam_upd[i++] = -1;
					while( i != NUM_PERTEAM && i != NUM_PLAYERS );

					game_info.audible_flags[DEFENSE] = 0;
				}

				goto AUDIBLE_RESTART;
			}

			sleep(1);
			
			// Make sure starts are enabled
			if (first_kickoff || half_kickoff || existp(IDIOT_BOX2_PID,0xffffffff))
				start_enable(FALSE);
			else
				start_enable(TRUE);

			sleep(1);
		}

		// wait for the play to end
		while( game_info.game_mode != GM_PLAY_OVER && DBG_KO_BOOL )
			sleep( 1 );

#ifdef DBG_KO
		if (dbg_ko)
		{
			game_info.play_type = PT_KICKOFF;
			first_kickoff = TRUE;

			clock_active = 0;
			said_latehit_sp = 0;
			said_30_sp = 0;
			said_20_sp = 0;
			said_10_sp = 0;
			said_5_sp = 0;
		 	taunted = 0;

			dbg_ko = FALSE;
			continue;
		}
#endif //DBG_KO

		// stop the game clock
		clock_active = 0;
		said_latehit_sp = 0;
		said_30_sp = 0;
		said_20_sp = 0;
		said_10_sp = 0;
		said_5_sp = 0;
	 	taunted = 0;
		no_result_box = 0;

		// stop QB name calling...
		killall(QB_SPEECH_PID,0xffffffff);

		// discard fractional seconds left after each play
		game_info.game_time = game_info.game_time & 0x00FFFF00;

#ifdef PLAY_DEBUG
		fprintf( stderr, "  play end cause: %s\n", play_end_cause_names[game_info.play_end_cause] );
		fprintf( stderr, "  game_flags: " );
		print_game_flags();
#endif

		// if last_pos != off_side, the last team to touch the ball never
		// established clear control, so give control back to off_side.
		if( game_info.last_pos != game_info.off_side )
		{
#ifdef DEBUG
			fprintf( stderr, "last_pos(%d) doesn't match off_side(%d).\nRetur"
					"ning control to team %d.\n", game_info.last_pos, game_info.off_side,
					game_info.last_pos );
#endif
			game_info.off_side = game_info.last_pos;
		}

		// if off_side isn't off_init, make sure POSS_CHANGE bit is set.
		// holler if it's not
		if( game_info.off_side != game_info.off_init )
		{
			if(!( game_info.game_flags & GF_POSS_CHANGE ))
			{
				game_info.game_flags |= GF_POSS_CHANGE;
#ifdef DEBUG
				fprintf( stderr, "Weird.  off_side != off_init, but POSS_CHAN"
						"GE isn't set.  Please tell Jason you saw this.\n" );
#endif
			}
		}

		// round play-end field position toward the fifty yard line
		game_info.play_end_pos = (float)(((int)(game_info.play_end_pos)*10)/78) * 7.8f;

		// play-end bookkeeping
		fpend = FIELDX(game_info.play_end_pos);

		// set play_result
		game_info.last_play_type = game_info.play_type;
		game_info.play_result_flags = 0;
		result_functions[game_info.play_type][game_info.play_end_cause]();

		// if it's a touchdown, set fpend to 100
		if ((game_info.play_result == PR_TOUCHDOWN) ||
			(game_info.play_result == PR_CONVERSION))
		{
			fpend = 100;
		}
		else
		{	// otherwise make sure we're spotting the ball in bounds
			fpend = LESSER(fpend,99);
			fpend = GREATER(fpend,1);
		}

#ifndef NO_FIRE
		// check for loss of yards WRT fires
		if (((game_info.play_type == PT_NORMAL) || (game_info.play_type == PT_EXTRA_POINT)) &&
			(game_info.play_result != PR_INCOMPLETE) &&
			(game_info.play_result != PR_FIELD_GOAL) &&
			(game_info.off_side == game_info.off_init) &&
			(fpend < game_info.los ))
		{	// loss of yards, offensive fire gone, maybe def fire
			#ifdef FIRE_DEBUG
			fprintf( stderr, "FIRE: Loss of yards\n" );
			if (game_info.team_fire[game_info.off_side])
				fprintf( stderr, "FIRE: Offense loses fire\n" );
			#endif

			if (game_info.team_fire[game_info.off_side])
				snd_scall_bank(plyr_bnk_str,LOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL4);
			reset_team_fire( game_info.off_side );
			showhide_pinfo(1);

//			if (((tdata_blocks[!game_info.off_side].consecutive_sacks += 1) == DEFENSE_FIRECNT-1) &&
//				!(game_info.team_fire[!game_info.off_side]))
//			{
//				snd_scall_bank(ancr_swaped_bnk_str,184,ANCR_VOL2,127,LVL4);			// say "THEY'RE HEATING UP"
//			}
		 	if (((tdata_blocks[!game_info.off_side].consecutive_sacks += 1) == DEFENSE_FIRECNT) &&
				!(game_info.team_fire[!game_info.off_side]))
			{	// defense catches fire
				game_info.team_fire[!game_info.off_side] = TRUE;

				snd_scall_bank(ancr_swaped_bnk_str,185,ANCR_VOL2,127,LVL4);			// say "THEY'RE ON FIRE"
				no_result_box = 1;
				// Put fire burst on Defensive backs
				if (player_blocks[3].odata.plyrmode == MODE_QB)
					i = 3;
				else
					i = 10;
				if (player_blocks[i].odata.pobj_hit != NULL)
				{
					fbplyr_data *guy_we_hit;

					snd_scall_bank(plyr_bnk_str,FIRE_BURST_SND,VOLUME4,127,PRIORITY_LVL5);			// make explosion sound
					snd_scall_bank(plyr_bnk_str,FIRE_BURST_SND,VOLUME4,127,PRIORITY_LVL5);			// play twice for EFFECT!!!
					guy_we_hit = (fbplyr_data *)(player_blocks[i].odata.pobj_hit);
					qcreate ("fburst", SPFX_PID, player_burst_into_flames, guy_we_hit->plyrnum, 6, 4, 0);
				}


				// set last_receiver to rcvr 0 in case of carry-over to offense
				tdata_blocks[!game_info.off_side].last_receiver = 4 + 7 * !game_info.off_side;
				#ifdef FIRE_DEBUG
				fprintf( stderr, "FIRE: Defense catches fire\n" );
				#endif
			}
		}
		else
		{
			if (!game_info.team_fire[!game_info.off_side])
				tdata_blocks[!game_info.off_side].consecutive_sacks = 0;
		}

		#ifdef FIRE_DEBUG
		if (game_info.team_fire[!game_info.off_side])
			fprintf( stderr, "FIRE: Defense is on fire\n" );
		else
			fprintf( stderr, "FIRE: Defense has %d consecutive losses\n",
				tdata_blocks[!game_info.off_side].consecutive_sacks);
		#endif
#endif

		// inc stats.
		if(( game_info.game_flags & (GF_BALL_PUNTED|GF_BALL_KICKED)) &&
			( game_info.off_side != game_info.off_init ) &&
			( game_info.play_result == PR_NEW_POSSESSION ) &&
			( game_info.ball_carrier / 7 == game_info.off_side ))
		{
			// punt or first kickoff.  give return yards.
			tdata_blocks[game_info.off_side].return_yards += (fpend - game_info.ko_catch);
			tdata_blocks[game_info.off_side].total_yards += (fpend - game_info.ko_catch);
			tdata_blocks[game_info.off_side].kick_returns += 1;
			#ifdef STATS_DEBUG
			fprintf( stderr, "  STAT> %d yard kick(1) return, now %d for %d\n",
				 fpend - game_info.ko_catch,
				 tdata_blocks[game_info.off_side].kick_returns,
				 tdata_blocks[game_info.off_side].return_yards);
			#endif
		}
		else if(( game_info.game_flags & (GF_QUICK_KICK)) &&
			( game_info.off_side == game_info.off_init ) &&
			(( game_info.play_result == PR_NEW_POSSESSION ) ||
			( game_info.play_result == PR_TOUCHDOWN )) &&
			( game_info.ball_carrier / 7 == game_info.off_side ))
		{
			// quick kickoff.  give return yards.
			tdata_blocks[game_info.off_side].return_yards += (fpend - game_info.ko_catch);
			tdata_blocks[game_info.off_side].total_yards += (fpend - game_info.ko_catch);
			tdata_blocks[game_info.off_side].kick_returns += 1;
			#ifdef STATS_DEBUG
			fprintf( stderr, "  STAT> %d yard kick(2) return, now %d for %d\n",
				 fpend - game_info.ko_catch,
				 tdata_blocks[game_info.off_side].kick_returns,
				 tdata_blocks[game_info.off_side].return_yards);
			#endif
		}
		else if ((game_info.game_flags & GF_FWD_PASS) &&
			(game_info.play_result != PR_INCOMPLETE) &&
			!(game_info.game_flags & GF_POSS_CHANGE) &&
			(game_info.play_end_cause != PEC_DEAD_BALL_IB) &&
			(game_info.off_side == game_info.off_init))
		{
			// completed pass.  give pass yards
			tdata_blocks[game_info.off_side].passing_yards += fpend - game_info.los;
			tdata_blocks[game_info.off_side].total_yards += fpend - game_info.los;
			tdata_blocks[game_info.off_side].completions += 1;
			if (game_info.play_result == PR_TOUCHDOWN)
				tdata_blocks[game_info.off_side].pass_touchdowns++;
				
			#ifdef STATS_DEBUG
			fprintf( stderr, "  STAT> %d yard pass, now %d of %d for %d\n",
				 fpend - game_info.los,
				 tdata_blocks[game_info.off_side].completions,
				 tdata_blocks[game_info.off_side].passes,
				 tdata_blocks[game_info.off_side].passing_yards );
			#endif
		}
		else if (!( game_info.game_flags & GF_FWD_PASS) &&
			(game_info.off_side == game_info.off_init) &&
			!(game_info.game_flags & GF_BALL_PUNTED) &&
			(game_info.ball_carrier %7 == 3) &&
			!(game_info.game_flags & GF_BALL_CROSS_LOS))
		{
			// sack.  give sack yards to defense
			tdata_blocks[!game_info.off_side].sack_yards -= fpend - game_info.los;
			tdata_blocks[!game_info.off_side].sacks += 1;
			#ifdef STATS_DEBUG
			fprintf( stderr, "  STAT> %d yard sack, now %d for %d\n",
				 -(fpend - game_info.los),
				 tdata_blocks[game_info.off_side].sacks,
				 tdata_blocks[game_info.off_side].sack_yards );
			#endif
		}
		else if( !( game_info.game_flags & GF_FWD_PASS ) &&
			( game_info.off_side == game_info.off_init ) &&
			!( game_info.game_flags & GF_BALL_PUNTED ) &&
			!( game_info.game_flags & GF_BALL_KICKED ) &&
			( game_info.play_result != PR_INCOMPLETE ))
		{
			// rush. give rush yards
			tdata_blocks[game_info.off_side].rushing_yards += fpend - game_info.los;
			tdata_blocks[game_info.off_side].total_yards += fpend - game_info.los;
			tdata_blocks[game_info.off_side].rushes += 1;
			#ifdef STATS_DEBUG
			fprintf( stderr, "  STAT> %d yard rush, now %d for %d\n",
				 fpend - game_info.los,
				 tdata_blocks[game_info.off_side].rushes,
				 tdata_blocks[game_info.off_side].rushing_yards );
			#endif
		}

		if( game_info.team_play[game_info.off_init] == &play_field_goal )
			tdata_blocks[game_info.off_init].fg_attempts += 1;
			
#ifdef STATS_DEBUG
		dump_game_state(FALSE);
#endif
		// next play defaults to PT_NORMAL
		game_info.play_type = PT_NORMAL;
		game_info.just_got_1st = 0;
		
		dont_update_los = FALSE;

		// clear half_kickoff
		half_kickoff = FALSE;

		// set spot, down, ytg, possession, and play type based on play results
		switch( game_info.play_result )
		{
			case PR_DOWN:
			case PR_FUMBLED_OOB:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: ball down/fumbled oob at %d\n", fpend );
#endif
				if( fpend >= game_info.first_down )
				{
#ifdef PLAY_DEBUG
					fprintf( stderr, "  result(2): first down\n");
#endif
					if (game_info.down == 4)
						tdata_blocks[game_info.off_side].forthdwn_convs += 1;
						
					game_info.down = 1;
					game_info.los = fpend;
					game_info.first_down = game_info.los + FDYARDS;

					if( game_info.first_down > 100 || pup_no1stdown )
						game_info.first_down = 100;

					tdata_blocks[game_info.off_side].first_downs++;
					game_info.just_got_1st = 1;
#ifndef NO_FIRE
					#ifdef FIRE_DEBUG
					if (game_info.team_fire[!game_info.off_side])
						fprintf( stderr, "FIRE: Defense loses fire (1st down)\n" );
					#endif
					if (game_info.team_fire[!game_info.off_side])
						snd_scall_bank(plyr_bnk_str,LOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL4);
					reset_team_fire( !game_info.off_side );
					showhide_pinfo(1);
#endif
				}
				else if( game_info.down == 4 )
				{
					game_info.play_end_pos = WORLDX(fpend);
#ifdef PLAY_DEBUG
					fprintf( stderr, "  result(1): turnover on downs\n");
#endif
					// Turnover on downs
					dont_update_los = TRUE;
					game_info.off_turnover = 1;
					game_info.down = 1;

					if (game_info.team_fire[game_info.off_side])
						snd_scall_bank(plyr_bnk_str,LOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL4);

					// reset fire on team that blew it
					reset_team_fire( game_info.off_side );

					// if they're not on fire, reset fire on team that got it
					if (!game_info.team_fire[!game_info.off_side])
						reset_team_fire( !game_info.off_side );

					game_info.off_side = !game_info.off_side;
					game_info.los = FIELDX(game_info.play_end_pos);
					game_info.first_down = game_info.los + FDYARDS;
					if( game_info.first_down > 100 || pup_no1stdown )
						game_info.first_down = 100;

//					reset_onfire_values();
				}
				else
				{
					game_info.los = fpend;
					game_info.down += 1;
				}
				break;

			case PR_INCOMPLETE:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: incomplete\n");
#endif
				game_info.off_side = game_info.off_init;
				game_info.down += 1;
				if( game_info.down == 5 )
				{
#ifdef PLAY_DEBUG
					fprintf( stderr, "  result(2): turnover on downs\n");
#endif
					if (game_info.team_fire[game_info.off_side])
						snd_scall_bank(plyr_bnk_str,LOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL4);

					// reset fire on team that blew it
					reset_team_fire( game_info.off_side );

					// if they're not on fire, reset fire on team that got it
					if (!game_info.team_fire[!game_info.off_side])
						reset_team_fire( !game_info.off_side );

					game_info.off_side = !game_info.off_side;
					game_info.down = 1;
					// Turnover on downs
					dont_update_los = TRUE;
					game_info.off_turnover = 1;
					game_info.los = 100 - game_info.los;
					game_info.first_down = game_info.los + FDYARDS;
					if( game_info.first_down > 100 || pup_no1stdown )
						game_info.first_down = 100;
				}
				break;

			case PR_TOUCHBACK:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: touchback\n");
#endif
				game_info.down = 1;
				game_info.los = 20;
				game_info.first_down = (pup_no1stdown) ? 100 : (game_info.los + FDYARDS);
				break;

			case PR_PAT_SAFETY:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: extra-point safety\n");
#endif
				game_info.scores[!game_info.off_side][game_info.game_quarter] += 1;
				game_info.off_side = game_info.off_init;
				game_info.play_type = PT_KICKOFF;
				tdata_blocks[game_info.off_side].safetys++;
				break;

			case PR_SAFETY:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: safety\n");
#endif
				game_info.scores[!game_info.off_side][game_info.game_quarter] += 2;
				game_info.play_type = PT_KICKOFF;
				tdata_blocks[game_info.off_side].safetys++;
				break;

			case PR_NEW_POSSESSION:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: new possession on %d\n", fpend );
#endif
				game_info.down = 1;
				game_info.los = fpend;
				game_info.first_down = game_info.los + FDYARDS;
				if( game_info.first_down > 100 || pup_no1stdown )
					game_info.first_down = 100;
				break;

			case PR_TOUCHDOWN:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: touchdown\n" );
#endif
				if ((game_info.down == 4) &&
					(game_info.off_side == game_info.off_init))
					tdata_blocks[game_info.off_side].forthdwn_convs += 1;
					
				game_info.down = -1;
				game_info.scores[game_info.off_side][game_info.game_quarter] += 6;
				game_info.play_type = PT_EXTRA_POINT;
				game_info.los = 90;
				game_info.first_down = 100;
				tdata_blocks[game_info.off_side].touchdowns++;
#ifndef NO_FIRE
				#ifdef FIRE_DEBUG
				if (game_info.team_fire[!game_info.off_side])
					fprintf( stderr, "FIRE: Defense loses fire (touchdown)\n" );
				#endif
				if (game_info.team_fire[!game_info.off_side])
					snd_scall_bank(plyr_bnk_str,LOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL4);
				reset_team_fire( !game_info.off_side );
				showhide_pinfo(1);
#endif
				break;

			case PR_FAILED_CONVERSION:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: failed conversion\n" );
#endif
				game_info.off_side = game_info.off_init;
				game_info.play_type = PT_KICKOFF;
				tdata_blocks[game_info.off_side].two_pt_attempts++;
				// if they were on fire, screw 'em, they suck
				if (game_info.team_fire[game_info.off_side])
					reset_team_fire( game_info.off_side );
				break;

			case PR_CONVERSION:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: 2-pt conversion\n" );
#endif
				game_info.scores[game_info.off_side][game_info.game_quarter] += 2;
				game_info.off_side = game_info.off_init;
				game_info.play_type = PT_KICKOFF;
				tdata_blocks[game_info.off_side].two_pt_attempts++;
				tdata_blocks[game_info.off_side].two_pt_convs++;
				break;

			case PR_FIELD_GOAL:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: field goal\n" );
#endif
				game_info.scores[game_info.off_side][game_info.game_quarter] += 3;
				game_info.off_side = game_info.off_init;
				game_info.play_type = PT_KICKOFF;
				tdata_blocks[game_info.off_side].field_goals++;
#ifndef NO_FIRE
				#ifdef FIRE_DEBUG
				if (game_info.team_fire[!game_info.off_side])
					fprintf( stderr, "FIRE: Defense loses fire (field goal)\n" );
				#endif
				if (game_info.team_fire[!game_info.off_side])
					snd_scall_bank(plyr_bnk_str,LOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL4);
				reset_team_fire( !game_info.off_side );
				showhide_pinfo(1);
#endif
				break;

			case PR_MISSED_KICK:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: missed kick\n" );
#endif
				#ifdef FIRE_DEBUG
				if (game_info.team_fire[game_info.off_side])
					fprintf( stderr, "FIRE: Defense loses fire (missed kick)\n" );
				#endif
				if (game_info.team_fire[game_info.off_side])
					snd_scall_bank(plyr_bnk_str,LOST_FIRE_SND,VOLUME4,127,PRIORITY_LVL4);

				reset_team_fire( game_info.off_side );

				game_info.off_side = !game_info.off_side;
				game_info.los = 100-game_info.los;
				game_info.down = 1;
				game_info.first_down = game_info.los + FDYARDS;
				if( game_info.first_down > 100 || pup_no1stdown )
					game_info.first_down = 100;
				break;

			case PR_PUNT_OOB:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: punt out of bounds\n" );
#endif
				game_info.off_side = !game_info.off_init;
				game_info.down = 1;
				game_info.los = 100-fpend;
				game_info.first_down = game_info.los + FDYARDS;
				if( game_info.first_down > 100 || pup_no1stdown )
					game_info.first_down = 100;
				break;

			case PR_DOWNED_PUNT:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: punt downed\n" );
#endif
				game_info.off_side = !game_info.off_init;
				game_info.down = 1;

				// this number has to be 99, since that's the cap value on
				// fpend above.  No harm done, really. We fuzz the end zone
				// in lots of places.
				if( fpend >= 99 )
					game_info.los = 20;
				else
					game_info.los = 100-fpend;
				game_info.first_down = game_info.los + FDYARDS;
				if( game_info.first_down > 100 || pup_no1stdown )
					game_info.first_down = 100;
				break;

			case PR_KICKOFF_OOB:
#ifdef PLAY_DEBUG
				fprintf( stderr, "  result: kickoff out of bounds\n" );
#endif
				game_info.off_side = !game_info.off_init;
				game_info.los = 40;
				game_info.first_down = (pup_no1stdown) ? 100 : (40 + FDYARDS);
				game_info.down = 1;
				break;
		}

		update_status_box = 1;
	
		// set show_fd and show_los
		if (!dont_update_los)
		{
			if( game_info.off_init == game_info.off_side )
			{
				game_info.show_los = game_info.los;
				game_info.show_fd = game_info.first_down;
			}
			else
			{
				game_info.show_los = 100-game_info.los;
					game_info.show_fd = 100-game_info.first_down;
			}
		}

		// if last play wasn't a kickoff, next play isn't a kickoff,
		// last play was a turnover, and off_side == first_pos, toggle
		// captain
		if ((game_info.play_type != PT_KICKOFF) &&
			(game_info.last_play_type != PT_KICKOFF) &&
			(game_info.off_side != game_info.off_init) &&
			(game_info.off_side == game_info.first_pos))
		{
			game_info.captain = !game_info.captain;
		}

		// wait for any replay to finish up before putting up play result box
// JTA - no replay!
//		while( playback )
//			sleep( 1 );

		// Turn on a play result box - it gets deleted before play_select or at end of quarters
		if( result_box( TRUE ))
			// Returns true if score plate is the shown - keep scores on screen longer
			wait_for_all_buttons( 70, 265 );	// 235
		else
			// Returns false if just result box appears on screen
			wait_for_all_buttons( 30, 205 ); 	// 160

		// if a replay got started while the result box was up, wait for it to finish
// JTA - No replay!
//		while( playback )
//			sleep( 1 );

		// Get rid of score plate or result box
		result_box( FALSE );

		// clear completion counter if no completed forward pass
		if ((forward_catcher == -1) &&
			(tdata_blocks[game_info.off_side].consecutive_comps))
		{
			tdata_blocks[game_info.off_side].consecutive_comps = 0;
			#ifdef FIRE_DEBUG
			fprintf( stderr, "FIRE: no fwd pass, completions[%d] = 0\n",
				game_info.off_side );
			#endif
		}

		#ifdef FIRE_DEBUG
		if (game_info.team_fire[game_info.off_side])
			fprintf( stderr, "FIRE: fire receiver is %2x\n",
				JERSEYNUM(tdata_blocks[game_info.off_side].last_receiver));
		#endif

		// spot between hash marks
		game_info.line_pos = game_info.play_end_z;
		if( fabs(game_info.line_pos) > HASHWIDTH )
			game_info.line_pos = game_info.line_pos / fabs( game_info.line_pos ) * HASHWIDTH;

#ifdef FG_DEBUG
		game_info.off_side = 1;
		game_info.play_type = PT_NORMAL;
		game_info.down = 4;
		fprintf( stderr, "field pos:" );
		game_info.los = 10 * (getchar()-'0');
		game_info.los += getchar()-'0';
		game_info.first_down = 30 + game_info.los;
#endif
#ifdef ONSIDE_KICK_DEBUG
		game_info.play_type = PT_KICKOFF;
		tdata_blocks[0].lost_fumbles = 0;
		tdata_blocks[1].lost_fumbles = 0;
		tdata_blocks[0].interceptions = 0;
		tdata_blocks[1].interceptions = 0;
#endif

eoq:
		// at this point the play has ended...so check if end of quarter
		if(( game_info.game_time == 0 ) &&
			( game_info.play_type != PT_EXTRA_POINT ) &&
			( game_info.game_state != GS_ATTRACT_MODE ))
		{
			cambot.new_quarter = 1;
			start_enable(FALSE);
			active_players = 0;
			
			// update qtr_pstatus & count active players
			if (p_status & 0x1)
			{
				qtr_pstatus[0] |= 1<<game_info.game_quarter;
				active_players += 1;
			}
			if (p_status & 0x2)
			{
				qtr_pstatus[1] |= 1<<game_info.game_quarter;
				active_players += 1;
			}
			if (p_status & 0x4)
			{
				qtr_pstatus[2] |= 1<<game_info.game_quarter;
				active_players += 1;
			}
			if (p_status & 0x8)
			{
				qtr_pstatus[3] |= 1<<game_info.game_quarter;
				active_players += 1;
			}

			// chalk quarters_played * XXX_PLAYER_AUD
			increment_audit(QUARTERS_PLAYED_AUD);
			increment_audit(ONE_PLAYER_AUD+active_players-1);

			// human vs human?
			hvh = ((p_status & TEAM_MASK(0)) && (p_status & TEAM_MASK(1)));

			// maybe hit two-vs-cpu audit
			if (four_plr_ver && (active_players == 2) && !hvh)
				increment_audit( TWOVCPU_PLAYER_AUD );

			// now update all quarter-dependent percents
			update_quarters_percents();

			switch (game_info.game_quarter)
			{
				case EOQ_1:
				case EOQ_3:
					if(game_info.game_quarter == EOQ_1)
					{
						add_to_audit( ONE_QUARTER_AUD, active_players );
						recalc_percent_aud( ONE_QUARTER_AUD, GAME_START_AUD, ONE_QUARTER_PERCENT_AUD);
					}
					else
					{
						add_to_audit( THREE_QUARTER_AUD, active_players );
						recalc_percent_aud( THREE_QUARTER_AUD, GAME_START_AUD, THREE_QUARTER_PERCENT_AUD);
					}

					paid_credits_deduct();
					eoq_handling();
					coaching_tip_scrn();
					if (buyin_scrn())
						game_over();
					else
						start_new_quarter();
					break;
				case EOQ_2:
					add_to_audit( TWO_QUARTER_AUD, active_players );
					recalc_percent_aud( TWO_QUARTER_AUD, GAME_START_AUD, TWO_QUARTER_PERCENT_AUD);
					paid_credits_deduct();
					eoq_handling();
					// blow out the player uniform textures (do we have to ??)
//					delete_multiple_textures(TXID_PLAYER,0xffffffff);
//					delete_multiple_textures(TXID_FIELD,0xffffffff);
					team_stats_page();
					if (buyin_scrn())
						game_over();
					else
					{
//						load_player_textures();
//						load_field_textures();
//						load_stadium_textures();
						start_new_quarter();
					}
					game_info.off_side = !game_info.first_pos;
					game_info.play_type = PT_KICKOFF;
					half_kickoff = TRUE;
					break;
				case EOQ_4:
				case EOQ_OT1:
				case EOQ_OT2:
				case EOQ_OT3:
					if(game_info.game_quarter == EOQ_4)
					{
						add_to_audit( FOUR_QUARTER_AUD, active_players );
						recalc_percent_aud( FOUR_QUARTER_AUD, GAME_START_AUD, FOUR_QUARTER_PERCENT_AUD);
					}
					else if(game_info.game_quarter == EOQ_OT1)
					{
						add_to_audit( ONE_OVERTIME_AUD, active_players );
						recalc_percent_aud( ONE_OVERTIME_AUD, GAME_START_AUD, ONE_OVERTIME_PERCENT_AUD);
					}
					else if(game_info.game_quarter == EOQ_OT2)
					{
						add_to_audit( TWO_OVERTIME_AUD, active_players );
						recalc_percent_aud( TWO_OVERTIME_AUD, GAME_START_AUD, TWO_OVERTIME_PERCENT_AUD);
					}
					else
					{
						add_to_audit( THREE_OVERTIME_AUD, active_players );
						recalc_percent_aud( THREE_OVERTIME_AUD, GAME_START_AUD, THREE_OVERTIME_PERCENT_AUD);
					}
					paid_credits_deduct();

					// All done if there is a winner or still tied at end of 3rd overtime
					if (check_scores() || game_info.game_quarter == EOQ_OT3)
					{
						// hit the FINISHED_GAMES audit
						increment_audit( FINISHED_GAMES_AUD );

						increment_audit( ONE_PLAYER_FINISH_AUD-1+active_players );
						if (four_plr_ver & (active_players == 2) && !hvh)
							increment_audit( TWOVCPU_PLAYER_FINISH_AUD );

						recalc_percent_aud( ONE_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							ONE_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( TWO_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							TWO_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( THREE_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							THREE_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( FOUR_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							FOUR_PLAYER_FINISH_PERCENT_AUD );
						recalc_percent_aud( TWOVCPU_PLAYER_FINISH_AUD, FINISHED_GAMES_AUD,
							TWOVCPU_PLAYER_FINISH_PERCENT_AUD );

						// set the initials_entered audit
//						for( i = 0; i < (four_plr_ver ? 4 : 2); i++)
//						{
//							if ((plyr_data_ptrs[i]->plyr_name[0] != '-') &&
//								(plyr_data_ptrs[i]->plyr_name[0] != 0) &&
//								(p_status & (1<<i)))
//							{
//								do_percent_aud(INITIALS_ENTERED_AUD, GAME_START_AUD, INITIALS_ENTERED_PERCENT_AUD);
//							}
//						}

						if (hvh)
						{	// humans on both sides
							increment_audit( HVH_FINISH_AUD );
							get_audit(TOTAL_HVSH_POINTS_AUD, &vaud);
							vaud += compute_score(0);
							vaud += compute_score(1);
							set_audit(TOTAL_HVSH_POINTS_AUD, vaud);
							if(!(get_audit(HVH_FINISH_AUD, &gaud)))
							{
								vaud /= gaud;
								set_audit(AVERAGE_HVSH_POINTS_AUD, vaud);
							}

							if(compute_score(0) > compute_score(1))
							{
								do_percent_aud(LEFT_SIDE_WINS_AUD, HVH_FINISH_AUD, LEFT_SIDE_WINS_PERCENT_AUD);
							}

							// Don't audit the tie games
							if(compute_score(0) != compute_score(1))
							{
								if(!(get_audit(TOTAL_WINNING_POINTS_AUD, &vaud)))
								{
									if(compute_score(0) > compute_score(1))
									{
										vaud += compute_score(0);
									}
									else
									{
										vaud += compute_score(1);
									}
									set_audit(TOTAL_WINNING_POINTS_AUD, vaud);
									if(!(get_audit(HVH_FINISH_AUD, &gaud)))
									{
										vaud /= gaud;
										set_audit(AVERAGE_WINNING_SCORE_AUD, vaud);
									}
								}

								if(!(get_audit(TOTAL_LOSING_POINTS_AUD, &vaud)))
								{
									if(compute_score(0) < compute_score(1))
									{
										vaud += compute_score(0);
									}
									else
									{
										vaud += compute_score(1);
									}
									set_audit(TOTAL_LOSING_POINTS_AUD, vaud);
									if(!(get_audit(HVH_FINISH_AUD, &gaud)))
									{
										vaud /= gaud;
										set_audit(AVERAGE_LOSING_SCORE_AUD, vaud);
									}
								}
							}
						}
						else
						{	// human vs cpu
							increment_audit( HVC_FINISH_AUD );
							get_audit(TOTAL_HVSC_POINTS_AUD, &vaud);
							vaud += compute_score(0);
							vaud += compute_score(1);
							set_audit(TOTAL_HVSC_POINTS_AUD, vaud);
							if(!(get_audit(FINISHED_GAMES_AUD, &gaud)))
							{
								vaud /= gaud;
								set_audit(AVERAGE_HVSC_POINTS_AUD, vaud);
							}

							// Don't audit the tie games
							if(compute_score(0) != compute_score(1))
							{
								if (p_status2 & TEAM_MASK(0))
								{
									if(compute_score(0) < compute_score(1))
									{
										do_percent_aud(TOTAL_CPU_VICTORIES_AUD, HVC_FINISH_AUD, CPU_VICTORY_PERCENT_AUD);
										if(!(get_audit(LARGEST_CPU_VICTORY_AUD, &vaud)))
										{
											if(compute_score(1) - compute_score(0) > vaud)
											{
												set_audit(LARGEST_CPU_VICTORY_AUD, compute_score(1) - compute_score(0));
											}
										}
									}
									else
									{
										if(!(get_audit(LARGEST_CPU_LOSS_AUD, &vaud)))
										{
											if(compute_score(0) - compute_score(1) > vaud)
											{
												set_audit(LARGEST_CPU_LOSS_AUD, compute_score(0) - compute_score(1));
											}
										}
									}
								}
								else
								{
									if(compute_score(0) > compute_score(1))
									{
										do_percent_aud(TOTAL_CPU_VICTORIES_AUD, HVC_FINISH_AUD, CPU_VICTORY_PERCENT_AUD);
										if(!(get_audit(LARGEST_CPU_VICTORY_AUD, &vaud)))
										{
											if(compute_score(0) - compute_score(1) > vaud)
											{
												set_audit(LARGEST_CPU_VICTORY_AUD, compute_score(0) - compute_score(1));
											}
										}
									}
									else
									{
										if(!(get_audit(LARGEST_CPU_LOSS_AUD, &vaud)))
										{
											if(compute_score(1) - compute_score(0) > vaud)
											{
												set_audit(LARGEST_CPU_LOSS_AUD, compute_score(1) - compute_score(0));
											}
										}
									}
								}
							}
						}

						recalc_percent_aud( HVH_FINISH_AUD,
											FINISHED_GAMES_AUD,
											HVH_FINISH_PERCENT_AUD );

//$						if((p_status2 & 3) == 3)
//$						{
//$							if(compute_score(0) > compute_score(1))
//$							{
//$								do_percent_aud(LEFT_SIDE_WINS_AUD, TWO_PLAYER_AUD, LEFT_SIDE_WINS_PERCENT_AUD);
//$							}
//$						}

						// Do end-of-quarter stuff & save stats for attract mode display
						eoq_handling();
						game_info_last = game_info;
						tdata_blocks_last[0] = tdata_blocks[0];
						tdata_blocks_last[1] = tdata_blocks[1];
						game_info.game_mode = GM_GAME_OVER;

						// blow out the player uniform textures (do we have to ??)
						delete_multiple_textures(TXID_PLAYER,0xffffffff);
						delete_multiple_textures(TXID_FIELD,0xffffffff);

						// Set record-to-cmos bits
						i = (1 << game_info.game_quarter) - 1;
						for (pnum = 0; pnum < (four_plr_ver ? MAX_PLYRS : 2); pnum++)
						{
							// Set record flag if valid initials
							if ((plyr_data_ptrs[pnum]->plyr_name[0] != 0) &&
								(plyr_data_ptrs[pnum]->plyr_name[0] != '-') &&
								(qtr_pstatus[pnum] == i))
							{
								record_me |= 1 << pnum;
							}
						}

						// Do game finishing screens
						team_stats_page();
						game_over();

						// Grand Champ stuff happens next
						if (do_grand_champ)					//(1)
						{
							show_grand_champ_movie(0, 0);
						}

						// This is where the free stuff is awarded - free stuff is
						// only awarded if the player(s) bought a full game up front

						// Make humans-were-involved mask
						for (pnum = MAX_PLYRS-1, i = 0; pnum >= 0; pnum--)
						{
							i <<= 1;
							if (qtr_pstatus[pnum]) i++;
						}

						// Determine if & who to chk for free stuff
						while (1)
						{
							// Set to chk H vs H scores
							pnum = -1;

							// HH vs HH and full game was bought up front?
							if (!get_adjustment(H4_FREE_GAME_ADJ, &val))
								if (old_game_purchased == 15)
									break;

							// H vs H and full game was bought up front?
							if (!get_adjustment(HH_FREE_GAME_ADJ, &val))
								if ((!four_plr_ver && old_game_purchased == 3) ||
										(old_game_purchased == 5  && i == 5) ||
										(old_game_purchased == 6  && i == 6) ||
										(old_game_purchased == 9  && i == 9) ||
										(old_game_purchased == 10 && i == 10))
									break;

							// H vs CPU and full game was bought up front?
							if (!get_adjustment(HC_FREE_GAME_ADJ, &val))
							{
								// Set to chk team 1 H vs CPU scores
								pnum = 0;
								if (four_plr_ver)
								{
									if (old_game_purchased == 3  && i == 3)
										break;
								}
								else if (old_game_purchased == 1  && i == 1)
									break;

								// Set to chk team 2 H vs CPU scores
								pnum = 1;
								if (!four_plr_ver)
								{
									if (old_game_purchased == 2  && i == 2)
										break;
								}
								else if (old_game_purchased == 12 && i == 12)
									break;
							}

							// No free stuff to chk!
							val = 0;
							break;
						}

						// Do the chk if set
						if (val)
						{
							// Did team 1 beat team 2 & was it allowed ?
							if (compute_score(0) > compute_score(1) && pnum != 1)
							{
								// YES - Give team 1 the free game
								pnum = 0;
							}
							else
							// Did team 2 beat team 1 & was it allowed ?
							if (compute_score(1) > compute_score(0) && pnum != 0)
							{
								// YES - Give team 2 the free game
								pnum = 1;
							}
							else
								// Tie game or not allowed - no free stuff!
								pnum = -1;

							// If at this point pnum is >= 0 it indicates a team
							// is being awarded free stuff; it only gets free stuff
							// if its last game DID NOT have any free stuff.
							if (pnum >= 0)
							{
								if (!free_game_count[pnum])
								{
									// Reset other teams free game count
									free_game_count[pnum ^ 1] = 0;

									val = (four_plr_ver) ?
										((pnum) ? 12 : 3):
										((pnum) ? 2  : 1);

									// Tell the team it has a free game coming
									// He/they MUST press start BEFORE timer on this
									// screen runs out to actually get the free game.
									if (show_player_free_game(old_game_purchased & val))
									{
										// Put the player/team in the game
										p_status = old_game_purchased & val;
										p_status2 = p_status;
										game_purchased = p_status;

										// Increment this teams free game count
										// (except it was a 4 plyr - let them repeat)
										if (old_game_purchased != 15)
											free_game_count[pnum]++;

										// Set game state to pre-game
										game_info.game_state   = GS_PRE_GAME;
										game_info.game_mode    = GM_INITIALIZING;
										game_info.game_quarter = 0;

										// Give team the free stuff
										for (i = 0; i < MAX_PLYRS; i++)
										{
											if (p_status & (1 << i))
											{
												pdata_blocks[i].quarters_purchased = 4;
												// Chalk a free stuff
												increment_audit(FREE_GAME_AUD);
											}
											else
											{
												pdata_blocks[i].quarters_purchased = 0;
											}
										}

										// Start the pre-game screens
										qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 0, 0, 0, 0);
	//									qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 1, 0, 0, 0);

										// Kill myself
										die(0);
									}
								}
							}
						}

						// Game is totally over if it gets here
						free_game_count[0] = 0;
						free_game_count[1] = 0;
						game_info.game_state = GS_GAME_OVER;
					}
					else
					{
						eoq_handling();
						coaching_tip_scrn();
						if (buyin_scrn())
							game_over();
						else
							start_new_quarter();
					}

					if (ot_kickoff == -1)
					{
						ot_kickoff = randrng(2);
					}
					game_info.off_side = !ot_kickoff;
					ot_kickoff = !ot_kickoff;

					game_info.play_type = PT_KICKOFF;
					break;
			}
		}
	}
/*****************************************************************************/
/*****************************************************************************/

//	do_game_over_screens();

	// Create the attract mode loop
	iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
}


//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
static int match_receiver( int pos )
{
	int i, ctrl;

	for( i = PPOS_WPN1; i <= PPOS_WPN3; i++ )
	{
		ctrl = (game_info.team_play[game_info.off_side]->formation + i)->control;
		if((ctrl & LU_POS_MASK) == pos)
			return i;
	}

#ifdef DEBUG
	// OH We are so fucked!
	fprintf( stderr, "bad receiver match.  Check the LU_assignments in the current plays.\n" );
	fprintf( stderr, "\n");
	sleep(1);
	lockup();
#else
	while(1) ;
#endif
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------
//- return TRUE if 2 or more offensive weapons are on the left
static int strong_left( void )
{
	int		i, pos=0,neg=0;

	for( i = 4; i < 7; i++ )
		if( player_blocks[i+7*game_info.off_side].odata.z >= 0.0f )
			pos++;
		else
			neg++;

	if( game_info.off_side )
	{
		if( pos > neg )
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if( pos > neg )
			return FALSE;
		else
			return TRUE;
	}

}
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
static void load_player_textures(void)
{
	struct texture_node *pnode;
	
	if( away_jersey[LESSER(game_info.team[0],game_info.team[1])][GREATER(game_info.team[0],game_info.team[1])] )
	{
		// uniforms too similar, someone wears away jerseys
		load_uniform( 0, game_info.team[0], game_info.home_team ? HA_AWAY : HA_HOME, PC_WHITE );
		chk_game_proc_abort();
		load_uniform( 1, game_info.team[0], game_info.home_team ? HA_AWAY : HA_HOME, PC_BLACK );
		chk_game_proc_abort();
		load_uniform( 2, game_info.team[1], game_info.home_team ? HA_HOME : HA_AWAY, PC_WHITE );
		chk_game_proc_abort();
		load_uniform( 3, game_info.team[1], game_info.home_team ? HA_HOME : HA_AWAY, PC_BLACK );
		chk_game_proc_abort();
	}
	else
	{
		load_uniform( 0, game_info.team[0], HA_HOME, PC_WHITE );
		chk_game_proc_abort();
		load_uniform( 1, game_info.team[0], HA_HOME, PC_BLACK );
		chk_game_proc_abort();
		load_uniform( 2, game_info.team[1], HA_HOME, PC_WHITE );
		chk_game_proc_abort();
		load_uniform( 3, game_info.team[1], HA_HOME, PC_BLACK );
		chk_game_proc_abort();
	}
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------

//extern struct texture_node * ptn_gndarrow;	// for ground arrows

static void eoq_handling(void)
{
	int i;

	announce_eoq_speech();

	qcreate("eoqmsg",EOQ_PID, show_end_quarter_proc, 0, 0, 0, 0 );

	// Wait until message times out
	i = tsec * 10;
	while (existp(EOQ_PID,0xFFFFFFFF) && --i)
		sleep(1);

	if (!i)
#if defined(SEATTLE)
		install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif

	// Turn on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way so we can sync-up the sound
	while (plates_on != PLATES_ON && --i) sleep(1);
	// Do sound only if the transition finished, not the time-out
//	if (i) snd_scall_bank(plyr_bnk_str, P_HARD_HIT2_SND, VOLUME4, 127, PRIORITY_LVL5);
	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME3, 127, PRIORITY_LVL5);

	game_info.game_state = GS_EOQ_BUY_IN;
	game_info.game_quarter++;
	showhide_status_box(0);					// hide status box
	showhide_pinfo(0);							// hide turbo bars

#ifdef USE_DISK_CMOS
	/* update CMOS */
	write_cmos_to_disk(TRUE);
	update_other_cmos_to_disk(TRUE);
#endif

	killall(PLYRINFO_PID,0xffffffff);

	// Delete all plyrinfo/score plate/turbo bar/arrow imgs
	// Then delete the associated textures also!
	// Remember to restart these images upon next quarter start

	killall(STATUS_BOX_PID,0xFFFFFFFF);					// Upper left corner
	killall(SCR_PLATE_PID,0xFFFFFFFF);					// Yardage info stuff

	del1c(OID_PLYRINFO,0xffffffff);
	del1c(OID_SCRPLATE,0xffffffff);
	del1c(OID_INFOBOX ,0xffffffff);
	del1c(OID_RESULTBOX,0xffffffff);
	del1c(OID_TURBARROW,0xffffffff);

	delete_multiple_strings( SID_TLCLOCK, 0xffffffff );
	delete_multiple_strings( SID_TLSCORES, 0xffffffff );
	delete_multiple_strings( SID_TLNAMES, 0xffffffff );

	unlock_multiple_textures(SCR_PLATE_TID,0xFFFFFFFF);
	delete_multiple_textures(SCR_PLATE_TID,0xFFFFFFFF);

	// Tell the gnd arrows we just killed their texture
//	ptn_gndarrow = NULL;

	// Suspend all non-destructable processes
	suspend_multiple_processes(0, 0x8000);
	hide_all_3d_objects();
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
static void start_new_quarter(void)
{
	register int i;

#if DEBUG
//	printf("Beginning of start_new_quarter routine\n");
#endif

	game_info.game_state = GS_GAME_MODE;
	game_info.off_turnover = 0;

	// this might be a bad location for this sound stuff...re-think later.
#ifndef NOAUDIO
//	snd_stop_all();

	coin_sound_holdoff(TRUE);

	// stop and delete eoq-tune
	snd_bank_delete(eoq_tune_bnk_str);

	// load speech bank....
	if (snd_bank_load_playing(get_ancr_str(ancr_swaped_bnk_str)) == 0xeeee)
//	if (snd_bank_load(get_ancr_str(ancr_swaped_bnk_str)) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// load player taunts bank
	if (snd_bank_load_playing(get_taunt_str(taunt_bnk_str)) == 0xeeee)
//	if (snd_bank_load(get_taunt_str(taunt_bnk_str)) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// load next tune
//	if (snd_bank_load_playing(qtr_tunes_tbl[game_info.game_quarter]) == 0xeeee)
//		increment_audit(SND_LOAD_FAIL_AUD);

	coin_sound_holdoff(FALSE);

	// stop drum roll
	snd_stop_all();

	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	
	// re-start main_play tune and audience sounds
//	snd_scall_bank(qtr_tunes_tbl[game_info.game_quarter],0,VOLUME0,127,SND_PRI_SET|SND_PRI_TRACK0);
	snd_scall_bank(game_tune_bnk_str,0,MUSIC_VOL,127,SND_PRI_SET|SND_PRI_TRACK0);
	snd_scall_bank(generic_bnk_str,1,VOLUME1,127,SND_PRI_SET|SND_PRI_TRACK1);
#endif

	// initialize player info displays - we deleted them upon going to buyin screen/coaching tips
	plyrinfo_init();
	// create 'upper-left corner' status box
	qcreate( "score", STATUS_BOX_PID, score_status_box, 0, 0, 0, 0 );

	// Clr player continue flags if not in-game
	i = 0;
	do {
		if (!(p_status & (1<<i)))
			p_status2 &= ~(1<<i);
	} while (++i < MAX_PLYRS);

	showhide_status_box(1);					// hide status box
	showhide_pinfo(1);						// hide turbo bars
	unhide_all_3d_objects();
	hide_multiple_objects( OID_REPLAY, -1 );
	resume_all();

	// Make sure starts are enabled
	start_enable(TRUE);

	// If NOT in attract mode, disable starts after 30 seconds
	if(game_info.game_state != GS_ATTRACT_MODE)
	{
		qcreate("soff", 0, start_off_proc, 0, 0, 0, 0);
	}

	qcreate("gmclock",GAME_CLOCK_PID,game_clock_proc,0,0,0,0);		// restart the clock proc.
	sleep(1);
#if DEBUG

//	printf("End of start_new_quarter routine\n");
#endif
}

//---------------------------------------------------------------------------------------------------------------------------
//						This routine handles getting the game from GAME_STATE to ATTRACT MODE state
//---------------------------------------------------------------------------------------------------------------------------
static void game_over(void)
{
	int i;

	clear_pups();

	bought_in_first = 0;

	show_them_hiscore_tables_first = 0;

	old_game_purchased = game_purchased;
	game_purchased = 0;
	
	game_info.game_state = GS_GAME_OVER;
	
	// save cmos records if end of game and there is a winner!
	if ((game_info.game_quarter > 3 && check_scores()) || game_info.game_quarter == 7)
	{
		save_player_records();
		if (four_plr_ver)
			check_world_records_4p();
		else
			check_world_records_2p();
			
		snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);
		show_final_player_stats();
	}

	// reset state ram....and stuff
	game_info.game_quarter = 0;
	
	// !!!FIX -- do sound bank checksums...if fail...reload sounds
#ifndef NOAUDIO
	// !!!FIX (probably play tune here...or something)
	// Stop curent music
	snd_stop_track(SND_TRACK_0);
	snd_stop_track(SND_TRACK_1);
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);
	
	coin_sound_holdoff(TRUE);

	snd_bank_delete(eoq_tune_bnk_str);
	snd_bank_delete(team2_bnk_str);
	snd_bank_delete(team1_bnk_str);
	snd_bank_delete(game_tune_bnk_str);
	snd_bank_delete(cursor_bnk_str);
	snd_bank_delete("gameover");

	strcpy(cursor_bnk_str, "cursors");
	if (snd_bank_load_playing(cursor_bnk_str) == 0xeeee)
//	if (snd_bank_load(cursor_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
	if (snd_bank_load_playing(intro_bnk_str) == 0xeeee)
//	if (snd_bank_load(intro_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved (SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	// OKAY...at this point...lets be anal and verify that the sounds have been loaded..
	//  if this fails...it resets the board and reloads the PERM. banks
	verify_loaded_snd_banks();

	coin_sound_holdoff(FALSE);
#endif
	
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
static void verify_loaded_snd_banks(void)
{
char * pre_game_bnks[] = {"players","generic","tkperm","cursors","intro", NULL};
int i;

	i = 0;
	while (pre_game_bnks[i] != NULL)
	{
		if (snd_is_bank_loaded(pre_game_bnks[i]) != 1)
		{
			// ERROR...didnt find bank in list of banks loaded...FUCK..reload everything
			snd_board_error_recovery();
			break;
		}
		i++;
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void load_permanent_snds(void)
{
	// Hold off coin sounds while downloading banks
	coin_sound_holdoff(TRUE);

	// Download the banks
//	if (snd_bank_load_playing(plyr_bnk_str) == 0xeeee)
	if (snd_bank_load(plyr_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
//	if (snd_bank_load_playing(generic_bnk_str) == 0xeeee)
	if (snd_bank_load(generic_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
//	if (snd_bank_load_playing(ancr_bnk_str) == 0xeeee)
	if (snd_bank_load(ancr_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
	strcpy(cursor_bnk_str, "cursors");
//	if (snd_bank_load_playing(cursor_bnk_str) == 0xeeee)
	if (snd_bank_load(cursor_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
//	if (snd_bank_load_playing(intro_bnk_str) == 0xeeee)
	if (snd_bank_load(intro_bnk_str) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);

	// Grab the checksum for this bank
//	checksum = snd_checksum_banks(ancr_bnk_str,ancr_bnk_str);

	// Reserve tracks
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);		// trk 0: music, trk 1:audience, trk 5: annoucner
//	snd_set_reserved(SND_TRACK_0|SND_TRACK_5);		// trk 0: music, trk 1:audience, trk 5: annoucner

	// Coin sounds are OK to do now
	coin_sound_holdoff(FALSE);

//	snd_ver_num = snd_get_opsys_ver();
}

//---------------------------------------------------------------------------------------------------------------------------
//			This process controls the GAME CLOCK.
//---------------------------------------------------------------------------------------------------------------------------

// Ticks per second adjustment
static int time_table[5] =
{
	30,						// EXTRA SLOW   APPROX. PERIOD TIME 4:00
	25,						// SLOW				  APPROX. PERIOD TIME 3:30
	20,						// MEDIUM			  APPROX. PERIOD TIME 2:00
	18,						// FAST				  APPROX. PERIOD TIME 2:00
	16						// EXTRA FAST   APPROX. PERIOD TIME 1:45
};

static void game_clock_proc(int *args)
{
 int gt;
 int ticks_per_second;			// from CMOS

	// !!!FIX
	// init the clock to the starting time (from CMOS)
	// actually, the clock will always read the same at the start, but CMOS will dictate its rate of decline
#ifdef NOCLOCK
	game_info.game_time = 0x013B00;
#else
	game_info.game_time = QUARTER_TIME;
#endif

// Put this in to test quick periods, game flow stuff!
#ifdef SHORT_QUARTERS
	game_info.game_time = 0x00000100;			// 1 second
#endif

// Build with -DQ_GAME to make it a quick game
#ifdef Q_GAME
	if (pup_qgame)
	{
		game_info.game_time = 0x00000900;		// 9 seconds
		if (!game_info.game_quarter)
		{
			if (pup_qgame & TEAM1_MASK)
				game_info.scores[0][0] = 3;
			if (pup_qgame & TEAM2_MASK)
				game_info.scores[1][0] = 3;
		}
	}
#endif

	gt = game_info.game_time;

//	clock_active = 1;

	while (gt != 0)
	{
		if (clock_active && game_info.game_state != GS_ATTRACT_MODE)
		{
#ifndef NOCLOCK
			if (gt & 0x000000FF)								// 10ths != 0 ?
				gt -= 0x00000001;									// subtract a 10th
			else
			{																	 	// tenths = 0
				if (gt & 0x0000FF00)					 		// seconds != 0 ?
				{
					gt -= 0x00000100;								// reduce second count
					game_info.play_time++;					// Bump up play time cntr
					tdata_blocks[game_info.off_side].time_of_poss += 1;

					// Less than 30 seconds left in half or game?
					// If so, we slow clock down to real time to allow people more plays
					if (((game_info.game_time & 0x00FFFF00) <= 0x00001f00) && (game_info.game_quarter == 1 || game_info.game_quarter == 3))
					{
						gt |= 0x00000022;							// reset 10ths to real seconds! (57 ticks/second) - or so...

						// Play a warning snd every other clock second
						if (game_info.game_time & 0x00000100)
//							snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME2,127,PRIORITY_LVL3);
							snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME4,127,PRIORITY_LVL5);
					}
					// Has player burned too much time on this play?  If so, slow down the clock to real time
					else if (game_info.play_time >= 18)
						gt |= 0x00000030;							// reset 10ths to real seconds! (57 ticks/second)
					else
					{
						// This is getting CMOS adjustment every second - put into an int
						if (get_adjustment(TIME_ADJ,&ticks_per_second))			// Returns non zero if an error
							ticks_per_second = -1;

						if (ticks_per_second < 0 || ticks_per_second >= (int)(sizeof(time_table)/sizeof(int)))
						{
#if DEBUG
	// If CMOS is bad, we should default to a reasonable value anyway!
	fprintf( stderr, "Bogus CMOS tick count\n" );
//	lockup();
#endif
							fprintf( stderr, "Bogus CMOS tick count: %d\n", ticks_per_second );	//!!!!!!
							ticks_per_second = 3;					// Default for bad CMOS read
						}

	//					fprintf( stderr, "CMOS tick count %d\n",ticks_per_second );
						gt |= time_table[ticks_per_second];					// reset 10ths = about 3 minutes per period - 75 cents
					}
				}
				else															// seconds = 0
				{
					if (gt & 0x00FF0000)						// minutes != 0 ?
					{
						game_info.play_time++;					// Bump up play time cntr
						tdata_blocks[game_info.off_side].time_of_poss += 1;
						gt -= 0x00010000;							// reduce minutes
						gt |= 0x00003B00;							// reset seconds (59 seconds)
						gt |= 0x00000018;							// reset 10ths
					}
					else
					{
						gt = 0;
						snd_scall_bank(plyr_bnk_str,GUN_SND,VOLUME4,127,PRIORITY_LVL5);
						sleep(30);
						snd_scall_bank(plyr_bnk_str,GUN_SND,VOLUME4,127,PRIORITY_LVL5);
						sleep(30);
						snd_scall_bank(plyr_bnk_str,GUN_SND,VOLUME4,127,PRIORITY_LVL5);
						die(0);										// PERIOD OVER!!
					}
				}
			}
			game_info.game_time = gt;
#endif //NOCLOCK
		}
	sleep(1);
	}
}

#ifdef BOG_METER
//////////////////////////////////////////////////////////////////////////////
static unsigned int proc_start, proc_end;
static float ptime[15],ftime[15];
static int ptcount[15];

#if 0
//////////////////////////////////////////////////////////////////////////////
static void bog_display( void )
{
	float		frate,prate;
	static int	ccount = 0;
	int			i;
	int			onscreen=0;	// used to be global

	if (((ccount%2048) < 10 ) && (ccount % 2048))
	{
		ccount++;
		return;
	}

	ptime[onscreen] += ((float)(proc_end-proc_start) / 1000000.0f);
	ftime[onscreen] += ((float)mthread_get_framerate() / 1000000.0f);
	ptcount[onscreen]++;

	if(ccount++ % 512)
		return;

	for( i = 0; i < 15; i++ )
	{
		fprintf( stderr, "%d.\t", i );
		if( ptcount[i] )
		{
			frate = ftime[i] / (float)ptcount[i];
			prate = ptime[i] / (float)ptcount[i];

			fprintf( stderr, "%.2f\t%.2f\n", prate, frate );
		}
		else
		{
			fprintf( stderr, "0\t0\n" );
		}

		ptime[i] = 0.0f;
		ftime[i] = 0.0f;
		ptcount[i] = 0;
	}
}
#else
//////////////////////////////////////////////////////////////////////////////
static void bog_display( void )
{
	float		frate,prate;
	static int	ccount = 0;

	ccount++;// = (ccount + 1) % 5;

	if( ccount & 1 )
		return;

	delete_multiple_strings( 0xdeadbeef, 0xffffffff );

	frate = (float)mthread_get_framerate() / 1000000.0f;
	prate = (float)(proc_end-proc_start) / 1000000.0f;

	set_string_id( 0xdeadbeef );
	set_text_font(FONTID_BAST10);
	set_text_position( 60.0f, 72.0f, 1.0f );
	oprintf("%dj%dc%.2f",
		(HJUST_CENTER|VJUST_CENTER),
		(prate > 17.5f) ? LT_RED : LT_YELLOW,
		prate);
//	set_text_position( 60.0f, 61.0f, 1.0f );
//	oprintf("%dj%dc%.2f",
//		(HJUST_CENTER|VJUST_CENTER),
//		((frate-prate) > 17.5f) ? LT_RED : LT_YELLOW,
//		frate-prate);
	set_text_position( 60.0f, 50.0f, 1.0f );
	oprintf("%dj%dc%.2f",
		(HJUST_CENTER|VJUST_CENTER),
		(frate > 17.5f) ? LT_RED : LT_YELLOW,
		frate);

	if ((frate > 17.5f) && (game_info.game_mode == GM_IN_PLAY))
	{
		fprintf( stderr, "frate: %.2f\nprate: %.2f\n", frate, prate );
		getchar();
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
static void start_pclock( void )
{
   	_ioctl (5, FIOCGETTIMER3, (int)&proc_start);
}

//////////////////////////////////////////////////////////////////////////////
static void end_pclock( void )
{
   	_ioctl (5, FIOCGETTIMER3, (int)&proc_end);
}

//////////////////////////////////////////////////////////////////////////////
static void init_bog_monitor( void )
{
	pre_proc_func( start_pclock );
	post_proc_func( bog_display );
	post_proc_func( end_pclock );
}

//////////////////////////////////////////////////////////////////////////////
#endif //BOG_METER


//////////////////////////////////////////////////////////////////////////////
static int cpu_do_onside( void )
{
	int		minute_or_less, losing_by;

	// first off, 3% of the time, just do an onside kick for the hell of it
	if (!(rand() % 33))
		return TRUE;

	// 20% of the time, when you might otherwise do an onside kick, don't
	if (!(rand() % 5))
		return FALSE;

	// do an onside kick if we're losing and there's less than a minute to
	// play in 4th or overtime.  OR if we're down by more than 7 any time
	// in the 4th or later.
	if (game_info.game_quarter < 3)
		return FALSE;

	minute_or_less = ((game_info.game_time < 0x3c00));

	losing_by = compute_score(!game_info.off_side) -
				compute_score(game_info.off_side);

	if ((minute_or_less) && (losing_by > 0))
		return TRUE;

	if (losing_by > 7)
		return TRUE;

	return FALSE;
}

static void reinit_offense_players(void)
{
	int		std_offense[] = {0,1,2,3,4,5,6};
	int		kickoff[] = {14,10,7,11,12,8,13};
	int		kick_return1[] = {0,1,2,4,5,6,7};
	int		kick_return2[] = {0,1,2,5,4,6,7};
	int		kick_return3[] = {0,1,2,6,5,4,7};
	int		*returns[] = {kick_return1, kick_return2, kick_return3};
	int		returners[30] = {0,0,0,0,0,0,0,2,0,0,
							 0,0,0,0,0,0,0,0,0,0,
							 0,0,0,0,0,0,0,0,0,0};
	int		punt[] = {0,1,2,14,4,5,6};
	int		fgoal[] = {0,1,2,3,4,5,14};
	int		otbase, offbase;
	int		i;
	int		*offroster;
	play_t	*offplay;
		
	offroster = std_offense;
	offplay = game_info.team_play[game_info.off_side];
	
	if ((offplay == &play_punt) ||
		(offplay == &play_punt))
		offroster = punt;
	else if ((offplay == &play_field_goal) ||
		(offplay == &play_fake_field_goal))
		offroster = fgoal;
	else if ((offplay == &play_kickoff_313) ||
		(offplay == &play_kickoff2_313) ||
		(offplay == &play_onside_313))
	{
		offroster = kickoff;
	}
	else if (offplay == &play_kick_return_short)
	{
		offroster = returns[returners[game_info.team[game_info.off_side]]];
	}
	
	otbase = game_info.team[game_info.off_side] * NUM_PERTEAM_SPD;
	offbase = 7 * game_info.off_side;
	
	for (i=0; i < 7; i++)
		reinit_player( offbase + i, otbase + offroster[i] );
}

static void reinit_defense_players(void)
{
	int		std_defense[] = {7,8,9,10,11,12,13};
	int		kickoff[] = {14,10,7,11,12,8,13};
	int		kick_return1[] = {0,1,2,4,5,6,7};
	int		kick_return2[] = {0,1,2,5,4,6,7};
	int		kick_return3[] = {0,1,2,6,5,4,7};
	int		*returns[] = {kick_return1, kick_return2, kick_return3};
	int		returners[30] = {0,0,0,0,0,0,0,2,0,0,
							 0,0,0,0,0,0,0,0,0,0,
							 0,0,0,0,0,0,0,0,0,0};
	int		punt[] = {0,1,2,14,4,5,6};
	int		fgoal[] = {0,1,2,3,4,5,14};
	int		otbase, dtbase, offbase, defbase;
	int		i;
	int		*offroster, *defroster;
	play_t	*offplay;
	
	
	defroster = std_defense;
	offplay = game_info.team_play[game_info.off_side];
	
	if ((offplay == &play_punt) ||
		(offplay == &play_punt))
	{
	//	offroster = punt;
	}
	else if ((offplay == &play_field_goal) ||
		(offplay == &play_fake_field_goal))
	{
	//	offroster = fgoal;
	}
	else if ((offplay == &play_kickoff_313) ||
		(offplay == &play_kickoff2_313) ||
		(offplay == &play_onside_313))
	{
		defroster = returns[returners[game_info.team[!game_info.off_side]]];
	}
	else if (offplay == &play_kick_return_short)
	{
		defroster = kickoff;
	}
	
	dtbase = game_info.team[!game_info.off_side] * NUM_PERTEAM_SPD;
	defbase = 7 * !game_info.off_side;
	
	for (i=0; i < 7; i++)
		reinit_player( defbase + i, dtbase + defroster[i] );
}


//////////////////////////////////////////////////////////////////////////////
//  side < 0 to do all players
//  side 0 or 1 to do that team only
//
static void lineup_players( int side )
{
	static float base_speeds[] = {
		0.0f, 0.0f, LINE_SPEED, LINE_SPEED,
		QB_SPEED, QB_SPEED, WPN_SPEED, WPN_SPEED,
		DB_SPEED, DB_SPEED, WPN_SPEED
	};
	static float base_turbo_speeds[] = {
		0.0f, 0.0f, LINE_TURBO_SPEED, LINE_TURBO_SPEED,
		QB_TURBO_SPEED, QB_TURBO_SPEED, WPN_TURBO_SPEED, WPN_TURBO_SPEED,
		DB_TURBO_SPEED, DB_TURBO_SPEED, WPN_TURBO_SPEED
	};

	fbplyr_data *	ppdata;
	fline_t *		pform;

	int i = (side <= 0 ?           0 : NUM_PERTEAM);
	int j = (side == 0 ? NUM_PERTEAM : NUM_PLAYERS);
	int target;

	clear_random_control_flags();

	// do stuff for everyone
	for( ; i < j; i++ )
	{
		ppdata = player_blocks + i;

		ppdata->odata.flags &= ~(PF_NOCOLL|PF_CELEBRATE|PF_REVWPN|PF_LATEHITABLE|PF_POP_HELMET|PF_TURBO);
		ppdata->odata.flags &= ~(PF_SPEED_OVERRIDE|PF_DIZZY|PF_FLASHWHITE|PF_UNBLOCKABLE|PF_DRAGGING_PLR);
		ppdata->odata.flags &= ~(PF_OLD_TURBO|PF_BACKPEDAL|PF_THROWING|PF_PEND_FCHECK|PF_FLASH_RED|PF_WAVE_ARM);
		ppdata->odata.fwd = ppdata->team ? 768 : 256;
		ppdata->odata.tgt_fwd = ppdata->odata.fwd;
		ppdata->odata.ax = 0.0f;
		ppdata->odata.az = 0.0f;
		ppdata->odata.vx = 0.0f;
		ppdata->odata.vy = 0.0f;
		ppdata->odata.vz = 0.0f;
		ppdata->odata.adata[0].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);
		ppdata->odata.adata[1].animode &= ~(MODE_PUPPET|MODE_PUPPETEER);
		ppdata->odata.ascale = ppdata->odata.default_ascale;

		ppdata->attack_time = 0;
		ppdata->attack_mode = 0;
		ppdata->celebrate_time = 0;
		ppdata->stick_cur = 24;
		ppdata->but_cur = 0;
		ppdata->job = JOB_NONE;
		ppdata->puppet_link = NULL;
		ppdata->block_count = 0;
		ppdata->no_catch_time = 0;
		ppdata->speed = 0.0f;
		ppdata->override_time = 0;
		ppdata->fake_zone_time = 0;
	}

	if(( side < 0 ) || ( side == game_info.off_side ))
	{
		// line up offense
		for( i = 0; i < NUM_PERTEAM; i++ )
		{
			pform = game_info.team_play[game_info.off_side]->formation + i;
			ppdata = player_blocks + i + game_info.off_side * NUM_PERTEAM;

			ppdata->odata.x = WORLDX(game_info.los) + pform->x * (ppdata->team ? -1.0f : 1.0f);
			if (ppdata->odata.x > (FIELDHB-YD2UN))
				ppdata->odata.x = (FIELDHB-YD2UN);
			if (ppdata->odata.x < -(FIELDHB-YD2UN))
				ppdata->odata.x = -(FIELDHB-YD2UN);

			ppdata->odata.z = game_info.line_pos + pform->z * (game_info.off_side ? -1.0f : 1.0f)
					* (game_info.off_flip ? -1.0f : 1.0f);

			change_anim( &ppdata->odata, pform->seq );

			ppdata->odata.y = 0.0f;

			ppdata->odata.plyrmode = pform->mode;
		}
	}

// Always do defense
//	if(( side < 0 ) || ( side == !game_info.off_side ))
	{
		// line up defense
		for( i = 0; i < NUM_PERTEAM; i++ )
		{
			pform = game_info.team_play[!game_info.off_side]->formation + i;
			ppdata = player_blocks + i + !game_info.off_side * NUM_PERTEAM;

			// New conditional to allow redoing man-to-man defenders during an offensive lineup
			if(( side < 0 ) || ( side == !game_info.off_side ) ||
				( pform->control & LU_POS_MASK ) || ( pform->control & LU_WEAK ))
			{
				ppdata->odata.x = WORLDX(game_info.los) + pform->x * (ppdata->team ? -1.0f : 1.0f);
				if (ppdata->odata.x > (FIELDHB-YD2UN))
					ppdata->odata.x = (FIELDHB-YD2UN);
				if (ppdata->odata.x < -(FIELDHB-YD2UN))
					ppdata->odata.x = -(FIELDHB-YD2UN);

				ppdata->odata.z = game_info.line_pos + pform->z * (game_info.off_side ? -1.0f : 1.0f);

				if( pform->control & LU_POS_MASK )
				{
					target = match_receiver( pform->control & LU_POS_MASK );

					if( !( pform->control & LU_COVER ))
						ppdata->odata.z = player_blocks[target + NUM_PERTEAM * game_info.off_side].odata.z;

					drone_cover_assign( ppdata->plyrnum, target + NUM_PERTEAM * game_info.off_side );
				}

				if( pform->control & LU_WEAK )
				{
					if( strong_left() )
						ppdata->odata.z = game_info.line_pos - pform->z * (game_info.off_side ? -1.0f : 1.0f);
				}

				ppdata->odata.y = 0.0f;

				ppdata->odata.plyrmode = pform->mode;

				change_anim( &ppdata->odata, pform->seq );
			}
		}
	}

	almost_fire_pflash = 0;			// didnt signal man gonna-be-on-fire
	fire_pflash = 0;
	db_pflash = 0;
	qb_evaded = 0;
}

//-------------------------------------------------------------------------------------------------
// This routine is called at the end of every QUARTER. It updates the players purchased info.
// Clears players' <please_wait>.
//
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
static void paid_credits_deduct(void)
{
	register pdata_t * pdata;
	register int i = 0;

	do
	{
		pdata = pdata_blocks + i;

		// Is player in game?
		if(p_status & (1<<i))
		{
			// YES - decrement quarters purchased cnt (should always happen)
			if(pdata->quarters_purchased)
			{
				pdata->quarters_purchased--;
			}

			// Does this player have any quarters left ?
			if(pdata->quarters_purchased <= 0)
			{
				// NOPE - Take him out of the game
				p_status &= ~(1<<i);
				pdata->credits_paid = 0;
			}
		}
		pdata->please_wait = 0;
	} while(++i < MAX_PLYRS);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void snd_board_error_recovery(void)
{
	// Initialize the sound system
	if(snd_init() != 0)
	{
		printf ("\n");
		printf ("main_init_sound(): ERROR initializing sound system.\n");
	}

	// Initialize the sound bank managment goop
	snd_bank_init();

	// Load permanent sounds
	load_permanent_snds();
}


//////////////////////////////////////////////////////////////////////////////
/* nfl.c ********************************************************************/
