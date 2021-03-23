#pragma once

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
	TopoPatch* readTopoPatch(const unsigned char* data, int offset);
	void readTextureChunkOffsets(World* world, const unsigned char* data, int dataLength, int worldTexOffsetsOffset, int texMinx, int texMiny, int texMaxx, int texMaxy);

};