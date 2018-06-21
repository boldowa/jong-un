/**
 * unko.c
 *   - Object insertion tool main src
 */

#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bolib.h>
#include <bolib/file/RomFile.h>
#include <bolib/file/TextFile.h>
#include "compiler.h"
#include "common/puts.h"
#include "common/Option.h"
#include "common/Environment.h"
#include "smw/libsmw.h"
#include "asar/asardll.h"
#include "unko/version.h"
#include "unko/Signature.h"
#include "unko/ParseCmdDefs.h"
#include "unko/ParseList.h"
#include "unko/LibsInsertMan.h"
#include "unko/Objects.h"
#include "unko/Libraries.h"
#include "unko/Asarctl.h"

/**
 * Asm path
 */
#define AsmPath			"sys/unko.asm"
#define SmwLibPath		"sys/smwlibs.asm"

/**
 * Default list file name
 */
#define DefaultListName		"list.txt"

/**
 * Default libraries dir name
 */
#define DefaultLibDirName	"libraries"

/**
 * Default objects dir name
 */
#define DefaultObjDirName	"objects"

/**
 * SMW library address
 */
#define ExecutePtrLong		0x0086fa

#if isWindows
#  define SHARED_LIB_PREFIX ""
#  define SHARED_LIB_EXT ".dll"
#  define EXECUTABLE_EXT ".exe"
#else
#  define SHARED_LIB_PREFIX "lib"
#  define SHARED_LIB_EXT ".so"
#  define EXECUTABLE_EXT ""
#endif

#define ASARLIB_FILE SHARED_LIB_PREFIX "asar" SHARED_LIB_EXT

typedef struct OptionValue {
	const char* listName;
	const char* libsDirName;
	const char* objsDirName;
	List* defineList;
} OptionValue;

typedef void (*InsertAsmInjection_t)(TextFile*);


static void InitList(InsertListStruct* lst)
{
	int i,j;
	for(i=0; i<IL_GROUPS; i++)
	{
		for(j=0; j<IL_NUMS; j++)
		{
			lst->ngroup[i][j] = NULL;
		}
	}
}

static void ReleaseList(InsertListStruct* lst)
{
	int i,j;
	for(i=0; i<IL_GROUPS; i++)
	{
		for(j=0; j<IL_NUMS; j++)
		{
			free(lst->ngroup[i][j]);
		}
	}
}

/**
 * @brief Show version info
 */
static void ShowVersion()
{
	int asar_ver_maj;
	int asar_ver_min;
	int asar_ver_bug;
	int api_ver_maj;
	int api_ver_min;

	asar_ver_maj = asar_version()/10000;
	asar_ver_min = asar_version()/100%100;
	asar_ver_bug = asar_version()%100;

	api_ver_maj = asar_apiversion()/100;
	api_ver_min = asar_apiversion()%100;

	printf("%s v%.2lf [Code: v0x%04x]\n", AppName, AppVersion, CodeVersion);
	printf("--- %s\n", UNKO_GIT_REVISION);
	printf("  by boldowa\n");
	printf("  since    : May 13 2017\n");
	printf("  compiled : %s\n", __DATE__);
	printf("             with %s ver.%s\n", UNKO_C_COMPILER, UNKO_C_COMPILER_VERSION);
	printf("  Asar     : v%d.%d.%d API v%d.%d\n",
			asar_ver_maj, asar_ver_min, asar_ver_bug,
			api_ver_maj, api_ver_min);
}

/**
 * @brief Show application usage
 *
 * @param pg program name
 * @param opt option struct array pointer
 */
static void ShowUsage(const char* pg, const OptionStruct* opt)
{
	printf("Usage: %s [options] <rom>\n", pg);
	Option_Usage(opt);
}

static bool IsUnkoMainData(const uint8* data, const uint32 len)
{
	if(0x300 > len)
	{
		return false;
	}

	/* move to tail */
	data += len;

	/* Term + "MAIN" */
	data -= 5;
	if(0 != memcmp("MAIN", data, 4))
	{
		/* I don't know the reason, but it's reraly displaced. */
		if(0 != memcmp("MAIN", --data, 4))
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

static void RomClean_v0100(RomFile* rom)
{
	uint8* ptr;

	/* restore obj2D */
	ptr = rom->GetSnesPtr(rom, 0x0da4d9);
	write24(ptr, NrmObjEmpty);
	ptr = rom->GetSnesPtr(rom, 0x0dc21e);
	write24(ptr, NrmObjEmpty);
	ptr = rom->GetSnesPtr(rom, 0x0dce1e);
	write24(ptr, NrmObjEmpty);
	ptr = rom->GetSnesPtr(rom, 0x0dda1e);
	write24(ptr, NrmObjEmpty);
	ptr = rom->GetSnesPtr(rom, 0x0de91e);
	write24(ptr, NrmObjEmpty);

	/* restore ExecutePtrLong */
	ptr = rom->GetSnesPtr(rom, 0x0da452);
	write24(ptr, ExecutePtrLong);
	ptr = rom->GetSnesPtr(rom, 0x0dc197);
	write24(ptr, ExecutePtrLong);
	ptr = rom->GetSnesPtr(rom, 0x0dcd97);
	write24(ptr, ExecutePtrLong);
	ptr = rom->GetSnesPtr(rom, 0x0dd997);
	write24(ptr, ExecutePtrLong);
	ptr = rom->GetSnesPtr(rom, 0x0de897);
	write24(ptr, ExecutePtrLong);
	ptr = rom->GetSnesPtr(rom, 0x0da10c);
	write24(ptr, ExecutePtrLong);
}

static uint16 GetCodeVersion(RomFile* rom, const uint32 adrMain)
{
	uint8* ptr;
	uint16 sz;
	ptr = rom->GetSnesPtr(rom, adrMain);
	sz = read16(&ptr[4]);

	/* 9 ... rats header +1 */
	/* 5 ... main signature length */
	/* 2 ... code version length */
	/* move to MAIN sig */
	ptr = ptr + (int)sz + 9 - 5;
	if(0 != memcmp("MAIN", ptr, 4))
	{
		if(0 != memcmp("MAIN", --ptr, 4))
		{
			return 0;
		}
	}
	ptr = ptr - SigLen - 2;
	return read16(ptr);
}

static bool UninstallUnko(RomFile* rom, const uint32 adrMain)
{
	uint16 codeVer;

	/* get main code version */
	codeVer = GetCodeVersion(rom, adrMain);
	if(0 == codeVer)
	{
		putfatal("%s: version info is missing...", __func__);
		return false;
	}

	/* clean main code */
	if(false == rom->RatsClean(rom, adrMain))
	{
		return false;
	}
	putinfo(AppName " main code uninstalled from $%06x", adrMain);

	/* clean hijacks */
	putinfo("Hijacks tweaking...");
	switch(codeVer)
	{
		case 0x0001:
		case 0x0100:
		case 0x0101:
		case 0x0110:
			RomClean_v0100(rom);
			break;
		default:
			putfatal("%s: clean code is nothing for code ver 0x%04x...", __func__, codeVer);
			return false;
	}

	return true;
}

static void PutsSignature(TextFile* asmFile)
{
	asmFile->printf(asmFile, "\n");
	asmFile->printf(asmFile, "db\t\"" Signature "\", $00\n");
	asmFile->printf(asmFile, "db\t\"MAIN\", $00\n");
}

static bool InsertAsm(RomFile* rom, const char* path, const InsertAsmInjection_t injection)
{
	jmp_buf e;
	TextFile* libAsm;
	TextFile* tmpAsm;
	const char* linebuf;
	const char* const * asarprints;
	int printcnt;

	/* for asar */
	bool result;
	int romlen = (int)rom->size_get(rom);

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
		if(FileOpen_NoError != libAsm->open2(libAsm, "r"))
		{
			puterror("Can't open \"%s\"", libAsm->path_get(libAsm));
			longjmp(e,1);
		}
		if(FileOpen_NoError != tmpAsm->open2(tmpAsm, "w"))
		{
			puterror("Can't open \"%s\"", tmpAsm->path_get(tmpAsm));
			longjmp(e,1);
		}

		/* generate tmpasm */
		tmpAsm->printf(tmpAsm, "%s\n", rommap.name);
		/* -- main code -- */
		tmpAsm->printf(tmpAsm, "!map = %d\n", rommap.val);
		tmpAsm->printf(tmpAsm, "freecode\n");
		linebuf = libAsm->getline(libAsm);
		while(NULL != linebuf)
		{
			tmpAsm->printf(tmpAsm, "%s\n", linebuf);
			linebuf = libAsm->getline(libAsm);
		}
		/* execute injection code */
		if(NULL != injection) injection(tmpAsm);

		/* close */
		tmpAsm->close(tmpAsm);
		libAsm->close(libAsm);

		/* patch */
		asar_reset();
		result = asar_patch(
				tmpAsm->path_get(tmpAsm),
				(char*)rom->GetSnesPtr(rom, 0x8000),
				(int)rom->size_get(rom),
				&romlen);
		/* print asm puts */
		asarprints = asar_getprints(&printcnt);
		{
			int i;
			for(i=0; i<printcnt; i++)
			{
				putinfo("%s: %s", libAsm->path_get(libAsm), asarprints[i]);
			}
		}
		if(false == result)
		{
			putasarerr();
			puterror("Failed to insert \"%s\"", libAsm->path_get(libAsm));
			longjmp(e, 1);
		}
	}
	else
	{
		delete_TextFile(&libAsm);
		delete_TextFile(&tmpAsm);
		return false;
	}

	delete_TextFile(&libAsm);
	delete_TextFile(&tmpAsm);
	remove(TempAsmName);
	return true;
}

static bool InstallUnko(RomFile* rom)
{
	char* asmPath;
	bool res;
	uint32 adrMain;
	uint16 codeVer;

	asmPath = Str_concat(Environment.ExeDir, AsmPath);
	res = InsertAsm(rom, asmPath, PutsSignature);
	free(asmPath);

	if(false == res)
	{
		return false;
	}

	/* Verify the install version */
	adrMain = rom->RatsSearch(rom, 0x108000, IsUnkoMainData);
	if(ROMADDRESS_NULL == adrMain) return false;
	codeVer = GetCodeVersion(rom, adrMain);
	if(CodeVersion != codeVer)
	{
		putfatal("Program error: Unmatch asm code version");
		putfatal("               Program: 0x%04x | asm: 0x%04x", CodeVersion, codeVer);
		return false;
	}

	putinfo("%s installed.", AsmPath);
	return true;
}

static LabelDataStruct* CopyLibs(const struct labeldata* srclab)
{
	char* labname;
	LabelDataStruct *lab;

	labname = Str_copy(srclab->name);
	lab = malloc(sizeof(LabelDataStruct));

	if( (NULL == labname) || (NULL == lab))
	{
		free(labname);
		free(lab);
		return NULL;
	}

	lab->loc = srclab->location;
	lab->name = labname;

	return lab;
}

static bool GenerateSMWLibs(RomFile* rom, List* smwlibs)
{
	char* asmPath;
	int i;
	int labcnt;
	const struct labeldata* labels;
	LabelDataStruct* smwlab;
	bool res;

	asmPath = Str_concat(Environment.ExeDir, SmwLibPath);
	res = InsertAsm(rom, asmPath, NULL);
	free(asmPath);
	if(false == res)
	{
		return false;
	}
	putinfo("%s configured.", SmwLibPath);

	labels = asar_getalllabels(&labcnt);
	for(i=0; i<labcnt; i++)
	{
		if(0 == strncmp(":pos_", labels[i].name, 5))	/* + labs */
		{
			continue;
		}
		if(0 == strncmp(":neg_", labels[i].name, 5))	/* - labs */
		{
			continue;
		}
		if(0 == labels[i].location)
		{
			continue;
		}

		smwlab = CopyLibs(&labels[i]);
		if(NULL == smwlab)
		{
			putfatal("%s : memory error", __func__);
			free(smwlab);
			return false;
		}
		smwlibs->push(smwlibs,smwlab);
	}

	return true;
}

static bool InitializeUnko(RomFile* rom)
{
	uint32 adrMain;
	uint16 codeVer;

	/* Install check */
	if(ROMADDRESS_NULL != (adrMain = rom->RatsSearch(rom, 0x108000, IsUnkoMainData)))
	{
		/* uninstall library code */
		putdebug("Uninstall libs.");
		if(false == UninstallLibs(rom))
		{
			return false;
		}

		/* uninstall objects */
		putdebug("Uninstall objects.");
		if(false == UninstallObjects(rom, adrMain))
		{
			return false;
		}

		/* get main code version */
		codeVer = GetCodeVersion(rom, adrMain);
		if(0 == codeVer)
		{
			putfatal("%s: version info is missing...", __func__);
			return false;
		}

		/* check main code version */
		if(CodeVersion != codeVer)
		{
			if(CodeVersion < codeVer)
			{
				puterror("Version of this tool is older than rom code version.");
				return false;
			}
			/* Version up */
			putinfo("Version of code in rom is older than tool's one. upgrading...");
			if(false == UninstallUnko(rom, adrMain))
			{
				puterror("Failed to uninstall old code.");
				return false;
			}
			if(false == InstallUnko(rom))
			{
				puterror("Failed to upgrade.");
				return false;
			}
			putinfo("Succeeded to upgrade " AppName ".");
		}
	}
	else
	{
		putinfo(AppName " isn't installed to this rom. Installing...");
		if(false == InstallUnko(rom))
		{
			return false;
		}
		putinfo("Succeeded to install " AppName ".");
	}
	return true;
}

static bool Insert(RomFile* rom, const OptionValue* opt)
{
	bool result;
	jmp_buf e;
	uint32 adrMain;

	InsertListStruct lst;
	List* smwlibs;
	LibsInsertMan *libsInsMan;
	int libscnt;
	int objscnt;

	InitList(&lst);

	/* Check Lunar Magic */
	if(false == IsLMInstalled(rom))
	{
		puterror("LunarMagic isn't installed to this rom : \"%s\".", rom->path_get(rom));
		return false;
	}

	/* read list file */
	putinfo("--- Reading objects list file...");
	if(false == ParseList(opt->listName, &lst))
	{
		puterror("Can't parse \"%s\".", opt->listName);
		return false;
	}

	/* Initialize */
	putinfo("--- Rom initializing...");
	if(false == InitializeUnko(rom))
	{
		puterror("Initialize failed.");
		ReleaseList(&lst);
		return false;
	}

	/* Generate libraries list */
	smwlibs = new_List(NULL, DeleteLabelDataStruct);
	libsInsMan = new_LibsInsertMan();

	/* Insert */
	result = false;
	if(0 == setjmp(e))
	{
		if((NULL == libsInsMan) || (NULL == smwlibs))
		{
			putfatal("Libs list create failed(memory error).");
			longjmp(e, 1);
		}

		/* Set-up smw libraries */
		putinfo("--- Generating SMWLib database...");
		if(false == GenerateSMWLibs(rom, smwlibs))
		{
			puterror("SMWLibs generate failed.");
			longjmp(e, 1);
		}

		/* build library database */
		putinfo("--- Searching libraries...");
		libsInsMan->buildData(libsInsMan, opt->libsDirName);
		if(0 == libsInsMan->filesCount(libsInsMan))
		{
			putinfo("Library is nothing.");
		}
		else
		{
			putinfo("Library: %d files", (int)libsInsMan->filesCount(libsInsMan));
			putinfo("         %d labels", (int)libsInsMan->labelsCount(libsInsMan));
		}

		/* search main data */
		adrMain = rom->RatsSearch(rom, 0x108000, IsUnkoMainData);
		if(ROMADDRESS_NULL == adrMain)
		{
			puterror("Main-code search failed.");
			longjmp(e, 1);
		}

		/* Insert objects */
		putinfo("--- Inserting objects...");
		if(false == InsertObjects(rom, opt->objsDirName, adrMain, &lst, smwlibs, libsInsMan, &libscnt, &objscnt, opt->defineList))
		{
			puterror("Object insert failed.");
			longjmp(e, 1);
		}
		if(0 == objscnt)
		{
			putinfo("Object is nothing.");
		}

		putinfo("Number of inserted libraries: %d", libscnt);
		putinfo("Number of inserted objects: %d", objscnt);
		result = true;
	}
	else
	{
		putdebug("Insert error occcured...");
	}

	/* delete libs list */
	delete_List(&smwlibs);
	delete_LibsInsertMan(&libsInsMan);
	ReleaseList(&lst);

	return result;
}

static bool Uninstall(RomFile* rom, const OptionValue* opt)
{
	bool result = true;
	uint32 adrMain;

	/* search main data */
	adrMain = rom->RatsSearch(rom, 0x108000, IsUnkoMainData);
	if(ROMADDRESS_NULL == adrMain)
	{
		puterror(AppName " isn't installed.");
		return false;
	}

	/* uninstall */
	result &= UninstallLibs(rom);
	result &= UninstallObjects(rom, adrMain);
	result &= UninstallUnko(rom, adrMain);

	return result;
}

static bool WriteRom(const char *rompath, const OptionValue* opt, bool (*proc)(RomFile*, const OptionValue*))
{
	RomFile* rom;
	bool result;

	if((NULL == rompath) || (NULL == proc))
	{
		putfatal("%s: Program logic error: rompath null or proc is missing.", __func__);
		return false;
	}

	/* rom file open */
	rom = new_RomFile(rompath);
	if(FileOpen_NoError != rom->open(rom))
	{
		puterror("Can't open \"%s\".", rompath);
		delete_RomFile(&rom);
		return false;
	}

	/* set enviroment */
	Environment.RomDir = rom->dir_get(rom);
	if(0 == strcmp("", rom->dir_get(rom)))
	{
		Environment.RomDir = Environment.CurDir;
	}
	SetSearchPath();
	{
		int i;
		for(i=0; Environment.SearchPath[i] != NULL; i++)
		{
			putdebug("Environment.SearchPath[%d] = %s", i, Environment.SearchPath[i]);
		}
	}
	getmapmode(rom);

	/* smw rom check */
	if(false == IsSmw(rom))
	{
		puterror("This rom isn't smw rom file.");
		delete_RomFile(&rom);
		return false;
	}

	/* execute rom access proc */
	result = proc(rom, opt);

	/* write rom */
	if(true == result)
	{
		result = rom->write(rom);
		if(false == result)
		{
			putfatal("ROM write error occured...");
		}
	}

	rom->close(rom);
	delete_RomFile(&rom);

	return result;
}

/* global options ---------------------------------*/
bool vdebug		= false;	/* @brief option value for puts.h */

/**
 * @brief Unko main
 *
 * @param argc arguments count
 * @param argv arguments value
 *
 * @return result
 */
int Unko(int argc, char** argv)
{
	/* options */
	bool showHelp = false;
	bool showVersion = false;
	bool uninstallMode = false;
	OptionValue opt;
	List* defineList;

	/* var */
	bool result;
	int i;

	/* command-line options */
	SetOptStruct defOptSt = { ParseCmdDefs, &defineList };
	OptionStruct options[] = {
		{ "list", 'l', "specify list file path (default: " DefaultListName ")", OptionType_String, (void*)&opt.listName },
		{ "library", 'L', "specify library dir (default: " DefaultLibDirName ")", OptionType_String, (void*)&opt.libsDirName },
		{ "object", 'o', "specify object dir (default: " DefaultObjDirName ")", OptionType_String, (void*)&opt.objsDirName },
		{ "define", 'D', "add asm define", OptionType_FunctionString, &defOptSt },
		{ "uninstall", 'u', "uninstall " AppName, OptionType_Bool, &uninstallMode },
		{ "debug", 'd', "verbose debug info", OptionType_Bool, &vdebug },
		{ "version", 'v', "show version", OptionType_Bool, &showVersion },
		{ "help", '?', "show help message", OptionType_Bool, &showHelp },
		/* term */
		{ NULL, '\0', NULL, OptionType_Term, NULL },
	};

	defineList = new_List(CloneDefine, DelDefine);
	if(NULL == defineList)
	{
		putfatal("%s : memory error", __func__);
		return -1;
	}

	SetSystemEnvironment();
	opt.listName = DefaultListName;
	opt.libsDirName = DefaultLibDirName;
	opt.objsDirName = DefaultObjDirName;
	opt.defineList = defineList;

	/* init asar */
	{
		char dllpath[MAX_PATH];
		sprintf_s(dllpath, MAX_PATH, "%s%s", Environment.ExeDir, ASARLIB_FILE);
		if(!asar_init_with_dll_path(dllpath))
		{
			putfatal("asar_init() failed.");
			delete_List(&defineList);
			return -1;
		}
	}

	/* Get command-line options */
	if(false == Option_Parse(&argc, &argv, options))
	{
		puterror("Option parse failed...");
		asar_close();
		delete_List(&defineList);
		return -1;
	}
	putdebug("ExeDir: %s",Environment.ExeDir);
	putdebug("CurDir: %s",Environment.CurDir);

	/* Show help / version */
	if(true == showVersion)
	{
		ShowVersion();
	}
	if(true == showHelp)
	{
		ShowUsage(argv[0], options);
	}

	/* Show usage message */
	if(argc != 2)
	{
		if ((false == showVersion) && (false == showHelp))
		{
			printf("Usage: %s [options] <rom>\n", argv[0]);
			printf("Please try '-?' or '--help' option, and you can get more information.\n");
		}
		asar_close();
		delete_List(&defineList);
		return 0;
	}

	/* Puts debug message */
	for(i=0; i<argc; i++)
	{
		putdebug("argv[%d] = %s", i, argv[i]);
	}
	putdebug("list name: %s", opt.listName);
	putdebug("libs dir : %s", opt.libsDirName);
	putdebug("objs dir : %s", opt.objsDirName);

	if(true == uninstallMode)
	{
		putinfo("--- Uninstalling " AppName "...");
		result = WriteRom(argv[1], NULL, Uninstall);
	}
	else
	{
		result = WriteRom(argv[1], &opt, Insert);
	}

	asar_close();
	delete_List(&defineList);

	if(false == result)
	{
		putinfo("--- Failed...");
		return -1;
	}

	putinfo("--- Succeeded.");
	return 0;
}

