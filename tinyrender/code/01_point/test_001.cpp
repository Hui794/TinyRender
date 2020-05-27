#include "tgaimage.h"
#include "model.h"
#include <iostream>
using std::cout; using std::endl;

#define test001filename ".\\obj\\african_head.obj"

Model* model = NULL;
const int width = 800;
const int height = 800;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);


Vec2i world2screen(Vec3f v) {
    return Vec2i(int((v.x + 1.) * width / 2.), int((v.y + 1.) * height / 2.));
}

int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model(test001filename);
    }

    TGAImage image(width, height, TGAImage::RGB);


    for (int i = 0; i != model->nverts(); i++) {
        Vec3f v = model->vert(i);
        Vec2i p = world2screen(v);
        image.set(p.x, p.y, white);
    }

    image.flip_vertically(); // have the origin at the left bottom corner of the image
    image.write_tga_file("output001.tga");
    return 0;
}
