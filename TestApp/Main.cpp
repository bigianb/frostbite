#pragma once
#include "SDL.h"

#include "ModelWindow.h"
#include "windows.h"
#include "LmpRepository.h"
#include "GobFile.h"
#include "VifDecoder.h"
#include "TexDecoder.h"
#include "AnimDecoder.h"
#include "Model.h"
#include "Texture.h"
#include "AnimData.h"
#include "WorldReader.h"
#include "World.h"

LmpRepository* lmpRepository;

TexturedMesh* readTexturedMesh(string dataPath, const char* lmpName, const char* modelName)
{
	LmpFile* lmpFile = lmpRepository->getLmp(lmpName);
	LmpEntry* vifEntry = lmpFile->findEntry(modelName, ".vif");
    LmpEntry* texEntry = lmpFile->findEntry(modelName, ".tex");
    TexturedMesh* texturedMesh = new TexturedMesh();
    VifDecoder vifDecoder;
    TexDecoder texDecoder;
	texturedMesh->meshList = vifDecoder.decode(vifEntry->data,0);
    texturedMesh->texture = texDecoder.decode(texEntry->data, 0);
    
	for (auto& mesh : *texturedMesh->meshList){
		mesh->adjustUVs(texturedMesh->texture->widthPixels, texturedMesh->texture->heightPixels);
	}
    return texturedMesh;
}

Model* readModel(string dataPath, const char* lmpName, vector<string> meshNames, const char *anmName)
{
    Model* model = new Model();
    for (string meshName : meshNames){
        TexturedMesh* texturedMesh = readTexturedMesh(dataPath, lmpName, meshName.c_str());
        model->texturedMeshList.push_back(texturedMesh);
    }
	LmpFile* lmpFile = lmpRepository->getLmp(lmpName);

    LmpEntry* anmEntry = anmName == nullptr ? nullptr : lmpFile->findEntry(anmName, ".anm");
    
    if (anmEntry != nullptr){
        AnimDecoder animDecoder;
        model->animData = animDecoder.decode(anmEntry->data, anmEntry->length);
    } else {
        model->animData = nullptr;
    }
    return model;
}

int main(int argc, char **argv) {
	string dataPath = "D:\\emu\\bgda\\BG\\DATA\\";
	if (argc == 2){
		dataPath = argv[1];
	}
	lmpRepository = new LmpRepositoryImpl(dataPath, GameType::DARK_ALLIANCE);

//	GobFile cuttownGob = GobFile(dataPath + "CUTTOWN.GOB", GameType::DARK_ALLIANCE);
//	World* world = WorldReader().readWorld(&cuttownGob, "cuttown");

    const char* lmpName = "DWARF.LMP";
    vector<string> meshNames;
    meshNames.push_back("dwarf");
 //   meshNames.push_back("hair");

	Model* model = readModel(dataPath, lmpName, meshNames, "l_idlea");

	vector<string> animNames = lmpRepository->getLmp(lmpName)->findFilenamesByExtension(".anm");

    ModelWindow modelWindow;

    modelWindow.init();

	int maxFrame = model->animData->NumFrames;

	float targetFrameRate = 60.0f / 1000.0f;		// frames per millisecond
	unsigned int animStartClock = SDL_GetTicks();
    bool done=false;
    while (!done){
		unsigned int tick = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                done = true;
            } else if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.sym == SDLK_ESCAPE){
                    done = true;
                }
            }
        }
		unsigned int tock = SDL_GetTicks();
		int animMillis = tock - animStartClock;
		int frame = (int)(animMillis * targetFrameRate) % maxFrame;
        modelWindow.drawFrame(model, frame);
		
		unsigned int deltaclock = SDL_GetTicks() - tick;
		
		if ( deltaclock != 0 ){
			unsigned int currentFPS = 1000 / deltaclock;
			char fpsString[64];
			sprintf(fpsString, "FPS: %d\n", currentFPS);
			OutputDebugStringA(fpsString);
		}
		
    }

    modelWindow.exit();
	
	return 0;
}



