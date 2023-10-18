#ifndef _DRONE__H
#define _DRONE__H

extern int qb_evaded;

void drone_update( void );
void drone_cover_assign( int, int );
void drone_catch( int, char *, int );

void init_drones_prelineup( int side );
void init_drones_postlineup( int side );

void dump_drone_states( void );
void drone_motion_block( void );

#endif