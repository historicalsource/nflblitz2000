//-------------------------------------------------------------------------------------------------
//	GAME_SEL.H - Game Select Functions for Blitz 2000 and NBA Showtime
//
//	James T. Anderson
//  Midway Games, Inc.
//  Copyright (c) 1999
//-------------------------------------------------------------------------------------------------
#ifndef __GAME_SEL_H_DEFINED__
#define __GAME_SEL_H_DEFINED__
//-------------------------------------------------------------------------------------------------

#define NFL_RUNNING		0
#define NBA_RUNNING		1

//-------------------------------------------------------------------------------------------------

typedef struct _reload_info {
	int p_status;
	int bought_in_first;
	int quarters_purchased[4];
	int credits;
	int service_credits;
	int coin_units;
	int bonus_units;
	int remainder_units;
} reload_info_t;

//-------------------------------------------------------------------------------------------------

int game_select(int cur_game);
void write_game_info(void);
void load_game_info(void);
void display_loading(int game);

//-------------------------------------------------------------------------------------------------
#endif