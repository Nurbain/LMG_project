#ifndef SKYBOX_H
#define SKYBOX_H

// STL
#include <iostream>
#include <vector>
#include <fstream>

// Graphics
// - GLEW (always before "gl.h")
#include <GL/glew.h>
// - GL
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
// - GLUT
#include <GL/glut.h>

//SOIL
#include "SOIL.h"

class SkyBox{
public:
    // - mesh
    GLuint mCubemapVertexArray;
    GLuint mCubemapVertexBuffer;
    GLuint mCubemapIndexBuffer;
    // - shader
    GLuint mCubeMapShaderProgram;
    // - texture
    GLuint texture;

    // - repository
    std::string ImgRepository;

    // - scale
    float scale;


    SkyBox();

    // Methode d'initialisation
    bool initializeCubemap();
    bool initializeCubemapGeometry();
    bool initializeCubemapTextures();
    bool initializeCubemapShader();
};

#endif
