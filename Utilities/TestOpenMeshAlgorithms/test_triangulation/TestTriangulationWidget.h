#ifndef TESTTRIANGULATIONWIDGET_H
#define TESTTRIANGULATIONWIDGET_H
#include <GCL/Core/Math/MathDefines.h>
#include <QWidget>
namespace GCL { namespace Test {
class TestTriangulationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestTriangulationWidget(QWidget *parent = 0);
    ~TestTriangulationWidget();

signals:

public slots:


protected:
    void keyPressEvent(QKeyEvent *e);
//    void mousePressEvent(QMouseEvent *e);
//    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

    void paintEvent(QPaintEvent *);
private:
    int find_closest_point_id(const Vec3& pos);

    bool connect_edges();

    void clear();

    void apply();
private:
    QList< Vec3 > points_;
    QList< Vec2i > edges_;
    QList< int > boundary_markers_;


    QList< Vec3 > new_points_;
    QList< Vec3i > triangles_;
    int mode_;     ///* 0: none;  1: add points ;  2: connect edges as boundary; 3: connect edges as inner edge
    int selected_id0_;
    int selected_id1_;

};
}}
#endif // TESTTRIANGULATIONWIDGET_H
