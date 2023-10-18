/****************************************************************************/
/*                                                                          */
/* playbook.h - Prototypes and definitions for playbook.c                   */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* Copyright (c) 1997 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#ifndef __PLAYBOOK_H__
#define __PLAYBOOK_H__

#include "PlayStuf.h"

extern fline_t for_punt[];
extern fline_t for_pro_set[];
extern fline_t for_shotgun[];
extern fline_t for_std_def[];

// offensive plays
// Team specific plays

// General plays used on first 4 pages for any team
extern play_t play_gen_1;
extern play_t play_gen_2;
extern play_t play_gen_3;
extern play_t play_gen_4;
extern play_t play_gen_5;
extern play_t play_gen_6;

// Arizona
extern play_t play_az_1;
extern play_t play_az_2;
extern play_t play_az_3;

// Atlanta
extern play_t play_at_1;
extern play_t play_at_2;
extern play_t play_at_3;

// Baltimore
extern play_t play_ba_1;
extern play_t play_ba_2;
extern play_t play_ba_3;

// Buffalo
extern play_t play_bu_1;
extern play_t play_bu_2;
extern play_t play_bu_3;

// Carolina
extern play_t play_ca_1;
extern play_t play_ca_2;
extern play_t play_ca_3;

// Chicago
extern play_t play_ch_1;
extern play_t play_ch_2;
extern play_t play_ch_3;

// Cincinatti
extern play_t play_ci_1;
extern play_t play_ci_2;
extern play_t play_ci_3;










extern play_t play_QBdeep;
extern play_t play_side_slide;
extern play_t play_hooknladder;
extern play_t play_spider;
extern play_t play_zigzag;
extern play_t play_monkey;
extern play_t play_switch;
extern play_t play_cruisn;
extern play_t play_hurricane;

extern play_t play_upper_cut;
extern play_t play_blizzard;
extern play_t play_hail_mary;
extern play_t play_quake_shake;
extern play_t play_upthe_gut;
extern play_t play_criss_cross;
extern play_t play_dawg_hook;
extern play_t play_blitz_beater;
extern play_t play_screen_rgt;
extern play_t play_one_man_back;
extern play_t play_cross_slant;
extern play_t play_reverse_zip;
extern play_t play_sweep_rgt;
extern play_t play_middle_pick;
extern play_t play_da_bomb;
extern play_t play_utb_deep;
extern play_t play_subzero;
extern play_t play_razzle_dazzle;

extern play_t play_extra_point;
extern play_t play_punt;
extern play_t play_fake_punt;
extern play_t play_field_goal;
extern play_t play_fake_field_goal;

// defensive plays
extern play_t play_3_man_cover;
extern play_t play_goal_line;
extern play_t play_2_man_blitz;
extern play_t play_near_zone;
extern play_t play_suicide_blitz;
extern play_t play_safe_cover;
extern play_t play_3_man_blitz;
extern play_t play_medium_zone;
extern play_t play_deep_zone;

extern play_t play_stuff_it;
extern play_t play_stuff_it2;
extern play_t play_fake_zone;
extern play_t play_knock_down;
extern play_t play_left_zone;
extern play_t play_right_zone;
extern play_t play_stop_run;
extern play_t play_mix_it_up;
extern play_t play_spread;
extern play_t play_man_on_man;


extern play_t play_block_punt;
extern play_t play_block_fg;
extern play_t play_punt_return;

// special plays
extern play_t play_kickoff_313;
extern play_t play_kickoff2_313;
extern play_t play_onside_313;
extern play_t play_kick_return;
extern play_t play_onside_kick_return;
extern play_t play_kickoff_short;
extern play_t play_kick_return_short;

// scripts
extern int ds_block[];
extern int db_rush_qb[];
extern int ds_improvise[];
extern int db_zone_sl3[];
extern int db_zone_sr3[];

#endif
