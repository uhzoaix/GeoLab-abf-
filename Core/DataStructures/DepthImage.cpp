#include "DepthImage.h"

namespace GCL {
DepthImage::DepthImage():width_(1),height_(1),axis_x_(1,0,0),axis_y_(0,1,0),center_(0,0,0)
{
}

DepthImage::DepthImage(size_t w, size_t h):width_(w),height_(h),axis_x_(1,0,0),axis_y_(0,1,0),center_(0,0,0)
{

}

void DepthImage::resize(size_t w, size_t h)
{
    image_.clear();
    image_.resize(w * h);
    width_ = w;
    height_ = h;
}

void DepthImage::normalize()
{
    Scalar len = (width_ > height_) ? width_ : height_;
    axis_x_ = axis_x_.normalize() * (1.0 / len);
    axis_y_ = axis_y_.normalize() * (1.0 / len);
}

const Vec4 &DepthImage::color(unsigned int x, unsigned int y) const
{
    return this->at(x,y).color();
}

Vec4 &DepthImage::color(unsigned int x, unsigned int y)
{
    return this->at(x,y).color();
}

 Scalar DepthImage::depth(unsigned int x, unsigned int y) const
{
    return this->at(x,y).depth();

}

Scalar &DepthImage::depth(unsigned int x, unsigned int y)
{
    return this->at(x,y).depth();
}

const DepthImage::ColorDepth &DepthImage::at(unsigned int x, unsigned int y) const
{
    return image_[x + y * width_];
}

DepthImage::ColorDepth &DepthImage::at(unsigned int x, unsigned int y)
{
    return image_[x + y * width_];

}

const DepthImage::ColorDepth &DepthImage::at(unsigned int num) const
{
    return image_[num];
}

DepthImage::ColorDepth &DepthImage::at(unsigned int num)
{
    return image_[num];
}

void DepthImage::setCenter(const Vec3 &center)
{
    center_ = center;
}

void DepthImage::setAxis(const Vec3 &axis_x, const Vec3 &axis_y)
{
    axis_x_ = axis_x;
    axis_y_ = axis_y;
}

Vec3 DepthImage::getVertexPosition(unsigned int i) const
{
    Vec3 tmp = center_ + axis_x_ * (i % width_ - (Scalar)(width_ / 2)) + axis_y_ * (i / width_ - (Scalar)(height_ / 2)) + (axis_x_ ^ axis_y_).normalize() * depth(i % width_, i  / width_);
    return tmp;
}

Vec3 DepthImage::getVertexNormal(unsigned int i) const
{
    return (axis_x_ ^ axis_y_).normalize();
}


Vec4 DepthImage::getVertexColor(unsigned int i) const
{
    return color(i % width_, i /  width_);
}

void DepthImage::setVertexColor(unsigned int i, const Vec4 &color)
{
    this->at(i % width_, i /  width_).color() = color;
}

bool DepthImage::isVertexSelected(unsigned int i) const
{
    return image_[i].isSelected();
}

void DepthImage::setVertexSelected(unsigned int i, bool isSelected)
{
    image_[i].setSelected(isSelected);
}

unsigned int DepthImage::getFacesNumber() const
{
    return 0;
//    return (width_ - 1) * (height_ - 1) * 2;
}

GeometryBase::TriIndices DepthImage::getFaceTriIndices(unsigned int i) const
{
    return GeometryBase::TriIndices();
    if(i > getFacesNumber()) return GeometryBase::TriIndices();
    int y = i / 2 / (width_ - 1);
    int x = i / 2 % (width_ - 1);
    int pos0 = y * width_ + x;
    int pos1 = y * width_ + x + 1;
    int pos2 = (y + 1) * width_ + x + 1;
    int pos3 = (y + 1) * width_ + x;
    if(i % 2 == 0)
    {
        return GeometryBase::TriIndices(pos0,pos1,pos2);
    }
    else
    {
        return GeometryBase::TriIndices(pos0,pos2,pos3);
    }
}




}
