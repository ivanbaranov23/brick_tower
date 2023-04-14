#pragma once
#include "../math/box.h"
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

	Dynamic_box(Box b, double m);
	~Dynamic_box();

	void add_force(v2 gpos, Force force);
	Vector get_matrix_row(int corner_number);

	v2 get_total_force();
	double get_total_rotation();
};

