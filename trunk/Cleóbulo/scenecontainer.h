#ifndef SCENECONTAINER_H
#define SCENECONTAINER_H

#include <vector>

class Joint;
class ObjectAnimator;

class SceneContainer
{
public:
    SceneContainer();
    //! Scene definition
    static void CreateDefaultScene();



    //! Animation parameters
    static int start_frame();
    static int end_frame();
    static int current_frame();
    static void GoToNextFrame();
    static void SetFrameRange(int start, int end);
    static void SetCurrentFrame(int frame);

    //! Objects management
    static size_t HowManyObjects();
    static Joint* ObjectAt(size_t i);

private:
    static int start_frame_, end_frame_,current_frame_;
    static std::vector<ObjectAnimator*> animators_;
    static std::vector<Joint*> objects_;
};

#endif // SCENECONTAINER_H
