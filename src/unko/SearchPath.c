#include "types.h"
#include <stdlib.h>
#include "Str.h"
#include "puts.h"
#include "Enviroment.h"
#include "SearchPath.h"

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

	for(i=0; NULL != Enviroment.SearchPath[i]; i++)
	{
		p = Str_concat(Enviroment.SearchPath[i], suffix);
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

