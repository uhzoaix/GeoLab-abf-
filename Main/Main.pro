QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TEMPLATE = app
TARGET = GeoLab

INCLUDEPATH *= ./
INCLUDEPATH *= ../
INCLUDEPATH += ./3rdparty/include \

SOURCES += \
    main.cpp

win32{
    DEFINES += OM_STATIC_BUILD
    win32-g++ {
    DESTDIR = $$PWD/../bin/mingw/

    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-unused-but-set-parameter
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -std=c++11
    DEFINES += _COMPILE_GCC
    DEFINES += USE_FREEGLUT
    LIBS += -L$$PWD/../lib/mingw/ -lGCLGui
    LIBS += -L$$PWD/../lib/mingw/ -lGCLCore
    LIBS += -L$$PWD/Libraries/lib/OpenMesh/mingw/ -lOpenMeshCore
    LIBS += -L$$PWD/Libraries/lib/GL/mingw/ -lfreeglut
    }
    LIBS += -lopengl32 -lglu32
    !win32-g++ {
        DEFINES += _USE_MATH_DEFINES
        win32-msvc2010 {
            DESTDIR = $$PWD/../bin/vs2010/

            CONFIG(release, debug|release){
                LIBS += -L$$PWD/../../lib/vs2010/ -lGCLGui
                LIBS += -L$$PWD/../../lib/vs2010/ -lGCLCore
                LIBS += -L$$PWD/Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCore
            }

            CONFIG(debug, debug|release){
                LIBS += -L$$PWD/../../lib/vs2010/ -lGCLGuid
                LIBS += -L$$PWD/../../lib/vs2010/ -lGCLCored
                LIBS += -L$$PWD/Libraries/lib/OpenMesh/vs2010/ -lOpenMeshCored
            }
        }
        win32-msvc2012 {
            DESTDIR = $$PWD/../bin/vs2012/
            LIBS += -L$$PWD/../lib/vs2012/ -lGCLGui
            LIBS += -L$$PWD/../lib/vs2012/ -lGCLCore
            LIBS += -L$$PWD/Libraries/lib/OpenMesh/vs2012/ -lOpenMeshCore
        }
        win32-msvc2013 {
            CONFIG(release, debug|release){
                DESTDIR = $$PWD/../../build/Release/bin/vs2013/
                LIBS += -L$$PWD/../../build/Release/bin/vs2013/ -lGeoLabGui
                LIBS += -L$$PWD/../../build/Release/bin/vs2013/ -lGeoLabCore
                LIBS += -L$$PWD/../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCore
            }

            CONFIG(debug, debug|release){
                DESTDIR = $$PWD/../../build/Debug/bin/vs2013/
                LIBS += -L$$PWD/../../build/Debug/bin/vs2013/ -lGeoLabGuid
                LIBS += -L$$PWD/../../build/Debug/bin/vs2013/ -lGeoLabCored
                LIBS += -L$$PWD/../../3rdparty/lib/OpenMesh/vs2013/ -lOpenMeshCored
            }
           # LIBS += -L$$PWD/../../3rdparty/lib/GL/msvc/ -lglut32
        }
        DEPENDPATH += $$PWD/../../3rdparty/bin/GL/msvc/

        LIBS += -L$$PWD/../../3rdparty/lib/GL/msvc/ -lglut32
#        LIBS += -L$$PWD/Libraries/lib/GL/msvc/ -lglew32
    }
}


