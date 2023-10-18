#ifndef _COIN__H
#define _COIN__H

/*****************************************************************************/
/*                                                                           */
/* FILE: COIN.H                                                              */
/*                                                                           */
/* Space Race                                                                */
/*                                                                           */
/* Defines and function prototypes for coin functions.                       */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

/***** DEFINES ***************************************************************/

#define COIN_NUM_SLOTS 5

#define PRICE_LEAD_STR_SIZE 8       /* pricing leading monetary symbol str */
#define PRICE_TRAIL_STR_SIZE 8      /* pricing trailing monetary symbol str */
#define PRICE_INFO_STR_SIZE 80      /* pricing string e.g. "1 credit / 1 coin" */

/***** STRUCTURE DEFINITIONS *************************************************/


typedef struct coin_credits {
	int credits;
	int service_credits;
	int coin_units;
	int bonus_units;
	int remainder_units;
	} coin_credits_t;

typedef struct coin_totalizer {
	int mult [COIN_NUM_SLOTS];			/* how many clicks per coin */
	int output [COIN_NUM_SLOTS];		/* which meter to click */
	int count [COIN_NUM_SLOTS];			/* how many clicks remain to be done */
	int use_multipliers;			    /* whether or not to use mult field */
	int mode;							/* clicker or totalizer */
	} coin_totalizer_t;

typedef struct coin_state {
	int slam;		 					/* slam happened and is in debounce period */
    int message_type;                   /* how much coin message info displayed */
    int free_play;                      /* 0 = no, 1 = free play */    
	int show_volume;					/* 0 = no, N > 0 == show for N ticks */
	int sound_holdoff;					/* 1 = no sound calls, 0 = OK to make snd */
	} coin_state_t;

enum {COIN_MESSAGE_SHORT, COIN_MESSAGE_LONG};

/* this pricing info will be set by the pricing mode */
/* stored in the CMOS adjustments - this is our local copy */

typedef struct coin_pricing {
	int units [COIN_NUM_SLOTS];			/* num units each slot gives per coin */
	int units_per_credit;				/* num units needed for a credit */
	int units_per_bonus;				/* num units before bonus credit given */
	int minimum_units;					/* minimum units to get any credits */
	int credits_to_start;				/* num credits needed to start */
	int credits_to_continue;			/* num units needed to continue */
	int maximum_credits;				/* max credits allowed to accumulate */
	int coins_per_bill;					/* used to calculate money strings */
	int show_credit_fractions;			/* y/n show fractions as they add up */
    char lead_str [PRICE_LEAD_STR_SIZE];    /* e.g. "$" or "L" or "Y" */
    char trail_str [PRICE_LEAD_STR_SIZE];   /* e.g. "DM" or "FR" */
    char info_str [PRICE_INFO_STR_SIZE];    /* e.g. "1 credit / 1 coin" */
	} coin_pricing_t;

/***** FUNCTION PROTOTYPES ***************************************************/

extern int	get_credits_to_continue(void);
extern int	get_credits_to_start(void);
extern int	get_full_game_credits(void);
extern int	check_credits_to_continue(void);
extern int	get_total_credits(void);
extern void decrement_credits(short dec_cnt);


/* setup and init functions */
extern void coin_init_all (void);
extern void coin_init_totalizer (void);
extern void coin_init_pricing (void);
extern void coin_game_over (void);
extern void coin_init_state (void);
extern void coin_init_units (void);

/* deal with actual coin events */
extern void coin_svc_cred_handler (int sw_state, int sw_id);
extern void coin_slam_handler (int sw_state, int sw_id);
extern void coin_slam_proc (int *arg);
extern void coin_sw_handler (int sw_state, int sw_id);
extern void coin_control (int sw_state, int sw_id);
extern void coin_strobe_clicker (unsigned int mask);
extern void coin_clicker_proc (int *args);

/* deal with start button */
extern void coin_start_handler (int sw_state, int sw_id);

/* coin drop sounds */
extern void coin_sound_proc (int *args);

/* check coin state */
extern int coin_check_freeplay (void);
extern int check_credits_to_start (void);
extern int coin_check_units (void);
extern void coin_change_mesg_state (int state);

/* show "insert coins", "press start", etc. */
extern void coin_show_insert_proc (int *args);
extern int coin_show_insert_sleep (int sleep_time);

/* show player credits and pricing info */
extern void coin_show_credits_proc (int *args);
extern int coin_show_credits_sleep (int sleep_time);

/* volume adjustment */
extern void coin_volume_handler (int sw_state, int sw_id);
extern void coin_volume_proc (int *args);

extern void coin_sound_holdoff (int flag);

/***** END of COIN.H *********************************************************/

#endif