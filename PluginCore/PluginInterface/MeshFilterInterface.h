#ifndef MESHFILTERINTERFACE_H
#define MESHFILTERINTERFACE_H
#include "PluginInterface.h"
#include "RichParameters.h"
#include <memory>
namespace GCL {

class OMesh;
namespace Plugins {
class GCL_PLUGIN_SHARED_EXPORT MeshFilterInterface : public PluginInterface
{
    Q_OBJECT
public:
    enum FilterType
    {
        FILTER_REPAIRING = 0,   ///<* Repairing: Fill Hole, Normal Updated, ...
        FILTER_REMESHING = 1,   ///<* Remeshing: Simplification, Optimization, Subdivision
        FILTER_SMOOTHING = 2,   ///<* Smoothing: Smoothing, Fairing
        FILTER_SAMPLING =  3,   ///<* Sampling: sampling, noise adding
        FILTER_RECONSTRUCTION = 4, ///<*  Reconstruction: Reconstruction
        FILTER_PARAMETRIZATION = 5, ///<* Parametrization: Parametrization
        FILTER_OTHER = 6        ///<* Other
    };

    MeshFilterInterface(QObject* parent = 0);
    virtual ~MeshFilterInterface();
    int getType() const {return PLUGIN_FILTER;}
    QString getCategory() const;

    static size_t getNumberOfFilterCategories()  {return 7;}
    static QString getFilterCategoryName(size_t i);

    virtual int getFilterType() const = 0;
    virtual void initParameters() = 0;

    OMesh* getMesh();

    void updateMesh(int dirtyType);


    RichParameters& Parameters() {return parameters_;}
    const RichParameters& Parameters() const {return parameters_;}
public slots:
    virtual bool apply() = 0;



protected:
    RichParameters parameters_;
};
}}


#define GCL_MESHFILTER_INTERFACE_IID "gcl.mesh.meshfilterinterface/1.0"
Q_DECLARE_INTERFACE(GCL::Plugins::MeshFilterInterface,
                    GCL_MESHFILTER_INTERFACE_IID)
#endif // MESHFILTERINTERFACE_H
