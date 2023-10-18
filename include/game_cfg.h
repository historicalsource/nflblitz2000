//-------------------------------------------------------------------------------------------------
//	GAME_CFG.H - Game Configuration Stuff
//
//	James T. Anderson
//  Midway Games, Inc.
//  Copyright (c) 1999
//-------------------------------------------------------------------------------------------------
#ifndef __GAME_CFG_H_DEFINED__
#define __GAME_CFG_H_DEFINED__
//-------------------------------------------------------------------------------------------------

//---[ Definitions ]-------------------------------------------------------------------------------

#define GC_CPU			1
#define GC_PLAYBOOK		2
#define GC_FIELD		3
#define GC_FOG			4
#define	GC_SKILL		5
#define GC_AUDIBLE		6
#define GC_CAMERA		7

//---[ Functions ]---------------------------------------------------------------------------------

void game_config(void);
int game_config_confirm(void);
void clear_user_config(void);
int user_config(void);

void apply_user_config(int key);


//-------------------------------------------------------------------------------------------------
#endif