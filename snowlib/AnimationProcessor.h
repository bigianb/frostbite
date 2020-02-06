#pragma once

struct FloatVector;
class Mesh;
class AnimData;

class AnimationProcessor
{
public:
    FloatVector* softwareSkin(Mesh* mesh, AnimData* animData, int frame);
};
