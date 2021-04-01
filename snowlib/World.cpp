#include "World.h"

World::World() : textureChunkOffsets(100*100, 0)
{
	topoStartCol = 0;
	topoStartRow = 0;
	numTopoCols = 0;
	numTopoRows = 0;
}

World::~World()
{
	for (auto& element : topoElements)
	{
		delete element;
	}
	for (auto& patch : topoPatches)
	{
		delete patch;
	}
	for (auto& element : elements)
	{
		delete element;
	}
}

TopoPatch::TopoPatch(int width, int height)
{
	minHeight = 0;
	maxHeight = 0;
	x0 = y0 = 0;
	w = width;
	h = height;
	heights = new int[w * h];
}

TopoPatch::~TopoPatch()
{
	delete[] heights; heights = nullptr;
}
