#include "RichParameters.h"
#include <Core/Utilities/LogSystem.h>
namespace GCL { namespace Plugins {


Parameter::Parameter(const QVariant &defaultVal)
    :mIndex(0), mDefaultIndex(0),
      mVal(defaultVal),mDefaultVal(defaultVal),mHasValidator(false),mType(-1)
{
}

Parameter::Parameter(const bool &defaultVal)
    : mIndex(0), mDefaultIndex(0),
      mVal(QVariant(defaultVal)),mDefaultVal(QVariant(defaultVal)),mHasValidator(false),mType(BOOL)
{

}

Parameter::Parameter(const int &defaultVal)
    : mIndex(0), mDefaultIndex(0),
      mVal(QVariant(defaultVal)),mDefaultVal(QVariant(defaultVal)),mHasValidator(false),mType(INT)
{

}

Parameter::Parameter(const double &defaultVal)
    :mIndex(0), mDefaultIndex(0),
      mVal(QVariant(defaultVal)),mDefaultVal(QVariant(defaultVal)),mHasValidator(false),mType(REAL)
{

}




Parameter::Parameter(const QStringList &vals, const int &defaultIndex)
    :mIndex(defaultIndex), mDefaultIndex(defaultIndex),mHasValidator(false),mType(STRINGLIST)
{
    setValue(vals,defaultIndex);
}
void Parameter::setValue(const QStringList &vals, const int &index)
{
    mVal = QVariant(vals);
    mIndex = index;
}

double Parameter::getDefault() const
{
    if(mType == STRINGLIST)
    {
        return (double)mDefaultIndex;
    }
    else
    {
        return (double)mDefaultVal.toDouble();
    }
}


QString Parameter::getName() const
{
    return mName;
}

void Parameter::setName(const QString &value)
{
    mName = value;
}
RichParameters::RichParameters()
{

}

RichParameters::~RichParameters()
{
    foreach (Parameter* p, mParameterVector) {
        delete p;
    }
}

QString RichParameters::getName() const
{
    return mName;
}

void RichParameters::setName(const QString &value)
{
    mName = value;
}
QString RichParameters::getLabel() const
{
    return mLabel;
}

void RichParameters::setLabel(const QString &value)
{
    mLabel = value;
}


QString RichParameters::getIntroduction() const
{
    return mIntroduction;
}

void RichParameters::setIntroduction(const QString &value)
{
    mIntroduction = value;
}

QVariant RichParameters::getValue(const QString &name) const
{
    if(!mParametersDictionary.contains(name)) return QVariant();
    Parameter* parameter = mParametersDictionary.value(name);
    return parameter->getValue();

}

bool RichParameters::getValueBool(const QString &name) const
{
    if(!mParametersDictionary.contains(name)) return false;
    Parameter* parameter = mParametersDictionary.value(name);
    if(parameter->getValue().type() != QVariant::Bool)
    {
        DebugLog<<"(parameter->getValue().type() != QVariant::Bool)"<<std::endl;
        return 0;
    }
    return parameter->getValueBool();
}

int RichParameters::getValueInt(const QString &name) const
{
    if(!mParametersDictionary.contains(name)) return 0;
    Parameter* parameter = mParametersDictionary.value(name);
    if(parameter->getValue().type() != QVariant::Int)
    {
        DebugLog<<"(parameter->getValue().type() != QVariant::Int)"<<std::endl;
        return 0;
    }
    return parameter->getValueInt();
}

double RichParameters::getValueDouble(const QString &name) const
{
    if(!mParametersDictionary.contains(name)) return 0;
    Parameter* parameter = mParametersDictionary.value(name);
    if(parameter->getValue().type() != QVariant::Double)
    {
        DebugLog<<"(parameter->getValue().type() != QVariant::Double)"<<std::endl;
        return 0;
    }
    return parameter->getValueDouble();
}
QStringList RichParameters::getValueStringList(const QString &name) const
{
    if(!mParametersDictionary.contains(name)) return QStringList();
    Parameter* parameter = mParametersDictionary.value(name);
    return parameter->getValue().toStringList();
}

int RichParameters::getListIndex(const QString &name) const
{
    if(!mParametersDictionary.contains(name)) return 0;
    Parameter* parameter = mParametersDictionary.value(name);
    return parameter->getStringListIndex();
}

void RichParameters::setValue(const QString &name, const QVariant &val)
{
    if(!mParametersDictionary.contains(name)) return;
    Parameter* parameter = mParametersDictionary.value(name);
    parameter->setValue(val);

}
void RichParameters::setValue(const QString &name, bool val)
{
    if(!mParametersDictionary.contains(name)) return;
    Parameter* parameter = mParametersDictionary.value(name);
    parameter->setValue(val);

}
void RichParameters::setValue(const QString &name, int val)
{
    if(!mParametersDictionary.contains(name)) return;
    Parameter* parameter = mParametersDictionary.value(name);
    parameter->setValue(val);

}
void RichParameters::setValue(const QString &name, double val)
{
    if(!mParametersDictionary.contains(name)) return;
    Parameter* parameter = mParametersDictionary.value(name);
    parameter->setValue(val);

}

void RichParameters::setListIndex(const QString &name,int index)
{
    if(!mParametersDictionary.contains(name)) return;
    Parameter* parameter = mParametersDictionary.value(name);
    parameter->setValueIndex(index);
}
void RichParameters::addParameter(const QString &name, bool defaultVal , const QString &label, const QString &tooltip)
{
    if(!mParametersDictionary.contains(name))
    {
        Parameter* parameter = new Parameter(defaultVal);
        addParameter(name,parameter,label,tooltip);
    }
}

void RichParameters::addParameter(const QString &name, int defaultVal , const QString &label, const QString &tooltip, bool hasValidate, int minValue, int maxValue)
{
    if(!mParametersDictionary.contains(name))
    {
        Parameter* parameter = new Parameter(defaultVal);
        if(hasValidate)
        {
            parameter->setValidMax((double)maxValue);
            parameter->setValidMin((double)minValue);
        }
        addParameter(name,parameter,label,tooltip);

    }
}

void RichParameters::addParameter(const QString &name, double defaultVal , const QString &label, const QString &tooltip, bool isValidate, double minValue, double maxValue)
{
    if(!mParametersDictionary.contains(name))
    {
        Parameter* parameter = new Parameter(defaultVal);
        if(parameter->hasValidator())
        {
            parameter->setValidMax((double)maxValue);
            parameter->setValidMin((double)minValue);
        }
        addParameter(name,parameter,label,tooltip);

    }
}

void RichParameters::addParameter(const QString &name, const QStringList &stringlist, int defaultIndex, const QString &label, const QString &tooltip)
{
    if(!mParametersDictionary.contains(name))
    {
        Parameter* parameter = new Parameter(stringlist,defaultIndex);
        addParameter(name,parameter,label,tooltip);
    }
}

void RichParameters::addParameter(const QString &name, Parameter *parameter, const QString &label, const QString &tooltip)
{
    parameter->setName(name);
    parameter->setLabel(label);
    parameter->setToolTip(tooltip);
    mParameterVector.push_back(parameter);
    mParametersDictionary.insert(name,parameter);
}

}}
