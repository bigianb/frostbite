#include "WorldReader.h"
#include "World.h"
#include "LmpRepository.h"
#include "DataUtil.h"
#include "VifDecoder.h"

World* WorldReader::readWorld(LmpRepository* lmpRepository, const char* name)
{
	World* world = new World();
	std::string worldLmpName = std::string(name) + ".lmp";
	LmpFile* world_lmp = lmpRepository->getLmp(worldLmpName);
	LmpEntry* world_entry = world_lmp->findEntry(world_lmp->findFilenamesByExtension("world")[0].c_str());
	decodeWorldFile(world, world_entry->data, world_entry->length);
	return world;
}

void WorldReader::decodeWorldFile(World* world, const unsigned char* data, int dataLength)
{
	int numElements = DataUtil::getLEInt(data, 0);
	int offset4 = DataUtil::getLEInt(data, 4);

	decodeTopography(world, data, dataLength);

	int elementBase = DataUtil::getLEInt(data, 0x24);
	int i28 = DataUtil::getLEInt(data, 0x28);
	int i2C = DataUtil::getLEInt(data, 0x2C);

	int cols_38 = DataUtil::getLEInt(data, 0x30);
	int rows_38 = DataUtil::getLEInt(data, 0x34);
	int offset_38 = DataUtil::getLEInt(data, 0x38);

	int i3C = DataUtil::getLEInt(data, 0x3C);
	int i40 = DataUtil::getLEInt(data, 0x40);
	int i44 = DataUtil::getLEInt(data, 0x44);
	int i48 = DataUtil::getLEInt(data, 0x48);

	int offset4C = DataUtil::getLEInt(data, 0x4C);
	int len50 = DataUtil::getLEInt(data, 0x50);
	int offset54 = DataUtil::getLEInt(data, 0x54);

	int texMinxy = DataUtil::getLEInt(data, 0x58);
	int texMaxxy = DataUtil::getLEInt(data, 0x5C);

	int texMinx = texMinxy % 100;
	int texMiny = texMinxy / 100;
	int texMaxx = texMaxxy % 100;
	int texMaxy = texMaxxy / 100;

	int offset60 = DataUtil::getLEInt(data, 0x60);
	int textureArrayOffset = DataUtil::getLEInt(data, 0x64);
	int offset68 = DataUtil::getLEInt(data, 0x68);
	int worldTexOffsetsOffset = DataUtil::getLEInt(data, 0x6C);

	readTextureChunkOffsets(world, data, dataLength, worldTexOffsetsOffset, texMinx, texMiny, texMaxx+1, texMaxy);
	readElements(world, data, dataLength, elementBase, numElements, texMinx, texMiny);
}

void WorldReader::readElements(World* world, const unsigned char* data, int dataLength, int elementBase, int numElements, int texMinx, int texMiny)
{
	for (int idx = 0; idx < numElements; ++idx) {
		auto element = new WorldElement();
		int elementSize = 0x38;
		if (GameType::CHAMPIONS_RTA == gameType || GameType::JL_HEROES == gameType) {
			elementSize = 0x3C;
		}
		int elementOffset = elementBase + idx * elementSize;

		DataUtil reader(data, elementOffset);

		int vifDataOffset = reader.getLEInt();
		
		if (GameType::DARK_ALLIANCE == gameType) {
			int Tex2 = reader.getLEInt();
		}
		int vifLen = reader.getLEInt();
		float x1 = reader.getLEFloat();
		float y1 = reader.getLEFloat();
		float z1 = reader.getLEFloat();
		float x2 = reader.getLEFloat();
		float y2 = reader.getLEFloat();
		float z2 = reader.getLEFloat();

		element->boundingBox = FloatBox(x1, y1, z1, x2 - x1, y2 - y1, z2 - z1);

		int textureNum = reader.getLEInt() / 0x40;

		int texCellxy = reader.getLEUShort();
		int y = texCellxy / 100;
		int x = texCellxy % 100;
		if (GameType::CHAMPIONS_RTA == gameType || GameType::JL_HEROES == gameType) {
			x += texMinx;
			y += texMiny;
		}
		if (textureNum != 0) {
			// TODO
		}
		int texWidth = 100;
		int texHeight = 100;
		/*
		if (element.Texture != null)
		{
			texWidth = element.Texture.PixelWidth;
			texHeight = element.Texture.PixelHeight;
		}
		*/
		unsigned char nregs = data[vifDataOffset + 0x10];
		int vifStartOffset = (nregs + 2) * 0x10;
		int vifDataLength = vifLen * 0x10 - vifStartOffset;
		
		element->mesh.meshList = VifDecoder().decode(data, vifDataOffset + vifStartOffset);


		world->elements.push_back(element);
	}
}

void WorldReader::readTextureChunkOffsets(World* world, const unsigned char* data, int dataLength, int worldTexOffsetsOffset, int texMinx, int texMiny, int texMaxx, int texMaxy)
{
	for (int y = texMiny; y <= texMaxy; ++y)
	{
		for (int x = texMinx; x <= texMaxx; ++x)
		{
			int cellOffset = ((y - texMiny) * 100 + x - texMinx) * 8;
			// This test is needed to deal with town.world in BGDA which addresses textures outside of the maximum x range.
			if (dataLength >=  cellOffset + 4)
			{
				int addr;
				if (GameType::CHAMPIONS_RTA == gameType || GameType::JL_HEROES == gameType)
				{
					// TODO: Figure out what this should really be
					addr = 0x800;
				}
				else
				{
					addr = DataUtil::getLEInt(data, cellOffset);
				}
				world->setTextureChunkOffset(x, y, addr);
			}
		}
	}
}

void WorldReader::decodeTopography(World* world, const unsigned char* data, int dataLength)
{
	world->topoStartCol = DataUtil::getLEInt(data, 8);
	world->topoStartRow = DataUtil::getLEInt(data, 0x0C);

	world->numTopoCols = DataUtil::getLEInt(data, 0x10);
	world->numTopoRows = DataUtil::getLEInt(data, 0x14);
	int topoElementsPerCellOffset = DataUtil::getLEInt(data, 0x18);
	int numTopoElements = DataUtil::getLEInt(data, 0x1C);
	int topoArrayOffset = DataUtil::getLEInt(data, 0x20);

	// Allows us to quickly look up patches from the offsets stored in the file.
	std::unordered_map<int, std::shared_ptr<TopoPatch>> patchAddressMap;

	world->topoElements.resize(numTopoElements);
	for (int el = 0; el < numTopoElements; ++el){
		int topoElementOffset = topoArrayOffset + el * 0x1C;
		TopoElement* element = new TopoElement();
		element->llx = DataUtil::getLEShort(data, topoElementOffset);
		element->lly = DataUtil::getLEShort(data, topoElementOffset + 2);
		element->urx = DataUtil::getLEShort(data, topoElementOffset + 4);
		element->ury = DataUtil::getLEShort(data, topoElementOffset + 6);
		element->int8 = DataUtil::getLEInt(data, topoElementOffset + 8);

		int patchOffset = DataUtil::getLEInt(data, topoElementOffset + 0xc);
		if (patchAddressMap.find(patchOffset) == patchAddressMap.end()){
			patchAddressMap[patchOffset] = readTopoPatch(data, patchOffset);
		}
		element->patch = patchAddressMap[patchOffset];
		element->flags = DataUtil::getLEShort(data, topoElementOffset + 0x10);
		element->x0 = DataUtil::getLEShort(data, topoElementOffset + 0x12);
		element->y0 = DataUtil::getLEShort(data, topoElementOffset + 0x14);
		element->baseHeight = DataUtil::getLEShort(data, topoElementOffset + 0x16);
		element->cos_alpha = DataUtil::getLEShort(data, topoElementOffset + 0x18) / 32767.0;
		element->sin_alpha = DataUtil::getLEShort(data, topoElementOffset + 0x1A) / 32767.0;
		world->topoElements.push_back(element);
	}

	world->topoElementsPerCell.resize(world->numTopoCols * world->numTopoRows);

	int index = 0;
	for (int r = 0; r < world->numTopoRows; ++r){
		for (int c = 0; c < world->numTopoCols; ++c){
			int elementsInCellListOffset = DataUtil::getLEInt(data, topoElementsPerCellOffset + index*4);
			
			auto& elementList = world->topoElementsPerCell[index];

			int elementId = DataUtil::getLEShort(data, elementsInCellListOffset);
			while (elementId >= 0){
				elementsInCellListOffset += 2;
				elementList.push_back(world->topoElements[elementId]);
				elementId = DataUtil::getLEShort(data, elementsInCellListOffset);
			}
			
			index++;
		}
	}
}

std::shared_ptr<TopoPatch> WorldReader::readTopoPatch(const unsigned char* data, int offset)
{
	int w = DataUtil::getLEInt(data, offset + 8);
	int h = DataUtil::getLEInt(data, offset + 0x0c);
	auto patch = std::make_shared<TopoPatch>(w, h);
	patch->x0 = DataUtil::getLEInt(data, offset);
	patch->y0 = DataUtil::getLEInt(data, offset + 4);
	patch->minHeight = DataUtil::getLEShort(data, offset + 0x10);
	patch->maxHeight = DataUtil::getLEShort(data, offset + 0x12);
	int j = 0;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int i = y * w + x;
			patch->heights[j++] = DataUtil::getLEShort(data, offset + 0x14 + i * 2);
		}
	}
	return patch;
}