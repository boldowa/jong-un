/**
 * @file ParseObjPuts.c
 */
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include <bolib.h>
#include "common/puts.h"
#include "common/Funex.h"
typedef struct _RomFile RomFile;
#include "unko/Asarctl.h"
#include "unko/ParseObjPuts.h"

bool MatchExportValue(const char* str, char* declaration, void* data)
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

enum {
	Match_True = 0,
	Match_False,
	Match_On,
	Match_Off,
	UnmatchBool
};
bool MatchBooleanValue(const char* str, char* declaration, void* data)
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
			return true;

		default:
			break;
	}
	return false;
}

bool MatchIntegerValue(const char* str, char* declaration, void* data)
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

bool ParseObjPuts(const char* str, void* data,
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
