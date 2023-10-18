#if 0	// Not used

#include <stdio.h>
#include <goose/goose.h>
#include "include/fontid.h"
#include "include/game.h"
#include "include/id.h"
#include "include/select.h"
#include "include/Nfl.h"

static void _game_over(void);
void do_game_over_screens(void);

extern font_info_t			bast75_font;

void do_game_over_screens(void)
{
	_game_over();
	wipeout();
}
	

static void _game_over(void)
{
	font_node_t	*fn;

	if((fn = create_font(&bast75_font, FONTID_BAST75)) == NULL)
	{
#ifdef DEBUG
		lockup();
#endif
		return;
	}

	set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
	set_text_transparency_mode(TRANSPARENCY_ENABLE);
	set_string_id(0);
	set_text_bgnd_color(BLACK);
	set_text_color(LT_GREEN);
	set_text_font(FONTID_BAST75);
	set_text_z_inc(1.0f);
	set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES * 0.50f, 1.0f);
	oprintf("GAME OVER");
	set_text_z_inc(0.0f);

	if (plates_on >= PLATES_COMING_ON)
	{
		int i;

		// Wait for plates; set time-out so we don't hang
		i = PLATE_TRANSITION_TICKS * 2;
		// Do it this way since we can't know how many ticks have lapsed since transition began
		while (plates_on != PLATES_ON && --i) sleep(1);

		// Take away the plates only if the transition finished, not the time-out
		if (i)
		{
			iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);

			// Wait for plates
			sleep(PLATE_TRANSITION_TICKS+1);
		}
		// Make sure plates are gone
		plates_on = PLATES_OFF;
	}

//what the ???
//	load_permanent_snds();

	sleep(30);

	fade(0.0f, 20, NULL);

	sleep(30);

	delete_multiple_strings(0, -1);
}
#endif
