/*
 * vc.h
 *
 *  Created on: 2011-07-11
 *      Author: wino
 */

#ifndef VC_H_
#define VC_H_

#include <allegro.h>

#define VC_DLG 19

#define vcOKButton 1

#define vcTypeOponentDoNotWin 3
#define vcTypeTakeAll 4
#define vcTypeTakeSome 7

#define vcTurnsBeforeEnd 5
#define vcCitesConquered 8
#define vcNonActiveList 12
#define vcActiveList 15
#define vcTitleText 16

int d_vc_clr_proc(int msg,DIALOG *d,int c);
char *vc_add_non_active(int idx, int *list_size);
char *vc_add_active(int idx, int *list_size);


int d_move_to_active_proc(int msg,DIALOG *d,int c);
int d_move_to_non_active_proc(int msg,DIALOG *d,int c);
int d_vc_cancelbtn_proc(int msg,DIALOG *d,int c);

extern DIALOG vc_dlg[VC_DLG];
extern char ParseVC[1024];

#endif /* VC_H_ */
