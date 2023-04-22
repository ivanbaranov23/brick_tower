#pragma once
#include "my_image.h"
#include "../math/box.h"
#include "../physics/force.h"

class Adv_image: public Image
{
public:
	using Image::Image;

	void fill(Pixel p);
	void draw_rect(Box& b, Pixel p);

	void draw_line(v2 p1, v2 p2, double width, Pixel p = { 0, 0, 0, 255 });
	void draw_arrow(v2 p1, v2 p2, double width, Pixel p = { 0, 0, 0, 255 });
	void draw_force(v2 g_pos, Force& f, double scale);

	void draw_circle(v2 center, int dir, Pixel p = { 0, 0, 0, 255 });
};

