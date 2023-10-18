#if defined(VEGAS)

//-------------------------------------------------------------------------------------------------
//	GAME_SEL.C - Game Select Functions for Blitz 2000 and NBA Showtime
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

#include "/video/nfl/include/game.h"
#include "/video/nfl/include/id.h"			// Graphic IDs
#include "/video/nfl/include/select.h"		// Misc Japple Crap
#include "/video/nfl/include/sndcalls.h"	// Sound calls
#include "/video/nfl/include/game_sel.h"
#include "/video/nfl/include/coin.h"
#include "/video/nfl/include/flash.h"

#include "/video/nfl/include/gamesel.h"

//-------------------------------------------------------------------------------------------------
//	External Variables
//-------------------------------------------------------------------------------------------------

extern int p_status;
extern int bought_in_first;
extern pdata_t pdata_blocks[];					// from main.c; defined in "game.h"
extern coin_credits_t coins;
extern int force_selection;
extern image_info_t *sparkle_imgs[];

//-------------------------------------------------------------------------------------------------
//	Defines / Variables
//-------------------------------------------------------------------------------------------------

#define RELOAD_FILE			"reload.fmt"

#define MIN_FADE	153
#define MAX_FADE 	255
#define FADE_DELAY	20

#define	SEL_TIME		5
#define SEL_PRETIME		5

#define TIMEOUT_X		(SPRITE_HRES/2.0f)
#define TIMEOUT_Y		20.0f


#define BUTTON_DELAY	30

//-------------------------------------------------------------------------------------------------
// Variables
//-------------------------------------------------------------------------------------------------
static tmu_ram_texture_t gamesel_textures[] = {
	{"gs_bg00.wms",	BKGRND_TID},
	{"gs_bg01.wms",	BKGRND_TID},
	{"gs_bg02.wms",	BKGRND_TID},

	{"gs_fg00.wms",JAPPLE_BOX_TID},
	{"gs_fg01.wms",JAPPLE_BOX_TID},
	{"gs_fg02.wms",JAPPLE_BOX_TID},
	{"gs_fg03.wms",JAPPLE_BOX_TID},

	{"gs_lg.wms",JAPPLE_BOX_TID},		// Logos
	{"flash.wms", JAPPLE_BOX_TID},		// Sparkles

	{"gs_fl00.wms", JAPPLE_BOX_TID},
	{"gs_fl01.wms", JAPPLE_BOX_TID},

	{NULL,0}
};

static int logo_xy[][2] = 
{
	{ 318, 55 },
	{ 348, 28 },
	{ 363, 34 },
	{ 391, 46 },
	{ 418, 22 },
	{ 448, 58 },
	{ 482, 58 },
	{ 493, 49 },
	{ 500, 24 },
	{ 318, 55 },
	{ 348, 58 },
	{ 363, 28 },
	{ 391, 23 },
	{ 418, 44 },
	{ 448, 32 },
	{ 482, 40 },
	{ 493, 39 },
	{ 500, 45 },
};

static image_info_t *logo_flash_imgs[] =
{
	&sparkc01,&sparkc02,&sparkc03,&sparkc04,&sparkc05,
	&sparkc06,&sparkc07,&sparkc08,&sparkc09,&sparkc10
};

//-------------------------------------------------------------------------------------------------
//	Functions
//-------------------------------------------------------------------------------------------------
static void load_gamesel_textures(void);
static void logo_sparkle(int *args);
static void one_logo_sparkle(int *args);
static void flash_selected(int side);

extern int set_coin_hold(int);

//-------------------------------------------------------------------------------------------------
// game_select - Screen selects between Blitz and NBA Showtime.
//-------------------------------------------------------------------------------------------------
int game_select(int cur_game)
{
	sprite_info_t *bg;
	sprite_info_t *lcursor[3];
	sprite_info_t *rcursor[3];
	sprite_info_t *pcursor[3];
	sprite_info_t *logos[2];
	int done;
	int i;
	int cursor;
	int joy;
	int pjoy;
	int btn;
	int joy1, joy2, joy3, joy4;
	int btn1, btn2, btn3, btn4;
	int res;
	int btn_delay = BUTTON_DELAY;
	int alpha = MAX_FADE;
	int alpha_speed = -4;
	int delay_alpha = FADE_DELAY;

	done = 0;
	cursor = 0;
	res = 0;

	load_gamesel_textures();
	// draw background
	bg = beginobj(&bs_selpg,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	bg->id = 1;
	bg = beginobj(&bs_selpg_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	bg->id = 1;
	bg = beginobj(&bs_selpg_c2,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	bg->id = 1;
	bg = beginobj(&bs_selpg_c3,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND_TID);
	bg->id = 1;
	// Load up cursors;
	lcursor[0] = beginobj(&bs_lcovr,  		  0.0F, SPRITE_VRES, 400.0F, JAPPLE_BOX_TID);
	lcursor[0]->id = 1;
	lcursor[1] = beginobj(&bs_lcovr_c1,  		  0.0F, SPRITE_VRES, 400.0F, JAPPLE_BOX_TID);
	lcursor[1]->id = 1;
	lcursor[2] = beginobj(&bs_lcovr_c2,  		  0.0F, SPRITE_VRES, 400.0F, JAPPLE_BOX_TID);
	lcursor[2]->id = 1;

	rcursor[0] = beginobj(&bs_rcovr,  		  0.0F, SPRITE_VRES, 400.0F, JAPPLE_BOX_TID);
	rcursor[0]->id = 1;
	rcursor[1] = beginobj(&bs_rcovr_c1,  		  0.0F, SPRITE_VRES, 400.0F, JAPPLE_BOX_TID);
	rcursor[1]->id = 1;
	rcursor[2] = beginobj(&bs_rcovr_c2,  		  0.0F, SPRITE_VRES, 400.0F, JAPPLE_BOX_TID);
	rcursor[2]->id = 1;

	logos[0] = beginobj(&bs_blitz,  		  0.0F, SPRITE_VRES, 350.0F, JAPPLE_BOX_TID);
	logos[0]->id = 1;

	logos[1] = beginobj(&bs_show,  		  0.0F, SPRITE_VRES, 350.0F, JAPPLE_BOX_TID);
	logos[1]->id = 1;

	hide_sprite(rcursor[0]);
	hide_sprite(rcursor[1]);
	hide_sprite(rcursor[2]);

	hide_sprite(logos[1]);

	qcreate("flash", TIMER_PID, logo_sparkle, 0, 0, 0, 0);

//----[ SOUND ]-----
	sleep(10);
//	snd_stop_all();
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	coin_sound_holdoff(TRUE);
	// load game tune
//	strcpy(game_tune_bnk_str, game_tunes_tbl[3]);	// always L_MAIN -- for testing
	strcpy(game_tune_bnk_str, "intro1");
	if (snd_bank_load_playing(game_tune_bnk_str) == 0xeeee)
//	if (snd_bank_load_playing(qtr_tunes_tbl[game_info.game_quarter]) == 0xeeee)
		increment_audit(SND_LOAD_FAIL_AUD);
	// track 0 is the tune, track 1 is the audience, track 5 is the announcer
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	// Play drum loop
	snd_scall_bank(game_tune_bnk_str, 0, VOLUME4, 127, SND_PRI_SET|SND_PRI_TRACK0);
	coin_sound_holdoff(FALSE);

//------------------
	// Re-create the wipeout'd plate objects
	create_plates();
	// Take away the plates
	iqcreate("transit", 0x55aa|NODESTRUCT, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	qcreate("timer", TIMER_PID, timedown, SEL_PRETIME, SEL_TIME, TIMEOUT_X, TIMEOUT_Y);

	sleep(1);

	// Lets see them
	draw_enable(1);
	normal_screen();
	force_selection = 0;

	joy1 = (p_status & (1 << 0) ? get_joy_val_cur(0) : 0);
	joy2 = (p_status & (1 << 1) ? get_joy_val_cur(1) : 0);
	joy3 = (p_status & (1 << 2) ? get_joy_val_cur(2) : 0);
	joy4 = (p_status & (1 << 3) ? get_joy_val_cur(3) : 0);
	joy = pjoy = joy1 | joy2 | joy3 | joy4;

	while(!done)
	{
		pcursor[0] = (cursor ? rcursor[0] : lcursor[0] );
		pcursor[1] = (cursor ? rcursor[1] : lcursor[1] );
		pcursor[2] = (cursor ? rcursor[2] : lcursor[2] );

		pcursor[0]->state.constant_color = ((pcursor[0]->state.constant_color & 0xffffff) | (alpha <<24)); 
		pcursor[1]->state.constant_color = ((pcursor[1]->state.constant_color & 0xffffff) | (alpha <<24)); 
		pcursor[2]->state.constant_color = ((pcursor[2]->state.constant_color & 0xffffff) | (alpha <<24)); 


		if (!delay_alpha)
		{
			alpha += alpha_speed;
			if (alpha >= MAX_FADE)
			{
				alpha_speed *= -1;
				alpha = MAX_FADE;
			}
			if (alpha <= MIN_FADE)
			{
				alpha_speed *= -1;
				alpha = MIN_FADE;
			}
		} else {
			delay_alpha -= 1;
		}

		joy1 = (p_status & (1 << 0) ? get_joy_val_cur(0) : 0);
		joy2 = (p_status & (1 << 1) ? get_joy_val_cur(1) : 0);
		joy3 = (p_status & (1 << 2) ? get_joy_val_cur(2) : 0);
		joy4 = (p_status & (1 << 3) ? get_joy_val_cur(3) : 0);
		joy = joy1 | joy2 | joy3 | joy4;
		if (joy != pjoy)
		{
			pjoy = joy;
			if ((joy & JOY_RIGHT) && (cursor != 1))
			{
				snd_scall_bank(game_tune_bnk_str,1,VOLUME3,255,PRIORITY_LVL3);
				cursor = 1;
				alpha = MAX_FADE;
				alpha_speed = -4;
				delay_alpha = FADE_DELAY;

			}
			if ((joy & JOY_LEFT) && (cursor != 0))
			{
				snd_scall_bank(game_tune_bnk_str,1,VOLUME3, 0,PRIORITY_LVL3);
				cursor = 0;
				alpha = MAX_FADE;
				alpha_speed = -4;
				delay_alpha = FADE_DELAY;
			}

			if (cursor)
			{
				unhide_sprite(rcursor[0]);
				unhide_sprite(rcursor[1]);
				unhide_sprite(rcursor[2]);

				hide_sprite(lcursor[0]);
				hide_sprite(lcursor[1]);
				hide_sprite(lcursor[2]);

				hide_sprite(logos[0]);
				unhide_sprite(logos[1]);
			} else {
				hide_sprite(rcursor[0]);
				hide_sprite(rcursor[1]);
				hide_sprite(rcursor[2]);
				
				unhide_sprite(lcursor[0]);
				unhide_sprite(lcursor[1]);
				unhide_sprite(lcursor[2]);
				hide_sprite(logos[1]);
				unhide_sprite(logos[0]);
			}
		}

		btn1 = (p_status & (1<<0) ? get_but_val_down(0) : 0);
		btn2 = (p_status & (1<<1) ? get_but_val_down(1) : 0);
		btn3 = (p_status & (1<<2) ? get_but_val_down(2) : 0);
		btn4 = (p_status & (1<<3) ? get_but_val_down(3) : 0);
		btn = btn1|btn2|btn3|btn4;
		if (btn_delay > 0)
			btn_delay -= 1;
		if ( ((btn) && (!btn_delay)) || (force_selection))
		{
			done = 1;
			res = (cursor == cur_game ? FALSE : TRUE);
			snd_scall_bank(game_tune_bnk_str,2,VOLUME4, 127,PRIORITY_LVL3);
			flash_selected(cursor);
		}
		sleep(1);
	}

	sleep(40);

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

	draw_enable(0);

	kill_plates();

	// Cleanup
	killall( TIMER_PID, -1 );
	unlock_multiple_textures( JAPPLE_BOX_TID, -1 );
	delete_multiple_textures( JAPPLE_BOX_TID, -1 );
	unlock_multiple_textures( BKGRND_TID, -1 );
	delete_multiple_textures( BKGRND_TID, -1 );
	del1c( 1, -1 );

	snd_bank_delete(game_tune_bnk_str);

	// Start coin recorder
	set_coin_hold(TRUE);

	return (res);
}

//-------------------------------------------------------------------------------------------------
// load_sweeps_textures - Load all texture pages associate with this screen.
//-------------------------------------------------------------------------------------------------
static void load_gamesel_textures(void)
{
	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

//	imgs_loaded = 0;

	if (load_textures_into_tmu(gamesel_textures))
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
//	write_game_info - Write out p_status variable and number of credits and crutial info before
//						an exec command is called to go to NBA or NFL stuff
//-------------------------------------------------------------------------------------------------
void write_game_info(void)
{
	FILE *fp;
	int cnt;

	reload_info_t info;

	fp = fopen(RELOAD_FILE, "wb");
#if DEBUG
	if (!fp)
	{
		fprintf(stderr, "Can not open file %s\n", RELOAD_FILE);
	}
#endif

	if (fp)
	{
		// populate struct
		if (!four_plr_ver)
		{
			info.p_status = (p_status & 1) | ((p_status & 2) << 1);
			info.bought_in_first = (bought_in_first & 1) | ((bought_in_first & 2) << 1);

			info.quarters_purchased[0] = pdata_blocks[0].quarters_purchased;
			info.quarters_purchased[2] = pdata_blocks[1].quarters_purchased;
		}
		else
		{
			info.p_status = p_status;
			info.bought_in_first = bought_in_first;

			for (cnt = 0; cnt < MAX_PLYRS; cnt++)
				info.quarters_purchased[cnt] = pdata_blocks[cnt].quarters_purchased;
		}

		info.credits         = coins.credits;
		info.service_credits = coins.service_credits;
		info.coin_units      = coins.coin_units;
		info.bonus_units     = coins.bonus_units;
		info.remainder_units = coins.remainder_units;

		fwrite(&info, sizeof(reload_info_t), 1, fp);
		fflush(fp);
		fclose(fp);
#if 0
fprintf(stderr, "WRITE: p_status = %d\n", p_status);
fprintf(stderr, "WRITE: bought_in_first = %d\n", bought_in_first);
#endif
#if 0
fprintf(stderr, "WRITE: credits         =%d\n", coins.credits);
fprintf(stderr, "WRITE: service_credits =%d\n", coins.service_credits);
fprintf(stderr, "WRITE: coin_units      =%d\n", coins.coin_units);
fprintf(stderr, "WRITE: bonus_units     =%d\n", coins.bonus_units);
fprintf(stderr, "WRITE: remainder_units =%d\n", coins.remainder_units);
#endif
	}
}

//-------------------------------------------------------------------------------------------------
//	load_game_info - Load in game information needed when "reload" occurs from NBA to NFL and 
//						vice versa.
//-------------------------------------------------------------------------------------------------
void load_game_info(void)
{
	FILE *fp;
	int cnt;

	reload_info_t info;

	fp = fopen(RELOAD_FILE, "rb");
#if DEBUG
	if (!fp)
	{
		fprintf(stderr, "Can not open %s\n", RELOAD_FILE);
	}
#endif

	if (fp)
	{
		fread(&info, sizeof(reload_info_t), 1, fp);
		fclose(fp);
		remove(RELOAD_FILE);

		// update game variables
		if (!four_plr_ver)
		{
			p_status = p_status2 = (info.p_status & 1) | ((info.p_status & 4) >> 1);
			bought_in_first = (info.bought_in_first & 1) | ((info.bought_in_first & 4) >> 1);

			pdata_blocks[0].quarters_purchased = info.quarters_purchased[0];
			pdata_blocks[1].quarters_purchased = info.quarters_purchased[2];
		}
		else
		{
			p_status = p_status2 = info.p_status;
			bought_in_first = info.bought_in_first;

			for (cnt = 0; cnt < MAX_PLYRS; cnt++)
				pdata_blocks[cnt].quarters_purchased = info.quarters_purchased[cnt];
		}

		coins.credits         = info.credits;
		coins.service_credits = info.service_credits;
		coins.coin_units      = info.coin_units;
		coins.bonus_units     = info.bonus_units;
		coins.remainder_units = info.remainder_units;
#if 0
fprintf(stderr, "READ: p_status = %d\n", p_status);
fprintf(stderr, "READ: bought_in_first = %d\n", bought_in_first);
#endif
#if 0
fprintf(stderr, "READ: credits         =%d\n", coins.credits);
fprintf(stderr, "READ: service_credits =%d\n", coins.service_credits);
fprintf(stderr, "READ: coin_units      =%d\n", coins.coin_units);
fprintf(stderr, "READ: bonus_units     =%d\n", coins.bonus_units);
fprintf(stderr, "READ: remainder_units =%d\n", coins.remainder_units);
#endif
	}
}

//-------------------------------------------------------------------------------------------------
// logo_sparkle
//-------------------------------------------------------------------------------------------------
static void logo_sparkle(int *args)
{
	int x, y;
	int cnt, rnd;

	for (cnt=0; cnt < 100; cnt++)
	{
		rnd = randrng( sizeof(logo_xy) / (sizeof(int) * 2));
		x = logo_xy[rnd][0];
		y = logo_xy[rnd][1];
		qcreate("sparkle", TIMER_PID, one_logo_sparkle, x, y, 0, 0);
		sleep(randrng(230));
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void one_logo_sparkle(int *args)
{
	sprite_info_t *flash_obj;
	int x, y;
	int cnt;

	x = args[0];
	y = args[1];
	flash_obj = beginobj(logo_flash_imgs[0],x,y,9.0F, JAPPLE_BOX_TID);
	flash_obj->id = 1;
	flash_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	flash_obj->w_scale = 0.8f;	//3.0f;
	flash_obj->h_scale = 0.8f;	//3.0f;
	generate_sprite_verts(flash_obj);

	for (cnt=0; cnt < (int)(sizeof(logo_flash_imgs)/sizeof(image_info_t *))-1; cnt++)
	{
		sleep(3);
		change_img_tmu(flash_obj, logo_flash_imgs[cnt],JAPPLE_BOX_TID);
	}
	delobj(flash_obj);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void display_loading(int game)
{
	sprite_info_t *bg;

	if (game == NFL_RUNNING)
	{
		bg = beginobj(&pw_blitz,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(&pw_blitz_c1,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(&pw_blitz_c2,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(&pw_blitz_c3,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
	} else {
		bg = beginobj(&pw_show,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(&pw_show_c1,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(&pw_show_c2,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
		bg = beginobj(&pw_show_c3,	0.0f, SPRITE_VRES, 140.0f, 0xbeef);
	}	
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
static void flash_selected(int side)
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
	
	x = (side ? 384 : 128);
	y = 192;

	img_cnt = (sizeof(flash_imgs) / sizeof(image_info_t *));
	
	fl_obj = beginobj(flash_imgs[0], x, y, 9.0f, JAPPLE_BOX_TID);
	fl_obj->id = 1;
	fl_obj->state.alpha_rgb_dst_function = GR_BLEND_ONE;
	fl_obj->state.texture_filter_mode = GR_TEXTUREFILTER_BILINEAR;
	fl_obj->w_scale = 3.0f;
	fl_obj->h_scale = 3.0f;
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

//--------------------------------------------------------------------------------------------

#endif //VEGAS
