QT += core gui

CONFIG += c++11 console
CONFIG -= app_bundle

CONFIG += gl_debug


# leave debug wrappers and macros for debug build only
CONFIG(debug,debug|release) {
  CONFIG(gl_debug) {
      DEFINES += GL_DEBUG
  }
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    window.cpp \
    vertex.cpp \
    transform3d.cpp \
    input.cpp \
    camera3d.cpp \
    OpenGL/openglerror.cpp \
    transform.cpp \
    assimp/assimp/color4.inl \
    assimp/assimp/material.inl \
    assimp/assimp/matrix3x3.inl \
    assimp/assimp/matrix4x4.inl \
    assimp/assimp/quaternion.inl \
    assimp/assimp/vector2.inl \
    assimp/assimp/vector3.inl \
    shader.cpp \
    texture.cpp \
    VQS.cpp \
    Quaternion.cpp \
    mesh.cpp \
    OpenGL/openglfunctions.cpp \
    OpenGL/openglprofiler.cpp \
    OpenGL/openglmarkerresult.cpp \
    OpenGL/openglframeresults.cpp \
    AnimatedModel.cpp \
    gbuffer.cpp \
    SphericalHarmonics.cpp \
    aoBuffer.cpp \
    rgbe.cpp


HEADERS += \
    window.h \
    vertex.h \
    transform3d.h \
    input.h \
    camera3d.h \
    OpenGL/openglbuffer.h \
    OpenGL/openglcommon.h \
    OpenGL/openglerror.h \
    OpenGL/openglshaderprogram.h \
    OpenGL/openglvertexarrayobject.h \
    macros.h \
    transform.h \
    assimp/assimp/Compiler/poppack1.h \
    assimp/assimp/Compiler/pstdint.h \
    assimp/assimp/Compiler/pushpack1.h \
    assimp/assimp/ai_assert.h \
    assimp/assimp/anim.h \
    assimp/assimp/camera.h \
    assimp/assimp/cexport.h \
    assimp/assimp/cfileio.h \
    assimp/assimp/cimport.h \
    assimp/assimp/color4.h \
    assimp/assimp/config.h \
    assimp/assimp/DefaultLogger.hpp \
    assimp/assimp/defs.h \
    assimp/assimp/Exporter.hpp \
    assimp/assimp/Importer.hpp \
    assimp/assimp/importerdesc.h \
    assimp/assimp/IOStream.hpp \
    assimp/assimp/IOSystem.hpp \
    assimp/assimp/light.h \
    assimp/assimp/Logger.hpp \
    assimp/assimp/LogStream.hpp \
    assimp/assimp/material.h \
    assimp/assimp/matrix3x3.h \
    assimp/assimp/matrix4x4.h \
    assimp/assimp/mesh.h \
    assimp/assimp/metadata.h \
    assimp/assimp/NullLogger.hpp \
    assimp/assimp/postprocess.h \
    assimp/assimp/ProgressHandler.hpp \
    assimp/assimp/quaternion.h \
    assimp/assimp/scene.h \
    assimp/assimp/texture.h \
    assimp/assimp/types.h \
    assimp/assimp/vector2.h \
    assimp/assimp/vector3.h \
    assimp/assimp/version.h \
    baselight.h \
    texture.h \
    shader.h \
    VQS.h \
    Quaternion.h \
    utility.h \
    mesh.h \
    libs/stb_image.h \
    OpenGL/openglfunctions_4_3_core.h \
    OpenGL/openglfunctions.h \
    OpenGL/openglprofiler.h \
    OpenGL/openglmarkerscoped.h \
    OpenGL/openglmarkerresult.h \
    OpenGL/openglframeresults.h \
    AnimatedModel.h \
    gbuffer.h \
    SphericalHarmonics.h \
    aoBuffer.h \
    rgbe.h


RESOURCES += \
    resources.qrc

unix: !macx {
    INCLUDEPATH +=  /usr/include
}

INCLUDEPATH += $$PWD/OpenGL
INCLUDEPATH += $$PWD/assimp
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/debug/ -lassimp
else:unix: LIBS += -L$$PWD/libs/ -lassimp

INCLUDEPATH += $$PWD/libs
DEPENDPATH += $$PWD/libs
