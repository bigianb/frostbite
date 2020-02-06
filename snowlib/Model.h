#pragma once
#include <vector>

class Texture;
class AnimData;

class TexturedMesh
{
public:
    std::vector<Mesh*>* meshList;
    Texture* texture;
};

class Model
{
public:
    std::vector<TexturedMesh*> texturedMeshList;
    AnimData* animData;
};
