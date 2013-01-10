#include "oriinterpolator.h"
#include "quaternioninterpolation.h"
#include <cmath>

OriInterpolator::OriInterpolator():GenericInterpolator()
{
    type_ = kSlerp;
}

void OriInterpolator::SetStartFrame(int start_frame) {
    start_frame_ = start_frame;
}

void OriInterpolator::SetLastFrame(int last_frame) {
    last_frame_ = last_frame;
}

qglviewer::Quaternion OriInterpolator::GetOrientationAt(int frame, bool* valid) {

    int interval = ChooseInterval(frame);

    if((interval == -1) or (interval==steps_.size()-1)) {
        if(valid!=NULL) *valid = false;
        return qglviewer::Quaternion();
    }
    QuaternionInterpolator* interpolator = quaternions_curves_.at(interval);

    double t = ((double)(frame - steps_.at(interval).frame_))/((double)(steps_.at(interval+1).frame_ - steps_.at(interval).frame_));

    t = speedControl(t);

    if(valid!=NULL) *valid = true;
    return interpolator->Evaluate(t);
}

int OriInterpolator::ChooseInterval(int frame) {
    if(steps_.size() == 0) return -1;

    for(size_t i = 0 ; i < steps_.size() ; i++ ) {
        if( steps_.at(i).frame_ > frame)
            return i-1;
    }
    if( steps_.at(steps_.size()-1).frame_ < frame)
        return steps_.size()-2;
    return 0;
}

void OriInterpolator::SetInterpolationType(InterpolationType type) {
    type_ = type;
}

qglviewer::Quaternion OriInterpolator::Bisect(qglviewer::Quaternion a, qglviewer::Quaternion b) {
    double aux[] = {a[0]+b[0],a[1]+b[1],a[2]+b[2],a[3]+b[3]};
    double m = std::sqrt(aux[0]*aux[0] + aux[1]*aux[1] + aux[2]*aux[2] + aux[3]*aux[3]);
    qglviewer::Quaternion new_quaternion(aux[0]/m,aux[1]/m,aux[2]/m,aux[3]/m);
    return new_quaternion;

}

qglviewer::Quaternion OriInterpolator::Double(qglviewer::Quaternion a, qglviewer::Quaternion b) {
    double dot = qglviewer::Quaternion::dot(a,b);
    double q[4];
    for(size_t i = 0 ; i < 4 ; i++)
        q[i] = 2.0*dot*b[i] - a[i];
    return qglviewer::Quaternion(q[0],q[1],q[2],q[3]);
}

void OriInterpolator::GenerateMainCurve() {
    switch(type_) {
    case kBezier:
        GenerateBezierCurve();
        break;
    default:
        GenerateLinearCurve();
        break;
    }
}

void OriInterpolator::GenerateLinearCurve() {
    if(steps_.size() <= 1) return;

    for(size_t i = 0 ; i < steps_.size()-1 ; i++ ) {
        QuaternionInterpolator* interpolator = new SlerpQuaternion(steps_.at(i).orientation_,steps_.at(i+1).orientation_);
        quaternions_curves_.push_back(interpolator);
    }
}

void OriInterpolator::GenerateBezierCurve() {
    if(steps_.size() <= 2) {
        GenerateLinearCurve();
        return;
    }

    qglviewer::Quaternion a,b,pa;
    for(size_t i = 0 ; i < steps_.size() ; i++) {
        if(i==0) {
            pa = steps_.at(i).orientation_;
        }else if(i==steps_.size()-1){
            quaternions_curves_.push_back(new CasteljauQuaternion(steps_.at(i-1).orientation_,pa,steps_.at(i).orientation_,steps_.at(i).orientation_));
        }else{
            a = Bisect(Double(steps_.at(i-1).orientation_,steps_.at(i).orientation_),steps_.at(i+1).orientation_);
            b = Double(a,steps_.at(i).orientation_);
            quaternions_curves_.push_back(new CasteljauQuaternion(steps_.at(i-1).orientation_,pa,b,steps_.at(i).orientation_));
            pa = a;
        }
    }
}

void OriInterpolator::ClearAll() {
    steps_.clear();
    for(size_t i = 0 ; i < quaternions_curves_.size() ; i++ )
        delete quaternions_curves_.at(i);
    quaternions_curves_.clear();
}

void OriInterpolator::LoadOrientations(std::vector<OrientationStep>& steps) {
    ClearAll();

    for(size_t i = 0 ; i < steps.size() ; i++ )
        steps_.push_back(steps.at(i));

    GenerateMainCurve();
}

void OriInterpolator::AddOrientation(OrientationStep) {

}
