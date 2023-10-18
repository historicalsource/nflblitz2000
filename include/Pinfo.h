#ifndef _PINFO__H
#define _PINFO__H

extern ARROWINFO		arrowinfo[];
extern int				update_status_box;
extern image_info_t		gameclock;
extern int				no_result_box;

void change_ostring_string(sprite_node_t * psn, ostring_t * string, char * psz, float xpos, int justify);
void update_ostring_string(ostring_t * string, char * psz);
void update_ostring_string_nonalpha(ostring_t * string, char * psz);

void whois_qb_idiot_box_proc(int *args);
void kick_meter_proc(int *args);
void punt_meter_proc(int *args);
void show_end_quarter_proc(int *args);
void showhide_pinfo(int flag);
void showhide_status_box(int flag);
void plyrinfo_init(void);
void instructions_proc(int *args);
void score_status_box(int *args);
void opening_msg_proc(int *args);
void yardage_info_plate(int *args);
int result_box( int );
void display_fumble_proc(int *);
void show_sack_proc(int *);

#endif