/****************************************************************************/
/*                                                                          */
/* result.c - Functions for figuring out how the play ended.                */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "include/id.h"
#include "include/game.h"
#include "include/player.h"

//
// Play types
//
//PT_NORMAL				0
//PT_KICKOFF			1
//PT_FIELD_GOAL			2
//PT_PUNT				3
//PT_EXTRA_POINT		4

//
// How'd the play end?
//
//PEC_CAR_OOB			0		// carrier out of bounds
//PEC_CAR_TACKLED_IB	1		// carrier tackled in bounds
//PEC_CAR_TACKLED_OWNEZ	2		// carrier tackled in own end zone
//PEC_CAR_TOUCHDOWN		3		// carrier reaches enemy end zone
//PEC_DEAD_BALL_IB		4		// dead ball inbounds
//PEC_BALL_OOB			5		// ball hit ground out of bounds

static void _norm_car_oob( void );
static void _norm_car_ib( void );
static void _norm_car_oez( void );
static void _norm_car_td( void );
static void _norm_deadball( void );
static void _norm_oobball( void );
static void _kick_car_oob( void );
static void _kick_car_ib( void );
static void _kick_car_oez( void );
static void _kick_car_td( void );
static void _kick_deadball( void );
static void _kick_oobball( void );
static void _fg_car_oob( void );
static void _fg_car_ib( void );
static void _fg_car_oez( void );
static void _fg_car_td( void );
static void _fg_deadball( void );
static void _fg_oobball( void );
static void _punt_car_oob( void );
static void _punt_car_ib( void );
static void _punt_car_oez( void );
static void _punt_car_td( void );
static void _punt_deadball( void );
static void _punt_oobball( void );
static void _pat_car_oob( void );
static void _pat_car_ib( void );
static void _pat_car_oez( void );
static void _pat_car_td( void );
static void _pat_deadball( void );
static void _pat_oobball( void );

extern int	fpend;				// from nfl.c
	
void (*result_functions[N_TYPES][N_CAUSES])(void) = {
	{_norm_car_oob,	_norm_car_ib,	_norm_car_oez,	_norm_car_td,	_norm_deadball,	_norm_oobball},
	{_kick_car_oob,	_kick_car_ib,	_kick_car_oez,	_kick_car_td,	_kick_deadball,	_kick_oobball},
	{_fg_car_oob,	_fg_car_ib,		_fg_car_oez,		_fg_car_td,		_fg_deadball,	_fg_oobball},
	{_punt_car_oob,	_punt_car_ib,	_punt_car_oez,	_punt_car_td,	_punt_deadball,	_punt_oobball},
	{_pat_car_oob,	_pat_car_ib,	_pat_car_oez,	_pat_car_td,	_pat_deadball,	_pat_oobball}
};

//////////////////////////////////////////////////////////////////////////////
// play_type = NORMAL

// carrier out of bounds between goal lines.
// possibilities: DOWN, NEW_POSSESSION
static void _norm_car_oob( void )
{
	if( game_info.game_flags & GF_POSS_CHANGE )
	{
		game_info.play_result = PR_NEW_POSSESSION;
		return;
	}
	
	// no possession change.  down.
	game_info.play_result = PR_DOWN;
}

// carrier downed inbounds
// possibilities: DOWN, NEW_POSSESSION
static void _norm_car_ib( void )
{
	// works just like out of bounds between goal lines
	_norm_car_oob();
}

// carrier downed/oob in own end zone
// possibilities: SAFETY, TOUCHBACK
static void _norm_car_oez( void )
{
	if(( game_info.game_flags & GF_POSS_CHANGE ) &&
		( game_info.game_flags & GF_REC_IN_EZ ) && 
		!( game_info.game_flags & GF_LEFT_EZ ))
	{
		game_info.play_result = PR_TOUCHBACK;
	}
	else
	{
		game_info.play_result = PR_SAFETY;
	}
}

// carrier reached enemy end zone
// possibilities: TOUCHDOWN
static void _norm_car_td( void )
{
	game_info.play_result = PR_TOUCHDOWN;
}

// ball declared dead inbounds
// possibilities: INCOMPLETE, DOWNED PUNT, MISSED F.G., F.G.
static void _norm_deadball( void )
{
	if (ball_obj.type == LB_PUNT)
	{
		if( fpend <= 0 )
			game_info.play_result = PR_SAFETY;
		else
			game_info.play_result = PR_DOWNED_PUNT;
	}
	else if (ball_obj.type == LB_FIELD_GOAL)
	{
		if( game_info.game_flags & GF_THROUGH_UPRIGHTS )
			game_info.play_result = PR_FIELD_GOAL;
		else
			game_info.play_result = PR_MISSED_KICK;
	}
	else
		game_info.play_result = PR_INCOMPLETE;
}

// ball went out of bounds
// possibilities: SAFETY, NEW_POSSESSION,
// INCOMPLETE, FUMBLED_OOB, TOUCHBACK
static void _norm_oobball( void )
{
	switch( ball_obj.type )
	{
		case LB_PASS:
		case LB_BOBBLE:
			game_info.play_result = PR_INCOMPLETE;
			break;
		case LB_FUMBLE:
		case LB_LATERAL:
			if( fpend <= 0 )
			{
				// ball oob in own end zone
				if(( game_info.game_flags & GF_POSS_CHANGE ) &&
					( game_info.game_flags & GF_REC_IN_EZ ) && 
					!( game_info.game_flags & GF_LEFT_EZ ))
				{
					game_info.play_result = PR_TOUCHBACK;
				}
				else
				{
					game_info.play_result = PR_SAFETY;
				}
			}
			else if( fpend >= 100 )
			{
				// ball oob in enemy end zone
				game_info.off_side = !game_info.off_side;
				game_info.play_result = PR_TOUCHBACK;
			}
			else
			{
				float ffum;
				
				// ball oob between goal lines
				// spot ball at point of fumble, or point of oob, whichever
				// is farther from enemy end zone.
				if( game_info.game_flags & GF_POSS_CHANGE )
					game_info.play_result = PR_NEW_POSSESSION;
				else
					game_info.play_result = PR_FUMBLED_OOB;
				
				ffum = FIELDX(game_info.fumble_x);
				if( ffum < fpend )
					game_info.play_end_pos = game_info.fumble_x;
			}
			break;
		case LB_PUNT:
			if( fpend >= 100 )
			{
				// ball oob in enemy end zone
				game_info.off_side = !game_info.off_side;
				game_info.play_result = PR_TOUCHBACK;
			}
			else
				game_info.play_result = PR_PUNT_OOB;
			break;
		case LB_FIELD_GOAL:
			game_info.play_result = PR_MISSED_KICK;
			break;
		
		
		default:
			// wasn't a pass, fumble, or lateral.  WTF?
#ifdef DEBUG
			fprintf( stderr, "Bad play result.\n" );
			lockup();
#else
			while(1) ;
#endif
	}
}

//////////////////////////////////////////////////////////////////////////////
// carrier out of bounds between goal lines.
// possibilities: NEW_POSSESSION
static void _kick_car_oob( void )
{
	game_info.play_result = PR_NEW_POSSESSION;
}

// carrier downed inbounds
// possibilities: NEW_POSSESSION
static void _kick_car_ib( void )
{
	game_info.play_result = PR_NEW_POSSESSION;
}

// carrier downed/oob in own end zone
// possibilities: SAFETY, TOUCHBACK
static void _kick_car_oez( void )
{
	if(( game_info.game_flags & GF_REC_IN_EZ ) && 
		!( game_info.game_flags & GF_LEFT_EZ ))
	{
		game_info.play_result = PR_TOUCHBACK;
	}
	else
	{
		game_info.play_result = PR_SAFETY;
	}
}

// carrier reached enemy end zone
// possibilities: TOUCHDOWN
static void _kick_car_td( void )
{
	game_info.play_result = PR_TOUCHDOWN;
}

// ball declared dead inbounds
// possibilities: NEW_POSSESSION
static void _kick_deadball( void )
{
	game_info.play_result = PR_NEW_POSSESSION;
}

// ball went out of bounds
// possibilities: SAFETY, NEW_POSSESSION, KICKOFF_OOB, TOUCHBACK
static void _kick_oobball( void )
{
	switch( ball_obj.type )
	{
		case LB_KICKOFF:
			game_info.play_result = PR_KICKOFF_OOB;
			break;
		case LB_FUMBLE:
		case LB_LATERAL:
			if( fpend <= 0 )
			{
				// ball oob in own end zone
				if(( game_info.game_flags & GF_POSS_CHANGE ) &&
					( game_info.game_flags & GF_REC_IN_EZ ) && 
					!( game_info.game_flags & GF_LEFT_EZ ))
				{
					game_info.play_result = PR_TOUCHBACK;
				}
				else
				{
					game_info.play_result = PR_SAFETY;
				}
			}
			else if( fpend >= 100 )
			{
				// ball oob in enemy end zone
				game_info.off_side = !game_info.off_side;
				game_info.play_result = PR_TOUCHBACK;
			}
			else
			{
				float ffum;
				
				// ball oob between goal lines
				// spot ball at point of fumble, or point of oob, whichever
				// is farther from enemy end zone.
				game_info.play_result = PR_NEW_POSSESSION;
				ffum = FIELDX(game_info.fumble_x);
				
				if( ffum < fpend )
					game_info.play_end_pos = game_info.fumble_x;
			}
			break;
		default:
#ifdef DEBUG
			fprintf( stderr, "Weird: Ball went out of bounds on a kickoff"
					" and wasn't a KICKOFF or a FUMBLE.\n" );
			lockup();
#else
			while(1) ;
#endif
			break;
			
	}
}

//////////////////////////////////////////////////////////////////////////////
// carrier out of bounds between goal lines.
static void _fg_car_oob( void )
{}
// carrier downed inbounds
static void _fg_car_ib( void )
{}
// carrier downed/oob in own end zone
static void _fg_car_oez( void )
{}
// carrier reached enemy end zone
static void _fg_car_td( void )
{}
// ball declared dead inbounds
static void _fg_deadball( void )
{}
// ball went out of bounds
static void _fg_oobball( void )
{}

//////////////////////////////////////////////////////////////////////////////
// carrier out of bounds between goal lines.
static void _punt_car_oob( void )
{}
// carrier downed inbounds
static void _punt_car_ib( void )
{}
// carrier downed/oob in own end zone
static void _punt_car_oez( void )
{}
// carrier reached enemy end zone
static void _punt_car_td( void )
{}
// ball declared dead inbounds
static void _punt_deadball( void )
{}
// ball went out of bounds
static void _punt_oobball( void )
{}

//////////////////////////////////////////////////////////////////////////////
// carrier out of bounds between goal lines.
// possibilities: FAILED_CONVERSION
static void _pat_car_oob( void )
{
	game_info.play_result = PR_FAILED_CONVERSION;
}

// carrier downed inbounds
// possibilities: FAILED_CONVERSION
static void _pat_car_ib( void )
{
	game_info.play_result = PR_FAILED_CONVERSION;
}

// carrier downed/oob in own end zone
// possibilities: FAILED_CONVERSION, PAT_SAFETY
static void _pat_car_oez( void )
{
	if(( game_info.game_flags & GF_POSS_CHANGE ) &&
		( game_info.game_flags & GF_REC_IN_EZ ) && 
		!( game_info.game_flags & GF_LEFT_EZ ))
	{
		game_info.play_result = PR_FAILED_CONVERSION;
	}
	else
	{
		game_info.play_result = PR_PAT_SAFETY;
	}
}

// carrier reached enemy end zone
// possibilities: CONVERSION
static void _pat_car_td( void )
{
	game_info.play_result = PR_CONVERSION;
}

// ball declared dead inbounds
// possibilities: FAILED_CONVERSION
static void _pat_deadball( void )
{
	game_info.play_result = PR_FAILED_CONVERSION;
}

// ball went out of bounds
// possibilities: FAILED_CONVERSION, PAT_SAFETY
static void _pat_oobball( void )
{
	if(( fpend <= 0 ) &&
		(( ball_obj.type == LB_FUMBLE ) || ( ball_obj.type == LB_LATERAL)))
		game_info.play_result = PR_PAT_SAFETY;
	else
		game_info.play_result = PR_FAILED_CONVERSION;
}

//////////////////////////////////////////////////////////////////////////////
