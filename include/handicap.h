#ifndef HANDICAP_H
#define HANDICAP_H

#define OFFH	(game_info.team_handicap[game_info.off_side])
#define DEFH	(game_info.team_handicap[!(game_info.off_side)])
#define PLYRH(x) (game_info.team_handicap[(x)->team])
#define PNUMH(x) (game_info.team_handicap[player_blocks[x].team])

void set_handicaps( void );

extern float dback_speed_ht[12][6];
extern float dline_speed_ht[12][6];
extern int def_acceltime_ht[12][2];
extern int of_bobble_ht[12];
extern int db_preact_ht[12];
extern int blitz_time_ht[12];
extern int drn_evade_ht[12];
extern int drn_qb_range_ht[12];
extern int bob_up_ht[12];
extern int bob_cap_ht[12];
extern int spin_cap_ht[12];
extern int hurdle_cap_ht[12];
extern int bobble_human_ht[12];
extern int bobble_hteam_ht[12];
extern int bobble_drone_ht[12];
extern int fg_bonus_ht[12];
extern int plow_ht[12];
extern int early_hit_ht[12];
extern int drone_db_att_ht[12];
extern int db_stand_bobble_ht[12];
extern int db_air_batdown_ht[12];
extern float bozo_cookie_ht[12];
extern int bozo_base_ht[12];
extern int bozo_rand_ht[12];
extern int min_ko_catch_ht[12];
extern int max_ko_catch_ht[12];
extern int def_hteam_dive_ht[12];
extern int def_drone_dive_ht[12];
extern float dive_range_ht[12];
extern int cp_breakout_ht[12];
extern int db_cheapshot_ht[12];
extern int db_cheapshot_blitz_ht[12];
extern int def_superman_ht[12];
extern int oline_block_ht[12];
extern int nocatch_ht[12];
extern int zone_thinkmin_ht[12];
extern int zone_thinkmax_ht[12];
extern int qb_stiff_arm_ht[12];

#endif // HANDICAP_H