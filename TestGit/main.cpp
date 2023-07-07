#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
Model* model = NULL;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const int width = 800;
const int height = 500;
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)//在image上的点(x0,y0)和点(x1,y1)之间画一条color的线
{
	if (x0 == x1 && y0 == y1)//如果是同一个点，则只画这一个点
	{
		image.set(x0, y0, color);
		return;
	}
	if (std::abs(y1 - y0) > std::abs(x1 - x0))//y之间像素多
	{
		int miny = std::min(y0, y1);
		int maxy = std::max(y0, y1);
		float step= 1/ (float)(y1 - y0);//每一步走多长
		float t = (miny - y0 - 1) / (float)(y1 - y0);
		for (int y = miny; y <= maxy; y++)
		{
			t +=step;
			int x = t * (x1 - x0) + x0;
			image.set(x, y, color);
		}
	}
	else//x之间像素多
	{
		int minx = std::min(x0, x1);
		int maxx = std::max(x0, x1);
		float step = 1 / (float)(x1 - x0);
		float t = (minx - x0 - 1) / (float)(x1 - x0);
		for (int x = minx; x <= maxx; x++)
		{
			t += step;
			int y = t * (y1 - y0) + y0;
			image.set(x, y, color);
		}
	}

}
void rasterize(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color, int ybuffer[])
{
	if (t0.x > t1.x)std::swap(t0, t1);
	for (int x = t0.x; x <= t1.x; ++x)
	{
		int y = 1.0*(x - t0.x) / (t1.x - t0.x) * (t1.y - t0.y) + t0.y;
		if (y > ybuffer[x])
		{
			ybuffer[x] = y;
			for (int i = 0; i < 16; ++i)
			{
				image.set(x, i, color);
			}
		}
	}
}
Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {//求重心坐标的u，v
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}
void triangle(Vec3f* t,Vec2i* uv,float **zbuffer, float intensity,TGAImage &image)
{

	Vec2i bboxmin;
	Vec2i bboxmax;
	//得到包围盒的边界
	bboxmin.x = std::min(t[0].x, std::min(t[1].x, t[2].x));
	bboxmin.y = std::min(t[0].y, std::min(t[1].y, t[2].y));
	bboxmax.x = std::max(t[0].x, std::max(t[1].x, t[2].x));
	bboxmax.y = std::max(t[0].y, std::max(t[1].y, t[2].y));
	bboxmin.x = std::max(bboxmin.x, 0);
	bboxmin.y = std::max(bboxmin.y, 0);
	bboxmax.x = std::min(bboxmax.x, image.get_width() - 1);
	bboxmax.y = std::min(bboxmax.y, image.get_height() - 1);
	//用叉乘来判断点是否在三角形
	for (int x = bboxmin.x; x <= bboxmax.x; ++x)
	{
		for (int y = bboxmin.y; y <= bboxmax.y; ++y)
		{
			Vec3f p;
			p.x = x;
			p.y = y;
			p.z = 0;
			Vec3f u=barycentric(t[0], t[1], t[2], p);
			p[2] = u[0] * t[0].z + u[1] * t[1].z + u[2] * t[2].z;
			Vec2i uvP;

			if (u[0] < 0 || u[1] < 0 || 1. - u[0] - u[1] < 0)continue;
			if (p[2] > zbuffer[x][y])
			{
				uvP = uv[0] * u[0] + uv[1] * u[1] + uv[2] * u[2];
				zbuffer[x][y] = p[2];
				TGAColor color = model->diffuse(uvP);
				
				image.set(x,y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity));
			}
		}
	}
}
Vec3f world2screen(Vec3f v) {
	return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}
int main(int argc, char** argv) {
	Vec3f light_dir = { 0,0,-1 };
	TGAImage image(width, height, TGAImage::RGB);
	model = new Model("modelObject/african_head/african_head.obj");
	float* zbuffer[width];
	for (int i = 0; i < width; ++i)
	{
		zbuffer[i] = new float[height];
	}
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; ++j)
		{
			zbuffer[i][j] = -5;
		}
	}

	int m = model->nfaces();
	for (int i = 0; i < m; ++i)
	{
		std::vector<int>face = model->face(i);
		Vec3f worldC[3];
		Vec3f screenC[3];


		for (int j = 0; j < 3; j++)
		{
			worldC[j] = model->vert(face[j]);
			screenC[j] = world2screen(worldC[j]);
		}
		Vec3f n_ = cross(worldC[2]-worldC[1], worldC[1]-worldC[0]);
		n_.normalize();
		float intensity = n_.x * light_dir.x + n_.y * light_dir.y + n_.z * light_dir.z;
		if (intensity > 0)
		{
			Vec2i uv[3];
			for (int k = 0; k < 3; k++) {
				uv[k] = model->uv(i, k);
			}
			triangle(screenC, uv, zbuffer,intensity, image);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;

}

