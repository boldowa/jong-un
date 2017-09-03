/**
 * Libraries.c
 */
#include "common/types.h"
#include <setjmp.h>
#include "unko/Signature.h"
#include "common/puts.h"
#include "common/Str.h"
#include "common/List.h"
#include "common/Enviroment.h"
#include "file/File.h"
#include "file/RomFile.h"
#include "file/TextFile.h"
#include "asar/asardll.h"
#include "unko/Asarctl.h"
#include "unko/LibsInsertMan.h"
#include "unko/Libraries.h"

static bool IsUnkoLibData(const uint8* data, const uint32 len)
{
	if((SigLen+5) > len)
	{
		return false;
	}

	/* move to tail */
	data += len;

	/* Term + "LIB_" */
	data -= 5;
	if(0 != memcmp("LIB_", data, 4))
	{
		/* I don't know the reason, but it's well displaced. */
		if(0 != memcmp("LIB_", --data, 4))
		{
			return false;
		}
	}

	/* Signature */
	data -= SigLen;
	if(0 != strcmp(Signature, (char*)data))
	{
		return false;
	}

	return true;
}

static bool InsertAsm(
		RomFile* rom,
		const char* path,
		List* smwlibs,
		List* libs,
		List* defineList
	      )
{
	jmp_buf  e;
	TextFile* libAsm;
	TextFile* tmpAsm;
	const char* linebuf;
	Iterator* lnode;

	/* for asar */
	bool result;
	const struct labeldata* labels;
	LabelDataStruct* lab;
	Define* def;
	char* labname;
	int labcnt;
	int romlen = (int)rom->size_get(rom);
	const char* const * asarprints;
	int printcnt;

	libAsm = new_TextFile(path);
	tmpAsm = new_TextFile(TempAsmName);

	if(0 == setjmp(e))
	{
		/* open check */
		if((NULL == libAsm) || (NULL == tmpAsm))
		{
			longjmp(e,1);
		}

		/* file open */
		if(FileOpen_NoError != libAsm->Open2(libAsm, "r"))
		{
			puterror("Can't open \"%s\"", libAsm->super.path_get(&libAsm->super));
			longjmp(e,1);
		}
		if(FileOpen_NoError != tmpAsm->Open2(tmpAsm, "w"))
		{
			puterror("Can't open \"%s\"", tmpAsm->super.path_get(&tmpAsm->super));
			longjmp(e,1);
		}

		/* generate tmpasm */
		tmpAsm->Printf(tmpAsm, "%s\n", rommap.name);
		/* -- defines -- */
		for(lnode = defineList->begin(defineList); NULL != lnode; lnode=lnode->next(lnode))
		{
			def = lnode->data(lnode);
			tmpAsm->Printf(tmpAsm, "!%s = %s\n", def->name, def->val);
		}
		/* -- lib code -- */
		for(lnode = smwlibs->begin(smwlibs); NULL != lnode; lnode=lnode->next(lnode))
		{
			lab = lnode->data(lnode);
			tmpAsm->Printf(tmpAsm, "org $%06x\n", lab->loc);
			tmpAsm->Printf(tmpAsm, "%s:\n", lab->name);
		}
		/* -- main code -- */
		tmpAsm->Printf(tmpAsm, "!map = %d\n", rommap.val);
		tmpAsm->Printf(tmpAsm, "freecode\n");
		linebuf = libAsm->GetLine(libAsm);
		while(NULL != linebuf)
		{
			tmpAsm->Printf(tmpAsm, "%s\n", linebuf);
			linebuf = libAsm->GetLine(libAsm);
		}
		/* Signature */
		tmpAsm->Printf(tmpAsm, "\n");
		tmpAsm->Printf(tmpAsm, "db\t\"" Signature "\", $00\n");
		tmpAsm->Printf(tmpAsm, "db\t\"LIB_\", $00\n");

		/* close */
		tmpAsm->super.Close(&tmpAsm->super);
		libAsm->super.Close(&libAsm->super);

		/* patch */
		putdebug("asar_pattch \"%s\"", tmpAsm->super.path_get(&tmpAsm->super));
		asar_reset();
		result = asar_patch(
				tmpAsm->super.path_get(&tmpAsm->super),
				(char*)rom->GetSnesPtr(rom, 0x8000),
				(int)rom->size_get(rom),
				&romlen);
		putdebug("result: %d", (int)result);
		/* print asm puts */
		asarprints = asar_getprints(&printcnt);
		{
			int i;
			for(i=0; i<printcnt; i++)
			{
				putinfo("  %s: %s", libAsm->super.path_get(&libAsm->super), asarprints[i]);
			}
		}
		if(false == result)
		{
			putasarerr();
			puterror("  Failed to insert \"%s\"", libAsm->super.path_get(&libAsm->super));
			longjmp(e, 1);
		}

		/* get library label */
		labels = asar_getalllabels(&labcnt);
		{
			int i;
			for(i = 0; i < labcnt; i++)
			{
				if(0 == strncmp(":pos_", labels[i].name, 5))	/* + labs */
				{
					continue;
				}
				if(0 == strncmp(":neg_", labels[i].name, 5))	/* - labs */
				{
					continue;
				}
				if(0 == strncmp("SMW", labels[i].name, 3))	/* smw libraries */
				{
					continue;
				}

				labname = Str_copy(labels[i].name);
				lab = malloc(sizeof(LabelDataStruct));
				if((NULL == labname) || (NULL == lab))
				{
					putfatal("%s : memory error", __func__);
					free(labname);
					free(lab);
					longjmp(e, 1);
				}
				lab->loc = labels[i].location;
				lab->name = labname;
				libs->push(libs, lab);
			}
		}
	}
	else
	{
		delete_TextFile(&libAsm);
		delete_TextFile(&tmpAsm);
		return false;
	}

	putinfo("  %s installed.", libAsm->super.path_get(&libAsm->super));
	delete_TextFile(&libAsm);
	delete_TextFile(&tmpAsm);
	remove(TempAsmName);
	return true;
}

bool InsertLibraries(
		RomFile* rom,
		List* labels,
		LibsInsertMan* libsInsMan,
		List* smwlibs,
		List* libs,
		int* libscnt,
		List* defines)
{
	Iterator *itLab;
	LibraryFileItem* fileItem;
	const char* label;

	for(itLab = labels->begin(labels); NULL != itLab; itLab = itLab->next(itLab))
	{
		label = (const char*)itLab->data(itLab);
		fileItem = libsInsMan->searchLibrary(libsInsMan, label);

		/* Already installed check */
		if(fileItem->isInserted)
		{
			continue;
		}

		putinfo("  Install %s ...", fileItem->libpath);
		if(false == InsertAsm(rom, fileItem->libpath, smwlibs, libs, defines))
		{
			return false;
		}
		(*libscnt)++;
	}
	return true;
}


bool UninstallLibs(RomFile* rom)
{
	uint32 sa = 0x108000;

	while(sa != ROMADDRESS_NULL)
	{
		sa = rom->RatsSearch(rom, sa, IsUnkoLibData);
		if(ROMADDRESS_NULL != sa)
		{
			putdebug("Uninstall lib at $%06x", sa);
			if(false == rom->RatsClean(rom, sa))
			{
				putdebug("Libcode uninstall failed at 0x%06x", sa);
				return false;
			}
			putinfo("Libcode uninstalled from 0x%06x", sa);
		}
	}

	return true;
}
