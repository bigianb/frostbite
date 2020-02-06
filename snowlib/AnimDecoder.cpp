#include "AnimDecoder.h"
#include "AnimData.h"
#include "DataUtil.h"

AnimData* AnimDecoder::decode(const unsigned char* data, int length)
{
    // Decode BGDA
    int endIndex = length;
    AnimData* animData = new AnimData();
    animData->NumBones = DataUtil::getLEInt(data, 0);
    animData->Offset4Val = DataUtil::getLEInt(data, 4);
    animData->Offset14Val = DataUtil::getLEInt(data, 0x14);
    animData->Offset18Val = DataUtil::getLEInt(data, 0x18);
    int offset8Val = DataUtil::getLEInt(data, 8);

    int bindingPoseOffset = DataUtil::getLEInt(data, 0x0C);
    animData->bindingPose = new glm::vec3[animData->NumBones];
    for (int i = 0; i < animData->NumBones; ++i)
    {
        animData->bindingPose[i].x = -DataUtil::getLEShort(data, bindingPoseOffset + i * 8 + 0) / 64.0f;
        animData->bindingPose[i].y = -DataUtil::getLEShort(data, bindingPoseOffset + i * 8 + 2) / 64.0f;
        animData->bindingPose[i].z = -DataUtil::getLEShort(data, bindingPoseOffset + i * 8 + 4) / 64.0f;
    }

    // Skeleton structure
    int offset10Val = DataUtil::getLEInt(data, 0x10);
    animData->skeletonDef = new int[animData->NumBones];
    for (int i = 0; i < animData->NumBones; ++i)
    {
        animData->skeletonDef[i] = data[offset10Val + i];
    }

    AnimPose* curPose = new AnimPose[animData->NumBones];

    for (int boneNum = 0; boneNum < animData->NumBones; ++boneNum)
    {
        AnimPose& pose = curPose[boneNum];
        pose.BoneNum = boneNum;
        pose.FrameNum = 0;
        int frameOff = offset8Val + boneNum * 0x0e;

        pose.Position.x = DataUtil::getLEShort(data, frameOff) / 64.0f;
        pose.Position.y = DataUtil::getLEShort(data, frameOff + 2) / 64.0f;
        pose.Position.z = DataUtil::getLEShort(data, frameOff + 4) / 64.0f;

        pose.Rotation.w = DataUtil::getLEShort(data, frameOff + 6) / 4096.0f;
        pose.Rotation.x = DataUtil::getLEShort(data, frameOff + 8) / 4096.0f;
        pose.Rotation.y = DataUtil::getLEShort(data, frameOff + 0x0A) / 4096.0f;
        pose.Rotation.z = DataUtil::getLEShort(data, frameOff + 0x0C) / 4096.0f;

        pose.Velocity.x = pose.Velocity.y = pose.Velocity.z = 0.0f;
        pose.AngularVelocity.w = pose.AngularVelocity.x = pose.AngularVelocity.y = pose.AngularVelocity.z = 0.0f;

        // This may give us duplicate frame zero poses, but that's ok.
        animData->MeshPoses.push_back(pose);
    }
    int* curAngVelFrame = (int*)calloc(animData->NumBones, sizeof(int));
    int* curVelFrame = (int*)calloc(animData->NumBones, sizeof(int));

    animData->NumFrames = 1;

    int totalFrame = 0;
    int otherOff = offset8Val + animData->NumBones * 0x0e;

    AnimPose* pose = nullptr;
    while (otherOff < endIndex) {
        int count = data[otherOff++];
        unsigned char byte2 = data[otherOff++];
        int boneNum = byte2 & 0x3f;
        if (boneNum == 0x3f) break;

        totalFrame += count;

        if (pose == nullptr || pose->FrameNum != totalFrame || pose->BoneNum != boneNum)
        {
            if (pose != nullptr)
            {
                animData->MeshPoses.push_back(*pose);
            }
            delete pose;
            pose = new AnimPose();
            pose->FrameNum = totalFrame;
            pose->BoneNum = boneNum;
            pose->Position = curPose[boneNum].Position;
            pose->Rotation = curPose[boneNum].Rotation;
            pose->AngularVelocity = curPose[boneNum].AngularVelocity;
            pose->Velocity = curPose[boneNum].Velocity;
        }

        // bit 7 specifies whether to read 4 (set) or 3 elements following
        // bit 6 specifies whether they are shorts or bytes (set).
        if ((byte2 & 0x80) == 0x80) {
            int w, x, y, z;
            if ((byte2 & 0x40) == 0x40) {
                w = (char)data[otherOff++];
                x = (char)data[otherOff++];
                y = (char)data[otherOff++];
                z = (char)data[otherOff++];
            } else {
                w = DataUtil::getLEShort(data, otherOff);
                x = DataUtil::getLEShort(data, otherOff+2);
                y = DataUtil::getLEShort(data, otherOff+4);
                z = DataUtil::getLEShort(data, otherOff+6);
                otherOff += 8;
            }
            glm::quat angVel(w, x, y, z);

            glm::quat prevAngVel = pose->AngularVelocity;
            float coeff = (totalFrame - curAngVelFrame[boneNum]) / 131072.0f;
            glm::quat angDelta = prevAngVel * coeff;
            pose->Rotation = pose->Rotation + angDelta;
                    
            pose->FrameNum = totalFrame;
            pose->AngularVelocity = angVel;

            curPose[boneNum].Rotation = pose->Rotation;
            curPose[boneNum].AngularVelocity = pose->AngularVelocity;
            curAngVelFrame[boneNum] = totalFrame;
        }
        else
        {
            int x, y, z;
            if ((byte2 & 0x40) == 0x40) {
                x = (char)data[otherOff++];
                y = (char)data[otherOff++];
                z = (char)data[otherOff++];
            } else {
                x = DataUtil::getLEShort(data, otherOff);
                y = DataUtil::getLEShort(data, otherOff + 2);
                z = DataUtil::getLEShort(data, otherOff + 4);
                otherOff += 6;
            }
            glm::vec3 vel(x, y, z);
            glm::vec3 prevVel = pose->Velocity;
            float coeff = (totalFrame - curVelFrame[boneNum]) / 512.0f;
            glm::vec3 posDelta = prevVel * coeff;
            pose->Position = pose->Position + posDelta;                
            pose->FrameNum = totalFrame;
            pose->Velocity = vel;

            curPose[boneNum].Position = pose->Position;
            curPose[boneNum].Velocity = pose->Velocity;
            curVelFrame[boneNum] = totalFrame;
        }
    }
    animData->MeshPoses.push_back(*pose);
    delete pose;
    free(curAngVelFrame);
    free(curVelFrame);
    animData->NumFrames = totalFrame+1;
    animData->BuildPerFramePoses();
    animData->BuildPerFrameFKPoses();
    return animData;
}
