#include "/Video/Include/String.h"				// for 'memcpy'
#include "/Video/Nfl/Include/Game.h"
#include "/Video/Nfl/Include/audits.h"
#include "/Video/Nfl/Include/plyrseq.h"
#include "/Video/Nfl/Include/PlayBook.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/cap.h"
#include "/Video/Nfl/Include/PlaySel.h"
#include "/Video/Nfl/Include/PlayStuf.h"
#include "/Video/Nfl/Include/Pproto.h"
#include "/Video/Nfl/Models/plyslct.h"

#include "/Video/Nfl/Include/Plays.h"


PLAYSTUF__SLOT off_slots[] =
{
	#define OFF_SLOT_ID_ZIG_ZAG			0
	{ "ZIG ZAG",		&play_zigzag,			ZIG_ZAG_AUD,		{ &limb_dualte_zigzag,		&limb_dualte_zigzag_f } },

	#define OFF_SLOT_ID_SPIDER_LEGS		1
	{ "SPIDER LEGS",	&play_spider,			SPIDER_LEGS_AUD,	{ &limb_dualte_spider,		&limb_dualte_spider_f } },

	#define OFF_SLOT_ID_SLIP_SLIDE		2
	{ "SLIP SLIDE",		&play_side_slide,		SLIP_SLIDE_AUD,		{ &limb_dualte_sideslide,	&limb_dualte_sideslide_f } },

	#define OFF_SLOT_ID_QB_POST			3
	{ "QB POST",		&play_QBdeep,			QB_POST_AUD,		{ &limb_dualte_qbpost,		&limb_dualte_qbpost_f } },

	#define OFF_SLOT_ID_QUICK_DISH		4
	{ "QUICK DISH",		&play_hooknladder,		QUICK_DISH_AUD,		{ &limb_dualte_quickdish,	&limb_dualte_quickdish_f } },

	#define OFF_SLOT_ID_TEAMPLAY_1		5
	{ "TEAM PLAY 1",	NULL,					0,					{ &limb_blank,				&limb_blank } },		// limbs are for create a play

	#define OFF_SLOT_ID_TEAMPLAY_2		6
	{ "TEAM PLAY 2",	NULL,					0,					{ &limb_blank,				&limb_blank } },		// limbs are for create a play

	#define OFF_SLOT_ID_TEAMPLAY_3		7
	{ "TEAM PLAY 3",	NULL,					0,					{ &limb_blank,				&limb_blank } },		// limbs are for create a play

	#define OFF_SLOT_ID_MONKEY			8
	{ "MONKEY",			&play_monkey,			MONKEY_AUD,			{ &limb_dualte_monkey,		&limb_dualte_monkey_f } },

	#define OFF_SLOT_ID_UPPER_CUT		9
	{ "UPPER CUT",		&play_upper_cut,		UPPER_CUT_AUD,		{ &limb_twoone_uppercut,	&limb_twoone_uppercut_f } },

	#define OFF_SLOT_ID_BLIZZARD		10
	{ "BLIZZARD",		&play_blizzard,			BLIZZARD_AUD,		{ &limb_twoone_blizzard,	&limb_twoone_blizzard_f } },

	#define OFF_SLOT_ID_HAIL_MARY2		11
	{ "HAIL MARY 2",	&play_hail_mary,		HAIL_MARY2_AUD,		{ &limb_twoone_hailmary,	&limb_twoone_hailmary_f } },

	#define OFF_SLOT_ID_TURMOIL			12
	{ "TURMOIL",		&play_quake_shake,		TURMOIL_AUD,		{ &limb_fourmen_quakeshake,	&limb_fourmen_quakeshake_f } },

	#define OFF_SLOT_ID_UP_THE_GUT		13
	{ "UP THE GUT",		&play_upthe_gut,		UP_THE_GUT_AUD,		{ &limb_fourmen_upthegut,	&limb_fourmen_upthegut_f } },

	#define OFF_SLOT_ID_CRISS_CROSS		14
	{ "CRISS CROSS",	&play_criss_cross,		CRISS_CROSS_AUD,	{ &limb_fourmen_crisscross,	&limb_fourmen_crisscross_f } },

	#define OFF_SLOT_ID_DAWG_HOOK		15
	{ "DAWG HOOK",		&play_dawg_hook,		DAWG_HOOK_AUD,		{ &limb_fourmen_dawghook,	&limb_fourmen_dawghook_f } },

	#define OFF_SLOT_ID_BACK_SPLIT		16
	{ "BACK SPLIT",		&play_blitz_beater,		BACK_SPLIT_AUD,		{ &limb_onewide_blitzbeater,&limb_onewide_blitzbeater_f } },

	#define OFF_SLOT_ID_SCREEN_RIGHT	17
	{ "SCREEN RIGHT",	&play_screen_rgt,		SCREEN_AUD,			{ &limb_onewide_screenrt,	&limb_onewide_screenrt_f } },

	#define OFF_SLOT_ID_HB_BLOCK		18
	{ "HB BLOCK",		&play_one_man_back,		HB_BLOCK_AUD,		{ &limb_one11_onemanback,	&limb_one11_onemanback_f } },

	#define OFF_SLOT_ID_CROSS_SLANT		19
	{ "CROSS SLANT",	&play_cross_slant,		SLANT_AUD,			{ &limb_one11_crossslant,	&limb_one11_crossslant_f } },

	#define OFF_SLOT_ID_REVERSE_ZIP		20
	{ "REVERSE ZIP",	&play_reverse_zip,		REVERSE_ZIP_AUD,	{ &limb_one11_reversezip,	&limb_one11_reversezip_f } },

	#define OFF_SLOT_ID_SWEEP_RIGHT		21
	{ "SWEEP RIGHT",	&play_sweep_rgt,		SWEEP_AUD,			{ &limb_one11_sweeprt,		&limb_one11_sweeprt_f } },

	#define OFF_SLOT_ID_MIDDLE_PICK		22
	{ "MIDDLE PICK",	&play_middle_pick,		MIDDLE_PICK_AUD,	{ &limb_twoone_middlepick,	&limb_twoone_middlepick_f } },

	#define OFF_SLOT_ID_DA_BOMB			23
	{ "DA BOMB",		&play_da_bomb,			DA_BOMB_AUD,		{ &limb_trips_dabomb,		&limb_trips_dabomb_f } },

	#define OFF_SLOT_ID_UTB_DEEP		24
	{ "U.T.B. DEEP",	&play_utb_deep,			UTB_DEEP_AUD,		{ &limb_trips_utbdeep,		&limb_trips_utbdeep_f } },

	#define OFF_SLOT_ID_SUBZERO			25
	{ "SUBZERO",		&play_subzero,			SUB_ZERO_AUD,		{ &limb_trips_subzero,		&limb_trips_subzero_f } },

	#define OFF_SLOT_ID_SUPER_FLY		26
	{ "SUPER FLY",		&play_razzle_dazzle,	SUPER_FLY_AUD,		{ &limb_trips_razzledazzle,	&limb_trips_razzledazzle_f } },

	#define OFF_SLOT_ID_SWITCH_UP		27	
	{ "SWITCH UP",		&play_switch,			0,					{ &limb_switchup,			&limb_switchup_f } },

	#define OFF_SLOT_ID_CRUISN			28		
	{ "CRUIS'N",		&play_cruisn,			0,					{ &limb_fourmen_cruisin,	&limb_fourmen_cruisin_f } },

	#define OFF_SLOT_ID_HURRICANE		29	
	{ "HURRICANE",		&play_hurricane,		0,					{ &limb_trips_hurricane,	&limb_trips_hurricane_f } },

	#define OFF_SLOT_ID_DEEP_ATTACK		30
	{ "DEEP ATTACK",	&play_gen_1,			0,					{ &ps_gen_1,				NULL } },

	#define OFF_SLOT_ID_FLOOD_SLIT		31
	{ "FLOOD SLIT",		&play_gen_2,			0,					{ &ps_gen_2,				NULL } },

	#define OFF_SLOT_ID_POST_DRAG		32
	{ "POST DRAG",		&play_gen_3,			0,					{ &ps_gen_3,				NULL } },

	#define OFF_SLOT_ID_TASTY_TREAT		33
	{ "TASTY TREAT",	&play_gen_4,			0,					{ &ps_gen_4,				NULL } },

	#define OFF_SLOT_ID_KOMBAT			34
	{ "KOMBAT",			&play_gen_5,			0,					{ &ps_gen_5,				NULL } },

	#define OFF_SLOT_ID_BLACK_RAIN		35
	{ "BLACK RAIN",		&play_gen_6,			0,					{ &ps_gen_6,				NULL } },

	#define OFF_SLOT_ID_PUNT			36
	{ "PUNT",			&play_punt,				PUNT_AUD,			{ &limb_kicks_punt,			&limb_kicks_punt_f } },

	#define OFF_SLOT_ID_FAKE_PUNT		37
	{ "FAKE PUNT",		&play_fake_punt,		FAKE_PUNT_AUD,		{ &limb_kicks_fakepunt,		&limb_kicks_fakepunt_f } },

	#define OFF_SLOT_ID_FIELD_GOAL		38
	{ "FIELD GOAL",		&play_field_goal,		FIELD_GOAL_AUD,		{ &limb_kicks_fieldgoal,	&limb_kicks_fieldgoal_f } },

	#define OFF_SLOT_ID_FAKE_FG			39
	{ "FAKE F.G.",		&play_fake_field_goal,	FAKE_FG_AUD,		{ &limb_kicks_fakefieldgoal,&limb_kicks_fakefieldgoal_f } },

	#define OFF_SLOT_ID_EXTRA_POINT		40
	{ "EXTRA POINT",	&play_extra_point,		EXTRA_POINT_AUD,	{ &limb_blank,				&limb_blank } },

	{ NULL, NULL, NULL, { NULL, NULL } },
};

PLAYSTUF__SLOT def_slots[] =
{
	#define DEF_SLOT_ID_SAFE_COVER		0
	{ "SAFE COVER",		&play_safe_cover,		SAFE_COVER_AUD,		{ &limb_defc_safecover,		&limb_blank } },

	#define DEF_SLOT_ID_ONE_MAN_BLITZ	1
	{ "1 MAN BLITZ",	&play_3_man_cover,		ONE_MAN_BLITZ_AUD,	{ &limb_defa_3mancover,		&limb_blank } },

	#define DEF_SLOT_ID_STUFF_IT		2
	{ "STUFF IT",		&play_stuff_it,			0,					{ &defplay1,				NULL } },

	#define DEF_SLOT_ID_TWO_MAN_BLITZ	3
	{ "2 MAN BLITZ",	&play_2_man_blitz,		TWO_MAN_BLITZ_AUD,	{ &limb_defc_2manblitz,		&limb_blank } },

	#define DEF_SLOT_ID_NEAR_ZONE		4
	{ "NEAR ZONE",		&play_near_zone,		NEAR_ZONE_AUD,		{ &limb_defd_nearzone,		&limb_blank } },

	#define DEF_SLOT_ID_SUICIDE_BLITZ	5
	{ "SUICIDE BLITZ",	&play_suicide_blitz,	SUICIDE_BLITZ_AUD,	{ &limb_defa_suicideblitz,	&limb_blank } },

	#define DEF_SLOT_ID_ZONE_BLITZ		6
	{ "SIDE BLITZ",		&play_3_man_blitz,		ZONE_BLITZ_AUD,		{ &limb_defc_3manblitz,		&limb_blank } },

	#define DEF_SLOT_ID_FAKE_ZONE		7
	{ "FAKE ZONE",		&play_fake_zone,		0,					{ &defplay2,				NULL } },
																								
	#define DEF_SLOT_ID_KNOCK_DOWN		8
	{ "KNOCK DOWN",		&play_knock_down,		0,					{ &defplay3,				NULL } },



	#define DEF_SLOT_ID_GOAL_LINE		9
	{ "GOAL LINE",		&play_goal_line,		GOAL_LINE_AUD,		{ &limb_defb_goalline,		&limb_blank } },

	#define DEF_SLOT_ID_MEDIUM_ZONE		10
	{ "MEDIUM ZONE",	&play_medium_zone,		MEDIUM_ZONE_AUD,	{ &limb_defa_mediumzone,	&limb_blank } },

	#define DEF_SLOT_ID_DEEP_ZONE		11
	{ "DEEP ZONE",		&play_deep_zone,		DEEP_ZONE_AUD,		{ &limb_defe_deepzone,		&limb_blank } },

	#define DEF_SLOT_ID_LEFT_ZONE		12
	{ "LEFT ZONE",		&play_left_zone,		0,					{ &defplay4,				NULL } },

	#define DEF_SLOT_ID_STOP_RUN		13
	{ "STOP RUN",		&play_stop_run,			0,					{ &defplay5,				NULL } },

	#define DEF_SLOT_ID_RIGHT_ZONE		14
	{ "RIGHT ZONE",		&play_right_zone,		0,					{ &defplay6,				NULL } },

	#define DEF_SLOT_ID_MAN_ON_MAN		15
	{ "MAN ON MAN",		&play_man_on_man,		SAFE_COVER_AUD,		{ &limb_defc_safecover,		&limb_blank } },

	#define DEF_SLOT_ID_MIX_IT_UP		16
	{ "MIX IT UP",		&play_mix_it_up,		0,					{ &defplay7,				NULL } },

	#define DEF_SLOT_ID_SPREAD			17
	{ "SHIFT RIGHT",	&play_spread,			0,					{ &defplay8,				NULL } },

	#define DEF_SLOT_ID_BLOCK_PUNT		18
	{ "BLOCK PUNT",		&play_block_punt,		BLOCK_PUNT_AUD,		{ &limb_def_blockpunt,		&limb_blank } },

	#define DEF_SLOT_ID_BLOCK_FG		19
	{ "BLOCK F.G.",		&play_block_fg,			BLOCK_FG_AUD,		{ &limb_def_blockfg,		&limb_blank } },

	#define DEF_SLOT_ID_PUNT_RETURN		20
	{ "PUNT RETURN",	&play_punt_return,		PUNT_RETURN_AUD,	{ &limb_def_puntreturn,		&limb_blank } },

	#define DEF_SLOT_ID_STUFF_IT2		21
	{ "STUFF IT",		&play_stuff_it2,		0,					{ &defplay1,				NULL } },

	{ NULL, NULL, NULL, { NULL, NULL } },
};

PLAYSTUF__SLOT team_slots[][3] =
{
	//	TEAM_CARDINALS 
	{
		{ "SHORT JET",		&play_az_1,			TEAMPLAY_1_AUD,		{ &ps_ari_1,				NULL } },
		{ "STACK IT",		&play_az_2,			TEAMPLAY_2_AUD,		{ &ps_ari_3,				NULL } },
		{ "SMACK YOU",		&play_az_3,			TEAMPLAY_3_AUD,		{ &ps_ari_2,				NULL } },
	},
	//	TEAM_FALCONS
	{
		{ "FLY AWAY",		&play_at_1,			TEAMPLAY_1_AUD,		{ &ps_atl_1,				NULL } },
		{ "SMILEY FACE",	&play_at_2,			TEAMPLAY_2_AUD,		{ &ps_atl_2,				NULL } },
		{ "DOG BONE",		&play_at_3,			TEAMPLAY_3_AUD,		{ &ps_atl_3,				NULL } },
	},
	//	TEAM_RAVENS
	{
		{ "DEEP THRO IT",	&play_ba_1,			TEAMPLAY_1_AUD,		{ &ps_bal_1,				NULL } },
		{ "STRAIGHT IN",	&play_ba_2,			TEAMPLAY_2_AUD,		{ &ps_bal_2,				NULL } },
		{ "BACK SIDE",		&play_ba_3,			TEAMPLAY_3_AUD,		{ &ps_bal_3,				NULL } },
	},
	//	TEAM_BILLS
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_PANTHERS
	{
		{ "DEEP SPANK",		&play_ca_1,			TEAMPLAY_1_AUD,		{ &ps_car_1,				NULL } },
		{ "2 GO DEEP",		&play_ca_2,			TEAMPLAY_2_AUD,		{ &ps_car_2,				NULL } },
		{ "UNDERTAKER",		&play_ca_3,			TEAMPLAY_3_AUD,		{ &ps_car_3,				NULL } },
	},
	//	TEAM_BEARS
	{
		{ "SWOOP OUT",		&play_ch_1,			TEAMPLAY_1_AUD,		{ &ps_chi_1,				NULL } },
		{ "RUMBLE",			&play_ch_2,			TEAMPLAY_2_AUD,		{ &ps_chi_2,				NULL } },
		{ "WIDE SPREAD",	&play_ch_3,			TEAMPLAY_3_AUD,		{ &ps_chi_3,				NULL } },
	},
	//	TEAM_BENGALS
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_BROWNS
	{
		{ "SHORT JET",		&play_az_1,			TEAMPLAY_1_AUD,		{ &ps_ari_1,				NULL } },
		{ "STACK IT",		&play_az_2,			TEAMPLAY_2_AUD,		{ &ps_ari_3,				NULL } },
		{ "SMACK YOU",		&play_az_3,			TEAMPLAY_3_AUD,		{ &ps_ari_2,				NULL } },
	},
	//	TEAM_DALLAS
	{
		{ "FLY AWAY",		&play_at_1,			TEAMPLAY_1_AUD,		{ &ps_atl_1,				NULL } },
		{ "SMILEY FACE",	&play_at_2,			TEAMPLAY_2_AUD,		{ &ps_atl_2,				NULL } },
		{ "DOG BONE",		&play_at_3,			TEAMPLAY_3_AUD,		{ &ps_atl_3,				NULL } },
	},
	//	TEAM_DENVER
	{
		{ "DEEP THRO IT",	&play_ba_1,			TEAMPLAY_1_AUD,		{ &ps_bal_1,				NULL } },
		{ "STRAIGHT IN",	&play_ba_2,			TEAMPLAY_2_AUD,		{ &ps_bal_2,				NULL } },
		{ "BACK SIDE",		&play_ba_3,			TEAMPLAY_3_AUD,		{ &ps_bal_3,				NULL } },
	},
	//	TEAM_DETROIT
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_GREEN BAY
	{
		{ "DEEP SPANK",		&play_ca_1,			TEAMPLAY_1_AUD,		{ &ps_car_1,				NULL } },
		{ "2 GO DEEP",		&play_ca_2,			TEAMPLAY_2_AUD,		{ &ps_car_2,				NULL } },
		{ "UNDERTAKER",		&play_ca_3,			TEAMPLAY_3_AUD,		{ &ps_car_3,				NULL } },
	},
	//	TEAM_INDIANAPOLIS
	{
		{ "SWOOP OUT",		&play_ch_1,			TEAMPLAY_1_AUD,		{ &ps_chi_1,				NULL } },
		{ "RUMBLE",			&play_ch_2,			TEAMPLAY_2_AUD,		{ &ps_chi_2,				NULL } },
		{ "WIDE SPREAD",	&play_ch_3,			TEAMPLAY_3_AUD,		{ &ps_chi_3,				NULL } },
	},
	//	TEAM_JACKSONVILLE
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_KANSAS CITY
	{
		{ "SHORT JET",		&play_az_1,			TEAMPLAY_1_AUD,		{ &ps_ari_1,				NULL } },
		{ "STACK IT",		&play_az_2,			TEAMPLAY_2_AUD,		{ &ps_ari_3,				NULL } },
		{ "SMACK YOU",		&play_az_3,			TEAMPLAY_3_AUD,		{ &ps_ari_2,				NULL } },
	},
	//	TEAM_MIAMI
	{
		{ "FLY AWAY",		&play_at_1,			TEAMPLAY_1_AUD,		{ &ps_atl_1,				NULL } },
		{ "SMILEY FACE",	&play_at_2,			TEAMPLAY_2_AUD,		{ &ps_atl_2,				NULL } },
		{ "DOG BONE",		&play_at_3,			TEAMPLAY_3_AUD,		{ &ps_atl_3,				NULL } },
	},
	//	TEAM_MINNESOTA
	{
		{ "DEEP THRO IT",	&play_ba_1,			TEAMPLAY_1_AUD,		{ &ps_bal_1,				NULL } },
		{ "STRAIGHT IN",	&play_ba_2,			TEAMPLAY_2_AUD,		{ &ps_bal_2,				NULL } },
		{ "BACK SIDE",		&play_ba_3,			TEAMPLAY_3_AUD,		{ &ps_bal_3,				NULL } },
	},
	//	TEAM_NEW ENGLAND
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_NEWORLEANS
	{
		{ "DEEP SPANK",		&play_ca_1,			TEAMPLAY_1_AUD,		{ &ps_car_1,				NULL } },
		{ "2 GO DEEP",		&play_ca_2,			TEAMPLAY_2_AUD,		{ &ps_car_2,				NULL } },
		{ "UNDERTAKER",		&play_ca_3,			TEAMPLAY_3_AUD,		{ &ps_car_3,				NULL } },
	},
	//	TEAM_NYGIANTS
	{
		{ "SWOOP OUT",		&play_ch_1,			TEAMPLAY_1_AUD,		{ &ps_chi_1,				NULL } },
		{ "RUMBLE",			&play_ch_2,			TEAMPLAY_2_AUD,		{ &ps_chi_2,				NULL } },
		{ "WIDE SPREAD",	&play_ch_3,			TEAMPLAY_3_AUD,		{ &ps_chi_3,				NULL } },
	},
	//	TEAM_NYJETS
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_RAIDERS
	{
		{ "SHORT JET",		&play_az_1,			TEAMPLAY_1_AUD,		{ &ps_ari_1,				NULL } },
		{ "STACK IT",		&play_az_2,			TEAMPLAY_2_AUD,		{ &ps_ari_3,				NULL } },
		{ "SMACK YOU",		&play_az_3,			TEAMPLAY_3_AUD,		{ &ps_ari_2,				NULL } },
	},
	//	TEAM_PHILLY
	{
		{ "FLY AWAY",		&play_at_1,			TEAMPLAY_1_AUD,		{ &ps_atl_1,				NULL } },
		{ "SMILEY FACE",	&play_at_2,			TEAMPLAY_2_AUD,		{ &ps_atl_2,				NULL } },
		{ "DOG BONE",		&play_at_3,			TEAMPLAY_3_AUD,		{ &ps_atl_3,				NULL } },
	},
	//	TEAM_PITTSBURGH
	{
		{ "DEEP THRO IT",	&play_ba_1,			TEAMPLAY_1_AUD,		{ &ps_bal_1,				NULL } },
		{ "STRAIGHT IN",	&play_ba_2,			TEAMPLAY_2_AUD,		{ &ps_bal_2,				NULL } },
		{ "BACK SIDE",		&play_ba_3,			TEAMPLAY_3_AUD,		{ &ps_bal_3,				NULL } },
	},
	//	TEAM_SANDIEGO
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_49RS
	{
		{ "DEEP SPANK",		&play_ca_1,			TEAMPLAY_1_AUD,		{ &ps_car_1,				NULL } },
		{ "2 GO DEEP",		&play_ca_2,			TEAMPLAY_2_AUD,		{ &ps_car_2,				NULL } },
		{ "UNDERTAKER",		&play_ca_3,			TEAMPLAY_3_AUD,		{ &ps_car_3,				NULL } },
	},
	//	TEAM_SEATTLE
	{
		{ "SWOOP OUT",		&play_ch_1,			TEAMPLAY_1_AUD,		{ &ps_chi_1,				NULL } },
		{ "RUMBLE",			&play_ch_2,			TEAMPLAY_2_AUD,		{ &ps_chi_2,				NULL } },
		{ "WIDE SPREAD",	&play_ch_3,			TEAMPLAY_3_AUD,		{ &ps_chi_3,				NULL } },
	},
	//	TEAM_RAMS
	{
		{ "POST ROLL",		&play_bu_1,			TEAMPLAY_1_AUD,		{ &ps_buf_1,				NULL } },
		{ "BLITZ BEATER",	&play_bu_2,			TEAMPLAY_2_AUD,		{ &ps_buf_2,				NULL } },
		{ "THUNDER",		&play_bu_3,			TEAMPLAY_3_AUD,		{ &ps_buf_3,				NULL } },
	},
	//	TEAM_BUCS
	{
		{ "SHORT JET",		&play_az_1,			TEAMPLAY_1_AUD,		{ &ps_ari_1,				NULL } },
		{ "STACK IT",		&play_az_2,			TEAMPLAY_2_AUD,		{ &ps_ari_3,				NULL } },
		{ "SMACK YOU",		&play_az_3,			TEAMPLAY_3_AUD,		{ &ps_ari_2,				NULL } },
	},
	//	TEAM_OILERS
	{
		{ "FLY AWAY",		&play_at_1,			TEAMPLAY_1_AUD,		{ &ps_atl_1,				NULL } },
		{ "SMILEY FACE",	&play_at_2,			TEAMPLAY_2_AUD,		{ &ps_atl_2,				NULL } },
		{ "DOG BONE",		&play_at_3,			TEAMPLAY_3_AUD,		{ &ps_atl_3,				NULL } },
	},
	//	TEAM_WASHINGTON
	{
		{ "DEEP THRO IT",	&play_ba_1,			TEAMPLAY_1_AUD,		{ &ps_bal_1,				NULL } },
		{ "STRAIGHT IN",	&play_ba_2,			TEAMPLAY_2_AUD,		{ &ps_bal_2,				NULL } },
		{ "BACK SIDE",		&play_ba_3,			TEAMPLAY_3_AUD,		{ &ps_bal_3,				NULL } },
	},
	{
		{ NULL, NULL, NULL, { NULL, NULL } },
	},
};

PLAYSTUF__SLOT extra_slots[] =
{
	#define EXTRA_SLOT_ID_CUSTOM		0
	{ "CUSTOM",			&play_extra_point,		0,					{ &limb_blank,				&limb_blank } },

	{ NULL, NULL, NULL, { NULL, NULL } },
};

PLAYSTUF__PAGE play_pages[2][PLAYSTUF__MAX_PLAY_PAGES] =		//	[OFF/DEF][PAGE]
{
	// OFFENSE PLAY PAGES
	{
		// Page 1
		{
			{
			&off_slots[OFF_SLOT_ID_TEAMPLAY_1],		&off_slots[OFF_SLOT_ID_TEAMPLAY_2],		&off_slots[OFF_SLOT_ID_TEAMPLAY_3],
			&off_slots[OFF_SLOT_ID_DEEP_ATTACK],	&off_slots[OFF_SLOT_ID_FLOOD_SLIT],		&off_slots[OFF_SLOT_ID_POST_DRAG],
//			&off_slots[OFF_SLOT_ID_ZIG_ZAG],		&off_slots[OFF_SLOT_ID_SPIDER_LEGS],	&off_slots[OFF_SLOT_ID_MONKEY],
			&off_slots[OFF_SLOT_ID_TASTY_TREAT],	&off_slots[OFF_SLOT_ID_KOMBAT],			&off_slots[OFF_SLOT_ID_BLACK_RAIN],
			},
		},
		// Page 2
		{
			{
			&off_slots[OFF_SLOT_ID_UPPER_CUT],		&off_slots[OFF_SLOT_ID_DA_BOMB],		&off_slots[OFF_SLOT_ID_HAIL_MARY2],
			&off_slots[OFF_SLOT_ID_TURMOIL],		&off_slots[OFF_SLOT_ID_BACK_SPLIT],		&off_slots[OFF_SLOT_ID_SUBZERO],
			&off_slots[OFF_SLOT_ID_DAWG_HOOK],		&off_slots[OFF_SLOT_ID_UTB_DEEP],		&off_slots[OFF_SLOT_ID_CROSS_SLANT],
			},
		},
		// Page 3
		{
			{
			&off_slots[OFF_SLOT_ID_ZIG_ZAG],		&off_slots[OFF_SLOT_ID_SPIDER_LEGS],	&off_slots[OFF_SLOT_ID_MONKEY],
			&off_slots[OFF_SLOT_ID_SLIP_SLIDE],		&off_slots[OFF_SLOT_ID_QB_POST],		&off_slots[OFF_SLOT_ID_QUICK_DISH],
			&off_slots[OFF_SLOT_ID_SWITCH_UP],		&off_slots[OFF_SLOT_ID_CRUISN],			&off_slots[OFF_SLOT_ID_HURRICANE],
			},
		},
		// Page 4
		{
			{
			&off_slots[OFF_SLOT_ID_BLIZZARD],		&off_slots[OFF_SLOT_ID_CRISS_CROSS],	&off_slots[OFF_SLOT_ID_UP_THE_GUT],
			&off_slots[OFF_SLOT_ID_SCREEN_RIGHT],	&off_slots[OFF_SLOT_ID_SUPER_FLY],		&off_slots[OFF_SLOT_ID_MIDDLE_PICK],
			&off_slots[OFF_SLOT_ID_SWEEP_RIGHT],	&off_slots[OFF_SLOT_ID_REVERSE_ZIP],	&off_slots[OFF_SLOT_ID_HB_BLOCK],
			},
		},
		// Page 5
		{
			{
			&extra_slots[EXTRA_SLOT_ID_CUSTOM],		&extra_slots[EXTRA_SLOT_ID_CUSTOM],		&extra_slots[EXTRA_SLOT_ID_CUSTOM],
			&extra_slots[EXTRA_SLOT_ID_CUSTOM],		&extra_slots[EXTRA_SLOT_ID_CUSTOM],		&extra_slots[EXTRA_SLOT_ID_CUSTOM],
			&extra_slots[EXTRA_SLOT_ID_CUSTOM],		&extra_slots[EXTRA_SLOT_ID_CUSTOM],		&extra_slots[EXTRA_SLOT_ID_CUSTOM],
			},
		},
	},
	// DEFENSE PLAY PAGES
	{
		// Page 1
		{
			{
			&def_slots[DEF_SLOT_ID_SAFE_COVER],		&def_slots[DEF_SLOT_ID_STUFF_IT],		&def_slots[DEF_SLOT_ID_TWO_MAN_BLITZ],
			&def_slots[DEF_SLOT_ID_FAKE_ZONE],		&def_slots[DEF_SLOT_ID_SUICIDE_BLITZ],	&def_slots[DEF_SLOT_ID_KNOCK_DOWN],
			&def_slots[DEF_SLOT_ID_ONE_MAN_BLITZ],	&def_slots[DEF_SLOT_ID_NEAR_ZONE],		&def_slots[DEF_SLOT_ID_SPREAD],
			},
		},
		// Page 2
		{
			{
			&def_slots[DEF_SLOT_ID_MEDIUM_ZONE],	&def_slots[DEF_SLOT_ID_GOAL_LINE],		&def_slots[DEF_SLOT_ID_DEEP_ZONE],
			&def_slots[DEF_SLOT_ID_LEFT_ZONE],		&def_slots[DEF_SLOT_ID_STOP_RUN],		&def_slots[DEF_SLOT_ID_RIGHT_ZONE],
			&def_slots[DEF_SLOT_ID_MAN_ON_MAN],		&def_slots[DEF_SLOT_ID_MIX_IT_UP],		&def_slots[DEF_SLOT_ID_ZONE_BLITZ],
			},
		},
		// Page 3
		{
			{
			NULL, NULL, NULL,
			NULL, NULL, NULL,
			NULL, NULL, NULL,
			},
		},
		// Page 4
		{
			{
			NULL, NULL, NULL,
			NULL, NULL, NULL,
			NULL, NULL, NULL,
			},
		},
		// Page 5
		{
			{
			NULL, NULL, NULL,
			NULL, NULL, NULL,
			NULL, NULL, NULL,
			},
		},
	},
};


/////////////////////////////////
//	MEMBER DECLARATIONS
/////////////////////////////////

//	"Original" plays
static play_t * original_play[2];
static int original_flip;
static cap_play_t * original_custom;

//	FIX ME: MOVE THESE TO PLAYSEL.C

//	Play pages
static PLAYSTUF__PAGE current_pages[2][PLAYSTUF__MAX_PLAY_PAGES];			//	[off/def][page]

//	Current page ID for each side - which page each side is currently on (index into 'current_pages' array)
static int current_page_ids[2];												//	[off/def]




/////////////////////////////////
//	MEMBER ACCESS FUNCTIONS
/////////////////////////////////

play_t * PlayStuf__GetAudiblePlay( int pnum, int off_def, int aud_num )
{
	int team = game_info.off_side;
	if( off_def == DEFENSE )
		team ^= 1;

	if( aud_num == 0 )
	{
		if( off_def == OFFENSE )
		{
			game_info.off_flip = original_flip;

			if( original_play[team] == &custom_play )
			{
				convert_play( original_custom );
			}
		}
		return original_play[team];
	}
	else
	{
		signed short id = audible_ids[pnum].ids[off_def][aud_num-1];

		if( off_def == OFFENSE )
		{
			game_info.off_flip = id < 0;
			if( id < 0 )
				id = -id;
		}
		id--;

		return PlaySel__MakeChoice( off_def, (id / PLAYSTUF__PLAYS_PER_PAGE), pnum, (id % PLAYSTUF__PLAYS_PER_PAGE), team, 0 );
	}
}

PLAYSTUF__SLOT * PlayStuf__GetSlot( int side, int page, int num )
{
	return current_pages[side][page].slot_ptrs[num];
}

void PlayStuf__SetCurrentPlayPage( int side, int page )
{
	current_page_ids[side] = page;
}

int PlayStuf__GetCurrentPlayPage( int side )
{
	return current_page_ids[side];
}


/////////////////////////////////
//	NORMAL FUNCTIONS
/////////////////////////////////

//	This function is called before each play begins (in-game)
//	to reset all audible mechanisms
void PlayStuf__ResetAudibles(void)
{
	int i, x;

	game_info.audible_num[OFFENSE] = 0;
	game_info.audible_num[DEFENSE] = 0;

	//	clear the "audible was called" flag for both sides
	game_info.audible_flags[OFFENSE] = 0;		
	game_info.audible_flags[DEFENSE] = 0;

	// Determine which sticks we should be paying attention to
	for (x = 0; x < 2; x++)
	{
		if (!four_plr_ver)
			game_info.audible_plyr[x] = x;
		else
		{
			i = x + x;
			if ((p_status & (3<<i)) == (3<<i))
				game_info.audible_plyr[x] = (x == game_info.off_side) ? off_captain() : game_info.captain;
			else
				game_info.audible_plyr[x] = ((p_status >> i) & 3) - 1;
			game_info.audible_plyr[x] += i;
		}
	}

	//	Reset the audible double-tap mechanisms
	audible_turbo_timer[0] = 0;
	audible_turbo_timer[1] = 0;
	audible_tap_action[0] = 0;
	audible_tap_action[1] = 0;

	original_play[0] = game_info.team_play[0];
	original_play[1] = game_info.team_play[1];
	original_flip    = game_info.off_flip;
	original_custom  = convert_play_last;
}

/////////////////////////////////
/////////////////////////////////

void PlayStuf__SetupCurrentPlayPages( int do_specials, int do_extra_pt )
{
	int off_team_id, i, j;

	//	Set the normal plays
	//	Copy the normal set of play pages (without tweaked plays like F.G., Punt, Extra Point, Block F.G., Team special plays)
	for( i = 0; i < 2; i++ )
	{
		for( j = 0; j < PLAYSTUF__MAX_PLAY_PAGES; j++ )
		{
			memcpy(&current_pages[i][j], &play_pages[i][j], sizeof(PLAYSTUF__PAGE));
		}
	}

	if( do_specials )
	{
		//	Set the 3 team-special plays
		off_team_id = game_info.team[game_info.off_side];

		current_pages[OFFENSE][0].slot_ptrs[0] = &team_slots[off_team_id][0];
		current_pages[OFFENSE][0].slot_ptrs[1] = &team_slots[off_team_id][1];
		current_pages[OFFENSE][0].slot_ptrs[2] = &team_slots[off_team_id][2];

		// New stuff it play was too strong - if not within your own 30, use an alternate stuff it play
		// that isn't as strong.
		if (game_info.los < 85)
			current_pages[DEFENSE][0].slot_ptrs[1] = &def_slots[DEF_SLOT_ID_STUFF_IT2];


		//	"Tweak" the plays, depending on the situation in the game
		//	(for field goals, punts, extra point, etc.)
		//	Currently all "tweaks" are only on the first page (for both offense and defense)

//#define FG_PUNT  1
#if FG_PUNT == 1
		//	DEBUG MODE - FIELD GOALS EVERY TIME
#elif FG_PUNT == 2
		//	DEBUG MODE - PUNT EVERY TIME - DON'T DO FIELD GOAL
		if(0)
		{
#else
		//	NORMAL GAME MODE - CHECK NORMAL CASES FOR FIELD GOAL
		if ((game_info.los >= 50 || pup_superfg) && (game_info.play_type != PT_EXTRA_POINT))
		{
			if((game_info.down == 4)
			||	((game_info.game_quarter == 1 || game_info.game_quarter >= 3) && (game_info.game_time < 0x00001000)))
			{
#endif
				//	Offer field goal plays (fake, kick & block)
				//	Offense can kick or fake a field goal (2 different plays)
				current_pages[OFFENSE][0].slot_ptrs[PLAYSTUF__PLAYS_PER_PAGE-3] = &off_slots[OFF_SLOT_ID_FIELD_GOAL];
				current_pages[OFFENSE][0].slot_ptrs[PLAYSTUF__PLAYS_PER_PAGE-2] = &off_slots[OFF_SLOT_ID_FAKE_FG];

				//	Defense blocks field goal
				current_pages[DEFENSE][0].slot_ptrs[PLAYSTUF__PLAYS_PER_PAGE-3] = &def_slots[DEF_SLOT_ID_BLOCK_FG];

#if FG_PUNT == 1
		//	DEBUG MODE - FIELD GOALS EVERY TIME
#elif FG_PUNT == 2
		//	DEBUG MODE - PUNT EVERY TIME - DON'T DO FIELD GOAL
		}
#else
		//	NORMAL GAME MODE
			}
		}
#endif

#if FG_PUNT == 1
		//	DEBUG MODE - FIELD GOAL EVERY TIME (DON'T PUNT)
		if(0)
		{
#elif FG_PUNT == 2
		//	DEBUG MODE - PUNT EVERY TIME
#else
		//	NORMAL GAME MODE - CHECK NORMAL CASES FOR PUNTING
		if((game_info.los < 50 && !pup_nopunt && !pup_superfg) && (game_info.down == 4))
		{
#endif
			//	Offer punt plays (kick, fake, & punt return)
			//	Offense can kick or fake a punt
			current_pages[OFFENSE][0].slot_ptrs[PLAYSTUF__PLAYS_PER_PAGE-3] = &off_slots[OFF_SLOT_ID_PUNT];
			current_pages[OFFENSE][0].slot_ptrs[PLAYSTUF__PLAYS_PER_PAGE-2] = &off_slots[OFF_SLOT_ID_FAKE_PUNT];

			//	Defense can do "punt return"
			current_pages[DEFENSE][0].slot_ptrs[PLAYSTUF__PLAYS_PER_PAGE-2] = &def_slots[DEF_SLOT_ID_PUNT_RETURN];

#if FG_PUNT == 1
		//	DEBUG MODE - FIELD GOAL EVERY TIME (DON'T PUNT)
		}
#elif FG_PUNT == 2
		//	DEBUG MODE - PUNT EVERY TIME
#else
		//	NORMAL GAME MODE
		}
#endif
	}
	//	EXTRA POINT (offense only)
	if( do_extra_pt && game_info.play_type == PT_EXTRA_POINT )
	{	
		current_pages[OFFENSE][0].slot_ptrs[0] = &off_slots[OFF_SLOT_ID_EXTRA_POINT];
	}
}
