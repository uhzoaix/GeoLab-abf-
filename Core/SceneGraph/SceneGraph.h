#ifndef GCL_SCENEGRAPH_H
#define GCL_SCENEGRAPH_H
#include "Node.h"
#include <QMutex>
#include <memory>
#include <QList>
#include "../gclnamespace.h"
namespace GCL{
namespace  Decoration{
class DecorationBase;
}
namespace SceneGraph {
/**
    This is the root of Scene tree;
    The Camera and the Lights wound be set before rendering;
    We add an QMutex here to make sure sthread safe.

    I (Shiwei) pefer Qt's containers(QVector, QList, QString ...)  than STL's,
    since more grace coding technique can be used, e.g. foreach ...(STL in MSVS 2010 dosen't support foreach...);

*/
class Camera;
class Lights;
class GLStates;
class EventHandler;
class Manipulator;
class NodeFactory;
class Shader;
class GCL_CORE_SHARED_EXPORT SceneGraphNode : public Node
{
    Q_OBJECT
public:
   explicit SceneGraphNode();
   virtual ~SceneGraphNode();

    Node *getScenegraph();
    int getNodeType() const;
    QString getNodeTypeName() const;

    ////lights
    inline Lights* getLights() { return lights_.get(); }

    /// camera
    inline Camera* getCamera() { return camera_.get();}
    inline const Camera* getCamera() const {return camera_.get();}


    /// shader
    inline Shader* getShader() {return shader_.get();}
    inline const Shader* getShader() const {return shader_.get();}
    /// gl states
    inline GLStates* getGLState() {return gl_states_.get();}
    inline const GLStates* getGLState() const {return gl_states_.get();}

    ////visitor
    virtual void accept(Visitor & _visitor);

    ////Locks the mutex. If another thread has locked the mutex then this call will block until that thread has unlocked it.
    void lock();

    ////Unlocks the mutex. Attempting to unlock a mutex in a different thread to the one that locked it results in an error. Unlocking a mutex that is not locked results in undefined behavior.
    void unlock();

    /// Mainipulator : the default is trackballmanipulator
    inline Manipulator* getManipulator() {return manipulator_.get();}
    void setManipulator(const std::shared_ptr<Manipulator>& _manipulator);

    /// NodeFactory
    inline NodeFactory* getNodeFactory() {return node_factory_.get();}
    void setNodeFactory(const std::shared_ptr<NodeFactory>& _node_factory) {node_factory_ = _node_factory;}

    /// EventHandlers
    void addEventHandler(const std::shared_ptr<EventHandler>& _handler);

    /// After calling getEventHandlerByName(), you should check wheather shared_ptr contains NULL pointer
    /// h = getEventHandlerByName(name);
    /// if(h.get() != NULL)
    /// {
    ///     ...
    /// }
    std::shared_ptr<EventHandler> getEventHandlerByName(const QString& name);
    void removeEventHandlerByName(const QString& name);
    void removeEventHandlerByCategory(const QString& category);
    void addDecoration(const std::shared_ptr<Decoration::DecorationBase>& _render_element);
    std::shared_ptr<Decoration::DecorationBase> getDecorationByName(const QString& name);
    void removeDecorationByName(const QString& name);
    void renderDecorations(bool with_camera = true  );

    /// Events:
     void resize(int w, int h);

     void mouseMove(int posX, int posY);

     void mousePress(int posX,int posY, unsigned int  button, unsigned int modifier = NoModifier);

     void mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier = NoModifier);

     void keyUp(const unsigned int& key, unsigned int modifier = NoModifier);

     void keyDown(const unsigned int& key, unsigned int modifier = NoModifier);

     void wheel(const float& delta);

     void focusOnNode(Node* _node);

     virtual void nodeUpdated(int dirtyType = 0);
signals:
     void onUpdate();
private:
    QMutex mutex_;

    std::shared_ptr<Shader> shader_;

    std::shared_ptr<Camera> camera_;

    std::unique_ptr<Lights> lights_;

    std::shared_ptr<GLStates> gl_states_;

    std::shared_ptr<Manipulator> manipulator_;

    std::shared_ptr<NodeFactory> node_factory_;

    QList< std::shared_ptr<EventHandler> > event_handlers_;

    QList< std::shared_ptr<Decoration::DecorationBase> > decorations_;

};
}}
#endif // SCENEGRAPH_H
