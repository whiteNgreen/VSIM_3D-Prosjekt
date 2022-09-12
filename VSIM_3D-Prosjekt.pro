QT          += core gui widgets opengl

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

INCLUDEPATH += \
    ./gsl
#    ./stb_image

SOURCES += main.cpp \
    Objects/bakke.cpp \
    Shaders/shader.cpp \
    aabbfence.cpp \
    beziercurve.cpp \
    camera.cpp \
    Objects/cubemap.cpp \
    gsl/matrix3x3.cpp \
    gsl/matrix4x4.cpp \
    gsl/vector2d.cpp \
    gsl/vector3d.cpp \
    Objects/light.cpp \
    logger.cpp \
    mainwindow.cpp \
    objmesh.cpp \
    octahedronball.cpp \
    quadtree.cpp \
    renderwindow.cpp \
    simpleobject.cpp \
    terrain.cpp \
    texture.cpp \
    trianglesurface.cpp \
    vertex.cpp \
    visualobject.cpp

HEADERS += \
    Objects/bakke.h \
    Shaders/shader.h \
    aabbfence.h \
    beziercurve.h \
    camera.h \
    Objects/cubemap.h \
    gsl/gltypes.h \
    gsl/math_constants.h \
    gsl/matrix3x3.h \
    gsl/matrix4x4.h \
    gsl/vector2d.h \
    gsl/vector3d.h \
    Objects/light.h \
    logger.h \
    mainwindow.h \
    objmesh.h \
    octahedronball.h \
    quadtree.h \
    renderwindow.h \
    simpleobject.h \
    terrain.h \
    texture.h \
    trianglesurface.h \
    vertex.h \
    visualobject.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    Shaders/cubemapShader.frag \
    Shaders/cubemapShader.vert \
    Shaders/phongshader.frag \
    Shaders/phongshader.vert \
    Shaders/plainshader.frag \
    Shaders/plainshader.vert \
    Shaders/textureshader.frag \
    Shaders/textureshader.vert \
