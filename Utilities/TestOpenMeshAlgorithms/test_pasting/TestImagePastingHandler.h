#ifndef TESTIMAGEPASTINGHANDLER_H
#define TESTIMAGEPASTINGHANDLER_H

#include <GCL/Core/SceneGraph/EventHandler/EventHandler.h>
namespace GCL {
namespace Utilities {
class ImagePasting;
}

namespace SceneGraph {

class SceneGraphNode;
class TestImagePastingHandler : public EventHandler
{
public:
    TestImagePastingHandler(Utilities::ImagePasting* image_pasting, SceneGraphNode* _scenegraph);
    ~TestImagePastingHandler();

    virtual int mouseMove(int posX, int posY) {return 0;}
    virtual int mousePress(int posX,int posY, unsigned int button, unsigned int modifier = NoModifier) { button_ = button; modifier_ = modifier; return 0;}
    virtual int mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier = NoModifier) {button_ = NoButton; modifier = NoModifier; return 0;}
    virtual int keyUp(const unsigned int& key, unsigned int modifier = NoModifier) {return 0;}
    virtual int keyDown(const unsigned int& key, unsigned int modifier = NoModifier) {return 0;}

private:
    SceneGraphNode* scenegraph_;
};
}}
#endif // TESTIMAGEPASTINGHANDLER_H
