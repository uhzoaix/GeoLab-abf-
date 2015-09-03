#include "ProgressTool.h"
#include "LogSystem.h"

namespace GCL{
ProgressTool* ProgressTool::progressTool_ = NULL;
ProgressTool::ProgressTool(QObject *parent)
    :QObject(parent)
{
    progressTool_ = this;
}



ProgressTool::~ProgressTool()
{
}

void ProgressTool::initProgress(int maxSize, const std::string &progressName)
{
     emit this->signalInitProgress(maxSize,progressName);
}



void ProgressTool::setProgressPos(int pos)
{
    emit this->signalSetProgressPos(pos);
}

void ProgressTool::finishProgress()
{
    emit this->signalFinishProgress();
}
}
