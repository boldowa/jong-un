/**
 * @file ParseList.c
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <bolib.h>
#include "common/Environment.h"
#include "common/puts.h"
#include "common/Funex.h"
#include <bolib/file/TextFile.h>
#include "unko/ParseList.h"

typedef struct ListItemSt {
	int inx;
	char* name;
} ListItemSt;

enum {
	Match_Group = 0,
	Match_List,
	Unmatch
};

const char* const GrpName[] = {
	"Normal",
	"Castle",
	"Rope",
	"Underground",
	"GhostHouse",
	"ExtendObject",
	"Object2D"
};

static bool MatchList(const char* line, void* data)
{
	size_t len;
	size_t i;
	char* asmstr;
	size_t asmlen;
	uint32 hex = 0;
	ListItemSt *li = (ListItemSt*)data;

	len = strlen(line);
	i = 0;

	SkipSpaces(line, &i, len);

	/* get index */
	if(false == ishex(line[i]))
	{
		return false;
	}
	for(; i<len; i++)
	{
		if(false == ishex(line[i])) break;
		hex <<= 4;
		hex = hex + (uint32)atoh(line[i]);
	}
	if(255<hex)
	{
		puterror("Index is too large.");
		return false;
	}

	SkipSpaces(line, &i, len);

	/* filename */
	asmlen = strlen(&line[i]);
	if(0 == asmlen)
	{
		/* number only */
		li->name = NULL;
		return true;
	}
	asmstr = calloc(asmlen+1, sizeof(char));
	if(NULL == asmstr)
	{
		putfatal("%s: memory error.", __func__);
		return false;
	}
	strcpy_s(asmstr, asmlen+1, &line[i]);

	li->inx = (int)hex;
	li->name = asmstr;

	return true;
}

static bool MatchGroup(const char* line, void* data)
{
	size_t len;
	size_t i;
	size_t st;
	char* grpstr;
	size_t grplen;

	len = strlen(line);
	i= 0;

	SkipSpaces(line, &i, len);

	if('[' != line[i])
	{
		return false;
	}
	st = ++i;

	for(; i<len; i++)
	{
		if(']' == line[i])
		{
			break;
		}
	}
	if(i != len-1)
	{
		return false;
	}

	grplen = i-st;
	grpstr = calloc(grplen+1, sizeof(char));
	if(NULL == grpstr)
	{
		putfatal("%s: memory error.", __func__);
		return false;
	}
	strncpy_s(grpstr, grplen+1, &line[st], grplen);

	for(i=0; i<(sizeof(GrpName)/sizeof(char*)); i++)
	{
		if(0 == strcmp(GrpName[i], grpstr))
		{
			free(grpstr);
			*((int*)data) = (int)i;
			return true;
		}
	}

	putwarn("Group \"%s\" is missing.", grpstr);
	free(grpstr);
	return false;
}

static void RemoveComment(char* line)
{
	char* s;
	int i;
	int len;
	s = strstr(line, "//");
	if(NULL != s)
	{
		*s = '\0';
	}

	len = (int)strlen(line);
	for(i = len-1; i>=0; i--)
	{
		if(false == IsSpace(line[i]))
		{
			break;
		}
		line[i] = '\0';
	}
}

bool ParseList(const char* listName, InsertList list)
{
	jmp_buf e;
	char* path = NULL;
	char* lstPath = NULL;
	TextFile* lstFile;
	const char* linebuf;
	char* buf = NULL;
	int grpNo;
	ListItemSt listItem;
	int i;

	FunexStruct fs[] = {
		{ MatchGroup, &grpNo },
		{ MatchList, &listItem },
		/* --- */
		{ NULL, NULL }
	};

	/* search list */
	for(i=0; NULL != Environment.SearchPath[i]; i++)
	{
		free(path);
		path = Str_concat(Environment.SearchPath[i], listName);
		if(fexists(path))
		{
			lstPath = path;
			break;
		}
	}

	if(NULL == lstPath)
	{
		puterror("Objects list file \"%s\" not found.",listName);
		free(path);
		return false;
	}

	/* open list file */
	if(0 == setjmp(e))
	{
		lstFile = new_TextFile(lstPath);
		free(lstPath);
		if(FileOpen_NoError != lstFile->open2(lstFile, "r"))
		{
			longjmp(e,1);
		}

		putdebug("Parse %s", lstFile->path_get(lstFile));
		linebuf = lstFile->getline(lstFile);
		while(NULL != linebuf)
		{
			/* comment process */
			buf = Str_copy(linebuf);
			RemoveComment(buf);

			/* empty line */
			if(0 == strlen(buf))
			{
				free(buf); buf=NULL;
				linebuf = lstFile->getline(lstFile);
				continue;
			}

			/* parse list */
			switch(FunexMatch(buf, fs))
			{
				case Match_Group:
					putdebug("Group: %s", GrpName[grpNo]);
					break;

				case Match_List:
					if(NULL == listItem.name) break;

					if(list->ngroup[grpNo][listItem.inx] != NULL)
					{
						puterror("%s: line %d: conflict object. (object %s-%02x: %s / %s)",
								lstFile->path_get(lstFile),
								lstFile->row_get(lstFile),
								GrpName[grpNo],
								listItem.inx,
								list->ngroup[grpNo][listItem.inx],
								listItem.name);
						free(listItem.name);
						longjmp(e, 1);
					}
					list->ngroup[grpNo][listItem.inx] = listItem.name;
					putdebug("  %s-%02x: \"%s\"", GrpName[grpNo], listItem.inx, listItem.name);
					break;

				/* Unmatch */
				default:
					puterror("%s: line %d: syntax error.",
							lstFile->path_get(lstFile),
							lstFile->row_get(lstFile));
					longjmp(e, 1);
			}

			/* next */
			free(buf); buf=NULL;
			linebuf = lstFile->getline(lstFile);
		}

		delete_TextFile(&lstFile);
	}
	else
	{
		delete_TextFile(&lstFile);
		free(buf);
		return false;
	}
	return true;
}
