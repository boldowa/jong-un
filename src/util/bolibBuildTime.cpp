#include "bolibBuildTime.h"

bolibBuildTime::bolibBuildTime()
: dateTime(__DATE__ " " __TIME__)
{
}

bolibBuildTime::~bolibBuildTime()
{
}

const char* bolibBuildTime::GetDateTime()
{
    return dateTime;
}

