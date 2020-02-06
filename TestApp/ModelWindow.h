#pragma once

#include "GL/glew.h"

#include "SDL_opengl.h"
#include "SDL.h"

class Mesh;
class Model;

class ModelWindow
{
   uint32_t screen_width;
   uint32_t screen_height;

   SDL_Window *window;

   GLuint verbose;
   GLuint vshader;
   GLuint fshader;
   GLuint mshader;
   GLuint program;
   GLuint program2;
   GLuint tex_fb;
   GLuint tex;
   GLuint buf;
// julia attribs
   GLuint unif_color, attr_vertex, unif_scale, unif_offset, unif_tex, unif_centre; 
// mandelbrot attribs
   GLuint attr_vertex2, unif_scale2, unif_offset2, unif_centre2;

   GLuint shaderProgramID;

public:
    ModelWindow()
    {
        screen_width=640; screen_height=480;
    }

    bool init();

    void exit();

    void drawFrame(Model* model, int frame);
private:
    void drawCube();
    void drawMesh(Model* model, int frame);
};
