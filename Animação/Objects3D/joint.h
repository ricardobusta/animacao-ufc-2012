#ifndef JOINT_H
#define JOINT_H

#include "object3d.h"

class GenericMatrix;

class Joint : public Object3D
{
public:
    Joint(Object3D* children_object);
    virtual ~Joint();

    void addChildJoint(Joint* child);

    int howManyChilds();
    Joint* childAt(int index);
    Object3D *childObject();

    Joint *parent();

    qglviewer::Vec globalPosition();
    qglviewer::Vec globalEffectorPosition();
    GenericMatrix globalTransformationMatrix();
protected:
    virtual void drawObject(int depth);
    virtual void drawBoundingBox();
    virtual void drawBone(int depth);

private:
    Object3D* child_object_;
    std::vector<Joint*> children_joint_;

    Joint *parent_joint_;
};

#endif // JOINT_H
