#include "World.h"

World::World()
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
}

TopoPatch::TopoPatch(int width, int height)
{
	w = width;
	h = height;
	heights = new int[w * h];
}

TopoPatch::~TopoPatch()
{
	delete[] heights;
}
