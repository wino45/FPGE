/*
 * callback.h
 *
 *  Created on: 2010-03-10
 *      Author: wino
 */

#ifndef CALLBACK_H_
#define CALLBACK_H_

extern DIALOG generate_dlg[9];
extern DIALOG rtnfix_dlg[9];

short get_id_from_2_bytes(int i);
int draw_new_unique_names();
int ctrl_alt_n_keycallback();
int draw_new_all_names();
int ctrl_a_keycallback();
void mark_problems();
int draw_new_terain();
int ctrl_t_keycallback();
int draw_new_road();
int push_new_road();
int ctrl_alt_r_keycallback();
int draw_new_display_names();
int ctrl_d_keycallback();
int ctrl_alt_f_keycallback();
int ctrl_alt_s_keycallback();
int ctrl_x_keycallback();
int ctrl_e_keycallback();
int ctrl_b_keycallback();
int ctrl_g_keycallback();
int ctrl_alt_m_keycallback();
int generate_dialog();
void do_fix_roads();
void do_fix_names();
void do_fix_names_hard();
void do_fix_terrain_types();
void do_fix_sea_air_transport();
void do_fix_owner(int from, int to);
void list_conversion_of_units(int,int,int);
int ctrl_q_keycallback();
int ctrl_y_keycallback();
int ctrl_p_keycallback();
int draw_new_gnd_transport();
int ctrl_l_keycallback();
int draw_new_weather();
int ctrl_w_keycallback();
int draw_new_hex();
int draw_new_flag();
int ctrl_h_keycallback();
int ctrl_i_keycallback();
int ctrl_z_keycallback();
int draw_new_scenario_units();
int ctrl_alt_u_keycallback();
int change_scenarioUnitsMode();
int draw_new_all_units();
int ctrl_v_keycallback();
int ctrl_c_keycallback();
int draw_new_unit_order();
int ctrl_o_keycallback();
int draw_new_spottings();
int ctrl_k_keycallback();
int ctrl_j_keycallback();

int key_1_keycallback();
int key_2_keycallback();
int key_3_keycallback();
int key_4_keycallback();
int key_5_keycallback();
int key_6_keycallback();
int key_7_keycallback();
int key_8_keycallback();
int key_9_keycallback();

int ctrl_1_keycallback();
int ctrl_2_keycallback();
int ctrl_3_keycallback();
int ctrl_4_keycallback();
int ctrl_5_keycallback();
int ctrl_6_keycallback();
int ctrl_7_keycallback();
int ctrl_8_keycallback();
int ctrl_9_keycallback();

int f1_keycallback();
int f2_keycallback();
int f3_keycallback();
int f4_keycallback();
int f5_keycallback();
int f6_keycallback();
int f7_keycallback();
int f8_keycallback();
int f9_keycallback();
int f10_keycallback();
int f11_keycallback();
int f12_keycallback();

void draw_tiles_matrix();
void position_gui_elements(int , int );
void mark_road_problems();
void mark_city_problems();



#endif /* CALLBACK_H_ */
