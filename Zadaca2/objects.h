#pragma once
#include <cmath>
#include "geometry.h"
#include "ray.h"
#include "material.h"

struct Object 
{
  public:
    Material material;
    virtual bool ray_intersect(const Ray &ray, float &tin,float &tout, Vec3f &normal) const = 0;  
};

struct Sphere : public Object
{
    float r;
    Vec3f c;
      
    public:
    Sphere(const Vec3f &c, const float &r) : c(c), r(r) { }
    Sphere(const Vec3f &c, const float &r, const Material &mat) : c(c), r(r)
    { 
        Object::material = mat;
    }

    bool ray_intersect(const Ray &ray, float &tin,float &tout,Vec3f &normal) const
    {
        Vec3f e_minus_c = ray.origin - c;
        float d2 = ray.direction * ray.direction;
        
        float disc = pow(ray.direction * e_minus_c, 2) - d2 * (e_minus_c * e_minus_c - r * r);
        
        if (disc < 0)
        {
            return false;
        }
        else
        {
            bool ray_inside_sphere = e_minus_c * e_minus_c <= r * r;
            if (ray_inside_sphere)
            {
                float t1 = (-ray.direction * e_minus_c + sqrt(disc)) / d2;
                tout =  (-ray.direction * e_minus_c - sqrt(disc)) / d2;
                tin = t1;
            }
            else
            {
                float t2 = (-ray.direction * e_minus_c - sqrt(disc)) / d2;
                tout = (-ray.direction * e_minus_c + sqrt(disc)) / d2;
                tin = t2;
            }
            
            Vec3f hit_point = ray.origin + ray.direction * tin;
            Vec3f leave_point = ray.origin + ray.direction * tout;
            normal = (hit_point - c).normalize();
            return true;
        }
    }
};

