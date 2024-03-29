#include "Utils/scenecontainer.h"
#include "Objects3D/joint.h"
#include "Objects3D/object3d.h"
#include "Interpolation/objectanimator.h"

#include "Curves/curve.h"
#include "Objects3D/curveobject.h"
#include "Interpolation/quaternioninterpolation.h"

#include "Objects3D/torus.h"

#include "Objects3D/fileobj.h"

#include "Widgets/viewer.h"

#include "Objects3D/particle.h"

#include <QQuaternion>
Viewer* SceneContainer::viewer_reference_ = NULL;

std::vector<Joint*> SceneContainer::objects_;
std::vector<Object3D*> SceneContainer::extras_;
std::vector<ObjectAnimator*> SceneContainer::animators_;
//std::vector<IKTarget*> SceneContainer::ik_targets_;
IKTarget SceneContainer::ikTarget;

Object3D* SceneContainer::selected_object_ = NULL;

bool SceneContainer::animate_position_ = true;
bool SceneContainer::animate_orientation_ = true;

int SceneContainer::start_frame_ = 0;
int SceneContainer::end_frame_ = 100;
int SceneContainer::current_frame_ = 0;

bool SceneContainer::render_box_over_object_ = false;
bool SceneContainer::draw_bones_ = false;

bool SceneContainer::draw_with_names_ = false;

bool SceneContainer::alternate_colors_ = true;

bool SceneContainer::ik_mode_ = false;

unsigned int SceneContainer::render_options_ = 0;

SceneContainer::SceneContainer(){}

void SceneContainer::drawObjects() {
    for(unsigned int i = 0; i < objects_.size();i++){
        Joint *object = objects_.at(i);
        render_options_ = RENDER_SHADER;
        object->draw(0);
    }
}

void SceneContainer::drawObjectsNoShader() {
    for(unsigned int i = 0; i < objects_.size();i++){
        Joint *object = objects_.at(i);
        render_options_ = RENDER_NONE;
        object->draw(0);
    }
}

void SceneContainer::drawExtras()
{
    render_options_ = RENDER_NONE;
    for(unsigned int i = 0 ; i < extras_.size() ; i++ ) {
        Object3D* object = extras_.at(i);
        object->draw(0);
    }
//    for(unsigned int i = 0 ; i < ik_targets_.size() ; i++ ) {
//        Object3D* object = ik_targets_.at(i);
//        render_options_ = RENDER_NONE;
//        object->draw(0);
//        ik_targets_.at(i)->DrawGlobal();
//    }
}

void SceneContainer::drawParticles()
{
    render_options_ = RENDER_SHADER;
    ParticleSpawner::handleParticleSpawners();
}


Joint* SceneContainer::addObject(QString label, QString objfile, QString texfile, QVector3D position, QQuaternion rotation, Joint* parent, float length, QVector3D objPos, QQuaternion objRot){
    FileObj *newobj = new FileObj();
    newobj->setLabel( label.toStdString() );
    newobj->loadFile(objfile);
    newobj->loadTex(texfile);
    newobj->setNewPosition(qglviewer::Vec(objPos.x(),objPos.y(),objPos.z()));
    newobj->setNewOrientation(qglviewer::Quaternion(objRot.x(),objRot.y(),objRot.z(),objRot.scalar()));
    Joint *newjoint = new Joint(newobj);
    newjoint->length_ = length;
    //newjoint->SetLabel( ( label+" Joint" ).toStdString());
    newjoint->setNewPosition(qglviewer::Vec(position.x(),position.y(),position.z()));
    newjoint->setNewOrientation(qglviewer::Quaternion(rotation.x(),rotation.y(),rotation.z(),rotation.scalar()));
    if(parent == NULL){
        SceneContainer::objects_.push_back(newjoint);
    }else{
        parent->addChildJoint(newjoint);
    }
    ObjectAnimator *newanimator = new ObjectAnimator(newjoint);
    if(newjoint->parent()!=NULL){
        newanimator->setParent( newjoint->parent()->getAnimator() );
    }
    SceneContainer::animators_.push_back(newanimator);

    return newjoint;
}

void SceneContainer::updateCurrentScene(int scene) {
    clearScene();
    switch(scene){
    case 1:
        createArmScene();
        break;
    case 2:
        createMonkeyHeadScene();
        break;
    case 3:
        createMonkeySnakeScene();
        break;
    case 4:
        createRobotScene();
        break;
    default:
        break;
    }
}

int SceneContainer::start_frame() {
    return start_frame_;
}

int SceneContainer::end_frame() {
    return end_frame_;
}

int SceneContainer::current_frame() {
    return current_frame_;
}

void SceneContainer::goToNextFrame() {
    int frame = current_frame_;
    int next_frame = (frame+1)%(end_frame_+1);
    SceneContainer::setCurrentFrame(next_frame);
}

void SceneContainer::setFrameRange(int start, int end) {
    start_frame_    = start;
    end_frame_      = end;
    for(size_t i = 0 ; i < SceneContainer::animators_.size() ; i++ ) {
        ObjectAnimator* animator = SceneContainer::animators_.at(i);
        animator->setFrameRange(start,end);
    }
}

void SceneContainer::setCurrentFrame(int frame) {
    current_frame_ = frame;
    for(size_t i = 0 ; i < SceneContainer::animators_.size() ; i++ ) {
        ObjectAnimator* animator = SceneContainer::animators_.at(i);
        animator->setCurrentFrame(current_frame_);
    }
}

unsigned int SceneContainer::howManyObjects() {
    return objects_.size();
}

Joint* SceneContainer::objectAt(int i) {
    return objects_.at(i);
}

void SceneContainer::setAnimated(bool animate_position, bool animate_orientation) {
    animate_position_       = animate_position;
    animate_orientation_    = animate_orientation;
}

bool SceneContainer::animatePosition() {
    return animate_position_;
}

bool SceneContainer::animateOrientation() {
    return animate_orientation_;
}

void SceneContainer::setPositionInterpolationType(PosInterpolator::InterpolationType type) {
    for(size_t i = 0 ; i < SceneContainer::animators_.size() ; i++ ) {
        ObjectAnimator* animator = SceneContainer::animators_.at(i);
        animator->setPosInterpolationType(type);
    }
}

void SceneContainer::setOrientationInterpolationType(OriInterpolator::InterpolationType type) {
    for(size_t i = 0 ; i < SceneContainer::animators_.size() ; i++ ) {
        ObjectAnimator* animator = SceneContainer::animators_.at(i);
        animator->setOriInterpolationType(type);
    }
}

Object3D* SceneContainer::selectedObject() {
    return selected_object_;
}

void SceneContainer::setSelectedObject(Object3D* selected) {
    selected_object_ = selected;
}

void SceneContainer::setSelectedPosition(qglviewer::Vec new_position) {
    if(selected_object_ == NULL) return;

    selected_object_->setNewPosition(new_position);
}

void SceneContainer::setSelectedOrientation(qglviewer::Quaternion new_orientation) {
    if(selected_object_ == NULL) return;

    selected_object_->setNewOrientation(new_orientation);
}

void SceneContainer::displaySelectedPositionTrajectory(bool display) {
    if(selected_object_ == NULL) return;

    if(selected_object_->getAnimator()!=NULL) {

        if(display) {
            selected_object_->getAnimator()->calculateTrajectory(TrajectoryObject::kPosition);
            extras_.push_back(selected_object_->getAnimator()->getTrajectory());
        }else{
            extras_.clear();
        }
    }
}

void SceneContainer::displaySelectedOrientationTrajectory(bool display) {
    if(selected_object_ == NULL) return;

    if(selected_object_->getAnimator()!=NULL) {
        if(display) {
            selected_object_->getAnimator()->calculateTrajectory(TrajectoryObject::kOrientation);
            extras_.push_back(selected_object_->getAnimator()->getTrajectory());
        }else{
            extras_.clear();
        }
    }
}

void SceneContainer::addCurrentPositionKeyframe() {
    if(selected_object_ == NULL) return;

    if(selected_object_->getAnimator()==NULL) {
        ObjectAnimator* new_animator = new ObjectAnimator(selected_object_);
        animators_.push_back(new_animator);
    }
    selected_object_->getAnimator()->addKeyPosition(current_frame_,selected_object_->position());

}

void SceneContainer::addCurrentOrientationKeyframe() {
    if(selected_object_ == NULL) return;

    if(selected_object_->getAnimator()==NULL) {
        ObjectAnimator* new_animator = new ObjectAnimator(selected_object_);
        animators_.push_back(new_animator);
    }
    selected_object_->getAnimator()->addKeyOrientation(current_frame_,selected_object_->orientation());
}

void SceneContainer::removeCurrentPositionKeyframe() {
    if(selected_object_ == NULL) return;

    if(selected_object_->getAnimator()!=NULL) {
        selected_object_->getAnimator()->removeKeyPosition(current_frame_);
    }
}

void SceneContainer::removeCurrentOrientationKeyframe() {
    if(selected_object_ == NULL) return;

    if(selected_object_->getAnimator()!=NULL) {
        selected_object_->getAnimator()->removeKeyOrientation(current_frame_);
    }
}

bool SceneContainer::renderBox()
{
    return render_box_over_object_;
}

void SceneContainer::setRenderBox(bool box){
    render_box_over_object_ = box;
}

bool SceneContainer::drawBones()
{
    return draw_bones_;
}

void SceneContainer::setDrawBones(bool bones)
{
    draw_bones_ = bones;
}

//void SceneContainer::AddIKTarget(IKTarget *target)
//{
//    ik_targets_.push_back(target);
//}

bool SceneContainer::renderOptions(int options_mask)
{
    if( (options_mask & render_options_) > 0 )
        return true;
    else
        return false;
}

QString SceneContainer::genOutput()
{
    QString output;
    return "";
}

void SceneContainer::clearScene()
{
    ParticleSpawner::clearParticleSpawners();
    while(animators_.size()>0){
        delete animators_.back();
        animators_.pop_back();
    }
    while(objects_.size()>0){
        delete objects_.back();
        objects_.pop_back();
    }
}

bool SceneContainer::getDrawWithNames()
{
    return draw_with_names_;
}

void SceneContainer::setDrawWithNames(bool draw_with_names)
{
    draw_with_names_ = draw_with_names;
}

void SceneContainer::createMonkeyHeadScene(){
    //Face Scene
    Joint* face_head = addObject("Head",":/models/face_head.obj","://textures/wooden.png",QVector3D(0,4,0), QQuaternion(1,0,0,0), NULL);
    /*Joint* face_teeth_top =*/ addObject("Teeth Up",":/models/face_teeth_top.obj",":/textures/stache_teeth.png",QVector3D(0,-1.5,1), QQuaternion(1,0,0,0), face_head);
    Joint* face_teeth_bot = addObject("Teeth Down",":/models/face_teeth_bot.obj",":/textures/stache_teeth.png",QVector3D(0,-1.5,1), QQuaternion(1,0,0,0), face_head);
    Joint* face_eye_lid_back_left = addObject("Eye Lid Back Left",":/models/face_eye_lid_back.obj",":/textures/wooden.png",QVector3D(0.7,0.5,1.7), QQuaternion(1,0,0,0), face_head);
    Joint* face_eye_lid_back_right = addObject("Eye Lid Back Right",":/models/face_eye_lid_back.obj",":/textures/wooden.png",QVector3D(-0.7,0.5,1.7), QQuaternion(1,0,0,0), face_head);
    /*Joint* face_eye_left =*/ addObject("Eye Left",":/models/face_eye.obj",":/textures/eye.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_left);
    /*Joint* face_eye_right =*/ addObject("Eye Right",":/models/face_eye.obj",":/textures/eye.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_right);
    Joint* face_eye_lid_top_left = addObject("Eye Lid Top Left",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_left);
    Joint* face_eye_lid_top_right = addObject("Eye Lid Top Right",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_right);
    Joint* face_eye_lid_bot_left = addObject("Eye Lid Bot Left",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(cos(M_PI_4),sin(M_PI_4),0,0), face_eye_lid_back_left);
    Joint* face_eye_lid_bot_right = addObject("Eye Lid Bot Right",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(cos(M_PI_4),sin(M_PI_4),0,0), face_eye_lid_back_right);
    /*Joint* face_brow_left =*/ addObject("Eye Brow Left",":/models/face_brow.obj",":/textures/wooden.png",QVector3D(0.3,1,0), QQuaternion(1,0,0,0), face_eye_lid_back_left);
    /*Joint* face_brow_right =*/ addObject("Eye Brow Right",":/models/face_brow.obj",":/textures/wooden.png",QVector3D(-0.3,1,0), QQuaternion(1,0,0,0), face_eye_lid_back_right);
    /*Joint* face_stache =*/ addObject("Stache",":/models/face_stache.obj",":/textures/stache_teeth.png",QVector3D(0,-0.5,2), QQuaternion(1,0,0,0), face_head);
    /*Joint* face_hat =*/ addObject("Hat",":/models/face_hat.obj",":/textures/wooden.png",QVector3D(0,1.4,0), QQuaternion(1,0,0,0), face_head);

    ObjectAnimator *animate;

    animate = face_teeth_bot->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(50, 0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_top_left->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, -0.5,0,0);
    animate->addKeyOrientation(50, 0,0,0);
    animate->addKeyOrientation(75, -0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_bot_left->getAnimator();
    animate->addKeyOrientation(0, 1.6,0,0);
    animate->addKeyOrientation(25, 2.1,0,0);
    animate->addKeyOrientation(50, 1.6,0,0);
    animate->addKeyOrientation(75, 2.1,0,0);
    animate->addKeyOrientation(100, 1.6,0,0);

    animate = face_eye_lid_top_right->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, -0.5,0,0);
    animate->addKeyOrientation(50, 0,0,0);
    animate->addKeyOrientation(75, -0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_bot_right->getAnimator();
    animate->addKeyOrientation(0, 1.6,0,0);
    animate->addKeyOrientation(25, 2.1,0,0);
    animate->addKeyOrientation(50, 1.6,0,0);
    animate->addKeyOrientation(75, 2.1,0,0);
    animate->addKeyOrientation(100, 1.6,0,0);

    viewer_reference_->camera()->fitSphere(qglviewer::Vec(0,5,0),6);
}

void SceneContainer::createArmScene(){
    //Hand Scene
    Joint* arm_base = addObject("Arm Base",":/models/arm_base.obj",":/textures/wooden.png",QVector3D(0,0.75,0), QQuaternion(1,0,0,0), NULL,0.25);
    Joint* arm_part1 = addObject("Part 1",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(0,0.25,0),QQuaternion(1,0,0,0),arm_base, 3.7);
    Joint* arm_part2 = addObject("Part 2",":/models/arm_part2.obj",":/textures/wooden.png",QVector3D(0,3.7,0),QQuaternion(1,0,0,0),arm_part1, 4.7);
    Joint* arm_hand = addObject("Hand",":/models/arm_hand.obj",":/textures/wooden.png",QVector3D(0,4.7,0),QQuaternion(1,0,0,0),arm_part2, 1.4f);
    ParticleSpawner *ps = new ParticleSpawner(0, QVector3D(0,1.4,0), QColor(255,255,0,100),arm_hand,1);

    Joint* arm_finger_5_1 = addObject("Finger 5 Part 1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0.5,1.4,0),QQuaternion(1,0,0,0),arm_hand);
    Joint* arm_finger_4_1 = addObject("Finger 4 Part 1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0.2,1.5,0),QQuaternion(1,0,0,0),arm_hand);
    Joint* arm_finger_3_1 = addObject("Finger 3 Part 1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.1,1.6,0),QQuaternion(1,0,0,0),arm_hand);
    Joint* arm_finger_2_1 = addObject("Finger 2 Part 1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.4,1.5,0),QQuaternion(1,0,0,0),arm_hand);
    Joint* arm_finger_1_1 = addObject("Finger 1 Part 1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.6,1,0),QQuaternion(1,0,0,0),arm_hand);

    /*Joint* arm_finger_5_2 =*/ addObject("Finger 5 Part 2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),arm_finger_5_1);
    /*Joint* arm_finger_4_2 =*/ addObject("Finger 4 Part 2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),arm_finger_4_1);
    /*Joint* arm_finger_3_2 =*/ addObject("Finger 3 Part 2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),arm_finger_3_1);
    /*Joint* arm_finger_2_2 =*/ addObject("Finger 2 Part 2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),arm_finger_2_1);
    /*Joint* arm_finger_1_2 =*/ addObject("Finger 1 Part 2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),arm_finger_1_1);

    ObjectAnimator *animate;

    animate = arm_base->getAnimator();

    animate->addKeyPosition(0,qglviewer::Vec(0,0,0));
    animate->addKeyPosition(25,qglviewer::Vec(2,2,3));
    animate->addKeyPosition(50,qglviewer::Vec(4,0,5));
    animate->addKeyPosition(75,qglviewer::Vec(6,2,3));
    animate->addKeyPosition(100,qglviewer::Vec(8,0,0));

    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_part1->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_part2->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_hand->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_finger_1_1->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_finger_2_1->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_finger_3_1->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_finger_4_1->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_finger_5_1->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    viewer_reference_->camera()->fitSphere(qglviewer::Vec(0,5,0),8);
}
void SceneContainer::createMonkeySnakeScene(){
    Joint* arm_base = addObject("Arm Base",":/models/arm_base.obj",":/textures/wooden.png",QVector3D(0,0.75,0), QQuaternion(1,0,0,0), NULL);
    Joint* arm_part1 = addObject("Part 1",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(0,0.25,0),QQuaternion(1,0,0,0),arm_base);
    Joint* arm_part2 = addObject("Part 2",":/models/arm_part2.obj",":/textures/wooden.png",QVector3D(0,3.7,0),QQuaternion(1,0,0,0),arm_part1);
    Joint* arm_hand = addObject("Hand",":/models/arm_hand.obj",":/textures/wooden.png",QVector3D(0,4.7,0),QQuaternion(1,0,0,0),arm_part2);

    ObjectAnimator *animate;

    animate = arm_base->getAnimator();

    animate->addKeyPosition(0,qglviewer::Vec(0,0,0));
    animate->addKeyPosition(25,qglviewer::Vec(2,2,3));
    animate->addKeyPosition(50,qglviewer::Vec(4,0,5));
    animate->addKeyPosition(75,qglviewer::Vec(6,2,3));
    animate->addKeyPosition(100,qglviewer::Vec(8,0,0));

    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_part1->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_part2->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    animate = arm_hand->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 90,0,0);
    animate->addKeyOrientation(50, 90,90,30);
    animate->addKeyOrientation(75, 90,90,90);
    animate->addKeyOrientation(100, 0,0,0);

    //Face Scene
    Joint* face_head = addObject("Head",":/models/face_head.obj","://textures/wooden.png",QVector3D(0,2,0), QQuaternion(1,0,0,0), arm_hand);
    /*Joint* face_teeth_top =*/ addObject("Teeth Up",":/models/face_teeth_top.obj",":/textures/stache_teeth.png",QVector3D(0,-1.5,1), QQuaternion(1,0,0,0), face_head);
    Joint* face_teeth_bot = addObject("Teeth Down",":/models/face_teeth_bot.obj",":/textures/stache_teeth.png",QVector3D(0,-1.5,1), QQuaternion(1,0,0,0), face_head);
    Joint* face_eye_lid_back_left = addObject("Eye Lid Back Left",":/models/face_eye_lid_back.obj",":/textures/wooden.png",QVector3D(0.7,0.5,1.7), QQuaternion(1,0,0,0), face_head);
    Joint* face_eye_lid_back_right = addObject("Eye Lid Back Right",":/models/face_eye_lid_back.obj",":/textures/wooden.png",QVector3D(-0.7,0.5,1.7), QQuaternion(1,0,0,0), face_head);
    /*Joint* face_eye_left =*/ addObject("Eye Left",":/models/face_eye.obj",":/textures/eye.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_left);
    /*Joint* face_eye_right =*/ addObject("Eye Right",":/models/face_eye.obj",":/textures/eye.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_right);
    Joint* face_eye_lid_top_left = addObject("Eye Lid Top Left",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_left);
    Joint* face_eye_lid_top_right = addObject("Eye Lid Top Right",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_right);
    Joint* face_eye_lid_bot_left = addObject("Eye Lid Bot Left",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(cos(M_PI_4),sin(M_PI_4),0,0), face_eye_lid_back_left);
    Joint* face_eye_lid_bot_right = addObject("Eye Lid Bot Right",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(cos(M_PI_4),sin(M_PI_4),0,0), face_eye_lid_back_right);
    /*Joint* face_brow_left =*/ addObject("Eye Brow Left",":/models/face_brow.obj",":/textures/wooden.png",QVector3D(0.3,1,0), QQuaternion(1,0,0,0), face_eye_lid_back_left);
    /*Joint* face_brow_right =*/ addObject("Eye Brow Right",":/models/face_brow.obj",":/textures/wooden.png",QVector3D(-0.3,1,0), QQuaternion(1,0,0,0), face_eye_lid_back_right);
    /*Joint* face_stache =*/ addObject("Stache",":/models/face_stache.obj",":/textures/stache_teeth.png",QVector3D(0,-0.5,2), QQuaternion(1,0,0,0), face_head);
    Joint* face_hat = addObject("Hat",":/models/face_hat.obj",":/textures/wooden.png",QVector3D(0,1.4,0), QQuaternion(1,0,0,0), face_head);
    ParticleSpawner *ps = new ParticleSpawner(0,QVector3D(0,3,0), QColor(255,255,0,100),face_hat,0);

    animate = face_teeth_bot->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(50, 0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_top_left->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, -0.5,0,0);
    animate->addKeyOrientation(50, 0,0,0);
    animate->addKeyOrientation(75, -0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_bot_left->getAnimator();
    animate->addKeyOrientation(0, 1.6,0,0);
    animate->addKeyOrientation(25, 2.1,0,0);
    animate->addKeyOrientation(50, 1.6,0,0);
    animate->addKeyOrientation(75, 2.1,0,0);
    animate->addKeyOrientation(100, 1.6,0,0);

    animate = face_eye_lid_top_right->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, -0.5,0,0);
    animate->addKeyOrientation(50, 0,0,0);
    animate->addKeyOrientation(75, -0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_bot_right->getAnimator();
    animate->addKeyOrientation(0, 1.6,0,0);
    animate->addKeyOrientation(25, 2.1,0,0);
    animate->addKeyOrientation(50, 1.6,0,0);
    animate->addKeyOrientation(75, 2.1,0,0);
    animate->addKeyOrientation(100, 1.6,0,0);

    viewer_reference_->camera()->fitSphere(qglviewer::Vec(0,7,0),9);
}

void SceneContainer::createRobotScene(){

    //! Body
    Joint* body = addObject("body",":/models/body.obj",":/textures/flammable.jpg",QVector3D(0,10,0),QQuaternion(1,0,0,0),NULL);
    Joint* neck = addObject("neck",":/models/arm_hand.obj",":/textures/wooden.png",QVector3D(0,3.6,0),QQuaternion(1,0,0,0),body);
    /*Joint* tail =*/ addObject("Tail",":/models/tail.obj",":/textures/wooden.png",QVector3D(0,-1.05,-2),QQuaternion(1,0,0,0),body);

    //! Face
    Joint* face_head = addObject("face_head",":/models/face_head.obj","://textures/wooden.png",QVector3D(0,2,0), QQuaternion(1,0,0,0), neck);
    /*Joint* face_teeth_top =*/ addObject("face_teeth_top",":/models/face_teeth_top.obj",":/textures/stache_teeth.png",QVector3D(0,-1.5,1), QQuaternion(1,0,0,0), face_head);
    Joint* face_teeth_bot = addObject("face_teeth_bot",":/models/face_teeth_bot.obj",":/textures/stache_teeth.png",QVector3D(0,-1.5,1), QQuaternion(1,0,0,0), face_head);
    Joint* face_eye_lid_back_left = addObject("face_eye_lid_back_left",":/models/face_eye_lid_back.obj",":/textures/wooden.png",QVector3D(0.7,0.5,1.7), QQuaternion(1,0,0,0), face_head);
    Joint* face_eye_lid_back_right = addObject("face_eye_lid_back_right",":/models/face_eye_lid_back.obj",":/textures/wooden.png",QVector3D(-0.7,0.5,1.7), QQuaternion(1,0,0,0), face_head);
    Joint* face_eye_left = addObject("face_eye_left",":/models/face_eye.obj",":/textures/eye.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_left);
    Joint* face_eye_right = addObject("face_eye_right",":/models/face_eye.obj",":/textures/eye.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_lid_back_right);
    Joint* face_eye_lid_top_left = addObject("face_eye_lid_top_left",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_left);
    Joint* face_eye_lid_top_right = addObject("face_eye_lid_top_right",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(1,0,0,0), face_eye_right);
    Joint* face_eye_lid_bot_left = addObject("face_eye_lid_bot_left",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(cos(M_PI_4),sin(M_PI_4),0,0), face_eye_left);
    Joint* face_eye_lid_bot_right = addObject("face_eye_lid_bot_right",":/models/face_eye_lid_front.obj",":/textures/wooden.png",QVector3D(0,0,0), QQuaternion(cos(M_PI_4),sin(M_PI_4),0,0), face_eye_right);
    /*Joint* face_brow_left =*/ addObject("face_brow_left",":/models/face_brow.obj",":/textures/wooden.png",QVector3D(0.3,1,0), QQuaternion(1,0,0,0), face_eye_left);
    /*Joint* face_brow_right =*/ addObject("face_brow_right",":/models/face_brow.obj",":/textures/wooden.png",QVector3D(-0.3,1,0), QQuaternion(1,0,0,0), face_eye_right);
    /*Joint* face_stache =*/ addObject("face_stache",":/models/face_stache.obj",":/textures/stache_teeth.png",QVector3D(0,-0.5,2), QQuaternion(1,0,0,0), face_head);
    /*Joint* face_hat =*/ addObject("face_hat",":/models/face_hat.obj",":/textures/wooden.png",QVector3D(0,1.4,0), QQuaternion(1,0,0,0), face_head);

    //! Left Hand
    Joint* left_arm_part1 = addObject("left_arm_part1",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(2,2.4,0),QQuaternion(cos(-M_PI_4),0,0,sin(-M_PI_4)),body);
    Joint* left_arm_part2 = addObject("left_arm_part2",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(0,3.7,0),QQuaternion(1,0,0,0),left_arm_part1);
    Joint* left_arm_hand = addObject("left_arm_hand",":/models/arm_hand.obj",":/textures/wooden.png",QVector3D(0,3.7,0),QQuaternion(1,0,0,0),left_arm_part2);

    Joint* left_arm_finger_5_1 = addObject("left_arm_finger_5_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0.5,1.4,0),QQuaternion(1,0,0,0),left_arm_hand);
    Joint* left_arm_finger_4_1 = addObject("left_arm_finger_4_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0.2,1.5,0),QQuaternion(1,0,0,0),left_arm_hand);
    Joint* left_arm_finger_3_1 = addObject("left_arm_finger_3_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.1,1.6,0),QQuaternion(1,0,0,0),left_arm_hand);
    Joint* left_arm_finger_2_1 = addObject("left_arm_finger_2_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.4,1.5,0),QQuaternion(1,0,0,0),left_arm_hand);
    Joint* left_arm_finger_1_1 = addObject("left_arm_finger_1_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.6,1,0),QQuaternion(1,0,0,0),left_arm_hand);

    /*Joint* left_arm_finger_5_2 =*/ addObject("left_arm_finger_5_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),left_arm_finger_5_1);
    /*Joint* left_arm_finger_4_2 =*/ addObject("left_arm_finger_4_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),left_arm_finger_4_1);
    Joint* left_arm_finger_3_2 = addObject("left_arm_finger_3_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),left_arm_finger_3_1);
    /*Joint* left_arm_finger_2_2 =*/ addObject("left_arm_finger_2_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),left_arm_finger_2_1);
    /*Joint* left_arm_finger_1_2 =*/ addObject("left_arm_finger_1_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),left_arm_finger_1_1);
    ParticleSpawner *ps = new ParticleSpawner(0,QVector3D(0,0,0), QColor(255,255,0,100),left_arm_finger_3_2,2);

    //! Right Hand
    Joint* right_arm_part1 = addObject("right_arm_part1",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(-2,2.4,0),QQuaternion(cos(M_PI_4),0,0,sin(M_PI_4)),body);
    Joint* right_arm_part2 = addObject("right_arm_part2",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(0,3.7,0),QQuaternion(1,0,0,0),right_arm_part1);
    Joint* right_arm_hand = addObject("right_arm_hand",":/models/arm_hand.obj",":/textures/wooden.png",QVector3D(0,3.7,0),QQuaternion(cos(M_PI_2),0,sin(M_PI_2),0),right_arm_part2);

    Joint* right_arm_finger_5_1 = addObject("right_arm_finger_5_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0.5,1.4,0),QQuaternion(1,0,0,0),right_arm_hand);
    Joint* right_arm_finger_4_1 = addObject("right_arm_finger_4_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0.2,1.5,0),QQuaternion(1,0,0,0),right_arm_hand);
    Joint* right_arm_finger_3_1 = addObject("right_arm_finger_3_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.1,1.6,0),QQuaternion(1,0,0,0),right_arm_hand);
    Joint* right_arm_finger_2_1 = addObject("right_arm_finger_2_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.4,1.5,0),QQuaternion(1,0,0,0),right_arm_hand);
    Joint* right_arm_finger_1_1 = addObject("right_arm_finger_1_1",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(-0.6,1,0),QQuaternion(1,0,0,0),right_arm_hand);

    /*Joint* right_arm_finger_5_2 =*/ addObject("right_arm_finger_5_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),right_arm_finger_5_1);
    /*Joint* right_arm_finger_4_2 =*/ addObject("right_arm_finger_4_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),right_arm_finger_4_1);
    /*Joint* right_arm_finger_3_2 =*/ addObject("right_arm_finger_3_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),right_arm_finger_3_1);
    /*Joint* right_arm_finger_2_2 =*/ addObject("right_arm_finger_2_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),right_arm_finger_2_1);
    /*Joint* right_arm_finger_1_2 =*/ addObject("right_arm_finger_1_2",":/models/arm_finger.obj",":/textures/wooden.png",QVector3D(0,0.6,0),QQuaternion(1,0,0,0),right_arm_finger_1_1);

    //! Left Leg
    //Hand Scene
    Joint* arm_part2 = addObject("Part 2",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(-2,-1,0),QQuaternion(1,0,0,0), body, 1, QVector3D(0,-3.7,0));
    Joint* arm_part1 = addObject("Part 1",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(0,-3.7,0),QQuaternion(1,0,0,0), arm_part2, 1, QVector3D(0,-3.7,0));
    /*Joint* arm_base =*/ addObject("Arm Base",":/models/arm_base.obj",":/textures/wooden.png",QVector3D(0,-3.7,0), QQuaternion(1,0,0,0), arm_part1, 1, QVector3D(0,-0.4,0));

    //! Right Leg
    Joint* larm_part2 = addObject("Part 2",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(2,-1,0),QQuaternion(1,0,0,0), body, 1, QVector3D(0,-3.7,0));
    Joint* larm_part1 = addObject("Part 1",":/models/arm_part1.obj",":/textures/wooden.png",QVector3D(0,-3.7,0),QQuaternion(1,0,0,0), larm_part2, 1, QVector3D(0,-3.7,0));
    /*Joint* larm_base =*/ addObject("Arm Base",":/models/arm_base.obj",":/textures/wooden.png",QVector3D(0,-3.7,0), QQuaternion(1,0,0,0), larm_part1, 1, QVector3D(0,-0.4,0));

    ObjectAnimator *animate;

    animate = face_teeth_bot->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(25, 0.5,0,0);
    animate->addKeyOrientation(50, 0,0,0);
    animate->addKeyOrientation(75, 0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_top_left->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(50, -0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_bot_left->getAnimator();
    animate->addKeyOrientation(0, 1.6,0,0);
    animate->addKeyOrientation(50, 2.1,0,0);
    animate->addKeyOrientation(100, 1.6,0,0);

    animate = face_eye_lid_top_right->getAnimator();
    animate->addKeyOrientation(0, 0,0,0);
    animate->addKeyOrientation(50, -0.5,0,0);
    animate->addKeyOrientation(100, 0,0,0);

    animate = face_eye_lid_bot_right->getAnimator();
    animate->addKeyOrientation(0, 1.6,0,0);
    animate->addKeyOrientation(50, 2.1,0,0);
    animate->addKeyOrientation(100, 1.6,0,0);

    viewer_reference_->camera()->fitSphere(qglviewer::Vec(0,10,0),11);
}


bool SceneContainer::alternateColors()
{
    return alternate_colors_;
}

void SceneContainer::setAlternateColors(bool alternate){
    alternate_colors_ = alternate;
}


std::vector<ObjectAnimator *>* SceneContainer::getAnimators()
{
    return &animators_;
}


bool SceneContainer::ikMode()
{
    return ik_mode_;
}

void SceneContainer::setIKMode(bool ik)
{
    ik_mode_ = ik;
}


void SceneContainer::drawPost()
{
    ikTarget.drawGlobal();
}
