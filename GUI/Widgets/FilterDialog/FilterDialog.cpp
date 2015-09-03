#include "FilterDialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QValidator>
#include <QComboBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <PluginCore/PluginInterface/MeshFilterInterface.h>
#include <PluginCore/PluginInterface/RichParameters.h>
namespace GCL { namespace GUI {
using namespace Plugins;

ParameterRowWidget::ParameterRowWidget(Parameter *para, QWidget *parent)
    :mpParameter(para),QWidget(parent)
{
    if(para == NULL)
    {
        return;
    }
    QHBoxLayout *pHboxLayout = new QHBoxLayout;
    this->setLayout(pHboxLayout);
    QLabel *pNameLabel = new QLabel(mpParameter->getLabel());
    this->setToolTip(mpParameter->getToolTip());
    this->setWhatsThis(mpParameter->getToolTip());
    pHboxLayout->addWidget(pNameLabel,1);
    if(mpParameter->getType() == Parameter::BOOL)
    {
        QCheckBox *pCheckBox = new QCheckBox;
        pHboxLayout->addWidget(pCheckBox,2);
        pCheckBox->setChecked(mpParameter->getValueBool());
        mpValueWidget = pCheckBox;
//        connect(pCheckBox,SIGNAL(clicked(bool)),this,SLOT(valueChange(bool)));
    }
    else if(mpParameter->getType() == Parameter::INT)
    {
        QLineEdit *pLineEdit = new QLineEdit;
        if(mpParameter->hasValidator())
        {
            int minvalue = (int)mpParameter->getValidMin();
            int maxvalue = (int)mpParameter->getValidMax();
            pLineEdit->setValidator(new QIntValidator(minvalue,maxvalue,this));
        }
        pLineEdit->setText(QString::number(mpParameter->getValueInt()));
        pHboxLayout->addWidget(pLineEdit,2);
        mpValueWidget = pLineEdit;
    }
    else if(mpParameter->getType() == Parameter::REAL)
    {
        QLineEdit *pLineEdit = new QLineEdit;
        if(mpParameter->hasValidator())
        {
            double minvalue = mpParameter->getValidMin();
            double maxvalue = mpParameter->getValidMax();
            pLineEdit->setValidator(new QDoubleValidator((double)minvalue,(double)maxvalue,4,this));
        }
         pLineEdit->setText(QString::number(mpParameter->getValueDouble()));
        pHboxLayout->addWidget(pLineEdit,2);
         mpValueWidget = pLineEdit;
    }
    else if(mpParameter->getType() == Parameter::STRINGLIST)
    {
        QComboBox *pComboBox = new QComboBox();
        pComboBox->addItems(mpParameter->getValueStringList());
        pComboBox->setCurrentIndex(mpParameter->getStringListIndex());
        pHboxLayout->addWidget(pComboBox,2);
        mpValueWidget = pComboBox;
    }
    this->setFixedWidth(300);
}

void ParameterRowWidget::updateParameter()
{

    if(mpParameter->getType() ==  Parameter::INT)
    {
        QLineEdit* plineEdit = dynamic_cast<QLineEdit *>(mpValueWidget);
        QString t = plineEdit->text();
        bool ok;
        int value = t.toInt(&ok);
        if(ok)
        {
            mpParameter->setValue(value);
        }
    }
    else if(mpParameter->getType() == Parameter::REAL)
    {
        QLineEdit* plineEdit = dynamic_cast<QLineEdit *>(mpValueWidget);
        QString t = plineEdit->text();
        bool ok;
        float value = t.toFloat(&ok);
        if(ok)
        {
            mpParameter->setValue(value);
        }
    }
    else if(mpParameter->getType() == Parameter::BOOL)
    {
         QCheckBox *pCheckBox =  dynamic_cast<QCheckBox *>(mpValueWidget);
         mpParameter->setValue(pCheckBox->isChecked());
    }
    else
    {
         QComboBox *pComboBox =  dynamic_cast<QComboBox *>(mpValueWidget);
         mpParameter->setValueIndex(pComboBox->currentIndex());
    }
    update();
}

void ParameterRowWidget::resetParameter()
{
    if(mpParameter->getType() ==  Parameter::INT)
    {
        QLineEdit* plineEdit = dynamic_cast<QLineEdit *>(mpValueWidget);
        plineEdit->setText(QString::number(mpParameter->getValueInt()));
    }
    else if(mpParameter->getType() == Parameter::REAL)
    {
        QLineEdit* plineEdit = dynamic_cast<QLineEdit *>(mpValueWidget);
        plineEdit->setText(QString::number(mpParameter->getValueDouble()));
    }
    else if(mpParameter->getType() == Parameter::BOOL)
    {
         QCheckBox *pCheckBox =  dynamic_cast<QCheckBox *>(mpValueWidget);
         pCheckBox->setChecked(mpParameter->getValueBool());
    }
    else
    {
         QComboBox *pComboBox =  dynamic_cast<QComboBox *>(mpValueWidget);
         pComboBox->setCurrentIndex(mpParameter->getStringListIndex());
    }
    update();
}

void ParameterRowWidget::resetDefaultParameter()
{
    if(mpParameter->getType() ==  Parameter::INT)
    {
        QLineEdit* plineEdit = dynamic_cast<QLineEdit *>(mpValueWidget);
        plineEdit->setText(QString::number((int)mpParameter->getDefault()));
    }
    else if(mpParameter->getType() == Parameter::REAL)
    {
        QLineEdit* plineEdit = dynamic_cast<QLineEdit *>(mpValueWidget);
        plineEdit->setText(QString::number(mpParameter->getDefault()));
    }
    else if(mpParameter->getType() == Parameter::BOOL)
    {
         QCheckBox *pCheckBox =  dynamic_cast<QCheckBox *>(mpValueWidget);
         pCheckBox->setChecked((bool)(mpParameter->getDefault() > 0.5f));
    }
    else
    {
         QComboBox *pComboBox =  dynamic_cast<QComboBox *>(mpValueWidget);
         pComboBox->setCurrentIndex(mpParameter->getDefault());
    }
    update();
}


FilterDialog::FilterDialog(QWidget *parent)
    :QDialog(parent),mpFilter(NULL)
{
    this->setStyleSheet("font-size:14px;");
    mpVBoxLayout = new QVBoxLayout(this);
    this->setLayout(mpVBoxLayout);
    mpIntroductionLabel = new QLabel(this);
    mpIntroductionLabel->setWordWrap(true);
    mpVBoxLayout->addWidget(mpIntroductionLabel);

    QHBoxLayout *pHboxLayout = new QHBoxLayout;
    QPushButton *pButton = new QPushButton(tr("Default"),this);
    connect(pButton,SIGNAL(clicked()),this,SLOT(OnDefaultButtonClick()));
    pHboxLayout->addWidget(pButton);

    pButton = new QPushButton(tr("Apply"),this);
    connect(pButton,SIGNAL(clicked()),this,SLOT(OnApplyButtonClick()));
    pHboxLayout->addWidget(pButton);

    pButton = new QPushButton(tr("Cancel"),this);
    connect(pButton,SIGNAL(clicked()),this,SLOT(OnCancelButtonClick()));
    pHboxLayout->addWidget(pButton);
    mpVBoxLayout->addLayout(pHboxLayout);

}

void FilterDialog::showFilterDialog(MeshFilterInterface *pFilter)
{
    if(pFilter == NULL) return;
    mpFilter = pFilter;
    foreach(ParameterRowWidget* pWidget, mParameterRowWidgetList)
    {
        mpVBoxLayout->removeWidget(pWidget);
        delete pWidget;
    }
    mParameterRowWidgetList.clear();

    this->setWindowTitle(mpFilter->Parameters().getLabel());
    mpIntroductionLabel->setText(mpFilter->Parameters().getIntroduction());
    int count = 1;
    foreach(Parameter* para, mpFilter->Parameters().getAllParameters())
    {
        ParameterRowWidget* pWidget = new ParameterRowWidget(para,this);
        mParameterRowWidgetList.append(pWidget);
        mpVBoxLayout->insertWidget(count++,pWidget);
    }
    this->show();
}

void FilterDialog::OnDefaultButtonClick()
{
    foreach(ParameterRowWidget* pWidget, mParameterRowWidgetList)
    {
        pWidget->resetDefaultParameter();
    }
}

void FilterDialog::OnApplyButtonClick()
{
    foreach(ParameterRowWidget* pWidget, mParameterRowWidgetList)
    {
        pWidget->updateParameter();
    }
    emit readyToApplyFilter(mpFilter);
}

void FilterDialog::OnCancelButtonClick()
{
    foreach(ParameterRowWidget* pWidget, mParameterRowWidgetList)
    {
        pWidget->resetParameter();
    }
    this->hide();
}


OpenFilterDialogAction::OpenFilterDialogAction(MeshFilterInterface *pFilter, FilterDialog *pDialog, QObject *parent)
    :QAction(parent),mpFilter(pFilter),mpDialog(pDialog)
{
    if(!pFilter) return;
    this->setText(mpFilter->Parameters().getLabel());
    this->setToolTip(mpFilter->Parameters().getIntroduction());
    connect(this,SIGNAL(triggered()),this,SLOT(showDialog()));
}

void OpenFilterDialogAction::showDialog()
{
    if(mpDialog == NULL) return;
    mpDialog->showFilterDialog(mpFilter);
}
}}
