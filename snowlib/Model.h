#pragma once
#include <vector>

#include "Texture.h"
#include "AnimData.h"
class Mesh;

class TexturedMesh
{
public:
    TexturedMesh() {
        meshList = nullptr;
        texture = nullptr;
    }
    ~TexturedMesh() {
        delete meshList; meshList = nullptr;
        delete texture; texture = nullptr;
    }
    std::vector<Mesh*>* meshList;
    Texture* texture;
};

class Model
{
public:
    Model() {
        animData = nullptr;
    }
    ~Model() {
        delete animData; animData = nullptr;
    }
    std::vector<TexturedMesh*> texturedMeshList;
    AnimData* animData;
};
