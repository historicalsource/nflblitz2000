#include	<goose/goose.h>

#define	IMAGEDEFINE	1
#include	"include\sysfont.h"
#include	"include\bigfnt.h"

static image_info_t	*bast7t_font_table[] = {
&bast7texc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast7tnum,				/* # - 0x23 */
&bast7tdol,				/* $ - 0x24 */
&bast7tpct,				/* % - 0x25 */
&bast7tand,				/* & - 0x26 */
&bast7tapo,				/* ' - 0x27 */
&bast7tlpr,				/* ( - 0x28 */
&bast7trpr,				/* ) - 0x29 */
(void *)0,				/* * - 0x2a */
&bast7tpls,				/* + - 0x2b */
&bast7tcom,				/* , - 0x2c */
&bast7tdsh,				/* - - 0x2d */
&bast7tper,				/* . - 0x2e */
&bast7tsls,				/* / - 0x2f */
&bast7t_0,				/* 0 - 0x30 */
&bast7t_1,				/* 1 - 0x31 */
&bast7t_2,				/* 2 - 0x32 */
&bast7t_3,				/* 3 - 0x33 */
&bast7t_4,				/* 4 - 0x34 */
&bast7t_5,				/* 5 - 0x35 */
&bast7t_6,				/* 6 - 0x36 */
&bast7t_7,				/* 7 - 0x37 */
&bast7t_8,				/* 8 - 0x38 */
&bast7t_9,				/* 9 - 0x39 */
&bast7tcol,				/* : - 0x3a */
&bast7tsem,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast7tque,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast7t_a,				/* A - 0x41 */
&bast7t_b,				/* B - 0x42 */
&bast7t_c,				/* C - 0x43 */
&bast7t_d,				/* D - 0x44 */
&bast7t_e,				/* E - 0x45 */
&bast7t_f,				/* F - 0x46 */
&bast7t_g,				/* G - 0x47 */
&bast7t_h,				/* H - 0x48 */
&bast7t_i,				/* I - 0x49 */
&bast7t_j,				/* J - 0x4a */
&bast7t_k,				/* K - 0x4b */
&bast7t_l,				/* L - 0x4c */
&bast7t_m,				/* M - 0x4d */
&bast7t_n,				/* N - 0x4e */
&bast7t_o,				/* O - 0x4f */
&bast7t_p,				/* P - 0x50 */
&bast7t_q,				/* Q - 0x51 */
&bast7t_r,				/* R - 0x52 */
&bast7t_s,				/* S - 0x53 */
&bast7t_t,				/* T - 0x54 */
&bast7t_u,				/* U - 0x55 */
&bast7t_v,				/* V - 0x56 */
&bast7t_w,				/* W - 0x57 */
&bast7t_x,				/* X - 0x58 */
&bast7t_y,				/* Y - 0x59 */
&bast7t_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast7t_a,				/* a - 0x61 */
&bast7t_b,				/* b - 0x62 */
&bast7t_c,				/* c - 0x63 */
&bast7t_d,				/* d - 0x64 */
&bast7t_e,				/* e - 0x65 */
&bast7t_f,				/* f - 0x66 */
&bast7t_g,				/* g - 0x67 */
&bast7t_h,				/* h - 0x68 */
&bast7t_i,				/* i - 0x69 */
&bast7t_j,				/* j - 0x6a */
&bast7t_k,				/* k - 0x6b */
&bast7t_l,				/* l - 0x6c */
&bast7t_m,				/* m - 0x6d */
&bast7t_n,				/* n - 0x6e */
&bast7t_o,				/* o - 0x6f */
&bast7t_p,				/* p - 0x70 */
&bast7t_q,				/* q - 0x71 */
&bast7t_r,				/* r - 0x72 */
&bast7t_s,				/* s - 0x73 */
&bast7t_t,				/* t - 0x74 */
&bast7t_u,				/* u - 0x75 */
&bast7t_v,				/* v - 0x76 */
&bast7t_w,				/* w - 0x77 */
&bast7t_x,				/* x - 0x78 */
&bast7t_y,				/* y - 0x79 */
&bast7t_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};

static image_info_t	*bast8t_font_table[] = {
&bast8texc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast8tnum,				/* # - 0x23 */
&bast8tdol,				/* $ - 0x24 */
&bast8tpct,				/* % - 0x25 */
&bast8tand,				/* & - 0x26 */
&bast8tapo,				/* ' - 0x27 */
&bast8tprl,				/* ( - 0x28 */
&bast8tprr,				/* ) - 0x29 */
&bast8tast,				/* * - 0x2a */
&bast8tpls,				/* + - 0x2b */
&bast8tcom,				/* , - 0x2c */
&bast8tdas,				/* - - 0x2d */
&bast8tper,				/* . - 0x2e */
&bast8tsls,				/* / - 0x2f */
&bast8t_0,				/* 0 - 0x30 */
&bast8t_1,				/* 1 - 0x31 */
&bast8t_2,				/* 2 - 0x32 */
&bast8t_3,				/* 3 - 0x33 */
&bast8t_4,				/* 4 - 0x34 */
&bast8t_5,				/* 5 - 0x35 */
&bast8t_6,				/* 6 - 0x36 */
&bast8t_7,				/* 7 - 0x37 */
&bast8t_8,				/* 8 - 0x38 */
&bast8t_9,				/* 9 - 0x39 */
&bast8tcol,				/* : - 0x3a */
&bast8tsem,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast8tque,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast8t_a,				/* A - 0x41 */
&bast8t_b,				/* B - 0x42 */
&bast8t_c,				/* C - 0x43 */
&bast8t_d,				/* D - 0x44 */
&bast8t_e,				/* E - 0x45 */
&bast8t_f,				/* F - 0x46 */
&bast8t_g,				/* G - 0x47 */
&bast8t_h,				/* H - 0x48 */
&bast8t_i,				/* I - 0x49 */
&bast8t_j,				/* J - 0x4a */
&bast8t_k,				/* K - 0x4b */
&bast8t_l,				/* L - 0x4c */
&bast8t_m,				/* M - 0x4d */
&bast8t_n,				/* N - 0x4e */
&bast8t_o,				/* O - 0x4f */
&bast8t_p,				/* P - 0x50 */
&bast8t_q,				/* Q - 0x51 */
&bast8t_r,				/* R - 0x52 */
&bast8t_s,				/* S - 0x53 */
&bast8t_t,				/* T - 0x54 */
&bast8t_u,				/* U - 0x55 */
&bast8t_v,				/* V - 0x56 */
&bast8t_w,				/* W - 0x57 */
&bast8t_x,				/* X - 0x58 */
&bast8t_y,				/* Y - 0x59 */
&bast8t_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast8t_a,				/* a - 0x61 */
&bast8t_b,				/* b - 0x62 */
&bast8t_c,				/* c - 0x63 */
&bast8t_d,				/* d - 0x64 */
&bast8t_e,				/* e - 0x65 */
&bast8t_f,				/* f - 0x66 */
&bast8t_g,				/* g - 0x67 */
&bast8t_h,				/* h - 0x68 */
&bast8t_i,				/* i - 0x69 */
&bast8t_j,				/* j - 0x6a */
&bast8t_k,				/* k - 0x6b */
&bast8t_l,				/* l - 0x6c */
&bast8t_m,				/* m - 0x6d */
&bast8t_n,				/* n - 0x6e */
&bast8t_o,				/* o - 0x6f */
&bast8t_p,				/* p - 0x70 */
&bast8t_q,				/* q - 0x71 */
&bast8t_r,				/* r - 0x72 */
&bast8t_s,				/* s - 0x73 */
&bast8t_t,				/* t - 0x74 */
&bast8t_u,				/* u - 0x75 */
&bast8t_v,				/* v - 0x76 */
&bast8t_w,				/* w - 0x77 */
&bast8t_x,				/* x - 0x78 */
&bast8t_y,				/* y - 0x79 */
&bast8t_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};

static image_info_t	*bast10_font_table[] = {
&bast10exc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast10num,				/* # - 0x23 */
&bast10dol,				/* $ - 0x24 */
&bast10pct,				/* % - 0x25 */
&bast10and,				/* & - 0x26 */
&bast10apo,				/* ' - 0x27 */
&bast10prl,				/* ( - 0x28 */
&bast10prr,				/* ) - 0x29 */
&bast10ast,				/* * - 0x2a */
&bast10pls,				/* + - 0x2b */
&bast10com,				/* , - 0x2c */
&bast10dsh,				/* - - 0x2d */
&bast10per,				/* . - 0x2e */
&bast10sls,				/* / - 0x2f */
&bast10_0,				/* 0 - 0x30 */
&bast10_1,				/* 1 - 0x31 */
&bast10_2,				/* 2 - 0x32 */
&bast10_3,				/* 3 - 0x33 */
&bast10_4,				/* 4 - 0x34 */
&bast10_5,				/* 5 - 0x35 */
&bast10_6,				/* 6 - 0x36 */
&bast10_7,				/* 7 - 0x37 */
&bast10_8,				/* 8 - 0x38 */
&bast10_9,				/* 9 - 0x39 */
&bast10col,				/* : - 0x3a */
&bast10sem,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast10que,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast10_a,				/* A - 0x41 */
&bast10_b,				/* B - 0x42 */
&bast10_c,				/* C - 0x43 */
&bast10_d,				/* D - 0x44 */
&bast10_e,				/* E - 0x45 */
&bast10_f,				/* F - 0x46 */
&bast10_g,				/* G - 0x47 */
&bast10_h,				/* H - 0x48 */
&bast10_i,				/* I - 0x49 */
&bast10_j,				/* J - 0x4a */
&bast10_k,				/* K - 0x4b */
&bast10_l,				/* L - 0x4c */
&bast10_m,				/* M - 0x4d */
&bast10_n,				/* N - 0x4e */
&bast10_o,				/* O - 0x4f */
&bast10_p,				/* P - 0x50 */
&bast10_q,				/* Q - 0x51 */
&bast10_r,				/* R - 0x52 */
&bast10_s,				/* S - 0x53 */
&bast10_t,				/* T - 0x54 */
&bast10_u,				/* U - 0x55 */
&bast10_v,				/* V - 0x56 */
&bast10_w,				/* W - 0x57 */
&bast10_x,				/* X - 0x58 */
&bast10_y,				/* Y - 0x59 */
&bast10_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast10_a,				/* a - 0x61 */
&bast10_b,				/* b - 0x62 */
&bast10_c,				/* c - 0x63 */
&bast10_d,				/* d - 0x64 */
&bast10_e,				/* e - 0x65 */
&bast10_f,				/* f - 0x66 */
&bast10_g,				/* g - 0x67 */
&bast10_h,				/* h - 0x68 */
&bast10_i,				/* i - 0x69 */
&bast10_j,				/* j - 0x6a */
&bast10_k,				/* k - 0x6b */
&bast10_l,				/* l - 0x6c */
&bast10_m,				/* m - 0x6d */
&bast10_n,				/* n - 0x6e */
&bast10_o,				/* o - 0x6f */
&bast10_p,				/* p - 0x70 */
&bast10_q,				/* q - 0x71 */
&bast10_r,				/* r - 0x72 */
&bast10_s,				/* s - 0x73 */
&bast10_t,				/* t - 0x74 */
&bast10_u,				/* u - 0x75 */
&bast10_v,				/* v - 0x76 */
&bast10_w,				/* w - 0x77 */
&bast10_x,				/* x - 0x78 */
&bast10_y,				/* y - 0x79 */
&bast10_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};

static image_info_t	*bast13_font_table[] = {
&bast13exc,				/* ! - 0x21 */
&bast13quo,				/* " - 0x22 */
&bast13num,				/* # - 0x23 */
&bast13dol,				/* $ - 0x24 */
&bast13pct,				/* % - 0x25 */
&bast13and,				/* & - 0x26 */
&bast13apo,				/* ' - 0x27 */
&bast13prl,				/* ( - 0x28 */
&bast13prr,				/* ) - 0x29 */
&bast13ast,				/* * - 0x2a */
&bast13add,				/* + - 0x2b */
&bast13com,				/* , - 0x2c */
&bast13das,				/* - - 0x2d */
&bast13per,				/* . - 0x2e */
&bast13sls,				/* / - 0x2f */
&bast13_0,				/* 0 - 0x30 */
&bast13_1,				/* 1 - 0x31 */
&bast13_2,				/* 2 - 0x32 */
&bast13_3,				/* 3 - 0x33 */
&bast13_4,				/* 4 - 0x34 */
&bast13_5,				/* 5 - 0x35 */
&bast13_6,				/* 6 - 0x36 */
&bast13_7,				/* 7 - 0x37 */
&bast13_8,				/* 8 - 0x38 */
&bast13_9,				/* 9 - 0x39 */
&bast13col,				/* : - 0x3a */
(void *)0,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast13que,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast13_a,				/* A - 0x41 */
&bast13_b,				/* B - 0x42 */
&bast13_c,				/* C - 0x43 */
&bast13_d,				/* D - 0x44 */
&bast13_e,				/* E - 0x45 */
&bast13_f,				/* F - 0x46 */
&bast13_g,				/* G - 0x47 */
&bast13_h,				/* H - 0x48 */
&bast13_i,				/* I - 0x49 */
&bast13_j,				/* J - 0x4a */
&bast13_k,				/* K - 0x4b */
&bast13_l,				/* L - 0x4c */
&bast13_m,				/* M - 0x4d */
&bast13_n,				/* N - 0x4e */
&bast13_o,				/* O - 0x4f */
&bast13_p,				/* P - 0x50 */
&bast13_q,				/* Q - 0x51 */
&bast13_r,				/* R - 0x52 */
&bast13_s,				/* S - 0x53 */
&bast13_t,				/* T - 0x54 */
&bast13_u,				/* U - 0x55 */
&bast13_v,				/* V - 0x56 */
&bast13_w,				/* W - 0x57 */
&bast13_x,				/* X - 0x58 */
&bast13_y,				/* Y - 0x59 */
&bast13_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast13_a,				/* a - 0x61 */
&bast13_b,				/* b - 0x62 */
&bast13_c,				/* c - 0x63 */
&bast13_d,				/* d - 0x64 */
&bast13_e,				/* e - 0x65 */
&bast13_f,				/* f - 0x66 */
&bast13_g,				/* g - 0x67 */
&bast13_h,				/* h - 0x68 */
&bast13_i,				/* i - 0x69 */
&bast13_j,				/* j - 0x6a */
&bast13_k,				/* k - 0x6b */
&bast13_l,				/* l - 0x6c */
&bast13_m,				/* m - 0x6d */
&bast13_n,				/* n - 0x6e */
&bast13_o,				/* o - 0x6f */
&bast13_p,				/* p - 0x70 */
&bast13_q,				/* q - 0x71 */
&bast13_r,				/* r - 0x72 */
&bast13_s,				/* s - 0x73 */
&bast13_t,				/* t - 0x74 */
&bast13_u,				/* u - 0x75 */
&bast13_v,				/* v - 0x76 */
&bast13_w,				/* w - 0x77 */
&bast13_x,				/* x - 0x78 */
&bast13_y,				/* y - 0x79 */
&bast13_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};

#if 0
static image_info_t	*bast13_font_table[] = {
(void *)0,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
(void *)0,				/* # - 0x23 */
(void *)0,				/* $ - 0x24 */
(void *)0,				/* % - 0x25 */
(void *)0,				/* & - 0x26 */
(void *)0,				/* ' - 0x27 */
(void *)0,				/* ( - 0x28 */
(void *)0,				/* ) - 0x29 */
(void *)0,				/* * - 0x2a */
(void *)0,				/* + - 0x2b */
(void *)0,				/* , - 0x2c */
(void *)0,				/* - - 0x2d */
(void *)0,				/* . - 0x2e */
(void *)0,				/* / - 0x2f */
&bast13_0,				/* 0 - 0x30 */
&bast13_1,				/* 1 - 0x31 */
&bast13_2,				/* 2 - 0x32 */
&bast13_3,				/* 3 - 0x33 */
&bast13_4,				/* 4 - 0x34 */
&bast13_5,				/* 5 - 0x35 */
&bast13_6,				/* 6 - 0x36 */
&bast13_7,				/* 7 - 0x37 */
&bast13_8,				/* 8 - 0x38 */
&bast13_9,				/* 9 - 0x39 */
&bast13col,				/* : - 0x3a */
(void *)0,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
(void *)0,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast13_a,				/* A - 0x41 */
&bast13_b,				/* B - 0x42 */
&bast13_c,				/* C - 0x43 */
&bast13_d,				/* D - 0x44 */
&bast13_e,				/* E - 0x45 */
&bast13_f,				/* F - 0x46 */
&bast13_g,				/* G - 0x47 */
&bast13_h,				/* H - 0x48 */
&bast13_i,				/* I - 0x49 */
&bast13_j,				/* J - 0x4a */
&bast13_k,				/* K - 0x4b */
&bast13_l,				/* L - 0x4c */
&bast13_m,				/* M - 0x4d */
&bast13_n,				/* N - 0x4e */
&bast13_o,				/* O - 0x4f */
&bast13_p,				/* P - 0x50 */
&bast13_q,				/* Q - 0x51 */
&bast13_r,				/* R - 0x52 */
&bast13_s,				/* S - 0x53 */
&bast13_t,				/* T - 0x54 */
&bast13_u,				/* U - 0x55 */
&bast13_v,				/* V - 0x56 */
&bast13_w,				/* W - 0x57 */
&bast13_x,				/* X - 0x58 */
&bast13_y,				/* Y - 0x59 */
&bast13_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast13_a,				/* a - 0x61 */
&bast13_b,				/* b - 0x62 */
&bast13_c,				/* c - 0x63 */
&bast13_d,				/* d - 0x64 */
&bast13_e,				/* e - 0x65 */
&bast13_f,				/* f - 0x66 */
&bast13_g,				/* g - 0x67 */
&bast13_h,				/* h - 0x68 */
&bast13_i,				/* i - 0x69 */
&bast13_j,				/* j - 0x6a */
&bast13_k,				/* k - 0x6b */
&bast13_l,				/* l - 0x6c */
&bast13_m,				/* m - 0x6d */
&bast13_n,				/* n - 0x6e */
&bast13_o,				/* o - 0x6f */
&bast13_p,				/* p - 0x70 */
&bast13_q,				/* q - 0x71 */
&bast13_r,				/* r - 0x72 */
&bast13_s,				/* s - 0x73 */
&bast13_t,				/* t - 0x74 */
&bast13_u,				/* u - 0x75 */
&bast13_v,				/* v - 0x76 */
&bast13_w,				/* w - 0x77 */
&bast13_x,				/* x - 0x78 */
&bast13_y,				/* y - 0x79 */
&bast13_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};
#endif

static image_info_t	*clock12_font_table[] = {
(void *)0,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
(void *)0,				/* # - 0x23 */
(void *)0,				/* $ - 0x24 */
(void *)0,				/* % - 0x25 */
(void *)0,				/* & - 0x26 */
(void *)0,				/* ' - 0x27 */
(void *)0,				/* ( - 0x28 */
(void *)0,				/* ) - 0x29 */
(void *)0,				/* * - 0x2a */
(void *)0,				/* + - 0x2b */
(void *)0,				/* , - 0x2c */
(void *)0,				/* - - 0x2d */
(void *)0,				/* . - 0x2e */
(void *)0,				/* / - 0x2f */
&sp_0,						/* 0 - 0x30 */
&sp_1,						/* 1 - 0x31 */
&sp_2,						/* 2 - 0x32 */
&sp_3,						/* 3 - 0x33 */
&sp_4,						/* 4 - 0x34 */
&sp_5,						/* 5 - 0x35 */
&sp_6,						/* 6 - 0x36 */
&sp_7,						/* 7 - 0x37 */
&sp_8,						/* 8 - 0x38 */
&sp_9,						/* 9 - 0x39 */
(void *)0,				/* : - 0x3a */
(void *)0,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
(void *)0,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
(void *)0,				/* A - 0x41 */
(void *)0,				/* B - 0x42 */
(void *)0,				/* C - 0x43 */
(void *)0,				/* D - 0x44 */
(void *)0,				/* E - 0x45 */
(void *)0,				/* F - 0x46 */
(void *)0,				/* G - 0x47 */
(void *)0,				/* H - 0x48 */
(void *)0,				/* I - 0x49 */
(void *)0,				/* J - 0x4a */
(void *)0,				/* K - 0x4b */
(void *)0,				/* L - 0x4c */
(void *)0,				/* M - 0x4d */
(void *)0,				/* N - 0x4e */
(void *)0,				/* O - 0x4f */
(void *)0,				/* P - 0x50 */
(void *)0,				/* Q - 0x51 */
(void *)0,				/* R - 0x52 */
(void *)0,				/* S - 0x53 */
(void *)0,				/* T - 0x54 */
(void *)0,				/* U - 0x55 */
(void *)0,				/* V - 0x56 */
(void *)0,				/* W - 0x57 */
(void *)0,				/* X - 0x58 */
(void *)0,				/* Y - 0x59 */
(void *)0,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
(void *)0,				/* a - 0x61 */
(void *)0,				/* b - 0x62 */
(void *)0,				/* c - 0x63 */
(void *)0,				/* d - 0x64 */
(void *)0,				/* e - 0x65 */
(void *)0,				/* f - 0x66 */
(void *)0,				/* g - 0x67 */
(void *)0,				/* h - 0x68 */
(void *)0,				/* i - 0x69 */
(void *)0,				/* j - 0x6a */
(void *)0,				/* k - 0x6b */
(void *)0,				/* l - 0x6c */
(void *)0,				/* m - 0x6d */
(void *)0,				/* n - 0x6e */
(void *)0,				/* o - 0x6f */
(void *)0,				/* p - 0x70 */
(void *)0,				/* q - 0x71 */
(void *)0,				/* r - 0x72 */
(void *)0,				/* s - 0x73 */
(void *)0,				/* t - 0x74 */
(void *)0,				/* u - 0x75 */
(void *)0,				/* v - 0x76 */
(void *)0,				/* w - 0x77 */
(void *)0,				/* x - 0x78 */
(void *)0,				/* y - 0x79 */
(void *)0				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};


static image_info_t	*bast18_font_table[] = {
&bast18exc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast18num,				/* # - 0x23 */
&bast18dol,				/* $ - 0x24 */
&bast18pct,				/* % - 0x25 */
&bast18and,				/* & - 0x26 */
&bast18apo,				/* ' - 0x27 */
&bast18prl,				/* ( - 0x28 */
&bast18prr,				/* ) - 0x29 */
&bast18ast,				/* * - 0x2a */
(void *)0,				/* + - 0x2b */
&bast18com,				/* , - 0x2c */
&bast18dsh,				/* - - 0x2d */
&bast18per,				/* . - 0x2e */
&bast18sls,				/* / - 0x2f */
&bast18_0,				/* 0 - 0x30 */
&bast18_1,				/* 1 - 0x31 */
&bast18_2,				/* 2 - 0x32 */
&bast18_3,				/* 3 - 0x33 */
&bast18_4,				/* 4 - 0x34 */
&bast18_5,				/* 5 - 0x35 */
&bast18_6,				/* 6 - 0x36 */
&bast18_7,				/* 7 - 0x37 */
&bast18_8,				/* 8 - 0x38 */
&bast18_9,				/* 9 - 0x39 */
&bast18col,				/* : - 0x3a */
(void *)0,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
(void *)0,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast18_a,				/* A - 0x41 */
&bast18_b,				/* B - 0x42 */
&bast18_c,				/* C - 0x43 */
&bast18_d,				/* D - 0x44 */
&bast18_e,				/* E - 0x45 */
&bast18_f,				/* F - 0x46 */
&bast18_g,				/* G - 0x47 */
&bast18_h,				/* H - 0x48 */
&bast18_i,				/* I - 0x49 */
&bast18_j,				/* J - 0x4a */
&bast18_k,				/* K - 0x4b */
&bast18_l,				/* L - 0x4c */
&bast18_m,				/* M - 0x4d */
&bast18_n,				/* N - 0x4e */
&bast18_o,				/* O - 0x4f */
&bast18_p,				/* P - 0x50 */
&bast18_q,				/* Q - 0x51 */
&bast18_r,				/* R - 0x52 */
&bast18_s,				/* S - 0x53 */
&bast18_t,				/* T - 0x54 */
&bast18_u,				/* U - 0x55 */
&bast18_v,				/* V - 0x56 */
&bast18_w,				/* W - 0x57 */
&bast18_x,				/* X - 0x58 */
&bast18_y,				/* Y - 0x59 */
&bast18_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast18_a,				/* a - 0x61 */
&bast18_b,				/* b - 0x62 */
&bast18_c,				/* c - 0x63 */
&bast18_d,				/* d - 0x64 */
&bast18_e,				/* e - 0x65 */
&bast18_f,				/* f - 0x66 */
&bast18_g,				/* g - 0x67 */
&bast18_h,				/* h - 0x68 */
&bast18_i,				/* i - 0x69 */
&bast18_j,				/* j - 0x6a */
&bast18_k,				/* k - 0x6b */
&bast18_l,				/* l - 0x6c */
&bast18_m,				/* m - 0x6d */
&bast18_n,				/* n - 0x6e */
&bast18_o,				/* o - 0x6f */
&bast18_p,				/* p - 0x70 */
&bast18_q,				/* q - 0x71 */
&bast18_r,				/* r - 0x72 */
&bast18_s,				/* s - 0x73 */
&bast18_t,				/* t - 0x74 */
&bast18_u,				/* u - 0x75 */
&bast18_v,				/* v - 0x76 */
&bast18_w,				/* w - 0x77 */
&bast18_x,				/* x - 0x78 */
&bast18_y,				/* y - 0x79 */
&bast18_z				/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};


static image_info_t	*bast25_font_table[] = {
&bast25exc,				/* ! - 0x21 */
(void *)0,				/* " - 0x22 */
&bast25num,				/* # - 0x23 */
&bast25dol,				/* $ - 0x24 */
&bast25pct,				/* % - 0x25 */
&bast25and,				/* & - 0x26 */
(void *)0,				/* ' - 0x27 */
&bast25prl,				/* ( - 0x28 */
&bast25prr,				/* ) - 0x29 */
(void *)0,				/* * - 0x2a */
&bast25add,				/* + - 0x2b */
&bast25com,				/* , - 0x2c */
&bast25dsh,				/* - - 0x2d */
&bast25per,				/* . - 0x2e */
&bast25sls,				/* / - 0x2f */
&bast25_0,				/* 0 - 0x30 */
&bast25_1,				/* 1 - 0x31 */
&bast25_2,				/* 2 - 0x32 */
&bast25_3,				/* 3 - 0x33 */
&bast25_4,				/* 4 - 0x34 */
&bast25_5,				/* 5 - 0x35 */
&bast25_6,				/* 6 - 0x36 */
&bast25_7,				/* 7 - 0x37 */
&bast25_8,				/* 8 - 0x38 */
&bast25_9,				/* 9 - 0x39 */
&bast25col,				/* : - 0x3a */
&bast25sem,				/* ; - 0x3b */
(void *)0,				/* < - 0x3c */
(void *)0,				/* = - 0x3d */
(void *)0,				/* > - 0x3e */
&bast25que,				/* ? - 0x3f */
(void *)0,				/* @ - 0x40 */
&bast25_a,				/* A - 0x41 */
&bast25_b,				/* B - 0x42 */
&bast25_c,				/* C - 0x43 */
&bast25_d,				/* D - 0x44 */
&bast25_e,				/* E - 0x45 */
&bast25_f,				/* F - 0x46 */
&bast25_g,				/* G - 0x47 */
&bast25_h,				/* H - 0x48 */
&bast25_i,				/* I - 0x49 */
&bast25_j,				/* J - 0x4a */
&bast25_k,				/* K - 0x4b */
&bast25_l,				/* L - 0x4c */
&bast25_m,				/* M - 0x4d */
&bast25_n,				/* N - 0x4e */
&bast25_o,				/* O - 0x4f */
&bast25_p,				/* P - 0x50 */
&bast25_q,				/* Q - 0x51 */
&bast25_r,				/* R - 0x52 */
&bast25_s,				/* S - 0x53 */
&bast25_t,				/* T - 0x54 */
&bast25_u,				/* U - 0x55 */
&bast25_v,				/* V - 0x56 */
&bast25_w,				/* W - 0x57 */
&bast25_x,				/* X - 0x58 */
&bast25_y,				/* Y - 0x59 */
&bast25_z,				/* Z - 0x5a */
(void *)0,				/* [ - 0x5b */
(void *)0,				/* \ - 0x5c */
(void *)0,				/* ] - 0x5d */
(void *)0,				/* ^ - 0x5e */
(void *)0,				/*   - 0x5f */
(void *)0,				/*   - 0x60 */
&bast25_a,				/* a - 0x61 */
&bast25_b,				/* b - 0x62 */
&bast25_c,				/* c - 0x63 */
&bast25_d,				/* d - 0x64 */
&bast25_e,				/* e - 0x65 */
&bast25_f,				/* f - 0x66 */
&bast25_g,				/* g - 0x67 */
&bast25_h,				/* h - 0x68 */
&bast25_i,				/* i - 0x69 */
&bast25_j,				/* j - 0x6a */
&bast25_k,				/* k - 0x6b */
&bast25_l,				/* l - 0x6c */
&bast25_m,				/* m - 0x6d */
&bast25_n,				/* n - 0x6e */
&bast25_o,				/* o - 0x6f */
&bast25_p,				/* p - 0x70 */
&bast25_q,				/* q - 0x71 */
&bast25_r,				/* r - 0x72 */
&bast25_s,				/* s - 0x73 */
&bast25_t,				/* t - 0x74 */
&bast25_u,				/* u - 0x75 */
&bast25_v,				/* v - 0x76 */
&bast25_w,				/* w - 0x77 */
&bast25_x,				/* x - 0x78 */
&bast25_y,				/* y - 0x79 */
&bast25_z					/* z - 0x7a */
/* { - 0x7b */
/* | - 0x7c */
/* } - 0x7d */
/* ~ - 0x7e */
};


//static image_info_t	*bast35_font_table[] = {
//&bast35exc,				/* ! - 0x21 */
//(void *)0,				/* " - 0x22 */
//&bast35num,				/* # - 0x23 */
//&bast35dol,				/* $ - 0x24 */
//&bast35pct,				/* % - 0x25 */
//&bast35and,				/* & - 0x26 */
//&bast35apo,				/* ' - 0x27 */
//&bast35prl,				/* ( - 0x28 */
//&bast35prr,				/* ) - 0x29 */
//(void *)0,				/* * - 0x2a */
//&bast35add,				/* + - 0x2b */
//&bast35com,				/* , - 0x2c */
//&bast35dsh,				/* - - 0x2d */
//&bast35per,				/* . - 0x2e */
//&bast35sls,				/* / - 0x2f */
//&bast35_0,				/* 0 - 0x30 */
//&bast35_1,				/* 1 - 0x31 */
//&bast35_2,				/* 2 - 0x32 */
//&bast35_3,				/* 3 - 0x33 */
//&bast35_4,				/* 4 - 0x34 */
//&bast35_5,				/* 5 - 0x35 */
//&bast35_6,				/* 6 - 0x36 */
//&bast35_7,				/* 7 - 0x37 */
//&bast35_8,				/* 8 - 0x38 */
//&bast35_9,				/* 9 - 0x39 */
//&bast35col,				/* : - 0x3a */
//&bast35sem,				/* ; - 0x3b */
//(void *)0,				/* < - 0x3c */
//(void *)0,				/* = - 0x3d */
//(void *)0,				/* > - 0x3e */
//&bast35que,				/* ? - 0x3f */
//(void *)0,				/* @ - 0x40 */
//&bast35_a,				/* A - 0x41 */
//&bast35_b,				/* B - 0x42 */
//&bast35_c,				/* C - 0x43 */
//&bast35_d,				/* D - 0x44 */
//&bast35_e,				/* E - 0x45 */
//&bast35_f,				/* F - 0x46 */
//&bast35_g,				/* G - 0x47 */
//&bast35_h,				/* H - 0x48 */
//&bast35_i,				/* I - 0x49 */
//&bast35_j,				/* J - 0x4a */
//&bast35_k,				/* K - 0x4b */
//&bast35_l,				/* L - 0x4c */
//&bast35_m,				/* M - 0x4d */
//&bast35_n,				/* N - 0x4e */
//&bast35_o,				/* O - 0x4f */
//&bast35_p,				/* P - 0x50 */
//&bast35_q,				/* Q - 0x51 */
//&bast35_r,				/* R - 0x52 */
//&bast35_s,				/* S - 0x53 */
//&bast35_t,				/* T - 0x54 */
//&bast35_u,				/* U - 0x55 */
//&bast35_v,				/* V - 0x56 */
//&bast35_w,				/* W - 0x57 */
//&bast35_x,				/* X - 0x58 */
//&bast35_y,				/* Y - 0x59 */
//&bast35_z,				/* Z - 0x5a */
//(void *)0,				/* [ - 0x5b */
//(void *)0,				/* \ - 0x5c */
//(void *)0,				/* ] - 0x5d */
//(void *)0,				/* ^ - 0x5e */
//(void *)0,				/*   - 0x5f */
//(void *)0,				/*   - 0x60 */
//&bast35_a,				/* a - 0x61 */
//&bast35_b,				/* b - 0x62 */
//&bast35_c,				/* c - 0x63 */
//&bast35_d,				/* d - 0x64 */
//&bast35_e,				/* e - 0x65 */
//&bast35_f,				/* f - 0x66 */
//&bast35_g,				/* g - 0x67 */
//&bast35_h,				/* h - 0x68 */
//&bast35_i,				/* i - 0x69 */
//&bast35_j,				/* j - 0x6a */
//&bast35_k,				/* k - 0x6b */
//&bast35_l,				/* l - 0x6c */
//&bast35_m,				/* m - 0x6d */
//&bast35_n,				/* n - 0x6e */
//&bast35_o,				/* o - 0x6f */
//&bast35_p,				/* p - 0x70 */
//&bast35_q,				/* q - 0x71 */
//&bast35_r,				/* r - 0x72 */
//&bast35_s,				/* s - 0x73 */
//&bast35_t,				/* t - 0x74 */
//&bast35_u,				/* u - 0x75 */
//&bast35_v,				/* v - 0x76 */
//&bast35_w,				/* w - 0x77 */
//&bast35_x,				/* x - 0x78 */
//&bast35_y,				/* y - 0x79 */
//&bast35_z					/* z - 0x7a */
///* { - 0x7b */
///* | - 0x7c */
///* } - 0x7d */
///* ~ - 0x7e */
//};


static image_info_t	*bast23_font_table[] =
{
	&bas23exc,				/* ! - 0x21 */
	(void *)0,				/* " - 0x22 */
	&bas23num,				/* # - 0x23 */
	&bas23dol,				/* $ - 0x24 */
	&bas23pct,				/* % - 0x25 */
	&bas23and,				/* & - 0x26 */
	&bas23apo,				/* ' - 0x27 */
	&bas23prl,				/* ( - 0x28 */
	&bas23prr,				/* ) - 0x29 */
	(void *)0,				/* * - 0x2a */
	&bas23add,				/* + - 0x2b */
	&bas23com,				/* , - 0x2c */
	&bas23dsh,				/* - - 0x2d */
	&bas23per,				/* . - 0x2e */
	&bas23sls,				/* / - 0x2f */
	&bast23_0,				/* 0 - 0x30 */
	&bast23_1,				/* 1 - 0x31 */
	&bast23_2,				/* 2 - 0x32 */
	&bast23_3,				/* 3 - 0x33 */
	&bast23_4,				/* 4 - 0x34 */
	&bast23_5,				/* 5 - 0x35 */
	&bast23_6,				/* 6 - 0x36 */
	&bast23_7,				/* 7 - 0x37 */
	&bast23_8,				/* 8 - 0x38 */
	&bast23_9,				/* 9 - 0x39 */
	&bas23col,				/* : - 0x3a */
	&bas23com,				/* ; - 0x3b */	//sem!
	(void *)0,				/* < - 0x3c */
	(void *)0,				/* = - 0x3d */
	(void *)0,				/* > - 0x3e */
	&bas23que,				/* ? - 0x3f */
	(void *)0,				/* @ - 0x40 */
	&bast23_a,				/* A - 0x41 */
	&bast23_b,				/* B - 0x42 */
	&bast23_c,				/* C - 0x43 */
	&bast23_d,				/* D - 0x44 */
	&bast23_e,				/* E - 0x45 */
	&bast23_f,				/* F - 0x46 */
	&bast23_g,				/* G - 0x47 */
	&bast23_h,				/* H - 0x48 */
	&bast23_i,				/* I - 0x49 */
	&bast23_j,				/* J - 0x4a */
	&bast23_k,				/* K - 0x4b */
	&bast23_l,				/* L - 0x4c */
	&bast23_m,				/* M - 0x4d */
	&bast23_n,				/* N - 0x4e */
	&bast23_o,				/* O - 0x4f */
	&bast23_p,				/* P - 0x50 */
	&bast23_q,				/* Q - 0x51 */
	&bast23_r,				/* R - 0x52 */
	&bast23_s,				/* S - 0x53 */
	&bast23_t,				/* T - 0x54 */
	&bast23_u,				/* U - 0x55 */
	&bast23_v,				/* V - 0x56 */
	&bast23_w,				/* W - 0x57 */
	&bast23_x,				/* X - 0x58 */
	&bast23_y,				/* Y - 0x59 */
	&bast23_z,				/* Z - 0x5a */
	(void *)0,				/* [ - 0x5b */
	(void *)0,				/* \ - 0x5c */
	(void *)0,				/* ] - 0x5d */
	(void *)0,				/* ^ - 0x5e */
	(void *)0,				/*   - 0x5f */
	(void *)0,				/*   - 0x60 */
	&bast23_a,				/* a - 0x61 */
	&bast23_b,				/* b - 0x62 */
	&bast23_c,				/* c - 0x63 */
	&bast23_d,				/* d - 0x64 */
	&bast23_e,				/* e - 0x65 */
	&bast23_f,				/* f - 0x66 */
	&bast23_g,				/* g - 0x67 */
	&bast23_h,				/* h - 0x68 */
	&bast23_i,				/* i - 0x69 */
	&bast23_j,				/* j - 0x6a */
	&bast23_k,				/* k - 0x6b */
	&bast23_l,				/* l - 0x6c */
	&bast23_m,				/* m - 0x6d */
	&bast23_n,				/* n - 0x6e */
	&bast23_o,				/* o - 0x6f */
	&bast23_p,				/* p - 0x70 */
	&bast23_q,				/* q - 0x71 */
	&bast23_r,				/* r - 0x72 */
	&bast23_s,				/* s - 0x73 */
	&bast23_t,				/* t - 0x74 */
	&bast23_u,				/* u - 0x75 */
	&bast23_v,				/* v - 0x76 */
	&bast23_w,				/* w - 0x77 */
	&bast23_x,				/* x - 0x78 */
	&bast23_y,				/* y - 0x79 */
	&bast23_z					/* z - 0x7a */
	/* { - 0x7b */
	/* | - 0x7c */
	/* } - 0x7d */
	/* ~ - 0x7e */
};

static image_info_t	*bast75_font_table[] =
{
	&bas75exc,				/* ! - 0x21 */
	(void *)0,				/* " - 0x22 */
	&bas75num,				/* # - 0x23 */
	&bas75dol,				/* $ - 0x24 */
	&bas75pct,				/* % - 0x25 */
	&bas75and,				/* & - 0x26 */
	&bas75apo,				/* ' - 0x27 */
	&bas75prl,				/* ( - 0x28 */
	&bas75prr,				/* ) - 0x29 */
	(void *)0,				/* * - 0x2a */
	&bas75add,				/* + - 0x2b */
	&bas75com,				/* , - 0x2c */
	&bas75dsh,				/* - - 0x2d */
	&bas75per,				/* . - 0x2e */
	&bas75sls,				/* / - 0x2f */
	&bast75_0,				/* 0 - 0x30 */
	&bast75_1,				/* 1 - 0x31 */
	&bast75_2,				/* 2 - 0x32 */
	&bast75_3,				/* 3 - 0x33 */
	&bast75_4,				/* 4 - 0x34 */
	&bast75_5,				/* 5 - 0x35 */
	&bast75_6,				/* 6 - 0x36 */
	&bast75_7,				/* 7 - 0x37 */
	&bast75_8,				/* 8 - 0x38 */
	&bast75_9,				/* 9 - 0x39 */
	&bas75col,				/* : - 0x3a */
	&bas75com,				/* ; - 0x3b */ //sem!
	(void *)0,				/* < - 0x3c */
	(void *)0,				/* = - 0x3d */
	(void *)0,				/* > - 0x3e */
	&bas75que,				/* ? - 0x3f */
	(void *)0,				/* @ - 0x40 */
	&bast75_a,				/* A - 0x41 */
	&bast75_b,				/* B - 0x42 */
	&bast75_c,				/* C - 0x43 */
	&bast75_d,				/* D - 0x44 */
	&bast75_e,				/* E - 0x45 */
	&bast75_f,				/* F - 0x46 */
	&bast75_g,				/* G - 0x47 */
	&bast75_h,				/* H - 0x48 */
	&bast75_i,				/* I - 0x49 */
	&bast75_j,				/* J - 0x4a */
	&bast75_k,				/* K - 0x4b */
	&bast75_l,				/* L - 0x4c */
	&bast75_m,				/* M - 0x4d */
	&bast75_n,				/* N - 0x4e */
	&bast75_o,				/* O - 0x4f */
	&bast75_p,				/* P - 0x50 */
	&bast75_q,				/* Q - 0x51 */
	&bast75_r,				/* R - 0x52 */
	&bast75_s,				/* S - 0x53 */
	&bast75_t,				/* T - 0x54 */
	&bast75_u,				/* U - 0x55 */
	&bast75_v,				/* V - 0x56 */
	&bast75_w,				/* W - 0x57 */
	&bast75_x,				/* X - 0x58 */
	&bast75_y,				/* Y - 0x59 */
	&bast75_z,				/* Z - 0x5a */
	(void *)0,				/* [ - 0x5b */
	(void *)0,				/* \ - 0x5c */
	(void *)0,				/* ] - 0x5d */
	(void *)0,				/* ^ - 0x5e */
	(void *)0,				/*   - 0x5f */
	(void *)0,				/*   - 0x60 */
	&bast75_a,				/* a - 0x61 */
	&bast75_b,				/* b - 0x62 */
	&bast75_c,				/* c - 0x63 */
	&bast75_d,				/* d - 0x64 */
	&bast75_e,				/* e - 0x65 */
	&bast75_f,				/* f - 0x66 */
	&bast75_g,				/* g - 0x67 */
	&bast75_h,				/* h - 0x68 */
	&bast75_i,				/* i - 0x69 */
	&bast75_j,				/* j - 0x6a */
	&bast75_k,				/* k - 0x6b */
	&bast75_l,				/* l - 0x6c */
	&bast75_m,				/* m - 0x6d */
	&bast75_n,				/* n - 0x6e */
	&bast75_o,				/* o - 0x6f */
	&bast75_p,				/* p - 0x70 */
	&bast75_q,				/* q - 0x71 */
	&bast75_r,				/* r - 0x72 */
	&bast75_s,				/* s - 0x73 */
	&bast75_t,				/* t - 0x74 */
	&bast75_u,				/* u - 0x75 */
	&bast75_v,				/* v - 0x76 */
	&bast75_w,				/* w - 0x77 */
	&bast75_x,				/* x - 0x78 */
	&bast75_y,				/* y - 0x79 */
	&bast75_z					/* z - 0x7a */
	/* { - 0x7b */
	/* | - 0x7c */
	/* } - 0x7d */
	/* ~ - 0x7e */
};

static image_info_t	*bast16_font_table[] =
{
	&bast16exc,				/* ! - 0x21 */
	(void *)0,				/* " - 0x22 */
	&bast16num,				/* # - 0x23 */
	&bast16dol,				/* $ - 0x24 */
	&bast16pct,				/* % - 0x25 */
	&bast16and,				/* & - 0x26 */
	&bast16apo,				/* ' - 0x27 */
	&bast16prl,				/* ( - 0x28 */
	&bast16prr,				/* ) - 0x29 */
	(void *)0,				/* * - 0x2a */
	&bast16add,				/* + - 0x2b */
	&bast16com,				/* , - 0x2c */
	&bast16das,				/* - - 0x2d */
	&bast16per,				/* . - 0x2e */
	&bast16sls,				/* / - 0x2f */
	&bast16_0,				/* 0 - 0x30 */
	&bast16_1,				/* 1 - 0x31 */
	&bast16_2,				/* 2 - 0x32 */
	&bast16_3,				/* 3 - 0x33 */
	&bast16_4,				/* 4 - 0x34 */
	&bast16_5,				/* 5 - 0x35 */
	&bast16_6,				/* 6 - 0x36 */
	&bast16_7,				/* 7 - 0x37 */
	&bast16_8,				/* 8 - 0x38 */
	&bast16_9,				/* 9 - 0x39 */
	&bast16col,				/* : - 0x3a */
	&bast16sem,				/* ; - 0x3b */
	(void *)0,				/* < - 0x3c */
	(void *)0,				/* = - 0x3d */
	(void *)0,				/* > - 0x3e */
	(void *)0,				/* ? - 0x3f */
	(void *)0,				/* @ - 0x40 */
	&bast16_a,				/* A - 0x41 */
	&bast16_b,				/* B - 0x42 */
	&bast16_c,				/* C - 0x43 */
	&bast16_d,				/* D - 0x44 */
	&bast16_e,				/* E - 0x45 */
	&bast16_f,				/* F - 0x46 */
	&bast16_g,				/* G - 0x47 */
	&bast16_h,				/* H - 0x48 */
	&bast16_i,				/* I - 0x49 */
	&bast16_j,				/* J - 0x4a */
	&bast16_k,				/* K - 0x4b */
	&bast16_l,				/* L - 0x4c */
	&bast16_m,				/* M - 0x4d */
	&bast16_n,				/* N - 0x4e */
	&bast16_o,				/* O - 0x4f */
	&bast16_p,				/* P - 0x50 */
	&bast16_q,				/* Q - 0x51 */
	&bast16_r,				/* R - 0x52 */
	&bast16_s,				/* S - 0x53 */
	&bast16_t,				/* T - 0x54 */
	&bast16_u,				/* U - 0x55 */
	&bast16_v,				/* V - 0x56 */
	&bast16_w,				/* W - 0x57 */
	&bast16_x,				/* X - 0x58 */
	&bast16_y,				/* Y - 0x59 */
	&bast16_z,				/* Z - 0x5a */
	(void *)0,				/* [ - 0x5b */
	(void *)0,				/* \ - 0x5c */
	(void *)0,				/* ] - 0x5d */
	(void *)0,				/* ^ - 0x5e */
	(void *)0,				/*   - 0x5f */
	(void *)0,				/*   - 0x60 */
	&bast16_a,				/* a - 0x61 */
	&bast16_b,				/* b - 0x62 */
	&bast16_c,				/* c - 0x63 */
	&bast16_d,				/* d - 0x64 */
	&bast16_e,				/* e - 0x65 */
	&bast16_f,				/* f - 0x66 */
	&bast16_g,				/* g - 0x67 */
	&bast16_h,				/* h - 0x68 */
	&bast16_i,				/* i - 0x69 */
	&bast16_j,				/* j - 0x6a */
	&bast16_k,				/* k - 0x6b */
	&bast16_l,				/* l - 0x6c */
	&bast16_m,				/* m - 0x6d */
	&bast16_n,				/* n - 0x6e */
	&bast16_o,				/* o - 0x6f */
	&bast16_p,				/* p - 0x70 */
	&bast16_q,				/* q - 0x71 */
	&bast16_r,				/* r - 0x72 */
	&bast16_s,				/* s - 0x73 */
	&bast16_t,				/* t - 0x74 */
	&bast16_u,				/* u - 0x75 */
	&bast16_v,				/* v - 0x76 */
	&bast16_w,				/* w - 0x77 */
	&bast16_x,				/* x - 0x78 */
	&bast16_y,				/* y - 0x79 */
	&bast16_z					/* z - 0x7a */
	/* { - 0x7b */
	/* | - 0x7c */
	/* } - 0x7d */
	/* ~ - 0x7e */
};


// bast7 thin font definition
font_info_t	bast7t_font = {
sizeof(bast7t_font_table)/sizeof(void *),
0x21,
0x7a,
8,
0x5,
0,
sysfont_tex_name,
bast7t_font_table,
0						// 0 = don't scale when in lo res mode
						// 1 = scale font when in lo res mode
};

// bast8 thin font definition
font_info_t	bast8t_font = {
sizeof(bast8t_font_table)/sizeof(void *),
0x21,
0x7a,
9,
0x5,
0,
sysfont_tex_name,
bast8t_font_table,
0
};

// bast10 font definition
font_info_t	bast10_font = {
sizeof(bast10_font_table)/sizeof(void *),
0x21,
0x7a,
11,
0x5,
0,
sysfont_tex_name,
bast10_font_table,
0
};

// bast10 font for low res definition
font_info_t	bast10low_font = {
sizeof(bast10_font_table)/sizeof(void *),
0x21,
0x7a,
11,
0x5,
0,
sysfont_tex_name,
bast10_font_table,
0
};

// clock12 font definition
font_info_t	clock12_font = {
sizeof(clock12_font_table)/sizeof(void *),
0x21,
0x7a,
8,
0x5,
0,
sysfont_tex_name,
clock12_font_table,
0
};


// bast13 font definition
font_info_t	bast13_font = {
sizeof(bast13_font_table)/sizeof(void *),
0x21,
0x7a,
15,				//maximum character height
0x6,			//space in pixels
1,				//inter character spacing
sysfont_tex_name,
bast13_font_table,
0
};

// bast18 font definition
font_info_t	bast18_font = {
sizeof(bast18_font_table)/sizeof(void *),
0x21,
0x7a,
20,
0x8,
1,
sysfont_tex_name,
bast18_font_table,
0
};

// bast23 font definition
font_info_t	bast23_font = {
sizeof(bast23_font_table)/sizeof(void *),
0x21,
0x7a,
33,						//maximum character height
0x10,					//space in pixels
-3,						//inter character spacing
sysfont_tex_name,
bast23_font_table,
0
};

// bast23 font definition
font_info_t	bast23low_font = {
sizeof(bast23_font_table)/sizeof(void *),
0x21,
0x7a,
33,						//maximum character height
0x10,					//space in pixels
-3,						//inter character spacing
sysfont_tex_name,
bast23_font_table,
0
};

// bast25 font definition
font_info_t	bast25_font = {
sizeof(bast25_font_table)/sizeof(void *),
0x21,
0x7a,
33,
0x10,
2,
sysfont_tex_name,
bast25_font_table,
0
};

// bast75 (big) font definition
font_info_t	bast75_font = {
sizeof(bast75_font_table)/sizeof(void *),
0x21,
0x7a,
75,				//maximum character height
24,				//space in pixels
-6,				//inter character spacing
sysfont_tex_name,
bast75_font_table,
1
};

// bast16 font definition
font_info_t	bast16_font = {
sizeof(bast16_font_table)/sizeof(void *),
0x21,
0x7a,
18,				//maximum character height
10,				//space in pixels
-3,				//inter character spacing
font23_tex_name,
bast16_font_table,
0
};

// bast23 font definition
font_info_t	bast23scale_font = {
sizeof(bast23_font_table)/sizeof(void *),
0x21,
0x7a,
33,						//maximum character height
0x10,					//space in pixels
-3,						//inter character spacing
sysfont_tex_name,
bast23_font_table,
1
};

//// bast35 font definition
//font_info_t	bast35_font = {
//sizeof(bast35_font_table)/sizeof(void *),
//0x21,
//0x7a,
//47,
//0x10,
//0x0,
//sysfont_tex_name,
//bast35_font_table
//};
