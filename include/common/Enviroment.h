#pragma once

#define TempAsmName	"temp.asm"
#define SearchPathNums	4

typedef struct EnviromentStruct {
	const char  CurDir[MAX_PATH];
	const char  ExeDir[MAX_PATH];
	const char* RomDir;
	const char* SearchPath[SearchPathNums];
} EnviromentStruct;

extern EnviromentStruct Enviroment;

void SetSystemEnviroment();
void SetSearchPath();


