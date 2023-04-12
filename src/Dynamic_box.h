#pragma once
#include "box.h"
#include "force.h"
#include <vector>
#include <utility>

typedef std::vector<double> Vector;

struct Box_corner {
	v2 local_pos;
	v2 normal = v2();
	int id = -1;
};

struct Dynamic_box: public Box
{
	double mass;
	double moment_inertia;

	Box_corner corners[4];

	bool dynamic = true;

	std::vector<std::pair<v2, Force>> forces;
	
	//int corners[4] = { -1, -1, -1, -1 };

	double angular_velocity = 0;
	double total_force_moment = 0;

	Dynamic_box(Box b, double m);
	~Dynamic_box();

	void add_force(v2 gpos, Force force);
	Vector get_matrix_row(int corner_number);

};

