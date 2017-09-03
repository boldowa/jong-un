extern "C"
{
#include "common/types.h"
#include "common/List.h"
#include "common/Enviroment.h"
#include "file/FilePath.h"
#include "file/File.h"
#include "file/RomFile.h"
#include "asardll.h"
#include "unko/ParseList.h"
#include "unko/LibsInsertMan.h"
#include "unko/Libraries.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(Libraries)
{
	void setup()
	{
	}

	void teardown()
	{
	}
};

TEST(Libraries, Create)
{
	FAIL("Start here");
}

