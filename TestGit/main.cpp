#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model* model = NULL;
const int width = 200;
const int height = 200;
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
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
	line(t0.x, t0.y, t1.x, t1.y, image, color);
	line(t1.x, t1.y, t2.x, t2.y, image, color);
	line(t2.x, t2.y, t0.x, t0.y, image, color);
}
int main(int argc, char** argv) {

	TGAImage image(width, height, TGAImage::RGB);
	/*
	model = new Model("modelObject/african_head.obj");
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, white);
	for (int i = 0; i < model->nfaces(); ++i)//遍历所有的三角形面
	{
		std::vector<int>face = model->face(i);//取其中一个面
		for (int j = 0; j < 3; ++j)
		{
			Vec3f v0 = model->vert(face[j]);//得到该面的一个点
			Vec3f v1 = model->vert(face[(j+1)%3]);//得到该面的下一个点
			printf("%f %f %f %f\n", v0.x, v0.y, v1.x, v1.y);
			int x0 = (v0.x + 1) * width / 2;
			int y0 = (v0.y + 1) * height / 2;
			int x1 = (v1.x + 1) * width / 2;
			int y1 = (v1.y + 1) * height / 2;
			line(x0, y0, x1, y1, image, white);
		}
	}
	*/
	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	triangle(t0[0], t0[1], t0[2], image, white);
	triangle(t1[0], t1[1], t1[2], image, green);
	triangle(t2[0], t2[1], t2[2], image, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

