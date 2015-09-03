

TEMPLATE = lib

CONFIG(release, debug|release){
    TARGET = OpenMeshAlgorithms
}
CONFIG(debug, debug|release){
    TARGET = OpenMeshAlgorithmsd
}

#sharedlib
{
    DEFINES *= GCL_EXPORT_SHARED_LIB
    DEFINES *= GCL_OPENMESHALGORITHMS_SHARED_LIB
}
#staticlib
{
    #CONFIG += staticlib
}

INCLUDEPATH *= ./

INCLUDEPATH *= ../
INCLUDEPATH *= ../../
INCLUDEPATH += ../../../3rdparty/include \








win32{
    DEFINES += OM_STATIC_BUILD
    win32-g++ {
    DESTDIR = $$PWD/../../../lib/mingw/

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-unused-but-set-parameter
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -std=c++11
    DEFINES += _COMPILE_GCC
    DEFINES += USE_FREEGLUT
    LIBS += -L$$PWD/../../../lib/mingw/ -lGCLCore
    LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/mingw/ -lOpenMeshCore
    }
    !win32-g++ {
        DEFINES += _USE_MATH_DEFINES
        win32-msvc2010 {
            DESTDIR = $$PWD/../../../lib/vs2010/
            CONFIG(release, debug|release){
                 LIBS += -L$$PWD/../../../lib/vs2010/ -lGCLCore
                 LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCore
            }
            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2010/ -lGCLCored
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCored
            }

        }
        win32-msvc2012 {
            DESTDIR = $$PWD/../../../lib/vs2012/
            CONFIG(release, debug|release){
                 LIBS += -L$$PWD/../../../lib/vs2012/ -lGCLCore
                 LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCore
            }
            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2012/ -lGCLCored
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCored
            }
        }
        win32-msvc2013 {
            CONFIG(release, debug|release){
                 DESTDIR = $$PWD/../../../build/Release/bin/vs2013/
                 LIBS += -L$$PWD/../../../build/Release/bin/vs2013/ -lGeoLabCore
                 LIBS += -L$$PWD/../../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCore
            }
            CONFIG(debug, debug|release){
                DESTDIR = $$PWD/../../../build/Debug/bin/vs2013/
                LIBS += -L$$PWD/../../../build/Debug/bin/vs2013/ -lGeoLabCored
                LIBS += -L$$PWD/../../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCored
            }
        }
        DEPENDPATH += $$PWD/../../../3rdparty/bin/GL/msvc/
    }
}

HEADERS += \
    Geometry/NearestTriangleSearch/KDTreeNode.h \
    Geometry/NearestTriangleSearch/NearestTriangleSearch.h \
    Geometry/Remeshing/IsolevelExtractor.h \
    Geometry/Remeshing/IsotropicRemeshing.h \
    Geometry/BasicMeasuring/BasicMeasuring.h \
    Geometry/NearestTriangleSearch/Ray.h \
    LeastSquareLaplacian/LeastSquareLaplacian.h \
    LeastSquareLaplacian/LS_LaplacianEigenSolver.h \
    gcl_export.h \
    FairingAndSmoothing/FiberMeshFairing.h \
    FairingAndSmoothing/FairingBase.h \
    Triangulation/Triangulation.h \
    Triangulation/triangle/triangle.h \
    Geometry/NearestTriangleSearch/NearestTrianglesQuery.h \
    Pasting/ImagePasting.h

SOURCES += \
    Geometry/NearestTriangleSearch/NearestTriangleSearch.cpp \
    Geometry/Remeshing/IsolevelExtractor.cpp \
    Geometry/Remeshing/IsotropicRemeshing.cpp \
    Geometry/BasicMeasuring/BasicMeasuring.cpp \
    Geometry/NearestTriangleSearch/Ray.cpp \
    LeastSquareLaplacian/LeastSquareLaplacian.cpp \
    FairingAndSmoothing/FiberMeshFairing.cpp \
    Triangulation/Triangulation.cpp \
    Triangulation/triangle/triangle.c \
    Geometry/NearestTriangleSearch/NearestTrianglesQuery.cpp \
    Pasting/ImagePasting.cpp
