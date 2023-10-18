/****************************************************************************/
/*                                                                          */
/* sounds.c - Sound call handler code                                       */
/*                                                                          */
/* Written by:  Jeff "Japple" Johnson										*/
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <goose/goose.h>
#include <string.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Anim/genseq.h"
#include "/Video/Nfl/Include/gameadj.h"
#include "/Video/Nfl/Include/Sounds.h"

extern float	distxz[NUM_PLAYERS][NUM_PLAYERS];

void extra_point_speech(void);
void field_goal_speech(void);
void tackle_spch(obj_3d *, int, int);
void get_hit_snd(obj_3d *, int, int, int);
void grnd_hit_snd(obj_3d *, int, int, int);
void attack_snd(obj_3d *, int, int, int);
void hut_snd(obj_3d *, int, int, int);
void qb_actions_sp(int *args);
void catch_sounds(obj_3d *);
void throw_ball_sounds(void);
void idiot_speech(void);
void lateral_speech(void);
static void sacked_sp_proc(int *args);
static void tackled_sp_proc(int *args);
static void tackled2_sp_proc(int *args);
//static void yards_gained_sp_proc(int *args);
void blowout_sp(void);
void gutsy_call_sp(void);
void safety_sp(void);
void qb_runs_past_los_sp(void);
void clock_low_sp(void);
void intercepted_ball_sp(void);
void bobble_ball_sounds(void);
void fumble_ball_sounds(void);
void incomplete_pass_sp(void);
void deflected_ball_sp(void);
void eoq_speech_proc(int *args);
void catch_speech_proc(int *args);
void hurdle_speech(void);
void spin_move_speech(void);
void stiff_arm_speech(void);
//void stiff_arm_f_speech(void);
void plyr_open_sp(obj_3d *, int);
void taunt_speech(obj_3d *, int, int);
void celebrate_speech(obj_3d *, int);
void almost_touchdown_sp(obj_3d *);
void first_down_speech(void);
void first_down_sp_proc(int *args);
void versus_speech_proc(void);
int need_big_play_sp(void);
void disappointed_speech(void);


//int said_scr_sp = 0;
int said_latehit_sp = 0;
int late_hit_sp_cnt = 0;
//int tackle_cnt = 0;
int said_30_sp = 0;
int said_20_sp = 0;
int said_10_sp = 0;
int said_5_sp = 0;
int	last_los = -1;

static char *team_bnk_ptrs[] = {team1_bnk_str,team2_bnk_str};

static int field_goal_sp[] = {100,101};
//static int extra_point_sp[] = {100,101,102};

static int gutsy_sp[] = {219,220,221,219,220,221,222};

static int linemen_hit_snds[] = {
							P_HIT10_SND,
							P_HIT11_SND,
							P_HIT12_SND,
							P_HIT13_SND,
							P_HIT16_SND };

static int non_carrier_hit_snds[] = {
							P_HIT1_SND,
							P_HIT2_SND,
							P_HIT3_SND,
							P_HIT4_SND,
							P_HIT5_SND,
							P_HIT6_SND,
							P_HIT7_SND,
							P_HIT8_SND,
							P_HIT14_SND,
							P_HIT15_SND,
							P_HIT17_SND };

static int carrier_hit_snds[] = {
							P_HARD_HIT1_SND,
							P_HIT2_SND,
							P_HARD_HIT2_SND,
							P_HIT3_SND,
							P_HARD_HIT3_SND };

static int plr_attack_snds[] = {
							P_ATTK1_SND,
							P_ATTK2_SND,
							P_ATTK3_SND,
							P_ATTK4_SND,
							P_ATTK5_SND,
							P_ATTK6_SND,
							P_ATTK7_SND };

static int grnd_hit_snds[] = {
							P_GRND_HIT1_SND,
							P_GRND_HIT2_SND,
							P_GRND_HIT3_SND };

static int neck_crack_snds[] = {
								P_NECK_CRCK1_SND,
								P_NECK_CRCK2_SND,
								P_NECK_CRCK3_SND,
								P_NECK_CRCK4_SND };

static int plr_land_grunt_snds[] = {
									P_LAND_GRUNT1_SP,
						 			P_LAND_GRUNT2_SP,
						 			P_LAND_GRUNT3_SP,
						 			P_LAND_GRUNT4_SP,
						 			P_LAND_GRUNT5_SP,
						 			P_LAND_GRUNT6_SP,
						 			P_LAND_GRUNT7_SP,
						 			P_LAND_GRUNT8_SP,
						 			P_LAND_GRUNT9_SP,
						 			P_LAND_GRUNT10_SP,
						 			P_LAND_GRUNT11_SP };

static int fly_thru_air_snds[] = {
								P_GET_THRWN1_SP,
					 			P_GET_THRWN2_SP,
					 			P_GET_THRWN3_SP,
					 			P_GET_THRWN4_SP,
					 			P_GET_THRWN5_SP };


static int qb_point_snds[] =
	{
		P_HUT3_SP,
		30,
		31,
		32
	};
		

static int hut_snds[] =
	{
		P_HUT2_SP,
		P_HUT3_SP,
	};


static int crowd_cheers_snds[] =
								{
									CRWD_CHEER1_SND,
									CRWD_CHEER2_SND,
									CRWD_CHEER1_SND,
								};

static int tackle_crowd_cheers_snds[] =
								{
									CRWD_CHEER1_SND,
									CRWD_CHANT1_SND,
									CRWD_CHEER2_SND,
									CRWD_CHANT2_SND,
									CRWD_CHEER2_SND,
									CRWD_BOO1_SND,
									CRWD_CHANT1_SND,
									CRWD_CHEER2_SND,
									CRWD_CHEER1_SND,
									CRWD_CHANT2_SND,
								};


//static int qb_grunt_snds[] = {
//						 	  	P_LAND_GRUNT3_SP,
//						 	  	P_LAND_GRUNT5_SP,
//								P_LAND_GRUNT8_SP };
					
static int throw_ball_snds[] = {P_THRW_BALL1_SND,P_THRW_BALL2_SND,P_THRW_BALL3_SND,P_THRW_BALL4_SND};
					
						
//static int down_sp[] = {	
//						FIRST_DWN_SP,
//						SECOND_DWN_SP,
//						THIRD_DWN_SP,
//						FOURTH_DWN_SP};

static int down_sp[] = {	
						25,
						26,
						27,
						28};

//static int down_shrt_sp[] = {
//							FIRST_DWN_SP,
//							SECOND_DWN_SHT_SP,
//							THIRD_DWN_SHT_SP,
//							FOURTH_DWN_SP};
//						
//static int down_long_sp[] = {
//							FIRST_DWN_SP,
//							SECOND_DWN_LNG_SP,
//							THIRD_DWN_LNG_SP,
//							FOURTH_DWN_LONG_SP};
						
static int down_n_goal_sp[] = {
							FIRST_AND_GOAL_SP,
							SECOND_AND_GOAL_SP,
							THIRD_AND_GOAL_SP,
							FOURTH_AND_GOAL_SP };
						
//static int ramble_sp[] = {
//							LOOKS_LIKE_RUN_SP,
//						  	LOOKS_LIKE_BLITZ_SP	};
					
					
// **** NOTE: **** These numbers are speech calls....roughly of the same type of call...in different banks
//
//  ie... #60 "WHY" (bank1) but its "TRUELY PATHETIC DISPLAY" in bank2
//

static int fades_back_sp[] = {22,23,24};

static int disappointed_sp[] = {45,46};

static int idiot_sp[] = {60,61,62,63,64,65};
							
static int monkey_tackle_sp[] = {78,79};
								
static int weak_tackle_sp[] = {0,1,2,5,11,231};
							 
static int hard_tackle_sp[] = {1,2,3,4,5,6,7,8,9,10,11,14,240,241,230,231,232,233};

static int qb_sacked_sp[] = {3,6,7,12,13,13,28,30,31,170,171,240};

static int qb_runs_sp[] = {175,176,211,177,178,179};

static int catch_short_sp[] = {70,84,85,86,87,88,89};

static int catch_long_sp[] = {70,71,72,73,74,82,83,87,88,89,90,91};
		
static int catch_fire_sp[] = {182,183,71};

static int catch_bobble_sp[] = {77,87,88,71,72,99};

static int throw_short_sp[] = {32,40,50,51,52};
				
static int throw_med_sp[] = {32,41,36,39,40,41,42,43,53};
					
static int throw_long_sp[] = {37,34,41,58,42,41,53,54,57,58,59,57,41,210,211};

static int throw_jump_sp[] = {48,49,50,52};
			
static int ancr_safety_sp[] = {61,63,109,202};

static int hit_receiver_sp[] = {92,134};
					
static int leads_by_sp[] = {135,112,113,114,115,116,117,118,119,120,	// tie, lb 1, 2,3,4,5,6,7..etc
							121,122,123,124,125,0,0,126,0,0,0,127,		// lb 10,11,12,13,14,7,21
							0,0,0,0,0,0,128};							// leads by 28
							 
static int down_by_sp[] = {135,71,73,74,75,76,77,78,79,80,				// tie, down by 1, 2,3,4,5,6,7,8,9
						   81,82,83,84,85,0,0,87,0,0,0,88,				// down by 10,11,12,13,14,7,21
						   0,0,0,0,0,0,90};							// down by 28

//static int yards_gained_sp[] = {0,0,0,0,0,0,0,0,0,0,190,
//								0,192,0,194,0,196,0,198,0,200,0,
//								202,0,204,0,206,0,208,0,210};
						   
static int late_hit_sp[] = {140,141,142,143,144,145,146,146,147,148,149};

static int no_swear_late_hit_sp[] = {140,141,149,142,143,146,147,140,143,142,146,140,141,149,142,146,143,147};
					
//static int qb_bitch_slap_sp[] = {245,247,248};

static int qb_duck_sp[] = {246,247,248};

static int hurdle_sp[] = {154,155,155,155};

static int lateral_sp[] = {46,52};

static int head_plow_sp[] = {72,152,153,153};

static int intercepted_sp[] = {190,190,191,191,192};

static int bobble_sp[] = {98,98,195};

static int fumble_sp[] = {196,196,197};

static int didnt_hold_ball_sp[] = {96,97,98,97,96,96,109};
static int incomplete_sp[] = {33,56,55,62,63,64,96,96,96,109};

static int deflected_sp[] = {194,195,33,194,195};

static int ancr_blowout_sp[] = {117,126,137};

static int im_open_sp[] = {
					P_WHAT_WAITING_4_SP,
					P_THROW_TO_ME_SP,
					P_IM_CLEAR_SP,
					P_OVER_HERE_SP,
					P_THROW_TO_ME2_SP,
					P_CMON_THROW_IT_SP,
					P_YO_OVER_HERE_SP,
					P_ANYDAY_NOW_SP,
					P_IM_WAITING_SP };

static int critical_sp[] = {137,138,139};

static int clock_sp[] = {204,0,205,206,0};

static int short_taunt_sp[] = {0,1,2,3,4, 53, 51};
static int long_taunt_sp[] =  {20,21,22,23,25,26,50,51,52,53};
static int laugh_taunt_sp[] =  {40,41,42};
static int histep_taunt_sp[] = {12,13,4};

//static int celebrate_sp[] = {0,1,2,3,4,5};

#define TCNV(x)	(((int)((float)(x) * 1.046f)) +1 +1 +1)			//+rndup +endtck +buff
			// !!!FIX (make delay length of team speech)
static float team_city_sp_lengths[NUM_TEAMS] = {41.0f,39.8f,43.9f,41.0f,41.9f,45.2f,46.5f,36.0f,35.6f,36.4f,
                                                41.4f,47.3f,48.6f,52.3f,38.5f,43.1f,39.8f,38.5f,40.2f,37.7f,
                                                34.7f,47.7f,33.9f,48.6f,56.5f,35.6f,49.0f,42.7f,37.3f,45.6f,45.6f};

//static float team_name_sp_lengths[NUM_TEAMS] = {36.0f,35.2f,36.4f,32.2f,49.0f,42.7f,46.0f,51.1f,51.1f, 51.1f,
//                                                44.4f,44.4f,51.9f,38.9f,47.3f,47.3f,50.6f,41.9f,43.5f,40.2f,
//                                                47.7f,48.1f,51.1f,72.0f,64.5f,53.6f,41.0f,58.6f,39.3f,51.9f,51.9f};

static float team_name_sp_lengths[NUM_TEAMS] = {36.0f,35.2f,36.4f,32.2f,49.0f,42.7f,46.0f,51.1f, 51.1f,51.1f,42.7f,
                                                44.4f,41.0f,51.9f,38.9f,47.3f,47.3f,50.6f,41.9f,43.5f, 43.5f, //40.2f,
                                                47.7f,48.1f,51.1f,
                                                70.8f,	//chargers
                                                64.5f,	//49ers
                                                53.6f,	//seahawks
                                                41.0f,	//rams
                                                58.6f,	//bucs
                                                42.3f,	//ten
                                                51.9f	//wash
                                               	};
		
												
static float full_team_name1_sp_lengths[NUM_TEAMS] = { 89.2f, 77.9f, 85.4f, 67.8f, 86.2f, 77.0f, 90.4f,  82.0f, 82.0f, 77.4f, 74.5f,
                                                       82.9f, 80.4f, 94.2f, 82.0f, 85.4f, 82.0f, 73.7f, 71.6f, 78.3f, 68.2f,
                                                       59.4f, 91.2f, 86.2f, 88.3f,108.4f, 78.7f, 77.0f,108.4f, 75.8f, 82.5f};

#if 0												
static float full_team_name2_sp_lengths[NUM_TEAMS] = { 96.3f, 79.9f, 94.6f, 78.3f, 98.4f, 91.2f, 96.7f, 91.7f, 81.6f, 90.0f,
                                                      106.3f, 91.2f,107.6f, 95.9f, 93.3f, 97.5f,104.6f, 82.0f, 92.9f, 82.5f,
                                                       87.1f, 98.8f, 99.6f,119.3f,134.8f, 97.1f, 91.7f,114.3f, 88.3f,101.3f,101.3f};
#endif												

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void field_goal_speech(void)
{
 	snd_scall_bank(ancr_swaped_bnk_str,field_goal_sp[randrng(sizeof(field_goal_sp)/sizeof(int))],VOLUME4,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void extra_point_speech(void)
{
 	snd_scall_bank(ancr_swaped_bnk_str,field_goal_sp[randrng(sizeof(field_goal_sp)/sizeof(int))],VOLUME4,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void versus_speech_proc(void)
{
	snd_scall_bank(ancr_bnk_str,MATCHUP_SP,VOLUME3,127,LVL1);
	sleep(TCNV(58.6f));

	snd_scall_bank(team_bnk_ptrs[0],FULL_TEAM_NAME1_SP,VOLUME3,127,LVL2);
	sleep((int)(full_team_name1_sp_lengths[game_info.team[0]]));

	if (randrng(2))
	{	snd_scall_bank(ancr_bnk_str,VERSUS_SP,VOLUME3,127,LVL3);
		sleep(TCNV(36.4f));
	}
	else
	{
		snd_scall_bank(ancr_bnk_str,TAKEON_SP,VOLUME3,127,LVL3);
		sleep(TCNV(36.4f));
	}

	snd_scall_bank(team_bnk_ptrs[1],FULL_TEAM_NAME2_SP,VOLUME3,127,LVL4);
//	sleep(full_team_name2_sp_lengths[game_info.team[1]]);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
#if 0
void first_down_speech(void)
{
	if ((game_info.game_mode == GM_LINING_UP) || (game_info.game_mode == GM_PRE_SNAP))
		return;

	snd_scall_bank(generic_bnk_str,FIRST_DWN_SP,ANCR_VOL1,127,LVL2);
//	snd_scall_bank(ancr_bnk_str,FIRST_DWN_SP,VOLUME3,127,LVL3);
//	qcreate("firstdn",0,first_down_sp_proc, 0, 0, 0, 0);
}
#endif
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
#if 0
void first_down_sp_proc(int *args)
{
	const struct st_plyr_data *pspd;
	
	if ((randrng(100) < 30) && (game_info.ball_carrier != -1))
	{
		// say WEAPON...gets the 1st down!
//		pspd = teaminfo[game_info.team[game_info.off_side]].static_data;
//		pspd += game_info.ball_carrier % 7;
//		if (pspd->ancr == 0)
//			die(0);
//		snd_scall_bank(team_bnk_ptrs[game_info.off_side],pspd->ancr+3,VOLUME3,127,LVL4);
//		sleep(58);
		if ((game_info.game_mode == GM_LINING_UP) || (game_info.game_mode == GM_PRE_SNAP))
			die(0);
		snd_scall_bank(ancr_bnk_str,GETS_THE_FIRST_DOWN_SP,VOLUME3,127,LVL4);
	}
	else	
		snd_scall_bank(generic_bnk_str,FIRST_DWN_SP,ANCR_VOL1,127,LVL2);
//		snd_scall_bank(ancr_bnk_str,FIRST_DWN_SP,VOLUME3,127,LVL4);
}
#endif

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void almost_touchdown_sp(obj_3d *pobj)
{
	if (game_info.game_mode == GM_PLAY_OVER)
		return;

	// player must run in from at least the 70 yard line
	if (game_info.los > 70)
		return;
	
	// not if player is being tackled.
	if (pobj->adata[0].animode & MODE_REACTING)
		return;
		
	// only on normal plays
	if (game_info.play_type != PT_NORMAL)
		return;
	
	// not if player is in the air
	if (game_info.game_flags & GF_AIR_CATCH)
		return;

	if ((FIELDX(pobj->x) == 70) && (said_30_sp != 1))
	{
		said_30_sp = 1;
		snd_scall_bank(ancr_swaped_bnk_str,HES_AT_THE_30_SP,ANCR_VOL2,127,LVL2);
	}
	else if ((FIELDX(pobj->x) == 80) && (said_20_sp != 1))
	{
		said_20_sp = 1;
		snd_scall_bank(ancr_swaped_bnk_str,THE_20_SP,ANCR_VOL2,127,LVL3);
	}
	else if ((FIELDX(pobj->x) == 90) && (said_10_sp != 1))
	{
		said_10_sp = 1;
		snd_scall_bank(ancr_swaped_bnk_str,THE_10_SP,ANCR_VOL2,127,LVL3);
	}
	else if ((FIELDX(pobj->x) == 95) && (said_5_sp != 1))
	{
		said_5_sp = 1;
		snd_scall_bank(ancr_swaped_bnk_str,THE_5_TD_SP,ANCR_VOL2,127,LVL4);
	}
}
	
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void disappointed_speech(void)
{
	snd_scall_bank(taunt_bnk_str,disappointed_sp[randrng(sizeof(disappointed_sp)/sizeof(int))],VOLUME4,127,PRIORITY_LVL3);
}
					
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void taunt_speech(obj_3d *pobj, int sp, int percent)
{

	if (sp == -1)
		snd_scall_bank(taunt_bnk_str,short_taunt_sp[randrng(sizeof(short_taunt_sp)/sizeof(int))],VOLUME4,127,PRIORITY_LVL3);
	else if (sp == -2)
		snd_scall_bank(taunt_bnk_str,long_taunt_sp[randrng(sizeof(long_taunt_sp)/sizeof(int))],VOLUME4,127,PRIORITY_LVL3);
	else if (sp == -3)
		snd_scall_bank(taunt_bnk_str,laugh_taunt_sp[randrng(sizeof(laugh_taunt_sp)/sizeof(int))],VOLUME4,127,PRIORITY_LVL3);
	else if (sp == -4)
	{
		if (randrng(100) < 10)
			snd_scall_bank(taunt_bnk_str,histep_taunt_sp[randrng(sizeof(histep_taunt_sp)/sizeof(int))],VOLUME4,127,PRIORITY_LVL4);
	}
	else if (sp == -5)
	{
		if (randrng(100) < 10)
			snd_scall_bank(taunt_bnk_str,11,VOLUME4,127,PRIORITY_LVL3);	// scream
	}
	else
		snd_scall_bank(taunt_bnk_str,sp,VOLUME4,127,PRIORITY_LVL3);
}
				
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
#if 0
void celebrate_speech(obj_3d *pobj, int sp)
{
	snd_scall_bank(taunt_bnk_str,celebrate_sp[randrng(sizeof(celebrate_sp)/sizeof(int))],VOLUME2,127,PRIORITY_LVL3);
}
#endif							
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void plyr_open_sp(obj_3d *pobj, int sp)
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	int	i;
	
	for (i=0; i < NUM_PLAYERS; i++)
	{
		// dont check myself
		if (ppdata->plyrnum == i)
			continue;
		
		// ignore if on same team
		if (ppdata->team == player_blocks[i].team)
			continue;
			
		if (distxz[ppdata->plyrnum][i]/YD2UN < 4.0f)
			break;
	}
	
	// this player is open!!!	
	if (i >= NUM_PLAYERS-1)
	{
		if (sp == -1)
			snd_scall_bank(plyr_bnk_str,im_open_sp[randrng(sizeof(im_open_sp)/sizeof(int))],VOLUME3,127,PRIORITY_LVL3);
		else	
			snd_scall_bank(plyr_bnk_str,sp,VOLUME3,127,PRIORITY_LVL3);
	}
}
	
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void monkey_on_back_sp(void)
{
	if (randrng(100) < 30)
		snd_scall_bank(taunt_bnk_str,38,VOLUME4,38,PRIORITY_LVL4);
	else
		snd_scall_bank(ancr_swaped_bnk_str,monkey_tackle_sp[randrng(sizeof(monkey_tackle_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
int on_fire_sp(int plyr)
{
	int	team;

	team = player_blocks[plyr].team;

	if (tdata_blocks[team].consecutive_comps == 2)
	{
	  	snd_scall_bank(ancr_swaped_bnk_str,180,ANCR_VOL2,127,LVL4);		// hes lighting it up
		return 1;
	}
	else if (tdata_blocks[team].consecutive_comps == OFFENSE_FIRECNT)
	{
		snd_scall_bank(ancr_swaped_bnk_str,181,ANCR_VOL2,127,LVL4);
		return 1;
	}
	else if (tdata_blocks[team].consecutive_comps >= OFFENSE_FIRECNT)
	{
		snd_scall_bank(ancr_swaped_bnk_str,catch_fire_sp[randrng(sizeof(catch_fire_sp)/sizeof(int))],ANCR_VOL2,127,LVL3);
		return 1;
	}
	else
		return 0;
}
							
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void hurdle_speech(void)
{
	if (randrng(100) < 25)
		snd_scall_bank(ancr_swaped_bnk_str,hurdle_sp[randrng(sizeof(hurdle_sp)/sizeof(int))],ANCR_VOL1,127,LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void spin_move_speech(void)
{
	if (randrng(100) < 35)
		snd_scall_bank(ancr_swaped_bnk_str,150,ANCR_VOL1,127,LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void gutsy_call_sp(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,gutsy_sp[randrng(sizeof(gutsy_sp)/sizeof(int))],ANCR_VOL1,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void stiff_arm_speech(void)
{
	if (randrng(100) < 35)
		snd_scall_bank(ancr_swaped_bnk_str,151,ANCR_VOL2,127,LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
#if 0
void stiff_arm_f_speech(void)
{
	if (randrng(100) < 35)
		snd_scall_bank(ancr_swaped_bnk_str,151,ANCR_VOL2,127,LVL3);		// THIS CAN BE REMOVED IF WE NEED TO!!!!!
//		snd_scall_bank(ancr_swaped_bnk_str,236,ANCR_VOL2,127,LVL3);
}
#endif
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void announce_eoq_speech(void)
{
	qcreate("eoq_sp",0,eoq_speech_proc, 0, 0, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void eoq_speech_proc (int *args)
{
int scr1,scr2,tm_won;

	sleep(5);	
	scr1 = compute_score(0);
	scr2 = compute_score(1);
	tm_won = 0;
	switch(game_info.game_quarter)
	{
		case EOQ_1:
			snd_scall_bank(ancr_bnk_str,END_OF_1ST_SP,ANCR_VOL2,127,LVL5);
			break;
		case EOQ_2:
			snd_scall_bank(ancr_bnk_str,WERE_AT_HALFTIME_SP,ANCR_VOL2,127,LVL5);
			break;
		case EOQ_3:
			snd_scall_bank(ancr_bnk_str,END_OF_3RD_SP,ANCR_VOL2,127,LVL5);
			break;
		case EOQ_4:
		case EOQ_OT1:
		case EOQ_OT2:
		case EOQ_OT3:
			if (scr1 == scr2)
			{
				if (game_info.game_quarter == (MAX_QUARTERS-1))
					snd_scall_bank(ancr_bnk_str,TIE_GAME_SP,ANCR_VOL2,127,LVL5);
				else		
				 	snd_scall_bank(ancr_bnk_str,WE_HAVE_OVERTIME_SP,ANCR_VOL2,127,LVL5);
				break;	
			}
			else
			{
				if (scr1 > scr2)
				{
					snd_scall_bank(team_bnk_ptrs[0],TEAM_NAME_SP,ANCR_VOL2,127,LVL5);
					tm_won = game_info.team[0];
				}
				else
				{
					snd_scall_bank(team_bnk_ptrs[1],TEAM_NAME_SP,ANCR_VOL2,127,LVL5);
					tm_won = game_info.team[1];
				}
				sleep(TCNV(team_name_sp_lengths[tm_won]));
				snd_scall_bank(ancr_bnk_str,WIN_THE_GAME_SP,ANCR_VOL2,127,LVL5);
				break;
			}
			break;	
	}
}

#if 0
#if DEBUG
//--------------------------------------------------------------------------------------------------------------------------
// debug_win_calls - Function is used to test the timing on the eoq_speech call when someone wins.
//--------------------------------------------------------------------------------------------------------------------------
extern void coin_sound_holdoff(int);

void debug_win_calls(int team)
{
	char city_bank[9];

	memset(city_bank, 0, sizeof(char) * 9);
	strncpy(city_bank, teaminfo[team].name, 8);
	// Load up team name bank
	coin_sound_holdoff(TRUE);
	if (snd_bank_load_playing(city_bank) == 0xeeee)
		return;
	coin_sound_holdoff(FALSE);

	// Make our sound calls
	snd_scall_bank(city_bank,TEAM_NAME_SP,ANCR_VOL2,127,LVL5);
	sleep(TCNV(team_name_sp_lengths[team]));
	snd_scall_bank(ancr_bnk_str,WIN_THE_GAME_SP,ANCR_VOL2,127,LVL5);

	// Blow away bank!
	snd_bank_delete(city_bank);
}
#endif
#endif

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void hit_intended_receiver_sp(void)
{
	qcreate("hitrecvr",0,hit_intended_receiver_sp_proc, 0, 0, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
void hit_intended_receiver_sp_proc(int *args)
{
	sleep(CONTROL_TIME+1);
	// if ball was somehow caught... dont say BROKEN UP... say CATCH speech... dramatic speech..
	if (game_info.ball_carrier != ball_obj.int_receiver)
		snd_scall_bank(ancr_swaped_bnk_str,hit_receiver_sp[randrng(sizeof(hit_receiver_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
//	else
//		snd_scall_bank(ancr_swaped_bnk_str,catch_long_sp[randrng(sizeof(catch_long_sp)/sizeof(int))],ANCR_VOL1,127,LVL2);
//		snd_scall_bank(ancr_swaped_bnk_str,91,ANCR_VOL2,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void safety_sp(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,ancr_safety_sp[randrng(sizeof(ancr_safety_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void blowout_sp(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,ancr_blowout_sp[randrng(sizeof(ancr_blowout_sp)/sizeof(int))],ANCR_VOL1,127,LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void qb_runs_past_los_sp(void)
{
	if (randrng(100) < 45)
		snd_scall_bank(ancr_swaped_bnk_str,qb_runs_sp[randrng(sizeof(qb_runs_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}


//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void qb_duck_speech(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,qb_duck_sp[randrng(sizeof(qb_duck_sp)/sizeof(int))],VOLUME4,127,PRIORITY_LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void clock_low_sp(void)
{
	int	snd,nbp_spch;

	nbp_spch = need_big_play_sp(); 

	if (!nbp_spch)
	{			
		snd = clock_sp[randrng(sizeof(clock_sp)/sizeof(int))];
		if (snd)
			snd_scall_bank(ancr_swaped_bnk_str,snd,ANCR_VOL1,127,LVL3);
	}
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
int need_big_play_sp(void)
{
	int off_scr,def_scr,diff;

	if (((game_info.game_time & 0x00FFFF00) <= 0x00000C00) &&
		(game_info.game_quarter == 1 || game_info.game_quarter >= 3))
	{	// under 12 seconds in half or game
		off_scr = compute_score(game_info.off_side);
		def_scr = compute_score(!game_info.off_side);
		
		diff = off_scr - def_scr;
		if (diff < 0)
			diff *= -1;

		if ((off_scr < def_scr) && (diff <= 7))
		{	//offense has ball and is losing by less than a touchdown
			snd_scall_bank(ancr_swaped_bnk_str,critical_sp[randrng(sizeof(critical_sp)/sizeof(int))],ANCR_VOL1,127,LVL3);
			return 1;		// made speech call
		}
	}
	return 0;				// didnt make speech call
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void intercepted_ball_sp(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,intercepted_sp[randrng(sizeof(intercepted_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void incomplete_pass_sp(void)
{

	if (ball_obj.type == LB_BOBBLE)
		snd_scall_bank(ancr_swaped_bnk_str,didnt_hold_ball_sp[randrng(sizeof(didnt_hold_ball_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
	else
		snd_scall_bank(ancr_swaped_bnk_str,incomplete_sp[randrng(sizeof(incomplete_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void deflected_ball_sp(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,deflected_sp[randrng(sizeof(deflected_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void fumble_ball_sounds(void)
{

	snd_scall_bank(plyr_bnk_str,INTERCEPT_SND,VOLUME2,127,PRIORITY_LVL4);
	snd_scall_bank(ancr_swaped_bnk_str,fumble_sp[randrng(sizeof(fumble_sp)/sizeof(int))],ANCR_VOL2,127,LVL5);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void bobble_ball_sounds(void)
{
	snd_scall_bank(plyr_bnk_str,P_MISS_BALL_SND,VOLUME2,127,PRIORITY_LVL4);
	snd_scall_bank(ancr_swaped_bnk_str,bobble_sp[randrng(sizeof(bobble_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}
	
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void head_plow_speech(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,head_plow_sp[randrng(sizeof(head_plow_sp)/sizeof(int))],ANCR_VOL1,127,LVL2);
}
			
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void idiot_speech(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,idiot_sp[randrng(sizeof(idiot_sp)/sizeof(int))],ANCR_VOL1,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void lateral_speech(void)
{
	snd_scall_bank(ancr_swaped_bnk_str,lateral_sp[randrng(sizeof(lateral_sp)/sizeof(int))],ANCR_VOL2,127,LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void late_hit_speech(void)
{
		
	if (!said_latehit_sp)
	{
		said_latehit_sp = 1;
		if (late_hit_sp_cnt >= (int)(sizeof(late_hit_sp)/sizeof(int)))
			late_hit_sp_cnt = 0;
		qcreate("lhit_sp",0,late_hit_speech_proc, 0, 0, 0, 0);
	}
}
		
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void late_hit_speech_proc(int *args)
{
	int	can_swear;
	
	sleep(20);		//12
	
	if ((game_info.game_mode == GM_LINING_UP) || (game_info.game_mode == GM_PRE_SNAP))
		die(0);
	
	get_adjustment(SWEAR_ADJ, &can_swear);
	if (can_swear)
		snd_scall_bank(ancr_swaped_bnk_str,late_hit_sp[late_hit_sp_cnt++],ANCR_VOL1,127,LVL3);
	else	
		snd_scall_bank(ancr_swaped_bnk_str,no_swear_late_hit_sp[late_hit_sp_cnt++],ANCR_VOL1,127,LVL3);
}
			
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void catch_sounds(obj_3d *pobj)
{
	int			i;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;

	// make catch sounds
	snd_scall_bank(plyr_bnk_str,P_CATCH_BALL_SND,VOLUME4,127,PRIORITY_LVL4);

	switch (ball_obj.type)
	{
		case LB_PUNT:
		case LB_KICKOFF:
		case LB_FIELD_GOAL:
			crowd_cheer_snds();
		case LB_LATERAL:
			break;
	
		case LB_PASS:
		case LB_BOBBLE:
			if (FIELDX(pobj->x) < game_info.los)
				return;
			qcreate("catch_sp",CATCH_SPEECH_PID,catch_speech_proc, ppdata->plyrnum, ppdata->team, 0, 0);
			break;
			
		case LB_FUMBLE:
			snd_scall_bank(team_bnk_ptrs[game_info.off_side],CITY_NAME_SP,ANCR_VOL2,127,LVL3);		// say city name
			break;
	}
}
		
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void catch_speech_proc(int *args)
{	
	int			pnum,fire_sp;
	const struct st_plyr_data *pspd;


	pnum = args[0];
	
	sleep(CONTROL_TIME);
	
	// if after X ticks...players doesnt have ball (ie..bobbled..etc) no speech
	if (player_blocks[pnum].plyrnum != game_info.ball_carrier)
	{
		if (randrng(100) < 30)
			snd_scall_bank(generic_bnk_str,CRWD_BOO1_SND,VOLUME3,127,PRIORITY_LVL2);
		die(0);
	}
	
	crowd_cheer_snds();

	fire_sp = on_fire_sp(pnum);
	if (fire_sp)
		die(0);

	if (ball_obj.type == LB_BOBBLE)
	{
		snd_scall_bank(ancr_swaped_bnk_str,catch_bobble_sp[randrng(sizeof(catch_bobble_sp)/sizeof(int))],ANCR_VOL1,127,LVL2);
	}
	else if (randrng(100) < 45)
	{
		// say WEAPON name
		pspd = teaminfo[game_info.team[game_info.off_side]].static_data;
		pspd += game_info.ball_carrier % 7;
		if (pspd->ancr != 0)
		{
			snd_scall_bank(team_bnk_ptrs[game_info.off_side],pspd->ancr,VOLUME3,127,LVL3);
			die(0);
		}
	}
	else if ((FIELDX(player_blocks[pnum].odata.x) - game_info.los) < 25.0f)
		snd_scall_bank(ancr_swaped_bnk_str,catch_short_sp[randrng(sizeof(catch_short_sp)/sizeof(int))],ANCR_VOL1,127,LVL2);
	else
		snd_scall_bank(ancr_swaped_bnk_str,catch_long_sp[randrng(sizeof(catch_long_sp)/sizeof(int))],ANCR_VOL1,127,LVL2);
	
	// just keep this process alive...for the above average length sound call
	sleep(90);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void throw_ball_sounds(void)
{
	int		i,dist;
	const struct st_plyr_data *pspd;

	if (ball_obj.type == LB_LATERAL)
	{
		snd_scall_bank(plyr_bnk_str,P_LATERAL_BALL_SND,VOLUME2,127,PRIORITY_LVL3);
		return;
	}

	// throw ball sound
	if (ball_obj.flags & BF_FIRE_PASS)
		snd_scall_bank(plyr_bnk_str,P_THRW_BALL_FIRE_SND,VOLUME3,127,PRIORITY_LVL3);
	else	
		snd_scall_bank(plyr_bnk_str,throw_ball_snds[randrng(sizeof(throw_ball_snds)/sizeof(int))],VOLUME3,127,PRIORITY_LVL3);
	
	// no speech if throwing to someone behind line-of-scrimage
	if (FIELDX(ball_obj.tx) < game_info.los)
		return;
	
//	dist = 	FIELDX(ball_obj.tx) - FIELDX(ball_obj.ox);
	dist = 	FIELDX(ball_obj.tx) - game_info.los;

	if (dist < 10)
		snd_scall_bank(ancr_swaped_bnk_str,throw_short_sp[randrng(sizeof(throw_short_sp)/sizeof(int))],ANCR_VOL2,127,LVL1);
	else if (dist < 25)
	{
		if (player_blocks[game_info.off_side ? 10 : 3].odata.adata[0].seq_index == (SEQ_HURDLE))
			snd_scall_bank(ancr_swaped_bnk_str,throw_jump_sp[randrng(sizeof(throw_jump_sp)/sizeof(int))],ANCR_VOL2,127,LVL1);
	
		else if (randrng(100) < 25)
		{
			// say TO WEAPON... (non-excited)
			pspd = teaminfo[game_info.team[game_info.off_side]].static_data;
			pspd += ball_obj.int_receiver % 7;
			snd_scall_bank(team_bnk_ptrs[game_info.off_side],pspd->ancr+3,ANCR_VOL2,127,LVL1);
		}
		else
			snd_scall_bank(ancr_swaped_bnk_str,throw_med_sp[randrng(sizeof(throw_med_sp)/sizeof(int))],ANCR_VOL2,127,LVL1);
	}
	else
	{
		if (randrng(100) < 10)
		{
			// say TO WEAPON... (non-excited)
			pspd = teaminfo[game_info.team[game_info.off_side]].static_data;
			pspd += ball_obj.int_receiver % 7;
			snd_scall_bank(team_bnk_ptrs[game_info.off_side],pspd->ancr+7,ANCR_VOL2,127,LVL1);
		}
		else
			snd_scall_bank(ancr_swaped_bnk_str,throw_long_sp[randrng(sizeof(throw_long_sp)/sizeof(int))],ANCR_VOL2,127,LVL1);
	}	
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void qb_actions_sp(int *args)
{
	int		pnum,talk_time,move_down_cnt,move_left_cnt,move_right_cnt;
	int		say_fade,say_left,say_right;

	fbplyr_data *ppdata;
	
	pnum = args[0];
	talk_time = 35;
	ppdata = player_blocks + pnum;
	move_down_cnt = 0;
	move_left_cnt = 0;
	move_right_cnt = 0;
	say_fade = randrng(80) + 30;
	say_left = randrng(65) + 20;
	say_right = randrng(65) + 20;


	// no talking if punt or field goal!!!	
	if ((game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL) ||
		(game_info.team_play[game_info.off_side]->flags & PLF_PUNT))
		die(0);
	
	// announce qb name
	while ((ppdata->odata.plyrmode == MODE_QB) &&
		   ((game_info.ball_carrier != -1) || (game_info.game_flags & GF_HIKING_BALL)) &&
		   (game_info.game_mode != GM_PLAY_OVER))
	{
		sleep(1);
		// moving down ?
		if (ppdata->stick_cur > 34)
		{
			move_down_cnt++;
			if ((move_down_cnt == say_fade) && (randrng(100) < 45))
				snd_scall_bank(ancr_swaped_bnk_str,fades_back_sp[randrng(sizeof(fades_back_sp)/sizeof(int))],ANCR_VOL1,127,LVL1);
		}
		else if (ppdata->stick_cur == 21)
		{
			move_left_cnt++;
			if ((move_left_cnt == say_left) && (randrng(100) < 45))
				snd_scall_bank(ancr_swaped_bnk_str,25,ANCR_VOL1,127,LVL1); 		// "rolls left"
		}
		else if (ppdata->stick_cur == 27)
		{
			move_right_cnt++;
			if ((move_right_cnt == say_right) && (randrng(100) < 45))
				snd_scall_bank(ancr_swaped_bnk_str,26,ANCR_VOL1,127,LVL1); 		// "rolls right"
		}
		else if (--talk_time <= 0)
		{
			snd_scall_bank(team_bnk_ptrs[game_info.off_side],QB_NAME_SP,ANCR_VOL1,127,LVL0);		// just say QB's name
			talk_time = 160;
		}	
	}
}
							 
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void pre_snap_speech_proc(int *args)
{
	int i = 0;
	int	off_scr = 0;
	int	def_scr = 0;
	int	scr_dif	= 0;
	int	rnd;
	
	rnd = randrng(100);
	
	
	sleep(1);

	if (game_info.game_flags & GF_QUICK_KICK)
	{
		if (randrng(100) < 40)
			die(0);
		if (randrng(100) < 70)
		{
			snd_scall_bank(ancr_swaped_bnk_str,86,ANCR_VOL2,127,LVL3);
			die(0);
		}
		else
		{		
			snd_scall_bank(team_bnk_ptrs[game_info.off_side],CITY_NAME_SP,ANCR_VOL2,127,LVL4);		// say city name
			sleep(TCNV(team_name_sp_lengths[game_info.team[game_info.off_side]]));
			snd_scall_bank(ancr_bnk_str,184,ANCR_VOL2,127,LVL4);
			die(0);
		}
	}

	// going for field goal ?
	if (game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL)
	{
		snd_scall_bank(ancr_swaped_bnk_str,LINE_UP_FOR_FG_SP,ANCR_VOL1,127,LVL2);
		die(0);
	}
	
	// going for it on 4th down ?
	if ((game_info.down == 4) &&
		(!(game_info.team_play[game_info.off_side]->flags & PLF_FAKE_PUNT)) &&
		(!(game_info.team_play[game_info.off_side]->flags & PLF_PUNT)) &&
		(!(game_info.team_play[game_info.off_side]->flags & PLF_FAKE_FG)) &&
		(!(game_info.team_play[game_info.off_side]->flags & PLF_FIELD_GOAL)))
	{
		if (game_info.los < 20)
			snd_scall_bank(ancr_bnk_str,DEEP_IN_TERRITORY_SP,ANCR_VOL1,127,LVL2);
		else
			gutsy_call_sp();
	 	die(0);
	}

	// going for 2 points ?
	if (game_info.play_type == PT_EXTRA_POINT)
	{
		if (rnd < 55) 
			snd_scall_bank(ancr_swaped_bnk_str,GOING_FOR_2_SP,ANCR_VOL1,127,LVL1);
		else	
			snd_scall_bank(ancr_swaped_bnk_str,GOING_FOR_IT_SP,ANCR_VOL1,127,LVL1);
	 	die(0);
	}

	// say X and goal speech ?
	if ((game_info.first_down >= 99) && (game_info.los > 85))
	{
		if ((game_info.down == 2) && (rnd < 35))
			snd_scall_bank(ancr_bnk_str,132,ANCR_VOL1,127,LVL2);
		else
			snd_scall_bank(ancr_bnk_str,down_n_goal_sp[game_info.down-1],ANCR_VOL1,127,LVL1);
	 	die(0);
	}

	// say LOS position
	if ((game_info.play_type == PT_NORMAL) && (last_los != game_info.los))
	{
		last_los = game_info.los;
		if (game_info.los == 5)
		{
			snd_scall_bank(ancr_bnk_str,190,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 10)
		{
			snd_scall_bank(ancr_bnk_str,191,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 15)
		{
			snd_scall_bank(ancr_bnk_str,192,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 20)
		{
			snd_scall_bank(ancr_bnk_str,193,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 25){
			snd_scall_bank(ancr_bnk_str,194,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 30)
		{
			snd_scall_bank(ancr_bnk_str,195,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 35)
		{
			snd_scall_bank(ancr_bnk_str,196,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 40)
		{
			snd_scall_bank(ancr_bnk_str,197,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 45)
		{
			snd_scall_bank(ancr_bnk_str,198,ANCR_VOL1,127,LVL2);
			die(0);
		}
		else if (game_info.los == 50)
		{
			snd_scall_bank(ancr_bnk_str,199,ANCR_VOL1,127,LVL2);
			die(0);
		}

		// say 1st down ?
		if ((game_info.first_down - game_info.los == 30) && (game_info.down == 1))
		{
			if (randrng(100) < 60) 
				snd_scall_bank(generic_bnk_str,FIRST_AND_30_SP,ANCR_VOL1,127,LVL2);
//				snd_scall_bank(ancr_bnk_str,FIRST_AND_30_SP,ANCR_VOL1,127,LVL1);
			else
				snd_scall_bank(generic_bnk_str,FIRST_DWN_SP,ANCR_VOL1,127,LVL2);
//				snd_scall_bank(ancr_bnk_str,FIRST_DWN_SP,VOLUME3,127,LVL1);
		 	die(0);
		}

		if (rnd < 60)
		{
			// announce down and length ?
//			if (game_info.first_down - game_info.los > 25)
//				snd_scall_bank(ancr_bnk_str,down_long_sp[game_info.down-1],ANCR_VOL1,127,LVL1);
//			else if (game_info.first_down - game_info.los < 8)
//				snd_scall_bank(ancr_bnk_str,down_shrt_sp[game_info.down-1],ANCR_VOL1,127,LVL1);
//			else	
			snd_scall_bank(generic_bnk_str,down_sp[game_info.down-1],ANCR_VOL1,127,LVL1);
//				snd_scall_bank(ancr_bnk_str,down_sp[game_info.down-1],VOLUME3,127,LVL1);
		 	die(0);
		}
		else if (rnd < 90)
			die(0);					// say nothing
		else if (rnd < 100)
		{
			// team scores
//			said_scr_sp = 1;
			off_scr = compute_score(game_info.off_side);
			def_scr = compute_score(!game_info.off_side);
			scr_dif = abs(off_scr - def_scr);
			if (scr_dif == 0)
			{
			 	if (off_scr != 0)			// tie game...and not 0-0
			   		snd_scall_bank(ancr_bnk_str,TIE_GAME_SP,ANCR_VOL1,127,LVL1);
				die(0);
			}
			if (scr_dif > 28)
			{
				blowout_sp();
				die(0);
			}
			
			snd_scall_bank(team_bnk_ptrs[game_info.off_side],CITY_NAME_SP,ANCR_VOL1,127,LVL3);		// say city name
			sleep(TCNV(team_city_sp_lengths[game_info.team[game_info.off_side]]));

			if ((game_info.game_flags & GF_BALL_CROSS_LOS) ||
				(game_info.game_flags & GF_PLAY_ENDING))
				die(0);

			if (compute_score(game_info.off_side) > compute_score(!game_info.off_side))
			{
				if (leads_by_sp[scr_dif] != 0)
					snd_scall_bank(ancr_bnk_str,leads_by_sp[scr_dif],ANCR_VOL1,127,LVL3);
			}
			else
			{
				if (down_by_sp[scr_dif] != 0)
					snd_scall_bank(ancr_bnk_str,down_by_sp[scr_dif],ANCR_VOL1,127,LVL3);
			}
		}
	}
}


//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the announcer speech call specified or if -1 then a random sound
//
// NOTE: 'TK.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//		   
//
//--------------------------------------------------------------------------------------------------------------------------
void tackle_spch(obj_3d *pobj, int strength, int snd1)
{
	int			pt,pa,i;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	
	// no speech after play is over
//	if (game_info.game_mode == GM_PLAY_OVER)
//		return;
	
	// no tackle speech...if not ball carrier
	if (ppdata->plyrnum != game_info.ball_carrier)
		return;
	
	// qb sacked speech ?	
	if (pobj->plyrmode == MODE_QB)
	{
		qcreate("sacked",0,sacked_sp_proc, 0, 0, 0, 0);
		return;
	}

	// yards gained....speech	
//	if ((randrng(100) < 20) && (!(game_info.game_flags & GF_POSS_CHANGE)))
//	{
//		qcreate("yrd_sp",0,yards_gained_sp_proc, ppdata->plyrnum, 0, 0, 0);
//		return;	
//	}
	
	// passed in sound number ?	
	if (snd1 > 0)
	{
		qcreate("tackle",0,tackled_sp_proc, snd1, 0, 0, 0);
		return;	
	}
	
	qcreate("tackle",0,tackled2_sp_proc, strength, 0, 0, 0);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//static void yards_gained_sp_proc(int *args)
//{
//	int yrds, pnum;
//
//	pnum = args[0];
//
//	if (game_info.play_type != PT_NORMAL)
//		die(0);
//		
//	while (existp(CATCH_SPEECH_PID,0xFFFFFFFF))
//		sleep(1);
//	
//	// waited too long ?
//	if (game_info.game_mode != GM_PLAY_OVER)
//		die(0);
//	
//	yrds = game_info.los - game_info.old_los;
////	yrds = FIELDX(player_blocks[pnum].odata.x) - game_info.old_los;
//	if ((yrds < 5) &&
//		!(game_info.game_flags & GF_POSS_CHANGE) &&
//		(game_info.off_turnover == 0) &&
//		(game_info.just_got_1st != 1))
//	{
//		// goes nowhere speech
//		snd_scall_bank(ancr_swaped_bnk_str,131,ANCR_VOL1,127,LVL3);
//		die(0);
//	}
//	else if ((yrds > 30) && (!(game_info.game_flags & GF_POSS_CHANGE)) &&
//			 (game_info.just_got_1st != 1) && (game_info.off_turnover == 0))
//	{
//		snd_scall_bank(ancr_bnk_str,211,ANCR_VOL1,127,LVL3);
//		die(0);
//	}
//	else
//	{	
//		if (yards_gained_sp[yrds] != 0)
//			snd_scall_bank(ancr_bnk_str,yards_gained_sp[yrds],ANCR_VOL1,127,LVL3);
//	}
//}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
static void sacked_sp_proc(int *args)
{
	sleep(5);
	snd_scall_bank(ancr_swaped_bnk_str,qb_sacked_sp[randrng(sizeof(qb_sacked_sp)/sizeof(int))],ANCR_VOL2,127,LVL4);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
static void tackled_sp_proc(int *args)
{
	int snd1;
	
	snd1 = args[0];

	sleep(7);
	
	while (existp(CATCH_SPEECH_PID,0xFFFFFFFF))
		sleep(1);
	
//	// if get tackled near LOS...
//	if ((game_info.game_flags & GF_BALL_CROSS_LOS) &&
//		(FIELDX(ball_obj.odata.x) <= game_info.los+1))
//	{
//		snd_scall_bank(ancr_bnk_str,133,ANCR_VOL2,127,LVL3);		// says "goes nowhere"
//	}

	// waited too long ?
	if (game_info.game_mode != GM_PLAY_OVER)
		die(0);
	
	snd_scall_bank(ancr_bnk_str,snd1,ANCR_VOL2,127,LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
static void tackled2_sp_proc(int *args)
{
	int strength;
	strength = args[0];

	while (existp(CATCH_SPEECH_PID,0xFFFFFFFF))
		sleep(1);
	sleep(10);
	
	// waited too long ?
	if (game_info.game_mode != GM_PLAY_OVER)
		die(0);
	
//	// if get tackled near LOS...
//	if ((game_info.game_flags & GF_BALL_CROSS_LOS) &&
//		(FIELDX(ball_obj.odata.x) <= game_info.los+1))
//	{
//		snd_scall_bank(ancr_bnk_str,133,ANCR_VOL2,127,LVL3);		// says "goes nowhere"
//	}

	// play random speech within group
	if (strength == HS_WEAK)
		snd_scall_bank(ancr_swaped_bnk_str,weak_tackle_sp[randrng(sizeof(weak_tackle_sp)/sizeof(int))],ANCR_VOL2,127,LVL3);
	else if (strength == HS_HARD)
		snd_scall_bank(ancr_swaped_bnk_str,hard_tackle_sp[randrng(sizeof(hard_tackle_sp)/sizeof(int))],ANCR_VOL2,127,LVL3);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void crowd_cheer_snds(void)
{
	snd_scall_bank(generic_bnk_str,crowd_cheers_snds[randrng(sizeof(crowd_cheers_snds)/sizeof(int))],VOLUME3,127,PRIORITY_LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void crowd_tackle_snds(void)
{
	snd_scall_bank(generic_bnk_str,
					tackle_crowd_cheers_snds[randrng(sizeof(tackle_crowd_cheers_snds)/sizeof(int))],
					VOLUME3,
					127,
					PRIORITY_LVL2);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void get_hit_snd(obj_3d *pobj, int snd1, int snd2, int snd3)
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	int priority;

	// If a lineman, and am off screen, no sound
	if (!(pobj->flags & PF_ONSCREEN))
		return;
		
//	if (!(pobj->flags & PF_ONSCREEN) && (pobj->plyrmode == MODE_LINE))
//		return;

	// if both players are linemen...softer sounds
	if ((pobj->plyrmode == MODE_LINE) && (pobj->pobj_hit->plyrmode == MODE_LINE))
		snd_scall_bank(plyr_bnk_str,linemen_hit_snds[randrng(sizeof(linemen_hit_snds)/sizeof(int))],VOLUME2,127,PRIORITY_LVL1);
		
	// Louder volume if ball carrier
//	if ((ppdata->plyrnum == game_info.ball_carrier) ||
//		(ppdata->plyrnum == ball_obj.int_receiver) ||
//		(!ISDRONE(ppdata)) ||
//		(!ISDRONE((fbplyr_data *)(pobj->pobj_hit))))
  	if ((ppdata->plyrnum == game_info.ball_carrier) || (ppdata->plyrnum == ball_obj.int_receiver))
//	if ((!ISDRONE(ppdata)) || (!ISDRONE((fbplyr_data *)(pobj->pobj_hit))))
	{
		// hit sound to a human player
		if (snd1 < 0)
			snd_scall_bank(plyr_bnk_str,carrier_hit_snds[randrng(sizeof(carrier_hit_snds)/sizeof(int))],VOLUME4,127,PRIORITY_LVL4);
		else if (snd1 > 0)
			snd_scall_bank(plyr_bnk_str,snd1,VOLUME4,127,PRIORITY_LVL4);
		
		// fly thru air sounds		
		if (snd2 < 0)
			snd_scall_bank(plyr_bnk_str,fly_thru_air_snds[randrng(sizeof(fly_thru_air_snds)/sizeof(int))],VOLUME2,127,PRIORITY_LVL2);
		else if (snd2 > 0)
			snd_scall_bank(plyr_bnk_str,snd2,VOLUME2,127,PRIORITY_LVL2);
		
		// just play passed in sound #3
		if (snd3 > 0)
			snd_scall_bank(plyr_bnk_str,snd3,VOLUME4,127,PRIORITY_LVL2);
	}
	else
	{	
		// hit sound to a non-human
		if (snd1 < 0)
		{
//			if (ppdata->plyrnum == ball_obj.int_receiver)
//				snd_scall_bank(plyr_bnk_str,carrier_hit_snds[randrng(sizeof(carrier_hit_snds)/sizeof(int))],VOLUME4,127,PRIORITY_LVL3);
//			else
				snd_scall_bank(plyr_bnk_str,non_carrier_hit_snds[randrng(sizeof(non_carrier_hit_snds)/sizeof(int))],VOLUME3,127,PRIORITY_LVL2);
		}
		else if (snd1 > 0)
			snd_scall_bank(plyr_bnk_str,snd1,VOLUME3,127,PRIORITY_LVL3);
			
		// fly thru air sounds		
		if (snd2 < 0)
			snd_scall_bank(plyr_bnk_str,fly_thru_air_snds[randrng(sizeof(fly_thru_air_snds)/sizeof(int))],VOLUME2,127,PRIORITY_LVL1);
		else if (snd2 > 0)
			snd_scall_bank(plyr_bnk_str,snd2,VOLUME2,127,PRIORITY_LVL1);
	
		// just play passed in sound #3
		if (snd3 > 0)
			snd_scall_bank(plyr_bnk_str,snd3,VOLUME4,127,PRIORITY_LVL1);
	}	
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void grnd_hit_snd(obj_3d *pobj, int snd1, int snd2, int snd3)
{
	int			 priority,priority2,volume;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	

	// Louder volume if ball carrier, intended receiver, or a human
//	if ((ppdata->plyrnum == game_info.ball_carrier) ||
//			(ppdata->plyrnum == ball_obj.int_receiver) ||
//			(!ISDRONE(ppdata)) ||
//			(pobj->pobj_hit && !ISDRONE((fbplyr_data *)(pobj->pobj_hit))))
	if ((!ISDRONE(ppdata)) || (pobj->pobj_hit && !ISDRONE((fbplyr_data *)(pobj->pobj_hit))))
	{
		priority = PRIORITY_LVL4;
		priority2 = PRIORITY_LVL3;
		volume = VOLUME4;
	}
	else	
	{
		priority = PRIORITY_LVL2;
		priority2 = PRIORITY_LVL2;
		volume = VOLUME2;
	}

	// ground hit sounds
	if (snd1 < 0)
		snd_scall_bank(plyr_bnk_str,grnd_hit_snds[randrng(sizeof(grnd_hit_snds)/sizeof(int))],volume,127,priority);
	else if (snd1 > 0)
		snd_scall_bank(plyr_bnk_str,snd1,volume,127,priority);
		
	// neck cracking sound
	if (snd2 < 0)
		snd_scall_bank(plyr_bnk_str,neck_crack_snds[randrng(sizeof(neck_crack_snds)/sizeof(int))],volume,127,priority2);
	else if (snd2 > 0)	
		snd_scall_bank(plyr_bnk_str,snd2,volume,127,priority2);
	
	// player land grunt sounds
	if (snd3 < 0)
		snd_scall_bank(plyr_bnk_str,plr_land_grunt_snds[randrng(sizeof(plr_land_grunt_snds)/sizeof(int))],volume,127,PRIORITY_LVL1);
	else if (snd3 > 0)
		snd_scall_bank(plyr_bnk_str,snd3,volume,127,PRIORITY_LVL1);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void attack_snd(obj_3d *pobj, int snd1, int snd2, int snd3)
{
	int			 priority;
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	
//	if (ppdata->plyrnum == game_info.plyr_control[ppdata->team])
// 4PC-JMS-Untested
	if (ISHUMAN(ppdata))
		priority = PRIORITY_LVL2;
	else
		priority = PRIORITY_LVL1;
	
	if (snd1 < 0)
		snd_scall_bank(plyr_bnk_str,plr_attack_snds[randrng(sizeof(plr_attack_snds)/sizeof(int))],VOLUME2,127,priority);
	else if (snd1 > 0)
		snd_scall_bank(plyr_bnk_str,snd1,VOLUME2,127,priority);
			
	if (snd2 > 0)
		snd_scall_bank(plyr_bnk_str,snd2,VOLUME2,127,priority);
	if (snd3 > 0)
		snd_scall_bank(plyr_bnk_str,snd3,VOLUME2,127,priority);
}

//--------------------------------------------------------------------------------------------------------------------------
//				This ANI_CODE routine plays the sound call specified or if -1 then a random hut sound
//
// NOTE: 'PLAYERS.BNK' is assumed
//
// INPUT: -1 = play random else sound call number
//--------------------------------------------------------------------------------------------------------------------------
void hut_snd(obj_3d *pobj, int snd1, int snd2, int snd3)
{
//	if (!(game_info.audible == 2))
//	{	
	if (snd1 == -1)
		snd_scall_bank(plyr_bnk_str,hut_snds[randrng(sizeof(hut_snds)/sizeof(int))],VOLUME2,127,PRIORITY_LVL1);
	else if (snd1 == -2)
		snd_scall_bank(plyr_bnk_str,qb_point_snds[randrng(sizeof(qb_point_snds)/sizeof(int))],VOLUME2,127,PRIORITY_LVL1);
	else
		snd_scall_bank(plyr_bnk_str,snd1,VOLUME2,127,PRIORITY_LVL1);
//	}	
}

//--------------------------------------------------------------------------------------------------------------------------
