#ifndef GCL_ABSTRACTSELECTION_H
#define GCL_ABSTRACTSELECTION_H
#include "gcl_global.h"
#include <QList>
#include <memory>
#include "../../Math/MathDefines.h"
#include "../EventHandler/EventHandler.h"
#include "../Visitor.h"
#include "Prefilter.h"


namespace GCL { namespace SceneGraph {
/** An enum for the possible states of a Selection */
enum SelectionState {
  SELECTION_PAUSED,   /**< This selector is inactive */
  SELECTION_UPDATING, /**< This selector is changing it's representation */
  SELECTION_SELECTING /**< This selector is being evaluated by the scene */
} ;

/** An enum for the possible types of a Selection */
enum SelectionType{
  SELECTION_STANDARD, /**< This selection deselects all others */
  SELECTION_ADD,  /**< This selection adds to all previously selected things */
  SELECTION_REMOVE, /**< This selection removes from all previously selected things */
  SELECTION_TOGGLE  /**< This selection inverts selection states */
} ;



/**
    The AbstractSelection class serves as parent for all selection algorithms.
    It supports mouse input handling, which are used to build the representation
    for the selection area, and inside tests for vertices, edges, and faces.
    Subclasses also support to draw itself.

    \ref Dolphin
    \author Gerhard Roethlin

*/
class SceneGraphNode;
class GCL_CORE_SHARED_EXPORT AbstractSelection : public EventHandler, public Visitor
{
public:
  /**
  The constructor of AbstractSelections.
  This class can not be instasntiated itself, but this constructor
  sets up data that is used by subclasses. The passed scenegraph is
  later used to perform selection on. The View is used to draw the controlls
  but also to get information about the camera used in screen space mode.
  \param _scenegraph  The scenegraph on which selections are performed.
  */
  explicit AbstractSelection(SceneGraphNode* _scenegraph);
  /**
  The deconstructor of AbstractSelections.
  */
  virtual ~AbstractSelection() {}

  /** Is this Selection also selecting Nodes in the Scenegraph
    Nodes are selected if one their their primitives would be selected.
    If true, primitives are not selected.
    */
  bool objectMode() const {return objectMode_;}

  /** Set this Selection to also select Nodes in the Scenegraph
    Nodes are selected if one their their primitives would be selected.
    If true, primitives are not selected.
    */
  void setObjectMode( bool _b );

  /** Converts a current state into a new state given the Selectors Type
    \param _b the current state
    \return the new state
    */
  bool applySelectionType( bool _b ) const;


public: // Input Handling

  virtual EventHandlerType getType() const {return Selection;}

  virtual const char* getEventHandlerName() const {return "AbstractionSelection";}
  /**
  It would call selection_start() to start the Selection;
  Only Ctrl + LeftMouseButton is available.
  \param _x X coordinate in screen space
  \param _y Y coordinate in screen space
  \return true if the event was handled, false otherwise
  */
  virtual int mousePress(int posX, int posY, unsigned int button, unsigned int modifier =  ControlModifier);

  /**
  It would call selection_moving() to continue the selection area creation.
  \param _x X coordinate in screen space
  \param _y Y coordinate in screen space
  \return true if the event was handled, false otherwise
  */
  virtual int mouseMove(int _x, int _y);

  /**
  It would call selection_end() to set up acceleration structures, and start the selection.
  Only Ctrl + LeftMouseButton is available.
  \param _x X coordinate in screen space
  \param _y Y coordinate in screen space
  \return true if the event was handled, false otherwise
  */
  virtual int mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier = ControlModifier);

  /**
  Does nothing.
  \param _delta The ammount by which the wheel was moved.
  \return true if the event was handled, false otherwise
  */
  virtual  int wheel(const float &delta) {return 0;}


  /**
  Changes the PreFilter based on key presses. Call in overrides.
  \param _key The key which is pressed
  \return true if the event was handled, false otherwise
  */
  virtual int keyUp(const unsigned int &key, unsigned int modifier = ControlModifier);

public: // Selection Procedure
      /**
      Selection Start, called by mousePress
      \param _x X coordinate in screen space
      \param _y Y coordinate in screen space
      \return true if the event was handled, false otherwise
      */
    virtual int selection_start(int _x, int _y) {return GCL_FALSE;}
      /**
      Selection Moving, called by mouseMove
      \param _x X coordinate in screen space
      \param _y Y coordinate in screen space
      \return true if the event was handled, false otherwise
      */
    virtual int selection_moving(int _x, int _y) {return GCL_FALSE;}
      /**
      Selection End, called by mouseRelease
      \param _x X coordinate in screen space
      \param _y Y coordinate in screen space
      \return true if the event was handled, false otherwise
      */
    virtual int selection_stop(int _x, int _y) {return GCL_FALSE;}

public: // State
  /**
  Returns the type of this selection.
  \return the type of this selection.
  \see  SelectionType
  */
  SelectionType getSelectionType() const {return selectionType_;}

  /**
  Sets the type of this selection.
  \param _type  The new type of this selection.
  */
  void setSelectionType(SelectionType _type) {selectionType_ = _type;}

  /**
  Returns the state this selector is in.
  The state can not be changed externaly.
  \return the state this selector is in.
  \see  SelectionState
  */
  SelectionState getSelectionState() const {return selectionState_;}

protected:
  /**
  Sets the state this selector is in.
  \param _state The new state of the selector.
  */
  void setSelectionState(SelectionState _state) {selectionState_ = _state;}

public: // Drawing
  /**
  Draws the selection overlay.
  */
  virtual void drawOverlay() const;


public: // Inside Testing
  /**
  Tests if the passed point is inside the selection area.
  The selection area is defined by the mouse input, or accessors.
  Subclasses of this class have to implement this function. If a normal is
  passed, it is used to eliminate back facing vertices.
  \param _a a point in world space.
  \param _na  the normal of the point _a.
  \return true if _point is inside the selection area, false otherwise
  */
  virtual bool isInsideVertex(const Vec3& _a, const Vec3& _na = Vec3()) const = 0;

  /**
  Tests if the edge between the two points is inside the selection area.
  The default implementation considers an edge to be inside if both
  vertices are inside. Subclasses can overwrite it for a more elaborate
  version. If a normal is passed, it is used to eliminate back facing vertices.
  \param _a the first point of the edge.
  \param _b the second point of the edge.
  \param _na  the normal of the point _a.
  \param _nb  the normal of the point _b.
  \return true if the edge is inside the selection area, false otherwise
  */
  virtual bool isInsideEdge(const Vec3& _a, const Vec3& _b,
      const Vec3& _na = Vec3(),const Vec3& _nb = Vec3()) const;

  /**
  Tests if the face defined by the three points is inside the selection area.
  The default implementation considers a face to be inside if all
  vertices are inside. Subclasses can overwrite it for a more elaborate
  version.  If a normal is passed, it is used to eliminate back facing vertices.
  \param _a the first point of the face.
  \param _b the second point of the face.
  \param _c the third point of the face.
  \param _na  the normal of the point _a.
  \param _nb  the normal of the point _b.
  \param _nc  the normal of the point _c.
  \return true if the face is inside the selection area, false otherwise
  */
  virtual bool isInsideFace(const Vec3& _a,const Vec3& _b, const Vec3& _c,
      const Vec3& _na = Vec3(), const Vec3& _nb = Vec3(), const Vec3& _nc = Vec3()) const;

  /**
  Decides if a vertex ( in Primitive Mode) / object otherwise, should be selected.
  Uses isInsideVertex, and the current selection state _b of the vertex / object.
  \param _s the current selection state of the vertex
  \param _f output, gets set to true if the selection in object mode of a Node is finished
  \param _a a point in world space.
  \param _na  the normal of the point _a.
  \param _param
  \return the new selection state of the vertex / object
  */
  bool selectVertex( bool _s, bool& _f, const Vec3& _a, const Vec3& _na = Vec3() ) const;

  /**
  Decides if an edge ( in Primitive Mode) / object otherwise,  should be selected.
  Uses isInsideEdge, and the current selection state _b of the edge / object.
  \param _s the current selection state of the edge
  \param _f output, get set to true if the selection in object mode of a Node is finished
  \param _a the first point of the edge.
  \param _b the second point of the edge.
  \param _na  the normal of the point _a.
  \param _nb  the normal of the point _b.
  \return the new selection state of the edge / object
  */
  bool selectEdge( bool _s, bool& _f, const Vec3& _a, const Vec3& _b,
                           const Vec3& _na = Vec3(), const Vec3& _nb = Vec3()) const;

  /**
  Decides if a face ( in Primitive Mode) / object otherwise, should be selected.
  Uses isInsideFace, and the current selection state _b of the face / object.
  \param _s the current selection state of the face
  \param _f output, get set to true if the selection in object mode of a Node is finished
  \param _a the first point of the face.
  \param _b the second point of the face.
  \param _c the third point of the face.
  \param _na  the normal of the point _a.
  \param _nb  the normal of the point _b.
  \param _nc  the normal of the point _c.
  \return the new selection state of the face / object
  */
  bool selectFace( bool _s, bool& _f, const Vec3& _a, const Vec3& _b, const Vec3& _c,
                           const Vec3& _na = Vec3(), const Vec3& _nb = Vec3(), const Vec3& _nc = Vec3()) const;



public: // PreFilter
  /**
  Adds a PreFilter for this selector. PreFilters are used to cheaply
  throw away vertexes that are not intended to be selected.
  This method has a switch statement for all supported preFilter types...
  \param _ft A preFilter type
  */
  void addPreFilter(PreFilter::PreFilterType _ft);

  /**
  Removes a PreFilter.
  \param _ft A preFilter type
  \return true if a filter was removed, false otherwise
  */
  bool removePreFilter(PreFilter::PreFilterType _ft);

  /**
  Prepares all prefilters.
  */
  void preparePreFilter();

  /**
  Returns true or false based on the validity check on all added PreFilters.
  \param _a The point
  \param _s The point in screen space
  \param _na  The normal
  \return True if the vertex is valid, false if it's not
  */
  bool checkPreFilter(const Vec3& _a,const Vec3& _s, const Vec3& _na) const;



public: // Selection Start
  /**
  Starts the selection with the current representation.
  */
  void startSelection();

public: // Visitor

  virtual bool enter(SceneGraphNode * _sceneGraphNode);
  virtual bool enter(GroupNode * _groupNode);
  virtual bool enter(GeometryNode * _geometryNode);

//  /**
//  Part of the visitor implementation of the selection node. Returns
//  true if the given Node is visible, and false otherwise. Therefore
//  only traverses Nodes which are visible. This method also changes the
//  state of the ScreenProjection to allow for object space transformations.
//  \param _transformationNode  A TransformationNode node.
//  \return true if the childs should be traversed, false otherwise
//  */
//  virtual bool enter(TransformationNode * _transformationNode);

//  /**
//  Part of the visitor implementation. Pops the stack of the object transformation.
//  \param _transformationNode  A TransformationNode node.
//  */
//  virtual void leave(TransformationNode * _transformationNode);

protected:


  /**
  Returns the scenegraph on which the selection is done.
  \return the scenegraph on which the selection is done.
  */
  SceneGraphNode* getScenegraph() {return scenegraph_;}
  const SceneGraphNode* getScenegraph() const {return scenegraph_;}
  QList<std::shared_ptr<PreFilter>> preFilter_;

private:

  SelectionType selectionType_;

  SelectionState selectionState_;

  SceneGraphNode* scenegraph_;

  bool objectMode_;

};
}}
#endif // ABSTRACTSELECTION_H
