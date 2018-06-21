#pragma once

#define TempAsmName	"temp.asm"
#define SearchPathNums	4

typedef struct EnvironmentStruct {
	const char  CurDir[MAX_PATH];
	const char  ExeDir[MAX_PATH];
	const char* RomDir;
	const char* SearchPath[SearchPathNums];
} EnvironmentStruct;

extern EnvironmentStruct Environment;

void SetSystemEnvironment();
void SetSearchPath();


