#include <vector>
#include <iostream>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "our_gl.h"
#include "geometry.h"
using std::cout; using std::endl;
#define CLAMP(t) ((t>1.f)?1.f:((t<0.f)?0.f:t))
#define test012filename ".\\obj\\african_head.obj"

Model* model = NULL;
const int width = 1000;
const int height = 1000;
const int zoom = 4;	// 4
const int xoffset = 0;
const int yoffset = 0;

Vec3f light_dir(1, 1, 1);
Vec3f eye(0, -1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

struct FlatShader :public IShader {
	mat<3, 3, float> varying_tri;	//	record the transformed triangle

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
		gl_Vertex = Projection * ModelView * gl_Vertex;
		varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
		gl_Vertex = ViewPort * gl_Vertex;
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		Vec3f n = cross(varying_tri.col(1) - varying_tri.col(0),
			varying_tri.col(2) - varying_tri.col(0)).normalize();	// the normalize norm of the tri
		float intensity = CLAMP(n * light_dir);		// light * norm = lcos
		color = TGAColor(255, 255, 255) * intensity;
		return false;
	}
};

struct GouraudShader : public IShader {
	Vec3f varying_intensity;	// write by vertex shader , read by fragment shader

	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));	// read the vertex from obj file
		gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;
		varying_intensity[nthvert] = CLAMP(model->normal(iface, nthvert) * light_dir);
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		float intensity = varying_intensity * bar;	// interpolate intensity for current Pixel
		/*if (intensity > .85)  intensity = 1;	//F
		else if (intensity > .60) intensity = .80;
		else if (intensity > .45) intensity = .60;
		else if (intensity > .30) intensity = .45;
		else if (intensity > .15) intensity = .30;
		else intensity = 0;*/
		color = TGAColor(255, 255, 255) * intensity;
		return false;	// do not discard pixel
	}
};

struct Shader : public IShader {
	mat<2, 3, float> varying_uv;	//	write by vertex shader, read by fragment shader
	mat<4, 4, float> uniform_M;		//	Projection*ModelView
	mat<4, 4, float> uniform_MIT;	//	(Projection*ModelView).invert_transpose()


	virtual Vec4f vertex(int iface, int nthvert) {
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));	//  read the vertex from obj file
		return ViewPort * Projection * ModelView * gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		Vec2f uv = varying_uv * bar;	//	interpolate uv for current Pixel
		Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();	// transform normal vector
		Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize(); // transfrom light direction
		float intensity = std::max(0.f, n * l);
		color = model->diffuse(uv) * intensity;
		return false;	// do not discard pixel
	}
};

int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model(test012filename);
	}

	lookat(eye, center, up);
	viewport(xoffset + width / 8, yoffset + height / 8, width * 3 / zoom, height * 3 / zoom);
	projection(-1.f / (eye - center).norm());
	light_dir.normalize();

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

	// FlatShader shader;
	//GouraudShader shader;
	Shader shader;
	shader.uniform_M = Projection * ModelView;
	shader.uniform_MIT = (Projection * ModelView).invert_transpose();
	for (int i = 0;i < model->nfaces();i++) {
		cout << i << endl;
		std::vector<int> face = model->face(i);
		Vec4f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, image, zbuffer);
	}

	image.flip_vertically();
	zbuffer.flip_vertically();
	image.write_tga_file("output012.tga");
	zbuffer.write_tga_file("zbuffer012.tga");

	delete model;
	return 0;
}