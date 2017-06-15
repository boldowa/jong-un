#include "jong_unBuildTime.h"

jong_unBuildTime::jong_unBuildTime()
: dateTime(__DATE__ " " __TIME__)
{
}

jong_unBuildTime::~jong_unBuildTime()
{
}

const char* jong_unBuildTime::GetDateTime()
{
    return dateTime;
}

