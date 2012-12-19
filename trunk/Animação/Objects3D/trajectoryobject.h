#ifndef TRAJECTORYOBJECT_H
#define TRAJECTORYOBJECT_H

#include "object3d.h"
#include <GL/glu.h>

class ObjectAnimator;

class TrajectoryObject : public Object3D
{
public:
    enum TrajectoryType {kPosition,kOrientation,kBoth};
    TrajectoryObject(ObjectAnimator* animator);

    void Update();
    void Update(int start_frame, int end_frame);

    void SetType(TrajectoryType type);

private:
    void DrawObject(bool animate_position = true, bool animate_orientation = true);

    void DrawPositionsCurve();

    TrajectoryType type_;
    std::vector<qglviewer::Vec> positions_;
    std::vector<qglviewer::Vec> keyframes_;
    ObjectAnimator* animator_;
    GLUquadric* quadric_;
    double radius_;
};

#endif // TRAJECTORYOBJECT_H
