#ifndef HEIGTHMAP_H
#define HEIGTHMAP_H

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

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class HeigthMap{
public:
    // - mesh
    GLuint mHeigthMapVertexArray;
    GLuint mHeigthMapVertexBuffer;
    GLuint mHeigthMapIndexBuffer;
    GLuint mHeigthMapTextureCoordinateBuffer;

    // - shader
    GLuint mHeigthMapShaderProgram;
    // - texture
    GLuint texture;

    int numberOfVertices_;
    int numberOfIndices_;

    // - repository
    std::string ImgRepository;

    HeigthMap(){}

    // Methode d'initialisation
    bool initializeHeigthMap();
    bool initializeArrayBuffer();
    bool initializeVertexArray();
    bool initializeMaterial();
    bool initializeShaderProgram();
};

#endif
