/**
 * Asarctl.h
 */
#pragma once

typedef struct LabelDataStruct {
	char* name;
	int loc;
} LabelDataStruct;

typedef struct Define {
	char* name;
	char* val;
}Define;

typedef enum {
	MT_Unknown = 0,
	MT_LoRom,
	MT_HiRom,
	MT_SA1Rom,
	MT_ExLoRom,
	MT_ExHiRom,
} MapType;

typedef struct RomMapStruct {
	const char name[10];
	MapType    val;
} RomMapStruct;

extern RomMapStruct rommap;

void putasarerr();
void DeleteLabelDataStruct(void* data);

#ifndef RomFile
#  define _Asarctl_Temprorary_RomFileDefine
   typedef struct _RomFile RomFile;
#endif
void getmapmode(RomFile*);
#ifdef _Asarctl_Temprorary_RomFileDefine
#  undef RomFile
#endif

