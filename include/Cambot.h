#ifndef _CAMBOT__H
#define _CAMBOT__H

#include "/Video/Nfl/Include/Ani3d.h"

extern VIEWCOORD cambot;

extern void set_cambot_theta(float);
extern void set_cambot_phi(float);
extern void update_cambot_xform(void);
extern void cambot_proc( int * );

#endif