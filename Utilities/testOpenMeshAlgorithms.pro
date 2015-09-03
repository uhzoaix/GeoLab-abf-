QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TEMPLATE = app
CONFIG += console
TARGET = testOpenMeshAlgorithms

INCLUDEPATH *= ./
INCLUDEPATH *= ../
INCLUDEPATH *= ../Libraries/include \

DEFINES *= GCL_EXPORT_SHARED_LIB







win32{
    DEFINES += OM_STATIC_BUILD
    win32-g++ {

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-unused-but-set-parameter
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -std=c++11
    DEFINES += _COMPILE_GCC
    DEFINES += USE_FREEGLUT
    LIBS += -L$$PWD/../../lib/mingw/ -lOpenMeshAlgorithms
    LIBS += -L$$PWD/../../lib/mingw/ -lGCLCore
    LIBS += -L$$PWD/../Libraries/lib/OpenMesh/mingw/ -lOpenMeshCore
    }
    !win32-g++ {
        DEFINES += _USE_MATH_DEFINES
        win32-msvc2010 {
            LIBS += -L$$PWD/../../lib/vs2010/ -lOpenMeshAlgorithms
            LIBS += -L$$PWD/../../lib/vs2010/ -lGCLGui
            LIBS += -L$$PWD/../../lib/vs2010/ -lGCLCore
            LIBS += -L$$PWD/../Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCore
        }
        win32-msvc2012 {
            LIBS += -L$$PWD/../../lib/vs2012/ -lOpenMeshAlgorithms
            LIBS += -L$$PWD/../../lib/vs2012/ -lGCLGui
            LIBS += -L$$PWD/../../lib/vs2012/ -lGCLCore
            LIBS += -L$$PWD/../Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCore
        }
        win32-msvc2013 {
            LIBS += -L$$PWD/../../lib/vs2013/ -lOpenMeshAlgorithms
            LIBS += -L$$PWD/../../lib/vs2013/ -lGCLGui
            LIBS += -L$$PWD/../../lib/vs2013/ -lGCLCore
            LIBS += -L$$PWD/../Libraries/lib/OpenMesh/vs2013/ -lOpenMeshCore
        }
        DEPENDPATH += $$PWD/../Libraries/bin/GL/msvc/
        LIBS += -L$$PWD/../Libraries/lib/GL/msvc/ -lglut32

    }
}

#SOURCES += \
#    TestOpenMeshAlgorithms/test_intersection/test_intersection.cpp \
#    TestOpenMeshAlgorithms/test_intersection/mainwindow.cpp \
#    TestOpenMeshAlgorithms/test_intersection/MeshIntersections.cpp \
#    TestOpenMeshAlgorithms/test_intersection/IntersectionNode.cpp



#HEADERS += \
#    TestOpenMeshAlgorithms/test_intersection/mainwindow.h \
#    TestOpenMeshAlgorithms/test_intersection/MeshIntersections.h \
#    TestOpenMeshAlgorithms/test_intersection/IntersectionNode.h

#HEADERS += \
#    TestOpenMeshAlgorithms/test_triangulation/TestTriangulationWidget.h

#SOURCES += \
#    TestOpenMeshAlgorithms/test_triangulation/TestTriangulationWidget.cpp \
#    TestOpenMeshAlgorithms/main.cpp

HEADERS += \
    TestOpenMeshAlgorithms/test_pasting/ImagePastingNode.h \
    TestOpenMeshAlgorithms/test_pasting/mainwindow.h \
    TestOpenMeshAlgorithms/test_pasting/TestImagePastingHandler.h

SOURCES += \
    TestOpenMeshAlgorithms/test_pasting/ImagePastingNode.cpp \
    TestOpenMeshAlgorithms/test_pasting/testimagepasting.cpp \
    TestOpenMeshAlgorithms/test_pasting/mainwindow.cpp \
    TestOpenMeshAlgorithms/test_pasting/TestImagePastingHandler.cpp



