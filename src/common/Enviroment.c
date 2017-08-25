#include "types.h"
#include <string.h>
#ifdef WIN32
#  include <windows.h>
#endif
#ifdef UNIX
#  ifndef __USE_XOPEN2K
#    define __USE_XOPEN2K
#  endif
#  include <unistd.h>
#endif
#include "FilePath.h"
#include "Enviroment.h"

EnviromentStruct Enviroment;

void SetSystemEnviroment()
{
	size_t len;
	char tmp[MAX_PATH];
	int i;

	memset((char*)Enviroment.CurDir, '\0', MAX_PATH);
	memset((char*)Enviroment.ExeDir, '\0', MAX_PATH);
	Enviroment.RomDir = NULL;

	for(i=0; i<4; i++)
	{
		Enviroment.SearchPath[i] = NULL;
	}

	/* Set exe dir */
#if defined(WIN32)
	/* Get absolute path of exe */
	GetModuleFileName(NULL, tmp, MAX_PATH-1);
	{
		FilePath* path;

		path = new_FilePath(tmp);
		strcpy_s((char*)Enviroment.ExeDir, MAX_PATH, path->dir_get(path));
		delete_FilePath(&path);
	}

	/* Get absolute path of current */
	GetFullPathName(".\\", MAX_PATH-1, (char*)Enviroment.CurDir, NULL);
	len = strlen(Enviroment.CurDir);
	if('\\' != Enviroment.CurDir[len-1] && '/' != Enviroment.CurDir[len-1])
	{
		((char*)Enviroment.CurDir)[len] = '\\';
	}

#elif defined(UNIX)
	/* Get absolute path of exe */
	readlink("/proc/self/exe", tmp, MAX_PATH-1);
	{
		FilePath* path;

		path = new_FilePath(tmp);
		strcpy_s((char*)Enviroment.ExeDir, MAX_PATH, path->dir_get(path));
		delete_FilePath(&path);
	}

	/* Get absolute path of current */
	getcwd((char*)Enviroment.CurDir, MAX_PATH-1);
	len = strlen(Enviroment.CurDir);
	if('/' != Enviroment.CurDir[len-1])
	{
		((char*)Enviroment.CurDir)[len] = '/';
	}
#else
  #error "This system isn't supported."
#endif
}

void SetSearchPath()
{
	int i=0;

	Enviroment.SearchPath[i] = Enviroment.CurDir;

	if((Enviroment.RomDir != NULL) && (0 != strcmp("", Enviroment.RomDir)))
	{
		if(0 != strcmp(Enviroment.SearchPath[i], Enviroment.RomDir))
		{
			i++;
			Enviroment.SearchPath[i] = Enviroment.RomDir;
		}
	}

	if(0 != strcmp(Enviroment.SearchPath[i], Enviroment.ExeDir))
	{
		i++;
		Enviroment.SearchPath[i] = Enviroment.ExeDir;
	}

#if defined(UNIX)
	Enviroment.SearchPath[0] = "./";
#elif defined(WIN32)
	Enviroment.SearchPath[0] = ".\\";
#endif

}
