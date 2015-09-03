#ifndef TESTTOOL_H
#define TESTTOOL_H
#include "LogSystem.h"



#ifndef TIME_TEST
#define TimeTestInit(name)
#define TimeTestCheck()
#else
#define TimeTestInit(name) TimeTestTool::getInstance()->init(name)
#define TimeTestCheck() TimeTestTool::getInstance()->check()
#endif
#include <time.h>
#include <string>
class TimeTestTool
{
private:
    TimeTestTool() {}
    clock_t t0;
    clock_t t1;
    clock_t t2;
    static TimeTestTool* mpTimeTestTool;
    std::string mName;
public:
    ~TimeTestTool() {}
    static TimeTestTool* getInstance();
    void init(const std::string& name)
    {
        t0 = clock();
        t1 = t0;
        DebugLog<<"TimeTest: "<<name<<std::endl;
        mName = name;
    }
    void check()
    {
        t2 = clock();
        DebugLog<<"TimeTest: "<<mName<<"---> Total Time: "<<t2 - t0<<"ms, Sub Time: "<< t2 - t1<< "ms "<<std::endl;
        t1 = t2;
    }
};

#endif // TESTTOOL_H
