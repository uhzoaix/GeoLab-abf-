#ifndef GCL_SCENEGRAPH_EVENTHANDLER_H
#define GCL_SCENEGRAPH_EVENTHANDLER_H
#include "../../gclnamespace.h"

namespace GCL { namespace SceneGraph {
/*
* EventHandler provides a basic interface for any class which wants to handle
a GUI Events.
*/

class EventHandler
{
public:
    enum EventHandlerType
    {
        Normal,
        Manipulator,
        Selection,
        CtrlPointsTransformer
    };

    EventHandler(): enabled_(true) {}
    virtual ~EventHandler() {}
    virtual const char* getCategory() const {return "";}
    virtual const char* getEventHandlerName() const= 0;
    virtual EventHandlerType getType() const {return  Normal;}

    void setEnable(bool enable) {enabled_ = enable;}
    bool isEnable() const {return enabled_;}

    virtual int resize(int w, int h) {return 0;}
    virtual int mouseMove(int posX, int posY) {return 0;}
    virtual int mousePress(int posX,int posY, unsigned int button, unsigned int modifier = NoModifier) { button_ = button; modifier_ = modifier; return 0;}
    virtual int mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier = NoModifier) {button_ = NoButton; modifier = NoModifier; return 0;}
    virtual int keyUp(const unsigned int& key, unsigned int modifier = NoModifier) {return 0;}
    virtual int keyDown(const unsigned int& key, unsigned int modifier = NoModifier) {return 0;}
    virtual int wheel(const float& delta) {return 0;}

    int getButton()  const {return button_;}
    int getModifier() const {return modifier_;}
protected:
    unsigned int button_;
    unsigned int modifier_;

private:
    bool enabled_;


};
}}
#endif // EVENTHANDLER_H
