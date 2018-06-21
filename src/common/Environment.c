/**
 * Environment.c
 */
#include <bolib.h>
#if isWindows
#  include <windows.h>
#elif isUnix
#  ifndef __USE_XOPEN2K
#    define __USE_XOPEN2K
#  endif
#  include <unistd.h>
#endif
#include <memory.h>
#include "common/Environment.h"

EnvironmentStruct Environment;

void SetSystemEnvironment()
{
	size_t len;
	char tmp[MAX_PATH];

	memset((char*)Environment.CurDir, '\0', MAX_PATH);
	memset((char*)Environment.ExeDir, '\0', MAX_PATH);
	Environment.RomDir = NULL;

	/* Set exe dir */
#if isWindows
	/* Get absolute path of exe */
	GetModuleFileName(NULL, tmp, MAX_PATH-1);
	{
		FilePath* path;

		path = new_FilePath(tmp);
		strcpy_s((char*)Environment.ExeDir, MAX_PATH, path->dir_get(path));
		delete_FilePath(&path);
	}

	/* Get absolute path of current */
	GetFullPathName(".\\", MAX_PATH-1, (char*)Environment.CurDir, NULL);
	len = strlen(Environment.CurDir);
	if('\\' != Environment.CurDir[len-1] && '/' != Environment.CurDir[len-1])
	{
		((char*)Environment.CurDir)[len] = '\\';
	}

#elif isUnix
	/* Get absolute path of exe */
	readlink("/proc/self/exe", tmp, MAX_PATH-1);
	{
		FilePath* path;

		path = new_FilePath(tmp);
		strcpy_s((char*)Environment.ExeDir, MAX_PATH, path->dir_get(path));
		delete_FilePath(&path);
	}

	/* Get absolute path of current */
	getcwd((char*)Environment.CurDir, MAX_PATH-1);
	len = strlen(Environment.CurDir);
	if('/' != Environment.CurDir[len-1])
	{
		((char*)Environment.CurDir)[len] = '/';
	}
#else
  #error "This system isn't supported."
#endif
}

static bool spExists(int index, const char* path)
{
	int i;
	for(i=0; i<=index; i++)
	{
		if(0 == strcmp(Environment.SearchPath[i], path))
		{
			return true; 
		}
	}
	return false;
}

void SetSearchPath()
{
	int i=0;

	for(i=0; i<4; i++)
	{
		Environment.SearchPath[i] = NULL;
	}

	i=0;
	Environment.SearchPath[i] = Environment.CurDir;

	if((Environment.RomDir != NULL) && (0 != strcmp("", Environment.RomDir)))
	{
		if(0 != strcmp(Environment.SearchPath[i], Environment.RomDir))
		{
			i++;
			Environment.SearchPath[i] = Environment.RomDir;
		}
	}

	if(false == spExists(i, Environment.ExeDir))
	{
		i++;
		Environment.SearchPath[i] = Environment.ExeDir;
	}

#if isUnix
	Environment.SearchPath[0] = "./";
#elif isWindows
	Environment.SearchPath[0] = ".\\";
#endif

}
