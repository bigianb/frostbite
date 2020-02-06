#include "ModelWindow.h"
#include "SDL.h"
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "Logger.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <assert.h>
#include "AnimationProcessor.h"

#define check() {\
    GLenum error = glGetError(); \
    if (error != 0){ \
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Gl Error: %d\n", error); \
        assert(false); \
    } \
}

float cubeVertices[] = {
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f
            };

float cubeColors[] = {
            0.0f,  1.0f,  0.0f,  1.0f,
            0.0f,  1.0f,  0.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,
            1.0f,  0.0f,  0.0f,  1.0f,
            1.0f,  0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.0f,  1.0f,  1.0f
        };

GLuint cubeIndices[] = {
            0, 4, 5, 0, 5, 1,
            1, 5, 6, 1, 6, 2,
            2, 6, 7, 2, 7, 3,
            3, 7, 4, 3, 4, 0,
            4, 7, 6, 4, 6, 5,
            3, 0, 1, 3, 1, 2
            };

float cubeRotation = 0.0f;

void ModelWindow::drawCube()
{
    // Now render to the main frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    // Clear the background (not really necessary I suppose)
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    check();
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(cubeRotation, 1.0f, 1.0f, 1.0f);
    check();
    glFrontFace(GL_CW);
    check();
    glVertexPointer(3, GL_FLOAT, 0, cubeVertices);
    glColorPointer(4, GL_FLOAT, 0, cubeColors);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, cubeIndices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glLoadIdentity(); 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    check();
    glFlush();
    glFinish();
    check();
        
    SDL_GL_SwapWindow(window);

    cubeRotation -= 0.15f;
}

const char* fragmentShaderCode =
"#version 120\n"
"varying vec2 UV;\n"
"uniform sampler2D myTextureSampler;\n"
"void main(){\n"
"   gl_FragColor = texture2D( myTextureSampler, UV );\n"
"//   gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0);\n"
"}\n";

const char* vertexShaderCode = 
"#version 120\n"
"attribute vec3 vertexPosition_modelspace;\n"
"attribute vec2 vertexUV;\n"
"varying vec2 UV;\n"
"uniform mat4 MVP;\n"
"void main(){\n"
"    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);\n"
"    UV = vertexUV;\n"
"}";

GLuint LoadShaders()
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    glShaderSource(VertexShaderID, 1, &vertexShaderCode , NULL);
    glCompileShader(VertexShaderID);
    check();

    glShaderSource(FragmentShaderID, 1, &fragmentShaderCode , NULL);
    glCompileShader(FragmentShaderID);
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
            std::vector<char> ProgramErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", &ProgramErrorMessage[0]);
            Logger::getLogger()->log(&ProgramErrorMessage[0]);
    }
    check();
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    
    
    check();
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
            std::vector<char> ProgramErrorMessage(InfoLogLength+1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", &ProgramErrorMessage[0]);
            Logger::getLogger()->log(&ProgramErrorMessage[0]);
    }
    check();
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    check();
    return ProgramID;
}

void ModelWindow::drawMesh(Model* model, int frame)
{
    AnimationProcessor animationProcessor;   

    glColor3f(1.0f,1.0f,1.0f);
    // Now render to the main frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    // Clear the background (not really necessary I suppose)
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    check();
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    check();

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
	glm::mat4 Projection = glm::perspective(45.0f, (float)screen_width / screen_height, 0.1f, 1000.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
                                        glm::vec3(0,-100,50), // Camera is at (0,-100,50), in World Space
                                        glm::vec3(0,0,30), // and looks at
                                        glm::vec3(0,0,1)  // Head is up 
                            );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model      = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around


    GLuint MatrixID = glGetUniformLocation(shaderProgramID, "MVP");
    check();
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(shaderProgramID, "vertexPosition_modelspace");
    GLuint vertexUVID = glGetAttribLocation(shaderProgramID, "vertexUV");
    GLuint textureSamplerID  = glGetUniformLocation(shaderProgramID, "myTextureSampler");
    check();

    for (TexturedMesh* texturedMesh : model->texturedMeshList){

        Texture* texture = texturedMesh->texture;

        // Create one OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);
 
        glActiveTexture(GL_TEXTURE0);
        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureID);
 
        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, texture->widthPixels, texture->heightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        check();
        glUseProgram(shaderProgramID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniform1i(textureSamplerID, 0);    

        for (auto& mesh : *texturedMesh->meshList)
        {
            FloatVector* animatedPositions = animationProcessor.softwareSkin(mesh, model->animData, frame);

            GLuint vertexbuffer;
            glGenBuffers(1, &vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * 12, animatedPositions != nullptr ? animatedPositions : mesh->positions, GL_STATIC_DRAW);

            GLuint uvbuffer;
            glGenBuffers(1, &uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * 8, mesh->uvCoords, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(
                    vertexPosition_modelspaceID,  // The attribute we want to configure
                    3,                            // size
                    GL_FLOAT,                     // type
                    GL_FALSE,                     // normalized?
                    0,                            // stride
                    (void*)0                      // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glVertexAttribPointer(
                    vertexUVID,                   // The attribute we want to configure
                    2,                            // size : U+V => 2
                    GL_FLOAT,                     // type
                    GL_FALSE,                     // normalized?
                    0,                            // stride
                    (void*)0                      // array buffer offset
            );
            glVertexPointer(3, GL_FLOAT, 0, mesh->positions);

            glDrawElements(GL_TRIANGLES, mesh->triangleIndices.size(), GL_UNSIGNED_INT, mesh->triangleIndices.data());
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            glDeleteBuffers(1, &vertexbuffer);
            glDeleteBuffers(1, &uvbuffer);

            delete[] animatedPositions;
        }
        glDeleteTextures(1, &textureID);
    }    
    glLoadIdentity(); 

    glFlush();
    glFinish();
    check();
    SDL_GL_SwapWindow(window);

    cubeRotation -= 0.015f;
}

bool ModelWindow::init()
{
    bool success = true;

	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	/* Initialize SDL for video output */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to initialize SDL: %s\n", SDL_GetError());
		success = false;
	}
    if (success){
	    /* Create a 640x480 OpenGL screen */
	    window = SDL_CreateWindow( "Model Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL );
	    if ( !window ) {
		    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create OpenGL window: %s\n", SDL_GetError());
		    success = false;
	    }
    }
	if (success && !SDL_GL_CreateContext(window)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create OpenGL context: %s\n", SDL_GetError());
		success = false;
	}
    if (success){
        GLenum err = glewInit();
        if (GLEW_OK != err){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GLEW init failed\n");
		    success = false;
        }
    }
    if (success){
        glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT );

        glMatrixMode(GL_PROJECTION);
        shaderProgramID = LoadShaders();

        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS); 
    }
    return success;
}

void ModelWindow::exit()
{
    glDeleteProgram(shaderProgramID);
    SDL_Quit();
}

void ModelWindow::drawFrame(Model* model, int frame)
{
    if (nullptr == model){
        drawCube();
    } else {
        drawMesh(model, frame);
    }
}
