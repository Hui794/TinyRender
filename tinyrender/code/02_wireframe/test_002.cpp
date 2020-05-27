/*#include "tgaimage.h"
#include "model_f.h"
using std::cout; using std::endl;

#define test002filename ".\\obj\\african_head.obj"

Model* model = NULL;
const int width = 800;
const int height = 800;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {	// if steep , swap xy
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {	// from left to right
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	for (int x = x0;x <= x1;x++) {
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 + (y1 - y0) * t;	// y - y0 = (x - x0) * (x1 - x0)
		if (steep) {
			image.set(y, x, color);	// if steep , swap xy again
		}
		else {
			image.set(x, y, color);
		}
	}
}

int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model(test002filename);
	}

	TGAImage image(width, height, TGAImage::RGB);

	for (int i = 0;i < model->nfaces();i++) {
		std::vector<int> face = model->face(i);	//	get the face ( triangle of three verts )
		for (int j = 0;j < 3;j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);	// when j = 2 , j + 1 % 3 = 0
			int x0 = (v0.x + 1.) * width / 2;
			int y0 = (v0.y + 1.) * height / 2;
			int x1 = (v1.x + 1.) * width / 2;
			int y1 = (v1.y + 1.) * height / 2;
			line(x0, y0, x1, y1, image, white);
		}
	}

	image.flip_vertically();
	image.write_tga_file("output002.tga");

	return 0;
}*/