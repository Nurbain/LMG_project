#ifndef MODEL3D_H
#define MODEL3D_H

#include <iostream>
#include <vector>

// - GL
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AssetLoader.h"

using namespace std;
class Model3D{
public:
    AssetLoader* _Loader;
    vector<vector<glm::vec3>> vertices;
    vector<vector<glm::vec3>> normals;
    vector<vector<unsigned int>> indices;
    vector<vector<glm::vec2>> textures;
    vector<Texture> AllTexture;
    vector<GLuint> modelTexture;


    std::string path;
    vector<vector<glm::vec3>> OBBs;
    int nb_mesh;
    vector<glm::mat4> transform;

    vector<glm::vec3> aabb_min;
    vector<glm::vec3> aabb_max;

    int selectedModel;

    void setSelect(int n){
        selectedModel = n;
    }

    Model3D();
    void loadMesh(const std::string filename);
    void initialize(const std::vector<std::vector<glm::vec3>>& position,std::vector<std::vector<unsigned int> > &);
};


#endif
