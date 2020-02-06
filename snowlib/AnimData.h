#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct AnimPose
{
    AnimPose()
    {
        // Signal uninitialised
        BoneNum = -1;
        FrameNum = -1;
    }

    glm::vec3 Position;
    glm::quat Rotation;

    glm::quat AngularVelocity;
    glm::vec3 Velocity;

    int BoneNum;
    int FrameNum;
};


class AnimData
{
public:
    int NumBones;
    int NumFrames;
    int Offset4Val;
    int Offset14Val;
    int Offset18Val;     // These are 4 bytes which are all ored together

    int* skeletonDef;

    glm::vec3* bindingPose;

    std::string Other;

    std::vector<AnimPose> MeshPoses;

    // With forward kinematics applied, Array numFrames * numBones
    AnimPose* perFrameFKPoses;

    AnimPose& getFKPose(int frame, int bone)
    {
        return perFrameFKPoses[frame * NumBones + bone];
    }

    void BuildPerFrameFKPoses();
    void BuildPerFramePoses();

private:
    // Array numFrames * numBones
    AnimPose* perFramePoses;
};

