#ifndef GCL_NODE_H
#define GCL_NODE_H
#include "Core/gcl_global.h"
#include <QObject>
#include <memory>
namespace GCL{
class AABB;
namespace SceneGraph {
/*
* Main and basic tree node class of the scenegraph.
* It is a scenegraph without camera or lights management.
* the lighting and camera setup has to be done before.
* Be different with OSG, it inherits class QObject for convenience to manage memory and children of tree.
* The signal-slot  methods may useful in some cases.
*
*/


class Visitor;
class RenderModes;
class Material;
class NodeAction;
class GCL_CORE_SHARED_EXPORT Node : public  QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible)
    Q_PROPERTY(bool selected READ selected WRITE setSelected)
public:

    explicit Node(QObject* parent = 0);
    virtual ~Node();
public:
    //  return the basic nodetype defined in NodeType.h
    virtual int getNodeType() const = 0;
    //  return the node type name : it can be diffrent with nodetype
    virtual QString getNodeTypeName() const = 0;
    ////visitor pattern
    virtual void accept(Visitor & _visitor) = 0;

    void setName(const QString& _name);

    virtual Node* getScenegraph();

    RenderModes* getRenderModes() {return render_modes_.get();}
    bool  hasParent() const {return (parent() != 0);}
    Node* getParent() {return qobject_cast<Node*>(parent());}
    Node* getChild(const int _index) {	return qobject_cast<Node*>(children()[_index]);}
    int getChildIndex(const Node * const _child) const;
    int getNumberOfChildren() const {return children().size();}

    inline bool selected() { return selected_; }
    void setSelected(bool _selected) {selected_ = _selected;}
    inline bool visible() { return visible_; }
    void setVisible(bool _visible) {visible_ = _visible;}

    virtual void nodeUpdated(int dirtyType = 0);

public: // AABB
    virtual void recomputeAABB();
    bool isDirtyAABB() const;
    const AABB* getAABB();
    void markBoundingVolumesDirty();

public: // Actions
    virtual void initNodeActions();
    bool getNodeActionValue(const QString& action_name);
    void addNodeAction(const std::shared_ptr<NodeAction>& action);
    int getNodeActionsNumber() const;
    NodeAction* getNodeAction(int i);
public slots:
    virtual void slotAction(const QString& action_name);
    virtual void slotCheckableAction(const QString& action_name, bool checked);




public:
    //isRenderSettingsOverriding specifies, wheter the current node's draw modes and gl states are active or not.
    //if not, the states of the parent are automatically used by the renderer.
    //note that for the scenegraph node, changing to false has no effect!
    void setRenderSettingsOverriding(bool _isOverriding);
    inline bool isRenderSettingsOverriding() { return render_setting_overriding_; }

public:

    unsigned int getDescendants(QList<Node*>& _descendants, bool _onlySelected=false)
    {
        return getDescendants<Node*>(_descendants, _onlySelected);
    }

    template<typename T>
    unsigned int getDescendants(QList<T>& _descendants, bool _onlySelected=false)
    {
        unsigned int n = 0;

        QList<T> nodeChildren = findChildren<T>();
        typename QList<T>::iterator nodeIt, nodeEnd =nodeChildren.end();
        for(nodeIt=nodeChildren.begin(); nodeIt!=nodeEnd; ++nodeIt)
        {
            if(!_onlySelected || (*nodeIt)->selected()) {
                _descendants.push_back(*nodeIt);
                ++n;
            }
            n += (*nodeIt)->getDescendants(_descendants,_onlySelected);
        }

        return n;
    }

    Node *getFirstDescendant(bool _onlySelected=false) const
    {
        return getFirstDescendant<Node*>(_onlySelected);
    }

    template <typename T>
    T getFirstDescendant(bool _onlySelected=false) const
    {
        if (!_onlySelected)
        {
            return qobject_cast<T>(findChild<T>());
        }
        else
        {
            QList<T> nodeChildren = findChildren<T>();
            typename QList<T>::iterator nodeIt, nodeEnd =nodeChildren.end();
            for(nodeIt=nodeChildren.begin(); nodeIt!=nodeEnd; ++nodeIt)
            {
                if((*nodeIt)->selected()) {
                    return *nodeIt;
                }
            }
            return NULL;
        }
    }
protected:
    std::shared_ptr<AABB> aabb_;
    QList<std::shared_ptr<NodeAction> > actions_;
    bool aabbIsDirty_;

private:
    std::unique_ptr<RenderModes> render_modes_;
    bool visible_;
    bool selected_;
    bool render_setting_overriding_;

};
}}
#endif // NODE_H
