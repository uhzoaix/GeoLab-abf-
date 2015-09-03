#ifndef GCL_GUI_GUINODEFACTORY_H
#define GCL_GUI_GUINODEFACTORY_H

#include <Core/SceneGraph/NodeFactory.h>
namespace GCL {
namespace SceneGraph{
class Node;
class OpenMeshNode;
}
namespace GUI {
/**
 * @brief The GuiNodeFactory class
 *      这是NodeFactory类的派生类. 由于读入模型文件时,往往涉及许多GUI交互,
 * 比如载入IO插件,比如写入recent files等. 派生出该类,可以用来解决类似的问题;
 * @author Shiwei
 * @date Nov 12, 2014
 */
class MainApplication;
class GCL_GUI_SHARED_EXPORT GuiNodeFactory : public SceneGraph::NodeFactory
{
public:
    GuiNodeFactory(MainApplication& _mainapplication);
    virtual ~GuiNodeFactory();
    /// Create OpenMesh Node by filename
    virtual SceneGraph::OpenMeshNode *createOpenMeshNode(const QString& filename,SceneGraph::Node* parent);
    SceneGraph::DepthImageNode* createDepthImageNode(const QString& filename, SceneGraph::Node *parent);
private:
    MainApplication& mainapplication_;
};
}}


#endif // GUINODEFACTORY_H
