#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"
class Model {
private:
    std::vector<Vec3f> verts_;      // vector of verts
    std::vector<std::vector<int>> faces_;   // vector of faces
    std::vector<Vec2f> uvs_;    // vector of uvs
    TGAImage diffusemap_;
    void load_texture(std::string filename, const char* suffix, TGAImage& img);
public:
    Model(const char* filename);    //  get model from file
    ~Model();
    int nverts();       // get the number of verts
    int nfaces();       // get the number of faces
    Vec3f vert(int i);  // get the i vert
    Vec2f uv(int i);    // get the i uv
    std::vector<int> face(int idx); // get the idx face
    TGAColor diffuse(Vec2f uv);
};

#endif
