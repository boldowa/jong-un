/**
 * libsmw.c
 */
#include <string.h>
#include <stdlib.h>
#include <bolib.h>
#include "common/puts.h"
#include <assert.h>
#include <bolib/file/RomFile.h>
#include "smw/libsmw.h"

bool IsSmw(RomFile* rom)
{
	uint8* ptr;

	assert(rom);
	if(0x80000 > rom->size_get(rom)) return false;

	/* check rom */
	ptr = rom->GetSnesPtr(rom, 0x806f); /* Main loop */
	if(NULL == ptr)
	{
		return false;
	}
	if(0 == memcmp(ptr, (const void*)"\x58\xe6\x13\x20", 4))	/* CLI : INC $13 : JSR $**** */
	{
		uint32 x = read16(ptr+4);	/* It'll get $9322 : GetGamemode */
		ptr = rom->GetSnesPtr(rom, x);
		if(ptr != NULL)
		{
			if(0 == memcmp(ptr+7, (const void*)"\x91\x93\x0f\x94\x6f\x9f\xae\x96", 8))	/* Check Gamemode routine */
			{
				return true;
			}
		}
	}

	return false;
}

bool IsLMInstalled(RomFile* rom)
{
	uint8* p;
	const char LMSig[] = "Lunar Magic Version ";
	const size_t LMSigLen = strlen(LMSig);
	char sver[8];
	double dver;

	assert(rom);
	p = rom->GetSnesPtr(rom, 0x0ff0a0);
	if(NULL == p)
	{
		putdebug("%s: NULL Pointer",__func__);
		return false;
	}
	if(0 != strncmp(LMSig, (char*)p, LMSigLen))
	{
		putdebug("%s: LMSig unmatch",__func__);
		return false;
	}
	strncpy_s(sver, 8, (char*)p+LMSigLen, 4);
	dver = atof(sver);

	if(1.81 > dver)
	{
		putdebug("%s: LMver unmatch",__func__);
		puterror("Note: This tool only supports Lunar Magic 1.81 or higher.");
		return false;
	}

	return true;
}

