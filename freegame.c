#include <stdio.h>
#include <stdlib.h>
#include	<string.h>
#include <dos.h>
#include	<dir.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/nflcmos.h"
#include "/Video/Nfl/Include/select.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/gameadj.h"
#include "/Video/Nfl/Include/trivia.h"
#include "/Video/Nfl/Include/FreeGame.h"

extern float station_xys[MAX_PLYRS][2][2];
extern float japbx_cntr_xys[MAX_PLYRS][2][2];

void plyr_strt_butn(int sig, int swid);

static int	timeout;

static void dummy(int sig, int id)
{
}

static void free_game_timer(int *args)
{
	void * old_dcsw[MAX_PLYRS];
	int	pnum = args[0];
	int	seconds = 10;
	int	sw_mask = 0;
	int	half_second;
	int	sw = 0;

	old_dcsw[0] = set_dcsw_handler(P1_START_SWID, dummy);
	old_dcsw[1] = set_dcsw_handler(P2_START_SWID, dummy);
	if (four_plr_ver)
	{
		old_dcsw[2] = set_dcsw_handler(P3_START_SWID, dummy);
		old_dcsw[3] = set_dcsw_handler(P4_START_SWID, dummy);
	}

	if (pnum & 1)
		sw_mask |= P1_START_SW;
	if (pnum & 2)
		sw_mask |= P2_START_SW;
	if (pnum & 4)
		sw_mask |= P3_START_SW;
	if (pnum & 8)
		sw_mask |= P4_START_SW;

	while(--seconds >= 0)
	{
		set_text_position(SPRITE_HRES/2.0f, 81.0f +SPRITE_VRES/384.0f, 1.1f);
		set_text_color(LT_CYAN);
		set_text_font(FONTID_BAST18);
		set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
		set_string_id(0x55a9);

		oprintf("%d", seconds);

		snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME4,127,PRIORITY_LVL3);
		half_second = tsec;	//(tsec * 3) / 4;
		while(--half_second && !((sw = get_dip_coin_current() & sw_mask)))
		{
			sleep(1);
		}
		delete_multiple_strings(0x55a9, -1);
		if (sw) break;

		snd_scall_bank(cursor_bnk_str,CLOCK_LOW_SND,VOLUME4,127,PRIORITY_LVL3);
		half_second = 25;	//tsec / 4;
		while(--half_second && !((sw = get_dip_coin_current() & sw_mask)))
		{
			sleep(1);
		}
		if (sw) break;
	}

	set_dcsw_handler(P1_START_SWID, old_dcsw[0]);
	set_dcsw_handler(P2_START_SWID, old_dcsw[1]);
	if (four_plr_ver)
	{
		set_dcsw_handler(P3_START_SWID, old_dcsw[2]);
		set_dcsw_handler(P4_START_SWID, old_dcsw[3]);
	}

	bought_in_first = 0;

	if (sw)
	{
		if (sw & P1_START_SW)
			bought_in_first = 1;
		else
		if (sw & P2_START_SW)
			bought_in_first = 2;
		else
		if (sw & P3_START_SW)
			bought_in_first = 4;
		else
		if (sw & P4_START_SW)
			bought_in_first = 8;

		timeout = -1;
	}
	else
	{
		timeout = 1;
	}
}

#ifdef DEBUG
void show_player_free_game_test(int min, int max)
{
	int i;
	draw_enable(1);

	// Turn on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way since we can't know how many ticks have lapsed since transition began
	while (plates_on != PLATES_ON && --i) sleep(1);
	// Do sound only if the transition finished, not the time-out
	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME3, 127, PRIORITY_LVL5);

	while(1)
	{
		int sw = get_player_sw_current();
		if (!four_plr_ver) sw &= 0x0ffff;
		if (sw)
		{
			int pnum = 0;
			int p = 1;
			while (sw)
			{
				if (sw & (P_A|P_B|P_C)) pnum |= p;
				sw >>= 8;
				p <<= 1;
			}
			show_player_free_game(pnum);
		}
		sleep(2);
	}
}
#endif

int show_player_free_game(int pnum)
{
	process_node_t * pflasher;
	sprite_info_t * psprite;
	ostring_t * pmsg0;
	ostring_t * pmsg1;
	float ypos, yinc;
	float fx, fy;
	int i, j;

	// Kill everything but the plates
	draw_enable(0);

	wipeout();

	// Re-create the wipeout'd plate objects
	create_plates();

	// Put up the background
	draw_backdrop();

	// Set up all of the text drawing stuff
	set_text_font(FONTID_BAST25);
	set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
	set_string_id(0x55aa);
	set_text_z_inc(0.0f);

	switch (pnum)
	{
		case 4:
		case 12:
			i = LT_RED;
			break;
		case 8:
			i = LT_YELLOW;
			break;
		case 2:
			i = LT_GREEN;
			break;
		default:
			i = LT_BLUE;
			break;
	}
	set_text_color(i);

	yinc = (float)get_font_height(FONTID_BAST25) + 3.0f;

	ypos = 306.0f +SPRITE_VRES/384.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 1.1f);
	pmsg0 = oprintf("CONGRATULATIONS");

	ypos -= yinc;
	if (is_low_res) ypos -= 8.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 1.1f);
	if (pnum == 3 || pnum == 12)
		pmsg1 = oprintf("PLAYERS %d & %d!", pnum==3?1:3, pnum==3?2:4);
	else
		pmsg1 = oprintf("PLAYER %d!", pnum<4?pnum:(pnum==4?3:4));

	fx = (float)((int)(SPRITE_HRES - trivbox.w - trivbox_c1.w) >> 1);
	fy = ypos + (float)(((int)(trivbox.h + yinc) >> 1));
	if (is_low_res) fy += 6.0f;
	psprite = beginobj(&trivbox,    fx, fy, 499.0f, JAPPLE_BOX_TID);
	psprite->id = 1;
	//psprite->w_scale = 1.0f;
	//psprite->h_scale = 1.0f;
	//generate_sprite_verts(psprite);
	psprite = beginobj(&trivbox_c1, fx, fy, 499.0f, JAPPLE_BOX_TID);
	psprite->id = 1;
	//psprite->w_scale = 1.0f;
	//psprite->h_scale = 1.0f;
	//generate_sprite_verts(psprite);

	pflasher = qcreate("pflash2", 0, flash_text_proc, (int)pmsg0, (int)pmsg1, 0, 0);

	set_text_color(WHITE);

	ypos -= yinc * 2.0f;
	if (is_low_res) ypos -= 3.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 1.1f);
	oprintf("PRESS START NOW FOR");

	ypos -= yinc;
	if (is_low_res) ypos -= 8.0f;
	set_text_position(SPRITE_HRES/2.0f, ypos, 1.1f);
	oprintf("A FREE GAME!");

	j = pnum;
	i = 0;
	while (j)
	{
		if (j & 1)
		{
			// Show japple box
			qcreate("credbox", CREDIT_PID, plyr_credit_box,
				i,
				station_xys[i][four_plr_ver][X_VAL],
				station_xys[i][four_plr_ver][Y_VAL],
				JAPPLE_BOX_TID);

			print_players_name(i,
				japbx_cntr_xys[i][four_plr_ver][X_VAL],
				japbx_cntr_xys[i][four_plr_ver][Y_VAL],
				NULL,
				-1);
		}
		j >>= 1;
		i++;
	}

	draw_enable(1);

	// Only remove plates if they're there (should be!)
	if (plates_on == PLATES_ON)
	{
		// Take away the plates
		iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);

		// Wait for plates
		sleep(PLATE_TRANSITION_TICKS+1);

		// Make sure plates are gone
		plates_on = PLATES_OFF;
	}

	snd_scall_bank(generic_bnk_str,19,VOLUME4,127,SND_PRI_SET|SND_PRI_TRACK0);

	// Let it be seen for up to 5 seconds
	timeout = 0;
	iqcreate("time", 0, free_game_timer, pnum, 0, 0, 0);

	while(!timeout)
	{
		sleep(1);
	}

	snd_stop_track(SND_TRACK_0);
	snd_set_reserved(SND_TRACK_0|SND_TRACK_1|SND_TRACK_5);

	// Turn on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way since we can't know how many ticks have lapsed since transition began
	while (plates_on != PLATES_ON && --i) sleep(1);
	// Do sound only if the transition finished, not the time-out
	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME3, 127, PRIORITY_LVL5);

	// Delete the whole pile of crap
	killall(CREDIT_PID, -1);
	kill(pflasher, 0);
	sleep(1);
	delete_multiple_strings(0x55aa, -1);

	// Delete the backgound
	del1c(1, -1);
	del1c(OID_JAPPLE_BOX, -1);

	if(timeout > 0)
	{
		return(0);
	}
	return(1);
}
