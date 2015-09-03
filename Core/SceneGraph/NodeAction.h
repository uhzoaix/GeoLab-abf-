#ifndef GCL_NODEACTION_H
#define GCL_NODEACTION_H
#include "Core/gcl_global.h"
#include <QString>
namespace GCL { namespace SceneGraph {
class GCL_CORE_SHARED_EXPORT NodeAction
{
public:
    NodeAction(const QString& name, bool isCheckable = false, const QString& label = "", const QString& category = "",
               const QString& tooltip = "");
    void setName(const QString& name);
    void setLabel(const QString& label);
    void setCategroy(const QString& category);
    void setToolTip(const QString& tooltip);

    const QString& getName() const {return name_;}
    const QString& getLabel() const {return label_;}
    const QString& getCategory() const {return category_;}
    const QString& getToolTip() const {return tooltip_;}

    void setCheckable(bool t) {is_checkable_ = t;}
    bool isCheckable() const {return is_checkable_;}
private:
    QString category_;
    QString name_;
    QString label_;
    QString tooltip_;
    bool is_checkable_;
};
}}
#endif // NODEACTION_H
