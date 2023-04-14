#pragma once
#include "../math/v2.h"
#include <vector>

class Force
{
public:
	bool found;
	int length_id = -1;
	static std::vector<double> lengths;

	v2 direction;

	Force(v2 dir);
	~Force();

	v2 get();

	void assign_length(int n);
	void add_new_length(double def_length = 0.0);
	void make_opposite_to(Force &f);
};
