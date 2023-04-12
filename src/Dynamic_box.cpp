#include "stdafx.h"
#include "Dynamic_box.h"

#include "utilities.h"

Dynamic_box::Dynamic_box(Box b, double m) :Box(b), mass(m) {
	moment_inertia = mass / 12.0 * size.length_squared();

	for (int i = 0; i < 4; i++) {
		corners[i] = { size * (-0.5) + (rect_corners[i] * size) };
	}
}

Dynamic_box::~Dynamic_box()
{
}

void Dynamic_box::add_force(v2 gpos, Force force){
	v2 lpos = (gpos - pos).rotated(-rotation);
	forces.push_back({ lpos, force });
}

Vector Dynamic_box::get_matrix_row(int corner_number)
{
	Vector ans(Force::lengths.size(), 0.0);

	v2 normal = corners[corner_number].normal;
	v2 local_pos = corners[corner_number].local_pos;

	for (auto applied_force : forces)
	{
		Force& force = applied_force.second;
		v2 force_pos = applied_force.first;
		
		double linear = normal.dot(force.direction) / mass;

		double rotation = normal.dot( force_pos.cross(force.direction.cross(local_pos)) ) / moment_inertia;
		
		ans[force.length_id] = linear + rotation;
	}
	return ans;
}
