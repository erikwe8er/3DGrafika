#pragma once
#include "geometry.h"

class Material
{
  public:
    Vec3f diffuse_colour;
    float diffuse_coef = 1;
    
    float specular_coef = 1;
    float phong_exp = 1000;

    float opacity = 1;
    float reflex_coef = 0.1;
    float refract_coef = 1;
    
    Material() : diffuse_colour(Vec3f(0, 0, 0)) { }
    Material(const Vec3f &colour,const float opacity, const float reflex_coef) : diffuse_colour(colour), opacity(opacity), reflex_coef(reflex_coef) { }
};