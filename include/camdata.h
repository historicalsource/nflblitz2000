#ifndef _CAMDATA__H
#define _CAMDATA__H

#include "/Video/Nfl/Include/Ani3d.h"

//////////////////////////////////////////////////////////////////////////////
// Declare here so the poop can come last
//
// Pfx   Type
//
// cd   Coordinate Data path	
// fs   Float-Spline
// nd   Normalized Data path	
// rs   Rail-Spline
// sn   SNap camera
// zm   ZooM camera
//

extern CAMSEQINFO	csi_kickoff[];
extern CAMSEQINFO	csi_fs_fgtrk[];
extern CAMSEQINFO	csi_zm_endplayfg[];
extern CAMSEQINFO	csi_fs_endplay[];
extern CAMSEQINFO	csi_cd_endplay[];
extern CAMSEQINFO	csi_fs_endqtr[];


extern float nd_newcam1[]; extern float nd_newcam2[];
extern float nd_newcam3[]; extern float nd_newcam4[];
extern float ndn_newcam4[];

extern float nd_kickoff0[]; extern float nd_kickoff1[];
extern float nd_kickoff2[]; extern float nd_kickoff3[];

extern float nd_koff1_sal[];
extern float ndn_koff1_sal[];

extern float cd_suplex[];

extern float cd_endplay1[]; extern float cd_endplay2[];
extern float cd_endplay3[]; extern float cd_endplay4[];
extern float cd_endplay5[]; extern float cd_endplay6[];

#endif