#include "image.h"
#include "geometry.h"
#include "ray.h"
#include "objects.h"
#include "scene.h"
#include "light.h"
#include <cmath>
using namespace std;

// compile with: g++ -std=c++17 raytrace.cpp -o out.exe

Ray ray_to_pixel(Vec3f origin, const Image &img, int i, int j)
{
    Ray ray = Ray();

    float aspect_ratio = (float)img.width / img.height;
 
    float x = (-1 + 2 * (i + 0.5) / (float)img.width) * aspect_ratio;
    float y = (1 - 2 * (j + 0.5) / (float)img.height);
    float z = -1;

    ray.origin = origin;
    ray.direction = Vec3f(x, y, z).normalize();
    return ray;
}

void render(const Scene &scene)
{
    // dimenzije slike
    const int width = 1024;
    const int height = 768;
    
    Vec3f origin = Vec3f(0, 0, 0);
    
    Image img = Image(width, height);
    
    // crtanje slike
    for (size_t j = 0; j < height; ++j)
    {
        for (size_t i = 0; i < width; ++i)
        {
            Ray ray = ray_to_pixel(origin, img, i, j);
            Vec3f colour = scene.cast_ray(ray,0);
            img.set_pixel(i, j, colour);
        }
    }
    
    img.save("./img.ppm");
}

int main()
{
    Scene scene;
    
    Material red(Vec3f(1, 0, 0),0.4,0.2);
    Material green(Vec3f(0, 1, 0),0.7,0.25);
    Material blue(Vec3f(0, 0, 1),0.5,0.2);
    Material grey(Vec3f(0.5, 0.5, 0.5),1,0.2);

    Sphere s1(Vec3f(2, -1, -12), 3, red);
    Sphere s2(Vec3f(0, -2, -7), 1.5, blue);
    Sphere s3(Vec3f(9, -2, -20), 4, green);

    scene.add_object(&s1);
    scene.add_object(&s2);
    scene.add_object(&s3);

    Light l1(Vec3f(-25, 20, 30), 2500);
    Light l2(Vec3f(30, 30, 20), 2000);
    
    scene.add_light(&l1);
    scene.add_light(&l2);
    
    render(scene);
    return 0;
}