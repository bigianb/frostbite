#pragma once

#include <vector>
#include <memory>
#include "VertexDefs.h"
#include "Model.h"

class TopoPatch
{
public:
	TopoPatch(int w, int h);
	~TopoPatch();

	int x0, y0, w, h;
	int minHeight, maxHeight;
	int* heights;
};

class TopoElement
{
public:
	int llx, lly, urx, ury;
	int int8;
	std::shared_ptr<TopoPatch> patch;
	int flags;
	int x0, y0;
	int baseHeight;
	double cos_alpha, sin_alpha;
};

class WorldElement
{
public:
	FloatBox boundingBox;
	TexturedMesh mesh;

	// The position before rotation
	FloatVector pos;

	bool usesRotFlags;
	int xyzRotFlags;

	double cosAlpha;
	double sinAlpha;
	// Whether we should flip the y axis (when not using rot flags)
	bool negYaxis;

	// Store info to access again
	int VifDataOffset;
	int VifDataLength;
};

class World
{
public:

	World();
	~World();

	// Topography
	//
	// The topography is a compressed height map for the world.
	// The world is broken down into a coarse grid of cells.
	// The topography is specified by a collection of elements.
	// A topography element positions and rotates a reusable topography patch.
	// A patch is a simple rectangular array of heights which can be used by
	// multiple topography elements.
	// In order to speed up the processing to derive a height of a given coordinate,
	// a list of all the topographical elements that intersect each cell is stored.

	/// The column that the 0th column in topoElementsPerCell represents
	int topoStartCol;
	/// The row that the 0th row in topoElementsPerCell represents
	int topoStartRow;
	/// The number of columns in the topoElementsPerCell vector
	int numTopoCols;
	/// The number of rows in the topoElementsPerCell vector
	int numTopoRows;
	/// The topo elements that intersect a coarse grid.
	std::vector<std::vector<TopoElement*>> topoElementsPerCell;

	// All of the topo elements. This collection owns the elements.
	std::vector<TopoElement*> topoElements;

	// All of the topo patches. This collection owns the patches.
	std::vector<TopoPatch*> topoPatches;

	// All of the elements. This collection owns the elements.
	std::vector<WorldElement*> elements;

	// The texture chunk offsets. 2D array with a 100 x 100 dimension.
	std::vector<int> textureChunkOffsets;

	void setTextureChunkOffset(int x, int y, int addr) { textureChunkOffsets.at(y * 100 + x) = addr; }
};
