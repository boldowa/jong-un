/**
 * @file LibrariesTest.cpp
 */
#include <bolib.h>
#include <bolib/file/RomFile.h>
extern "C"
{
#include "common/Environment.h"
#include "asar/asardll.h"
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

/**
 * Test for InsertLibraries method
 */
TEST(Libraries, InsertLibraries)
{
	/* TODO: Insert */

	/* TODO: Uninstall */
}

/**
 * Test for UninstallLibraries method
 */
TEST(Libraries, UninstallLibraries)
{
	/* tested in InsertLibraries, so nothing to do here */
}

