
QT       += core

TEMPLATE = lib
CONFIG += plugin


unix {
    target.path = /usr/lib
    INSTALLS += target
}


TEMPLATE = lib
CONFIG += plugin

CONFIG(debug, debug|release){
    TARGET = Filter_Parameterziation_ABFd
}
CONFIG(release, debug|release){
    TARGET = Filter_Parameterization_ABF
}

INCLUDEPATH *= ./
INCLUDEPATH *= $$PWD/../../
INCLUDEPATH += $$PWD/../../Libraries/include \

DEFINES *= GCL_EXPORT_SHARED_LIB

win32{
    DEFINES += OM_STATIC_BUILD
    win32-g++ {
    DESTDIR = $$PWD/../../../bin/mingw/plugins

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-unused-but-set-parameter
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -Wno-sign-compare
    QMAKE_CXXFLAGS += -std=c++11
    DEFINES += _COMPILE_GCC
    DEFINES += USE_FREEGLUT
    LIBS += -L$$PWD/../../../lib/mingw/ -lOpenMeshAlgorithms
    LIBS += -L$$PWD/../../../lib/mingw/ -lGCLPlugins
    LIBS += -L$$PWD/../../../lib/mingw/ -lGCLCore
    LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/mingw/ -lOpenMeshCore
    LIBS += -L$$PWD/../../Libraries/lib/GL/mingw/ -lfreeglut
    }
    !win32-g++ {
        DEFINES += _USE_MATH_DEFINES
        win32-msvc2010 {
            DESTDIR = $$PWD/../../../bin/vs2010/plugins
            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2010/ -lOpenMeshAlgorithmsd
                LIBS += -L$$PWD/../../../lib/vs2010/ -lGCLPluginsd
                LIBS += -L$$PWD/../../../lib/vs2010/ -lGCLCored
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2010/ -lOpenMeshAlgorithms
                LIBS += -L$$PWD/../../../lib/vs2010/ -lGCLPlugins
                LIBS += -L$$PWD/../../../lib/vs2010/ -lGCLCore
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCore
            }
        }
        win32-msvc2012 {
            DESTDIR = $$PWD/../../../bin/vs2012/plugins
            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2012/ -lOpenMeshAlgorithmsd
                LIBS += -L$$PWD/../../../lib/vs2012/ -lGCLPluginsd
                LIBS += -L$$PWD/../../../lib/vs2012/ -lGCLCored
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2012/ -lOpenMeshAlgorithms
                LIBS += -L$$PWD/../../../lib/vs2012/ -lGCLPlugins
                LIBS += -L$$PWD/../../../lib/vs2012/ -lGCLCore
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCore
            }
        }
        win32-msvc2013 {
            DESTDIR = $$PWD/../../../bin/vs2013/plugins
            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2013/ -lOpenMeshAlgorithmsd
                LIBS += -L$$PWD/../../../lib/vs2013/ -lGCLPluginsd
                LIBS += -L$$PWD/../../../lib/vs2013/ -lGCLCored
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2013/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                LIBS += -L$$PWD/../../../lib/vs2013/ -lOpenMeshAlgorithms
                LIBS += -L$$PWD/../../../lib/vs2013/ -lGCLPlugins
                LIBS += -L$$PWD/../../../lib/vs2013/ -lGCLCore
                LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/vs2013/ -lOpenMeshCore
            }
        }
        DEPENDPATH += $$PWD/../../Libraries/bin/GL/msvc/
    }
    LIBS += -lopengl32 -lglu32
}




