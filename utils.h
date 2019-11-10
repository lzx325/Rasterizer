#ifndef UTILS_H
#define UTILS_H
#include <armadillo>
arma::fmat44 viewport(size_t x,size_t y, size_t w, size_t h);
arma::fmat44 perspective_projection(float n, float f);
arma::fmat44 orthographic_projection(float l,float r, float b, float t, float n, float f);
arma::fmat44 modelview(arma::fvec3 eye, arma::fvec3 center);
arma::fvec3 barycentric(arma::fvec2 A, arma::fvec2 B, arma::fvec2 C, arma::fvec2 P);
arma::fmat44 rotation_matrix_by_axis(float theta, char axis='z');
#endif // UTILS_H
