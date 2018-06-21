/**
 * @file SearchPath.c
 */
#include <bolib.h>
#include <stdlib.h>
#include "common/puts.h"
#include "common/Environment.h"
#include "unko/SearchPath.h"

void DestroySearchPath(char** paths)
{
	int i;
	for(i=0; NULL != paths[i]; i++)
	{
		free(paths[i]);
	}
}

bool ConstructSearchPath(char** paths, const char* suffix)
{
	int i;
	char* p;

	if(NULL == paths || NULL == suffix) return false;

	for(i=0; NULL != Environment.SearchPath[i]; i++)
	{
		p = Str_concat(Environment.SearchPath[i], suffix);
		if(NULL == p)
		{
			putfatal("%s: memory error.", __func__);
			DestroySearchPath(paths);
			return false;
		}
		paths[i] = p;
	}
	for(;i<SearchPathNums; i++)
	{
		paths[i] = NULL;
	}

	return true;
}

