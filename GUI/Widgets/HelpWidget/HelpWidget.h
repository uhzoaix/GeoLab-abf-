#ifndef HELPWIDGET_H
#define HELPWIDGET_H
#include "Core/gcl_global.h"

#include <QTabWidget>
class QTextEdit;
namespace GCL { namespace GUI {
class GCL_GUI_SHARED_EXPORT HelpWidget : public QTabWidget
{
    Q_OBJECT
public:


    explicit HelpWidget(const QString& helpString = "",
                        const QString& aboutString = "",QWidget *parent = 0);

    void setHelpText(const QString& helpString);
    void setAboutText(const QString& aboutString);
public slots:
    void showHelpWidget(const QString& helpString, const QString& aboutString);
private:

    QTextEdit* helpTextEdit_;
    QTextEdit* aboutTextEdit_;

};
}}
#endif // HELPWIDGET_H
