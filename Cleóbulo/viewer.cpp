#include "viewer.h"
#include <vector>

#include "arclength.h"
#include <GL/glu.h>
#include "posinterpolator.h"
#include "scenecontainer.h"
#include "joint.h"


Viewer::Viewer(QWidget* parent) :
    QGLViewer(parent){

}

void Viewer::draw() {
    for( size_t i = 0 ; i < SceneContainer::HowManyObjects(); i++ ) {
        Joint* object = SceneContainer::ObjectAt(i);
        object->Draw(SceneContainer::AnimatePosition(),SceneContainer::AnimateOrientation());
    }

    /*glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINE_STRIP);
    int slices = 50;
    double slice_size = 1.0/((double)(slices-1));
    for(int i = 0 ; i < slices ; i++ )
    {
        qglviewer::Vec p = nova_->Evaluate(slice_size*((double)(i)));
        glVertex3d(p.x,p.y,p.z);
        i += 0.02;
    }
    glEnd();

    glPushMatrix();
    glTranslated(0,0,0);
    gluSphere(gluNewQuadric(),0.02,15,15);
    glPopMatrix();

    glPushMatrix();
    glTranslated(1.5,3.0,3.0);
    gluSphere(gluNewQuadric(),0.02,15,15);
    glPopMatrix();

    glPushMatrix();
    glTranslated(3,0,0);
    gluSphere(gluNewQuadric(),0.02,15,15);
    glPopMatrix();

    glColor3f(1.0,0.0,0.0);
    for(int i = 0 ; i < 101 ; i++ ) {
        glPushMatrix();
        glTranslated(global_p[i].x,global_p[i].y,global_p[i].z-0.004);
        gluSphere(gluNewQuadric(),0.02,15,15);
        glPopMatrix();
    }

    glColor3f(0.0,0.0,1.0);
    for(int i = 0 ; i < 11 ; i++ ) {
        glPushMatrix();
        qglviewer::Vec point = nova_->Evaluate(i*0.1);
        glTranslated(point.x,point.y,point.z+0.004);
        gluSphere(gluNewQuadric(),0.02,15,15);
        glPopMatrix();
    }

    glColor3f(0.0,1.0,1.0);
    glBegin(GL_LINE_STRIP);
    for(int i = 0 ; i < 11 ; i++ ) {
        glPushMatrix();
        qglviewer::Vec point;
        point = pos_interpolator.GetPositionAt(i*10);
        glVertex3f(point.x,point.y,point.z);
        //gluSphere(gluNewQuadric(),0.02,15,15);
        glPopMatrix();
    }
    glEnd();*/
    this->drawText(10,10,QString("Frame: %1").arg(SceneContainer::current_frame()));
}



void Viewer::Play() {
    if(!animationIsStarted()) {
        startAnimation();
    }
}

void Viewer::Stop() {
    if(animationIsStarted()) {
        stopAnimation();
    }
    if(SceneContainer::current_frame()!=0) {
        SceneContainer::SetCurrentFrame(0);
        emit CurrentFrame(0);
        updateGL();
    }
}

void Viewer::Pause() {
    if(animationIsStarted()) {
        stopAnimation();
    }
}

void Viewer::animate() {

    SceneContainer::GoToNextFrame();
    emit CurrentFrame(SceneContainer::current_frame());
}

void  Viewer::SetCurrentFrame(int frame) {
    SceneContainer::SetCurrentFrame(frame);
    updateGL();
}

void Viewer::init() {
    SceneContainer::CreateDefaultScene();
    SceneContainer::SetFrameRange(0,100);
   /* glDisable(GL_LIGHTING);

    nova_ = new BezierQuadratic(qglviewer::Vec(0,0,0),qglviewer::Vec(1.5,3.0,3.0),qglviewer::Vec(3,0,0));
    ArcLength arc(nova_);
    arc.AdaptativeGaussianPrecalculation();
    for(int i = 0 ; i < 101 ; i++ )
        global_p[i] = arc.GetByNormalizedS(i*0.01);


    pos_interpolator.SetStartFrame(0);
    pos_interpolator.SetLastFrame(100);


    std::vector<PositionStep> positions;
    for(int i = 0 ; i < 11 ; i++ ) {
        PositionStep p;
        p.position_ = nova_->Evaluate(i*0.1) + qglviewer::Vec(0,0,-1);
        int frame = 10*i;

        p.frame_ = frame;
        positions.push_back(p);
    }

    for(int i = 0 ; i < positions.size() ; i++ )
        std::cout << "Frame: " << positions.at(i).frame_ << " Position: " << std::endl;

    pos_interpolator.LoadPoints(positions);
    //for(int i = 0 ; i < 101 ; i++ )
    //    global_p[i] = pos_interpolator.GetPositionAt(i);*/
    setSceneRadius(10.0f);

}

QString Viewer::helpString() const {
    return QString();
}
