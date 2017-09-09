extern "C"
{
#include "common/types.h"
#include <string.h>
#include "common/Str.h"
#include "common/Enviroment.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(Enviroment)
{
	void setup()
	{
	}

	void teardown()
	{
	}
};

TEST(Enviroment, SetEnviroment)
{
	SetSystemEnviroment();
	CHECK(NULL != Enviroment.ExeDir);
	CHECK(NULL != Enviroment.CurDir);
}

TEST(Enviroment, SetSearchPath)
{
	strcpy_s((char*)Enviroment.ExeDir, MAX_PATH, "./tests/");
	strcpy_s((char*)Enviroment.CurDir, MAX_PATH, "./testdata/");
	Enviroment.RomDir = Str_copy("./testdata/");

	SetSearchPath();

	POINTERS_EQUAL(NULL, Enviroment.SearchPath[2]);

	strcpy_s((char*)Enviroment.ExeDir, MAX_PATH, "./testdata/");
	strcpy_s((char*)Enviroment.CurDir, MAX_PATH, "./tests/");

	SetSearchPath();

	POINTERS_EQUAL(NULL, Enviroment.SearchPath[2]);

	free((char*)Enviroment.RomDir);
	Enviroment.RomDir = NULL;
}

