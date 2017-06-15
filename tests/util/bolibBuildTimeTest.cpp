#include "CppUTest/TestHarness.h"
#include "bolibBuildTime.h"

TEST_GROUP(bolibBuildTime)
{
  bolibBuildTime* projectBuildTime;

  void setup()
  {
    projectBuildTime = new bolibBuildTime();
  }
  void teardown()
  {
    delete projectBuildTime;
  }
};

TEST(bolibBuildTime, Create)
{
  CHECK(0 != projectBuildTime->GetDateTime());
}

