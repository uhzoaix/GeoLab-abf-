#include "Prefilter.h"
#include "../SceneGraph.h"
#include "../Camera.h"
#include "ZBufferRenderer.h"
namespace GCL{ namespace SceneGraph {

PreFilter::PreFilter(SceneGraphNode *scenegraph)
    :scenegraph_(scenegraph)
{

}

NormalFilter::NormalFilter(SceneGraphNode *_scenegraph)
    :PreFilter(_scenegraph)
{

}

void NormalFilter::prepareFilter()
{

}

bool NormalFilter::isValid(Vec3 _a, Vec3 _s, Vec3 _na) const
{
    Q_UNUSED(_a)

    if (_s[2] < 0 || _s[2] > 1) return false;

    if (scenegraph_->getCamera()->get_direction() * _na < 0)
    {
      return false;
    }
    return true;
}


const char *NormalFilter::getName() const
{
    return "NormalFilter";
}

PreFilter::PreFilterType NormalFilter::getType() const
{
    return PreFilter::Normal;
}

ZBufferFilter::ZBufferFilter(SceneGraphNode *_scenegraph)
    :PreFilter(_scenegraph)
{
    threshold_ = 0.006f;
}

void ZBufferFilter::prepareFilter()
{
    std::shared_ptr<ZBufferRenderer> zb(new ZBufferRenderer());
    zb->render(scenegraph_);
    zb->getViweport(vp_);
    std::shared_ptr<float> zbtmp = zb->getZBuffer();
    zBuff_.clear();
    for(int i=0; i < vp_[2] * vp_[3]; i++)
    {
        zBuff_.push_back(zbtmp.get()[i]);
    }
}

bool ZBufferFilter::isValid(Vec3 _p, Vec3 _s, Vec3 _n) const
{
    Q_UNUSED(_p)
    Q_UNUSED(_n)

    if (_s[0] < 0 || _s[0] > fabs((float)vp_[2])) return false;
    if (_s[1] < 0 || _s[1] > fabs((float)vp_[3])) return false;
    if (_s[2] < 0 || _s[2] > 1) return false;

    int idx = vp_[2]*static_cast<int>(floor(_s[1]+0.5))+
      static_cast<int>(floor(_s[0]+0.5));

    if (idx >= vp_[2]*vp_[3]) return false; // Out of bounds

    float depth = zBuff_[idx];

    // Arbitrary threshold constant, not optimal
//    DebugLog<<depth<<" "<<_s[2]<<std::endl;

    if (depth >= _s[2] - threshold_)
    {
        return true;
    }

    return false;
}



const char *ZBufferFilter::getName() const
{
    return "ZBufferFilter";
}

PreFilter::PreFilterType ZBufferFilter::getType() const
{
    return PreFilter::ZBuffer;
}



}}
