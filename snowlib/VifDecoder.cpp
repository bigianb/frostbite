#include "VifDecoder.h"
#include "DataUtil.h"
#include "GIFTag.h"
#include "Logger.h"
#include "VertexDefs.h"

using namespace std;

struct VLoc
{	
	VLoc(int va, int vb, int vc) : v1(va), v2(vb), v3(vc){}
    int v1;
    int v2;
    int v3;
};

struct UV
{
	UV(short uu, short vv) : u(uu), v(vv){}

    short u;
    short v;
};

struct Chunk
{
    int mscalID;
    GIFTag* gifTag0;
    GIFTag* gifTag1;
    vector<ShortVector> vertices;
    vector<SByteVector> normals;
    vector<VLoc> vlocs;
    vector<UV> uvs;
    vector<VertexWeight> vertexWeights;
    unsigned short* extraVlocs;
	int numExtraVlocs;
    vector<GIFTag> DIRECTGifTags;
};

#define NOP_CMD    0x00
#define STCYCL_CMD 0x01
#define ITOP_CMD   0x04
#define STMOD_CMD  0x05
#define FLUSH_CMD  0x11
#define MSCAL_CMD  0x14
#define STMASK_CMD 0x20
#define DIRECT_CMD 0x50

vector<Chunk*>* readVerts(const unsigned char* data, int offset, int endOffset)
{
	auto chunks = new vector<Chunk*>();
	Chunk* currentChunk = new Chunk();
    Chunk* previousChunk = nullptr;
    while (offset < endOffset) {
		int vifCommand = data[offset + 3] & 0x7f;
        int numCommand = data[offset + 2];
        int immCommand = DataUtil::getLEShort(data, offset);
		switch (vifCommand) {
            case NOP_CMD:
			case STCYCL_CMD:
			case ITOP_CMD:
			case STMOD_CMD:
			case FLUSH_CMD:
                offset += 4;
                break;
            
            case MSCAL_CMD:
                if (immCommand != 66 && immCommand != 68 && immCommand != 70)
                {
					Logger::getLogger()->logFormat("**** Microcode %d not supported", immCommand);
                }
                currentChunk->mscalID = immCommand;
                chunks->push_back(currentChunk);
                previousChunk = currentChunk;
                currentChunk = new Chunk();

                offset += 4;
                break;

			case STMASK_CMD:
                offset += 8;
                break;

            case DIRECT_CMD:
 
                for (int i = 0; i < immCommand; i++)
                {
                    GIFTag gifTag;
                    gifTag.parse(data, offset + 4 + i*16);
					currentChunk->DIRECTGifTags.push_back(gifTag);
                }

                offset += 4;
                offset += immCommand * 16;
                break;

			default:
                if ((vifCommand & 0x60) == 0x60) {
                    // unpack command
                    bool mask = ((vifCommand & 0x10) == 0x10);
                    int vn = (vifCommand >> 2) & 3;
                    int vl = vifCommand & 3;
                    int addr = immCommand & 0x1ff;
                    bool flag = (immCommand & 0x8000) == 0x8000;
                    bool usn = (immCommand & 0x4000) == 0x4000;

                    offset += 4;
                    if (vn == 1 && vl == 1) {
                        // v2-16
                        // The UVs come after the MSCAL instruction.
                        if (previousChunk != nullptr) {
                            for (int uvnum = 0; uvnum < numCommand; ++uvnum) {
                                short u = DataUtil::getLEShort(data, offset);
                                short v = DataUtil::getLEShort(data, offset + 2);
                                previousChunk->uvs.push_back(UV(u, v));
                                offset += 4;
                            }
                        } else {
                            int numBytes = numCommand * 4;
                            offset += numBytes;
                        }
                    } else if (vn == 2 && vl == 1) {
                        // v3-16
                        // each vertex is 128 bits, so num is the number of vertices
                        for (int vnum = 0; vnum < numCommand; ++vnum) {
                            if (!usn) {
                                short x = DataUtil::getLEShort(data, offset);
                                short y = DataUtil::getLEShort(data, offset + 2);
                                short z = DataUtil::getLEShort(data, offset + 4);
                                offset += 6;

                                currentChunk->vertices.push_back(ShortVector(x, y, z));
                            } else {
                                int x = DataUtil::getLEUShort(data, offset);
                                int y = DataUtil::getLEUShort(data, offset + 2);
                                int z = DataUtil::getLEUShort(data, offset + 4);
                                offset += 6;

                                currentChunk->vlocs.push_back(VLoc(x,y,z));
                            }
                        }
                        offset = (offset + 3) & ~3;
                    } else if (vn == 2 && vl == 2) {
                        // v3-8
                        int idx = offset;
                        for (int vnum = 0; vnum < numCommand; ++vnum) {

                            char x = (char)data[idx++];
                            char y = (char)data[idx++];
                            char z = (char)data[idx++];
                            currentChunk->normals.push_back(SByteVector(x, y, z));
                        }
                        int numBytes = ((numCommand * 3) + 3) & ~3;
                        offset += numBytes;
                    } else if (vn == 3 && vl == 0) {
                        // v4-32
                        if (1 == numCommand) {
                            currentChunk->gifTag0 = new GIFTag();
                            currentChunk->gifTag0->parse(data, offset);
                        } else if (2 == numCommand) {
                            currentChunk->gifTag0 = new GIFTag();
                            currentChunk->gifTag0->parse(data, offset);
                            currentChunk->gifTag1 = new GIFTag();
                            currentChunk->gifTag1->parse(data, offset + 16);
                        } else {
							Logger::getLogger()->log("unknown number of gif commands.\n");
                        }
                        int numBytes = numCommand * 16;
                        offset += numBytes;
                    } else if (vn == 3 && vl == 1) {
                        // v4-16
                        int numShorts = numCommand * 4;
                        if (usn) {
                            currentChunk->extraVlocs = new unsigned short[numShorts];
							currentChunk->numExtraVlocs = numShorts;
                            for (int i = 0; i < numCommand; ++i) {
                                currentChunk->extraVlocs[i*4] = DataUtil::getLEUShort(data, offset + i * 8);
                                currentChunk->extraVlocs[i * 4 + 1] = DataUtil::getLEUShort(data, offset + i * 8 + 2);
                                currentChunk->extraVlocs[i * 4 + 2] = DataUtil::getLEUShort(data, offset + i * 8 + 4);
                                currentChunk->extraVlocs[i * 4 + 3] = DataUtil::getLEUShort(data, offset + i * 8 + 6);
                            }
                        } else {
                            Logger::getLogger()->log("Unsupported tag\n");
                        }
                        offset += numShorts * 2;
                    } else if (vn == 3 && vl == 2) {
                        // v4-8
                        int numBytes = numCommand * 4;
                        int curVertex=0;
                        for (int i = 0; i < numCommand; ++i) {
                            VertexWeight vw;
                            vw.startVertex = curVertex;
                            vw.bone1 = data[offset++] / 4;
                            vw.boneWeight1 = data[offset++];
                            vw.bone2 = data[offset++];
                            if (vw.bone2 == 0xFF) {
                                // Single bone                                       
                                vw.boneWeight2 = 0;
                                int count = data[offset++];
                                curVertex += count;
                            } else {
                                vw.bone2 /= 4;
                                vw.boneWeight2 = data[offset++];
                                ++curVertex;

                                if (vw.boneWeight1 + vw.boneWeight2 < 0xFF)
                                {
                                    ++i;
                                    vw.bone3 = data[offset++] / 4;
                                    vw.boneWeight3 = data[offset++];
                                    vw.bone4 = data[offset++];
                                    int bw4 = data[offset++];
                                    if (vw.bone4 != 0xFF)
                                    {
                                        vw.bone4 /= 4;
                                        vw.boneWeight4 = bw4;
                                    }
                                } else {
									vw.bone3 = 0xFF;
									vw.boneWeight3 = 0;
									vw.bone4 = 0xFF;
									vw.boneWeight4 = 0;
								}

                            }
                            vw.endVertex = curVertex - 1;
							currentChunk->vertexWeights.push_back(vw);
                        }
                                
                    } else {
                        offset = endOffset;
                    }
                } else {
                    Logger::getLogger()->logFormat("Unknown command: %d", vifCommand);
                    offset = endOffset;
                }
                break;
		}
	}
	return chunks;
}

VertexWeight emptyVertexWeight;

VertexWeight& FindVertexWeight(vector<VertexWeight>& weights, int vertexNum)
{
    for(auto& weight : weights) {
        if (vertexNum >= weight.startVertex && vertexNum <= weight.endVertex) {
            return weight;
        }
    }
	if (!weights.empty()) {
		Logger::getLogger()->log("Failed to find vertex weight\n");
    }
    return emptyVertexWeight;
}

Mesh* chunksToMesh(vector<Chunk*>* chunks)
{
    Mesh* mesh = new Mesh();
    int numVertices = 0;
    for (auto& chunk : *chunks) {
		numVertices += chunk->gifTag0->nloop;
    }

	// Make the assumption that this will be enough to deal with multiple uvs on the same vertex.
    mesh->positions = new FloatVector[numVertices*2];
    mesh->normals = new FloatVector[numVertices*2];
    mesh->uvCoords = new FloatPoint[numVertices*2];

    int vstart = 0;
    for (auto& chunk : *chunks) {
        if ((chunk->gifTag0->prim & 0x07) != 4) {
			Logger::getLogger()->log("Can only deal with tri strips");
        }
		int vnum=vstart;
        for (auto& vertex : chunk->vertices) {
			FloatVector& fv = mesh->positions[vnum++];
			fv.x = vertex.x / 16.0f;
			fv.y = vertex.y / 16.0f;
			fv.z = vertex.z / 16.0f;
        }
		vnum=vstart;
        for (auto& normal : chunk->normals) {
			FloatVector& meshNormal = mesh->normals[vnum++];
			meshNormal.x = normal.x / 127.0f;
			meshNormal.y = normal.y / 127.0f;
			meshNormal.z = normal.z / 127.0f;
        }
        for (auto& vw : chunk->vertexWeights) {
            VertexWeight vwAdjusted = vw;
            vwAdjusted.startVertex += vstart;
			const int numChunkVertices = chunk->vertices.size();
			if (vwAdjusted.endVertex >= numChunkVertices) {
                vwAdjusted.endVertex = numChunkVertices - 1;
            }
            vwAdjusted.endVertex += vstart;
            if (vw.startVertex <= (numChunkVertices-1)) {
				mesh->vertexWeights.push_back(vwAdjusted);
            }
        }
		int vstripLen = chunk->gifTag0->nloop;
        int* vstrip = new int[vstripLen];
        int regsPerVertex = chunk->gifTag0->nreg;
		int numVlocs = chunk->vlocs.size();
        int numVertsInChunk = chunk->vertices.size();
        for (int vlocIndx = 2; vlocIndx < numVlocs; ++vlocIndx) {
            int v = vlocIndx - 2;
            int stripIdx2 = (chunk->vlocs[vlocIndx].v2 & 0x1FF) / regsPerVertex;
            int stripIdx3 = (chunk->vlocs[vlocIndx].v3 & 0x1FF) / regsPerVertex;
            if (stripIdx3 < vstripLen && stripIdx2 < vstripLen) {
                vstrip[stripIdx3] = vstrip[stripIdx2] & 0x1FF;

                bool skip2 = (chunk->vlocs[vlocIndx].v3 & 0x8000) == 0x8000;
                if (skip2) {
                    vstrip[stripIdx3] |= 0x8000;
                }
            }
            int stripIdx = (chunk->vlocs[vlocIndx].v1 & 0x1FF) / regsPerVertex;
            bool skip = (chunk->vlocs[vlocIndx].v1 & 0x8000) == 0x8000;

            if (v < numVertsInChunk && stripIdx < vstripLen) {
                vstrip[stripIdx] = skip ? (v | 0x8000) : v;
            }
        }
        int numExtraVlocs = chunk->extraVlocs[0];
        for (int extraVloc = 0; extraVloc < numExtraVlocs; ++extraVloc) {
            int idx = extraVloc * 4 + 4;
            int stripIndxSrc = (chunk->extraVlocs[idx] & 0x1FF) / regsPerVertex;
            int stripIndxDest = (chunk->extraVlocs[idx + 1] & 0x1FF) / regsPerVertex; ;
            vstrip[stripIndxDest] = (chunk->extraVlocs[idx + 1] & 0x8000) | (vstrip[stripIndxSrc] & 0x1FF);

            stripIndxSrc = (chunk->extraVlocs[idx + 2] & 0x1FF) / regsPerVertex;
            stripIndxDest = (chunk->extraVlocs[idx + 3] & 0x1FF) / regsPerVertex; ;
            vstrip[stripIndxDest] = (chunk->extraVlocs[idx + 3] & 0x8000) | (vstrip[stripIndxSrc] & 0x1FF);
        }
        int triIdx = 0;
        for (int i = 2; i < vstripLen; ++i) {
            int vidx1 = vstart + (vstrip[i - 2] & 0xFF);
            int vidx2 = vstart + (vstrip[i - 1] & 0xFF);
            int vidx3 = vstart + (vstrip[i] & 0xFF);

            int uv1 = i - 2;
            int uv2 = i - 1;
            int uv3 = i;

            // Flip the faces (indices 1 and 2) to keep the winding rule consistent.
            if ((triIdx & 1) == 1) {
                int temp = uv1;
                uv1 = uv2;
                uv2 = temp;

                temp = vidx1;
                vidx1 = vidx2;
                vidx2 = temp;
            }

            if ((vstrip[i] & 0x8000) == 0) {
                float udiv = 16.0f;
                float vdiv = 16.0f;

                FloatPoint p1(chunk->uvs[uv1].u / udiv, chunk->uvs[uv1].v / vdiv);
                FloatPoint p2(chunk->uvs[uv2].u / udiv, chunk->uvs[uv2].v / vdiv);
                FloatPoint p3(chunk->uvs[uv3].u / udiv, chunk->uvs[uv3].v / vdiv);

                if (mesh->uvCoords[vidx1].isInitialised() && p1 != mesh->uvCoords[vidx1])
                {
                    // There is more than 1 uv assigment to this vertex, so we need to duplicate it.
					int originalVIdx = vidx1;
                    vidx1 = vstart + numVertsInChunk;
                    numVertsInChunk++;
                    mesh->positions[vidx1] = mesh->positions[originalVIdx];
					mesh->normals[vidx1] = mesh->normals[originalVIdx];
                    
                    VertexWeight& weight = FindVertexWeight(chunk->vertexWeights, originalVIdx - vstart);
                    if (weight.boneWeight1 > 0)
                    {
                        VertexWeight vw = weight;
                        vw.startVertex = vidx1;
                        vw.endVertex = vidx1;
						mesh->vertexWeights.push_back(vw);
                    }
                }
				if (mesh->uvCoords[vidx2].isInitialised() && p2 != mesh->uvCoords[vidx2])
                {
                    // There is more than 1 uv assigment to this vertex, so we need to duplicate it.
					int originalVIdx = vidx2;
                    vidx2 = vstart + numVertsInChunk;
                    numVertsInChunk++;
                    mesh->positions[vidx2] = mesh->positions[originalVIdx];
					mesh->normals[vidx2] = mesh->normals[originalVIdx];
                    
                    VertexWeight& weight = FindVertexWeight(chunk->vertexWeights, originalVIdx - vstart);
                    if (weight.boneWeight1 > 0)
                    {
                        VertexWeight vw = weight;
                        vw.startVertex = vidx2;
                        vw.endVertex = vidx2;
						mesh->vertexWeights.push_back(vw);
                    }
                }
				if (mesh->uvCoords[vidx3].isInitialised() && p3 != mesh->uvCoords[vidx3])
                {
                    // There is more than 1 uv assigment to this vertex, so we need to duplicate it.
					int originalVIdx = vidx3;
                    vidx3 = vstart + numVertsInChunk;
                    numVertsInChunk++;
                    mesh->positions[vidx3] = mesh->positions[originalVIdx];
					mesh->normals[vidx3] = mesh->normals[originalVIdx];
                    
                    VertexWeight& weight = FindVertexWeight(chunk->vertexWeights, originalVIdx - vstart);
                    if (weight.boneWeight1 > 0)
                    {
                        VertexWeight vw = weight;
                        vw.startVertex = vidx3;
                        vw.endVertex = vidx3;
						mesh->vertexWeights.push_back(vw);
                    }
                }

				mesh->uvCoords[vidx1] = p1;
                mesh->uvCoords[vidx2] = p2;
                mesh->uvCoords[vidx3] = p3;
                        
                // Double sided hack. Should fix this with normals really
                mesh->triangleIndices.push_back(vidx1);
                mesh->triangleIndices.push_back(vidx2);
                mesh->triangleIndices.push_back(vidx3);

                mesh->triangleIndices.push_back(vidx2);
                mesh->triangleIndices.push_back(vidx1);
                mesh->triangleIndices.push_back(vidx3);
            }
            ++triIdx;
        }
        vstart += numVertsInChunk;
    }
	FloatVector* positions = new FloatVector[vstart];
	memcpy(positions, mesh->positions, vstart * sizeof(FloatVector));
	delete[] mesh->positions;
	mesh->positions = positions;

	FloatVector* normals = new FloatVector[vstart];
	memcpy(normals, mesh->normals, vstart * sizeof(FloatVector));
	delete[] mesh->normals;
	mesh->normals = normals;

	FloatPoint* uvCoords = new FloatPoint[vstart];
	memcpy(uvCoords, mesh->uvCoords, vstart * sizeof(FloatPoint));
	delete[] mesh->uvCoords;
	mesh->uvCoords = uvCoords;

    mesh->numVertices = vstart;

    return mesh;
}


vector<Mesh*>* VifDecoder::decode(const unsigned char* data, int base_addr)
{
	vector<Mesh*>* meshes = new vector<Mesh*>();
	int numMeshes = data[0x12];
	int offset1 = DataUtil::getLEInt(data, 0x24);
    for (int meshNum = 0; meshNum < numMeshes; ++meshNum) {
        int offsetVerts = DataUtil::getLEInt(data, 0x28 + meshNum * 4) - base_addr;
        int offsetEndVerts = DataUtil::getLEInt(data, 0x2C + meshNum * 4)- base_addr;
        auto chunks = readVerts(data, offsetVerts, offsetEndVerts);
        Mesh* mesh = chunksToMesh(chunks);
        meshes->push_back(mesh);
    }
	return meshes;
}




