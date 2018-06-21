/**
 * Objects.c
 */
#include <setjmp.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <bolib.h>
#include <bolib/file/RomFile.h>
#include <bolib/file/TextFile.h>
#include "common/puts.h"
#include "common/Funex.h"
#include "common/Environment.h"
#include "unko/Signature.h"
#include "unko/ParseList.h"
#include "unko/SearchPath.h"
#include "asar/asardll.h"
#include "unko/Asarctl.h"
#include "unko/LibsInsertMan.h"
#include "unko/Libraries.h"
#include "unko/ParseObjPuts.h"
#include "unko/Objects.h"

/**
 * InsertGroup
 */
static InsertListRangeStruct range1[] = {
	{0x29, 0x2c, NrmObjEmpty},
	{0x2e, 0x2f, NrmObjEmpty},
	{-1, -1}
};
static InsertListRangeStruct range2[] = {
	{0x29, 0x2c, NrmObjEmpty},
	{0x2e, 0x33, NrmObjEmpty},
	{-1, -1}
};
static InsertListRangeStruct range3[] = {
	{0x29, 0x2c, NrmObjEmpty},
	{0x2e, 0x31, NrmObjEmpty},
	{-1, -1}
};
static InsertListRangeStruct range4[] = {
	{0x29, 0x2c, NrmObjEmpty},
	{0x2e, 0x33, NrmObjEmpty},
	{-1, -1}
};
static InsertListRangeStruct range5[] = {
	{0x29, 0x2c, NrmObjEmpty},
	{-1, -1}
};
static InsertListRangeStruct range6[] = {
	{0x02, 0x0f, 0x000000},
	{0x98, 0xff, 0x0da6d1},
	{-1, -1}
};
static InsertListRangeStruct range7[] = {
	{0x00, 0xff, 0x0da8c3},
	{-1, -1}
};
static InsertListGroupStruct Grps[] = {
	{ 0x0da452, range1 }, /* Normal */
	{ 0x0dc197, range2 }, /* Castle */
	{ 0x0dcd97, range3 }, /* Rope */
	{ 0x0dd997, range4 }, /* Underground */
	{ 0x0de897, range5 }, /* Ghosthouse */
	{ 0x0da10f, range6 }, /* ExObj */
	{ ROMADDRESS_NULL, range7 }, /* 2D */

	{ ROMADDRESS_NULL, NULL }
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

enum {
	Match_Export = 0,
	Match_Visible,
	Match_XSize,
	Match_YSize,
	Match_HorzElongationAmount,
	Match_VertElongationAmount,
	UnmatchPrint
};
static bool MatchExport(const char* line, void* data)
{
	return ParseObjPuts(line, data, ExportDeclarator, MatchExportValue);
}
static bool MatchVisible(const char* line, void* data)
{
	return ParseObjPuts(line, data, VisibleDeclarator, MatchBooleanValue);
}
static bool MatchXSize(const char* line, void* data)
{
	return ParseObjPuts(line, data, XSizeDeclarator, MatchIntegerValue);
}
static bool MatchYSize(const char* line, void* data)
{
	return ParseObjPuts(line, data, YSizeDeclarator, MatchIntegerValue);
}
static bool MatchHorzElongationAmount(const char* line, void* data)
{
	return ParseObjPuts(line, data, HorzElongationAmount, MatchIntegerValue);
}
static bool MatchVertElongationAmount(const char* line, void* data)
{
	return ParseObjPuts(line, data, VertElongationAmount, MatchIntegerValue);
}

static bool InsertedCheck(const void* sval, const void* lval)
{
	char* path;
	InsertInf *inf;

	path = (char*)sval;
	inf = (InsertInf*)lval;

	return (0 == strcmp(path, inf->path));
}

static uint16 GetCodeSize(RomFile* rom, const uint32 codeadr)
{
	uint8* ptr;

	ptr = rom->GetSnesPtr(rom, codeadr);
	return (uint16)(read16(ptr-4) + 1);
}

static bool GenerateTempAsm(
		TextFile* objAsm,
		TextFile* tmpAsm,
		List* defineList,
		List* smwlibs,
		List* libs,
		uint32 adr,
		int objGroup,
		int objIndex)
{
	int i;
	Iterator* lnode;
	LabelDataStruct* lab;
	Define *def;
	const char* linebuf;
	bool mainDefined;

	/* file open */
	if(FileOpen_NoError != objAsm->open2(objAsm, "r"))
	{
		puterror("Can't open \"%s\"", objAsm->path_get(objAsm));
		return false;
	}
	if(FileOpen_NoError != tmpAsm->open2(tmpAsm, "w"))
	{
		puterror("Can't open \"%s\"", tmpAsm->path_get(tmpAsm));
		return false;
	}

	/* generate tmpasm */
	tmpAsm->printf(tmpAsm, "%s\n", rommap.name);
	/* -- defines -- */
	for(lnode = defineList->begin(defineList); NULL != lnode; lnode=lnode->next(lnode))
	{
		def = lnode->data(lnode);
		tmpAsm->printf(tmpAsm, "!%s = %s\n", def->name, def->val);
	}
	/* -- smwlib code -- */
	for(lnode = smwlibs->begin(smwlibs); NULL != lnode; lnode=lnode->next(lnode))
	{
		lab = lnode->data(lnode);
		tmpAsm->printf(tmpAsm, "org $%06x\n", lab->loc);
		tmpAsm->printf(tmpAsm, "%s:\n", lab->name);
	}
	/* -- lib code -- */
	for(lnode = libs->begin(libs); NULL != lnode; lnode=lnode->next(lnode))
	{
		lab = lnode->data(lnode);
		tmpAsm->printf(tmpAsm, "org $%06x\n", lab->loc);
		tmpAsm->printf(tmpAsm, "%s:\n", lab->name);
	}
	/* -- table code -- */
	tmpAsm->printf(tmpAsm, "org $%06x\n", adr);
	tmpAsm->printf(tmpAsm, "dl main\n", adr);
	/* -- main code -- */
	tmpAsm->printf(tmpAsm, "!map = %d\n", rommap.val);
	tmpAsm->printf(tmpAsm, "!object_group = %d\n", objGroup);
	tmpAsm->printf(tmpAsm, "!object_number = %d\n", objIndex);
	i = 0;
	mainDefined = false;
	linebuf = objAsm->getline(objAsm);
	while(NULL != linebuf)
	{
		i++;
		if(true == MatchMainLabel(linebuf))
		{
			/* multiple declare check */
			if(true == mainDefined)
			{
				puterror("%s: line %d: \"main\" label redefined.", objAsm->path_get(objAsm), i);
				return false;
			}
			tmpAsm->printf(tmpAsm, "freecode\n");
			mainDefined = true;
		}
		tmpAsm->printf(tmpAsm, "%s\n", linebuf);
		linebuf = objAsm->getline(objAsm);
	}

	/* close */
	tmpAsm->close(tmpAsm);
	objAsm->close(objAsm);

	if(false == mainDefined)
	{
		puterror("%s: \"main\" label not found.", objAsm->path_get(objAsm));
		return false;
	}

	return true;
}


/**
 * @brief Get undefined symbol for search the library function
 *
 * @param errmsg error message generated by asar
 * @param symTypeStr Symbol type string data
 *
 * @return NULL: unknown error / NOT NULL: symbol string data
 */
static char* GetUndefinedSymbol(const char* const errmsg, const char* const symTypeStr)
{
	jmp_buf e;
	char* work;
	size_t i;
	size_t beg;
	size_t len;
	char* ssearch;
	char* symbol = NULL;

	work = Str_copy(errmsg);
	len = strlen(work);
	if(0 == setjmp(e))
	{
		ssearch = strstr(work, symTypeStr);
		if(NULL == ssearch) longjmp(e,1);

		i = beg = (size_t)(ssearch - work) + strlen(symTypeStr) + 1; /* strTypeStr + "'" */
		SkipUntilChar(work, &i, '\'', len);
		if(i == len) longjmp(e, 1);

		work[i] = '\0';
		i+=2;
		if(0 != strncasecmp("wasn't found.", &work[i], 13)) longjmp(e, 1);

		symbol = Str_copy(&work[beg]);
	}
	else
	{
		/* nothing to do */
	}

	free(work);
	return symbol;
}


static void addErrorLabel(List* labels, const char* errline)
{
	/**
	 * Error message match finder
	 *
	 *   Label error format:
	 *     ... - 1.50
	 *       Label XXXXX not found
	 *     1.60 - xxx
	 *       (EXXXX): Label 'XXXXX' wasn't found.
	 */
	char* label;

	label = GetUndefinedSymbol(errline, "Label ");
	if(NULL != label)
	{
		labels->push(labels, label);
	}
}

typedef struct {
	RomFile* rom;
	char** dirs;
	List* smwlibs;
	List* libs;
	List* insList;
	List* defineList;
	LibsInsertMan* libsInsMan;
	int objGroup;
	int objIndex;
	char* name;
	uint32 adr;
	int *libscnt;
}InsertAsmArgs;

#ifdef CPPUTEST
void labFree(void* data){free(data);}
#else
#define labFree free
#endif
static bool InsertAsm(InsertAsmArgs *args)
{
	int i;
	jmp_buf  e;
	FilePath* objPath = NULL;
	TextFile* objAsm;
	TextFile* tmpAsm;
	char* path = NULL;
	Iterator* lnode;

	/* for asar */
	bool result;
	LabelDataStruct* lab;
	int romlen = (int)args->rom->size_get(args->rom);
	InsertInf* inf;
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
	for(i=0; NULL != args->dirs[i]; i++)
	{
		free(path);
		path = Str_concat(args->dirs[i], args->name);
		if(fexists(path))
		{
			objPath = new_FilePath(path);
			break;
		}
	}
	free(path);

	if(NULL == objPath)
	{
		puterror("\"%s\" not found.", args->name);
		return false;
	}

	/* skip assemble if src is already inserted. */
	lnode = args->insList->search(args->insList, objPath->path_get(objPath), InsertedCheck);
	if(NULL != lnode)
	{
		uint8* ptr;
		inf = (InsertInf*)lnode->data(lnode);

		putinfo("  %s is already inserted. update pointer (to $%06x). ", objPath->path_get(objPath), inf->loc);
		ptr = args->rom->GetSnesPtr(args->rom, args->adr);
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

		/* Pass1 - Insert without libraries */
		if(false == GenerateTempAsm(objAsm, tmpAsm,
					args->defineList, args->smwlibs, args->libs,
					args->adr, args->objGroup, args->objIndex))
		{
			longjmp(e,1);
		}
		/* patch */
		asar_reset();
		result = asar_patch(
				tmpAsm->path_get(tmpAsm),
				(char*)args->rom->GetSnesPtr(args->rom, 0x8000),
				(int)args->rom->size_get(args->rom),
				&romlen);

		if(false == result)
		{
			int i;
			int errnums;
			const struct errordata* err;
			List* labels = new_List(NULL, labFree);

			assert(labels);
			/* Collect label not found error */
			err = asar_geterrors(&errnums);
			for(i=0; i<errnums; i++)
			{
				addErrorLabel(labels, err[i].rawerrdata);
			}

			/* Library insert */
			InsertLibraries(args->rom, labels, args->libsInsMan, args->smwlibs, args->libs, args->libscnt, args->defineList);
			delete_List(&labels);

			/* Pass2 - Reinsert object */
			if(false == GenerateTempAsm(objAsm, tmpAsm,
						args->defineList, args->smwlibs, args->libs,
						args->adr, args->objGroup, args->objIndex))
			{
				longjmp(e,1);
			}
			/* patch */
			asar_reset();
			result = asar_patch(
					tmpAsm->path_get(tmpAsm),
					(char*)args->rom->GetSnesPtr(args->rom, 0x8000),
					(int)args->rom->size_get(args->rom),
					&romlen);
		}
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
						args->libs->push(args->libs, lab);
						break;

					case Match_Visible:
					case Match_XSize:
					case Match_YSize:
					case Match_HorzElongationAmount:
					case Match_VertElongationAmount:
						putdebug("  Property updated: %s", asarprints[i]);
						break;

					default:
						putinfo("  %s: %s", objAsm->path_get(objAsm), asarprints[i]);
						break;
				}
			}
		}
		/* check result */
		if(false == result)
		{
			putasarerr();
			puterror("Failed to assemble \"%s\"", objAsm->path_get(objAsm));
			longjmp(e, 1);
		}
		/* Check locatio */
		codeadr = (uint32)asar_getlabelval("main");
		if(0x108000 > (codeadr & 0x7fffff))
		{
			puterror("%s: Probably empty asm file.(loc = $%06x)", objAsm->path_get(objAsm), codeadr);
			longjmp(e, 1);
		}

		/* add inserted file list */
		inf = calloc(1, sizeof(InsertInf));
		path = Str_copy(objAsm->path_get(objAsm));
		if(NULL == path || NULL == path)
		{
			putfatal("%s: memory error.", __func__);
			free(inf);
			free(path);
			longjmp(e,1);
		}
		inf->loc = codeadr;
		inf->path = path;
		args->insList->push(args->insList, inf);
	}
	else
	{
		delete_TextFile(&objAsm);
		delete_TextFile(&tmpAsm);
		return false;
	}

	putinfo("  \"%s\" inserted at $%06x. (Size: 0x%x bytes)", objAsm->path_get(objAsm), codeadr, GetCodeSize(args->rom, codeadr));
	delete_TextFile(&objAsm);
	remove(tmpAsm->path_get(tmpAsm));
	delete_TextFile(&tmpAsm);

	return true;
}

static uint32 Get2DObjTblAdr(RomFile* rom, const uint32 adrMain)
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
		List* smwlibs,
		LibsInsertMan* libsInsMan,
		int* libscnt,
		int* objscnt,
		List* defineList)
{
	jmp_buf e;
	const InsertListRangeStruct* range;
	int i,j;
	uint32 base;
	uint32 tbladr;
	char* dirs[SearchPathNums];
	List* libs;
	List* insList;
	uint8* ptr;
	InsertAsmArgs insAsmArgs;

	(*libscnt) = 0;
	(*objscnt) = 0;


	if(0 == setjmp(e)) /* try */
	{
		libs = new_List(NULL, DeleteLabelDataStruct);
		insList = new_List(CloneInsertInf, DeleteInsertInf);
		if(NULL == insList)
		{
			putfatal("%s: memory error.", __func__);
			longjmp(e, 1);
		}

		{
			size_t len;
			char* path;

			len = strlen(dirname);
			path = calloc(len+2, sizeof(char));
			if(NULL == path)
			{
				putfatal("%s: memory error.", __func__);
				longjmp(e, 1);
			}
			strcpy_s(path, len+2, dirname);
#if isUnix
			if('/' != path[len-1])
			{
				path[len] = '/';
				path[len+1] = '\0';
			}
#elif isWindows
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
				longjmp(e, 1);
			}
			free(path);
		}

		/**
		 * construct insAsmArgs
		 */
		insAsmArgs.rom = rom;			/* ROM file object */
		insAsmArgs.dirs = dirs;			/* SearchPath */
		insAsmArgs.smwlibs = smwlibs;		/* SMW libraries */
		insAsmArgs.libs = libs;			/* user libraries */
		insAsmArgs.libsInsMan = libsInsMan;	/* Library install manager */
		insAsmArgs.insList = insList;		/* inserted object list */
		insAsmArgs.defineList = defineList;	/* command-line define list */
		insAsmArgs.libscnt = libscnt;		/* inserted libraries count */

		for(i=0; Grps[i].ranges != NULL; i++)
		{
			putinfo("[%s]", GrpName[i]);
			if(ROMADDRESS_NULL == Grps[i].sa)
			{
				base = Get2DObjTblAdr(rom, adrMain);
				if(ROMADDRESS_NULL == base)
				{
					putfatal("%s: Obj2D table is missing...", __func__);
					longjmp(e, 1);
				}
			}
			else
			{
				base = Grps[i].sa;
			}
			for(range = Grps[i].ranges; 0 < range->max; range++)
			{
				insAsmArgs.objGroup = i;
				for(j=range->min; j <= range->max; j++)
				{
					insAsmArgs.objIndex = j;
					if(NULL == lst->ngroup[i][j])
					{
						/* nothing to do */
						continue;
					}
					insAsmArgs.name = lst->ngroup[i][j];

					/* get object pointer */
					tbladr = base + ((uint32)j*3);

					/* empty check */
					ptr = rom->GetSnesPtr(rom, tbladr);
					if(range->empty != read24(ptr))
					{
						puterror("Object %s-%02x isn't empty. (Maybe it has been changed with other tools.)", GrpName[i], j);
						longjmp(e, 1);
					}

					/* Insert object */
					insAsmArgs.adr = tbladr;
					putinfo("  Insert %02x", j);
					if(false == InsertAsm(&insAsmArgs))
					{
						longjmp(e, 1);
					}

					(*objscnt)++;
				}
			}
		}
	}
	else /* catch error */
	{
		DestroySearchPath(dirs);
		delete_List(&insList);
		delete_List(&libs);
		return false;
	}

	DestroySearchPath(dirs);
	delete_List(&insList);
	delete_List(&libs);
	return true;
}

static bool SearchUInt32(const void* sval, const void* lval)
{
	uint32* v1 = (uint32*)sval;	/* search value */
	uint32* v2 = (uint32*)lval;	/* list's value */

	return (*v1 == *v2);
}

#ifdef CPPUTEST
void uniFree(void* data){free(data);}
#else
#  define uniFree free
#endif
bool UninstallObjects(RomFile* rom, const uint32 adrMain)
{
	int i,j;
	uint32 sa;
	uint32 szMain;
	InsertListRangeStruct* rs;
	uint8* tbl;

	/* uninstall list */
	uint32* uniVal;
	List* uniList;

	uniList = new_List(NULL, uniFree);
	if(NULL == uniList)
	{
		putdebug("%s : memory error.", __func__);
		return false;
	}

	for(i=0; NULL != Grps[i].ranges; i++)
	{
		if(ROMADDRESS_NULL == Grps[i].sa)
		{
			/* Get 2D table */
			tbl = rom->GetSnesPtr(rom, adrMain);
			szMain = read16(&tbl[4]);
			/* move to MAIN sig */
			tbl = tbl + (int)szMain + 9 - 5;
			/* address fix */
			if(0 != memcmp("MAIN", tbl, 4))
			{
				if(0 != memcmp("MAIN", --tbl, 4))
				{
					putfatal("%s: Obj2D table is missing...", __func__);
					delete_List(&uniList);
					return false;
				}
			}
			tbl -= (2 + SigLen + 0x300);
		}
		else
		{
			tbl = rom->GetSnesPtr(rom, Grps[i].sa);
		}
		for(rs = Grps[i].ranges; 0 < rs->max; rs++)
		{
			for(j = rs->min; j <= rs->max; j++)
			{
				int ti = j*3;

				sa = read24(&tbl[ti]);

				/* check previous deletes */
				if(NULL != uniList->search(uniList, &sa, SearchUInt32))
				{
					/* found */
					write24(&tbl[ti], rs->empty);
					putinfo("Object %s-%02x pointer restored", GrpName[i], j);
					continue;
				}

				if(rs->empty == sa)
				{
					putdebug("  skip %s-%02x", GrpName[i], j);
					continue;
				}
				if(0x108000 > (sa & 0x7fffff))
				{
					putfatal("Program error: Invalid range : grp=%s, obj=%02x sa = $%06x", GrpName[i], j, sa);
					delete_List(&uniList);
					return false;
				}

				/* uninstall data */
				if(false == rom->RatsClean(rom, sa-8))
				{
					putdebug("%s : Rats_Clean failed. sa = $%06x, object = %s-%02x", __func__, sa, GrpName[i],j);
					delete_List(&uniList);
					return false;
				}

				/* add delete list */
				uniVal = calloc(1, sizeof(uint32));
				if(NULL == uniVal)
				{
					putdebug("%s : memory error.", __func__);
					return false;
				}
				*uniVal = sa;
				uniList->push(uniList, uniVal);

				/* rom address fix */
				write24(&tbl[ti], rs->empty);
				putinfo("Object %s-%02x is uninstalled from $%06x", GrpName[i], j, sa);
			} /* range min-max loop */
		} /* ranges loop */
	} /* Grp loop */

	delete_List(&uniList);
	return true;
}
