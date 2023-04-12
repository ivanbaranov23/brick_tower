#pragma once
#include <vector>

using std::vector;
typedef vector<double> Vector;

class Matrix
{
public:
	vector<vector<double>> values;
	int w, h;

	Matrix();

	void set_up_as_Identity(int n);

	void set_col(int ind, Vector col);
	Vector get_col(int ind);

	void set_row(int ind, Vector row);

	//void swap_col(int id1, int id2);
	void swap_row(int id1, int id2);

	Vector operator*(Vector v);
	void operator*=(double s);
};

