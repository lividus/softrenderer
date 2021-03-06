// cpurender.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue	 = TGAColor(0, 0, 255, 255);

const int ImageWidth = 200;
const int ImageHeight = 200;

Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

Model *model = NULL;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	bool swaped = false;

	if( std::abs(x0-x1) < std::abs(y0-y1) )
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		swaped = true;
	}

	if( x1 < x0 )
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx =  x1 - x0;
	int dy =  y1 - y0;
	int k = std::abs(dy) * 2;
	int yStep = y1 > y0 ? 1 : -1; 
	int y = y0;
	int error = 0;
	for (int x = x0; x <= x1; x++)
	{		
		if(swaped)
			image.set(y,x,color);
		else
			image.set(x,y,color);

		error += k;

		if( error > dx )
		{
			y += yStep;
			error -= dx*2;
		}
	}
}

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color)
{
	line(t0.x, t0.y, t1.x, t1.y, image, color);
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
	if( t0.y > t1.y ) std::swap(t0, t1);
	if( t0.y > t2.y ) std::swap(t0, t2);
	if( t1.y > t2.y ) std::swap(t1, t2);

	int totalHeight = t2.y - t0.y;
	int segmentBottom = t1.y - t0.y + 1;
	int segmentUp = t2.y - t1.y + 1;
	Vec2i totalWidth = t2 - t0;
	Vec2i bottomWidth = t1 - t0;

	for (int y = t0.y; y < t1.y; y++)
	{		
		float curH = y-t0.y;
        float alpha = curH/totalHeight;
        float beta  = curH/segmentBottom; // be careful with divisions by zero
        Vec2i A = t0 + totalWidth*alpha;
        Vec2i B = t0 + bottomWidth*beta;
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, y, color); // attention, due to int casts t0.y+i != A.y
        }
	}

    /*line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);*/
}

void TestLine(TGAImage &image)
{
	for (int i = 0; i < 1000000 ; i++)
	{
		line(13, 20, 80, 40, image, white);
		line(20, 13, 40, 80, image, red);
		line(80, 40, 13, 20, image, red);
	}
}

void DrawModel(std::string modelName, TGAImage &image)
{
	model = new Model(modelName);

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);		
        for (int j=0; j < face.size(); j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*ImageWidth/2.;
            int y0 = (v0.y+1.)*ImageHeight/2.;
            int x1 = (v1.x+1.)*ImageWidth/2.;
            int y1 = (v1.y+1.)*ImageHeight/2.;
            line(x0, y0, x1, y1, image, white);
        }
	}
}

int main(int argc, char *argv[])
{
	TGAImage image(ImageWidth, ImageHeight, TGAImage::RGB);

	std::string modelName = "obj/african_head.obj";	
	if( argc > 1 )
		modelName = argv[1];

	//DrawModel(modelName, image);

	triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);

	image.flip_vertically();
	image.write_tga_file("output.tga",false);
    return 0;
}

