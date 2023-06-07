//#include "stdafx.h"
#include "dynamic_box.h"

#include "../utilities.h"


v2 Box_corner::global(){
	return parent->to_global(local_pos);
}


Dynamic_box::Dynamic_box(Box b, double m) :Box(b), mass(m) {
	moment_inertia = mass / 12.0 * size.length_squared();

	for (int i = 0; i < 4; i++) {
		corners[i] = { size * (-0.5) + (rect_corners[i] * size), this };
	}
}

Dynamic_box::~Dynamic_box()
{
}

void Dynamic_box::add_force(v2 gpos, Force force){
	v2 lpos = (gpos - pos).rotated(-rotation);
	forces.push_back({ lpos, force });
}
void Dynamic_box::add_force_local_pos(v2 lpos, Force force){
	forces.push_back({ lpos, force });
}

Vector Dynamic_box::get_matrix_row(v2 normal, v2 g_pos)
{
	Vector ans(Force::lengths.size(), 0.0);
	if (!dynamic)
		return ans;
	//v2 normal = corners[corner_number].normal;
	v2 local_pos = to_local(g_pos);

	for (auto applied_force : forces)
	{
		Force& force = applied_force.second;
		v2 force_pos = applied_force.first;
		if (force.static_force)
			continue;
		
		double linear = normal.dot(force.direction) / mass;

		double rotation = normal.dot( force_pos.cross(force.direction.cross(local_pos)) ) / moment_inertia;
		//cout << rotation << endl;
		
		ans[force.length_id] += linear + rotation;
	}
	return ans;
}

v2 Dynamic_box::get_total_force(){
	v2 total = v2();
	for (int i = 0; i < forces.size(); i++)
	{
		total += forces[i].second.get();
	}
	return total;
}
double Dynamic_box::get_total_rotation(){
	double total = 0.0;
	for (int i = 0; i < forces.size(); i++)
	{
		total += forces[i].first.cross(forces[i].second.get()).z;
	}
	return total / moment_inertia;
}

v2 Dynamic_box::get_total_difference_at_corner(int k){
	v2 ans = v2(0.0, 0.0);
	v2 local_pos = corners[k].local_pos;

	for (auto applied_force : forces)
	{
		Force& force = applied_force.second;
		v2 force_pos = applied_force.first;
		
		v2 linear = force.direction / mass;

		v2 rotation = force_pos.cross(force.direction.cross(local_pos)) / moment_inertia;
		
		ans += linear + rotation;
	}
	return ans;
}

v2 Dynamic_box::get_acceleration(v2 gpos){
	if (!dynamic)return v2();
	v2 ans = v2(0.0, 0.0);
	v2 local_pos = to_local(gpos);

	for (auto applied_force : forces)
	{
		Force& force = applied_force.second;
		v2 force_pos = applied_force.first;
		
		v2 linear = force.direction / mass;

		v2 rotation = force_pos.cross(force.direction.cross(local_pos)) / moment_inertia;
		
		ans += linear + rotation;
	}
	return ans;
}

Force* Dynamic_box::get_last_force(){
	return &forces[forces.size() - 1].second;
}