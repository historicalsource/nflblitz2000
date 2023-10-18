/****************************************************************************/
/*                                                                          */
/* playbook.c - Plays, formations, etc.                                     */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* Copyright (c) 1997 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include "include/playbook.h"
#include "include/plyrseq.h"
#include "include/game.h"
#include "include/player.h"

// *********************
// OFFENSIVE FORMATIONS
// *********************
fline_t formation_stack[] =
{
	{-12.0, -17.5,stance3pt_anim,	MODE_LINEPS,0},  					// left guard
	{-12.0,   0.0,center_set_anim,	MODE_LINEPS,0},						// center
	{-12.0,  17.5,stance3pt_anim,	MODE_LINEPS,0},  					// right guard
	{-30.0,   0.0,qbset_anim,		MODE_QBPS,	LU_CAPN},				// QB
	{-45.0,  60.0,wr_set_anim,	MODE_WPNPS,	LU_CENTER|LU_MOTION|LU_PLYR2},	// wr2
	{-15.0,  32.0,wr_set_anim,	MODE_WPNPS,	LU_LEFT},				// wr1
	{-30.5,  46.0,wr_set_anim,	MODE_WPNPS,	LU_RIGHT}, 				// rb
};

fline_t formation_sidei[] =
{
	{-12.0, -17.5,stance3pt_anim,	MODE_LINEPS,0},  					// left guard
	{-12.0,   0.0,center_set_anim,	MODE_LINEPS,0},						// center
	{-12.0,  17.5,stance3pt_anim,	MODE_LINEPS,0},  					// right guard
	{-35.0,   0.0,qbset_anim,		MODE_QBPS,	LU_CAPN},				// QB
	{-45.0,  33.0,wr_set_anim,	MODE_WPNPS,	LU_CENTER|LU_MOTION|LU_PLYR2},	// wr2
	{-15.0,  55.0,wr_set_anim,	MODE_WPNPS,	LU_LEFT},				// wr1
	{-30.5,  55.0,wr_set_anim,	MODE_WPNPS,	LU_RIGHT}, 				// rb
};

fline_t formation_tightends[] =
{
	{-12.0, -17.5,stance3pt_anim,	MODE_LINEPS,0},  					// left guard
	{-12.0,   0.0,center_set_anim,	MODE_LINEPS,0},						// center
	{-12.0,  17.5,stance3pt_anim,	MODE_LINEPS,0},  					// right guard
	{-18.0,   0.0,qbset_anim,		MODE_QBPS,	LU_CAPN},				// QB
	{-50.0,   9.0,wr_random_anim,	MODE_WPNPS,	LU_CENTER|LU_MOTION|LU_PLYR2},	// wr2
	{-15.0, -40.0,wr_random_anim,	MODE_WPNPS,	LU_LEFT},				// wr1
	{-15.5,  40.0,wr_random_anim,	MODE_WPNPS,	LU_RIGHT}, 				// rb
};

fline_t formation_2plus1[] =
{
	{-12.0, -17.5,stance3pt_anim,	MODE_LINEPS,0},  					// left guard
	{-12.0,   0.0,center_set_anim,	MODE_LINEPS,0},						// center
	{-12.0,  17.5,stance3pt_anim,	MODE_LINEPS,0},  					// right guard
	{-23.0,   0.0,qb_random_anim,	MODE_QBPS,	LU_CAPN},				// QB (shotgun)
	{-34.0, -55.0,wr_set_anim,		MODE_WPNPS,	LU_CENTER|LU_MOTION|LU_PLYR2},	// wr2
	{-15.0, -74.0,wr_random_anim,	MODE_WPNPS,	LU_LEFT},				// wr1
	{-15.5,  55.0,wr_random_anim,	MODE_WPNPS,	LU_RIGHT}, 				// rb
};

fline_t formation_1plus1plus1[] =
{
	{-12.0, -17.5,stance3pt_anim,	MODE_LINEPS,0},  		// left guard
	{-12.0,   0.0,center_set_anim,	MODE_LINEPS,0},			// center
	{-12.0,  17.5,stance3pt_anim,	MODE_LINEPS,0},  		// right guard
	{-38.0,   0.0,qb_random_sg_anim,MODE_QBPS,	LU_CAPN},	// QB (shotgun)
	{-23.0, -65.0,wr_set_anim,		MODE_WPNPS,	LU_PLYR2|LU_LEFT|LU_MOTION}, // wr1
	{-67.0, -25.0,wr_random_anim,	MODE_WPNPS,	LU_CENTER},			// wr2
	{-15.0,  55.0,wr_random_anim,	MODE_WPNPS,	LU_RIGHT},			// rb
};

fline_t formation_trips[] =
{
	{-12.0, -17.5,stance3pt_anim,	MODE_LINEPS,0},  			// left guard
	{-12.0,   0.0,center_set_anim,	MODE_LINEPS,0},				// center
	{-12.0,  17.5,stance3pt_anim,	MODE_LINEPS,0},  			// right guard
	{-38.0,   0.0,qb_random_sg_anim,MODE_QBPS,	LU_CAPN},		// QB
	{-23.0,  46.0,wr_set_anim,		MODE_WPNPS,	LU_PLYR2|LU_LEFT|LU_MOTION},	// RB
	{-15.0,  66.0,wr_random_anim,	MODE_WPNPS,	LU_CENTER},	  	// WR1
	{-15.0,  86.0,wr_random_anim,	MODE_WPNPS,	LU_RIGHT},	   	// WR2
};

fline_t formation_1wr2back[] =
{					
	{-12.0,-17.5,stance3pt_anim,	MODE_LINEPS,0},				// left guard
	{-12.0,  0.0,center_set_anim,	MODE_LINEPS,0},				// center
	{-12.0, 17.5,stance3pt_anim, 	MODE_LINEPS,0},				// right guard
	{-23.0,  0.0,qb_random_anim, 	MODE_QBPS,	LU_CAPN},		// QB
	{-34.0, 60.0,wr_set_anim,		MODE_WPNPS,	LU_PLYR2|LU_RIGHT|LU_MOTION},				// WR2
	{-42.5,-30.0,stance2pt_anim_rnd,MODE_WPNPS,	LU_LEFT},				// RB
	{-42.5, 30.0,stance2pt2_anim_rnd,MODE_WPNPS,	LU_CENTER},				// WR1
};

fline_t formation_4lineman[] =
{
	{-12.0,-17.5,stance3pt_anim,	MODE_LINEPS,0}, 					// left guard
	{-12.0,  0.0,center_set_anim,	MODE_LINEPS,0},						// center
	{-12.0, 17.5,stance3pt_anim,	MODE_LINEPS,0},		 				// right guard
	{-23.0,  0.0,qb_random_anim,	MODE_QBPS,	LU_CAPN}, 				// QB
	{-34.0,-70.0,wr_set_anim,		MODE_WPNPS,	LU_PLYR2|LU_LEFT|LU_MOTION},		// WR1 (TE)
	{-42.5,-26.5,stance2pt2_anim_rnd,MODE_WPNPS,	LU_CENTER},				// WR2
	{-15.0, 37.0,wr_random_anim,	MODE_WPNPS,	LU_RIGHT},				// WR3
};

fline_t formation_punts[] =
{
	{-12.0,-17.5,stance3pt_anim,	MODE_LINEPS,0},  				// left guard
	{-12.0,  0.0,center_set_anim,	MODE_LINEPS,0},					// center
	{-12.0, 17.5,stance3pt_anim,	MODE_LINEPS,0},  				// right guard
	{-99.0,  0.0,shotgun_anim,		MODE_QBPS,	LU_CAPN},			// kicker
	{-20.0, 40.0,wr_set_anim,		MODE_WPNPS,	LU_PLYR2|LU_RIGHT},			// rb
	{-20.0,-40.0,stance3pt_anim,	MODE_WPNPS,	LU_LEFT},		 	// wr1
	{-40.0,-25.0,stance3pt_anim,	MODE_WPNPS,	LU_CENTER},			// wr2
};

fline_t formation_field_goal[] =
{
	{-12.0,-17.5,stance3pt_anim,	MODE_LINEPS,0},  				// 
	{-12.0,  0.0,center_set_anim,	MODE_LINEPS,0},					// center
	{-12.0, 17.5,stance3pt_anim,	MODE_LINEPS,0},  				// right guard
	{-65.5,  9.0,qbhut_knee_anim,	MODE_QBPS,	LU_CAPN},			// qb
	{-34.0,-40.0,stance2pt2_anim_rnd,MODE_WPNPS,	LU_PLYR2|LU_LEFT}, 			// wr1
	{-20.0, 40.0,stance2pt_anim_rnd,MODE_WPNPS,	LU_RIGHT},			// wr2
	{-112.0,  0.0,wr_set_anim,		MODE_KICKER,LU_CENTER},			// kicker
};

// ********************
// DEFENSIVE FORMATIONS
// ********************
// Near Zone play
fline_t def_formation_a[] = 
{
	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,lm_random_anim,	MODE_LINEPS,0},	  		// LINE2
	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE3
	{-25.0, 0.0,db_random_anim,		MODE_DBPS,	LU_PLYR2|LU_RIGHT},	// DB1
	{-25.0,-40.0,db_random_anim,	MODE_DBPS,	LU_WEAK},	// DB2
	{-25.0, 0.0,db_random_anim,		MODE_DBPS,	LU_CAPN|LU_LEFT},	// DB3
	{-25.0, 0.0,db_random_anim,		MODE_DBPS,	LU_CENTER},	// DB4
};					

// Goal Line play
fline_t def_formation_b[] =
{
	{-11.0, -8.5,lm_random_anim,	MODE_LINEPS,0},		// LINE1
	{-11.0,  8.5,lm_random_anim,	MODE_LINEPS,0},		// LINE2
	{-25.0,-30.0,db_man2man_random_anim,	MODE_DBPS,	LU_CAPN},	// LINE3
	{-25.0, 30.0,db_man2man_random_anim,	MODE_DBPS,	LU_PLYR2},	// DB1
	{-13.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_LEFT},	// DB2
	{-13.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_RIGHT},	// DB3
	{-13.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_CENTER},	// DB4
};

fline_t def_formation_c[] = 
{
	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,lm_random_anim,	MODE_LINEPS,0},	  	// LINE2
	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
	{-25.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT},	// DB1
	{-60.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_CAPN},	// DB2
	{-25.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_CENTER},	// DB3
	{-25.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_RIGHT},	// DB4
};

fline_t def_formation_l[] = 
{
	{-11.0,-21.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,lm_random_anim,	MODE_LINEPS,0},	  	// LINE2
	{-11.0, 21.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
	{-45.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT},	// DB1
	{-80.0,  50.0,db_zone_random_anim,	MODE_DBPS,	LU_CAPN},	// DB2
	{-45.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_CENTER},	// DB3
	{-45.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_RIGHT},	// DB4
};

// This is medium Zone play
fline_t def_formation_d[] = 
{
	{-11.0,-8.5,lm_random_anim,	 	MODE_LINEPS,0},  		// LINE1
	{-11.0, 8.5,lm_random_anim,	 	MODE_LINEPS,0},	  		// LINE2
	{-40.0,-55.0,db_zone_random_anim, 	MODE_DBPS,  LU_CAPN},		// LINE3
	{-40.0,  0.0,db_zone_random_anim, 	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB1
	{-80.0,-22.0,db_zone_random_anim, 	MODE_DBPS,	LU_LEFT|LU_COVER},	// DB2
	{-80.0, 22.0,db_zone_random_anim, 	MODE_DBPS,	LU_RIGHT|LU_COVER},	// DB3
	{-40.0, 55.0,db_zone_random_anim, 	MODE_DBPS,	LU_PLYR2},	// DB4
};

// This is for Deep Zone play
fline_t def_formation_e[] = 
{
	{-11.0, -8.5,lm_random_anim,	MODE_LINEPS,0},		// LINE1
	{-11.0,  8.5,lm_random_anim,	MODE_LINEPS,0},		// LINE2
	{-90.0,-35.5,db_zone_random_anim,	MODE_DBPS,	LU_CAPN},	// LINE3
	{-90.0, 35.0,db_zone_random_anim,	MODE_DBPS,	LU_PLYR2},	// DB1
	{-150.0,-80.0,db_zone_random_anim,	MODE_DBPS,	LU_LEFT|LU_COVER},	// DB2
	{-150.0,  0.0,db_zone_random_anim,	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB3
	{-150.0, 80.0,db_zone_random_anim,	MODE_DBPS,	LU_RIGHT|LU_COVER},	// DB4
};

// Near zone
fline_t def_formation_f[] = 
{
	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,stance3pt_anim,	MODE_LINEPS,0},	  	// LINE2
	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
	{-80.0,-55.0,db_zone_random_anim,	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB1
	{-110.0,-22.0,db_zone_random_anim,	MODE_DBPS,	LU_CAPN|LU_RIGHT|LU_COVER}, 	// DB2
	{-110.0, 22.0,db_zone_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT|LU_COVER},	// DB3
	{-80.0, 55.0,db_zone_random_anim,	MODE_DBPS,	LU_COVER},	// DB4
};

// Left zone
fline_t def_formation_g[] = 
{
	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,stance3pt_anim,	MODE_LINEPS,0},	  	// LINE2
	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
	{-60.0,-55.0,db_zone_random_anim,	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB1
	{-80.0,-22.0,db_zone_random_anim,	MODE_DBPS,	LU_CAPN|LU_RIGHT|LU_COVER}, 	// DB2
	{-30.0, -35.0,db_zone_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT|LU_COVER},	// DB3
	{-40.0, 55.0,db_zone_random_anim,	MODE_DBPS,	LU_COVER},	// DB4
};

// Right zone
fline_t def_formation_h[] = 
{
	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,stance3pt_anim,	MODE_LINEPS,0},	  	// LINE2
	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
	{-60.0,55.0,db_zone_random_anim,	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB1
	{-80.0,22.0,db_zone_random_anim,	MODE_DBPS,	LU_CAPN|LU_RIGHT|LU_COVER}, 	// DB2
	{-30.0, 35.0,db_zone_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT|LU_COVER},	// DB3
	{-40.0, -55.0,db_zone_random_anim,	MODE_DBPS,	LU_COVER},	// DB4
};

fline_t def_formation_punt_return[] = 
{
	{-11.0,-17.5,stance3pt_anim,	MODE_LINEPS,0},		// LINE1
	{-11.0,  0.0,stance3pt_anim,	MODE_LINEPS,0},		// LINE2
	{-11.0, 17.5,stance3pt_anim,	MODE_LINEPS,0},		// LINE3
	{-60.0,-45.0,db_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT},		// DB1
	{-60.0, 45.0,db_random_anim,	MODE_DBPS,	LU_RIGHT},		// DB2
	{-110.0, 0.0,db_random_anim,	MODE_DBPS,	LU_CENTER}, 	// DB3
	{-310.0, 0.0,db_random_anim,	MODE_DBPS,	LU_CAPN},	// DB4
};

// Stop run
fline_t def_formation_i[] = 
{
	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,stance3pt_anim,	MODE_LINEPS,0},	  	// LINE2
	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
	{-40.0,-75.0,db_zone_random_anim,	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB1
	{-40.0,-42.0,db_man2man_random_anim,	MODE_DBPS,	LU_CAPN|LU_RIGHT|LU_COVER}, 	// DB2
	{-40.0, 42.0,db_zone_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT|LU_COVER},	// DB3
	{-70.0, 75.0,db_man2man_random_anim,	MODE_DBPS,	LU_COVER},	// DB4
};

// Stop run
fline_t def_formation_j[] = 
{
	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
	{-11.0,  0.0,stance3pt_anim,	MODE_LINEPS,0},	  	// LINE2
	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
	{-40.0,-75.0,db_zone_random_anim,	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB1
	{-40.0,-42.0,db_man2man_random_anim,	MODE_DBPS,	LU_CAPN|LU_RIGHT|LU_COVER}, 	// DB2
	{-40.0, 42.0,db_man2man_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT|LU_COVER},	// DB3
	{-80.0, 75.0,db_zone_random_anim,	MODE_DBPS,	LU_COVER},	// DB4
};

// Mix it up
fline_t def_formation_k[] = 
{
	{-11.0, -8.5,lm_random_anim,	MODE_LINEPS,0},		// LINE1
	{-11.0,  8.5,lm_random_anim,	MODE_LINEPS,0},		// LINE2
	{-35.0,  0.0,lm_random_anim,	MODE_LINEPS,0},	  	// LINE2
	{-30.0,-75.0,db_zone_random_anim,	MODE_DBPS,	LU_CENTER|LU_COVER},	// DB1
	{-30.0,-42.0,db_zone_random_anim,	MODE_DBPS,	LU_CAPN|LU_RIGHT|LU_COVER}, 	// DB2
	{-30.0, 42.0,db_man2man_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT|LU_COVER},	// DB3
	{-30.0, 75.0,db_zone_random_anim,	MODE_DBPS,	LU_COVER},	// DB4
};

//fline_t def_formation_c[] = 
//{
//	{-11.0,-17.5,lm_random_anim,	MODE_LINEPS,0},  		// LINE1
//	{-11.0,  0.0,lm_random_anim,	MODE_LINEPS,0},	  	// LINE2
//	{-11.0, 17.5,lm_random_anim,	MODE_LINEPS,0},		// LINE3
//	{-25.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_PLYR2|LU_LEFT},	// DB1
//	{-60.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_CAPN},	// DB2
//	{-25.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_CENTER},	// DB3
//	{-25.0,  0.0,db_man2man_random_anim,	MODE_DBPS,	LU_RIGHT},	// DB4
//};




// *********************
// SPECIAL FORMATIONS
// *********************
fline_t for_kickoff_313[] =
{
	{-117.0f,0.0f,stance2pt_anim_rnd,MODE_KICKER,0},	// kicker
	
	{-78.0f,-156.0f,stance2pt2_anim_rnd,MODE_WPNPS,0},
	{-78.0f,-97.5f,stance2pt2_anim_rnd,MODE_WPNPS,0},	// left 3
	{-78.0f,-39.0f,stance2pt_anim_rnd,MODE_WPNPS,LU_PLYR2},
	
	{-78.0f,39.0f,stance2pt2_anim_rnd,MODE_WPNPS,LU_CAPN},
	{-78.0f,97.5f,stance2pt2_anim_rnd,MODE_WPNPS,0},	// right 3
	{-78.0f,156.0f,stance2pt_anim_rnd,MODE_WPNPS,0},
};

// Used for player 2 
fline_t for_kickoff2_313[] =
{
	{-117.0f,0.0f,stance2pt_anim_rnd,MODE_KICKER,0},	// kicker
	
	{-78.0f,-156.0f,stance2pt_anim_rnd,MODE_WPNPS,0},
	{-78.0f,-97.5f,stance2pt2_anim_rnd,MODE_WPNPS,0},	// left 3
	{-78.0f,-39.0f,stance2pt2_anim_rnd,MODE_WPNPS,LU_CAPN},
	
	{-78.0f,39.0f,stance2pt2_anim_rnd,MODE_WPNPS,LU_PLYR2},
	{-78.0f,97.5f,stance2pt2_anim_rnd,MODE_WPNPS,0},	// right 3
	{-78.0f,156.0f,stance2pt_anim_rnd,MODE_WPNPS,0},
};

fline_t for_kickoff_short[] =
{
//	{-234.0f,0.0f,run_hold_anim,MODE_KICKER,0},	// kicker
	{-234.0f,0.0f,run_hold_anim,MODE_DBPS,0},
	
	{-39.0f,-156.0f,run_hold_anim,MODE_DBPS,0},
	{-39.0f,-97.5f,run_hold_anim,MODE_DBPS,0},	// left 3
	{-39.0f,-39.0f,run_hold_anim,MODE_DBPS,LU_PLYR2},
	
	{-39.0f,39.0f,run_hold_anim,MODE_DBPS,LU_CAPN},
	{-39.0f,97.5f,run_hold_anim,MODE_DBPS,0},	// right 3
	{-39.0f,156.0f,run_hold_anim,MODE_DBPS,0},
};

fline_t for_kick_return_short[] =
{
	{-39.0f,-78.0f,stance2pt2_anim_rnd,MODE_WPNPS,0},
	{-39.0f,0.0f,stance2pt2_anim_rnd,MODE_WPNPS,0},	// front 3
	{-39.0f,78.0f,stance2pt_anim_rnd,MODE_WPNPS,0},
	
	{-273.0f,0.0f,c_standing_anim,MODE_WPNPS,LU_CAPN},	// returner
	
	{-117.0f,-52.0f,run_hold_anim,MODE_WPNPS,0},
//	{-273.0f-100.0f,50.0f,run_hold_anim,MODE_WPNPS,0},	// back 3 - for lateral guy
	{-273.0f-20.0f,50.0f,run_hold_anim,MODE_WPNPS,LU_PLYR2},	// back 3 - for lateral guy
	{-117.0f,52.0f,run_hold_anim,MODE_WPNPS,0},
};

fline_t for_kick_return[] =
{
	{-117.0f-0.0f-270.0f,-78.0f,stance2pt2_anim_rnd,MODE_DBPS,0},
	{-117.0f+19.5f-270.0f,0.0f,stance2pt_anim_rnd,MODE_DBPS,0},	// front 3
	{-117.0f-0.0f-270.0f,78.0f,stance2pt2_anim_rnd,MODE_DBPS,0},
	
	{-117.0f-370.5f,0.0f,c_standing2_anim,MODE_DBPS,LU_CAPN},	// returner
	
	{-117.0f-234.0f-90.0f,-52.0f,stance2pt_anim_rnd,MODE_DBPS,0},
	{-117.0f-370.5f-20.0f,-45.0f,stance2pt2_anim_rnd,MODE_DBPS,LU_PLYR2},	// back 3	- for lateral guy
	{-117.0f-234.0f-90.0f,52.0f,stance2pt_anim_rnd,MODE_DBPS,0},
};

fline_t for_onside_kick_return[] =
{
	{-117.0f-0.0f,-78.0f,stance2pt2_anim_rnd,MODE_DBPS,0},
	{-117.0f+19.5f,0.0f,stance2pt2_anim_rnd,MODE_DBPS,LU_CAPN},	// front 3
	{-117.0f-0.0f,78.0f,stance2pt_anim_rnd,MODE_DBPS,LU_PLYR2},
	
	{-117.0f-370.5f,0.0f,stance2pt2_anim_rnd,MODE_DBPS,0},	// returner
	
	{-117.0f-234.0f,-52.0f,stance2pt_anim_rnd,MODE_DBPS,0},
	{-117.0f-234.0f,0.0f,stance2pt2_anim_rnd,MODE_DBPS,0},	// back 3
	{-117.0f-234.0f,52.0f,stance2pt_anim_rnd,MODE_DBPS,0},
};

//---------------------------
// defensive back assignments
//---------------------------
int db_zone[] =
{
	DS_COVER_TGT
};

// short zone, los->15, left third
int	db_zone_sl3[] =
{
	DS_ZONE,DZ_LEFT_THIRD,-5,20
};

// short zone, los->15, middle third
int	db_zone_sm3[] =
{
	DS_ZONE,DZ_MIDDLE_THIRD,-5,18
};

// short zone, los->15, right third
int	db_zone_sr3[] =
{
	DS_ZONE,DZ_RIGHT_THIRD,-5,18
};

// short zone, los->15, left half
int	db_zone_sl2[] =
{
	DS_ZONE,DZ_LEFT_HALF,-5,18
};

// short zone, los->15, right half
int	db_zone_sr2[] =
{
	DS_ZONE,DZ_RIGHT_HALF,-5,18
};

// deep zone, 15->EZ, left half
int	db_zone_dl2[] =
{
	DS_ZONE,DZ_LEFT_HALF,13,DZ_EZ
};

// deep zone, 15->EZ, right half
int	db_zone_dr2[] =
{
	DS_ZONE,DZ_RIGHT_HALF,13,DZ_EZ
};

// deep zone, 15->EZ, left third
int	db_zone_dl3[] =
{
	DS_ZONE,DZ_LEFT_THIRD,13,DZ_EZ
};

// deep zone, 15->EZ, middle third
int	db_zone_dm3[] =
{
	DS_ZONE,DZ_MIDDLE_THIRD,13,DZ_EZ
};

// deep zone, 15->EZ, right third
int	db_zone_dr3[] =
{
	DS_ZONE,DZ_RIGHT_THIRD,13,DZ_EZ
};

// deep zone, 15->end, full field
int	db_zone_dw[] =
{
//	DS_COVER_WEAK,				// cover weak side on trips
	DS_ZONE,DZ_WIDE,13,DZ_EZ
};

int db_man[] =
{
	DS_COVER_TGT
};

int db_rush_qb[] = 
{
	DS_RUSH_QB
};

// misc drone scripts
int ds_none[] =
{
	DS_IMPROVISE
};

int ds_improvise[] =
{
	DS_IMPROVISE
};

int ds_block[] =
{
	DS_BLOCK
};


// ***********
// PASS ROUTES
// ***********

// Used in Hail Mary
int pr_goto_endzone[] =
{
	DS_FACE,0,
	DS_DELAY_BLOCK,60,
//	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,10,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,35,
	DS_TURBO_ON,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_JUKE,
	DS_FAKE_OUT,512,60,25,
	DS_RUN_YDS,50,
	DS_IMPROVISE
};

// Used by QB after getting rid of ball on some plays
// Used in Tight End formation
int pr_QBgoto_endzone[] =
{
	DS_FACE,0,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,20,
//	DS_SPIN,
//	DS_FAKE_OUT,-256,60,35,
//	DS_RUN_YDS,2,
//	DS_WAVE_ARM,
//	DS_RUN_YDS,8,
//	DS_TURBO_OFF,
//	DS_JUKE,
//	DS_FAKE_OUT,512,60,25,
//	DS_RUN_YDS,50,
	DS_IMPROVISE
};

// Used by QB after getting rid of ball on some plays
// Used in Tight End formation
int pr_QBgoto_endzone2[] =
{
	DS_FACE,0,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,15,
//	DS_SPIN,
//	DS_FAKE_OUT,-256,60,35,
//	DS_RUN_YDS,2,
//	DS_WAVE_ARM,
//	DS_RUN_YDS,8,
//	DS_TURBO_OFF,
//	DS_JUKE,
//	DS_FAKE_OUT,512,60,25,
//	DS_RUN_YDS,50,
	DS_IMPROVISE
};

// Used in Da Bomb play
int pr_goto_endzone3[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,5,
	DS_JUKE,
	DS_FAKE_OUT,-256,60,25,
	DS_RUN_YDS,10,
	DS_SPIN,
	DS_FAKE_OUT,256,60,35,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Hail Mary
int pr_goto_endzone2[] =
{
	DS_WAIT_SNAP,
	DS_JUKE,
	DS_FAKE_OUT,-256,40,30,
	DS_FACE,0,
//	DS_TURBO_ON,
	DS_RUN_YDS,30,
//	DS_TURBO_OFF,
	DS_RUN_YDS,40,
	DS_IMPROVISE
};

// Used in Upper Cut play
int pr_med_hitch_up[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,15,
	DS_TURBO_OFF,
	DS_FAKE_OUT,-256,50,25,
	DS_FACE,384,
	DS_RUN_YDS,2,
	DS_FAKE_OUT,384,50,30,
	DS_FACE,0,
	DS_RUN_YDS,4,
	DS_JUKE,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_RUN_YDS,20,
	DS_TURBO_OFF,
	DS_FAKE_OUT,128,40,0,
	DS_FACE,512,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Carolina #1
int pr_med_hitch_up2[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,0,
	DS_FAKE_OUT,-256,50,25,
	DS_GOTO_LOS_PLUS,15,
	DS_TURBO_OFF,
	DS_FAKE_OUT,-256,50,45,
	DS_FACE,384,
	DS_RUN_YDS,2,
	DS_FAKE_OUT,384,50,35,
	DS_FACE,0,
	DS_RUN_YDS,4,
	DS_JUKE,
	DS_FAKE_OUT,128,40,40,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_FAKE_OUT,128,40,0,
	DS_FACE,512,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// used in Upper Cut play
int pr_instant_slant[] =
{
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,128,
	DS_RUN_YDS,30,
	DS_FAKE_OUT,256,50,0,
	DS_FACE,-256,
	DS_RUN_YDS,6,
	DS_WAVE_ARM,
	DS_RUN_YDS,24,
	DS_IMPROVISE
};

// used in Criss Cross play
int pr_instant_slant3[] =
{
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,128,
	DS_RUN_YDS,5,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,30,
	DS_RUN_YDS,13,
	DS_FAKE_OUT,256,50,0,
	DS_FACE,-256,
	DS_RUN_YDS,6,
	DS_WAVE_ARM,
	DS_TURBO_ON,
	DS_RUN_YDS,18,
	DS_TURBO_OFF,
	DS_FACE,128,
	DS_FAKE_OUT,-128,50,0,
	DS_RUN_YDS,18,
	DS_IMPROVISE
};

// used in Gern #6
int pr_instant_slant13[] =
{
	DS_FACE,-96,
	DS_GOTO_LOS_PLUS,4,
	DS_FAKE_OUT,0,50,30,
	DS_TURBO_ON,
	DS_FACE,160,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,40,
	DS_RUN_YDS,6,
	DS_FAKE_OUT,-256,50,40,
	DS_FACE,256,
	DS_RUN_YDS,4,
	DS_WAVE_ARM,
	DS_TURBO_ON,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_FACE,-128,
	DS_FAKE_OUT,128,50,40,
	DS_RUN_YDS,18,
	DS_IMPROVISE
};

// used in Carolina #1
int pr_instant_slant8[] =
{
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,128,
	DS_RUN_YDS,5,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,35,
	DS_RUN_YDS,13,
	DS_FAKE_OUT,256,50,40,
	DS_FACE,-256,
	DS_RUN_YDS,6,
	DS_WAVE_ARM,
	DS_RUN_YDS,12,
	DS_IMPROVISE
};

// used in Criss Cross play
int pr_instant_slant4[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,128,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,30,
	DS_RUN_YDS,17,
	DS_FAKE_OUT,256,50,0,
	DS_FACE,-256,
	DS_RUN_YDS,21,
	DS_FACE,-384,
	DS_FAKE_OUT,-128,50,0,
	DS_RUN_YDS,3,
	DS_IMPROVISE
};

// used in Ravens #2
int pr_instant_slant7[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,-160,
	DS_RUN_YDS,5,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,30,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,-256,50,0,
	DS_FACE,256,
	DS_RUN_YDS,21,
	DS_FACE,-256,
	DS_FAKE_OUT,128,60,35,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// used in Ravens #3
int pr_instant_slant6[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-160,
	DS_GOTO_LOS_PLUS,5,
	DS_FAKE_OUT,0,50,40,
	DS_FACE,128,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,30,
	DS_RUN_YDS,15,
	DS_FAKE_OUT,-256,50,40,
	DS_FACE,256,
	DS_RUN_YDS,11,
	DS_FAKE_OUT,256,50,0,
	DS_FACE,-256,
	DS_RUN_YDS,21,
	DS_FACE,-384,
	DS_FAKE_OUT,-128,50,0,
	DS_RUN_YDS,3,
	DS_IMPROVISE
};

// Used in Blizzard play
int pr_instant_slant2[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,2,
	DS_FACE,128,
//	DS_FACE,64,
	DS_FAKE_OUT,0,40,35,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,-256,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};
// Used in Carolina #1
int pr_instant_slant9[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,3,
	DS_FACE,-192,
//	DS_FACE,64,
	DS_FAKE_OUT,256,40,35,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,8,
	DS_FAKE_OUT,0,50,30,
	DS_FACE,256,
	DS_TURBO_ON,
	DS_RUN_YDS,9,
	DS_TURBO_OFF,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_SPIN,
	DS_FAKE_OUT,128,60,30,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};


// used in Cross Slant play
int pr_instant_slant5[] =
{
	DS_FACE,0,
	DS_DELAY_BLOCK,90,
	DS_GOTO_LOS_PLUS,1,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,128,
	DS_RUN_YDS,30,
	DS_FAKE_OUT,256,50,0,
	DS_FACE,-256,
	DS_RUN_YDS,6,
	DS_WAVE_ARM,
	DS_RUN_YDS,24,
	DS_FACE,512,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,10,
	DS_FACE,256,
	DS_FAKE_OUT,512,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,16,
	DS_IMPROVISE
};

// used in Chicago #2
int pr_instant_slant10[] =
{
	DS_FACE,0,
	DS_TURBO_ON,
	DS_DELAY_BLOCK,90,
	DS_GOTO_LOS_PLUS,3,
	DS_FAKE_OUT,0,50,30,
	DS_FACE,-96,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FAKE_OUT,0,50,30,
	DS_FACE,256,
	DS_RUN_YDS,6,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_RUN_YDS,12,
	DS_FACE,512,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,4,
	DS_FACE,-256,
	DS_FAKE_OUT,512,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,16,
	DS_IMPROVISE
};

// used in Cinci #2
int pr_instant_slant11[] =
{
	DS_FACE,0,
	DS_TURBO_ON,
	DS_DELAY_BLOCK,90,
	DS_GOTO_LOS_PLUS,3,
	DS_FAKE_OUT,-416,50,40,
	DS_FACE,-128,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,9,
	DS_FAKE_OUT,0,50,40,
	DS_FACE,256,
	DS_RUN_YDS,6,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,40,
	DS_RUN_YDS,12,
	DS_FACE,512,
	DS_FAKE_OUT,128,50,40,
	DS_RUN_YDS,4,
	DS_FACE,-256,
	DS_FAKE_OUT,512,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,16,
	DS_IMPROVISE
};

// Used in Upper Cut play
int pr_run_behind_qb_up[] =
{
	DS_WAIT_SNAP,
	DS_FAKE_OUT,256,75,85,
	DS_TURBO_ON,
	DS_FACE,384,
	DS_RUN_YDS,10,
	DS_FACE,256,
	DS_RUN_YDS,12,
	DS_FAKE_OUT,256,50,0,
	DS_FACE,0,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_JUKE,
	DS_FAKE_OUT,128,75,0,
	DS_TURBO_OFF,
	DS_FACE,-256,
	DS_RUN_YDS,1,
	DS_SPIN,
	DS_FAKE_OUT,128,75,0,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in Reverse Zip play
int pr_run_behind_qb_up2[] =
{
	DS_WAIT_SNAP,
	DS_MOVE_DEFENDER,256,120,
	DS_TURBO_ON,
	DS_FACE,416,
	DS_RUN_YDS,5,
	DS_FACE,384,
	DS_RUN_YDS,5,
	DS_FACE,352,
	DS_RUN_YDS,2,
	DS_FACE,320,
	DS_RUN_YDS,2,
	DS_FACE,288,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_RUN_YDS,2,
	DS_FACE,224,
	DS_RUN_YDS,2,
	DS_FACE,192,
	DS_RUN_YDS,2,
	DS_FACE,160,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_FACE,96,
	DS_RUN_YDS,2,
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_FACE,32,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,15,
	DS_FACE,512,
	DS_FAKE_OUT,0,60,0,
	DS_RUN_YDS,5,
	DS_FACE,-256,
	DS_FAKE_OUT,512,75,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,18,
	DS_TURBO_OFF,
	DS_FACE,-128,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used by Blizzard play
int pr_short_slant_in[] =
{
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,6,
	DS_JUKE,
	DS_FAKE_OUT,64,50,0,
	DS_FACE,-256,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,10,			// Need 6 yds after arm wave!
	DS_TURBO_ON,
	DS_FACE,0,
	DS_RUN_YDS,7,
//	DS_FACE,512,
//	DS_FAKE_OUT,-256,40,0,
//	DS_RUN_YDS,4,
//	DS_FACE,256,
//	DS_RUN_YDS,7,
//	DS_WAVE_ARM,
//	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,13,
	DS_IMPROVISE
};

// Used in Blizzard play
int pr_med_up_cut[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,-96,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,8,
	DS_FACE,128,
	DS_FAKE_OUT,-384,40,0,
	DS_RUN_YDS,8,
	DS_FAKE_OUT,256,40,0,
	DS_FACE,-256,
	DS_RUN_YDS,8,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Atlanta custom play
int pr_med_up_cut_rgt[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,128,
	DS_RUN_YDS,16,
	DS_JUKE,
	DS_FACE,-96,
	DS_FAKE_OUT,256,40,40,
	DS_TURBO_ON,
	DS_RUN_YDS,6,
	DS_FACE,128,
	DS_TURBO_OFF,
	DS_FAKE_OUT,-384,40,40,
	DS_RUN_YDS,8,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,30,
	DS_RUN_YDS,8,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in One Man Back
int pr_med_up_cut2[] =
{
	DS_FACE,0,
	DS_JUKE,
	DS_FAKE_OUT,256,40,40,
	DS_RUN_YDS,9,
	DS_SPIN,
	DS_FAKE_OUT,128,40,40,
	DS_RUN_YDS,9,
	DS_FACE,384,
	DS_FAKE_OUT,-384,40,0,
	DS_RUN_YDS,8,
	DS_FAKE_OUT,-256,40,0,
	DS_FACE,256,
	DS_RUN_YDS,8,
	DS_SPIN,
	DS_FAKE_OUT,256,40,30,
	DS_FACE,0,
	DS_RUN_YDS,14,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,4,
	DS_FACE,-256,
	DS_FAKE_OUT,128,75,0,
	DS_RUN_YDS,25,
	DS_IMPROVISE
};

// Used in Hail Mary
int pr_med_chair[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,2,
	DS_JUKE,
	DS_FAKE_OUT,256,40,35,
	DS_RUN_YDS,12,
	DS_FAKE_OUT,-256,40,0,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,16,
	DS_FAKE_OUT,290,60,0,
	DS_FACE,0,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,35,
	DS_IMPROVISE
};

// Used in Quake Shake play	//Turmoil play
int pr_long_slant_rgt_up[] =
{
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,0,
	DS_RUN_YDS,8,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,4,
	DS_TURBO_OFF,
	DS_FACE,64,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,15,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,4,
	DS_FACE,512,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,3,
	DS_IMPROVISE
};

// Used in Sweep Right play
int pr_med_up_slant_lft2[] =
{
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,75,0,
	DS_RUN_YDS,9,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_FACE,512,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,10,
	DS_FACE,-256,
	DS_FAKE_OUT,512,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,17,
	DS_FACE,0,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in UTB Deep play
int pr_med_up_slant_lft3[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,9,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,30,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,23,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,2,
	DS_FACE,-256,
	DS_FAKE_OUT,512,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,13,
	DS_FACE,0,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in Criss Cross play
int pr_short_slip[] =
{
	DS_WAIT_SNAP,
	DS_MOVE_DEFENDER,256,90,
	DS_TURBO_ON,
	DS_FACE,384,
	DS_RUN_YDS,6,
	DS_FACE,352,
	DS_RUN_YDS,2,
	DS_FACE,320,
	DS_RUN_YDS,2,
	DS_FACE,288,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_RUN_YDS,2,
	DS_FACE,224,
	DS_RUN_YDS,2,
	DS_FACE,192,
	DS_RUN_YDS,2,
	DS_FACE,160,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_FACE,96,
	DS_RUN_YDS,2,
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_FACE,32,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,40,
	DS_RUN_YDS,6,
	DS_FACE,-256,
	DS_FAKE_OUT,128,50,40,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,1,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_FACE,64,
	DS_FAKE_OUT,-384,75,35,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Sweep Right play
int pr_short_slip6[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_MOVE_DEFENDER,256,100,
//	DS_FACE,-256,
//	DS_RUN_YDS,4,
//	DS_FACE,256,
//	DS_RUN_YDS,4,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_RUN_YDS,11,
	DS_FACE,224,
	DS_RUN_YDS,2,
	DS_FACE,192,
	DS_RUN_YDS,2,
	DS_FACE,160,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_FACE,96,
	DS_RUN_YDS,2,
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_FACE,32,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,12,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,1,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,17,
	DS_FACE,128,
	DS_FAKE_OUT,-384,75,35,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Chi 2
int pr_short_slip9[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_MOVE_DEFENDER,256,100,
//	DS_FACE,-256,
//	DS_RUN_YDS,4,
	DS_FACE,512,
	DS_RUN_YDS,4,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_RUN_YDS,11,
	DS_FACE,224,
	DS_RUN_YDS,2,
	DS_FACE,192,
	DS_RUN_YDS,2,
	DS_FACE,160,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_FACE,96,
	DS_RUN_YDS,2,
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_FACE,32,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,30,
	DS_RUN_YDS,12,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,30,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,1,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,17,
	DS_FACE,128,
	DS_FAKE_OUT,-384,75,35,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Cinci #2
int pr_short_slip7[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_MOVE_DEFENDER,256,100,
//	DS_FACE,-256,
//	DS_RUN_YDS,4,
//	DS_FACE,256,
//	DS_RUN_YDS,4,
	DS_TURBO_ON,
	DS_FACE,512,
	DS_RUN_YDS,2,
	DS_FACE,256,
	DS_RUN_YDS,11,
	DS_FACE,224,
	DS_RUN_YDS,2,
	DS_FACE,192,
	DS_RUN_YDS,2,
	DS_FACE,160,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_FACE,96,
	DS_RUN_YDS,2,
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_FACE,32,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,40,
	DS_RUN_YDS,12,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,40,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,1,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,17,
	DS_FACE,128,
	DS_FAKE_OUT,-384,75,35,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Dawg Hook play
int pr_short_slip2[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_MOVE_DEFENDER,256,90,
	DS_TURBO_ON,
	DS_FACE,384,
	DS_RUN_YDS,6,
	DS_FACE,352,
	DS_RUN_YDS,2,
	DS_FACE,320,
	DS_RUN_YDS,2,
	DS_FACE,288,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_RUN_YDS,2,
	DS_FACE,224,
	DS_RUN_YDS,2,
	DS_FACE,192,
	DS_RUN_YDS,2,
	DS_FACE,160,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_FACE,96,
	DS_RUN_YDS,2,
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_FACE,32,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,25,
	DS_FACE,512,
	DS_FAKE_OUT,0,60,0,
	DS_RUN_YDS,5,
	DS_FACE,-256,
	DS_FAKE_OUT,512,75,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Blitz Beater play
int pr_short_slip3[] =
{
	DS_WAIT_SNAP,
//	DS_TURBO_ON,
	DS_MOVE_DEFENDER,-256,90,
	DS_FACE,-256,
	DS_RUN_YDS,6,
	DS_FACE,-128,
	DS_RUN_YDS,6,
//	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,40,0,
	DS_RUN_YDS,6,
	DS_FACE,256,
	DS_FAKE_OUT,0,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,1,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_FACE,-64,
	DS_FAKE_OUT,384,75,35,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Cross Slant play
int pr_short_slip5[] =
{
	DS_WAIT_SNAP,
	DS_MOVE_DEFENDER,-256,90,
	DS_TURBO_ON,
	DS_FACE,-256,
	DS_RUN_YDS,9,
	DS_FACE,-128,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,9,
	DS_JUKE,
	DS_FAKE_OUT,-256,75,0,
	DS_FACE,256,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_FACE,-64,
	DS_FAKE_OUT,384,75,35,
	DS_TURBO_ON,
	DS_RUN_YDS,23,
	DS_TURBO_OFF,
	DS_FACE,-256,
	DS_FAKE_OUT,64,75,0,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in Screen Right play
int pr_short_slip4[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_MOVE_DEFENDER,-256,90,
	DS_FACE,-352,
	DS_RUN_YDS,2,
	DS_FACE,-320,
	DS_RUN_YDS,2,
	DS_FACE,-288,
	DS_RUN_YDS,2,
	DS_FACE,-256,
	DS_RUN_YDS,2,
	DS_FACE,-224,
	DS_RUN_YDS,2,
	DS_FACE,-192,
	DS_RUN_YDS,1,
	DS_FACE,-160,
	DS_RUN_YDS,1,
	DS_FACE,-128,
	DS_RUN_YDS,1,
	DS_FACE,-96,
	DS_RUN_YDS,1,
	DS_FACE,-64,
	DS_RUN_YDS,1,
	DS_FACE,-32,
	DS_RUN_YDS,1,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,15,
	DS_FACE,512,
	DS_FAKE_OUT,0,60,0,
	DS_RUN_YDS,9,
	DS_FACE,256,
	DS_FAKE_OUT,512,75,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in Cini #2,3
int pr_short_slip8[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_MOVE_DEFENDER,-256,90,
//	DS_TURBO_ON,
	DS_FACE,-480,
	DS_RUN_YDS,1,
	DS_FACE,-448,
	DS_RUN_YDS,1,
	DS_FACE,-416,
	DS_RUN_YDS,1,
	DS_FACE,-384,
	DS_RUN_YDS,1,
	DS_FACE,-352,
	DS_RUN_YDS,1,
	DS_FACE,-320,
	DS_RUN_YDS,1,
	DS_FACE,-288,
	DS_RUN_YDS,1,
	DS_FACE,-256,
	DS_RUN_YDS,1,
	DS_FACE,-224,
	DS_RUN_YDS,1,
	DS_FACE,-192,
	DS_RUN_YDS,1,
	DS_FACE,-160,
	DS_RUN_YDS,1,
	DS_FACE,-128,
	DS_RUN_YDS,1,
	DS_FACE,-96,
	DS_RUN_YDS,1,
	DS_FACE,-64,
	DS_RUN_YDS,1,
	DS_FACE,-32,
	DS_RUN_YDS,1,
//	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,40,0,
	DS_RUN_YDS,11,
	DS_FACE,384,
	DS_FAKE_OUT,-128,60,30,
	DS_RUN_YDS,3,
	DS_FACE,128,
	DS_FAKE_OUT,-256,75,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_FACE,-256,
	DS_FAKE_OUT,256,75,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Quake Shake play //Turmoil play
int pr_med_slant_up_lft[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-160,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,7,
	DS_FAKE_OUT,-256,40,20,
	DS_FACE,0,
	DS_RUN_YDS,10,
	DS_FAKE_OUT,0,40,0,
	DS_FACE,256,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,21,
	DS_TURBO_OFF,
//	DS_FACE,512,
//	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,7,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,7,
	DS_FACE,0,
	DS_FAKE_OUT,-256,40,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Up the Gut play
int pr_med_up[] =
{
	DS_FACE,0,
	DS_RUN_YDS,10,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,35,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in Up the Gut play
int pr_med_thru_middle[] =
{
	DS_WAIT_SNAP,
	DS_FAKE_OUT,256,50,0,
	DS_FACE,65,
	DS_RUN_YDS,2,
	DS_SPIN,
	DS_FAKE_OUT,128,50,30,
	DS_RUN_YDS,7,
	DS_TURBO_ON,
	DS_FACE,-256,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,12,
	DS_IMPROVISE
};

// Used in Middle Pick play
int pr_med_thru_middle2[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,2,
	DS_SPIN,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,22,
	DS_FACE,256,
	DS_FAKE_OUT,0,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,21,
	DS_TURBO_OFF,
	DS_FACE,512,
	DS_FAKE_OUT,0,256,0,
	DS_RUN_YDS,3,
	DS_IMPROVISE
};

// Used in Up the Gut play
int pr_rgt_up_cut_left[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-256,
	DS_TURBO_ON,
	DS_FAKE_OUT,0,50,30,
	DS_RUN_YDS,8,
	DS_FAKE_OUT,-256,40,0,
	DS_FACE,0,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,13,
	DS_JUKE,
	DS_FAKE_OUT,0,75,30,
	DS_FACE,384,
	DS_RUN_YDS,8,
	DS_FACE,256,
	DS_TURBO_ON,
	DS_FAKE_OUT,512,50,30,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};
// Used in Quake Shake play //Turmoil play
int pr_med_slip[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_FAKE_OUT,-128,75,60,
	DS_FACE,-288,
	DS_RUN_YDS,2,
	DS_FACE,-256,
	DS_RUN_YDS,2,
	DS_FACE,-224,
	DS_RUN_YDS,2,
	DS_FACE,-192,
	DS_RUN_YDS,2,
	DS_FACE,-160,
	DS_RUN_YDS,2,
	DS_FACE,-128,
	DS_RUN_YDS,2,
	DS_FACE,-96,
	DS_RUN_YDS,2,
	DS_FACE,-64,
	DS_RUN_YDS,2,
	DS_FACE,-32,
	DS_RUN_YDS,2,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_TURBO_ON,
	DS_JUKE,
	DS_FAKE_OUT,384,75,30,
	DS_RUN_YDS,9,
	DS_FACE,256,
	DS_FAKE_OUT,0,75,30,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
//	DS_FACE,256,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,256,75,0,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in QBdeep out play
int pr_QBdeep_HB[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FAKE_OUT,-128,75,60,
	DS_FACE,-288,
	DS_RUN_YDS,2,
	DS_FACE,-256,
	DS_RUN_YDS,2,
	DS_FACE,-224,
	DS_RUN_YDS,2,
	DS_FACE,-192,
	DS_RUN_YDS,2,
	DS_FACE,-160,
	DS_RUN_YDS,2,
	DS_FACE,-128,
	DS_RUN_YDS,2,
	DS_FACE,-96,
	DS_RUN_YDS,2,
	DS_FACE,-64,
	DS_RUN_YDS,2,
	DS_FACE,-32,
	DS_RUN_YDS,2,
	DS_FACE,0,
	DS_RUN_YDS,1,
//	DS_TURBO_ON,
	DS_JUKE,
	DS_FAKE_OUT,384,75,40,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,9,
	DS_FACE,256,
	DS_FAKE_OUT,0,75,40,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,

//	DS_TURBO_OFF,

	DS_SPIN,
	DS_FAKE_OUT,256,75,40,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in Arz #1
int pr_QBdeep_HB2[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FAKE_OUT,-128,75,60,
	DS_FACE,-288,
	DS_RUN_YDS,1,
	DS_FACE,-256,
	DS_RUN_YDS,1,
	DS_FACE,-224,
	DS_RUN_YDS,1,
	DS_FACE,-192,
	DS_RUN_YDS,1,
	DS_FACE,-160,
	DS_RUN_YDS,1,
	DS_FACE,-128,
	DS_RUN_YDS,1,
	DS_FACE,-96,
	DS_RUN_YDS,1,
	DS_FACE,-64,
	DS_RUN_YDS,1,
	DS_FACE,-32,
	DS_RUN_YDS,1,
	DS_FACE,0,
	DS_RUN_YDS,1,
//	DS_TURBO_ON,
	DS_JUKE,
	DS_FAKE_OUT,384,75,40,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,9,
	DS_FACE,256,
	DS_FAKE_OUT,-128,75,40,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,

//	DS_TURBO_OFF,

	DS_SPIN,
	DS_FAKE_OUT,256,75,40,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};
// Used in spider eyes play
int pr_spider_HB[] =
{
	DS_WAIT_SNAP,
	DS_FACE,256,
	DS_MOVE_DEFENDER,256,120,
	DS_RUN_YDS,12,
	DS_FACE,224,
	DS_RUN_YDS,1,
	DS_FACE,192,
	DS_RUN_YDS,1,
	DS_FACE,160,
	DS_RUN_YDS,1,
	DS_FACE,128,
	DS_RUN_YDS,1,
	DS_FACE,96,
	DS_RUN_YDS,1,
	DS_FACE,64,
	DS_RUN_YDS,1,
	DS_FACE,32,
	DS_RUN_YDS,1,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_TURBO_ON,
	DS_JUKE,
	DS_FAKE_OUT,-384,50,40,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,9,
	DS_FACE,-128,
	DS_FAKE_OUT,-128,55,40,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,256,50,30,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in zigzag play
int pr_zigzag_HB[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_FAKE_OUT,256,45,30,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_JUKE,
	DS_FAKE_OUT,384,50,40,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,8,
	DS_FACE,-64,
	DS_FAKE_OUT,256,45,40,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_SPIN,
	DS_FACE,256,
	DS_FAKE_OUT,-128,50,40,
	DS_RUN_YDS,10,
	DS_TURBO_ON,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in monkey play
int pr_monkey_HB[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_MOVE_DEFENDER,256,120,
	DS_RUN_YDS,18,
	DS_FACE,224,
	DS_RUN_YDS,1,
	DS_FACE,192,
	DS_RUN_YDS,1,
	DS_FACE,160,
	DS_RUN_YDS,1,
	DS_FACE,128,
	DS_RUN_YDS,1,
	DS_FACE,96,
	DS_RUN_YDS,1,
	DS_FACE,64,
	DS_RUN_YDS,1,
	DS_FACE,32,
	DS_RUN_YDS,1,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,512,75,40,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,9,
	DS_FACE,-128,
	DS_FAKE_OUT,128,35,30,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_TURBO_OFF,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,0,40,40,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in switch back play - motion man rgt
int pr_switch_HB[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,64,
	DS_FAKE_OUT,256,45,35,
	DS_GOTO_LOS_PLUS,10,
	DS_FACE,-256,
	DS_TURBO_OFF,
	DS_FAKE_OUT,128,40,50,
	DS_RUN_YDS,8,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,256,40,45,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,160,40,35,
	DS_TURBO_ON,
	DS_RUN_YDS,12,
	DS_TURBO_OFF,
	DS_RUN_YDS,5,
	DS_IMPROVISE
};

// Used in side slide play
int pr_slide_HB[] =
{
	DS_WAIT_SNAP,
	DS_FACE,256,
	DS_MOVE_DEFENDER,256,120,
	DS_RUN_YDS,12,
	DS_FACE,224,
	DS_RUN_YDS,1,
	DS_FACE,192,
	DS_RUN_YDS,1,
	DS_FACE,160,
	DS_RUN_YDS,1,
	DS_FACE,128,
	DS_RUN_YDS,1,
	DS_FACE,96,
	DS_RUN_YDS,1,
	DS_FACE,64,
	DS_RUN_YDS,1,
	DS_FACE,32,
	DS_RUN_YDS,1,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_TURBO_ON,
	DS_JUKE,
	DS_FAKE_OUT,512,65,40,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,9,
	DS_FACE,-128,
	DS_FAKE_OUT,256,65,40,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,256,55,40,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in hook n ladder play
int pr_hook_HB[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_TURBO_ON,
	DS_FACE,-256,
	DS_FAKE_OUT,-128,50,50,
//	DS_MOVE_DEFENDER,-64,90,
	DS_RUN_YDS,7,
//	DS_FACE,-224,
//	DS_RUN_YDS,2,
//	DS_FACE,-192,
//	DS_RUN_YDS,2,
//	DS_FACE,-160,
//	DS_RUN_YDS,2,
	DS_FACE,-192,
	DS_RUN_YDS,9,
	DS_FACE,-96,
	DS_RUN_YDS,2,
	DS_FACE,-64,
	DS_RUN_YDS,2,
	DS_FACE,-32,
	DS_RUN_YDS,2,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_TURBO_OFF,
	DS_FAKE_OUT,256,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
//	DS_TURBO_OFF,
	DS_RUN_YDS,9,
	DS_FACE,256,
	DS_FAKE_OUT,-384,35,30,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,40,
	DS_FACE,256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in cruisn play
int pr_cruisn_LFT[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_MOVE_DEFENDER,256,90,
	DS_TURBO_ON,
	DS_FACE,384,
	DS_RUN_YDS,6,
	DS_FACE,352,
	DS_RUN_YDS,2,
	DS_FACE,320,
	DS_RUN_YDS,2,
	DS_FACE,288,
	DS_RUN_YDS,2,
	DS_FACE,256,
	DS_RUN_YDS,2,
	DS_FACE,224,
	DS_RUN_YDS,2,
	DS_FACE,192,
	DS_RUN_YDS,2,
	DS_FACE,160,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_FACE,96,
	DS_RUN_YDS,2,
	DS_FACE,64,
	DS_RUN_YDS,2,
	DS_FACE,32,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,12,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,17,
	DS_FACE,64,
	DS_FAKE_OUT,-384,75,35,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_IMPROVISE
};


// Used in Dawg Hook play
int pr_long_up_rgt_dwn[] =
{
	DS_FACE,-32,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,5,
	DS_SPIN,
	DS_FAKE_OUT,256,75,30,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,8,
	DS_JUKE,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,256,
	DS_RUN_YDS,8,
	DS_FACE,512,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,4,
	DS_FACE,-256,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,5,
	DS_FACE,0,
	DS_FAKE_OUT,512,50,0,
	DS_RUN_YDS,18,
	DS_IMPROVISE
};

// Used in Carolina #3
int pr_long_up_rgt_dwn4[] =
{
	DS_FACE,0,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,5,
	DS_SPIN,
	DS_FAKE_OUT,256,75,40,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,6,
	DS_JUKE,
	DS_FAKE_OUT,0,50,30,
	DS_FACE,256,
	DS_RUN_YDS,13,
	DS_FACE,-384,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,4,
	DS_FACE,-256,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,9,
	DS_FACE,128,
	DS_FAKE_OUT,-128,50,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Atlanta #3
int pr_long_up_rgt_dwn3[] =
{
	DS_FACE,-48,
//	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,5,
	DS_SPIN,
	DS_FAKE_OUT,256,75,30,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,8,
	DS_JUKE,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,256,
	DS_RUN_YDS,8,
	DS_FACE,512,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,4,
	DS_FACE,-256,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,5,
	DS_FACE,0,
	DS_FAKE_OUT,512,50,0,
	DS_RUN_YDS,18,
	DS_IMPROVISE
};

// Used in Sub Zero play
int pr_long_up_rgt_dwn2[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_GOTO_LOS_PLUS,5,
	DS_SPIN,
	DS_FAKE_OUT,256,75,30,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,8,
	DS_JUKE,
	DS_FAKE_OUT,0,50,0,
	DS_FACE,256,
	DS_RUN_YDS,8,
	DS_FACE,512,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,10,
	DS_FACE,-256,
	DS_FAKE_OUT,128,50,0,
	DS_RUN_YDS,5,
	DS_FACE,0,
	DS_FAKE_OUT,512,50,0,
	DS_RUN_YDS,18,
	DS_IMPROVISE
};

// Used in Dawg Hook play
int pr_up_left_cut_rgt[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,8,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,12,
	DS_TURBO_OFF,
	DS_FAKE_OUT,256,40,0,
	DS_FACE,-128,
	DS_RUN_YDS,24,
	DS_FAKE_OUT,0,40,0,
	DS_FACE,512,
	DS_RUN_YDS,5,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,13,
	DS_FACE,-128,
	DS_FAKE_OUT,256,50,0,
	DS_RUN_YDS,17,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in General #5
int pr_up_left_cut_rgt5[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_FAKE_OUT,0,50,20,
	DS_RUN_YDS,8,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,12,
	DS_TURBO_OFF,
	DS_FAKE_OUT,256,40,0,
	DS_FACE,-128,
	DS_RUN_YDS,24,
	DS_FAKE_OUT,0,40,0,
//	DS_FACE,512,
//	DS_RUN_YDS,5,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,5,
	DS_FACE,-128,
	DS_FAKE_OUT,256,50,0,
	DS_RUN_YDS,17,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Cross Slant play
int pr_up_left_cut_rgt2[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,20,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,19,
	DS_TURBO_OFF,
	DS_FAKE_OUT,256,40,0,
	DS_FACE,-256,
	DS_RUN_YDS,24,
	DS_IMPROVISE
};

// Used in Sub Zero play
int pr_up_left_cut_rgt3[] =
{
	DS_FACE,0,
	DS_RUN_YDS,10,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,18,
	DS_TURBO_OFF,
	DS_FACE,-64,
	DS_FAKE_OUT,256,60,0,
	DS_RUN_YDS,24,
	DS_FACE,512,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,12,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_FACE,-128,
	DS_FAKE_OUT,256,50,0,
	DS_RUN_YDS,4,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,10,
	DS_FACE,64,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Buffalo #1
int pr_up_left_cut_rgt4[] =
{
	DS_FACE,-64,
	DS_RUN_YDS,10,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,14,
	DS_TURBO_OFF,
	DS_FACE,-128,
	DS_FAKE_OUT,256,60,0,
	DS_RUN_YDS,20,
	DS_FACE,512,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,4,
	DS_FACE,-384,
	DS_FAKE_OUT,256,50,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,3,
	DS_FACE,0,
	DS_FAKE_OUT,512,50,0,
	DS_RUN_YDS,10,
	DS_FACE,256,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

// Used in Blitz Beater play
int pr_short_loop_up[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_MOVE_DEFENDER,-256,90,
	DS_FACE,-256,
	DS_RUN_YDS,9,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_SPIN,
	DS_FAKE_OUT,256,75,35,
	DS_RUN_YDS,10,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,7,
	DS_FACE,256,
	DS_FAKE_OUT,128,50,0,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

// Used in Blitz Beater play
int pr_med_up_lft_cut_rgt[] =
{
	DS_FACE,0,
	DS_RUN_YDS,3,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,30,
	DS_RUN_YDS,7,
	DS_JUKE,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_FACE,128,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in Middle Pick play
int pr_med_up_lft_cut_rgt2[] =
{
	DS_FACE,0,
	DS_RUN_YDS,3,
	DS_FAKE_OUT,-256,60,30,
	DS_RUN_YDS,10,
	DS_JUKE,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_FACE,128,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,13,
	DS_FACE,-384,
	DS_FAKE_OUT,128,60,25,
	DS_RUN_YDS,3,
	DS_FACE,-256,
	DS_FAKE_OUT,512,40,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Screen Right play
int pr_long_slant_cut_up[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-96,
	DS_GOTO_LOS_PLUS,1,
	DS_SPIN,
	DS_FAKE_OUT,256,60,0,
	DS_RUN_YDS,9,
	DS_FAKE_OUT,-128,40,0,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,3,
	DS_JUKE,
	DS_FAKE_OUT,0,70,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

// Used in Chicago #1
int pr_long_slant_cut_up2[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,96,
	DS_GOTO_LOS_PLUS,1,
	DS_SPIN,
	DS_FAKE_OUT,-256,60,40,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,-128,40,30,
	DS_FACE,0,
	DS_TURBO_OFF,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_JUKE,
	DS_FAKE_OUT,0,70,30,
	DS_FACE,256,
	DS_RUN_YDS,3,
	DS_WAVE_ARM,
	DS_RUN_YDS,4,
	DS_FAKE_OUT,128,40,0,
	DS_FACE,-384,
	DS_RUN_YDS,7,
	DS_FACE,-256,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in Screen Right play
int pr_med_up_slant_lft[] =
{
	DS_FACE,0,
	
	
//	DS_BLOCK,
	
	
	
	
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,75,0,
	DS_RUN_YDS,9,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,19,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,9,
	DS_FACE,512,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Cin #3 
int pr_med_up_slant_lft4[] =
{
	DS_FACE,-32,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,75,30,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,35,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,12,
	DS_FACE,512,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in One Man Back play
int pr_very_short_up[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,2,
// Go into pick up Blitzer mode
	DS_BLOCK,
	DS_IMPROVISE
};

// Used in Reverse Zip play
int pr_very_short_up2[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,2,
// Go into pick up Blitzer mode
	DS_BLOCK,
	DS_IMPROVISE
};

// Used in Sweep Right play
int pr_very_short_up3[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,2,
// Go into pick up Blitzer mode
	DS_BLOCK,
	DS_IMPROVISE
};

// Used by QB when he passes ball to WR in backfield and blocks
int pr_QB_block[] =
{
	DS_BLOCK,
};

// Used by QB when he passes ball to WR in backfield and looks for a pass
int pr_QB_sweep[] =
{

// FIX!!
	DS_BLOCK,			


// Put in a delay here before he goes out on route
	DS_WAIT,20,		//40
	DS_FAKE_OUT,-128,75,60,
	DS_FACE,-288,
	DS_RUN_YDS,2,
	DS_FACE,-256,
	DS_RUN_YDS,2,
	DS_FACE,-224,
	DS_RUN_YDS,2,
	DS_FACE,-192,
	DS_RUN_YDS,2,
	DS_FACE,-160,
	DS_RUN_YDS,2,
	DS_FACE,-128,
	DS_RUN_YDS,2,
	DS_FACE,-96,
	DS_RUN_YDS,2,
	DS_FACE,-64,
	DS_RUN_YDS,2,
	DS_FACE,-32,
	DS_RUN_YDS,2,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_TURBO_ON,
	DS_JUKE,
	DS_FAKE_OUT,384,75,30,
	DS_RUN_YDS,9,
	DS_FACE,384,
	DS_FAKE_OUT,0,75,30,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,256,75,0,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in One Man Back play
int pr_short_in[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,21,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,384,50,0,
	DS_RUN_YDS,9,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,16,
	DS_TURBO_ON,
	DS_WAVE_ARM,
	DS_RUN_YDS,4,
	DS_TURBO_OFF,
	DS_RUN_YDS,4,
	DS_FACE,0,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,8,
	DS_FACE,512,
	DS_FAKE_OUT,0,75,0,
	DS_RUN_YDS,10,
	DS_FACE,320,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,15,
	DS_FACE,256,
	DS_FAKE_OUT,512,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,3,
	DS_FACE,0,
	DS_JUKE,
	DS_FAKE_OUT,256,70,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Gen 6
int pr_short_in2[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,256,40,25,
	DS_TURBO_ON,
	DS_RUN_YDS,6,
	DS_FACE,256,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_TURBO_ON,
	DS_RUN_YDS,20,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,0,
	DS_FAKE_OUT,384,50,0,
	DS_RUN_YDS,9,
	DS_FACE,-256,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,14,
	DS_TURBO_ON,
	DS_WAVE_ARM,
	DS_RUN_YDS,4,
	DS_TURBO_OFF,
	DS_RUN_YDS,4,
	DS_FACE,0,
	DS_FAKE_OUT,-256,50,0,
	DS_RUN_YDS,8,
	DS_FACE,512,
	DS_FAKE_OUT,0,75,0,
	DS_RUN_YDS,10,
	DS_FACE,320,
	DS_FAKE_OUT,0,50,0,
	DS_RUN_YDS,12,
	DS_FACE,256,
	DS_FAKE_OUT,512,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,3,
	DS_FACE,0,
	DS_JUKE,
	DS_FAKE_OUT,256,70,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};
// Used in Reverse Zip play
int pr_med_up_cut_cut[] =
{
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_SPIN,
	DS_FAKE_OUT,256,60,35,
	DS_RUN_YDS,13,
	DS_JUKE,
	DS_FAKE_OUT,-256,40,0,
	DS_FACE,192,
	DS_RUN_YDS,10,
	DS_FACE,-192,
	DS_FAKE_OUT,256,60,30,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,0,60,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,25,
	DS_FACE,512,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,10,
	DS_FACE,-256,
	DS_FAKE_OUT,512,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

// Used in Razzle Dazzle play - super fly
int pr_med_up_cut_cut2[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_SPIN,
	DS_FAKE_OUT,256,60,35,
	DS_RUN_YDS,7,
	DS_JUKE,
	DS_FAKE_OUT,-256,40,0,
	DS_FACE,128,
	DS_RUN_YDS,8,
	DS_FACE,-128,
	DS_FAKE_OUT,256,60,30,
	DS_TURBO_ON,
	DS_RUN_YDS,15,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,0,60,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,30,
	DS_FACE,512,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,13,
	DS_FACE,-256,
	DS_FAKE_OUT,512,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,3,
	DS_IMPROVISE
};

// Used in Middle Pick play
int pr_short_up_lft_slant[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,5,
	DS_JUKE,
	DS_FAKE_OUT,-256,75,25,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FAKE_OUT,256,40,0,
	DS_FACE,128,
	DS_RUN_YDS,18,
	DS_FAKE_OUT,128,40,0,
	DS_FACE,512,
	DS_RUN_YDS,3,
	DS_IMPROVISE
};

// Used in Razzle Dazzle play - super fly
int pr_short_up_lft_slant2[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,5,
	DS_JUKE,
	DS_FAKE_OUT,-256,75,25,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FAKE_OUT,128,40,0,
	DS_FACE,256,
	DS_RUN_YDS,15,
	DS_FAKE_OUT,128,40,0,
	DS_FACE,512,
	DS_RUN_YDS,6,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_FACE,0,
	DS_FAKE_OUT,-256,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,5,
	DS_IMPROVISE
};


// Used in Da Bomb play
int pr_short_up_long_slant[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_SPIN,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,5,
	DS_FACE,64,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,10,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_RUN_YDS,30,
	DS_IMPROVISE
};

// Used in Gen 6
int pr_short_up_long_slant3[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_GOTO_LOS_PLUS,1,
	DS_FAKE_OUT,256,50,25,
	DS_SPIN,
	DS_RUN_YDS,5,
	DS_FACE,64,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_RUN_YDS,2,
	DS_TURBO_OFF,
	DS_RUN_YDS,3,
	DS_SPIN,
	DS_FACE,-256,
	DS_FAKE_OUT,512,45,30,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FACE,384,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,5,
	DS_FACE,160,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Atlanta play 3
int pr_short_up_long_slant2[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,4,
	DS_SPIN,
	DS_FAKE_OUT,-128,40,35,
	DS_TURBO_OFF,
	DS_FACE,160,
	DS_RUN_YDS,5,
	DS_WAVE_ARM,
	DS_RUN_YDS,2,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_FACE,288,
	DS_RUN_YDS,10,
	DS_FAKE_OUT,0,40,30,
	DS_FACE,-256,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_WAVE_ARM,
	DS_RUN_YDS,13,
	DS_TURBO_OFF,
	DS_SPIN,
	DS_FAKE_OUT,128,40,30,
	DS_FACE,-288,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in Da Bomb play
int pr_med_up_long_slant[] =
{
	DS_FACE,0,
	DS_DELAY_BLOCK,90,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,40,40,
	DS_TURBO_ON,
	DS_FACE,160,
	DS_RUN_YDS,24,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,512,30,40,
	DS_RUN_YDS,30,
	DS_IMPROVISE
};

// Used in UTB Deep play
int pr_long_up_long_slant[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,7,
	DS_SPIN,
	DS_FAKE_OUT,-128,75,0,
	DS_FACE,64,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,14,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

// Used in UTB Deep play
int pr_short_up_cut_rgt_up[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,5,
	DS_JUKE,
	DS_FACE,-128,
	DS_FAKE_OUT,0,75,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,-256,40,0,
	DS_FACE,0,
	DS_RUN_YDS,25,
	DS_IMPROVISE
};

// Used in Sub Zero play
int pr_short_down_rgt_up[] =
{
	DS_WAIT_SNAP,
	DS_MOVE_DEFENDER,0,20,
	DS_FACE,512,
	DS_RUN_YDS,5,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,6,
	DS_FACE,0,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_SPIN,
	DS_FAKE_OUT,256,40,30,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_IMPROVISE
};

// Used in Razzle Dazzle play - super fly
int pr_med_chair2[] =
{
//	DS_FACE,0,
//	DS_RUN_YDS,6,
//	DS_JUKE,
//	DS_FAKE_OUT,256,40,0,
	DS_FACE,-256,
	DS_RUN_YDS,11,
	DS_FACE,0,
	DS_FAKE_OUT,64,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Chicago 3
int pr_med_chair3[] =
{
//	DS_FACE,0,
//	DS_RUN_YDS,6,
//	DS_JUKE,
//	DS_FAKE_OUT,256,40,0,
	DS_FACE,-256,
	DS_RUN_YDS,7,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_JUKE,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,256,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,4,
	DS_WAVE_ARM,
	DS_TURBO_OFF,
	DS_RUN_YDS,30,
	DS_IMPROVISE
};

// Used in Cinci #1
int pr_med_chair4[] =
{
	DS_FACE,-256,
	DS_RUN_YDS,7,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,30,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_JUKE,
	DS_FAKE_OUT,-256,40,0,
	DS_JUKE,
	DS_FAKE_OUT,-384,40,30,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,256,
	DS_FAKE_OUT,-128,40,40,
	DS_TURBO_ON,
	DS_RUN_YDS,4,
	DS_WAVE_ARM,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

int pr_none[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,3,
	DS_IMPROVISE
};

// Used in Fake Punt play
int pr_fake_punt_l[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_JUKE,
	DS_RUN_YDS,6,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,256,50,0,
	DS_RUN_YDS,9,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Fake Punt play
int pr_fake_punt_c[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,60,25,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_RUN_YDS,10,
	DS_FACE,0,
	DS_FAKE_OUT,-256,60,25,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_FACE,-128,
	DS_FAKE_OUT,-256,60,25,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Fake Punt play
int pr_fake_punt_r[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,10,
	DS_JUKE,
	DS_FAKE_OUT,256,60,25,
	DS_FACE,-128,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,256,60,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Fake field goal play
int pr_fakefg_l[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,60,25,
	DS_FACE,128,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,-256,60,0,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in Fake field goal play
int pr_fakefg_c[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,-256,
	DS_RUN_YDS,2,
	DS_FACE,-224,
	DS_RUN_YDS,2,
	DS_FACE,-192,
	DS_RUN_YDS,2,
	DS_FACE,-160,
	DS_RUN_YDS,2,
	DS_FACE,-128,
	DS_RUN_YDS,2,
	DS_FACE,-96,
	DS_RUN_YDS,2,
	DS_FACE,-64,
	DS_RUN_YDS,2,
	DS_FACE,-32,
	DS_RUN_YDS,2,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_TURBO_OFF,
	DS_JUKE,
	DS_FAKE_OUT,384,75,30,
	DS_RUN_YDS,9,
	DS_FACE,128,
	DS_FAKE_OUT,0,75,30,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_FACE,256,
	DS_FAKE_OUT,384,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_SPIN,
	DS_FAKE_OUT,256,75,0,
	DS_FACE,-256,
	DS_RUN_YDS,14,
	DS_IMPROVISE
};

// Used in Fake field goal play
int pr_fakefg_r[] =
{
// This motion man can attach to the hike on
// fake FG attempts only
// DS_WAIT_SNAP,
	DS_FACE,0,
	DS_GOTO_LOS_PLUS,1,
	DS_JUKE,
	DS_FAKE_OUT,-256,60,25,
	DS_FACE,64,
	DS_TURBO_ON,
	DS_RUN_YDS,10,
	DS_FACE,0,
	DS_FAKE_OUT,-256,60,0,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in QBdeep play
int pr_QBdeep_LFT[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,7,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,0,
//	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_SPIN,
	DS_FACE,-256,
	DS_FAKE_OUT,512,45,30,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in general #5
int pr_QBdeep_LFT2[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,7,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,0,
//	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_SPIN,
	DS_FACE,-256,
	DS_FAKE_OUT,512,45,30,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_FACE,64,
	DS_FAKE_OUT,512,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in QBdeep play
int pr_QBdeep_RGT[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,8,
	DS_FACE,0,
	DS_FAKE_OUT,128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,8,
	DS_SPIN,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,30,
	DS_RUN_YDS,14,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in spider eyes play
int pr_spider_LFT[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,64,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,7,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_SPIN,
	DS_FACE,256,
	DS_FAKE_OUT,-128,45,30,
	DS_RUN_YDS,18,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in spider eyes play
int pr_spider_RGT[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,10,
	DS_FACE,0,
	DS_FAKE_OUT,-256,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,8,
	DS_SPIN,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,30,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,8,
	DS_FACE,384,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in zigzag play
int pr_zigzag_LFT[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,0,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,8,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_SPIN,
	DS_FACE,128,
	DS_FAKE_OUT,-128,45,30,
	DS_RUN_YDS,18,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in ARZ #3
int pr_zigzag_LFT2[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,64,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,8,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,5,
	DS_SPIN,
	DS_FACE,-128,
	DS_FAKE_OUT,256,55,35,
	DS_RUN_YDS,13,
	DS_FACE,0,
	DS_RUN_YDS,2,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,12,
	DS_FACE,-256,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in ATL #3
int pr_zigzag_LFT3[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,6,
	DS_FACE,256,
	DS_FAKE_OUT,0,40,30,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_TURBO_ON,
	DS_RUN_YDS,6,
	DS_TURBO_OFF,
	DS_RUN_YDS,14,
	DS_SPIN,
	DS_FACE,-128,
	DS_FAKE_OUT,256,55,35,
	DS_RUN_YDS,13,
	DS_FACE,0,
	DS_RUN_YDS,2,
	DS_FACE,-256,
	DS_FAKE_OUT,128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,12,
	DS_FACE,512,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in zigzag play
int pr_zigzag_RGT[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,0,
	DS_TURBO_ON,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,8,
	DS_FACE,-256,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,4,
	DS_TURBO_OFF,
	DS_RUN_YDS,2,
	DS_SPIN,
	DS_FACE,0,
	DS_FAKE_OUT,-160,40,30,
	DS_RUN_YDS,8,
	DS_FACE,256,
	DS_RUN_YDS,14,
	DS_TURBO_ON,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_FACE,128,
	DS_FAKE_OUT,-256,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,8,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in monkey play
int pr_monkey_LFT[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_SPIN,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,10,
	DS_SPIN,
	DS_FACE,0,
	DS_FAKE_OUT,-256,45,30,
	DS_RUN_YDS,18,
	DS_FACE,384,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in monkey play
int pr_monkey_RGT[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,-128,
	DS_TURBO_ON,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_RUN_YDS,4,
	DS_FACE,0,
	DS_FAKE_OUT,-256,40,0,
	DS_RUN_YDS,6,
	DS_FACE,256,
	DS_FAKE_OUT,-128,40,0,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_RUN_YDS,2,
	DS_SPIN,
	DS_FAKE_OUT,-128,40,30,
	DS_RUN_YDS,10,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in Atlanta #1
int pr_monkey_RGT2[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,90,
	DS_FACE,-144,
	DS_TURBO_ON,
	DS_FAKE_OUT,128,40,40,
	DS_RUN_YDS,13,
	DS_TURBO_OFF,
	DS_RUN_YDS,4,
	DS_FACE,0,
	DS_FAKE_OUT,-256,40,35,
	DS_RUN_YDS,8,
	DS_FACE,256,
	DS_TURBO_ON,
	DS_FAKE_OUT,-128,40,40,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_RUN_YDS,2,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_RUN_YDS,10,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_FACE,0,
	DS_FAKE_OUT,128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};
// Used in switch back play - rear LFT
int pr_switch_LFT[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_RUN_YDS,10,
	DS_SPIN,
	DS_FACE,0,
	DS_FAKE_OUT,192,45,30,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,8,
	DS_TURBO_OFF,
	DS_FAKE_OUT,128,40,30,
	DS_FACE,-256,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,0,60,30,
	DS_FACE,512,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,-128,40,30,
	DS_FACE,64,
	DS_TURBO_ON,
	DS_RUN_YDS,7,
	DS_TURBO_OFF,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

// Used in switch back play - rear RGT
int pr_switch_RGT[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,110,
	DS_FACE,0,
	DS_RUN_YDS,4,
	DS_JUKE,
	DS_FACE,-224,
	DS_FAKE_OUT,256,40,45,
	DS_RUN_YDS,14,
	DS_FACE,0,
	DS_FAKE_OUT,256,40,40,
	DS_RUN_YDS,24,
	DS_FACE,320,
	DS_TURBO_ON,
	DS_FAKE_OUT,0,40,0,
	DS_RUN_YDS,14,
	DS_TURBO_OFF,
	DS_IMPROVISE
};

//// Used in switch back play - rear LFT
//int pr_switch_LFT[] =
//{
//	DS_WAIT_SNAP,
//	DS_FACE,-128,
//	DS_RUN_YDS,10,
//	DS_SPIN,
//	DS_FACE,0,
//	DS_FAKE_OUT,192,45,30,
//	DS_TURBO_ON,
//	DS_RUN_YDS,1,
//	DS_WAVE_ARM,
//	DS_RUN_YDS,8,
//	DS_TURBO_OFF,
//	DS_FAKE_OUT,128,40,30,
//	DS_FACE,-256,
//	DS_RUN_YDS,5,
//	DS_FAKE_OUT,0,60,30,
//	DS_FACE,512,
//	DS_RUN_YDS,5,
//	DS_FAKE_OUT,-128,40,30,
//	DS_FACE,64,
//	DS_TURBO_ON,
//	DS_RUN_YDS,7,
//	DS_TURBO_OFF,
//	DS_RUN_YDS,20,
//	DS_IMPROVISE
//};
//
//// Used in switch back play - rear RGT
//int pr_switch_RGT[] =
//{
//	DS_WAIT_SNAP,
//	DS_DELAY_BLOCK,110,
//	DS_FACE,0,
//	DS_RUN_YDS,4,
//	DS_JUKE,
//	DS_FACE,-224,
//	DS_FAKE_OUT,256,40,45,
//	DS_RUN_YDS,14,
//	DS_FACE,0,
//	DS_FAKE_OUT,256,40,40,
//	DS_RUN_YDS,24,
//	DS_FACE,320,
//	DS_TURBO_ON,
//	DS_FAKE_OUT,0,40,0,
//	DS_RUN_YDS,14,
//	DS_TURBO_OFF,
//	DS_IMPROVISE
//};

// Used in cruisn play
int pr_cruisn_HB[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_FACE,0,
	DS_RUN_YDS,6,
	DS_TURBO_ON,
	DS_FAKE_OUT,-128,40,35,
	DS_FACE,-256,
	DS_RUN_YDS,7,
	DS_FACE,128,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,128,40,35,
	DS_FACE,-128,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,-256,40,35,
	DS_FACE,128,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,128,40,35,
	DS_TURBO_OFF,
	DS_FACE,-128,
	DS_RUN_YDS,5,
	DS_FACE,256,
	DS_FAKE_OUT,-128,40,45,
	DS_RUN_YDS,20,
	DS_IMPROVISE
};

// Used in cruisn play
int pr_cruisn_RGT[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_FACE,0,
	DS_RUN_YDS,8,
	DS_FAKE_OUT,-128,40,35,
	DS_FACE,128,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,12,
	DS_SPIN,
	DS_FAKE_OUT,0,40,35,
	DS_FACE,-256,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,30,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in hurricane play - left guy in trips
int pr_hurricane_HB[] =
{
	DS_WAIT_SNAP,
	DS_DELAY_BLOCK,60,
	DS_MOVE_DEFENDER,256,120,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_RUN_YDS,16,
	DS_TURBO_OFF,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,35,
	DS_RUN_YDS,7,
	DS_FACE,0,
	DS_FAKE_OUT,128,40,35,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,2,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,128,40,35,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_FAKE_OUT,0,40,35,
	DS_FACE,-384,
	DS_RUN_YDS,5,
	DS_FAKE_OUT,0,40,35,
	DS_FACE,-256,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in hurricane play - middle guy
int pr_hurricane_LFT[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_RUN_YDS,4,
	DS_SPIN,
	DS_FAKE_OUT,-128,40,35,
	DS_FACE,256,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,4,
	DS_TURBO_OFF,
	DS_RUN_YDS,7,
	DS_FAKE_OUT,-128,40,35,
	DS_FACE,128,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,10,
	DS_TURBO_OFF,
	DS_RUN_YDS,9,
	DS_FACE,0,
	DS_RUN_YDS,10,
	DS_FAKE_OUT,0,40,35,
	DS_FACE,-256,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in hurricane play - rgt most guy in trips
int pr_hurricane_RGT[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_RUN_YDS,8,
	DS_FACE,0,
	DS_FAKE_OUT,256,45,30,
	DS_TURBO_ON,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_TURBO_ON,
	DS_FACE,256,
	DS_FAKE_OUT,0,45,30,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,45,30,
	DS_RUN_YDS,10,
	DS_FACE,320,
	DS_RUN_YDS,10,
	DS_IMPROVISE
};

// Used in side slide play
int pr_slide_LFT[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,-256,40,40,
	DS_SPIN,
	DS_RUN_YDS,15,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,0,
	DS_TURBO_OFF,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_SPIN,
	DS_FACE,256,
	DS_FAKE_OUT,-256,45,40,
	DS_RUN_YDS,7,
	DS_TURBO_ON,
	DS_FAKE_OUT,512,45,30,
	DS_FACE,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,-384,
	DS_FAKE_OUT,128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in RAVENS PLAY #1
int pr_slide_LFT2[] =
{
	DS_WAIT_SNAP,
	DS_TURBO_ON,
	DS_FACE,-128,
	DS_FAKE_OUT,-256,40,40,
	DS_SPIN,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-256,40,0,
	DS_TURBO_OFF,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,15,
	DS_SPIN,
	DS_FACE,256,
	DS_FAKE_OUT,-256,45,40,
	DS_RUN_YDS,17,
	DS_TURBO_ON,
	DS_FAKE_OUT,512,45,30,
	DS_FACE,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,-384,
	DS_FAKE_OUT,128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};
// Used in side slide play
int pr_slide_RGT[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_FAKE_OUT,256,40,40,
	DS_RUN_YDS,12,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_OFF,
	DS_RUN_YDS,3,
	DS_RUN_YDS,14,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in arz #1
int pr_slide_RGT2[] =
{
	DS_WAIT_SNAP,
	DS_FACE,-128,
	DS_FAKE_OUT,256,40,40,
	DS_RUN_YDS,12,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_SPIN,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_OFF,
	DS_RUN_YDS,8,
	DS_FACE,0,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_FAKE_OUT,-128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in hook n ladder play
int pr_hook_LFT[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_FAKE_OUT,-256,40,40,
	DS_SPIN,
	DS_RUN_YDS,5,
	DS_FACE,-128,
	DS_TURBO_ON,
	DS_FAKE_OUT,128,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,5,
	DS_FACE,128,
	DS_SPIN,
	DS_FAKE_OUT,-256,45,40,
	DS_RUN_YDS,7,
	DS_TURBO_ON,
	DS_FAKE_OUT,512,45,30,
	DS_FACE,0,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_TURBO_OFF,
	DS_RUN_YDS,10,
	DS_FACE,-384,
	DS_FAKE_OUT,128,40,0,
	DS_TURBO_ON,
	DS_RUN_YDS,5,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in hook n ladder play
int pr_hook_RGT[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FACE,-164,
	DS_FAKE_OUT,128,45,0,
	DS_RUN_YDS,12,
	DS_FACE,0,
	DS_FAKE_OUT,128,30,30,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,6,
	DS_SPIN,
	DS_FACE,320,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_OFF,
	DS_RUN_YDS,14,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,0,
	DS_RUN_YDS,5,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// Used in Ravens #3
int pr_hook_RGT2[] =
{
	DS_WAIT_SNAP,
	DS_FACE,0,
	DS_RUN_YDS,1,
	DS_JUKE,
	DS_FACE,-164,
	DS_FAKE_OUT,64,50,40,
	DS_RUN_YDS,10,
	DS_FACE,0,
	DS_FAKE_OUT,256,40,20,
	DS_TURBO_ON,
	DS_RUN_YDS,2,
	DS_WAVE_ARM,
	DS_RUN_YDS,3,
	DS_SPIN,
	DS_FACE,256,
	DS_FAKE_OUT,-256,40,30,
	DS_TURBO_OFF,
	DS_RUN_YDS,24,
	DS_FACE,0,
	DS_FAKE_OUT,512,40,0,
	DS_RUN_YDS,5,
	DS_TURBO_ON,
	DS_RUN_YDS,3,
	DS_TURBO_OFF,
	DS_RUN_YDS,15,
	DS_FACE,-256,
	DS_FAKE_OUT,256,40,0,
	DS_RUN_YDS,1,
	DS_WAVE_ARM,
	DS_RUN_YDS,7,
	DS_IMPROVISE
};

// **********
// PASS plays
// **********

//*****************************************************************************************

//	TEAM GENERAL PLAYS FOR FIRST 4 PAGES
play_t	play_gen_1 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_short_slip,pr_med_up_long_slant,pr_med_chair4}
};

play_t	play_gen_2 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
//	pr_QB_sweep,
	pr_instant_slant11,pr_short_slip7,pr_med_up_cut2}
};

play_t	play_gen_3 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
//	pr_QB_block,
//	pr_QBgoto_endzone,	//QB after pass
	pr_long_slant_cut_up2,pr_short_slip8,pr_med_up_slant_lft4}
};

play_t	play_gen_4 = 
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
	pr_QBgoto_endzone,	//QB after pass
	pr_short_slip7,pr_long_up_rgt_dwn,pr_up_left_cut_rgt}
};

play_t	play_gen_5 = 
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_QBdeep_LFT2,pr_zigzag_RGT,pr_up_left_cut_rgt5}
};

play_t	play_gen_6 = 
{
	formation_1wr2back,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_short_in2,pr_short_up_long_slant3,pr_instant_slant13}
};

//*****************************************************************************************

//	TEAM_CARDINALS
play_t play_az_1 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
//	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_QBdeep_HB2,pr_QBdeep_LFT,pr_slide_RGT2}
};


play_t play_az_2 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_run_behind_qb_up2,pr_instant_slant2,pr_goto_endzone}
};

play_t play_az_3 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
//	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_short_up_long_slant,pr_long_up_rgt_dwn,pr_zigzag_LFT2}
};

//*****************************************************************************************

//	TEAM_FALCONS
play_t play_at_1 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
//	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_zigzag_HB,pr_monkey_RGT2,pr_med_up_cut_rgt}
};


play_t play_at_2 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
//	pr_QB_sweep,
	pr_QBgoto_endzone2,	//QB after pass
	//hb,lft WR,rgt WR
	pr_run_behind_qb_up2,pr_short_slip2,pr_instant_slant4}
};

play_t play_at_3 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
//	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_med_hitch_up2,pr_instant_slant9,pr_instant_slant8}
};

										
//*****************************************************************************************

//	TEAM_RAVENS
play_t play_ba_1 =
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
//	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_short_up_long_slant2,pr_long_up_rgt_dwn3,pr_zigzag_LFT3}
};


play_t play_ba_2 =
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_slide_LFT2,pr_med_thru_middle,pr_QBdeep_RGT}
};

play_t play_ba_3 =
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
//	pr_QB_sweep,
//	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_hook_RGT2,pr_spider_HB,pr_instant_slant6,}
};

//*****************************************************************************************

//	TEAM_BILLS
play_t	play_bu_1 =
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
//	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_med_chair,pr_med_slip,pr_instant_slant7}
};

play_t	play_bu_2 =
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
	pr_QBgoto_endzone,	//QB after pass
	pr_up_left_cut_rgt4,pr_short_slip,pr_short_loop_up}

//	formation_trips,
//	0,
//	{ds_none,ds_none,ds_none,
//	pr_QB_sweep,
//	pr_short_up_cut_rgt_up,pr_med_up_slant_lft3,pr_long_up_long_slant}
};

play_t	play_bu_3 =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_up_left_cut_rgt3,pr_long_up_rgt_dwn2,pr_short_down_rgt_up}
};

//*****************************************************************************************

//	TEAM_PANTHERS
play_t	play_ca_1 =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_short_up_cut_rgt_up,pr_med_up_slant_lft3,pr_long_up_long_slant}
};

play_t	play_ca_2 =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_short_slip6,pr_med_up_slant_lft2,pr_very_short_up3}
};

play_t	play_ca_3 =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_sweep,
	pr_QBgoto_endzone,	//QB after pass
	pr_short_slip,pr_short_slip2,pr_long_up_rgt_dwn4}
};

//*****************************************************************************************

//	TEAM_CHICAGO
play_t	play_ch_1 =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_short_slip4,pr_long_slant_cut_up2,pr_med_up_slant_lft}
};

play_t	play_ch_2 =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
//	pr_QB_sweep,
	pr_med_up_cut2,pr_short_slip9,pr_instant_slant10}
};

play_t	play_ch_3 =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_sweep,
	pr_QB_block,
//	pr_QBgoto_endzone,	//QB after pass
	pr_short_slip,pr_med_up_long_slant,pr_med_chair3}
};


//*****************************************************************************************

//	TEAM_CINCINATTI
play_t	play_ci_1 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_short_slip,pr_med_up_long_slant,pr_med_chair4}
};

play_t	play_ci_2 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
//	pr_QB_sweep,
	pr_instant_slant11,pr_short_slip7,pr_med_up_cut2}
};

play_t	play_ci_3 =
{
	formation_stack,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
//	pr_QB_block,
//	pr_QBgoto_endzone,	//QB after pass
	pr_long_slant_cut_up2,pr_short_slip8,pr_med_up_slant_lft4}
};


play_t play_QBdeep =
{
	formation_tightends,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_QBdeep_HB,pr_QBdeep_LFT,pr_QBdeep_RGT}
};

play_t play_side_slide =
{
	formation_tightends,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_slide_HB,pr_slide_LFT,pr_slide_RGT}
};

play_t play_hooknladder =
{
	formation_tightends,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QB_block,
// QB will run downfield as soon as he gets rid of the ball
// to H vgB also!
	pr_QBgoto_endzone,	//QB after pass
	//hb,lft WR,rgt WR
	pr_hook_HB,pr_hook_LFT,pr_hook_RGT}
};

play_t play_spider =
{
	formation_sidei,
	0,
	{ds_none,ds_none,ds_none,
	pr_QBgoto_endzone,
	//hb,lft WR,rgt WR
	pr_spider_HB,pr_spider_LFT,pr_spider_RGT}
};

play_t play_zigzag =
{
	formation_sidei,
	0,
	{ds_none,ds_none,ds_none,
	pr_QBgoto_endzone,
	//hb,lft WR,rgt WR
	pr_zigzag_HB,pr_zigzag_LFT,pr_zigzag_RGT}
};

play_t play_switch =
{
	formation_1wr2back,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QBgoto_endzone,
	pr_QB_block,
	//hb,lft WR,rgt WR
	pr_switch_HB,pr_switch_LFT,pr_switch_RGT}
};

play_t play_cruisn =
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QBgoto_endzone,
	pr_QB_block,
	//hb,lft WR,rgt WR
	pr_cruisn_HB,pr_cruisn_LFT,pr_cruisn_RGT}
};

play_t play_hurricane =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
//	pr_QBgoto_endzone,
	pr_QB_block,
	//hb,lft WR,rgt WR
	pr_hurricane_HB,pr_hurricane_LFT,pr_hurricane_RGT}
};

play_t play_monkey =
{
	formation_sidei,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_monkey_HB,pr_monkey_LFT,pr_monkey_RGT}
};

play_t play_upper_cut =
{
	formation_2plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
//	pr_run_behind_qb_up,pr_med_hitch_up,pr_instant_slant}
	pr_instant_slant,pr_med_hitch_up,pr_run_behind_qb_up}
};

play_t play_blizzard =
{
	formation_2plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
//	pr_med_up_cut,pr_instant_slant2,pr_short_slant_in}
	pr_short_slant_in,pr_instant_slant2,pr_med_up_cut}
};


play_t  play_hail_mary =
{
//	formation_trips,
	formation_2plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
//	pr_goto_endzone,pr_goto_endzone2,pr_goto_endzone3}
	pr_goto_endzone,pr_med_chair,pr_goto_endzone2}
};

play_t  play_quake_shake =			//Turmoil play
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_long_slant_rgt_up,pr_med_slip,pr_med_slant_up_lft}
};

play_t	play_upthe_gut = 
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_med_up,pr_med_thru_middle,pr_rgt_up_cut_left}
};

play_t	play_criss_cross = 
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_instant_slant3,pr_short_slip,pr_instant_slant4}
};

play_t	play_dawg_hook = 
{
	formation_4lineman,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_long_up_rgt_dwn,pr_short_slip2,pr_up_left_cut_rgt}
};

play_t	play_blitz_beater =
{
	formation_1wr2back,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_med_up_lft_cut_rgt,pr_short_slip3,pr_short_loop_up}
};

play_t	play_screen_rgt =
{
	formation_1wr2back,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_med_up_slant_lft,pr_long_slant_cut_up,pr_short_slip4}
};

play_t	play_one_man_back =		//HB Block play
{
	formation_1plus1plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_med_up_cut2,pr_very_short_up,pr_short_in}
};

play_t	play_cross_slant =
{
	formation_1plus1plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_instant_slant5,pr_short_slip5,pr_up_left_cut_rgt2}
};

play_t	play_reverse_zip =
{
	formation_1plus1plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_med_up_cut_cut,pr_very_short_up2,pr_run_behind_qb_up2}
};

play_t	play_sweep_rgt =
{
	formation_1plus1plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_med_up_slant_lft2,pr_short_slip6,pr_very_short_up3}
};

play_t	play_middle_pick =
{
	formation_2plus1,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_med_up_lft_cut_rgt2,pr_med_thru_middle2,pr_short_up_lft_slant}
};

play_t	play_da_bomb =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_block,
	pr_med_up_long_slant,pr_short_up_long_slant,pr_goto_endzone3}
};

play_t	play_utb_deep =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_short_up_cut_rgt_up,pr_med_up_slant_lft3,pr_long_up_long_slant}
};

play_t	play_subzero =
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_up_left_cut_rgt3,pr_long_up_rgt_dwn2,pr_short_down_rgt_up}
};

play_t	play_razzle_dazzle = //- super fly
{
	formation_trips,
	0,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_med_chair2,pr_short_up_lft_slant2,pr_med_up_cut_cut2}
};


// *********************
// OFFENSIVE MISC. PLAYS
// *********************
play_t play_punt =
{
	formation_punts,
	PLF_PUNT,
	{ds_none,ds_none,ds_none,
	ds_none,
	pr_none,pr_none,pr_none}
};

play_t play_fake_punt =
{
	formation_punts,
	PLF_FAKE_PUNT,
	{ds_none,ds_none,ds_none,
	ds_none,
	pr_fake_punt_r,pr_fake_punt_l,pr_fake_punt_c}
};

play_t play_field_goal =
{
	formation_field_goal,
	PLF_FIELD_GOAL,
	{ds_none,ds_none,ds_none,
	ds_none,
	pr_none,pr_none,pr_none}
};

play_t play_fake_field_goal =
{
	formation_field_goal,
	PLF_FAKE_FG,
	{ds_none,ds_none,ds_none,
	pr_QB_sweep,
	pr_fakefg_l,pr_fakefg_c,pr_fakefg_r}
};


play_t play_extra_point =
{
	formation_field_goal,
	0,
	{ds_none,ds_none,ds_none,
	ds_none,
	pr_none,pr_none,pr_none}
};


// *************
// DEFENSE PLAYS
// *************
play_t play_3_man_cover = 		//3 MAN COVER = 1 MAN BLITZ
{
	def_formation_c,
	0,
	{ds_none,ds_none,ds_none,
	db_man,db_zone_dw,db_rush_qb,db_man}
};

play_t play_mix_it_up =
{
	def_formation_k,
	PLF_REDZONE|PLF_LINEDIVE|PLF_BLITZ|PLF_FAKEZONE,
	{db_rush_qb,db_rush_qb,ds_none,
	db_zone_sl2,db_zone_dw,db_rush_qb,db_man}
};

play_t play_spread =
{
	def_formation_j,
	PLF_REDZONE|PLF_BLITZ|PLF_CHEAPSHOT,
	{ds_none,db_rush_qb,db_rush_qb,
	db_man,db_man,db_rush_qb,db_man}
};

play_t play_goal_line =
{
	def_formation_b,
	PLF_REDZONE,
	{ds_none,ds_none,
	db_zone_sl2,db_zone_sr2,db_man,db_man,db_man}
};

// New Ass kicking goal line stance - used fromn 15 yd line in
play_t play_stuff_it =
{
	def_formation_b,
//	PLF_REDZONE|PLF_LINEDIVE,
	PLF_REDZONE|PLF_CHEAPSHOT,
	{db_rush_qb,ds_none,
	db_zone_sl2,db_zone_sr2,db_man,db_man,db_man}
};

//Used rest of field
play_t play_stuff_it2 =
{
	def_formation_b,
	PLF_REDZONE|PLF_LINEDIVE,
	{ds_none,ds_none,
	db_zone_sl2,db_zone_sr2,db_man,db_man,db_man}
};

play_t play_2_man_blitz =
{
	def_formation_c,
	PLF_BLITZ,
	{ds_none,ds_none,ds_none,
	db_rush_qb,db_zone_dw,db_rush_qb,db_man}
};

play_t play_near_zone =
{
	def_formation_f,
	0,
	{ds_none,ds_none,db_zone_sl3,
	db_zone_sm3,db_zone_dl2,db_zone_dr2,db_zone_sr3}
};

play_t play_stop_run =
{
	def_formation_i,
	PLF_REDZONE|PLF_CHEAPSHOT|PLF_FAKEZONE,
	{ds_none,ds_none,db_zone_sl3,
	db_zone_sm3,db_man,db_man,db_zone_sr3}
};

play_t play_suicide_blitz =
{
	def_formation_a,
	PLF_BLITZ|PLF_REDZONE,
	{ds_none,ds_none,ds_none,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

play_t play_safe_cover =
{ 
	def_formation_c,
	0,
	{ds_none,ds_none,ds_none,
	db_man,db_zone_dw,db_man,db_man}
};

play_t play_man_on_man =
{ 
	def_formation_l,
	0,
	{ds_none,ds_none,ds_none,
	db_man,db_zone_dw,db_man,db_man}
};

play_t play_left_zone =
{
	def_formation_g,
	PLF_REDZONE|PLF_CHEAPSHOT,
	{db_rush_qb,ds_none,db_zone_sl2,
	db_zone_dl3,db_zone_dl2,db_man,db_man}
};

play_t play_right_zone =
{
	def_formation_h,
	PLF_REDZONE|PLF_CHEAPSHOT,
	{db_rush_qb,ds_none,db_zone_sr2,
	db_man,db_zone_dr2,db_zone_dr3,db_man}
};

play_t play_3_man_blitz =
{
	def_formation_c,
	PLF_BLITZ|PLF_REDZONE|PLF_CHEAPSHOT|PLF_LINEDIVE,
	{ds_none,db_rush_qb,ds_none,
	db_rush_qb,db_zone_dl2,db_zone_dr2,db_rush_qb}
};

play_t play_medium_zone =
{
	def_formation_d,
	0,
	{db_rush_qb,ds_none,ds_none,
	db_zone_sl2,db_zone_dl2,db_zone_dr2,db_zone_sr2}
};

play_t play_fake_zone =
{
	def_formation_d,
	PLF_FAKEZONE|PLF_REDZONE,
	{ds_none,db_rush_qb,ds_none,
	db_zone_sl2,db_zone_dl2,db_zone_dr2,db_zone_sr2}
};

play_t play_deep_zone =
{
	def_formation_e,
	0,
	{ds_none,ds_none,db_zone_sl2,
	db_zone_sr2,db_zone_dl3,db_zone_dm3,db_zone_dr3}
};

play_t play_knock_down =
{
	def_formation_e,
	PLF_CHEAPSHOT|PLF_SMART_INT|PLF_REDZONE,
	{ds_none,ds_none,db_zone_sl2,
	db_zone_sr2,db_zone_dl3,db_zone_dm3,db_zone_dr3}
};

// defensive special team plays
play_t play_block_punt =
{
	def_formation_a,
	0,
	{ds_none,ds_none,ds_none,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

play_t play_block_fg =
{
	def_formation_a,
	0,
	{ds_none,ds_none,ds_none,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

play_t play_punt_return =
{
	def_formation_punt_return,
	0,
	{ds_none,ds_none,ds_none,
	db_man,db_man,db_man,db_man}
};


// *************
// SPECIAL PLAYS
// *************
play_t play_kickoff_313 =
{
	for_kickoff_313,
	0,
	{db_rush_qb,db_rush_qb,db_rush_qb,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

play_t play_kickoff2_313 =
{
	for_kickoff2_313,
	0,
	{db_rush_qb,db_rush_qb,db_rush_qb,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

play_t play_onside_313 =
{
	for_kickoff_313,
	PLF_ONSIDE_KICK,
	{db_rush_qb,db_rush_qb,db_rush_qb,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

play_t play_kick_return =
{
	for_kick_return,
	0,
	{ds_block,ds_block,ds_block,
	ds_block,ds_block,ds_block,ds_block}
};

play_t play_onside_kick_return =
{
	for_onside_kick_return,
	0,
	{ds_block,ds_block,ds_block,
	ds_block,ds_block,ds_block,ds_block}
};

play_t play_kickoff_short =
{
	for_kickoff_short,
	0,
	{db_rush_qb,db_rush_qb,db_rush_qb,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

play_t play_kick_return_short =
{
	for_kick_return_short,
	0,
	{db_rush_qb,db_rush_qb,db_rush_qb,
	db_rush_qb,db_rush_qb,db_rush_qb,db_rush_qb}
};

