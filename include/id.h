//-----------------------------------------------------------------------------
//
//  General Blitz ID's (procss, texture, string, etc.)
//
//-----------------------------------------------------------------------------
//  PID - Process ID's 
//-----------------------------------------------------------------------------
// $Revision: 85 $
//-----------------------------------------------------------------------------
#define NODESTRUCT				0x8000
#define GAME_GRP_PID			0x0000				// General game group process ids
#define GAME_PROC_PID			0x0010				// game_proc
#define AMODE_GRP_PID			0x0100				// Attract mode process group ids
#define CC_GRP_PID				0x0200				// Coin counter process group ids
#define VOLUME_GRP_PID			0x0300				// Volume adjust process id
#define COIN_IN_GRP_PID			0x0400				// Coin drop sound process
#define PLAYER_PID				0x0500
#define PRE_PLAYER_PID			(PLAYER_PID+1)
#define POST_PLAYER_PID			(PLAYER_PID+2)
#define QBYELL_PID				0x0580				// QB yells signals/audibles process
#define SNAP_MON_PID			0x0590				// watches for A+B after snap
#define THROW_MON_PID			0x0591				// watches for A+B after throw
#define FIELD_PID				0x0600
#define STATUS_BOX_PID			0x0700
#define SCR_PLATE_PID			0x0800
#define BALL_PID				0x0900
#define LOOSE_HELMET_PID		0x0950
#define COLLIS_PID				0x0A00
#define REPLAY_PID				0x0B00
#define PRE_GAME_SCRNS_PID		0x0C00
#define SMOKE_PID				0x0D00
#define AUDIBLE_PID				0x0E00
#define GAME_CLOCK_PID			0x0F00
#define MESSAGE_PID				0x0F10				// Display big messages process

#define MSG_FUMBLE_PID			(MESSAGE_PID+1)
#define MSG_RESULT_PID			(MESSAGE_PID+2)
#define MSG_LOCLOCK_PID			(MESSAGE_PID+3)
#define MSG_4THDN_PID			(MESSAGE_PID+4)
#define MSG_CAMBOT_PID			(MESSAGE_PID+5)

#define PLYRINFO_PID			0x0F80
#define PLYR_CURSOR_PID			0x0F81
#define SPFX_PID				0x0F98				// on-fire, dust, blood..etc
#define QB_SPEECH_PID			0x0F99				// qb speech
#define IDIOT_BOX_PID			0x0F9a				// Idiot boxes
#define IDIOT_BOX2_PID			0x0F9b				// Idiot boxes
#define EOQ_PID					0x0F9c				// EOQ message proc
#define DELAY_PID				0x0F9d				// Delay music 
#define BONUS_PID				0x0F9e				// Bonus message on yes/no scrn
#define LOAD_TEXTURES_PID		0x0F9f				// load misc. textures
#define MESSAGE2_PID			0x0Fa0				// Flip message on play select
#define MESSAGE3_PID			0x0Fb0				// Free message on play select
#define MESSAGE4_PID			0x0Fc0				// Flash proc handling 30 second mode
#define MESSAGE5_PID			0x0Fd0				// ID for sack text process
#define MESSAGE6_PID			0x0Fd1				// ID for pflasher3
#define CREATE_PLAY_PID			0x1000				// create-a-play base
#define STREAM_PID				0x1100				// streaming process
#define FADE_PID				0x1200				// stream fader process
#define PFLASH_PID				0x1300				// player flash white
#define CAMBOT_PID				0x1400				// cambot process
#define AUDIBLE_MSG_PID			0x1500

#define CATCH_SPEECH_PID      0x0CCC

#define	HELMET1_PID				0x0dee
#define	HELMET2_PID				0x0def

#define SIMPLE_CYCLE_PROC_ID  0x5555				// Hiscore list color cycler
#define CREDIT_PAGE_ID        0x5600

#define TRANSIT_PID          (0x55aa+NODESTRUCT)	// Transition proc
#define BACKDOOR_PID         (0x7000+NODESTRUCT)	// Backdoor proc

#define DEBUG_SW_PID		(0x6000+NODESTRUCT)		// Debug switches proc

//-----------------------------------------------------------------------------
//  TID - Texture ID's
//-----------------------------------------------------------------------------
#define TXID_PLAYER			1   // moved from \\video\include\goose\texture.h
#define TXID_FIELD			2
#define TXID_STADIUM		3
#define TXID_PLRSHDW		4
#define TXID_PLRCURSOR		5

#define P1_YN_PLAQ_TID		20000   // leave 4 (1 for each player)
#define P2_YN_PLAQ_TID      20005   // leave 4 (1 for each player)
#define P3_YN_PLAQ_TID      20010   // leave 4 (1 for each player)
#define P4_YN_PLAQ_TID      20015   // leave 4 (1 for each player)

#define YES_NO_PLAQ_TID		20020
#define PLAQ_CURSORS_TID    20021
#define JAPPLE_BOX_TID      20022
#define TM_SEL_LOGOS_TID    20023
#define TM_SELECT_TID	 	20024
#define	STATS_PG_TID	    20025
#define	PLAY_SELECT_TID	    20026
#define	NFL_LOGO_TID		20027
#define	ROTATE_LOGO_TID		20028
#define	NUMBER_3D_TID		20029
#define HISCORE_PG_TID		20030
#define	BARS_PG_TID         20031
#define	OPT_SCR_TID         20032
//#define	INGAME_TID          20032
#define	MUGSHOT2_TID        20033
#define	BKGRND_TID        	20034
#define	BKGRND2_TID        	20035

#define PLYRINFO_TID		21001				// Game-time player info tid
#define SCR_PLATE_TID		21002				// Game-time score plate tid
#define MESSAGE_TID	    	21003				// Game-time big messages tid

#define CAP_TID				0x6000				// create-a-play ID base
#define CAP_TOP_TID			0x63FF				// everything between these two reserved for CAP

#define SPFX_TID            10                  // id for the SPECIAL EFFECTS...smoke..etc
#define HELMET_TID          21004


//-----------------------------------------------------------------------------
//  SID - String ID's
//-----------------------------------------------------------------------------
#define FUMBLE_SID			0x0034
#define	PLAY_SEL_MSG_ID		0x0100
#define	PLAY_SEL_MSG2_ID	0x0200
#define	PLAY_SEL_MSG3_ID	0x0300
#define	PLAY_SEL_SCORE_MSG_ID   0x0400          // For score on play select screen
#define CAP_ID				0x500				// for create-a-play
#define	PLAY_SELECT_ID		51
#define	PLAY_NAMES_ID		75

#define	CREDIT_MSGS_ID		60					// & next 3 for "Credit" msgs

#define	STAT_PG_TXT_ID		740

#define	SID_FGPER			800					// show fg percentage text
#define	SID_FG_METER		801
#define	SID_PUNT_METER		802

#define	SID_AUDIBLE_T1		803
#define	SID_AUDIBLE_T2		804

#define SID_TLCLOCK			0x0006
#define SID_TLSCORES		0x0005
#define SID_TLNAMES			0x0004


//-----------------------------------------------------------------------------
//  MID - Message ID's
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  OID - Object ID's
//-----------------------------------------------------------------------------

// 2d & 3D object ID's
#define OID_JAPPLE_BOX		3
#define OID_TMSEL_LOGO		0x200001
#define OID_CHLNGR_BOX		0x2ca000		// leave 4 for id per plyr #

#define OID_VERSUS			0x1500			// reserve entire 0x100 ids
#define	OID_STAT_PG		    55				// leave 4 after this for (plyr + 1)
#define	OID_STAT2_PG	    60				// leave 4 after this for (plyr + 1)
#define	OID_STAT3_PG	    65				// leave 4 after this for (plyr + 1)

#define OID_FIELD			0x100
#define OID_CURSOR			OID_FIELD|0x1
#define OID_PLAYER			0x200
#define OID_PRESHADOW		0x280
#define OID_SHADOW			0x290
#define OID_STADIUM			0x300
#define OID_LOOSE_HELMET	0x450
#define OID_BALL			0x400
#define OID_MISC			0x500
#define OID_SMKPUFF			0x510
#define OID_AMODE			0x600
#define OID_PLYRINFO		0x700
#define OID_SCRPLATE		OID_PLYRINFO|1
#define OID_INFOBOX 		OID_PLYRINFO|2
#define OID_RESULTBOX 		OID_PLYRINFO|4
#define OID_TURBARROW  		OID_PLYRINFO|8
#define OID_MESSAGE		    0x800               // Big messages during gameplay                
#define OID_MESSAGE2	    0x801               // Clock low
//#define OID_MESSAGE3	    0x810               // Fumble background
#define OID_FUMBLE_MSG		0x802
#define OID_IDIOTBOX		0x810
#define OID_P1CURSOR		0x900
#define OID_P2CURSOR		0x901

#define OID_P1HELMET		0x208
#define OID_P2HELMET		0x209

#define	OID_NBR3D	    	0xA00
#define OID_REPLAY	    	0xB00
#define OID_CAP				0xC00

#define	AMODE_GRP_OID	(0<<8)

#define	AMODE_CREDPROC_OID			(AMODE_GRP_OID|1)
#define	AMODE_CREDPROC_CREDITS_STRING_OID	(AMODE_GRP_OID|2)

#define	OID_FGPER			810					// show fg percentage text
#define	OID_FG_METER		811
#define	OID_PUNT_METER		812
#define OID_AUDIBLE_T1		813
#define OID_AUDIBLE_T2		814

#define OID_GC_BG		0xb000					// grand champ ID's
#define OID_GC_FG		0xf000

//-----------------------------------------------------------------------------
//  DRAWORDER - 3D object draw sequence
//-----------------------------------------------------------------------------
#define	DRAWORDER_FOGON		0x00008000
#define	DRAWORDER_FIELD		0x00010000
#define	DRAWORDER_FIELDFX		0x00020000
#define	DRAWORDER_PRESHADOW	0x00020100
#define	DRAWORDER_SHADOW		0x00020200
#define	DRAWORDER_STADIUM		0x00030000
#define	DRAWORDER_PLAYER		0x00040000
#define	DRAWORDER_FIELD2		0x00048000
#define	DRAWORDER_BALL			0x00050000
#define	DRAWORDER_DEBRIS		0x00054000
#define	DRAWORDER_FOGOFF		0x00058000
#define	DRAWORDER_PLAYSEL		0x00060000

//-----------------------------------------------------------------------------
//  RL - Process run levels
//-----------------------------------------------------------------------------
#define RL_CAMBOT		0x100
#define RL_AUDIBLE		0x200
#define RL_PLAYER		0x300
#define RL_CURSORS		0x400
#define RL_COLLIS		0x500
#define RL_BALL			0x600
#define RL_REPLAY		0x900

//-----------------------------------------------------------------------------
