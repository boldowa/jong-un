/**
 * @file AsardllTest.cpp
 */
extern "C"
{
#include "asar/asardll.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(asardll)
{
	void setup()
	{
	}

	void teardown()
	{
	}
};

TEST(asardll, Create)
{
	CHECK(asar_init());
	CHECK(0 != asar_version());
	asar_close();
}

