#include <iostream>
#include <fstream>
#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"
#include <random>

using namespace std;

Vec3 RandomInUnitSphere()
{
	Vec3 p;
	do {
		p = 2.0f * Vec3((rand()%100 / float(100)), (rand() % 100 / float(100)), (rand() % 100 / float(100))) - Vec3(1.0f, 1.0f, 1.0f);
	} while (dot(p, p) >= 1.0f);

	return p;
}

Vec3 Color(const Ray& r, Hitable *world)
{
	hit_record rec;
	if (world->hit(r, 0.0, FLT_MAX, rec))
	{
		Vec3 target = rec.p + rec.normal + RandomInUnitSphere();
		//递归，每次吸收50%的能量
		return 0.5f*Color(Ray(rec.p, target - rec.p), world);
		//return Color(Ray(rec.p, target - rec.p), world);
	}
	else
	{
		//绘制背景
		Vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f*(unit_direction.y() + 1.0f);
		//(1-t)*白色+t*蓝色，结果是一个蓝白的渐变
		return (1.0f - t)*Vec3(1.0f, 1.0f, 1.0f) + t*Vec3(0.5f, 0.7f, 1.0f);
	}
}

int main()
{

	ofstream outfile;
	outfile.open("ch7Image.ppm");

	int nx = 200;
	int ny = 100;
	//采样次数
	int ns = 100;
	outfile << "P3\n" << nx << " " << ny << "\n255\n";

	Hitable *list[2];
	list[0] = new Sphere(Vec3(0.0f, 0.0f, -1.0f), 0.5f);
	list[1] = new Sphere(Vec3(0.0f, -100.5f, -1.0f), 100.0f);
	Hitable *world = new HitableList(list, 2);

	Camera cam;

	//随机数引擎
	default_random_engine reng;
	uniform_real_distribution<float> uni_dist(0.0f, 1.0f);

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			Vec3 col(0.0f, 0.0f, 0.0f);
			//每个区域采样ns次
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + uni_dist(reng)) / float(nx);
				float v = float(j + uni_dist(reng)) / float(ny);
				Ray r = cam.getRay(u,v);
				//Vec3 p = r.point_at_parameter(2.0);
				//将本区域（(u,v)到(u+1,v+1)）的颜色值累加
				col += Color(r, world);
			}
			//获得区域的颜色均值
			col /= float(ns);
			//gamma矫正
			col = Vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);
			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}
	outfile.close();
	return 0;
}