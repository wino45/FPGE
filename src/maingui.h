/*
 * maingui.h
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */

#include <allegro.h>
#include "callback.h"
#include "fpge.h"

#ifndef MAINGUI_H_
#define MAINGUI_H_

//these are the indexes of components for the main dialog dm
#define dmUnitBtnIdx 1
#define dmUlistBtnIdx dmUnitBtnIdx+1
#define dmTerBtnIdx dmUlistBtnIdx+1
#define dmVictBtnIdx dmTerBtnIdx+1
#define dmOwnBtnIdx dmVictBtnIdx+1
#define dmLoadBtnIdx dmOwnBtnIdx+1
#define dmSaveBtnIdx dmLoadBtnIdx+1
#define dmExitBtnIdx dmSaveBtnIdx+1
#define dmMapBmpIdx dmExitBtnIdx+1
#define dmLeftArrowIdx dmMapBmpIdx+1
#define dmRightArrowIdx dmLeftArrowIdx+1
#define dmUpArrowIdx dmRightArrowIdx+1
#define dmDownArrowIdx dmUpArrowIdx+1
#define dmHSlideIdx dmDownArrowIdx+1
#define dmVSlideIdx dmHSlideIdx+1
#define dmGndRBIdx dmVSlideIdx+1
#define dmAirRBIdx dmGndRBIdx+1
#define dmSeaRBIdx dmAirRBIdx+1
#define dmNoneRBIdx dmSeaRBIdx+1
#define dmMapInfoTxtIdx dmNoneRBIdx+1
#define dmMapStatusIdx dmMapInfoTxtIdx+1
#define dmPlaceBtnIdx dmMapStatusIdx+1
#define dmExpBtnIdx dmPlaceBtnIdx+1
#define dmStrBtnIdx dmExpBtnIdx+1
#define dmEntBtnIdx dmStrBtnIdx+1
#define dmCoreRBIdx dmEntBtnIdx+1
#define dmAuxRBIdx  dmCoreRBIdx+1
#define dmAlliedRBIdx dmAuxRBIdx+1
#define dmScenBtnIdx dmAlliedRBIdx+1
#define dmTileBtnIdx dmScenBtnIdx+1
#define dmDeployBtnIdx dmTileBtnIdx+1
#define dmNeutralBtnIdx dmDeployBtnIdx+1
#define dmHelpBtnIdx dmNeutralBtnIdx+1
#define dmSearchRBtnIdx dmHelpBtnIdx+1
#define dmRectImportBtnIdx dmSearchRBtnIdx+1
#define dmRectExportBtnIdx dmRectImportBtnIdx+1
#define dmFilterStatusTxtIdx dmRectExportBtnIdx+1
#define dmAlliedAuxRBIdx 94

#define MAIN_DLG_SIZE 98
#define LOAD_LAYER_DLG_SIZE 10
#define UNITS_BMP_DLG_SIZE (9+5+4)
#define COUNTRY_UNITS_BMP_DLG_SIZE (15)

extern DIALOG main_dlg[MAIN_DLG_SIZE];
extern DIALOG load_dlg[7+4];
extern DIALOG saveas_dlg[7+5+1];
extern DIALOG configuration_dlg[7+3];
extern DIALOG load_layer_dlg[LOAD_LAYER_DLG_SIZE];
extern DIALOG units_bmp_dlg[UNITS_BMP_DLG_SIZE];
extern DIALOG country_units_bmp_dlg[COUNTRY_UNITS_BMP_DLG_SIZE];

int about_box();
int help_dialog();
int change_weather();
int change_unique_names();
int change_display_names();
int change_spottings();
int new_map_dialog();
int load_dialog();
int do_scenario_dialog();
int units_place_mode();
int unit_dialog();
int unit_list_dialog();
int unitsum_dialog();
int search_replace_dialog();
int map_resize_dialog();

int do_rectangle_import();
int do_rectangle_export();

int entrench_mode();
int strength_mode();
int expirience_mode();
int transport_mode();
int select_fragment_mode();
int place_fragment_mode();
int flag_display_toggle();
int owner_mode();
int victory_mode();
int deploy_mode();
int neutral_mode();
int save_dialog();
int save_scenario_procedure();
int save_as_dialog();
int configuration_dialog();
int compare_by_name_and_country(const void *ii, const void *jj);
int compare_by_old_id_and_country(const void *ii, const void *jj);
int compare_by_old_id(const void *ii, const void *jj);

void check_and_save_mapnames();
int load_map_layer_dialog();
int export_units_bmp_dialog();
int export_country_units_bmp_dialog();

#endif /* MAINGUI_H_ */
