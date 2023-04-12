#pragma once

struct v2{
public:
	double x;
	double y;

	//for cross product only
	double z;

	v2();
	v2(double x, double y);
	~v2();

	double length() const;
	double length_squared() const;
	double angle() const;

	v2 operator+(v2 a) const;
	void operator+=(v2 a);
	
	v2 operator-(v2 a) const;
	void operator-=(v2 a);

	v2 operator*(v2 a) const;
	void operator*=(v2 a);

	v2 operator/(v2 a) const;
	void operator/=(v2 a);

	v2 operator*(double s) const;
	void operator*=(double s);

	v2 operator/(double s) const;
	void operator/=(double s);

	double dot(v2 a) const;
	v2 cross(v2 a) const;

	v2 normalized() const;
	v2 rotated(double a) const;

	operator bool() const;

};

