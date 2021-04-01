#pragma once

#include <memory>
#include "GameType.h"

class World;
class LmpRepository;
class TopoPatch;

class WorldReader
{
public:
	WorldReader(GameType gameType) : gameType(gameType) {}
	World* readWorld(LmpRepository* lmpRepository, const char* name);

private:
	GameType gameType;

	void decodeWorldFile(World* world, const unsigned char* data, int dataLength);
	void decodeTopography(World* world, const unsigned char* data, int dataLength);
	std::shared_ptr<TopoPatch> readTopoPatch(const unsigned char* data, int offset);
	void readTextureChunkOffsets(World* world, const unsigned char* data, int dataLength, int worldTexOffsetsOffset, int texMinx, int texMiny, int texMaxx, int texMaxy);
	void readElements(World* world, const unsigned char* data, int dataLength, int elementBase, int numElements, int texMinx, int texMiny);
};