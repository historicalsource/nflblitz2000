#ifndef _SOUNDS__H
#define _SOUNDS__H

extern int said_latehit_sp;
extern int said_30_sp;
extern int said_20_sp;
extern int said_10_sp;
extern int said_5_sp;

extern void pre_snap_speech_proc(int *args);
extern void announce_eoq_speech(void);
extern void qb_duck_speech(void);
extern void head_plow_speech(void);
extern void crowd_cheer_snds(void);
extern void crowd_tackle_snds(void);
extern void late_hit_speech_proc(int *args);
extern void hit_intended_receiver_sp_proc (int *args);
extern int on_fire_sp(int);
extern void monkey_on_back_sp(void);
extern void late_hit_speech(void);
extern void hit_intended_receiver_sp(void);

#endif