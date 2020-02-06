#include "Mesh.h"
#include "assert.h"

void Mesh::adjustUVs(int textureWidth, int textureHeight)
{
	for (int i=0; i<numVertices; ++i)
	{
		FloatPoint& uv = uvCoords[i];
		uv.x /= textureWidth;
		uv.y /= textureHeight;

		//uv.y = 1.0f - uv.y;

	}
}
