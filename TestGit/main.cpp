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
void triangle(Vec2i* t, TGAImage& image, TGAColor color)
{

	Vec2i bboxmin;
	Vec2i bboxmax;
	bboxmin.x = std::min(t[0].x, std::min(t[1].x, t[2].x));
	bboxmin.y = std::min(t[0].y, std::min(t[1].y, t[2].y));
	bboxmax.x = std::max(t[0].x, std::max(t[1].x, t[2].x));
	bboxmax.y = std::max(t[0].y, std::max(t[1].y, t[2].y));
	bboxmin.x = std::max(bboxmin.x, 0);
	bboxmin.y = std::max(bboxmin.y, 0);
	bboxmax.x = std::min(bboxmax.x, image.get_width() - 1);
	bboxmax.y = std::min(bboxmax.y, image.get_height() - 1);
	//PAx+uABx+vACx=0
	//PAy+uABy+vACy=0
	for (int x = bboxmin.x; x <= bboxmax.x; ++x)
	{
		for (int y = bboxmin.y; y <= bboxmax.y; ++y)
		{
			int flag = 0;
			for (int i = 0; i < 3; ++i)
			{
				int x0 = t[(i + 1) % 3].x - t[i].x;
				int y0 = t[(i + 1) % 3].y - t[i].y;
				int x1 = x - t[i].x;
				int y1 = y - t[i].y;
				if (x0 * y1 - x1 * y0 > 0)flag++;
				else if (x0 * y1 - x1 * y0 < 0)flag--;
				else if (flag >= 0)flag++;
				else flag--;
			}
			if (flag == 3 || flag == -3)
			{
				image.set(x, y, color);
			}
		}
	}
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
	Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
	triangle(pts, image, white);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

