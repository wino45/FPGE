/*
 * colors.h
 *
 *  Created on: 2010-07-10
 *      Author: wino
 */

#ifndef FPGE_COLORS_H_
#define FPGE_COLORS_H_

extern int fpge_mask_color;
extern int fpge_colors_bits;
extern int fpge_colors_bits_shift;

int make_color_fpge(int r, int g, int b);
int find_pal_element(int color_to_find);
int find_pal_element92(int color_to_find);
int colors_to24bits(int color_to_convert);
int colors_to24bits_ex(int color_to_convert, int swap);
void initialize_dm();
void prepare_dialogs();
void set_pal();
void set_color_mode();
void set_weather_palette();
void dialog_to24bits(DIALOG *dialog_to_convert, int size);

#endif /* COLORS_H_ */
