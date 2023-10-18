#ifndef _PLAYSEL__H
#define _PLAYSEL__H

extern int miss_kick;

void load_play_select_textures(void);
void play_select(void);

void init_play_limbs(int off_side, int side);
void clobber_playsel( void );
void clean_playsel( void );
void update_play_page(int scrn_side, int page, int flip, int curpos);

play_t * PlaySel__MakeChoice(int side, int page, int p_num, int cur_pos, int tm_num, int force_choose);

#endif