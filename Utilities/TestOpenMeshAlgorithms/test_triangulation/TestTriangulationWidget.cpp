#include "TestTriangulationWidget.h"
#include <OpenMeshAlgorithms/Triangulation/Triangulation.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPen>
namespace GCL { namespace Test {
TestTriangulationWidget::TestTriangulationWidget(QWidget *parent) : QWidget(parent),mode_(0),selected_id0_(-1),selected_id1_(-1)
{
    this->setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background,Qt::white);
    this->setPalette(pal);
}

TestTriangulationWidget::~TestTriangulationWidget()
{

}

void TestTriangulationWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_0)
    {
        mode_ = 0; selected_id0_ = selected_id1_ = -1;
    }
    if(e->key() == Qt::Key_1)
    {
        mode_ = 1; selected_id0_ = selected_id1_ = -1;
    }
    if(e->key() == Qt::Key_2)
    {
        mode_ = 2; selected_id0_ = selected_id1_ = -1;
    }
    if(e->key() == Qt::Key_3)
    {
        mode_ = 3; selected_id0_ = selected_id1_ = -1;
    }
    if(e->key() == Qt::Key_9)
    {
        mode_ = 0;
        clear();
    }
    if(e->key() == Qt::Key_R)
    {
        apply();
    }
    update();
}

void TestTriangulationWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(mode_ == 1)
    {
        points_.push_back(Vec3((double)e->pos().x(),(double)e->pos().y(),0.0));
    }
    else if(mode_ == 2 || mode_ == 3)
    {
        if(selected_id0_ == -1)
        {
            selected_id0_ = find_closest_point_id(Vec3((double)e->pos().x(),(double)e->pos().y(),0.0));
        }
        else
        {
            selected_id1_  = find_closest_point_id(Vec3((double)e->pos().x(),(double)e->pos().y(),0.0));
        }
        if(selected_id0_ != -1 && selected_id1_ != -1)
        {
            connect_edges();
        }
    }
    update();
}

void TestTriangulationWidget::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    QPen pen;
    QBrush brush;
    brush.setColor(Qt::blue);

    for(int i=0; i < points_.size(); i++)
    {
        if(i == selected_id0_)
        {
            pen.setColor(Qt::red);
        }
        else if(i == selected_id1_)
        {
            pen.setColor(Qt::green);
        }
        else
        {
            pen.setColor(Qt::blue);
        }
        pen.setWidth(3);
        paint.setPen(pen);
        QPoint tpos((int)points_[i][0],(int)points_[i][1]);
        paint.drawEllipse(tpos,5,5);
    }

    for(int i=0; i < edges_.size(); i++)
    {
        if(boundary_markers_[i])
        {
            pen.setColor(Qt::red);
        }
        else
        {
            pen.setColor(Qt::green);
        }
        pen.setWidth(3);
        Vec3 p0 = points_[edges_[i][0]];
        Vec3 p1 = points_[edges_[i][1]];
        paint.setPen(pen);
        paint.drawLine((int)p0[0],(int)p0[1],(int)p1[0],(int)p1[1]);
    }


    for(int i=0; i < new_points_.size(); i++)
    {
        pen.setColor(Qt::black);
        pen.setWidth(2);
        paint.setPen(pen);
        QPoint tpos((int)new_points_[i][0],(int)new_points_[i][1]);
        paint.drawEllipse(tpos,2,2);
    }

    for(int i=0; i < triangles_.size(); i++)
    {
        pen.setColor(Qt::blue);
        pen.setWidth(1);
        paint.setPen(pen);
        for(int j=0; j < 3; j++)
        {
            Vec3 p0 = new_points_[triangles_[i][j]];
            Vec3 p1 = new_points_[triangles_[i][(j+1)%3]];
            paint.setPen(pen);
            paint.drawLine((int)p0[0],(int)p0[1],(int)p1[0],(int)p1[1]);
        }
    }
}

int TestTriangulationWidget::find_closest_point_id(const Vec3 &pos)
{
    int ans = -1;
    double dist = -1;
    for(int i=0; i < points_.size(); i++)
    {
        if(ans == -1)
        {
            ans = i;
            dist = (points_[i] - pos).length();
        }
        else
        {
            double tmp = (points_[i] - pos).length();
            if(tmp < dist)
            {
                dist = tmp;
                ans = i;
            }
        }
    }
    return ans;
}

bool TestTriangulationWidget::connect_edges()
{
    edges_.push_back(Vec2i(selected_id0_,selected_id1_));
    boundary_markers_.push_back((mode_ == 2));
    selected_id0_ = selected_id1_;
    selected_id1_ = -1;
    return true;
}

void TestTriangulationWidget::clear()
{
    points_.clear();
    edges_.clear();
    boundary_markers_.clear();
    new_points_.clear();
    triangles_.clear();
}

void TestTriangulationWidget::apply()
{
    Utilities::Triangulation triangulation;
    triangulation.apply(points_,edges_,boundary_markers_,new_points_,triangles_);
}

}}
