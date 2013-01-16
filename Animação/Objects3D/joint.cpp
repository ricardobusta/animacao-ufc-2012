#include "joint.h"

Joint::Joint(Object3D* child_object)
{
    if(child_object!=NULL){
        label_ = (child_object->label() + " Joint");
    }else{
        label_ = "Empty Joint";
    }
    child_object_ = child_object;

    parent_joint_ = NULL;
}

void Joint::addChildJoint(Joint* child) {
    children_joint_.push_back(child);
    child->parent_joint_ = this;
}

int Joint::howManyChilds() {
    return children_joint_.size();

}

Joint* Joint::childAt(int index) {
    return children_joint_.at(index);
}

void Joint::drawObject() {
    child_object_->draw();

    for(size_t i = 0 ; i < children_joint_.size() ; i++ )
        children_joint_.at(i)->draw();
}


Object3D *Joint::childObject()
{
    return child_object_;
}

void Joint::drawBoundingBox()
{
    child_object_->drawBoundingBox();
}

void Joint::drawBone()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);

    for(size_t i=0;i<children_joint_.size();i++){
        qglviewer::Vec pos = children_joint_.at(i)->position();

        glVertex3f(0,0,0);
        glVertex3f(pos.x,pos.y,pos.z);
    }
    glEnd();
    glPopAttrib();
}
