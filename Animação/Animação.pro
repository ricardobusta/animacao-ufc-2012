#-------------------------------------------------
#
# Project created by QtCreator 2012-11-20T14:48:34
#
#-------------------------------------------------

QT       += core gui xml opengl

TARGET = Animação
TEMPLATE = app

#DEFINES += DEBUG_TEXT

HEADERS += \
    Curves/curve.h \
    Curves/arclength.h \
    Interpolation/quaternioninterpolation.h \
    Interpolation/posinterpolator.h \
    Interpolation/oriinterpolator.h \
    Interpolation/objectanimator.h \
    Interpolation/interpolator.h \
    Interpolation/animation_step.h \
    Objects3D/trajectoryobject.h \
    Objects3D/object3d.h \
    Objects3D/joint.h \
    Objects3D/curveobject.h \
    Utils/scenecontainer.h \
    Widgets/viewer.h \
    Widgets/timebar.h \
    mainwindow.h \
    Objects3D/fileobj.h \
    Widgets/rotationwidget.h \
    Objects3D/torus.h \
    Interpolation/genericinterpolator.h \
    Utils/matrix4d.h \
    Utils/iksolver.h \
    Utils/genericmatrix.h \
    ikdialog.h \
    Objects3D/iktarget.h \
    Objects3D/particle.h

SOURCES += \
    Curves/arclength.cpp \
    Interpolation/quaternioninterpolation.cpp \
    Interpolation/posinterpolator.cpp \
    Interpolation/oriinterpolator.cpp \
    Interpolation/objectanimator.cpp \
    Interpolation/interpolator.cpp \
    Objects3D/trajectoryobject.cpp \
    Objects3D/object3d.cpp \
    Objects3D/joint.cpp \
    Objects3D/curveobject.cpp \
    Utils/scenecontainer.cpp \
    Widgets/viewer.cpp \
    Widgets/timebar.cpp \
    main.cpp \
    mainwindow.cpp \
    Objects3D/fileobj.cpp \
    Widgets/rotationwidget.cpp \
    Objects3D/torus.cpp \
    Interpolation/genericinterpolator.cpp \
    Utils/matrix4d.cpp \
    Utils/iksolver.cpp \
    Utils/genericmatrix.cpp \
    ikdialog.cpp \
    Objects3D/iktarget.cpp\
    Objects3D/particle.cpp


FORMS += \
    mainwindow.ui \
    ikdialog.ui

RESOURCES += \
    Resources/buttons/buttons.qrc \
    Resources/textures/textures.qrc \
    Resources/models/models.qrc \
    Resources/shaders/shaders.qrc

        #tive que recolocar pra compilar, o outro não funcionou.	85	LIBS *= -lGLU
unix:!macx: LIBS += -L$$PWD/../../../../../../usr/lib/x86_64-linux-gnu/ -lqglviewer-qt4
INCLUDEPATH += $$PWD/../../../../../../usr/include/QGLViewer
DEPENDPATH += $$PWD/../../../../../../usr/include/QGLViewer
unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../../usr/lib/x86_64-linux-gnu/libqglviewer-qt4.a

exists( /usr/lib/libQGLViewer.so ) {
LIBS *= -lqglviewer-qt4
}

LIBS *= -lGLU

RESOURCES +=

OTHER_FILES += \
    todo.txt \
    Resources/shaders/toon.vert \
    Resources/shaders/toon.frag \
    Resources/shaders/phong.vert \
    Resources/shaders/phong.frag
