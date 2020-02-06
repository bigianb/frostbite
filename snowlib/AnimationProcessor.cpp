#include "AnimationProcessor.h"
#include "Mesh.h"
#include "AnimData.h"
#include <glm/gtc/matrix_transform.hpp>

FloatVector* AnimationProcessor::softwareSkin(Mesh* mesh, AnimData* animData, int frame)
{
    FloatVector* animatedPositions;
    if (animData == nullptr || frame < 0 || mesh->vertexWeights.empty()){
        animatedPositions = nullptr;
    } else {
        auto vwIterator = mesh->vertexWeights.begin();
        VertexWeight* vw = &*vwIterator++;
        animatedPositions = new FloatVector[mesh->numVertices];
        for (int vnum=0; vnum<mesh->numVertices; ++vnum){
            if (vw->endVertex < vnum)
            {
                vw = &*vwIterator++;   
                
            }
            glm::vec4 vpos = glm::vec4(mesh->positions[vnum].x, mesh->positions[vnum].y, mesh->positions[vnum].z, 1.0f);
            int bone1No = vw->bone1;

            glm::vec3 bindingPos1 = animData->bindingPose[bone1No];
            const AnimPose& bone1Pose = animData->getFKPose(frame, bone1No);
            glm::vec3 joint1Pos = bone1Pose.Position;

            glm::vec3 inverseBindingPos1 = -bindingPos1;
            glm::mat4 m1 = glm::translate(glm::mat4(1.0f), inverseBindingPos1);
            m1 = glm::mat4_cast(bone1Pose.Rotation) * m1;
            glm::vec4 vpos1 = m1 * vpos;
            m1 = glm::translate(glm::mat4(1.0f), bone1Pose.Position);
            vpos1 = m1 * vpos1;

            if (vw->bone2 == 0xFF) {
                vpos = vpos1;
            } else {
                // multi-bone
                int bone2No = vw->bone2;
                glm::vec3 bindingPos2 = animData->bindingPose[bone2No];
                const AnimPose& bone2Pose = animData->getFKPose(frame, bone2No);
                glm::vec3 joint2Pos = bone2Pose.Position;              

                glm::vec3 inverseBindingPos2 = -bindingPos2;
                glm::mat4 m2 = glm::translate(glm::mat4(1.0f), inverseBindingPos2);
                m2 = glm::mat4_cast(bone2Pose.Rotation) * m2;
                glm::vec4 vpos2 = m2 * vpos;
                m2 = glm::translate(glm::mat4(1.0f), bone2Pose.Position);
                vpos2 = m2 * vpos2;
				if (vw->bone3 == 0xFF) {
					float boneSum = (float)(vw->boneWeight1 + vw->boneWeight2);
                    float bone1Coeff = vw->boneWeight1 / boneSum;
                    float bone2Coeff = vw->boneWeight2 / boneSum;

					vpos = vpos1 * bone1Coeff + vpos2 * bone2Coeff;
				} else {
					int bone3No = vw->bone3;
					glm::vec3 bindingPos3 = animData->bindingPose[bone3No];
					const AnimPose& bone3Pose = animData->getFKPose(frame, bone3No);
					glm::vec3 joint3Pos = bone3Pose.Position;              

					glm::vec3 inverseBindingPos3 = -bindingPos3;
					glm::mat4 m3 = glm::translate(glm::mat4(1.0f), inverseBindingPos3);
					m3 = glm::mat4_cast(bone3Pose.Rotation) * m3;
					glm::vec4 vpos3 = m3 * vpos;
					m3 = glm::translate(glm::mat4(1.0f), bone3Pose.Position);
					vpos3 = m3 * vpos3;
					if (vw->bone4 == 0xFF) {
                        float boneSum = (float)(vw->boneWeight1 + vw->boneWeight2 + vw->boneWeight3);
                        float bone1Coeff = vw->boneWeight1 / boneSum;
                        float bone2Coeff = vw->boneWeight2 / boneSum;
                        float bone3Coeff = vw->boneWeight3 / boneSum;
						vpos = vpos1 * bone1Coeff + vpos2 * bone2Coeff + vpos3 * bone3Coeff;
					} else {
						int bone4No = vw->bone4;
						glm::vec3 bindingPos4 = animData->bindingPose[bone4No];
						const AnimPose& bone4Pose = animData->getFKPose(frame, bone4No);
						glm::vec3 joint4Pos = bone4Pose.Position;              

						glm::vec3 inverseBindingPos4 = -bindingPos4;
						glm::mat4 m4 = glm::translate(glm::mat4(1.0f), inverseBindingPos4);
						m4 = glm::mat4_cast(bone4Pose.Rotation) * m4;
						glm::vec4 vpos4 = m4 * vpos;
						m4 = glm::translate(glm::mat4(1.0f), bone4Pose.Position);
						vpos4 = m4 * vpos4;

                        float boneSum = (float)(vw->boneWeight1 + vw->boneWeight2 + vw->boneWeight3 + vw->boneWeight4);
                        float bone1Coeff = vw->boneWeight1 / boneSum;
                        float bone2Coeff = vw->boneWeight2 / boneSum;
                        float bone3Coeff = vw->boneWeight3 / boneSum;
                        float bone4Coeff = vw->boneWeight4 / boneSum;
						vpos = vpos1 * bone1Coeff + vpos2 * bone2Coeff + vpos3 * bone3Coeff + vpos4 * bone4Coeff;
					}
				}
            }
            animatedPositions[vnum].x = vpos.x;
            animatedPositions[vnum].y = vpos.y;
            animatedPositions[vnum].z = vpos.z;
        }
    }
    return animatedPositions;
}
