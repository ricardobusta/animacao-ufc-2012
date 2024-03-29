#include "timebar.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "Objects3D/object3d.h"
#include "Interpolation/objectanimator.h"
#include "Interpolation/animation_step.h"
#include "Utils/scenecontainer.h"

TimeBar::TimeBar(QWidget *parent) :
    QWidget(parent)
{
    //setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    number_of_frames_ = 101;
    current_frame_ = 0;
    step_width_ = 10;

    setNumberOfFrames(number_of_frames_);

    this->has_pos_frames_.resize(number_of_frames_);
    this->has_ori_frames_.resize(number_of_frames_);
}
void TimeBar::setNumberOfFrames(int total) {
    number_of_frames_ = total;

    //int const_height = 50;
    //this->setGeometry(0,0,(number_of_frames_)*(step_width_-1)+1,const_height);
    //this->setFixedSize((number_of_frames_)*(step_width_-1)+1,const_height);
    this->setMinimumWidth((number_of_frames_)*(step_width_-1)+1);
    if(current_frame_ >= number_of_frames_) {
        setCurrentFrame(number_of_frames_ -1);
        emit setSelectedFrame(current_frame_);
    }

}

void TimeBar::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);

    QColor frame_base_color = palette().background().color(); /*(255,255,255);*/
    QColor frame_alternate_color = palette().midlight().color();/*(230,230,230);*/
    QColor frame_border_color = palette().midlight().color().darker();/*(200,200,200);*/

    QColor keyframe_pos_color(0,200,255);
    QColor keyframe_ori_color(255,140,0);

    QColor border_color = palette().shadow().color().darker();
    QColor selection_color = palette().highlight().color(); /*(0,0,0);*/

    if(!this->isEnabled()){
        keyframe_pos_color = palette().background().color();
        keyframe_ori_color = palette().background().color();
        frame_alternate_color = frame_base_color;
        frame_border_color = frame_base_color.darker();
        selection_color = border_color;
    }

    //double step_width = ((double)this->width()) / ((double)number_of_frames_);
    //step_width_ =
    //step_width_ = step_width_ > 1 ? step_width_ : 1;
    int box_width = step_width_;
    int h_step = box_width-1;
    int v_step = height()-2;

    painter.setPen(Qt::SolidLine);
    painter.setPen(frame_border_color);

    //Light Frames
    painter.setBrush(frame_base_color);
    for(int i = 0 ; i < number_of_frames_ ; i+=2 ){
        painter.drawRect((h_step)*i,0,h_step,(v_step));
        //painter.drawRect((h_step)*i,(v_step/2),h_step,(v_step/2));
    }
    //Dark Frames
    painter.setBrush(frame_alternate_color);
    for(int i = 1 ; i < number_of_frames_ ; i+=2 ){
        painter.drawRect((h_step)*i,0,h_step,(v_step));
        //painter.drawRect((h_step)*i,(v_step/2),h_step,(v_step/2));
    }

    //Current Frame Border
    painter.setBrush(QBrush(selection_color/*,Qt::DiagCrossPattern*/));
    painter.setPen(selection_color);
    painter.drawRect((h_step)*current_frame_,0,h_step,v_step);

    //Other Objects Key Frames
    painter.setPen(frame_alternate_color);
    painter.setBrush(frame_base_color);
    for(int i=0; i< number_of_frames_; i++){
        if(has_pos_frames_.at(i)){
            painter.drawRect((h_step)*i+3,3,h_step-6,(v_step/2)-6);
        }
        if(has_ori_frames_.at(i)){
            painter.drawRect((h_step)*i+3,(v_step/2)+3,h_step-6,(v_step/2)-6);
        }
    }

    //Position KeyFrames
    painter.setPen(keyframe_pos_color.darker(150));
    painter.setBrush(keyframe_pos_color);
    for(int i=0;i<pos_key_frames_.size();i++){
        painter.drawRect((h_step)*pos_key_frames_.at(i)+3,3,h_step-6,(v_step/2)-6);
    }
    //Orientation Keyframes
    painter.setPen(keyframe_ori_color.darker(150));
    painter.setBrush(keyframe_ori_color);
    for(int i=0;i<ori_key_frames_.size();i++){
        painter.drawRect((h_step)*ori_key_frames_.at(i)+3,(v_step/2)+3,h_step-6,(v_step/2)-6);
    }

    //Widget Border
    painter.setBrush(Qt::NoBrush);
    painter.setPen(border_color);
    painter.drawRect(0,0,(number_of_frames_*(box_width-1)),v_step);

    painter.end();
}

void TimeBar::mousePressEvent ( QMouseEvent * event ) {
    if(rect().contains(event->pos())) {
        int select = event->pos().x() / (step_width_-1);
        if(select < number_of_frames_) {
            current_frame_ = select;
            update();
            emit setSelectedFrame(current_frame_);
        }
    }
}

void TimeBar::setCurrentFrame(int frame) {
    current_frame_ = frame < number_of_frames_ ? frame : number_of_frames_-1;
    update();
}


void TimeBar::setKeyFrames(Object3D *object)
{
    this->pos_key_frames_.clear();
    this->ori_key_frames_.clear();

    this->has_pos_frames_.clear();
    this->has_ori_frames_.clear();

    this->has_pos_frames_.resize(number_of_frames_);
    this->has_ori_frames_.resize(number_of_frames_);

    if(object == NULL or object->getAnimator()==NULL){ repaint(); return; }

    //std::vector<OrientationStep> *orivec = &object->GetAnimator()->GetKeyOrientations();
    //std::vector<PositionStep> *posvec = &object->GetAnimator()->GetKeyPositions();
    std::vector<ObjectAnimator*> *animators = SceneContainer::getAnimators();

    for(unsigned int i=0;i<animators->size();i++){
        ObjectAnimator *ani = animators->at(i);
        for(int j=0;j<ani->getKeyPositions()->size();j++){
            has_pos_frames_[ ani->getKeyPositions()->at(j).frame_ ] = true;
        }

        for(unsigned int j=0;j<ani->getKeyOrientations()->size();j++){
            has_ori_frames_[ ani->getKeyOrientations()->at(j).frame_ ] = true;
        }
    }

    for(unsigned int i=0;i<object->getAnimator()->getKeyOrientations()->size();i++){
        ori_key_frames_.push_back(object->getAnimator()->getKeyOrientations()->at(i).frame_);
    }
    for(unsigned int i=0;i<object->getAnimator()->getKeyPositions()->size();i++){
        pos_key_frames_.push_back(object->getAnimator()->getKeyPositions()->at(i).frame_);
    }

    /*pos_key_frames_.push_back(1);
    pos_key_frames_.push_back(2);
    pos_key_frames_.push_back(3);
    pos_key_frames_.push_back(4);

    ori_key_frames_.push_back(1);
    ori_key_frames_.push_back(2);
    ori_key_frames_.push_back(3);
    ori_key_frames_.push_back(4);*/

    repaint();
}
