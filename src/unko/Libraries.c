#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#if !defined(WIN32)
#  include <dirent.h>
#else
#  include <windows.h>
#endif
#include "Signature.h"
#include "puts.h"
#include "Str.h"
#include "List.h"
#include "Enviroment.h"
#include "FilePath.h"
#include "File.h"
#include "RomFile.h"
#include "TextFile.h"
#include "ParseList.h"
#include "SearchPath.h"
#include "asardll.h"
#include "Asarctl.h"
#include "Libraries.h"

static bool InsertAsm(RomFile* rom, const char* asmPath, List* libs, List* smwlibs, List* defineList)
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

	libAsm = new_TextFile(asmPath);
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
		asar_reset();
		result = asar_patch(
				tmpAsm->super.path_get(&tmpAsm->super),
				(char*)rom->GetSnesPtr(rom, 0x8000),
				(int)rom->size_get(rom),
				&romlen);
		/* print asm puts */
		asarprints = asar_getprints(&printcnt);
		{
			int i;
			for(i=0; i<printcnt; i++)
			{
				putinfo("%s: %s", libAsm->super.path_get(&libAsm->super), asarprints[i]);
			}
		}
		if(false == result)
		{
			putasarerr();
			puterror("Failed to insert \"%s\"", libAsm->super.path_get(&libAsm->super));
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

	putinfo("%s installed.", libAsm->super.path_get(&libAsm->super));
	delete_TextFile(&libAsm);
	delete_TextFile(&tmpAsm);
	remove(TempAsmName);
	return true;

}

void SearchLibraries(List* asmList, char * const * dirs)
{
	int i;
	FilePath* fp;
#if !defined(WIN32)
	char* ext;
	DIR *dir;
	struct dirent *dp;

	for(i=0; NULL != dirs[i]; i++)
	{
		dir = opendir(dirs[i]);
		if(NULL == dir)
		{
			continue;
		}

		for(dp = readdir(dir); NULL != dp; dp = readdir(dir))
		{
			fp = new_FilePath(Str_concat(dirs[i], dp->d_name));
			ext = Str_copy(fp->ext_get(fp));
			Str_tolower(ext);

			if(0 != strcmp(".asm", ext))
			{
				free(ext);
				delete_FilePath(&fp);
				continue;
			}
			asmList->push(asmList, fp);
		}
	}
#else
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;
	char* path = NULL;

	for(i=0; NULL != dirs[i]; i++)
	{
		path = Str_concat(dirs[i], "*.asm");

		hFind = FindFirstFile(path, &win32fd);
		if(INVALID_HANDLE_VALUE == hFind)
		{
			free(path);
			continue;
		}

		do{
			fp = new_FilePath(Str_concat(dirs[i], win32fd.cFileName));
			asmList->push(asmList, fp);
		} while(FindNextFile(hFind, &win32fd));
		free(path);
	}
#endif
}

static void deleteAsmList(void* d)
{
	delete_FilePath((FilePath**)&d);
}

bool InsertLibraries(RomFile* rom, const char* dirname, List* libs, List* smwlibs, int* cnt, List* defineList)
{
	List* asmList;
	FilePath* asmPath;
	char* dirs[SearchPathNums];

	(*cnt) = 0;

	{
		size_t len;
		char* path;

		len = strlen(dirname);
		path = calloc(len+2, sizeof(char));
		if(NULL == path)
		{
			putfatal("%s: memory error.", __func__);
			return false;
		}
		strcpy_s(path, len+2, dirname);
#if defined(UNIX)
		if('/' != path[len-1])
		{
			path[len] = '/';
			path[len+1] = '\0';
		}
#elif defined(WIN32)
		if(('\\' != path[len-1]) || ('/' != path[len-1]))
		{
			path[len] = '\\';
			path[len+1] = '\0';
		}
#else
#error "This system isn't supported."
#endif
		if(false == ConstructSearchPath(dirs, path))
		{
			return false;
		}
		free(path);
	}


	asmList = new_List(NULL, deleteAsmList);
	if(NULL == asmList)
	{
		putfatal("%s: memory error", __func__);
		DestroySearchPath(dirs);
		return false;
	}

	/* search libraries */
	SearchLibraries(asmList, dirs);

	/* Install libraries */
	asmPath = (FilePath*)asmList->pop(asmList);
	while(NULL != asmPath)
	{
		if(false == InsertAsm(rom, asmPath->path_get(asmPath), libs, smwlibs, defineList))
		{
			DestroySearchPath(dirs);
			return false;
		}

		(*cnt)++;
		delete_FilePath(&asmPath);
		asmPath = (FilePath*)asmList->pop(asmList);
	}

	delete_List(&asmList);
	DestroySearchPath(dirs);
	return true;
}
