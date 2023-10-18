/****************************************************************************/
/*                                                                          */
/* Detect.c - player-to-player detection                                    */
/*                                                                          */
/* Written by:  DJT                                                         */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <goose/goose.h>

#include "/Video/Nfl/Include/Anim.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/pproto.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/sndcalls.h"
#include "/Video/Nfl/Include/handicap.h"
#include "/Video/Nfl/Include/profile.h"
#include "/Video/Nfl/Include/debris.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/PInfo.h"
#include "/Video/Nfl/Include/CamBot.h"
#include "/Video/Nfl/Include/CamData.h"
#include "/Video/Nfl/Include/PMisc.h"
#include "/Video/Nfl/Include/Sounds.h"
#include "/Video/Nfl/Include/Detect.h"
#include "/Video/Nfl/Include/cap.h"

#include "/Video/Nfl/Include/plyrseq.h"				// anim sequences
#include "/Video/Nfl/Anim/genseq.h"					// anim sequences #defines

/*
** Local data/prototypes
*/
static float line_int_check( float *, float *, float *, float *, float );
static float point2line_d( float *, float *, float  * );
static void do_collision( fbplyr_data *, fbplyr_data *, int );
static void extend_line( float *, float *, float, float, float *, float * );

static int evade_vs_tackle( fbplyr_data *, fbplyr_data *, int, int );
static int block_vs_rush( fbplyr_data *, fbplyr_data *, int, int );
static int fumble_or_bobble( fbplyr_data *, fbplyr_data *, int, int );

static debris_config_t shards_data;

#if 0
static char *smoke_textures[] = {
				"TRAIL1.WMS",
				"TRAIL2.WMS",
				"TRAIL3.WMS",
				"TRAIL4.WMS",
				"TRAIL5.WMS",
				"TRAIL6.WMS",
				"TRAIL7.WMS",
				"TRAIL8.WMS",
				"TRAIL9.WMS",
				"TRAIL10.WMS",
				"TRAIL11.WMS",
				"TRAIL12.WMS",
				"TRAIL13.WMS",
				"TRAIL14.WMS",
				"TRAIL15.WMS",
				"TRAIL16.WMS",
				"TRAIL17.WMS" };
#endif

// player to player distance/angle table
float	distxz[NUM_PLAYERS][NUM_PLAYERS];
int		p2p_angles[NUM_PLAYERS][NUM_PLAYERS];
int		abusing_spin = 0;
int		victim_seq;		// sequence victim was running when he got hit
float	victim_y;		// victim Y at the instant he got hit
float	mvx,mvz;		// average velocity of two guys involved in a collision (before change_anims)

int		qb_evaded;		// flag to signal wheter QB has done BITCH slap or DUCK move already

/*
** Definitions
*/
#ifdef JASON
//#define BLOCK_DEBUG
//#define HIT_DEBUG
#endif

#define PASS1_DIST		24.0f				// do no checking unless closer than this
#define BODY_RADIUS		5.0f				// body cylinder radius

//   Z forward 
//
//.......0.......
//.......|.......
//...1...|...7...	inside circle is dir1
//....\..0../....	outside circle is dir2
//......\|/......
//2----1-*-3----6
//....../|\......
//..../..2..\....
//...3...|...5...
//.......|.......
//.......4.......
typedef void (*cfunc)( fbplyr_data *, fbplyr_data *, int, int );

static void att_none( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_push( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_kick( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_dive( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_late_hit( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_spear_head( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_swipe( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_head( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
static void att_hurdle( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 );
void delay_plow_proc(int *args);

cfunc cfuncs[] = { att_none, att_push, att_dive, att_kick, att_late_hit, att_spear_head, att_swipe, att_head, att_hurdle };

// flail seqs.
// !!!FIX.. need probibilites on these!!
static char *flail_anim_tbl[] =
{
	h_flailm_anim,
	r_flaild_anim,
	h_flailm_anim
};

static char *stiff_arm_flail_tbl[] =
{
	h_stiff_arm_react_anim,
	h_flailm_anim,
	h_stiff_arm_react2_anim,
	h_stiff_arm_react3_anim,
	h_flailm_anim
};

// Table used on ball carrier
static char *flail_carrier_anim_tbl[] =
{
	r_flaild_carrier_anim,
	h_flailm_carrier_anim
};

// Table used on ball carrier
static char *stumble_anim_tbl[] =
{
	r_stmbl_anim,
	r_flailnd_anim,
};


//
// NON-TURBO tackle animation tables
//
static char *std_hit_frnt_tbl[] = {
	h_spin_anim,
	h_falnk1_anim,
	h_front1_anim,
	h_front4_anim,
	h_bsidef_anim,
	fall_10,
	fall_12,
	NULL 
};

static char *std_hit_side_tbl[] = {
	h_spin_anim,
	h_back5_anim,
	h_falnk1_anim,
	h_back6_anim,
	h_front4_anim,
	h_bsidel_anim,
	fall_04,
	fall_10,
	NULL 
};

static char *std_hit_back_tbl[] = {
	h_back_anim,
	h_back1_anim,
	h_back5_anim,
	h_back2_anim,
	h_back4_anim,
	h_back5_anim,
	h_back6_anim,
//#ifndef SEATTLE
	fall_13,
	fall_16,
//#endif
	fall_18,
	NULL 
};

static char *std_hit_sidef_tbl[] = {
	h_back5_anim,
	h_back6_anim,
	h_back_anim,
	h_spinf_anim,
	h_falnk1_anim,
	h_front4_anim,
	h_bsider_anim,
	fall_04f,
	fall_10,
	NULL 
};


//
// TURBO tackle animation tables
//
static char *trb_hit_frnt_tbl[] = {
	h_side1_anim,
	h_falnk1_anim,
	h_spin_anim,
	h_blindl_anim,
	h_bsidef_anim,
	h_front4_anim,
	h_front5_anim,
	h_heli1_anim,
	h_blindl_anim,
	fall_10,
	fall_12,
	NULL 
};

static char *trb_hit_side_tbl[] = {
	h_spinf_anim,
	h_blindl_anim,
	h_bsidel_anim,
	fall_04,
	fall_10,
	NULL 
};

static char *trb_hit_back_tbl[] = {
	h_back6_anim,
	h_back5_anim,
	h_spinf_anim,
//#ifndef SEATTLE
	fall_13,
	fall_16,
//#endif
	fall_18,
	NULL 
};

static char *trb_hit_sidef_tbl[] = {
	h_spin_anim,
	h_blindr_anim,
	h_bsider_anim,
	fall_04f,
	fall_10,
	NULL 
};

// hit player who is dragging player reactions
static char *drag_hit_tbl[] = {
	h_side1_anim,
	h_falnk1_anim,
	h_blindl_anim,
	h_bsidel_anim,
	h_bsider_anim, 
	h_bsidef_anim 
};


//
// PUPPET-MOVE tackle animation tables (NFL_PUSSY_MODE tables)
//
static char *puppet_frnt_tbl[] = {
//	p_drag_arm_a_anim,	p_drag_arm_v_anim,
	p_clothes_line_a_anim, p_clothes_line_v_anim,
	p_flip_a_anim,		p_flip_v_anim,
	p_ftac_a_anim,		p_ftac_v_anim,
//#ifndef SEATTLE
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//#endif
	p_tkdn_a_anim,		p_tkdn_v_anim,
	p_thrw_a_anim,		p_thrw_v_anim,
	p_frol_a_anim,		p_frol_v_anim,
	p_flip_a_anim,		p_flip_v_anim,
	p_clothes_line_a_anim, p_clothes_line_v_anim,
	p_ftac_a_anim,		p_ftac_v_anim,
	p_fslm_a_anim,		p_fslm_v_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_armg3_a_anim,		p_armg3_v_anim,
	NULL,				NULL };

static char *puppet_side_tbl[] = {
	p_hip_toss_a_anim,	p_hip_toss_v_anim,
	p_flip_a_anim,		p_flip_v_anim,
	p_csid_a_anim,		p_csid_v_anim,
	p_tkdn_a_anim,		p_tkdn_v_anim,
	p_thrw_a_anim,		p_thrw_v_anim,
	p_flip_a_anim,		p_flip_v_anim,
	p_grab_jersey_a_anim,p_grab_jersey_v_anim,
	p_fslm_a_anim,		p_fslm_v_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_flip_a_anim,		p_flip_v_anim,
//#ifndef SEATTLE
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//#endif
	p_armg_a_anim,		p_armg_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	p_hip_toss_a_anim,	p_hip_toss_v_anim,
	p_armg2_a_anim,		p_armg2_v_anim,
	p_armg3_a_anim,		p_armg3_v_anim,
	NULL,				NULL };

static char *puppet_back_tbl[] = {
	p_suplb_a_anim,		p_suplb_v_anim,
	p_grab_jersey_a_anim,p_grab_jersey_v_anim,
	p_hip_toss_a_anim,	p_hip_toss_v_anim,
	p_csid_a_anim,		p_csid_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	p_tkdn_a_anim,		p_tkdn_v_anim,
	p_grab_jersey_a_anim,p_grab_jersey_v_anim,
	p_hip_toss_a_anim,	p_hip_toss_v_anim,
//#ifndef SEATTLE
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//#endif
	p_hip_toss_a_anim,	p_hip_toss_v_anim,
	p_btac_a_anim,		p_btac_v_anim,
//	p_thrw_a_anim,		p_thrw_v_anim,
//	p_spin_a_anim,		p_spin_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	NULL,				NULL };

static char *puppet_sidef_tbl[] = {
	p_hip_toss_a_anim,	p_hip_toss_v_anim,
	p_csid_a_anim,		p_csid_v_anim,
//#ifndef SEATTLE
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//#endif
	p_flip_a_anim,		p_flip_v_anim,
	p_tkdn_a_anim,		p_tkdn_v_anim,
	p_grab_jersey_a_anim,p_grab_jersey_v_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_thrw_a_anim,		p_thrw_v_anim,
	p_flip_a_anim,		p_flip_v_anim,
	p_fslm_a_anim,		p_fslm_v_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_armg_a_anim,		p_armg_v_anim,
	p_hip_toss_a_anim,	p_hip_toss_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	p_armg2_a_anim,		p_armg2_v_anim,
	p_armg3_a_anim,		p_armg3_v_anim,
	NULL,				NULL };

//
// PUPPET-MOVE tackle animation tables (KICK-ASS VOILENT TABLES)
//
static char *violent_puppet_frnt_tbl[] = {
	p_ftac_a_anim,		p_ftac_v_anim,
	p_ftac_a_anim,		p_ftac_v_anim,
	p_tkdn_a_anim,		p_tkdn_v_anim,
//	p_thrw_a_anim,		p_thrw_v_anim,
//#ifndef SEATTLE
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//#endif
//	p_face_a_anim,		p_face_v_anim,
//	p_hdbuta_anim,		p_hdbutv_anim,
//	p_hdlcka_anim,		p_hdlckv_anim,
//	p_hdbut2a_anim,		p_hdbut2v_anim,
//	p_hdbut3a_anim,		p_hdbut3v_anim,
//	p_pile_a_anim,		p_pile_v_anim,
//	p_knee_a_anim,		p_knee_v_anim,
//	p_knee2_a_anim,		p_knee2_v_anim,
//	p_knee3_a_anim,		p_knee3_v_anim,
//	p_fshk_a_anim,		p_fshk_v_anim,
//	p_fslm_a_anim,		p_fslm_v_anim,
//	p_supl_a_anim,		p_supl_v_anim,
//	p_hdlcka_anim,		p_hdlckv_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_armg_a_anim,		p_armg_v_anim,
	p_frol_a_anim,		p_frol_v_anim,
	p_armg2_a_anim,		p_armg_v_anim,
	NULL,				NULL };

static char *violent_puppet_side_tbl[] = {
	p_csid_a_anim,		p_csid_v_anim,
	p_tkdn_a_anim,		p_tkdn_v_anim,
//	p_thrw_a_anim,		p_thrw_v_anim,
//#ifndef SEATTLE
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//#endif
	p_face_a_anim,		p_face_v_anim,
//	p_hdbuta_anim,		p_hdbutv_anim,
//	p_hdlcka_anim,		p_hdlckv_anim,
//	p_hdbut2a_anim,		p_hdbut2v_anim,
//	p_hdbut3a_anim,		p_hdbut3v_anim,
//	p_pile_a_anim,		p_pile_v_anim,
//	p_knee_a_anim,		p_knee_v_anim,
//	p_knee2_a_anim,		p_knee2_v_anim,
//	p_knee3_a_anim,		p_knee3_v_anim,
//	p_fshk_a_anim,		p_fshk_v_anim,
	p_fslm_a_anim,		p_fslm_v_anim,
//	p_supl_a_anim,		p_supl_v_anim,
//	p_hdlcka_anim,		p_hdlckv_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_armg_a_anim,		p_armg_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	p_armg2_a_anim,		p_armg_v_anim,
	NULL,				NULL };

static char *violent_puppet_back_tbl[] = {
	p_csid_a_anim,		p_csid_v_anim,
	p_tkdn_a_anim,		p_tkdn_v_anim,
//	p_thrw_a_anim,		p_thrw_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	p_btac_a_anim,		p_btac_v_anim,
//	p_pile_a_anim,		p_pile_v_anim,
//#ifndef SEATTLE
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//#endif
	p_suplb_a_anim,		p_suplb_v_anim,
	p_btac_a_anim,		p_btac_v_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	NULL,				NULL };

static char *violent_puppet_sidef_tbl[] = {
	p_csid_a_anim,		p_csid_v_anim,
	p_tkdn_a_anim,		p_tkdn_v_anim,
//	p_thrw_a_anim,		p_thrw_v_anim,
//#ifndef SEATTLE
//	p_thrw2_a_anim,		p_thrw2_v_anim,
//	p_thrw3_a_anim,		p_thrw3_v_anim,
//#endif
	p_face_a_anim,		p_face_v_anim,
//	p_hdbuta_anim,		p_hdbutv_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
//	p_hdbut2a_anim,		p_hdbut2v_anim,
//	p_hdlcka_anim,		p_hdlckv_anim,
//	p_hdbut3a_anim,		p_hdbut3v_anim,
//	p_pile_a_anim,		p_pile_v_anim,
//	p_knee_a_anim,		p_knee_v_anim,
//	p_knee2_a_anim,		p_knee2_v_anim,
//	p_knee3_a_anim,		p_knee3_v_anim,
//	p_fshk_a_anim,		p_fshk_v_anim,
	p_fslm_a_anim,		p_fslm_v_anim,
//	p_supl_a_anim,		p_supl_v_anim,
//	p_hdlcka_anim,		p_hdlckv_anim,
	p_spin_a_anim,		p_spin_v_anim,
	p_armg_a_anim,		p_armg_v_anim,
	p_tbak_a_anim,		p_tbak_v_anim,
	p_armg2_a_anim,		p_armg_v_anim,
	NULL,				NULL };
	

//
// Animation direction (dir1) table pointers
//
static char **std_hit_tbls[] = {
	std_hit_frnt_tbl,
	std_hit_side_tbl,
	std_hit_back_tbl,
	std_hit_sidef_tbl };
	
static char **trb_hit_tbls[] = {
	trb_hit_frnt_tbl,
	trb_hit_side_tbl,
	trb_hit_back_tbl,
	trb_hit_sidef_tbl };
	
static char **puppet_mv_tbls[] = {
	puppet_frnt_tbl,
	puppet_side_tbl,
	puppet_back_tbl,
	puppet_sidef_tbl };

static char **violent_puppet_mv_tbls[] = {
	violent_puppet_frnt_tbl,
	violent_puppet_side_tbl,
	violent_puppet_back_tbl,
	violent_puppet_sidef_tbl };
	
	
//
// 'Late hit' ANIMATIONS
//
static char *late_hits_tbl[] = {
	butt_drp_anim,
	belly_flop_anim,
	m_nostomp
};

static char *violent_late_hits_tbl[] = {
//	gut_stomp_anim,
	elbow_drp_anim,
//	punch_dwn_anim,
	belly_flop_anim,
//	foot_stomp_anim,
	butt_drp_anim,
	m_nostomp

};
//	knee_drp_anim };
	
// percentanges for attacker to miss carrier if carrier in spin move
static int avoid_tackle_percent[] = {60,40,20,5};			// 0, 1, 2, 3rd spin
static int redzone_avoid_tackle_percent[] = {23,14,5,3};	// 0, 1, 2, 3rd spin

///////////////////////////////////////////////////////////////////////////////
//
// player_detect_proc() - does player-to-player collision detection
//
void player_detect_proc(int *parg)
{
	int		i, j;
	float	ext1[3], ext2[3], dist;
	float	low_body_focus[NUM_PLAYERS][3];
	fbplyr_data	*ppdatai, *ppdataj;

	while( TRUE )
	{
		pftimer_end( PRF_COLLIS );
		sleep( 1 );

		// no collisions while initializing
		if( game_info.game_mode == GM_INITIALIZING )
			continue;

		pftimer_start( PRF_COLLIS );

		// compute all player distances & angles
		for( i = 0; i < NUM_PLAYERS; i++ )
		{
			ppdatai = player_blocks + i;

			distxz[i][i] = 0.0f;
			p2p_angles[i][i] = 0;
			for( j = i+1; j < NUM_PLAYERS; j++ )
			{
				ppdataj = player_blocks + j;
				dist = fsqrt((ppdatai->odata.x - ppdataj->odata.x) * (ppdatai->odata.x - ppdataj->odata.x) +
						(ppdatai->odata.y - ppdataj->odata.y) * (ppdatai->odata.y - ppdataj->odata.y) + 
						(ppdatai->odata.z - ppdataj->odata.z) * (ppdatai->odata.z - ppdataj->odata.z));
				distxz[i][j] = dist;
				distxz[j][i] = dist;
				p2p_angles[i][j] = ptangle( &(ppdatai->odata.x), &(ppdataj->odata.x) );
				p2p_angles[j][i] = (p2p_angles[i][j] + 512) % 1024;
			}
		}

		// body cylinder connects neck and midpoint of knees & pelvis.  compute these in advance
		for( i = 0; i < NUM_PLAYERS; i++ )
		{
			low_body_focus[i][0] = (player_blocks[i].jpos[JOINT_PELVIS][0] +
									player_blocks[i].jpos[JOINT_RKNEE][0] +
									player_blocks[i].jpos[JOINT_LKNEE][0]) / 3.0f;
			low_body_focus[i][1] = (player_blocks[i].jpos[JOINT_PELVIS][1] +
									player_blocks[i].jpos[JOINT_RKNEE][1] +
									player_blocks[i].jpos[JOINT_LKNEE][1]) / 3.0f;
			low_body_focus[i][2] = (player_blocks[i].jpos[JOINT_PELVIS][2] +
									player_blocks[i].jpos[JOINT_RKNEE][2] +
									player_blocks[i].jpos[JOINT_LKNEE][2]) / 3.0f;
		}

		for( i = 0; i < NUM_PLAYERS; i++ )
		{
			ppdatai = player_blocks + i;

			if( ppdatai->odata.flags & PF_NOCOLL )
				continue;

			for( j = i+1; j < NUM_PLAYERS; j++ )
			{
				ppdataj = player_blocks + j;

				if( distxz[i][j] > PASS1_DIST )
					continue;

				if( ppdataj->odata.flags & PF_NOCOLL )
					continue;

//				if( ppdataj->odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER))
//					continue;

				// close enough -- perform more detailed check

				if ( (!((ppdatai->odata.adata[0].animode | ppdataj->odata.adata[0].animode) & (MODE_PUPPET|MODE_PUPPETEER))) ||
					((ppdataj->odata.flags & PF_DRAGGING_PLR) || (ppdatai->odata.flags & PF_DRAGGING_PLR)) )
//				if ( !((ppdatai->odata.adata[0].animode | ppdataj->odata.adata[0].animode) & (MODE_PUPPET|MODE_PUPPETEER)) )
				{
					// first check for attack cylinders vs body cylinders.  i attacking j
					if( ppdatai->attack_mode )
					{
						if( ppdatai->acj1 == ppdatai->acj2 )
						{
							// spherical attack zone -- not yet supported
#ifdef DEBUG
							fprintf( stderr, "Error: spherical attack zones not supported.\n" );
							lockup();
#else
							while(1) ;
#endif
						}
						else
						{
							extend_line( ppdatai->jpos[ppdatai->acj1],
									ppdatai->jpos[ppdatai->acj2],
									ppdatai->acx1, ppdatai->acx2,
									ext1, ext2 );
							if( line_int_check( ext1, ext2, ppdataj->jpos[0],
									low_body_focus[j], 5.0f + ppdatai->acr ))
								{
								// collision
								do_collision( ppdatai, ppdataj, ppdatai->attack_mode );
							}
						}
					}

					// j attacking i
					if( ppdataj->attack_mode )
					{
						if( ppdataj->acj1 == ppdataj->acj2 )
						{
							// spherical attack zone -- not yet supported
#ifdef DEBUG
							fprintf( stderr, "Error: spherical attack zones not supported.\n" );
							lockup();
#else
							while(1) ;
#endif
						}
						else
						{
							extend_line( ppdataj->jpos[ppdataj->acj1],
									ppdataj->jpos[ppdataj->acj2],
									ppdataj->acx1, ppdataj->acx2,
									ext1, ext2 );
							if( line_int_check( ext1, ext2,	ppdatai->jpos[0],
									low_body_focus[i], 5.0f + ppdataj->acr ))
							{
								// collision
								do_collision( ppdataj, ppdatai, ppdataj->attack_mode );
							}
						}
					}
				}

				// now check body vs body

				// no body collisions if either guy has the MODE_NOCYL bit set
				if(( ppdatai->odata.adata[0].animode | ppdataj->odata.adata[0].animode ) & MODE_NOCYL )
					continue;

				if( distxz[i][j] < ( 2.0f * BODY_RADIUS ))
				{
					do_collision( ppdatai, ppdataj, ATTMODE_NONE );
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// line_int_check() - returns TRUE if the distance between lines a and b,
// defined by endpoints a1,a2 and b1,b2, at their point of closest approach,
// is less than or equal to d.
//////////////////////////////////////////////////////////////////////////////
#define CLAMP(x,lo,hi) ((x<lo) ? lo : ((x>hi) ? hi : x))

static float line_int_check( float *a1, float *a2, float *b1, float *b2, float d )
{
	vec3d	v1,v2;
	float	m[12];
	float	t,s,mag,tt,ss;
	float	dx,dy,dz,dist;
	int		bad_t = FALSE, bad_s = FALSE;
	float	*pp1,*pp2, p1[3], p2[3];

	v1.x = a2[0] - a1[0];
	v1.y = a2[1] - a1[1];
	v1.z = a2[2] - a1[2];
	v1.w = 1.0f;

	v2.x = b2[0] - b1[0];
	v2.y = b2[1] - b1[1];
	v2.z = b2[2] - b1[2];
	v2.w = 1.0f;

	m[0] = b1[0] - a1[0];
	m[1] = b1[1] - a1[1];
	m[2] = b1[2] - a1[2];

	vxv( &v1, &v2, (vec3d *)(m+8) );

	mag = magv( (vec3d *)(m+8) );

	if( mag == 0.0f )
	{
		// lines are parallel
		s = point2line_d( b1, b2, a1 );

		dx = b1[0] + s*(b2[0]-b1[0]) - a1[0];
		dy = b1[1] + s*(b2[1]-b1[1]) - a1[1];
		dz = b1[2] + s*(b2[2]-b1[2]) - a1[2];

		if((dx*dx + dy*dy + dz*dz) <= d*d )
			return TRUE;
		else
			return FALSE;
	}

	m[4] = v2.x;
	m[5] = v2.y;
	m[6] = v2.z;

	t = det( m ) / (mag*mag);

	m[4] = v1.x;
	m[5] = v1.y;
	m[6] = v1.z;

	s = det( m ) / (mag*mag);

	// useless.  this init just prevents a warning
	pp1 = a1; pp2 = b1;

	if( t > 1.0f )
	{
		t = 1.0f;
		pp1 = a2;
		bad_t = TRUE;
	}
	if( t < 0.0f )
	{
		t = 0.0f;
		pp1 = a1;
		bad_t = TRUE;
	}
	if( s > 1.0f )
	{
		s = 1.0f;
		pp2 = b2;
		bad_s = TRUE;
	}
	if( s < 0.0f )
	{
		s = 0.0f;
		pp2 = b1;
		bad_s = TRUE;
	}

	if( bad_t && bad_s )
	{
		// both t and s were out of range, and both have been capped.
		// the shortest distance between the two segments ends with one
		// of these endpoints, but we have to check both to figure out
		// which.
		tt = point2line_d( a1, a2, pp2 );
		tt = CLAMP( t, 0.0f, 1.0f );
		p1[0] = a1[0] + tt*(a2[0]-a1[0]);
		p1[1] = a1[1] + tt*(a2[1]-a1[1]);
		p1[2] = a1[2] + tt*(a2[2]-a1[2]);

		dist = (pp2[0]-p1[0])*(pp2[0]-p1[0]) +
				(pp2[1]-p1[1])*(pp2[1]-p1[1]) +
				(pp2[2]-p1[2])*(pp2[2]-p1[2]);

		if( dist <= d*d )
			return TRUE;

		ss = point2line_d( b1, b2, pp1 );
		ss = CLAMP( ss, 0.0f, 1.0f );
		p2[0] = b1[0] + ss*(b2[0]-b1[0]);
		p2[1] = b1[1] + ss*(b2[1]-b1[1]);
		p2[2] = b1[2] + ss*(b2[2]-b1[2]);

		dist = (pp1[0]-p2[0])*(pp1[0]-p2[0]) +
				(pp1[1]-p2[1])*(pp1[1]-p2[1]) +
				(pp1[2]-p2[2])*(pp1[2]-p2[2]);

		if( dist <= d*d )
			return TRUE;

		return FALSE;
	}
	else if( bad_t )
	{
		// t was out of range, s was in.  t has been capped and is
		// definitely the point on line1 closest to line2.  Recompute
		// s based on new capped t.
		s = point2line_d( b1, b2, pp1 );
		s = CLAMP( s, 0.0f, 1.0f );

		p2[0] = b1[0] + s*(b2[0]-b1[0]);
		p2[1] = b1[1] + s*(b2[1]-b1[1]);
		p2[2] = b1[2] + s*(b2[2]-b1[2]);
		pp2 = p2;
	}
	else if( bad_s )
	{
		t = point2line_d( a1, a2, pp2 );
		t = CLAMP( t, 0.0f, 1.0f );

		p1[0] = a1[0] + t*(a2[0]-a1[0]);
		p1[1] = a1[1] + t*(a2[1]-a1[1]);
		p1[2] = a1[2] + t*(a2[2]-a1[2]);

		pp1 = p1;
	}
	else
	{
		p1[0] = a1[0] + t*(a2[0]-a1[0]);
		p1[1] = a1[1] + t*(a2[1]-a1[1]);
		p1[2] = a1[2] + t*(a2[2]-a1[2]);

		p2[0] = b1[0] + s*(b2[0]-b1[0]);
		p2[1] = b1[1] + s*(b2[1]-b1[1]);
		p2[2] = b1[2] + s*(b2[2]-b1[2]);

		pp1 = p1;
		pp2 = p2;
	}

	dist = (pp2[0]-pp1[0])*(pp2[0]-pp1[0]) +
			(pp2[1]-pp1[1])*(pp2[1]-pp1[1]) +
			(pp2[2]-pp1[2])*(pp2[2]-pp1[2]);

	if( dist <= d*d )
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// point2line_d() - returns t for the point on a line L1L2 closest to point P.
//////////////////////////////////////////////////////////////////////////////
static float point2line_d( float *l1, float *l2, float *p )
{
	float v[3],rmp[3];
	float t;

	v[0] = l2[0] - l1[0];
	v[1] = l2[1] - l1[1];
	v[2] = l2[2] - l1[2];

	rmp[0] = p[0] - l1[0];
	rmp[1] = p[1] - l1[1];
	rmp[2] = p[2] - l1[2];

	t = (rmp[0]*v[0] + rmp[1]*v[1] + rmp[2]*v[2]) / (v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );

	return t;
}

//////////////////////////////////////////////////////////////////////////////
// extend line() - Given a line defined by endpoints p1 and p2, extend the
// endpoints by distances x1 and x2, respectively, and store the extended
// endpoints in xp1 and xp2.
static void extend_line( float *p1, float *p2, float x1, float x2, float *xp1, float *xp2 )
{
	float	v[3];
	float	mag;

	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	mag = 1.0f / fsqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	v[0] *= mag;	v[1] *= mag;	v[2] *= mag;

	xp1[0] = p1[0] - v[0] * x1;
	xp1[1] = p1[1] - v[1] * x1;
	xp1[2] = p1[2] - v[2] * x1;

	xp2[0] = p2[0] + v[0] * x2;
	xp2[1] = p2[1] + v[1] * x2;
	xp2[2] = p2[2] + v[2] * x2;
}

//////////////////////////////////////////////////////////////////////////////
// do_collision() - All collisions come through here.
static void do_collision( fbplyr_data *att, fbplyr_data *def, int mode )
{
	char *game_modes[] = { "GM_LINING_UP", "GM_PRE_SNAP", "GM_IN_PLAY",
							"GM_PLAY_OVER", "GM_GAME_OVER", "GM_PRE_KICKOFF" };
	int	old_game_mode;
	int	dir1, dir2;
	
	// game_mode should never change during this function
	// if it does, holler
	old_game_mode = game_info.game_mode;

	// TODO: have this use actual angle of impact, not just attacker facing
	dir2 = att->odata.fwd - def->odata.fwd;

#ifdef DEBUG
	if ((dir2 + 1536 +  64) < 0 || dir2 >= (2147481600 + 384))
		fprintf(stderr,"--> Bad dir2 in <do_collision>: %d\r\n", dir2);
#endif

	while (dir2 < 0) dir2 += 1024;					// fix if it needs to be
	while (dir2 >= 2147481600) dir2 -= 1024;		// fix if it needs to be (k = 2^31 - 2048)

	dir1 = ((dir2 + 1536 + 128)%1024) / 256;		// four directions
	dir2 = ((dir2 + 1536 +  64)%1024) / 128;		// eight directions

	def->odata.pobj_hit = (obj_3d *)att;
	att->odata.pobj_hit = (obj_3d *)def;

	mvx = (att->odata.vx + def->odata.vx) / 2.0f;
	mvz = (att->odata.vz + def->odata.vz) / 2.0f;
	
	victim_seq = def->odata.adata[0].seq_index;
	victim_y = def->odata.y;

	cfuncs[mode]( att, def, dir1, dir2 );

	if(( game_info.game_mode != old_game_mode ) &&
		( game_info.play_end_cause != PEC_CAR_TOUCHDOWN ))
	{
		fprintf( stderr, "Warning: game_mode changed from %s to %s during do_collision.\n",
			game_modes[old_game_mode], game_modes[game_info.game_mode] );
		fprintf( stderr, "attack mode: %d, new att script: %p, new def script %p\n",
			mode, att->odata.adata[0].pscrhdr, def->odata.adata[0].pscrhdr );
	}
}

//////////////////////////////////////////////////////////////////////////////
static void att_none( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
	// push both away along the line connecting them.  Each moves a distance
	// proportional to the dot product of their velocity with the connecting
	// line.
	vec3d		c;
	float		da,dd,m,dy,mag;
	fbplyr_data	*tmp;


	dy = fabs( att->odata.y - def->odata.y );

	// too much y difference?
	if( dy > 30.0f )
		return;

	// are we connected?
	if( att->puppet_link == def )
		return;

	// swap such that def is team with ball
	if((( game_info.game_mode != GM_PLAY_OVER ) && (att->team == game_info.off_side)) ||
		(att->team == game_info.last_pos ))
	{
		tmp = att;
		att = def;
		def = tmp;
	}

	// make sure victim seq is correct
	// it might've been the attacker's seq in a no-mode attack
	victim_seq = def->odata.adata[0].seq_index;
	victim_y = def->odata.y;
	
	// sort cases by def
	if (att->team == def->team)
	{
		// same side.  fall through to cylinder case
		if ((def->odata.flags & PF_LATEHITABLE) &&
			(!(def->odata.adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))) &&
			(!(att->odata.adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))) )
		{
		 	att->puppet_link = def;
		 	def->puppet_link = att;
			change_anim((obj_3d *)att, p_hlpup_back_a_anim);
			change_anim((obj_3d *)def, p_hlpup_back_v_anim);
//			change_anim((obj_3d *)att, p_hlpup_chest_a_anim);
//			change_anim((obj_3d *)def, p_hlpup_chest_v_anim);
			return;
		}
	}
	else if ((def->plyrnum == game_info.ball_carrier) || (game_info.game_mode == GM_PLAY_OVER))
	{
		// def is ball carrier
		// ok... here we could have a case where the player (attacker) did a attmode move, the attack
		// box is off and he's still in the move...which means the attmode didnt get the guy...
		// so just cyclinder off defender!!!
		if ((att->odata.adata[0].seq_index != (SEQ_T_CRZDIV)) &&
			(att->odata.adata[0].seq_index != (SEQ_T_SPRHED)))
//		if ((att->odata.adata[0].seq_index != (SEQ_T_DIVE)) &&
//			(att->odata.adata[0].seq_index != (SEQ_T_CRZDIV)) &&
//			(att->odata.adata[0].seq_index != (SEQ_T_SPRHED)))
		{
			if (evade_vs_tackle( def, att, dir1, dir2 ))
				return;
		}	
	}
	else if (def->job == JOB_BLOCK)
	{
		// def is blocker
		if( block_vs_rush( def, att, dir1, dir2 ))
			return;
	}
	else
	{
		// def is weapon on route, fall through to cylinder case
	}

	// normal cylinder interaction
	c.x = def->odata.x - att->odata.x;
	c.z = def->odata.z - att->odata.z;

	mag = c.x*c.x + c.z*c.z;

	if( mag < 0.001f )
	{
		c.x  = 1.0f;
		c.z = 0.0f;
	}
	else
	{
		m = 1.0f / fsqrt( mag );
		c.x *= m;
		c.z *= m;
	}

	da = fabs( att->odata.vx * c.x + att->odata.vz * c.z );
	dd = fabs( def->odata.vx * c.x + def->odata.vz * c.z );

	// if neither has any velocity along the line connecting them, set them equal
	if(( fabs(da) < 0.001 ) && ( fabs(dd) < 0.001 ))
	{
		da = 1.0f;
		dd = 1.0f;
	}

	// if one is a receiver who isn't blocking, make the defender do most of the moving.
	if(( def->odata.plyrmode == MODE_WPN ) && ( def->job != JOB_BLOCK ))
		da += 6.0f;

	// if either is unint, make the other do almost all the moving
	if( def->odata.adata[0].animode & MODE_UNINT )
		da += 6.0f;
	if( att->odata.adata[0].animode & MODE_UNINT )
		dd += 6.0f;

	// if either is in a puppet move, have the other do ALL the moving.
	// if either is REACTING and NOT a puppet, have him do all the moving
	// EXCEPTION: If PLAY_ENDING, do NOT have the REACTING guy do all the moving.
	if (def->odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER))
	{
		dd = 0.0f; da = 1.0f;
	}
	else if ((def->odata.adata[0].animode & MODE_REACTING) &&
			((!(game_info.game_flags & GF_PLAY_ENDING)) && (def->odata.y > 6.5f)))

	{
		da = 0.0f; dd = 1.0f;
	}
	
	if( att->odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER))
	{
		da = 0.0f; dd = 1.0f;
	}
	else if ((att->odata.adata[0].animode & MODE_REACTING) &&
			((!(game_info.game_flags & GF_PLAY_ENDING)) && (att->odata.y > 6.5f)))
//			!(game_info.game_flags & GF_PLAY_ENDING))
	{
		dd = 0.0f; da = 1.0f;
	}

	// no force on earth can move a lineman before the snap
	if (def->odata.plyrmode == MODE_LINEPS)
	{
		da = 1.0f; dd = 0.0f;
	}

	if (att->odata.plyrmode == MODE_LINEPS)
	{
		da = 0.0f; dd = 1.0f;
	}

	m = 1.0f / (da + dd);
	m *= ((2.0f * BODY_RADIUS) - distxz[att->plyrnum][def->plyrnum]);
	da *= m;
	dd *= m;

	att->odata.x -= da * c.x;
	att->odata.z -= da * c.z;

	def->odata.x += dd * c.x;
	def->odata.z += dd * c.z;
}

//////////////////////////////////////////////////////////////////////////////
static void att_push( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
	char *reactions[] = { h_front1_anim, h_side1f_anim, h_back1_anim, h_side1_anim };
	int spin,i;
	fbplyr_data *puppet = def->puppet_link;
	float	rx,ry,rz, *tm;
	

	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_push: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// dont allow player to be pushed if hes lying on the ground
	if ((def->odata.y + def->odata.ay) < 2.0f)
		return;

	// dont allow player to be pushed if hes on same team
	if (def->team == att->team)
		return;

	// dont push dude, if he is in a puppet...or reacting already!
	if (def->odata.adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))
		return;

	// if hit the DUDE being DRAGGED...ignore
	if (puppet != NULL)
	{
		if (puppet->odata.flags & PF_DRAGGING_PLR)
			return;
	}

	// dont push a MAN dragging player
	if (def->odata.flags & PF_DRAGGING_PLR)
		return;

	// clear attacker's attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	// Do this AFTER attack is cleared!
	// If we're on defense and defender has JOB_BLOCK set, ignore this 50% of the time
	if ((att->team == !game_info.off_side) &&
		(def->job == JOB_BLOCK) &&
		(randrng(2)))
		return;

	// if victim is in spin move...1st time ignore most hits...2nd time very hittable...etc
	if (def->odata.adata[0].seq_index == (SEQ_SPIN))
	{
		spin = game_info.spin_cnt[def->team];
		if (spin >= 3)
		{
			abusing_spin = 1;
			spin = 3;
		}
		if (FIELDX(def->odata.x) > 80)
		{	// player spinning in red zone
			if (randrng(100) < redzone_avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
		else
		{	// player spinning elsewhere
			if (randrng(100) < avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
	}
	else
		abusing_spin = 0;

	// allow diver to hit me...this helps the defense
	if ((randrng(100) < 55) &&
		((def->attack_mode == ATTMODE_DIVE) || (def->attack_mode == ATTMODE_SPEAR_HEAD)))
	{
		return;
	}

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

	// TEST TEST TEST
#if 0
	for (i=0; i<20; i++)
	{
		tm = cambot.xform;
		rx = def->jpos[JOINT_NECK][0];
		ry = def->jpos[JOINT_NECK][1];
		rz = def->jpos[JOINT_NECK][2];
		// get starting point
		shards_data.x = rx * tm[0] + ry * tm[4] + rz * tm[8] + cambot.x;
		shards_data.y = rx * tm[1] + ry * tm[5] + rz * tm[9] + cambot.y;
		shards_data.z = rx * tm[2] + ry * tm[6] + rz * tm[10] + cambot.z;
//		shards_data.mx = 0.0f;
//		shards_data.mz = 0.0f;
		shards_data.mx = ((float)randrng(8000))/14000.0f;
		shards_data.mz = ((float)randrng(8000))/14000.0f;
		if (randrng(100) < 40)
		{
			shards_data.mx *= -1.0f;
			shards_data.mz *= -1.0f;
		}
		shards_data.my = ((float)randrng(8000))/9000.0f;
		shards_data.velocity = ((float)randrng(8000))/13000.0f;
		shards_data.mvelocity = 0.15f;
		shards_data.friction = .02f;
		shards_data.gravity = .015f;
		shards_data.x_ang = 180.0f;
		shards_data.y_ang = 180.0f;
		shards_data.z_ang = 180.0f;
		shards_data.num_particles = 1;
		shards_data.create_ticks = 2;
// 		shards_data.texture_list = smoke_textures;
		shards_data.texture_list = NULL;
		make_debris(&shards_data);
	}
		// TEST TEST TEST
#endif

	// will QB bitch slap tackler ?
	if ((def->odata.plyrmode == MODE_QB) &&
		(randrng(100) < 90) &&
		(game_info.play_time <= 2) &&
		(!qb_evaded) && 
		(ISDRONE(def)) &&
		(game_info.play_type == PT_NORMAL) &&
		(!(def->odata.adata[0].animode & MODE_UNINT)))
	{
		qb_evaded = 1;
		// clear defender's attack mode
		change_anim((obj_3d *)def, (def->odata.flags & PF_FLIP) ? stiff_arm2_f_anim : stiff_arm2_anim);
		snd_scall_bank(plyr_bnk_str,OUTTA_MY_FACE_SP,VOLUME3,127,LVL3);
		def->odata.flags &= ~PF_THROWING;
		return;
	}
	

	// 60% of time do flail
	if ((randrng(100) < 50) &&
		(def->plyrnum != game_info.ball_carrier) &&
		!(game_info.game_flags & GF_PLAY_ENDING))
	{
		if (((obj_3d *)att)->adata[1].animode & MODE_STIFF_ARM)
			change_anim((obj_3d *)def ,stiff_arm_flail_tbl[randrng(sizeof(stiff_arm_flail_tbl)/sizeof(char *))]);
		else
			change_anim((obj_3d *)def ,flail_anim_tbl[randrng(sizeof(flail_anim_tbl)/sizeof(char *))]);
		crowd_tackle_snds();
		snd_scall_bank(plyr_bnk_str,P_HARD_HIT2_SND,VOLUME4,127,PRIORITY_LVL4);		// explosion
	}
	else
	{
		change_anim((obj_3d *)def, reactions[dir1]);
//		snd_scall_bank(plyr_bnk_str,BACK_OFF_MAN_SP,VOLUME1,127,PRIORITY_LVL1);
		snd_scall_bank(plyr_bnk_str,P_HARD_HIT2_SND,VOLUME4,127,PRIORITY_LVL4);		// explosion
	}

	// if victim is carrier and keeps ball, mark end of his progress
	if (( def->plyrnum == game_info.ball_carrier ) &&
		( game_info.game_mode == GM_IN_PLAY ) &&
		!( game_info.game_flags & GF_PLAY_ENDING ))
	{
		if( !fumble_or_bobble( def, att, 70, 430 ))
		{
			// inc stat.
			if (!ISDRONE(att))
				tdata_blocks[att->team].tackles++;
		
			// Set arrows-off flags
			arrowinfo[0].ai_flags |= AF_INACTIVE;
			arrowinfo[1].ai_flags |= AF_INACTIVE;
			arrowinfo[2].ai_flags |= AF_INACTIVE;
			arrowinfo[3].ai_flags |= AF_INACTIVE;
		
			// check for TD regardless of AIR_CATCH bit
			if (td_check(TDC_NOCONTROL|TDC_NOAIRCATCH) && ((att->odata.y < 0.2f) || (def->odata.y < 0.2f)))
			{
				game_info.game_mode = GM_PLAY_OVER;
				game_info.play_end_cause = PEC_CAR_TOUCHDOWN;
			}
//			qcreate("whistle",0, whistle_proc, 0, 0, 0, 0);

			game_info.play_end_pos = def->odata.x;
			game_info.play_end_z = def->odata.z;
			game_info.game_flags |= GF_PLAY_ENDING;
#ifdef PLAY_DEBUG
			fprintf( stderr, "  carrier %2x(%d) hit, going down\n", JERSEYNUM(def->plyrnum), def->team );
			fprintf( stderr, "  play_end_pos set(1) to %5.2f, field %d\n", game_info.play_end_pos, FIELDX(game_info.play_end_pos) );
#endif
#ifndef NOSETCAM
			select_csi_item( csi_fs_endplay );
			cambot.pmodeobj = &(def->odata);
#endif
//			update_onfire_stuff(att, def);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void att_dive( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
	int		brng,i,spin;
	fbplyr_data *puppet = def->puppet_link;
	int		anim = 0;
//	float	rx,ry,rz, *tm;


	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_dive: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// ignore this if we're on the same team
	if( att->team == def->team )
		return;

	// clear attacker's attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	// if victim is in spin move...1st time ignore most hits...2nd time very hittable...etc
	if (def->odata.adata[0].seq_index == (SEQ_SPIN))
	{
		spin = game_info.spin_cnt[def->team];
		if (spin >= 3)
		{
			abusing_spin = 1;
			spin = 3;
		}
		if (FIELDX(def->odata.x) > 80)
		{	// player spinning in red zone
			if (randrng(100) < redzone_avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
		else
		{	// player spinning elsewhere
			if (randrng(100) < avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
	}
	else
		abusing_spin = 0;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

//#ifndef DAN

	// if victim is ON GROUND...lying down... stop attacker...so he lands on victim
	if ((def->odata.y + def->odata.ay) < 2.0f)
		return;

	// if hit the DUDE being DRAGGED...ignore
	if (puppet != NULL)
	{
		if (puppet->odata.flags & PF_DRAGGING_PLR)
			return;
	}

	// if ball carrier is dragging a player...knock'em down... and clear puppet shit!!
	if (def->odata.flags & PF_DRAGGING_PLR)
	{

		def->odata.flags &= ~PF_DRAGGING_PLR;
		def->puppet_rel_time = pcount;
		def->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER|MODE_NOCYL);
		def->puppet_link->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER|MODE_NOCYL);
		change_anim((obj_3d *)def->puppet_link ,h_bsider_anim);
		def->puppet_link->puppet_link = NULL;
		def->puppet_link = NULL;

//		def->odata.flags &= ~PF_DRAGGING_PLR;
//		puppet->puppet_link = NULL;
//		puppet->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);
//		change_anim((obj_3d *)puppet ,h_bsider_anim);
//		def->puppet_link = NULL;
//		def->puppet_rel_time = pcount;
//		def->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);

		// sometimes... let dive move RE_ATTACH
		if (randrng(100) > 10)
		{
			change_anim((obj_3d *)def ,drag_hit_tbl[randrng(sizeof(drag_hit_tbl)/sizeof(char *))]);
			return;
		}
	}


	//	if victim is busy...ignore
	if( ((obj_3d *)def)->adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))
	{
		// DEBUG!!! keep...
//		if (randrng(100) > 5)
			return;
	}	

	// if hit intended receiver...	
//	if ((def->plyrnum == ball_obj.int_receiver) &&
//		(ISHUMAN(att)) &&
//		(game_info.ball_carrier == -1) &&
//		(game_info.game_mode == GM_IN_PLAY) &&
//		(ball_obj.type == LB_PASS))
//	{
//		hit_intended_receiver_sp();
//	}
	
	// this is the bearing from def to att
	brng = bearing( def->plyrnum, att->plyrnum );

	// if attacker hits ball carrier above knees...do normal dive else ankle tackle
	if (att->odata.y <= 5.0f)
	{	// hit below knees
		if ((randrng(100) < 15) &&
			(fsqrt(def->odata.vx*def->odata.vx + def->odata.vz*def->odata.vz) > 1.20f) &&
			!(def->odata.flags & PF_BACKPEDAL))
		{
			change_anim((obj_3d *)def ,stumble_anim_tbl[randrng(sizeof(stumble_anim_tbl)/sizeof(char *))]);
			crowd_cheer_snds();
			return;
		}
		else if (brng >= 180)
		{
		 	att->puppet_link = def;
		 	def->puppet_link = att;
//			if (randrng(100) < 0)
			if (randrng(100) < 50)
			{
			 	change_anim((obj_3d *)att, p_ankl_a_anim);
			 	change_anim((obj_3d *)def, p_ankl_v_anim);
				crowd_tackle_snds();
			}
			else
			{
			 	change_anim((obj_3d *)att, p_ank2_a_anim);
			 	change_anim((obj_3d *)def, p_ank2_v_anim);
				crowd_tackle_snds();
			}
//			tackle_spch((obj_3d *)def, 0, SHOE_STRING_TACKLE_SP);
		}
		else
		{
			change_anim((obj_3d *)def, h_frflip_anim);
			crowd_tackle_snds();
		}
	}
	else
	{	// being hit with dive...above knees


		// if ball carrier...and on fire...go into this move!!!
		if ((def->plyrnum == game_info.ball_carrier) &&
			(def->plyrnum == tdata_blocks[def->team].last_receiver) &&
			(def->odata.plyrmode != MODE_QB) &&
			(!(def->odata.flags & PF_CELEBRATE)) &&
			(!(def->odata.flags & PF_DRAGGING_PLR)) &&
			(game_info.game_mode == GM_IN_PLAY) &&
			(game_info.team_fire[def->team]))
		{
			att->puppet_link = def;
			def->puppet_link = att;

			def->odata.flags |= PF_DRAGGING_PLR;

			if (randrng(100) < 50)
			{
				change_anim((obj_3d *)att, p_run_drag2_a_anim);
				change_anim((obj_3d *)def, p_run_drag2_v_anim);
				monkey_on_back_sp();
			}
			else
			{
				change_anim((obj_3d *)att, p_run_drag_a_anim);
				change_anim((obj_3d *)def, p_run_drag_v_anim);
				monkey_on_back_sp();
			}
			return;
		}

		// will QB bitch slap tackler ?
		if ((def->odata.plyrmode == MODE_QB) &&
			(randrng(100) < 90) &&
			(game_info.play_time <= 2) &&
			(!qb_evaded) &&
			(ISDRONE(def)) &&
			(game_info.play_type == PT_NORMAL) &&
			(!(def->odata.adata[0].animode & MODE_UNINT)))
		{
			qb_evaded = 1;
			// clear defender's attack mode
			att->attack_mode = 0;
			att->attack_time = 0;
			change_anim((obj_3d *)def, (def->odata.flags & PF_FLIP) ? stiff_arm2_f_anim : stiff_arm2_anim);
			snd_scall_bank(plyr_bnk_str,OUTTA_MY_FACE_SP,VOLUME3,127,LVL3);
			def->odata.flags &= ~PF_THROWING;
			return;
		}
		// will QB duck ?
		else if ((def->odata.plyrmode == MODE_QB) &&
				 (randrng(100) < qb_stiff_arm_ht[PLYRH(def)]) &&
				 (game_info.play_time < 6) &&
				 (game_info.play_type == PT_NORMAL) &&
				 (!(def->odata.adata[0].animode & MODE_UNINT)) &&
				 (!qb_evaded))
		{
			qb_evaded = 1;
			// clear defender's attack mode
			att->attack_mode = 0;		// only do for DUCKING
			att->attack_time = 0;
			change_anim((obj_3d *)def, (def->odata.flags & PF_FLIP) ? duck_flip_anim : duck_anim);
			snd_scall_bank(plyr_bnk_str,OUTTA_MY_FACE_SP,VOLUME3,127,LVL3);
			def->odata.flags &= ~PF_THROWING;
			qb_duck_speech();
			return;
		}

		if (brng >= 180)
		{
			att->puppet_link = def;
			def->puppet_link = att;
			
			anim = randrng(8) + 1;
//			anim = 8;

			crowd_tackle_snds();
			switch(anim)
			{
//				case 0:
//					change_anim((obj_3d *)att, p_dive_a_anim);
//					change_anim((obj_3d *)def, p_dive_v_anim);
				case 1:
					change_anim((obj_3d *)att, p_hip_toss_a_anim);
					change_anim((obj_3d *)def, p_hip_toss_v_anim);
					break;
				case 2:
					change_anim((obj_3d *)att, p_spin_a_anim);
					change_anim((obj_3d *)def, p_spin_v_anim);
					break;
				case 3:
					change_anim((obj_3d *)att, p_armg3_a_anim);
					change_anim((obj_3d *)def, p_armg3_v_anim);
//					if (randrng(100) < 50)
//						snd_scall_bank(taunt_bnk_str,11,VOLUME3,127,PRIORITY_LVL3);
					break;
				case 4:
					change_anim((obj_3d *)att, p_head_grab_a_anim);
					change_anim((obj_3d *)def, p_head_grab_v_anim);
					break;
				case 5:
					change_anim((obj_3d *)att, p_dive_a_anim);
					change_anim((obj_3d *)def, p_dive_v_anim);
					break;
				case 6:
					change_anim((obj_3d *)att, p_frol_a_anim);
					change_anim((obj_3d *)def, p_frol_v_anim);
					break;
				case 7:
					change_anim((obj_3d *)att, p_wtac_a_anim);
					change_anim((obj_3d *)def, p_wtac_v_anim);
				case 8:
					change_anim((obj_3d *)att, p_waist_drop_a_anim);
					change_anim((obj_3d *)def, p_waist_drop_v_anim);
					break;
				case 9:
					change_anim((obj_3d *)att, p_grab_jersey_a_anim);
					change_anim((obj_3d *)def, p_grab_jersey_v_anim);
					break;
				default:
					change_anim((obj_3d *)att, p_ftac_a_anim);
					change_anim((obj_3d *)def, p_ftac_v_anim);
					break;
			}
		}
		else
		{
			att->puppet_link = def;
			def->puppet_link = att;
			
			if (randrng(100) < 65)
			{
				change_anim((obj_3d *)att, p_ftac_a_anim);
				change_anim((obj_3d *)def, p_ftac_v_anim);
			}
			else
			{
				change_anim((obj_3d *)att, p_clothes_line_a_anim);
				change_anim((obj_3d *)def, p_clothes_line_v_anim);
			}
			crowd_tackle_snds();
		}
	}
//#endif //DAN

	// if victim is carrier and keeps ball, mark end of his progress
	if(( def->plyrnum == game_info.ball_carrier ) &&
		( game_info.game_mode == GM_IN_PLAY ) &&
		!( game_info.game_flags & GF_PLAY_ENDING ))
	{
		if( !fumble_or_bobble( def, att, 60, 560 ))
		{
			// inc stat.
			if (!ISDRONE(att))
				tdata_blocks[att->team].tackles++;
		
			// Set arrows-off flags
			arrowinfo[0].ai_flags |= AF_INACTIVE;
			arrowinfo[1].ai_flags |= AF_INACTIVE;
			arrowinfo[2].ai_flags |= AF_INACTIVE;
			arrowinfo[3].ai_flags |= AF_INACTIVE;
		
			// check for TD regardless of AIR_CATCH bit
			if( td_check(TDC_NOCONTROL|TDC_NOAIRCATCH) && (( att->odata.y < 0.2f ) || ( def->odata.y < 0.2f )))
			{
				game_info.game_mode = GM_PLAY_OVER;
				game_info.play_end_cause = PEC_CAR_TOUCHDOWN;
			}
//			qcreate("whistle",0, whistle_proc, 0, 0, 0, 0);
			game_info.play_end_pos = def->odata.x;
			game_info.play_end_z = def->odata.z;
			game_info.game_flags |= GF_PLAY_ENDING;
#ifdef PLAY_DEBUG
			fprintf( stderr, "  carrier %2x(%d) hit, going down\n", JERSEYNUM(def->plyrnum), def->team );
			fprintf( stderr, "  play_end_pos set(2) to %5.2f, field %d\n", game_info.play_end_pos, FIELDX(game_info.play_end_pos) );
#endif
#ifndef NOSETCAM
			if ((def->odata.adata[0].seq_index != (SEQ_P_FLIP_V)) &&
				(def->odata.adata[0].seq_index != (SEQ_WSTDROPA)) &&
				(def->odata.adata[0].seq_index != (SEQ_CLOTHS_V)))
			{
				select_csi_item( csi_fs_endplay );
				cambot.pmodeobj = &(def->odata);
			}
#endif
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void att_kick( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
	int spin;
	
	// ignore this if we're on the same team
	if( att->team == def->team )
		return;

	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_kick: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// clear attacker's attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	att->odata.vx /= 2.0f;
	att->odata.vz /= 2.0f;

	// if victim is in spin move...1st time ignore most hits...2nd time very hittable...etc
	if (def->odata.adata[0].seq_index == (SEQ_SPIN))
	{
		spin = game_info.spin_cnt[def->team];
		if (spin >= 3)
		{
			abusing_spin = 1;
			spin = 3;
		}
		if (FIELDX(def->odata.x) > 80)
		{	// player spinning in red zone
			if (randrng(100) < redzone_avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
		else
		{	// player spinning elsewhere
			if (randrng(100) < avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
	}
	else
		abusing_spin = 0;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

	// if victim is ON GROUND...lying down... stop attacker...so he lands on victim
	if ((def->odata.y + def->odata.ay) < 2.0f)
		return;
	
	// if victim is carrier and keeps ball, mark end of his progress
	if(( def->plyrnum == game_info.ball_carrier ) &&
		( game_info.game_mode == GM_IN_PLAY ) &&
		!( game_info.game_flags & GF_PLAY_ENDING ))
	{
		if( !fumble_or_bobble( def, att, 80, 430 ))
		{
			// inc stat.
			if (!ISDRONE(att))
				tdata_blocks[att->team].tackles++;
		
			// Set arrows-off flags
			arrowinfo[0].ai_flags |= AF_INACTIVE;
			arrowinfo[1].ai_flags |= AF_INACTIVE;
			arrowinfo[2].ai_flags |= AF_INACTIVE;
			arrowinfo[3].ai_flags |= AF_INACTIVE;
		
			// check for TD regardless of AIR_CATCH bit
			if( td_check(TDC_NOCONTROL|TDC_NOAIRCATCH) && (( att->odata.y < 0.2f ) || ( def->odata.y < 0.2f )))
			{
				game_info.game_mode = GM_PLAY_OVER;
				game_info.play_end_cause = PEC_CAR_TOUCHDOWN;
			}
//			qcreate("whistle",0, whistle_proc, 0, 0, 0, 0);
			game_info.play_end_pos = def->odata.x;
			game_info.play_end_z = def->odata.z;
			game_info.game_flags |= GF_PLAY_ENDING;
#ifdef PLAY_DEBUG
			fprintf( stderr, "  carrier %2x(%d) hit, going down\n", JERSEYNUM(def->plyrnum), def->team );
			fprintf( stderr, "  play_end_pos set(3) to %5.2f, field %d\n", game_info.play_end_pos, FIELDX(game_info.play_end_pos) );
#endif
#ifndef NOSETCAM
			select_csi_item( csi_fs_endplay );
			cambot.pmodeobj = &(def->odata);
#endif
			crowd_tackle_snds();
//			update_onfire_stuff(att, def);
		}
	}
	change_anim( (obj_3d *)def, h_front4_anim);
}


//////////////////////////////////////////////////////////////////////////////
static void att_late_hit( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_late_hit: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// ignore this if we're on the same team
	if( att->team == def->team )
		return;

	// clear attacker's attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

	crowd_tackle_snds();
	late_hit_speech();

	if (def->odata.adata[0].seq_index == (SEQ_M_WOB_FU))
		change_anim( (obj_3d *)def, m_cnv_fu1_anim);
	else if (def->odata.adata[0].seq_index == (SEQ_M_WOB_FD))
		change_anim( (obj_3d *)def, m_cnv_fd2_anim);
}

//////////////////////////////////////////////////////////////////////////////
static void att_spear_head( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
	int		rnd,spin;
	char	**anim_tbl;
	fbplyr_data *puppet = def->puppet_link;


	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_punch: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// ignore this if we're on the same team
	if( att->team == def->team )
		return;

	// clear MY attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	// slow ME down after make contact
	att->odata.vx *= .7f;
	att->odata.vz *= .7f;

	// if victim is in spin move...1st time ignore most hits...2nd time very hittable...etc
	// !!!FIX (turmell factor this)
	if (def->odata.adata[0].seq_index == (SEQ_SPIN))
	{
		spin = game_info.spin_cnt[def->team];
		if (spin >= 3)
		{
			abusing_spin = 1;
			spin = 3;
		}
		if (FIELDX(def->odata.x) > 80)
		{	// player spinning in red zone
			if (randrng(100) < redzone_avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
		else
		{	// player spinning elsewhere
			if (randrng(100) < avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
	}
	else
		abusing_spin = 0;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

	// if victim is ON GROUND...lying down... no new anims
	// if player on/near ground...dont hit unless LATE HITABLE	
	if ((def->odata.y + def->odata.ay) < 2.0f)
		return;

	// if hit the DUDE being DRAGGED...ignore
	if (puppet != NULL)
	{
		if (puppet->odata.flags & PF_DRAGGING_PLR)
			return;
	}
	
	// if ball carrier is dragging a player...knock'em down... and clear puppet shit!!
	if (def->odata.flags & PF_DRAGGING_PLR)
	{
		def->odata.flags &= ~PF_DRAGGING_PLR;
		def->puppet_rel_time = pcount;
		def->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER|MODE_NOCYL);
		def->puppet_link->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER|MODE_NOCYL);
		change_anim((obj_3d *)def->puppet_link ,h_bsider_anim);
		def->puppet_link->puppet_link = NULL;
		def->puppet_link = NULL;

//		def->odata.flags &= ~PF_DRAGGING_PLR;
//		puppet->puppet_link = NULL;
//		puppet->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);
//		change_anim((obj_3d *)puppet ,h_bsider_anim);
//		def->puppet_link = NULL;
//		def->puppet_rel_time = pcount;
//		def->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);

		// sometimes... let dive move RE_ATTACH
		if (randrng(100) > 10)
		{
			change_anim((obj_3d *)def ,drag_hit_tbl[randrng(sizeof(drag_hit_tbl)/sizeof(char *))]);
			return;
		}
	}

	//	if victim is busy...ignore
	if (((obj_3d *)def)->adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))
		return;

	// if attacker hits ball carrier below knees...do % stumble run or flail
	if ((att->odata.y <= 5.0f) &&
		(fsqrt(def->odata.vx*def->odata.vx + def->odata.vz*def->odata.vz) > 1.20f) &&
		(randrng(100) < 15) &&
		!(def->odata.flags & PF_BACKPEDAL) &&
		!(game_info.game_flags & GF_PLAY_ENDING))
	{
		change_anim((obj_3d *)def ,stumble_anim_tbl[randrng(sizeof(stumble_anim_tbl)/sizeof(char *))]);
		crowd_cheer_snds();
		return;
	}

	// if ball carrier...and on fire...go into this move!!!
	if ((def->plyrnum == game_info.ball_carrier) &&
		(def->plyrnum == tdata_blocks[def->team].last_receiver) &&
		(def->odata.plyrmode != MODE_QB) &&
		(!(def->odata.flags & PF_CELEBRATE)) &&
		(!(def->odata.flags & PF_DRAGGING_PLR)) &&
		(game_info.game_mode == GM_IN_PLAY) &&
		(game_info.team_fire[def->team]))
		
	{
		att->puppet_link = def;
		def->puppet_link = att;

		def->odata.flags |= PF_DRAGGING_PLR;

		if (randrng(100) < 50)
		{
			change_anim((obj_3d *)att, p_run_drag2_a_anim);
			change_anim((obj_3d *)def, p_run_drag2_v_anim);
			monkey_on_back_sp();
		}
		else
		{
			change_anim((obj_3d *)att, p_run_drag_a_anim);
			change_anim((obj_3d *)def, p_run_drag_v_anim);
			monkey_on_back_sp();
		}
		return;
	}

	// will QB bitch slap tackler ?
	if ((def->odata.plyrmode == MODE_QB) &&
		(randrng(100) < 90) &&
		(game_info.play_time <= 2) &&
		(!qb_evaded) &&
		(ISDRONE(def)) &&
		(game_info.play_type == PT_NORMAL) &&
		(!(def->odata.adata[0].animode & MODE_UNINT)))
	{
		qb_evaded = 1;
		// clear defender's attack mode
		att->attack_mode = 0;
		att->attack_time = 0;
		change_anim((obj_3d *)def, (def->odata.flags & PF_FLIP) ? stiff_arm2_f_anim : stiff_arm2_anim);
		snd_scall_bank(plyr_bnk_str,OUTTA_MY_FACE_SP,VOLUME3,127,LVL3);
		def->odata.flags &= ~PF_THROWING;
		return;
	}
	// will QB duck ?
	else if ((def->odata.plyrmode == MODE_QB) &&
			 (randrng(100) < qb_stiff_arm_ht[PLYRH(def)]) &&
			 (game_info.play_time < 6) &&
			 (game_info.play_type == PT_NORMAL) &&
			 (!(def->odata.adata[0].animode & MODE_UNINT)) &&
			 (!qb_evaded))
	{
		qb_evaded = 1;
		// clear defender's attack mode
		att->attack_mode = 0;		// only do for DUCKING
		att->attack_time = 0;
		change_anim((obj_3d *)def, (def->odata.flags & PF_FLIP) ? duck_flip_anim : duck_anim);
		snd_scall_bank(plyr_bnk_str,OUTTA_MY_FACE_SP,VOLUME3,127,LVL3);
		def->odata.flags &= ~PF_THROWING;
	   	qb_duck_speech();
		return;
	}

	maybe_pop_helmet((obj_3d *)def);

	// get animation table ptr. based on direction
	anim_tbl = trb_hit_tbls[dir1];

	// get number of animations
	for (rnd=0; anim_tbl[rnd]; rnd++);

	// get a random reaction seq.
	rnd = randrng(rnd);

	if ((def->plyrnum == ball_obj.int_receiver) &&
//		ISHUMAN(att) &&
		!(ball_obj.flags & BF_BOUNCE) &&
		(game_info.game_mode == GM_IN_PLAY) &&
		(ball_obj.type == LB_PASS))
	{
		hit_intended_receiver_sp();
	}
	
	// start turbo-tackle animations
	change_anim( (obj_3d *)def, anim_tbl[rnd] );

	// if victim is carrier and keeps ball, mark end of his progress
	if(( def->plyrnum == game_info.ball_carrier ) &&
		( game_info.game_mode == GM_IN_PLAY ) &&
		!( game_info.game_flags & GF_PLAY_ENDING ))
	{
		if( !fumble_or_bobble( def, att, 65, 560 ))
		{
			// inc stat.
			if (!ISDRONE(att))
				tdata_blocks[att->team].tackles++;
		
			// Set arrows-off flags
			arrowinfo[0].ai_flags |= AF_INACTIVE;
			arrowinfo[1].ai_flags |= AF_INACTIVE;
			arrowinfo[2].ai_flags |= AF_INACTIVE;
			arrowinfo[3].ai_flags |= AF_INACTIVE;
		
			// check for TD regardless of AIR_CATCH bit
			if( td_check(TDC_NOCONTROL|TDC_NOAIRCATCH) && (( att->odata.y < 0.2f ) || ( def->odata.y < 0.2f )))
			{
				game_info.game_mode = GM_PLAY_OVER;
				game_info.play_end_cause = PEC_CAR_TOUCHDOWN;
			}
//			qcreate("whistle",0, whistle_proc, 0, 0, 0, 0);
			game_info.play_end_pos = def->odata.x;
			game_info.play_end_z = def->odata.z;
			game_info.game_flags |= GF_PLAY_ENDING;
#ifdef PLAY_DEBUG
			fprintf( stderr, "  carrier %2x(%d) hit, going down\n", JERSEYNUM(def->plyrnum), def->team );
			fprintf( stderr, "  play_end_pos set(4) to %5.2f, field %d\n", game_info.play_end_pos, FIELDX(game_info.play_end_pos) );
#endif
#ifndef NOSETCAM
			select_csi_item( csi_fs_endplay );
			cambot.pmodeobj = &(def->odata);
#endif
			crowd_tackle_snds();
//			update_onfire_stuff(att, def);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void att_swipe( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
#if 0
	int spin;

	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_swipe: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// ignore this if we're on the same team
	if( att->team == def->team )
		return;


	// clear attacker's attack mode
	att->attack_mode = 0;
	att->attack_time = 0;

	// if victim is in spin move...1st time ignore most hits...2nd time very hittable...etc
	if (def->odata.adata[0].seq_index == (SEQ_SPIN))
	{
		spin = game_info.spin_cnt[def->team];
		if (spin >= 3)
		{
			abusing_spin = 1;
			spin = 3;
		}
		if (FIELDX(def->odata.x) > 80)
		{	// player spinning in red zone
			if (randrng(100) < redzone_avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
		else
		{	// player spinning elsewhere
			if (randrng(100) < avoid_tackle_percent[spin])
				return;					// no reactions, just cyclinder interactions
		}
	}
	else
		abusing_spin = 0;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
static void att_head( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
	char *reactions[] = { h_heli1_anim, h_blindr_anim, h_back_anim, h_blindl_anim };
	fbplyr_data *puppet = def->puppet_link;


	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_push: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// ignore this if we're on the same team
//	if( att->team == def->team )
//		return;

	// dont allow player to be hit if hes lying on the ground
	if ((def->odata.y + def->odata.ay) < 2.0f)
		return;

	// if defender is involved in a puppet move, quit
	if( ((obj_3d *)def)->adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))
		return;

	// if hit the DUDE being DRAGGED...ignore
	if (puppet != NULL)
	{
		if (puppet->odata.flags & PF_DRAGGING_PLR)
			return;
	}

	// clear attacker's attack mode (DONT DO...cause i want to hit multiple dudes!!)
//	att->attack_mode = 0;
//	att->attack_time = 0;

	// clear defender's attack mode
	def->attack_mode = 0;
	def->attack_time = 0;

	if (!plowhd_snd)
	{
		plowhd_snd = 1;
		snd_scall_bank(plyr_bnk_str,BOWLING_PIN_SND,VOLUME4,127,PRIORITY_LVL4);
		qcreate ("plow", 0, delay_plow_proc, 0,0,0,0);
	}

	if ((randrng(100) < 55) &&
		!(game_info.game_flags & GF_PLAY_ENDING))
		change_anim((obj_3d *)def ,flail_anim_tbl[randrng(sizeof(flail_anim_tbl)/sizeof(char *))]);
	else
		change_anim( (obj_3d *)def, reactions[dir1] );
}


//////////////////////////////////////////////////////////////////////////////
static void att_hurdle( fbplyr_data *att, fbplyr_data *def, int dir1, int dir2 )
{
#if 0
	char *reactions[] = { h_heli1_anim, h_blindr_anim, h_back_anim, h_blindl_anim };

	if(( !att ) || ( !def ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in att_push: %p %p\n", att, def );
		lockup();
#else
		while(1) ;
#endif
	}

	// ignore this if we're on the same team
	if( att->team == def->team )
		return;

	// dont allow player to be hit if hes lying on the ground
	if ((def->odata.y + def->odata.ay) < 2.0f)
		return;

	// if defender is involved in a puppet move, quit (just cyclinder)
	if( ((obj_3d *)def)->adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))
		return;

	// allow diver to hit me outta my hurdle...this helps the defense
	// !!!FIX (turmel factor this)
	if ((randrng(100) < 55) &&
		((def->attack_mode == ATTMODE_DIVE) || (def->attack_mode == ATTMODE_SPEAR_HEAD)))
	{
		// clear attacker's attack mode
		att->attack_mode = 0;
		att->attack_time = 0;
		change_anim( (obj_3d *)att, reactions[dir1] );
		return;
	}

	// clear defender's attack mode (just have him cylinder off me)
	def->attack_mode = 0;
	def->attack_time = 0;
#endif
}


//////////////////////////////////////////////////////////////////////////////
// return false to fall through to cylinder interaction, else true
static int evade_vs_tackle( fbplyr_data *carrier, fbplyr_data *tackler,
		int dir1, int dir2 )
{
	int		rnd,pup_prcnt,bob_percent,fumble_percent;
	int		brng,spin;
	char	**anim_tbl;
	fbplyr_data *puppet = carrier->puppet_link;

	if(( !carrier ) || ( !tackler ))
	{
#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in evade_vs_tackle: %p %p\n",
				carrier, tackler );
		lockup();
#else
		while(1) ;
#endif
	}

	// if attacker is UNINT, quit
	if( tackler->odata.adata[0].animode & MODE_UNINT )
		return FALSE;

	// if hit the DUDE being DRAGGED...ignore
//	if (puppet != NULL)
//	{
//		if (puppet->odata.flags & PF_DRAGGING_PLR)
//			return FALSE;
//	}

	// bust-up guy carrying another player
	if (carrier->odata.flags & PF_DRAGGING_PLR)
	{
		carrier->odata.flags &= ~PF_DRAGGING_PLR;
		carrier->puppet_rel_time = pcount;
		carrier->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER|MODE_NOCYL);
		carrier->puppet_link->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER|MODE_NOCYL);
		change_anim((obj_3d *)carrier->puppet_link ,h_bsider_anim);
		carrier->puppet_link->puppet_link = NULL;
		carrier->puppet_link = NULL;

//		carrier->odata.flags &= ~PF_DRAGGING_PLR;
//		puppet->puppet_link = NULL;				// guy being dragged
//		puppet->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);
//		change_anim((obj_3d *)puppet ,h_bsider_anim);
//		carrier->puppet_link = NULL;
//		carrier->puppet_rel_time = pcount;
//		carrier->odata.adata[0].animode &= ~(MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER);

		change_anim((obj_3d *)carrier ,drag_hit_tbl[randrng(sizeof(drag_hit_tbl)/sizeof(char *))]);
		return TRUE;
	}

	// if defender is REACTING or involved in a puppet move, quit
 	if (carrier->odata.adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))
		return FALSE;

	// clear attacker's attack mode
	tackler->attack_mode = 0;
	tackler->attack_time = 0;
	
	// clear defender's attack mode
	carrier->attack_mode = 0;
	carrier->attack_time = 0;
		
	// if carrier in spin move...sometimes flail attacker!!	
	if (carrier->odata.adata[0].seq_index == (SEQ_SPIN))
	{
		spin = game_info.spin_cnt[carrier->team];
		if (spin >= 3)
		{
			abusing_spin = 1;
			spin = 3;
		}
		if (FIELDX(carrier->odata.x) > 80)
		{	// player spinning in red zone
			if (randrng(100) < redzone_avoid_tackle_percent[spin])
			{
				change_anim((obj_3d *)tackler ,flail_anim_tbl[randrng(sizeof(flail_anim_tbl)/sizeof(char *))]);
				return TRUE;
			}
		}
		else
		{	// player spinning elsewhere
			if (randrng(100) < avoid_tackle_percent[spin])
			{
				change_anim((obj_3d *)tackler ,flail_anim_tbl[randrng(sizeof(flail_anim_tbl)/sizeof(char *))]);
				return TRUE;
			}
		}
//		if (randrng(100) < avoid_tackle_percent[spin])
//		{
//			change_anim((obj_3d *)tackler ,flail_anim_tbl[randrng(sizeof(flail_anim_tbl)/sizeof(char *))]);
//			return TRUE;					// no reactions, just cyclinder interactions
//		}
	}
	else
		abusing_spin = 0;
	

	// will QB bitch slap tackler ?
	if ((carrier->odata.plyrmode == MODE_QB) &&
		(randrng(100) < 90) &&
		(game_info.play_time <= 2) &&
		(!qb_evaded) &&
		(ISDRONE(carrier)) &&
		(game_info.play_type == PT_NORMAL) &&
		(!(carrier->odata.adata[0].animode & MODE_UNINT)))
	{
		qb_evaded = 1;
		// clear defender's attack mode
		change_anim((obj_3d *)carrier, (carrier->odata.flags & PF_FLIP) ? stiff_arm2_f_anim : stiff_arm2_anim);
		snd_scall_bank(plyr_bnk_str,OUTTA_MY_FACE_SP,VOLUME3,127,LVL3);
		carrier->odata.flags &= ~PF_THROWING;
		return TRUE;
	}

	// if player on/near ground...dont hit unless LATE HITABLE	
	if (((carrier->odata.y + carrier->odata.ay) < 2.0f) && !(carrier->odata.flags & PF_LATEHITABLE))
		return FALSE;
	
	// dont allow to be hit again, unless...late hit-able or DIZZY
//	if ((game_info.game_mode == GM_PLAY_OVER) && (!(carrier->odata.flags & PF_LATEHITABLE)) && (!(carrier->odata.flags & PF_DIZZY)))
//		return FALSE;


	// if ball carrier...and on fire...go into this move!!!
	if ((carrier->plyrnum == game_info.ball_carrier) &&
		(carrier->plyrnum == tdata_blocks[carrier->team].last_receiver) &&
		(carrier->odata.plyrmode != MODE_QB) &&
		(!(carrier->odata.flags & PF_CELEBRATE)) &&
		(!(carrier->odata.flags & PF_DRAGGING_PLR)) &&
		(game_info.game_mode == GM_IN_PLAY) &&
		(game_info.team_fire[carrier->team]))
	{
		tackler->puppet_link = carrier;
		carrier->puppet_link = tackler;

		carrier->odata.flags |= PF_DRAGGING_PLR;

		if (randrng(100) < 50)
		{
			change_anim((obj_3d *)tackler, p_run_drag2_a_anim);
			change_anim((obj_3d *)carrier, p_run_drag2_v_anim);
			monkey_on_back_sp();
		}
		else
		{
			change_anim((obj_3d *)tackler, p_run_drag_a_anim);
			change_anim((obj_3d *)carrier, p_run_drag_v_anim);
			monkey_on_back_sp();
		}
		return TRUE;
	}

	// legal tackle.  Smack him.
	if (carrier->odata.flags & PF_LATEHITABLE)
	{
//		if (tackler->plyrnum == game_info.plyr_control[tackler->team])
		if (ISHUMAN(tackler))
		{
			if (pup_violence)
				change_anim((obj_3d *)tackler, violent_late_hits_tbl[ randrng( sizeof(violent_late_hits_tbl)/sizeof(char *) )] );
			else	
				change_anim((obj_3d *)tackler, late_hits_tbl[ randrng( sizeof(late_hits_tbl)/sizeof(char *) )] );
		}
		return TRUE;
	}

	// this is the bearing from def to att
	brng = bearing( tackler->plyrnum, carrier->plyrnum );

	if (brng >= 180)				// if players aren't facing each other....
		return FALSE;
	
	// 15% of time do puppet moves
	// TEMP TEMP
	pup_prcnt = 20;
//	pup_prcnt = 900;

	// carrier the qb ? if so, do puppet moves way more often
	if (carrier->odata.plyrmode == MODE_QB)
		pup_prcnt += 35;

	if (FIELDX(carrier->odata.x) >= 95)
		pup_prcnt = 75;
		
	if (carrier->odata.flags & PF_DIZZY)
		pup_prcnt = 90;

	// decide whether to do puppet move or normal move
	if (randrng(100) < pup_prcnt)
	{
		// clear dizzy flag
 		carrier->odata.flags &= ~PF_DIZZY;

		// set links
		carrier->puppet_link = tackler;
		tackler->puppet_link = carrier;



		// argue if play is over 10% of the time!
		if ((game_info.game_mode == GM_PLAY_OVER) && 
			(randrng(100) < 40))
		{
			change_anim((obj_3d *)tackler, p_argue_a_anim);
			change_anim((obj_3d *)carrier, p_argue_v_anim);
		} else {
			// get puppet table ptr. based on direction
//			fprintf(stderr, "%d - dir = %d\n", pup_violence, dir1);
			if (pup_violence)
				anim_tbl = violent_puppet_mv_tbls[dir1];
			else	
				anim_tbl = puppet_mv_tbls[dir1];

			// get number of animations
			for (rnd=0; anim_tbl[rnd*2]; rnd++);
//			fprintf(stderr, "table size = %d\n", rnd);

			// get a random puppet seq.
			rnd = randrng(rnd);

			// start puppet moves
			change_anim((obj_3d *)tackler, anim_tbl[rnd*2]);
			change_anim((obj_3d *)carrier, anim_tbl[(rnd*2)+1]);
		}
	}
	else if ((randrng(100) < 10) &&
			 ((carrier->odata.y + carrier->odata.ay) < 12.0f) &&
			 !(game_info.game_flags & GF_PLAY_ENDING))
	{	// 10% of time do flail as a reaction to a hit
		change_anim((obj_3d *)carrier ,flail_carrier_anim_tbl[randrng(sizeof(flail_carrier_anim_tbl)/sizeof(char *))]);
		return TRUE;
	}
	else if (tackler->odata.flags & PF_TURBO)
	{
		// get animation table ptr. based on direction
		anim_tbl = trb_hit_tbls[dir1];

		// get number of animations
		for (rnd=0; anim_tbl[rnd]; rnd++);

		// get a random reaction seq.
		rnd = randrng(rnd);

		// start turbo-tackle animations
		if (randrng(100) < 65)
			change_anim( (obj_3d *)tackler, t_dshoul_anim );
		else
			change_anim( (obj_3d *)tackler, m_swipe_anim );
		
		
		change_anim( (obj_3d *)carrier, anim_tbl[rnd] );
	}
	else
	{
		// get animation table ptr. based on direction
		anim_tbl = std_hit_tbls[dir1];

		// get number of animations
		for (rnd=0; anim_tbl[rnd]; rnd++);

		// get a random reaction seq.
		rnd = randrng(rnd);

		// start non-turbo-tackle animations
		change_anim( (obj_3d *)tackler, t_stshld_anim );
		change_anim( (obj_3d *)carrier, anim_tbl[rnd] );
  	}

	// victim is ball carrier, so mark end of his progress
	if(( game_info.game_mode == GM_IN_PLAY ) &&
		!( game_info.game_flags & GF_PLAY_ENDING ))
	{
		if( !fumble_or_bobble( carrier, tackler, 50, 290 ))
		{
			// inc stat.
			if (!ISDRONE(tackler))
				tdata_blocks[tackler->team].tackles++;
		
			// Set arrows-off flags
			arrowinfo[0].ai_flags |= AF_INACTIVE;
			arrowinfo[1].ai_flags |= AF_INACTIVE;
			arrowinfo[2].ai_flags |= AF_INACTIVE;
			arrowinfo[3].ai_flags |= AF_INACTIVE;
		
			// check for TD regardless of AIR_CATCH bit
			if( td_check(TDC_NOCONTROL|TDC_NOAIRCATCH) && (( tackler->odata.y < 0.2f ) || ( carrier->odata.y < 0.2f )))
			{
				game_info.game_mode = GM_PLAY_OVER;
				game_info.play_end_cause = PEC_CAR_TOUCHDOWN;
			}

//			qcreate("whistle",0, whistle_proc, 0, 0, 0, 0);
			game_info.play_end_pos = carrier->odata.x;
			game_info.play_end_z = carrier->odata.z;
			game_info.game_flags |= GF_PLAY_ENDING;
#ifdef PLAY_DEBUG
			fprintf( stderr, "  carrier %2x(%d) hit, going down\n", JERSEYNUM(carrier->plyrnum), carrier->team );
			fprintf( stderr, "  play_end_pos set(6) to %5.2f, field %d\n", game_info.play_end_pos, FIELDX(game_info.play_end_pos) );
#endif
#ifndef NOSETCAM

			// special case..NO CAMERA movement on these moves!
			if ((carrier->odata.adata[0].seq_index != (SEQ_P_FLIP_V)) &&
				(carrier->odata.adata[0].seq_index != (SEQ_JERSY_V)) &&
				(carrier->odata.adata[0].seq_index != (SEQ_CLOTHS_V)))
			{
				select_csi_item( csi_fs_endplay );
				cambot.pmodeobj = &(carrier->odata);
			}
#endif
			crowd_tackle_snds();
//			update_onfire_stuff(tackler, carrier);
		}
	}
	return TRUE;
}


#if 0
//////////////////////////////////////////////////////////////////////////////
void update_onfire_stuff( fbplyr_data *tackler, fbplyr_data *carrier)
{

	//	DEFENSE
	if ((carrier->odata.plyrmode == MODE_QB) && (tackler->team != game_info.off_side))
	{
		// tackled qb... turn off offensive fire and also check for defensive fire
		game_info.team_fire[!tackler->team] = 0;
		tdata_blocks[!tackler->team].consecutive_comps = 0;
		tdata_blocks[!tackler->team].last_receiver = -1;

		// now check for defensive fire
		if ((++tdata_blocks[tackler->team].consecutive_sacks == DEFENSE_FIRECNT) &&
			(game_info.team_fire[tackler->team] == 0))
		{
			if ((in_end_zone((obj_3d *)carrier) == 0) && (near_my_endzone((obj_3d *)carrier)  == 0))
				qcreate("message", MESSAGE5_PID, show_sack_proc, tdata_blocks[tackler->team].consecutive_sacks, 0, 0, 0);		// Double
			game_info.team_fire[tackler->team] = 1;
//			on_fire_sp();
//			change_anim( (obj_3d *)ppdata, taunt_4_anim );
		}
		else
			if ((in_end_zone((obj_3d *)carrier) == 0) && (near_my_endzone((obj_3d *)carrier)  == 0))
			{
				qcreate("message", MESSAGE5_PID, show_sack_proc, tdata_blocks[tackler->team].consecutive_sacks, 0, 0, 0);
//				on_fire_sp();
			}
	}
	else
	{
		// clear defensive fire count
		tdata_blocks[tackler->team].consecutive_sacks = 0;
	}
}	
#endif

//////////////////////////////////////////////////////////////////////////////
static int fumble_or_bobble( fbplyr_data *victim, fbplyr_data *attacker,
		int bobble_base, int fumble_base )
{
	// note: pbobble is chance in one HUNDRED of a bobble.  pfumble is
	// the chances in TEN THOUSAND of a fumble. (500 = 5%)
	int		pbobble, pfumble, phigh;
	int		up_by, okbase;
	
	// quit if victim isn't ball carrier
	if( game_info.ball_carrier != victim->plyrnum )
		return FALSE;

	// quit if game_mode isn't IN_PLAY
	if( game_info.game_mode != GM_IN_PLAY )
		return FALSE;
		
	up_by = compute_score(game_info.off_side) - compute_score(!game_info.off_side);
	
	#ifdef HIT_DEBUG
	fprintf( stderr, "  HIT> bbase: %d  fbase: %d\n", bobble_base, fumble_base );
	#endif
	
	switch( ball_obj.type )
	{
		case LB_PASS:
		case LB_BOBBLE:
		case LB_LATERAL:
		case LB_KICKOFF:
		case LB_FUMBLE:
			#ifdef HIT_DEBUG
			fprintf( stderr, "  HIT> attmode bobble pct: %d\n", bobble_base );
			#endif
			pbobble = bobble_base;
			
			if (ISHUMAN(attacker))
			{
				// human making the hit
				pbobble = GREATER(pbobble,bobble_human_ht[PLYRH(attacker)]);
			}
			else if (ISHUMANTEAM(attacker))
			{
				// drone teammate of human
				pbobble = GREATER(pbobble,bobble_hteam_ht[PLYRH(attacker)]);
			}
			else
			{
				// drone team player
				pbobble = GREATER(pbobble,PLYRH(attacker)[bobble_drone_ht]);
			}
			#ifdef HIT_DEBUG
			fprintf( stderr, "  HIT> adj(1) bobble pct: %d\n", pbobble );
			#endif
			
			// give it up a lot on onside kicks
			if (game_info.game_flags & GF_ONSIDE_KICK)
			{
				pbobble += 30;
			}
			pbobble -= (pcount - ball_obj.catch_pcount );
			#ifdef HIT_DEBUG
			fprintf( stderr, "  HIT> adj(2) bobble pct: %d\n", pbobble );
			#endif
						
			// in any case, no bobbling after CONTROL_TIME ticks of possession.
			if((pcount - ball_obj.catch_pcount) >= CONTROL_TIME )
			{
				pbobble = 0;
			}
			#ifdef HIT_DEBUG
			fprintf( stderr, "  HIT> adj(3) bobble pct: %d\n", pbobble );
			#endif

			// if defense is on fire, increase bobble percentage.
			// Either double the chance of bobbling, or halve the chance of
			// not bobbling, whichever is less
			if ((game_info.team_fire[!game_info.off_side]) &&
				(game_info.off_side == victim->team))
			{
				pbobble = LESSER((pbobble*2),(pbobble+(100-pbobble)/2));
			}
			// cap pbobble based on difficulty
			pbobble = LESSER(pbobble,bob_cap_ht[PLYRH(victim)]);
			#ifdef HIT_DEBUG
			fprintf( stderr, "  HIT> adj(4) bobble pct: %d\n", pbobble );
			#endif

			break;
			
		default:
			pbobble = 0;
			break;
	}
	
	pfumble = fumble_base;
	
	// get chance of ball going up on a bobble
	phigh = 100 * bob_up_ht[PLYRH(victim)];
	
	// if victim is airborne, boost chances of fumble
	if (victim_y > 0.0f)
	{
		pfumble *= 3;
		pfumble /= 2;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> airborne victim, y=%7.2f, pfumble = %d\n", victim_y, pfumble );
		#endif
	}

	// if AIR_CATCH bit is set, fumbles are really bobbles.
	if ((game_info.game_flags & GF_AIR_CATCH) &&
		(pfumble > (pbobble*100)))
	{
		pbobble = pfumble/100;
		pfumble = 0;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> air_catch is set, pbobble=%d, pfumble=%d\n",
			pbobble, pfumble );
		#endif
	}

	// if we're on the opening kickoff of either half, halve chance of fumble
	if ((game_info.game_flags & GF_OPENING_KICK) &&
		!(game_info.game_flags & GF_ONSIDE_KICK))
	{
		pfumble /= 50;
		phigh /= 3;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> opening kickoff, pfumble=%d\n", pfumble );
		#endif
	}
	
	// if we're losing, reduce fumble chance, more if in second half
	if ((up_by < 0) && !pup_noassist )
	{
		pfumble *= 2;
		pfumble /= 3;
		phigh *= 2;
		phigh /= 3;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> losing, pfumble=%d\n", pfumble );
		#endif
		
		if (game_info.game_quarter > 1)
		{
			pfumble /= 4;
			phigh /= 4;
			#ifdef HIT_DEBUG
			fprintf( stderr, "  HIT> losing in second half, pfumble=%d\n",
				pfumble );
			#endif
		}
			
	}
	
	// if we're losing by eleven or more, never fumble
	if ((up_by <= -11) && !pup_noassist)
	{
		pfumble = 0;
		phigh /= 2;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> losing by 11 or more(%d), pfumble=%d\n",
			-up_by, pfumble );
		#endif
	}
	
	// if we're in the fourth period, we're winning by four or more, and
	// we're in enemy territory, boost fumble chance.
	if ((game_info.game_quarter > 2 ) &&
		(up_by >= 4) &&
		(!pup_noassist) &&
		(FIELDX(victim->odata.x) >= 50))
	{
		pfumble *= 5;
		pfumble /= 4;
		phigh *= 5;
		phigh /= 4;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> last period, winning by 4 or more(%d), "
			"in enemy territory(%d), pfumble = %d\n", -up_by, game_info.los,
			pfumble );
		#endif
	}
	
	// if we're in the fourth period, we're winning by eight or more, and 
	// we're in our own territory, double fumble chances.
	if(( game_info.game_quarter > 2 ) &&
		(!pup_noassist) && 
		( up_by >= 8 ) &&
		( FIELDX(victim->odata.x) < 50 ))
	{
		pfumble *= 5;
		pfumble /= 4;
		phigh *= 5;
		phigh /= 4;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> last period, winning by 8 or more(%d), i"
			"n own territory(%d), pfumble = %d\n", -up_by, game_info.los,
			pfumble );
		#endif
	}
		
	// if our last possession ended with an interception or lost fumble,
	// divide chances by 5.
	
	// if we've lost more than one fumble, or more than opponent, reduce
	// chance of fumble
	if ((tdata_blocks[victim->team].lost_fumbles > 1) ||
		(tdata_blocks[victim->team].lost_fumbles > tdata_blocks[attacker->team].lost_fumbles))
	{
		pfumble /= 2;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> fumbled already this game, pfumble=%d\n", pfumble );
		#endif
	}
	
	// if we've had 4 or more lost fumbles and interceptions, divide
	// chances by 10.
	if ((tdata_blocks[victim->team].lost_fumbles +
		tdata_blocks[!victim->team].interceptions) >= 4)
	{
		pfumble /= 100;
		phigh /= 100;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> 4 or more(%d) turnovers, pfumble=%d\n",
			tdata_blocks[victim->team].lost_fumbles +
			tdata_blocks[victim->team].interceptions, pfumble );
		#endif
	}
	
	// outrageous cheating goes here
	if (drones_lose && drones_winning)
	{
//		int		human_pos = (p_status & (1<<game_info.off_side));
		int		human_pos = (p_status & TEAM_MASK(0)) ? 0 : 1;
		
		if (human_pos) 
		{
			// humans won't fumble or bobble (much) when drones are tanking
			pfumble /= 20;
			pbobble = LESSER(25,pbobble);
			phigh = 0;
		}
		else
		{
			pfumble = GREATER(2000,pfumble);
			if( pbobble )
				pbobble = 100;
			phigh = 10000;
		}
		
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> drones want to lose, pfumble=%d\n", pfumble );
		#endif
	}
	
	// if computer assistance is off, never fumble except for onside kicks and spin abuse
	if (pup_nofumbles)
		pfumble = 0;

	// on onside kicks, fumble often if you've caught the ball recently
	// always bobble up on onside kicks
	if (game_info.game_flags & GF_ONSIDE_KICK)
	{
		phigh = 10000;
		okbase = LESSER(5000,10000 - 200 * (pcount-ball_obj.catch_pcount));
		okbase *= 2;
		okbase /= (2 + game_info.fumbles_on_play);
		pfumble = GREATER(pfumble,okbase);
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> hit on onside kick after %d ticks, pfumble = %d\n",
			pcount-ball_obj.catch_pcount, pfumble);
		#endif
	}

	// fire receiver never bobbles
	if ((game_info.team_fire[victim->team]) &&
		(victim->team == game_info.off_side) &&
		(tdata_blocks[victim->team].last_receiver == victim->plyrnum))
	{
		pbobble = 0;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> Fire receiver never bobbles.  pbobble = 0\n" );
		#endif
	}

	// nobody on a fire team ever fumbles
	if (game_info.team_fire[victim->team])
	{
		pfumble = 0;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> Fire team never fumbles.  pfumble = 0" );
		#endif
	}

	// if you're abusing the spin, you die, fool.
	// drones are immune to this effect, but normally show
	// a little restraint with spins anyway.
	if ((game_info.spin_cnt[victim->team] > 2) &&
		(abusing_spin) &&
		(!ISDRONETEAM(victim)))
	{
		pfumble += 5000;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> abusing spin, pfumble=%d\n", pfumble );
		#endif
	}
	
	// if this is one of my first three plays, no fumbling
	if ((tdata_blocks[victim->team].off_plays < 4) &&
		(!ISDRONE(victim)))
	{
		pfumble = 0;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> off_plays = %d, pfumble=%d\n",
			tdata_blocks[victim->team].off_plays, pfumble );
		#endif
	}
	
	// super drones never fumble
	if (ISDRONETEAM(victim) && pup_superdrones)
		pfumble = 0;
	
	// if ball was a fumble, lateral, or kickoff, bobbles are really fumbles
	if (((ball_obj.type == LB_FUMBLE) ||
		(ball_obj.type == LB_LATERAL) ||
		(ball_obj.type == LB_PUNT) ||
		(ball_obj.type == LB_KICKOFF)) &&
		((pbobble*100) > pfumble ))
	{
		if (game_info.game_flags & GF_ONSIDE_KICK)
			pfumble = pbobble*100;
		else
			pfumble = LESSER(pbobble,20)*100;
		pbobble = 0;
		#ifdef HIT_DEBUG
		fprintf( stderr, "  HIT> fumble, lateral, or kickoff, pbobble=%d, pfumble=%d\n",
			pbobble, pfumble );
		#endif
	}
	
	if( force_fumble )
	{
		pfumble = 100000;
		force_fumble = FALSE;
	}

	if( force_bobble )
	{
		pbobble = 1000;
		phigh = 10000;
		force_bobble = FALSE;
	}
	
#ifdef HIT_DEBUG
	fprintf( stderr, "  HIT> pbobble/high: %d(%d)  pfumble: %d\n", pbobble, phigh, pfumble );
#endif
	
	// only check for the more likely of the two
	if( pfumble > (pbobble*100) )
	{
		if( randrng(10000) < pfumble )
		{
#ifdef PLAY_DEBUG
			fprintf( stderr, "  %2x(%d) takes hit and fumbles at %5.2f, field %d\n",
				JERSEYNUM(victim->plyrnum), victim->team, victim->odata.x,
				FIELDX(victim->odata.x) );
			fprintf( stderr, "  chance of fumble was %d/10000\n", pfumble );
#endif
			qcreate("message", MSG_FUMBLE_PID, display_fumble_proc, 0, 0, 0, 0);
			fumble_ball( victim, mvx/2.0f, mvz/2.0f );
			game_info.fumbles_on_play += 1;
			return TRUE;
		}
	}
	else
	{
		if( randrng(100) < pbobble )
		{

// Bug - no bobbles ever happen!
#if 0


			int is_p = (
				(ISHUMAN(victim)) &&
				(game_info.team_play[victim->team] == &custom_play) &&
				(
					((victim->position == PPOS_WPN1) && (custom_play.flags & PLF_PRI_RCVR1)) ||
					((victim->position == PPOS_WPN2) && (custom_play.flags & PLF_PRI_RCVR2)) ||
					((victim->position == PPOS_WPN3) && (custom_play.flags & PLF_PRI_RCVR3))
				));

			// If carrier is primary receiver, discard a % of bobbles
			if (is_p && randrng(100) < 15)
			{
#ifdef DEBUG
				fprintf( stderr, "  \007Primary Receiver bobble DISCARDED!\n" );
#endif
			}
			else
			{
#ifdef DEBUG
				if (is_p)
					fprintf( stderr, "  \007Primary Receiver bobble NOT DISCARDED!\n" ),
#endif






#endif




#ifdef PLAY_DEBUG
				fprintf( stderr, "  %2x(%d) takes hit and bobbles at %5.2f, field %d\n",
					JERSEYNUM(victim->plyrnum), victim->team, victim->odata.x,
					FIELDX(victim->odata.x) );
				fprintf( stderr, "  chance of bobble was %d/100\n", pbobble );
#endif
				bobble_pass( victim, mvx/1.5f, ((randrng(10000) > phigh) ? -1.0f : 0.0f), mvz/1.5f );
				return TRUE;


//			}






		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// return false to fall through to cylinder interaction, else true

typedef struct _block_result_t
{
	int		prob;
	int		puppet;
	char	*rusher_anim,*blocker_anim;
} block_result_t;

block_result_t	bt_block_total[] =
{
	{3,TRUE,tie1c_a_anim,tie1c_v_anim},
	{3,TRUE,tie2c_a_anim,tie2c_v_anim},
	{2,TRUE,tie3c_a_anim,tie3c_v_anim},
	{0,0,NULL,NULL},
};

block_result_t	bt_block_marginal[] =
{
	{1,TRUE,pushlg1v_anim,pushlg1a_anim},
	{1,FALSE,pushbl2_anim,h_flailm_anim},
	{0,0,NULL,NULL}
};

block_result_t	bt_nowhere[] =
{
	{1,FALSE,pushbl1_anim,pushbl1_anim},
	{1,FALSE,pushbl1_anim,pushbl2_anim},
	{2,FALSE,pushbl1_anim,pushbl3_anim},
	{3,FALSE,pushbl2_anim,pushbl1_anim},
	{1,FALSE,pushbl2_anim,pushbl2_anim},
	{2,FALSE,pushbl2_anim,pushbl3_anim},
	{1,FALSE,pushbl3_anim,pushbl1_anim},
	{2,FALSE,pushbl3_anim,pushbl2_anim},
	{1,FALSE,pushbl3_anim,pushbl3_anim},
	{2,TRUE,tie1c_a_anim,tie1c_v_anim},
	{2,TRUE,tie3c_a_anim,tie3c_v_anim},
	{0,0,NULL,NULL}
};

block_result_t  bt_rush_marginal[] = 
{
	{1,FALSE,pushbl1_anim,h_flailm_anim},
	{1,FALSE,pushbl2_anim,r_flaild_anim},
	{1,FALSE,pushbl2_anim,h_flailm_anim},
	{1,FALSE,pushbl3_anim,r_flaild_anim},
	{4,TRUE,pushlg1a_anim,pushlg1v_anim},
	{0,0,NULL,NULL}
};

block_result_t	bt_rush_total[] = 
{
//#ifndef SEATTLE
//	{1,TRUE,p_thrw2_a_anim,p_thrw2_v_anim},
//#endif
	{2,TRUE,spin_a_anim,spin_v_anim},
	{2,TRUE,p_asidea_anim,p_asidev_anim},
	{2,TRUE,p_asidfa_anim,p_asidfv_anim},
	{4,FALSE,NULL,h_flailm_anim},
	{0,0,NULL,NULL}
};

block_result_t bt_rush_total2[] =
{
	{1,FALSE,NULL,h_flailm_anim},
	{0,0,NULL,NULL}
};

//////////////////////////////////////////////////////////////////////////////
static int block_vs_rush( fbplyr_data *blocker, fbplyr_data *rusher, int dir1, int dir2 )
{
	int				result, pt,pa,i;
	block_result_t	*block_tables[] = { bt_block_total, bt_block_marginal,
			bt_nowhere, bt_rush_marginal, bt_rush_total, bt_rush_total2 };
				//off vs def
	int		line_vs_line[] = { 2,5,4,2,1,0 };
	int		line_vs_wimp[] = { 2,6,5,2,1,0 };
	int		wimp_vs_line[] = { 1,2,3,6,3,1 };
	int		wimp_vs_wimp[] = { 1,2,3,6,3,1 };		//wr vs. db typically
// TEST
//	int		any_vs_human[] = { 2,5,4,2,1,0 };
	int		any_vs_human[] = { 0,0,2,2,2,3 };
	int		*pprob;
	int		bc_adjust[] = { -1,1,1,2,3,4 };
	float	rb[2],rv[2], rbdrv;

	if(( !blocker ) || ( !rusher ))
	{
		#ifdef DEBUG
		fprintf( stderr, "Bad att/def pair in block_vs_rush: %p %p\n",
				blocker, rusher );
		lockup();
		#else
		while(1) ;
		#endif
	}

	// if blocker is unint & !CANBLOCK, quit
	if(( blocker->odata.adata[0].animode & MODE_UNINT ) &&
			!( blocker->odata.adata[0].animode & MODE_CANBLOCK))
		return FALSE;

	// if rusher is REACTING, PUPPET, or PUPPETEER, quit
	if( rusher->odata.adata[0].animode & (MODE_REACTING|MODE_PUPPET|MODE_PUPPETEER))
		return FALSE;
		
	// if rusher isn't moving more or less toward blocker, probably quit
	rb[0] = blocker->odata.x - rusher->odata.x;
	rb[1] = blocker->odata.z - rusher->odata.z;
	rv[0] = rusher->odata.vx;
	rv[1] = rusher->odata.vz;
	norm2( rb );
	norm2( rv );
	rbdrv = rb[0]*rv[0] + rb[1]*rv[1];
	
	// EXCEPTION: Human blocker can wrap up an attacker from any angle
	if ((rbdrv < 0.5f) &&
		ISDRONE(blocker) &&
		!((rv[0] == 0.0f) && (rv[1] == 0.0f)) &&
		!((rb[0] == 0.0f) && (rb[1] == 0.0f)))
		return FALSE;
	

	// choose table probability set
//	if ((rusher->plyrnum == game_info.plyr_control[rusher->team]) && (game_info.game_mode == GM_IN_PLAY))
	if (ISHUMAN(rusher))
		pprob = any_vs_human;
	else if( rusher->plyrnum % 7 < 3 )
	{
		if( blocker->plyrnum % 7 < 3 )
			pprob = line_vs_line;
		else
			pprob = wimp_vs_line;
	}
	else
	{
		if( blocker->plyrnum % 7 < 3 )
			pprob = line_vs_wimp;
		else
			pprob = wimp_vs_wimp;
	}

	// choose table
	// get total prob for table set
	pt = 0;
	for( i = 0; i < 6; i++ )
		pt += pprob[i];

	// get result
	pa = randrng(pt);
	result = -1;
	while( pa >= 0 )
		pa -= pprob[++result];

	// adjust result by block_count adjustment
	i = LESSER(rusher->block_count,(int)(sizeof(bc_adjust)/sizeof(int)));

	#ifdef BLOCK_DEBUG
	if (!ISDRONE(rusher))
	{
		fprintf( stderr, "\n==========\n%2x blocks %2x\n", JERSEYNUM(blocker->plyrnum), JERSEYNUM(rusher->plyrnum));
		fprintf( stderr, "table: %d %d %d %d %d %d\n", pprob[0],pprob[1],pprob[2],pprob[3],pprob[4],pprob[5] );
		fprintf( stderr, "block #%d(%d) for rusher\n", rusher->block_count, i );
		fprintf( stderr, "result: %d", result );
	}
	#endif

	// don't give negative column shifts to humans
	if (ISDRONE(rusher))
		result += bc_adjust[i];
	else
		result += GREATER(0,bc_adjust[i]);
	
	// give three column shifts to humans if drones are winning
	// and are supposed to tank
	if (drones_lose && drones_winning)
	{
		if (ISDRONETEAM(blocker))
			result += 3;
		else
			result -= 3;
	}
	
	// offensive line adjustment
	if (!ISDRONE(rusher))
		result -= oline_block_ht[OFFH];
	
	// powerup block & blitz checks
	if ((pup_block & (TEAM_MASK(game_info.off_side))) ||
		(game_info.team_fire[game_info.off_side]))
	{
//		fprintf( stderr, "block bonus\n" );
		result -= (1+randrng(2));
	}
		
	if ((pup_blitzers & (TEAM_MASK(!game_info.off_side))) ||
		(game_info.team_fire[!game_info.off_side]))
	{
//		fprintf( stderr, "blitz bonus\n" );
		result += (1+randrng(2));
	}

	// red zone adjustment (everyone blocks/rushes better inside their own 20)
	if (FIELDX(rusher->odata.x) >= 75)
		result += 2;
	if (FIELDX(blocker->odata.x) <= 25)
		result -= 2;

	if( result < 0 )
		result = 0;
	if( result > 5 )
		result = 5;
		
	// nobody blocks superman
	if (rusher->odata.flags & PF_UNBLOCKABLE)
	{
		if (game_info.team_play[!game_info.off_side]->flags & PLF_REDZONE)	// Smash thru the line!
  			result = GREATER(result,(randrng(2) ? 2 : 4));
		else
  			result = GREATER(result,(randrng(2) ? 4 : 5));
	}

	#ifdef BLOCK_DEBUG
	if (!ISDRONE(rusher))
	{
		fprintf( stderr, "  adjusted: %d\n==========\n", result );
		getchar();
	}
	#endif

	// get total prob for table
	pt = 0;
	for( i = 0; block_tables[result][i].prob; i++ )
		pt += block_tables[result][i].prob;

	// choose entry within table
	pa = randrng(pt);
	i = -1;

	while( pa >= 0 )
		pa -= block_tables[result][++i].prob;

	if (block_tables[result][i].puppet)
	{
		if(( rusher->odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER )) ||
		   ( blocker->odata.adata[0].animode & (MODE_PUPPET|MODE_PUPPETEER )))
		{
#ifdef DEBUG
			fprintf( stderr, "attempt to link to someone already in a puppet move.  tard!!!.\n" );
			lockup();
#else
			while(1) ;
#endif
		}

		rusher->puppet_link = blocker;
		blocker->puppet_link = rusher;
	}

	if (block_tables[result][i].rusher_anim != NULL)
		change_anim( (obj_3d *)rusher, block_tables[result][i].rusher_anim );
		
	if (block_tables[result][i].blocker_anim != NULL)
		change_anim( (obj_3d *)blocker, block_tables[result][i].blocker_anim );

	// inc rusher's block count
	rusher->block_count += 1;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//
//  		This process delays before speaking plow speech
//		
//			INPUT:  Nothing
//			OUTPUT:	Nothing
//
//-------------------------------------------------------------------------------------------------
void delay_plow_proc(int *args)
{
	sleep(50);
	head_plow_speech();
}

//////////////////////////////////////////////////////////////////////////////
