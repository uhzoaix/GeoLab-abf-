TEMPLATE = lib

CONFIG(debug, debug|release){
    TARGET = GeoLabCored
}

CONFIG(release, debug|release){
    TARGET = GeoLabCore
}


QT +=core opengl xml

#sharedlib
{
    DEFINES *= GCL_EXPORT_SHARED_LIB
    DEFINES *= GCL_CORE_SHARED_LIB
}
#staticlib
{
    #CONFIG += staticlib
}
INCLUDEPATH *= ../
INCLUDEPATH *= ../../
INCLUDEPATH += ../../3rdparty/include \



HEADERS += \
    DataStructures/GCLTraits.h \
    DataStructures/GeometryBase.h \
    DataStructures/OMesh.h \
    Math/HomoMatrix.h \
    Math/MathDefines.h \
    Math/Quaternion.h \
    Math/Vec.h \
    SceneGraph/Camera.h \
    SceneGraph/GeometryNode.h \
    SceneGraph/GroupNode.h \
    SceneGraph/Lights.h \
    SceneGraph/Material.h \
    SceneGraph/Node.h \
    SceneGraph/NodeType.h \
    SceneGraph/OpenMeshNode.h \
    SceneGraph/Renderer.h \
    SceneGraph/RenderModes.h \
    SceneGraph/RenderVisitor.h \
    SceneGraph/SceneGraph.h \
    SceneGraph/Visitor.h \
    SceneGraph/EventHandler/EventHandler.h \
    SceneGraph/EventHandler/Manipulator.h \
    SceneGraph/EventHandler/NormalManipulator.h \
    SceneGraph/EventHandler/TrackBallManipulator.h \
    SceneGraph/Selection/AbstractSelection.h \
    SceneGraph/Selection/Prefilter.h \
    SceneGraph/Selection/RectSelection.h \
    SceneGraph/Selection/ZBufferRenderer.h \
    SceneGraph/Decorations/Arcball.h \
    SceneGraph/Decorations/DecorationBase.h \
    SceneGraph/Decorations/Rect.h \
    gclnamespace.h \
    SceneGraph/NodeFactory.h \
    DataStructures/AABB.h \
    DataStructures/CtrlPointsBase.h \
    DataStructures/DeformationBase.h \
    DataStructures/OMeshCtrlPoints.h \
    SceneGraph/OMeshCtrlPointsNode.h \
    SceneGraph/CtrlPointsNode.h \
    SceneGraph/NodeAction.h \
    Utilities/LogSystem.h \
    Utilities/ProgressTool.h \
    Utilities/Settings.h \
    Utilities/TestTool.h \
    gcl_global.h \
    DataStructures/BarycentricCoordinates.h \
    DataStructures/DepthImage.h \
    SceneGraph/DepthImageNode.h \
    SceneGraph/GLStates.h \
    SceneGraph/TransformationNode.h \
    SceneGraph/Shader.h \
    DataStructures/LightMesh.h \
    SceneGraph/LightMeshNode.h



SOURCES += \
    DataStructures/OMesh.cpp \
    SceneGraph/Camera.cpp \
    SceneGraph/GeometryNode.cpp \
    SceneGraph/GroupNode.cpp \
    SceneGraph/Lights.cpp \
    SceneGraph/Material.cpp \
    SceneGraph/Node.cpp \
    SceneGraph/OpenMeshNode.cpp \
    SceneGraph/RenderModes.cpp \
    SceneGraph/RenderVisitor.cpp \
    SceneGraph/SceneGraph.cpp \
    SceneGraph/Visitor.cpp \
    SceneGraph/EventHandler/Manipulator.cpp \
    SceneGraph/EventHandler/NormalManipulator.cpp \
    SceneGraph/EventHandler/TrackBallManipulator.cpp \
    SceneGraph/Selection/AbstractSelection.cpp \
    SceneGraph/Selection/Prefilter.cpp \
    SceneGraph/Selection/RectSelection.cpp \
    SceneGraph/Selection/ZBufferRenderer.cpp \
    SceneGraph/Decorations/Arcball.cpp \
    SceneGraph/Decorations/Rect.cpp \
    SceneGraph/NodeFactory.cpp \
    DataStructures/AABB.cpp \
    DataStructures/CtrlPointsBase.cpp \
    DataStructures/DeformationBase.cpp \
    DataStructures/OMeshCtrlPoints.cpp \
    SceneGraph/OMeshCtrlPointsNode.cpp \
    SceneGraph/CtrlPointsNode.cpp \
    SceneGraph/NodeAction.cpp \
    Utilities/ProgressTool.cpp \
    Utilities/Settings.cpp \
    Utilities/TestTool.cpp \
    DataStructures/BarycentricCoordinates.cpp \
    DataStructures/DepthImage.cpp \
    SceneGraph/DepthImageNode.cpp \
    SceneGraph/GLStates.cpp \
    SceneGraph/TransformationNode.cpp \
    SceneGraph/Shader.cpp \
    DataStructures/LightMesh.cpp \
    SceneGraph/LightMeshNode.cpp


#win32
win32{
    DEFINES += OM_STATIC_BUILD
    win32-g++ {
    DESTDIR = ../../lib/mingw/
    LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/mingw/ -lOpenMeshCore
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-unused-but-set-parameter
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_CXXFLAGS += -output-def
    DEFINES += _COMPILE_GCC
    DEFINES += USE_FREEGLUT
    LIBS += -L$$PWD/../../Libraries/lib/GL/mingw/ -lfreeglut
    }
    !win32-g++ {
        DEFINES += _USE_MATH_DEFINES
        win32-msvc2010 {
            DESTDIR = $$PWD/../../../lib/vs2010/
            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCore
            }
        }
        win32-msvc2012 {
            DESTDIR = $$PWD/../../../lib/vs2012/
            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCore
            }
        }
        win32-msvc2013 {
            CONFIG(debug, debug|release){
                DESTDIR = $$PWD/../../build/Debug/bin/vs2013/
                LIBS += -L$$PWD/../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                DESTDIR = $$PWD/../../build/Release/bin/vs2013/
                LIBS += -L$$PWD/../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCore
            }
        }
        LIBS += -L$$PWD/../../3rdparty/lib/GL/msvc/ -lglew32
        LIBS += -L$$PWD/../../3rdparty/lib/GL/msvc/ -lglut32
    }
}
