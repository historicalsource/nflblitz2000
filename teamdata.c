/****************************************************************************/
/*                                                                          */
/* teamdata.c - Team & Individual player data								*/
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <goose/sprites.h>

#include "include/ani3d.h"
#include "include/game.h"
#include "include/player.h"

#include "include/general.h"
#include "include/tmsel.h"
#include "include/nflcmos.h"

#define	IMAGEDEFINE	1
//#include "include/mugshot.h"
//#include "include/rlogo1.h"
//#include "include/rlogo2.h"
#include "include/smlogos.h" 


extern image_info_t tb_free;

extern LIMB limb_mikel;
extern LIMB limb_johnc;
extern LIMB limb_danf;
extern LIMB limb_thug;
extern LIMB limb_skull;
extern LIMB limb_shinok;
extern LIMB limb_raiden;
extern LIMB limb_brain;
extern LIMB limb_dude;
extern LIMB limb_johnr;
extern LIMB limb_luism;
extern LIMB limb_jenh;
extern LIMB limb_jeffj;
extern LIMB limb_jasons;
extern LIMB limb_dant;
extern LIMB limb_jimg;
extern LIMB limb_sald;
extern LIMB limb_markt;
extern LIMB limb_hd_brian;
extern LIMB limb_hd_paulo;

// Table data for team info screen

static image_info_t * pti_ari[] = {	&ti_ari,  0,  &ti_cardinals, &ti_cardinals_c1, 0}; // Arizona Cardinals
static image_info_t * pti_atl[] = {	&ti_atl,  0,  &ti_falcons,   0};                   // Altlanta Falcons
static image_info_t * pti_bal[] = {	&ti_bal,  0,  &ti_ravens,    0};                   // Baltimore Ravens
static image_info_t * pti_buf[] = {	&ti_buf,  0,  &ti_bills,     0};                   // Buffalo Bills
static image_info_t * pti_car[] = {	&ti_car,  0,  &ti_panthers,  0};                   // Carolina Panthers
static image_info_t * pti_chi[] = {	&ti_chi,  0,  &ti_bears,     &ti_bears_c1,     0}; // Chicago Bears
static image_info_t * pti_cin[] = {	&ti_cin,  0,  &ti_bengals,   0};                   // Cincinnati Bengals
static image_info_t * pti_cle[] = {	&ti_cle,  0,  &ti_browns,    0};                   // Cleveland Browns
static image_info_t * pti_dal[] = {	&ti_dal,  0,  &ti_cowboys,   &ti_cowboys_c1,   0}; // Dallas Cowboys
static image_info_t * pti_den[] = {	&ti_den,  0,  &ti_broncos,   &ti_broncos_c1,   0}; // Denver Broncos
static image_info_t * pti_det[] = {	&ti_det,  0,  &ti_lions,     0};                   // Detroit Lions
static image_info_t * pti_gb[]  = {	&ti_gre,  0,  &ti_packers,   0};                   // Green Bay Packers
static image_info_t * pti_ind[] = {	&ti_ind,  0,  &ti_colts,     0};                   // Indianapolis Colts
static image_info_t * pti_jac[] = {	&ti_jac,  0,  &ti_jaguars,   0};                   // Jacksonville Jaguars
static image_info_t * pti_kc[]  = {	&ti_kan,  0,  &ti_chiefs,    0};                   // Kansas City Chiefs
static image_info_t * pti_mia[] = {	&ti_mia,  0,  &ti_dolphins,  0};                   // Miami Dolphins
static image_info_t * pti_min[] = {	&ti_min,  0,  &ti_vikings,   0};                   // Minnesota Vikings
static image_info_t * pti_ne[]  = {	&ti_ne,   0,  &ti_patriots,  0};                   // New England Patriots
static image_info_t * pti_no[]  = {	&ti_nor,  0,  &ti_saints,    0};                   // New Orleans Saints
static image_info_t * pti_nyg[] = {	&ti_nyg,  0,  &ti_giants,    0};                   // New York Giants
static image_info_t * pti_nyj[] = {	&ti_nyj,  0,  &ti_jets,      0};                   // New York Jets
static image_info_t * pti_oak[] = {	&ti_oak,  0,  &ti_raiders,   0};                   // Oakland Raiders
static image_info_t * pti_phi[] = {	&ti_phi,  0,  &ti_eagles,    0};                   // Philadelphia Eagles
static image_info_t * pti_pit[] = {	&ti_pit,  0,  &ti_steelers,  0};                   // Pittsburgh Steelers
static image_info_t * pti_sd[]  = {	&ti_sand, 0,  &ti_chargers,  0};                   // San Diego Chargers
static image_info_t * pti_sf[]  = {	&ti_sanf, 0,  &ti_49ers,     0};                   // San Francisco 49'ers
static image_info_t * pti_sea[] = {	&ti_sea,  0,  &ti_seahawks,  0};                   // Seattle Seahawks
static image_info_t * pti_sl[]  = {	&ti_stl,  0,  &ti_rams,      &ti_rams_c1,     0};  // St. Louis Rams
static image_info_t * pti_tb[]  = {	&ti_tam,  0,  &ti_bucs,      0};                   // Tampa Bay Bucs
static image_info_t * pti_ten[] = {	&ti_ten,  0,  &ti_titans,    0};                   // Tennessee Titans
static image_info_t * pti_was[] = {	&ti_was,  0,  &ti_redskins,  &ti_redskins_c1, 0};  // Washington Redskins


// Table data for rotating team logos

#if 0
static image_info_t * prl_ari[] = {																// Arizona Cardinals
	&car01,&car02,&car03,&car04,&car05,&car06,&car07,&car08,&car09,&car10,&car11,&car12,&car13,
	&car14,&car15,&car16,&car17,&car18,&car19,&car20,&car21,&car22,&car23,&car24,&car25,&car26,
	&car27,&car28,&car29,&car30,&car31,&car32,&car33,&car34,&car35,&car36,&car37,&car38,&car39,
0};
static image_info_t * prl_atl[] = {																// Altlanta Falcons
	&fal01,&fal02,&fal03,&fal04,&fal05,&fal06,&fal07,&fal08,&fal09,&fal10,&fal11,&fal12,&fal13,
	&fal14,&fal15,&fal16,&fal17,&fal18,&fal19,&fal20,&fal21,&fal22,&fal23,&fal24,&fal25,&fal26,
	&fal27,&fal28,&fal29,&fal30,&fal31,&fal32,&fal33,&fal34,&fal35,&fal36,&fal37,&fal38,&fal39,
0};
static image_info_t * prl_bal[] = {																// Baltimore Ravens
	&rav01,&rav02,&rav03,&rav04,&rav05,&rav06,&rav07,&rav08,&rav09,&rav10,&rav11,&rav12,&rav13,
	&rav14,&rav15,&rav16,&rav17,&rav18,&rav19,&rav20,&rav21,&rav22,&rav23,&rav24,&rav25,&rav26,
	&rav27,&rav28,&rav29,&rav30,&rav31,&rav32,&rav33,&rav34,&rav35,&rav36,&rav37,&rav38,&rav39,
0};
static image_info_t * prl_buf[] = {																// Buffalo Bills
	&bil01,&bil02,&bil03,&bil04,&bil05,&bil06,&bil07,&bil08,&bil09,&bil10,&bil11,&bil12,&bil13,
	&bil14,&bil15,&bil16,&bil17,&bil18,&bil19,&bil20,&bil21,&bil22,&bil23,&bil24,&bil25,&bil26,
	&bil27,&bil28,&bil29,&bil30,&bil31,&bil32,&bil33,&bil34,&bil35,&bil36,&bil37,&bil38,&bil39,
0};
static image_info_t * prl_car[] = {																// Carolina Panthers
	&pan01,&pan02,&pan03,&pan04,&pan05,&pan06,&pan07,&pan08,&pan09,&pan10,&pan11,&pan12,&pan13,
	&pan14,&pan15,&pan16,&pan17,&pan18,&pan19,&pan20,&pan21,&pan22,&pan23,&pan24,&pan25,&pan26,
	&pan27,&pan28,&pan29,&pan30,&pan31,&pan32,&pan33,&pan34,&pan35,&pan36,&pan37,&pan38,&pan39,
0};
static image_info_t * prl_chi[] = {																// Chicago Bears
	&bea01,&bea02,&bea03,&bea04,&bea05,&bea06,&bea07,&bea08,&bea09,&bea10,&bea11,&bea12,&bea13,
	&bea14,&bea15,&bea16,&bea17,&bea18,&bea19,&bea20,&bea21,&bea22,&bea23,&bea24,&bea25,&bea26,
	&bea27,&bea28,&bea29,&bea30,&bea31,&bea32,&bea33,&bea34,&bea35,&bea36,&bea37,&bea38,&bea39,
0};
static image_info_t * prl_cin[] = {																// Cincinnati Bengals
	&ben01,&ben02,&ben03,&ben04,&ben05,&ben06,&ben07,&ben08,&ben09,&ben10,&ben11,&ben12,&ben13,
	&ben14,&ben15,&ben16,&ben17,&ben18,&ben19,&ben20,&ben21,&ben22,&ben23,&ben24,&ben25,&ben26,
	&ben27,&ben28,&ben29,&ben30,&ben31,&ben32,&ben33,&ben34,&ben35,&ben36,&ben37,&ben38,&ben39,
0};
static image_info_t * prl_cle[] = {																// Cleveland Browns
	&brw01,&brw02,&brw03,&brw04,&brw05,&brw06,&brw07,&brw08,&brw09,&brw10,&brw11,&brw12,&brw13,
	&brw14,&brw15,&brw16,&brw17,&brw18,&brw19,&brw20,&brw21,&brw22,&brw23,&brw24,&brw25,&brw26,
	&brw27,&brw28,&brw29,&brw30,&brw31,&brw32,&brw33,&brw34,&brw35,&brw36,&brw37,&brw38,&brw39,
0};
static image_info_t * prl_dal[] = {																// Dallas Cowboys
	&cow01,&cow02,&cow03,&cow04,&cow05,&cow06,&cow07,&cow08,&cow09,&cow10,&cow11,&cow12,&cow13,
	&cow14,&cow15,&cow16,&cow17,&cow18,&cow19,&cow20,&cow21,&cow22,&cow23,&cow24,&cow25,&cow26,
	&cow27,&cow28,&cow29,&cow30,&cow31,&cow32,&cow33,&cow34,&cow35,&cow36,&cow37,&cow38,&cow39,
0};
static image_info_t * prl_den[] = {																// Denver Broncos
	&bro01,&bro02,&bro03,&bro04,&bro05,&bro06,&bro07,&bro08,&bro09,&bro10,&bro11,&bro12,&bro13,
	&bro14,&bro15,&bro16,&bro17,&bro18,&bro19,&bro20,&bro21,&bro22,&bro23,&bro24,&bro25,&bro26,
	&bro27,&bro28,&bro29,&bro30,&bro31,&bro32,&bro33,&bro34,&bro35,&bro36,&bro37,&bro38,&bro39,
0};
static image_info_t * prl_det[] = {																// Detroit Lions
	&lio01,&lio02,&lio03,&lio04,&lio05,&lio06,&lio07,&lio08,&lio09,&lio10,&lio11,&lio12,&lio13,
	&lio14,&lio15,&lio16,&lio17,&lio18,&lio19,&lio20,&lio21,&lio22,&lio23,&lio24,&lio25,&lio26,
	&lio27,&lio28,&lio29,&lio30,&lio31,&lio32,&lio33,&lio34,&lio35,&lio36,&lio37,&lio38,&lio39,
0};
static image_info_t * prl_gb[]  = {																// Green Bay Packers
	&pac01,&pac02,&pac03,&pac04,&pac05,&pac06,&pac07,&pac08,&pac09,&pac10,&pac11,&pac12,&pac13,
	&pac14,&pac15,&pac16,&pac17,&pac18,&pac19,&pac20,&pac21,&pac22,&pac23,&pac24,&pac25,&pac26,
	&pac27,&pac28,&pac29,&pac30,&pac31,&pac32,&pac33,&pac34,&pac35,&pac36,&pac37,&pac38,&pac39,
0};
static image_info_t * prl_ind[] = {																// Indianapolis Colts
	&col01,&col02,&col03,&col04,&col05,&col06,&col07,&col08,&col09,&col10,&col11,&col12,&col13,
	&col14,&col15,&col16,&col17,&col18,&col19,&col20,&col21,&col22,&col23,&col24,&col25,&col26,
	&col27,&col28,&col29,&col30,&col31,&col32,&col33,&col34,&col35,&col36,&col37,&col38,&col39,
0};
static image_info_t * prl_jac[] = {																// Jacksonville Jaguars
	&jag01,&jag02,&jag03,&jag04,&jag05,&jag06,&jag07,&jag08,&jag09,&jag10,&jag11,&jag12,&jag13,
	&jag14,&jag15,&jag16,&jag17,&jag18,&jag19,&jag20,&jag21,&jag22,&jag23,&jag24,&jag25,&jag26,
	&jag27,&jag28,&jag29,&jag30,&jag31,&jag32,&jag33,&jag34,&jag35,&jag36,&jag37,&jag38,&jag39,
0};
static image_info_t * prl_kc[]  = {																// Kansas City Chiefs
	&chi01,&chi02,&chi03,&chi04,&chi05,&chi06,&chi07,&chi08,&chi09,&chi10,&chi11,&chi12,&chi13,
	&chi14,&chi15,&chi16,&chi17,&chi18,&chi19,&chi20,&chi21,&chi22,&chi23,&chi24,&chi25,&chi26,
	&chi27,&chi28,&chi29,&chi30,&chi31,&chi32,&chi33,&chi34,&chi35,&chi36,&chi37,&chi38,&chi39,
0};
static image_info_t * prl_mia[] = {																// Miami Dolphins
	&dol01,&dol02,&dol03,&dol04,&dol05,&dol06,&dol07,&dol08,&dol09,&dol10,&dol11,&dol12,&dol13,
	&dol14,&dol15,&dol16,&dol17,&dol18,&dol19,&dol20,&dol21,&dol22,&dol23,&dol24,&dol25,&dol26,
	&dol27,&dol28,&dol29,&dol30,&dol31,&dol32,&dol33,&dol34,&dol35,&dol36,&dol37,&dol38,&dol39,
0};
static image_info_t * prl_min[] = {																// Minnesota Vikings
	&vik01,&vik02,&vik03,&vik04,&vik05,&vik06,&vik07,&vik08,&vik09,&vik10,&vik11,&vik12,&vik13,
	&vik14,&vik15,&vik16,&vik17,&vik18,&vik19,&vik20,&vik21,&vik22,&vik23,&vik24,&vik25,&vik26,
	&vik27,&vik28,&vik29,&vik30,&vik31,&vik32,&vik33,&vik34,&vik35,&vik36,&vik37,&vik38,&vik39,
0};
static image_info_t * prl_ne[]  = {																// New England Patriots
	&pat01,&pat02,&pat03,&pat04,&pat05,&pat06,&pat07,&pat08,&pat09,&pat10,&pat11,&pat12,&pat13,
	&pat14,&pat15,&pat16,&pat17,&pat18,&pat19,&pat20,&pat21,&pat22,&pat23,&pat24,&pat25,&pat26,
	&pat27,&pat28,&pat29,&pat30,&pat31,&pat32,&pat33,&pat34,&pat35,&pat36,&pat37,&pat38,&pat39,
0};
static image_info_t * prl_no[]  = {																// New Orleans Saints
	&sai01,&sai02,&sai03,&sai04,&sai05,&sai06,&sai07,&sai08,&sai09,&sai10,&sai11,&sai12,&sai13,
	&sai14,&sai15,&sai16,&sai17,&sai18,&sai19,&sai20,&sai21,&sai22,&sai23,&sai24,&sai25,&sai26,
	&sai27,&sai28,&sai29,&sai30,&sai31,&sai32,&sai33,&sai34,&sai35,&sai36,&sai37,&sai38,&sai39,
0};
static image_info_t * prl_nyg[] = {																// New York Giants
	&gia01,&gia02,&gia03,&gia04,&gia05,&gia06,&gia07,&gia08,&gia09,&gia10,&gia11,&gia12,&gia13,
	&gia14,&gia15,&gia16,&gia17,&gia18,&gia19,&gia20,&gia21,&gia22,&gia23,&gia24,&gia25,&gia26,
	&gia27,&gia28,&gia29,&gia30,&gia31,&gia32,&gia33,&gia34,&gia35,&gia36,&gia37,&gia38,&gia39,
0};
static image_info_t * prl_nyj[] = {																// New York Jets
	&jet01,&jet02,&jet03,&jet04,&jet05,&jet06,&jet07,&jet08,&jet09,&jet10,&jet11,&jet12,&jet13,
	&jet14,&jet15,&jet16,&jet17,&jet18,&jet19,&jet20,&jet21,&jet22,&jet23,&jet24,&jet25,&jet26,
	&jet27,&jet28,&jet29,&jet30,&jet31,&jet32,&jet33,&jet34,&jet35,&jet36,&jet37,&jet38,&jet39,
0};
static image_info_t * prl_oak[] = {																// Oakland Raiders
	&rai01,&rai02,&rai03,&rai04,&rai05,&rai06,&rai07,&rai08,&rai09,&rai10,&rai11,&rai12,&rai13,
	&rai14,&rai15,&rai16,&rai17,&rai18,&rai19,&rai20,&rai21,&rai22,&rai23,&rai24,&rai25,&rai26,
	&rai27,&rai28,&rai29,&rai30,&rai31,&rai32,&rai33,&rai34,&rai35,&rai36,&rai37,&rai38,&rai39,
0};
static image_info_t * prl_phi[] = {																// Philadelphia Eagles
	&eag01,&eag02,&eag03,&eag04,&eag05,&eag06,&eag07,&eag08,&eag09,&eag10,&eag11,&eag12,&eag13,
	&eag14,&eag15,&eag16,&eag17,&eag18,&eag19,&eag20,&eag21,&eag22,&eag23,&eag24,&eag25,&eag26,
	&eag27,&eag28,&eag29,&eag30,&eag31,&eag32,&eag33,&eag34,&eag35,&eag36,&eag37,&eag38,&eag39,
0};
static image_info_t * prl_pit[] = {																// Pittsburgh Steelers
	&ste01,&ste02,&ste03,&ste04,&ste05,&ste06,&ste07,&ste08,&ste09,&ste10,&ste11,&ste12,&ste13,
	&ste14,&ste15,&ste16,&ste17,&ste18,&ste19,&ste20,&ste21,&ste22,&ste23,&ste24,&ste25,&ste26,
	&ste27,&ste28,&ste29,&ste30,&ste31,&ste32,&ste33,&ste34,&ste35,&ste36,&ste37,&ste38,&ste39,
0};
static image_info_t * prl_sd[]  = {																// San Diego Chargers
	&cha01,&cha02,&cha03,&cha04,&cha05,&cha06,&cha07,&cha08,&cha09,&cha10,&cha11,&cha12,&cha13,
	&cha14,&cha15,&cha16,&cha17,&cha18,&cha19,&cha20,&cha21,&cha22,&cha23,&cha24,&cha25,&cha26,
	&cha27,&cha28,&cha29,&cha30,&cha31,&cha32,&cha33,&cha34,&cha35,&cha36,&cha37,&cha38,&cha39,
0};
static image_info_t * prl_sf[]  = {																// San Francisco 49'ers
	&for01,&for02,&for03,&for04,&for05,&for06,&for07,&for08,&for09,&for10,&for11,&for12,&for13,
	&for14,&for15,&for16,&for17,&for18,&for19,&for20,&for21,&for22,&for23,&for24,&for25,&for26,
	&for27,&for28,&for29,&for30,&for31,&for32,&for33,&for34,&for35,&for36,&for37,&for38,&for39,
0};
static image_info_t * prl_sea[] = {																// Seattle Seahawks
	&sea01,&sea02,&sea03,&sea04,&sea05,&sea06,&sea07,&sea08,&sea09,&sea10,&sea11,&sea12,&sea13,
	&sea14,&sea15,&sea16,&sea17,&sea18,&sea19,&sea20,&sea21,&sea22,&sea23,&sea24,&sea25,&sea26,
	&sea27,&sea28,&sea29,&sea30,&sea31,&sea32,&sea33,&sea34,&sea35,&sea36,&sea37,&sea38,&sea39,
0};
static image_info_t * prl_sl[]  = {																// St. Louis Rams
	&ram01,&ram02,&ram03,&ram04,&ram05,&ram06,&ram07,&ram08,&ram09,&ram10,&ram11,&ram12,&ram13,
	&ram14,&ram15,&ram16,&ram17,&ram18,&ram19,&ram20,&ram21,&ram22,&ram23,&ram24,&ram25,&ram26,
	&ram27,&ram28,&ram29,&ram30,&ram31,&ram32,&ram33,&ram34,&ram35,&ram36,&ram37,&ram38,&ram39,
0};
static image_info_t * prl_tb[]  = {																// Tampa Bay Bucs
	&buc01,&buc02,&buc03,&buc04,&buc05,&buc06,&buc07,&buc08,&buc09,&buc10,&buc11,&buc12,&buc13,
	&buc14,&buc15,&buc16,&buc17,&buc18,&buc19,&buc20,&buc21,&buc22,&buc23,&buc24,&buc25,&buc26,
	&buc27,&buc28,&buc29,&buc30,&buc31,&buc32,&buc33,&buc34,&buc35,&buc36,&buc37,&buc38,&buc39,
0};
static image_info_t * prl_ten[] = {																// Tennessee Titans
	&tit01,&tit02,&tit03,&tit04,&tit05,&tit06,&tit07,&tit08,&tit09,&tit10,&tit11,&tit12,&tit13,
	&tit14,&tit15,&tit16,&tit17,&tit18,&tit19,&tit20,&tit21,&tit22,&tit23,&tit24,&tit25,&tit26,
	&tit27,&tit28,&tit29,&tit30,&tit31,&tit32,&tit33,&tit34,&tit35,&tit36,&tit37,&tit38,&tit39,
0};
static image_info_t * prl_was[] = {																// Washington Redskins
	&red01,&red02,&red03,&red04,&red05,&red06,&red07,&red08,&red09,&red10,&red11,&red12,&red13,
	&red14,&red15,&red16,&red17,&red18,&red19,&red20,&red21,&red22,&red23,&red24,&red25,&red26,
	&red27,&red28,&red29,&red30,&red31,&red32,&red33,&red34,&red35,&red36,&red37,&red38,&red39,
0};

#endif

// Player roster data for each team

#define SC(x)	(1.05f * x)
#define PSUNK	PS_NORM
#define PCUNK	PC_WHITE
#define NUNK	0x00
#define PPUNK	PP_K
#define FNUNK	"UNK"
#define SUNK	-1

// speech index numbers
#define	QB		10		// add 7 to get excited version of this call

#define	W1		11		// add 3 to get "to wpn name" ... add 7 to get excited "to wpn name" call
#define	W2		12
#define	W3		13

#define NA		0

// Cardinals
const struct st_plyr_data spd_car[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x54, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x62, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x68, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x16, PP_QB, 0        , &qac_plum, &rac_plum , QB, "Plummer",    "Jake"     }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x29, PP_RB, 0	    , 0        , &rac_murr , W1, "Murrell",    "Adrian"   }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x85, PP_WR, 0	    , 0        , &rac_moor , W3, "Moore",      "Rob"      }, // 75"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x89, PP_WR, 0	    , 0        , &rac_bost , W2, "Boston",     "David"    }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x97, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x35, PP_CB, 0        , 0        , &rac_will , NA, "Williams",   "Aeneas"   }, // 70"  PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x42, PP_S , 0        , 0        , &rac_lass , NA, "Lassiter",   "Kwamie"   }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x28, PP_S , 0        , 0        , &rac_benn , NA, "Bennett",    "Tommy"    }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x59, PP_LB, 0        , 0        , &rac_fred , NA, "Frederickson","Rob"     }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x6 , PP_K , 0        , 0        , &rac_nedn , NA, "Nedney",     "Joe"      }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Falcons
const struct st_plyr_data spd_fal[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x70, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x68, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x12, PP_QB, 0		, &qaf_chan, &raf_chan , QB, "Chandler",   "Chris"    }, // 76"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x32, PP_RB, 0		, 0        , &raf_ande , W2, "Anderson",   "Jamal"    }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0		, 0        , &raf_math , W3, "Mathis",     "Terance"  }, // 70"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0		, 0        , &raf_call , W1, "Calloway",   "Chris"    }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x92, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x34, PP_S , 0        , 0        , &raf_buch , NA, "Buchanan",   "Ray"      }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x58, PP_LB, 0        , 0        , &raf_tugg , NA, "Tuggle",     "Jessie"   }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x41, PP_S , 0        , 0        , &raf_robi , NA, "Robinson",   "Eugene"   }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x56, PP_LB, 0		, 0        , &raf_broo , NA, "Brooking",   "Keith"    }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x5,  PP_K , 0        , 0        , &raf_and2 , NA, "Andersen",   "Morten"   }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Ravens
const struct st_plyr_data spd_rav[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x69, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x75, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x19, PP_QB, 0		, &qbr_mitc, &rbr_mitc , QB, "Mitchell",   "Scott"    }, // 75"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x33, PP_RB, 0		, 0        , &rbr_holm , W1, "Holmes",     "Priest"   }, // 71"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x84, PP_WR, 0		, 0        , &rbr_lewi , W2, "Lewis",      "Jermaine" }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x21, PP_WR, 0		, 0        , &rbr_metc , W3, "Metcalf",    "Eric"     }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x73, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x26, PP_CB, 0        , 0        , &rbr_wood , NA, "Woodson",    "Rod"      }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x37, PP_S , 0        , 0        , &rbr_thom , NA, "Thompson",   "Bennie"   }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x52, PP_LB, 0        , 0        , &rbr_lew2 , NA, "Lewis",      "Ray"      }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x22, PP_CB, 0		, 0        , &rbr_star , NA, "Starks",     "Duane"    }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rbr_stov , NA, "Stover",     "Matt"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Bills
const struct st_plyr_data spd_bil[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x61, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x60, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x70, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x7 , PP_QB, 0        , &qbb_flut, &rbb_flut , QB, "Flutie",     "Doug"     }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x23, PP_RB, 0        , 0        , &rbb_smit , W1, "Smith",      "Antowain" }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0		, 0        , &rbb_moul , W2, "Moulds",     "Eric"     }, // 70"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x85, PP_TE, 0        , 0        , &rbb_riem , W3, "Riemersma",  "Jay"      }, // 72"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x92, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x24, PP_S,  0        , 0        , &rbb_schu , NA, "Schulz",     "Kurt"     }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x27, PP_CB, 0        , 0        , &rbb_irvi , NA, "Irvin",      "Ken"      }, // 72"  PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x56, PP_LB, 0        , 0        , &rbb_cowa , NA, "Cowart",     "Sam"      }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x20, PP_S , 0        , 0        , &rbb_jone , NA, "Jones",      "Henry"    }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x2 , PP_K , 0        , 0        , &rbb_chri , NA, "Christie",   "Steve"    }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Panthers
const struct st_plyr_data spd_pan[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x75, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x7 , PP_QB, 0		, &qcp_beue, &rcp_beue , QB, "Beuerlein",  "Steve"    }, // 77"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x21, PP_RB, 0		, 0        , &rcp_biak , W1, "Biakabatuka","Tim"      }, // 72"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x85, PP_TE, 0		, 0        , &rcp_wall , W2, "Walls",      "Wesley"   }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x87, PP_WR, 0        , 0        , &rcp_muha , W3, "Muhammad",   "Muhsin"   }, // 72"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x69, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x71, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x30, PP_S , 0        , 0        , &rcp_mint , NA, "Minter",	   "Mike"     }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x91, PP_LB, 0		, 0        , &rcp_gree , NA, "Greene",     "Kevin"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x33, PP_CB, 0        , 0        , &rcp_evan , NA, "Evans",      "Doug"     }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x25, PP_CB, 0        , 0        , &rcp_davi , NA, "Davis",      "Eric"     }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x4 , PP_K , 0        , 0        , &rcp_kasa , NA, "Kasay",      "John"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Bears
const struct st_plyr_data spd_bea[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x64, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x58, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x71, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x8 , PP_QB, 0        , &qcb_mcno, &rcb_mcno , QB, "McNown",     "Cade"     }, // 73"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x44, PP_RB, 0		, 0        , &rcb_enis , W1, "Enis",       "Curtis"   }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0		, 0        , &rcb_conw , W3, "Conway",     "Curtis"   }, // 72"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_RB, 0		, 0        , &rcb_engr , W2, "Engram",     "Bobby"    }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x99, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x55, PP_LB, 0        , 0        , &rcb_harr , NA, "Harris",     "Sean"     }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x92, PP_LB, 0        , 0        , &rcb_mint , NA, "Minter",     "Barry"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x37, PP_S , 0        , 0        , &rcb_parr , NA, "Parrish",    "Tony"     }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x27, PP_CB, 0        , 0        , &rcb_har2 , NA, "Harris",     "Walt"     }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x1 , PP_K , 0        , 0        , &rcb_jaeg , NA, "Jaeger",     "Jeff"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Bengals
const struct st_plyr_data spd_ben[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x71, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x8 , PP_QB, 0		, &qci_blak, &rci_blak , QB, "Blake",      "Jeff"     }, // 72"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_RB, 0        , 0        , &rci_dill , W1, "Dillon",     "Corey"    }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0		, 0        , &rci_pick , W3, "Pickens",    "Carl"     }, // 74"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x86, PP_WR, 0        , 0        , &rci_scot , W2, "Scott",      "Darnay"   }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x92, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x96, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x94, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x51, PP_LB, 0		, 0        , &rci_spik , NA, "Spikes",     "Takeo"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x56, PP_LB, 0		, 0        , &rci_simm , NA, "Simmons",    "Brian"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x27, PP_CB, 0        , 0        , &rci_hawk , NA, "Hawkins",    "Artrell"  }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x23, PP_CB, 0        , 0        , &rci_sawy , NA, "Sawyer",     "Corey"    }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x9 , PP_K , 0        , 0        , &rci_pelf , NA, "Pelfrey",    "Doug"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Browns
const struct st_plyr_data spd_brw[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x71, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x67, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x2 , PP_QB, 0		, &qcb_couc, &rcb_couc , QB, "Couch",	   "Tim"      }, // 74"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x42, PP_RB, 0		, 0        , &rcb_kirb , W1, "Kirby",      "Terry"    }, // 70"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x86, PP_WR, 0		, 0        , &rcb_shep , W2, "Shepherd",   "Leslie"   }, // 75"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x85, PP_WR, 0        , 0        , &rcb_john , W3, "Johnson",    "Kevin"    }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x92, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x94, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x96, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x54, PP_LB, 0        , 0        , &rcb_spie , NA, "Spielman",   "Chris"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x23, PP_CB, 0        , 0        , &rcb_pope , NA, "Pope",	   "Marquez"  }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x56, PP_LB, 0		, 0        , &rcb_full , NA, "Fuller",	   "Corey"    }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x95, PP_LB, 0        , 0        , &rcb_mill , NA, "Miller",	   "Jamir"    }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x4 , PP_K , 0        , 0        , &rcb_daws  , NA, "Dawson",	   "Phil"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Cowboys
const struct st_plyr_data spd_cow[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x73, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x61, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x79, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x8 , PP_QB, 0		, &qdc_aikm, &rdc_aikm , QB, "Aikman",     "Troy"     }, // 76"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x22, PP_RB, 0		, 0        , &rdc_smit , W1, "Smith",      "Emmitt"   }, // 69"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x88, PP_WR, 0		, 0        , &rdc_irvi , W2, "Irvin",      "Michael"  }, // 74"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0		, 0        , &rdc_isma , W3, "Ismail",     "Raghib"   }, // 73"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x95, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x21, PP_CB, 0        , 0        , &rdc_sand , NA, "Sanders",    "Deion"    }, // 73"  PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x26, PP_CB, 0        , 0        , &rdc_smit , NA, "Smith",	   "Kevin"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_S , 0        , 0        , &rdc_wood , NA, "Woodson",    "Darren"   }, // 73"  PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x55, PP_LB, 0        , 0        , &rdc_cory , NA, "Coryatt",    "Quentin"  }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rdc_cunn , NA, "Cunningham", "Richie"   }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Broncos
const struct st_plyr_data spd_bro[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x69, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x66, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x6 , PP_QB, 0		, &qdb_bris, &rdb_bris , QB, "Brister",    "Bubby"    }, // 75"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x30, PP_RB, 0		, 0        , &rdb_davi , W1, "Davis",      "Terrell"  }, // 71"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x84, PP_TE, 0		, 0        , &rdb_shar , W2, "Sharpe",     "Shannon"  }, // 74"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x87, PP_WR, 0        , 0        , &rdb_mcca , W3, "McCaffrey",  "Ed"      }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x91, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x34, PP_S , 0        , 0        , &rdb_brax , NA, "Braxton",    "Tyrone"   }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x53, PP_LB, 0        , 0        , &rdb_roma , NA, "Romanowski", "Bill"     }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x51, PP_LB, 0        , 0        , &rdb_mobl , NA, "Mobley",     "John"     }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x34, PP_CB, 0		, 0        , &rdb_cart , NA, "Carter",     "Dale"    }, // 75"  PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x1 , PP_K , 0        , 0        , &rdb_elam , NA, "Elam",       "Jason"    }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Lions
const struct st_plyr_data spd_lio[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x72, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x53, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x64, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x10, PP_QB, 0		, &qdl_batc, &rdl_batc , QB, "Batch",      "Charlie"  }, // 78"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x33, PP_RB, 0		, 0        , &rdl_irvi , W1, "Irvin",      "Sedrick"  }, // 68"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x84, PP_WR, 0		, 0        , &rdl_moor , W2, "Moore",      "Herman"   }, // 75"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x87, PP_WR, 0        , 0        , &rdl_mort , W3, "Morton",     "Johnnie"  }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x91, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x95, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x94, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x50, PP_LB, 0        , 0        , &rdl_clai , NA, "Claiborne",  "Chris"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x27, PP_S , 0        , 0        , &rdl_carr , NA, "Carrier",    "Mark"     }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x57, PP_LB, 0        , 0        , &rdl_boyd , NA, "Boyd",       "Stephen"  }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_CB, 0		, 0        , &rdl_rice , NA, "Rice",	   "Ron"      }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x4 , PP_K , 0        , 0        , &rdl_hans , NA, "Hanson",     "Jason"    }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Packers
const struct st_plyr_data spd_pac[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x73, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x72, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x4 , PP_QB, 0		, &qgp_favr, &rgp_favr , QB, "Favre",      "Brett"    }, // 74"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x25, PP_RB, 0        , 0        , &rgp_leve , W1, "Levens",     "Dorsey"	  }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x86, PP_WR, 0        , 0        , &rgp_free , W2, "Freeman",    "Antonio"  }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x89, PP_TE, 0		, 0        , &rgp_chmu , W3, "Chmura",     "Mark"     }, // 72"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x71, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x36, PP_S , 0		, 0        , &rgp_butl , NA, "Butler",     "LeRoy"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x51, PP_LB, 0        , 0        , &rgp_will , NA, "Williams",   "Brian"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x53, PP_LB, 0        , 0        , &rgp_koon , NA, "Koonce",     "George"   }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x37, PP_CB, 0        , 0        , &rgp_will , NA, "Williams",   "Tyrone"   }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x8 , PP_K , 0        , 0        , &rgp_long , NA, "Longwell",   "Ryan"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Colts
const struct st_plyr_data spd_col[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x73, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x64, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x58, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x18, PP_QB, 0		, &qic_mann, &ric_mann , QB, "Manning",    "Peyton"   }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x32, PP_RB, 0		, 0        , &ric_jame , W1, "James",      "Edgerrin" }, // 70"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x88, PP_WR, 0		, 0        , &ric_harr , W2, "Harrison",   "Marvin"   }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x85, PP_TE, 0		, 0        , &ric_dilg , W3, "Dilger",     "Ken"      }, // 77"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x99, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x62, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x37, PP_S,  0        , 0        , &ric_cota , NA, "Cota",       "Chad"     }, // 75"  PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x97, PP_LB, 0        , 0        , &ric_benn , NA, "Bennett",    "Cornelius"}, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x20, PP_CB, 0        , 0        , &ric_burr , NA, "Burris",     "Jeff"     }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x38, PP_CB, 0        , 0        , &ric_pool , NA, "Poole",      "Tyrone"   }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x12, PP_K , 0        , 0        , &ric_vand , NA, "Vanderjagt", "Mike"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Jaguars
const struct st_plyr_data spd_jag[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x79, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x73, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x71, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x8 , PP_QB, 0		, &qjj_brun, &rjj_brun , QB, "Brunell",    "Mark"     }, // 73"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_RB, 0		, 0        , &rjj_tayl , W1, "Taylor",     "Fred"     }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x82, PP_WR, 0        , 0        , &rjj_smit , W3, "Smith",      "Jimmy"    }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x87, PP_WR, 0        , 0        , &rjj_mcca , W2, "McCardell",  "Keenan"   }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x56, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x97, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x91, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x95, PP_LB, 0		, 0        , &rjj_paup , NA, "Paup",       "Bryce"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x51, PP_LB, 0		, 0        , &rjj_hard , NA, "Hardy",      "Kevin"    }, // 76"  PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x37, PP_CB, 0        , 0        , &rjj_lake , NA, "Lake",       "Carnell"  }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x20, PP_CB, 0        , 0        , &rjj_dari , NA, "Darius",     "Donovin"  }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x1 , PP_K , 0        , 0        , &rjj_holl , NA, "Hollis",     "Mike"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Chiefs
const struct st_plyr_data spd_chi[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x79, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x61, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x68, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x18, PP_QB, 0		, &qkc_grba, &rkc_grba , QB, "Grbac",      "Elvis"    }, // 77"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x39, PP_RB, 0        , 0        , &rkc_morr , W1, "Morris",     "Bam"      }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x89, PP_WR, 0        , 0        , &rkc_riso , W3, "Rison",      "Andre"    }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x88, PP_TE, 0        , 0        , &rkc_gonz , W2, "Gonzalez",   "Tony"     }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x99, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x75, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x53, PP_LB, 0        , 0        , &rkc_patt , NA, "Patton",     "Marvcus"  }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x24, PP_S , 0        , 0        , &rkc_gray , NA, "Gray",       "Carlton"  }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x58, PP_LB, 0		, 0        , &rkc_thom , NA, "Thomas",     "Derrick"  }, // 75"  PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x59, PP_LB, 0        , 0        , &rkc_edwa , NA, "Edwards",    "Donnie"     }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x10, PP_K , 0        , 0        , &rkc_stoy , NA, "Stoyanovich","Pete"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Dolphins
const struct st_plyr_data spd_dol[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x61, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x13, PP_QB, 0		, &qmd_mari, &rmd_mari , QB, "Marino",     "Dan"      }, // 76"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x33, PP_RB, 0		, 0        , &rmd_jabb , W1, "Jabbar",     "Karim-Abdul"},//     PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0		, 0        , &rmd_mcdu , W3, "McDuffie",   "O.J."     }, // 70"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0		, 0        , &rmd_mart , W2, "Martin",     "Tony"     }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x95, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x92, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x54, PP_LB, 0		, 0        , &rmd_thom , NA, "Thomas",     "Zach"     }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x52, PP_LB, 0        , 0        , &rmd_jone , NA, "Jones",      "Robert"   }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x31, PP_S , 0        , 0        , &rmd_mar2 , NA, "Marion",     "Brock"    }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x27, PP_CB, 0        , 0        , &rmd_buck , NA, "Buckley",    "Terrell"  }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x10, PP_K , 0        , 0        , &rmd_mare , NA, "Mare",       "Olindo"   }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Vikings
const struct st_plyr_data spd_vik[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x72, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x64, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x7 , PP_QB, 0		, &qmv_cunn, &rmv_cunn , QB, "Cunningham", "Randall"  }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x26, PP_RB, 0        , 0        , &rmv_smit , W3, "Smith",      "Robert"   }, // 72"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0		, 0        , &rmv_cart , W1, "Carter",     "Cris"     }, // 75"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x84, PP_WR, 0        , 0        , &rmv_moss , W2, "Moss",       "Randy"    }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x96, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x99, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x24, PP_S , 0        , 0        , &rmv_grif , NA, "Griffith",   "Robert"   }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x58, PP_LB, 0        , 0        , &rmv_mcda , NA, "McDaniel",   "Ed"       }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x42, PP_CB, 0        , 0        , &rmv_thom , NA, "Thomas",     "Orlando"  }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x57, PP_LB, 0        , 0        , &rmv_rudd , NA, "Rudd",       "Dwayne"   }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rmv_ande , NA, "Anderson",   "Gary"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Patriots
const struct st_plyr_data spd_pat[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x68, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x64, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x11, PP_QB, 0		, &qnp_bled, &rnp_bled , QB, "Bledsoe",    "Drew"     }, // 77"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x33, PP_RB, 0		, 0        , &rnp_faul , W1, "Faulk",      "Kevin"    }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x87, PP_TE, 0        , 0        , &rnp_coat , W3, "Coates",     "Ben"      }, // 77"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x88, PP_WR, 0		, 0        , &rnp_glen , W2, "Glenn",      "Terry"    }, // 71"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x95, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x55, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x74, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x24, PP_CB, 0        , 0        , &rnp_lawx , NA, "Law",        "Ty"       }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x54, PP_LB, 0        , 0        , &rnp_brus , NA, "Bruschi",    "Tedy"     }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x59, PP_LB, 0        , 0        , &rnp_katz , NA, "Katzenmoyer","Andy"     }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x32, PP_S , 0        , 0        , &rnp_clay , NA, "Clay",     "Willie"   }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_K , 0        , 0        , &rnp_vina , NA, "Vinatieri",  "Adam"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Saints
const struct st_plyr_data spd_sai[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x62, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x67, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x12, PP_QB, 0        , &qns_hobe, &rns_hobe , QB, "Hobert",     "Billy Joe"}, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x34, PP_RB, 0        , 0        , &rns_will , W1, "Williams",   "Ricky"    }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x82, PP_WR, 0		, 0        , &rns_kenn , W3, "Kennison",   "Eddie"    }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x88, PP_WR, 0        , 0        , &rns_hast , W2, "Hastings",   "Andre"    }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x99, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x94, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x43, PP_CB, 0        , 0        , &rns_ambr , NA, "Ambrose",    "Ashley"   }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x22, PP_CB, 0        , 0        , &rns_drak , NA, "Drakeford",  "Tyrone"   }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x55, PP_LB, 0        , 0        , &rns_mitc , NA, "Mitchell",   "Kevin"    }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x29, PP_S , 0		, 0        , &rns_knig , NA, "Knight",     "Sammy"    }, // 67"  PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x10, PP_K , 0        , 0        , &rns_brie , NA, "Brien",      "Doug"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Giants
const struct st_plyr_data spd_gia[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x72, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x53, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x5 , PP_QB, 0        , &qng_coll, &rng_coll , QB, "Collins",    "Kerry"    }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x33, PP_RB, 0        , 0        , &rng_brow , W2, "Brown",      "Gary"     }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x88, PP_WR, 0        , 0        , &rng_hill , W1, "Hilliard",   "Ike"      }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0        , 0        , &rng_toom , W3, "Toomer",     "Amani"    }, // 72"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x92, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x97, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x75, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x31, PP_CB, 0        , 0        , &rng_seho , NA, "Sehorn",     "Jason"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_LB, 0        , 0        , &rng_arms , NA, "Armstead",   "Jesse"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x22, PP_CB, 0        , 0        , &rng_spar , NA, "Sparks",     "Phillippi"}, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x20, PP_S , 0        , 0        , &rng_garn , NA, "Garnes",	   "Sam"      }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rng_dalu , NA, "Daluiso",    "Brad"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Jets
const struct st_plyr_data spd_jet[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x63, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x62, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x75, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x16, PP_QB, 0        , &qnj_test, &rnj_test , QB, "Testaverde", "Vinny"    }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_RB, 0		, 0        , &rnj_mart , W2, "Martin",     "Curtis"   }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x19, PP_WR, 0		, 0        , &rnj_john , W3, "Johnson",    "Keyshawn" }, // 75"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x80, PP_WR, 0        , 0        , &rnj_chre , W1, "Chrebet",    "Wayne"    }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x99, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x95, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x72, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x57, PP_LB, 0        , 0        , &rnj_lewi , NA, "Lewis",	   "Mo"       }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x27, PP_S , 0        , 0        , &rnj_atwa , NA, "Atwater",    "Steve"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x31, PP_CB, 0        , 0        , &rnj_glen , NA, "Glenn",      "Aaron"    }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x51, PP_LB, 0		, 0        , &rnj_coxx , NA, "Cox",        "Bryan"    }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x9 , PP_K , 0        , 0        , &rnj_hall , NA, "Hall",       "John"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Raiders
const struct st_plyr_data spd_rai[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x72, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x63, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x12, PP_QB, 0		, &qor_gann, &ror_gann , QB, "Gannon",     "Rich"     }, // 76"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x26, PP_RB, 0        , 0        , &ror_kauf , W2, "Kaufman",    "Napoleon" }, // 69"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0		, 0        , &ror_brow , W1, "Brown",      "Tim"      }, // 72"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x82, PP_WR, 0        , 0        , &ror_jett , W3, "Jett",       "James"    }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x96, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x67, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x94, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x24, PP_CB, 0        , 0        , &ror_wood , NA, "Woodson",    "Charles"  }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x21, PP_CB, 0        , 0        , &ror_alle , NA, "Allen",      "Eric"     }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x22, PP_S , 0        , 0        , &ror_minc , NA, "Mincy",      "Charles"  }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x54, PP_LB, 0        , 0        , &ror_biek , NA, "Biekert",    "Greg"     }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x5 , PP_K , 0        , 0        , &ror_hust , NA, "Husted",     "Michael"  }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Eagles
const struct st_plyr_data spd_eag[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x72, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x73, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x76, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x5 , PP_QB, 0        , &qpe_mcna, &rpe_mcna , QB, "McNabb",     "Donovan"  }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x22, PP_RB, 0        , 0        , &rpe_stal , W1, "Staley",     "Duce"     }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0        , 0        , &rpe_john , W2, "Johnson",    "Charles"  }, // 72"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0        , 0        , &rpe_smal , W3, "Small",      "Torrance" }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x59, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x79, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x45, PP_S , 0        , 0        , &rpe_hauc , NA, "Hauck",      "Tim"      }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x57, PP_LB, 0        , 0        , &rpe_darl , NA, "Darling",    "James"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x51, PP_LB, 0        , 0        , &rpe_thom , NA, "Thomas",     "William"  }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x23, PP_CB, 0        , 0        , &rpe_vinc , NA, "Vincent",    "Troy"     }, // 72"  PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x9 , PP_K , 0        , 0        , &rpe_joh2 , NA, "Johnson",    "Norm"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Steelers
const struct st_plyr_data spd_ste[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x73, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x10, PP_QB, 0		, &qps_stew, &rps_stew , QB, "Stewart",    "Kordell"  }, // 73"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x36, PP_RB, 0		, 0        , &rps_bett , W2, "Bettis",     "Jerome"   }, // 71"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x87, PP_TE, 0        , 0        , &rps_brue , W1, "Bruener",    "Mark"     }, // 76"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x88, PP_WR, 0        , 0        , NULL	   , W3, "Hawkins",    "Courtney" }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x74, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x76, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x92, PP_LB, 0        , 0        , &rps_gild , NA, "Gildon",     "Jason"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x99, PP_LB, 0        , 0        , &rps_kirk , NA, "Kirkland",   "Levon"    }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x45, PP_CB, 0        , 0        , &rps_davi , NA, "Davis",      "Travis"   }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x41, PP_S , 0        , 0        , &rps_flow , NA, "Flowers",    "Lethon"   }, // 73"  PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rps_brow , NA, "Brown",      "Kris"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Chargers
const struct st_plyr_data spd_cha[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x68, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x70, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x4 , PP_QB, 0        , &qsc_harb, &rsc_harb , QB, "Harbaugh",   "Jim"      }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x20, PP_RB, 0		, 0        , &rsc_mean , W2, "Means",      "Natrone"  }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x81, PP_WR, 0        , 0        , &rsc_grah , W1, "Graham",     "Jeff"     }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x88, PP_TE, 0        , 0        , &rsc_jone , W3, "Jones",      "Freddie"  }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x95, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x54, PP_LB, 0        , 0        , &rsc_hill , NA, "Hill",       "Eric"     }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x55, PP_LB, 0		, 0        , &rsc_seau , NA, "Seau",       "Junior"   }, // 75"  PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x37, PP_S , 0        , 0        , &rsc_harr , NA, "Harrison",   "Rodney"   }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x29, PP_CB, 0        , 0        , &rsc_shaw , NA, "Shaw",       "Terrance" }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rsc_carn , NA, "Carney",     "John"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Fortyniners
const struct st_plyr_data spd_for[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x65, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x66, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x67, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x8 , PP_QB, 0		, &qsf_youn, &rsf_youn , QB, "Young",      "Steve"    }, // 74"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x33, PP_RB, 0        , 0        , &rsf_phil , W2, "Phillips",   "Lawrence" }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x83, PP_WR, 0		, 0        , &rsf_stok , W3, "Stokes",     "J.J. "    }, // 76"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0		, 0        , &rsf_rice , W1, "Rice",       "Jerry"    }, // 74"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x56, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x97, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x94, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x51, PP_LB, 0		, 0        , &rsf_nort , NA, "Norton",     "Ken"      }, // 74"  PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x46, PP_S , 0        , 0        , &rsf_mcdo , NA, "McDonald",   "Tim"      }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x43, PP_S , 0		, 0        , &rsf_hank , NA, "Hanks",      "Merton"   }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x55, PP_LB, 0        , 0        , &rsf_tubb , NA, "Tubbs",      "Winfred"  }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x7 , PP_K , 0        , 0        , &rsf_rich , NA, "Richie",	   "Wade"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Seahawks
const struct st_plyr_data spd_sea[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x71, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x52, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x79, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x7 , PP_QB, 0        , &qss_kitn, &rss_kitn , QB, "Kitna",      "Jon"      }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x32, PP_RB, 0		, 0        , &rss_watt , W1, "Watters",    "Ricky"    }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x82, PP_WR, 0		, 0        , &rss_gall , W3, "Galloway",   "Joey"     }, // 71"  PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x86, PP_TE, 0		, 0        , &rss_faur , W2, "Fauria",     "Christian"}, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x97, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x98, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x24, PP_CB, 0		, 0        , &rss_spri , NA, "Springs",    "Shawn"    }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x20, PP_S , 0        , 0        , &rss_bell , NA, "Bellamy",    "Jay"      }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x94, PP_LB, 0        , 0        , &rss_brow , NA, "Brown",      "Chad"     }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x33, PP_S , 0        , 0        , &rss_will , NA, "Williams",   "Darryl"   }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x2 , PP_K , 0        , 0        , &rss_pete , NA, "Peterson",   "Todd"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Rams
const struct st_plyr_data spd_ram[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x76, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x66, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x72, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x10, PP_QB, 0		, &qsr_gree, &rsr_gree , QB, "Green",      "Trent"    }, //      PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_RB, 0        , 0        , &rsr_faul , W1, "Faulk",      "Marshall" }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x80, PP_WR, 0		, 0        , &rsr_bruc , W2, "Bruce",      "Isaac"    }, // 72"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x88, PP_WR, 0		, 0        , &rsr_holt , W3, "Holt",	   "Torry"    }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x91, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x75, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x79, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x41, PP_CB, 0		, 0        , &rsr_lygh , NA, "Lyght",      "Todd"     }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x59, PP_LB, 0        , 0        , &rsr_coll , NA, "Collins",    "Todd"     }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x35, PP_S , 0        , 0        , &rsr_lyle , NA, "Lyle",       "Keith"    }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x21, PP_CB, 0        , 0        , &rsr_mccl , NA, "McCleon",    "Dexter"   }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x14, PP_K , 0        , 0        , &rsr_wilk , NA, "Wilkens",    "Jeff"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Bucs
const struct st_plyr_data spd_buc[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x74, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x70, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x61, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x12, PP_QB, 0		, &qtb_dilf, &rtb_dilf , QB, "Dilfer",     "Trent"    }, // 76"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x40, PP_RB, 0        , 0        , &rtb_alst , W1, "Alstott",    "Mike"     }, //      PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_RB, 0		, 0        , &rtb_dunn , W2, "Dunn",       "Warrick"  }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x87, PP_WR, 0		, 0        , &rtb_eman , W3, "Emanuel",    "Bert"     }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x72, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x77, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x99, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x21, PP_CB, 0        , 0        , &rtb_abra , NA, "Abraham",    "Donnie"   }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x55, PP_LB, 0        , 0        , &rtb_broo , NA, "Brooks",     "Derrick"  }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x56, PP_LB, 0		, 0        , &rtb_nick , NA, "Nickerson",  "Hardy"    }, // 74"  PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x22, PP_S , 0        , 0        , &rtb_minc , NA, "Mincy",      "Charles"  }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rtb_elez , NA, "Elezovic",   "Peter"    }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Titans
const struct st_plyr_data spd_tit[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x53, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x77, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x74, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x9 , PP_QB, 0		, &qtt_mcna, &rtt_mcna , QB, "McNair",     "Steve"    }, // 74"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x27, PP_RB, 0		, 0        , &rtt_geor , W1, "George",     "Eddie"    }, // 75"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x82, PP_WR, 0		, 0        , &rtt_thig , W3, "Thigpen",    "Yancey"   }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x87, PP_WR, 0		, 0        , &rtt_dyso , W2, "Dyson",      "Kevin"    }, //      PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x78, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x96, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x92, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x29, PP_CB, 0        , 0        , &rtt_lewi , NA, "Lewis",      "Darryll"  }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x56, PP_LB, 0        , 0        , &rtt_mart , NA, "Marts",      "Lonnie"   }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x31, PP_S , 0        , 0        , &rtt_robe , NA, "Robertson",  "Marcus"   }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x58, PP_LB, 0        , 0        , &rtt_bowd , NA, "Bowden",     "Joe"      }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x3 , PP_K , 0        , 0        , &rtt_delg , NA, "Del Greco",  "Al"       }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};
// Redskins
const struct st_plyr_data spd_red[] = {
// spd, pwr,  qb,  wr, off, def,    scale,    size,    color,    #,   pos,   mug img, tmsel img,   name img, ancr, lastname,     frstname
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x62, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x52, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x75, PP_OL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_OLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x14, PP_QB, 0		, &qwr_john, &rwr_john , QB, "Johnson",    "Brad"     }, // 74"  PPOS_QBACK
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x20, PP_RB, 0		, 0        , &rwr_hick , W1, "Hicks",      "Skip"     }, // 70"  PPOS_WPN1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x82, PP_WR, 0		, 0        , &rwr_west , W2, "Westbrook",  "Michael"  }, // 75"  PPOS_WPN2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_WHITE, 0x80, PP_TE, 0        , 0        , &rwr_alex , W3, "Alexander",  "Stephen"  }, //      PPOS_WPN3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x90, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x93, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x96, PP_DL, 0        , 0        , 0         , NA, "",           ""         }, //      PPOS_DLINE3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x35, PP_S , 0        , 0        , &rwr_evan , NA, "Evans",      "Leomont"  }, //      PPOS_DBACK1
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x28, PP_CB, 0        , 0        , &rwr_gree , NA, "Green",      "Darrell"  }, //      PPOS_DBACK2
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_BLACK, 0x24, PP_CB, 0		, 0        , &rwr_bail , NA, "Bailey",     "Champ"    }, //      PPOS_DBACK3
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_FAT , PC_BLACK, 0x57, PP_LB, 0        , 0        , &rwr_harv , NA, "Harvey",     "Ken"      }, //      PPOS_DBACK4
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PS_NORM, PC_WHITE, 0x17, PP_K , 0        , 0        , &rwr_blan , NA, "Blanchard",  "Cary"     }, //      kicker
 { 0,   0,   0,   0,   0,   0,   SC(1.0f), PSUNK  , PCUNK   , NUNK, PPUNK, 0        , 0        , 0         , NA, "",           ""         }  //
};


// Team order-specific tables. To change the team order:
//   a) shuffle the [away_jersey] table rows/columns to make first-to-last
//     order go from left-to-right/top-to-bottom respectively
//   b) shuffle the [teaminfo] table to make first-to-last order go from
//     first-to-last entries respectively
//   c) renumber the [TEAM_name] #defines in <GAME.H> to correspond to the
//     new first-to-last order
//
// All remaining team-order references should be resolved by their dependency
// on the <GAME.H> [TEAM_name] values; (it appears that) the team audit values/
// CMOS offsets will automatically reorder as appropriate when a team-order
// change is made; always try to enclose any new team data elements in the
// [teaminfo] structure, either directly or by handled reference

#define x		0
#if 1	// Blitz 2000 - JTA
char away_jersey[NUM_TEAMS][NUM_TEAMS] = {
//CAR RAV PAN   BEN COW LIO   COL CHI VIK   SAI JET EAG   CHA SEA BUC	RED
//  FAL BIL BEA   BRN BRO PAC   JAG DOL PAT   GIA RAI STE   FOR RAM OIL
 {1,1,1,1,1,1,  1,1,0,1,0,0,  1,1,1,1,1,1,  1,0,0,1,1,0,  1,1,0,0,1,0,  1,},	//CAR
 {x,1,1,1,1,1,  1,1,0,1,0,1,  1,1,0,1,1,1,  1,1,1,1,1,1,  1,0,1,1,1,0,  1,},	//FAL
 {x,x,1,1,1,1,  1,1,1,1,1,1,  1,1,1,1,1,1,  1,1,1,1,1,1,  1,0,1,1,1,0,  0,},	//RAV
 {x,x,x,1,1,1,  1,1,1,1,1,0,  1,1,0,1,1,1,  0,1,1,1,1,0,  1,0,1,1,0,1,  0,},	//BIL
 {x,x,x,x,1,1,  1,1,0,1,1,1,  1,1,0,1,1,1,  1,1,1,1,1,1,  1,0,1,0,1,0,  0,},	//PAN
 {x,x,x,x,x,1,  1,1,1,1,1,1,  1,1,0,1,1,1,  1,1,1,1,1,1,  1,0,1,1,1,0,  1,},	//BEA

 {x,x,x,x,x,x,  1,1,0,1,0,1,  0,0,0,0,1,1,  1,1,1,1,1,1,  1,1,1,1,1,0,  1,},	//BEN
 {x,x,x,x,x,x,  x,1,0,1,0,1,  0,0,0,0,1,1,  1,1,1,1,1,1,  1,1,1,1,1,0,  1,},	//BRN
 {x,x,x,x,x,x,  x,x,1,1,1,0,  1,0,0,0,1,1,  0,1,0,0,0,0,  1,0,1,1,0,1,  0,},	//COW
 {x,x,x,x,x,x,  x,x,x,1,1,1,  1,1,1,1,1,1,  1,1,1,1,1,1,  1,1,1,1,1,1,  1,},	//BRO
 {x,x,x,x,x,x,  x,x,x,x,1,0,  1,1,0,1,1,1,  0,1,0,0,1,0,  1,0,1,0,0,1,  0,},	//LIO
 {x,x,x,x,x,x,  x,x,x,x,x,1,  0,0,0,0,0,1,  1,0,1,1,1,1,  1,0,0,1,1,0,  0,},	//PAC

 {x,x,x,x,x,x,  x,x,x,x,x,x,  1,1,0,1,1,1,  1,1,1,1,1,0,  1,0,1,0,0,1,  0,},	//COL
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,1,0,1,1,1,  0,1,1,1,1,0,  1,0,1,1,0,1,  0,},	//JAG
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,1,0,0,0,  0,0,1,0,0,0,  0,1,0,0,1,0,  1,},	//CHI
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,1,1,1,  0,1,1,1,1,0,  1,0,1,0,0,1,  0,},	//DOL
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,1,1,  0,1,1,1,1,1,  1,0,1,0,0,1,  0,},	//VIK
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,1,  1,1,0,1,1,1,  1,0,1,1,0,1,  0,},	//PAT
 
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  1,1,1,1,1,1,  1,0,0,1,1,0,  1,},	//SAI
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,1,1,1,1,1,  1,0,1,1,0,1,  0,},	//GIA
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,1,1,1,0,  1,0,1,0,0,0,  0,},	//JET
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,1,1,1,  1,0,1,0,1,1,  0,},	//RAI
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,1,1,  1,0,1,1,1,0,  0,},	//EAG
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,1,  1,1,1,1,0,0,  0,},	//STE
 
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  1,0,1,1,0,0,  1,},	//CHA
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,1,0,0,1,0,  1,},	//FOR
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,1,1,0,1,  0,},	//SEA
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,1,1,0,  1,},	//RAM
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,1,0,  1,},	//BUC
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,1,  0,},	//OIL
 
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  1,},	//RED
};
#else	// Blitz 99
char away_jersey[NUM_TEAMS][NUM_TEAMS] = {
//CAR RAV PAN   BEN BRO PAC   JAG DOL PAT   GIA RAI STE   FOR RAM TIT   BRW
//  FAL BIL BEA   COW LIO COL   CHI VIK SAI   JET EAG CHA   SEA BUC RED
 {1,1,1,1,1,1,  1,0,1,0,0,1,  1,1,1,1,1,1,  0,0,1,1,0,1,  1,0,0,1,0,1,  0},	//CAR
 {x,1,1,1,1,1,  1,0,1,0,1,1,  1,0,1,1,1,1,  1,1,1,1,1,1,  0,1,1,1,0,1,  0},	//FAL
 {x,x,1,1,1,1,  1,0,1,1,1,1,  1,1,1,1,1,1,  1,1,1,1,1,1,  0,1,1,1,0,0,  0},	//RAV
 {x,x,x,1,1,1,  1,0,1,1,0,1,  1,0,1,1,1,0,  1,1,1,1,0,1,  0,1,1,0,1,0,  0},	//BIL
 {x,x,x,x,1,1,  1,0,1,1,1,1,  1,0,1,1,1,1,  1,1,1,1,1,1,  0,1,0,1,0,0,  0},	//PAN
 {x,x,x,x,x,1,  1,0,1,1,1,1,  1,0,1,1,1,1,  1,1,1,1,1,1,  0,1,1,1,0,1,  0},	//BEA

 {x,x,x,x,x,x,  1,0,1,0,1,0,  0,0,0,1,1,1,  1,1,1,1,1,1,  1,1,1,1,0,1,  0},	//BEN
 {x,x,x,x,x,x,  x,1,0,0,0,0,  0,0,0,0,0,0,  0,0,0,0,0,0,  0,0,0,0,0,0,  0},	//COW
 {x,x,x,x,x,x,  x,x,1,0,1,1,  1,1,1,1,1,1,  1,1,1,1,1,1,  1,1,1,1,1,1,  0},	//BRO
 {x,x,x,x,x,x,  x,x,x,1,0,1,  1,0,1,1,1,0,  1,0,0,1,0,1,  0,1,0,0,1,0,  0},	//LIO
 {x,x,x,x,x,x,  x,x,x,x,1,0,  0,0,0,0,1,1,  0,1,1,1,1,1,  0,0,1,1,0,0,  0},	//PAC
 {x,x,x,x,x,x,  x,x,x,x,x,1,  1,0,1,1,1,1,  1,1,1,1,0,1,  0,1,0,0,0,0,  0},	//COL

 {x,x,x,x,x,x,  x,x,x,x,x,x,  1,0,1,1,1,0,  1,1,1,1,0,1,  0,1,1,0,1,0,  0},	//JAG
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,1,0,0,0,0,  0,1,0,0,0,0,  1,0,0,1,0,1,  0},	//CHI
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,1,1,1,0,  1,1,1,1,0,1,  0,1,0,0,1,0,  0},	//DOL
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,1,1,0,  1,1,1,1,1,1,  0,1,0,0,1,0,  0},	//VIK
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,1,1,  1,0,1,1,1,1,  0,1,1,0,1,0,  0},	//PAT
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,1,  1,1,1,1,1,1,  0,0,1,1,0,1,  0},	//SAI

 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  1,1,1,1,1,1,  0,1,1,0,1,0,  0},	//GIA
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,1,1,1,0,1,  0,1,0,0,0,0,  0},	//JET
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,1,1,1,1,  0,1,0,1,1,0,  0},	//RAI
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,1,1,1,  0,1,1,1,0,0,  0},	//EAG
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,1,1,  1,1,1,0,0,0,  0},	//STE
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,1,  0,1,1,0,0,1,  0},	//CHA

 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  1,0,0,1,0,1,  0},	//FOR
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,1,1,0,1,0,  0},	//SEA
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,1,1,0,1,  0},	//RAM
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,1,0,1,  0},	//BUC
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,1,0,  0},	//TIT
 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,1,  0},	//RED

 {x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  x,x,x,x,x,x,  1}	//BRW

};
#endif

#undef x

// Team data (in TEAM CITY alpha order)

const struct fbteam_data teaminfo[] = {
// attr				diff   name          home           abbrev prefix static_data pii_logo   pii_logo30 pii_cn    pii_cnb    prl       pti       oneleghel
//{ {1, 1, 3, 4, 2},  4,  "Cardinals",  "Arizona",       "ARZ", "CAR", spd_car, &l_cards,    &l_ariz30, &cn_ariz, &cnb_ariz, &prl_ari, &pti_ari, FALSE },
//{ {1, 0, 3, 2, 3},  4,  "Falcons",    "Atlanta",       "ATL", "FAL", spd_fal, &l_falcons,  &l_atla30, &cn_atla, &cnb_atla, &prl_atl, &pti_atl, TRUE },
//{ {3, 3, 2, 2, 4},  5,  "Ravens",     "Baltimore",     "BAL", "RAV", spd_rav, &l_ravens,   &l_balt30, &ca_balt, &cab_balt, &prl_bal, &pti_bal, FALSE },
//{ {2, 4, 2, 4, 1},  6,  "Bills",      "Buffalo",       "BUF", "BIL", spd_bil, &l_bills,    &l_buff30, &ca_buff, &cab_buff, &prl_buf, &pti_buf, TRUE },
//{ {3, 3, 4, 4, 2},  6,  "Panthers",   "Carolina",      "CAR", "PAN", spd_pan, &l_panthers, &l_caro30, &cn_caro, &cnb_caro, &prl_car, &pti_car, FALSE },
//{ {0, 3, 3, 2, 0},  3,  "Bears",      "Chicago",       "CHI", "BEA", spd_bea, &l_bears,    &l_chic30, &cn_chic, &cnb_chic, &prl_chi, &pti_chi, FALSE },
//{ {4, 1, 2, 3, 1},  4,  "Bengals",    "Cincinnati",    "CIN", "BEN", spd_ben, &l_bengals,  &l_cinc30, &ca_cinc, &cab_cinc, &prl_cin, &pti_cin, TRUE },
//{ {5, 4, 3, 4, 5},  7,  "Cowboys",    "Dallas",        "DAL", "COW", spd_cow, &l_cowboys,  &l_dall30, &cn_dall, &cnb_dall, &prl_dal, &pti_dal, FALSE },
//{ {5, 5, 3, 5, 2}, 10,  "Broncos",    "Denver",        "DEN", "BRO", spd_bro, &l_broncos,  &l_denv30, &ca_denv, &cab_denv, &prl_den, &pti_den, FALSE },
//{ {3, 5, 2, 2, 3},  7,  "Lions",      "Detroit",       "DET", "LIO", spd_lio, &l_lions,    &l_detr30, &cn_detr, &cnb_detr, &prl_det, &pti_det, TRUE },
//{ {5, 3, 5, 4, 3}, 11,  "Packers",    "Green Bay",     "G.B", "PAC", spd_pac, &l_packers,  &l_gree30, &cn_gree, &cnb_gree, &prl_gb,  &pti_gb,  TRUE },
//{ {0, 3, 1, 4, 2},  3,  "Colts",      "Indianapolis",  "IND", "COL", spd_col, &l_colts,    &l_indi30, &ca_indi, &cab_indi, &prl_ind, &pti_ind, TRUE },
//{ {4, 3, 3, 4, 2},  9,  "Jaguars",    "Jacksonville",  "JAC", "JAG", spd_jag, &l_jaguars,  &l_jack30, &ca_jack, &cab_jack, &prl_jac, &pti_jac, TRUE },
//{ {3, 4, 4, 2, 4},  9,  "Chiefs",     "Kansas City",   "K.C", "CHI", spd_chi, &l_chiefs,   &l_kans30, &ca_kans, &cab_kans, &prl_kc,  &pti_kc,  FALSE },
//{ {5, 3, 2, 3, 2},  8,  "Dolphins",   "Miami",         "MIA", "DOL", spd_dol, &l_dolphins, &l_miam30, &ca_miam, &cab_miam, &prl_mia, &pti_mia, FALSE },
//{ {3, 3, 4, 1, 3},  9,  "Vikings",    "Minnesota",     "MIN", "VIK", spd_vik, &l_vikings,  &l_minn30, &cn_minn, &cnb_minn, &prl_min, &pti_min, TRUE },
//{ {5, 4, 2, 5, 3},  8,  "Patriots",   "New England",   "N.E", "PAT", spd_pat, &l_patriots, &l_newe30, &ca_newe, &cab_newe, &prl_ne,  &pti_ne,  TRUE },
//{ {1, 2, 2, 3, 3},  5,  "Saints",     "New Orleans",   "N.O", "SAI", spd_sai, &l_saints,   &l_newo30, &cn_newo, &cnb_newo, &prl_no,  &pti_no,  TRUE },
//{ {1, 3, 3, 2, 3}, 10,  "Giants",     "New York",      "NYG", "GIA", spd_gia, &l_giants,   &l_nygi30, &cn_nygi, &cnb_nygi, &prl_nyg, &pti_nyg, TRUE },
//{ {3, 3, 2, 2, 4},  8,  "Jets",       "New York",      "NYJ", "JET", spd_jet, &l_jets,     &l_nyje30, &ca_nyje, &cab_nyje, &prl_nyj, &pti_nyj, FALSE },
//{ {4, 4, 1, 0, 3},  5,  "Raiders",    "Oakland",       "OAK", "RAI", spd_rai, &l_raiders,  &l_oakl30, &ca_oakl, &cab_oakl, &prl_oak, &pti_oak, TRUE },
//{ {2, 4, 1, 3, 2},  6,  "Eagles",     "Philadelphia",  "PHI", "EAG", spd_eag, &l_eagles,   &l_phil30, &cn_phil, &cnb_phil, &prl_phi, &pti_phi, FALSE },
//{ {4, 2, 2, 4, 2},  9,  "Steelers",   "Pittsburgh",    "PIT", "STE", spd_ste, &l_steelers, &l_pitt30, &ca_pitt, &cab_pitt, &prl_pit, &pti_pit, TRUE },
//{ {2, 2, 2, 3, 3},  5,  "Chargers",   "San Diego",     "S.D", "CHA", spd_cha, &l_chargers, &l_sand30, &ca_sand, &cab_sand, &prl_sd,  &pti_sd,  FALSE },
//{ {5, 3, 4, 3, 2}, 10,	"49ers",      "San Francisco", "S.F", "FOR", spd_for, &l_49ers,    &l_sanf30, &cn_sanf, &cnb_sanf, &prl_sf,  &pti_sf,  TRUE },
//{ {2, 2, 1, 3, 4},  7,  "Seahawks",   "Seattle",       "SEA", "SEA", spd_sea, &L_seahawks, &l_seat30, &ca_seat, &cab_seat, &prl_sea, &pti_sea, TRUE },
//{ {2, 3, 3, 3, 2},  3,	"Rams",       "St. Louis",     "STL", "RAM", spd_ram, &l_rams,     &l_stlo30, &cn_stlo, &cnb_stlo, &prl_sl,  &pti_sl,  TRUE },
//{ {4, 4, 4, 4, 3},  8,  "Buccaneers", "Tampa Bay",     "T.B", "BUC", spd_buc, &l_bucs,     &l_tamp30, &cn_tamp, &cnb_tamp, &prl_tb,  &pti_tb,  TRUE },
//{ {2, 4, 1, 3, 0},  6,  "Titans",     "Tennessee",     "TEN", "TIT", spd_tit, &l_titans,   &l_hous30, &ca_hous, &cab_hous, &prl_ten, &pti_ten, FALSE },
//{ {3, 4, 2, 5, 2},  7,  "Redskins",   "Washington",    "WAS", "RED", spd_red, &l_redskins, &l_wash30, &cn_wash, &cnb_wash, &prl_was, &pti_was, FALSE }

{ {4, 2, 3, 2, 1},  6,  "Cardinals",  "Arizona",       "ARZ", "CAR", spd_car, &l_cards,    &l_ariz30, &cn_ariz, &cnb_ariz, NULL, &pti_ari },
{ {4, 4, 5, 4, 3}, 11,  "Falcons",    "Atlanta",       "ATL", "FAL", spd_fal, &l_falcons,  &l_atla30, &cn_atla, &cnb_atla, NULL, &pti_atl },
{ {2, 2, 1, 2, 3},  4,  "Ravens",     "Baltimore",     "BAL", "RAV", spd_rav, &l_ravens,   &l_balt30, &ca_balt, &cab_balt, NULL, &pti_bal },
{ {3, 4, 4, 4, 2},  9,  "Bills",      "Buffalo",       "BUF", "BIL", spd_bil, &l_bills,    &l_buff30, &ca_buff, &cab_buff, NULL, &pti_buf },
{ {3, 1, 3, 1, 1},  6,  "Panthers",   "Carolina",      "CAR", "PAN", spd_pan, &l_panthers, &l_caro30, &cn_caro, &cnb_caro, NULL, &pti_car },
{ {3, 2, 2, 3, 3},  5,  "Bears",      "Chicago",       "CHI", "BEA", spd_bea, &l_bears,    &l_chic30, &cn_chic, &cnb_chic, NULL, &pti_chi },
{ {4, 2, 3, 1, 2},  3,  "Bengals",    "Cincinnati",    "CIN", "BEN", spd_ben, &l_bengals,  &l_cinc30, &ca_cinc, &cab_cinc, NULL, &pti_cin },
{ {4, 3, 3, 4, 3},  7,  "Browns",     "Cleveland",     "CLE", "BRW", spd_brw, &l_browns,   &l_clev30, &cn_clev, &cnb_clev, NULL, &pti_cle },
{ {4, 4, 4, 3, 4},  8,  "Cowboys",    "Dallas",        "DAL", "COW", spd_cow, &l_cowboys,  &l_dall30, &cn_dall, &cnb_dall, NULL, &pti_dal },
{ {5, 5, 5, 3, 5}, 11,  "Broncos",    "Denver",        "DEN", "BRO", spd_bro, &l_broncos,  &l_denv30, &ca_denv, &cab_denv, NULL, &pti_den },
{ {3, 4, 3, 3, 5},  5,  "Lions",      "Detroit",       "DET", "LIO", spd_lio, &l_lions,    &l_detr30, &cn_detr, &cnb_detr, NULL, &pti_det },
{ {4, 1, 4, 4, 4}, 19,  "Packers",    "Green Bay",     "G.B", "PAC", spd_pac, &l_packers,  &l_gree30, &cn_gree, &cnb_gree, NULL, &pti_gb  },
{ {2, 1, 3, 1, 4},  4,  "Colts",      "Indianapolis",  "IND", "COL", spd_col, &l_colts,    &l_indi30, &ca_indi, &cab_indi, NULL, &pti_ind },
{ {4, 4, 3, 3, 4}, 10,  "Jaguars",    "Jacksonville",  "JAC", "JAG", spd_jag, &l_jaguars,  &l_jack30, &ca_jack, &cab_jack, NULL, &pti_jac },
{ {2, 2, 2, 4, 3},  7,  "Chiefs",     "Kansas City",   "K.C", "CHI", spd_chi, &l_chiefs,   &l_kans30, &ca_kans, &cab_kans, NULL, &pti_kc  },
{ {4, 3, 3, 5, 3}, 10,  "Dolphins",   "Miami",         "MIA", "DOL", spd_dol, &l_dolphins, &l_miam30, &ca_miam, &cab_miam, NULL, &pti_mia },
{ {5, 3, 5, 3, 5}, 11,  "Vikings",    "Minnesota",     "MIN", "VIK", spd_vik, &l_vikings,  &l_minn30, &cn_minn, &cnb_minn, NULL, &pti_min },
{ {3, 1, 4, 3, 4},  9,  "Patriots",   "New England",   "N.E", "PAT", spd_pat, &l_patriots, &l_newe30, &ca_newe, &cab_newe, NULL, &pti_ne  },
{ {2, 1, 1, 1, 4},  7,  "Saints",     "New Orleans",   "N.O", "SAI", spd_sai, &l_saints,   &l_newo30, &cn_newo, &cnb_newo, NULL, &pti_no  },
{ {1, 3, 1, 2, 1},  7,  "Giants",     "New York",      "NYG", "GIA", spd_gia, &l_giants,   &l_nygi30, &cn_nygi, &cnb_nygi, NULL, &pti_nyg },
{ {5, 3, 5, 4, 2}, 11,  "Jets",       "New York",      "NYJ", "JET", spd_jet, &l_jets,     &l_nyje30, &ca_nyje, &cab_nyje, NULL, &pti_nyj },
{ {1, 3, 3, 5, 1},  7,  "Raiders",    "Oakland",       "OAK", "RAI", spd_rai, &l_raiders,  &l_oakl30, &ca_oakl, &cab_oakl, NULL, &pti_oak },
{ {1, 3, 1, 2, 1},  4,  "Eagles",     "Philadelphia",  "PHI", "EAG", spd_eag, &l_eagles,   &l_phil30, &cn_phil, &cnb_phil, NULL, &pti_phi },
{ {1, 4, 1, 4, 2},  6,  "Steelers",   "Pittsburgh",    "PIT", "STE", spd_ste, &l_steelers, &l_pitt30, &ca_pitt, &cab_pitt, NULL, &pti_pit },
{ {1, 3, 1, 5, 3},  4,  "Chargers",   "San Diego",     "S.D", "CHA", spd_cha, &l_chargers, &l_sand30, &ca_sand, &cab_sand, NULL, &pti_sd  },
{ {5, 5, 5, 2, 3}, 10,	"49ers",      "San Francisco", "S.F", "FOR", spd_for, &l_49ers,    &l_sanf30, &cn_sanf, &cnb_sanf, NULL, &pti_sf  },
{ {3, 2, 2, 1, 3},  8,  "Seahawks",   "Seattle",       "SEA", "SEA", spd_sea, &l_seahawks, &l_seat30, &ca_seat, &cab_seat, NULL, &pti_sea },
{ {1, 1, 1, 3, 2},  6,	"Rams",       "St. Louis",     "STL", "RAM", spd_ram, &l_rams,     &l_stlo30, &cn_stlo, &cnb_stlo, NULL, &pti_sl  },
{ {2, 5, 5, 5, 2},  8,  "Buccaneers", "Tampa Bay",     "T.B", "BUC", spd_buc, &l_bucs,     &l_tamp30, &cn_tamp, &cnb_tamp, NULL, &pti_tb  },
{ {2, 4, 4, 3, 4},  8,  "Titans",     "Tennessee",     "TEN", "TIT", spd_tit, &l_titans,   &l_hous30, &ca_hous, &cab_hous, NULL, &pti_ten },
{ {3, 3, 3, 2, 2},  4,  "Redskins",   "Washington",    "WAS", "RED", spd_red, &l_redskins, &l_wash30, &cn_wash, &cnb_wash, NULL, &pti_was },

};

struct diff_set	diff_settings[9] =
{
	//	panic	drone min	human max
	{	99,		0,			10},	// level 3
	{	99,		1,			10},	// level 4
	{	99,		2,			10},	// level 5
	{	99,		4,			9},		// level 6
	{	99,		5,			8},		// level 7
	{	14,		6,			7},		// level 8
	{	 8,		7,			6},		// level 9
	{	 6,		8,			5},		// level 10
	{	 0,	   11,			5},		// level 11
};

#define TEAM_XXX	-1
const int team_ladder[9][5] = {
	{TEAM_FAL,TEAM_BEA,TEAM_COL,TEAM_XXX,-1},
	{TEAM_RAI,TEAM_LIO,TEAM_TIT,TEAM_BRW,-1},
	{TEAM_CHA,TEAM_STE,TEAM_SAI,TEAM_RAM,-1},
	{TEAM_CAR,TEAM_DOL,TEAM_VIK,TEAM_RED,-1},
	{TEAM_BEN,TEAM_PAN,TEAM_SEA,TEAM_JET,-1},
	{TEAM_CHI,TEAM_RAV,TEAM_EAG,TEAM_GIA,-1},
	{TEAM_BIL,TEAM_COW,TEAM_JAG,TEAM_FOR,-1},
	{TEAM_BRO,TEAM_BUC,TEAM_PAT,TEAM_XXX,-1},
	{TEAM_PAC,TEAM_XXX,TEAM_XXX,TEAM_XXX,-1}
	};

const int beat_all_teams[4] = { TEAM_PAC,TEAM_BUC,TEAM_BRO,TEAM_PAT };

char *head_images[] = {
	"hed_mark.wms",
	"hed_bran.wms",
	"hed_dan.wms",
	"hed_dude.wms",
	"hed_jasn.wms",
	"hed_jeff.wms",
	"hed_jen.wms",
	"hed_jim.wms",
	"hed_luis.wms",
	"hed_radn.wms",
	"hed_root.wms",
	"hed_shnk.wms",
	"hed_skul.wms",
	"hed_thug.wms",
	"hed_sal.wms",
	"hed_danf.wms",
	"hed_jonc.wms",
	"hed_mik.wms",
	"hed_bleb.wms",
	"hed_palo.wms"
	};
	
LIMB *head_limbs[] = {
	&limb_markt,
	&limb_brain,
	&limb_dant,
	&limb_dude,
	&limb_jasons,
	&limb_jeffj,
	&limb_jenh,
	&limb_jimg,
	&limb_luism,
	&limb_raiden,
	&limb_johnr,
	&limb_shinok,
	&limb_skull,
	&limb_thug,
	&limb_sald,
	&limb_danf,
	&limb_johnc,
	&limb_mikel,
	&limb_hd_brian,
	&limb_hd_paulo
	};

	
	
