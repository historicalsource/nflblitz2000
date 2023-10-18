/*
 *		$Archive: /video/nfl/include/GAMEADJ.H $
 *		$Revision: 15 $
 *		$Date: 9/16/99 6:06a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifndef	__GAMEADJ_H__
#define	__GAMEADJ_H__


#ifndef	__ADJUST_H__
#include	<goose/adjust.h>
#endif


#define ADD_HIDDEN_ADJUST(a, b, c) fprintf((a), "startadjust %d default %d adjustdone\r\n\r\n", (b), (c)[(b)-FIRST_GAME_ADJ].default_val);

typedef enum {
	CREDITS_FOR_FULL_GAME_ADJ = FIRST_GAME_ADJ,	/* Credits needed for a full game */
	DIFFICULTY_ADJ,
	MOVIE_SND_ADJ,
	ATTRACT_VOLUME_LEVEL_ADJ,
	TIME_ADJ,
	OPERATOR_MSG_ADJ,
	H4_FREE_GAME_ADJ,
	HH_FREE_GAME_ADJ,
	HC_FREE_GAME_ADJ,
	H4_FREE_QUARTER_ADJ,
	HH_FREE_QUARTER_ADJ,
	HC_FREE_QUARTER_ADJ,
	VIOLENCE_ADJ,
	SWEAR_ADJ,
	CLEAR_RECORDS_ADJ,
	TOURNAMENT_MODE_ADJ,
	DOOR_OPEN_ADJ,
} game_adjustment_number_t;

int check_all_adjustments(void);
void restore_all_adjustments(void);

#endif
