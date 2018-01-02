#ifndef MODEL3D_H
#define MODEL3D_H

#include <iostream>
#include <vector>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AssetLoader.h"

class Model3D{
public:
    AssimpLoader* _Loader;
    Model3D();
    void loadMesh(const std::string filename);
    void initialize(const std::vector<std::vector<glm::vec3>>& position,std::vector<std::vector<unsigned int> > &);
};


#endif
