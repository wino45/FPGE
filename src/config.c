/*
 * config.c
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */

#include <stdio.h>
#include "fpge.h"
#include "callback.h"
#include "filename.h"
#include "terr.h"
#include "findname.h"
#include "unit.h"

int save_config(){
	int magic=0, exit_code=0;
	FILE *inf;

	magic+='F';
	magic+=(int)'P'<<8;
	magic+=(int)'G'<<16;
	magic+=(int)'C'<<24;

	char path[MAX_PATH];

	strncpy(path,fpge_config,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"wb");
	if (inf) {
		fwrite(&magic,sizeof(magic),1,inf);
		fwrite(&card_old,sizeof(card_old),1,inf);
		fwrite(&w_old,sizeof(w_old),1,inf);
		fwrite(&h_old,sizeof(h_old),1,inf);
		//filters
		fwrite(&drawNames,sizeof(drawNames),1,inf);
		fwrite(&drawAllNames,sizeof(drawAllNames),1,inf);
		fwrite(&drawTerrain,sizeof(drawTerrain),1,inf);
		fwrite(&drawRoads,sizeof(drawRoads),1,inf);
		fwrite(&showDecimal,sizeof(showDecimal),1,inf);
		fwrite(&showFilter,sizeof(showFilter),1,inf);
		fwrite(&filterTerrain,sizeof(filterTerrain),1,inf);
		fwrite(&filterName,sizeof(filterName),1,inf);
		fwrite(&showCounter,sizeof(showCounter),1,inf);
		fwrite(&drawGndTransport,sizeof(drawGndTransport),1,inf);
		fwrite(&showWeather,sizeof(showWeather),1,inf);
		fwrite(&showHex,sizeof(showHex),1,inf);
		fwrite(&graphical_overide,sizeof(graphical_overide),1,inf);
		fwrite(&scenarioUnitsMode,sizeof(scenarioUnitsMode),1,inf);
		fwrite(&displayAllUnits,sizeof(displayAllUnits),1,inf);
		fwrite(&graphical_overide_hex,sizeof(graphical_overide_hex),1,inf);
		fwrite(&displayUnitsOrder,sizeof(displayUnitsOrder),1,inf);
		fwrite(&show_ranges,sizeof(show_ranges),1,inf);
		fwrite(&show_problems,sizeof(show_problems),1,inf);
		fwrite(&colorizeNames,sizeof(colorizeNames),1,inf);

		//terrain filters and match
		fwrite(&tdTTStrOn,sizeof(tdTTStrOn),1,inf);
		fwrite(&tdRDStrOn,sizeof(tdRDStrOn),1,inf);
		fwrite(&tdTNStrOn,sizeof(tdTNStrOn),1,inf);
		fwrite(&tdGLNStrOn,sizeof(tdGLNStrOn),1,inf);
		fwrite(&tdSDStrOn,sizeof(tdSDStrOn),1,inf);

		fwrite(&tdTTStrMatchOn,sizeof(tdTTStrMatchOn),1,inf);
		fwrite(&tdRDStrMatchOn,sizeof(tdRDStrMatchOn),1,inf);
		fwrite(&tdTNStrMatchOn,sizeof(tdTNStrMatchOn),1,inf);
		fwrite(&tdGLNStrMatchOn,sizeof(tdGLNStrMatchOn),1,inf);
		fwrite(&tdSDStrMatchOn,sizeof(tdSDStrMatchOn),1,inf);

		fwrite(&tdMatchMatchOn,sizeof(tdMatchMatchOn),1,inf);
		fwrite(&GUI_fill_default_tt,sizeof(GUI_fill_default_tt),1,inf);
		//names dialog
		fwrite(&findname_sort,sizeof(findname_sort),1,inf);
		fwrite(&findname_filter,sizeof(findname_filter),1,inf);
		//units
		fwrite(&scenario_units_only_filter,sizeof(scenario_units_only_filter),1,inf);
		fwrite(&exclude_reserved_units_filter,sizeof(exclude_reserved_units_filter),1,inf);

		fwrite(&showHexMatrix,sizeof(showHexMatrix),1,inf);
		fwrite(&showMatrixMode,sizeof(showMatrixMode),1,inf);

		fwrite(&showHexColor,sizeof(showHexColor),1,inf);
		fwrite(&showHexMatrixColor,sizeof(showHexMatrixColor),1,inf);

		fwrite(&sortMatrixMode,sizeof(sortMatrixMode),1,inf);

		fwrite(&drawRoadsDebug,sizeof(drawRoadsDebug),1,inf);
		fwrite(&showDecimalDebug,sizeof(showDecimalDebug),1,inf);
		fwrite(&drawNamesDebug,sizeof(drawNamesDebug),1,inf);
		fwrite(&drawAllNamesDebug,sizeof(drawAllNamesDebug),1,inf);
		fwrite(&colorizeNamesDebug,sizeof(colorizeNamesDebug),1,inf);
		fwrite(&drawTerrainDebug,sizeof(drawTerrainDebug),1,inf);



		fclose(inf);
	}else{
		exit_code= 3; //cfg file cannot be saved
	}
	return exit_code;
}

int load_config(){
	int magic=0, filemagic, exit_code=0;
	FILE *inf;

	magic+='F';
	magic+=(int)'P'<<8;
	magic+=(int)'G'<<16;
	magic+=(int)'C'<<24;

	char path[MAX_PATH];

	strncpy(path,fpge_config,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf) {
		fread(&filemagic,sizeof(filemagic),1,inf);
		if (filemagic==magic){
			fread(&card_old,sizeof(card_old),1,inf);
			fread(&w_old,sizeof(w_old),1,inf);
			fread(&h_old,sizeof(h_old),1,inf);
			//filters
			fread(&drawNames,sizeof(drawNames),1,inf);
			fread(&drawAllNames,sizeof(drawAllNames),1,inf);
			fread(&drawTerrain,sizeof(drawTerrain),1,inf);
			fread(&drawRoads,sizeof(drawRoads),1,inf);
			fread(&showDecimal,sizeof(showDecimal),1,inf);
			fread(&showFilter,sizeof(showFilter),1,inf);
			fread(&filterTerrain,sizeof(filterTerrain),1,inf);
			fread(&filterName,sizeof(filterName),1,inf);
			fread(&showCounter,sizeof(showCounter),1,inf);
			fread(&drawGndTransport,sizeof(drawGndTransport),1,inf);
			fread(&showWeather,sizeof(showWeather),1,inf);
			fread(&showHex,sizeof(showHex),1,inf);
			fread(&graphical_overide,sizeof(graphical_overide),1,inf);
			fread(&scenarioUnitsMode,sizeof(scenarioUnitsMode),1,inf);
			fread(&displayAllUnits,sizeof(displayAllUnits),1,inf);
			fread(&graphical_overide_hex,sizeof(graphical_overide_hex),1,inf);
			fread(&displayUnitsOrder,sizeof(displayUnitsOrder),1,inf);
			fread(&show_ranges,sizeof(show_ranges),1,inf);
			fread(&show_problems,sizeof(show_problems),1,inf);
			fread(&colorizeNames,sizeof(colorizeNames),1,inf);
			//terrain filters and match
			fread(&tdTTStrOn,sizeof(tdTTStrOn),1,inf);
			fread(&tdRDStrOn,sizeof(tdRDStrOn),1,inf);
			fread(&tdTNStrOn,sizeof(tdTNStrOn),1,inf);
			fread(&tdGLNStrOn,sizeof(tdGLNStrOn),1,inf);
			fread(&tdSDStrOn,sizeof(tdSDStrOn),1,inf);

			fread(&tdTTStrMatchOn,sizeof(tdTTStrMatchOn),1,inf);
			fread(&tdRDStrMatchOn,sizeof(tdRDStrMatchOn),1,inf);
			fread(&tdTNStrMatchOn,sizeof(tdTNStrMatchOn),1,inf);
			fread(&tdGLNStrMatchOn,sizeof(tdGLNStrMatchOn),1,inf);
			fread(&tdSDStrMatchOn,sizeof(tdSDStrMatchOn),1,inf);

			fread(&tdMatchMatchOn,sizeof(tdMatchMatchOn),1,inf);
			fread(&GUI_fill_default_tt,sizeof(GUI_fill_default_tt),1,inf);
			//names dialog
			fread(&findname_sort,sizeof(findname_sort),1,inf);
			fread(&findname_filter,sizeof(findname_filter),1,inf);
			//units
			fread(&scenario_units_only_filter,sizeof(scenario_units_only_filter),1,inf);
			fread(&exclude_reserved_units_filter,sizeof(exclude_reserved_units_filter),1,inf);

			fread(&showHexMatrix,sizeof(showHexMatrix),1,inf);
			fread(&showMatrixMode,sizeof(showMatrixMode),1,inf);

			fread(&showHexColor,sizeof(showHexColor),1,inf);
			fread(&showHexMatrixColor,sizeof(showHexMatrixColor),1,inf);

			fread(&sortMatrixMode,sizeof(sortMatrixMode),1,inf);

			fread(&drawRoadsDebug,sizeof(drawRoadsDebug),1,inf);
			fread(&showDecimalDebug,sizeof(showDecimalDebug),1,inf);
			fread(&drawNamesDebug,sizeof(drawNamesDebug),1,inf);
			fread(&drawAllNamesDebug,sizeof(drawAllNamesDebug),1,inf);
			fread(&colorizeNamesDebug,sizeof(colorizeNamesDebug),1,inf);
			fread(&drawTerrainDebug,sizeof(drawTerrainDebug),1,inf);
		}else{
			exit_code= 2;// bad magic number
		}
		fclose(inf);
	}else{
		exit_code= 1; //cfg file not found
	}
	//do some checks
	//if (show_problems==1)  {
	//	drawRoads=1;
	//	mark_road_problems();
	//}
	//do some checks
	//if (show_problems==2)  {
	//	mark_city_problems();
	//}
	return exit_code;
}
