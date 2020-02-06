#pragma once

class World;
class LmpRepository;
class TopoPatch;

class WorldReader
{
public:
	World* readWorld(LmpRepository* lmpRepository, const char* name);

private:
	void decodeWorldFile(World* world, const unsigned char* data, int dataLength);
	void decodeTopography(World* world, const unsigned char* data, int dataLength);
	TopoPatch* readTopoPatch(const unsigned char* data, int offset);
};