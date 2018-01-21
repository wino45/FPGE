/*
 * ag.h
 *
 *  Created on: 2010-08-05
 *      Author: wino
 */

#ifndef AG_H_
#define AG_H_

#define AG_LONG_DES 256
#define AG_SHORT_DES 1024

int load_tileart();
int load_ag_nupl();
int load_ag_scenario_desc();

int save_ag_scenario(int n, int save_type);
int save_ag(int save_type);
int load_ag_scn(int scenario_number, int show_info);

#endif /* AG_H_ */
