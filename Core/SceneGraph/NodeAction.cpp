#include "NodeAction.h"

namespace GCL { namespace SceneGraph {


NodeAction::NodeAction(const QString& name, bool isCheckable,const QString& label, const QString& category,
                       const QString& tooltip)
    :name_(name),label_(label),category_(category),tooltip_(tooltip),is_checkable_(isCheckable)
{
    if(label_ == "") label_ = name_;
}

void NodeAction::setName(const QString &name)
{
    name_ = name;
}

void NodeAction::setLabel(const QString &label)
{
    label_ = label;
}

void NodeAction::setCategroy(const QString &category)
{
    category_ = category;
}

void NodeAction::setToolTip(const QString &tooltip)
{
    tooltip_ = tooltip;
}


}}
