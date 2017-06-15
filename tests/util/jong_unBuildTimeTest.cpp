#include "CppUTest/TestHarness.h"
#include "jong_unBuildTime.h"

TEST_GROUP(jong_unBuildTime)
{
  jong_unBuildTime* projectBuildTime;

  void setup()
  {
    projectBuildTime = new jong_unBuildTime();
  }
  void teardown()
  {
    delete projectBuildTime;
  }
};

TEST(jong_unBuildTime, Create)
{
  CHECK(0 != projectBuildTime->GetDateTime());
}

