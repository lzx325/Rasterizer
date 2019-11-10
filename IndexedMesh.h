#ifndef INDEXEDMESH_H
#define INDEXEDMESH_H
#include<vector>
#include<armadillo>
#include <string>
class IndexedMesh {
private:
    std::vector<arma::fvec3> verts;
    std::vector<arma::uvec3> faces; // attention, this Vec3i means vertex/uv/normal
    std::vector<arma::fvec3> normals;
public:
    IndexedMesh(const std::string& filename);
    ~IndexedMesh();
    size_t nverts();
    size_t nfaces();
    arma::fvec3 get_normal(size_t iface);
    arma::fvec3 get_vert(size_t i);
    arma::fvec3 get_vert(size_t iface, size_t nthvert);
    arma::uvec3 get_face(size_t idx);
};
#endif // INDEXEDMESH_H
