#pragma once

#include "VertexDefs.h"
#include "VertexWeight.h"
#include <vector>

class Mesh
{
public:
	std::vector<int> triangleIndices;
	FloatVector* positions;
	FloatVector* normals;
	FloatPoint* uvCoords;

    int numVertices;

	std::vector<VertexWeight> vertexWeights;

	void adjustUVs(int textureWidth, int textureHeight);
};
