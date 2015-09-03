#ifndef LAPLACIANSMOOTHINGPLUGIN_H
#define LAPLACIANSMOOTHINGPLUGIN_H
#include <PluginCore/PluginInterface/MeshFilterInterface.h>

namespace GCL {

class OMesh;
namespace Plugins {
/**
 * @brief The LaplacianSmoothingPlugin class
 *       Laplacian Smoothing algorithm
 * @author Shiwei Wang
 * @date 2015.1.30
 * @ref  Laplacian Smoothing
 */

class LaplacianSmoothingPlugin : public MeshFilterInterface
{
    Q_OBJECT
    GCL_MESH_PLUGIN_IID_EXPORTER(GCL_MESHFILTER_INTERFACE_IID)
    Q_INTERFACES(GCL::Plugins::MeshFilterInterface)
public:
    explicit LaplacianSmoothingPlugin();
    int getFilterType() const;
    void initParameters();
public slots:
    bool apply();
public:
    void localLaplacianSmoothing(OMesh& mesh,double steplen = 0.1);

    void globalLaplacianSmoothing(OMesh& mesh,double steplen = 0.1);


};
}}
#endif // LAPLACIANSMOOTHINGPLUGIN_H
