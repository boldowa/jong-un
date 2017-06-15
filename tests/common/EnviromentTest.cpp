extern "C"
{
#include "types.h"
#include "Enviroment.h"
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

