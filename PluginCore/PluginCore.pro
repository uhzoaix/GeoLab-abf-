QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TEMPLATE = lib

CONFIG(debug, debug|release){
    TARGET = GeoLabPluginCored
}

CONFIG(release, debug|release){
    TARGET = GeoLabPluginCore
}


#sharedlib
{
    DEFINES *= GCL_EXPORT_SHARED_LIB
    DEFINES *= GCL_PLUGIN_SHARED_LIB
}
#staticlib
{
    #CONFIG += staticlib
}

INCLUDEPATH *= ../
INCLUDEPATH *= ../../
INCLUDEPATH += ../../3rdparty/include \

win32{
    DEFINES += OM_STATIC_BUILD
    win32-g++ {
    DESTDIR = $$PWD/../../../lib/mingw/

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-unused-but-set-parameter
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -std=c++11
    DEFINES += _COMPILE_GCC
    DEFINES += USE_FREEGLUT
    LIBS += -L$$PWD/../../../lib/mingw/ -lGCLCore
    LIBS += -L$$PWD/../../Libraries/lib/OpenMesh/mingw/ -lOpenMeshCore
    LIBS += -L$$PWD/../../Libraries/lib/GL/mingw/ -lglew32
    LIBS += -L$$PWD/../../Libraries/lib/GL/mingw/ -lfreeglut
    }
    LIBS += -lopengl32 -lglu32
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
                 DESTDIR = $$PWD/../../build/Release/bin/vs2013/
                 LIBS += -L$$PWD/../../build/Release/bin/vs2013/ -lGeoLabCore
                 LIBS += -L$$PWD/../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCore
            }
            CONFIG(debug, debug|release){
                DESTDIR = $$PWD/../../build/Debug/bin/vs2013/
                LIBS += -L$$PWD/../../build/Debug/bin/vs2013/ -lGeoLabCored
                LIBS += -L$$PWD/../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCored
            }
        }

        DEPENDPATH += $$PWD/../../3rdparty/bin/GL/msvc/
    }
}

OTHER_FILES +=

HEADERS += \
    PluginManager/PluginManager.h \
    PluginInterface/ImageIOInterface.h \
    PluginInterface/MeshFilterInterface.h \
    PluginInterface/MeshIOInterface.h \
    PluginInterface/NormalPluginInterface.h \
    PluginInterface/PluginInterface.h \
    PluginInterface/RichParameters.h

SOURCES += \
    PluginManager/PluginManager.cpp \
    PluginInterface/ImageIOInterface.cpp \
    PluginInterface/MeshFilterInterface.cpp \
    PluginInterface/MeshIOInterface.cpp \
    PluginInterface/NormalPluginInterface.cpp \
    PluginInterface/PluginInterface.cpp \
    PluginInterface/RichParameters.cpp




