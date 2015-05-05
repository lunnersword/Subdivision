#-------------------------------------------------
#
# Project created by QtCreator 2014-03-04T19:13:59
#
#-------------------------------------------------

QT       += core gui
QT       += opengl widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = subdivision
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    basefaces.cpp \
    loop_faces.cpp \
    doo_faces.cpp \
    cat_faces.cpp \
    basetopolopy.cpp \
    topmanager.cpp \
    glwidget.cpp \
    modelshader.cpp \
    blendwidget.cpp

HEADERS  += mainwindow.h \
    doo_faces.h \
    basefaces.h \
    loop_faces.h \
    cat_faces.h \
    basetopology.h \
    topmanager.h \
    glwidget.h \
    defines.h \
    modelshader.h \
    blendwidget.h

FORMS    +=

RESOURCES += \
    subdivision.qrc

OTHER_FILES +=

DISTFILES += \
    modelPointLightFragmentShader.frag \
    modelDirectionLightFragmentShader.frag \
    modelFragmentShader.frag \
    modelVertexShader.vert \
    specialFragmentShader.frag \
    specialVertexShader.vert \
    modelSpotlightFragmentShader.frag \
    multipleLights.frag \
    multiplelights.vert \
    multiplelights330.vert \
    multiplelights330.frag

