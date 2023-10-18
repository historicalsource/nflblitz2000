//////////////////////////////////////////////////////////////////////////////
//  Camera angle & distance defines
//
#ifndef CAMDEFS_H
#define CAMDEFS_H

#include "/Video/Include/Math.h"
#include "/Video/Nfl/Include/Ani3d.h"
#include "/Video/Nfl/Include/Game.h"

// Camera angle defines

#define THETA_MIN_DG	0
#define THETA_MAX_DG	360
#define THETA_MIN		(DG2RD((float)THETA_MIN_DG))
#define THETA_MAX		(DG2RD((float)THETA_MAX_DG))

#define PHI_MIN_DG		89
#define PHI_MAX_DG		-89
#define PHI_MIN			(DG2RD((float)PHI_MIN_DG))
#define PHI_MAX			(DG2RD((float)PHI_MAX_DG))

#define THETA_STEP_MAX	(DG2RD(6.0f))			// Max theta stepping
#define PHI_STEP_MAX	(DG2RD(3.0f))			// Max phi stepping

#define ALPHA			(DG2RD(20.556f)/FOCFAC)	// Angle of viewline to vertical limit
#define DELTA			(DG2RD(180.0f)-ALPHA)	// Angle opposite of viewline (base)

// Camera distancing defines

#define FIX_FDX			((0.5f+2.0f)*FOCFAC)	// X-distance Z/X ratio & buffer-factor
#define FIX_FDX_PS		((0.1f+2.0f)*FOCFAC)	//  same but for pre-snap
#define FIX_FDZ			((0.3f+1.0f)*FOCFAC)	// Z-distance buffer-factor

#define FIXUN	(YD2UN / FOCFAC * FIX_FDX_PS)	// Modified YD2UN for pre-snap

// Camera-mode parameters/limits

#define NORMAL_BACKLIM		( 6.0f*FT2UN+FIELDHB)	// X-limit, backfield

#define NORMAL_SIDELIM			( 8.0f*FT2UN+FIELDW)	// Z-limit, sideline
#define NORMAL_SIDELIM_PB		(12.0f*FT2UN+FIELDW)	// Z-limit, sideline
#define NORMAL_SIDELIM_4P		(12.0f*FT2UN+FIELDW)	// Z-limit, sideline
#define NORMAL_SIDELIM_4PPB		(12.0f*FT2UN+FIELDW)	// Z-limit, sideline
#define NORMAL_SIDELIM_QB		(14.0f*FT2UN+FIELDW)	// Z-limit, sideline for MODE_QB
#define NORMAL_SIDELIM_QBPB		(14.0f*FT2UN+FIELDW)	// Z-limit, sideline for MODE_QB
#define NORMAL_SIDELIM_QB4P		(14.0f*FT2UN+FIELDW)	// Z-limit, sideline for MODE_QB
#define NORMAL_SIDELIM_QB4PPB	(14.0f*FT2UN+FIELDW)	// Z-limit, sideline for MODE_QB

#define NORMAL_DIST_MIN		(19.0f*YD2UN)		// Distance, minimum
#define NORMAL_DIST_MINPB	(27.0f*YD2UN)		// Distance, minimum for pull-back
#define NORMAL_DIST_MIN4P	(27.0f*YD2UN)		// Distance, minimum
#define NORMAL_DIST_MIN4PPB	(27.0f*YD2UN)		// Distance, minimum for pull-back
#define NORMAL_DIST_QB		(30.0f*YD2UN)		// Distance, default for MODE_QB
#define NORMAL_DIST_QBPB	(38.0f*YD2UN)//42.0	// Distance, default for MODE_QB pull-back
#define NORMAL_DIST_QB4P	(38.0f*YD2UN)//42.0	// Distance, default for MODE_QB
#define NORMAL_DIST_QB4PPB	(38.0f*YD2UN)//42.0	// Distance, default for MODE_QB pull-back

#define NORMAL_DIST_GOAL	( 0.0f*YD2UN)//7.0	// Distance, goal-approach effects
#define NORMAL_DIST_MAX_PS	(19.0f*FIXUN)		// Distance, maximum for pre-snap

#define NORMAL_THETA		(DG2RD( 90.0f))		// Theta, default
#define NORMAL_THETA_OPP	(DG2RD(270.0f))		// Theta, default opposing
#define NORMAL_PHI			(DG2RD(-25.0f))		// Phi, default
#ifdef SPECIAL_MODES
#define NORMAL_PHI_A		(DG2RD(-30.0f))		// Phi, default alternate
#endif //SPECIAL_MODES


#define SIDE_BACKLIM		( 6.0f*FT2UN+FIELDHB)	// X-limit, backfield
#define SIDE_SIDELIM		( 8.0f*FT2UN+FIELDW)	// Z-limit, sideline
#define SIDE_SIDELIM_QB		(14.0f*FT2UN+FIELDW)	// Z-limit, sideline for MODE_QB

#define SIDE_DIST_GOAL		( 7.0f*YD2UN)		// Distance, goal-approach effects
#define SIDE_DIST_MAX_PS	(19.0f*FIXUN)		// Distance, maximum for pre-snap
#define SIDE_DIST_MIN		(17.0f*YD2UN)		// Distance, minimum
#define SIDE_DIST_MINPB		(17.0f*YD2UN)		// Distance, minimum for pull-back
#define SIDE_DIST_QB		(30.0f*YD2UN)		// Distance, default for MODE_QB
#define SIDE_DIST_QBPB		(30.0f*YD2UN)		// Distance, default for MODE_QB pull-back

#define SIDE_THETA			(DG2RD(180.0f))		// Theta, default
#define SIDE_THETA_OPP		(DG2RD(0.0f))		// Theta, default opposing
#define SIDE_PHI			(DG2RD(-35.0f))		// Phi, default


#define TOP_BACKLIM			( 6.0f*FT2UN+FIELDHB)	// X-limit, backfield
#define TOP_SIDELIM			( 8.0f*FT2UN+FIELDW)	// Z-limit, sideline
#define TOP_SIDELIM_QB		(14.0f*FT2UN+FIELDW)	// Z-limit, sideline for MODE_QB

#define TOP_DIST_GOAL		( 4.0f*YD2UN)		// Distance, goal-approach effects
#define TOP_DIST_MAX_PS		(19.0f*FIXUN)		// Distance, maximum for pre-snap
#define TOP_DIST_MIN		(17.0f*YD2UN)		// Distance, minimum
#define TOP_DIST_MINPB		(17.0f*YD2UN)		// Distance, minimum for pull-back
#define TOP_DIST_QB			(30.0f*YD2UN)		// Distance, default for MODE_QB
#define TOP_DIST_QBPB		(30.0f*YD2UN)		// Distance, default for MODE_QB pull-back

#define TOP_THETA			(DG2RD( 90.0f))		// Theta, default
#define TOP_THETA_OPP		(DG2RD(270.0f))		// Theta, default opposing
#define TOP_PHI				(DG2RD(-89.5f))		// Phi, default


#define TRGY_NORMAL			( 6.0f*FT2UN)		// Target Y default
#define TRGY_SIDE			( 6.0f*FT2UN)		// Target Y default
#define TRGY_TOP			( 0.0f*YD2UN)		// Target Y default
#define TRGY_ZOOM			( 5.0f*FT2UN)		// Target Y default
#define TRGY_SPLINE			( 4.0f*FT2UN)		// Target Y default
#define TRGY_RAIL			( 5.0f*FT2UN)		// Target Y default
#define TRGY_PATH			( 5.0f*FT2UN)		// Target Y default
#define TRGY_COURSE			( 5.0f*FT2UN)		// Target Y default

#define ZOFF_NORMAL_ONSIDE	( 8.0f*YD2UN)		// Rotated Z offset for onside kick
#define ZOFF_NORMAL_QB		( 6.0f*YD2UN)		// Rotated Z offset for MODE_QB
#define ZOFF_SIDE_QB		( 6.0f*YD2UN)		// Rotated Z offset for MODE_QB
#define ZOFF_TOP_QB			( 6.0f*YD2UN)		// Rotated Z offset for MODE_QB

#ifdef DBG_KO
#define KI(n,x,y,z,a)	KO_INFO ki_##n = {x,y,z,a,&n,#n}
#else
#define KI(n,x,y,z,a)	KO_INFO ki_##n = {x,y,z,a,&n}
#endif //DBG_KO

////////////////////////
// Camera kickoff structures & defines

typedef struct tagKO_INFO
{
	float	x,y,z;								// camera x,y,z position
	int		auto_hike_cntr; 					// ticks till kickoff starts
	void	*pseqinfo;							// generic ptr for info spec data
#ifdef DBG_KO
	char *	szname;								// data id string
#endif //DBG_KO
} KO_INFO;

////////////////////////
// Camera pop-replay structures & defines

// Possible pop-replay camera owners in order of priority
enum {
	POP_IDLE = -1,								// Always lowest

	POP_FUMBLE,
	POP_FG_UPRIGHTS,

	POP_ACTIVE									// Always highest
};

typedef struct tagPOP_INFO
{
	int			tobj;							// Replay target obj
	int			ecnt;							// Replay frame cnt: post-event
	int			rcnt;							// Replay frame cnt: total
	int			rdly;							// Replay slomo delay ticks
	CAMSEQINFO *pcsi;							// Replay CSI table ptr
} POP_INFO;

#define TRGY_FAC	1000000.0f
#define TRGY_CNV(x)	((void *)(int)(x*TRGY_FAC))

#define POP_TOBJ_EVENT			0				// Watch event vertex
#define POP_TOBJ_BALL			1				// Watch ball
#define POP_TOBJ_WHO_THREW		2				// Watch <ball_obj.who_threw>
#define POP_TOBJ_INT_RECEIVER	3				// Watch <ball_obj.int_receiver>

////////////////////////

#define POP_FMBL1_TOBJ	POP_TOBJ_EVENT			// Replay target obj
#define POP_FMBL1_ECNT	40						// Replay frame cnt: post-event
#define POP_FMBL1_RCNT	50						// Replay frame cnt: total
#define POP_FMBL1_RDLY	0						// Replay slomo delay ticks

#define POP_FMBL1_MCNT	(POP_FMBL1_RCNT*\
						(POP_FMBL1_RDLY+1))		// Replay mode tick total

#define POP_FMBL2_TOBJ	POP_TOBJ_EVENT			// Replay target obj
#define POP_FMBL2_ECNT	20						// Replay frame cnt: post-event
#define POP_FMBL2_RCNT	28						// Replay frame cnt: total
#define POP_FMBL2_RDLY	1						// Replay slomo delay ticks

#define POP_FMBL2_MCNT	(POP_FMBL2_RCNT*\
						(POP_FMBL2_RDLY+1))		// Replay mode tick total

#define POP_FMBL3_TOBJ	POP_TOBJ_EVENT			// Replay target obj
#define POP_FMBL3_ECNT	25						// Replay frame cnt: post-event
#define POP_FMBL3_RCNT	35						// Replay frame cnt: total
#define POP_FMBL3_RDLY	0						// Replay slomo delay ticks

#define POP_FMBL3_MCNT	(POP_FMBL3_RCNT*\
						(POP_FMBL3_RDLY+1))		// Replay mode tick total

////////////////////////

#define POP_FGHP1_TOBJ	POP_TOBJ_EVENT			// Replay target obj
#define POP_FGHP1_ECNT	8						// Replay frame cnt: post-event
#define POP_FGHP1_RCNT	20						// Replay frame cnt: total
#define POP_FGHP1_RDLY	2						// Replay slomo delay ticks

#define POP_FGHP1_MCNT	(POP_FGHP1_RCNT*\
						(POP_FGHP1_RDLY+1))		// Replay mode tick total

#define POP_FGHP2_TOBJ	POP_TOBJ_EVENT			// Replay target obj
#define POP_FGHP2_ECNT	12						// Replay frame cnt: post-event
#define POP_FGHP2_RCNT	28						// Replay frame cnt: total
#define POP_FGHP2_RDLY	1						// Replay slomo delay ticks

#define POP_FGHP2_MCNT	(POP_FGHP2_RCNT*\
						(POP_FGHP2_RDLY+1))		// Replay mode tick total

//////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef CAMDEFS_H
