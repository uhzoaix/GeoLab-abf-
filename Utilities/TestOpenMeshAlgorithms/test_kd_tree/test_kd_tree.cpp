#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/IOInstances.hh>
#include <GCL/Core/DataStructures/OMesh.h>
#include <Utilities/OpenMeshAlgorithms/Geometry/NearestTriangleSearch/NearestTrianglesQuery.h>
#include <ctime>
#include <memory>
using namespace GCL;
int main()
{
    OMesh mesh;
    std::cout<<OpenMesh::IO::read_mesh(mesh,"mtfy_init.stl")<<std::endl;
    mesh.MeshNormalize();
    Utilities::NearestTrianglesFinder nearest_triangles_query;
    clock_t t0 = clock();
    nearest_triangles_query.build_tree(&mesh);
    std::cout<<clock() - t0 <<std::endl;
    std::shared_ptr< Utilities::TriangleIntersectionQuery > query0(new Utilities::TriangleIntersectionQuery(
                                                                       Vec3(3,4,0), Vec3(1,1,0),Vec3(0,0,0)));

    nearest_triangles_query.query_on_tree(query0.get());
    query0->print();
    std::cout<<query0->get_query_number()<<std::endl;
    std::shared_ptr< Utilities::TriangleIntersectionQuery > query1(new Utilities::TriangleIntersectionQuery(Vec3(3,4,0), Vec3(1,1,0),Vec3(0,0,0)));

    for(size_t i=0; i < mesh.getFacesNumber(); i++)
    {
        query1->query_on_face(&mesh,mesh.face_handle_(i));
    }
    query1->print();
    std::cout<<query1->get_query_number()<<std::endl;

    return 0;
}
