#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include "geometry.h"
class Model {
private:
    std::vector<Vec3f> verts_;      // vector of verts
    std::vector<std::vector<int>> faces_;   // vector of faces
public:
    Model(const char* filename);    //  get model from file
    ~Model();
    Vec3f vert(int i);  // get the i vert
    std::vector<int> face(int idx); // get the idx face
    int nverts();       // get the number of verts
    int nfaces();       // get the number of faces
};

#endif
