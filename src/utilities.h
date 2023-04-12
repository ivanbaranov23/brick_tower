#pragma once

//includes
#include "v2.h"

#include <cmath>
#include <vector>
#include <iostream>

//consts
const double pi = 3.14159265358979323846;
const v2 gravity_v{ 0, 9.8 };

//v2 consts
const v2 v2_LEFT = v2(-1.0, 0.0);
const v2 v2_RIGHT = v2(1.0, 0.0);
const v2 v2_UP = v2(0.0, -1.0);
const v2 v2_DOWN = v2(0.0, 1.0);

const v2 v2_ONE = v2(1.0, 1.0);
const v2 v2_ZERO = v2(0.0, 0.0);

const v2 rect_corners[4] = {
	v2_ZERO,
	v2_RIGHT,
	v2_ONE,
	v2_DOWN
};

const v2 normals[4] = {
	v2_UP,
	v2_RIGHT,
	v2_DOWN,
	v2_LEFT
};

//usings
using std::vector;
using std::cout;
using std::endl;