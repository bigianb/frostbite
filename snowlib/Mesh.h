#pragma once

#include "VertexDefs.h"
#include "VertexWeight.h"
#include <vector>

class Mesh
{
public:
	Mesh() {
		positions = normals = nullptr;
		uvCoords = nullptr;
		numVertices = 0;
	}
	~Mesh() {
		delete positions; positions = nullptr;
		delete normals; normals = nullptr;
		delete uvCoords; uvCoords = nullptr;
	}

	std::vector<int> triangleIndices;
	FloatVector* positions;
	FloatVector* normals;
	FloatPoint* uvCoords;

    int numVertices;

	std::vector<VertexWeight> vertexWeights;

	void adjustUVs(int textureWidth, int textureHeight);
};
