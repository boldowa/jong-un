#include "CppUTest/TestHarness.h"
#include "unkoBuildTime.h"

TEST_GROUP(unkoBuildTime)
{
  unkoBuildTime* projectBuildTime;

  void setup()
  {
    projectBuildTime = new unkoBuildTime();
  }
  void teardown()
  {
    delete projectBuildTime;
  }
};

TEST(unkoBuildTime, Create)
{
  CHECK(0 != projectBuildTime->GetDateTime());
}

