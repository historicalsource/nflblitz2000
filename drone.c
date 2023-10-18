/****************************************************************************/
/*                                                                          */
/* drone.c - Drone logic (finally)                                          */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* Copyright (c) 1997 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <math.h>
#include <goose/goose.h>
#include <stdlib.h>

#include "/Video/Nfl/Include/ani3d.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/playbook.h"
#include "/Video/Nfl/Include/pproto.h"
#include "/Video/Nfl/Include/handicap.h"
#include "/Video/Nfl/Include/profile.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/Drone.h"
#include "/Video/Nfl/Include/Cambot.h"
#include "/Video/Nfl/Anim/genseq.h"

#define USE_OLD_QB		FALSE
#ifdef JASON
#undef USE_OLD_QB
#define USE_OLD_QB		FALSE
#define DRONEQB_DEBUG
//#define DRONE_DEBUG
//#define BLOCK_DEBUG
//#define ZONE_DEBUG
#endif

// miscellaneous defines
#define WAIT()
#define CAP(a,b,c)		LESSER(GREATER((a),(b)),(c))

typedef struct _drone_t
{
	int		*pscript;
	int		stick,but;
	int		old_stick,old_but;	// prev different stick/but
	int		os_time,ob_time;	// ticks spent at prev stick/but
	int		target;			// guy we're covering, guy covering us, guy we're blocking, etc
	int		pass_react_time;// ticks until we react to ball being thrown
	int		think_time;
	int		move_time;		// used for smoothing motion (zone)
	float	mx,my,mz;		// marked location (for whatever reason)
	int		flags;			// DF_???, defined below
	float	gx,gz;			// 'ghost' X and Z pos
	float	vgx,vgz;		// 'ghost' X and Z vel
	int		fake_time;		// number of ticks we'll follow target's ghost
	int		catch_time;		// jump X ticks before ball arrives at X
	int		order_time;		// ticks remaining on DS_MOVE_DEFENDER movement
	int		order_dir;
	int		blitz_time;		// ticks before we go into blitz mode
	int		wait_time;		// ticks receiver will wait
	int		evade_time;		// ticks before weapon will consider more evade moves
	int		block_time;		// ticks before receiver will go into block mode
	char	*pending_catch;	// catch we'll do
	int		throw_time;		// qb hurdling, throw ball in X ticks
	float	zzlow, zzhigh;	// low and high zone z values
	float	zxlow, zxhigh;	// low and high zone x values
#ifdef ZONE_DEBUG
	//temp zone debugging stuff
	int		ztargets[4];
	int		ztcnt;
#endif
	float	zpoint[2];		// target point for zone defender
	float	zpv[2];			// movement of target point for zone defender
} drone_t;

// coverage types
#define CT_HUMAN		0
#define CT_ZONE			1
#define CT_MAN_US		2
#define CT_MAN_OTHER	3

// int_time modes
#define IT_LOWEST		0
#define IT_HIGHEST		1

#ifdef DEBUG
char *drone_flag_names[] =  { "DF_RUNYDS", "DF_WAITINT", "DF_GHOST_ACTIVE",
		"DF_BLITZ", "DF_TURBO", "DF_CANNED_CATCH", "DF_ORDERED", "DF_WAITING",
		"DF_MOTION", "DF_WILL_INT", "DF_WILL_ATTNOW", "DF_WILL_ATTLATER",
		"DF_WILL_POS", "DF_REACHED_X", "DF_IMPROVISING", "DF_SMART_INT",
		"DF_CAN_DIVE", "DF_QB_RUN", "DF_QB_RLEFT", "DF_QB_RRIGHT",
		"DB_CAN_CHEAP", "DF_CAN_EVADE", "DF_DEBUG", NULL };

char *ct_names[] = { "HUMAN", "ZONE", "MAN US", "MAN OTHER" };
#endif

drone_t drone_blocks[14];
drone_t *pdrone;
int		stick_drone;			// true if we're stick-control only

// drone blocker data
static int		rush_times[7];				// intercept times for rushers to carrier
											// this is public only so icmp can see it.
static int		blocked[7];					// Is rusher gonna be blocked in time?
static int		int_times[7][7];			// indexed first by rusher, then by blocker
static int		weighted_int_times[7][7];	// int_times, weighted for urgency & current assignments
static int		urgent[7];
static int		rushers[7];
static int		blockers[7];
static int		rcnt;

// drone receiver data
static int		weapons[7];
static int		cover_times[7][7];
static int		open_times[7];
static int		defenders[7];
static int		coverage[7];
static int		likely_yards[7];
static int		wcnt,dcnt;

//static int		cover_types[7][7];

// drone qb thinking data
static float	qb_dropback;				// how far will he drop back?
static int		roll_dir;					// original roll direction (-1=left,0=stay in pocket,1=right)
static int		sack_time;					// #of ticks until I get hit
static int		best_open;					// open time I computed on guy I've decided to throw at
static int		motion_dir;					// direction we want motion guy to move in (-1=left,0=none,1=right)
static int		gain_sought;				// how many yards minimum are we looking for on this play
static int		qb_flags;					// QB_??? defined below

#define QB_HURDLE_PASS			0x0001		// hurdle when you throw

// referenced data
extern int		dir49_table[];
extern int		p2p_angles[NUM_PLAYERS][NUM_PLAYERS];
extern float	distxz[NUM_PLAYERS][NUM_PLAYERS];
extern int		did_catch;
extern int		freeze;

extern char c_jmpint_anim;
extern char juke_c_anim;
extern char spin_anim;
extern char r_histep_anim;
extern char m_wavqb_anim;
extern char m_wavqbf_anim;
extern char c_no_vel1_anim;
extern char c_no_vel2_anim;

//extern catch_t *catch_anim_tbls[];

// global functions
void get_drone_switches( fbplyr_data *, int );
//float odistxz( obj_3d *, obj_3d * );

// local functions
static int evade_dir( fbplyr_data *, int );
static int icmp( const void *, const void * );
//static int spot_cmp( const void *, const void * );
static int dir2stick( int );
static int carrier_dir( fbplyr_data * );
static float carrier_dist( fbplyr_data * );
static int past_los( fbplyr_data * );
static void move_to_block( int blocker, int rusher, int target, float vmax );
static void move_to_intercept( int, int, float );
static void assign_blockers( void );
static int intercept_time( int, int, float );
static int nearest_available_blocker( int *, int *, int );
static float pdistxz( float *, float * );
static int sell_fake( int, int, int, int );
static float nearest_defender( fbplyr_data *, int * );
static void do_zone( fbplyr_data * );
static void db_choose_defense( fbplyr_data * );
static float int_time( float *, float, float *, float *, float, int );
static void update_receiver_data( void );
static void mtm_unhappiness( fbplyr_data *, float, float, float, float, float, float (*)(fbplyr_data *, float, float, int ));
static float qb_unhappiness( fbplyr_data *, float, float, int );
static float dbps_unhappiness( fbplyr_data *, float, float, int );
static float carrier_unhappiness( fbplyr_data *, float, float, int );
static float dline_unhappiness( fbplyr_data *, float, float, int );
static void choose_wr_imp_point( fbplyr_data * );
#if USE_OLD_QB
static int find_receiver( fbplyr_data *, int, int, int, int, int );
#else
static int find_receiver( fbplyr_data *, int, int );
#endif
static int clean_shot( fbplyr_data *, int );
static int find_attacker( fbplyr_data * );
static int find_diver( fbplyr_data * );
static int trips_lineup( void );
static int choose_roll_dir( void );
static int likely_gain( int );

static void _drone_lineup( fbplyr_data *ppdata );
static void _drone_dead( fbplyr_data *ppdata );
static void _drone_line( fbplyr_data *ppdata );
static void _drone_lineps( fbplyr_data *ppdata );
static void _drone_qbps( fbplyr_data *ppdata );
static void _drone_qb( fbplyr_data *ppdata );
static void _drone_wpnps( fbplyr_data *ppdata );
static void _drone_wpn( fbplyr_data *ppdata );
static void _drone_dbps( fbplyr_data *ppdata );
static void _drone_db( fbplyr_data *ppdata );
static void _drone_kicker( fbplyr_data *ppdata );
static void _drone_carrier( fbplyr_data *ppdata );

static void (*drone_functions[])(fbplyr_data *) = {
			_drone_lineup, _drone_dead, _drone_lineps, _drone_line, _drone_qbps,
			_drone_qb, _drone_wpnps, _drone_wpn, _drone_dbps, _drone_db,
			_drone_kicker };

//////////////////////////////////////////////////////////////////////////////
//  side < 0 to do all players
//  side 0 or 1 to do that team only
//
void init_drones_prelineup( int side )
{
	int btime, qb;
	int i = (side <= 0 ?           0 : NUM_PERTEAM);
	int j = (side == 0 ? NUM_PERTEAM : NUM_PLAYERS);

	btime = blitz_time_ht[game_info.team_handicap[!game_info.off_side]];

	qb_flags = 0;

	for( ; i < j; i++ )
	{
		drone_blocks[i].pscript = game_info.team_play[i/7]->routes[i%7];
		drone_blocks[i].stick = 24;
		drone_blocks[i].old_stick = 24;
		drone_blocks[i].but = 0;
		drone_blocks[i].old_but = 0;
		drone_blocks[i].os_time = 1000;
		drone_blocks[i].ob_time = 1000;
		drone_blocks[i].target = -1;
		drone_blocks[i].pass_react_time = 0;
		drone_blocks[i].think_time = 0;
		drone_blocks[i].mx = 0.0f;
		drone_blocks[i].mz = 0.0f;
		drone_blocks[i].flags = 0;
		drone_blocks[i].fake_time = 0;
		drone_blocks[i].move_time = 0;
		drone_blocks[i].order_time = 0;
		drone_blocks[i].evade_time = 0;
		drone_blocks[i].throw_time = 0;

		if (((i / NUM_PERTEAM == !game_info.off_side) && (game_info.play_type != PT_KICKOFF)) ||
			(ISDRONETEAM(player_blocks + i)))
		{
			drone_blocks[i].blitz_time = 3 * btime + randrng(3*btime);


			// lower blitz time for linemen in goal line play
			// Goal line play allows man to man, but has a couple of linemen who will rush in sooner
#if 0
			if( game_info.team_play[i/NUM_PERTEAM]->flags & PLF_REDZONE)
			{
				// set blitzers to 0-40 tick delay
#ifdef DEBUG
//				fprintf( stderr, "Red Zone flagged play - speed up blitz start!\n" );
#endif
				drone_blocks[i].blitz_time = 1;
//				drone_blocks[i].blitz_time = 10 + randrng(20+btime);
			}
#endif

			// lower blitz time on blitz plays
			if( game_info.team_play[i/NUM_PERTEAM]->flags & PLF_BLITZ)
			{
				// set blitzers to 0-40 tick delay
				if (game_info.game_quarter)
					drone_blocks[i].blitz_time = 1 + randrng(btime);
				else
					drone_blocks[i].blitz_time = 20 + randrng(20+btime);			// Easier in 1st quarter
			}
		}
		else
			drone_blocks[i].blitz_time = 0;


		if (game_info.team_play[i/NUM_PERTEAM]->formation[i%NUM_PERTEAM].control & LU_MOTION)
			drone_blocks[i].flags |= DF_MOTION;

		if (randrng(2))
			drone_blocks[i].flags |= DF_SMART_INT;

		if (game_info.team_play[i/NUM_PERTEAM]->flags & PLF_BLITZ)
		{	// blitz
			if (randrng(100) < db_cheapshot_blitz_ht[game_info.team_handicap[i/NUM_PERTEAM]])
				drone_blocks[i].flags |= DF_CAN_CHEAP;
		}

		if (p_status & (TEAM_MASK(i/NUM_PERTEAM)))
		{	// human-controlled team
			if (randrng(100) < def_hteam_dive_ht[game_info.team_handicap[i/NUM_PERTEAM]])
				drone_blocks[i].flags |= DF_CAN_DIVE;
		}
		else
		{	//  drone team
			if (i/NUM_PERTEAM == !game_info.off_side)
			{	// drone defense
				if (randrng(100) < def_drone_dive_ht[game_info.team_handicap[i/NUM_PERTEAM]])
					drone_blocks[i].flags |= DF_CAN_DIVE;

				if (!(game_info.team_play[i/NUM_PERTEAM]->flags & PLF_BLITZ))
				{	// no blitz
					if (randrng(100) < db_cheapshot_ht[game_info.team_handicap[i/NUM_PERTEAM]])
						drone_blocks[i].flags |= DF_CAN_CHEAP;
				}
			}
		}

		// Play Flag for dirty little fun stuff
		if ((game_info.team_play[!game_info.off_side]->flags & PLF_CHEAPSHOT) && (randrng(100) < 35 ))
		{
			drone_blocks[i].flags |= (DF_CAN_CHEAP|DF_CAN_DIVE);
#ifdef DEBUG
//		fprintf(stderr, "Cheap shot guy!\n");
#endif
		}
#ifdef DEBUG
		else
//			fprintf(stderr, "Not a cheap shot guy!\n");
#endif

		// Play Flag for Smart Intercepting of Ball carrier path
		if (game_info.team_play[!game_info.off_side]->flags & PLF_SMART_INT)
		{
			drone_blocks[i].flags |= DF_SMART_INT;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//  side < 0 to do all players
//  side 0 or 1 to do that team only
//
void init_drones_postlineup( int side )
{
	int i = (side <= 0 ?           0 : NUM_PERTEAM);
	int j = (side == 0 ? NUM_PERTEAM : NUM_PLAYERS);
	int k = !game_info.off_side;
	int l;

	if(( side < 0 ) || ( side == k ))
	{
		// run one tick of drone processing to init scripts for defensive players
		k = k * NUM_PERTEAM;
		l = k + NUM_PERTEAM;
		for( ; k < l; k++ )
		{
			pdrone = drone_blocks + k;
			drone_functions[player_blocks[k].odata.plyrmode]( player_blocks + k );
		}
	}

	// hand out superman bits
	for( ; i < j; i++ )
	{
		if (!(p_status & TEAM_MASK(i/NUM_PERTEAM)) &&
			((randrng(100) < def_superman_ht[game_info.team_handicap[i/NUM_PERTEAM]])))
			player_blocks[i].odata.flags |= PF_UNBLOCKABLE;
	}

	// randomly decide how far the QB will be willing to drop back on this play
	if ((randrng(4)) || (game_info.los <= 20))
		qb_dropback = 8.0f * YD2UN;
	else
		qb_dropback = 12.0f * YD2UN;

	// choose a direction for the motion guy
	motion_dir = randrng(3) - 1;
	motion_dir = 1;

#if 1	//(DHS)
	// decide how many yards we're looking to get at a minimum (if we need > 20 yards, don't wait for it, try for 20 & hope for the best...)
	gain_sought = LESSER(20, 1 + (game_info.first_down - game_info.los) / (5 - LESSER(4,game_info.down+1)));
#else
	// decide how many yards we're looking to get at a minimum
	gain_sought = 1 + (game_info.first_down - game_info.los) / (5 - LESSER(4,game_info.down+1));
#endif

	// set roll_dir to uninitialized
	roll_dir = -2;

	// gratuitous hurdle-pass?
	if (!randrng(3))
		qb_flags |= QB_HURDLE_PASS;
}

//////////////////////////////////////////////////////////////////////////////
// just had a change of possession.  resets to appropriate drone scripts
void drone_poss_change( void )
{
	int i;

	for( i = 0; i < 14; i++ )
		if( (i/7) == game_info.off_side )
			drone_blocks[i].pscript = ds_block;
		else
			drone_blocks[i].pscript = db_rush_qb;
}

//////////////////////////////////////////////////////////////////////////////
// tells a drone who he's covering (and who he's covered by)
void drone_cover_assign( int back, int receiver )
{
	drone_blocks[back].target = receiver;
	drone_blocks[receiver].target = back;

	// SPECIAL:  If our target is heating, maybe set CAN_CHEAP bit
	if ((tdata_blocks[game_info.off_side].last_receiver == receiver) &&
		(tdata_blocks[game_info.off_side].consecutive_comps == OFFENSE_FIRECNT-1) &&
		(ISDRONETEAM(player_blocks+back)))
	{
//		fprintf( stderr, "drone db %x is lined up over heater\n", JERSEYNUM(back));
		drone_blocks[back].flags |= DF_CAN_CHEAP;
	}
}

//////////////////////////////////////////////////////////////////////////////
// drone has tried to run out of bounds.  If he's a receiver running
// a route, abort and go into improvise mode.
void abort_drone_route( int plyrnum )
{
	pdrone->pscript = ds_improvise;
}

//////////////////////////////////////////////////////////////////////////////
// gets called every tick before any player processes go off
void drone_update( void )
{
	if( game_info.game_mode == GM_IN_PLAY )
	{
		assign_blockers();
		update_receiver_data();
	}
}

//////////////////////////////////////////////////////////////////////////////
void drone_motion_block( void )
{
	int		i;

	// look for a man in motion
	for( i = 0; i < 14; i++ )
	{
		if( drone_blocks[i].flags & DF_MOTION )
			drone_blocks[i].pscript = ds_block;
	}
}

//////////////////////////////////////////////////////////////////////////////
void drone_catch( int plyr, char *script, int time )
{
	drone_blocks[plyr].catch_time = time;
	drone_blocks[plyr].pending_catch = script;
	drone_blocks[plyr].flags |= DF_CANNED_CATCH;
}

//////////////////////////////////////////////////////////////////////////////
void get_drone_switches( fbplyr_data *ppdata, int stick_only )
{
	int		stick_change;

	pdrone = drone_blocks + ppdata->plyrnum;

	#ifdef NODRONES
#ifdef JASON
if( ppdata->odata.plyrmode != MODE_WPN )
#endif
{
	ppdata->stick_cur = 24;
	ppdata->but_cur = 0;
	return;
}
	#endif

	stick_drone = stick_only;

	pdrone->evade_time = GREATER(0,pdrone->evade_time-1);

	if ((pdrone->blitz_time) &&
		(game_info.game_mode == GM_IN_PLAY) &&
		(ppdata->team != game_info.off_side))
	{
		pdrone->blitz_time -= 1;

		if( pdrone->blitz_time == 0 )
		{
//		fprintf(stderr, "Switching to BLITZ!\n");
			pdrone->flags |= DF_BLITZ;
		}
	}

	if(( pdrone->think_time -= 1 ) >= 0 )
	{
		pdrone->ob_time += 1;
		pdrone->os_time += 1;
		return;
	}

	drone_functions[ppdata->odata.plyrmode]( ppdata );

	stick_change = GREATER(abs((pdrone->stick/7)-(pdrone->old_stick/7)),
					abs((pdrone->stick%7)-(pdrone->old_stick%7)));

	if( pdrone->stick == pdrone->old_stick )
		pdrone->os_time += 1;
	else
	{
		// if this is a minor stick change and we've changed
		// the stick in the last 30 ticks, blow it off.
		// no smoothing on QBs.
		if ((pdrone->os_time <= 30) &&
			(stick_change <= 1) &&
			(ppdata->odata.plyrmode != MODE_QB))
		{
			pdrone->stick = pdrone->old_stick;
			pdrone->os_time += 1;
		}
		else
		{
			pdrone->old_stick = pdrone->stick;
			pdrone->os_time = 0;
		}
	}

	if( !stick_drone )
	{
		if( pdrone->but == pdrone->old_but )
			pdrone->ob_time += 1;
		else
		{
			pdrone->old_but = pdrone->but;
			pdrone->ob_time = 0;
		}
	}

	ppdata->stick_cur = pdrone->stick;

	if( !stick_drone )
		ppdata->but_cur = pdrone->but;

	return;
}

//////////////////////////////////////////////////////////////////////////////
// 0 <= dir <= 1023
static int dir2stick( int dir )
{
	int i;
	int table[33][2] = {{0,3},{52,2},{76,9},{96,1},{128,0},{160,7},
		{180,15},{204,14},{256,21},{308,28},{332,29},{352,35},{384,42},
		{416,43},{436,37},{460,44},{512,45},{564,46},{588,39},{608,47},
		{640,48},{672,41},{692,33},{716,34},{768,27},{820,20},{844,19},
		{864,13},{896,6},{928,5},{948,11},{972,4},{1024,3}};

	// TODO: Make this a faster binary search
	for( i = 0; i < 32; i++ )
	{
		// if dir is closer to table[i] than to table[i+1], i is the solution
		if( ABS(table[i][0]-dir) < ABS(table[i+1][0]-dir) )
			return table[i][1];
	}

	return table[i][1];
}

//////////////////////////////////////////////////////////////////////////////
static int carrier_dir( fbplyr_data *ppdata )
{
	int i;
	int	tgt;

	if( game_info.ball_carrier == -1 )
		tgt = ball_obj.int_receiver;
	else
		tgt = game_info.ball_carrier;

	i = p2p_angles[ppdata->plyrnum][tgt];
	i = (i - (int)RD2GR(cambot.theta)) & 1023;

	return i;
}

//////////////////////////////////////////////////////////////////////////////
static float carrier_dist( fbplyr_data *ppdata )
{
	float f;
	int	tgt;

	if( game_info.ball_carrier == -1 )
		tgt = ball_obj.int_receiver;
	else
		tgt = game_info.ball_carrier;

	f = distxz[ppdata->plyrnum][tgt];

	return f;
}

//////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void dump_drone_states( void )
{
	int			i,j;
	fbplyr_data	*victim;

	fprintf( stderr, "who\t\tcov\tyards\n" );
	for( i = 0; i < wcnt; i++ )
	{
		fprintf( stderr, "receiver %x\t%d\t%d\n",
			JERSEYNUM(weapons[i]),
			coverage[i],
			likely_yards[i] );
	}

	getchar();

	return;

	fprintf( stderr, "Drone states:\n" );
	for( i = 0; i < 14; i++ )
	{
		fprintf( stderr, " %2x: ", JERSEYNUM(i) );

		switch( player_blocks[i].job )
		{
			case JOB_NONE:
				fprintf( stderr, "JOB_NONE  " );
				break;
			case JOB_RUSH:
				fprintf( stderr, "JOB_RUSH  " );
				break;
			case JOB_COVER:
				fprintf( stderr, "JOB_COVER %2x  ", JERSEYNUM(drone_blocks[i].target));
				if( drone_blocks[i].fake_time != 0 )
					fprintf( stderr, "fake_time: %d  ", drone_blocks[i].fake_time );
				break;
			case JOB_BLOCK:
				fprintf( stderr, "JOB_BLOCK %2x  ", JERSEYNUM(drone_blocks[i].target));
				break;
			case JOB_CATCH:
				fprintf( stderr, "JOB_CATCH  " );
				break;
			case JOB_BREAKUP:
				fprintf( stderr, "JOB_BREAKUP  " );
				break;
			case JOB_ZONE:
				fprintf( stderr, "JOB_ZONE  " );
				#ifdef ZONE_DEBUG
				fprintf( stderr, "\nX: %f to %f\nZ: %f to %f\n",
						drone_blocks[i].zxlow,
						drone_blocks[i].zxhigh,
						drone_blocks[i].zzlow,
						drone_blocks[i].zzhigh );
				fprintf( stderr, "tcnt: %d\n", drone_blocks[i].ztcnt );
				for( j = 0; j < drone_blocks[i].ztcnt; j++ )
					fprintf( stderr, "%2x: %f %f\n",
						JERSEYNUM(drone_blocks[i].ztargets[j]),
						player_blocks[drone_blocks[i].ztargets[j]].odata.x,
						player_blocks[drone_blocks[i].ztargets[j]].odata.z );
				fprintf( stderr, "tgt point: %f %f\n\n",
					drone_blocks[i].zpoint[0],
					drone_blocks[i].zpoint[1] );
				fprintf( stderr, "tgt point vels: %f %f\n\n",
					drone_blocks[i].zpv[0],
					drone_blocks[i].zpv[1] );
				#endif
				break;
		}

		if (drone_blocks[i].move_time > 0)
			fprintf( stderr, "move_time: %d  ", pdrone->move_time );

		if (drone_blocks[i].target != -1)
			fprintf( stderr, "target: %2x  ", JERSEYNUM( drone_blocks[i].target ));

		for (j = 0; drone_flag_names[j]; j++)
			if (drone_blocks[i].flags & (1<<j))
				fprintf( stderr, " %s", drone_flag_names[j]+3 );

		fprintf( stderr, "\n" );

#if 0
		switch( *(drone_blocks[i].pscript) )
		{
			case DS_WAIT_SNAP:
				fprintf( stderr, "WAIT_SNAP\n" );
				break;
			case DS_IMPROVISE:
				fprintf( stderr, "IMPROVISE\n" );
				break;
			case DS_COVER_TGT:
				fprintf( stderr, "COVER_TGT\n" );
				break;
			case DS_RUSH_QB:
				fprintf( stderr, "RUSH_QB\n" );
				break;
			default:
				fprintf( stderr, "other\n" );
				break;
		}
#endif
	}
#if 0
	fprintf( stderr, "urgent: %d %d %d %d %d %d %d\n",
			urgent[0],
			urgent[1],
			urgent[2],
			urgent[3],
			urgent[4],
			urgent[5],
			urgent[6] );
	fprintf( stderr, "rushers: %d %d %d %d %d %d %d\n",
			rushers[0],
			rushers[1],
			rushers[2],
			rushers[3],
			rushers[4],
			rushers[5],
			rushers[6] );
	fprintf( stderr, "blockers: %d %d %d %d %d %d %d\n",
			blockers[0],
			blockers[1],
			blockers[2],
			blockers[3],
			blockers[4],
			blockers[5],
			blockers[6] );
	fprintf( stderr, "blocked: %d %d %d %d %d %d %d\n",
			blocked[0],
			blocked[1],
			blocked[2],
			blocked[3],
			blocked[4],
			blocked[5],
			blocked[6] );

	fprintf( stderr, "targets:" );
	for( i = 0; (blockers[i] != -1) && (i < 7); i++ )
		fprintf( stderr, " %d", drone_blocks[blockers[i]].target );
	fprintf( stderr, "\n" );


 	for( i = 0; (rushers[i] != -1) && (i < 7); i++ )
	{
		fprintf( stderr, "rusher %2x ttq: %d\n",
			JERSEYNUM(rushers[urgent[i]]), rush_times[urgent[i]] );

		for( j = 0; (blockers[j] != -1) && (j < 7); j++ )
		{
			fprintf( stderr, "  blocker %2x tti: %d (%d)",
				JERSEYNUM( blockers[j] ), int_times[urgent[i]][j], weighted_int_times[urgent[i]][j] );

			if( drone_blocks[blockers[j]].target == rushers[urgent[i]] )
				fprintf( stderr, " *\n" );
			else
				fprintf( stderr, "\n" );
		}
	}
#endif

#if 0
	fprintf( stderr, "sack_time: %d\n", sack_time );
	// coverage stats
	for( i = 0; i < wcnt; i++ )
	{
		fprintf( stderr, "receiver %2x:  %d\n", JERSEYNUM(weapons[i]), open_times[i] );
		for( j = 0; j < dcnt; j++ )
		{
			fprintf( stderr, "  defender %2x:  %9s  %d\n",
				JERSEYNUM(defenders[j]),
				ct_names[cover_types[i][j]],
				cover_times[i][j] );
		}
	}
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////////
#if 0
static int past_los( fbplyr_data *ppdata )
{
	if( FIELDX(ppdata->odata.x) >= game_info.los )
		return TRUE;
	else
		return FALSE;
}
#endif

//////////////////////////////////////////////////////////////////////////////
static void _drone_lineup( fbplyr_data *ppdata )
{
	pdrone->stick = 24;
	pdrone->but = 0;

	return;
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_dead( fbplyr_data *ppdata )
{
	pdrone->stick = 24;
	pdrone->but = 0;

	return;
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_lineps( fbplyr_data *ppdata )
{
	pdrone->stick = 24;
	pdrone->but = 0;

	return;
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_line( fbplyr_data *ppdata )
{
	float	dx,dz;
	
	// if play is over, hold still
	#ifndef NOLINE
	if(( game_info.game_mode == GM_PLAY_OVER ) ||
		( game_info.game_flags & GF_PLAY_ENDING ))
	#endif
	{
		pdrone->stick = 24;
		pdrone->but = 0;
		return;
	}

	// if we're defense, rush
	if( ppdata->team != game_info.off_side )
	{
		pdrone->but = (pdrone->flags & DF_BLITZ) ? P_C : 0;
		
		if (game_info.ball_carrier != -1)
		{
			dx = ppdata->odata.x - player_blocks[game_info.ball_carrier].odata.x;
			dz = ppdata->odata.z - player_blocks[game_info.ball_carrier].odata.z;
			if (fabs(dz) > fabs(dx))
				pdrone->but = P_C;
		}

		mtm_unhappiness( ppdata,
						 (pdrone->but & P_C) ? LINE_TURBO_SPEED : LINE_SPEED,
						 (pdrone->but & P_C) ? LINE_BACKP_TURBO_SPEED : LINE_BACKP_SPEED,
						 1.0f,		// ffactor
						 20.0f,		// static friction
						 40.0f,		// moving friction
						 dline_unhappiness );
						 
		// if we're real close, dive at him or something
		if ((game_info.ball_carrier != -1) &&
			(distxz[ppdata->plyrnum][game_info.ball_carrier] < 30.0f) &&
			(clean_shot( ppdata, game_info.ball_carrier)))
		{
			if ((pdrone->flags & DF_CAN_DIVE) ||
				(game_info.team_play[!game_info.off_side]->flags & PLF_LINEDIVE))
			{
				pdrone->but = P_A|P_C;
			}
		}
		ppdata->job = JOB_RUSH;
	}
	else
	{
		ppdata->job = JOB_BLOCK;

		if( pdrone->target >= 0 )
		{
			ppdata->odata.flags |= PF_BACKPEDAL;
			ppdata->bptarget = pdrone->target;

			move_to_block( ppdata->plyrnum, pdrone->target, game_info.ball_carrier, LINE_TURBO_SPEED );
			pdrone->but = 0;
		}
		else
		{
			pdrone->stick = 24;
			pdrone->but = 0;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_qbps( fbplyr_data *ppdata )
{
	int		dir;
	int cnt;
	int num_plyrs;
	int max;

	pdrone->stick = 24;

	num_plyrs = 0;
	max = four_plr_ver ? 4 : 2; 
	for (cnt = 0; cnt < max; cnt++)
	{
		if (p_status & (1<<cnt))
			num_plyrs += 1;
	}

//fprintf(stderr, "num_plyrs = %d\n", num_plyrs);
	if (num_plyrs == 1)
	{
//		if ((audible_tap_action[0] == 2) || (audible_tap_action[1] == 2))
		if ((audible_tap_action[0] == 1) || (audible_tap_action[1] == 1))
			return;
	}

	// if we're waiting on the motion guy, just return
	if ((motion_dir) &&
		(player_blocks[game_info.off_side*7+4].stick_cur != 24))
		return;

	// hike from 0 to 30 ticks after we're allowed to (unless we waited for a motion guy
	if (((pcount % 30) && !motion_dir) || (pcount & 1))
	{
		pdrone->but = 0;
		return;
	}

	// snap the ball
	pdrone->but = P_B|P_C;

	return;
}

#if USE_OLD_QB
//////////////////////////////////////////////////////////////////////////////
// OLD Quarterback
static void _drone_qb( fbplyr_data *ppdata )
{
	int		i,j, best,best_time, dir, rush_yards, need_yards, open_time;
	int		diver, defender_count;
	float	point[3], dx,dz, f, friction, ffactor;

	f = ppdata->team ? -1.0f : 1.0f;

	// set roll_dir if we've just taken the snap
	if (roll_dir == -2)
		roll_dir = choose_roll_dir();

	// decide how many yards we're looking to get at a minimum
	need_yards = 1 + (game_info.first_down - game_info.los) / (5 - LESSER(4,game_info.down+1));

	// unless we're inside the enemy fifteen, never look for fewer than 15 yards
	// on a second down.
	if (game_info.down == 2)
	{
		need_yards = GREATER(need_yards,15);
		need_yards = LESSER(need_yards,100-game_info.los);
	}

	// if our throw bit is set, throw and return
	if (pdrone->flags & DF_THROW_NEXT)
	{
		pdrone->flags &= ~DF_THROW_NEXT;
		pdrone->but = P_B|P_C;
		pdrone->stick = 24;
		return;
	}

	if( game_info.play_type == PT_KICKOFF )
	{
		pdrone->stick = 24;
		pdrone->but = 0;
		return;
	}

	// if play is over, hold still
	if(( game_info.game_mode == GM_PLAY_OVER ) ||
		( game_info.game_flags & GF_PLAY_ENDING ))
	{
		pdrone->stick = 24;
		pdrone->but = 0;
		return;
	}

	// drop back after the snap
	if ((pcount - game_info.play_pcount) < 15)
	{
		dir = ppdata->team ? 256 : 768;
		dir += 128 * roll_dir;
		dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
		pdrone->stick = dir2stick( dir );
		pdrone->os_time = 60;
		pdrone->but = P_C;
		return;
	}

	// if we're already throwing, skip it
	if (ppdata->odata.flags & PF_THROWING)
		return;
		
	// figure out how many yards we figure we could get rushing
	// look for guys in our front 90 arc.  Figure we can get 1/2
	// of the distance to him.  Call it 20 yards if we seem to
	// be wide open.
	rush_yards = 30 + FIELDX(ppdata->odata.x) - game_info.los;
	defender_count = 0;
	for (i = 7*(!ppdata->team); i < 7 + 7*(!ppdata->team); i++)
	{
		dx = f*(player_blocks[i].odata.x - ppdata->odata.x);
		dz = fabs( player_blocks[i].odata.z - ppdata->odata.z );

		// is guy not in front of us?
		if (dx <= dz * 1.5f)
			continue;

		// figure we can get two thirds of the distance from line to defender
		j = ((FIELDX(player_blocks[i].odata.x) - game_info.los) * 2) / 3;

		rush_yards = LESSER(rush_yards,j);
	}

	#ifdef DRONEQB_DEBUG
	#ifdef DEBUG
	if (halt) fprintf( stderr, "need %d, can get %d\n", need_yards, rush_yards );
	#endif
	#endif

	if (pdrone->flags & DF_QB_RUN)
	{
		// we're running, but if we see a guy who's open and it looks
		// like we can pass for more yards than we're likely to get
		// by running, AND we probably can't get enough yards running,
		// hurdle-throw to this guy.
		best = find_receiver( ppdata, need_yards, drn_qb_range_ht[PLYRH(ppdata)], 0, 1, 1 );
		if( best != -1 )
		{
			player_blocks[ppdata->plyrnum].drone_throw_target = weapons[best];
			pdrone->but = P_B|P_C;
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "DRONE> running qb panic-throws at %2x, open_time = %d\n",
				JERSEYNUM(weapons[best]), best_open );
			WAIT();
			#endif
			return;
		}
	}
	else
	{
		// if we can get the yards we need, go for it
		if (rush_yards >= need_yards)
		{
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "DRONE> qb needs %d yards, figures he can get %d, is going for it.\n",
				need_yards, rush_yards );
			WAIT();
			#endif
			pdrone->flags |= DF_QB_RUN;
		}
	}

	friction = (float)(GREATER(20,60 - (pdrone->os_time*2)));
	ffactor = 1.0f - 0.01f * (float)(GREATER(0,20 - pdrone->os_time));

	// is anyone diving at us?
	diver = find_diver( ppdata );
	if (diver != -1)
	{	// there's a diver.  defend myself
		#ifdef DRONEQB_DEBUG
		fprintf( stderr, "QB> defender %2x is diving at me\n", JERSEYNUM(diver) );
		WAIT();
		#endif
		
		dir = p2p_angles[ppdata->plyrnum][diver] + 512;
		dir = (dir + 1024 - (int)RD2GR(cambot.theta)) % 1024;
		
		pdrone->stick = dir2stick(dir);

		// hurdle
		pdrone->but = P_A|P_C;

		// don't return -- may panic-throw
	}
	else if (pdrone->flags & DF_QB_RUN)
	{
		mtm_unhappiness( ppdata, QB_TURBO_SPEED, QB_BACKP_TURBO_SPEED,
			1.0f,			// ffactor
			friction,		// static friction
			friction/2.0f,	// moving friction
			carrier_unhappiness );
		pdrone->but = P_C;
		return;
	}
	else
	{
		mtm_unhappiness( ppdata, QB_TURBO_SPEED, QB_BACKP_TURBO_SPEED,
				ffactor,		// ffactor
				friction,		// static friction
				friction/2.0f,	// moving friction
				qb_unhappiness );
		pdrone->but = P_C;
	}
	
	// wait until we see an open downfield guy or we're under
	// huge pressure
	if (rcnt &&	(distxz[ppdata->plyrnum][rushers[urgent[0]]] < (YD2UN*4.0f)))
	{
		#ifdef DRONEQB_DEBUG
		fprintf( stderr, "DRONE> qb panics because %2x is %.1f yards away.\n",
			JERSEYNUM(rushers[urgent[0]]), 
			distxz[ppdata->plyrnum][rushers[urgent[0]]] / YD2UN );
		#endif
		best = find_receiver( ppdata, need_yards, drn_qb_range_ht[PLYRH(ppdata)], 0, 1, 10 );

		if( best != -1 )
		{
			player_blocks[ppdata->plyrnum].drone_throw_target = weapons[best];
			pdrone->but = P_A|P_C;
			pdrone->flags |= DF_THROW_NEXT;
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "DRONE> qb panic-throws at %2x, open_time = %d\n",
				JERSEYNUM(weapons[best]), best_open );
			WAIT();
			#endif
		}
		else
		{
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "DRONE> panicked qb decides to run for it.\n" );
			WAIT();
			#endif
			pdrone->flags |= DF_QB_RUN;
		}
	}
	else
	{
		best = find_receiver( ppdata, need_yards, drn_qb_range_ht[PLYRH(ppdata)], 10, 1, 10 );

		if( best != -1 )
		{
			player_blocks[ppdata->plyrnum].drone_throw_target = weapons[best];
			pdrone->but = P_A|P_C;
			pdrone->flags |= DF_THROW_NEXT;
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "drone qb throws at %2x, open_time = %d\n",
				JERSEYNUM(weapons[best]), best_open );
			WAIT();
			#endif
		}
	}

	return;
}

#else
//////////////////////////////////////////////////////////////////////////////
// NEW Quarterback

static float qb_panic_dist[12] = {YD2UN*4.0f, YD2UN*4.0f, YD2UN*4.0f, YD2UN*4.0f, YD2UN*4.0f, YD2UN*4.0f, YD2UN*5.0f, YD2UN*5.0f, YD2UN*5.0f, YD2UN*5.0f, YD2UN*6.0f, YD2UN*6.0f};	// (DHS)

static void _drone_qb( fbplyr_data *ppdata )
{
	int		i,j, best,best_time, dir, rush_yards, open_time;
	int		defender_count;
	float	point[3], dx,dz, f, friction, ffactor;

	f = ppdata->team ? -1.0f : 1.0f;

	// set roll_dir if we've just taken the snap
	if (roll_dir == -2)
		roll_dir = choose_roll_dir();

	// if our throw bit is set, throw and return
	if (pdrone->flags & DF_THROW_NEXT)
	{
		pdrone->flags &= ~DF_THROW_NEXT;
		pdrone->but = P_B|P_C;
		pdrone->stick = 24;
		return;
	}

	// if we're already throwing, quit now
	if (ppdata->odata.flags & PF_THROWING)
		return;
		
	// if play is over, hold still
	if ((game_info.game_mode == GM_PLAY_OVER) ||
		(game_info.game_flags & GF_PLAY_ENDING) ||
		(game_info.play_type == PT_KICKOFF))
	{
		pdrone->stick = 24;
		pdrone->but = 0;
		return;
	}

	// drop back after the snap
	if ((pcount - game_info.play_pcount) < 15)
	{
		dir = ppdata->team ? 256 : 768;
		dir += 128 * roll_dir;
		dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
		pdrone->stick = dir2stick( dir );
		pdrone->os_time = 60;
		pdrone->but = P_C;
		return;
	}

	// figure out how many yards we figure we could get rushing
	// look for guys in our front 90 arc.  Figure we can get 1/2
	// of the distance to him.  Call it 20 yards if we seem to
	// be wide open.
//	rush_yards = 30 + FIELDX(ppdata->odata.x) - game_info.los;
//	defender_count = 0;
//	for (i = 7*(!ppdata->team); i < 7 + 7*(!ppdata->team); i++)
//	{
//		int		brng;
//		float	brng_factor;
//		int		yards;
//
//		brng = p2p_angles[ppdata->plyrnum][i];
//		brng = (1024 + brng) % 1024;
//		brng = (brng > 512) ? 1024 - brng : brng;
//		brng_factor = 0.5f + ((float)brng) / 256.0f;
//		yards = (int)(brng_factor * distxz[ppdata->plyrnum][i] / YD2UN);
//
//		rush_yards = LESSER(rush_yards,yards);
//	}

	rush_yards = likely_gain( ppdata->plyrnum );

	#ifdef DRONEQB_DEBUG
	#ifdef DEBUG
	if (halt) fprintf( stderr, "need %d, can get %d\n", gain_sought, rush_yards );
	#endif
	#endif

	// if we think we can get the yards we need by running, run.
	if (rush_yards >= gain_sought)
	{
		#ifdef DRONEQB_DEBUG
		fprintf( stderr, "DRONE> qb needs %d yards, figures he can get %d, is going for it.\n",
			gain_sought, rush_yards );
		WAIT();
		#endif
		pdrone->flags |= DF_QB_RUN;
	}

	if (pdrone->flags & DF_QB_RUN)
	{
		// we're running, but if we see a guy who's open and it looks
		// like we can pass for more yards than we're likely to get
		// by running, hurdle-throw to this guy.
		best = find_receiver( ppdata, 0, rush_yards+1 );
		if( best != -1 )
		{
			player_blocks[ppdata->plyrnum].drone_throw_target = weapons[best];

			// if nearest defender is right on me, hurdle
			if ((nearest_defender( ppdata, NULL ) <= 25.0f) ||
				(qb_flags & QB_HURDLE_PASS))
			{
				pdrone->but = P_A|P_C;
				pdrone->flags |= DF_THROW_NEXT;

				// fix qb dive behind los (DHS)
				ppdata->non_buta_time=0;	

			}
			else
				pdrone->but = P_B|P_C;
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "DRONE> running qb throws at %2x, figuring on a %d yard gain\n",
				JERSEYNUM(weapons[best]),
				likely_yards[best] + FIELDX(player_blocks[weapons[best]].odata.x) - game_info.los );
			WAIT();
			#endif
			return;
		}
	}

	friction = (float)(GREATER(20,60 - (pdrone->os_time*2)));
	ffactor = 1.0f - 0.01f * (float)(GREATER(0,20 - pdrone->os_time));

	// unless we're unint, look for nearby defenders
	if (!(ppdata->odata.adata[0].animode & MODE_UNINT))
	{
		int		diver = -1, attacker = -1;

		// is anyone diving at us?
		diver = find_diver( ppdata );
	
		// if not, is anyone REAL close anyway?
		if (diver == -1)
			attacker = find_attacker( ppdata );
	
		if ((diver != -1) || (attacker != -1))
		{	// there's an attacker.  defend myself
			int		enemy;

			enemy = (diver == -1) ? attacker : diver;

			pdrone->stick = evade_dir( ppdata, attacker );
	
			// hurdle or spin (spin only if enemy is diving - doesn't work so well against
			// ordinary collision attacks)
			if (randrng(1) || (enemy == attacker))
			{	// hurdle
//				fprintf( stderr, "\nhurdling	\n" );
				// help fix dive behind los bug for qb (DHS)
				if (!(pdrone->flags & DF_THROW_NEXT))
				{
					pdrone->but = P_A|P_C;
					ppdata->non_buta_time=0;	
				}
			}
			else
			{	// spin (qb spins with D button)
//				fprintf( stderr, "\nspinning\n" );
				pdrone->but = P_D;
				// qb doesn't look at drone_script, but just in case, it
				// should be valid if we're hitting D.
				ppdata->drone_script = &spin_anim;
				return;
			}
	
//			getchar();
		}
	}						  

	if (pdrone->flags & DF_QB_RUN)
	{
		mtm_unhappiness( ppdata, QB_TURBO_SPEED, QB_BACKP_TURBO_SPEED,
			1.0f,			// ffactor
			friction,		// static friction
			friction/2.0f,	// moving friction
			carrier_unhappiness );

		#if 1
		// don't cacnel previous set command (DHS)
		pdrone->but |= P_C;
		#else
		pdrone->but = P_C;
		#endif
		return;
	}
	else
	{
		mtm_unhappiness( ppdata, QB_TURBO_SPEED, QB_BACKP_TURBO_SPEED,
				ffactor,		// ffactor
				friction,		// static friction
				friction/2.0f,	// moving friction
				qb_unhappiness );
		pdrone->but = P_C;
	}
	
	// wait until we see an open downfield guy or we're under
	// huge pressure
	if (rcnt &&	(distxz[ppdata->plyrnum][rushers[urgent[0]]] < (/*YD4*/qb_panic_dist[game_info.team_handicap[game_info.off_side]])))	//(DHS) make decision more handicap based
	{
		int		desperate = FALSE;

		if (game_info.down == 4)
			desperate = TRUE;

		if (FIELDX(ppdata->odata.x) <= 0)
			desperate = TRUE;

		#ifdef DRONEQB_DEBUG
		fprintf( stderr, "DRONE> qb panics because %2x is %.1f yards away.\n",
			JERSEYNUM(rushers[urgent[0]]), 
			distxz[ppdata->plyrnum][rushers[urgent[0]]] / YD2UN );
		#endif
#if 1
		best = find_receiver( ppdata, 0, 0 );		// anyone open?		(DHS)
#else
		best = find_receiver( ppdata, 0, -1 );		// anyone open?		(DHS)
#endif
		if ((best == -1) && desperate)
			best = find_receiver( ppdata, 1, 0);	// single coverage?
		if ((best == -1) && desperate)
			best = find_receiver( ppdata, 10, 0);	// desperate - hit longest guy

		if( best != -1 )
		{
			player_blocks[ppdata->plyrnum].drone_throw_target = weapons[best];
			if ((nearest_defender( ppdata, NULL ) <= 25.0f) ||
				(qb_flags & QB_HURDLE_PASS))
			{
				pdrone->but = P_A|P_C;
				pdrone->flags |= DF_THROW_NEXT;
				ppdata->non_buta_time=0;		// (DHS)
			}
			else
				pdrone->but = P_B|P_C;
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "DRONE> qb panic-throws at %2x\n",	JERSEYNUM(weapons[best]));
			WAIT();
			#endif
		}
		else
		{
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "DRONE> panicked qb decides to run for it.\n" );
			WAIT();
			#endif
			pdrone->flags |= DF_QB_RUN;
		}
	}
	else
	{
		best = find_receiver( ppdata, 0, gain_sought );

		if( best != -1 )
		{
			player_blocks[ppdata->plyrnum].drone_throw_target = weapons[best];
			if ((nearest_defender( ppdata, NULL ) <= 25.0f) ||
				(qb_flags & QB_HURDLE_PASS))
			{
				pdrone->but = P_A|P_C;
				pdrone->flags |= DF_THROW_NEXT;
				ppdata->non_buta_time = 0;
			}
			else
				pdrone->but = P_B|P_C;
			#ifdef DRONEQB_DEBUG
			fprintf( stderr, "drone qb throws at %2x, likely gain of %d\n",
				JERSEYNUM(weapons[best]),
				likely_yards[best] + FIELDX(player_blocks[weapons[best]].odata.x) - game_info.los );
			WAIT();
			#endif
		}
	}

	return;
}

#endif
//////////////////////////////////////////////////////////////////////////////
static void _drone_wpnps( fbplyr_data *ppdata )
{
	int		my_qb, dir;

	if( pdrone->flags & DF_MOTION )
	{
		if (ISHUMANTEAM(ppdata))
		{
			// if on human-controlled team, read player stick for l/r
			my_qb = ppdata->team * 7 + 3;
			pdrone->stick = player_blocks[my_qb].stick_cur;
			pdrone->but = 0;
			return;
		}
		else
		{
			if (motion_dir < 0)
				pdrone->stick = 21;
			else if (motion_dir > 0)
				pdrone->stick = 27;
			else
				pdrone->stick = 24;

			pdrone->stick = 24 + CAP(motion_dir,-1,1) * 3;

			pdrone->but = (pdrone->stick == 24) ? 0 : P_C;
			return;
		}
	}

	// if it's a kickoff, the ball hasn't been caught or hit the ground yet,
	// we're on the kicking team, AND the kicker is downfield, move downfield
	if(( game_info.play_type == PT_KICKOFF ) &&
		( ball_obj.type == LB_KICKOFF ) &&
		( fabs(player_blocks[7*game_info.off_side].odata.x) < fabs(ppdata->odata.x)))
	{
		dir = game_info.off_side ? 768 : 256;
		dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
		pdrone->stick = dir2stick( dir );
		pdrone->but = 0;
		return;
	}

	pdrone->stick = 24;
	pdrone->but = 0;

	return;
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_wpn( fbplyr_data *ppdata )
{
	int		i, dir, my_qb, rnd;
	float	f, point[3];
	int		done = FALSE;
	int		dir2ball;
	char	**anim_tbl;
	obj_3d	*pobj = (obj_3d *)ppdata;

	// if play is over, hold still
	#ifndef NOWPNS
	if( game_info.game_mode == GM_PLAY_OVER )
	#endif
	{
		pdrone->stick = 24;
		pdrone->but = 0;
		return;
	}

	// if we've got the ball, run with it
	if( ppdata->plyrnum == game_info.ball_carrier )
	{
		_drone_carrier( ppdata );
		return;
	}

	// if it's a kickoff, the ball hasn't been caught or hit the ground yet,
	// we're on the kicking team, move downfield
	if(( game_info.play_type == PT_KICKOFF ) &&
		( ppdata->team == game_info.off_init ) &&
		( game_info.ball_carrier == -1 ) &&
		( ball_obj.type == LB_KICKOFF ) &&
		!( ball_obj.flags & BF_BOUNCE ))
	{
		point[0] = ppdata->odata.x + (ppdata->team ? -10.0f : 10.0f);
		point[2] = ppdata->odata.z;
		dir  = ptangle( &(ppdata->odata.x), point );
		dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
		pdrone->stick = dir2stick( dir );
		pdrone->but = P_C;
		return;
	}

	// if !UNINT, look for commands from QB (if real drone) or Player (if fake drone or drone team)
	// if we're unint and the intended receiver, look for commands from a human player
	// DON'T bother with this if we're a fake drone
	if ((!( pobj->adata[0].animode & MODE_UNINT )) &&
		(ISHUMANTEAM(ppdata)) &&
		(ball_obj.type == LB_PASS) &&
		(game_info.ball_carrier == -1) &&
		(!stick_drone) &&
		(ball_obj.int_receiver == ppdata->plyrnum))
	{
		// if I'm a real drone, look at the guy who threw the ball.
		// if I'm a player pretending to be a drone because I'm the int receiver, look at that player
		my_qb = ppdata->plyrnum;

		if (ppdata->station == -1)
			my_qb = ball_obj.who_threw;

		// check if human player pressed the 'A' button...
		if (ball_obj.total_time - ball_obj.flight_time < 50)
		{
			// if QB threw ball, and ball is in air and above the head of the players...allow player to jump for ball
			if (player_blocks[my_qb].but_new & P_A)
			{
				ppdata->drone_script = &c_jmpint_anim;
				pdrone->but = (pdrone->flags & DF_TURBO) ? P_C|P_D : P_D;
				return;
			}
		}
	}


	// if we're aware that the ball has been thrown, go for the X
	if( game_info.ball_carrier == -1 )
	{
		// if the ball has JUST been thrown/fumbled, set our pass_react_time
		switch( ball_obj.flight_time )
		{
			case 0:
				// thrown earlier this tick.  ignore it.
				break;

			case 1:
				// thrown last tick.  set timer.
				if( ball_obj.type == LB_PASS )
					pdrone->pass_react_time = 3 + randrng(24) + randrng(24) + randrng(24);

				// catch on pretty quick to fumbles
				if( ball_obj.type == LB_FUMBLE )
					pdrone->pass_react_time = 3 + randrng(24);

				// if it's aimed behind the line of scrimmage and not at us,
				// never catch on.
				if(( FIELDX(ball_obj.tx) < game_info.los ) &&
					( ball_obj.type != LB_FUMBLE ))
					pdrone->pass_react_time = 10000;

				// catch on right away if we're the intended receiver
				if( ball_obj.int_receiver == ppdata->plyrnum )
					pdrone->pass_react_time = 0;

				break;

			default:
				if (((ball_obj.total_time - ball_obj.flight_time) == pdrone->catch_time) &&
					(ball_obj.int_receiver == ppdata->plyrnum) &&
					(game_info.ball_carrier == -1) && (pdrone->flags & DF_CANNED_CATCH) &&
					(ball_obj.type == LB_PASS))
				{
					// set catch seq. for drone
					ppdata->drone_script = pdrone->pending_catch;
					did_catch = TRUE;

					// signal a special move
					pdrone->but = (pdrone->flags & DF_TURBO ) ? P_C|P_D : P_D;
					pdrone->flags &= ~DF_CANNED_CATCH;
					return;
				}

				// countdown
				if( pdrone->pass_react_time )
					pdrone->pass_react_time = GREATER(0,pdrone->pass_react_time-1);
				else
				{
					float	my_dist;

					point[0] = ball_obj.tx;
					point[2] = ball_obj.tz;

					my_dist = pdistxz(&(ppdata->odata.x), point);

					if (my_dist < 2.0f )
					{
						pdrone->stick = 24;
						pdrone->but = 0;

						if (!(pdrone->flags & DF_CANNED_CATCH) && (FIELDX(pobj->x) > game_info.los))
						{
							// weapon has reached X...while ball in flight..wait a bit..then jump for it!!
							pdrone->flags |= DF_CANNED_CATCH;
							ppdata->job = JOB_CATCH;
							pdrone->catch_time = 10;
							pdrone->pending_catch = &c_no_vel1_anim;
							return;
						}

					}
					else
					{
						float	ball_time,time_nonturbo,time_turbo;
						float	ball_xz_vel,ball_dist;

						dir  = ptangle( &(ppdata->odata.x), point );
						dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
						pdrone->stick = dir2stick( dir );

//						pdrone->but = (pdrone->flags & DF_TURBO ) ? P_C: 0;
						// if we're going to need turbo to reach the X in time, use it, otherwise don't.
						ball_xz_vel = fsqrt(ball_obj.odata.vx*ball_obj.odata.vx + ball_obj.odata.vz*ball_obj.odata.vz);
						point[0] = ball_obj.tx;
						point[2] = ball_obj.tz;
						ball_dist = pdistxz(&(ball_obj.odata.x),point);
						if(ball_dist == 0.0f && ball_xz_vel == 0.0f)
						{
							ball_time = 1000000.0f;
						}
						else
						{
							ball_time = ball_dist / ball_xz_vel;
						}
						time_nonturbo = my_dist / WPN_SPEED;
						time_turbo = my_dist / WPN_TURBO_SPEED;

						pdrone->but = (time_nonturbo <= ball_time) ? 0 : P_C;

						ppdata->job = JOB_CATCH;
						return;
					}
				}
				break;
		}
	}

	// if carrier is in MODE_WPN, become a blocker for him
	if( player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_WPN )
	{
		if (pdrone->flags & DF_DELAY_BLOCK)
		{
			pdrone->block_time -= 1;

			if (pdrone->block_time <= 0)
				pdrone->flags &= ~DF_DELAY_BLOCK;
		}

		if (!(pdrone->flags & DF_DELAY_BLOCK))
		{
			ppdata->job = JOB_BLOCK;

			if( pdrone->target >= 0 )
			{
				ppdata->odata.flags |= PF_BACKPEDAL;
				ppdata->bptarget = pdrone->target;

	 			move_to_block( ppdata->plyrnum, pdrone->target, game_info.ball_carrier, WPN_SPEED );
				pdrone->but = 0;
			}
			else
			{
				pdrone->stick = 24;
				pdrone->but = 0;
			}

			return;
		}
	}

	while( !done )
	{
		switch( *pdrone->pscript )
		{
			case DS_WAIT_SNAP:
				pdrone->pscript += 1;
				break;

			case DS_BLOCK:
				ppdata->odata.flags |= PF_BACKPEDAL;
				ppdata->bptarget = pdrone->target;

				if (pdrone->flags & DF_DEBUG)
					fprintf( stderr, "recvr %2x goes into BLOCK mode\n", JERSEYNUM(ppdata->plyrnum));

				pdrone->but = 0;
	 			move_to_block( ppdata->plyrnum, pdrone->target, game_info.ball_carrier, WPN_SPEED );
				ppdata->job = JOB_BLOCK;
				done = TRUE;
				break;

			case DS_DELAY_BLOCK:
				pdrone->flags |= DF_DELAY_BLOCK;
				pdrone->block_time = pdrone->pscript[1];
				pdrone->pscript += 2;
				break;

			case DS_FACE:
				i = pdrone->pscript[1];
				pdrone->pscript += 2;
				if( ppdata->odata.flags & PF_REVWPN )
					i *= -1;

				i = (i + (game_info.off_side ? 768 : 256) - (int)RD2GR( cambot.theta )) & 1023;
				pdrone->stick = dir2stick( i );

				if (pdrone->flags & DF_DEBUG)
					fprintf( stderr, "recvr %2x turns to %d\n", JERSEYNUM(ppdata->plyrnum), i);

				// override the smoothing effect for receivers
				pdrone->old_stick = pdrone->stick;
				break;

			case DS_GOTO_LOS_PLUS:
				i = pdrone->pscript[1];
				if( FIELDX(ppdata->odata.x) >= ( game_info.los + i ))
				{
					pdrone->pscript += 2;

					if (pdrone->flags & DF_DEBUG)
						fprintf( stderr, "recvr %2x reaches los+%d\n", JERSEYNUM(ppdata->plyrnum), i);

				}
				else
				{
					ppdata->job = JOB_NONE;
					done = TRUE;
				}
				break;

			case DS_RUN_YDS:
				f = FIELDHZ / 50.0f * (float)(pdrone->pscript[1]);
				if( pdrone->flags & DF_RUNYDS )
				{
					// have we run far enough?
					// OR will we be out of bounds after one more yard of travel
					// along present heading?
					if ((pdistxz(&(pdrone->mx),&(ppdata->odata.x)) >= f ) ||
						(0))
					{
						pdrone->flags &= ~DF_RUNYDS;
						pdrone->pscript += 2;
						if (pdrone->flags & DF_DEBUG)
							fprintf( stderr, "recvr %2x finishes RUN_YDS(%d)\n", JERSEYNUM(ppdata->plyrnum), i);
					}
					else
					{
						done = TRUE;
					}
				}
				else
				{
					if (pdrone->flags & DF_DEBUG)
						fprintf( stderr, "recvr %2x begins RUN_YDS(%d)\n", JERSEYNUM(ppdata->plyrnum), i);
					pdrone->flags |= DF_RUNYDS;
					pdrone->mx = ppdata->odata.x;
					pdrone->mz = ppdata->odata.z;
					ppdata->job = JOB_NONE;
					done = TRUE;
				}
				break;

			case DS_IMPROVISE:
				if (!(pdrone->flags & DF_IMPROVISING))
				{	// pick a target point
					if (pdrone->flags & DF_DEBUG)
						fprintf( stderr, "recvr %2x begins improvising\n", JERSEYNUM(ppdata->plyrnum));
					choose_wr_imp_point( ppdata );
					pdrone->flags |= DF_IMPROVISING;
				}

				if (pdistxz(&(pdrone->mx),&(ppdata->odata.x)) <= 2.0f)
				{	// pick new target point
					choose_wr_imp_point( ppdata );
				}

				// move toward target point
				dir  = ptangle( &(ppdata->odata.x), &(pdrone->mx) );
				dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
				pdrone->stick = dir2stick( dir );
				pdrone->but = 0;

				done = TRUE;
				break;

			case DS_JUKE:
				// if the guy covering us is greater than 4 yards away, don't do this
				if( pdrone->flags & DF_WAITINT )
				{
					if( ppdata->odata.adata[0].animode & MODE_UNINT )
						done = TRUE;
					else
					{
						pdrone->pscript += 1;
						pdrone->flags &= ~DF_WAITINT;
						pdrone->but = 0;
					}
				}
				else
				{
					if( nearest_defender( ppdata, NULL ) > (8.0f*6.0f*FOCFAC))
					{
						pdrone->pscript += 1;
					}
					else
					{
						ppdata->drone_script = &juke_c_anim;
						pdrone->but = P_D;
						pdrone->flags |= DF_WAITINT;
						ppdata->job = JOB_NONE;
						done = TRUE;
					}
				}
				break;

			case DS_SPIN:
				if( pdrone->flags & DF_WAITINT )
				{
					if( ppdata->odata.adata[0].animode & MODE_UNINT )
						done = TRUE;
					else
					{
						pdrone->pscript += 1;
						pdrone->flags &= ~DF_WAITINT;
						pdrone->but = 0;
					}

				}
				else
				{
					if( nearest_defender( ppdata, NULL ) > (8.0f*6.0f*FOCFAC))
					{
						pdrone->pscript += 1;
					}
					else
					{
						ppdata->drone_script = &spin_anim;
						pdrone->but = P_D;
						pdrone->flags |= DF_WAITINT;
						ppdata->job = JOB_NONE;
						done = TRUE;
					}
				}
				break;

			case DS_FAKE_OUT:
				if( sell_fake( ppdata->plyrnum, pdrone->target, pdrone->pscript[2], pdrone->pscript[3] ))
				{
					i = pdrone->pscript[1];
					if( ppdata->odata.flags & PF_REVWPN )
						i *= -1;
					i += ( game_info.off_side ? 768 : 256 );

					pdrone->gx = ppdata->odata.x;
					pdrone->gz = ppdata->odata.z;
					pdrone->vgx = FRELX( 0.0f, WPN_SPEED, i );
					pdrone->vgz = FRELZ( 0.0f, WPN_SPEED, i );
					pdrone->flags |= DF_GHOST_ACTIVE;
				}

				pdrone->pscript += 4;
				break;

			case DS_TURBO_ON:
				pdrone->flags |= DF_TURBO;
				pdrone->but |= P_C;
				pdrone->pscript += 1;
				break;

			case DS_TURBO_OFF:
				pdrone->flags &= ~DF_TURBO;
				pdrone->but &= ~P_C;
				pdrone->pscript += 1;
				break;

			case DS_WAVE_ARM:
				if( pdrone->flags & DF_WAITINT )
				{
					if( ppdata->odata.adata[0].animode & MODE_UNINT )
					{
						ppdata->job = JOB_NONE;
						done = TRUE;
					}
					else
					{
						pdrone->pscript += 1;
						pdrone->flags &= ~DF_WAITINT;
					}
				}
				else
				{
					if( nearest_defender( ppdata, NULL ) < (3.0f*6.0f*FOCFAC))
					{
						pdrone->pscript += 1;
					}
					else
					{
						pobj->flags |= PF_WAVE_ARM;
						pdrone->flags |= DF_WAITINT;
						ppdata->job = JOB_NONE;
						done = TRUE;
					}
				}
				break;

			case DS_MOVE_DEFENDER:
				// don't do this if the ball's in the air, or carrier is WPN
				if ((game_info.ball_carrier == -1 ) &&
					!(game_info.game_flags & GF_HIKING_BALL))
				{
					pdrone->pscript += 3;
					break;
				}
				if ((game_info.ball_carrier != -1) &&
					(player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_WPN))
				{
					pdrone->pscript += 3;
					break;
				}

				i = pdrone->pscript[1];
				if( ppdata->odata.flags & PF_REVWPN )
					i *= -1;
				i = (i + (game_info.off_side ? 768 : 256) - (int)RD2GR( cambot.theta )) & 1023;

				if( pdrone->target == -1 )
				{
					pdrone->pscript += 3;
					break;
				}

				drone_blocks[pdrone->target].order_dir = dir2stick( i );
				drone_blocks[pdrone->target].order_time = pdrone->pscript[2];
				drone_blocks[pdrone->target].flags |= DF_ORDERED;

				pdrone->pscript += 3;

				break;

			case DS_COVER_TGT:
			case DS_RUSH_QB:
				// must've been a turnover.  we're now on offense
				ppdata->job = JOB_BLOCK;
				done = TRUE;
				break;

			case DS_WAIT:
				if( pdrone->flags & DF_WAITING )
				{
					pdrone->wait_time -= 1;
					if( pdrone->wait_time == 0 )
					{
						pdrone->flags &= ~DF_WAITING;
						pdrone->pscript += 2;
					}
					else
					{
						done = TRUE;
					}
				}
				else
				{
					pdrone->flags |= DF_WAITING;
					pdrone->wait_time = pdrone->pscript[1];
					pdrone->stick = 24;
					pdrone->but = 0;
					done = TRUE;
				}
				break;

			default:
				fprintf( stderr, "drone %2x hits unknown script command %d\n",
					ppdata->static_data->number, *pdrone->pscript );
				pdrone->pscript += 1;
				break;
		}
	}

	// I'm running a route.  If there's a defensive back more or less right in front of me, do a running
	// push to clear him out
	if ((nearest_defender(ppdata, &i) < 15.6f) &&
		(ISDRONETEAM(ppdata)) &&
		!(ppdata->odata.adata[1].animode & MODE_RUNNING_PUSH) &&
		(abs(bearing(ppdata->plyrnum,i)) <=128))
	{
//		fprintf( stderr, "receiver %x thinks he can push defender %x\n",
//			JERSEYNUM(ppdata->plyrnum),
//			JERSEYNUM(i) );
//		freeze = TRUE;
		pdrone->but = P_B|P_C;
	}


	if( pdrone->flags & DF_GHOST_ACTIVE )
	{
		pdrone->gx += pdrone->vgx;
		pdrone->gz += pdrone->vgz;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_carrier( fbplyr_data *ppdata )
{
	float	point[3];
	int		dir, nearest=-1, brng, i;
	obj_3d	*pobj = (obj_3d *)ppdata;
	float	friction,ffactor;

	friction = (float)(GREATER(20,60 - (pdrone->os_time*2)));
	ffactor = 1.0f - 0.01f * (float)(GREATER(0,20 - pdrone->os_time));

	mtm_unhappiness( ppdata, CARRIER_SPEED, CARRIER_SPEED,
		1.0f,			// ffactor
		friction,		// static friction
		friction/2.0f,	// moving friction
		carrier_unhappiness );

	pdrone->but = 0;

	// if the best thing we can think of is to stand still,
	// move forward
	if (pdrone->stick == 24)
		pdrone->stick = 3;

	// go no further if we're unint
	if ((ppdata->odata.adata[0].animode & MODE_UNINT) ||
		(ppdata->odata.adata[1].animode & MODE_UNINT))
		return;

	// go no further if our evade_time is set
	if( pdrone->evade_time )
		return;

	// no evasion if we're tanking
	if( drones_lose && drones_winning )
		return;

	// find nearest enemy player
	for( i = 0; (i < 7) && (rushers[i] != -1); i++ )
	{
		if(( nearest == -1 ) || (rush_times[i] < rush_times[nearest]))
			nearest = i;
	}

	if( nearest != -1)
	{
		if( rush_times[nearest] < 20 )
		{
			#ifdef DRONE_DEBUG
			fprintf( stderr, "drone %2x> %2x is too close, ",
				JERSEYNUM(ppdata->plyrnum),
				JERSEYNUM(rushers[nearest]));
			#endif

			// okay, this guy is a problem. how to deal?
			brng = bearing( ppdata->plyrnum, rushers[nearest] );
			#ifdef DRONE_DEBUG
			fprintf( stderr, "bearing is %d, ", brng );
			#endif

			if (!(pdrone->flags & DF_CAN_EVADE))
			{
				if (randrng(100) >= drn_evade_ht[PLYRH(ppdata)])
				{
					#ifdef DRONE_DEBUG
					fprintf( stderr, "no evasion\n" );
					#endif
					pdrone->evade_time = 40;
					return;
				}
				else
					pdrone->flags |= DF_CAN_EVADE;
			}

			if( brng <= 160 )
			{
				// stiff arm him
				if( rush_times[nearest] < 10 )
				{
					pdrone->but = P_B|P_C;
					pdrone->flags &= ~DF_CAN_EVADE;
					#ifdef DRONE_DEBUG
					fprintf( stderr, "do a stiff-arm\n" );
					#endif
				}
				else
					pdrone->but = 0;
				return;
			}
			else
			{
				// hurdle or spin
				if( player_blocks[rushers[nearest]].odata.y > 0.0f )
				{
					if( game_info.hurdle_cnt[ppdata->team] >= hurdle_cap_ht[PLYRH(ppdata)] )
						return;

					// hurdle
					pdrone->but = P_A|P_C;
					pdrone->flags &= ~DF_CAN_EVADE;
					#ifdef DRONE_DEBUG
					fprintf( stderr, "do a hurdle\n" );
					#endif
				}
				else
				{
					if( game_info.spin_cnt[ppdata->team] >= spin_cap_ht[PLYRH(ppdata)] )
						return;

					// spin
					pdrone->but = P_C;
					ppdata->non_turbo_time = 1;
					pdrone->flags &= ~DF_CAN_EVADE;
					#ifdef DRONE_DEBUG
					fprintf( stderr, "do a spin\n" );
					#endif
				}
				return;
			}
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_dbps( fbplyr_data *ppdata )
{
	int		done = FALSE, dir;
	float	tf, friction, ffactor;

	// do script processing even before the snap
	while( !done )
	{
		switch( *pdrone->pscript )
		{
			case DS_WAIT_SNAP:
				pdrone->pscript++;
				break;

			case DS_COVER_WEAK:
				if ((dir = trips_lineup()))
				{
					pdrone->pscript = (dir == -1) ? db_zone_sl3 : db_zone_sr3;
					pdrone->flags = DF_ZONE_PRESNAP;
				}
				else
					pdrone->pscript++;
				break;

			case DS_RUSH_QB:
				ppdata->job = JOB_RUSH;
				done = TRUE;
				break;

			case DS_BLOCK:
				ppdata->job = JOB_BLOCK;
				done = TRUE;
				break;

			case DS_COVER_TGT:
				ppdata->job = JOB_COVER;
				done = TRUE;
				break;

			case DS_SETFLAG:
				pdrone->flags |= pdrone->pscript[1];
				pdrone->pscript += 2;
				break;

			case DS_ZONE:
				if (game_info.team_play[!game_info.off_side]->flags & PLF_FAKEZONE)
				{
					ppdata->fake_zone_time = 30 + randrng(180);
				}

				if( ppdata->job == JOB_ZONE )
				{
					done = TRUE;
					break;
				}

				switch( pdrone->pscript[1] )
				{
					case DZ_LEFT_THIRD:
						pdrone->zzlow = -(FIELDW+YD2UN);
						pdrone->zzhigh = -(FIELDW/3.0f-YD2UN);
						break;
					case DZ_MIDDLE_THIRD:
						pdrone->zzlow = -(FIELDW/3.0f+YD2UN);
						pdrone->zzhigh = (FIELDW/3.0f+YD2UN);
						break;
					case DZ_RIGHT_THIRD:
						pdrone->zzlow = (FIELDW/3.0f-YD2UN);
						pdrone->zzhigh = (FIELDW+YD2UN);
						break;
					case DZ_LEFT_HALF:
						pdrone->zzlow = -(FIELDW+YD2UN);
						pdrone->zzhigh = YD2UN;
						break;
					case DZ_RIGHT_HALF:
						pdrone->zzlow = -YD2UN;
						pdrone->zzhigh = (FIELDW+YD2UN);
						break;
					case DZ_WIDE:
						pdrone->zzlow = -(FIELDW+YD2UN);
						pdrone->zzhigh = (FIELDW+YD2UN);
						break;
					default:
						#ifdef DEBUG
						fprintf( stderr, "Bogus zone width.\n" );
						lockup();
						#endif
						break;
				}

				if( pdrone->pscript[2] < -10 )
				{
					#ifdef DEBUG
					fprintf( stderr, "Bogus zone depth(1).\n" );
					lockup();
					#else
					pdrone->pscript[2] = -10;
					#endif
				}

				if( game_info.los + pdrone->pscript[2] > 100 )
					pdrone->zxlow = WORLDX(100);
				else
					pdrone->zxlow = WORLDX(game_info.los + pdrone->pscript[2] );

				if( pdrone->pscript[3] == DZ_EZ )
					pdrone->zxhigh = WORLDX(110);
				else if( pdrone->pscript[3] <= pdrone->pscript[2] )
				{
					#ifdef DEBUG
					fprintf( stderr, "Bogus zone depth(2).\n" );
					lockup();
					#else
					pdrone->pscript[3] = pdrone->pscript[2] + 1;
					#endif
				}
				else
					pdrone->zxhigh = WORLDX(game_info.los + pdrone->pscript[3] );

				// if team 1 has the ball, swap and negate zzlow and zzhigh,
				// and swap zxlow and zxhigh
				if( game_info.off_side )
				{
					tf = pdrone->zzlow;
					pdrone->zzlow = -1.0f * pdrone->zzhigh;
					pdrone->zzhigh = -1.0f * tf;

					tf = pdrone->zxlow;
					pdrone->zxlow = pdrone->zxhigh;
					pdrone->zxhigh = tf;
				}

				ppdata->job = JOB_ZONE;

				break;

			default:
				ppdata->job = JOB_NONE;
				done = TRUE;
				break;
		}
	}

	pdrone->stick = 24;
	pdrone->but = 0;

	if(( pdrone->target != -1 ) &&
		( ppdata->job != JOB_ZONE ) &&
		( drone_blocks[pdrone->target].flags & DF_MOTION ))
	{
		friction = (float)(GREATER(20,60 - (pdrone->os_time*2)));
		ffactor = 1.0f - 0.01f * (float)(GREATER(0,20 - pdrone->os_time));

		mtm_unhappiness( ppdata, DBPS_SPEED, DBPS_SPEED,
			1.0f,			// ffactor
			friction*2.0f,	// static friction
			friction/2.0f,	// moving friction
//			60.0f,
//			30.0f,
			dbps_unhappiness );


		// if we're moving, set think_time to some massively negative
		// number so that we won't lose track of the guy any more.
		if( pdrone->stick != 24 )
			pdrone->think_time = -100000;

#if 1	//(DHS)
		pdrone->think_time += 6 + randrng(40) - (game_info.team_handicap[!game_info.off_side]>>1);		//adjust this a bit for difficulty
#else
		pdrone->think_time += 6 + randrng(40);
#endif
	}
	else if (pdrone->flags & DF_ZONE_PRESNAP)
		do_zone( ppdata );

	return;
}

//////////////////////////////////////////////////////////////////////////////
#define DB_A	0
#define DB_B	1
#define DB_C1	2
#define DB_C2a1	3
#define DB_C2a2	4
#define DB_C2a3	5
#define DB_C2a4	6
#define DB_C2b1	7
#define DB_C2b2	8
#define DB_C2C	9
#define DB_D	10
#define DB_E	11
#define DB_F	12
#define DB_G	13
#define DB_H	14

static void _drone_db( fbplyr_data *ppdata )
{
	int		state;
	int		dir, bdir,newstick;
	float	point[3], pd;
	obj_3d	*pobj = (obj_3d *)ppdata;

	#ifdef NORUSH
	pdrone->stick = 24;
	pdrone->but = 0;
	return;
	#endif

	// A. ball dead => freeze
	// B. ball in QB's hand ==> cover our guy/rush QB/guard our zone
	// C. ball has been thrown to WPN and is in flight
	//    C1. we're unaware => cover our guy/rush QB/guard our zone
	//    C2. we're aware
	//       C2a. we're going to attack
	//          C2a1. we're not in position, move
	//          C2a2. we're in position, hold
	//          C2a3. it's time to attack, hit
	//          C2a4. move to attack int receiver
	//       C2b. we're going to intercept
	//          C2b1. not time yet -- move to int ball
	//          C2b2. jump for it
	//       C2c. move into position to make tackle later
	// D. ball in WPN's hand => get carrier
	// E. ball free (bobbled/fumbled) => get ball
	// F. ball is non-onside kickoff in flight.  freeze.
	// G. ball is onside kick in flight.  get it.
	// H. we're in the middle of a running push - follow through
	if ((game_info.game_mode == GM_PLAY_OVER) ||
		(game_info.game_flags & GF_PLAY_ENDING))
	{
		state = DB_A; // ball dead
	}
	else if (pobj->adata[1].animode & MODE_RUNNING_PUSH)
	{
		state = DB_H;
	}
	else if ((game_info.play_type == PT_KICKOFF) &&
			(ball_obj.type == LB_KICKOFF) &&
			!(game_info.game_flags & GF_ONSIDE_KICK) &&
			(game_info.ball_carrier == -1))
	{
		state = DB_F; // non-onside kickoff in flight
	}
	else if ((game_info.play_type == PT_KICKOFF) &&
			(ball_obj.type == LB_KICKOFF) &&
			(game_info.game_flags & GF_ONSIDE_KICK) &&
			(game_info.ball_carrier == -1))
	{
		state = DB_G; // onside kickoff in flight
	}
	else if( game_info.ball_carrier == -1 )
	{
		if( game_info.game_flags & GF_HIKING_BALL )
		{
			state = DB_B; // ball in QB's hand
		}
		else if ((ball_obj.int_receiver == -1) ||
				(ball_obj.type == LB_FUMBLE) ||
				(ball_obj.type == LB_PUNT) ||
				(ball_obj.type == LB_BOBBLE))
		{
			state = DB_E; // ball bobbled/fumbled
		}
		else
		{
			switch( ball_obj.flight_time )
			{
				case 0: // just thrown this tick.  Can't notice.
				{
					state = DB_B;
					break;
				}
				case 1: // thrown last tick. set react time
				{
					int	btime;

					btime = db_preact_ht[PLYRH(ppdata)];

					if( ppdata->job == JOB_RUSH )
						pdrone->pass_react_time = randrng(btime)/3;
					else if( ppdata->job == JOB_ZONE )
						pdrone->pass_react_time = randrng(btime);
					else if( ball_obj.int_receiver == pdrone->target )
						pdrone->pass_react_time = GREATER(pdrone->fake_time,(randrng(btime) * 2 / 3));
					else
						pdrone->pass_react_time = randrng(btime) + randrng(btime);
						
					// Ignore pass if targeted behind LOS at someone we're not
					// personally covering.  UNLESS the ball has already crossed
					// the line of scrimmage.  Like if it's a lateral targeted
					// behind the los.
					if ((FIELDX(ball_obj.tx) < game_info.los ) &&
						!(game_info.game_flags & GF_BALL_CROSS_LOS) &&
						((ppdata->job != JOB_COVER) ||
						(pdrone->target != ball_obj.int_receiver)))
						pdrone->pass_react_time = 10000;

					state = DB_C1;
					break;
				}
				default: // thrown some time back.  state depends on react time
				{
					if( pdrone->pass_react_time )
					{
						pdrone->pass_react_time -= 1;
						state = DB_C1;
					}
					else
					{
						// if we've only just noticed the throw, decide if we're
						// going to attack or intercept
						if( pdrone->flags & DF_WILL_INT)
						{
							// try to intercept
							if ((pdrone->catch_time-=1) <= 200 )
							{
								state = DB_C2b2;
							} else {
								state = DB_C2b1;
							}
						}
						else if( pdrone->flags & DF_WILL_ATTNOW)
						{
							// intercept and attack ASAP
							if( distxz[ppdata->plyrnum][ball_obj.int_receiver] > 12.0f )
								state = DB_C2a4; // move to int receiver
							else
								state = DB_C2a3; // attack int receiver

							pdrone->flags &= ~DF_WILL_ATTNOW;
						}
						else if( pdrone->flags & DF_WILL_ATTLATER)
						{
							// attack when he catches
							point[0] = ball_obj.tx;
							point[2] = ball_obj.tz;

							if( pdistxz( &(ppdata->odata.x), point ) > 12.0f )
								state = DB_C2a1;	// move into position
							else
							{
								if((ball_obj.total_time - ball_obj.flight_time) > 18 )
									state = DB_C2a2; // wait
								else
									state = DB_C2a3; // hit him
							}
						}
						else if( pdrone->flags & DF_WILL_POS)
						{
							// get into position for a later tackle
							state = DB_C2C;
						}
						else
						{
							// choose
							db_choose_defense( ppdata );
							state = DB_C2a1;
						}
					}
					break;
				}
			}
		}
	}
	else
	{
		if( player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_QB )
			state = DB_B; // ball in QB's hand
		else
			state = DB_D; // ball in WPN's hand
	}

	switch( state )
	{
		case DB_A: // ball dead, freeze
		case DB_F: // opening kickoff, freeze
			pdrone->stick = 24;
			pdrone->but = 0;
			break;
		case DB_B: // ball in QB's hand. cover our guy/rush QB
		case DB_C1: // ball thrown, we're unaware. cover our guy/rush QB
			if(( ppdata->job == JOB_COVER ) &&
				( pdrone->target != -1 ) &&
				( pdrone->target != game_info.ball_carrier ))
			{
				if(( pdrone->flags & DF_ORDERED ) && ( pdrone->order_time > 0 ))
				{
					pdrone->stick = pdrone->order_dir;
					pdrone->order_time -= 1;

					if( pdrone->order_time == 0 )
						pdrone->flags &= ~DF_ORDERED;
					ppdata->odata.flags &= ~PF_BACKPEDAL;
					pdrone->but = 0;
					break;
				}
				else if(( drone_blocks[pdrone->target].flags & DF_GHOST_ACTIVE ) &&
					( pdrone->fake_time > 0 ))
				{
					pdrone->fake_time -= 1;

					// track his ghost
					point[0] = drone_blocks[pdrone->target].gx;
					point[2] = drone_blocks[pdrone->target].gz;
				}
				else
				{
					// track him
					point[0] = player_blocks[pdrone->target].odata.x;
					point[2] = player_blocks[pdrone->target].odata.z;
					ppdata->odata.flags |= PF_BACKPEDAL;
					ppdata->bptarget = pdrone->target;
				}
				
				if ((distxz[ppdata->plyrnum][pdrone->target] < 34.0f) &&
					(pdrone->flags & DF_CAN_CHEAP))
				{
					// take the cheap shot
//					printf( "drone db %x takes cheap shot at %x\n",
//						JERSEYNUM(ppdata->plyrnum), JERSEYNUM(pdrone->target));
					dir = p2p_angles[ppdata->plyrnum][pdrone->target];
					dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
					pdrone->stick = dir2stick( dir );
					pdrone->but = P_B|P_C;
					pdrone->flags ^= DF_CAN_CHEAP;
				}
				else
				{
					point[0] += game_info.off_side ? -4.0f*YD2UN : 4.0f*YD2UN;
					pd = pdistxz( &(pobj->x), point);
					dir  = ptangle( &(ppdata->odata.x), point );
					dir = (dir - (int)RD2GR(cambot.theta)) & 1023;

					newstick = dir2stick( dir );
					if ((pd > YD2UN) || (pdrone->os_time > 30))
						pdrone->stick = newstick;
					pdrone->but = 0;
				}
			}
			else if( ppdata->job == JOB_ZONE )
			{
				// cover our zone
				if ((distxz[ppdata->plyrnum][pdrone->target] < 34.0f) &&
					(pdrone->flags & DF_CAN_CHEAP))
				{
					// take the cheap shot
					dir = p2p_angles[ppdata->plyrnum][pdrone->target];
					dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
					pdrone->stick = dir2stick( dir );
					pdrone->but = P_B|P_C;
					pdrone->flags ^= DF_CAN_CHEAP;
				}
				else
				{
					do_zone( ppdata );
				}
			}
			else
			{
				// kill the QB
				pdrone->stick = dir2stick( carrier_dir(ppdata) );
				if( pdrone->flags & DF_BLITZ )
				{
					pdrone->but = P_C;
					if( carrier_dist( ppdata ) < 36.0f )
//						if( randrng( 100 ) < 10 )
					{
							pdrone->but = P_A|P_C;
//							fprintf(stderr, "B> Dive\n");
					}
				}
				else
					pdrone->but = 0;

				if( randrng( 60 ) == 0 )
					pdrone->think_time = 20 + randrng( 30 );
				else
					pdrone->think_time = 2 + randrng(5);
			}
			break;
		case DB_C2a1: // move into attack position
//			fprintf( stderr, "%2x moving\n", JERSEYNUM(ppdata->plyrnum) );
			point[0] = ball_obj.tx;
			point[2] = ball_obj.tz;
			dir = ptangle( &(ppdata->odata.x), point );
			dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
			pdrone->stick = dir2stick( dir );
			pdrone->but = 0;
			break;
		case DB_C2a2: // wait
//			fprintf( stderr, "%2x waiting\n", JERSEYNUM(ppdata->plyrnum) );
			pdrone->stick = 24;
			pdrone->but = 0;
			break;
		case DB_C2a3: // attack
//			fprintf( stderr, "%2x attacking\n", JERSEYNUM(ppdata->plyrnum) );
			dir = p2p_angles[ppdata->plyrnum][ball_obj.int_receiver];
			dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
			pdrone->stick = dir2stick( dir );
			pdrone->but = P_B|P_C;
			break;
		case DB_C2a4: // move to intercept receiver
//			fprintf( stderr, "%2x pursuing\n", JERSEYNUM(ppdata->plyrnum) );
			move_to_intercept( ppdata->plyrnum, ball_obj.int_receiver, DB_SPEED );
			pdrone->but = 0;
			break;
		case DB_C2b1: // move to intercept ball
//			fprintf( stderr, "%2x moving to int ball\n", JERSEYNUM(ppdata->plyrnum) );
			dir = ptangle( &(ppdata->odata.x), &(pdrone->mx) );
			dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
			pdrone->stick = dir2stick( dir );
			pdrone->but = 0;
			break;
		case DB_C2b2: // jump for ball
//			fprintf( stderr, "%2x jumping for ball\n", JERSEYNUM(ppdata->plyrnum) );
			pdrone->stick = pdrone->old_stick;
			pdrone->but = P_A;
			break;
		case DB_D: // wpn with ball.  attack
			ppdata->job = JOB_RUSH;
			if ((FIELDX(ppdata->odata.x) <= FIELDX(ball_obj.odata.x)) ||
				(pdrone->flags & DF_BLITZ))
			{	// fast intercept
				if (pdrone->flags & DF_SMART_INT)
					move_to_intercept( ppdata->plyrnum, game_info.ball_carrier, DB_TURBO_SPEED );
				else
				{
					dir  = ptangle( &(ppdata->odata.x), &(player_blocks[game_info.ball_carrier].odata.x) );
					dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
					pdrone->stick = dir2stick( dir );
				}
				pdrone->but = P_C;
			}
			else
			{	// slow intercept
				if (pdrone->flags & DF_SMART_INT)
					move_to_intercept( ppdata->plyrnum, game_info.ball_carrier, DB_SPEED );
				else
				{
					dir  = ptangle( &(ppdata->odata.x), &(player_blocks[game_info.ball_carrier].odata.x) );
					dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
					pdrone->stick = dir2stick( dir );
				}
				pdrone->but = 0;
			}

			// if we're real close, dive at him or something
			if ((distxz[ppdata->plyrnum][game_info.ball_carrier] < 32.0f) &&
				((ISDRONETEAM(ppdata)) || (!randrng(10))) &&	// rare on human-controlled teams
				((pdrone->flags & DF_CAN_DIVE) || (!randrng(100))))
				{
				pdrone->but = P_A|P_C;
//				fprintf(stderr, "C> Dive\n");
				}

			break;
		case DB_C2C: // move into position
		case DB_E: // ball bobbled/free.  get it
		case DB_G: // onside kick. get it
			point[0] = ball_obj.tx;
			point[2] = ball_obj.tz;
			dir  = ptangle( &(ppdata->odata.x), point );
			bdir  = ptangle( &(ppdata->odata.x), &(ball_obj.odata.x) );

			// if we're more or less facing the ball and we're within four units of the X, stop.
			if ((abs(bdir - pobj->tgt_fwd) < 64) &&
				(pdistxz(&(ppdata->odata.x),point) <= 4.0f))
			{
				pdrone->stick = 24;
				pdrone->but = 0;
			}
			// okay, if we're within two units of the X, but not facing the ball, turn to face it
			else if (pdistxz(&(ppdata->odata.x),point) <= 2.0f)
			{
				bdir = (bdir - (int)RD2GR(cambot.theta)) & 1023;
				pdrone->stick = dir2stick( bdir );
				pdrone->but = 0;
			}
			else
			{
				dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
				pdrone->stick = dir2stick( dir );
				pdrone->but = 0;
			}

			break;

		case DB_H:	// follow-through on running push
			dir = p2p_angles[ppdata->plyrnum][pdrone->target];
			dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
			pdrone->stick = dir2stick( dir );
			pdrone->but = P_B|P_C;
			break;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////
#define DCD_ATT_NOW		0
#define DCD_ATT_LATER	1
#define DCD_INTERCEPT	2
#define DCD_POSITION	3
#define DCD_EITHER		4

static void db_choose_defense( fbplyr_data *ppdata )
{
	int		bbbo, nwf, ttip, ttib;
	float	bt1,bt2, ball_y1,ball_y2,ball_y;
//	char	*dcd_strings[] = { "DCD_ATT_NOW", "DCD_ATT_LATER", "DCD_INTERCEPT", "DCD_POSITION", "DCD_EITHER" };
	obj_3d	*ptgt, *pobj;
	int		can_int = FALSE,can_att = FALSE;
	int		result = -1, i;
	int		nint=0,natt=0,npos=0;

	// we can safely use int_receiver without checking.  We wouldn't
	// be here if it wasn't valid.
	ptgt = (obj_3d *)(player_blocks+ball_obj.int_receiver);
	pobj = (obj_3d *)(ppdata);

	// drone teammates of human players just take up positions
	if (!ISDRONETEAM(ppdata))
	{
		pdrone->flags |= DF_WILL_POS;
		return;
	}

	// how many ticks to intercept player?
	ttip = (int)(int_time( &(pobj->x), DB_SPEED, &(ptgt->x), &(ptgt->vx), 8.0f, IT_LOWEST ));

	// how many ticks to intercept ball?
	bt1 = int_time( &(pobj->x), DB_SPEED, &(ball_obj.odata.x), &(ball_obj.odata.vx), 6.0f, IT_LOWEST );
	bt2 = int_time( &(pobj->x), DB_SPEED, &(ball_obj.odata.x), &(ball_obj.odata.vx), 6.0f, IT_HIGHEST );
	ball_y1 = ball_obj.odata.y + ball_obj.odata.vy*bt1 + 0.5f*GRAVITY*bt1*bt1;
	ball_y2 = ball_obj.odata.y + ball_obj.odata.vy*bt2 + 0.5f*GRAVITY*bt2*bt2;

	if ((ball_y2 < ball_y1) && (ball_y2 > 0.0f))
	{
		ttib = (int)bt2;
		ball_y = ball_y2;
	}
	else
	{
		ttib = (int)bt1;
		ball_y = ball_y1;
	}

	if (ttib < (ball_obj.total_time - ball_obj.flight_time) &&
		(ball_y <= 35.0f ))
		can_int = TRUE;

	if( ttip < (CONTROL_TIME + ball_obj.total_time - ball_obj.flight_time))
		can_att = TRUE;

	if( !can_int && !can_att )
		result = DCD_POSITION;
	else if( !can_int )
		result = DCD_ATT_NOW;
	else if( !can_att )
		result = DCD_INTERCEPT;
	else
		result = DCD_EITHER;

	//fprintf( stderr, "db> %2x wakes up, %s int, %s att, chooses %s\n",
	//	JERSEYNUM(ppdata->plyrnum),
	//	can_int ? "can" : "can't",
	//	can_att ? "can" : "can't",
	//	dcd_strings[result] );

	// look at other drone defenders and count how many are assigned to
	// the various jobs.
	for( i = 7*ppdata->team; i < 7+7*ppdata->team; i++ )
	{
		if (drone_blocks[i].flags & DF_WILL_INT)
			nint++;

		if (drone_blocks[i].flags & DF_WILL_POS)
			npos++;

		if (drone_blocks[i].flags & (DF_WILL_ATTNOW|DF_WILL_ATTLATER))
			natt++;
	}
	//fprintf( stderr, "db> nint: %d npos: %d natt: %d\n", nint, npos, natt );

	// if nobody is set to take up tackling position, do so ourselves
	if (npos==0)
	{
		//fprintf( stderr, "nobody else positioning, switching to DCD_POSITION\n" );
		result = DCD_POSITION;
	}

	// if we're planning to intercept and someone else is already going to,
	// skip it and attack.
	if ((result==DCD_INTERCEPT) && (nint))
	{
		result = can_att ? DCD_ATT_NOW : DCD_POSITION;
		//fprintf( stderr, "someone else is intercepting, switch to %s\n",
		//	dcd_strings[result] );
	}

	// if we can do either, intercept if that's not covered, attack if it is
	if (result==DCD_EITHER)
	{
		result = nint ? DCD_ATT_NOW : DCD_INTERCEPT;
		//fprintf( stderr, "choosing %s\n", dcd_strings[result] );
	}

	// if we're gonna attack now, check for permission to do an early hit
	if ((randrng(100) > early_hit_ht[PLYRH(ppdata)]) &&
		(result == DCD_ATT_NOW))
	{
		result = DCD_ATT_LATER;
		//fprintf( stderr, "permission to take cheap shot denied, switching to DCD_ATT_LATER\n" );
	}

	// if we're trying ANYTHING but DCD_POSITION, check handicap permission
	if ((result != DCD_POSITION) &&
		(randrng(100) > drone_db_att_ht[PLYRH(ppdata)]))
	{
		result = DCD_POSITION;
		//fprintf( stderr, "permission to take aggressive action denied, switching to DCD_POSITION\n" );
	}
	//fprintf( stderr, "final result: %s\n", dcd_strings[result] );
	//WAIT();

	switch( result )
	{
		case DCD_ATT_NOW:
			pdrone->flags |= DF_WILL_ATTNOW;
			return;
		case DCD_ATT_LATER:
			pdrone->flags |= DF_WILL_ATTLATER;
			return;
		case DCD_INTERCEPT:
			pdrone->catch_time = ttib-1;
			pdrone->flags |= DF_WILL_INT;
			pdrone->mx = ball_obj.odata.x + (float)(ttib) * ball_obj.odata.vx;
			pdrone->mz = ball_obj.odata.z + (float)(ttib) * ball_obj.odata.vz;
			return;
		case DCD_POSITION: default:
			pdrone->flags |= DF_WILL_POS;
			return;
	}
}

//////////////////////////////////////////////////////////////////////////////
// compute time to intercept.  projectile starts at pxz, with a max velocity of
// pvxz.  target starts at txz with a velocity of tvxz.  pretend target and
// projectile are dadj closer than they are.
// float pointers are to arrays of XYZ, and we ignore Y.
static float int_time( float *pxz, float pvxz, float *txz, float *tvxz, float dadj, int mode )
{
	float	a,b,c, b2m4ac;
	float	d[3], md, r1, r2, r;

	d[0] = pxz[0] - txz[0];
	d[2] = pxz[2] - txz[2];

	a = tvxz[0]*tvxz[0] + tvxz[2]*tvxz[2] - pvxz * pvxz;
	b = -2.0f * ( d[0] * tvxz[0] + d[2] * tvxz[2] + pvxz * dadj);
	c = d[0] * d[0] + d[2] * d[2] - dadj * dadj;

	b2m4ac = (b*b) - (4.0f*a*c);

	if( b2m4ac < 0.0f )
		return 100000.0f;

	if( c < 0.001f )
		return 0.0f;

	if(( fabs( b ) < 0.001f ) && ( fabs( a ) < 0.001f ))
		return 200000.0f;

	if( fabs( a ) < 0.001f )
	{
		r = -c / b;

		if( r < 0 )
			return 400000.0f;
		else
			return( r );
	}

	if( fabs( b ) < 0.001f )
		if(( -c / a ) < 0.0f )
			return 300000.0f;
		else
			return fsqrt( -c / a );

	r1 = (-1.0f * b + fsqrt(b2m4ac)) / (2.0f * a);
	r2 = (-1.0f * b - fsqrt(b2m4ac)) / (2.0f * a);

	if(( r1 < 0.0f ) && ( r2 < 0.0f ))
		return 300000.0f;

	if(( r1 >= 0.0f ) && ( r2 >= 0.0f ))
	{	// both are nonnegative
		r = (mode == IT_LOWEST) ? LESSER(r1,r2) : GREATER(r1,r2);
	}
	else
		r = GREATER( r1, r2 );	// one is nonnegative

	if( r < 0.0f )
		fprintf( stderr, "returning neg r %f, r1,r2: %f %f",
			r,r1,r2 );

	return r;
}

//////////////////////////////////////////////////////////////////////////////
static void do_zone( fbplyr_data *ppdata )
{
	float	point[3], f, dpt[2];
	int		i, dir, targets[4], tcnt=0, deepest=0, newstick;
	int		zbase,zrnd;
	obj_3d	*ptgt;

	// team factor
	f = game_info.off_side ? -1.0f : 1.0f;
		
	ppdata->odata.flags |= PF_BACKPEDAL;

	pdrone->move_time = GREATER(pdrone->move_time-1,0);
	if (pdrone->move_time)
	{
		pdrone->zpoint[0] += pdrone->zpv[0];
		pdrone->zpoint[1] += pdrone->zpv[1];
		point[0] = pdrone->zpoint[0];
		point[2] = pdrone->zpoint[1];
	}
	else
	{
		// identify receivers in our zone
		for( i = game_info.off_side * 7 + 3; i < game_info.off_side * 7 + 7; i++ )
		{
			ptgt = (obj_3d *)(player_blocks + i);
	
			if((ptgt->x >= pdrone->zxlow ) &&
				(ptgt->x <= pdrone->zxhigh ) &&
				(ptgt->z >= pdrone->zzlow ) &&
				(ptgt->z <= pdrone->zzhigh ))
				targets[tcnt++] = i;
		}
	
		for( i = 1; i < tcnt; i++ )
			if( player_blocks[targets[i]].odata.x*f > player_blocks[targets[deepest]].odata.x*f )
				deepest = i;
	

//Fix!!!



		// if there's nobody in our zone, stand in the center laterally,
		// five yards from the front or in the middle, whichever is
		// closer to the line of scrimmage.
		if( tcnt == 0 )
		{
			point[2] = (pdrone->zzhigh + pdrone->zzlow) / 2.0f;
	
			if(( pdrone->zxhigh - pdrone->zxlow ) / YD2UN <= 10.0f )
				// narrow zone.  stand in the middle
				point[0] = (pdrone->zxhigh + pdrone->zxlow) / 2.0f;
			else
			{
				// more than 10 yards.  Stand five from the front
				if( game_info.off_side == 0)
					point[0] = pdrone->zxlow + 5.0f*YD2UN;
				else
					point[0] = pdrone->zxhigh - 5.0f*YD2UN;
			}
			dpt[0] = 0.0f;
			dpt[1] = 0.0f;
			ppdata->bptarget = game_info.ball_carrier;
		}
		else if( tcnt ==1 )
		{
			// one receiver.  stand 4 yards downfield
			point[0] = player_blocks[targets[0]].odata.x + f * YD2UN * 4.0f;
			point[2] = player_blocks[targets[0]].odata.z;
			dpt[0] = player_blocks[targets[0]].odata.vx;
			dpt[1] = player_blocks[targets[0]].odata.vz;
			ppdata->bptarget = targets[0];
		}
		else
		{
			// Two or more.  stand 4 yards downfield of their midpoint,
			// but at least 2 yards deeper than the deepest among them.
			// Weight the midpoint calculation in favor of the deepest.
			point[0] = 0.0f;	point[2] = 0.0f;
			dpt[0] = 0.0f;		dpt[1] = 0.0f;
			for( i = 0; i < tcnt; i++ )
			{
				point[0] += player_blocks[targets[i]].odata.x;
				point[2] += player_blocks[targets[i]].odata.z;
				dpt[0] += player_blocks[targets[i]].odata.vx;
				dpt[1] += player_blocks[targets[i]].odata.vz;
			}
			point[0] += player_blocks[targets[deepest]].odata.x;
			point[2] += player_blocks[targets[deepest]].odata.z;
			dpt[0] += player_blocks[targets[deepest]].odata.vx;
			dpt[1] += player_blocks[targets[deepest]].odata.vz;
	
			point[0] /= (float)(tcnt+1);
			point[2] /= (float)(tcnt+1);
			dpt[0] /= (float)(tcnt+1);
			dpt[1] /= (float)(tcnt+1);
	
			point[0] += f*YD2UN*4.0f;
	
			if( point[0]*f < 2.0f*YD2UN + player_blocks[targets[deepest]].odata.x*f )
				point[0] = player_blocks[targets[deepest]].odata.x + 2.0f*YD2UN;
			ppdata->bptarget = targets[deepest];
		}
		pdrone->zpoint[0] = point[0];
		pdrone->zpoint[1] = point[2];
		pdrone->zpv[0] = dpt[0];
		pdrone->zpv[1] = dpt[1];
		
		zbase = zone_thinkmin_ht[DEFH];
		zrnd = zone_thinkmax_ht[DEFH] + 1 - zbase;
		pdrone->move_time = zbase + randrng(zrnd);
	}

	dir  = ptangle( &(ppdata->odata.x), point );
	dir = (dir - (int)RD2GR(cambot.theta)) & 1023;

	newstick = dir2stick(dir);
	pdrone->but = (ppdata->odata.plyrmode == MODE_DBPS) ? P_C : 0;
	
	if (pdistxz( &(ppdata->odata.x), point ) < 3.0f)
	{
		pdrone->stick = 24;
		if (game_info.ball_carrier != -1)
		{
			ppdata->drone_face = p2p_angles[ppdata->plyrnum][game_info.ball_carrier];
			ppdata->odata.flags |= PF_DRONEFACE;
		}
	}
	else if ((pdistxz( &(ppdata->odata.x), point ) > YD2UN) ||
		(pdrone->os_time > 30))
		pdrone->stick = newstick;


	if (game_info.team_play[!game_info.off_side]->flags & PLF_FAKEZONE)
	{
	#if 1
		if (ppdata->fake_zone_time > 0)
		{
			ppdata->fake_zone_time -= 1;
			if (ppdata->fake_zone_time == 0)
				ppdata->job = JOB_COVER;
		}
	#endif
	}
			
	#ifdef ZONE_DEBUG
	pdrone->ztcnt = tcnt;
	for( i = 0; i < tcnt; i++ )
		pdrone->ztargets[i] = targets[i];
	#endif
}

//////////////////////////////////////////////////////////////////////////////
static void _drone_kicker( fbplyr_data *ppdata )
{
	float	point[3];
	int		dir, my_qb;

	if ((game_info.play_type == PT_KICKOFF) ||
		(game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL))
	{
		switch( game_info.game_mode )
		{
			case GM_PRE_SNAP:
				my_qb = game_info.captain_player[ppdata->team];
				pdrone->stick = 24;
				pdrone->but = player_blocks[my_qb].but_cur;
				break;

			case GM_PRE_KICKOFF:
				// move toward ball
				point[0] = ball_obj.tx;
				point[2] = ball_obj.tz;
				dir  = ptangle( &(ppdata->odata.x), point );
				dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
				pdrone->stick = dir2stick( dir );
				pdrone->but = 0;
				break;

			case GM_IN_PLAY:
				if (game_info.ball_carrier != -1)
				{
					// move toward ball
					point[0] = player_blocks[ball_obj.int_receiver].odata.x;
					if (game_info.off_side)
						point[2] = player_blocks[ball_obj.int_receiver].odata.z + 11.0f;		// aim for hands...of holder
					else
						point[2] = player_blocks[ball_obj.int_receiver].odata.z - 11.0f;		// aim for hands...of holder
					dir  = ptangle( &(ppdata->odata.x), point );
					dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
					pdrone->stick = dir2stick( dir );
					pdrone->but = 0;
				}
				break;

			default:
				pdrone->stick = 24;
				pdrone->but = 0;
				break;
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////
// gives blocking assignments to all available blockers.  Doesn't matter who
// calls this, but it should be called every few ticks, ideally by the first
// offensive lineman. (he's got the lowest plyrnum)
static void assign_blockers( void )
{
	int		assigned[7] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
	int		bcnt=0,acnt, i,j,k;
	int		done=FALSE;
	int		carrier;

	rcnt = 0;
	for( i = 0; i < 7; i++ )
	{
		urgent[i] = i;
		blockers[i] = -1;
		rushers[i] = -1;
	}

	// figure out who we're all protecting
	if( game_info.ball_carrier == -1 )
	{
		if( ball_obj.int_receiver == -1 )
		{
			// in this case, just fall back on protecting the offensive side QB
			carrier = game_info.off_side ? 10 : 3;
		}
		else
			carrier = ball_obj.int_receiver;
	}
	else
		carrier = game_info.ball_carrier;


	// identify rushers -- any drone with a rushing assignment and any player
	// within 5 yards of the line of scrimmage.  Also any drone assigned to cover
	// the QB.  If carrier is weapon, all enemy players are rushers.
	for( i = 7 * (!game_info.off_side); i < 7 + 7 * (!game_info.off_side); i++ )
	{
		if( player_blocks[i].odata.adata[0].animode & (MODE_PUPPET|MODE_REACTING|MODE_PUPPETEER) )
			continue;

		if(( player_blocks[i].job == JOB_RUSH ) ||
			( player_blocks[carrier].odata.plyrmode == MODE_WPN ) ||
			(( player_blocks[i].job == JOB_COVER) && ( drone_blocks[i].target == game_info.ball_carrier )) ||
//			(( game_info.plyr_control[!game_info.off_side] == i ) &&
			(( player_blocks[i].station != -1 ) &&
			(FIELDX(player_blocks[i].odata.x) < (game_info.los+5))))
			rushers[rcnt++] = i;
	}

	// identify blockers -- all interruptible offensive players with JOB_BLOCK
	// set except for ball carrier.
	for( i = 7 * game_info.off_side; i < 7 + 7 * game_info.off_side; i++ )
	{
		if( i == carrier )
			continue;

		if( player_blocks[i].odata.adata[0].animode & MODE_UNINT )
			continue;

		if( player_blocks[i].job == JOB_BLOCK )
			blockers[bcnt++] = i;
	}

	// compute intercept times for rushers to ball carrier.
	for( i = 0; i < 7; i++ )
		rush_times[i] = -1;

	for( i = 0; i < rcnt; i++ )
	{
		float	spd, cspd;
		obj_3d	*pb = (obj_3d *)(player_blocks + rushers[i]);

		// speed is basic mode speed or current speed, whichever is greater
		if( player_blocks[rushers[i]].odata.plyrmode == MODE_LINE )
			spd = LINE_TURBO_SPEED;
		else
			spd = DB_SPEED;

		cspd = fsqrt( pb->vx * pb->vx + pb->vz * pb->vz );

		if( cspd > spd )
			spd = cspd;

		rush_times[i] = int_time( &(player_blocks[rushers[i]].odata.x),
								  spd,
								  &(player_blocks[carrier].odata.x),
								  &(player_blocks[carrier].odata.vx),
								  8.0f, IT_LOWEST );

		// if int time is >= 10000, use 1000 + distance (for sorting)
		if( rush_times[i] >= 10000 )
			rush_times[i] += (int)(distxz[rushers[i]][carrier]);
	}

	// sort rushers by urgency
	for( i = rcnt; i < 7; i++ )
		urgent[i] = -1;
	qsort( urgent, rcnt, sizeof( int ), icmp );

	// quit right now if there's either no blockers or no rushers
	if(( rcnt == 0 ) || ( bcnt == 0 ))
		return;

	// compute intercept times for all blockers to all rushers.  Intercept
	// time is how many ticks it'll take to reach rusher from blocker's
	// current position if rusher moves at current velocity.
	// NOTE that the int_times array is indexed by blocker and rusher number,
	// not player number.  (i.e. defensive lineman wearing number 56 is player
	// number 2 but is rusher number 0.)

	for( i = 0; i < 7; i++ )
		for( j = 0; j < 7; j++ )
			int_times[i][j] = -1;

	// weight in favor of current assignments (subtract 10 from times)
	for( i = 0; i < rcnt; i++ )
	{
		for( j = 0; j < bcnt; j++ )
		{
			float	spd, cspd;
			obj_3d	*pb = (obj_3d *)(player_blocks + blockers[j]);

			// speed is basic mode speed or current speed, whichever is
			// greater
			if( player_blocks[blockers[j]].odata.plyrmode == MODE_LINE )
				spd = LINE_TURBO_SPEED;
			else
				spd = WPN_SPEED;

			cspd = fsqrt( pb->vx * pb->vx + pb->vz * pb->vz );

			if( cspd > spd )
				spd = cspd;

//			int_times[i][j] = (int)distxz[rushers[i]][blockers[j]];
			int_times[i][j] = (int)int_time( &(player_blocks[blockers[j]].odata.x),
											 spd,
											 &(player_blocks[rushers[i]].odata.x),
											 &(player_blocks[rushers[i]].odata.vx),
											 8.0f, IT_LOWEST );
			weighted_int_times[i][j] = int_times[i][j];

			// subtract ten from current assignments
			if( drone_blocks[blockers[j]].target == rushers[i] )
				weighted_int_times[i][j] -= 10;
		}
	}

	// adjust times based on urgency
	for( i = 0; i < rcnt; i++ )
	{
		float	sf;
		sf = ((float)(9-i))/3.0f;

		for( j = 0; j < bcnt; j++ )
		{
			weighted_int_times[urgent[i]][j] = (int)((float)(weighted_int_times[urgent[i]][j]) * sf);
		}
	}

	// assign blockers to rushers, in order of rusher urgency.  when we run
	// out of rushers, assign doubles.  when we run out of blockers, quit.
	for( i = 0; i < 7; i++ )
		assigned[i] = FALSE;

	acnt = 0;
	i = 0;

	while( acnt < bcnt )
	{
		int		best;

		// just assign anyone.  routine below will optimize
		best = acnt;

		// mark him as assigned
		assigned[best] = TRUE;

		// assign him
		drone_blocks[blockers[best]].target = urgent[i];

		acnt++;
		i = (i + 1 ) % rcnt;
	}

	// Now go through all blocking assignments and see if any two blockers
	// swapping assignments will improve matters any.  If so, swap them and
	// restart this check.  Repeat until further swapping doesn't yield any
	// improvements.
	do {
		int		swap = FALSE;

		done = TRUE;

		for( i = 0; (i < bcnt) && !swap; i++ )
		{
			for( j = i+1; (j < bcnt) && !swap; j++ )
			{
				int		dcur,dswap;
				int		it,jt;

				it = drone_blocks[blockers[i]].target;
				jt = drone_blocks[blockers[j]].target;

				dcur = weighted_int_times[it][i] + weighted_int_times[jt][j];
				dswap = weighted_int_times[jt][i] + weighted_int_times[it][j];

				if( dcur > dswap )
				{
					// swap!
					drone_blocks[blockers[i]].target = jt;
					drone_blocks[blockers[j]].target = it;

					done = FALSE;
					swap = TRUE;
				}
			}
		}
	} while( !done );

	#ifdef BLOCK_DEBUG
	#ifdef DEBUG
	if( halt )
	{
		fprintf( stderr, "\n##  int " );
		for( j = 0; j < bcnt; j++ )
			fprintf( stderr, " %2x  ", JERSEYNUM( blockers[j] ));
		fprintf( stderr, "\n" );

		for( i = 0; i < rcnt; i++ )
		{
			fprintf( stderr, "%2x: %3d ", JERSEYNUM(rushers[urgent[i]] ), rush_times[urgent[i]] );
			for( j = 0; j < bcnt; j++ )
			{
				if( drone_blocks[blockers[j]].target == urgent[i] )
					fprintf( stderr, "[%3d]", int_times[urgent[i]][j] );
				else
					fprintf( stderr, " %3d ", int_times[urgent[i]][j] );
			}
			fprintf( stderr, "\n" );
		}
	}
	#endif
	#endif

	// targets are currently rusher numbers.  translate them to player numbers.
	for( k = 0; k < bcnt; k++ )
		drone_blocks[blockers[k]].target = rushers[drone_blocks[blockers[k]].target];

}

//////////////////////////////////////////////////////////////////////////////
#if 0
static int nearest_available_blocker( int *times, int *assigned, int count )
{
	int		best,i;

	// init to first available
	for( best = 0; assigned[best]; best++ );

	// now see if there's any better
	for( i = best+1; i < count; i++ )
	{
		if(( times[i] < times[best] ) && ( !assigned[i] ))
			best = i;
	}

	return best;
}
#endif
//////////////////////////////////////////////////////////////////////////////
// icmp() - intercept time compare function for qsort().
static int icmp( const void *p1, const void *p2 )
{
	int		i1,i2;

	i1 = *((int *)p1);
	i2 = *((int *)p2);

	if( rush_times[i2] > rush_times[i1] )
		return -1;

	if( rush_times[i2] < rush_times[i1] )
		return 1;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// blocker moves to protect target from rusher
// if rusher is within BLOCK_DIST yards of target, move to intercept rusher.
// otherwise move toward a point BLOCK_DIST from target along line connecting
// target and rusher.

#define BLOCK_DIST		(7.0f*YD2UN)
#define BLOCK_DIST2		(11.0f*YD2UN)

static void move_to_block( int blocker, int rusher, int target, float vmax )
{
	obj_3d			*pb,*pr,*pt;
	float			tgt[3], d, block_dist;
	int				dir;

	// temp band-aid
	if( blocker == -1 )
		return;
	if( rusher == -1 )
		return;
	if( target == -1 )
		return;

	pb = (obj_3d *)(player_blocks + blocker);
	pr = (obj_3d *)(player_blocks + rusher);
	pt = (obj_3d *)(player_blocks + target);

	block_dist = (game_info.play_type == PT_KICKOFF) ? BLOCK_DIST2 : BLOCK_DIST;

	d = distxz[rusher][target];

	if( d < block_dist )
	{
		move_to_intercept( blocker, rusher, vmax );
		return;
	}

	tgt[0] = pt->x + block_dist/d * ( pr->x - pt->x );
	tgt[2] = pt->z + block_dist/d * ( pr->z - pt->z );

	dir = ptangle( &(pb->x), tgt );
	dir = (dir - (int)RD2GR(cambot.theta)) & 1023;

	pdrone->stick = dir2stick( dir );
	pdrone->but = 0;

	// if within 2 yards of this point and already not moving, or within half
	// a yard moving or not, face opponent or hold still
	d = fsqrt((pb->x-tgt[0])*(pb->x-tgt[0])+(pb->z-tgt[2])*(pb->z-tgt[2]));
	if((( d < 9.0f ) && (player_blocks[blocker].stick_cur == 24)) || ( d < 3.0f ))
		pdrone->stick = 24;
}

//////////////////////////////////////////////////////////////////////////////
// attacker moves to intercept target
static void move_to_intercept( int attacker, int target, float vmax )
{
	fbplyr_data		*pa,*pt;
	float			xot,zot,vxt,vzt;
	float			tgt[3];
	int				dir;
	int				itime;

	pa = player_blocks + attacker;
	pt = player_blocks + target;

	xot = pt->odata.x;
	zot = pt->odata.z;

	vxt = pt->odata.vx;
	vzt = pt->odata.vz;

	itime = intercept_time( attacker, target, vmax );

	// itime is best time to impact.  Aim for where target will be then.
	// if itime >= 1000, we can't catch the guy.  Just aim for where he
	// is right now.
	if( itime >= 1000 )
		itime = 0;

	tgt[0] = xot + (float)itime * vxt;
	tgt[2] = zot + (float)itime * vzt;

	dir = ptangle( &(pa->odata.x), tgt );
	dir = (dir - (int)RD2GR(cambot.theta)) & 1023;
	pdrone->stick = dir2stick( dir );
	pdrone->but = 0;
}

//////////////////////////////////////////////////////////////////////////////
static int intercept_time( int attacker, int target, float vmax )
{
	fbplyr_data		*pa,*pt;
	float			xoa,zoa,xot,zot,vxt,vzt, dx,dz, md, mf;
	float			a,b,c,b2m4ac;
	int				root1,root2;

	pa = player_blocks + attacker;
	pt = player_blocks + target;

	xoa = pa->odata.x;
	zoa = pa->odata.z;

	xot = pt->odata.x;
	zot = pt->odata.z;

	vxt = pt->odata.vx;
	vzt = pt->odata.vz;

	dx = xoa - xot;
	dz = zoa - zot;

	// looking for cylinder collision, not point collision.
	// pretend we're 8 units closer than we are
	md = fsqrt( dx*dx + dz*dz );

	if( md < 0.01f )
		return 1;

	mf = md - 8.0f;
	if( mf < 0.0f )
		mf = 0.0f;
	dx = dx / md * mf;
	dz = dz / md * mf;

	// now solve the quad equation
	a = vxt * vxt + vzt * vzt - (vmax*vmax);
	b = -2.0f * (vxt * dx + vzt * dz);
	c = dx * dx + dz * dz;

	if( fabs(a) < 0.01f )
	{
		// speeds are the same.  if b >= 0, target is moving away
		// and we can't catch him.
		if( b >= -0.0001f )
			return 1000;

		root1 = (int)(-1.0f * c / b);

		if( root1 > 1000 )
			root1 = 1000;

		return root1;
	}

	b2m4ac = b * b - 4.0f * a * c;

	if( b2m4ac < 0.0f )
	{
		// target is too fast.  Can't catch him.
		return 1000;
	}

	root1 = (int)((-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a));
	root2 = (int)((-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a));

	if(( root1 < 0 ) && ( root2 < 0 ))
		return 1000;

	if( root1 < 0 )
		return root2;

	if( root2 < 0 )
		return root1;

	if( root1 < root2 )
		return root1;

	return root2;

}

//////////////////////////////////////////////////////////////////////////////
static float pdistxz( float *p1, float *p2 )
{
	return fsqrt((p1[0]-p2[0]) * (p1[0]-p2[0]) + (p1[2] - p2[2]) * (p1[2] - p2[2]));
}

//////////////////////////////////////////////////////////////////////////////
// figure out how much, if any, of a fake the covering db will eat
static int sell_fake( int receiver, int cover, int prob, int forced_time )
{
	int		p,i,wpn;
	int		covers[5], ncover;

	// if nobody's covering him, don't bother
	if( cover == -1 )
		return FALSE;

	// if defender isn't on man to man coverage, don't bite
	if( player_blocks[cover].job != JOB_COVER )
		return FALSE;

	p = (int)((float)prob * bozo_cookie_ht[DEFH]);
	
	// quit if he doesn't bite at all
	if( randrng( 100 ) > p )
		return FALSE;

	// if fake_out command had a forced fake out time, use it
	if (forced_time)
		drone_blocks[cover].fake_time = forced_time;
	else
		drone_blocks[cover].fake_time = bozo_base_ht[DEFH] + randrng(bozo_rand_ht[DEFH]);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
static float nearest_defender( fbplyr_data *ppdata, int *who )
{
	int 	i;
	float	min, cur;

	min = 1000.0f;
	if (who) *who = -1;
//	for( i = 3; i < 7; i++ )
	for( i = !ppdata->team * 7 + 3; i < !ppdata->team * 7 + 7; i++ )
	{
//		cur = distxz[ppdata->plyrnum][i+(7*(!ppdata->team))];
		cur = distxz[ppdata->plyrnum][i];
		if( cur < min )
		{
			min = cur;
			if (who) *who = i;
		}
	}

//	fprintf( stderr, "recvr %2x, closest bad guy: %f\n",
//		JERSEYNUM(ppdata->plyrnum), min );

	return min;
}

#if USE_OLD_QB
//////////////////////////////////////////////////////////////////////////////
static void update_receiver_data( void )
{
	fbplyr_data	*ppdata;
	obj_3d		*pobj, *pwpn, *pdef;
	int			i,j, least, is_blocked;

	wcnt = 0;
	dcnt = 0;

	// go home if there's no drone QB
	if ((p_status & TEAM_MASK(game_info.off_side)) ||
		(game_info.ball_carrier == -1) ||
		(player_blocks[game_info.ball_carrier].odata.plyrmode != MODE_QB))
		return;

	// find defenders.  That's any drone with a JOB_COVER or JOB_ZONE assignment,
	// or any human-controlled player.
	for( i = 7*(!game_info.off_side); i < 7+7*(!game_info.off_side); i++ )
	{
		ppdata = player_blocks + i;

//		if( ppdata->plyrnum == game_info.plyr_control[!game_info.off_side] )
		if( ISHUMAN( ppdata ))
		{
			// human player.  count him
			defenders[dcnt++] = i;
		}
		else
		{
			// drone.  check his job
			if(( ppdata->job == JOB_COVER ) || ( ppdata->job == JOB_ZONE ))
				// pass defender.  count him
				defenders[dcnt++] = i;
		}
	}

	for (i = dcnt; i < 7; i++)
		defenders[i] = -1;

	// find eligible receivers.  That's any weapon who doens't have a ball and
	// isn't blocking.
	for (i = 7*(game_info.off_side); i < 7+7*(game_info.off_side); i++)
	{
		ppdata = player_blocks + i;
		pobj = (obj_3d *)ppdata;

		if ((ppdata->job == JOB_BLOCK) ||
			(pobj->plyrmode != MODE_WPN) ||
			(ppdata->plyrnum == game_info.ball_carrier))
			continue;

		weapons[wcnt++] = i;
	}

	for (i = wcnt; i < 7; i++)
		weapons[i] = -1;

	// now compute intercept times.  The method varies, depending on what kind
	// of defender it is.
	for (i = 0; i < wcnt; i++)
	{
		least = 10000;
		pwpn = (obj_3d *)(player_blocks+weapons[i]);
		for (j = 0; j < dcnt; j++)
		{
			pdef = (obj_3d *)(player_blocks+defenders[j]);
//			if (game_info.plyr_control[!game_info.off_side] == defenders[j])
			if (player_blocks[defenders[j]].station != -1)
			{
				// human defender
				cover_times[i][j] = (int)int_time( &(pdef->x), HUMAN_DEF_TURBO_SPEED, &(pwpn->x), &(pwpn->vx), 10.0f, IT_LOWEST );
				cover_types[i][j] = CT_HUMAN;
			}
			else if (player_blocks[defenders[j]].job == JOB_ZONE)
			{
				// zone defender
				cover_times[i][j] = (int)int_time( &(pdef->x), DB_SPEED, &(pwpn->x), &(pwpn->vx), 10.0f, IT_LOWEST );
				cover_types[i][j] = CT_ZONE;
			}
			else if (drone_blocks[defenders[j]].target == weapons[i])
			{
				// man coverage on us
				cover_times[i][j] = (int)int_time( &(pdef->x), DB_SPEED, &(pwpn->x), &(pwpn->vx), 10.0f, IT_LOWEST );
				cover_times[i][j] += 3 * drone_blocks[defenders[j]].fake_time;
				cover_types[i][j] = CT_MAN_US;
			}
			else
			{
				// man coverage on someone else
				cover_times[i][j] = 45 + (int)int_time( &(pdef->x), DB_SPEED, &(pwpn->x), &(pwpn->vx), 10.0f, IT_LOWEST );
				cover_types[i][j] = CT_MAN_OTHER;
				if (cover_times[i][j] < 0)
				{
					fprintf( stderr, "negative ct on man other\n" );
					fprintf( stderr, "int_time = %f %d\n",
						int_time( &(pdef->x), DB_SPEED, &(pwpn->x), &(pwpn->vx), 8.0f, IT_LOWEST ),
						(int)int_time( &(pdef->x), DB_SPEED, &(pwpn->x), &(pwpn->vx), 8.0f, IT_LOWEST ));
				}
			}

			if (!j)
				least = cover_times[i][j];
			else
				least = LESSER( least, cover_times[i][j] );
		}
		open_times[i] = least;
		#ifdef DRONEQB_DEBUG
//		if ((halt) || ((ball_obj.int_receiver == weapons[i]) && (game_info.ball_carrier == -1)))
		if ((halt) && ((ball_obj.int_receiver == weapons[i]) && (game_info.ball_carrier == -1)))
		{
			fprintf( stderr, "RECVR> %2x open %d\n",
				JERSEYNUM(weapons[i]),
				open_times[i] );
		}
		#endif
	}

	// figure sack time
	sack_time = 5000;
	for( i = 0; i < rcnt; i++ )
	{
		blocked[i] = FALSE;

		// search for a blocker assigned to this guy
		for( j = 0; (j<7) && (blockers[j] != -1); j++ )
		{
			if( drone_blocks[blockers[j]].target == rushers[i] )
			{
				if( rush_times[i] >= int_times[i][j])
					blocked[i] = TRUE;
			}
		}
		if( !blocked[i] )
			sack_time = LESSER(sack_time,rush_times[i]);
	}
}
#else
//////////////////////////////////////////////////////////////////////////////
// NEW version
static void update_receiver_data( void )
{
	fbplyr_data	*ppdata;
	obj_3d		*pobj, *pwpn, *pdef;
	int			i,j, fireman;
#ifdef DEBUG
	int			sw;
#endif
	float		throw_speed;

	wcnt = 0;
	dcnt = 0;

#ifndef JASON
	// go home if there's no drone QB
	if ((p_status & TEAM_MASK(game_info.off_side)) ||
		(game_info.ball_carrier == -1) ||
		(player_blocks[game_info.ball_carrier].odata.plyrmode != MODE_QB))
		return;
#endif

	// find defenders.  That's any drone with a JOB_COVER or JOB_ZONE assignment,
	// or any human-controlled player.
	for( i = 7*(!game_info.off_side); i < 7+7*(!game_info.off_side); i++ )
	{
		ppdata = player_blocks + i;

		if (ISHUMAN( ppdata ))
		{
			// human player.  count him
			defenders[dcnt++] = i;
		}
		else
		{
			// drone.  check his job
			if(( ppdata->job == JOB_COVER ) || ( ppdata->job == JOB_ZONE ))
				// pass defender.  count him
				defenders[dcnt++] = i;
		}
	}

	for (i = dcnt; i < 7; i++)
		defenders[i] = -1;

	// find eligible receivers.  That's any weapon who doens't have a ball and
	// isn't blocking.
	for (i = 7*(game_info.off_side); i < 7+7*(game_info.off_side); i++)
	{
		ppdata = player_blocks + i;
		pobj = (obj_3d *)ppdata;

		if ((ppdata->job == JOB_BLOCK) ||
			(pobj->plyrmode != MODE_WPN) ||
			(ppdata->plyrnum == game_info.ball_carrier))
			continue;

		weapons[wcnt++] = i;
	}

	for (i = wcnt; i < 7; i++)
		weapons[i] = -1;

	// figure the QB's likely throw speed
	throw_speed = (game_info.team_fire[game_info.off_side]) ? 7.50f : 5.70f;

	// set fireman
	fireman = (game_info.team_fire[game_info.off_side]) ? tdata_blocks[game_info.off_side].last_receiver : -1;

#ifdef DEBUG
	sw = get_player_sw_current() & P4_D;

	if (sw) fprintf( stderr, "\n\n" );
#endif

	// for each receiver, compute two values:  The likelihood of making the catch,
	// and the number of additional yards they're likely to get if they do make the
	// catch.
	for (i = 0; i < wcnt; i++)
	{
		float		throw_dist,ball_time,def_dist;
		float		catch_pos[3];

#ifdef DEBUG
		if (sw) fprintf( stderr, "receiver %x:\n", JERSEYNUM(weapons[i]));
#endif

		pwpn = (obj_3d *)(player_blocks+weapons[i]);

		coverage[i] = 0;
		likely_yards[i] = 0;

		// first, estimate time for the ball to reach him
		throw_dist = distxz[weapons[i]][game_info.ball_carrier];
		ball_time = throw_dist / throw_speed;

		// now use that time figure to guess at where we'd likely make
		// the catch.
		catch_pos[0] = pwpn->x + ball_time * pwpn->vx;
		catch_pos[1] = 0.0f;
		catch_pos[2] = pwpn->z + ball_time * pwpn->vz;

		// count defenders who could reach that X in the time it'd take the ball
		// to get there.  Figure ten ticks to react, regardless of cookie or zone
		// status.  Human player is likely to take over this defender and react
		// immediately.  However, showing up ten ticks late is time enough to break
		// up the pass, so just leave this factor out altogether.  Also, figure that
		// getting within 12.0f units of the X is enough to break it up.  Futher, figure
		// that any defender who does react will be moving at HUMAN_DEF_TURBO_SPEED.
		// FIRE!: If team is on fire and this is the fire receiver, add ten to the time
		// it'll take to make the intercept, to account for the no-bobble effect.

		for (j = 0; j < dcnt; j++)
		{
			float	x_dist,x_time;

			// how far from me to X?
			x_dist = pdistxz(&(player_blocks[defenders[j]].odata.x),catch_pos);
			x_dist = GREATER(0.0f,x_dist-12.0f);
			x_time = x_dist / HUMAN_DEF_TURBO_SPEED;

			if (weapons[i] == fireman)
				x_time += 10;

			// if I'm close enough, inc coverage on wpn
			if (x_time <= ball_time)
			{
				coverage[i]++;
				#ifdef DEBUG
				if (sw) fprintf( stderr, "  covered by %x\n", JERSEYNUM(defenders[j]));
				#endif
			}
		}

		// now estimate the post-catch yards.  First off, if we're covered, figure we get
		// nothing.  If we're not covered, start by figuring we make it to the end zone, then
		// look at each defender.  If he's bearing 0, figure we get 1/2 the distance between us.
		// for each 45 degrees of bearing, add 1/2 to the distance factor.  Thus if the guy is
		// directly beside us, figure we get 3/2 the distance between us.  if behind, 5/2.
		// If this is the fire guy, figure on another ten yards.

		if (coverage[i])
		{
			likely_yards[i] = 0;
			#ifdef DEBUG
			if (sw) fprintf( stderr, " yards 0\n" );
			#endif
		}
		else 
		{

		likely_yards[i] = likely_gain( weapons[i] );
		if (weapons[i] == fireman)
			likely_yards[i] += 10;

//			likely_yards[i] = 110 - FIELDX(pwpn->x);
//			#ifdef DEBUG
//			if (sw) fprintf( stderr, " yards starts at %d\n", likely_yards[i] );
//			#endif
//
//			for (j = (!game_info.off_side) * 7; j < (!game_info.off_side) * 7 + 7; j++)
//			{
//				int		brng;
//				float	brng_factor;
//				int		yards;
//
//				brng = p2p_angles[weapons[i]][j];
//				brng = (1024 + brng) % 1024;
//				brng = (brng > 512) ? 1024 - brng : brng;
//				brng_factor = 0.5f + ((float)brng) / 256.0f;
//				yards = (int)(brng_factor * distxz[weapons[i]][j] / YD2UN);
//				if (fireman == weapons[i])
//					yards += 10;
//
//				likely_yards[i] = LESSER(likely_yards[i],yards);
//				#ifdef DEBUG
//				if (sw) fprintf( stderr, "  defender %d yards %d, new likely = %d\n",
//					JERSEYNUM(j), yards, likely_yards[i] );
//				#endif
//			}
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
// move in a direction that minimizes unhappiness
static void mtm_unhappiness( fbplyr_data *ppdata, float spd1, float spd2,
		float ffactor, float static_friction, float moving_friction,
		float (*uf)(fbplyr_data *, float, float, int ))
{
	int		directions[25] = {0,1,2,3,4,5,6,13,20,27,34,41,48,47,46,45,44,
							  43,42,35,28,21,14,7,24};
	int		i,best,adj_dir,dir,old_best = 25;
	float	uh_dir[26], raw_uh[26], uspd;
	float	px[25],pz[25],pt[3];

	raw_uh[25] = -3.14159;
	uh_dir[25] = -3.14159;

	// sample unhappiness in all directions.
	px[24] = ppdata->odata.x;
	pz[24] = ppdata->odata.z;
	uh_dir[24] = uf( ppdata, ppdata->odata.x, ppdata->odata.z, 24 );
	raw_uh[24] = uh_dir[24];

	if (pdrone->old_stick == 24)
	{
		old_best = 24;
		uh_dir[24] *= ffactor;
		uh_dir[24] -= static_friction;
	}

	for( i = 0; i < 24; i++ )
	{
		dir = 1023 & (dir49_table[directions[i]] + (int)RD2GR(cambot.theta));
		if ((adj_dir = dir49_table[ppdata->stick_cur]) < 0) adj_dir = 0;
		adj_dir =  1023 & (adj_dir + (int)RD2GR(cambot.theta) - 256 + (512*ppdata->team));

		if ((adj_dir > 338) && (adj_dir < 692))
			uspd = spd2;
		else
			uspd = spd1;

		px[i] = ppdata->odata.x;
		pz[i] = ppdata->odata.z;

		px[i] += FRELX(0.0f,uspd,dir);
		pz[i] += FRELZ(0.0f,uspd,dir);

		uh_dir[i] = uf( ppdata, px[i], pz[i], directions[i] );
		raw_uh[i] = uh_dir[i];

		if (directions[i] == pdrone->old_stick)
		{
			old_best = i;
			uh_dir[i] *= ffactor;
			uh_dir[i] -= moving_friction;
		}

		if ((directions[(i+1)%24] == pdrone->old_stick) ||
			(directions[abs((i-1)%24)] == pdrone->old_stick))
		{
			uh_dir[i] *= (1.0f - 0.66f * (1.0f - ffactor));
			uh_dir[i] -= (0.66f * moving_friction);
		}
	}

	// pick best direction
	best = 24;
	for( i = 0; i < 24; i++ )
		if( uh_dir[best] > uh_dir[i] )
			best = i;

	if ((uh_dir[24] < moving_friction) ||
		((uh_dir[24] < static_friction) &&
		(pdrone->old_stick == 24)))
		best = 24;

#ifdef DEBUG
#if 0
	if (uf==qb_unhappiness)
	{
		if (directions[best] != pdrone->old_stick)
		{
			fprintf( stderr, "%d> old %d(%5.1f %5.1f)  new %d(%5.1f %5.1f)\n",
				pcount,
				pdrone->old_stick, raw_uh[old_best], uh_dir[old_best],
				directions[best], raw_uh[best], uh_dir[best] );
			WAIT();
		}
	}
#endif
#if 0
	if ((uf==dbps_unhappiness) && halt)
	{
		fprintf( stderr, "\n----------\n" );
		for( i = 0; i < 25; i++ )
		{
			if ((dir = dir49_table[directions[i]]) < 0) dir = 0;
			dir = 1023 & (dir + (int)RD2GR(cambot.theta));
			fprintf( stderr, "i: %3d  d: %3d  dir: %3d  x,z: %7.2f,%7.2f un: %7.2f %c\n",
				i, directions[i], dir, px[i],pz[i], uh_dir[i], (best==i) ? '*':' ' );
		}
		fprintf( stderr, "\n----------\n" );
	}
#endif
#endif

	pdrone->stick = directions[best];
}

//////////////////////////////////////////////////////////////////////////////

#define YD2		(YD2UN*2.0f)
#define YD3		(YD2UN*3.0f)
#define YD5		(YD2UN*5.0f)
#define YD8		(YD2UN*8.0f)
#define YD10	(YD2UN*10.0f)
#define YD12	(YD2UN*12.0f)
#define YD15	(YD2UN*15.0f)

//////////////////////////////////////////////////////////////////////////////
// compute defensive lineman unhappiness at point x z
static float dline_unhappiness( fbplyr_data *ppdata, float x, float z, int dir )
{
	int		i;
	float	un,dx,dz,rd,f;
	fline_t	*pform;

	un = 0.0f;
	f = ppdata->team ? -1.0f : 1.0f;

	// avoid teammates
	for (i = (ppdata->team)*7; i < (ppdata->team)*7+7; i++)
	{
		if (i == ppdata->plyrnum)
			continue;

		dx = x - player_blocks[i].odata.x;
		dz = z - player_blocks[i].odata.z;

		rd = fsqrt(dx*dx+dz*dz);

		if (rd < YD3)
			un += ((YD3-rd)*(YD3-rd));
	}

	// get the QB
	if (game_info.ball_carrier == -1)
	{	// ball in flight
		if (ball_obj.int_receiver != -1)
		{
			dx = x - player_blocks[ball_obj.int_receiver].odata.x;
			dz = z - player_blocks[ball_obj.int_receiver].odata.z;

			un += (dx*dx + dz*dz);
		}
	}
	else if (player_blocks[game_info.ball_carrier].odata.plyrmode == MODE_QB)
	{	// qb has ball.  rush/contain
		// if dz > 5 yards, move laterally to meet the qb, else go straight in
		dx = x - player_blocks[game_info.ball_carrier].odata.x;
		dz = z - player_blocks[game_info.ball_carrier].odata.z;

		un += (dx*dx + 6.0f*dz*dz);
	}
	else
	{	// wpn has ball.  kill
		dx = x - player_blocks[game_info.ball_carrier].odata.x;
		dz = z - player_blocks[game_info.ball_carrier].odata.z;

		un += (dx*dx + dz*dz);
	}

	// lineman wants to stay in bounds
	if ((fabs(z) >= (FIELDW-YD2UN)) &&
		(fabs(z) > fabs(ppdata->odata.z)))
		un += 10000.0f;

	return un;
}

//////////////////////////////////////////////////////////////////////////////
// compute qb unhappiness at point x z
static float qb_unhappiness( fbplyr_data *ppdata, float x, float z, int dir )
{
	float	un=0.0f, rd, dx,dz, f, sf;
	int		i, df5;

	f = ppdata->team ? -1.0f : 1.0f;

	// DYNAMIC STUFF
	// qb doesn't like being within 5 yards of a rusher
	// qb doesn't like being within 10 yards of an unblocked rusher
	for (i = 0; i < rcnt; i++)
	{
		dx = x - player_blocks[rushers[i]].odata.x;
		dz = z - player_blocks[rushers[i]].odata.z;

		rd = fsqrt(dx*dx+dz*dz);

		if (rd < YD8)
			un += ((YD8-rd)*(YD8-rd));

		if (!blocked[i] && (rd < YD12))
			un += ((YD12-rd)*(YD12-rd));
	}

	// qb doesn't like dropping back too far
	rd = WORLDX(game_info.los);
	dx = fabs(rd-x);
	if (dx > qb_dropback)
		un += 10.0f*((dx-qb_dropback)*(dx-qb_dropback)*(dx-qb_dropback));

	// qb doesn't like being too close to the sideline
	if ((fabs(z) >= (FIELDW-YD2UN)) &&
		(fabs(z) > fabs(ppdata->odata.z)))
		un += 1000.0f;

	// as qb approaches sidelines, scoot forward
	dx = fabs(WORLDX(game_info.los) - x);

	dz = fabs(z);
	dz = fabs(FIELDW - dz);
//	dz = fabs(FIELDW - fabs(z));

	if (dx > dz)
		un += ((dx-dz) * (dx-dz));

	// during the first three seconds of a play, favor the direction we're rolling
	if ((pcount - game_info.play_pcount < 3*tsec) &&
//		(pdrone->flags & (DF_QB_RRIGHT|DF_QB_RLEFT)))
		roll_dir)
	{
//		sf = f * ((pdrone->flags & DF_QB_RLEFT) ? -1.0f : 1.0f);
		sf = f * (float)roll_dir;

		if (sf*z < YD10)
			un += ((YD10 - sf*z) * (YD10 - sf*z));
	}

	return (un);
}

//////////////////////////////////////////////////////////////////////////////
// compute dbps unhappiness at point x z
static float dbps_unhappiness( fbplyr_data *ppdata, float x, float z, int dir )
{
	int		i;
	float	un,dx,dz,d,ix,f;
	fline_t	*pform;

	un = 0.0f;
	f = ppdata->team ? 1.0f : -1.0f;

	// db likes being lined up directly over the guy he's covering
	if( pdrone->target > -1 )
	{
		d = fabs(z - player_blocks[pdrone->target].odata.z);
		un += (d * LESSER(d,35.0f));
	}

	// db doesn't like bumping into his teammates
	for( i = ppdata->team * 7; i < 7 + (ppdata->team * 7); i++ )
	{
		if( i == ppdata->plyrnum )
			continue;

		dx = fabs(x - player_blocks[i].odata.x);
		dz = fabs(z - player_blocks[i].odata.z);
		if ((dx+(dz/2.0f)) < 10.0f)
			un +=  1000.0f * (10.0f-(dx+(dz/2.0f)));
	}

	// db likes being about as far back from the LOS as he lined up
	// and just HATES being any closer to the line than that
	pform = game_info.team_play[ppdata->team]->formation + (ppdata->plyrnum%7);
	ix = WORLDX(game_info.los) - f * pform->x;

	if (x*f < ix*f)
		// we're too far forward
		d = 100.0f * fabs(ix - x);
	else
		// we're too far back
		d = 10.0f * fabs(ix - x);

	un += d;

	return un;
}

//////////////////////////////////////////////////////////////////////////////
// compute carrier unhappiness at point x z
static float carrier_unhappiness( fbplyr_data *ppdata, float x, float z, int dir )
{
	int		i;
	float	un,dx,dz,rd,f;
	fline_t	*pform;

	un = 0.0f;
	f = ppdata->team ? -1.0f : 1.0f;

	// carrier wants to go downfield
	un += 600.0f + 300.0f * f * (ppdata->odata.x - x);

	// avoid tacklers
	for (i = !(ppdata->team)*7; i < !(ppdata->team)*7+7; i++)
	{
		int		is_blocked;

		dx = x - player_blocks[i].odata.x;
		dz = z - player_blocks[i].odata.z;

		rd = fsqrt(dx*dx+dz*dz);

		is_blocked = (player_blocks[i].odata.adata[0].animode &
						(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER));

		if (rd < YD8)
			un += ((YD8-rd)*(YD8-rd));

		if (!is_blocked && (rd < YD10))
			un += ((YD10-rd)*(YD10-rd));
	}

	// avoid teammates
	for (i = (ppdata->team)*7; i < (ppdata->team)*7+7; i++)
	{
		if (i == ppdata->plyrnum)
			continue;

		dx = x - player_blocks[i].odata.x;
		dz = z - player_blocks[i].odata.z;

		rd = fsqrt(dx*dx+dz*dz);

		if (rd <= 12.0f)
			un += 100.0f;
	}

	// as much as he wants to avoid tacklers, carrier WILL NOT
	// retreat upfield.
	if (f*x < f*ppdata->odata.x)
		un += 10000.0f;

	// carrier wants to stay in bounds
	if ((fabs(z) >= (FIELDW-YD2UN)) &&
		(fabs(z) > fabs(ppdata->odata.z)))
		un += 10000.0f;

	return un;
}

//////////////////////////////////////////////////////////////////////////////
// choose new target point for improvising receiver.
static void choose_wr_imp_point( fbplyr_data *ppdata )
{
	// pick a point that's:
	//  on the other side of the x axis
	//  no less than one yard from a sideline
	//  no more than twenty yards upfield of our current position
	//  no less than six yards downfield of the line of scrimmage
	//  no more than fifty yards downfield of the line of scrimmage
	//  no more than eight yards deep into the end zone

	int		xmin,xmax,tfx;
	int		zmax2,iz;
	float	tx,tz;

	xmin = GREATER(game_info.los+6,FIELDX(ppdata->odata.x)-20);
	xmax = LESSER(game_info.los+50,108);
	tfx = xmin + randrng(xmax-xmin+1);
	tx = WORLDX(tfx);

	zmax2 = (int)(FIELDW*FIELDW);
	iz = randrng(zmax2+1);
	tz = fsqrt((float)(iz));
	tz = LESSER((FIELDW-YD2UN),tz);
	if (ppdata->odata.z > 0.0f)
		tz =  -tz;

	pdrone->mx = tx;
	pdrone->mz = tz;

}

#if USE_OLD_QB
//////////////////////////////////////////////////////////////////////////////
// pick a receiver who's at least min_yds downfield, no more than max_yds
// downfield, and open by at least min_open ticks, corrected for throw time.
// choose the best by comparing (open_time*wopen)+(yards downfield*wdistance)
// returns weapon number of best recvr, or -1 if nobody matches the profile
static int find_receiver( fbplyr_data *ppdata, int min_yds, int max_yds, int min_open,
					int wopen, int wdistance )
{
	int			i, downfield, throw_time, open;
	int			best, best_val, val;
	float		dist, throw_vel;
	obj_3d		*ptgt, *pobj;

	best = -1;
	pobj = (obj_3d *)ppdata;

	// anyone in the end zone is a valid target
	if (game_info.los + min_yds > 101)
		min_yds = 0;

	// figure throw velocity
	throw_vel = game_info.team_fire[ppdata->team] ? 7.50f : 5.70f;

	for (i = 0; i < wcnt; i++)
	{
		ptgt = (obj_3d *)(player_blocks + weapons[i]);
		downfield = FIELDX(ptgt->x) - game_info.los;
		downfield = GREATER(downfield,0);
		dist = distxz[ppdata->plyrnum][weapons[i]];
		throw_time = (int)(dist / throw_vel);
		open = open_times[i] - throw_time;

		// far enough downfield?
		if ((min_yds) && (min_yds > downfield))
			continue;

		// in range?
//		if ((max_yds) && (max_yds < downfield))
//			continue;

		// open enough?
		if ((min_open) && (min_open > open))
			continue;

		// far enough from me?
		if (dist < YD5)
			continue;
			
		// this guy fits the profile so far - check him
		val = downfield * wdistance + open * wopen;
		if ((best == -1) ||	(val >= best_val))
		{
			best = i;
			best_val = val;
			best_open = open;
		}
	}

	return best;
}

#else
//////////////////////////////////////////////////////////////////////////////
// find a receiver with no more than 'cover' defenders on him, who'll make for
// a gain of at least 'yards.'
static int find_receiver( fbplyr_data *ppdata, int cover, int yards )
{
	int			i, gain, throw_time, open;
	int			best, best_yards = 0;
	obj_3d		*ptgt, *pobj;

	pobj = (obj_3d *)ppdata;
	best = -1;

	for (i = 0; i < wcnt; i++)
	{
		ptgt = (obj_3d *)(player_blocks + weapons[i]);
		gain = FIELDX(ptgt->x) - game_info.los;
		gain += likely_yards[i];

		// far enough downfield?
		if ((yards == -1) || (gain < yards))
			continue;

		// open enough?
		if (coverage[i] > cover)
			continue;

		// far enough from me?
		if (distxz[ppdata->plyrnum][weapons[i]] <= YD5)
			continue;

		// he'll do.  better than the others?
		if ((best == -1) ||	(yards > best_yards))
		{
			best = i;
			best_yards = yards;
		}
	}

	return best;
}

#endif
//////////////////////////////////////////////////////////////////////////////
// returns TRUE if ppdata has a clean shot at player target.  It's a clean shot
// if the target is the nearest enemy player in ppdata's front 90.
static int clean_shot( fbplyr_data *ppdata, int target )
{
	int i, near=-1;
	
	for (i = 7 * (!ppdata->team); i < 7 + 7 * (!ppdata->team); i++ )
	{
		if ((abs(bearing(ppdata->plyrnum, i)) <= 128 ) &&
			((near == -1) ||
			(distxz[ppdata->plyrnum][i] < distxz[ppdata->plyrnum][near] )))
			near = i;
	}
	
	return (near == target);
}

//////////////////////////////////////////////////////////////////////////////
static int find_diver( fbplyr_data *ppdata )
{
	int		i, best=-1;
	obj_3d	*ptgt;
	
	for (i = 7 * (!ppdata->team); i < 7 + 7 * (!ppdata->team); i++)
	{
		ptgt = (obj_3d *)(player_blocks + i);
		
		// in a dive?
		if ((ptgt->adata[0].seq_index != SEQ_T_CRZDIV) &&
			(ptgt->adata[0].seq_index != SEQ_T_SPRHED))
			continue;
			
		// aimed at us?
		if (player_blocks[i].throw_target != ppdata->plyrnum)
			continue;
			
		// close enough?
		if (distxz[ppdata->plyrnum][i] > 40.0f)
			continue;
			
		if ((best == -1) ||
			(distxz[ppdata->plyrnum][i] < distxz[ppdata->plyrnum][best]))
			best = i;
	}
	
	return best;
}

//////////////////////////////////////////////////////////////////////////////
static int find_attacker( fbplyr_data *ppdata )
{
	int		i, best=-1;
	
//	for (i = 7 * (!ppdata->team); i < 7 + 7 * (!ppdata->team); i++)
	for (i = 0; i < rcnt; i++)
	{
		// close enough?
		if (distxz[ppdata->plyrnum][rushers[i]] > 20.0f)
			continue;

		// in control?
		if (player_blocks[rushers[i]].odata.adata[0].animode & MODE_UNINT)
			continue;

		// blocked?
		if (blocked[i])
			continue;

		if ((best == -1) ||
			(distxz[ppdata->plyrnum][rushers[i]] < distxz[ppdata->plyrnum][best]))
			best = i;
	}
	
	return best;
}

//////////////////////////////////////////////////////////////////////////////
// returns 1 if all three weapons have Z >= QB
// returns -1 if all three weapons have Z <= QB
// return 0 otherwise
// NOTE: A motion receiver will count as both >=QB and <=QB
static int trips_lineup( void )
{
	int		qb, i, ge = 0, le = 0;

	qb = game_info.off_side * 7 + 3;

	for( i = qb+1; i < qb+4; i++ )
	{
		if ((player_blocks[i].odata.z <= player_blocks[qb].odata.z) ||
			(drone_blocks[i].flags & DF_MOTION))
			le += 1;
		if ((player_blocks[i].odata.z >= player_blocks[qb].odata.z) ||
			(drone_blocks[i].flags & DF_MOTION))
			ge += 1;
	}

	if (le == 3)
		return -1;

	if (ge == 3)
		return 1;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// -1 = left, 0 = pocket, 1 = right.
// roll to the side with fewer humans
// if no difference, roll to the side with fewer DBs.
// if still no difference, do random.
// one time in ten, just do it randomly anyway
static int choose_roll_dir( void )
{
	int		i, ldbs=0,rdbs=0,lhum=0,rhum=0;
	float	f = game_info.off_side ? -1.0f : 1.0f;

	// random
	if (randrng(10) == 0)
		return (randrng(3)-1);

	// side count
	for( i = !game_info.off_side * 7; i < !game_info.off_side * 7 + 7; i++ )
	{
		if (player_blocks[i].odata.plyrmode != MODE_DBPS)
			continue;

		if (player_blocks[i].odata.z * f < -3.9f)
		{	// left
			ldbs++;
			if ((ISHUMAN(player_blocks + i)) &&
				(FIELDX(player_blocks[i].odata.x) < game_info.los+1+randrng(5)))
				lhum++;
		}
		else if (player_blocks[i].odata.z * f > 3.9f)
		{	// right
			rdbs++;
			if ((ISHUMAN(player_blocks + i)) &&
				(FIELDX(player_blocks[i].odata.x) < game_info.los+1+randrng(5)))
				rhum++;
		}
	}

//	fprintf( stderr, "ldb %d rdb %d lhum %d rhum %d\n",
//		ldbs, rdbs, lhum, rhum );

	if (lhum > rhum)
		return 1;
	if (lhum < rhum)
		return -1;
	if (ldbs > rdbs)
		return 1;
	if (ldbs < rdbs)
		return -1;

	// tie - random
	return (randrng(3)-1);
}

//////////////////////////////////////////////////////////////////////////////
// evade direction should be 135 degrees from direction to attacker.  Of the
// two possibilities, rule out any that would lead out of bounds if followed
// for five yards.  If none or two are left, choose at random
static int evade_dir( fbplyr_data *ppdata, int attacker )
{
	int			dir1,dir2, ok1 = TRUE,ok2 = TRUE;
	float		x1,z1, x2,z2;

	dir1 = p2p_angles[ppdata->plyrnum][attacker] + 384;
	dir2 = p2p_angles[ppdata->plyrnum][attacker] - 384;

	x1 = ppdata->odata.x + FRELX( 0.0f, 5.0f, dir1 );
	z1 = ppdata->odata.z + FRELZ( 0.0f, 5.0f, dir1 );

	x2 = ppdata->odata.x + FRELX( 0.0f, 5.0f, dir2 );
	z2 = ppdata->odata.z + FRELZ( 0.0f, 5.0f, dir2 );

	if ((fabs(x1) > FIELDHZ) || (fabs(z1) > FIELDW))
		ok1 = FALSE;
	if ((fabs(x2) > FIELDHZ) || (fabs(z2) > FIELDW))
		ok2 = FALSE;

//	fprintf( stderr, "QB is at %f,%f facing %d\n",
//		ppdata->odata.x, ppdata->odata.z, ppdata->odata.fwd );

//	fprintf( stderr, "attacker is at %f,%f, facing %d, bearing %d\n",
//		player_blocks[attacker].odata.x,
//		player_blocks[attacker].odata.z,
//		player_blocks[attacker].odata.fwd,
//		p2p_angles[ppdata->plyrnum][attacker] );

//	fprintf( stderr, "evade directions are %d and %d\n", dir1, dir2 );

//	fprintf( stderr, "x1,z1 = %f,%f (%d)\n", x1,z1, ok1 );
//	fprintf( stderr, "x2,z2 = %f,%f (%d)\n", x2,z2, ok2 );

	dir1 = dir2stick(dir1 + 1024 - (int)RD2GR(cambot.theta)) % 1024;
	dir2 = dir2stick(dir2 + 1024 - (int)RD2GR(cambot.theta)) % 1024;

	if (ok1 && !ok2)
		return dir1;
		
	if (ok2 && !ok1)
		return dir2;

	return (randrng(2) ? ok1 : ok2);
}

//////////////////////////////////////////////////////////////////////////////
static int likely_gain( int plyr )
{
	int			j, brng, yards, ret;
	float		brng_factor;
	fbplyr_data	*ppdata;

	ppdata = player_blocks + plyr;

	// quit if not on offense
	if (ppdata->team != game_info.off_side)
		return 0;

	ret = 110 - FIELDX(ppdata->odata.x);

	for (j = (!game_info.off_side) * 7; j < (!game_info.off_side) * 7 + 7; j++)
	{
		brng = p2p_angles[plyr][j];
		brng = (1024 + brng) % 1024;
		brng = (brng > 512) ? 1024 - brng : brng;
		brng_factor = 0.5f + ((float)brng) / 256.0f;
		yards = (int)(brng_factor * distxz[plyr][j] / YD2UN);

		ret = LESSER(ret,yards);
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////////
