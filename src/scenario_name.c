/*
 * scenario_name.c
 *
 *  Created on: 10-07-2012
 *      Author: wino
 */
#include "fpge.h"
#include "pgf.h"
#include "load.h"
#include "filename.h"

void GetScenarioShortName(char *desc, int desc_size){
	char tmp_line[128];

	//Name
	if (pgf_mode){
	  snprintf(desc,desc_size,"%s",block1_Name);
	}else{
	  convert_from_cp1250_to_utf8(tmp_line,scn_short_description[getScenarioNumber()-1],SHORT_SCN_SIZE);
	  snprintf(desc,desc_size,"%s",tmp_line);
	}

}

void GetScenarioDescription(char *desc, int desc_size){
	char tmp_line[128];

	//Name
	if (pgf_mode){
	  snprintf(desc,desc_size,"%s",block1_Description);
	}else{
	  convert_from_cp1250_to_utf8(tmp_line,scn_long_description[getScenarioNumber()-1],SHORT_SCN_SIZE);
	  snprintf(desc,desc_size,"%s",tmp_line);
	}
}
