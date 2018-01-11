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

bool AssetLoader::loadData(vector<vector<glm::vec3>>& pVertices, vector<vector<glm::vec3>>& pNormales, vector<vector<unsigned int>>& pIndices, std::vector<std::vector<glm::vec2> > & pTextures, std::vector<Texture>& AllTexture){
    if(!_scene->HasMeshes()){
        return false;
    }

    pVertices.resize(_scene->mNumMeshes);
    pNormales.resize(_scene->mNumMeshes);
    pIndices.resize(_scene->mNumMeshes);
    pTextures.resize(_scene->mNumMeshes);

    for(unsigned int m=0;m < _scene->mNumMeshes;++m){
        vector<glm::vec3>& vertices = pVertices[m];
        vector<glm::vec3>& normals = pNormales[m];
        vector<unsigned int>& indices = pIndices[m];
        vector<glm::vec2>& textures = pTextures[m];

        const aiMesh* mesh = _scene->mMeshes[m];
        if(mesh->HasPositions()){
            vertices.resize(mesh->mNumVertices);
            textures.resize(mesh->mNumVertices);
            for(unsigned int v=0;v<mesh->mNumVertices;++v){
                const aiVector3D& vertex = mesh->mVertices[v];
                vertices[v] = glm::vec3(vertex.x,vertex.y,vertex.z);

                //Texture Stuff
                if(mesh->mTextureCoords[0]){
                    const aiVector3D& tex = mesh->mTextureCoords[0][v];
                    textures[v] = glm::vec2(tex.x,tex.y);
                }else
                    textures[v] = glm::vec2(0.0f,0.0f);
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



        //Get All material Stuff
        aiMaterial* material = _scene->mMaterials[mesh->mMaterialIndex];
        aiColor4D col;
        glm::vec3 modelMeshColor;
        aiGetMaterialColor(material,AI_MATKEY_COLOR_DIFFUSE,&col);
        modelMeshColor = glm::vec3(col.r,col.g,col.b);

        std::vector<Texture> textures_diffuse = getMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
        AllTexture.insert(AllTexture.end(),textures_diffuse.begin(),textures_diffuse.end());

        std::vector<Texture> textures_specular = getMaterialTextures(material, aiTextureType_SPECULAR, "specular");
        AllTexture.insert(AllTexture.end(), textures_specular.begin(), textures_specular.end());

        std::vector<Texture> textures_ambient = getMaterialTextures(material, aiTextureType_AMBIENT, "ambient");
        AllTexture.insert(AllTexture.end(), textures_ambient.begin(), textures_ambient.end());

    }


    return true;
}


vector<Texture> AssetLoader::getMaterialTextures(aiMaterial *material, aiTextureType type, std::string name)
{
    vector<Texture> textures;
    aiString str;

    for(unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        material->GetTexture(type, i, &str);

        Texture tmp;
        tmp.type = name;
        tmp.path = str.C_Str();
        textures.push_back(tmp);
        std::cout <<"textMat : " <<  tmp.path << std::endl;
    }
    return textures;
}
