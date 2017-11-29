#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <iostream>
#include <vector>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

class AssetLoader{
public:
    aiScene* _scene;
    AssetLoader();
    void import(const std::string filename);
    void loadData(std::vector<std::vector<glm::Vec3>>&);
};





#endif
