/******************************************************************************/
/*                                                                            */
/* showpnam.c  - seperate file that only compiles at opt level 0              */
/*                                                                            */
/* Written by:  DJT                                                           */
/* $Revision: 20 $                                                            */
/*                                                                            */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                      */
/* All Rights Reserved                                                        */
/*                                                                            */
/******************************************************************************/
#include	<math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/ani3D.h"
#include "/Video/Nfl/Include/coin.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/CamBot.h"
#include "/Video/Nfl/Include/Showpnam.h"

#define OFF_NAME_DY	(-6.0f)
#define DEF_NAME_DY	(16.0f)

#define BUF_X	(6.0f)
#define BUF_Y	(2.0f)

#define KXFAC	((hres*0.5f)-(hres*0.5f)*FOCFAC)
#define KYFAC	((vres*0.5f)-(vres*0.5f)*FOCFAC)

char showpnam_upd[NUM_PLAYERS];

void show_receiver_name_proc(int *);


/*****************************************************************************/
// show_player_names_proc()
//
void show_player_names_proc(int *parg)
{
	int was_low_res = is_low_res;

	if (game_info.play_type != PT_KICKOFF && game_info.game_state != GS_ATTRACT_MODE) {
		sprite_info_t * names_obj[NUM_PLAYERS];

		register sprite_info_t * psi;
		register sprite_info_t * psj;
		register float psx, psy;
		register int obtn, dbtn, team, i, j;

		// UnFucking Believeable that This Needs to be here!
		//  Sync-up with <last_player> coordinate updating
		sleep(4);

		i = NUM_PLAYERS;
		while (i--) {
			switch (player_blocks[i].position) {
				case PPOS_QBACK:
				case PPOS_WPN1:
				case PPOS_WPN2:
				case PPOS_WPN3:
				//case PPOS_LBACK1:
				//case PPOS_LBACK2:
				//case PPOS_DBACK1:
				//case PPOS_DBACK2:
					// Get img *; null if isn't one
					if (((image_info_t *)psi = player_blocks[i].static_data->pii_name)) {

						// Make sure you put this back before sleeping!!!
						is_low_res = 0;

						psi = beginobj((image_info_t *)psi, 0, 0, 2.5f, SCR_PLATE_TID);
						psi->id = OID_TURBARROW;

						// Back it goes!!!
						is_low_res = was_low_res;

						psi->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
						psi->state.constant_color = LT_YELLOW;

						hide_sprite(psi);

						// Flag a coordinate recalc
						showpnam_upd[i] = -1;
					}
					names_obj[i] = psi;
					break;

				default:
					names_obj[i] = NULL;
					break;
			}
		}

		// Loop here till dead
		while (game_info.game_mode != GM_IN_PLAY) {
			// Get button states to hide/unhide names
			if (game_info.off_side) {
//				obtn = P2_C;
				obtn = (four_plr_ver) ? P3_C|P4_C : P2_C;
//				dbtn = P1_C;
				dbtn = (four_plr_ver) ? P1_C|P2_C : P1_C;
			}
			else {
//				obtn = P1_C;
//				dbtn = P2_C;
				obtn = (four_plr_ver) ? P1_C|P2_C : P1_C;
				dbtn = (four_plr_ver) ? P3_C|P4_C : P2_C;
			}
			obtn &= (team = get_player_sw_close());
			dbtn &= (team);

			// Don't show names if instructions box is up
			if (idiot_boxes_shown < 0) {
				obtn = 0;
				dbtn = 0;
			}

			i = NUM_PLAYERS;
			while (i--) {
				if ((psi = names_obj[i])) {
					// 1 if on offense, 0 if on defense
					team = (int)(player_blocks[i].team == game_info.off_side);

					// Recalc coordinates if init or player moved or camera moved
					if (showpnam_upd[i] < 0 ||
							fabs(player_blocks[i].odata.vx) > 0.0001f ||
							fabs(player_blocks[i].odata.vy) > 0.0001f ||
							fabs(player_blocks[i].odata.vz) > 0.0001f ||
							fabs(cambot.dv) > 0.25f) {
						// Calc desired position & set recalc flag
						psy  = (team) ? (OFF_NAME_DY*hres*FOCFAC) : (DEF_NAME_DY*hres*FOCFAC);
						psy /= player_blocks[i].sz;

						psi->x_end = (float)(int)(player_blocks[i].sx * FOCFAC + KXFAC);
						psi->y_end = player_blocks[i].sy * FOCFAC + KYFAC + psy;

						if (was_low_res)
							// Reduced version of (-vres/2, * 2/3, +vres/2)
							psi->y_end = (4.0f * psi->y_end + vres) * 0.16666666f;
						psi->y_end = (float)(int)(psi->y_end);

						showpnam_upd[i] = 1;
					}
					else
						showpnam_upd[i] = 0;

					// If previous != desired, update position & set recalc flag
					psx = psi->x_end;
					psy = psi->y_end;
					if (psi->x != psx) {
						psi->x = psx;
						showpnam_upd[i] = 1;
					}
					if (psi->y != psy) {
						psi->y = psy;
						showpnam_upd[i] = 1;
					}

					// Chk if its time to hide/unhide names
					if ((team && obtn) || (!team && dbtn))
						((sprite_node_t *)psi->node_ptr)->mode ^= HIDE_SPRITE;
				}
				else
					// Clr update flag
					showpnam_upd[i] = 0;
			}
//#if 0
			// Chk for any overlaps; should reitterate till no overlap
			i = NUM_PLAYERS - 1;
			do {
				if (!(psi = names_obj[i]))
					continue;

				j = i;
				while (j--) {
					if (!(psj = names_obj[j]))
						continue;

					// Y intersection? Continue if not
					psx = psi->y;
					psy = psj->y;
					if (psx <= (psy - (psj->ii->h + BUF_Y)) ||
							psy <= (psx - (psi->ii->h + BUF_Y)))
						continue;
					// X intersection? Continue if not
					if (psi->x > (psj->x + (psj->ii->w + BUF_X)) ||
							psj->x > (psi->x + (psi->ii->w + BUF_X)))
						continue;
					// Intersection; move whichever is higher
					if (psx < psy) {
						//psj->y += 1.0f;
						psj->y += psj->ii->h - psy + psx + BUF_Y;
						showpnam_upd[j] = 1;
					}
					else {
						//psi->y += 1.0f;
						psi->y += psi->ii->h - psx + psy + BUF_Y;
						showpnam_upd[i] = 1;
					}
					i = NUM_PLAYERS;
					break;
				}
			} while (--i);
//#endif
			// Update positions
			i = NUM_PLAYERS;
			while (i--) {
				if (!showpnam_upd[i])
					continue;

				// Make sure you put this back before sleeping!!!
				is_low_res = 0;

				generate_sprite_verts(names_obj[i]);

				// Back it goes!!!
				is_low_res = was_low_res;
			}
			sleep(1);
		}

		// Delete all the names
		i = NUM_PLAYERS;
		while (i--) {
			if ((psi = names_obj[i]))
				delobj(psi);
		}
	}
	die(0);
}

/*****************************************************************************/
// show_receiver_name_proc()
//
void show_receiver_name_proc(int *parg)
{
	fbplyr_data	* ppdata = (fbplyr_data *)parg[0];
	sprite_info_t * psi;
	float psx, psy;
	int upd = -1;
	int ticks = 57 * 7/4;
	int was_low_res = is_low_res;

	// Chk name img *; null if isn't one
	if (((image_info_t *)psi = ppdata->static_data->pii_name))
	{
		// Make sure you put this back before sleeping!!!
		is_low_res = 0;

		psi = beginobj((image_info_t *)psi, 0, 0, 2.5f, SCR_PLATE_TID);
		psi->id = OID_TURBARROW;

		// Back it goes!!!
		is_low_res = was_low_res;


		psi->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
		psi->state.constant_color = LT_YELLOW;

		// Loop here till dead
		while  (game_info.game_mode == GM_IN_PLAY &&
				game_info.ball_carrier == ppdata->plyrnum &&
				ticks--)
		{
			// Recalc coordinates if init or player moved or camera moved
			if (upd < 0 ||
					fabs(ppdata->odata.vx) > 0.0001f ||
					fabs(ppdata->odata.vy) > 0.0001f ||
					fabs(ppdata->odata.vz) > 0.0001f ||
					fabs(cambot.dv) > 0.25f) {
				// Calc desired position & set recalc flag
				psy  = (OFF_NAME_DY*hres*FOCFAC);
				psy /= ppdata->sz;

				psi->x_end = (float)(int)(ppdata->sx * FOCFAC + KXFAC);
				psi->y_end = ppdata->sy * FOCFAC + KYFAC + psy;

				if (was_low_res)
					// Reduced version of (-vres/2, * 2/3, +vres/2)
					psi->y_end = (4.0f * psi->y_end + vres) * 0.16666666f;
				psi->y_end = (float)(int)(psi->y_end);

				upd = 1;
			}
			else
				upd = 0;

			// If previous != desired, update position & set recalc flag
			psx = psi->x_end;
			psy = psi->y_end;
			if (psi->x != psx) {
				psi->x = psx;
				upd = 1;
			}
			if (psi->y != psy) {
				psi->y = psy;
				upd = 1;
			}
			// Update position
			if (upd)
			{
				// Make sure you put this back before sleeping!!!
				is_low_res = 0;

				generate_sprite_verts(psi);

				// Back it goes!!!
				is_low_res = was_low_res;
			}
			sleep(1);
		}

		// Delete the name
		delobj(psi);
	}
	die(0);
}
