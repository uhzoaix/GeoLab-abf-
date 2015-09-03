#include "Material.h"
#include "Node.h"
#include "../Utilities/LogSystem.h"
namespace GCL{ namespace SceneGraph {

Material::Material() :
           node_(0) {
   setDefaultValues(false);
 }

 Material::~Material() {
 }


 void Material::setDefaultValues(bool _notifyObservers) {
   shininess_ = 100.f;

   specularity_[0] = .2f;
   specularity_[1] = .2f;
   specularity_[2] = .2f;
   specularity_[3] = 1.f;

   faceColor_[0] = 1.f;
   faceColor_[1] = 1.f;
   faceColor_[2] = 1.f;
   faceColor_[3] = 1.f;

   edgeColor_[0] = 0.4f;
   edgeColor_[1] = 0.4f;
   edgeColor_[2] = 0.4f;
   edgeColor_[3] = 1.f;

   pointColor_[0] = .2f;
   pointColor_[1] = .2f;
   pointColor_[2] = .2f;
   pointColor_[3] = 1.f;

   selectionColor_[0] = 0.9f;
   selectionColor_[1] = .2f;
   selectionColor_[2] = .2f;
   selectionColor_[3] = 1.f;

   normalColor_[0] = 1.0;
   normalColor_[1] = 0.0;
   normalColor_[2] = 0.0;
   normalColor_[3] = 1.0;

   edgeWidth_ = 1.f;
   vertexRadius_ = 5.f;

   normalLengthFactor_ = 0.05;

   offsetFactor_ = 1.0;
   offsetUnits_  = 1.0;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }


 void Material::setFaceColor(float _r, float _g, float _b, float _a, bool _notifyObservers){
   faceColor_[0] = _r;
   faceColor_[1] = _g;
   faceColor_[2] = _b;
   faceColor_[3] = _a;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }

 void Material::setEdgeColor(float _r, float _g, float _b, float _a, bool _notifyObservers){
   edgeColor_[0] = _r;
   edgeColor_[1] = _g;
   edgeColor_[2] = _b;
   edgeColor_[3] = _a;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }

 void Material::setPointColor(float _r, float _g, float _b, float _a, bool _notifyObservers){
   pointColor_[0] = _r;
   pointColor_[1] = _g;
   pointColor_[2] = _b;
   pointColor_[3] = _a;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }

 void Material::setSelectionColor(float _r, float _g, float _b, float _a, bool _notifyObservers){
   selectionColor_[0] = _r;
   selectionColor_[1] = _g;
   selectionColor_[2] = _b;
   selectionColor_[3] = _a;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }

 void Material::setNormalColor(float _r, float _g, float _b, float _a, bool _notifyObservers){
   normalColor_[0] = _r;
   normalColor_[1] = _g;
   normalColor_[2] = _b;
   normalColor_[3] = _a;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }


 void Material::setSpecularity(float _ratio, bool _notifyObservers) {
   specularity_[0] = _ratio;
   specularity_[1] = _ratio;
   specularity_[2] = _ratio;
   specularity_[3] = 1.f;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }

 void Material::setShininess(float _shininess, bool _notifyObservers) {
   shininess_ = _shininess;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }

 void Material::setNormalLengthFactor(double _factor, bool _notifyObservers)
 {
     normalLengthFactor_ = _factor;

     if(node_!=0 && _notifyObservers) {
       node_->nodeUpdated();
     }
 }

 void Material::setOffsetFactor(double _factor, bool _notifyObservers)
 {

     offsetFactor_ = _factor;

     if(node_!=0 && _notifyObservers) {
       node_->nodeUpdated();
     }
 }

 void Material::setOffsetUnits(double _units, bool _notifyObservers)
 {
     offsetUnits_ = _units;

     if(node_!=0 && _notifyObservers) {
       node_->nodeUpdated();
     }
 }

 void Material::setEdgeWidth( float r, bool _notifyObservers ) {
   edgeWidth_=r;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }

 void Material::setVertexRadius( float r, bool _notifyObservers ){
   vertexRadius_=r;

   if(node_!=0 && _notifyObservers) {
     node_->nodeUpdated();
   }
 }


}}
