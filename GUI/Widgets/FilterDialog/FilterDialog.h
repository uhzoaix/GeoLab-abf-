#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H
#include <QDialog>
#include <QAction>
#include "Core/gcl_global.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
namespace GCL {
namespace Plugins {
class Parameter;
class MeshFilterInterface;
}

namespace GUI {

using namespace Plugins;
class GCL_GUI_SHARED_EXPORT ParameterRowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParameterRowWidget(Parameter *para, QWidget *parent = 0);
    void updateParameter();
    void resetParameter();
    void resetDefaultParameter();

private:
    Parameter* mpParameter;
    QWidget* mpValueWidget;
};

class GCL_GUI_SHARED_EXPORT FilterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FilterDialog(QWidget *parent = 0);
signals:
    void readyToApplyFilter(Plugins::MeshFilterInterface*);
public slots:
    void showFilterDialog(Plugins::MeshFilterInterface* pFilter);
    void OnDefaultButtonClick();
    void OnApplyButtonClick();
    void OnCancelButtonClick();

private:
    QList<ParameterRowWidget *> mParameterRowWidgetList;
    MeshFilterInterface* mpFilter;
    QLabel* mpIntroductionLabel;
    QVBoxLayout *mpVBoxLayout;
};

class GCL_GUI_SHARED_EXPORT OpenFilterDialogAction : public QAction
{
    Q_OBJECT
public:
    explicit OpenFilterDialogAction(MeshFilterInterface* pFilter, FilterDialog* pDialog,  QObject* parent = 0);
public slots:
    void showDialog();
private:
    FilterDialog *mpDialog;
    MeshFilterInterface *mpFilter;
};
}}
#endif // PARAMETERDIALOG_H
