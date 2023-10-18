/******************************************************************************/
/*                                                                            */
/* ani3D.h - Animation & 3D Pipeline system header file                       */
/*                                                                            */
/* Written by:  Jason Skiles                                                  */
/* Version:     1.00                                                          */
/*                                                                            */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                      */
/* All Rights Reserved                                                        */
/*                                                                            */
/******************************************************************************/
#ifndef __ANI3D_H__
#define __ANI3D_H__

#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#ifdef DEBUG
	#define SPECIAL_MODES						// Def for additional camera modes
#endif

// no better place to put this
//#define GRAVITY	-0.110f     
#define GRAVITY	-0.08f     
/* Old .08 */

#define _M_PI		3.14159265358979323846

#define	DEG2RAD(x)	(((float)x/180.0f) * _M_PI)

/* animation sequence header */
typedef struct anim_header
{
	int		nframes;	/* frames in anim seq */
	int		nlimbs;		/* limbs in anim seq */
	int		offset;		/* file position of sequence */
	int		flags;		// AH_ flags defined below
} anihdr;

/* simple 4-vector */
typedef struct _vec3d
{
	float	x,y,z,w;
} vec3d;

/* 4-point spline struct */
typedef struct _SPLINE
{
	float	x1,y1,z1;		// starting pt (typically 0,0,0)
	float	x2,y2,z2;		// control pt 1
	float	x3,y3,z3;		// control pt 2
	float	x4,y4,z4;		// ending pt
} SPLINE;

/* cambot info & sequence-control struct */
typedef struct _CAMSEQINFO
{
	int		mode;			// camera mode
	int		mode_cnt;		// camera mode active tick count
	int		trav_cnt;		// travel time tick count
	float	theta;			// XZ-plane target angle
	float	phi;			// Y-elevation target angle
	float 	d;				// distance target from cambot to focus
	void	*pseqinfo;		// generic ptr for info spec data
} CAMSEQINFO;

/* camera struct */
typedef struct _VIEWCOORD
{
	CAMSEQINFO	csi;			// cambot info & sequence-control struct
	SPLINE		s;				// current spline track
	void		*pmodeobj;		// generic ptr for mode spec data
	int			tick_cnt;		// mode elapsed-time tick count
	int			mode_last;		// camera mode - last itteration
	int			mode_prev;		// camera mode - previous for resume mode
	int			was_playover;	// set if play was over, clr if wasn't
	int			new_quarter;	// set if a new quarter, clr if not
	int			kickoff_flag;	// set if doing a full or quick kickoff
	int			off_prev;		// which side had the ball last time (0 or 1)
	float		theta_trg;		// XZ-plane target angle
	float		phi_trg;		// Y-elevation target angle
	float		theta;			// XZ-plane view angle
	float		phi;			// Y-elevation view angle
	float 		d,x,y,z;		// camera distance to focus & x,y,z position
	float 		a,b,c;			// focus x,y,z position
	float 		e,f,g;			// point above focus in y on viewplane
	float 		dv,xv,yv,zv;	// camera distance,x,y,z movement velocities
	float 		tv,pv;			// camera theta,phi movement velocities
	float 		da,xa,ya,za;	// camera distance,x,y,z velocity accelerations
	float 		ta,pa;			// camera theta,phi velocity accelerations
	float		xform[12];		// matrix
	float		sin_theta;		// Sine of XZ-plane view angle
	float		cos_theta;		// Cosine of XZ-plane view angle
	float		sin_phi;		// Sine of Y-elevation view angle
	float		cos_phi;		// Cosine of Y-elevation view angle
//	float 		length;			// distance from cambot to viewplane
} VIEWCOORD;

typedef struct _ARROWINFO
{
	void *	arr_obj;
	void *	num_obj;
	void *	ppdata;
	float	plyr_sx;
	float	plyr_sy;
	float	plyr_sz;
	float	arr_ax;
	float	arr_ay;
	float	num_ax;
	float	num_ay;
	int		pnum;
	int		pnum_last;
	int		ai_flags;
	int		timer;
	int		show;
#ifdef SHOW3DARROWS
	int		show3d;
#endif
} ARROWINFO;

// flags for ARROWINFO.flags
#define AF_INACTIVE			0x01		// arrow is not active

#define AF_ONSCREEN			0x02		// arrow is on screen this tick

#define AF_OFFSCREENARROW	0x04		// player off-screen arrow was active

// cambot mode values
enum
{
// n-modes
	NMODE_FG = -1,
// primary modes
	CAM_NORMAL,
	CAM_SIDE,
	CAM_TOP,
	CAM_ATTRACT,
// transient modes
	CAM_ZOOM,
	CAM_ZO_NR,			// NR   - no ranging
	CAM_ZO_NFR,			// NFR  - no facing, no ranging
	CAM_SPLINE,
	CAM_SP_NFR,			// NFR  - no facing, no ranging
	CAM_SP_CNFR,		// CNFR - use last cambot, no facing, no ranging
	CAM_RAIL,
	CAM_RA_NF,			// NF   - no facing
	CAM_RA_NFT,			// NFT  - no facing, no tracking
	CAM_PATH,
	CAM_PA_NF,			// NF   - no facing
	CAM_PA_NFT,			// NFT  - no facing, no tracking
	CAM_COURSE,
	CAM_CO_C,			// C    - use last cambot
	CAM_CO_CNT,			// CNT  - use last cambot, no tracking
	CAM_P2P,
	CAM_GOPHER,
	CAM_RESUME,
//  total modes
	CAM_MODECNT,
//  n-mode special cases
	NMODE_NOCSI,		// no camseqinfo
#ifdef SPECIAL_MODES
	CAM_HELMET,
	CAM_NORM,
	CAM_NORMAL_A,
	CAM_NORMAL_SP1,
	CAM_NORMAL_SP2
#endif //SPECIAL_MODES
};

/* model source data structures */
typedef struct _VERTEX
{
	float	x,y,z;
} VERTEX;

typedef struct _ST
{
	float	s,t;
} ST;

typedef struct _TRI
{
	unsigned short	v1,v2,v3;
	unsigned short	t1,t2,t3;
	short			texture;
} TRI;

typedef struct _LIMB
{
	int		nvtx;
	int		ntris;
    VERTEX	*pvn;
	VERTEX	*pvtx;
	ST		*pst;
	TRI		*ptri;
} LIMB;

/* skeleton node */
typedef struct _SNODE
{
	float		x,y,z;			/* joint position (relative to parent) */
	int			flags;			/* flags */
	int			children[3];	/* child node indices (self-relative, 0 if none) */
} SNODE;


/* 4-point spline struct */
typedef struct _BSPLINE
{
	int		vcnt;
	float	b_vel, e_vel;	// begining & ending step-velocites
	float	accel;			// step-velocity acceleration rate
	VERTEX	v[1];			// start of pt data
} BSPLINE;


// There can be lots of 3D object type structs, but they all should have one
// of these structs as their first elements.  Data that's particular to the
// type of object goes after the obj_3d.  Data that's universal to all 3D
// objects goes inside the 3d object struct.  All calls to/from the anim
// system use type *obj_3d.  Calling/called functions can cast the pointer
// back to their more specific type if'n they need access to the extra data.
// IF THE OBJECT ANIMATION DATA MAY BE COMPRESSED OR INTERPOLATED, you have to
// include space for the uncompressed quats inside the enclosing data structure
// immediately after the obj_3d struct.
// NOTE ON INTERPOLATION:  The target frame must be uncompressed.  Therefore,
// if the target sequence is compressed, only the first frame is a legal dest.
// Source frame may be compressed.
// FOR STREAM INTERPOLATION: The sequence must be uncompressed.

typedef struct _anidata
{
	int			animode;		// animation mode
	int			hackval;		// MODE_TURNHACK arg
	char		*pscrhdr;		// animation script header (source code)
	char		*pscript;		// animation script pointer
	int			seq_index;		// sequence index (from xxxxxSEQ.H)
	int			seq_index2;		// interpolation source frame seq index
	int			anicnt;			// ticks to hold this frame
	float		*pframe0;		// frame 0 (float) pointer
	char		*pframe;		// head of frame (int) data
	float		*pxlate;		// translation (animpt) data
	int			iframe;			// frame number
	float		fframe;			// frame number (float format, iframe is rounded down)
								// fframe is only valid when animode & MODE_INTSTREAM
	anihdr		*pahdr;			// ani sequence header ptr (data)
	float		*pq;			// ptr to current frame data (script or object struct ram)
	float		*adata;			// ptr to anim data block (decompression scratch pad)
	float		*idata;			// ptr to interpolation base data
	int			icnt;			// interpolation counter
	int			scnt;			// stream interpolation counter
	float		sinc;			// stream interpolation frame increment
	int			vframe;			// frame index for ANI_INTTOSEQ.  Used with AA_VAR_FRAME flag.
	int			vcount;			// tick count for ANI_INTTOSEQ.  Used with AA_VAR_COUNT flag.
} anidata;

typedef struct _obj_3d
{
	float		x,y,z;			// world position
	float		vx,vy,vz;		// velocity
	float		friction;		// applied to X & Z velocities every tick.
	float		vxf,vzf; 		// facing-relative velocities (+z is forward)
	float		ax,ay,az;		// prev frame ani x & z
	float		ascale;			// x & z animpt scale factor
	int			fwd;			// facing direction (0-1023)
	int			tgt_fwd;		// intended facing direction (0-1023)
	int			turn_rate;		// turn rate per tick (reset any time you change tgt_fwd)
	int			fwd2;			// second facing direction (0-1023) (mostly unused)
	int			tgt_fwd2;		// second intended facing direction (0-1023)
	int			turn_rate2;		// second turn rate per tick
	anidata		adata[4];		// animation data (two parts, +2 for interpolation)
	int			plyrmode;		// player mode
#ifdef SEATTLE
	GrMipMapId_t *pdecal;		// array of decal indices
#else
	Texture_node_t **pdecal;		// array of decal indices
#endif
	int			flags;			// general object flags (defined in player.h)
	struct _obj_3d *pobj_hit;	// this is set to point to the last fbplyr to collide with me
	float		default_ascale;	// x & z animpt scale factor
} obj_3d;

//
// script interpreter function prototypes
//
void anim_sys_init( void );
int load_anim_block( int, char *, int );
void change_anim( obj_3d *, char * );
void change_anim1( obj_3d *, char * );
void change_anim2( obj_3d *, char * );
void animate_model( obj_3d * );

//
// 3D pipeline & associated function prototypes
//
#ifdef SEATTLE
void load_textures( char *[], int *, GrMipMapId_t *, int, int, int, int );
void render_node( SNODE *, LIMB **, float *, float *, GrMipMapId_t *, float * );
void render_node_mpc( SNODE *, LIMB **, float *, GrMipMapId_t *, float * );
void render_limb( LIMB *, float *, GrMipMapId_t *, int * );
void render_limb_noclip( LIMB *, float *, GrMipMapId_t *, int * );
void render_plate( LIMB *, float *, float *, GrMipMapId_t *, int * );
#else
void load_textures( char *[], int *, Texture_node_t **, int, int, int, int );
void render_node( SNODE *, LIMB **, float *, float *, Texture_node_t **, float * );
void render_node_mpc( SNODE *, LIMB **, float *, Texture_node_t **, float * );
void render_limb( LIMB *, float *, Texture_node_t **, int * );
void render_limb_noclip( LIMB *, float *, Texture_node_t **, int * );
void render_plate( LIMB *, float *, float *, Texture_node_t **, int * );
#endif
void xform_limb_c( LIMB *, float *, MidVertex * );
void generate_matrices( SNODE *, float *, float *, float *, float * );
void render_limb_jers( LIMB *, float *, int, int );
void mxm( float *m1, float *m2, float *m3 );
void qxq( float *, float *, float * );
void mat2quat( float *mat, float *q );
void slerp( float *, float *, float, float * );
void adjust_quat( float *, float * );
//float fsqrt( float );
void quat2mat( float *q, float *mat );
float isin( int );
float icos( int );
float sinli( float );
float cosli( float );
int iasin( float );
int iacos( float );
float sin_rad( float );
float cos_rad( float );
float asin_rad( float );
float acos_rad( float );
void rotx( int, float * );
void roty( int, float * );
void rotz( int, float * );
void rotxyz( int, int, int, float * );
void vxv( vec3d *, vec3d *, vec3d * );
void norm( vec3d * );
float magv( vec3d * );
float det( float * );
int ptangle( float *, float * );

#ifndef NULL
#define NULL 0
#endif

//
// skeleton node flags
//

#define SN_DBLSIDE_BIT		0					// limb is double-sided
#define SN_DBLSIDE			(1<<SN_DBLSIDE_BIT)

#define SN_NODRAW_BIT		1					// don't draw limb
#define SN_NODRAW			(1<<SN_NODRAW_BIT)

#define SN_HEADLIMB_BIT		2					// limb is the head
#define SN_HEADLIMB			(1<<SN_HEADLIMB_BIT)

// ani header flags
#define AH_COMPRESSED		0x1

//
// animation modes -- duplicated in ani3d.equ
//
#define MODE_END			0x0001

// don't let player interrupt sequence
#define MODE_UNINT			0x0002

// in two-part anim mode (set this on the first part)
#define MODE_TWOPART		0x0004

// in streaming interpolation mode
#define MODE_INTSTREAM		0x0008

// interpolate at next sequence transition (if supported by new sequence)
// use ANI_PREP_XITION to set this flag, target frame, and tick count.
#define MODE_INTXITION		0x0010

// Allow limited stick control.  Player can control direction, but
// run functions won't change the anim sequence.
#define MODE_ROTONLY        0x0020

// Zero bit.  Set whenever an ANI_CODE returns 0, cleared when one returns
// nonzero.
#define MODE_ZERO			0x0040

// Reacting to a no-mode attack.  Immune to further such attacks.
#define MODE_REACTING		0x0080

// Can pick up a block even if UNINT
#define MODE_CANBLOCK		0x0100

// No cylinder interaction with other players
#define MODE_NOCYL			0x0200

// Player is stiff arming
#define MODE_STIFF_ARM		0x0400

// Player can't catch the ball
#define MODE_NOCATCH		0x0800

// Player is stiff arming
#define MODE_RUNNING_PUSH	0x1000

// top 16 bits of mode flags must be individually set and cleared.
// ANI_SETMODE(mask) will result in two operations.  The low halfword
// of the mask will be assigned to the low halfword of the mode word.
// The high halfword will be ORed in.  The idea is that if you've got
// a mode like MODE_ANCHOR that you don't want gettting wiped out on
// every ANI_SETMODE command, put it in the high halfword.

// hold ref point (instead of anim pt) steady at next frame change
#define MODE_ANCHORY		0x00010000
#define MODE_ANCHORXZ		0x00020000
#define MODE_ANCHOR			(MODE_ANCHORY|MODE_ANCHORXZ)

// not the primary animation -- ignore translation
#define MODE_SECONDARY		0x00040000

// interpolating
#define MODE_INTERP			0x00080000

// noisy debugging
#define MODE_ANIDEBUG		0x00100000

// next time you interpolate, rotate the src root quat by hackval
#define MODE_TURNHACK		0x00200000

// puppet move.  various effects
#define MODE_PUPPETEER		0x00400000
#define MODE_PUPPET			0x00800000

//
// anim block indices -- duplicated in ani3d.equ
//
#define ASB_GENSEQ		0
#define	ASB_LAST		ASB_GENSEQ

// anim script tag
#define	SCRIPT			0x80

//
// special arguments to ANI commands -- duplicated & documented in ani3d.equ
#define	AA_PROPFRAME		-1
#define AA_VAR_FRAME		-2
#define AA_VAR_COUNT		-1
#define AA_TOGROUND			-1
#define AA_TOEND			-2
#define AA_RAND				-1

#define AA_TGT_CARRIER		-1
#define AA_TGT_TARGET		-2

// commands are defined in ani3d.equ

#endif	// #ifndef __ANI3D_H__

/******************************************************************************/
