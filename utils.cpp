#include "utils.h"
#include <cassert>
arma::fmat44 rotation_matrix_by_axis(float theta, char axis){
    arma::fmat44 m2(arma::fill::zeros);
    if(axis=='z'){
        m2={{cosf(theta),-sinf(theta),0,0},
           {sinf(theta),cosf(theta),0,0},
           {0,0,1,0},
           {0,0,0,1}};
    } else if(axis=='x'){
        m2={{1,0,0,0},
           {0,cosf(theta),-sinf(theta),0},
           {0,sinf(theta),cosf(theta),0},
           {0,0,0,1}};

    } else if(axis=='y'){
        m2={{cosf(theta),0,sinf(theta),0},
           {0,1,0,0},
           {-sinf(theta),0,cosf(theta),0},
           {0,0,0,1}};
    } else{
        assert(false);
    }
    return m2;
}
arma::fmat44 viewport(size_t x, size_t y, size_t w, size_t h) {
    // [-1,1]->[x,x+w]
    // [-1,1]->[y,y+w]
    // erase z
    arma::fmat44 mat(arma::fill::eye);
    mat(0,3) = x+w/2.f;
    mat(1,3) = y+h/2.f;
    mat(2,3) = 1.f;
    mat(0,0) = w/2.f;
    mat(1,1) = h/2.f;
    return mat;
}

arma::fmat44 perspective_projection(float n, float f) {
    arma::fmat44 mat;
    mat<<n<<0<<0<<0<<arma::endr
      <<0<<n<<0<<0<<arma::endr
      <<0<<0<<n+f<<-f*n<<arma::endr
      <<0<<0<<1<<0;
    return mat;
}
arma::fmat44 orthographic_projection(float l,float r, float b, float t, float n, float f){
    return {{2/(r-l),0,0,-(r+l)/(r-l)},
            {0,2/(t-b),0,-(t+b)/(t-b)},
            {0,0,2/(n-f),-(n+f)/(n-f)},
            {0,0,0,1}};
}
//center is the center of object
arma::fmat44 modelview(arma::fvec3 eye, arma::fvec3 center) {
    // formula 7.4
    arma::fvec3 up={0,1,0};
    arma::fvec3 z = (eye-center);
    z/=arma::norm(z);
    arma::fvec3 x = arma::cross(up,z);
    if(arma::norm(x)<1e-6f){
        if(arma::dot(up,z)>=0){
            arma::fmat44 Minv={{0,0,1,0},
                               {1,0,0,0},
                               {0,1,0,0},
                               {0,0,0,1}};
            arma::fmat44 Tr(arma::fill::eye);
            for(size_t i=0;i<3;i++){
                Tr(i,3)=-eye[i];
            }
            return Minv*Tr;
        } else{
            arma::fmat44 Minv={{0, 0, 1, 0},
                               {-1, 0, 0, 0},
                               {0, -1, 0, 0},
                               {0, 0, 0, 1}};
            arma::fmat44 Tr(arma::fill::eye);
            for(size_t i=0;i<3;i++){
                Tr(i,3)=-eye[i];
            }
            return Minv*Tr;
        }
    } else{
        x/=arma::norm(x);
        arma::fvec3 y = arma::cross(z,x);
        arma::fmat44 Minv(arma::fill::eye);
        arma::fmat44 Tr (arma::fill::eye);
        for (size_t i=0; i<3; i++) {
            Minv(0,i) = x[i];
            Minv(1,i) = y[i];
            Minv(2,i) = z[i];
            Tr(i,3) = -eye[i];
        }

        return Minv*Tr;
    }

}

arma::fvec3 barycentric(arma::fvec2 A, arma::fvec2 B, arma::fvec2 C, arma::fvec2 P) {
    arma::fvec3 s[2];
    for (size_t i=0;i<2;i++) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    arma::fvec3 u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2f)
        return arma::fvec3{1.f-(u(0)+u(1))/u(2), u(1)/u(2), u(0)/u(2)};
    return arma::fvec3{-1,1,1};
}
