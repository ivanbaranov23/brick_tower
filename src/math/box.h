#pragma once
#include "v2.h"

struct Box
{
	v2 pos;
	v2 size;
	double rotation;

	v2 to_local(v2 v);
	v2 to_global(v2 v);

	Box(double x, double y, double w, double h, double r = 0.0);
	Box(v2 p, v2 s, double r = 0.0);
	~Box();

	void operator+=(v2 p);

	v2 get_local_x();
	v2 get_local_y();

	//углы - 0---1
	//       |   |
	//       3---2
	v2 get_corner(int i);

	bool intersect(Box& b);
	bool intersect(v2 v);

	v2 get_normal(v2 v);

	Box get_AABB();
};

