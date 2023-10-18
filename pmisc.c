/****************************************************************************/
/*                                                                          */
/* pmisc.c - miscellaneous data and functions that might otherwise go in    */
/*           player.c, cuz player.c is sure getting big.                    */
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
#include <string.h>
#include <goose/goose.h>
#include <math.h>
#include <ctype.h>
#include <ioctl.h>

#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/pproto.h"
#include "/Video/Nfl/Include/handicap.h"
#include "/Video/Nfl/Include/profile.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/CamBot.h"
#include "/Video/Nfl/Include/Drone.h"
#include "/Video/Nfl/Include/PMisc.h"
#include "/Video/Nfl/Anim/genseq.dbg"

int force_fumble, force_bobble;
extern char *last_catch;
extern int p2p_angles[NUM_PLAYERS][NUM_PLAYERS];

extern void deflected_ball_sp(void);
extern int last_pass_hist[5];
extern int target_human;
extern int freeze;

void check_heap( void );
void check_free( void );
int get_heap_available( void );

///////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
char *animode_names[] = { "MODE_END", "MODE_UNINT", "MODE_TWOPART",
		"MODE_INTSTREAM", "MODE_INTXITION", "MODE_ROTONLY", "MODE_ZERO",
		"MODE_REACTING", "MODE_CANBLOCK", "MODE_NOCYL", "MODE_STIFF_ARM",
		"MODE_NOCATCH", "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
		"MODE_ANCHORY", "MODE_ANCHORXZ", "MODE_SECONDARY", "MODE_INTERP",
		"MODE_ANIDEBUG", "MODE_TURNHACK", "MODE_PUPPETEER", "MODE_PUPPET",
		"UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
		"UNDEFINED", "UNDEFINED", "UNDEFINED", NULL };

///////////////////////////////////////////////////////////////////////////////
char *ball_flag_names[] = { "BF_WHITE", "BF_VEL_PITCH", "BF_TURBO_PASS",
		"BF_FIRE_PASS", "BF_BOBBLE", "BF_BOUNCE", "BF_ALT", "BF_FREEZE",
		"BF_TGT_OFFSCRN", "BF_DEFLECTED", "BF_SPINING", "BF_SPINING",
		"BF_FG_BOUNCE", "BF_FG_HITPOST", NULL };

///////////////////////////////////////////////////////////////////////////////
char *loose_ball_names[] = { "LB_PASS", "LB_FUMBLE", "LB_PUNT", "LB_KICKOFF",
		"LB_FIELD_GOAL", "LB_BOBBLE", "LB_LATERAL", NULL };

///////////////////////////////////////////////////////////////////////////////
char *player_flag_names[] = { "PF_FLIP", "PF_ONSCREEN", "PF_HICOUNT", "PF_TURBO",
		"PF_OLD_TURBO", "PF_NOCOLL", "PF_ANITICK", "PF_INBND", "PF_CELEBRATE",
		"PF_REVWPN", "PF_FLASHWHITE", "PF_BACKPEDAL", "PF_LATEHITABLE", "PF_WAVE_ARM",
		"PF_DIZZY", "PF_SPEED_OVERRIDE", "PF_DBUT_ANIM_TWOPART", "PF_UNBLOCKABLE", NULL };

///////////////////////////////////////////////////////////////////////////////
char *game_flag_names[] = { "GF_REC_IN_EZ", "GF_LEFT_EZ", "GF_THROUGH_UPRIGHTS",
		"GF_POSS_CHANGE", "GF_BOBBLE", "GF_FD_LINE_CROSSED", "GF_FWD_PASS",
		"GF_PASS_THROWN", "GF_BALL_PUNTED", "GF_BALL_KICKED", "GF_PLAY_ENDING",
		"GF_FUMBLE", "GF_QUICK_KICK", "GF_AIR_CATCH", "GF_BALL_CROSS_LOS",
		"GF_HIKING_BALL", "GF_ONSIDE_KICK", "GF_OLS_SPEEDS_SET",
		"GF_OPENING_KICK", NULL };

///////////////////////////////////////////////////////////////////////////////
char *game_mode_names[] = { "GM_LINING_UP", "GM_PRE_SNAP", "GM_IN_PLAY",
		"GM_PLAY_OVER", "GM_GAME_OVER", "GM_PRE_KICKOFF" };

///////////////////////////////////////////////////////////////////////////////
char *play_type_names[] = { "PT_NORMAL", "PT_KICKOFF", "PT_FIELD_GOAL", "PT_PUNT",
		"PT_EXTRA_POINT" };

///////////////////////////////////////////////////////////////////////////////
char *play_end_cause_names[] = { "PEC_CAR_OOB", "PEC_CAR_TACKLED_IB",
		"PEC_CAR_TACKLED_OWNEZ", "PEC_CAR_TOUCHDOWN", "PEC_DEAD_BALL_IB",
		"PEC_BALL_OOB" };

///////////////////////////////////////////////////////////////////////////////
char *down_names[] = { "-1", "0", "1st", "2nd", "3rd", "4th" };

///////////////////////////////////////////////////////////////////////////////
char *plyrmode_names[] = { "MODE_LINEUP", "MODE_DEAD", "MODE_LINEPS", "MODE_LINE",
		 "MODE_QBPS", "MODE_QB", "MODE_WPNPS", "MODE_WPN", "MODE_DBPS", "MODE_DB",
		 "MODE_KICKER" };
		 
///////////////////////////////////////////////////////////////////////////////
char *job_names[] = { "JOB_NONE", "JOB_RUSH", "JOB_COVER", "JOB_BLOCK",
		"JOB_CATCH", "JOB_BREAKUP", "JOB_ZONE" };
#endif

///////////////////////////////////////////////////////////////////////////////
void set_defense_speeds( void )
{
	int		i, h, x;

	h = game_info.team_handicap[!game_info.off_side];

	// for now, just assign speeds randomly from the list
	for( i = (!game_info.off_side) * 7; i < (!game_info.off_side) * 7 + 7; i++ )
	{
		x = randrng(6);
		if( player_blocks[i].odata.plyrmode == MODE_LINE )
			player_blocks[i].speed = dline_speed_ht[h][x];
		else
			player_blocks[i].speed = dback_speed_ht[h][x];

		if( game_info.team_play[!game_info.off_side]->flags & PLF_REDZONE)	// Blitz delay
		{
			// set blitzers to 0-40 tick delay
#ifdef DEBUG
//			fprintf( stderr, "Red Zone flagged play - speed up override time!\n" );
#endif
			player_blocks[i].override_time = randrng(20);
//			player_blocks[i].override_time = def_acceltime_ht[h][0] +
//					randrng(def_acceltime_ht[h][1]);
		}
		else
			player_blocks[i].override_time = def_acceltime_ht[h][0] +
					randrng(def_acceltime_ht[h][1]);
	}
}

//////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void dump_game_state( int mode )
{
	int i;

	fprintf( stderr, "\ngame_info:\n" );
	fprintf( stderr, "  p_status = %x\n", p_status );
	fprintf( stderr, "  qtr_pstatus = %x, %x, %x, %x\n", qtr_pstatus[0], qtr_pstatus[1], qtr_pstatus[2], qtr_pstatus[3] );
	fprintf( stderr, "  game_mode = %s\n", game_mode_names[game_info.game_mode] );
	fprintf( stderr, "  off_side = %1d  off_init = %d\n", game_info.off_side, game_info.off_init );
	fprintf( stderr, "  drones_lose = %s\n", drones_lose ? "TRUE" : "FALSE" );
	fprintf( stderr, "  plyr_control = %d %d %d %d\n",
		game_info.plyr_control[0],
		game_info.plyr_control[1],
		game_info.plyr_control[2],
		game_info.plyr_control[3] );
	fprintf( stderr, "  game_quarter: %d  game_time: %d:%d:%d\n", game_info.game_quarter,
		((game_info.game_time>>16) & 0xff),
		((game_info.game_time>>8) & 0xff),
		(game_info.game_time & 0xff));
	fprintf( stderr, "  captain = %d\n", game_info.captain );
	fprintf( stderr, "  last_pos = %d\n", game_info.last_pos );
	fprintf( stderr, "  los = %d  old_los = %d\n", game_info.los, game_info.old_los );
	fprintf( stderr, "  %s down, first at %d\n", down_names[game_info.down-1], game_info.first_down );
	fprintf( stderr, "  team_play = %p %p\n", game_info.team_play[0], game_info.team_play[1] );
	fprintf( stderr, "  ball_carrier = %d(%2x)\n", game_info.ball_carrier, (game_info.ball_carrier == -1 ) ? 0xFF : JERSEYNUM(game_info.ball_carrier) );
	fprintf( stderr, "  play_type = %s\n", play_type_names[game_info.play_type] );
	fprintf( stderr, "  play_end_cause = %s\n", play_end_cause_names[game_info.play_end_cause] );
	fprintf( stderr, "  auto_hike_cntr = %d\n", game_info.auto_hike_cntr );
	fprintf( stderr, "  team_handicap = %d %d\n", game_info.team_handicap[0], game_info.team_handicap[1] );
	fprintf( stderr, "  game_flags:" );
	for( i = 0; game_flag_names[i]; i++ )
		if( game_info.game_flags & (1<<i))
			fprintf( stderr, " %s", game_flag_names[i] );
	fprintf( stderr, "\n" );
	fprintf( stderr, "  cambot mode = %d\n", cambot.csi.mode );

	if( game_info.ball_carrier == -1 )
	{
		fprintf( stderr, "\n ball position: %f %f %f\n",
			ball_obj.odata.x, ball_obj.odata.y, ball_obj.odata.z );
		fprintf( stderr, " ball velocity: %f %f %f\n",
			ball_obj.odata.vx, ball_obj.odata.vy, ball_obj.odata.vz );
		fprintf( stderr, " ball target: %f %f\n",
			ball_obj.tx, ball_obj.tz );
	}

	if (mode)
	{
		fprintf( stderr, "tdata:\n" );
		fprintf( stderr, " time of poss: %3d %3d\n", tdata_blocks[0].time_of_poss, tdata_blocks[1].time_of_poss );
		fprintf( stderr, "    off plays: %3d %3d\n", tdata_blocks[0].off_plays, tdata_blocks[1].off_plays );
		fprintf( stderr, "      fumbles: %3d %3d\n", tdata_blocks[0].fumbles, tdata_blocks[1].fumbles );
		fprintf( stderr, " lost fumbles: %3d %3d\n", tdata_blocks[0].lost_fumbles, tdata_blocks[1].lost_fumbles );
		fprintf( stderr, "interceptions: %3d %3d\n", tdata_blocks[0].interceptions, tdata_blocks[1].interceptions );
		fprintf( stderr, "  4th dn atts: %3d %3d\n", tdata_blocks[0].forthdwn_attempts, tdata_blocks[1].forthdwn_attempts);
		fprintf( stderr, " 4th dn convs: %3d %3d\n", tdata_blocks[0].forthdwn_convs, tdata_blocks[1].forthdwn_convs);
		fprintf( stderr, "      tackles: %3d %3d\n", tdata_blocks[0].tackles, tdata_blocks[1].tackles);
		fprintf( stderr, "        sacks: %3d %3d\n", tdata_blocks[0].sacks, tdata_blocks[1].sacks);
		fprintf( stderr, "   sack_yards: %3d %3d\n", tdata_blocks[0].sack_yards, tdata_blocks[1].sack_yards );
		fprintf( stderr, "       passes: %3d %3d\n", tdata_blocks[0].passes, tdata_blocks[1].passes );
		fprintf( stderr, "  completions: %3d %3d\n", tdata_blocks[0].completions, tdata_blocks[1].completions );
		fprintf( stderr, "passing_yards: %3d %3d\n", tdata_blocks[0].passing_yards, tdata_blocks[1].passing_yards );
		fprintf( stderr, "       rushes: %3d %3d\n", tdata_blocks[0].rushes, tdata_blocks[1].rushes );
		fprintf( stderr, "rushing_yards: %3d %3d\n", tdata_blocks[0].rushing_yards, tdata_blocks[1].rushing_yards );
		fprintf( stderr, " kick_returns: %3d %3d\n", tdata_blocks[0].kick_returns, tdata_blocks[1].kick_returns );
		fprintf( stderr, " return_yards: %3d %3d\n", tdata_blocks[0].return_yards, tdata_blocks[1].return_yards );
		fprintf( stderr, "  total_yards: %3d %3d\n", tdata_blocks[0].total_yards, tdata_blocks[1].total_yards );
		fprintf( stderr, "\n" );
		fprintf( stderr, "last_catch: %p\n", last_catch );
	}
}
#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void dump_anim_data( int mode )
{
	int			i,j;
	fbplyr_data	*ppdata;
	obj_3d		*pobj;

	fprintf( stderr, "\n" );
	for( j = 0; j < 14; j++ )
	{
		ppdata = player_blocks + j;
		pobj = (obj_3d *)ppdata;
		if( pobj->adata[0].animode & MODE_INTSTREAM )
		{
			fprintf( stderr, "\n%2x: %6s %9s:%5.2f",
					ppdata->static_data->number,
					plyrmode_names[pobj->plyrmode]+5,
					seq_names[pobj->adata[0].seq_index], pobj->adata[0].fframe );
		}
		else if( pobj->adata[0].animode & MODE_INTERP )
		{
			fprintf( stderr, "\n%2x: %6s %9s:%2d to %10s:%2d (%f)",
					ppdata->static_data->number,
					plyrmode_names[pobj->plyrmode]+5,
					seq_names[pobj->adata[0].seq_index2], pobj->adata[0].iframe,
					seq_names[pobj->adata[2].seq_index], pobj->adata[2].iframe,
					(float)(pobj->adata[0].icnt) / (float)(pobj->adata[2].icnt) );
		}
		else
		{
			fprintf( stderr, "\n%2x: %6s %9s:%-5d",
					ppdata->static_data->number,
					plyrmode_names[pobj->plyrmode]+5,
					seq_names[pobj->adata[0].seq_index], pobj->adata[0].iframe );
		}

		fprintf( stderr, " x,z: %f %f  FIELDX: %d\n", pobj->x, pobj->z, FIELDX(pobj->x) );

		if( pobj->adata[1].animode & MODE_TWOPART )
		{
			fprintf( stderr, "           %9s:%2d\n",
				seq_names[pobj->adata[1].seq_index], pobj->adata[1].iframe );
		}

		if( pobj->adata[0].animode )
		{
			fprintf( stderr, "  animode: " );
			for( i = 0; animode_names[i]; i++ )
			{
				if( pobj->adata[0].animode & (1<<i))
					fprintf( stderr, " %s", animode_names[i]+5 );
			}
			fprintf( stderr, "\n" );
		}

		if( pobj->adata[1].animode & MODE_TWOPART )
		{
			if( pobj->adata[1].animode )
			{
				fprintf( stderr, "  animode: " );
				for( i = 0; animode_names[i]; i++ )
				{
					if( pobj->adata[1].animode & (1<<i))
						fprintf( stderr, " %s", animode_names[i]+5 );
				}
				fprintf( stderr, "\n" );
			}
		}

		if( pobj->flags )
		{
			fprintf( stderr, "  plrflgs: " );
			for( i = 0; player_flag_names[i]; i++ )
			{
				if( pobj->flags & (1<<i))
				{
					fprintf( stderr, " %s", player_flag_names[i]+3 );
					if( (1<<i) == PF_SPEED_OVERRIDE )
						fprintf( stderr, "(%4.2f)", player_blocks[j].speed );
				}
			}
		}
		fprintf( stderr, "\n" );
		fprintf( stderr, "  job: %s\n", job_names[ppdata->job] );
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
void deflect_pass( obj_3d *pobj )
{
	float	v[3], mag;
	float	loss,t;
	int		i;
	fbplyr_data	*ppdata = (fbplyr_data *)pobj;

	// make speech calls
	deflected_ball_sp();
	
	// inc stat.
	tdata_blocks[ppdata->team].deflections++;
	
	v[0] = ball_obj.odata.vx;
	v[1] = ball_obj.odata.vy;
	v[2] = ball_obj.odata.vz;

	mag = fsqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );

	if( mag < 0.001f )
		return;

	v[0] /= mag;
	v[1] /= mag;
	v[2] /= mag;

	// figure out how much velocity we lose
	i = 65 + randrng(31);

	loss = ((float)i) / 100.0f;

	mag *= loss;

	// convert 10 to 90 percent of our xz velocity to y
	if( randrng(4))
		i = 10 + randrng(41);
	else
		i = 50 + randrng(41);

	loss = ((float)i) / 100.0f;

	v[1] += loss*(v[0] + v[1]);
	v[0] *= (1.0f - loss);
	v[2] *= (1.0f - loss);

	// rotate -256 to +256 bdegrees in y
	i = randrng(257) - randrng(257);

	ball_obj.odata.vx = FRELX(v[0]*mag,v[2]*mag,i);
	ball_obj.odata.vy = v[1]*mag;
	ball_obj.odata.vz = FRELZ(v[0]*mag,v[2]*mag,i);
	
	ball_obj.odata.vy = LESSER(ball_obj.odata.vy,2.7f);

	// set new target x
	t = solve_quad( 0.5f * GRAVITY, ball_obj.odata.vy, ball_obj.odata.y );

	ball_obj.tx = ball_obj.odata.x + t * ball_obj.odata.vx;
	ball_obj.tz = ball_obj.odata.z + t * ball_obj.odata.vz;

	// update who_threw
	ball_obj.flags |= BF_DEFLECTED;
	
	// have ball tumble...
	ball_obj.flags &= ~BF_VEL_PITCH;
}

///////////////////////////////////////////////////////////////////////////////
float solve_quad( float a, float b, float c )
{
	float	b2m4ac, r1, r2, r;

	if( fabs(c) < 0.001f )
		return 0.0f;

	if(( fabs( b ) < 0.001f ) && ( fabs( a ) < 0.001f ))
		return 200000.0f;

	if( fabs( a ) < 0.001f )
		return( -1.0f * c / b );

	if( fabs( b ) < 0.001f )
		if(( -c / a ) < 0.0f )
			return 300000.0f;
		else
			return fsqrt( -c / a );

	b2m4ac = (b*b) - (4.0f*a*c);

	if( b2m4ac < 0.0f )
		return 100000.0f;			// no real solution

	r1 = (-1.0f * b + fsqrt(b2m4ac)) / (2.0f * a);
	r2 = (-1.0f * b - fsqrt(b2m4ac)) / (2.0f * a);

	if(( r1 < 0.0f ) && ( r2 < 0.0f ))
		return GREATER( r1, r2 );	// both are negative

	if(( r1 >= 0.0f ) && ( r2 >= 0.0f ))
		return LESSER( r1, r2 );	// both are nonnegative
	else
		return GREATER( r1, r2 );	// one is nonnegative
}

///////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void print_ball_flags( void )
{
	int i;
	
	for( i = 0; ball_flag_names[i]; i++ )
		if( (1<<i) & ball_obj.flags )
			fprintf( stderr, "%s  ", ball_flag_names[i] );
			
	fprintf( stderr, "\n" );
}
#endif

///////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void print_game_flags( void )
{
	int i;
	
	for( i = 0; game_flag_names[i]; i++ )
		if( (1<<i) & game_info.game_flags )
			fprintf( stderr, "%s ", game_flag_names[i]+3 );
			
	fprintf( stderr, "\n" );
}
#endif

///////////////////////////////////////////////////////////////////////////////
static void show_commands( void );
void do_blitz_menu( void );

#if 0
static char	*ld_commands[] = {
"A - dump anim data",
"B - bobble on next hit",
"D - set down",
"F - fumble on next hit",
"G - dump game state",
"H - halt game processes",
"P - last pass data",
"R - show heap available",
"S - dump drone states",
"C - continue",
"Anything else - Show This Help"
};

static void show_commands(void)
{
	unsigned	i;

	for(i = 0; i < sizeof(ld_commands)/sizeof(void *); i++)
	{
		fprintf(stderr, "%s\r\n", ld_commands[i]);
	}
	fprintf(stderr, "Command:  ");
}

void do_blitz_menu( void )
{
	int	done = 0;
	int	cont = 1;
	int	c,d;

	while(!done)
	{
		show_commands();
		c = getchar();
		fprintf(stderr, "%c\r\n", c);
		switch(toupper(c))
		{
			case 'A':
				dump_anim_data(0);
				break;

			case 'B':				// Force bobble
				fprintf( stderr, "next hit will bobble\n" );
				force_bobble = TRUE;
				force_fumble = FALSE;
				break;

			case 'C':				// Continue execution
				cont = 1;
				done = 1;
				break;

			case 'D':				// set down
				fprintf( stderr, "set down to (1-4):" );
				d = getchar() - '0';
				if ((d<1) || (d>4))
					fprintf( stderr, "invalid entry\n" );
				else
					game_info.down = d;
				break;

			case 'F':				// Force fumble
				fprintf( stderr, "next hit will fumble\n" );
				force_fumble = TRUE;
				force_bobble = FALSE;
				break;

			case 'G':
				dump_game_state(TRUE);
				break;

			case 'H':
				freeze = TRUE;
				break;

			case 'P':
				fprintf( stderr, "target_human = %s\n", target_human ? "TRUE" : "FALSE" );
				fprintf( stderr, "%d %d %d %d %d\n",
					last_pass_hist[0],
					last_pass_hist[1],
					last_pass_hist[2],
					last_pass_hist[3],
					last_pass_hist[4] );
				break;

			case 'R':
				check_heap();
				check_free();
				fprintf( stderr, "heap available = %d\n", get_heap_available());
				#ifdef MALLOC_DEBUG
				check_fences();
				dump_alloclist();
				#endif
				break;

			case 'S':				// dump drone states
				dump_drone_states();
				break;

			default:
			{
				break;
			}
		}
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// returns TRUE if player wants to intercept ball, FALSE if he wants to
// bat it down.
int intercept_ok( obj_3d *pobj )
{
	if( game_info.down != 4 )
		return TRUE;
		
	if( FIELDX(pobj->x) < (game_info.los+5) )
		return TRUE;
		
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
float pass_rating( int att, int comp, int yds, int tds, int ints )
{
	float	a,b,c,d,r;
	
	if (att <= 0)
		return 0.0f;
		
	comp = GREATER(comp,0);
	tds = GREATER(tds,0);
	ints = GREATER(ints,0);
	
	a = (((float)comp / (float)att * 100.0f) - 30.0f) / 20.0f;
	b = (float)tds / (float)att * 20.0f;
	c = (9.5f - (float)ints / (float)att * 100.0f) / 4.0f;
	d = ((float)yds / (float)att - 3.0f) / 4.0f;
	
	r = (a+b+c+d) / 0.06f;
	
	return r;
}

//////////////////////////////////////////////////////////////////////////////
void norm2( float *v )
{
	float	mag;

	mag = fsqrt( v[0]*v[0] + v[1]*v[1] );

	if( mag <= 0.00001f )
	{
		v[0] = 0.0f;
		v[1] = 0.0f;
		return;
	}

	v[0] /= mag;
	v[1] /= mag;
}

///////////////////////////////////////////////////////////////////////////////
// returns the bearing on plyr2 from plyr1
// bearing is the angle between plyr1's facing direction and the vector
// connecting plyr1 to plyr2.
int bearing( int plyr1, int plyr2 )
{
	int		b;
	
	b = player_blocks[plyr1].odata.fwd - p2p_angles[plyr1][plyr2];
	
	b = (1024 + b) % 1024;
	
	b = (b > 512) ? 1024 - b : b;
	
	return b;
}

///////////////////////////////////////////////////////////////////////////////
// updates the captain_player fields in game_info.
// updates captain_player fields and player links
// to stations.
// call this after ANY change to plyr_control[]
void update_stations( void )
{
	int			i;

	// clear & reset player station handles
	for( i = 0; i < 14; i++ )
		player_blocks[i].station = -1;

	if (!four_plr_ver)
	{
		//FIX!!!???
		// This appears to be very wrong; doesn't <plyr_control> matter here too?
		player_blocks[game_info.plyr_control[0]].station = 0;
		player_blocks[game_info.plyr_control[1]].station = 1;
		return;
	}

	for( i = 0; i < 4; i++ )
	{
		if( game_info.plyr_control[i] != -1 )
			player_blocks[game_info.plyr_control[i]].station = i;
	}

	if( p_status & 0x3 )
	{
		int		lcaptain = game_info.off_side ? game_info.captain : off_captain();

		if( (p_status & 0x3) == 0x3 )
			game_info.captain_player[0] = game_info.plyr_control[lcaptain];
		else
			game_info.captain_player[0] = game_info.plyr_control[(p_status & 0x3) - 1];
	}
	else
	{	// no humans on team 1
		game_info.captain_player[0] = -1;
	}

	if( p_status & 0xC )
	{
		int		lcaptain = game_info.off_side ? off_captain() : game_info.captain;

		if( (p_status & 0xC) == 0xC )
			game_info.captain_player[1] = game_info.plyr_control[lcaptain+2];
		else
			game_info.captain_player[1] = game_info.plyr_control[((p_status >> 2) & 0x3) + 1];
	}
	else
	{	// no humans on team 1
		game_info.captain_player[1] = -1;
	}
#ifdef DEBUG
	fprintf(stderr,"captain_player[0]= %d   captain_player[1]= %d\r\n",game_info.captain_player[0],game_info.captain_player[1]);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// returns captain for offensive side, adjusted for always_receiver and
// always_qb powerups
int off_captain( void )
{
	int		p1,p2;

	p1 = 1<<(game_info.off_side * 2);
	p2 = p1<<1;

	// powerups have no effect in 2-player kit
	if (!four_plr_ver)
		return game_info.captain;

	// are there two humans on team?
	if ((p_status & (p1|p2)) != (p1|p2))
		return game_info.captain;

	// verify that powerups are in order.  That is:
	// - no player has both always_qb and always_receiver
	// - no powerup is held by both teammates

	if ((pup_always_qb & p1) &&
		(pup_always_receiver & p1))
	{
		pup_always_qb ^= p1;
		pup_always_receiver ^= p1;
		#ifdef DEBUG
		fprintf( stderr, "Warning: player %d has qb AND receiever powerups\n",
			game_info.off_side * 2 );
		#endif
	}

	if ((pup_always_qb & p2) &&
		(pup_always_receiver & p2))
	{
		pup_always_qb ^= p2;
		pup_always_receiver ^= p2;
		#ifdef DEBUG
		fprintf( stderr, "Warning: player %d has qb AND receiever powerups\n",
			game_info.off_side * 2 + 1 );
		#endif
	}

	if ((pup_always_qb & (p1|p2)) == (p1|p2))
	{
		pup_always_qb ^= (p1|p2);
		#ifdef DEBUG
		fprintf( stderr, "Warning: teammates on pup_always_qb\n" );
		#endif
	}
	if ((pup_always_receiver & (p1|p2)) == (p1|p2))
	{
		pup_always_receiver ^= (p1|p2);
		#ifdef DEBUG
		fprintf( stderr, "Warning: teammates on pup_always_receiver\n" );
		#endif
	}

	// now if either player has either powerup, act thereon
	if ((pup_always_qb & p1) || (pup_always_receiver & p2))
		return 0;

	if ((pup_always_qb & p2) || (pup_always_receiver & p1))
		return 1;

	return game_info.captain;
}

#ifdef PROFILE
///////////////////////////////////////////////////////////////////////////////
xtime_t	fprofiles[NUM_PROFILES];
int		nprofiles = 0;

///////////////////////////////////////////////////////////////////////////////
void reset_profiles( int sw_state, int sw_id )
{
	int		i;
	char	buf[20];

	if(sw_state != SWITCH_CLOSE)
		return;

	for( i = 0; i < NUM_PROFILES; i++ )
		fprofiles[i].name[0] = '\0';

	fprintf( stderr, "profiles reset\n" );

	fprintf( stderr, "\n" );

	init_profile( "assign_blockers", PRF_ABLOCK );
	init_profile( "player_detect_proc", PRF_COLLIS );
	init_profile( "player_shadow", PRF_SHADOW );
	init_profile( "last_player_proc", PRF_LASTPLAYER );
	init_profile( "player_field1", PRF_FIELD1 );
	init_profile( "player_field2", PRF_FIELD2 );
	init_profile( "receiver_update", PRF_RDATA );

	for( i = 0; i < 14; i++ )
	{
		sprintf( buf, "player %d", i+1 );
		init_profile( buf, PRF_PLAYER1+i );
	}
}

///////////////////////////////////////////////////////////////////////////////
void profile_dump( int sw_state, int sw_id )
{
	int		i;
	long	total = 0;

	if(sw_state != SWITCH_CLOSE)
		return;

	fprintf( stderr, "%20s%9s%9s%9s%9s%9s\n",
		"function", "high", "low", "mean", "last", "n" );

	for( i = 0; i < NUM_PROFILES; i++ )
	{
		if (fprofiles[i].name[0] == '\0')
			continue;

		fprintf( stderr, "%20s%9ld%9ld%9ld%9ld%9ld\n",
			fprofiles[i].name,
			fprofiles[i].high * 100L,
			fprofiles[i].low * 100L,
			fprofiles[i].mean * 100L,
			(fprofiles[i].lstop - fprofiles[i].lstart) * 100L,
			fprofiles[i].n );

		total += fprofiles[i].lstop - fprofiles[i].lstart;
	}

	fprintf( stderr, "%56d\n", total*100 );

	fprintf( stderr, "\n" );
}

///////////////////////////////////////////////////////////////////////////////
void init_profile( char *name, int id )
{
	if ((id < 0) || (id >= NUM_PROFILES))
	{
		fprintf( stderr, "init_profile(): Bogus id\n" );
		return;
	}

	if (fprofiles[id].name[0])
	{
		fprintf( stderr, "init_profile(): Attempt to reuse profile %d (%s) by %s\n",
			id, fprofiles[id].name, name );
		return;
	}

	strncpy( fprofiles[id].name, name, PFNAMELEN-1 );
	fprofiles[id].name[PFNAMELEN-1] = '\0';
	fprofiles[id].high = 0;
	fprofiles[id].low = 0;
	fprofiles[id].mean = 0;
	fprofiles[id].n = 0;
	fprofiles[id].lstart = 0;
	fprofiles[id].lstop = 0;
	fprofiles[id].state = 0;
}

///////////////////////////////////////////////////////////////////////////////
void pftimer_start( int id )
{
	int		ftime;

	if ((id < 0) || (id >= NUM_PROFILES))
	{
		fprintf( stderr, "pftimer_start(): Bogus id\n" );
		return;
	}

	if (!fprofiles[id].name[0])
		return;

	if (fprofiles[id].state != 0)
		return;

   	_ioctl (5, FIOCGETTIMER3, (int)&ftime);
	ftime /= 100;
	fprofiles[id].lstart = ftime;
	fprofiles[id].state = 1;
}

///////////////////////////////////////////////////////////////////////////////
void pftimer_end( int id )
{
	long	ftime;

	if ((id < 0) || (id >= NUM_PROFILES))
	{
		fprintf( stderr, "pftimer_end(): Bogus id\n" );
		return;
	}

	if (!fprofiles[id].name[0])
		return;

	if (fprofiles[id].state == 0)
		return;

   	_ioctl (5, FIOCGETTIMER3, (int)&ftime);

	ftime /= 100;

	fprofiles[id].lstop = ftime;

	ftime -= fprofiles[id].lstart;

	if (ftime == 0)
	{
		fprofiles[id].state = 0;
		return;
	}

	if (fprofiles[id].n)
	{
		fprofiles[id].low = LESSER( ftime, fprofiles[id].low );
		fprofiles[id].high = GREATER( ftime, fprofiles[id].high );
		fprofiles[id].mean = (fprofiles[id].mean * fprofiles[id].n + ftime) /
							(fprofiles[id].n+1);
	}
	else
	{
		fprofiles[id].low = ftime;
		fprofiles[id].high = ftime;
		fprofiles[id].mean = ftime;
	}

	fprofiles[id].n++;
	fprofiles[id].state = 0;
}

///////////////////////////////////////////////////////////////////////////////
#endif

extern sprite_node_t *sprite_node_list;

//////////////////////////////////////////////////////////////////////////////
// hide all the sprites in a string
void hide_string( void *parent )
{
	register sprite_node_t	*snode = sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
			hide_sprite( snode->si );

		snode = snode->next;
	}
}

//////////////////////////////////////////////////////////////////////////////
// unhide all the sprites in a string
void unhide_string( void *parent )
{
	register sprite_node_t	*snode = sprite_node_list;

	// Walk a sprite list
	while(snode)
	{
		// Node we are looking for ?
		if(snode->si->parent == parent)
			unhide_sprite( snode->si );

		snode = snode->next;
	}
}
///////////////////////////////////////////////////////////////////////////////
void update_gamestart_percents( void )
{
	recalc_percent_aud(TWO_QUARTER_AUD, GAME_START_AUD, TWO_QUARTER_PERCENT_AUD);
	recalc_percent_aud(THREE_QUARTER_AUD, GAME_START_AUD, THREE_QUARTER_PERCENT_AUD);
	recalc_percent_aud(FOUR_QUARTER_AUD, GAME_START_AUD, FOUR_QUARTER_PERCENT_AUD);
	recalc_percent_aud(ONE_OVERTIME_AUD, GAME_START_AUD, ONE_OVERTIME_PERCENT_AUD);
	recalc_percent_aud(TWO_OVERTIME_AUD, GAME_START_AUD, TWO_OVERTIME_PERCENT_AUD);
	recalc_percent_aud(THREE_OVERTIME_AUD, GAME_START_AUD, THREE_OVERTIME_PERCENT_AUD);
	recalc_percent_aud(FULL_GAME_PURCHASE_AUD, GAME_START_AUD, FULL_GAME_PURCHASE_PCT_AUD);
	recalc_percent_aud(INITIALS_ENTERED_AUD, GAME_START_AUD, INITIALS_ENTERED_PERCENT_AUD);
}

///////////////////////////////////////////////////////////////////////////////
void update_quarters_percents( void )
{
	recalc_percent_aud( ONE_PLAYER_AUD,QUARTERS_PLAYED_AUD,ONE_PLAYER_PERCENT_AUD );
	recalc_percent_aud( TWO_PLAYER_AUD,QUARTERS_PLAYED_AUD,TWO_PLAYER_PERCENT_AUD );
	recalc_percent_aud( THREE_PLAYER_AUD,QUARTERS_PLAYED_AUD,THREE_PLAYER_PERCENT_AUD );
	recalc_percent_aud( FOUR_PLAYER_AUD,QUARTERS_PLAYED_AUD,FOUR_PLAYER_PERCENT_AUD );
	recalc_percent_aud( TWOVCPU_PLAYER_AUD,QUARTERS_PLAYED_AUD,TWOVCPU_PLAYER_PERCENT_AUD );
}

///////////////////////////////////////////////////////////////////////////////
#if 0		// What the hell is this doing in here!?!??!
#define CAP(a,b,c)		LESSER(GREATER((a),(b)),(c))
float season_rating( int, int, int, int, float );
float season_rating( int wins, int losses, int scored, int allowed, float qb )
{
	float	w,s,q, rating;

	// guard against divide by zero
	wins = GREATER(wins,30);
	losses = GREATER(losses,0);
	allowed = GREATER(allowed,1);

	w = (float)wins / (float)(wins + losses);
	w = CAP(w,0.5f,1.0f);

	s = (float)scored / (float)allowed;
	s = CAP(s,1.0f,1.5f);

	q = CAP(qb,125.0f,175.0f);

	rating = 50.0f + (w-0.5f) * 50.0f + (s-1.0f) * 25.0f + (q-125.0f) / 2.0f;

	return rating;
}
#endif
///////////////////////////////////////////////////////////////////////////////

