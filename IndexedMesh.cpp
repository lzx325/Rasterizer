#include <iostream>
#include <cassert>

#include "IndexedMesh.h"
IndexedMesh::IndexedMesh(const std::string& filename){
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()){
        assert(false&&"mesh failed to load");
        return;
    }
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            arma::fvec3 v;
            for (size_t i=0;i<3;i++) iss >> v(i);
            verts.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            // attention, this Vec3i means vertex/uv/normal
            iss>>trash;
            arma::uvec3 v;
            for(size_t i=0;i<3;i++) {
                iss>>v(i);
                v(i)--;
            }
            faces.push_back(v);
        } else if (!line.compare(0,1,"#") || line.size()<=1u){
            continue;
        } else{
            std::cout<<line<<std::endl;
            assert(false&&"mesh format error");
        }
    }
    normals.reserve(faces.size());
    for(size_t i=0;i<faces.size();i++){
        auto AB=verts.at(faces[i](1))-verts.at(faces[i](0));
        auto AC=verts.at(faces[i](2))-verts.at(faces[i](0));
        arma::fvec3 normal=arma::cross(AB,AC);
        normal/=arma::norm(normal);
        normals.push_back(normal);
    }
}

IndexedMesh::~IndexedMesh() {}
size_t IndexedMesh::nverts() {
    return verts.size();
}

size_t IndexedMesh::nfaces() {
    return faces.size();
}
arma::fvec3 IndexedMesh::get_vert(size_t i) {
    return verts.at(i);
}
arma::fvec3 IndexedMesh::get_vert(size_t iface, size_t nthvert) {
    return verts[faces.at(iface)(nthvert)];
}
arma::fvec3 IndexedMesh::get_normal(size_t idx){
    return normals.at(idx);
}
arma::uvec3 IndexedMesh::get_face(size_t idx){
    return faces.at(idx);
}
