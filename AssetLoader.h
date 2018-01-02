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
    bool mIsInitialized;
    aiScene* _scene;
    Assimp::Importer* _importer;
    AssetLoader():mIsInitialized(false),_scene(nullptr){
        _importer = new Assimp::Importer();
        _importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,0);
        _importer->GetErrorString();
    }
    ~AssetLoader(){
        delete _importer;
        _importer = nullptr;
    }
    bool import(const std::string filename);
    bool loadData(std::vector<std::vector<glm::vec3>>&,std::vector<std::vector<glm::vec3>>&,std::vector<std::vector<unsigned int> > &);
};





#endif
