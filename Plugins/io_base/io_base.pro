
QT       += core


    DEFINES *= GCL_EXPORT_SHARED_LIB

unix {
    target.path = /usr/lib
    INSTALLS += target
}


TEMPLATE = lib
CONFIG += plugin

CONFIG(debug, debug|release){
    TARGET = io_based
}
CONFIG(release, debug|release){
    TARGET = io_base
}

INCLUDEPATH *= ./
INCLUDEPATH *= $$PWD/../../
INCLUDEPATH += $$PWD/../../../3rdparty/include \

DEFINES *= GCL_EXPORT_SHARED_LIB

win32{
    DEFINES += OM_STATIC_BUILD
    win32-g++ {
            CONFIG(debug, debug|release){
                DESTDIR = $$PWD/../../../build/Debug/bin/mingw/plugins
                LIBS += -L$$PWD/../../../build/Debug/bin/mingw/ -lOpenMeshAlgorithmsd
                LIBS += -L$$PWD/../../../build/Debug/bin/mingw/ -lGeoLabPluginCored
                LIBS += -L$$PWD/../../../build/Debug/bin/mingw/ -lGeoLabCored
                LIBS += -L$$PWD/../../../3rdparty/lib/OpenMesh/mingw/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                DESTDIR = $$PWD/../../../build/Release/bin/mingw/plugins
                LIBS += -L$$PWD/../../../build/Release/bin/mingw/ -lOpenMeshAlgorithms
                LIBS += -L$$PWD/../../../build/Release/bin/mingw/ -lGeoLabPluginCore
                LIBS += -L$$PWD/../../../build/Release/bin/mingw/ -lGeoLabCore
                LIBS += -L$$PWD/../../../3rdparty/lib/OpenMesh/mingw/ -lOpenMeshCore
            }

    DEPENDPATH += $$PWD/../../../3rdparty/bin/GL/mingw/

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-unused-but-set-parameter
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -Wno-sign-compare
    QMAKE_CXXFLAGS += -std=c++11
    DEFINES += _COMPILE_GCC
    DEFINES += USE_FREEGLUT
#    DEFINES += EIGEN_MPL2_ONLY
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
            CONFIG(debug, debug|release){
                DESTDIR = $$PWD/../../../build/Debug/bin/vs2013/plugins
                LIBS += -L$$PWD/../../../build/Debug/bin/vs2013/ -lOpenMeshAlgorithmsd
                LIBS += -L$$PWD/../../../build/Debug/bin/vs2013/ -lGeoLabPluginCored
                LIBS += -L$$PWD/../../../build/Debug/bin/vs2013/ -lGeoLabCored
                LIBS += -L$$PWD/../../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCored
            }
            CONFIG(release, debug|release){
                DESTDIR = $$PWD/../../../build/Release/bin/vs2013/plugins
                LIBS += -L$$PWD/../../../build/Release/bin/vs2013/ -lOpenMeshAlgorithms
                LIBS += -L$$PWD/../../../build/Release/bin/vs2013/ -lGeoLabPluginCore
                LIBS += -L$$PWD/../../../build/Release/bin/vs2013/ -lGeoLabCore
                LIBS += -L$$PWD/../../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCore
            }
        }
        DEPENDPATH += $$PWD/../../../3rdparty/bin/GL/msvc/
    }
    LIBS += -lopengl32 -lglu32
}
HEADERS += \
    IO_BasePlugin.h

SOURCES += \
    IO_BasePlugin.cpp
