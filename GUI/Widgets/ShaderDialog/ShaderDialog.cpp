#include "ShaderDialog.h"
#include "Core/SceneGraph/Shader.h"
#include <QLineEdit>
#include <QValidator>
#include <QSlider>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QFileInfo>
#include <QVBoxLayout>
#include "Core/SceneGraph/SceneGraph.h"
#define DECFACTOR 100000.0f


namespace GCL { namespace GUI {
using namespace SceneGraph;

UniformVariableRowWidget::UniformVariableRowWidget(SceneGraph::UniformVariable &UVar, QWidget *parent)
    :mUniformVariable(UVar),QWidget(parent)
{
    int varNum = mUniformVariable.getVarNumber();
    QHBoxLayout* pHBoxLayout =new QHBoxLayout(this);
    this->setLayout(pHBoxLayout);
    pHBoxLayout->addWidget(new QLabel(QString(mUniformVariable.name.c_str()),this),1);
    switch(mUniformVariable.widget)
    {
    case WIDGET_NONE:
        for(int i=0; i < varNum; i++)
        {
            QLineEdit* pline = new QLineEdit(this);
            pHBoxLayout->addWidget(pline);
            mWiddgetList.append(pline);
            pline->setAlignment(Qt::AlignRight);
            if(mUniformVariable.type == SINGLE_INT)
            {
                pline->setText(QString::number(mUniformVariable.ival));
                pline->setValidator(new QIntValidator(this));
            }
            else
            {
                pline->setText(QString::number(mUniformVariable.fval[i]));
                pline->setValidator(new QDoubleValidator(this));
            }
            connect(pline,SIGNAL(textChanged(QString)),this,SLOT(valueChanged(QString)));
        }
        break;
    case WIDGET_SLIDER:
        for(int i=0; i < varNum; i++)
        {
            QSlider *qslider = new QSlider(this);
            mWiddgetList.append(qslider);
            pHBoxLayout->addWidget(qslider);
            qslider->setTickPosition(QSlider::NoTicks);
            qslider->setOrientation(Qt::Horizontal);
            if (mUniformVariable.type == SINGLE_INT)
            {
                qslider->setTickInterval(mUniformVariable.step);
                qslider->setRange(mUniformVariable.min, mUniformVariable.max);
                qslider->setValue(mUniformVariable.ival);
            }
            else
            {
                qslider->setTickInterval(mUniformVariable.step*DECFACTOR);
                qslider->setRange(mUniformVariable.min*DECFACTOR, mUniformVariable.max*DECFACTOR);
                qslider->setValue(mUniformVariable.fval[i]*DECFACTOR);
            }
            connect(qslider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
        }
        break;
    case WIDGET_COLOR:
        QPushButton* pButton = new QPushButton(tr("Change Color"),this);
        pHBoxLayout->addWidget(pButton);
        connect(pButton,SIGNAL(clicked()),this,SLOT(valueChanged()));
    }
}



void UniformVariableRowWidget::valueChanged(const QString &)
{
    for(int i=0; i < mWiddgetList.size(); i++)
    {
        QLineEdit* plineEdit = dynamic_cast<QLineEdit *>(mWiddgetList[i]);
        if(mUniformVariable.type == SINGLE_INT)
        {
            mUniformVariable.ival = plineEdit->text().toInt();
        }
        else
        {
            mUniformVariable.fval[i] = plineEdit->text().toFloat();
        }
    }
    emit signal_value_changed();
}

void UniformVariableRowWidget::valueChanged(int)
{
    for(int i=0; i < mWiddgetList.size(); i++)
    {
        QSlider* pSlider = dynamic_cast<QSlider *>(mWiddgetList[i]);
        if(mUniformVariable.type == SINGLE_INT)
        {
            mUniformVariable.ival = pSlider->value();
        }
        else
        {
            mUniformVariable.fval[i] = (float)pSlider->value() / DECFACTOR;
        }
    }
    emit signal_value_changed();

}

void UniformVariableRowWidget::valueChanged()
{
    for(int i=0; i < 4; i++)
    {
        if(mUniformVariable.fval[i] > 1)
        {
            mUniformVariable.fval[i] = 0.9999f;
        }
        if(mUniformVariable.fval[i] < 0.f)
        {
            mUniformVariable.fval[i] = 0.f;
        }
    }
    QColor initColor(255 * mUniformVariable.fval[0], 255 * mUniformVariable.fval[1],
            255 * mUniformVariable.fval[2], 255 * mUniformVariable.fval[3]);
    QColor color = QColorDialog::getColor(initColor,this);
    mUniformVariable.fval[0] = (float)color.red() / (float)255;
    mUniformVariable.fval[1] = (float)color.green() / (float)255;
    mUniformVariable.fval[2] = (float)color.blue() / (float)255;
    mUniformVariable.fval[3] = (float)color.alpha() / (float)255;
    emit signal_value_changed();

}



ShaderDialog::ShaderDialog(SceneGraphNode *scenegraph, QWidget *parent):
    QDialog(parent),scenegraph_(scenegraph)
{
    mpVBoxLayout = new QVBoxLayout(this);
    this->setLayout(mpVBoxLayout);
    this->resize(500,500);
}

void ShaderDialog::initShaderByGDPAndShow(const QString &GDPfilename)
{

    scenegraph_->getShader()->loadShaderFromGDPFile(GDPfilename);
    resetUniformRowWidgets();
    update_scenegraph();
    this->show();
}

void ShaderDialog::update_scenegraph()
{
    scenegraph_->nodeUpdated();
}

void ShaderDialog::resetUniformRowWidgets()
{
    foreach(UniformVariableRowWidget* pWidget, mUniformVariableRowWidgetList)
    {
        mpVBoxLayout->removeWidget(pWidget);
        delete pWidget;
    }
    mUniformVariableRowWidgetList.clear();
    for(int i=0; i < scenegraph_->getShader()->getUniformVariablesNumber(); i++)
    {
        UniformVariableRowWidget* row_widget = new UniformVariableRowWidget(scenegraph_->getShader()->getUniformVariable(i),this);
        mUniformVariableRowWidgetList.push_back(row_widget);
        mpVBoxLayout->addWidget( mUniformVariableRowWidgetList.back());
        if(this->parent() != NULL)
        {
            connect(row_widget,SIGNAL(signal_value_changed()),this,SLOT(update_scenegraph()));
        }
    }

}




OpenShaderDialogAction::OpenShaderDialogAction(const QString &gdpFileName, ShaderDialog *pDialog, QObject *parent)
    :mGDPFileName(gdpFileName),mpDialog(pDialog),QAction(parent)
{
    QFileInfo fileInfo(gdpFileName);
    this->setText(fileInfo.baseName());
    connect(this,SIGNAL(openShaderDialogByGDP(QString)),pDialog,SLOT(initShaderByGDPAndShow(QString)));
    connect(this,SIGNAL(triggered(bool)),this,SLOT(openDialog(bool)));
    this->setCheckable(true);
}

void OpenShaderDialogAction::openDialog(bool t)
{
    if(t)
    {
        emit openShaderDialogByGDP(mGDPFileName);
    }
}
}}
