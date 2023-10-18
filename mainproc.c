#if defined(VEGAS)

#define USE_DISK_CMOS
void update_other_cmos_to_disk(int sleep_allowed);

#endif

/****************************************************************************/
/*                                                                          */
/* main.c - Main module for the game.                                       */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* $Revision: 270 $                                                         */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Right Reserved                                                       */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <goose/goose.h>
#include <string.h>

#define	REAL_TEST
//#define NO_STARTS

#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/coin.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/gameadj.h"
#include "/Video/Nfl/Include/ani3d.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/movie.h"
#include "/Video/Nfl/Include/profile.h"
#include "/Video/Nfl/Anim/genseq.h"
#include "/Video/Nfl/Include/game_sel.h"
#include "/Video/Nfl/Include/Attract.h"
#include "/Video/Nfl/Include/Nfl.h"
#include "/Video/Nfl/Include/MainProc.h"
#include "/video/nfl/include/game_sel.h"

#ifdef USE_DISK_CMOS
#include "/video/diag/dbcmos.c"
#endif

int set_coin_hold(int flag);
void dump_last_pass( int, int );
//void srmain_test_sound (void);
void show_pic_info( void );
void show_sound_version( void );
void check_files( void );
void get_resolution( float *, float * );
void plyr_strt_butn( int sig, int swid );
static void dump_tlist( int sig, int swid );
static void inc_pcount( void );
void uptime_proc( int * );
void start_diags( int * );
void do_test( int, int );
void main_proc(int *);
int paid_credits_add( int, int );
int do_start_press( int, int );
void do_file_check(void);
void start_enable(int);
void no_start(int, int);
int get_start_state(void);
void backdoor_proc(int *);
#if defined(SEATTLE)
static void dump_audits( int, int );
void dump_stack_trace( void );
#endif

#if defined(VEGAS)
unsigned long long get_timer_val(void);
#endif

extern int texture_load_in_progress;
extern int	credit_pg_on;
#if defined(SEATTLE)
extern int tick_counter;
#endif
extern texture_node_t	*tlist;
extern int is_dip_coinage;
extern void credit_page_proc( int *args);
extern void grab_install_proc(void);

extern short screen;
extern int reload;
extern int p_active;

#if defined(VEGAS)
unsigned long long	get_elapsed_time(void);
#endif

struct process_node *clicker_proc;
struct process_node *volume_proc;
struct process_node *coindrop_proc;

int sysfont_height;
int	snd_ver_num;
int start_state = FALSE;

//extern unsigned int tick_sndn;
extern unsigned int coin_snd_drop_calln;
extern unsigned int coin_snd_paid_calln;

int check_all_audits(void);

#if defined(SEATTLE)
extern int	uptime;
#endif

#ifdef DBG_HEAP
/*****************************************************************************/
int	_write(int, void *, int);
void * heap_walk(void * packet, unsigned int * owner, unsigned int * size,
								unsigned int * used, unsigned int * next);
int textify(char * sz, unsigned int val, int base10, int zeros);
unsigned int get_heap_count(void);

void do_heap_dump_handler(int sw_state, int sw_id)
{
	if (sw_state == SWITCH_CLOSE)
	{
		char * psz;
		void * packet1 = NULL;
		void * packet2 = NULL;
		void * packet3 = NULL;
		void * packet_next;
		char cbuf[200];
		unsigned int owner;
		unsigned int size;
		unsigned int used;
		unsigned int next;
		unsigned int count;
		unsigned int column1;
		unsigned int column2 = 0;
		unsigned int column3 = 0;
		int len;

		count = get_heap_count();
		if (count)
		{
			packet1 = heap_walk(NULL, NULL, NULL, NULL, NULL);
			column1 = count / 3;
			if (column1)
			{
				psz = " ADDRESS    OWNER    SIZE USE     ADDRESS    OWNER    SIZE USE     ADDRESS    OWNER    SIZE USE\r\n";

				column3 = column1;
				column2 = column1 + !!((count % 3) > 1);
				column1 = column1 + !!((count % 3) > 0);

				packet2 = packet1;
				len = column1;
				while (len--)
					packet2 = heap_walk(packet2, NULL, NULL, NULL, NULL);

				packet3 = packet2;
				len = column2;
				while (len--)
					packet3 = heap_walk(packet3, NULL, NULL, NULL, NULL);
			}
			else
			{
				psz = " ADDRESS    OWNER    SIZE USE\r\n";
				column1 = count;
			}

			_write(1, "\r\n", 2);
			_write(1, "System Heap:\r\n", 14);
			_write(1, psz, strlen(psz));

			psz = "$$$$$$$$ $$$$$$$$ $$$$$$$ YES";

			while (column1)
			{
				strcpy(cbuf, psz);

				packet_next = heap_walk(packet1, &owner, &size, &used, &next);

				len =
				textify(cbuf, (unsigned int)size,    1, 0);
				textify(cbuf, (unsigned int)owner,   0, 1);
				textify(cbuf, (unsigned int)packet1, 0, 1);

				if (!used)
				{
					cbuf[len-3] = ' ';
					cbuf[len-2] = '-';
					cbuf[len-1] = ' ';
				}

				packet1 = packet_next;
				column1--;

				if (column2)
				{
					strcat(cbuf, "    ");
					strcat(cbuf, psz);

					packet_next = heap_walk(packet2, &owner, &size, &used, &next);

					len =
					textify(cbuf, (unsigned int)size,    1, 0);
					textify(cbuf, (unsigned int)owner,   0, 1);
					textify(cbuf, (unsigned int)packet2, 0, 1);

					if (!used)
					{
						cbuf[len-3] = ' ';
						cbuf[len-2] = '-';
						cbuf[len-1] = ' ';
					}

					packet2 = packet_next;
					column2--;
				}

				if (column3)
				{
					strcat(cbuf, "    ");
					strcat(cbuf, psz);

					packet_next = heap_walk(packet3, &owner, &size, &used, &next);

					len =
					textify(cbuf, (unsigned int)size,    1, 0);
					textify(cbuf, (unsigned int)owner,   0, 1);
					textify(cbuf, (unsigned int)packet3, 0, 1);

					if (!used)
					{
						cbuf[len-3] = ' ';
						cbuf[len-2] = '-';
						cbuf[len-1] = ' ';
					}

					packet3 = packet_next;
					column3--;
				}

				strcat(cbuf, "\r\n");

				_write(1, cbuf, strlen(cbuf));
			}

			strcpy(cbuf, "Total # of Packets: $$$$$\r\n");
			textify(cbuf, (unsigned int)count, 1, 0);

			_write(1, cbuf, strlen(cbuf));
			_write(1, "\r\n", 2);
		}
		else
		{
			_write(1, "\r\n", 2);
			_write(1, "Corrupted System Heap!\r\n", 24);
		}
	}
}
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void uptime_proc(int *args)
{
#if defined(SEATTLE)
	int	val;
	int	pval;

	if(get_audit(UPTIME_AUDIT_NUM, &uptime))
	{
		uptime = 0;
	}

	while(1)
	{
		sleep(300);

		set_audit(UPTIME_AUDIT_NUM, uptime);
		if(game_info.game_state != GS_ATTRACT_MODE)
		{
			get_audit(PLAYTIME_AUD, &pval);
			if(pval >= 0)
			{
				pval += 5;
				set_audit(PLAYTIME_AUD, pval);
			}
			else
			{
				pval = 0;
				set_audit(PLAYTIME_AUD, pval);
			}
		}
		recalc_percent_aud(PLAYTIME_AUD, UPTIME_AUDIT_NUM, PLAYTIME_PERCENT_AUD);
	}
#elif defined(VEGAS)
	int				play_time;
	int				uptime;
	int				start_uptime;
	unsigned int	start_time = (unsigned int)get_elapsed_time();
	
	if(get_audit(UPTIME_AUDIT_NUM, &start_uptime) || (start_uptime < 0))
	{
		start_uptime = 0;
	}
	for (;;) {
		sleep(get_tsec() * 5);
		
		uptime = (unsigned int)get_elapsed_time() - start_time;
		uptime += start_uptime;

		/* uptime gets incremented every second in the inthandler */
		set_audit(UPTIME_AUDIT_NUM, uptime);
		if (game_info.game_state != GS_ATTRACT_MODE) {
			if (get_audit(PLAYTIME_AUD, &play_time) || (play_time < 0))
				play_time = 0;
			else
				play_time += 5;
			set_audit(PLAYTIME_AUD, play_time);
		}
		recalc_percent_aud(PLAYTIME_AUD, UPTIME_AUDIT_NUM, PLAYTIME_PERCENT_AUD);
	}
#endif
}

void start_diags(int *args)
{
#ifdef REAL_TEST
	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > 1)
	{
		movie_abort();
	}

	// For whatever reason, starting up the diags while the game is
	// initializing is bad!!  So -- don't do it.
	while (game_info.game_mode == GM_INITIALIZING) sleep(1);

	// Make sure disk stuff is NOT installed
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif

#ifdef USE_DISK_CMOS
	/* write out the CMOS before we shut down */
	write_cmos_to_disk(FALSE);
	update_other_cmos_to_disk(FALSE);
#endif
	
	exec("diag.exe", 0xcafebeef, args);
#endif
}

void do_test(int sig, int id)
{
	if(sig == SWITCH_OPEN)
	{
		return;
	}
	qcreate("diags", 0, start_diags, 0, 0, 0, 0);
}


#if 0
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: srmain_test_sound()                                             */
/*                                                                           */
/* Initializes the sound system.                                             */
/*                                                                           */
/* Downloads the engine sounds from disk to the sound system.                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 01 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/

void srmain_test_sound (void)

{ /* srmain_test_sound() */

#define OK 0

unsigned int snd_version;
int i;
int scall;
int sleep_time;
int num_calls;
char bank_name[2];

/*****************************************************************************/

//	#if SRMAIN_DBG
		printf ("\n");
		printf ("srmain_test_sound(): begin\n");
//	#endif

	printf ("\n");
	printf ("calling snd_init_multi()...\n");

	if (snd_init_multi(SND_OPSYS_0223) != OK)
		printf (" ERROR\n");
	else
		printf (" OK\n");

	snd_master_volume (50);

	while (1)
		{
		printf ("\n");
		printf ("calling snd_get_opsys_ver()...\n");
		snd_version = snd_get_opsys_ver();
		printf (" version: %04X\n", snd_version);

		printf ("calling snd_bank_init()...\n");
		snd_bank_init();

		printf ("loading groove1.bnk normal...\n");
		//snd_bank_load ("groove1");
		snd_bank_load ("generic");
		printf (" done\n");


		printf ("delay 10 ticks...\n");
		sleep (10);

		printf ("starting music playing...\n");
   	    //snd_scall_bank ("groove1", 0, 130, 127, 0);
		snd_set_reserved (SND_TRACK_0|SND_TRACK_1);
   	    snd_scall_bank ("generic", 18, 170, 127, SND_PRI_SET|SND_PRI_TRACK0);
		printf (" done\n");

		printf ("delay 10 ticks...\n");
		sleep (10);

		strcpy (bank_name, "intro");

		printf ("loading %s while playing...\n", bank_name);
		// this errors out...
		snd_bank_load_playing (bank_name);
		// ... but changing this to normal works OK
		//snd_bank_load ("cursors");
		printf (" done\n");

		printf ("delay 10 ticks...\n");
		sleep (10);

		printf ("loading mainfx.bnk while playing...\n");
		snd_bank_load_playing ("cursors");
		printf (" done\n");

		num_calls = 25;

		printf ("entering loop of %d sound calls...\n", num_calls);
		for (i=0; i < num_calls; i++)
			{
			scall = randrng(11);
			sleep_time = randrng(20) + 1;
 			printf (" sound call %02d of %02d: %d (sleep %d)\n", 
 			        i, num_calls, scall, sleep_time);
	   	    snd_scall_bank (bank_name, scall, 80, 127, 0);
			sleep (sleep_time);
			}

		} /* while */

//	#if SRMAIN_DBG
		printf ("\n");
		printf ("srmain_test_sound(): done\n");
//	#endif

} /* srmain_test_sound() */

/***** END of srmain_test_sound() ********************************************/
#endif //0


//-------------------------------------------------------------------------------------------------
// MAIN_PROC
//-------------------------------------------------------------------------------------------------
#define	CMOS_ADJ_RESTORED	1
#define	CMOS_DEAD			2

#define	N64CARD_AUDITED_ID	0xcafebeef

int	cmos_status = 0;

void main_proc(int *args)
{
	float	y_pos;
	int		val;
	int		failed;
	union
	{
		dip_inputs_t	di;
		unsigned int	val;
	} dip_in;

	dip_in.val = read_dip_direct();
	dip_in.val <<= DIP_SHIFT;
	dip_in.val ^= (0xffff << DIP_SHIFT);

	/* Set the background color */
	set_bgnd_color(0xff000000);

	// enable tex and sound CRCs
#if defined(SEATTLE)
	#ifndef NO_CRCS
	guTexUseCRC();
	snd_crc_check_enable();
	#endif
#endif

	// Load the fonts
	main_load_fonts();
//	main_init_sound();

	// start bog-dependent tick counter
	pcount = 0;
	pre_proc_func(inc_pcount);

#if defined(VEGAS)
	// put somethin' up if we are reloading
	if (reload)
	{
		draw_enable(0);
		display_loading(NFL_RUNNING);
		draw_enable(1);
		black_screen();
		sleep(2);
		fade(1.0, 30, NULL);
		sleep(30);
		draw_enable(0);	
	}
#endif


	/* Get the system font height */
	sysfont_height = get_font_height(FONTID_BAST10);
	sysfont_height += 4;

	/* Select the font and preset a couple of text items */
	set_text_font(FONTID_BAST10);
	set_text_color(WHITE);
	set_string_id(3);

	y_pos = SPRITE_VRES * 0.8F;

#if defined(VEGAS)
	if (!reload)
#endif
	{
		/* Display the game name and version */

		set_text_position(SPRITE_HRES/2.0F, y_pos, 1.0F);
		set_string_id(2);
		set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);

		y_pos -= sysfont_height;
		set_text_color(LT_YELLOW);
		set_text_position(SPRITE_HRES/2.0F, y_pos, 1.0F);
		oprintf("Version: %s", game_version);

		y_pos -= sysfont_height;
		set_text_position(SPRITE_HRES/2.0F, y_pos, 1.0F);
		oprintf("BUILD DATE: %s", __DATE__);

		y_pos -= sysfont_height;
		set_text_position(SPRITE_HRES/2.0F, y_pos, 1.0F);
		oprintf("BUILD TIME: %s", __TIME__);
	}

	/* Get and display the serial number and dom from the PIC */
	show_pic_info();

	/* restore the CMOS disk shadow */
#ifdef USE_DISK_CMOS
	read_cmos_from_disk(FALSE);
#endif
	if(!init_cmos())
	{
		// Is NEXT_AUD valid ?
		if((get_audit(NEXT_AUD, &val)) ||
			val != NEXT_AUD)
		{
			// NO - Clear all of the audits
			clear_all_audits();
		}
		// Set the value of NEXT_AUD
		set_audit(NEXT_AUD, NEXT_AUD);

#if 0
		// Is N64CARD_AUDITED_AUD valid ?
		if((get_audit(N64CARD_AUDITED_AUD, &val)) ||
			val != (int)N64CARD_AUDITED_ID)
		{
			// NO - Reset all of the N64CARD audits
			set_audit(N64CARD_PLUGINS_AUD, 0);
			set_audit(N64CARD_BLITZED_AUD, 0);
			set_audit(N64CARD_PLAY_OK_AUD, 0);
			set_audit(N64CARD_PLAY_BD_AUD, 0);
		}
		// Set the value of N64CARD_AUDITED_AUD
		set_audit(N64CARD_AUDITED_AUD, N64CARD_AUDITED_ID);
#endif

		// Check the adjustments - errors detected ?
		if(check_all_adjustments())
		{
			// YES - restore all adjustments to factory defaults
			restore_all_adjustments();
			if(check_all_adjustments())
			{
				cmos_status = CMOS_DEAD;
			}
			else
			{
				cmos_status = CMOS_ADJ_RESTORED;
			}
		}

		// Check the player records
		if (validate_plyr_records() && !cmos_status)
			cmos_status = CMOS_ADJ_RESTORED;

		// Should a restore to factory setting be done ?
		if((get_adjustment(FACTORY_RESTORE_ADJ, &val)) ||
			val)
		{
			// Adjustment failed or said to do the restore
//fprintf(stderr, "here\n");
			cmos_status = CMOS_ADJ_RESTORED;
			set_default_plyr_records();
			set_default_world_records();
			set_adjustment(FACTORY_RESTORE_ADJ, 0);
			clear_all_audits();
		}

		if(!get_adjustment(CLEAR_RECORDS_ADJ, &val))
		{
			if(val)
			{
				set_default_plyr_records();
				set_default_world_records();
				set_adjustment(CLEAR_RECORDS_ADJ, 0);
			}
		}

		if(!check_all_audits())
		{
			cmos_status = CMOS_DEAD;
		}
		else if(check_all_adjustments())
		{
			cmos_status = CMOS_DEAD;
		}
		else if(validate_plyr_records())
		{
			cmos_status = CMOS_DEAD;
		}

		if(cmos_status != CMOS_DEAD)
		{
			// Check the audits
			check_all_audits();

			// Create the uptime process
			qcreate("utime", 0x8000, uptime_proc, 0, 0, 0, 0);

			// Get the violence adjustment
			if(get_adjustment(VIOLENCE_ADJ, &violence))
			{
				violence = 0;
			}

			// Get the swearing adjustment
//			if(get_adjustment(SWEAR_ADJ, &swearing))
//			{
//				swearing = 0;
//			}
		}
	}
	else
	{
		fprintf(stderr, "Error occured in trying to INIT. cmos\r\n");
		cmos_status = CMOS_DEAD;
	}

	y_pos -= ((sysfont_height * 4.0f) * (384.0f / vres));
	set_text_position(SPRITE_HRES/2.0F, y_pos, 1.0F);

#ifdef GRAB_SCREEN
	grab_install_proc();
#endif

	// This was moved here to allow is_dip_coinage to
	// be initialized
	// init coin structs before allowing coins
	coin_init_all();

#if defined(VEGAS)
	if (!reload)
#endif
	{
		if(!cmos_status)
		{
			oprintf("%dcCMOS: %dcOK", WHITE, LT_GREEN);
		}
		else if(cmos_status == CMOS_ADJ_RESTORED)
		{
			oprintf("%dcCMOS: %dcFACTORY SETTINGS RESTORED", WHITE, LT_YELLOW);
		}
		else
		{
			oprintf("%dcCMOS:  %dcBAD", WHITE, LT_RED);
		}

		y_pos -= sysfont_height;
		set_text_position(SPRITE_HRES/2.0F, y_pos, 1.0F);
		if(cmos_status == CMOS_DEAD || is_dip_coinage)
		{
			oprintf("%dcUSING DIP COINAGE", LT_RED);
		}
		else
		{
			oprintf("%dcUSING CMOS COINAGE", LT_YELLOW);
		}
	}

#ifndef NOTESTS
#if 0
	// Check the files on the disk
	do_file_check();
#endif
#endif

	/* Get and display the version number from the sound system */
	show_sound_version();

	// Load up the sounds
	main_init_sound();

	// Assign functions to the coin inputs
	set_dcsw_handler(LEFT_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(RIGHT_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(CENTER_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(EXTRA_COIN_SWID, coin_sw_handler);
	set_dcsw_handler(BILL_VALIDATOR_SWID, coin_sw_handler);

	// assign switch handler for service credits button
#ifndef DBG_HEAP
	set_dcsw_handler(SERVICE_CREDIT_SWID, coin_svc_cred_handler);
#else
	set_dcsw_handler(SERVICE_CREDIT_SWID, do_heap_dump_handler);
#endif

	// assign switch handler for coin door slam button
	set_dcsw_handler(SLAM_SWID, coin_slam_handler);

	/* assign switch handler for volume buttons */
	set_dcsw_handler(VOLUME_UP_SWID, coin_volume_handler);
	set_dcsw_handler(VOLUME_DOWN_SWID, coin_volume_handler);

	set_dcsw_handler(TEST_SWID, do_test);
	set_dcsw_handler(DIP_BIT_15_SWID, do_test);

	// Assign the start buttons

#ifdef DEBUG
	set_dcsw_handler(SLAM_SWID, dump_tlist);
#endif

#ifdef PROFILE
	set_dcsw_handler(SLAM_SWID, profile_dump);
	set_dcsw_handler(EXTRA_COIN_SWID, reset_profiles);
#endif

#ifdef TILT_HEAP
	set_dcsw_handler(SLAM_SWID, dump_audits);
#endif

#ifdef PASS_DEBUG
	set_dcsw_handler(SLAM_SWID, dump_last_pass);
#endif
	
	// Load the anim data
	anim_sys_init();
	failed = load_anim_block( ASB_GENSEQ, "GENSEQ.ANI", GENSEQ_VERSION );
	if (failed)
		#ifdef DEBUG
		lockup();		// lockup if in debug
		#else
		while (1);		// infinite loop if not in debug -- FIX!!!???
		#endif

	// Show the loading message
#if defined(VEGAS)
	if (!reload)
#endif
	{
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES * 0.05f, 1.0f);
		oprintf("%dcINITIALIZING GAME...", LT_GREEN);
	}

	// enable the cpu bog detector
	mthread_enable_framerate();

	// Clr last game quarter so no bogus stats will be shown in attract
	game_info_last.game_quarter = 0;

	// set four_plr_ver
	if (dip_in.di.reserved2)
		four_plr_ver = 1;
	else
		four_plr_ver = 0;

	// used for Team select
	bought_in_first = 0;

	/* Let the stuff be displayed for a bit */
	sleep(1);

#if 0
	// init coin structs before allowing coins
	coin_init_all();
#endif

	// create the coin totalizer/clicker process
	clicker_proc = qcreate ("ccproc",CC_GRP_PID|NODESTRUCT,coin_clicker_proc, 0, 0, 0, 0);

	// create the volume adjust process
	volume_proc = qcreate ("volume", VOLUME_GRP_PID|NODESTRUCT, coin_volume_proc,0, 0, 0, 0);

	// create the coin drop sound process
	coindrop_proc = qcreate ("coindrop", COIN_IN_GRP_PID|NODESTRUCT, coin_sound_proc,0, 0, 0, 0);

#ifndef TM	// test tackles
#ifndef NOAUDIO
	load_permanent_snds();
#endif
#endif

	// initialize profiling
	#ifdef PROFILE
	reset_profiles( SWITCH_CLOSE, 0 );
	#endif

#if defined(VEGAS)
	if (reload == 2)		// We just switching attract modes?
		reload = 0;
	load_game_info();		// Load up any credits from reload?!
#endif

#if defined(VEGAS)
	if (reload)
	{
		int count = snd_get_first_sound(cursor_bnk_str);

		tick_sndn = count + TICK_SND;
		coin_snd_drop_calln = count + COIN_SND_DROP_CALL;
		coin_snd_paid_calln = count + COIN_SND_PAID_CALL;

		game_info.game_state = GS_PRE_GAME;
		qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 0, p_active, 0, 0);
	}
	else
#endif
	{
		// Create the attract mode loop
#if defined(VEGAS)
		set_coin_hold(FALSE);
#endif
		iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
	}

	// Disable drawing until attract starts up
	draw_enable(0);


}

//-------------------------------------------------------------------------------------------------
// This routine is called from 'plyr_strt_butn' and it handles the updating and setting of
// the players credit info.
//
// RETURNS: 0 - plr cant buy anymore, 1 - ok
//-------------------------------------------------------------------------------------------------
int get_credits(void);

int paid_credits_add(int pnum, int creds)
{
	register game_info_t * pgi = &game_info;
	register pdata_t * pdata = pdata_blocks + pnum;
	register int paid,full;
	int			aud, faud;

	// Calc proper # of buyable quarters
	full = NUM_QUARTERS - pgi->game_quarter;
	if(full < 1)
	{
		full = 1;
	}

	// Exit if player has bought all available game quarters
	if(pdata->quarters_purchased >= full)
	{
		return(0);
	}

	// Are we in pre-game ?
	if(pgi->game_state == GS_PRE_GAME)
	{
		// YES - Has the player already purchased at least 1 quarter
		if(pdata->quarters_purchased)
		{
			// YES - how much has he paid so far
			paid = get_credits_to_start() + ((pdata->quarters_purchased - 1) * get_credits_to_continue());

			// How many more does he need for a full game
			full = get_full_game_credits() - paid;

			// Is the number left for a full game purchase less than the number
			// to continue ?
			if(full <= get_credits_to_continue())
			{
				// YES - Are there enough credits to get the full game ?
				if(full <= get_credits())
				{
					// YES - set the creds value to full
					creds = full;

					// Award a full game
					pdata->quarters_purchased = NUM_QUARTERS;

					// Incrment amount player paid
					pdata->credits_paid += creds;

					// Decrement available credits
					decrement_credits(creds);

					// Set flag saying which player has bought a full game
					game_purchased |= (1<<pnum);

					// Make it obvious to the player, but not during the VS page.
					if (screen != VS_SCRN)
						qcreate("fgbought",0,fullgame_bought_msg,pnum,0,0,0);

					// Done
					return(1);
				}

				// NOT enough credits to get the full game
				else
				{
					// Do nothing
					return(0);
				}
			}

			// Takes more to get full game than to continue
			// Are there enough credits to continue ?
	 		else if(get_credits() >= get_credits_to_continue())
			{
				// YES - Award another quarter
				pdata->quarters_purchased++;

				// Incremtnt amount player paid
				pdata->credits_paid += creds;

				// Decremtnt available credits
				decrement_credits(creds);

				// Done
				return(1);
			}

			// Otherwise not enough credits for full game and not enough
			// credits to continue so don't do anything
			else
			{
				return(0);
			}
		}

		// No quarters purchase yet - award only one quarter
		else
		{
			// Award a quarter
			pdata->quarters_purchased = 1;

			// Increment amount paid
			pdata->credits_paid += creds;

			// Decrement number of credits available
			decrement_credits(creds);

			// Done
			return(1);
		}
	}

	// NOT in pre game - only allow full continue or start price
	else
	{
		// Award a quarter
		pdata->quarters_purchased++;

		// Increment amount he paid
		pdata->credits_paid += creds;

		// Decrement available credits
		decrement_credits(creds);

		// Done
		return(1);
	}
}

//-------------------------------------------------------------------------------------------------
// Do common start button processing; called from <plyr_strt_butn>.
//
// RETURNS: 0 - buyin failed; 1 - ok
//-------------------------------------------------------------------------------------------------
int do_start_press(int pnum, int creds)
{
	if(paid_credits_add(pnum, creds))
	{
		// Make start button press sound
		snd_scall_bank(cursor_bnk_str, EAT_CREDIT_SND, VOLUME4, 127, 1);

		// Was this guy already in game ?
		if(!(p_status2 & (1<<pnum)))
		{
			// NOPE - Increment total plays
			increment_audit(TOTAL_PLAYS_AUD);
		}

		// Set in-game bit
		p_status  |= 1<<pnum;

		// Set was in-game bit for future continue
		p_status2 |= 1<<pnum;

		// Done
		return(1);
	}
	else
	{
		return(0);
	}
}

//-------------------------------------------------------------------------------------------------
// This is the routine which gets called upon each START BUTTON press
//
//   INPUT: signal (up or down), and switch ID
//  OUTPUT: none
//-------------------------------------------------------------------------------------------------
// Question for Mark - If the player buys a full game up front and the
// score is tied after 4 quarters, does the player then get overtime quarters
// until there are either no more or the score at the end of one of the
// overtime quarters is NOT a tie?  If NOT, we should change the special
// deal message to read "Purchase 4 quarters up front for only X credits"!!

void plyr_strt_butn(int sig, int swid)
{
	register game_info_t * pgi = &game_info;
	register pdata_t * pdata;
	register int pnum;
	register int can_start;
	register int can_continue;

#ifdef NO_STARTS
	return;
#endif

	//
	// We only do something when the switch closes
	//
	if(sig == SWITCH_OPEN)
	{
		return;
	}

	// Check to see if there are enough credits to start
	can_start = check_credits_to_start();


	// Check to see if there are enough credits to continue
	can_continue = check_credits_to_continue();

	//
	// Get player number
	//
	switch(swid)
	{
		default:
		case P1_START_SWID:		// Player 1
		{
			pnum = 0;
			if (!(p_status & 2) && can_start)	// if player two isnt in... set plyr 1's bit
				bought_in_first |= 1;
			break;
		}
		case P2_START_SWID:		// Player 2
		{
			pnum = 1;
			if (!(p_status & 1) && can_start)	// if player one isnt in... set plyr 2's bit
				bought_in_first |= 2;
			break;
		}
		case P3_START_SWID:		// Player 3
		{
			pnum = 2;
			if (!(p_status & 8) && can_start)	// if player four isnt in... set plyr 3's bit
				bought_in_first |= 4;
			break;
		}
		case P4_START_SWID:		// Player 4
		{
			pnum = 3;
			if (!(p_status & 4) && can_start)	// if player three isnt in... set plyr 4's bit
				bought_in_first |= 8;
			break;
		}
//#endif
//#endif
//#endif
	}

	// Nothing to do if already bought full game
//	if(game_purchased & (1<<pnum))
//	{
//		return;
//	}

	// Set ptr to player data block
	pdata = pdata_blocks + pnum;

	// Seed random # generator
#if defined(SEATTLE)
	srandom(tick_counter);
#elif defined(VEGAS)
	srandom((int)get_timer_val());
#endif

#if DEBUG
	if((get_joy_val_cur(pnum) == JOY_UP) && (game_info.game_state == GS_ATTRACT_MODE))
	{

		// In case we were in a fade
		normal_screen();

		// Generate two random teams
		game_info.team[0] = randrng(NUM_TEAMS);
		game_info.team[1] = randrng(NUM_TEAMS);

		// Set up assorted state information
		if (four_plr_ver)
		{
#ifdef JASON
			p_status = 4;
			p_status2 = 4;
			game_purchased = 4;
			pdata_blocks[0].quarters_purchased = 0;
			pdata_blocks[1].quarters_purchased = 0;
			pdata_blocks[2].quarters_purchased = 4;
			pdata_blocks[3].quarters_purchased = 0;
#else
			p_status = 0xf;
			p_status2 = 0xf;
			game_purchased = 0xf;
			pdata_blocks[0].quarters_purchased = 4;
			pdata_blocks[1].quarters_purchased = 4;
			pdata_blocks[2].quarters_purchased = 4;
			pdata_blocks[3].quarters_purchased = 4;

#endif
			game_info.team_head[0] = -1;
			game_info.team_head[1] = -1;
			game_info.team_head[2] = -1;
			game_info.team_head[3] = -1;
		}
		else
		{
			p_status = 3;
			p_status2 = 3;
			game_purchased = 3;
			pdata_blocks[0].quarters_purchased = 4;
			pdata_blocks[1].quarters_purchased = 4;
		}



// DEBUG USED FOR TESTING OF PLAYS
//#if DEBUG
//		game_info.team[0] = 0;
//		game_info.team[1] = 0;
//
//		fprintf(stderr, "Starting just players 1 and 3 for play debug\r\n");
//		p_status = 0x5;
//		p_status2 = 0x5;
//		game_purchased = 0x5;
//		pdata_blocks[0].quarters_purchased = 4;
//		pdata_blocks[1].quarters_purchased = 0;
//		pdata_blocks[2].quarters_purchased = 4;
//		pdata_blocks[3].quarters_purchased = 0;
//#endif



		game_info.game_state = GS_PRE_GAME;

		// Create the game process
		fprintf(stderr, "create dog game_proc_debug\r\n");
		qcreate("game", 0, game_proc_debug, 0, 0, 0, 0);
		fprintf(stderr, "done with create game_proc_debug\r\n");

		// All done
		return;
	}
#endif

	//
	// task handler for start button - we only get here if the player has NOT
	// already purchased a full game.
	//
	switch(pgi->game_state)
	{
		// Diagnostics mode - We should NEVER see this state
		case GS_DIAGNOSTICS:
		{
			break;
		}

		// Attract mode - The only thing a player can do from attract mode
		// is start.  Once started from attract mode the game goes to PRE_GAME
		// state so continues from attract mode can NOT occur.
		case GS_ATTRACT_MODE:
		{
			// Don't allow press if <game_proc> is loading
			if (existp(GAME_PROC_PID, -1) && texture_load_in_progress)
				return;

			snd_stop_all();
			snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);		// trk 0: music, trk 1:audience, trk 5: annoucner

			// Enough credits to start ?
		 	if(can_start)
			{

				// YES - Initialize player state information
				p_status = 0;							// just to make sure
				p_status2 = 0;
				game_purchased = 0;					// no one has bought full game...

				// clear plyr credit ram
				pdata->credits_paid = 0;
				pdata->quarters_purchased = 0;

				pdata_blocks[0].please_wait = 0;
				pdata_blocks[1].please_wait = 0;
				// 2/4 plyr kit logic
				pdata_blocks[2].please_wait = 0;
				pdata_blocks[3].please_wait = 0;

				// Give this player 1 quarter
				if(do_start_press(pnum, get_credits_to_start()))
				{
					//fprintf(stderr,"Start from GS_ATTRACT_MODE w/ game_mode %d\r\n",pgi->game_mode);
					//// Disable drawing (re-enabled by pre-game screens)
					//draw_enable(0);

					// Disable the credit page
					credit_pg_on = 0;

					// Switch game to PRE_GAME state
					pgi->game_state = GS_PRE_GAME;

					// Quarter number is 0
					pgi->game_quarter = 0;

					// Create the pre-game screens
					qcreate("pre_game", PRE_GAME_SCRNS_PID, pre_game_scrns, 0, pnum, 0, 0);
				}
				else
				{
					// !!!FIX!!!
					// Lockup if failed!!!
					while (1) continue;
				}
			}
			else if(!credit_pg_on)
			{
				// Fire up the credit page
				qcreate("coinpg",CREDIT_PAGE_ID,credit_page_proc,0,0,0,0);
			}
			break;
		}

		// Pre-Game state - In this state the player can start, continue, and
		// get the special full game purchase deal
		case GS_PRE_GAME:
		{
			// Has the player already pressed start at least 1 time ?
			if(p_status2 & (1<<pnum))
			{
				// YES - Does the player have enough to continue or has the
				// player bought at least 1 quarter already ?
				if(can_continue || pdata->quarters_purchased)
				{
					// YES - Go award the player another quarter
					if(do_start_press(pnum, get_credits_to_continue()))
					{
						//fprintf(stderr,"Continue from GS_PRE_GAME w/ game_mode %d\r\n",pgi->game_mode);
						// chalk 'game continue' audit
//						increment_audit(CONTINUE_AUD);
					}
				}
			}

			// Player has not started yet.  In two player situations starting
			// from attract mode the second player to start ends up here.  If
			// the game is already in this state and a player has not yet started
			// the player ends up here too.
			else if(can_start)
			{
				// Clear this players state information
				pdata->credits_paid = 0;
				pdata->quarters_purchased = 0;
				pdata->please_wait = 0;

				// Go award a quarter
				if(do_start_press(pnum, get_credits_to_start()))
				{
					//fprintf(stderr,"Start from GS_ATTRACT_MODE w/ game_mode %d\r\n",pgi->game_mode);
					// chalk 'game start' audit
					increment_audit(GAME_START_AUD);
					update_gamestart_percents();
				}
			}
			break;
		}

		// End of quarter state - In this state the player can start or
		// continue but can NOT get the full game purchase deal.
		case GS_EOQ_BUY_IN:
		{
			// Nothing to do if game is over
			if (game_info.game_mode != GM_GAME_OVER)
			{
				// Has the player already pressed start at least 1 time ?
				if(p_status2 & (1<<pnum))
				{
					// YES - Does the player have enough to continue ?
					if(can_continue)
					{
						// YES - Go award the player another quarter
						if(do_start_press(pnum, get_credits_to_continue()))
						{
							//fprintf(stderr,"Continue from GS_EOQ_BUY_IN w/ game_mode %d\r\n",pgi->game_mode);
							// chalk 'game continue' audit
							increment_audit(CONTINUE_AUD);
						}
					}
				}

				// Are there enough credits for the player to start ?
				else if(can_start)
				{
					// Clear this players state information
					pdata->credits_paid = 0;
					pdata->quarters_purchased = 0;
					pdata->please_wait = 0;

					// Go award a quarter ?
					if(do_start_press(pnum, get_credits_to_start()))
					{
						//fprintf(stderr,"Start from GS_EOQ_BUY_IN w/ game_mode %d\r\n",pgi->game_mode);
						// chalk 'mid-game start'
						increment_audit(MID_GAME_START_AUD);
					}
				}
			}
			break;
		}

		// In Game state.  In this state we only allow new players that have
		// not already started to start up and play as the opposing team if
		// there is at least MINIMUM_QUARTER_TIME left in the current quarter.
		case GS_GAME_MODE:
		{
			// Is the player in the game ?
			if(!(p_status & (1<<pnum)))
			{
				// NOPE - Is the please wait message NOT being displayed AND
				// there ARE enough credits for him to start ?
				if(!pdata->please_wait && can_start)
				{
					// Is there enough time remaining in the current quarter
					// to allow this player to start or
					// is it free play with any time left at all?
					if (pgi->game_time >= MINIMUM_QUARTER_TIME ||
						(coin_check_freeplay() && pgi->game_time))
					{
						// YES - Go award him the quarter
						if(do_start_press(pnum, get_credits_to_start()))
						{
							//fprintf(stderr,"Start from GS_GAME_MODE w/ game_mode %d\r\n",pgi->game_mode);
							// chalk 'mid-game start' audit
							increment_audit(MID_GAME_START_AUD);
						}
						else
						{
							// !!!FIX!!!
							// Lockup if failed!!!
							while (1) continue;
						}
					}

					// Not enough time left in the quarter
					else if (!coin_check_freeplay())
					{
						// Tell him to wait
						// (or something else for 4th quarter or OT?)
						pdata->please_wait = 1;
					}
				}
			}
			break;
		}

		// In this game state the player can only start a new game and should
		// be allowed to do the same as game states ATTRACT and PRE_GAME.
		case GS_GAME_OVER:
		{
			break;
		}

		// The game should NEVER be in this state
		case GS_FREE_PRICE:
		case GS_HALF_PRICE:
		{
			break;
		}
	}
}

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: main_init_sound()                                               */
/*                                                                           */
/* Initializes the sound system.                                             */
/*                                                                           */
/* Downloads the engine sounds from disk to the sound system.                */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* MVB 01 Feb 97                                                             */
/*                                                                           */
/*****************************************************************************/
void main_init_sound (void)
{
	// Initialize the sound system
	if(snd_init_multi( SND_OPSYS_0223 ) != 0)
	{
		fprintf(stderr, "\r\nmain_init_sound(): ERROR initializing sound system.\r\n");
	}

	// Check for bogus opsys
	if(snd_get_opsys_ver() == 0xeeee)
	{
		fprintf(stderr, "\r\nmain_init_sound(): ERROR in sound system\r\n");
	}

	// Initialize the sound bank stuff
	snd_bank_init();

	/* Get the volume level from CMOS */
	if (get_adjustment(VOLUME_LEVEL_ADJ, &game_info.master_volume)) {
		/* CMOS Failure - set volume level to some default value */
		game_info.master_volume = 127;
	}
	/* Get the attract mode volume level from CMOS */
	if (get_adjustment(ATTRACT_VOLUME_LEVEL_ADJ, &game_info.attract_mode_volume)) {
		/* CMOS Failure - set volume level to some default value */
		game_info.attract_mode_volume = 40;
	}
	
	// Set the master volume level
	snd_master_volume(game_info.master_volume);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void inc_pcount( void )
{
	pcount += 1;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if DEBUG
static void dump_tlist(int sig, int id)
{
	if(sig == SWITCH_OPEN)
	{
		return;
	}

//	show_texture_list();
	do_list_dump();
}
#endif

void no_start(int sig, int swid)
{
}

void start_enable(int flag)
{
	// Are we in free play mode ?
	if(coin_check_freeplay())
	{
		// YES - Never disable start buttons
		start_state = TRUE;
		set_dcsw_handler(P1_START_SWID, plyr_strt_butn);
		set_dcsw_handler(P2_START_SWID, plyr_strt_butn);
		if(!(p_status & 1))
		{
			pdata_blocks[0].please_wait = 0;
		}
		if(!(p_status & 2))
		{
			pdata_blocks[1].please_wait = 0;
		}

		if (four_plr_ver)
		{
			// 2/4 plyr kit logic
			set_dcsw_handler(P3_START_SWID, plyr_strt_butn);
			set_dcsw_handler(P4_START_SWID, plyr_strt_butn);
			if(!(p_status & 4))
			{
				pdata_blocks[2].please_wait = 0;
			}
			if(!(p_status & 8))
			{
				pdata_blocks[3].please_wait = 0;
			}
		}

		return;
	}
	if(flag == TRUE)
	{
		start_state = TRUE;
		set_dcsw_handler(P1_START_SWID, plyr_strt_butn);
		set_dcsw_handler(P2_START_SWID, plyr_strt_butn);
		if(!(p_status & 1))
		{
			pdata_blocks[0].please_wait = 0;
		}
		if(!(p_status & 2))
		{
			pdata_blocks[1].please_wait = 0;
		}

		if (four_plr_ver)
		{
			// 2/4 plyr kit logic
			set_dcsw_handler(P3_START_SWID, plyr_strt_butn);
			set_dcsw_handler(P4_START_SWID, plyr_strt_butn);
			if(!(p_status & 4))
			{
				pdata_blocks[2].please_wait = 0;
			}
			if(!(p_status & 8))
			{
				pdata_blocks[3].please_wait = 0;
			}
		}
	}
	else
	{
		start_state = FALSE;
		set_dcsw_handler(P1_START_SWID, no_start);
		set_dcsw_handler(P2_START_SWID, no_start);
		if(!(p_status & 1))
		{
			pdata_blocks[0].please_wait = 1;
		}
		if(!(p_status & 2))
		{
			pdata_blocks[1].please_wait = 1;
		}

		if (four_plr_ver)
		{
			// 2/4 plyr kit logic
			set_dcsw_handler(P3_START_SWID, no_start);
			set_dcsw_handler(P4_START_SWID, no_start);
			if(!(p_status & 4))
			{
				pdata_blocks[2].please_wait = 1;
			}
			if(!(p_status & 8))
			{
				pdata_blocks[3].please_wait = 1;
			}
		}
	}
}

#if 0
int get_start_state(void)
{
	return(start_state);
}
#endif

static int sw_expect[] = {
P1_A, P1_A, P1_B, P1_A, P1_A, P1_B, P1_B, P1_C, P1_C, P1_A
};

static int check_val(int *sw_array, unsigned count)
{
	unsigned	i;

	for(i = 0; i < count; i++)
	{
		if(sw_array[i] != sw_expect[i])
		{
			return(0);
		}
	}
	return(1);
}

void backdoor_proc(int *args)
{
	int		sw[32];
	int		val;
	unsigned	count;

	// Do forever
	while(1)
	{
		// Allow other shit to run
		sleep(1);

		// Reset count
		count = 0;

		// Fill in array witch switches
		while(count < (sizeof(sw_expect)/sizeof(int)) && (get_player_sw_current() & P1_UP))
		{
			// Get player switches
			val = get_player_sw_current() & ~(P1_UP|0xFFFF0000);

			// Is one pressed ?
			if(val)
			{
				// Wait for it to release
				while(get_player_sw_current() & ~P1_UP)
				{
					sleep(1);
				}

				// Put it in the array
				sw[count] = val;

				// Increment the counter
				count++;

				// Is what we have so far valid ?
				if(!check_val(sw, count))
				{
					// NOPE - Start all over
					break;
				}
			}
			sleep(1);
		}

		// Is the array filled ?
		if(count == sizeof(sw_expect)/sizeof(int))
		{
			// YES - Is it the sequence we are looking for ?
			if(check_val(sw, count))
			{
				// YES - Run diagnostics
				sw[0] = 0;
				sw[1] = 0;
				sw[2] = 0;
				sw[3] = 0;
				start_diags(sw);
			}
		}
	}
}
//static int sw_expect[] = {
//P1_B, P1_B, P1_A, P1_A, P1_A, P1_B,
//P1_B, P1_A, P1_B, P1_C, P1_C, P1_B,
//P1_A, P1_C, P1_B, P1_B, P1_B, P1_C, P1_B
//};
//
//static int jsw_expect[] = {
//P1_A, P1_B, P1_A, P1_B, P1_A, P1_B, P1_B, P1_A, P1_C, P1_A, P1_B
//};
//
//static int check_val(int *sw_array, unsigned count)
//{
//	unsigned	i;
//
//	for(i = 0; i < count; i++)
//	{
//		if(sw_array[i] != sw_expect[i])
//		{
//			return(0);
//		}
//	}
//	return(1);
//}
//
//void backdoor_proc(int *args)
//{
//	int		sw[32];
//	int		jsw[32];
//	int		val;
//	unsigned	count = 0, jcount = 0;
//
//	// Do forever
//	while(1)
//	{
//		// Allow other shit to run
//		sleep(1);
//
//		if ((get_player_sw_current() & (P1_RLDU_MASK|P2_RLDU_MASK)) == P1_UP)
//		{
//			// Get player switches
//			if ((val = get_player_sw_close() & (P1_ABCD_MASK|P2_ABCD_MASK)))
//			{
//				{char * sz[] = {"","A","B","","C"};fprintf(stderr,"%s   cnt:%d\r\n",sz[val>>4],count);}
//				// Put it in the array
//				sw[count++] = val;
//
//				// Is what we have so far valid ?
//				if(!check_val(sw, count))
//				{
//					count = 0;
//				}
//				else if (count == (sizeof(sw_expect)/sizeof(int)))
//				{
//					// YES - Run diagnostics
//					sw[0] = 0;
//					sw[1] = 0;
//					sw[2] = 0;
//					sw[3] = 0;
//					start_diags(sw);
//				}
//			}
//		}
//		else
//			count = 0;
//
//		if ((get_player_sw_current() & (P1_RLDU_MASK|P2_RLDU_MASK)) == P1_DOWN)
//		{
//			// Get player switches
//			if ((val = get_player_sw_close() & (P1_ABCD_MASK|P2_ABCD_MASK)))
//			{
//				// Put it in the array
//				jsw[jcount++] = val;
//
//				// Is what we have so far valid ?
//				if(!check_val(jsw, jcount))
//				{
//					jcount = 0;
//				}
//				else if (jcount == (sizeof(jsw_expect)/sizeof(int)))
//				{
//					// do it
//					jcount = 0;
//				}
//			}
//		}
//		else
//			jcount = 0;
//	}
//}

//////////////////////////////////////////////////////////////////////////////
#if defined(SEATTLE)

#ifdef TILT_HEAP
static void dump_audits( int sw_state, int sw_id )
{
	if(sw_state != SWITCH_CLOSE)
		return;

//	fprintf( stderr, "heap available: %d\n", get_heap_available() );
	dump_raw_audits();
}
#endif


//////////////////////////////////////////////////////////////////////////////
// prepares a stack trace and dumps it to cmos

#define STACK_DEPTH		16
#define STACK_COUNT		8

void dump_stack_trace( void )
{
	int		pc,sp,ra, i;
	int		*regs = (int *)0x80014280;
	int		level = 0, done = FALSE, new = FALSE, new_ra;
	int		inst, offset;
	int		frames[STACK_DEPTH], copy_buffer[STACK_DEPTH * (STACK_COUNT-1)];
	int		*addr, *user_addr;

	pc = regs[PC];
	sp = regs[GP29];
	ra = regs[GP31];

	while((!done) && (level < STACK_DEPTH))
	{
//		printf( "frame %d: PC %p  SP %p\n", level, (int *)pc, (int *)sp );
		frames[level] = pc;
		new = FALSE;
		new_ra = FALSE;

		while( !new )
		{
			pc -= 4;
			inst = *((int *)pc);
			offset = inst & 0xffff;
			// sign-extend
			offset = offset << 16;
			offset = offset >> 16;

			if (pc == (int)0x800c4000)
			{
				done = TRUE;
				new = TRUE;
//				printf( "start reached - done\n" );
			}
			else if (inst == 0x03e00008)
			{	// jr ra
				if ((new_ra == FALSE) && (level > 0))
				{
					done = TRUE;
					new = TRUE;
//					printf( "proc top reached - done\n" );
				}
				else
				{
//					printf( "%08x %08x\t", pc, inst);
//					printf( "jr ra, new pc = %p\n", (int *)ra );
					pc = ra;
					new = TRUE;
				}
			}
			else if ((inst & 0xffff0000) == 0x27bd0000)
			{	// addiu sp,sp,CONST
//				printf( "%08x %08x\t", pc, inst);
//				printf( "addiu sp,sp,$%x\n", offset );
				sp -= offset;
			}
			else if ((inst & 0xffff0000) == 0xafbf0000)
			{	// sw ra,CONST(sp)
//				printf( "%08x %08x\t", pc, inst);
//				printf( "sw ra,$%x(sp)\n", offset );
				ra = *((int *)(sp + offset));
				new_ra = TRUE;
			}
		}
		
		level++;
	}

	for( i = level; i < STACK_DEPTH; i++ )
		frames[i] = 0;

	// now write them to cmos
	addr = get_cmos_user_address();
	user_addr = addr + 4096;

	// shuffle banks
	cmos_read( user_addr, (char *)copy_buffer, STACK_DEPTH * (STACK_COUNT-1) * sizeof(int) );
	cmos_write( user_addr + STACK_DEPTH * sizeof(int), (char *)copy_buffer, STACK_DEPTH * (STACK_COUNT-1) * sizeof(int) );

	// write new one
	cmos_write( user_addr, (char *)frames, STACK_DEPTH * sizeof(int) );
}
#endif
//////////////////////////////////////////////////////////////////////////////
