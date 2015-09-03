#ifndef ALGOPROGRESS_H
#define ALGOPROGRESS_H
#include "Core/gcl_global.h"
#include <string>
#include <QObject>
// Progress
namespace GCL{
class GCL_CORE_SHARED_EXPORT ProgressTool : public QObject
{
    Q_OBJECT
public:
    ProgressTool(QObject* parent = 0);
    ~ProgressTool();
    static ProgressTool* currentProgressTool() {return progressTool_;}
signals:
    void signalInitProgress(int maxSize, const std::string& progressName = "");
    void signalSetProgressPos(int pos);
    void signalFinishProgress();
public:
    void initProgress(int maxSize, const std::string& progressName = "");
    void setProgressPos(int pos);
    void finishProgress();
private:
    static ProgressTool* progressTool_;

};

#define PROGRESS_INIT(maxSize, progressName) ProgressTool::currentProgressTool()->initProgress(maxSize,progressName)
#define PROGRESS_POS(pos)  ProgressTool::currentProgressTool()->setProgressPos(pos)
#define PROGRESS_FINISH()  ProgressTool::currentProgressTool()->finishProgress()

}
#endif // ALGOPROGRESS_H
