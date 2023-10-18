/****************************************************************************/
/*                                                                          */
/* pipe3d.c - 3D object pipeline & related functions                        */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* $Header: /video/nfl/lib/ani3d/pipe3D.c 20    10/07/99 6:24p Janderso $       */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Right Reserved                                                       */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<math.h>
#include	<goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "/video/nfl/include/ani3D.h"
#include "/video/nfl/include/game.h"
#include "/video/nfl/include/audits.h"
#include "trigs.h"
#include "ani3Di.h"

extern	float	hres, vres;
extern	int	is_low_res;

#define PI_RAD			3.1415926f
#define PRINT_LOADS		FALSE

//
// local data
//
MidVertex		tmp_grvx[1024];
static MidVertex		*tmp_grv = tmp_grvx + 1;
static void draw_jersey_tris( LIMB *, MidVertex *, int, int );
static void xform_plate( LIMB *, float *, float *, MidVertex * );
#if 0
static void xform_limb_c( LIMB *, float *, MidVertex * );
static void xform_limb_sls( LIMB *, float *, MidVertex * );
static void draw_tris_fbc( LIMB *, GrMipMapId_t *, MidVertex * );
static void draw_triangles( LIMB *, GrMipMapId_t *, MidVertex *, int * );
#endif

//
// external data & functions
//
extern float bighead_scale;			// Big head scale for current player draw (0 = no scaling)

//////////////////////////////////////////////////////////////////////////////
// load_textures() - Load all the textures from tex_list[].  Store their
// aspect ratios in ratios[] and all their handles in decals[].
//
#ifdef SEATTLE
void load_textures( char *tex_list[], int *ratios, GrMipMapId_t *decals,
		int max_decals, int id, int minfilt, int maxfilt )
#else
void load_textures( char *tex_list[], int *ratios, Texture_node_t **decals,
		int max_decals, int id, int minfilt, int maxfilt )
#endif

{
	int		i;
	struct texture_node *pnode;

	for( i = 0; tex_list[i] != NULL; i++ )
	{
		if( i >= max_decals )
		{
#ifdef DEBUG
			lockup();
#else
			while(1) ;
#endif
		}

#if PRINT_LOADS
		fprintf(stderr,  "libAni3D: Loading texture %s...", tex_list[i] );
#endif

  		pnode = create_texture( tex_list[i], id, 0, CREATE_NORMAL_TEXTURE,
  				GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP, minfilt, maxfilt );

		if( pnode != NULL )
		{
			ratios[i] = pnode->texture_info.header.aspect_ratio;
			decals[i] = pnode->texture_handle;
#if PRINT_LOADS
			fprintf(stderr,  "done, handle: %d\r\n", (int)pnode->texture_handle );
#endif
		}
		else
		{
			increment_audit(TEX_LOAD_FAIL_AUD);
#ifdef SEATTLE
			decals[i] = 0;
#else
			decals[i] = (Texture_node_t *) NULL;
#endif
#if PRINT_LOADS
			fprintf(stderr,  "FAILED\r\n" );
			lockup();
#endif
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// render_node() - Draw limb and recursively draw any limbs beneath it
// in the object hierarchy.
//////////////////////////////////////////////////////////////////////////////
#define NORM	FALSE
#ifdef SEATTLE
void render_node( SNODE *limb, LIMB **model, float *parent_matrix, float *fp,
		GrMipMapId_t *decal_array, float *jpos )
#else
void render_node( SNODE *limb, LIMB **model, float *parent_matrix, float *fp,
		Texture_node_t **decal_array, float *jpos )
#endif
{
	float	lxform[12], matrix[12];
	float	tx,ty,tz;
#if NORM
	float	n1,n2,n3;
#endif

	/* xlate +joint pos */
	tx = limb->x;
	ty = limb->y;
	tz = limb->z;

	lxform[3] = tx;
	lxform[7] = ty;
	lxform[11] = tz;

	jpos[0] = parent_matrix[0] * tx + parent_matrix[1] * ty + parent_matrix[2] * tz + parent_matrix[3];
	jpos[1] = parent_matrix[4] * tx + parent_matrix[5] * ty + parent_matrix[6] * tz + parent_matrix[7];
	jpos[2] = parent_matrix[8] * tx + parent_matrix[9] * ty + parent_matrix[10] * tz + parent_matrix[11];

	/* rotate about this joint */
	quat2mat( fp, lxform );
	mxm( parent_matrix, lxform, matrix );

	/* xlate -joint pos */
	matrix[3] -= matrix[0] * tx + matrix[1] * ty + matrix[2] * tz;
	matrix[7] -= matrix[4] * tx + matrix[5] * ty + matrix[6] * tz;
	matrix[11] -= matrix[8] * tx + matrix[9] * ty + matrix[10] * tz;

#if NORM
	// verify that the matrix is normalized!
	n1 = fsqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	n2 = fsqrt(matrix[4] * matrix[4] + matrix[5] * matrix[5] + matrix[6] * matrix[6]);
	n3 = fsqrt(matrix[8] * matrix[8] + matrix[9] * matrix[9] + matrix[10] * matrix[10]);

	if(( n1 < 0.999f ) || ( n1 > 1.001 ) ||
	   ( n2 < 0.999f ) || ( n2 > 1.001 ) ||
	   ( n3 < 0.999f ) || ( n3 > 1.001 ))
	{
		fprintf(stderr,  "non-normalized quat %f %f %f %f\r\n", fp[0], fp[1], fp[2], fp[3] );
	}
#endif

	/* render any objects at this joint */
	if( !(limb->flags & SN_NODRAW ))
	{
		xform_limb_fs( *model, matrix, tmp_grv, is_low_res);

		if( limb->flags && SN_DBLSIDE )
			draw_tris_noar( *model, decal_array, tmp_grv, 0x04 );
		else
			draw_tris_noar( *model, decal_array, tmp_grv, 0x0C );
	}

	/* draw any children */
	if( limb->children[0] )
	{
		render_node( limb + limb->children[0],
					 model + limb->children[0],
					 matrix,
					 fp + limb->children[0] * 4,
					 decal_array,
					 jpos + limb->children[0] * 3 );
		if( limb->children[1] )
		{
			render_node( limb + limb->children[1],
						 model + limb->children[1],
						 matrix,
						 fp + limb->children[1] * 4,
						 decal_array,
						 jpos + limb->children[1] * 3 );
			if( limb->children[2] )
			{
				render_node( limb + limb->children[2],
							 model + limb->children[2],
							 matrix,
							 fp + limb->children[2] * 4,
							 decal_array,
							 jpos + limb->children[2] * 3 );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// generate_matrices() - Generate matrix for node & recurse through any sub-
// nodes.
//////////////////////////////////////////////////////////////////////////////
void generate_matrices( SNODE *limb, float *parent_matrix, float *fp, float *matrix, float *jpos )
{
	float	lxform[12];
	float	tx,ty,tz;

	/* xlate +joint pos */
	tx = limb->x;
	ty = limb->y;
	tz = limb->z;

	lxform[3] = tx;
	lxform[7] = ty;
	lxform[11] = tz;

	jpos[0] = parent_matrix[0] * tx + parent_matrix[1] * ty + parent_matrix[2] * tz + parent_matrix[3];
	jpos[1] = parent_matrix[4] * tx + parent_matrix[5] * ty + parent_matrix[6] * tz + parent_matrix[7];
	jpos[2] = parent_matrix[8] * tx + parent_matrix[9] * ty + parent_matrix[10] * tz + parent_matrix[11];

	/* rotate about this joint */
	quat2mat( fp, lxform );
	mxm( parent_matrix, lxform, matrix );

	if ((limb->flags & SN_HEADLIMB) && (bighead_scale))
	{
		matrix[0] *= bighead_scale; matrix[1] *= bighead_scale; matrix[2] *= bighead_scale;
		matrix[4] *= bighead_scale; matrix[5] *= bighead_scale; matrix[6] *= bighead_scale;
		matrix[8] *= bighead_scale; matrix[9] *= bighead_scale; matrix[10]*= bighead_scale;
	}

	/* xlate -joint pos */
	matrix[3] -= matrix[0] * tx + matrix[1] * ty + matrix[2] * tz;
	matrix[7] -= matrix[4] * tx + matrix[5] * ty + matrix[6] * tz;
	matrix[11] -= matrix[8] * tx + matrix[9] * ty + matrix[10] * tz;

	/* draw any children */
	if( limb->children[0] )
	{
		generate_matrices( limb + limb->children[0],
					 matrix,
					 fp + limb->children[0] * 4,
					 matrix + limb->children[0] * 12,
					 jpos + limb->children[0] * 3 );
		if( limb->children[1] )
		{
			generate_matrices( limb + limb->children[1],
						 matrix,
						 fp + limb->children[1] * 4,
						 matrix + limb->children[1] * 12,
						 jpos + limb->children[1] * 3 );
			if( limb->children[2] )
			{
				generate_matrices( limb + limb->children[2],
							 matrix,
							 fp + limb->children[2] * 4,
							 matrix + limb->children[2] * 12,
							 jpos + limb->children[2] * 3 );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// render_node_mpc() - Draw limb and recursively draw any limbs beneath it
// in the object hierarchy. (Matrices precomputed.)
//////////////////////////////////////////////////////////////////////////////
#define NORM	FALSE
#ifdef SEATTLE
void render_node_mpc( SNODE *limb, LIMB **model, float *matrix,
		GrMipMapId_t *decal_array, float *jpos )
#else
void render_node_mpc( SNODE *limb, LIMB **model, float *matrix,
		Texture_node_t **decal_array, float *jpos )
#endif
{
	/* render any objects at this joint */
	if( !(limb->flags & SN_NODRAW ))
	{
		xform_limb_fs( *model, matrix, tmp_grv, is_low_res);

		if( limb->flags && SN_DBLSIDE )
			draw_tris_noar( *model, decal_array, tmp_grv, 0x04 );
		else
			draw_tris_noar( *model, decal_array, tmp_grv, 0x0C );
	}

	/* draw any children */
	if( limb->children[0] )
	{
		render_node_mpc( limb + limb->children[0],
					 model + limb->children[0],
					 matrix + limb->children[0] * 12,
					 decal_array,
					 jpos + limb->children[0] * 3 );
		if( limb->children[1] )
		{
			render_node_mpc( limb + limb->children[1],
						 model + limb->children[1],
						 matrix + limb->children[1] * 12,
						 decal_array,
						 jpos + limb->children[1] * 3 );
			if( limb->children[2] )
			{
				render_node_mpc( limb + limb->children[2],
							 model + limb->children[2],
							 matrix + limb->children[2] * 12,
							 decal_array,
							 jpos + limb->children[2] * 3 );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// render_limb() - Draw limb.
//////////////////////////////////////////////////////////////////////////////
#ifdef SEATTLE
void render_limb( LIMB *limb, float *xform, GrMipMapId_t *decals,
				  int *ratios )
#else
void render_limb( LIMB *limb, float *xform, Texture_node_t **decals,
				  int *ratios )
#endif
{
	/* render any objects at this joint */
	xform_limb_fs( limb, xform, tmp_grv, is_low_res );
	draw_tris( limb, decals, ratios, tmp_grv, 0x0C );
}

//////////////////////////////////////////////////////////////////////////////
// render_limb_jers() - Draw limb. (special for jerseys)
//////////////////////////////////////////////////////////////////////////////
void render_limb_jers( LIMB *limb, float *xform, int d1, int d2 )
{
	/* render any objects at this joint */
	xform_limb_fs( limb, xform, tmp_grv, is_low_res );
	draw_jersey_tris( limb, tmp_grv, d1, d2 );
}

//////////////////////////////////////////////////////////////////////////////
// render_plate() - Draw limb. (special for jerseys)
//////////////////////////////////////////////////////////////////////////////
#ifdef SEATTLE
void render_plate( LIMB *limb, float *pos1, float *pos2, GrMipMapId_t *decals, int *ratios )
#else
void render_plate( LIMB *limb, float *pos1, float *pos2, Texture_node_t **decals, int *ratios )
#endif
{
	/* render any objects at this joint */
	xform_plate( limb, pos1, pos2, tmp_grv );
	draw_tris_noz( limb, decals, ratios, tmp_grv, 0x0C );
}

//////////////////////////////////////////////////////////////////////////////
// render_limb_noclip() - Draw limb. (no clipping)
//////////////////////////////////////////////////////////////////////////////
#ifdef SEATTLE
void render_limb_noclip( LIMB *limb, float *xform, GrMipMapId_t *decals,
				  int *ratios )
#else
void render_limb_noclip( LIMB *limb, float *xform, Texture_node_t **decals,
				  int *ratios )
#endif
{
	/* render any objects at this joint */
	xform_limb_fs( limb, xform, tmp_grv, is_low_res );
	draw_tris_noclip( limb, decals, ratios, tmp_grv, 0x0C );
}

//////////////////////////////////////////////////////////////////////////////
// transform vertices for a limb
//////////////////////////////////////////////////////////////////////////////
static void xform_plate( LIMB *limb, float *pos1, float *pos2, MidVertex *dest )
{
	int i;
	float tx, ty;

	for( i = 0; i < limb->nvtx; i++ )
	{
		/* transform (ha!) vertex into screen coordinates */
		tx = limb->pvtx[i].x + pos1[0] + pos2[0];
		ty = limb->pvtx[i].y + pos1[1] + pos2[1];

		tx *= (hres / SPRITE_HRES);
		ty *= (vres / SPRITE_VRES);

		/* clamp to mult of 0.25 */
		tx += (float)(1<<21);
		ty += (float)(1<<21);

		tx -= (float)(1<<21);
		ty -= (float)(1<<21);

		/* store results */
		tmp_grv[i].x = tx;
		tmp_grv[i].y = ty;
		tmp_grv[i].oow = 0.5f;
	}
}

//////////////////////////////////////////////////////////////////////////////
// transform and gouraud-shade a limb, single light source
//////////////////////////////////////////////////////////////////////////////
#if 0
static void xform_limb_sls( LIMB *limb, float *mat, MidVertex *dest )
{
	int i;
	float px, py, pz, oow;
	float tx, ty, tz;
	vec3d l;
	float ldotn;

	for( i = 0; i < limb->nvtx; i++ )
	{
		/* transform vertex into camera coordinates */
		px = limb->pvtx[i].x;
		py = limb->pvtx[i].y;
		pz = limb->pvtx[i].z;

		tx = px * mat[0] + py * mat[1] + pz * mat[2] + mat[3];
		ty = px * mat[4] + py * mat[5] + pz * mat[6] + mat[7];
		tz = px * mat[8] + py * mat[9] + pz * mat[10] + mat[11];

		if( tz == 0.0f )
			tz = 0.0001;

		/* project */
		oow = 1.0f / tz;
		tx = - (tx * 2 * oow);
		ty = ty * 2 * oow;

		/* scale */
		tx *= (float)(hres/2);
		ty *= (float)(hres/2);
		tx += (float)(hres/2);
		ty += (float)(vres/2);

		/* clamp to mult of 0.25 */
		tx += (float)(1<<21);
		ty += (float)(1<<21);

		tx -= (float)(1<<21);
		ty -= (float)(1<<21);

		/* compute L vector */
		l.x = 0.0f - px;
		l.y = 10.0f - py;
		l.z = 0.0f - pz;
		l.w = 1.0f;
		norm( &l );

		/* compute L dot N */
		ldotn = l.x * limb->pvn[i].x;
		ldotn += l.y * limb->pvn[i].y;
		ldotn += l.z * limb->pvn[i].z;

		if( ldotn < 0.0f )
			ldotn = 0.0f;

		tmp_grv[i].a_or_r = 0.3f + ldotn * 0.7f * 255.0f;
		tmp_grv[i].a_or_r += (float)(1<<21);
		tmp_grv[i].a_or_r -= (float)(1<<21);

		/* store results */
		tmp_grv[i].x = tx;
		tmp_grv[i].y = ty;
		tmp_grv[i].oow = oow; 
	}
}
#endif

#if 0
//////////////////////////////////////////////////////////////////////////////
static void draw_triangles( LIMB *limb, GrMipMapId_t *decals, MidVertex *vsrc,
		int *ratios )
{
	register int	count;
	register TRI	*ptri;
	float			oow;
	register ST		*pst;
	float			sf,tf;
	MidVertex		*v1, *v2, *v3;

	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;
	while(count--)
	{
		if( ptri->texture >= 0 )
		{
			guTexSource( decals[ptri->texture/4] );
			grTexLodBiasValue( GR_TMU0, -1.0f );

			sf = tf = 256.0f;

			switch( ratios[ptri->texture/4] )
			{
				case GR_ASPECT_8x1:	/* 8W x 1H */
					tf = 32.0f;
					break;
				case GR_ASPECT_4x1:	/* 4W x 1H */
					tf = 64.0f;
					break;
				case GR_ASPECT_2x1:	/* 2W x 1H */
					tf = 128.0f;
					break;
				case GR_ASPECT_1x1:	/* 1W x 1H */
					break;
				case GR_ASPECT_1x2:	/* 1W x 2H */
					sf = 128.0f;
					break;
				case GR_ASPECT_1x4:	/* 1W x 4H */
					sf = 64.0f;
					break;
				case GR_ASPECT_1x8:	/* 1W x 8H */
					sf = 32.0f;
					break;
				default:
#ifdef DEBUG
					lockup();
#else
					while(1) ;
#endif
					break;					
			}
		}

		v1 = vsrc + (ptri->v1/3);
		v2 = vsrc + (ptri->v2/3);
		v3 = vsrc + (ptri->v3/3);

		if( ptri->t1 != 255 )
		{
			oow = v1->oow;
			v1->sow = oow * sf * pst[ptri->t1].s;
			v1->tow = oow * tf * pst[ptri->t1].t;
		}
		if( ptri->t2 != 255 )
		{
			oow = v2->oow;
			v2->sow = oow * sf * pst[ptri->t2].s;
			v2->tow = oow * tf * pst[ptri->t2].t;
		}
		if( ptri->t3 != 255 )
		{
			oow = v3->oow;
			v3->sow = oow * sf * pst[ptri->t3].s;
			v3->tow = oow * tf * pst[ptri->t3].t;
		}

		guDrawTriangleWithClip( v3, v2, v1 );

		ptri++;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
static void draw_jersey_tris( LIMB *limb, MidVertex *vsrc, int d1, int d2 )
{
	register int	count;
	register TRI	*ptri;
	float			oow;
	register ST		*pst;
	float			sa;
	MidVertex		*v1, *v2, *v3;

	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;
	while(count--)
	{
		v1 = vsrc + (ptri->v1/3);
		v2 = vsrc + (ptri->v2/3);
		v3 = vsrc + (ptri->v3/3);

		if( ptri->texture )
			sa = 0.098f * d2;
		else
			sa = 0.098f * d1;

		if( ptri->t1 != 255 )
		{
			oow = v1->oow;
			v1->sow = oow * 128.0f * (sa + pst[ptri->t1].s);
			v1->tow = oow * 256.5f * pst[ptri->t1].t;
//			v1->sow = oow * 255.0f * (sa + pst[ptri->t1].s);
//			v1->tow = oow * 31.0f * pst[ptri->t1].t;
		}
		if( ptri->t2 != 255 )
		{
			oow = v2->oow;
			v2->sow = oow * 128.0f * (sa + pst[ptri->t2].s);
			v2->tow = oow * 256.5f * pst[ptri->t2].t;
//			v2->sow = oow * 255.0f * (sa + pst[ptri->t2].s);
//			v2->tow = oow * 31.0f * pst[ptri->t2].t;
		}
		if( ptri->t3 != 255 )
		{
			oow = v3->oow;
			v3->sow = oow * 128.0f * (sa + pst[ptri->t3].s);
			v3->tow = oow * 256.5f * pst[ptri->t3].t;
//			v3->sow = oow * 255.0f * (sa + pst[ptri->t3].s);
//			v3->tow = oow * 31.0f * pst[ptri->t3].t;
		}

		grDrawTriangleDma( v3, v2, v1, 0x04 );

		ptri++;
	}
}

#if 0
//////////////////////////////////////////////////////////////////////////////
static void draw_tris_fbc( LIMB *limb, GrMipMapId_t *decals, MidVertex *vsrc )
{
	register int	count;
	register TRI	*ptri;
	float			oow;
	register ST		*pst;
	float			sf,tf;
	MidVertex		*v1, *v2, *v3;
	MidVertex		*pvtx;

	count = limb->ntris;
	ptri = limb->ptri;
	pst = limb->pst;
	while(count--)
	{
		if( ptri->texture >= 0 )
		{
			guTexSource( decals[ptri->texture] );
			grTexLodBiasValue( GR_TMU0, 0.0f );
			grTexClampMode(0,GR_TEXTURECLAMP_CLAMP,GR_TEXTURECLAMP_CLAMP);
		}

		v1 = ptri->v1;
		v2 = ptri->v2;
		v3 = ptri->v3;

		if( ptri->t1 != -1 )
		{
			oow = v1->oow;
			v1->sow = oow * ptri->t1->s;
			v1->tow = oow * ptri->t1->t;
		}
		if( ptri->t2 != -1 )
		{
			oow = v2->oow;
			v2->sow = oow * ptri->t2->s;
			v2->tow = oow * ptri->t2->t;
		}
		if( ptri->t3 != -1 )
		{
			oow = v3->oow;
			v3->sow = oow * ptri->t3->s;
			v3->tow = oow * ptri->t3->t;
		}

//		guDrawTriangleWithSimpleClip( v3, v2, v1 );
		grDrawTriangleDma( v3, v2, v1, 4 );

		ptri++;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
// mxm() - multiply 4x3 matrix m1 by 4x3 matrix m2, store result in 4x3
// matrix m3.
//////////////////////////////////////////////////////////////////////////////
void mxm( float *m1, float *m2, float *m3 )
{
	int i;

	for( i = 8; i >=0; i-=4 )
	{
		m3[i] = m1[i] * m2[0] + m1[i+1] * m2[4] + m1[i+2] * m2[8];
		m3[i+1] = m1[i] * m2[1] + m1[i+1] * m2[5] + m1[i+2] * m2[9];
		m3[i+2] = m1[i] * m2[2] + m1[i+1] * m2[6] + m1[i+2] * m2[10];
		m3[i+3] = m1[i] * m2[3] + m1[i+1] * m2[7] + m1[i+2] * m2[11] + m1[i+3];
	}
}


//////////////////////////////////////////////////////////////////////////////
// mat2quat() - convert top left 3x3 of 4x4 matrix to quaternion.
//////////////////////////////////////////////////////////////////////////////
#define X	0
#define Y	1
#define Z	2
#define W	3

#if 0
void mat2quat( matrix mat, quat q )
{
	float	tr,s;
	int		i,j,k;
	int		nxt[3] = {Y,Z,X};
	float	mag;

	tr = mat[0][0] + mat[1][1] + mat[2][2];
	if( tr > 0.0f )
	{
		s = fsqrt( tr + 1.0f );
		q[W] = s * 0.5f;
		s = 0.5f / s;
		q[X] = -(mat[1][2] - mat[2][1]) * s;
		q[Y] = -(mat[2][0] - mat[0][2]) * s;
		q[Z] = -(mat[0][1] - mat[1][0]) * s;
	}
	else
	{
		i = X;
		if( mat[Y][Y] > mat[X][X] )
			i = Y;
		if( mat[Z][Z] > mat[i][i] )
			i = Z;

		j = nxt[i];
		k = nxt[j];

		s = fsqrt((mat[i][i] - (mat[j][j]+mat[k][k])) + 1.0f);
		q[i] = s*0.5f;
		s = 0.5f / s;
		q[W] =-(mat[j][k] - mat[k][j]) * s;
		q[j] =(mat[i][j] + mat[j][i]) * s;
		q[k] =(mat[i][k] + mat[k][i]) * s;
	}

	// normalize
	mag = fsqrt( q[X]*q[X] + q[Y]*q[Y] + q[Z]*q[Z] + q[W]*q[W] );

	q[X] /= mag;
	q[Y] /= mag;
	q[Z] /= mag;
	q[W] /= mag;
}
#endif

//////////////////////////////////////////////////////////////////////////////
// quat2mat() - convert quaternion into top left 3x3 corner of a 4x4 matrix.
//////////////////////////////////////////////////////////////////////////////
void quat2mat( float *q, float *mat )
{
	float	s,xs,ys,zs,wx,wy,wz,xx,xy,xz,yy,yz,zz;

	s = 2.0f;

	xs = q[0]*s;	ys = q[1]*s;	zs = q[2]*s;
	wx = q[3]*xs;	wy = q[3]*ys;	wz = q[3]*zs;
	xx = q[0]*xs;	xy = q[0]*ys;	xz = q[0]*zs;
	yy = q[1]*ys;	yz = q[1]*zs;	zz = q[2]*zs;

	mat[0] = 1.0f-(yy+zz);	mat[1] = xy-wz;			mat[2] = xz+wy;
	mat[4] = xy+wz;			mat[5] = 1.0f-(xx+zz);	mat[6] = yz-wx;
	mat[8] = xz-wy;			mat[9] = yz+wx;			mat[10] = 1.0f-(xx+yy);
}


//////////////////////////////////////////////////////////////////////////////
// qxq() - multiply two quaternions together
//////////////////////////////////////////////////////////////////////////////
void qxq( float *q1, float *q2, float *q )
{
	q[W] = q1[W]*q2[W]-q1[X]*q2[X]-q1[Y]*q2[Y]-q1[Z]*q2[Z];

	q[X] = q1[Y]*q2[Z]-q1[Z]*q2[Y]+q1[W]*q2[X]+q2[W]*q1[X];
	q[Y] = q1[Z]*q2[X]-q1[X]*q2[Z]+q1[W]*q2[Y]+q2[W]*q1[Y];
	q[Z] = q1[X]*q2[Y]-q1[Y]*q2[X]+q1[W]*q2[Z]+q2[W]*q1[Z];
}

//////////////////////////////////////////////////////////////////////////////
// slerp() - spherical linear interpolation between two quats
//////////////////////////////////////////////////////////////////////////////
#define EPSILON		0.00001f
#define HALFPI		(PI_RAD*0.5f)
void slerp( float *p, float *q, float t, float *qt )
{
	int		i;
	float	omega, sinom, cosom, sclp, sclq;

//fprintf(stderr,  "slerp: %f %9f %9f %9f %9f\ %9f %9f %9f %9f\r\n",
//			t, p[0], p[1], p[2], p[3], q[0], q[1], q[2], q[3] );

	if( t == 0.0f )
	{
		qt[0] = p[0];	qt[1] = p[1];	qt[2] = p[2];	qt[3] = p[3];
		return;
	}
	if( t == 1.0f )
	{
		qt[0] = q[0];	qt[1] = q[1];	qt[2] = q[2];	qt[3] = q[3];
		return;
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if((1.0f+cosom) > EPSILON )
	{
		if((1.0f-cosom) > EPSILON )
		{
			omega = acos_rad( cosom );
			sinom = sin_rad( omega );
			sclp = sin_rad((1.0f-t)*omega)/sinom;
			sclq = sin_rad(t*omega)/sinom;
		}
		else
		{
			sclp = 1.0f - t;
			sclq = t;
		}

		for( i = 0; i < 4; i++ )
			qt[i] = sclp*p[i] + sclq*q[i];
	}
	else
	{
		qt[0] = -p[1];	qt[1] = p[0];
		qt[2] = -p[3];	qt[3] = p[2];
		sclp = sin_rad((1.0f-t)*HALFPI);
		sclq = sin_rad(t*HALFPI);

		for( i = 0; i < 4; i++ )
			qt[i] = sclp*p[i] + sclq*qt[i];
	}
}

///////////////////////////////////////////////////////////////////////////////
// if -p is a better match to q, set p to -p.
void adjust_quat( float *p, float *q )
{
	float	pdq,npdq;

	pdq = (p[0]-q[0])*(p[0]-q[0]) +
			(p[1]-q[1])*(p[1]-q[1]) +
			(p[2]-q[2])*(p[2]-q[2]) +
			(p[3]-q[3])*(p[3]-q[3]);

	npdq = (p[0]+q[0])*(p[0]+q[0]) +
			(p[1]+q[1])*(p[1]+q[1]) +
			(p[2]+q[2])*(p[2]+q[2]) +
			(p[3]+q[3])*(p[3]+q[3]);

	if( npdq < pdq )
	{
		p[0] *= -1.0f;
		p[1] *= -1.0f;
		p[2] *= -1.0f;
		p[3] *= -1.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////
// t is an int from 0 to 1023.  PI is 512
//////////////////////////////////////////////////////////////////////////////
float isin( int t )
{
	t = t & 0x03ff;

	if( t <= 256 )
		return( trigs[t] );
	if( t <= 512 )
		return( trigs[512-t] );
	if( t <= 768 )
		return( -1.0f * trigs[t-512] );
	return( -1.0f * trigs[1024-t] );
}

//////////////////////////////////////////////////////////////////////////////
// sin (radians) using int table and linear interpolation
#if 0
float sinli( float f )
{
	int		i1;
	float	f1,f2,r1,r2,f3;
	
	f1 = f / (2.0f*PI_RAD) * 1024.0;
	if( f1 < 0.0f )
		i1 = (int)(f1-1.0f);
	else
		i1 = (int)f1;
		
	f2 = f1 - (float)i1;
	
	r1 = isin(i1);
	r2 = isin(i1+1);
		
	f3 = r1 + (r2-r1)*f2;
	
	return f3;
}
#endif

//////////////////////////////////////////////////////////////////////////////
#if 0
float cosli( float f )
{
	int		i1;
	float	f1,f2,r1,r2,f3;
	
	f1 = f / (2.0f*PI_RAD) * 1024.0;
	if( f1 < 0.0f )
		i1 = (int)(f1-1.0f);
	else
		i1 = (int)f1;
		
	f2 = f1 - (float)i1;
	
	r1 = icos(i1);
	r2 = icos(i1+1);
		
	f3 = r1 + (r2-r1)*f2;
	
	return f3;
}
#endif

//////////////////////////////////////////////////////////////////////////////
float icos( int t )
{
	t += 256;

	return( isin( t ));
}

//////////////////////////////////////////////////////////////////////////////
int iasin( float x )
{
	int		low, high, i, nflag = FALSE, done=FALSE;
	float	ld,hd;

	i = 0;
	if( x < 0.0f )
	{
		x *= -1.0f;
		nflag = TRUE;
	}
	
	if(!( x <= 1.0f ))
	{
#ifdef DEBUG
		fprintf( stderr, "Error.  Out or range argument to iasin()\n" );
		lockup();
#else
		while(1) ;
#endif
	}
	
	// find i such that trigs[i] <= x <= trigs[i+1]
	low = 0;
	high = 255;
	while( !done )
	{
		i = (low + high)/2;
		
		if( trigs[i] <= x )
		{
			if( x <= trigs[i+1] )
				done = TRUE;
			else
				low = i + 1;
		}
		else
		{
			if( x >= trigs[i-1] )
			{
				i -= 1;
				done = TRUE;
			}
			else
				high = i + 1;
		}
	}

	// now return that value, or the next higher, whichever is the
	// best fit.
	
	ld = trigs[i] - x;
	hd = trigs[i+1] - x;
	ld *= ld;	hd *= hd;
	
	if( hd < ld )
		i += 1;
	
	return( nflag ? -i : i );

}

//////////////////////////////////////////////////////////////////////////////
int iacos( float x )
{
	return( 256 - iasin( x ));
}

//////////////////////////////////////////////////////////////////////////////
float asin_rad( float x )
{
	int		low, high, mid, nflag = FALSE;
	float	d, val;

	if( x < 0.0f )
	{
		x *= -1.0f;
		nflag = TRUE;
	}

	// find the highest value in trigs[] that's lower than x
	low = 0;
	high = 256;
	while( low < high )
	{
		mid = (low + high)/2;
		if( trigs[mid] < x )
		{
			if( trigs[mid+1] > x )
				break;
			low = mid + 1;
		}
		else
		{
			if( trigs[mid-1] <= x )
			{
				mid -= 1;
				break;
			}
			high = mid + 1;
		}
	}

	// now do a linear interpolation between those mid and mid+1
	d = (x - trigs[mid]) / (trigs[mid+1] - trigs[mid]);
	val = ((float)mid + d) * PI_RAD / 512.0f;

	return( nflag ? -val : val );
	
}

//////////////////////////////////////////////////////////////////////////////
float acos_rad(float x)
{
	return( PI_RAD/2.0f - asin_rad(x));
}

//////////////////////////////////////////////////////////////////////////////
// normalize vector v
//////////////////////////////////////////////////////////////////////////////
void norm( vec3d *v )
{
	float m = magv( v );

	v->x /= m;
	v->y /= m;
	v->z /= m;
}


//////////////////////////////////////////////////////////////////////////////
// compute cross product of vectors v1 and v2
//////////////////////////////////////////////////////////////////////////////
void vxv( vec3d *v1, vec3d *v2, vec3d *v3 )
{	
	v3->x = v1->y * v2->z - v1->z * v2->y;
	v3->y = v1->z * v2->x - v1->x * v2->z;
	v3->z = v1->x * v2->y - v1->y * v2->x;
	v3->w = v1->w * v2->w;
}


//////////////////////////////////////////////////////////////////////////////
// compute magnitude of vector v
//////////////////////////////////////////////////////////////////////////////
float magv( vec3d *v )
{
	float f;

	f = v->x * v->x;
	f += v->y * v->y;
	f += v->z * v->z;

	f /= v->w;

	f = fsqrt( f );

	return f;
}

//////////////////////////////////////////////////////////////////////////////
// compute determinant of top left 3x3 of a 4x4 matrix
//////////////////////////////////////////////////////////////////////////////
float det( float *m )
{
	float d;

	d = m[0]*(m[5]*m[10]-m[6]*m[9]) -
		m[1]*(m[4]*m[10]-m[6]*m[8]) +
		m[2]*(m[4]*m[9]-m[5]*m[8]);

	return d;
}


//////////////////////////////////////////////////////////////////////////////
#if 0
float cos_rad(float x)
{
static float result;
static float x_2;
static float x_4;
static float x_6;

if(x < 0.0001 && x >= 0.0f)
	x = 0.0001;
if(x > -0.0001f && x < 0.0f )
	x = 0.0001f;


if(x >= 0 && x < PI_RAD*0.50f)
	{
	x_2 = x*x;    
	x_4 = x_2*x_2;    
	x_6 = x_4*x_2;    
	result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
	return(result);
	}

if(x >= PI_RAD*0.50f && x <= PI_RAD)
	{
    x = PI_RAD-x;
   	x_2 = x*x;    
	x_4 = x_2*x_2;    
	x_6 = x_4*x_2;    
	result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
	return(-result);
	}

if(x < 0 && x >= -PI_RAD*0.50f)
	{
    x = -x;
	x_2 = x*x;    
	x_4 = x_2*x_2;    
	x_6 = x_4*x_2;    
	result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
	return(result);
	}
 
	/* Else, (x < -PI_RAD*0.50f && x >= -PI_RAD) */
    x = -x;
    x = PI_RAD-x;
	x_2 = x*x;    
	x_4 = x_2*x_2;    
	x_6 = x_4*x_2;    
	result = 1.0f - x_2*0.50f+x_4*0.04167f-x_6*0.00139f;
	return(-result);
} /* cosine() */
#endif
//////////////////////////////////////////////////////////////////////////////
float sin_rad(float x)
{
static float result;
static float x_2;
static float x_3;
static float x_5;
static float x_7;

if(x < 0.0001f && x >= 0.0f )
	x = 0.0001f;

if(x > -0.0001f && x < 0.0f )
	x = -0.0001f;


if(x >= 0 && x < PI_RAD/2.0f)
	{
    x_2=x*x;
    x_3=x*x_2;
    x_5=x_3*x_2;
    x_7=x_5*x_2;
	result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
	return(result);
	}

if(x >= PI_RAD/2.0f && x <= PI_RAD)
	{
    x = PI_RAD-x;
	x_2=x*x;
	x_3=x*x_2;
	x_5=x_3*x_2;
	x_7=x_5*x_2;
	result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
	return(result);
	}

if(x < 0 && x >= -PI_RAD/2.0f)
	{
    x = -x;
	x_2=x*x;
	x_3=x*x_2;
	x_5=x_3*x_2;
	x_7=x_5*x_2;
	result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
	return(-result);
	}

	/* Else, (x < -PI_RAD*0.50f && x >= -PI_RAD) */
    x = -x;
    x = PI_RAD-x;
	x_2=x*x;
	x_3=x*x_2;
	x_5=x_3*x_2;
	x_7=x_5*x_2;
	result = x - x_3*0.166666f + x_5*0.0083333f - x_7*0.00020f;
	return(-result);
}

//////////////////////////////////////////////////////////////////////////////
void rotx( int t, float *m )
{
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;

	m[4] = 0.0f;
	m[5] = icos( t );
	m[6] = -1.0f * isin( t );

	m[8] = 0.0f;
	m[9] = -m[6];
	m[10] = m[5];
}

//////////////////////////////////////////////////////////////////////////////
void roty( int t, float *m )
{
	m[0] = icos( t );
	m[1] = 0.0f;
	m[2] = isin( t );

	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;

	m[8] = -m[2];
	m[9] = 0.0f;
	m[10] = m[0];
}

//////////////////////////////////////////////////////////////////////////////
void rotz( int t, float *m )
{

	m[0] = icos( t );
	m[1] = -1.0f * isin( t );
	m[2] = 0.0f;

	m[4] = -m[1];
	m[5] = m[0];
	m[6] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
void rotxyz( int x, int y, int z, float *m )
{
	float	xm[12],ym[12],zm[12], cm[12];

	xm[3] = 0.0f;	xm[7] = 0.0f;	xm[11] = 0.0f;
	ym[3] = 0.0f;	ym[7] = 0.0f;	ym[11] = 0.0f;
	zm[3] = 0.0f;	zm[7] = 0.0f;	zm[11] = 0.0f;

	rotx( x, xm );
	roty( y, ym );
	rotz( z, zm );

	mxm( zm, ym, cm );
	mxm( cm, xm, m );
}
#endif
//////////////////////////////////////////////////////////////////////////////
// transform vertices in a limb, but don't project, snap, or scale
// store transformed z in oow.
void xform_limb_c( LIMB *limb, float *mat, MidVertex *dest )
{
	int i;
	float px, py, pz;
	float tx, ty, tz;

	for( i = 0; i < limb->nvtx; i++ )
	{
		/* transform vertex into camera coordinates */
		px = limb->pvtx[i].x;
		py = limb->pvtx[i].y;
		pz = limb->pvtx[i].z;

		tx = px * mat[0] + py * mat[1] + pz * mat[2] + mat[3];
		ty = px * mat[4] + py * mat[5] + pz * mat[6] + mat[7];
		tz = px * mat[8] + py * mat[9] + pz * mat[10] + mat[11];

		/* store results */
		tmp_grv[i].x = tx;
		tmp_grv[i].y = ty;
		tmp_grv[i].oow = tz; 
	}
}

//////////////////////////////////////////////////////////////////////////////
