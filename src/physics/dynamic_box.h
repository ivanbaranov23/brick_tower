#pragma once
#include "../math/box.h"
#include "force.h"
#include <vector>
#include <utility>

typedef std::vector<double> Vector;

struct Dynamic_box;

struct Box_corner {
	v2 local_pos;
	Dynamic_box* parent;
	Dynamic_box* colliding = nullptr;
	v2 normal = v2();
	int id = -1;
	
	v2 global();
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
	void add_force_local_pos(v2 lpos, Force force);
	Vector get_matrix_row(v2 normal, v2 g_pos);

	v2 get_total_force();
	double get_total_rotation();
	v2 get_total_difference_at_corner(int k);
	v2 get_acceleration(v2 gpos);

	Force* get_last_force();
};

