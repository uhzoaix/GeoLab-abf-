#include "HelpWidget.h"
#include <QTextEdit>
namespace GCL { namespace GUI {
HelpWidget::HelpWidget(const QString &helpString,
                       const QString &aboutString, QWidget *parent) :
    QTabWidget(parent)
{
      helpTextEdit_ = new QTextEdit(this);
      helpTextEdit_->setReadOnly(true);
      helpTextEdit_->setHtml(helpString);
      this->addTab(helpTextEdit_,tr("Help"));

      aboutTextEdit_ = new QTextEdit(this);
      aboutTextEdit_->setReadOnly(true);
      aboutTextEdit_->setHtml(aboutString);
      this->addTab(aboutTextEdit_,tr("About"));

      this->resize(600,600);
}

void HelpWidget::setHelpText(const QString &helpString)
{
    helpTextEdit_->setHtml(helpString);
}

void HelpWidget::setAboutText(const QString &aboutString)
{
    aboutTextEdit_->setHtml(aboutString);
}

void HelpWidget::showHelpWidget(const QString &helpString, const QString &aboutString)
{
     helpTextEdit_->setHtml(helpString);
     aboutTextEdit_->setHtml(aboutString);
     this->show();
}


}}
