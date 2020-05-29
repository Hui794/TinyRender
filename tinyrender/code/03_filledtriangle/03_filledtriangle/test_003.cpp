#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
using std::cout; using std::endl;

#define test003filename ".\\obj\\Squirtle.obj"

Model* model = NULL;
const int width = 800;
const int height = 800;
const float zoom = 5;
const int xoffset = 250;
const int yoffset = -120;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {	// fill the triangle
	if (t0.y == t1.y && t0.y == t2.y) {
		return;
	}
	if (t0.y > t1.y) {		// sort t0 < t1 < t2
		std::swap(t0, t1);
	}
	if (t0.y > t2.y) {
		std::swap(t0, t2);
	}
	if (t1.y > t2.y) {
		std::swap(t1, t2);
	}
	int total_height = t2.y - t0.y;
	for (int i = 0;i < total_height;i++) {
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		int segment_hegiht = second_half ? t2.y - t1.y : t1.y - t0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_hegiht;
		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x;j <= B.x;j++) {
			image.set(j + xoffset, t0.y + i + yoffset, color);	// attention, due to int casts t0.y+i != A.y
		}
	}
}

Vec2i world2screen(Vec3f v) {
	return Vec2i(int((v.x + 1.) * width / zoom), int((v.y + 1.) * height / zoom));
}

int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model(test003filename);
	}

	TGAImage image(width, height, TGAImage::RGB);
	srand(time(NULL));

	for (int i = 0;i < model->nfaces();i++) {	
		std::vector<int> face = model->face(i);	// get the i face
		Vec3f world_coords[3];
		Vec2i screen_coords[3];
		for (int j = 0;j < 3;j++) {		
			world_coords[j] = model->vert(face[j]);		// get 3 verts of the face
			screen_coords[j] = world2screen(world_coords[j]);	// turn 3 verts into screen int
		}

		int r = std::rand() % 255;	// random rgb
		int g = std::rand() % 255;
		int b = std::rand() % 255;
		
		triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(r, g, b, 255));
	}

	image.flip_vertically();	//	have the origin at the left bottom corner of the image
	image.write_tga_file("outputSquirtle003.tga");
	return 0;
}