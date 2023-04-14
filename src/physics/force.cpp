//#include "stdafx.h"
#include "force.h"

std::vector<double> Force::lengths = {};

Force::Force(v2 dir):direction(dir){
	found = false;
}

Force::~Force()
{
}


v2 Force::get(){
	return direction.normalized() * lengths[length_id];
}

void Force::assign_length(int n)
{
	length_id = n;
}

void Force::add_new_length(double def_length)
{
	length_id = lengths.size();
	lengths.push_back(def_length);
}

void Force::make_opposite_to(Force & f)
{
	direction = f.direction * (-1.0);
	if (f.length_id == -1)
		f.add_new_length();
	assign_length(f.length_id);
}
