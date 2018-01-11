#include "Model3D.h"
#include <algorithm>

using namespace std;
Model3D::Model3D(){
    _Loader = new AssetLoader();
    selectedModel=-1;
}

void Model3D::loadMesh(const std::string filename){
    bool statusOk = _Loader->import(filename);
    assert(statusOk);
    bool loadOk = _Loader->loadData(vertices,normals,indices,textures,AllTexture,modelTexture);

    assert(loadOk);
    nb_mesh = _Loader->_scene->mNumMeshes;
    path = filename;
    OBBs.resize(nb_mesh);
    aabb_min.resize(nb_mesh);
    aabb_max.resize(nb_mesh);
    transform.resize(nb_mesh);

    for(int i=0;i<nb_mesh;i++){
        auto minMax_x = std::minmax_element(vertices[i].begin(), vertices[i].end(),[](const glm::vec3& v1, const glm::vec3& v2) {
            return v1.x < v2.x;
        });

        auto minMax_y = std::minmax_element(vertices[i].begin(), vertices[i].end(),[](const glm::vec3& v1, const glm::vec3& v2) {
            return v1.y < v2.y;
        });

        auto minMax_z = std::minmax_element(vertices[i].begin(), vertices[i].end(),[](const glm::vec3& v1, const glm::vec3& v2) {
            return v1.z < v2.z;
        });

        aabb_max[i] = glm::vec3(minMax_x.second->x,minMax_y.second->y-abs(minMax_y.first->y-minMax_y.second->y),minMax_z.second->z);
        aabb_min[i] = glm::vec3(minMax_x.first->x,minMax_y.first->y-2*abs(minMax_y.first->y-minMax_y.second->y),minMax_z.first->z);
    }




}
