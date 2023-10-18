//-------------------------------------------------------------------------------------------------
//	SWEEPS.C - Sweepstakes handling functions
//
//	James T. Anderson
//  Midway Games, Inc.
//  Copyright (c) 1999
//-------------------------------------------------------------------------------------------------
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

#include "/Video/Nfl/Include/sweeps.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/movie.h"
#include "/Video/Nfl/Include/Attract.h"
#include "/video/nfl/include/coin.h"

#include "/Video/Nfl/Include/plaq.h"		// Graphics
//#include "/Video/Nfl/Include/makeplay.h"	// Graphics

#define IMAGEDEFINE	1
#include "include/sweepstk.h"

//-------------------------------------------------------------------------------------------------
// Externals
//-------------------------------------------------------------------------------------------------

// Variables
extern short force_selection;							// !=0 force cursor to current choice
extern plyr_record_t *rec_sort_ram;						// pointer to the array of PLAYER RECORDs from cmos into ram
extern int texture_load_in_progress;

// Functions

//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------
#define PLATE_X	109.0f
#define PLATE_Y	135.0f
#define PLATE_Z	100.0f

#define STR_X	325.0f
#define	STR_Y	45.0f

#define MAX_POS	11
#define MAX_CODE_SIZE	7

#define MAX_WINNERS		91

#define	SWEEPS_TIME		10
#define SWEEPS_PRETIME	1*tsec

#define TIMEOUT_X		(SPRITE_HRES/2.0f)
#define TIMEOUT_Y		20.0f

#define MIN_TIMEOUT		1*tsec
#define WINNER_TIMEOUT	10*tsec
#define LOSER_TIMEOUT	10*tsec

#define SWEEPS_SID		0x568
#define NUMBER_SID		0x569

#define ALL_ABCD_MASK	(P1_ABCD_MASK|P2_ABCD_MASK|P3_ABCD_MASK|P4_ABCD_MASK)

#define SWEEPS_MUSIC_BANK	"sweeps"

//-------------------------------------------------------------------------------------------------
// Variables
//-------------------------------------------------------------------------------------------------
static char winning_nums[MAX_WINNERS][MAX_CODE_SIZE];

image_info_t *rules[][4] =  {
	{ &sw_rule1, &sw_rule1_c1, &sw_rule1_c2, &sw_rule1_c3 },
	{ &sw_rule2, &sw_rule2_c1, &sw_rule2_c2, &sw_rule2_c3 },
	{ &sw_rule3, &sw_rule3_c1, &sw_rule3_c2, &sw_rule3_c3 },
	{ &sw_rule4, &sw_rule4_c1, &sw_rule4_c2, &sw_rule4_c3 },
	{ &sw_rule5, &sw_rule5_c1, &sw_rule5_c2, &sw_rule5_c3 },
};

static image_info_t *cursor_tbl[] = {
	&l_1,&l_2,&l_3,
	&l_4,&l_5,&l_6,
	&l_7,&l_8,&l_9,
	&l_pspc,&l_0,&l_pdel,
};

static char pin_nbrs_trans[] = {
	CH_1,CH_2,CH_3,
	CH_4,CH_5,CH_6,
	CH_7,CH_8,CH_9,
	CH_SPC,CH_0,CH_DEL
};

static char instring[MAX_CODE_SIZE];


static tmu_ram_texture_t sweep_textures[] = {
	{"optbk00.wms",	BKGRND_TID},
	{"optbk01.wms",	BKGRND_TID},
	{"optbk02.wms",	BKGRND_TID},

	{"plqnme11.wms",JAPPLE_BOX_TID},
	{"plqcrd00.wms",JAPPLE_BOX_TID},			// 256 * 256 * 16bits
	{"plqcrd01.wms",JAPPLE_BOX_TID},
//	{"plqcrd02.wms",JAPPLE_BOX_TID},
	{"plqlet.wms",	JAPPLE_BOX_TID},

	{"plqfsh.wms",JAPPLE_BOX_TID},				// 256 * 256 * 16bits
	{NULL,0}
};

static char sweeps_fn[] = "sweeps.txt";

static int num_winners;

//-------------------------------------------------------------------------------------------------
// Function Prototypes
//-------------------------------------------------------------------------------------------------

static void load_sweeps_textures(void);
static void load_sweeps_winners(void);
static void get_sweeps_number(int *args);
static int check_for_winner(void);
static void winner_screen(void);
static void loser_screen(void);
static void print_instring(float x, float y, float z, int sid);
static void sweeps_rules(int min_time, int max_time);

//-------------------------------------------------------------------------------------------------
//	sweeps_proc - Thread to handle sweepsstakes entry and winner checks.
//-------------------------------------------------------------------------------------------------
void sweeps_proc(int *args)
{
	int done;

	done = 0;

	wipeout();
	if (rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}

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

	// Make sure the we fade back to normal if we came here from
	// a screen that faded.
	normal_screen();

	// Make sure any transitions go away
	delete_process_id(TRANSIT_PID);
	kill_plates();

	sweeps_rules(1*60, 8*60);

	// Load in Textures
	load_sweeps_textures();		// Load Textures
	load_sweeps_winners();		// Load Winning Numbers

	// Load in sound bank
	coin_sound_holdoff(TRUE);
	if (snd_bank_load_playing(SWEEPS_MUSIC_BANK) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
	coin_sound_holdoff(FALSE);

	qcreate("timer", TIMER_PID, timedown, SWEEPS_PRETIME, SWEEPS_TIME, TIMEOUT_X, TIMEOUT_Y);
	qcreate("sweeps", CREATE_PLAY_PID, get_sweeps_number, 0, 0, 0, 0);

	while (!done)
	{
		sleep(1);
		if (!existp(CREATE_PLAY_PID, 0xffffffff))
			done = 1;
	}

	killall(TIMER_PID, -1);		// Kill Timer thread

	qcreate("timer", TIMER_PID, timedown, SWEEPS_PRETIME, SWEEPS_TIME, TIMEOUT_X, TIMEOUT_Y);

	if ((check_for_winner()) && (!force_selection))
	{
//fprintf(stderr, "Winner!\n");
		snd_scall_bank(SWEEPS_MUSIC_BANK, 0, VOLUME4, 127, PRIORITY_LVL5);
		winner_screen();
	} else {
//fprintf(stderr, "Loser!\n");
		snd_scall_bank(SWEEPS_MUSIC_BANK, 1, VOLUME4, 127, PRIORITY_LVL5);
		loser_screen();
	}

	snd_bank_delete(SWEEPS_MUSIC_BANK);		// Delete our sound bank
		
	unlock_multiple_textures(BKGRND_TID,0xFFFFFFFF);
	del1c(0,0xffffffff);
	del1c(1,0xffffffff);

	iqcreate("attract", AMODE_GRP_PID, attract_loop, 0, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------
// winner_screen - Screen that shows winner's information
//-------------------------------------------------------------------------------------------------
static void winner_screen(void)
{
	sprite_info_t *psprite;
	int done;
	int timeout;

	timeout = MIN_TIMEOUT;
	done = 0;

	// draw background
	psprite = beginobj(&sw_winner,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_winner_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_winner_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_winner_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;

	while (timeout)
	{
		sleep(1);
		timeout-=1;
	}

	while(!done)
	{
		sleep(1);
		if (force_selection)
			done = 1;
		if ((get_player_sw_current() & ALL_ABCD_MASK))
			done = 1;
	}

	draw_enable(0);
 	del1c(0, 0xffffffff);
	del1c(1, 0xffffffff);
	del1c(BKGRND_TID, 0xffffffff);
	delete_multiple_strings(NUMBER_SID, 0xffffffff);
//	delete_multiple_strings(SWEEPS_SID, 0xffffffff);
}

//-------------------------------------------------------------------------------------------------
// loser_screen - Screen displaying Loser information
//-------------------------------------------------------------------------------------------------
static void loser_screen(void)
{
	sprite_info_t *psprite;
	int done;
	int timeout;

	timeout = MIN_TIMEOUT;
	done = 0;

	// draw background
	psprite = beginobj(&sw_loser,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_loser_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_loser_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_loser_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;


	while (timeout)
	{
		sleep(1);
		timeout-=1;
	}

	while(!done)
	{
		sleep(1);
		if (force_selection)
			done = 1;
		if ((get_player_sw_current() & ALL_ABCD_MASK))
			done = 1;
	}

	draw_enable(0);
 	del1c(0, 0xffffffff);
	del1c(1, 0xffffffff);
	del1c(BKGRND_TID, 0xffffffff);
}

//-------------------------------------------------------------------------------------------------
// check_for_winner - Check instring global for winning string.
//-------------------------------------------------------------------------------------------------
static int check_for_winner(void)
{
	int cnt;
	int res;

	res = 0;
	for (cnt=0; cnt < num_winners; cnt++)
	{
		if (!strncmp(winning_nums[cnt], instring, MAX_CODE_SIZE))
		{
			res = 1;
			break;
		}	
	}
	return(res);
}

//-------------------------------------------------------------------------------------------------
// get_sweeps_number - USER I/O function that grabs the sweepstakes digits
//-------------------------------------------------------------------------------------------------
static void get_sweeps_number(int *args)
{
	sprite_info_t *psprite;
	sprite_info_t *focus;
	int cur_idx;
	int pre_idx;
	int done;
	int joy1, joy2, joy3, joy4;
	int btn1, btn2, btn3, btn4;
	int joy, btn;
	int input_offset;

	done = 0;
	cur_idx = 0;
	pre_idx = 0;
	input_offset = 0;

	memset(instring, '-', MAX_CODE_SIZE * sizeof(char) );		// Clear input buffer

	// draw background
	psprite = beginobj(&sw_code,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_code_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_code_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;
	psprite = beginobj(&sw_code_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	psprite->id = 1;

#if 0
	psprite = beginobj(&pinplq1,	PLATE_X, PLATE_Y,PLATE_Z, JAPPLE_BOX_TID);
	psprite->id = 1;
	psprite = beginobj(&pinplq1_c1,	PLATE_X, PLATE_Y,PLATE_Z, JAPPLE_BOX_TID);
	psprite->id = 1;
#endif

	focus = beginobj(cursor_tbl[cur_idx],	PLATE_X, PLATE_Y,PLATE_Z-1.0f, JAPPLE_BOX_TID);	
	instring[input_offset] = pin_nbrs_trans[cur_idx];
	print_instring(STR_X, STR_Y, 5.0f, NUMBER_SID);

	draw_enable(1);

	while (!done)
	{
		// Check stick movement
		joy1 = get_joy_val_down(0);
		joy2 = get_joy_val_down(1);
		joy3 = get_joy_val_down(2);
		joy4 = get_joy_val_down(3);
		joy = joy1 | joy2 | joy3 | joy4;
		if (joy)
		{
			switch(joy)
			{
				case JOY_UP:
					if (cur_idx > 2)		// below first row ?
						cur_idx -= 3;		// yep, move up a row
					break;
				case JOY_DOWN:
					if (cur_idx < 9)		// not on last row ?
						cur_idx += 3;		// nope, move down a row
					break;
				case JOY_LEFT:
				case JOY_DWN_LFT:
				case JOY_UP_LFT:
					if (cur_idx > 0)
						cur_idx -= 1;
					break;
				case JOY_RIGHT:
				case JOY_DWN_RGT:
				case JOY_UP_RGT:
					if (cur_idx < 11)
						cur_idx += 1;
					break;
			}

			// Check for change in cursor position
			if (cur_idx != pre_idx)
			{
				pre_idx = cur_idx;
			 	snd_scall_bank(cursor_bnk_str,CAP_LET_CUR_SND,VOLUME2,127,PRIORITY_LVL1);
				instring[input_offset] = pin_nbrs_trans[cur_idx];
				change_img_tmu( focus, cursor_tbl[cur_idx], JAPPLE_BOX_TID );
			}

		}

		btn1 = get_but_val_down(0);
		btn2 = get_but_val_down(1);
		btn3 = get_but_val_down(2);
		btn4 = get_but_val_down(3);
		btn = btn1 | btn2 | btn3 | btn4;
		if (btn)
		{
			if (cur_idx == MAX_POS)
			{	// delete key
				instring[input_offset] = CH_DSH;
				if (input_offset > 0)
					input_offset -= 1;
			}
			else
			{	// regular key
				instring[input_offset] = pin_nbrs_trans[cur_idx];
				if (++input_offset == MAX_CODE_SIZE)
					done = TRUE;
				else
					instring[input_offset] = pin_nbrs_trans[cur_idx];	// echo current char. in next position
			}
			snd_scall_bank(cursor_bnk_str,CAP_LET_SEL_SND,VOLUME2,127,PRIORITY_LVL1);				// play sound
			flash_cursor(focus, JAPPLE_BOX_TID);
		}
		print_instring(STR_X, STR_Y, 5.0f, NUMBER_SID);
		sleep(1);
		if (force_selection)
			done = 1;
	}

	// unlock these textures so they can be deleted
	unlock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
	del1c(0,0xffffffff);
	del1c(1,0xffffffff);
	del1c(JAPPLE_BOX_TID,0xffffffff);

#if DEBUG
	fprintf(stderr,"sweeps number = %s\n", instring);
#endif
}

//-------------------------------------------------------------------------------------------------
// print_instring - Stupid function
//-------------------------------------------------------------------------------------------------
static void print_instring(float x, float y, float z, int sid)
{
	delete_multiple_strings(sid, 0xffffffff);
 	set_text_font (FONTID_BAST18);
	set_text_color(LT_YELLOW);
	set_string_id(sid);
	set_text_position(x, y, z);//79.0F, 5.0F);
	oprintf("%dj%s", (HJUST_CENTER|VJUST_CENTER), instring);
}

//-------------------------------------------------------------------------------------------------
// load_sweeps_textures - Load all texture pages associate with this screen.
//-------------------------------------------------------------------------------------------------
static void load_sweeps_textures(void)
{
	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

//	imgs_loaded = 0;

	if (load_textures_into_tmu(sweep_textures))
	{
		fprintf(stderr, "Couldn't load all sweeps textures into tmu\r\n");
#ifdef DEBUG
		lockup();
#else
		return;
#endif
	}
//	imgs_loaded = 1;

	// Disable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
	lock_multiple_textures(BKGRND_TID,0xFFFFFFFF);
}

//-------------------------------------------------------------------------------------------------
// load_sweeps_winners - Load in text file of winning numbers
//-------------------------------------------------------------------------------------------------
static void load_sweeps_winners(void)
{
	FILE *fp;
	char *buffer;
	char *cur_ptr;
	char *nxt_ptr;
	long f_size;
	int cnt;
	char tmp_str[20];

	num_winners = 0;
	if ((fp = fopen(sweeps_fn,"r")))
	{

		// Read in file
		fseek(fp, 0, SEEK_END);
		f_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer = malloc(sizeof(char) * f_size);
		memset(buffer, 0, sizeof(char) * f_size);
		fread(buffer, sizeof(char), f_size, fp);
		fclose(fp);

		cur_ptr = &buffer[4];		// Skip over CRC
		nxt_ptr = &buffer[4];
		// Parse buffer

		while (sscanf(cur_ptr, "%s\n", tmp_str))
		{
#if DEBUG
			fprintf(stderr,"[%s]\n", tmp_str);
#endif
			cur_ptr += strlen(tmp_str)+1;
			strcpy(winning_nums[num_winners], tmp_str);
			num_winners+=1;
			if (num_winners >= MAX_WINNERS)
				break;
		}
		free(buffer);
#if DEBUG
		fprintf(stderr, "num_winners = %d\n", num_winners);
#endif
	} else {
#if DEBUG
		fprintf(stderr, "ERROR - Could not load %s\n", sweeps_fn);
		while(1)
			sleep(1);
#else
		while(1);
#endif
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void sweeps_rules(int min_time, int max_time)
{
	sprite_info_t *bg;
	int cnt;
	int size = (sizeof(rules) / (sizeof(image_info_t *) * 4));

	for (cnt = 0; cnt < size; cnt++)
	{
		bg = beginobj(rules[cnt][0],    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(rules[cnt][1],    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(rules[cnt][2],    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(rules[cnt][3],    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
		draw_enable(1);	
		wait_for_any_butn_press(min_time, max_time);		
		draw_enable(0);
		del1c(0,0xFFFFFFFF);							// delete background
	}
}

//-------------------------------------------------------------------------------------------------
// sweeps_attract - Sweeps stakes attract screen
//-------------------------------------------------------------------------------------------------
void sweeps_attract(int min_time, int max_time)
{
	sprite_info_t *bg;

	wipeout();
	
	if (rec_sort_ram)
	{
		JFREE(rec_sort_ram);
		rec_sort_ram = NULL;
	}

	bg = beginobj(&sw_attract,    0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bg = beginobj(&sw_attract_c1, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bg = beginobj(&sw_attract_c2, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);
	bg = beginobj(&sw_attract_c3, 0.0F, SPRITE_VRES, 140.0f, 0xbeef);

	draw_enable(1);	
	fade(1.0, 20, NULL);
	
	wait_for_any_butn_press(min_time, max_time);


	fade(0.0f, 29, 0);		//0=full off
	sleep(30);

	// clean_up:
	delete_all_sprites();

	// delete background
	del1c(0,0xFFFFFFFF);

	sleep(1);
				
	draw_enable(0);
}

//--------------------------------------------------------------------------------------------[EOF]
