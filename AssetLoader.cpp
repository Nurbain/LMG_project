#include "AssetLoader.h"

#include <fstream>
#include <iostream>
#include <cstdio>
#include <stdexcept>


using namespace std;
bool AssetLoader::import(const std::string filename){
    std::ifstream fin(filename);
    if(!fin.fail()){
        fin.close();
    }else{
        this->_importer->GetErrorString();
        //std::cout << err_Importer << std::endl;
        std::cout << "erreur lecture du fichier "<< strerror(errno) << std::endl;
        return false;
    }

    try{
        _scene = const_cast<aiScene*>(_importer->ReadFile(filename,aiProcessPreset_TargetRealtime_Quality));
    }catch(std::runtime_error& err){
        std::cout << err.what();
        return false;
    }

    if(!_scene){
        std::cout << _importer->GetErrorString() << std::endl;
        return false;
    }
    return true;
}

bool AssetLoader::loadData(vector<vector<glm::vec3>>& pVertices,vector<vector<glm::vec3>>& pNormales,vector<vector<unsigned int>>& pIndices){
    if(!_scene->HasMeshes()){
        return false;
    }

    pVertices.resize(_scene->mNumMeshes);
    pNormales.resize(_scene->mNumMeshes);
    pIndices.resize(_scene->mNumMeshes);

    for(unsigned int m=0;m < _scene->mNumMeshes;++m){
        vector<glm::vec3>& vertices = pVertices[m];
        vector<glm::vec3>& normals = pNormales[m];
        vector<unsigned int>& indices = pIndices[m];

        const aiMesh* mesh = _scene->mMeshes[m];
        if(mesh->HasPositions()){
            vertices.resize(mesh->mNumVertices);
            for(unsigned int v=0;v<mesh->mNumVertices;++v){
                const aiVector3D& vertex = mesh->mVertices[v];
                vertices[v] = glm::vec3(vertex.x,vertex.y,vertex.z);
            }
            indices.resize(mesh->mNumFaces * 3);
            int k=0;
            for(unsigned int f=0;f< mesh->mNumFaces;++f){
                const struct aiFace& face = mesh->mFaces[f];
                assert(face.mNumIndices == 3);
                indices[k++] = face.mIndices[0];
                indices[k++] = face.mIndices[1];
                indices[k++] = face.mIndices[2];
            }
            if(mesh->HasNormals()){
                normals.resize(mesh->mNumVertices);
                for(unsigned int n=0;n<mesh->mNumVertices;++n){
                    const aiVector3D& normal = mesh->mNormals[n];
                    normals[n] = glm::vec3(normal.x,normal.y,normal.z);
                }
            }
        }

    }
    return true;
}
