//#include "stdafx.h"
#include "adv_image.h"

#include "../utilities.h"

#include <algorithm>

void Adv_image::fill(Pixel p){
	for (int x = 0; x < w; x++){
		for (int y = 0; y < h; y++){
			set(x, y, p);
		}
	}
}

void Adv_image::draw_rect(Box& b, Pixel p){
	Box AABB = b.get_AABB();
	int left = std::max(AABB.pos.x - AABB.size.x / 2 - 1, 0.0);
	int right = std::min(AABB.pos.x + AABB.size.x / 2 + 1, double(w));
	int top = std::max(AABB.pos.y - AABB.size.y / 2 - 1, 0.0);
	int bottom = std::min(AABB.pos.y + AABB.size.y / 2 + 1, double(h));

	for (int x = left; x < right; x++) {
		for (int y = top; y < bottom; y++) {
			if (b.intersect(v2(x, y)))
				set(x, y, p);
		}
	}
}

void Adv_image::draw_line(v2 p1, v2 p2, double width, Pixel p)
{
	//abs(ax+by+c) < width * sqrt(a*a+b*b) / 2
	
	double a = p2.y - p1.y;
	double b = p1.x - p2.x;
	double c = (p2.x - p1.x) * p1.y - (p2.y - p1.y) * p1.x;
	double sloped_width = width * sqrt(a * a + b * b) / 2.0;

	int left = (p1.x < p2.x)?int(p1.x):int(p2.x);
	left -= width / 2;
	left = (left > 0) ? left : 0;

	int top = (p1.y < p2.y) ? int(p1.y) : int(p2.y);
	top -= width / 2;
	top = (top > 0) ? top : 0;

	int right = (p1.x > p2.x) ? int(p1.x) : int(p2.x);
	right += width / 2;
	right = (right < w) ? right : w;

	int bottom = (p1.y > p2.y) ? int(p1.y) : int(p2.y);
	bottom += width / 2;
	bottom = (bottom < h) ? bottom : h;

	for(int x = left; x < right; x++)
		for (int y = top; y < bottom; y++) {
			if (abs(a * x + b * y + c) < sloped_width)
				set(x, y, p);
		}
}


void Adv_image::draw_arrow(v2 p1, v2 p2, double width, Pixel p) {
	Box b(p1, v2(8, 8));
	draw_rect(b, p);

	v2 dir = p2 - p1;
	draw_line(p1, p2, width, p);
	draw_line(p2, p2 + dir.normalized().rotated(pi * 0.75) * 10.0, width, p);
	draw_line(p2, p2 + dir.normalized().rotated(-pi * 0.75) * 10.0, width, p);
}

void Adv_image::draw_force(v2 g_pos, Force& force, double scale) {
	draw_line(g_pos, g_pos + force.get() * scale, 2.0);

}