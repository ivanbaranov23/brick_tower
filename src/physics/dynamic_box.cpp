//#include "stdafx.h"
#include "dynamic_box.h"

#include "../utilities.h"



Dynamic_box::Dynamic_box(Box b, double m) :Box(b), mass(m) {
	moment_inertia = mass / 12.0 * size.length_squared();

	for (int i = 0; i < 4; i++) {
		corners[i] = { size * (-0.5) + (rect_corners[i] * size), this };
		corners[i].global = pos + (size * (-0.5) + (rect_corners[i] * size)).rotated(rotation);
	}
}

Dynamic_box::~Dynamic_box()
{
}

void Dynamic_box::add_force(v2 gpos, Force force){
	v2 lpos = to_local(gpos);
	forces.push_back({ lpos, force });
}
void Dynamic_box::add_force_local_pos(v2 lpos, Force force){
	forces.push_back({ lpos, force });
}

Vector Dynamic_box::get_matrix_row(v2 normal, v2 g_pos, double rot)
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

		//cout << "force " << force.length_id << endl;
		v2 torque = force_pos.cross(force.direction);
		//t = r x F
		v2 angular_accel = v2();
		angular_accel.z = torque.z / moment_inertia;
		//aa = t / I
		//cout << "torque " << torque.x << " " << torque.y << " " << torque.z << endl;
		//al = aa x r
		v2 linear_accel_from_rotation = angular_accel.cross(local_pos);
		double rotation = normal.dot( linear_accel_from_rotation );
		
		//v2 aa = force_pos.cross(force.direction).cross(local_pos) / moment_inertia;
		//v2 test = force_pos.cross(force.direction).cross(force_pos) / moment_inertia;
		
		//cout << normal.dot(aa) << " " << (force_pos.length_squared() - (normal.dot(force_pos))*(normal.dot(force_pos)))/ moment_inertia << endl;
		//cout << "x " << force_pos.x << " " << local_pos.x << endl;
		//cout << "y " << force_pos.y << " " << local_pos.y << endl;
		//cout << normal.dot(test) << endl << endl;

		ans[force.length_id] += linear + rotation * rot;
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

		v2 torque = force_pos.cross(force.direction);
		//t = r x F
		v2 angular_accel = torque / moment_inertia;
		//aa = t / I

		//al = aa x r
		v2 rotation = angular_accel.cross(local_pos);
		
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

		v2 torque = force_pos.cross(force.direction);
		//t = r x F
		v2 angular_accel = torque / moment_inertia;
		//aa = t / I

		//al = aa x r
		v2 linear_accel_from_rotation = angular_accel.cross(local_pos);
		//double rotation = normal.dot( linear_accel_from_rotation );
		
		ans += linear + linear_accel_from_rotation;
	}
	return ans;
}

Force* Dynamic_box::get_last_force(){
	return &forces[forces.size() - 1].second;
}