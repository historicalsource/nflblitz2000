/****************************************************************************/
/*                                                                          */
/* handicap.c - big pile of handicapping tables                             */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1997 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "include/game.h"
#include "include/player.h"
#include "include/plyrseq.h"
#include "include/handicap.h"
#include "/Video/Nfl/Include/Pup.h"

extern struct diff_set diff_settings[9];
int	random_drones, human_winstreak;

int user_base_adjust = 0;

play_t *play_hist[5];

#ifdef JASON
//#define SUPER_DRONES
#endif

///////////////////////////////////////////////////////////////////////////////
// set player handicap settings based on current situation.
void set_handicaps( void )
{
	int		losing,winning,score0,score1,diff,hteam;
	int		base, panic, dmin, hmax, dteam;
	int		p1adj=0, p2adj=0;			// adjustments from powerups
	int		repetition_bonus = 0;
	
	score0 = compute_score(0);
	score1 = compute_score(1);
	drones_winning = FALSE;
	
	diff = abs(score0 - score1);
	
	diff += diff * (LESSER(3,game_info.game_quarter)) / 2;
	
	if ((game_info.off_side == 0) &&
		(pup_offense & TEAM_MASK(0)))
		p1adj += 2;
	if ((game_info.off_side == 1) &&
		(pup_defense & TEAM_MASK(0)))
		p1adj += 2;
	if ((game_info.off_side == 1) &&
		(pup_offense & TEAM_MASK(1)))
		p2adj += 2;
	if ((game_info.off_side == 0) &&
		(pup_defense & TEAM_MASK(1)))
		p2adj += 2;
		
	if (pup_drones & TEAM_MASK(0))
		p1adj += 2;
	if (pup_drones & TEAM_MASK(1))
		p2adj += 2;
		
	p1adj = LESSER(p1adj,3);
	p2adj = LESSER(p2adj,3);
	
	if ((p_status & TEAM_MASK(0)) &&
		(p_status & TEAM_MASK(1)))
	{
		// human vs human
		if ((diff < 2) || pup_noassist )
		{
			game_info.team_handicap[0] = 5 + p1adj;
			game_info.team_handicap[1] = 5 + p2adj;
			return;
		}
	
		winning = (score1 > score0);
		losing = !winning;
	
		game_info.team_handicap[winning] = 5-(diff/4);
		game_info.team_handicap[losing] = 5+((diff+2)/4);
		game_info.team_handicap[0] += p1adj;
		game_info.team_handicap[1] += p2adj;
		game_info.team_handicap[0] = LESSER(game_info.team_handicap[0],10);
		game_info.team_handicap[1] = LESSER(game_info.team_handicap[1],10);
		game_info.team_handicap[0] = GREATER(game_info.team_handicap[0],0);
		game_info.team_handicap[1] = GREATER(game_info.team_handicap[1],0);
	}
	else if( p_status == 0 )
	{
		// drone vs drone
		game_info.team_handicap[0] = teaminfo[game_info.team[0]].drone_base;
		game_info.team_handicap[1] = teaminfo[game_info.team[1]].drone_base;
#ifdef JASON
		game_info.team_handicap[0] = 5;
		game_info.team_handicap[1] = 5;
#endif
		return;
	}
	else
	{
		// human vs cpu

		hteam = (p_status & TEAM_MASK(0)) ? 0 : 1;
		winning = (score1 > score0);
		losing = !winning;

		// if human is on offense, record their play
		if (hteam == game_info.off_side)
		{
			int		i;
			// increment repetition_bonus for every time we've seen
			// this play among the last five.
			for(i = 0; i < 5; i++)
				if (play_hist[i] == game_info.team_play[hteam])
					repetition_bonus++;

//			fprintf( stderr, "current play: %x\n", game_info.team_play[hteam] );
//			fprintf( stderr, "last five plays:\n" );
//			fprintf( stderr, "%x\n", play_hist[0] );
//			fprintf( stderr, "%x\n", play_hist[1] );
//			fprintf( stderr, "%x\n", play_hist[2] );
//			fprintf( stderr, "%x\n", play_hist[3] );
//			fprintf( stderr, "%x\n", play_hist[4] );
//			fprintf( stderr, "rep bonus = %d\n", repetition_bonus );

			play_hist[4] = play_hist[3];
			play_hist[3] = play_hist[2];
			play_hist[2] = play_hist[1];
			play_hist[1] = play_hist[0];
			play_hist[0] = game_info.team_play[hteam];

		}

		
		if (random_drones)
			dteam = TEAM_BEARS;
		else
			dteam = game_info.team[!hteam];
			
		base = teaminfo[dteam].drone_base;
		base += user_base_adjust;

//fprintf(stderr, "base = %d / user = %d\n", base, user_base_adjust);

		if (human_winstreak >= 2)
			base += 1;
		if (human_winstreak >= 4)
			base += 1;
		base = LESSER(11,base);
		
		panic = diff_settings[base-3].panic;
		dmin = diff_settings[base-3].drone_min;
		hmax = diff_settings[base-3].human_max;
		
		if (base < 4)
			drones_lose = TRUE;
		else
			drones_lose = FALSE;
			
		if ((winning == hteam) || !diff)
		{
			// tie or humans winning
			game_info.team_handicap[hteam] = GREATER(0,(5-(diff/4)));
			game_info.team_handicap[!hteam] = LESSER(11,(base+((diff+2)/4)));
			
			if (diff < panic)
				game_info.team_handicap[!hteam] = LESSER(10,game_info.team_handicap[!hteam]);
		}
		else
		{
			// drone team winning
			drones_winning = TRUE;
			
			if (drones_lose)
			{
				game_info.team_handicap[hteam] = 10;
				game_info.team_handicap[!hteam] = 0;
			}
			else
			{
				game_info.team_handicap[winning] = GREATER(dmin,(base-(diff/4)));
				game_info.team_handicap[losing] = LESSER(hmax,(5+((diff+2)/4)));
			}
		}
		
		// repetition bonus
		game_info.team_handicap[hteam] -= repetition_bonus / 2;
		game_info.team_handicap[!hteam] += (repetition_bonus+1) / 2;
		
		#ifdef SUPER_DRONES
		if (1)
		#else
		if (pup_superdrones)
		#endif
		{
			game_info.team_handicap[hteam] = GREATER(5,game_info.team_handicap[hteam]);
			game_info.team_handicap[!hteam] = 11;
			drones_lose = FALSE;
		}

		game_info.team_handicap[hteam] += (hteam ? p2adj : p1adj);
		game_info.team_handicap[hteam] = LESSER(game_info.team_handicap[hteam],10);
		
	}
}

///////////////////////////////////////////////////////////////////////////////
// when a ball carrier is past the line of scrimmage,
// defensive backs choose randomly among these speeds
float dback_speed_ht[12][6] = {
	{1.60f, 1.55f, 1.50f, 1.45f, 1.40f, 1.35f},
	{1.65f, 1.60f, 1.55f, 1.50f, 1.45f, 1.40f},
	{1.70f, 1.65f, 1.60f, 1.55f, 1.50f, 1.45f},
	{1.75f, 1.70f, 1.65f, 1.60f, 1.55f, 1.50f},
	{1.80f, 1.75f, 1.70f, 1.65f, 1.60f, 1.55f},
	
	{1.85f, 1.80f, 1.75f, 1.70f, 1.65f, 1.60f},
	
	{1.85f, 1.85f, 1.80f, 1.75f, 1.70f, 1.65f},
	{1.85f, 1.85f, 1.85f, 1.80f, 1.75f, 1.70f},
	{1.90f, 1.85f, 1.85f, 1.85f, 1.80f, 1.75f},
	{1.90f, 1.90f, 1.85f, 1.85f, 1.85f, 1.80f},
	{1.90f, 1.90f, 1.90f, 1.85f, 1.85f, 1.85f},
	{2.00f, 2.00f, 2.00f, 2.00f, 2.00f, 2.00f},
};

// when a ball carrier is past the line of scrimmage,
// defensive linemen choose randomly among these speeds
float dline_speed_ht[12][6] = {
	{1.65f, 1.60f, 1.55f, 1.50f, 1.45f, 1.40f},
	{1.65f, 1.60f, 1.55f, 1.50f, 1.45f, 1.40f},
	{1.65f, 1.60f, 1.55f, 1.50f, 1.45f, 1.40f},
	{1.65f, 1.60f, 1.55f, 1.50f, 1.45f, 1.40f},
	{1.65f, 1.60f, 1.55f, 1.50f, 1.45f, 1.40f},
	
	{1.65f, 1.60f, 1.55f, 1.50f, 1.45f, 1.40f},

	{1.70f, 1.65f, 1.55f, 1.50f, 1.45f, 1.45f},
	{1.70f, 1.65f, 1.55f, 1.55f, 1.45f, 1.45f},
	{1.70f, 1.65f, 1.55f, 1.55f, 1.50f, 1.45f},
	{1.75f, 1.70f, 1.60f, 1.55f, 1.50f, 1.45f},
	{1.80f, 1.75f, 1.65f, 1.55f, 1.50f, 1.50f},
	{2.00f, 2.00f, 2.00f, 2.00f, 2.00f, 2.00f},
};

// they take column1 + randrng(column2) ticks to speed up
int def_acceltime_ht[12][2] = {
	{20,90},
	{18,80},
	{16,70},
	{14,60},
	{12,50},
	
	{10,45},
	
	{8,35},
	{6,30},
	{4,22},
	{2,15},
	{0,8},
	{0,1},
};

///////////////////////////////////////////////////////////////////////////////
// open-field bobble probabilities
int of_bobble_ht[12] = { 6,5,5,4,3, 2, 1,0,0,0,0, 0 };

///////////////////////////////////////////////////////////////////////////////
// base time for DBs to react to thrown ball
//int db_preact_ht[12] = { 65,60,55,45,35, 30, 22,15,10,8,4, 1 };
int db_preact_ht[12] = { 65,60,55,50,45, 45, 45,40,30,20,10, 1 };

///////////////////////////////////////////////////////////////////////////////
// base time for rushers to go into blitz mode on blitz plays
int blitz_time_ht[12] = { 90,75,70,60,55, 50, 35,30,13,7,2, 1 };

///////////////////////////////////////////////////////////////////////////////
// probability that carrier will do a stiff-arm/spin when appropriate
int drn_evade_ht[12] = { 5,10,15,25,30, 70, 80,90,95,100,100, 100 };

///////////////////////////////////////////////////////////////////////////////
// dist downfield from los for which drone QB will scan for receivers
int drn_qb_range_ht[12] = { 8,8,12,12,15, 30, 40,50,100,100,100, 100};

///////////////////////////////////////////////////////////////////////////////
// base chance of ball going up on a bobble
int bob_up_ht[12] = { 75,70,65,60,55, 50, 40,30,20,15,5, 0 };

///////////////////////////////////////////////////////////////////////////////
// cap on bobble percentage
int bob_cap_ht[12] = { 99,99,99,99,99, 99, 50,45,40,30,35, 0};

///////////////////////////////////////////////////////////////////////////////
// max spins/hurdles per play by drones
int spin_cap_ht[12] = { 1,1,1,1,1, 2, 2,2,2,3,4, 99 };
int hurdle_cap_ht[12] = { 1,1,1,1,1, 2, 3,3,4,4,5, 10 };

///////////////////////////////////////////////////////////////////////////////
// base chance of a bobble for human, human's drone teammate, and drone team
int bobble_human_ht[12] = { 50,55,60,65,70, 75, 80,85,90,95,100, 100 };
int bobble_hteam_ht[12] = { 30,35,40,45,50, 55, 64,73,82,91,100, 100 };
int bobble_drone_ht[12] = { 40,45,50,55,60, 65, 72,79,86,93,100, 100 };
// chance of NOCATCH defender catching anyway
int nocatch_ht[12] = {0,0,0,0,0, 0, 10,15,20,25,30, 100};

///////////////////////////////////////////////////////////////////////////////
// bonus to field goal success
//int fg_bonus_ht[12] = { -10,-10,-8,-8,-4, 0, 4,8,12,16,20, 200 };
int fg_bonus_ht[12] = { -10,-10,-8,-8,-4, 0, 4,8,12,16,20, 200 };

///////////////////////////////////////////////////////////////////////////////
// chance of doing head plow move
int plow_ht[12] = { 50,60,70,80,90, 100, 100,100,100,100,100, 100 };

///////////////////////////////////////////////////////////////////////////////
// chance of drone db's doing an early hit on the int recvr
int early_hit_ht[12] = { 0,0,10,15,20, 30, 40,60,80,100,100, 100 };

///////////////////////////////////////////////////////////////////////////////
// chance of drone db's any kind of modal attack on receiver
int drone_db_att_ht[12] = { 0,0,15,20,30, 50, 60,70,80,100,100, 100 };

///////////////////////////////////////////////////////////////////////////////
// chance of standing db bobbling a pass
int db_stand_bobble_ht[12] = {100,80,60,40,30, 20, 10,5,0,0,0, 0};

///////////////////////////////////////////////////////////////////////////////
// chance of airborne db batting down a pass
int db_air_batdown_ht[12] = {100,80,60,40,30, 20, 10,5,0,0,0, 0};

///////////////////////////////////////////////////////////////////////////////
// multiplier to bozo cookie strength
float bozo_cookie_ht[12] = { 4.0f,3.0f,2.0f,1.6f,1.3f, 1.0f, 0.8f,0.6f,0.4f,0.3f,0.20f, 0.0f };

// base bozo time
int bozo_base_ht[12] = {24,20,16,14,12, 12, 10,8,8,6,6, 0};
int bozo_rand_ht[12] = {60,50,40,34,28, 20, 18,16,14,12,10, 1};

// zone def adjustment times
int zone_thinkmin_ht[12] = {30,25,23,20,18, 15, 13,10,8,5,2, 0};
int zone_thinkmax_ht[12] = {60,50,45,40,35, 30, 25,20,15,10,5, 0};

///////////////////////////////////////////////////////////////////////////////
// where do you catch quick kickoffs
int min_ko_catch_ht[12] = {-8,-7,-7,-6,-5, -5, -2,1,4,7,10, 15};
int max_ko_catch_ht[12] = {2,3,5,7,8, 10, 11,13,13,14,15, 16};

///////////////////////////////////////////////////////////////////////////////
// chance of drone db/rushers using turbo+A attacks on carrier
int def_hteam_dive_ht[12] = {0,4,8,12,16, 20, 36,52,68,84,100, 100};
//int def_drone_dive_ht[12] = {2,12,20,30,35, 50, 65,80,85,95,100, 100};
int def_drone_dive_ht[12] = {5,18,30,40,50, 65, 75,85,95,99,99, 99};
// distance at which defenders will dive
//float dive_range_ht[12] = {44.0f,42.0f,40.0f,38.0f,36.0f, 34.0f, 32.0f,30.0f,30.0f,30.0f,30.0f, 30.0f};

///////////////////////////////////////////////////////////////////////////////
// chance of a lineman letting a human escape from lockup on change_player
int cp_breakout_ht[12] = {80,75,70,60,50, 40, 30,20,10,5,0, 0};

///////////////////////////////////////////////////////////////////////////////
// chance that a drone db on a drone team will knock down a receiver before
// the ball is thrown
//int db_cheapshot_ht[12] = {0,0,0,0,0, 5, 5,10,15,20,25, 100};
int db_cheapshot_ht[12] = {5,10,15,20,25, 30, 35,40,45,50,55, 60};

///////////////////////////////////////////////////////////////////////////////
// chance that a drone db on ANY team will knock down a receiver before
// the ball is thrown IF THE DEFENSE IS BLITZING
int db_cheapshot_blitz_ht[12] = {30,35,40,48,55, 66, 75,80,85,90,95, 100};

///////////////////////////////////////////////////////////////////////////////
// chance that a drone defensive player on a drone team will be an unblockable
// rushing monster
int def_superman_ht[12] = {0,0,0,0,0, 5, 5,10,20,30,40, 80};

///////////////////////////////////////////////////////////////////////////////
// column shifts in favor of blockers
int oline_block_ht[12] = {-5,-4,-3,-2,-1,0,1,2,3,4,5, 5};

///////////////////////////////////////////////////////////////////////////////
// qb stiff arm... if winning less often
//int qb_stiff_arm_ht[12] = {5,5,5,5,5, 5, 5,7,10,20,30, 45};
int qb_stiff_arm_ht[12] = {8,8,8,8,8, 8, 8,10,20,35,45, 45};

///////////////////////////////////////////////////////////////////////////////
