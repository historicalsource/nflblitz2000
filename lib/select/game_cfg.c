//-------------------------------------------------------------------------------------------------
//	GAME_CFG.C - Blitz game configuration screen.  Tweaks game specifics for user.
//
//	James T. Anderson
//  Midway Games, Inc.
//  Copyright (c) 1999
//-------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <assert.h>

#include "/video/nfl/include/game.h"
#include "/video/nfl/include/id.h"			// Graphic IDs
#include "/video/nfl/include/select.h"		// Misc Japple Crap
#include "/video/nfl/include/sndcalls.h"	// Sound calls
#include "/video/nfl/include/fontid.h"		// Fonts
#include "/video/nfl/include/externs.h"
#include "/video/nfl/include/id.h"
#include "/video/nfl/include/select.h"
#include "/video/nfl/include/pup.h"
#include "/video/nfl/include/game_cfg.h"

// Graphics Headers
#include "/video/nfl/include/plaq.h"
#define IMAGEDEFINE 1
#include "/video/nfl/include/custgame.h"	// Customize Game Graphics


//-------------------------------------------------------------------------------------------------
//	External Variables
//-------------------------------------------------------------------------------------------------

extern char plyr_bnk_str[];
extern int force_selection;
extern float station_xys[MAX_PLYRS][2][2];
extern int user_base_adjust;
extern image_info_t gridbox;

extern int fog_color; 
extern float fog_thickness;

//-------------------------------------------------------------------------------------------------
// Definitions / Typedefs
//-------------------------------------------------------------------------------------------------
#define CONFIRM_DELAY	10

#define HELP_SID	900

#define ALPHA_MASK	0x00ffffff
#define RED_MASK	0xff00ffff
#define GREEN_MASK	0xffff00ff
#define BLUE_MASK	0xffffff00

#define YESNO_X	256.0f
#define YESNO_Y 175.0f

#define LEFT_COLOR	0xff6400ff
#define RIGHT_COLOR 0xffff0000

#define BUTTON_FLAGS	(TURBO_BUTTON | A_BUTTON | B_BUTTON)

#define MAX_ANSWERS	6

typedef	struct gf_info 
{
	image_info_t *header;					// Question Plaq
	image_info_t *answers[MAX_ANSWERS];		// Answer Images for Highlights and results
	image_info_t *ganswers[MAX_ANSWERS];		// Answer Images for Highlights and results
	image_info_t *m_answers[MAX_ANSWERS];	// Menu Answer Images for plaqs
	int num_sel;							// Number of Selections
	int def_sel;							// Default Selection (Zero Based)
	int question_xy[MAX_ANSWERS][2];		// Questions XY
	int result_index;						// Answer Result Index into cg_plaq_xys table
} gf_info_t;

//-------------------------------------------------------------------------------------------------
// Variables
//-------------------------------------------------------------------------------------------------
static tmu_ram_texture_t gamesel_textures[] = {
	{"cg00.wms",	JAPPLE_BOX_TID},
	{"cg01.wms",	JAPPLE_BOX_TID},
	{"cg02.wms",	JAPPLE_BOX_TID},
	{"cg03.wms",	JAPPLE_BOX_TID},
	{"tmsel01.wms",	TM_SELECT_TID},

//	{"cg04.wms", JAPPLE_BOX_TID},
//	{"cg05.wms", JAPPLE_BOX_TID},
//	{"cg06.wms", JAPPLE_BOX_TID},
//	{"cg07.wms", JAPPLE_BOX_TID},
	{NULL,0}
};

static float cg_plaq_xys[][2] = {	// [cat][x,y]
	{  77, 272 },
	{  77, 221 },
	{  77, 170 },
	{  77, 119 },

	{ 435, 272 },
	{ 435, 221 },
	{ 435, 170 },
	{ 435, 119 },
};

static image_info_t *categories[] = {
	&label_cpu,
	&label_playbook,
	&label_field,
	&label_fog,
	&label_skill,
	&label_audible,
	&label_camera
};


static gf_info_t f_cpu = {
	&cpu,
	{ &opt_on, &opt_off, &opt_tourn, NULL, NULL, NULL },
	{ &opt_ong, &opt_offg, &opt_tourng, NULL, NULL, NULL },
	{ &opt_on, &opt_off, &opt_tourn2, NULL, NULL, NULL },
	3,
	0,
	{ {256, 241}, {256, 223}, {256,205}, {0,0}, {0,0}, {0,0}},
	0,
};

static gf_info_t f_play = {
	&play,
	{ &opt_extra, &opt_default, &opt_nocust, NULL, NULL, NULL },
	{ &opt_extrag, &opt_defaultg, &opt_nocustg, NULL, NULL, NULL },
	{ &opt_extra, &opt_defaul2, &opt_nocust2, NULL, NULL, NULL },
	3,
	1,
	{ {256, 241}, {256, 223}, {256,205}, {0,0}, {0,0}, {0,0}},
	1,
};

static gf_info_t f_field = {
	&field,
	{ &opt_grass, &opt_astro, &opt_dirt, &opt_snow, &opt_asphult, &opt_mud },
	{ &opt_grassg, &opt_astrog, &opt_dirtg, &opt_snowg, &opt_asphultg, &opt_mudg },
	{ &opt_grass, &opt_astro, &opt_dirt, &opt_snow, &opt_asphult, &opt_mud },
	6,
	0,
	{ {256, 247}, {256, 230}, {256,213}, {256,196}, {256, 179}, {256,162} },
	2,
};

static gf_info_t f_fog = {
	&fog,
	{ &opt_none, &opt_normal, &opt_thick, &opt_night, &opt_smog, NULL },
	{ &opt_noneg, &opt_normalg, &opt_thickg, &opt_nightg, &opt_smogg, NULL },
	{ &opt_none, &opt_normal, &opt_thick, &opt_night, &opt_smog, NULL },
	5,
	0,
	{ {256, 241}, {256, 223}, {256,205}, {256,187}, {256, 169}, {0, 0}},
	3,
};

static gf_info_t f_skill = {
	&skill,
	{ &opt_easy, &opt_normal, &opt_hard, NULL, NULL, NULL },
	{ &opt_easyg, &opt_normalg, &opt_hardg, NULL, NULL, NULL },
	{ &opt_easy, &opt_normal, &opt_hard, NULL, NULL, NULL },
	3,
	1,
	{ {256, 241}, {256, 223}, {256,205}, {0,0}, {0,0}, {0,0} },
	4,
};

static gf_info_t f_audible = {
	&audible,
	{ &opt_hide, &opt_show, NULL, NULL, NULL, NULL },
	{ &opt_hideg, &opt_showg, NULL, NULL, NULL, NULL },
	{ &opt_hide2, &opt_show2, NULL, NULL, NULL, NULL },
	2,
	0,
	{ {256, 241}, {256, 223}, {0,0}, {0,0}, {0,0}, {0,0} },
	5,
};

static gf_info_t f_camera = {
	&camera,

	{ &opt_normal, &opt_wide, NULL, NULL, NULL, NULL },
	{ &opt_normalg, &opt_wideg, NULL, NULL, NULL, NULL },
	{ &opt_normal, &opt_wide, NULL, NULL, NULL, NULL },
	2,
	0,
	{ {256, 241}, {256, 223}, {0,0}, {0,0}, {0,0}, {0, 0} },
	6,
};

static gf_info_t *f_questions[] = {
	&f_cpu,
	&f_play,
	&f_field,
	&f_fog,
	&f_skill,
	&f_audible,
	&f_camera,
};

#define NUM_QUESTIONS	(sizeof(f_questions) / sizeof(gf_info_t *))
static int feature_values[NUM_QUESTIONS];
static int apply = 0;
static int was_low_res;
static float yscale;
static float vscale;
static float yoff;

//-------------------------------------------------------------------------------------------------
// External Functions
//-------------------------------------------------------------------------------------------------
extern void hide_string(void *);
extern void unhide_string(void *);
extern void title_flash(int *args);
extern void load_title_flash(int tex, int snd);
extern void free_title_flash(void);

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
static void load_custom_textures(void);
static int select_game_feature(gf_info_t *p_feat, int val);
static void update_feature_values(void);
static void flash_exit(int *args);
static void pulse_sprite(int *args);

//-------------------------------------------------------------------------------------------------
// game_config - Let user change various features of Blitz
//-------------------------------------------------------------------------------------------------
void game_config(void)
{
//	sprite_info_t *bkgrnd_obj;
	sprite_info_t *ps;
	sprite_info_t *cur_gfx;
	sprite_info_t *header;
	ostring_t *txt[3];
	float ay;
	int done;
	int i;
	int joy1, joy2, joy3, joy4;
	int btn1, btn2, btn3, btn4;
	int pjoy, joy, btn;
	int cnt;
	int cursor;

	was_low_res = is_low_res;
	yscale = (is_low_res ?  0.76f : 1.0f);
	vscale = SPRITE_VRES * yscale;
	yoff   = (is_low_res ? -20.0f : 0.0f);

	draw_enable(0);

//	wipeout();
	load_custom_textures();
	
	cursor = 0;

//	// draw background
//	bkgrnd_obj = beginobj(&teamsel,  		  0.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj = beginobj(&teamsel_c1,   	  0.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//	bkgrnd_obj->id = 1;
//
//	bkgrnd_obj = beginobj(&teamsel,  		 256.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->mode = FLIP_TEX_H;
//	generate_sprite_verts(bkgrnd_obj);
//
//	bkgrnd_obj = beginobj(&teamsel_c1,       256.0F, SPRITE_VRES, 500.0F, BKGRND2_TID);
//	bkgrnd_obj->id = 1;
//	bkgrnd_obj->mode = FLIP_TEX_H;
//	generate_sprite_verts(bkgrnd_obj);

	ps = beginobj(&title,    0, SPRITE_VRES, 300, JAPPLE_BOX_TID);
	ps->id = 1;
	ps = beginobj(&title_c1, 0, SPRITE_VRES, 300, JAPPLE_BOX_TID);
	ps->id = 1;

	ps = beginobj(&infobox,  0, SPRITE_VRES, 290, JAPPLE_BOX_TID);
	ps->id = 1;

	ps = beginobj(&blank,    0, SPRITE_VRES, 290, JAPPLE_BOX_TID);
	ps->id = 1;

	i = LEFT_COLOR;
	ps = beginobj(&gridbox,
			77,
			135,
			200,
			TM_SELECT_TID);
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	ps->state.constant_color = i;

	ps = beginobj(&gridbox,
			77,
			135 + 127,
			200,
			TM_SELECT_TID);
	ps->mode = FLIP_TEX_V;
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	ps->state.constant_color = i;
	generate_sprite_verts(ps);

	i = RIGHT_COLOR;
	ps = beginobj(&gridbox,
			435,
			135,
			200,
			TM_SELECT_TID);
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	ps->state.constant_color = i;

	ps = beginobj(&gridbox,
			435,
			135 + 127,
			200,
			TM_SELECT_TID);
	ps->mode = FLIP_TEX_V;
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	ps->state.constant_color = i;
	generate_sprite_verts(ps);

	is_low_res = 0;
	for (cnt=0; cnt < ( sizeof(cg_plaq_xys) / (sizeof(float)*2) ); cnt++)
	{
		ay = (float)(int)(cg_plaq_xys[cnt][1] * yscale);

		ps = beginobj(&bar, cg_plaq_xys[cnt][0], ay + yoff, 100, JAPPLE_BOX_TID);
		ps->id = 1;
	}
	
	for (cnt=0; cnt < ( sizeof(categories) / sizeof(image_info_t *) ); cnt++)
	{
		ay = categories[cnt]->ay;
		categories[cnt]->ay = (float)(int)(ay * yscale);

		ps = beginobj(categories[cnt], 0, vscale + yoff, 40, JAPPLE_BOX_TID);

		categories[cnt]->ay = ay;

		feature_values[cnt] = f_questions[cnt]->def_sel;
	}

	ay = (float)(int)(cg_plaq_xys[cursor][1] * yscale);
	cur_gfx = beginobj(&barglow, cg_plaq_xys[cursor][0], ay + yoff, 90, JAPPLE_BOX_TID);
	qcreate("pulse", TIMER_PID, pulse_sprite, (int)cur_gfx, 100, 40, -8);

	is_low_res = was_low_res;

	header = beginobj(f_questions[cursor]->header, 0, SPRITE_VRES, 200, JAPPLE_BOX_TID);
	header->id = 1;

	update_feature_values();

//	qcreate("crdbox2",CREDIT_PID,plyr_credit_box,1,station_xys[1][four_plr_ver][X_VAL],station_xys[1][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//	qcreate("crdbox1",CREDIT_PID,plyr_credit_box,0,station_xys[0][four_plr_ver][X_VAL],station_xys[0][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//	if (four_plr_ver)
//	{
//		qcreate("crdbox3",CREDIT_PID,plyr_credit_box,2,station_xys[2][four_plr_ver][X_VAL],station_xys[2][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//		qcreate("crdbox4",CREDIT_PID,plyr_credit_box,3,station_xys[3][four_plr_ver][X_VAL],station_xys[3][four_plr_ver][Y_VAL],JAPPLE_BOX_TID);
//	}
	qcreate("flash", TIMER_PID, flash_exit, 0, 0, 0, 0);

	// Print Help Text
	set_text_font(FONTID_BAST10);
	set_string_id(HELP_SID);
	set_text_position(256.0f, 241.0f, 10.0f);
	txt[0] = oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, "PRESS BUTTON TO MODIFY");
	set_text_position(256.0f, 223.0f, 10.0f);
	txt[1] = oprintf( "%dj%dc%s", (HJUST_CENTER|VJUST_CENTER), WHITE, "A GAME FEATURE");

	// Delete Blitz logo at top of screen
	del1c( OID_TMSEL_LOGO, -1 );

	sleep(1);

	draw_enable(1);

	pjoy = -1;

	done = 0;
	force_selection = 0;
	while (!done)
	{
		joy1 = (p_status & (1 << 0) ? get_joy_val_cur(0) : 0);
		joy2 = (p_status & (1 << 1) ? get_joy_val_cur(1) : 0);
		joy3 = (p_status & (1 << 2) ? get_joy_val_cur(2) : 0);
		joy4 = (p_status & (1 << 3) ? get_joy_val_cur(3) : 0);
		joy = joy1 | joy2 | joy3 | joy4;
		if (pjoy < 0)
			pjoy = joy;

		if (joy != pjoy)
		{
			pjoy = joy;
			i = cursor;
			if (joy & JOY_LEFT)
			{
				if (cursor > 3)
					cursor -= 4;
			}
			if (joy & JOY_RIGHT)
			{
				if (cursor < 4)
					cursor += 4;
			}
			if (joy & JOY_UP)
			{
				if ((cursor != 0) && (cursor != 4))
				{
					cursor -= 1;
				} else if (cursor == 0) {
					cursor = 7;
				} else if (cursor == 4) {
					cursor = 3;
				}
			}
			if (joy & JOY_DOWN)
			{

				if ((cursor != 3) && (cursor != 7))
				{
					cursor += 1;
				} else if (cursor == 3) {
					cursor = 4;
				} else if (cursor == 7) {
					cursor = 0;
				}
			}
			if (cursor != 7)
			{
				unhide_sprite(header);
				change_img_tmu(header, f_questions[cursor]->header, JAPPLE_BOX_TID);
			}
			else
			{
				hide_sprite(header);
			}
			is_low_res = 0;
			ay = (float)(int)(cg_plaq_xys[cursor][1] * yscale);
			cur_gfx->x = cg_plaq_xys[cursor][0];
			cur_gfx->y = ay + yoff;
			generate_sprite_verts(cur_gfx);
			is_low_res = was_low_res;

			if (i != cursor)
				snd_scall_bank(cursor_bnk_str, 40, VOLUME3, 127, PRIORITY_LVL3);
		}

		btn1 = (p_status & (1<<0) ? get_but_val_down(0) : 0);
		btn2 = (p_status & (1<<1) ? get_but_val_down(1) : 0);
		btn3 = (p_status & (1<<2) ? get_but_val_down(2) : 0);
		btn4 = (p_status & (1<<3) ? get_but_val_down(3) : 0);
		btn = btn1|btn2|btn3|btn4;
		if (btn & BUTTON_FLAGS) 
		{
			snd_scall_bank(cursor_bnk_str, 27, VOLUME3, 127, PRIORITY_LVL4);
			if (cursor == 7)
			{
				done = 1;
			} else {
				hide_string(txt[0]);
				hide_string(txt[1]);
				feature_values[cursor] = select_game_feature(f_questions[cursor], feature_values[cursor]);
				update_feature_values();
				unhide_string(txt[0]);
				unhide_string(txt[1]);
			}
		}
		if (force_selection)
			done = 1;
		sleep(1);
	}

	// Bring on the plates
	iqcreate("transit", TRANSIT_PID, transit_proc, PLATE_TRANSITION_TICKS, 0, 0, 0);
	// Wait for plates; set time-out so we don't hang
	i = PLATE_TRANSITION_TICKS * 2;
	// Do it this way so we can sync-up the sound
	while (plates_on != PLATES_ON && --i) sleep(1);
	// Do sound only if the transition finished, not the time-out
	if (i) snd_scall_bank(plyr_bnk_str, LOCK_SND, VOLUME4, 127, PRIORITY_LVL5);

	draw_enable(0);

	killall(TIMER_PID, -1);		// Kill timer and flashing exit procs
	del1c(20, -1);
	del1c(10, -1);
	del1c(1, -1);
	delete_multiple_strings(HELP_SID, -1);
	apply=1;

}

//-------------------------------------------------------------------------------------------------
// pulse_sprite - Alpha pulsating sprite
//-------------------------------------------------------------------------------------------------
static void pulse_sprite(int *args)
{
	sprite_info_t *ps;
	float fade_max;
	float fade_min;
	int alpha;
	int speed;

	ps = (sprite_info_t *)args[0];
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	fade_max = ((255 * args[1]) / 100);
	fade_min = ((255 * args[2]) / 100);

#if DEBUG
fprintf(stderr, "%f to %f\n", fade_max, fade_min);
fflush(stdout);
#endif
	speed = args[3];

	alpha =  fade_max;		// start with maximum transperancy
	while (1)
	{
		sleep(1);

		alpha += speed;
		if (alpha >= fade_max)
		{
			speed *= -1;
			alpha = fade_max;
		}

		if (alpha <= fade_min)
		{
			speed *= -1;
			alpha = fade_min;
		}
		ps->state.constant_color = ((ps->state.constant_color & 0xffffff) | (alpha <<24)); 
		is_low_res = 0;
		generate_sprite_verts(ps);
		is_low_res = was_low_res;
	}
}

//-------------------------------------------------------------------------------------------------
// flash_exit - Displays Exit sprite and flash the sucker red
//-------------------------------------------------------------------------------------------------
static void flash_exit(int *args)
{
	sprite_info_t *ps;
	float ay;
	int red, green, blue;
	int delay = 20;

	is_low_res = 0;
	ay = (float)(int)(cg_plaq_xys[7][1] * yscale);
	ps = beginobj(&opt_exit, cg_plaq_xys[7][0], ay + yoff, 10, JAPPLE_BOX_TID);
	ps->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
	ps->id = 1;
	is_low_res = was_low_res;

	while(1)
	{
		red = 255;
		blue = 0;
		green = 0;
		ps->state.constant_color = ((ps->state.constant_color & RED_MASK) | (red<<16));
		ps->state.constant_color = ((ps->state.constant_color & GREEN_MASK) | (green<<8));
		ps->state.constant_color = ((ps->state.constant_color & BLUE_MASK) | (blue<<0));
		is_low_res = 0;
		generate_sprite_verts(ps);
		is_low_res = was_low_res;
		sleep(delay);

		red = 255;
		blue = 255;
		green = 255;
		ps->state.constant_color = ((ps->state.constant_color & RED_MASK) | (red<<16));
		ps->state.constant_color = ((ps->state.constant_color & GREEN_MASK) | (green<<8));
		ps->state.constant_color = ((ps->state.constant_color & BLUE_MASK) | (blue<<0));
		is_low_res = 0;
		generate_sprite_verts(ps);
		is_low_res = was_low_res;
		sleep(delay);
	}
}

//-------------------------------------------------------------------------------------------------
// update_feature_values - Refreshes all feature selections
//-------------------------------------------------------------------------------------------------
static void update_feature_values(void)
{
	sprite_info_t * ps;
	gf_info_t * p_feat;
	float * p_xy;
	float ay;
	int val;
	int cnt;

	del1c(20, -1);

	is_low_res = 0;
	for (cnt = 0; cnt < NUM_QUESTIONS; cnt++)
	{
		p_feat = f_questions[cnt];
		val    = feature_values[cnt];
		p_xy   = cg_plaq_xys[p_feat->result_index];

		ay = p_xy[1] * yscale;

		ps = beginobj(p_feat->m_answers[val], p_xy[0], ay + yoff, 30.0f, JAPPLE_BOX_TID);
		ps->id = 20;
	} 
	is_low_res = was_low_res;
}

//-------------------------------------------------------------------------------------------------
// select_game_feature -  Allow user to modify a feature
//-------------------------------------------------------------------------------------------------
static int select_game_feature(gf_info_t *p_feat, int val)
{

	sprite_info_t *bg;
	sprite_info_t *cur_gfx;
	float ay;
	int joy1, joy2, joy3, joy4;
	int btn1, btn2, btn3, btn4;
	int joy, pjoy, btn, pbtn;
	int cursor;
	int done;
	int cnt;
	int cnst_clr, com_func, tcnt;

	cursor = val;
	done = 0;


	switch (p_feat->num_sel)
	{
		case 2:
			bg = beginobj(&menu2, 0.0f, SPRITE_VRES, 95.0f, JAPPLE_BOX_TID);
			bg->id = 10;
			break;
		case 3:
			bg = beginobj(&menu3, 0.0f, SPRITE_VRES, 95.0f, JAPPLE_BOX_TID);
			bg->id = 10;
			break;
		case 5:	
		default:
			bg = beginobj(&menu5, 0.0f, SPRITE_VRES, 95.0f, JAPPLE_BOX_TID);
			bg->id = 10;
			break;	
	}

	is_low_res = 0;

	// Load Highlighted Text Graphic
	ay = (float)(int)(p_feat->question_xy[cursor][1] * yscale);
	cur_gfx = beginobj(p_feat->answers[cursor],
				p_feat->question_xy[cursor][0],
				ay + yoff,
				80,
				JAPPLE_BOX_TID);
	cur_gfx->id = 10;

	// Load Greyed Text Graphics
	for (cnt=0; cnt < p_feat->num_sel; cnt++)
	{
		ay = (float)(int)(p_feat->question_xy[cnt][1] * yscale);
		bg = beginobj(p_feat->ganswers[cnt],
				p_feat->question_xy[cnt][0],
				ay + yoff,
				90,
				JAPPLE_BOX_TID);
		bg->id = 10;

	}

	is_low_res = was_low_res;

	pjoy = -1;
	pbtn = -1;

	// save obj. state
	cnst_clr = cur_gfx->state.constant_color;
	com_func = cur_gfx->state.color_combiner_function;
	tcnt = 0;

	while (!done)
	{
		// Grab Joystick Values
		joy1 = (p_status & (1 << 0) ? get_joy_val_cur(0) : 0);
		joy2 = (p_status & (1 << 1) ? get_joy_val_cur(1) : 0);
		joy3 = (p_status & (1 << 2) ? get_joy_val_cur(2) : 0);
		joy4 = (p_status & (1 << 3) ? get_joy_val_cur(3) : 0);
		joy = joy1 | joy2 | joy3 | joy4;
		if (pjoy < 0)
			pjoy = joy;

		if (joy != pjoy)
		{
			pjoy = cursor;

			if ((joy & JOY_UP) && (cursor > 0))
				cursor -= 1;
			if ((joy & JOY_DOWN) && (cursor < (p_feat->num_sel-1)))
				cursor += 1;

			if (pjoy != cursor)
			{
				snd_scall_bank(cursor_bnk_str, 41, VOLUME3, 127, PRIORITY_LVL3);

				change_img_tmu(cur_gfx, p_feat->answers[cursor],JAPPLE_BOX_TID);

				is_low_res = 0;
				ay = (float)(int)(p_feat->question_xy[cursor][1] * yscale);
				cur_gfx->x = p_feat->question_xy[cursor][0];
				cur_gfx->y = ay + yoff;
				generate_sprite_verts(cur_gfx);
				is_low_res = was_low_res;
			}
			pjoy = joy;
		}

		// Grab Btn Values
		btn1 = (p_status & (1<<0) ? get_but_val_down(0) : 0);
		btn2 = (p_status & (1<<1) ? get_but_val_down(1) : 0);
		btn3 = (p_status & (1<<2) ? get_but_val_down(2) : 0);
		btn4 = (p_status & (1<<3) ? get_but_val_down(3) : 0);
		btn = btn1|btn2|btn3|btn4;
		if (pbtn < 0)
			pbtn = btn;

		if (btn != pbtn)
		{
			pbtn = btn;
			if (btn & BUTTON_FLAGS)
			{
				snd_scall_bank(cursor_bnk_str, 22, VOLUME3, 127, PRIORITY_LVL4);
				done = 1;
			}
		}

		if (tcnt >= (10+5))
			tcnt = 0;

		if (tcnt < (10))
		{
			// Change obj back
			cur_gfx->state.constant_color = cnst_clr;
			cur_gfx->state.color_combiner_function = com_func;
		}
		else
		if (tcnt < (10+5))
		{
			// Change obj color
			cur_gfx->state.constant_color = LT_RED;
			cur_gfx->state.color_combiner_function = GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB;
		}

		sleep(1);

		tcnt++;
		if (force_selection)
			done = 1;
	}
	del1c(10, -1);
	return(cursor);
}

//-------------------------------------------------------------------------------------------------
// game_config_confirm - Ask user if they want to customize game after team selection
//-------------------------------------------------------------------------------------------------
int game_config_confirm(void)
{
	static tmu_ram_texture_t gcc_texture_tbl[] = {
		{"cg_ts.wms",   TM_SELECT_TID },
		{"gs_fl00.wms", JAPPLE_BOX_TID },
		{"gs_fl01.wms", JAPPLE_BOX_TID },
		{NULL, 0},
	};
	sprite_info_t *cur_gfx;
	sprite_info_t *ps;
	sprite_info_t *palpha;
	int joy, btn;
	int cursor;
	int done;
	int alpha = 0;
	int alpha_speed = 4;
	int cnt;


	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info((void **)&cur_proc, suspend_self, resume_process);
#else
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

	if (load_textures_into_tmu(gcc_texture_tbl))
	{
		fprintf(stderr, "Couldn't load all <game_config_confirm> textures into tmu\r\n");
#ifdef DEBUG
		lockup();
#endif
	}

	// Disable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

	cursor = 0;
	done = 0;

	palpha = beginobj(&cg_alpha, 0.0f, SPRITE_VRES, 1.4f, TM_SELECT_TID);
	palpha->w_scale = 32.0f;
	palpha->h_scale = 6.0f;
	palpha->id = 2;
	palpha->state.constant_color = ((palpha->state.constant_color & 0xffffff) | (alpha <<24)); 
	generate_sprite_verts(palpha);

	ps = beginobj(&yesno, 0.0f, SPRITE_VRES, 1.3f, TM_SELECT_TID);
	ps->id = 2;

	ps = beginobj(&cg_yn, YESNO_X, YESNO_Y, 1.2f, TM_SELECT_TID);
	ps->id = 2;

	cur_gfx = beginobj(&cg_no, YESNO_X, YESNO_Y, 1.1f, TM_SELECT_TID);
	cur_gfx->id = 2;

	load_title_flash(TRUE, FALSE);

	snd_scall_bank(cursor_bnk_str, 22, VOLUME3, 127, PRIORITY_LVL4);

	cnt = 0;
	while(!done)
	{
		if (cnt < 40)
		{
			cnt += 1;
			alpha += alpha_speed;
			palpha->state.constant_color = ((palpha->state.constant_color & 0xffffff) | (alpha <<24)); 
			generate_sprite_verts(palpha);
		}
		joy  = (p_status & (1 << 0) ? get_joy_val_down(0) : 0);
		joy |= (p_status & (1 << 1) ? get_joy_val_down(1) : 0);
		joy |= (p_status & (1 << 2) ? get_joy_val_down(2) : 0);
		joy |= (p_status & (1 << 3) ? get_joy_val_down(3) : 0);

		if (((joy & JOY_UP)   && (cursor != 1)) ||
			((joy & JOY_DOWN) && (cursor != 0)))
		{
			cursor ^= 1;
			change_img_tmu(cur_gfx, (cursor ? &cg_yes : &cg_no), TM_SELECT_TID);
			snd_scall_bank(cursor_bnk_str, 44, VOLUME2, 127, PRIORITY_LVL4);
		}

		btn  = (p_status & (1<<0) ? get_but_val_down(0) : 0);
		btn |= (p_status & (1<<1) ? get_but_val_down(1) : 0);
		btn |= (p_status & (1<<2) ? get_but_val_down(2) : 0);
		btn |= (p_status & (1<<3) ? get_but_val_down(3) : 0);

		if (cnt > CONFIRM_DELAY)
		{
			if ((btn) || (force_selection))
			{
				if (force_selection)
					cursor = 0;
				done = 1;
				change_img_tmu(cur_gfx, (cursor ? &cg_yes : &cg_no), TM_SELECT_TID);

			}
		}
		sleep(1);
	}
	apply = cursor;

	if (cursor)
	{
		iqcreate("flash", 0x8000, title_flash, 256, YESNO_Y+10, 200, 0);		
	} else {
		iqcreate("flash", 0x8000, title_flash, 256, YESNO_Y-30, 200, 0);
	}
	sleep(40);

	// Delete objects if YES
	if (cursor == 1)
	{
		del1c( OID_CHLNGR_BOX, ~3 );				// delete all challenger boxes
		del1c( 2, -1);
	}
	free_title_flash();
	return cursor;
}


//-------------------------------------------------------------------------------------------------
// load_custom_textures - Load all texture pages associate with this screen.
//-------------------------------------------------------------------------------------------------
static void load_custom_textures(void)
{
	// Enable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

//	imgs_loaded = 0;

	if (load_textures_into_tmu(gamesel_textures))
	{
		fprintf(stderr, "Couldn't load all sweeps textures into tmu\r\n");
#ifdef DEBUG
		lockup();
#else
		return;
#endif
	}
//	imgs_loaded = 1;

	// Disable the disk interrupt callback
#if defined(SEATTLE)
	install_disk_info(0, 0, 0);
#else
	dio_mode(DIO_MODE_BLOCK);
#endif

	lock_multiple_textures(JAPPLE_BOX_TID,0xFFFFFFFF);
	lock_multiple_textures(TM_SELECT_TID,0xFFFFFFFF);

}

//-------------------------------------------------------------------------------------------------
// user_config - Query if the user has gone through the configuration / custimization screen
//-------------------------------------------------------------------------------------------------
int user_config(void)
{
	return apply;
}

//-------------------------------------------------------------------------------------------------
// clear_user_config - clear active status
//-------------------------------------------------------------------------------------------------
void clear_user_config(void)
{
	apply = 0;
	user_base_adjust = 0;		// Clear Handicap	
}

//-------------------------------------------------------------------------------------------------
// apply_user_config - Applies user specified custimzation flags
//-------------------------------------------------------------------------------------------------
void apply_user_config(int key)
{
	int skill_adjust[3] = { -3, 0, 3 };
	int value;

	value = feature_values[key-1];

	if (apply)
	{
		switch (key)
		{
			case GC_AUDIBLE:
				pup_showaudibles = value;
				break;
			case GC_FIELD:
				pup_field = value+1;	// start at NEW GRASS
				break;
			case GC_FOG:
				switch (value)
				{
					case 0:		// None
						pup_thickfog = 0;
						pup_fog = 0;
						fog_color = 0;
						fog_thickness = 0;
						break;
					case 1:		// Normal
						pup_thickfog = 0;
						pup_fog = 1;
						fog_color = 0x00a0a0a0;
						fog_thickness = 1250.0f;
						break;
					case 2:		// Thick
						pup_thickfog = 1;
						pup_fog = 0;
						fog_color = 0x00a0a0a0;
						fog_thickness = 500.0f;
						break;
					case 3:		// Night
						pup_thickfog = 0;
						pup_fog = 1;
						fog_color = 0x00000000;
						fog_thickness = 700.0f;
						break;
					case 4:		// Smog
						pup_thickfog = 0;
						pup_fog = 1;
						fog_color = 0x00673C00;		// Shit Brown
						fog_thickness = 700.0f;		// Normal Thickness
						break;
				}				
				break;
			case GC_CAMERA:
				pup_pullback = (value ? 3 : 0);
				break;
			case GC_CPU:
				if (value == 0)
				{
					pup_tourney = 0;
					pup_noassist = 0;
				} else if (value == 1) {
					pup_tourney = 0;
					pup_noassist = 1;
				} else {
					pup_noassist = 1;
					pup_tourney = 1;
				}			
				break;
			case GC_SKILL:
				user_base_adjust = skill_adjust[value];
				break;
			case GC_PLAYBOOK:
				pup_playbook = value;
				break;
		}
	}
}

//--------------------------------------------------------------------------------------------[EOF]
