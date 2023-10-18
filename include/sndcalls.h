/***************************************************************************/
/*                                                                         */
/* SNDCALLS.H                                                              */
/*                                                                         */
/* Sound call defines													   */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/* by Jeff Johnson														   */
/*                                                                         */
/* 27 Jan 97                                                               */
/*                                                                         */
/*                                                                         */
/* $Revision: 112 $                                                          */
/*                                                                         */
/***************************************************************************/

// defines for SPEECH
#define	LVL0	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL0
#define	LVL1	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL1
#define	LVL2	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL2
#define	LVL3	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL3
#define	LVL4	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL4
#define	LVL5	SND_PRI_SET_NOINT|SND_PRI_TRACK5|PRIORITY_LVL5


// Types of passes...tackles...hits... etc based on sequence. (must also update sndcalls.equ)
//   used for ancouncer speech
//

// hit strength
#define HS_WEAK     0           // flails..etc
#define HS_HARD     1           // knockdowns...etc

// tackle type
#define TT_RUN      0
#define TT_TRB_RUN  1
#define TT_PUPPET   2
#define TT_DIVE     3
#define TT_SPRHEAD  4
#define TT_DRPKICK  5
#define TT_PUNCH    6
#define TT_KICK     7
#define TT_PUSH     8

// reaction type
#define RT_FLAILED      0
#define RT_PUSHED       1
#define RT_KICKED       2
#define RT_PUNCHED      3
#define RT_STIFFARMED   4
#define RT_PLOWED       5
#define RT_THROWN       6
#define RT_BREAKNECK    7

//
// PRIORITIES
//
#define	PRIORITY_LVL0		20  //0             // lowest...
#define	PRIORITY_LVL1		15  //1
#define	PRIORITY_LVL2		10  //5
#define	PRIORITY_LVL3		5   //10
#define	PRIORITY_LVL4		1   //15
#define	PRIORITY_LVL5		0   //20            // highest priority

//
// SOUND CALLS
//
#define	VS_CHANGE_ICON_SND	19
#define	VS_GOT_CODE_SND		60
#define	VS_WRONG_CODE_SND	134

#define	CAP_LCB_CUR_SND		86
#define	CAP_LCB_SEL_SND		30 //84//89
#define	CAP_CUR_PLAY_SND	83
#define	CAP_SEL_PLAY_SND	95//84
#define	CAP_LET_CUR_SND		85
#define	CAP_LET_SEL_SND		97//84
#define	CAP_FRM_CUR_SND		87
#define	CAP_FRM_SEL_SND		91//95//84
#define	CAP_QB_CUR_SND		87
#define	CAP_QB_SEL_SND		91//95//84

#define	CAP_JOY_REPEAT_SND	88
#define	CAP_DROP_NODE_SND	89
#define	CAP_NODE_MENU_SND	87//90
#define	CAP_PICK_MENU_SND	91

#define	CAP_YESNO_CUR_SND	17 //44
#define	CAP_YESNO_SEL_SND	96//44

#define	OPTION_CURSR_SND	43
#define	OPTION_SELECT_SND	27
#define	OPTION_YN_CUR_SND	17 //44 //97

#define	YESNO_CURSR_SND		17 //12
#define	CURSOR_SND1			87 //42 //40 //47
#define	SELECT_SND1			27 //22 //42 // 15
#define	FIRSTDOWN_SND		16          // First down achieved while running over it
#define	OVER_LOS_SND		136 //55          // Ran over LOS
#define	SWITCH_SND  		23          // Switch on play result box & switch pages on play select
#define	SWITCH2_SND  		19          // Flip plays on play select
#define	SLIDEIN_SND  		31  //71    // Slide in play select page
#define	TMSEL_CURSR_SND		87 //03 // 43 //36
#define	TM_SELECTED_SND		30 //16 //27 //30 //27
#define	SEL_YESNO_SND		28
//#define	SEL_YESNO_SND		27
#define	LOST_FIRE_SND		194
#define	FIRE_BURST_SND		192
#define	ALMOST_FIRE_SND		193
#define	SPIN_SND    		133
#define	DOINK_SND    		134
#define	TIMEDOWN_SND		56 //17
#define BOWLING_PIN_SND     141
#define RECORD_FOUND_SND    80          // Used when entering initials

#define	PLAQ_SLIDE_SND		31 //20
#define	PLAQ_ROTATE_SND		31 //20
#define	PLAQ_STOP_SND		29

#define	RND_SELCT_STRT_SND	26
#define	RND_SELCT_STP_SND	27

#define	FULLGM_APPEAR_SND	24
#define	FULLGM_FLASH_SND	25

#define	STAT_AWARD_SND		22
//#define	SWOOSH_SND      	70
#define	SWOOSH2_SND      	190 //70
#define	LOCK_SND          	191 //96


//
// MISC. PLAYER SOUNDS
//
#define	P_HIT1_SND			1      //2
#define	P_HIT2_SND			2
#define	P_HIT3_SND			3
#define	P_HIT4_SND			4
#define	P_HIT5_SND			5
#define	P_HIT6_SND			6
#define	P_HIT7_SND			7
#define	P_HIT8_SND			8
#define	P_HIT9_SND			12       // soft
#define	P_HIT10_SND			13      // soft
#define	P_HIT11_SND			11      // soft
#define	P_HIT12_SND			12      // soft
#define	P_HIT13_SND			13      // soft
#define	P_HIT14_SND			92
#define	P_HIT15_SND			94
#define	P_HIT16_SND			122     // soft
#define	P_HIT17_SND			123

#define P_HARD_HIT1_SND     95
#define P_HARD_HIT2_SND     96
#define P_HARD_HIT3_SND     94
#define P_BLITZ_YELL_SND    98

#define	P_ATTK1_SND			51
#define	P_ATTK2_SND			64
#define	P_ATTK3_SND			79
#define	P_ATTK4_SND			91
#define	P_ATTK5_SND			17
#define	P_ATTK6_SND			18
#define	P_ATTK7_SND			19

#define	P_PUSH1_SND	 		17
#define	P_PUSH2_SND			19

#define	P_NECK_CRCK1_SND	115
#define	P_NECK_CRCK2_SND	115
#define	P_NECK_CRCK3_SND	115
#define	P_NECK_CRCK4_SND	118

#define	P_GRND_HIT1_SND		108
#define	P_GRND_HIT2_SND		117
#define	P_GRND_HIT3_SND		117
#define	P_GRND_HIT_SOFT_SND	113

#define	P_WHAT_WAITING_4_SP	168
#define	P_THROW_TO_ME_SP	169
#define P_IM_CLEAR_SP       170
#define P_OVER_HERE_SP      171
#define	P_THROW_TO_ME2_SP	172
#define	P_CMON_THROW_IT_SP	173
#define	P_YO_OVER_HERE_SP	174
#define	P_ANYDAY_NOW_SP		175
#define	P_IM_WAITING_SP		176


#define	P_DIVESTRT_SND		47
#define	P_DIVEHIT_SND 		57
#define	P_CATCH_BALL_SND	127 //130
#define	P_KICK_BALL_SND	    116
#define	P_MISS_BALL_SND	    148

#define	P_THRW_BALL1_SND	    	158
#define	P_THRW_BALL2_SND	    	158
#define	P_THRW_BALL3_SND	    	158
#define	P_THRW_BALL4_SND	    	158
#define	P_THRW_BALL_FIRE_SND    	131

#define	P_LATERAL_BALL_SND			159


#define	BALL_HIT_GRND_SND   108


//
// MISC. PLAYER SPEECH
//	 in 'players.bnk'
//
#define	P_LAND_GRUNT1_SP	14
#define	P_LAND_GRUNT2_SP	58
#define	P_LAND_GRUNT3_SP	21
#define	P_LAND_GRUNT4_SP	53
#define	P_LAND_GRUNT5_SP	5
#define	P_LAND_GRUNT6_SP	63
#define	P_LAND_GRUNT7_SP	56
#define	P_LAND_GRUNT8_SP	57
#define	P_LAND_GRUNT9_SP	62
#define	P_LAND_GRUNT10_SP	60
#define	P_LAND_GRUNT11_SP	59

#define	P_DNT_GET_UP_SP		88
#define	P_RIP_LIMP_SP		39
#define	P_BEAT_LK_DOG_SP	86
#define	P_WOO_WOO_SP		80
#define	P_YEAH_BABY_SP		81
#define	P_BUST_U_UP_SP		87
#define	P_KNK_TMORW_SP		84
#define	P_THRW_PLR_SP  		27
#define BACK_OFF_MAN_SP     49
#define OUTTA_MY_FACE_SP    50

#define	P_GET_THRWN1_SP	   	28
#define	P_GET_THRWN2_SP		29
#define	P_GET_THRWN3_SP		69
#define	P_GET_THRWN4_SP		68
#define	P_GET_THRWN5_SP		61

#define	P_42_SP 			30
#define	P_44_SP 			31
#define	P_BLUE_SP 			32
#define	P_HIKE_SP			33
#define	P_RED_SP 			36
#define	P_SET1_SP    		37
#define	P_SET2_SP    		37
#define	P_SET3_SP    		37

#define	P_HUT1_SP    		34
#define	P_HUT2_SP    		35
#define	P_HUT3_SP    		38
#define	P_LONG_HUT_SP 		35

//
// in 'generic.bnk'
//
#define CRWD_CHEER1_SND     11
#define CRWD_CHEER2_SND     12
#define CRWD_CHANT1_SND     20
#define CRWD_CHANT2_SND     21
#define CRWD_BOO1_SND       13
#define EOQ_MUSIC_SND       15
#define EOQ_MUSIC2_SND      17
#define VS_MUSIC_SND        18

//
// ANNOUNCER SPEECH
//
//
// 'tkperm.bnk'
//
#define	HES_LIGHTING_UP_SP		43
#define	HES_ON_FIRE_SP			45

#define TRIVIA_SP               6
#define MATCHUP_SP              10
#define VERSUS_SP               11
#define TAKEON_SP               12

#define FIRST_DWN_SP            25		// in "generic.bnk"
#define FIRST_AND_30_SP         29		// in "generic.bnk"

#define FIRST_AND_GOAL_SP       33

#define SECOND_DWN_SP           26		// in "generic.bnk"
//#define SECOND_DWN_LNG_SP       24
//#define SECOND_DWN_SHT_SP       25
#define SECOND_AND_GOAL_SP      34

#define THIRD_DWN_SP            27		// in "generic.bnk"
//#define THIRD_DWN_LNG_SP        27
//#define THIRD_DWN_SHT_SP        28
#define THIRD_AND_GOAL_SP       35

#define FOURTH_DWN_SP           28		// in "generic.bnk"
//#define FOURTH_DWN_LONG_SP      31
#define FOURTH_AND_GOAL_SP      36

#define ONE_MINUTE_2_GO_SP      68

#define DEEP_IN_TERRITORY_SP    38
#define LOOKS_LIKE_BLITZ_SP     18
#define LOOKS_LIKE_RUN_SP       19
#define TRIPS_R_SP              20
#define FULL_HOUSE_SP           20

#define GETS_THE_FIRST_DOWN_SP  178
#define HES_AT_THE_30_SP        160
#define THE_20_SP               161
#define THE_10_SP               162
#define THE_5_TD_SP             163

#define MAN_IN_MOTION_SP        21
#define SPINS_SP                7
#define JUKES_DEFENDER_SP       8
#define GAME_UNDER_WAY_SP       215
#define ONSIDE_KICK_SP          180

#define GOING_FOR_IT_SP         219

#define WERE_AT_HALFTIME_SP     39
#define WE_HAVE_OVERTIME_SP     40
#define END_OF_1ST_SP           175
#define END_OF_3RD_SP           176
#define WIN_THE_GAME_SP         142
#define WINS_THE_GAME_SP        143

#define GETS_1ST_DOWN_SP        177
#define ONSIDE_KICK_SP          180

#define TOUCHDOWN_SP            200
#define KICK_IS_UP_SP           113
#define AND_IT_GOOD_SP          115
#define NO_GOOD_SP              116

#define NO_GOOD_SND             161

#define LINE_UP_FOR_FG_SP       111
#define TIE_GAME_SP             135

//
// In 'tkbankX'...
//
#define SHOE_STRING_TACKLE_SP   15
#define PUT_IT_ON_BOARD_SP      100
#define AND_ITS_GOOD_SP         101
#define TACKS_ON_SP             102
#define GOING_FOR_2_SP          103


//
// In city banks... ie, bears.bnk
//
#define CITY_NAME_SP            0
#define TEAM_NAME_SP            1
#define FULL_TEAM_NAME1_SP      2
#define FULL_TEAM_NAME2_SP      3

#define QB_NAME_SP              10
#define WPN1_NAME_SP            11
#define WPN2_NAME_SP            12
#define WPN3_NAME_SP            13
#define TO_WPN1_NAME_SP         14
#define TO_WPN2_NAME_SP         15
#define TO_WPN3_NAME_SP         16
#define PUMPED_QB_NAME_SP       17
#define PUMPED_WPN1_NAME_SP     18
#define PUMPED_WPN2_NAME_SP     19
#define PUMPED_WPN3_NAME_SP     20

//
// Play select sounds (from "cursors.bnk")
//
#define P1_CURSR_SND     63 //17 //129
#define P2_CURSR_SND     64 //17 //130
#define HIDE_CURSOR_SND  60 //61

// fanfares...and misc rule sounds (ie...fumble)
#define TOUCHDOWN_SND       142
#define SAFETY_SND          142
#define TWO_PT_SND          156
#define SACK_SND            157
#define ITS_GOOD_SND        162
#define INTERCEPT_SND       143
#define HELMET_SND          61 //39     // 4th down snd on plays select screen as helmet flashes
#define CLOCK_LOW_SND       36      // Sound to play every couple of seconds at end of half/game
#define GUN_SND             147     // End of quarter gun
#define RICOCHET_SND        148     // Ricochet or idiot box go away
#define WHISTLE1_SND        139     // Whistle 2
#define WHISTLE2_SND        140     // Whistle 3
#define WARNING_SND         39      // warning


//
// COIN-IN SOUNDS and INFO
//
#define COIN_SND_DROP_BANK 		"cursors"		/* which bank the coin drop sound is in */
#define COIN_SND_DROP_CALL 		13				/* which sound call number in the bank */
#define COIN_SND_PAID_BANK 		"cursors" 	/* special sound when player gets paidup */
#define COIN_SND_PAID_CALL 		13
#define COIN_SOUND_INTERVAL 	15				/* min delay between coindrop sounds */
#define	EAT_CREDIT_SND			59   
#define	TICK_SND			    17              /* Tick sound for attract toggling */

//
// VOLUMES
//
#define COIN_SND_DROP_VOLUME 	195			 	/* track volume */
#define COIN_SND_PAID_VOLUME 	195
#define VOLUME0                 170-20          // Music    //150
#define VOLUME1                 135-20          // General stuff/speech
#define VOLUME2                 180-20          // Speech & effects we really want to hear
#define VOLUME3                 225-20          // ??? Big tackles perhaps
#define VOLUME4                 255-20          // Maximum
#define VOLUME5                 255             // Maximum

#define	ANCR_VOL1				225
#define	ANCR_VOL2				255

#define	MUSIC_VOL				255
#define	FANFARE_VOL				250


typedef struct sndinfo {
	char *bank_name;
	unsigned int scall;
    unsigned int volume;
    unsigned int pan;
    unsigned int priority;
	int max_times;
	int loops;
} sndinfo;
