#ifndef GCL_SELECTION_PREFILTER_H
#define GCL_SELECTION_PREFILTER_H
#include "gcl_global.h"
#include "../../Math/MathDefines.h"
#include <memory>
#include <vector>
namespace GCL{ namespace SceneGraph {
/**
    Reference: Dolphin Prefilter
**/
class SceneGraphNode;
class GCL_CORE_SHARED_EXPORT PreFilter
{
public:
    enum PreFilterType
    {
        Normal,
        ZBuffer
    };

    PreFilter(SceneGraphNode* scenegraph);

    virtual ~PreFilter() {}
    /**
    Prepares the filter to filter stuff.
    */
    virtual void prepareFilter() = 0;

    /**
    Checks if the given vertex is valid. As input it expects the coordinates
    of the point in both world and screen space. TODO: FIX, passing both is
    a cludge to speed up the computation.
    \param _a The point
    \param _s The point in screen space
    \param _na  The normal
    \return True if the vertex is valid, false if it's not
    */
    virtual bool isValid( Vec3 _a,  Vec3 _s,  Vec3 _na) const = 0;

    /**
    Returns the name of this filter.
    \return the name of this filter.
    */
    virtual const char* getName() const = 0;

    /**
    Returns the type of this filter.
    \return the type of this filter.
    */
    virtual PreFilterType getType() const = 0;
protected:
    SceneGraphNode* scenegraph_;
};

/**
Filters vertexes based on their normal.
*/
class NormalFilter : public PreFilter
{
public:
  /** Initializes the NormalFilter instance */
  NormalFilter(SceneGraphNode* _scenegraph);
  virtual void prepareFilter();
  virtual bool isValid(Vec3 _a, Vec3 _s, Vec3 _na) const;
  virtual const char* getName() const;
  virtual PreFilterType getType() const;
};

/**
Filters vertexes based on their depth relative to the ZBuffer.
*/
class ZBufferFilter : public PreFilter
{
public:
  /** Initializes the ZBufferFilter instance */
  ZBufferFilter(SceneGraphNode* _scenegraph);
  /**
  Renders the scenegraph into a depth buffer and stores it. The buffer is later
  used to answer isValid calls.
  \see ZBufferRenderer
  */
  virtual void prepareFilter();
  virtual bool isValid(Vec3 _p, Vec3 _s, Vec3 _n) const;
  virtual const char* getName() const;
  virtual PreFilterType getType() const;

  /**
  Sets the threshold of the Depth Buffer evaluation.
  \param _t A threshold, default: 0.002
  */
  void setThreshold(float _t) {threshold_ = _t;}

  /**
  Returns the threshold for the Depth Buffer evaluation.
  \return the threshold.
  */
  float getThreshold()  const {return threshold_;}
private:
  std::vector<float> zBuff_;
  int vp_[4];
  float threshold_;
};
}}
#endif // PREFILTER_H
