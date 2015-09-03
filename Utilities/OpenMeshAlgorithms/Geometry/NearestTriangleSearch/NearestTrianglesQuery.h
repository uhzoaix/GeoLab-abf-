#ifndef NEARESTTRIANGLESQUERY_H
#define NEARESTTRIANGLESQUERY_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <Core/Math/MathDefines.h>
#include "KDTreeNode.h"
#include <vector>
namespace GCL {
class OMesh;
namespace Utilities {
/**
 * @brief The NearestTrianglesFinder class
 *      The class NearestTriangleSearch is not easy to understood for me, hence I decide to write an convinient one.
 *      Queries of an mesh (triangles) in common :
 *          1: to check or to get intersection of a ray;
 *          2: to find cloeset vertex from a point;
 *          3: to find nearest k triangles from a point
 *          4: to get intersections of a triangle;
 *
 * @author  Shiwei Wang
 *
 */
class NeareastTrianglesQuery;
class GCL_OPENMESHALGORITHMS_SHARED_EXPORT NearestTrianglesFinder
{
public:

    explicit NearestTrianglesFinder();
    ~NearestTrianglesFinder();


    void build_tree(OMesh* mesh);

    void query_on_tree(NeareastTrianglesQuery* query);

    void destroy_tree();

    void set_max_depth(int _max_depth)  {_max_depth = max_depth_;}
    int get_max_depth() const {return max_depth_;}



private:
    void query_on_tree_node(KDTreeNode * node, NeareastTrianglesQuery* query);


    void destroyNode(KDTreeNode * node);


    KDTreeNode* build_tree_node(int _l, int _r, int depth);


    void get_positions_on_face(OMesh::FaceHandle _fh, Vec3& v0, Vec3& v1, Vec3& v2);

    std::vector<OMesh::FaceHandle> face_handles_;
    std::vector<OMesh::FaceHandle> tmp_face_handles_;


    KDTreeNode* root_;
    OMesh* mesh_;
    int max_depth_;

};


class  NeareastTrianglesQuery
{
public:
    explicit NeareastTrianglesQuery() {}
    virtual ~NeareastTrianglesQuery() {}


    virtual void query_on_face(OMesh* mesh, OMesh::FaceHandle fh) = 0;
    virtual bool check_whether_to_search_kdtree_node(KDTreeNode* node) = 0;
};



class GCL_OPENMESHALGORITHMS_SHARED_EXPORT ClosestPointQuery: public NeareastTrianglesQuery
{
public:
    explicit ClosestPointQuery(const Vec3& _point);
    ~ClosestPointQuery() {}
    void init(const Vec3& _point);
    void query_on_face(OMesh* mesh, OMesh::FaceHandle fh);
    bool check_whether_to_search_kdtree_node(KDTreeNode* node);

    Vec3 get_answer_point() const {return ans_;}


    int get_query_number() const {return count_;}
private:
    Vec3 point_;
    Vec3 ans_;
    Scalar dist_;
    int count_;

};

class GCL_OPENMESHALGORITHMS_SHARED_EXPORT TriangleIntersectionQuery : public NeareastTrianglesQuery
{
public:
    explicit TriangleIntersectionQuery(const Vec3& v0, const Vec3& v1, const Vec3& v2);
    ~TriangleIntersectionQuery();
    void init(const Vec3& v0, const Vec3& v1, const Vec3& v2);
    void query_on_face(OMesh* mesh, OMesh::FaceHandle fh);
    bool check_whether_to_search_kdtree_node(KDTreeNode* node);
    void print();
    int get_query_number() const {return count_;}
    int flag() const {return (facehandles_.size() > 0);}
    std::vector<OMesh::FaceHandle>& get_facehandles() {return facehandles_;}
private:
    std::vector<OMesh::FaceHandle> facehandles_;
    Vec3 v0_;
    Vec3 v1_;
    Vec3 v2_;
    int count_;

};
}}
#endif // NEARESTTRIANGLESQUERY_H
