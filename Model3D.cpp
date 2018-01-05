#include "Model3D.h"

using namespace std;
Model3D::Model3D(){
    _Loader = new AssetLoader();
}

void Model3D::loadMesh(const std::string filename){
    bool statusOk = _Loader->import(filename);
    assert(statusOk);
    bool loadOk = _Loader->loadData(vertices,normals,indices);
    assert(loadOk);
    nb_mesh = _Loader->_scene->mNumMeshes;
}
