#include "stdafx.h"
#include "matrix.h"

Matrix::Matrix()
{
	w = h = 0;
}

void Matrix::set_up_as_Identity(int n) {
	w = h = n;
	values.clear();
	for (int i = 0; i < n; i++) {
		values.push_back(vector<double>());
		for (int j = 0; j < n; j++) {
			values[i].push_back(double(i == j));
		}
	}
}

void Matrix::set_col(int ind, Vector col)
{
	for (int i = 0; i < h; i++) {
		values[ind][i] = col[i];
	}
}

Vector Matrix::get_col(int ind)
{
	Vector ans;
	for (int i = 0; i < h; i++) {
		ans.push_back(values[ind][i]);
	}
	return ans;
}


void Matrix::set_row(int ind, Vector row)
{
	for (int i = 0; i < w; i++) {
		values[i][ind] = row[i];
	}
}


void Matrix::swap_row(int id1, int id2)
{
	for (int i = 0; i < w; i++) {
		double k = values[i][id1];
		values[i][id1] = values[i][id2];
		values[i][id2] = k;
	}
}

Vector Matrix::operator*(Vector v)
{
	Vector ans;
	for (int i = 0; i < h; i++) {
		ans.push_back(0.0);
		for (int j = 0; j < w; j++) {
			ans[i] += values[j][i] * v[j];
		}
	}
	return ans;
}

void Matrix::operator*=(double s)
{
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			values[i][j] *= s;
		}
	}
}
