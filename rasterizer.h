#ifndef RASTERIZER_H
#define RASTERIZER_H
#include <vector>
#include <armadillo>
#include <QRgb>
#include <QImage>
#include <memory>
#include "IndexedMesh.h"
#include "tgaimage.h"
class Rasterizer;
class BaseTriangleShader{
public:
    virtual void set_face(size_t current_face_idx)=0;
    virtual bool fragment(const arma::fvec3& bar, QRgb& color)=0;
    virtual void reset()=0;
    virtual ~BaseTriangleShader(){}
};

class Shader: public BaseTriangleShader{
private:
    size_t current_face_idx;
    arma::fvec3 current_normal;
    arma::fvec3 current_intensity;
    arma::fmat current_uv;
    Rasterizer* rasterizer;
    bool ready;
    friend class Rasterizer;
public:
    Shader(Rasterizer* rasterizer): rasterizer(rasterizer), ready(false){}
    virtual void set_face(size_t current_face_idx) override;
    virtual bool fragment(const arma::fvec3& bar, QRgb& color) override;
    virtual void reset() override;
};
class Rasterizer{
private:
    friend class Shader;
    std::shared_ptr<Shader> shader_ptr;
    std::shared_ptr<IndexedMesh> mesh_ptr;
    std::shared_ptr<arma::fmat44> viewport_matrix_ptr;
    std::shared_ptr<arma::fmat44> projection_matrix_ptr;
    std::shared_ptr<arma::fmat44> modelview_matrix_ptr;

    arma::fvec3 center;
    bool use_texture;
    size_t img_width,img_height;
    std::vector<arma::fvec3> light_sources;

public:
    std::shared_ptr<TGAImage> texture_image;
    Rasterizer(std::shared_ptr<IndexedMesh> mesh_ptr,const arma::fvec3& eye,
               const arma::fvec3& center,const std::vector<arma::fvec3>& light_sources,
               float fov,const std::string& texture_file);
    void set_matrix(const arma::fvec3& eye,
                    const arma::fvec3& center,
                    float fov,bool use_texture);
    void rasterize_triangle(const arma::fmat& vertices, std::vector<QRgb> & image, std::vector<float>& zbuffer);
    QImage get_image();
};

#endif // RASTERIZER_H
