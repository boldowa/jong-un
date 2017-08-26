#include "unkoBuildTime.h"

unkoBuildTime::unkoBuildTime()
: dateTime(__DATE__ " " __TIME__)
{
}

unkoBuildTime::~unkoBuildTime()
{
}

const char* unkoBuildTime::GetDateTime()
{
    return dateTime;
}

