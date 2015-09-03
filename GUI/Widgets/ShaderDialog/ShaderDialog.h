#ifndef SHADERDIALOG_H
#define SHADERDIALOG_H

#include <QAction>
#include <QDialog>
class QTextEdit;
class QLabel;
class QVBoxLayout;
namespace GCL {
namespace SceneGraph {
struct UniformVariable;
class SceneGraphNode;

}

namespace GUI {
class UniformVariableRowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UniformVariableRowWidget(SceneGraph::UniformVariable& UVar,QWidget *parent = 0);
signals:
    void signal_value_changed();
public slots:
    void valueChanged(const QString&);
    void valueChanged(int);
    void valueChanged();
private:
    QList< QWidget* > mWiddgetList;
    SceneGraph::UniformVariable& mUniformVariable;
};


class ShaderDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ShaderDialog(SceneGraph::SceneGraphNode* scenegraph, QWidget *parent = 0);
public slots:
    void initShaderByGDPAndShow(const QString& GDPfilename);
    void update_scenegraph();
private:
    void resetUniformRowWidgets();
    QList< UniformVariableRowWidget* > mUniformVariableRowWidgetList;
    QVBoxLayout *mpVBoxLayout;
    SceneGraph::SceneGraphNode* scenegraph_;
};

class OpenShaderDialogAction : public QAction
{
    Q_OBJECT
public:
    explicit OpenShaderDialogAction(const QString& gdpFileName, ShaderDialog* pDialog, QObject* parent = 0);
signals:
    void openShaderDialogByGDP(const QString& GDPfilename);
public slots:
    void openDialog(bool t);
private:
    ShaderDialog* mpDialog;
    QString mGDPFileName;
};
}}

#endif // SHADERDIALOG_H
