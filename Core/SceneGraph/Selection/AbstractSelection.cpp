#include "AbstractSelection.h"
#include "../SceneGraph.h"
#include "../GroupNode.h"
#include "../GeometryNode.h"
#include "Prefilter.h"
#include "ZBufferRenderer.h"
namespace GCL { namespace SceneGraph {



AbstractSelection::AbstractSelection(SceneGraphNode *_scenegraph)
    :scenegraph_(_scenegraph),objectMode_(false),selectionType_(SELECTION_STANDARD)
{
}

void AbstractSelection::setObjectMode(bool _b)
{
    objectMode_ = _b;
}

bool AbstractSelection::applySelectionType(bool _b) const
{
    if(getSelectionType()==SELECTION_STANDARD){
        return _b;
    }else if(getSelectionType()==SELECTION_ADD){
        return true;
    }else if(getSelectionType()==SELECTION_REMOVE){
        return false;
    }else if(getSelectionType()==SELECTION_TOGGLE){
        return !_b;
    }
    return false;
}

int AbstractSelection::mousePress(int posX, int posY, unsigned int button, unsigned int modifier)
{
    EventHandler::mousePress(posX,posY,button,modifier);
    if(button != LeftButton || modifier != ControlModifier) return GCL_FALSE;
    return selection_start(posX,posY);
}

int AbstractSelection::mouseMove(int _x, int _y)
{
    EventHandler::mouseMove(_x,_y);
    if(getButton() != LeftButton || getModifier() != ControlModifier) return GCL_FALSE;
    return selection_moving(_x,_y);
}

int AbstractSelection::mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier)
{
    if(getButton() != LeftButton || getModifier() != ControlModifier) return GCL_FALSE;
    EventHandler::mouseRelease(posX,posY,button,modifier);
    return selection_stop(posX,posY);
}

int AbstractSelection::keyUp(const unsigned int &key, unsigned int modifier)
{
    if(modifier != ControlModifier) return GCL_FALSE;
    switch (key)
    {
    case Key_N:
      if (!removePreFilter(PreFilter::Normal))
        addPreFilter(PreFilter::Normal);
      return true;
    case Key_Z:
      if (!removePreFilter(PreFilter::ZBuffer))
        addPreFilter(PreFilter::ZBuffer);
      return true;
    case Key_T:
      setSelectionType(SELECTION_STANDARD);
      return true;
    case Key_A:
      setSelectionType(SELECTION_ADD);
      return true;
    case Key_S:
      setSelectionType(SELECTION_REMOVE);
      return true;
    case Key_Y:
      setSelectionType(SELECTION_TOGGLE);
      return true;
    case Key_W:
      setObjectMode(!objectMode());
      return GCL_TRUE;
    }
    return GCL_FALSE;
}



void AbstractSelection::drawOverlay() const
{

}

bool AbstractSelection::isInsideEdge(const Vec3 &_a, const Vec3 &_b, const Vec3 &_na, const Vec3 &_nb) const
{
    return isInsideVertex(_a, _na) &&
            isInsideVertex(_b, _nb);
}

bool AbstractSelection::isInsideFace(const Vec3 &_a, const Vec3 &_b, const Vec3 &_c, const Vec3 &_na, const Vec3 &_nb, const Vec3 &_nc) const
{
    return isInsideVertex(_a, _na) &&
      isInsideVertex(_b, _nb) &&
            isInsideVertex(_c, _nc);
}

bool AbstractSelection::selectVertex(bool _s, bool &_f, const Vec3 &_a, const Vec3 &_na) const
{
    _f=false;

    if(getSelectionType()==SELECTION_STANDARD)
    {
        _f=isInsideVertex(_a,_na);
        return _f;
    }
    else if(getSelectionType()==SELECTION_ADD)
    {
        if(!_s){
            _f=isInsideVertex(_a,_na);
            return _f;
        }else{
            return true;
        }
    }else if(getSelectionType()==SELECTION_REMOVE)
    {
        if(_s){
            _f=isInsideVertex(_a,_na);
            return !_f;
        }else{
            return false;
        }
    }else if(getSelectionType()==SELECTION_TOGGLE)
    {
        bool i=isInsideVertex(_a,_na);
        if(i){
            _f=true;
            return !_s;
        }else{
            _f=false;
            return _s;
        }
    }
    return false;
}

bool AbstractSelection::selectEdge(bool _s, bool &_f, const Vec3 &_a, const Vec3 &_b, const Vec3 &_na, const Vec3 &_nb) const
{
    _f=false;

    if(getSelectionType()==SELECTION_STANDARD)
    {
        _f=isInsideEdge(_a,_b,_na,_nb);
        return _f;
    }else if(getSelectionType()==SELECTION_ADD)
    {
        if(!_s){
            _f=isInsideEdge(_a,_b,_na,_nb);
            return _f;
        }else{
            return true;
        }
    }else if(getSelectionType()==SELECTION_REMOVE)
    {
        if(_s){
            _f=isInsideEdge(_a,_b,_na,_nb);
            return !_f;
        }else{
            return false;
        }
    }else if(getSelectionType()==SELECTION_TOGGLE)
    {
        bool i=isInsideEdge(_a,_b,_na,_nb);
        if(i){
            _f=true;
            return !_s;
        }else{
            _f=false;
            return _s;
        }
    }
    return false;
}

bool AbstractSelection::selectFace(bool _s, bool &_f, const Vec3 &_a, const Vec3 &_b, const Vec3 &_c, const Vec3 &_na, const Vec3 &_nb, const Vec3 &_nc) const
{
    _f=false;

    if(getSelectionType()==SELECTION_STANDARD)
    {
        _f=isInsideFace(_a,_b,_c,_na,_nb,_nc);

        return _f;
    }else if(getSelectionType()==SELECTION_ADD)
    {
        if(!_s){
            _f=isInsideFace(_a,_b,_c,_na,_nb,_nc);
            return _f;
        }else{
            return true;
        }
    }else if(getSelectionType()==SELECTION_REMOVE)
    {
        if(_s){
            _f=isInsideFace(_a,_b,_c,_na,_nb,_nc);
            return !_f;
        }else{
            return false;
        }
    }else if(getSelectionType()==SELECTION_TOGGLE)
    {
        bool i=isInsideFace(_a,_b,_c,_na,_nb,_nc);
        if(i){
            _f=true;
            return !_s;
        }else{
            _f=false;
            return _s;
        }
    }
    return false;
}

void AbstractSelection::addPreFilter(PreFilter::PreFilterType _ft)
{
    switch (_ft)
    {
    case PreFilter::Normal:
      preFilter_.push_back(std::shared_ptr<PreFilter>(new NormalFilter(getScenegraph())));
      break;
    case PreFilter::ZBuffer:
      preFilter_.push_back(std::shared_ptr<PreFilter>(new ZBufferFilter(getScenegraph())));
      break;
    default:
      break;
    }

}

bool AbstractSelection::removePreFilter(PreFilter::PreFilterType _ft)
{
    for(int i=0; i < preFilter_.size(); )
    {
      if (preFilter_[i]->getType() == _ft)
      {
        preFilter_.erase(preFilter_.begin() + i);
        return true;
      }
      else
      {
          i++;
      }
    }
    return false;
}

void AbstractSelection::preparePreFilter()
{
    foreach (const std::shared_ptr<PreFilter>& prefilter, preFilter_)
    {
        prefilter->prepareFilter();
    }

}

bool AbstractSelection::checkPreFilter(const Vec3 &_a, const Vec3 &_s, const Vec3 &_na) const
{
    bool isValid = true;
    foreach (const std::shared_ptr<PreFilter>& prefilter, preFilter_) {

      isValid = isValid && prefilter->isValid(_a, _s, _na);
    }
    return isValid;
}





void AbstractSelection::startSelection()
{
    preparePreFilter();

    setSelectionState(SELECTION_SELECTING);

    scenegraph_->accept(*this);

    setSelectionState(SELECTION_PAUSED);
}

bool AbstractSelection::enter(SceneGraphNode *_sceneGraphNode)
{
    if (_sceneGraphNode->visible())
    {
      return true;
    }
    return false;
}

bool AbstractSelection::enter(GroupNode *_groupNode)
{
    if (_groupNode->visible())
    {
      return true;
    }
    return false;
}

bool AbstractSelection::enter(GeometryNode *_geometryNode)
{
    if (_geometryNode->visible())
    {
        if(!objectMode())
        {

            if(!_geometryNode->selected())
            {
                return false;
            }
            else
            {

                _geometryNode->selectWithSelector(this);
            }
        }else
        {
            if(getSelectionType()==SELECTION_STANDARD)
                _geometryNode->setSelected(false);

            if(getSelectionType()==SELECTION_ADD && _geometryNode->selected() )
                return true;

            if(getSelectionType()==SELECTION_REMOVE && !_geometryNode->selected() )
                return true;

            _geometryNode->selectWithSelector(this);
            _geometryNode->nodeUpdated(Color_Dirty);/*todo: lets have a return value of the selectWithSelector, and refine the update.*/
            return true;
        }
    }
    return false;
}



}}
