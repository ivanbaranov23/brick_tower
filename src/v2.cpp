#include "stdafx.h"
#include "v2.h"
#include <cmath>

v2::v2():v2(0.0, 0.0)
{
}

v2::v2(double x, double y):x(x), y(y), z(0.0){}

v2::~v2(){}

double v2::length() const
{
	return sqrt(x * x + y * y);
}

double v2::length_squared() const
{
	return x * x + y * y;
}

double v2::angle() const
{
	return atan2(x, y);
}

v2 v2::operator+(v2 a) const
{
	return v2(x + a.x, y + a.y);
}

void v2::operator+=(v2 a)
{
	x += a.x;
	y += a.y;
}

v2 v2::operator-(v2 a) const
{
	return v2(x - a.x, y - a.y);
}

void v2::operator-=(v2 a)
{
	x -= a.x;
	y -= a.y;
}

v2 v2::operator*(v2 a) const
{
	return v2(x * a.x, y * a.y);
}

void v2::operator*=(v2 a)
{
	x *= a.x;
	y *= a.y;
}

v2 v2::operator/(v2 a) const
{
	return v2(x / a.x, y / a.y);
}

void v2::operator/=(v2 a)
{
	x /= a.x;
	y /= a.y;
}

v2 v2::operator*(double s) const
{
	return v2(x * s, y * s);
}

void v2::operator*=(double s)
{
	x *= s;
	y *= s;
}

v2 v2::operator/(double s) const
{
	return v2(x / s, y / s);
}

void v2::operator/=(double s)
{
	x /= s;
	y /= s;
}

double v2::dot(v2 a) const
{
	return x * a.x + y * a.y + z * a.z;
}

v2 v2::cross(v2 a) const{
	v2 ans;
	ans.x = y * a.z - z * a.y;
	ans.y = z * a.x - x * a.z;
	ans.z = x * a.y - y * a.x;
	return ans;
}

v2 v2::normalized() const
{
	if (*this)
		return (*this) / length();
	return v2();
}

v2 v2::rotated(double a) const
{
	return v2(x * cos(a) - y * sin(a), x * sin(a) + y * cos(a));
}

v2::operator bool() const
{
	return (abs(x) > 1e-7) || (abs(y) > 1e-7);
}
