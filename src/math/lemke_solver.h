#pragma once
#include <vector>
#include "matrix.h"

using namespace std;

typedef vector<double> Vector;




Vector operator+(Vector a, Vector b);
Vector operator-(Vector a, Vector b);

Vector operator/(Vector a, Vector b);

Vector operator*(Vector v, double a);

Vector select_sub_vector(Vector s, vector<int> w);

double operator*(Vector a, Vector b);

//minimaze fT * (A*f+b)
//f>=0
//A*f+b>=0
//for start f[i] = 0 for all i
bool solve_lemke(Matrix A, Vector b, Vector& f);
void finish_lemke(Matrix A, const Vector& b, const Vector& x, Vector& f, vector<int>& basis);
//solve Ax=b
Vector solve_linear(Matrix& A, Vector b);


const bool log_result = false;
const bool log_intermediate = false;
const double zero_tolerance = 1e-7;