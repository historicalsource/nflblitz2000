/****************************************************************************/
/*                                                                          */
/* ani3Di.h - prototypes & definitions local to r50ani3D.lib				*/
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* $Header: /video/nfl/lib/ani3d/ani3Di.h 2     11/05/97 1:18p Mlynch $       */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Right Reserved                                                       */
/*                                                                          */
/****************************************************************************/

void xform_limb_fs( LIMB *, float *, MidVertex *, int res);
#ifdef SEATTLE
void draw_tris_noar( LIMB *, GrMipMapId_t *, MidVertex *, int );
void draw_tris_noz( LIMB *, GrMipMapId_t *, int *, MidVertex *, int );
void draw_tris( LIMB *, GrMipMapId_t *, int *, MidVertex *, int );
void draw_tris_noclip( LIMB *, GrMipMapId_t *, int *, MidVertex *, int );
#else
void draw_tris_noar( LIMB *, Texture_node_t **, MidVertex *, int );
void draw_tris_noz( LIMB *, Texture_node_t **, int *, MidVertex *, int );
void draw_tris( LIMB *, Texture_node_t **, int *, MidVertex *, int );
void draw_tris_noclip( LIMB *, Texture_node_t **, int *, MidVertex *, int );
#endif
