#pragma once
#include "../math/v2.h"
#include <vector>

struct Box_corner;

class Force
{
public:
	bool found;
	int length_id = -1;
	static std::vector<double> lengths;

	Box_corner* parent = nullptr;
	Force* opposite_force = nullptr;

	v2 direction;
	bool static_force = false;

	Force(v2 dir);
	~Force();

	v2 get();

	void assign_length(int n);
	void add_new_length(double def_length = 0.0);
	void make_opposite_to(Force &f);
	void set_friction(v2 mu);
};
