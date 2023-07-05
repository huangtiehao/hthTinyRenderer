#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

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
int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	//line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, white);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

