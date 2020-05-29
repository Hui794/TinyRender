#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <limits>
using std::cout; using std::endl;

#define test006filename ".\\obj\\Squirtle.obj"

Model* model = NULL;
const int width = 1000;
const int height = 1000;
const float zoom = 5;	//2
const int xoffset = 300;	//0
const int yoffset = -120;	//0

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {		//	barycentric judge
	Vec3f s[2];
	for (int i = 2;i--;) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);	// X 
	if (std::abs(u[2]) > 1e-2) {	// dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}
	return Vec3f(-1, 1, 1);	// in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec3f* pts,float* zbuffer, TGAImage& image, TGAColor color) {
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0;i < 3;i++) {			// get the box of tri
		for (int j = 0;j < 2;j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x;P.x <= bboxmax.x;P.x++) {		//	scan and set
		for (P.y = bboxmin.y;P.y <= bboxmax.y;P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {	// thrown away
				continue;
			}
			P.z = 0;
			for (int i = 0;i < 3;i++) {
				P.z += pts[i][2] * bc_screen[i];
			}
			if (zbuffer[int(P.x + P.y * width)] <= P.z) {
				image.set(P.x, P.y, color);
				zbuffer[int(P.x + P.y * width)] = P.z;
			}
		}
	}
}

Vec3f world2screen(Vec3f v) {	//	attention we have to change x and y to int otherwise they maybe too close for the for loop
	return Vec3f(int(xoffset + (v.x + 1.) * width / zoom + .5), int(yoffset + (v.y + 1.) * height / zoom + .5), v.z);	//	add 0.5 to rounding off
}

int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model(test006filename);
	}

	float* zbuffer = new float[width * height];
	for (int i = width * height;i--;) {
		zbuffer[i] = -std::numeric_limits<float>::max();
	}

	TGAImage image(width, height, TGAImage::RGB);
	Vec3f light(0, 0, -1);	// light

	for (int i = 0;i < model->nfaces();i++) {
		std::vector<int> face = model->face(i);
		Vec3f world_coords[3];
		Vec3f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->vert(face[j]);
			screen_coords[j] = world2screen(world_coords[j]);
		}

		Vec3f norm = cross(world_coords[2] - world_coords[0], world_coords[1] - world_coords[0]);
		norm.normalize();	// the normalize norm of the tri
		float intensity = light * norm;	// light * norm = lcos@
		if (intensity > 0) {
			triangle(screen_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}

	image.flip_vertically();
	image.write_tga_file("outputSquirtle006.tga");
	return 0;
}