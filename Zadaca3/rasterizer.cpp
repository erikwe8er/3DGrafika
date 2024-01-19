#include <iostream>
#include <cmath>
#include <vector>
#include "tgaimage.h"
#include <utility>
using namespace std;

// compile with:
// g++ rasterizer.cpp tgaimage.cpp -o out.exe

void set_color(int x, int y, TGAImage &image, TGAColor color, bool invert = false)
{
    image.set(y, x, color);    
}

float line(float x0, float y0, float x1, float y1, float x, float y)
{
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

void line_naive(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (float t = 0; t < 1; t += 0.01)
    {
        int x = x0 * (1.0f - t) + x1 * t;
        int y = x0 * (1.0f - t) + y1 * t;
        set_color(x, y, image, color);
    }
}

void line_midpoint(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, bool invert)
{
    int y = y0;
    int d = line(x0, y0, x1, y1, x0 + 1, y0 + 0.5);
    
    int dx = (x1 - x0);
    int dy = (y1 - y0);
    int increment = 1;
    
    if (dy < 0)
    {
        // pravac ide od gore prema dolje
        dy = -dy;
        increment = -increment;
    }
    
    for (int x = x0; x <= x1; ++x)
    {
        if (invert)
        {
            set_color(x, y, image, color);
        }
        else
        {
            set_color(y, x, image, color);       
        }
        
        if (d < 0)
        {
            y = y + increment;
            d = d + dx - dy;
        }
        else
        {
            d = d - dy;
        }
    }
}

float norm(pair<int,int> vec)
{
    return sqrt(pow(vec.first,2)+pow(vec.second,2));
}

float dot(pair<int,int> fst,pair<int,int> snd)
{
    return fst.first*snd.first + fst.second + snd.second;
}

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    // 'transponiraj' duzinu ako je veci od 1
    bool invert = false;
    if (abs(x1 - x0) < abs(y1 - y0))
    {
        swap(x0, y0);
        swap(x1, y1);
        invert = true;
    }
    
    // zamijeni tocke ako pravac ide s desna na lijevo
    if (x1 < x0)
    {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    // nacrtaj duzinu
    line_midpoint(x0, y0, x1, y1, image, color, invert);
}

void draw_triangle_2d(int x0, int y0, int x1, int y1, int x2, int y2, TGAColor color, TGAImage &image)
{
    vector<int> bound_x,bound_y;
    int xs[3] = {x0,x1,x2};
    int ys[3] = {y0,y1,y2};
    bound_x.push_back(*min_element(begin(xs),end(xs)));
    bound_x.push_back(*max_element(begin(xs),end(xs)));
    bound_y.push_back(*min_element(begin(ys),end(ys)));
    bound_y.push_back(*max_element(begin(ys),end(ys)));
    for(int i=bound_x[0];i<=bound_x[1];i++)
    {
        for(int j=bound_y[0];j<=bound_y[1];j++)
        {
            pair<int,int> AB(x1-x0,y1-y0),BC(x2-x1,y2-y1),CA(x0-x2,y0-y2),AP(x0-i,y0-j),BP(x1-i,y1-j),CP(x2-i,y2-j);
            int crossAPAB = AP.first * AB.second - AB.first * AP.second;
            int crossBPBC = BP.first * BC.second - BC.first * BP.second;
            int crossCPCA = CP.first * CA.second - CA.first * CP.second;
            
            if((crossAPAB >= 0 && crossBPBC >= 0 && crossCPCA >=0) || (crossAPAB <= 0 && crossBPBC <= 0 && crossCPCA <=0))
            {
               image.set(i,j,color);
            }
        }
    }
}

void draw_triangle_2d_gouraud(int x0, int y0, int x1, int y1, int x2, int y2, TGAColor c0,TGAColor c1,TGAColor c2, TGAImage &image)
{
    vector<int> bound_x,bound_y;
    int xs[3] = {x0,x1,x2};
    int ys[3] = {y0,y1,y2};
    bound_x.push_back(*min_element(begin(xs),end(xs)));
    bound_x.push_back(*max_element(begin(xs),end(xs)));
    bound_y.push_back(*min_element(begin(ys),end(ys)));
    bound_y.push_back(*max_element(begin(ys),end(ys)));

    for(int i=bound_x[0];i<=bound_x[1];i++)
    {
        for(int j=bound_y[0];j<=bound_y[1];j++)
        {
            pair<int,int> v0(x1-x0,y1-y0),v1(x2-x0,y2-y0),v2(i-x0,j-y0);
            
            float den = v0.first * v1.second - v1.first * v0.second;

            float alpha = (v2.first * v1.second - v1.first * v2.second) / den;
            float beta = (v0.first * v2.second - v2.first * v0.second) / den;
            float gamma = 1.0f - alpha - beta;

            if(alpha >= 0 && alpha <= 1 && beta >=0 && beta <= 1 && gamma >=0 && gamma <=1)
            {
                image.set(i,j,TGAColor(alpha*c0.r+beta*c1.r+gamma*c2.r, alpha*c0.g+beta*c1.g+gamma*c2.g, alpha*c0.b+beta*c1.b+gamma*c2.b, 255));
            }
        }
    }
}

void draw_triangle(int x0, int y0, int z0, int x1, int y1, int z1, int x2, int y2, int z2, TGAColor color, TGAImage &image)
{
    float x0s = (float)x0/(float)z0;
    float y0s = (float)y0/(float)z0;

    float x1s = (float)x1/(float)z1;
    float y1s = (float)y1/(float)z1;

    float x2s = (float)x2/(float)z2;
    float y2s = (float)y2/(float)z2;

    float x0v = (float)512/(float)2 * x0s + (float)511/(float)2;
    float y0v = (float)512/(float)2 * y0s + (float)511/(float)2;

    float x1v = (float)512/(float)2 * x1s + (float)511/(float)2;
    float y1v = (float)512/(float)2 * y1s + (float)511/(float)2;

    float x2v = (float)512/(float)2 * x2s + (float)511/(float)2;
    float y2v = (float)512/(float)2 * y2s + (float)511/(float)2;

    vector<int> bound_x,bound_y;
    int xs[3] = {ceil(x0v),ceil(x1v),ceil(x2v)};
    int ys[3] = {ceil(y0v),ceil(y1v),ceil(y2v)};
    bound_x.push_back(*min_element(begin(xs),end(xs)));
    bound_x.push_back(*max_element(begin(xs),end(xs)));
    bound_y.push_back(*min_element(begin(ys),end(ys)));
    bound_y.push_back(*max_element(begin(ys),end(ys)));

    for(int i=bound_x[0];i<=bound_x[1];i++)
    {
        for(int j=bound_y[0];j<=bound_y[1];j++)
        {
            pair<int,int> v0(x1v-x0v,y1v-y0v),v1(x2v-x0v,y2v-y0v),v2(i-x0v,j-y0v);
            
            float den = v0.first * v1.second - v1.first * v0.second;

            float alpha = (v2.first * v1.second - v1.first * v2.second) / den;
            float beta = (v0.first * v2.second - v2.first * v0.second) / den;
            float gamma = 1.0f - alpha - beta;

            if(alpha >= 0 && alpha <= 1 && beta >=0 && beta <= 1 && gamma >=0 && gamma <=1)
            {
                image.set(i,j,color);
            }
        }
    }
}

void draw_triangle_tex(int x0, int y0, int z0, int x1, int y1, int z1, int x2, int y2, int z2,int u0, int v0,int u1, int v1, int u2, int v2, const char* path, TGAImage &image)
{
    TGAImage tex;
    tex.read_tga_file(path);

    float x0s = (float)x0/(float)z0;
    float y0s = (float)y0/(float)z0;

    float x1s = (float)x1/(float)z1;
    float y1s = (float)y1/(float)z1;

    float x2s = (float)x2/(float)z2;
    float y2s = (float)y2/(float)z2;

    float x0v = (float)512/(float)2 * x0s + (float)511/(float)2;
    float y0v = (float)512/(float)2 * y0s + (float)511/(float)2;

    float x1v = (float)512/(float)2 * x1s + (float)511/(float)2;
    float y1v = (float)512/(float)2 * y1s + (float)511/(float)2;

    float x2v = (float)512/(float)2 * x2s + (float)511/(float)2;
    float y2v = (float)512/(float)2 * y2s + (float)511/(float)2;

    vector<int> bound_x,bound_y;
    int xs[3] = {ceil(x0v),ceil(x1v),ceil(x2v)};
    int ys[3] = {ceil(y0v),ceil(y1v),ceil(y2v)};
    bound_x.push_back(*min_element(begin(xs),end(xs)));
    bound_x.push_back(*max_element(begin(xs),end(xs)));
    bound_y.push_back(*min_element(begin(ys),end(ys)));
    bound_y.push_back(*max_element(begin(ys),end(ys)));

    for(int i=bound_x[0];i<=bound_x[1];i++)
    {
        for(int j=bound_y[0];j<=bound_y[1];j++)
        {
            pair<int,int> v00(x1v-x0v,y1v-y0v),v11(x2v-x0v,y2v-y0v),v22(i-x0v,j-y0v);
            
            float den = v00.first * v11.second - v11.first * v00.second;

            float alpha = (v22.first * v11.second - v11.first * v22.second) / den;
            float beta = (v00.first * v22.second - v22.first * v00.second) / den;
            float gamma = 1.0f - alpha - beta;

            if(alpha >= 0 && alpha <= 1 && beta >=0 && beta <= 1 && gamma >=0 && gamma <=1)
            {
                int u = tex.get_width() * (alpha * u0 + beta * u1 + gamma * u2);
                int v = tex.get_height() * (alpha * v0 + beta * v1 + gamma * v2);
                image.set(i,j,tex.get(u,v));
            }
        }
    }
}



// dimenzije slike
const int width  = 512;
const int height = 512;

// definirajmo boje
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor blue  = TGAColor(0, 0, 255, 255);

int main()
{
    
    TGAImage image1(width, height, TGAImage::RGB);

    draw_triangle_2d(20,20,70,100,90,30,blue,image1);
    draw_triangle_2d(350,300,390,400,460,320,white,image1);
    draw_triangle_2d_gouraud(100, 30, 250, 240, 300, 40, blue,green,red,image1);

    image1.flip_vertically();
    image1.write_tga_file("zad3.tga");
    
    TGAImage image2(width, height, TGAImage::RGB);
    draw_triangle(-48,-10,82,29,-15,44,13,34,114,blue,image2);

    image2.flip_vertically();
    image2.write_tga_file("zad3.tga");

    TGAImage image3(width, height, TGAImage::RGB);
    draw_triangle_tex(-48,-10,82,29,-15,44,13,34,114, 0, 0, 0, 1, 1, 0, "texture.tga",image3);

    image3.flip_vertically();
    image3.write_tga_file("zad4a.tga");
}