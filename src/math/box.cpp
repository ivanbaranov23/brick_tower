//#include "stdafx.h"
#include "box.h"
#include <algorithm>

#include "../utilities.h"

v2 Box::to_local(v2 v)
{
	return (v - pos).rotated(-rotation);
}

v2 Box::to_global(v2 v)
{
	return pos + v.rotated(rotation);
}

Box::Box(double x, double y, double w, double h, double r):pos({x, y}), size({w, h}){
	rotation = r;
	//rotation  any
	while (rotation > pi / 2)
		rotation -= pi;
	while (rotation < -pi / 2)
		rotation += pi;
	//rotation  [-pi/2, pi/2]

	if (rotation > pi / 4.0) {
		rotation -= pi / 2.0;
		size = v2(h, w);
	}
	else if (rotation < -pi / 4.0) {
		rotation += pi / 2.0;
		size = v2(h, w);
	}
	//rotation  [-pi/4, pi/4]
}

Box::Box(v2 p, v2 s, double r):pos(p), size(s), rotation(r){
	
}

Box::~Box()
{
}

void Box::operator+=(v2 p)
{
	pos += p;
}

v2 Box::get_local_x()
{
	return v2(size.x, 0.0).rotated(rotation).normalized();
}

v2 Box::get_local_y()
{
	return v2(0.0, size.y).rotated(rotation).normalized();
}

v2 Box::get_corner(int i)
{
	return pos + (size / (-2.0) + rect_corners[i % 4] * size).rotated(rotation);
}

bool Box::intersect(Box & b)
{
	//check this
	v2 oX = b.get_local_x();
	v2 oY = b.get_local_y();
	
	double ominx = std::min({
		oX.dot(get_corner(0)),
		oX.dot(get_corner(1)),
		oX.dot(get_corner(2)),
		oX.dot(get_corner(3))
		});
	double omaxx = std::max({
		oX.dot(get_corner(0)),
		oX.dot(get_corner(1)),
		oX.dot(get_corner(2)),
		oX.dot(get_corner(3))
		});

	double ominy = std::min({
		oY.dot(get_corner(0)),
		oY.dot(get_corner(1)),
		oY.dot(get_corner(2)),
		oY.dot(get_corner(3))
		});
	double omaxy = std::max({
		oY.dot(get_corner(0)),
		oY.dot(get_corner(1)),
		oY.dot(get_corner(2)),
		oY.dot(get_corner(3))
		});

	//check other
	v2 tX = get_local_x();
	v2 tY = get_local_y();

	double tminx = std::min({
		tX.dot(b.get_corner(0)),
		tX.dot(b.get_corner(1)),
		tX.dot(b.get_corner(2)),
		tX.dot(b.get_corner(3))
		});
	double tmaxx = std::max({
		tX.dot(b.get_corner(0)),
		tX.dot(b.get_corner(1)),
		tX.dot(b.get_corner(2)),
		tX.dot(b.get_corner(3))
		});

	double tminy = std::min({
		tY.dot(b.get_corner(0)),
		tY.dot(b.get_corner(1)),
		tY.dot(b.get_corner(2)),
		tY.dot(b.get_corner(3))
		});
	double tmaxy = std::max({
		tY.dot(b.get_corner(0)),
		tY.dot(b.get_corner(1)),
		tY.dot(b.get_corner(2)),
		tY.dot(b.get_corner(3))
		});

	return (
		tminx <= omaxx &&
		ominx <= tmaxx &&
		tminy <= omaxy &&
		ominy <= tmaxy
		) && (
		ominx <= tmaxx &&
		tminx <= omaxx &&
		ominy <= tmaxy &&
		tminy <= omaxy);
}

bool Box::intersect(v2 v){
	v2 lv = to_local(v);

	return (lv.x >= -size.x / 2)&&(lv.x <= size.x / 2)&&(lv.y >= -size.y / 2)&&(lv.y <= size.y / 2);
}

v2 Box::get_normal(v2 v)
{
	v2 local_v = to_local(v);
	//cout << local_v.x << " " << local_v.y << endl;
	local_v /= size;
	//cout << local_v.x << " " << local_v.y << endl;
	local_v = local_v.normalized();
	
	//cout << local_v.x << " " << local_v.y << endl << endl;

	for (int i = 0; i < 4; i++) {
		if (normals[i].dot(local_v) > sqrt(2.0) / 2.0 - 1e-5)
			return normals[i].rotated(rotation);
	}

	return v2();
}

Box Box::get_AABB(){
	double left = std::min({get_corner(0).x, get_corner(1).x, get_corner(2).x, get_corner(3).x});
	double right = std::max({get_corner(0).x, get_corner(1).x, get_corner(2).x, get_corner(3).x});
	double top = std::min({get_corner(0).y, get_corner(1).y, get_corner(2).y, get_corner(3).y});
	double bottom = std::max({get_corner(0).y, get_corner(1).y, get_corner(2).y, get_corner(3).y});

	v2 AABB_pos = v2(left + right, top + bottom) / 2.0;
	v2 AABB_size = v2(right - left, bottom - top);
	return {AABB_pos, AABB_size};
}