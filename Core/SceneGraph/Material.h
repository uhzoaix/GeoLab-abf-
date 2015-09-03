#ifndef GCL_GRAPHSCENE_MATERIAL_H
#define GCL_GRAPHSCENE_MATERIAL_H
#include "gcl_global.h"
namespace GCL{ namespace SceneGraph {
/*
* class to store and manage materials for every geometry node.
* the colors define the ambient and diffuse material.
* the specularity and shininess control the rest and are globaly applied to all colors
* all values (except shininess: [0,128]) must have the range [0,1]. however this is not checked!
* TODO: add asserts.
*
* if a geometry does not need one of the three colors it can simply ignore it.
*/
class Node;
class GCL_CORE_SHARED_EXPORT Material {

 public:
   explicit Material();
   ~Material();

   void setDefaultValues(bool _notifyObservers = true);

   void setFaceColor(float _r, float _g, float _b, float _a, bool _notifyObservers = true);
   void setEdgeColor(float _r, float _g, float _b, float _a, bool _notifyObservers = true);
   void setPointColor(float _r, float _g, float _b, float _a, bool _notifyObservers = true);
   void setSelectionColor(float _r, float _g, float _b, float _a, bool _notifyObservers = true);
   void setNormalColor(float _r, float _g, float _b, float _a, bool _notifyObservers = true);
   void setEdgeWidth( float r, bool _notifyObservers  = true );
   void setVertexRadius( float r, bool _notifyObservers  = true );

   inline float* getFaceColor4fv() {return &faceColor_[0];}
   inline float* getEdgeColor4fv() {return &edgeColor_[0];}
   inline float* getPointColor4fv() {return &pointColor_[0];}
   inline float* getSelectionColor4fv() {return &selectionColor_[0];}
   inline float* getNormalColor4fv() {return &normalColor_[0];}


   //[0,1]
   void setSpecularity(float _ratio, bool _notifyObservers = true);
   //[0,1]
   void setShininess(float _shininess, bool _notifyObservers = true);

   void setNormalLengthFactor(double _factor, bool _notifyObservers = true);

   void setOffsetFactor(double _factor, bool _notifyObservers = true);
   void setOffsetUnits(double _units, bool _notifyObservers = true);

   inline float getSpecularity() { return specularity_[0]; }
   inline float* getSpecularity4fv() { return specularity_; }
   inline float getShininess() { return shininess_; }
   inline float getEdgeWidth(){ return edgeWidth_; }
   inline float getVertexRadius(){ return vertexRadius_; }
   inline float getNormalLengthFactor() { return normalLengthFactor_; }

   inline float getOffsetFactor() { return offsetFactor_;}
   inline float getOffsetUnits() { return offsetUnits_;}


   ////Observer
   inline void setNotifyingObservers(Node* _node) { node_ = _node; }


 private:
   float faceColor_[4];
   float edgeColor_[4];
   float pointColor_[4];
   float selectionColor_[4];
   float specularity_[4];
   float normalColor_[4];
   float shininess_;

   float edgeWidth_;
   float vertexRadius_;
   double normalLengthFactor_;

   Node* node_;

   float offsetFactor_;
   float offsetUnits_;


 };

}}
#endif // MATERIAL_H
