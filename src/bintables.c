/*
 * bintables.c
 *
 *  Created on: 12-07-2012
 *      Author: wino
 */

#include <stdio.h>

#include "fpge.h"
#include "load.h"
#include "move.h"
#include "filename.h"

int load_classic_bin_tables(){

		char path[MAX_PATH];
		FILE *inf;
		//int error=0;
		long offset=-1;

		sprintf(path,"%s%s","..\\",exe_ag);
		canonicalize_filename(path,path,MAX_PATH);
		inf = fopen(path, "rb");
		if (!inf){
			sprintf(path,"%s%s","..\\",exe_pgwin);
			canonicalize_filename(path,path,MAX_PATH);
			inf = fopen(path, "rb");
			if (!inf){
				//now panzer exe
				sprintf(path,"%s%s","..\\exe\\",exe_panzer);
				canonicalize_filename(path,path,MAX_PATH);
				inf = fopen(path, "rb");
				if (!inf){
					strncpy(path, exe_panzer,MAX_PATH);
					canonicalize_filename(path,path,MAX_PATH);
					inf = fopen(path, "rb");
					if (!inf)
						return ERROR_PANZER_EXE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
				}
			}
		}
		fseek(inf, 0, SEEK_END);
		long file_size = ftell(inf);

		//Weather

		//FPGE will understand only two ag.exe and one PG.exe and two panzer.exe sizes:
		if (file_size==2161664) offset=1861744; //ag.exe
		if (file_size==2167611) offset=1867520; //ag.exe
		if (file_size==2135040) offset=1825100; //pg.exe
		if (file_size==811233) offset=774917; //panzer.exe
		if (file_size==814305) offset=777989; //panzer.exe

		if (offset==-1) {
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_UNKNOWN_EXE;
		}
		fseek(inf,offset,SEEK_SET);

		int blocks_read = fread(PgStaticWeatherTable,PG_WEATHER_BIN_TABLE_SIZE,1,inf);
		if (blocks_read!=1){
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_WRONG_WEATHER_BINTAB;
		}

		//movement table
		if (file_size==2161664) offset=1871736; //ag.exe ???????????????
		if (file_size==2167611) offset=1877513; //ag.exe
		if (file_size==2135040) offset=1835096; //pg.exe
		if (file_size==811233) offset=784903; //panzer.exe
		if (file_size==814305) offset=787975; //panzer.exe

		fseek(inf,offset,SEEK_SET);

		blocks_read = fread(PgStaticMoveTable,PG_MOVE_BIN_TABLE_SIZE,1,inf);
		if (blocks_read!=1){
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_WRONG_MOVE_BINTAB;
		}
		fclose(inf);

		return 0;
}

int load_pgf_bin_tables(){

		char path[MAX_PATH];
		FILE *inf;
		int i,buf;
		int blocks_read;
		long offset=-1;

		sprintf(path,"%s%s","..\\..\\",exe_pgf);
		canonicalize_filename(path,path,MAX_PATH);
		inf = fopen(path, "rb");
		if (!inf){
			sprintf(path,"%s",exe_pgf);
			canonicalize_filename(path,path,MAX_PATH);
			inf = fopen(path, "rb");
			if (!inf){
				return ERROR_PGFOREVER_EXE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
			}
		}
		fseek(inf, 0, SEEK_END);
		long file_size = ftell(inf);

		//Weather

		//FPGE will understand only these PGForever sizes:
		if (file_size==757760) offset=527392; //PGForexer.exe 1.01

		if (offset==-1) {
			fclose(inf);
			return ERROR_PGFOREVER_EXE_BASE+ERROR_FPGE_UNKNOWN_EXE;
		}
		fseek(inf,offset,SEEK_SET);

		blocks_read=0;
		for(i=0;i<PG_WEATHER_BIN_TABLE_SIZE;i++){
			blocks_read += fread(&buf,4,1,inf);
			PgStaticWeatherTable[i]=(unsigned char)buf;
		}
		if (blocks_read!=PG_WEATHER_BIN_TABLE_SIZE){
			fclose(inf);
			return ERROR_PGFOREVER_EXE_BASE+ERROR_FPGE_WRONG_WEATHER_BINTAB;
		}

		//movement table
		if (file_size==757760) offset=525080; //PGForexer.exe 1.01

		fseek(inf,offset,SEEK_SET);

		blocks_read=0;
		for(i=0;i<PGF_MOVE_BIN_TABLE_SIZE;i++){
			blocks_read += fread(&buf,4,1,inf);
			if (buf==-100) buf=254; // ALL
			if (buf==-200) buf=255; // N/A
			PgStaticMoveTable[i]=(unsigned char)buf;
		}
		if (blocks_read!=PGF_MOVE_BIN_TABLE_SIZE){
			fclose(inf);
			return ERROR_PGFOREVER_EXE_BASE+ERROR_FPGE_WRONG_MOVE_BINTAB;
		}
		fclose(inf);

		return 0;
}

int load_pacgen_bin_tables(){

		char path[MAX_PATH];
		FILE *inf;
		//int error=0;
		long offset=-1;

		sprintf(path,"%s%s","..\\",exe_pacgen);
		canonicalize_filename(path,path,MAX_PATH);
		inf = fopen(path, "rb");
		if (!inf){
			return ERROR_PACGEN_EXE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
		}
		fseek(inf, 0, SEEK_END);
		long file_size = ftell(inf);

		//Weather

		//FPGE will understand only two ag.exe and one PG.exe and two panzer.exe sizes:
		if (file_size==976896) offset=0xd4ff0; //pacgen.exe v1.2

		if (offset==-1) {
			fclose(inf);
			return ERROR_PACGEN_EXE_BASE+ERROR_FPGE_UNKNOWN_EXE;
		}
		fseek(inf,offset,SEEK_SET);

		int blocks_read = fread(PgStaticWeatherTable,PACGEN_WEATHER_BIN_TABLE_SIZE,1,inf);
		if (blocks_read!=1){
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_WRONG_WEATHER_BINTAB;
		}

		//movement table
		if (file_size==976896) offset=0xd4a68; //pacgen.exe v1.2

		fseek(inf,offset,SEEK_SET);

		blocks_read = fread(PgStaticMoveTable,PACGEN_MOVE_BIN_TABLE_SIZE,1,inf);
		if (blocks_read!=1){
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_WRONG_MOVE_BINTAB;
		}
		fclose(inf);

		return 0;
}
