#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include "geometry.h"
class Model {
private:
    std::vector<Vec3f> verts_;      // vector of verts
public:
    Model(const char* filename);    //  get model from file
    ~Model();                   
    Vec3f vert(int i);  // get the i vert
    int nverts();       // get the number of verts
};

#endif
