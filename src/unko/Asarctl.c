/**
 * @file Asarctl.h
 */
#include <string.h>
#include <stdlib.h>
#include <bolib.h>
#include <assert.h>
#include "common/puts.h"
#include <bolib/file/RomFile.h>
#include "asar/asardll.h"
#include "unko/Asarctl.h"

RomMapStruct rommap;

void DeleteLabelDataStruct(void* data)
{
	LabelDataStruct* lab = (LabelDataStruct*)data;

	assert(lab);
	free(lab->name);
	free(lab);
}

void putasarerr()
{
	int nums;
	int ctr;

	const struct errordata* err;

	err = asar_geterrors(&nums);

	for(ctr=0; ctr<nums; ctr++)
	{
		puterror("%s", err[ctr].fullerrdata);
	}

}

void DeleteLibraryList(void* data)
{
	LabelDataStruct* lab = (LabelDataStruct*)data;

	assert(lab);
	free(lab->name);
	free(lab);
}

void getmapmode(RomFile* rom)
{
	memset((char*)rommap.name, '\0', sizeof(char)*10);
	rommap.val = MT_Unknown;

	putdebug("Rom: %s (%dK bytes)", rom->path_get(rom), (int)(rom->size_get(rom)/1024));
	putdebug("RomType: %02x", rom->type_get(rom));
	putdebug("RomMap: %02x", rom->mapmode_get(rom));

	switch(rom->mapmode_get(rom))
	{
		case MapMode_20:
		case MapMode_20H:
			strcpy_s((char*)rommap.name, 10, "lorom");
			rommap.val  = MT_LoRom;
			return;

		case MapMode_21:
		case MapMode_21H:
			strcpy_s((char*)rommap.name, 10, "hirom");
			rommap.val  = MT_HiRom;
			return;

		case MapMode_SA1:
			strcpy_s((char*)rommap.name, 10, "sa1rom");
			rommap.val  = MT_SA1Rom;
			return;

		case MapMode_25:
		case MapMode_25H:
			switch(rom->type_get(rom))
			{
				case RomType_ExLoRom:
					strcpy_s((char*)rommap.name, 10, "exlorom");
					rommap.val  = MT_ExLoRom;
					return;

				case RomType_ExHiRom:
					strcpy_s((char*)rommap.name, 10, "exhirom");
					rommap.val  = MT_ExHiRom;
					return;

				case RomType_LoRom:
				case RomType_HiRom:
				case RomType_Unknown:
				default:
					break;
			}

		case MapMode_SPC7110:
		case MapMode_Reserved:
		case MapMode_Unknown:
		default:
			break;
	}

	putwarn("Unknown map type.");
	return;
}
