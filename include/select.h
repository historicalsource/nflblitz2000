#ifndef _SELECT__H
#define _SELECT__H

#include "/Video/Nfl/Include/NflCmos.h"
#include "/Video/Include/Goose/Sprites.h"

#define NAME_ENTRY_SCRN	 	0
#define TM_SELECT_SCRN 	 	1
#define VS_SCRN        	 	2
#define BUY_IN_SCRN    	 	3
#define HALFTIME_SCRN  	 	4
#define PLAY_SELECT_SCRN	5
#define COACHING_TIPS_SCRN 6

#define SCREENS         7

// general stuff
//DJT moved it to game.h
//#define	MAX_PLYRS	4

#define	JOY_UP		1
#define	JOY_DOWN	2
#define	JOY_LEFT	4
#define	JOY_RIGHT	8

#define	JOY_DWN_LFT		6
#define	JOY_DWN_RGT		10
#define	JOY_UP_LFT		5
#define	JOY_UP_RGT		9

#define	TURBO_BUTTON	4		// white
#define	A_BUTTON		1		// red
#define	B_BUTTON	    2		// blue

#define	NO				0
#define	YES				1
#define	UP				0
#define	DOWN			1

#define	X_VAL			0
#define	Y_VAL			1
#define	W_VAL			0
#define	H_VAL			1

//
// process id's
//
#define ZERO_PID     	 	0

#define P1_CURSOR_PID    	200
#define P1_CHALNGR_PID   	201

#define P2_CURSOR_PID    	202
#define P2_CHALNGR_PID    	203

#define P3_CURSOR_PID    	204
#define P3_CHALNGR_PID    	205

#define P4_CURSOR_PID    	206
#define P4_CHALNGR_PID    	207

#define CREDIT_PID      	415

#define TM1_TM_SELECT_PID   400
#define TM1_CHALENGER_PID   401

#define TM2_TM_SELECT_PID   402
#define TM2_CHALENGER_PID   403

#define P1_PLAY_SEL_PID		500
#define P2_PLAY_SEL_PID		501

#define FLASH_PID			600
#define TIMER_PID			601
#define ROTATE_LOGO_PID		602

#define	NUM3D_PID			603

#define	PLAQ_AWARD_PID		604			// leave 4 after this for the players

//
// string ids
//
#define TIMER_MSG_ID	80
#define CREDIT_MSG_ID	81

#define P1_GEN_STR_ID    10
#define P1_NAME_STR_ID   11
#define P1_PIN_STR_ID    12

#define P2_GEN_STR_ID    20
#define P2_NAME_STR_ID   21
#define P2_PIN_STR_ID    22

#define P3_GEN_STR_ID    31
#define P3_NAME_STR_ID   32
#define P3_PIN_STR_ID    33

#define P4_GEN_STR_ID    42
#define P4_NAME_STR_ID   43
#define P4_PIN_STR_ID    44

//
// General Player defines
//
#define PLYR_1          0
#define PLYR_2          1
#define PLYR_3          2
#define PLYR_4          3

#define PLYR_1_BIT      (1<<0)
#define PLYR_2_BIT      (1<<1)
#define PLYR_3_BIT      (1<<2)
#define PLYR_4_BIT      (1<<3)

// defines
#define MENU_LEVELS		4		// yes no, enter intials, enter pin number, team select
#define	YESNO			0
#define	INITS			1
#define	PIN_NBR			2
#define	TM_SELECT		3

#define	STATS_PER_PAGE			6
#define	NUM_STATS				12


#define	NUM_ON_ROSTER			9					// doesn't include linemen
#define	NUM_CITY_NAMES			9					// 4 on top, middle, 4 on bottom
#define	NUM_LIMBS				10

#define	LTR_COLS				4	//5
#define	LTR_ROWS				8	//6
#define	PIN_COLS				3
#define	PIN_ROWS				4

//#define	NUM_VIS_PLAYS		9
//#define	PSEL_COLS			3
//#define	PSEL_ROWS			3

// offsets into TEAM SELECT 'side' object ptrs. array
#define	TM_LOGO_OBJ_PTR				 0
#define	TM_NME1_OBJ_PTR				 1
#define	TM_NME2_OBJ_PTR				 2
#define	TM_NME3_OBJ_PTR				 3
#define	TM_NME4_OBJ_PTR				 4	// center name
#define	TM_NME5_OBJ_PTR				 5
#define	TM_NME6_OBJ_PTR				 6
#define	TM_NME7_OBJ_PTR				 7
//#define	TM_NME8_OBJ_PTR				 8
//#define	TM_NME9_OBJ_PTR				 9
#define	TM_PASS_METR_OBJ_PTR	10
#define	TM_RUSH_METR_OBJ_PTR	11
#define	TM_RUSH_LIMN_OBJ_PTR	12
#define	TM_RUSH_DEFN_OBJ_PTR	13
#define	TM_RUSH_SPTM_OBJ_PTR	14
#define	TM_QB_NME_OBJ_PTR			15
#define	TM_RNME2_OBJ_PTR			16
#define	TM_RNME3_OBJ_PTR			17
#define	TM_RNME4_OBJ_PTR			18
#define	TM_RNME5_OBJ_PTR			19
#define	TM_RNME6_OBJ_PTR			20
#define	TM_RNME7_OBJ_PTR			21
#define	TM_RNME8_OBJ_PTR			22
#define	TM_RNME9_OBJ_PTR			23


// joystick repeat and delay counts
#define	TICKS_BEFORE_START_REPEAT		25		//35
#define	TICKS_BETWEEN_REPEATS				4			//5

// background scroll defines
#define	NUM_BKGRND_CHUNKS			4		//5
#define	NUM_PIECES_IN_CHUNK			2

#define	NUM_HLFTME_STATS			11

#define OVR_RNK		0						// indexes into the 'plr_ranks' table
#define	OFF_RNK		1
#define	STK_RNK		5
#define	DEF_RNK		7

//
//  structure def's
//
typedef struct tmu_ram_texture {
	char						*t_name;
	int							tex_id;
	} tmu_ram_texture_t;

// -- in Vsscreen.C
extern volatile int plates_on;
extern char plate_name[];
extern sprite_info_t * plate_obj[];
extern image_info_t plate_imgs[];

extern int show_them_hiscore_tables_first;

//
// Routine typedef's
//
// -- in Select.C
extern void challenger_needed_msg(int *);
extern void player_cursor(int *);
extern void timedown(int *);
extern void timedown_play_select(int *);
extern void timedown3d(int *);			// 3d objects rather than fonts
extern void credit_msg(int *);
extern void plyr_credit_box(int *);
extern void name_entry(int);
extern void team_selection(void);
extern void team_stats_page(void);
extern void print_player_stats(int,int,int);
extern void print_four_player_stats(int,int,int);
extern void wait_for_button_press(int plyr_num, int min_ticks, int wait_ticks);
extern void pre_game_scrns(int *);
extern void team_select_cntrl(int *args);
extern void flash_yes_no(sprite_info_t *cur_obj, float flsh_x, float flsh_y, int pnum, short answer);
extern void trans_to_yn_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum, float y_dest);
extern void trans_to_initials_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum);
extern void trans_to_pin_nbr_plaq(sprite_info_t *p_plaq_obj, sprite_info_t *p_plaq_obj2, int pnum);
extern void fullgame_bought_msg(int *args);
extern void update_cursor_imgs(sprite_info_t *cur_obj, int level, int c_pos, int tid);
extern void change_img_tmu(sprite_info_t *obj, image_info_t *ii, int tid);
extern void update_tm_data(short c_pos, short tm_nbr, int flag);
extern int load_textures_into_tmu(tmu_ram_texture_t *tex_tbl);
extern void load_name_entry_textures(int *args);
extern void load_tmsel_uniform(int);
extern int	get_but_val_down(int);
extern int	get_but_val_cur(int);
extern int	get_joy_val_down(int);
extern int	get_joy_val_cur(int);
extern void stat_flash(int *args);
extern void flash_cursor(sprite_info_t *obj, int tid);
extern void bckgrnd_scroll(int *args);
extern void dim_text(int *args);
extern void create_and_anim_midway_logo(int min_time, int max_time);
extern void flash_obj_white(sprite_info_t *obj, int, int );		// int delay
extern void draw_backdrop(void);
extern void coaching_tip_scrn(void);
extern int buyin_scrn(void);
extern void draw_backgrnd2(float z, int tid);
extern void flash_credit_football(int *args);
extern void flash_rank_plq(int *args);
extern void plyr_plaques_awards (int *args);
extern void nbr3d_draw_function(void *oi);
extern void helmet_draw_function(void *oi);
extern void helmet_proc(int *args);
extern void num3d_proc (int *args);
extern int censor_players_name(char *name, int ec);
extern float print_players_name(int, float, float, char *, int);
extern void create_plates(void);
extern void kill_plates(void);
extern void transit_proc(int *);
extern void vs_screen(void);

extern void clear_pups(void);

#endif