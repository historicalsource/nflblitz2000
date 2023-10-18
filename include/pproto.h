//
// misc player-related prototypes.
// these are generally only needed in player.c, plyrseq.c, and detect.c
//
void pick_random_mill(obj_3d *pobj);
int my_choose_target( obj_3d *);
void general_run( obj_3d * );
void face_attacker( obj_3d *, int );
int face_carrier( obj_3d *, int );
int face_throw_target( obj_3d *, int );
void face_ball( obj_3d *, int );
void maybe_dive( obj_3d * );
int carrier_bearing_le( obj_3d *, int );
int moving_backwards( obj_3d * );
void face_velocity( obj_3d *, int );
void release_puppet( obj_3d *, int );
void qb_release_ball( obj_3d * );
int plyr_have_ball( obj_3d * );
int rand_func( obj_3d *, int );
int plyr_rand( obj_3d *, int );
int choose_target( obj_3d *, int );
void player_down( obj_3d * );
void bobble_pass( fbplyr_data *, float bx, float by, float bz );
void fumble_ball( fbplyr_data *, float bx, float bz );
void pick_random_db_anim(obj_3d *);
void pick_random_db_zone_anim(obj_3d *pobj);
void pick_random_db_m2m_anim(obj_3d *pobj);
void pick_random_linemen_anim(obj_3d *);
void pick_random_shotgun_qb_anim(obj_3d *);
void pick_random_qb_anim(obj_3d *);
void pick_random_wr_anim(obj_3d *);
void drop_ball( obj_3d * );
void lateral_ball( obj_3d * );
void hike_ball(obj_3d *);
void punt_ball(obj_3d *);
void spike_ball(obj_3d *,int);
void spin_ball(obj_3d *);
int is_inplay( obj_3d * );
int is_ball_kicked( obj_3d * );
int in_end_zone( obj_3d * );
int near_my_endzone( obj_3d * );
int td_check( int );
void update_onfire_stuff(fbplyr_data *, fbplyr_data *);
//void check_on_fire(void);
void reset_onfire_values( void );
void reset_team_fire( int );
void set_defense_speeds( void );
void dump_anim_data( int );
void dump_game_state( int );
void deflect_pass( obj_3d * );
void print_ball_flags( void );
void print_game_flags( void );
int intercept_ok( obj_3d * );
float pass_rating( int, int, int, int, int );
void norm2( float * );
void abort_drone_route( int );
int bearing( int, int );
void update_stations( void );
int off_captain( void );

void hide_string( void * );
void unhide_string( void * );

// arg bits for td_check
#define TDC_NOAIRCATCH		0x01
#define TDC_NOCONTROL		0x02

void drone_poss_change( void );

// misc extern data
extern char *animode_names[];
extern char *ball_flag_names[];
extern char *loose_ball_names[];
extern char *player_flags_names[];
extern char *seq_names[];