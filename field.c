/****************************************************************************/
/*                                                                          */
/* field.c - field & stadium code                                           */
/*                                                                          */
/* Written by:  Jason Skiles/Japple                                         */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* $Revision: 64 $                                                          */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "/Video/Nfl/Include/ani3D.h"
#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/profile.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/Cambot.h"
#include "/Video/Nfl/Include/Field.h"
#include "/video/nfl/include/game_cfg.h"

char *snow_field_textures[] =
{
	"Beabg.wms",
	"Beaez1.wms",
	"Beaez2.wms",
	"Beaez3.wms",
	"ads_s.wms",
	"blitz_s.wms",
	"grass1_s.wms",
	"grass1_s.wms",
	"grass2_s.wms",
	"grass3_s.wms",
	"grass3_s.wms",
	"nfl_s.wms",
	"nflpa_s.wms",
	"nums_s.wms",
	"post_s.wms",
	"roof_s.wms",
	NULL
};


char *astro_turf_field_textures[] =
{
	"Beabg.wms",
	"Beaez1.wms",
	"Beaez2.wms",
	"Beaez3.wms",
	"ads98.wms",
	"blitz98.wms",
	"grass1_t.wms",
	"grass1_t.wms",
	"grass2_t.wms",
	"grass3_t.wms",
	"grass3_t.wms",
	"nfl98.wms",
	"nflpa98.wms",
	"nums_t.wms",
	"post98.wms",
	"roof98.wms",
	NULL
};

char *dirt_field_textures[] =
{
	"Beabg.wms",
	"Beaez1.wms",
	"Beaez2.wms",
	"Beaez3.wms",
	"ads98.wms",
	"blitz98.wms",
	"grass1_d.wms",
	"grass1_d.wms",
	"grass2_d.wms",
	"grass3_d.wms",
	"grass3_d.wms",
	"nfl98.wms",
	"nflpa98.wms",
	"nums_d.wms",
	"post98.wms",
	"roof98.wms",
	NULL
};


char *day_stadium_textures[] =
{
	"ads_d.wms",
	"blitz_d.wms",
	"ceil_d.wms",
	"crowdd_d.wms",
	"crowdl_d.wms",
	"post_d.wms",
	"roof_d.wms",
	"skybox_d.wms",
	"trim_d.wms",
	"trim_d.wms",
	"wall_d.wms",
	NULL
};

char *snow_stadium_textures[] =
{
	"ads_s.wms",
	"blitz_s.wms",
	"ceil_s.wms",
	"crowdd_s.wms",
	"crowdl_s.wms",
	"post_s.wms",
	"roof_s.wms",
	"skybox_s.wms",
	"trim_s.wms",
	"trim_s.wms",
	"wall_s.wms",
	NULL
};


char *asphalt_field_textures[] =
{
	"Beabg.wms",
	"Beaez1.wms",
	"Beaez2.wms",
	"Beaez3.wms",
	"ads98.wms",
	"blitz_a.wms",
	"grass1_a.wms",
	"grass1_a.wms",
	"grass2_a.wms",
	"grass3_a.wms",
	"grass3_a.wms",
	"nfl_a.wms",
	"nflpa_a.wms",
	"nums_a.wms",
	"post98.wms",
	"roof98.wms",
	NULL
};

char *mud_field_textures[] = {
	"Beabg.wms",
	"Beaez1.wms",
	"Beaez2.wms",
	"Beaez3.wms",
	"ads98.wms",
	"blitz_m.wms",
	"grass1_m.wms",
	"grass1_m.wms",
	"grass2_m.wms",
	"grass3_m.wms",
	"grass3_m.wms",
	"nfl_m.wms",
	"nflpa_m.wms",
	"nums_m.wms",
	"post98.wms",
	"roof98.wms",
	NULL
};

int os_zones[18] = {1,1,1,1,1,1, 1,1,1,1,1,1, 1,1,1,1,1,1};
int cur_zone = 0;


// These bits must coorespond to fields in the structure below
#define	LDI_MCF_DMODE	1
#define	LDI_MCF_DFUNC	2
#define	LDI_MCF_DMASK	4
#define	LDI_MCF_DBIAS	8
#define	LDI_MCF_CFUNC	16
#define	LDI_MCF_ASRC	32
#define	LDI_MCF_TFUNC	64
#define	LDI_MCF_CMODE	128
#define	LDI_MCF_RGBS	256
#define	LDI_MCF_RGBD	512
#define	LDI_MCF_AS		1024
#define	LDI_MCF_AD		2048
#define	LDI_MCF_CKMODE	4096
#define	LDI_MCF_CVAL	8192
#define	LDI_MCF_AFUNC	16384
#define	LDI_MCF_AVAL	32768

#define	LDI_MCF_ALL	(LDI_MCF_DMODE|LDI_MCF_DFUNC|LDI_MCF_DMASK|LDI_MCF_DBIAS| \
							LDI_MCF_CFUNC|LDI_MCF_ASRC|LDI_MCF_TFUNC|LDI_MCF_CMODE| \
							LDI_MCF_RGBS|LDI_MCF_RGBD|LDI_MCF_AS|LDI_MCF_AD|LDI_MCF_CKMODE| \
							LDI_MCF_CVAL|LDI_MCF_AFUNC|LDI_MCF_AVAL)

typedef struct limb_draw_info
{
	LIMB	*limb;				// Pointer to the LIMB to draw
	int	mode_change_flags;		// Mode change flags
	int	depth_mode;				// Depth mode
	int	depth_func;				// Depth buffer function
	int	depth_mask;				// Depth mask
	int	depth_bias;				// Depth bias
	int	cc_function;			// Color Combiner function
	int	alpha_source;			// Alpha source
	int	tc_function;			// Texture combiner function
	int	clamp_mode;				// S and T clamp modes
	int	rgb_src; 				// Source RGB Blend function
	int	rgb_dst; 				// Destination RGB Blend function
	int	a_src;					// Source Alpha Blend function
	int	a_dst;					// Destination Alpha Blend function
	int	chroma_mode;			// Chroma key mode
	int	chroma_key_value;		// Chroma key value
	int	alpha_test_function;	// Alpha test mode
	int	alpha_test_value;		// Alpha test value
	int	cull_mode;				// Culling mode
	int zone1,zone2;			// zone index (0-17)
} limb_draw_info_t;


//
// local data
//
#ifdef SEATTLE
static GrMipMapId_t		field_decals[32];
#else
static Texture_node_t	*field_decals[32];
#endif
static int				field_ratios[32];
static int				fez_tex_map[4] = {0, 0, 0, 0};
#ifdef SEATTLE
static GrMipMapId_t		stadium_decals[32];
#else
static Texture_node_t	*stadium_decals[32];
#endif
static int				stadium_ratios[32];
static int				field_texture_cnt;
static int				stadium_texture_cnt;
static int				is_foggy = 0;
int	fog_color;
float fog_thickness;

//static int				is_night = 0;
//static int				is_snow = 0;
//static int				is_turf = 0;

static void draw_field( void * );
static void draw_field2( void * );
static void draw_fogon( void * );
static void draw_fogoff( void * );
static void do_os_zones(void);
static void load_field_textures(void);
static void load_stadium_textures(void);

//
// external data
//
// striped field with new stadium
extern char *field2_textures[];
extern char *stadium_textures[];
//extern LIMB *stadium2_limbs[];

// BLITZ field and stadium
//extern char *field_textures[];
//extern LIMB *stadium_limbs[];
//extern LIMB *stadium_limbs2[];
//extern char *stadium_textures[];

// Snow Field limbs from models\stadium2.h
extern LIMB	limb_snow_obj;

// Field2 limbs from models\stadium2.h
extern LIMB	limb_adshadows98_obj;
extern LIMB	limb_goalpost298_obj;
extern LIMB	limb_goalpost198_obj;
extern LIMB	limb_yardmarkers98_obj;
extern LIMB	limb_pylons98_obj;
extern LIMB	limb_outershadow98_obj;
extern LIMB	limb_shadows98_obj;
extern LIMB	limb_nfllogo98_obj;
extern LIMB	limb_fieldnumbers98_obj;
extern LIMB	limb_mainfield98_obj;
extern LIMB	limb_ezlogo98_obj;
extern LIMB	limb_ezstripe98_obj;
extern LIMB	limb_outerlines98_obj;
extern LIMB	limb_outerfield98_obj;

//// STADIUM2 limbs from models\stadium2.h
extern LIMB	limb_exits98_obj;
extern LIMB	limb_lowertrim98_obj;
extern LIMB	limb_lowercrowd98_obj;
extern LIMB	limb_tunnels98_obj;
extern LIMB	limb_middlecrowd98_obj;
extern LIMB	limb_uppertrim98_obj;
extern LIMB	limb_skybox98_obj;
extern LIMB	limb_skybox298_obj;
extern LIMB	limb_uppercrowd98_obj;
extern LIMB	limb_jumbo98_obj;
extern LIMB	limb_jumbo298_obj;
extern LIMB	limb_roof198_obj;
extern LIMB	limb_roof298_obj;
extern LIMB	limb_supports198_obj;
extern LIMB	limb_supports298_obj;
extern LIMB	limb_skydome98_obj;


//// Field limbs from models\field.h
//extern LIMB	limb_outerfield_obj;
//extern LIMB	limb_mainfield_obj;
//extern LIMB	limb_ezstripe_obj;
//extern LIMB	limb_ezlogo_obj;
//extern LIMB	limb_fieldnumbers_obj;
//extern LIMB	limb_nfllogo_obj;
//extern LIMB	limb_outerlines_obj;
//extern LIMB	limb_shadows_obj;
//extern LIMB	limb_yardmarkers_obj;
//extern LIMB	limb_pylons_obj;
//extern LIMB	limb_goalpost_obj;
//extern LIMB	limb_goalpost2_obj;
//
//// Stadium limbs from models\stadium.h
//extern LIMB	limb_tunnelfloor_obj;
//extern LIMB	limb_upperbanners_obj;
//extern LIMB	limb_lowerbanners_obj;
//extern LIMB	limb_scoreboard_obj;
//extern LIMB	limb_upperjumbo_obj;
//extern LIMB	limb_uppercrowd_obj;
//extern LIMB	limb_lowershadow_obj;
//extern LIMB	limb_lowertrim_obj;
//extern LIMB	limb_lowerrail_obj;
//extern LIMB	limb_lowercrowd_obj;
//extern LIMB	limb_backwall_obj;
//extern LIMB	limb_bleachersalpha_obj;
//extern LIMB	limb_jumbo_obj;
//extern LIMB	limb_uppertrim_obj;
//extern LIMB	limb_bleachers_obj;
//extern LIMB	limb_lights_obj;
//extern LIMB	limb_screen1_obj;
//extern LIMB	limb_screen2_obj;
//extern LIMB	limb_skydome_obj;


void set_limb_mode(limb_draw_info_t *di);
void init_limb_draw_info(limb_draw_info_t *di, int num);


//
// This controls the draw order for limbs that make up the field2 (blitz '99)
//
static limb_draw_info_t field2_limb_draw_info[] = {
{
&limb_outerfield98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_ezstripe98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_ezlogo98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_ALWAYS,
FXTRUE,
10,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_mainfield98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_fieldnumbers98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_ALWAYS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_nfllogo98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_ALWAYS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_shadows98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_ALWAYS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
},
//{
//&limb_outershadow98_obj,
//0,
//GR_DEPTHBUFFER_WBUFFER,
//GR_CMP_ALWAYS,
//FXTRUE,
//0,
//GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
//GR_ALPHASOURCE_TEXTURE_ALPHA,
//GR_TEXTURECOMBINE_DECAL,
//GR_TEXTURECLAMP_CLAMP,
//GR_BLEND_SRC_ALPHA,
//GR_BLEND_ONE_MINUS_SRC_ALPHA,
//GR_BLEND_SRC_ALPHA,
//GR_BLEND_ONE_MINUS_SRC_ALPHA,
//GR_CHROMAKEY_DISABLE,
//0,
//GR_CMP_GREATER,
//0x10,
////0x20,
////0x40,
//GR_CULL_NEGATIVE
//},
{
&limb_adshadows98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_ALWAYS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
},
//{
//&limb_snow_obj,
//0,
//GR_DEPTHBUFFER_WBUFFER,
//GR_CMP_ALWAYS,
//FXTRUE,
//0,
//GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
//GR_ALPHASOURCE_TEXTURE_ALPHA,
//GR_TEXTURECOMBINE_DECAL,
//GR_TEXTURECLAMP_CLAMP,
//GR_BLEND_ONE,
//GR_BLEND_ZERO,
//GR_BLEND_ONE,
//GR_BLEND_ZERO,
////GR_BLEND_SRC_ALPHA,
////GR_BLEND_ONE_MINUS_SRC_ALPHA,
////GR_BLEND_SRC_ALPHA,
////GR_BLEND_ONE_MINUS_SRC_ALPHA,
//GR_CHROMAKEY_DISABLE,
//0,
//GR_CMP_GREATER,
//0x10,
////0x20,
////0x40,
//GR_CULL_NEGATIVE
//}
};

// misc field stuff drawing order info for FIELD2
static limb_draw_info_t field2_limb_draw_info2[] = {
{
&limb_yardmarkers98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_pylons98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
}
};

// goalpost drawing order info for FIELD2
static limb_draw_info_t goalposts2_limb_draw_info[] = {
{
&limb_goalpost198_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_goalpost298_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,	//GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_CC_ALPHA,				//GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,						//GR_BLEND_ONE,
GR_BLEND_ONE_MINUS_SRC_ALPHA,			//GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
}
};

//
//  Stadium2 draw order
//
#if 0
static limb_draw_info_t	stadium2_limb_draw_info[] = 
{
#ifndef SLIM_STADIUM
{
&limb_skydome98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_supports298_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_supports198_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_roof298_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_roof198_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_jumbo298_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_jumbo98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
//{
//&limb_ads298_obj,
//0,
//GR_DEPTHBUFFER_WBUFFER,
//GR_CMP_LESS,
//FXTRUE,
//0,
//GR_COLORCOMBINE_DECAL_TEXTURE,
//GR_ALPHASOURCE_TEXTURE_ALPHA,
//GR_TEXTURECOMBINE_DECAL,
//GR_TEXTURECLAMP_CLAMP,
//GR_BLEND_ONE,
//GR_BLEND_ZERO,
//GR_BLEND_ONE,
//GR_BLEND_ZERO,
//GR_CHROMAKEY_DISABLE,
//0,
//GR_CMP_ALWAYS,
//0,
//GR_CULL_NEGATIVE
//},
{
&limb_uppercrowd98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
},
{
&limb_skybox298_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
},
{
&limb_skybox98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
},
{
&limb_uppertrim98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
},
#endif
{
&limb_middlecrowd98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
},
{
&limb_tunnels98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_DECAL_TEXTURE,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_BLEND_ONE,
GR_BLEND_ZERO,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_ALWAYS,
0,
GR_CULL_NEGATIVE,
-1,-1
},
{
&limb_lowercrowd98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
},
{
&limb_lowertrim98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
},
{
&limb_exits98_obj,
0,
GR_DEPTHBUFFER_WBUFFER,
GR_CMP_LESS,
FXTRUE,
0,
GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA,
GR_ALPHASOURCE_TEXTURE_ALPHA,
GR_TEXTURECOMBINE_DECAL,
GR_TEXTURECLAMP_CLAMP,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_BLEND_SRC_ALPHA,
GR_BLEND_ONE_MINUS_SRC_ALPHA,
GR_CHROMAKEY_DISABLE,
0,
GR_CMP_GREATER,
0x40,
GR_CULL_NEGATIVE,
-1,-1
//GR_CULL_DISABLE
}
};
#else
#include "include\nstadlim.h"
#endif


static int	field_scaled = 0;


void init_limb_draw_info(limb_draw_info_t *di, int num)
{
	int	i;
	int	j;
	int	*t1;
	int	*t2;
	VERTEX	*pvtx;
	limb_draw_info_t	*dii;

	// Are we running in low res and the field has NOT been scaled yet?
#if 0
	if(is_low_res && !field_scaled)
	{
		dii = di;
		for(j = 0; j < num; j++)
		{
			// YES - Get pointer to vertices for the limb
			pvtx = dii->limb->pvtx;

			// Scale each of the vertices down in y
			for(i = 0; i < dii->limb->nvtx; i++)
			{
				pvtx->y *= (vres / 384.0f);
				pvtx++;
			}
			dii++;
		}
	}
#endif

	di->mode_change_flags = LDI_MCF_ALL;
	di++;
	for(i = 1; i < num; i++)
	{
		t1 = &(di-1)->depth_mode;
		t2 = &di->depth_mode;
		di->mode_change_flags = 0;
		for(j = 0; j < (int)(sizeof(limb_draw_info_t)/sizeof(int)) - 4; j++)
		{
			if(*t1 != *t2)
			{
				di->mode_change_flags |= (1<<j);
			}
			++t1;
			++t2;
		}
		++di;
	}
}

extern int	cull_enable;

void set_limb_mode(limb_draw_info_t *di)
{
	// Depth Buffer Mode
	if(di->mode_change_flags & LDI_MCF_DMODE)
	{
		grDepthBufferMode(di->depth_mode);
	}

	// Depth Buffer Function
	if(di->mode_change_flags & LDI_MCF_DFUNC)
	{
		grDepthBufferFunction(di->depth_func);
	}

	// Depth Buffer Mask
	if(di->mode_change_flags & LDI_MCF_DMASK)
	{
		grDepthMask(di->depth_mask);
	}

	// Depth Bias
	if(di->mode_change_flags & LDI_MCF_DBIAS)
	{
		grDepthBiasLevel(di->depth_bias);
	}

	// Color Combiner Function
	if(di->mode_change_flags & LDI_MCF_CFUNC)
	{
		guColorCombineFunction(di->cc_function);
	}

	// Alpha Source
	if(di->mode_change_flags & LDI_MCF_ASRC)
	{
		guAlphaSource(di->alpha_source);
	}

	// Texture Combiner Function
	if(di->mode_change_flags & LDI_MCF_TFUNC)
	{
		grTexCombineFunction(0, di->tc_function);
	}

	// Clamp Mode
	if(di->mode_change_flags & LDI_MCF_CMODE)
	{
		grTexClampMode(0, di->clamp_mode, di->clamp_mode);
	}

	// Source RGB Blend Function
	// Destination RGB Blend Function
	// Source Alpha Blend Function
	// Destination Alpha Blend Function
	if(di->mode_change_flags & (LDI_MCF_RGBS|LDI_MCF_RGBD|LDI_MCF_AS|LDI_MCF_AD))
	{
		grAlphaBlendFunction(di->rgb_src, di->rgb_dst, di->a_src, di->a_dst);
	}

	// Chromakey Mode
	if(di->mode_change_flags & LDI_MCF_CKMODE)
	{
		grChromakeyMode(di->chroma_mode);
	}

	// Chromakey Value
	if(di->mode_change_flags & LDI_MCF_CVAL)
	{
		grChromakeyValue(di->chroma_key_value);
	}

	// Alpha Test Function
	if(di->mode_change_flags & LDI_MCF_AFUNC)
	{
		grAlphaTestFunction(di->alpha_test_function);
	}

	// Alpha test value
	if(di->mode_change_flags & LDI_MCF_AVAL)
	{
		grAlphaTestReferenceValue(di->alpha_test_value);
	}
}

//void do_rain(void);
#if defined(SEATTLE)
static GrFog_t	fog_table[GR_FOG_TABLE_SIZE];
#else
static GrFog_t	*fog_table = NULL;
#endif

/****************************************************************************/
// field_setup - main field & stadium setup/texture load.
void field_setup( void )
{
	int	texture_count;
	int	i;
	int 	home_team = game_info.team[game_info.home_team];
	char	**f_textures;			// field textures
	char	**s_textures;			// stadium textures


	if (user_config())			// Apply user config of field 
	{
		apply_user_config(GC_FIELD);
	}

//pup_field = 6;
//pup_stadium = 3;

	switch (pup_field)
	{
		case 0:			// new grass
			f_textures = field2_textures;
			break;
		case 1:			// old grass (blitz)
			f_textures = field2_textures;
			break;
		case 2:			// turf
			f_textures = astro_turf_field_textures;
			break;
		case 3:			// dirt
			f_textures = dirt_field_textures;
			break;
		case 4:			// snow
			f_textures = snow_field_textures;
			break;
		case 5:			// Asphalt
			f_textures = asphalt_field_textures;
			break;
		case 6:
			f_textures = mud_field_textures;
			break;
		default:		// just in case...default to NEW field
			f_textures = field2_textures;
			break;
	}

	switch (pup_stadium)
	{
		case 0:			// day stadium
			s_textures = day_stadium_textures;
			break;
		case 1:			// night stadium
			s_textures = stadium_textures;
			break;
		case 2:			// snow stadium
			s_textures = snow_stadium_textures;
			break;
		default:		// just in case...default to new stadium
			s_textures = stadium_textures;
			break;
	}

	if(randrng(100) < 5)
		is_foggy = 1;
	else
		is_foggy = 0;

// !!!FIX...for all but snow stadium...dont draw limb_snow_obj

	// Initialize the drawing state info for the field limbs
	init_limb_draw_info(field2_limb_draw_info, sizeof(field2_limb_draw_info)/sizeof(limb_draw_info_t));

	// Initialize the drawing state info for the field limbs2
	init_limb_draw_info(field2_limb_draw_info2, sizeof(field2_limb_draw_info2)/sizeof(limb_draw_info_t));

	// Initialize the drawing state info for the goalposts limbs
	init_limb_draw_info(goalposts2_limb_draw_info, sizeof(goalposts2_limb_draw_info)/sizeof(limb_draw_info_t));

	// Initialize the drawing state info for the stadium
	if (pup_stadium != 3)
		init_limb_draw_info(stadium2_limb_draw_info, sizeof(stadium2_limb_draw_info)/sizeof(limb_draw_info_t));

	// Set the field scaled flag regardless of whether or not it was scaled
	field_scaled = 1;

	// Figure out how many textures there are for the field
	texture_count = 0;
	while(f_textures[texture_count])
	{
		++texture_count;
	}

#if 0
	// Allocate memory for the field decal handles
	if((field_decals = (GrMipMapId_t *)JMALLOC(sizeof(GrMipMapId_t) * texture_count)) == (GrMipMapId_t *)0)
	{
#ifdef DEBUG
		fprintf(stderr,"Can not allocate memory for field decal handle array\n");
		lockup();
#endif
		return;
	}

	// Allocate memory for the field ratios
	if((field_ratios = (int *)JMALLOC(sizeof(int) * texture_count)) == (int *)0)
	{
#ifdef DEBUG
		fprintf(stderr,"Can not allocate memory for field ratios array\n");
		lockup();
#endif
		JFREE(field_decals);
		return;
	}
#endif

	// Figure out which of the field textures are beabg.wms, beaez1.wms,
	// beaz2.wms, and beaz3.wms
	for(i = 0; i < texture_count; i++)
	{
		if (!(strcmp(f_textures[i], "beabg.wms")) ||
			!(strcmp(f_textures[i], "Beabg.wms")))
		{
			fez_tex_map[0] = i;
		}
		if (!(strcmp(f_textures[i], "beaez1.wms")) ||
			!(strcmp(f_textures[i], "Beaez1.wms")))
		{
			fez_tex_map[1] = i;
		}
		if (!(strcmp(f_textures[i], "beaez2.wms")) ||
			!(strcmp(f_textures[i], "Beaez2.wms")))
		{
			fez_tex_map[2] = i;
		}
		if (!(strcmp(f_textures[i], "beaez3.wms")) ||
			!(strcmp(f_textures[i], "Beaez3.wms")))
		{
			fez_tex_map[3] = i;
		}
	}

	// Set the textures to use for the endzones
	f_textures[fez_tex_map[0]][0] = teaminfo[home_team].prefix[0];
	f_textures[fez_tex_map[0]][1] = teaminfo[home_team].prefix[1];
	f_textures[fez_tex_map[0]][2] = teaminfo[home_team].prefix[2];

	f_textures[fez_tex_map[1]][0] = teaminfo[home_team].prefix[0];
	f_textures[fez_tex_map[1]][1] = teaminfo[home_team].prefix[1];
	f_textures[fez_tex_map[1]][2] = teaminfo[home_team].prefix[2];

	f_textures[fez_tex_map[2]][0] = teaminfo[home_team].prefix[0];
	f_textures[fez_tex_map[2]][1] = teaminfo[home_team].prefix[1];
	f_textures[fez_tex_map[2]][2] = teaminfo[home_team].prefix[2];

	f_textures[fez_tex_map[3]][0] = teaminfo[home_team].prefix[0];
	f_textures[fez_tex_map[3]][1] = teaminfo[home_team].prefix[1];
	f_textures[fez_tex_map[3]][2] = teaminfo[home_team].prefix[2];

	// Load the textures for the field
	field_texture_cnt = texture_count;
	load_field_textures();
	chk_game_proc_abort();

	// Figure out how many textures there are for the stadium
	texture_count = 0;
	while(s_textures[texture_count])
	{
		++texture_count;
	}

#if 0
	// Allocate memory for the stadium decal handles
	if((stadium_decals = (GrMipMapId_t *)JMALLOC(sizeof(GrMipMapId_t) * texture_count)) == (GrMipMapId_t *)0)
	{
#ifdef DEBUG
		fprintf(stderr,"Can not allocate memory for stadium decal handle array\n");
		lockup();
#endif
		return;
	}

	// Allocate memory for the stadium ratios
	if((stadium_ratios = (int *)JMALLOC(sizeof(int) * texture_count)) == (int *)0)
	{
#ifdef DEBUG
   	fprintf(stderr,"Can not allocate memory for stadium ratios array\n");
		lockup();
#endif
		JFREE(stadium_decals);
		return;
	}
#endif

	// Load the textures for the stadium
	stadium_texture_cnt = texture_count;
	load_stadium_textures();
	chk_game_proc_abort();
}
		
//-------------------------------------------------------------------------------------------------------
// do_field_procs - launch main field & stadium draw procs
//-------------------------------------------------------------------------------------------------------
void do_field_procs( void )
{
	// Create the drawing object to draw the field and stadium
	create_object("field",  OID_FIELD, OF_NONE, DRAWORDER_FIELD,  NULL, draw_field);
	create_object("field2", OID_FIELD, OF_NONE, DRAWORDER_FIELD2, NULL, draw_field2);
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
void do_user_fog(void)
{
#if defined(VEGAS)
	long	fog_tbl_size;
#endif

//fprintf(stderr,"HERE!!\n");
//fflush(stdout);
//sleep(1);

//	// Apply user configuration choices to powerups
//	if (user_config())
//	{
//		apply_user_config(GC_FOG);
//	}

	// Do we want fog?
	if ((!pup_thickfog) && (!pup_fog))
		return;

	// Have custom thick fog setting overide
	if (pup_thickfog)
		pup_fog = 0;

	is_foggy = 1;

#if defined(VEGAS)
	if(is_foggy && (grGet(GR_FOG_TABLE_ENTRIES, 4, &fog_tbl_size) == 4))
	{
		fog_table = realloc(fog_table, fog_tbl_size);
#else
	if(is_foggy)
	{
#endif
		grFogColorValue(fog_color);
		guFogGenerateLinear(fog_table, 1.0f, fog_thickness);
		grFogTable(fog_table);

		create_object("fogon", OID_FIELD, OF_NONE, DRAWORDER_FOGON, NULL, draw_fogon);
		create_object("fogoff", OID_FIELD, OF_NONE, DRAWORDER_FOGOFF, NULL, draw_fogoff);
	}

}
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
void do_vs_fog(void)
{
#if defined(VEGAS)
	long	fog_tbl_size;
#endif

	// Set fog flag; thick fog has priority
	if (pup_thickfog) is_foggy = 2;
	else
	if (pup_fog) is_foggy = 1;

	if (pup_field == 4)
		is_foggy = 1;


#if defined(VEGAS)
	if(is_foggy && (grGet(GR_FOG_TABLE_ENTRIES, 4, &fog_tbl_size) == 4))
	{
		fog_table = realloc(fog_table, fog_tbl_size);
#else
	if(is_foggy)
	{
#endif

		if (pup_field == 4)
		{
			fog_color = 0x00549EC9;
			grFogColorValue(fog_color);
			guFogGenerateLinear(fog_table, 1.0f, 3700.0f);
			grFogTable(fog_table);
		}
		else
		{
			fog_color = (pup_stadium == 1) ? 0x00808080 : 0x00a0a0a0;	// "darker" fog at night
			grFogColorValue(fog_color);
			guFogGenerateLinear(fog_table, 1.0f, (is_foggy == 1) ? 1250.0f : 500.0f);
			grFogTable(fog_table);
		}

		create_object("fogon", OID_FIELD, OF_NONE, DRAWORDER_FOGON, NULL, draw_fogon);
		create_object("fogoff", OID_FIELD, OF_NONE, DRAWORDER_FOGOFF, NULL, draw_fogoff);
	}
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
void load_field_textures(void)
{
	switch (pup_field)
	{
		case 0:			// new grass
			load_textures(field2_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 1:			// old grass (blitz)
			load_textures(field2_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 2:			// turf
			load_textures(astro_turf_field_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 3:			// dirt
			load_textures(dirt_field_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 4:			// snow
			load_textures(snow_field_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 5:
			load_textures(asphalt_field_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 6:
			load_textures(mud_field_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		default:		// just in case...default to NEW field
			load_textures(field2_textures, field_ratios, field_decals, field_texture_cnt, TXID_FIELD,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
	}
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
void load_stadium_textures(void)
{

	switch (pup_stadium)
	{
		case 0:			// day stadium
			load_textures(day_stadium_textures, stadium_ratios, stadium_decals, stadium_texture_cnt,
					TXID_FIELD,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 1:			// night stadium
			load_textures(stadium_textures, stadium_ratios, stadium_decals, stadium_texture_cnt,
					TXID_FIELD,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 2:			// snow stadium
			load_textures(snow_stadium_textures, stadium_ratios, stadium_decals, stadium_texture_cnt,
					TXID_FIELD,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
		case 3:			// NO stadium
			break;
		default:		// just in case...default to new stadium
			load_textures(stadium_textures, stadium_ratios, stadium_decals, stadium_texture_cnt,
					TXID_FIELD,	GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
			break;
	}
}		


//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
static float	xlate[12] = {
1.0f, 0.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f, 0.0f,
0.0f, 0.0f, 1.0f, 0.0f
};

static float	cm[12];
#ifdef SEATTLE
void render_limb_cc( LIMB *limb, float *xform, GrMipMapId_t *decals, int *ratios, int cull );
#else
void render_limb_cc( LIMB *limb, float *xform, Texture_node_t **decals, int *ratios, int cull );
#endif
extern MidVertex		tmp_grvx[1024];
static MidVertex	*tmp_grv = &tmp_grvx[1];
void xform_limb_fs( LIMB *, float *, MidVertex *, int);
#ifdef SEATTLE
void draw_tris( LIMB *, GrMipMapId_t *, int *, MidVertex *, int );
#else
void draw_tris( LIMB *, Texture_node_t **, int *, MidVertex *, int );
#endif

#ifdef SEATTLE
static void draw_world_object(register limb_draw_info_t *di, register int num_limbs, register GrMipMapId_t *decals, register int *ratios)
#else
static void draw_world_object(register limb_draw_info_t *di, register int num_limbs, register Texture_node_t **decals, register int *ratios)
#endif
{
	int		draw;

	while(num_limbs--)
	{
		draw = FALSE;

		// see if either of this limb's zones is on the draw list
		// if zone1 is -1, the object is always drawn
		if ((di->zone1 == -1) ||
			(os_zones[di->zone1] == TRUE))
			draw = TRUE;

		if ((di->zone2 != -1) &&
			(os_zones[di->zone2] == TRUE))
			draw = TRUE;

		// Set up the mode for this limb
		if(di->mode_change_flags)
			set_limb_mode(di);

		if (draw)
		{
			// Enable/disable backface culling for this limb
			cull_enable = di->cull_mode;

			// Transform the limb to screen coordinates
			xform_limb_fs(di->limb, cm, tmp_grv, is_low_res);

			// Draw the limb
			draw_tris(di->limb, decals, ratios, tmp_grv, 0);

			// Do next limb in draw list
		}
		++di;
	}
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
#ifdef SEATTLE
static void draw_world_goalposts(register limb_draw_info_t *di, register int num_limbs, register GrMipMapId_t *decals, register int *ratios)
#else
static void draw_world_goalposts(register limb_draw_info_t *di, register int num_limbs, register Texture_node_t **decals, register int *ratios)
#endif
{
//	vec3d	cb;
//	vec3d	cg;
//	vec3d	cg1;
//	float	magcb;
//	float	magcg;
//	float	magcg1;
//	float	cos_a;
//	float	cos_a1;
//	float	*from;
//	float	*from1;

	while(num_limbs--)
	{
		// Set up the mode for this limb
		if(di->mode_change_flags)
		{
			set_limb_mode(di);
		}

		// Enable/disable backface culling for this limb
		cull_enable = di->cull_mode;

		// Transform the limb to screen coordinates
		xform_limb_fs(di->limb, cm, tmp_grv, is_low_res);

		// Set appropriate alpha opacity
		grConstantColorValue(
				(((di->limb == &limb_goalpost198_obj && cambot.x < -GOAL_X) ||
				  (di->limb == &limb_goalpost298_obj && cambot.x >  GOAL_X)) &&
				 !(game_info.team_play[game_info.last_pos]->flags & (PLF_FIELD_GOAL|PLF_FAKE_FG))) ?
				((cambot.kickoff_flag && !(game_info.game_flags & GF_QUICK_KICK)) ? 0 : 150 << 24) : (255 << 24)
			);

//		grConstantColorValue(
//				((di->limb == &limb_goalpost_obj  && cambot.x < (7.0f-FIELDHB)) ||
//				 (di->limb == &limb_goalpost2_obj && cambot.x > (FIELDHB-7.0f))) ?
//				((cambot.kickoff_flag && !(game_info.game_flags & GF_QUICK_KICK)) ? 0 : 150 << 24) : (255 << 24)
//			);

		// Draw the limb
		draw_tris(di->limb, decals, ratios, tmp_grv, 0);

		// Do next limb in draw list
		++di;
	}
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
static void draw_field( void *oi )
{
	pftimer_start( PRF_FIELD1 );
	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

	// prepare os_zones
	do_os_zones();

	draw_world_object(field2_limb_draw_info,
		sizeof(field2_limb_draw_info)/sizeof(limb_draw_info_t),
		field_decals,
		field_ratios);

	if (pup_stadium != 3)
	{
		draw_world_object(stadium2_limb_draw_info,
			sizeof(stadium2_limb_draw_info)/sizeof(limb_draw_info_t),
			stadium_decals,
			stadium_ratios);
	}
	pftimer_end( PRF_FIELD1 );
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
static void draw_field2(void *oi)
{
	pftimer_start( PRF_FIELD2 );

	xlate[3] = -cambot.x;
	xlate[7] = -cambot.y;
	xlate[11] = -cambot.z;

	mxm(cambot.xform, xlate, cm);

	draw_world_object(field2_limb_draw_info2,
		sizeof(field2_limb_draw_info2)/sizeof(limb_draw_info_t),
		field_decals,
		field_ratios);

	draw_world_goalposts(goalposts2_limb_draw_info,
		sizeof(goalposts2_limb_draw_info)/sizeof(limb_draw_info_t),
		field_decals,
		field_ratios);

//	draw_world_object(field_limb_draw_info2,
//		sizeof(field_limb_draw_info2)/sizeof(limb_draw_info_t),
//		field_decals,
//		field_ratios);
//
//	draw_world_goalposts(goalposts_limb_draw_info,
//		sizeof(goalposts_limb_draw_info)/sizeof(limb_draw_info_t),
//		field_decals,
//		field_ratios);
	pftimer_end( PRF_FIELD2 );
}


//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
static void draw_fogon( void *oi )
{
#ifdef BANSHEE
	grFogMode(GR_FOG_WITH_TABLE_ON_FOGCOORD_EXT);
#else
	grFogMode(GR_FOG_WITH_TABLE);
#endif
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
static void draw_fogoff( void *oi )
{
	grFogMode(GR_FOG_DISABLE);
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
#if 0
#if 1
		if(di->limb == &limb_goalpost_obj)
		{
			if(cambot.x < -459.0f && fabs(cambot.z) < 100)
			{
				do_it = 0;
			}
		}
		else if(di->limb == &limb_goalpost2_obj)
		{
			if(cambot.x > 459.0f && fabs(cambot.z) < 100)
			{
				do_it = 0;
			}
		}
#else
		if(di->limb == &limb_goalpost_obj || di->limb == &limb_goalpost2_obj)
		{
			// Is Camera over the field ?
			if(fabs(cambot.x) > 459.0f)
			{
				// NOPE - Calculate angle formed by the vectors from the camera
				// to the ball and the camera to the goalpost
				from = &di->limb->pvtx[19].x;
				from1 = &di->limb->pvtx[34].x;

				// Calculate the CB vector
				cb.x = cambot.x - ball_obj.odata.x;
				cb.y = 0.0f;
				cb.z = cambot.z - ball_obj.odata.z;
				cb.w = 1.0f;

				// Calculate the CG vector
				cg.x = cambot.x - *from++;
				from++;
				cg.y = 0.0f;
				cg.z = cambot.z - *from;
				cg.w = 1.0f;

				// Calculate the CG1 vector
				cg1.x = cambot.x - *from1++;
				from1++;
				cg1.y = 0.0f;
				cg1.z = cambot.z - *from1;
				cg1.w = 1.0f;

				// Get magnitude of the CB Vector
				magcb = magv(&cb);

				// Get magnitude of the CG Vector
				magcg = magv(&cg);

				// Get magnitude of the CG Vector
				magcg1 = magv(&cg1);

				// Normalize the 2 vectors
				norm(&cb);
				norm(&cg);
				norm(&cg1);

				// Take dot product of the two vectors to get the cosine of the angle
				cos_a = cb.x * cg.x + cb.z * cg.z;
				cos_a1 = cb.x * cg1.x + cb.z * cg1.z;

				// Is the cosine greater than .707 ( < 45 degrees) ?
				if(cos_a >= 0.707106781f)
				{
					// YES - Is the goalpost closer to the camera than the ball
					if(magcg < magcb)
					{
						do_it = 0;
					}
				}
				if(cos_a1 >= 0.707106781f)
				{
					// YES - Is the goalpost closer to the camera than the ball
					if(magcg1 < magcb)
					{
						do_it = 0;
					}
				}
			}
		}
#endif
#endif
/****************************************************************************/
static void do_os_zones( void )
{
	LIMB *box_limbs[] = {
		&limb_box1a,&limb_box2a,&limb_box3a,
		&limb_box1b,&limb_box2b,&limb_box3b,
		&limb_box1c,&limb_box2c,&limb_box3c,
		&limb_box1d,&limb_box2d,&limb_box3d,
		&limb_box1e,&limb_box2e,&limb_box3e,
		&limb_box1f,&limb_box2f,&limb_box3f };

	int	i,j, lefts, aboves, rights, belows, behinds;
	static int do_calc = TRUE;

	float	mx,my;

//	if (get_dip_coin_current() & 0x100)
//		fprintf( stderr, "\n-----\n" );

//	if (get_dip_coin_close() & 0x100)
//	{
//		do_calc = !do_calc;
//		fprintf( stderr, "zone calc %s\n", do_calc ? "ON" : "OFF" );
//	}

//	if (get_dip_coin_current() & 0x80)
//		fprintf( stderr, "\n-----\n" );

	for( i = 0; i < 18; i++ )
	{
#if 1
		lefts = 0;	rights = 0; aboves = 0; belows = 0; behinds = 0;
		// transform the vertices
		xform_limb_c( box_limbs[i], cm, tmp_grv );

		// dump the transformed data
//		if (get_dip_coin_current() & 0x100)
		if (0)
		{
			fprintf( stderr, "Box%d%c\n", i % 3 + 1, 'a' + i / 3 );
			for (j = 0; j < 8; j++)
			{
				fprintf( stderr, " %d: %f %f %f - %f %f %f\n", j,
					(box_limbs[i])->pvtx[j].x,
					(box_limbs[i])->pvtx[j].y,
					(box_limbs[i])->pvtx[j].z,
					tmp_grv[j].x,
					tmp_grv[j].y,
					tmp_grv[j].oow );
			}
		}

		// count lefts, aboves, rights, downs, and behinds
		for( j = 0; j < 8; j++ )
		{
			mx = tmp_grv[j].oow * 1.2f * 0.5f;
			my = mx * 0.75f;

			if (-tmp_grv[j].x > mx)
				rights += 1;
			if (-tmp_grv[j].x < -mx)
				lefts += 1;
			if (tmp_grv[j].y > my)
				aboves += 1;
			if (tmp_grv[j].y < -my)
				belows += 1;
			if (tmp_grv[j].oow < 10.0f)
				behinds += 1;

		}

		os_zones[i] = TRUE;

		if ((rights == 8) ||
			(lefts == 8) ||
			(aboves == 8) ||
			(belows == 8) ||
			(behinds == 8))
			os_zones[i] = FALSE;

//		if (!do_calc)
//			os_zones[i] = TRUE;

//		if (get_dip_coin_current() & 0x80)
//		{
//			fprintf( stderr, "Box%d%c: %s\t%d %d %d %d %d\n",
//				i % 3 + 1,
//				'a' + i / 3,
//				os_zones[i] ? "ON" : "OFF",
//				rights, lefts, aboves, belows, behinds );
//		}
#else
		os_zones[i] = TRUE;
#endif
	}
}

/****************************************************************************/
