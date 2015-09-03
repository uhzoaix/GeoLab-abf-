#ifndef RICHPARAMETERS_H
#define RICHPARAMETERS_H
#include <QVariant>
#include <QString>
#include <QVector>
#include <QMap>
#include <Core/gcl_global.h>
namespace GCL { namespace Plugins {


class GCL_PLUGIN_SHARED_EXPORT Parameter
{
public:
    enum Type
    {
        NONE = -1,
        BOOL = 0,
        INT = 1,
        REAL = 2,
        STRINGLIST = 3
    };

    Parameter(const QVariant& defaultVal);
    Parameter(const bool& defaultVal);
    Parameter(const int &defaultVal);
    Parameter(const double &defaultVal);
    Parameter(const QStringList& vals, const int& defaultIndex);

    int getType() const {return mType;}
    QString getName() const;
    void setName(const QString &value);
    void setLabel(const QString& label) {mLabel = label;}
    void setToolTip(const QString& tooltip) {mToolTip = tooltip;}
    QString getLabel() const {return mLabel;}
    QString getToolTip() const {return mToolTip;}


    void setValue(const QVariant& val) {mVal = val;}
    void setValue(const bool& val) {mVal = QVariant(val);}
    void setValue(const int &val) {mVal = QVariant(val);}
    void setValue(const double &val) {mVal = QVariant(val);}
    void setValue(const QStringList& vals, const int& index);
    void setValueIndex(int index) {mIndex = index;}

    void resetDefault() {mVal = mDefaultVal; mIndex = mDefaultIndex;}

    QVariant getValue() const {return mVal;}
    int getStringListIndex() const {return mIndex;}

    bool getValueBool() const {return mVal.toBool();}
    int getValueInt() const {return mVal.toInt();}
    double getValueDouble() const {return mVal.toDouble();}
    QStringList getValueStringList() const {return mVal.toStringList();}

    double getDefault() const;


    void setHasValidator(bool t) {mHasValidator = t;}
    bool hasValidator() const {return mHasValidator;}
    void setValidMax(double val) {mValidatorMax = val;}
    double getValidMax() const {return mValidatorMax;}

    void setValidMin(double val) {mValidatorMin = val;}
    double getValidMin() const {return mValidatorMin;}

private:
    QVariant mVal;
    int mIndex;
    int mDefaultIndex;

    QVariant mDefaultVal;

    QString mName;
    QString mLabel;
    QString mToolTip;


    bool mHasValidator;
    double mValidatorMax;
    double mValidatorMin;

    int mType;
};

class GCL_PLUGIN_SHARED_EXPORT RichParameters
{
public:
    RichParameters();
    ~RichParameters();
    QString getName() const;
    void setName(const QString &value);

    QString getLabel() const;
    void setLabel(const QString &value);

    QString getIntroduction() const;
    void setIntroduction(const QString &value);

    QVariant getValue(const QString& name) const;
    bool getValueBool(const QString& name) const;
    int getValueInt(const QString& name) const;
    double getValueDouble(const QString& name) const;

    QStringList getValueStringList(const QString& name) const;
    int getListIndex(const QString& name) const;
    QVector<Parameter *>& getAllParameters() {return mParameterVector;}


    void setValue(const QString& name, const QVariant& val);
    void setValue(const QString& name, bool val);
    void setValue(const QString& name, int val);
    void setValue(const QString& name, double val);

    void setListIndex(const QString& name, int index);


    void addParameter(const QString& name, bool defaultVal , const QString& label="", const QString& tooltip="");
    void addParameter(const QString& name, int defaultVal , const QString& label="", const QString& tooltip="",
                      bool hasValidate=false, int minValue=-9999, int maxValue=9999);
    void addParameter(const QString& name, double defaultVal , const QString& label="", const QString& tooltip="",
                      bool isValidate=false, double minValue=-9999, double maxValue=9999);

    void addParameter(const QString& name, const QStringList& stringlist, int defaultIndex, const QString& label="", const QString& tooltip="");

private:
    void addParameter(const QString &name, Parameter* parameter, const QString &label, const QString &tooltip);

    QString mName;
    QString mLabel;
    QString mIntroduction;
    QMap<QString, Parameter*> mParametersDictionary;
    QVector<Parameter *> mParameterVector;
};
}}
#endif // RICHPARAMETERS_H
