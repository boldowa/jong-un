/**
 * libfileTest.cpp
 */

extern "C"
{
#include "file/libfile.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(libfile)
{
	void setup()
	{
		/* Nothing to do here */
	}

	void teardown()
	{
		/* Nothing to do here */
	}
};

TEST(libfile, fexists)
{
	CHECK_FALSE(fexists(""));

	CHECK(fexists("./testdata/file/test.txt"));
}

