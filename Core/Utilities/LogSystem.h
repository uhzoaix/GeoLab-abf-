#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H
#include <iostream>
#include <sstream>
#ifdef QT_CORE_LIB
#include <QFile>
#else
#include <fstream>
#endif

namespace GCL {


//  It seems that there is a conflict between std::ofstream and Qt in win32-g++-Release
//  so that std::stringstream is used to save the log information.
class LogSystem
{
public:
    LogSystem() {}
    ~LogSystem()
    {
#ifdef QT_CORE_LIB
        QFile file("log.txt");
        if(file.open(QIODevice::Append))
        {
            file.write(mOStringStream.str().c_str());
        }
        file.close();
#else
        std::ofstream out;
        out.open("log.txt",std::ios_base::app);
        if(out.good())
        {
            out<<mOStringStream.str();
        }
        out.close();
#endif
    }
    std::ostringstream& getStream() {return mOStringStream;}
private:
    std::ostringstream mOStringStream;
};

#define DebugEnd std::endl
#define DebugLog std::cout
#define InfoLog GCL::LogSystem().getStream()
#define WarnLog GCL::LogSystem().getStream()<<"Warn: "
#define ErrorLog GCL::LogSystem().getStream()<<"Error: "

}
#endif // LOGSYSTEM_H
