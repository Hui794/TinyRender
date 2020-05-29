#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
using std::cout; using std::endl;

#define test004filename ".\\obj\\Squirtle.obj"

Model* model = NULL;
const int width = 800;
const int height = 800;
const float zoom = 5;
const int xoffset = 250;
const int yoffset = -120;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

Vec3f barycentric(Vec2i A, Vec2i B, Vec2i C, Vec2i P) {		//	barycentric judge
	Vec3f s[2];
	for (int i = 2;i--;) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);	// X 
	if (std::abs(u[2]) > 1e-2) {	// dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}	// in this case generate negative coordinates, it will be thrown away by the rasterizator
	return Vec3f(-1, 1, 1);
}

void triangle(Vec2i* pts, TGAImage& image, TGAColor color) {	
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0;i < 3;i++) {			// get the box of tri
		for (int j = 0;j < 2;j++) {
			bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec2i P;
	for (P.x = bboxmin.x;P.x <= bboxmax.x;P.x++) {		//	scan and set
		for (P.y = bboxmin.y;P.y <= bboxmax.y;P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
				continue;
			}
			image.set(P.x, P.y , color);
		}
	}
}

Vec2i world2screen(Vec3f v) {
	return Vec2i(int(xoffset + (v.x + 1.) * width / zoom), int(yoffset + (v.y + 1.) * height / zoom));
}

int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model(test004filename);
	}

	TGAImage image(width, height, TGAImage::RGB);
	srand(time(NULL));

	for (int i = 0;i < model->nfaces();i++) {
		std::vector<int> face = model->face(i);
		Vec3f world_coords[3];
		Vec2i screen_coords[3];
		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->vert(face[j]);
			screen_coords[j] = world2screen(world_coords[j]);
		}

		int r = std::rand() % 255;
		int g = std::rand() % 255;
		int b = std::rand() % 255;

		triangle(screen_coords, image, TGAColor(r, g, b, 255));
	}

	image.flip_vertically();
	image.write_tga_file("outputSquirtle004.tga");
	return 0;
}
