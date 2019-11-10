#include "rasterizer.h"
#include "utils.h"
Rasterizer::Rasterizer(std::shared_ptr<IndexedMesh> mesh_ptr,const arma::fvec3& eye,
                       const arma::fvec3& center,const std::vector<arma::fvec3>& light_sources,
                       float fov)\
    : shader_ptr(new Shader(this)),mesh_ptr(mesh_ptr),light_sources(light_sources){
    set_matrix(eye,center,fov);
}
void Rasterizer::set_matrix(const arma::fvec3 &eye, const arma::fvec3 &center, float fov){
    float n=-1;
    float f=-500;
    float t=fabsf(n)*fov;
    float b=-fabsf(n)*fov;
    float l=-fabsf(n)*fov*4.f/3.f;
    float r=fabsf(n)*fov*4.f/3.f;
    img_width=1600;
    img_height=1200;
    size_t x=0;
    size_t y=0;
    size_t w=img_width;
    size_t h=img_height;
    viewport_matrix_ptr=std::shared_ptr<arma::fmat44>(new arma::fmat44(viewport(x,y,w,h)));
    auto M_ortho=orthographic_projection(l,r,b,t,n,f);
    auto M_per=perspective_projection(n,f);
    arma::fmat44 M_proj=M_ortho*M_per;
    projection_matrix_ptr=std::shared_ptr<arma::fmat44>(new arma::fmat44(M_proj));
    modelview_matrix_ptr=std::shared_ptr<arma::fmat44>(new arma::fmat44(modelview(eye,center)));
}
void Rasterizer::rasterize_triangle(const arma::fmat &vertices, std::vector<QRgb> &image, std::vector<float> &zbuffer){
    assert(vertices.n_rows==4 and vertices.n_cols==3);
    assert(image.size()==img_width*img_height);
    assert(zbuffer.size()==img_width*img_height);
    // 3x4
    arma::fmat pts  = ((*viewport_matrix_ptr)*vertices).t(); // transposed to ease access to each of the points
    // 3x2
    arma::fmat pts2(3,2);
    for (size_t i=0; i<3; i++){
        pts2.row(i) = pts(i,arma::span(0,1))/pts.row(i)(3);
    }

    arma::fvec2 bboxmin{std::numeric_limits<float>::max(),  std::numeric_limits<float>::max()};
    arma::fvec2 bboxmax{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
    arma::fvec2 clamp{float(img_width-1), float(img_height-1)};
    for (size_t i=0; i<3; i++) {
        for (size_t j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts2(i,j)));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2(i,j)));
        }
    }
    arma::uvec2 P;
    QRgb color;
    for (P(0)=(unsigned int)(bboxmin(0)); P(0)<=bboxmax(0); P(0)++) {
        for (P(1)=(unsigned int)bboxmin(1); P(1)<=bboxmax(1); P(1)++) {
            arma::fvec3 bc_screen;
            bc_screen  = barycentric(pts2.row(0).t(), pts2.row(1).t(), pts2.row(2).t(), arma::conv_to<arma::fvec>::from(P));


            arma::fvec3 bc_clip=bc_screen;
            float frag_depth = arma::dot(vertices.row(2).t(),bc_clip);
            if (bc_screen(0)<0 || bc_screen(1)<0 || bc_screen(2)<0 || zbuffer[P(0)+P(1)*img_width]>frag_depth) {
                continue;
            }
            bool discard = shader_ptr->fragment(bc_clip, color);
            if (!discard) {
                zbuffer[P(0)+P(1)*img_width] = frag_depth;
                image.at(P(0)+P(1)*img_width)=color;
            }
        }

    }
}

QImage Rasterizer::get_image(){
    std::vector<QRgb> img(img_width * img_height);
    std::vector<float> z_buffer(img_width*img_height,-std::numeric_limits<float>::infinity());
    // TODO: rasterize every triangle
    for(size_t i=0;i<mesh_ptr->nfaces();i++){
        shader_ptr->set_face(i);
        arma::fmat vertices(4,3,arma::fill::zeros);
        vertices(3,arma::span(0,2))=arma::fmat(1,3,arma::fill::ones);
        for(size_t j=0;j<3;j++){
            vertices(arma::span(0,2),j)=mesh_ptr->get_vert(i,j);
        }
        arma::fmat vertices_bak=vertices;
        vertices=(*projection_matrix_ptr)*(*modelview_matrix_ptr)*vertices;
        for(size_t j=0;j<3;j++){
            if(vertices(3,j)>-1e-6f){
                std::cout<<"homogeneous"<<std::endl;
                std::cout<<vertices<<std::endl;
                std::cout<<"eye space"<<std::endl;
                std::cout<<(*modelview_matrix_ptr)*vertices_bak<<std::endl;
                std::cout<<"world space"<<std::endl;
                std::cout<<vertices_bak<<std::endl;
                assert(false);
            } else{
                vertices(arma::span(0,3),j)/=vertices(3,j);
            }
        }
        rasterize_triangle(vertices,img,z_buffer);
        shader_ptr->reset();
    }
    QImage qimg(int(img_width),int(img_height),QImage::Format_RGB32);
    memcpy(qimg.bits(),img.data(),img_width*img_height*sizeof(QRgb));
    return qimg;
}

void Shader::set_face(size_t current_face_idx)
{
    assert(ready==false);
    this->current_face_idx=current_face_idx;
    current_normal=rasterizer->mesh_ptr->get_normal(current_face_idx);
    for(size_t i=0;i<3;i++){
        current_intensity(i)=0;
        auto n_lights=rasterizer->light_sources.size();
        for(size_t l=0;l<n_lights;l++){
            arma::fvec3 light_dir=(rasterizer->light_sources.at(l)  -  rasterizer->mesh_ptr->get_vert(current_face_idx,i));
            light_dir/=arma::norm(light_dir);
            current_intensity(i)+=std::max(0.f, arma::dot(current_normal,light_dir));
        }
    }
    ready=true;
}

bool Shader::fragment(const arma::fvec3& bar, QRgb& color)
{
    float intensity=arma::dot(bar,current_intensity);
    if(intensity>1) intensity=1;
    color=qRgb(int(255*intensity),int(255*intensity),int(255*intensity));
    return false;
}
void Shader::reset(){
    assert(ready==true);
    ready=false;
}
