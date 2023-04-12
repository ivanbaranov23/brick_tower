#pragma once
#include "my_image.h"
#include "box.h"
#include "force.h"

class Adv_image: public Image
{
public:
	using Image::Image;

	void fill(Pixel p);
	void draw_rect(Box& b, Pixel p);

	void draw_line(v2 p1, v2 p2, double width, Pixel p = { 0, 0, 0, 255 });
	void draw_arrow(v2 p1, v2 p2, double width, Pixel p = { 0, 0, 0, 255 });
	void draw_force(v2 g_pos, Force& f, double scale);
};

