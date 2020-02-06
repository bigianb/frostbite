#include "AnimData.h"
#include "glm/gtc/matrix_transform.hpp"

void AnimData::BuildPerFrameFKPoses()
{
    perFrameFKPoses = new AnimPose[NumFrames * NumBones];
    glm::vec3* parentPoints = new glm::vec3[64];
    glm::quat* parentRotations = new glm::quat[64];
    for (int idx = 0; idx < 64; ++idx) {
        parentPoints[idx] = glm::vec3(0.0f, 0.0f, 0.0f);
        parentRotations[idx] = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
    }
    for (int frame = 0; frame < NumFrames; ++frame) {
        for (int jointNum = 0; jointNum < NumBones; ++jointNum) {

            int parentIndex = skeletonDef[jointNum];
            const glm::vec3& parentPos = parentPoints[parentIndex];
            const glm::quat& parentRot = parentRotations[parentIndex];

            // The world position of the child joint is the local position of the child joint rotated by the
            // world rotation of the parent and then offset by the world position of the parent.
            const AnimPose& pose = perFramePoses[frame * NumBones + jointNum];

            glm::mat3 m = glm::mat3_cast(parentRot);
            glm::vec3 thisPos = m  * pose.Position;
            thisPos += parentPos;

            // The world rotation of the child joint is the world rotation of the parent rotated by the local rotation of the child.
            glm::quat thisRot = glm::normalize(parentRot * glm::normalize(pose.Rotation));
                    
            AnimPose fkPose;
            fkPose.Position = thisPos;
            fkPose.Rotation = thisRot;
            perFrameFKPoses[frame * NumBones + jointNum] = fkPose;

            parentPoints[parentIndex + 1] = fkPose.Position;
            parentRotations[parentIndex + 1] = fkPose.Rotation;
        }
    }
}

void AnimData::BuildPerFramePoses()
{
    perFramePoses = new AnimPose[NumFrames * NumBones];
    for (auto& pose : MeshPoses)
    {
        perFramePoses[pose.FrameNum * NumBones + pose.BoneNum] = pose;
    }
    for (int bone = 0; bone < NumBones; ++bone)
    {
        for (int frame = 1; frame < NumFrames; ++frame)
        {
            const AnimPose& prevPose = perFramePoses[(frame-1) * NumBones + bone];
            if (perFramePoses[frame * NumBones + bone].BoneNum < 0)
            {
                int frameDiff = frame - prevPose.FrameNum;
                float avCoeff = frameDiff / 131072.0f;
                glm::quat rotDelta = prevPose.AngularVelocity * avCoeff;

                float velCoeff = frameDiff / 512.0f;
                glm::vec3 posDelta = prevPose.Velocity * velCoeff;

                AnimPose& pose = perFramePoses[frame * NumBones + bone];
                pose.BoneNum = bone;
                pose.FrameNum = frame;
                pose.Position = prevPose.Position + posDelta;
				pose.Rotation = glm::normalize(prevPose.Rotation + rotDelta);
                pose.AngularVelocity = prevPose.AngularVelocity;
                pose.Velocity = prevPose.Velocity;
            }            
        }
    }
}