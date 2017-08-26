/**
 * Objects.c
 */
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <ctype.h>
#include <setjmp.h>
#include "Signature.h"
#include "puts.h"
#include "Str.h"
#include "List.h"
#include "Enviroment.h"
#include "ReadWrite.h"
#include "FilePath.h"
#include "File.h"
#include "RomFile.h"
#include "TextFile.h"
#include "ParseList.h"
#include "SearchPath.h"
#include "Funex.h"
#include "asardll.h"
#include "Asarctl.h"
#include "Libraries.h"
#include "Objects.h"

enum {
	Match_Export = 0,
	Match_Visible,
	Match_XSize,
	Match_YSize,
	Match_HorzElongationAmount,
	Match_VertElongationAmount,
	UnmatchPrint
};

enum {
	Match_True = 0,
	Match_False,
	Match_On,
	Match_Off,
	UnmatchBool
};

#define ExportDeclarator		"export"
#define VisibleDeclarator		"visible"
#define XSizeDeclarator			"xsize"
#define YSizeDeclarator			"ysize"
#define HorzElongationAmount		"horzelongationamount"
#define VertElongationAmount		"vertelongationamount"

typedef struct InsertInf {
	char*  path;
	uint32 loc;
} InsertInf;

static void* CloneInsertInf(const void* srcv)
{
	InsertInf* src;
	InsertInf* clone;
	char* path;

	if(NULL == srcv) return NULL;

	src = (InsertInf*)srcv;

	path = Str_copy(src->path);
	clone = calloc(1, sizeof(InsertInf));

	if((NULL == path) || (NULL == clone))
	{
		free(path);
		free(clone);
		return NULL;
	}
	strcpy_s(path, strlen(src->path)+1, src->path);
	clone->loc = src->loc;
	clone->path = path;

	return clone;
}

static void DeleteInsertInf(void* tgt)
{
	InsertInf* inf;
	if(NULL == tgt) return;

	inf = (InsertInf*)tgt;
	free(inf->path);
	free(inf);
}

static bool MatchMainLabel(const char* line)
{
	size_t len;
	size_t i;

	len = strlen(line);
	i = 0;

	SkipSpaces(line, &i, len);

	/* match main label */
	if(0 != strncmp("main:", &line[i], 5))
	{
		return false;
	}
	i += 5;

	if((len == i) || IsSpace(line[i]))
	{
		return true;
	}

	return false;
}

static bool MatchExportValue(const char* str, char* declaration, void* data)
{
	size_t len;
	size_t i;
	size_t st;

	int hex = 0;
	LabelDataStruct *lab = (LabelDataStruct*)data;

	len = strlen(str);
	i = 0;

	if('$' == str[i]) i++;
	st = i;

	/* check valid hex */
	for(; i<len; i++)
	{
		if(false == ishex(str[i]))
		{
			break;
		}
	}
	SkipSpaces(str, &i, len);
	if(i != len)
	{
		putwarn("Invalid export syntax. This export is ignored.");
		free(declaration);
		return false;
	}

	/* get hex val */
	i = st;
	for(; i<len; i++)
	{
		if(false == ishex(str[i])) break;
		hex <<= 4;
		hex = hex + atoh(str[i]);
	}
	if(hex >= 0x1000000)
	{
		putwarn("The export address is too big. This export is ignored.");
		free(declaration);
		return false;
	}

	lab->name = declaration;
	lab->loc = hex;

	return true;
}

static bool MatchString(const char* str, void* data)
{
	size_t len;
	size_t i;
	size_t cmplen;
	const char* cmp = (const char*)data;

	len = strlen(str);
	cmplen = strlen(cmp);

	if(0 != strncasecmp(cmp, str, cmplen)) return false;

	
	i = cmplen;
	SkipSpaces(str, &i, len);
	if(i != len) return false;

	return true;
}
static bool MatchBooleanValue(const char* str, char* declaration, void* data)
{
	bool* b = (bool*)data;

	FunexStruct fs[] = {
		{ MatchString, "true" },
		{ MatchString, "false" },
		{ MatchString, "on" },
		{ MatchString, "off" },
		/*----------------*/
		{ NULL, NULL }
	};

	free(declaration);	/* I don't need it */

	switch(FunexMatch(str, fs))
	{
		case Match_True:
		case Match_On:
			(*b) = true;
			return true;

		case Match_False:
		case Match_Off:
			(*b) = false;
			return false;

		default:
			break;
	}
	return false;
}

static bool MatchIntegerValue(const char* str, char* declaration, void* data)
{
	size_t i;
	size_t ed;

	int* val = (int*)data;
	
	free(declaration);	/* I don't need it */
	ed = strlen(str) - 1;
	SkipSpacesRev(str, &ed);

	i = 0;
	for(; i<ed; i++)
	{
		if(false == isdigit(str[i]))
		{
			break;
		}
	}
	if(ed != i) return false;

	(*val) = atoi(str);
	return true;
}

static bool MatchPrintCommon(const char* str, void* data,
		const char* searchStr, bool (*valueAnalyzer)(const char*, char*, void*))
{
	size_t len;
	size_t i;
	char* declaration;
	size_t declen;
	size_t st;

	if(NULL == valueAnalyzer)
	{
		putfatal("%s: Program error: invalid program, valueAnalyzer is NULL..", __func__);
		return false;
	}
	if(NULL == data)
	{
		putfatal("%s: Program error: data address is not assigned.", __func__);
		return false;
	}

	len = strlen(str);
	i = 0;

	SkipSpaces(str, &i, len);

	/* match */
	if(0 != strncasecmp(searchStr, &str[i], strlen(searchStr)))
	{
		return false;
	}
	i += strlen(searchStr);
	/* fail if str doesn't have space after searchstr. */
	if(false == IsSpace(str[i]))
	{
		return false;
	}

	SkipSpaces(str, &i, len);
	st = i;

	/* read until '=' */
	SkipUntilChar(str, &i, '=', len);
	if(i == len) return false;

	declen = i - st;
	declaration = calloc(declen+1, sizeof(char));
	if(NULL == declaration)
	{
		putfatal("%s: memory error.", __func__);
		return false;
	}
	strncpy_s(declaration, declen+1, &str[st], declen);
	CutOffTailSpaces(declaration);

	/* skip a char */
	i++;

	SkipSpaces(str, &i, len);
	if(i == len)
	{
		free(declaration);
		return false;
	}

	return valueAnalyzer(&str[i], declaration, data);
}

static bool MatchExport(const char* line, void* data)
{
	return MatchPrintCommon(line, data, ExportDeclarator, MatchExportValue);
}
static bool MatchVisible(const char* line, void* data)
{
	return MatchPrintCommon(line, data, VisibleDeclarator, MatchBooleanValue);
}
static bool MatchXSize(const char* line, void* data)
{
	return MatchPrintCommon(line, data, XSizeDeclarator, MatchIntegerValue);
}
static bool MatchYSize(const char* line, void* data)
{
	return MatchPrintCommon(line, data, YSizeDeclarator, MatchIntegerValue);
}
static bool MatchHorzElongationAmount(const char* line, void* data)
{
	return MatchPrintCommon(line, data, HorzElongationAmount, MatchIntegerValue);
}
static bool MatchVertElongationAmount(const char* line, void* data)
{
	return MatchPrintCommon(line, data, VertElongationAmount, MatchIntegerValue);
}

static bool InsertedCheck(const void* sval, const void* lval)
{
	char* path;
	InsertInf *inf;

	path = (char*)sval;
	inf = (InsertInf*)lval;

	return (0 == strcmp(path, inf->path));
}

uint16 GetCodeSize(RomFile* rom, const uint32 codeadr)
{
	uint8* ptr;

	ptr = rom->GetSnesPtr(rom, codeadr);
	return (uint16)(read16(ptr-4) + 1);
}

static bool InsertAsm(
		RomFile* rom,
		char** dirs,
		const char* name,
		const uint32 adr,
		List* libs,
		List* insList,
		List* defineList)
{
	int i;
	jmp_buf  e;
	struct stat sts;
	FilePath* objPath = NULL;
	TextFile* objAsm;
	TextFile* tmpAsm;
	const char* linebuf;
	char* path = NULL;
	Iterator* lnode;

	/* for asar */
	bool result;
	LabelDataStruct* lab;
	Define *def;
	int romlen = (int)rom->size_get(rom);
	InsertInf* inf;
	bool mainDefined;
	uint32 codeadr;

	ObjectProperty property = {
		true,	/* visible	: Visible */
		1,	/* xsize	: XSize */
		1,	/* ysize	: YSize */
		1,	/* horzea	: HorzElongationAmount */
		1,	/* vertea	: VertElongationAmount */
	};

	LabelDataStruct export;
	const char* const * asarprints;
	int   printcnt;
	FunexStruct fs[] = {
		{ MatchExport, &export },
		{ MatchVisible, &property.visible },
		{ MatchXSize, &property.xsize },
		{ MatchYSize, &property.ysize },
		{ MatchHorzElongationAmount, &property.horzea },
		{ MatchVertElongationAmount, &property.vertea },
		/* --- */
		{ NULL, NULL }
	};

	/* search object asm */
	for(i=0; NULL != dirs[i]; i++)
	{
		free(path);
		path = Str_concat(dirs[i], name);
#if !defined(WIN32) && !defined(_WIN32)
		if(0 == stat(path, &sts) && S_ISREG(sts.st_mode))
#else
		if(0 == stat(path, &sts) && (0 != (_S_IREAD & sts.st_mode)))
#endif
		{
			objPath = new_FilePath(path);
			break;
		}
	}
	free(path);

	if(NULL == objPath)
	{
		puterror("\"%s\" not found.", name);
		return false;
	}

	/* skip assemble if src is already inserted. */
	lnode = insList->search(insList, objPath->path_get(objPath), InsertedCheck);
	if(NULL != lnode)
	{
		uint8* ptr;
		inf = (InsertInf*)lnode->data(lnode);

		putinfo("  %s is already inserted. update pointer (to $%06x). ", objPath->path_get(objPath), inf->loc);
		ptr = rom->GetSnesPtr(rom, adr);
		write24(ptr, inf->loc);
		delete_FilePath(&objPath);
		return true;
	}

	path = Str_concat(objPath->dir_get(objPath), TempAsmName);
	if(NULL == path)
	{
		putfatal("%s: memory error.", __func__);
		return false;
	}

	objAsm = new_TextFile(objPath->path_get(objPath));
	tmpAsm = new_TextFile(path);
	free(path);
	delete_FilePath(&objPath);

	if(0 == setjmp(e))
	{
		/* open check */
		if((NULL == objAsm) || (NULL == tmpAsm))
		{
			longjmp(e,1);
		}

		/* file open */
		if(FileOpen_NoError != objAsm->Open2(objAsm, "r"))
		{
			puterror("Can't open \"%s\"", objAsm->super.path_get(&objAsm->super));
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
		for(lnode = libs->begin(libs); NULL != lnode; lnode=lnode->next(lnode))
		{
			lab = lnode->data(lnode);
			tmpAsm->Printf(tmpAsm, "org $%06x\n", lab->loc);
			tmpAsm->Printf(tmpAsm, "%s:\n", lab->name);
		}
		/* -- table code -- */
		tmpAsm->Printf(tmpAsm, "org $%06x\n", adr);
		tmpAsm->Printf(tmpAsm, "dl main\n", adr);
		/* -- main code -- */
		tmpAsm->Printf(tmpAsm, "!map = %d\n", rommap.val);
		i = 0;
		mainDefined = false;
		linebuf = objAsm->GetLine(objAsm);
		while(NULL != linebuf)
		{
			i++;
			if(true == MatchMainLabel(linebuf))
			{
				/* multiple declare check */
				if(true == mainDefined)
				{
					puterror("%s: line %d: \"main\" label redefined.", objAsm->super.path_get(&objAsm->super), i);
					longjmp(e, 1);
				}
				tmpAsm->Printf(tmpAsm, "freecode\n");
				mainDefined = true;
			}
			tmpAsm->Printf(tmpAsm, "%s\n", linebuf);
			linebuf = objAsm->GetLine(objAsm);
		}

		/* close */
		tmpAsm->super.Close(&tmpAsm->super);
		objAsm->super.Close(&objAsm->super);

		if(false == mainDefined)
		{
			puterror("%s: \"main\" label not found.", objAsm->super.path_get(&objAsm->super));
			longjmp(e, 1);
		}

		/* patch */
		asar_reset();
		result = asar_patch(
				tmpAsm->super.path_get(&tmpAsm->super),
				(char*)rom->GetSnesPtr(rom, 0x8000),
				(int)rom->size_get(rom),
				&romlen);
		/* get prints and export */
		asarprints = asar_getprints(&printcnt);
		{
			int i;
			for(i=0; i<printcnt; i++)
			{
				switch(FunexMatch(asarprints[i], fs))
				{
					case Match_Export:
						putdebug("Match export output: %s: $%06x", export.name, export.loc);

						lab = malloc(sizeof(LabelDataStruct));
						if(NULL == lab)
						{
							putfatal("%s: memory error.", __func__);
							free(export.name);
							longjmp(e, 1);
						}
						memcpy(lab, &export, sizeof(LabelDataStruct));
						libs->push(libs, lab);
						break;

					case Match_Visible:
					case Match_XSize:
					case Match_YSize:
					case Match_HorzElongationAmount:
					case Match_VertElongationAmount:
						putdebug("  Property updated: %s", asarprints[i]);
						break;

					default:
						putinfo("  %s: %s", objAsm->super.path_get(&objAsm->super), asarprints[i]);
						break;
				}
			}
		}
		/* check result */
		if(false == result)
		{
			putasarerr();
			puterror("Failed to assemble \"%s\"", objAsm->super.path_get(&objAsm->super));
			longjmp(e, 1);
		}
		/* Check locatio */
		codeadr = (uint32)asar_getlabelval("main");
		if(0x108000 > (codeadr & 0x7fffff))
		{
			puterror("%s: Probably empty asm file.(loc = $%06x)", objAsm->super.path_get(&objAsm->super), codeadr);
			longjmp(e, 1);
		}

		/* add inserted file list */
		inf = calloc(1, sizeof(InsertInf));
		path = Str_copy(objAsm->super.path_get(&objAsm->super));
		if(NULL == path || NULL == path)
		{
			putfatal("%s: memory error.", __func__);
			free(inf);
			free(path);
			longjmp(e,1);
		}
		inf->loc = codeadr;
		inf->path = path;
		insList->push(insList, inf);
	}
	else
	{
		delete_TextFile(&objAsm);
		delete_TextFile(&tmpAsm);
		return false;
	}

	putinfo("  \"%s\" inserted at $%06x. (Size: 0x%x bytes)", objAsm->super.path_get(&objAsm->super), codeadr, GetCodeSize(rom, codeadr));
	delete_TextFile(&objAsm);
	remove(tmpAsm->super.path_get(&tmpAsm->super));
	delete_TextFile(&tmpAsm);

	return true;
}

uint32 Get2DObjTblAdr(RomFile* rom, const uint32 adrMain)
{
	uint8* p;
	uint16 szMain;
	uint32 tbladr;

	tbladr = adrMain;
	/* get data pointer */
	p = rom->GetSnesPtr(rom, adrMain);
	/* get data size */
	szMain = read16(&p[4]);

	/* move to MAIN sig */
	p = p + (int)szMain + 9 -5;
	tbladr = tbladr + (uint32)szMain + 9 -7;

	if(0 != memcmp("MAIN", p, 4))
	{
		if(0 != memcmp("MAIN", --p, 4))
		{
			return ROMADDRESS_NULL;
		}
		tbladr--;	/* fix */
	}

	/* get table address */
	tbladr -= (uint32)(SigLen + 0x300);

	return tbladr;
}

bool InsertObjects(
		RomFile* rom,
		const char* dirname,
		const uint32 adrMain,
		const InsertListStruct* lst,
		const InsertListGroupStruct* Grps,
		List* libs,
		int* cnt,
		List* defineList)
{
	const InsertListRangeStruct* range;
	int i,j;
	uint32 base;
	uint32 tbladr;
	char* dirs[SearchPathNums];
	List* insList;
	uint8* ptr;

	(*cnt) = 0;

	insList = new_List(CloneInsertInf, DeleteInsertInf);
	if(NULL == insList)
	{
		putfatal("%s: memory error.", __func__);
		return false;
	}

	{
		size_t len;
		char* path;

		len = strlen(dirname);
		path = calloc(len+2, sizeof(char));
		if(NULL == path)
		{
			putfatal("%s: memory error.", __func__);
			delete_List(&insList);
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
			free(path);
			delete_List(&insList);
			return false;
		}
		free(path);
	}

	for(i=0; Grps[i].ranges != NULL; i++)
	{
		putinfo("[%s]", GrpName[i]);
		if(ROMADDRESS_NULL == Grps[i].sa)
		{
			base = Get2DObjTblAdr(rom, adrMain);
			if(ROMADDRESS_NULL == base)
			{
				putfatal("%s: Obj2D table is missing...", __func__);
				DestroySearchPath(dirs);
				delete_List(&insList);
				return false;
			}
		}
		else
		{
			base = Grps[i].sa;
		}
		for(range = Grps[i].ranges; 0 < range->max; range++)
		{
			for(j=range->min; j <= range->max; j++)
			{
				if(NULL == lst->ngroup[i][j])
				{
					/* nothing to do */
					continue;
				}

				/* get object pointer */
				tbladr = base + ((uint32)j*3);

				/* empty check */
				ptr = rom->GetSnesPtr(rom, tbladr);
				if(range->empty != read24(ptr))
				{
					puterror("Object %s-%02x isn't empty. (Maybe it has been changed with other tools.)", GrpName[i], j);
					return false;
				}

				/* Insert object */
				putinfo("  Insert %02x", j);
				if(false == InsertAsm(rom, dirs, lst->ngroup[i][j], tbladr, libs, insList, defineList))
				{
					DestroySearchPath(dirs);
					delete_List(&insList);
					return false;
				}

				(*cnt)++;
			}
		}
	}

	DestroySearchPath(dirs);
	delete_List(&insList);
	return true;
}
