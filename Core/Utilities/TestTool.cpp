#include "TestTool.h"

TimeTestTool* TimeTestTool::mpTimeTestTool = NULL;


TimeTestTool *TimeTestTool::getInstance()
{
    if(mpTimeTestTool == NULL)
    {
        mpTimeTestTool = new TimeTestTool();
    }
    return mpTimeTestTool;
}

