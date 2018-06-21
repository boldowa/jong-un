/**
 * @file ObjectsTest.cpp
 */
#include <bolib.h>
#include <bolib/file/RomFile.h>
extern "C"
{
#include "common/Environment.h"
#include "asar/asardll.h"
#include "unko/ParseList.h"
#include "unko/LibsInsertMan.h"
#include "unko/Objects.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(Objects)
{
	void setup()
	{
	}

	void teardown()
	{
	}
};

/**
 * Test for InsertObjects method
 */
TEST(Objects, InsertObjects)
{
	/* TODO: Insert */

	/* TODO: Uninstall */
}

/**
 * Test for UninstallObjects method
 */
TEST(Objects, UninstallObjects)
{
	/* tested in InsertObjects, so nothing to do here */
}

