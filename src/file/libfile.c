/**
 * libfile.c
 */
#include "common/types.h"
#if isWindows
#  include <windows.h>
#elif isUnix
#  include <sys/stat.h>
#endif

#include "file/libfile.h"

bool fexists(const char* const path)
{
#if isWindows

	HANDLE hFile;

	hFile = CreateFile(
			path,
			GENERIC_READ, FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
			NULL
			);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		return true;
	}
	return false;

#elif isUnix

	struct stat sts;

	if(0 == stat(path, &sts) && S_ISREG(sts.st_mode))
	{
		return true;
	}
	return false;

#endif
}
