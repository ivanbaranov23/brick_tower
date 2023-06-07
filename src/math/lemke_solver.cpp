//#include "stdafx.h"
#include "lemke_solver.h"

#include <string>
#include <iostream>
#include <algorithm>




Vector operator+(Vector a, Vector b)
{
	Vector ans = a;
	for (int i = 0; i < ans.size(); i++)
		ans[i] += b[i];
	return ans;
}

Vector operator-(Vector a, Vector b)
{
	Vector ans = a;
	for (int i = 0; i < ans.size(); i++)
		ans[i] -= b[i];
	return ans;
}

Vector operator/(Vector a, Vector b)
{
	Vector ans = a;
	for (int i = 0; i < ans.size(); i++)
		ans[i] /= b[i];
	return ans;
}

Vector operator*(Vector v, double a)
{
	Vector ans = v;
	for (int i = 0; i < ans.size(); i++)
		ans[i] *= a;
	return ans;
}

double operator*(Vector a, Vector b){
	double ans = 0.0;
	for (int i = 0; i < a.size(); i++)
		ans += a[i] * b[i];
	return ans;
}

Vector select_sub_vector(Vector s, vector<int> w)
{
	Vector ans;
	for (int i = 0; i < w.size(); i++)
		ans.push_back(s[w[i]]);
	return ans;
}


bool solve_lemke(Matrix A, Vector b, Vector & f)
{
	const int n = b.size();
	const int max_iteration = (1000 > 50 * n) ? (1000) : (50 * n);

	if (n == 0) {
		f.resize(0);
		return true;
	}

	Vector result, dj, dl, x, xj, Be, u, z0;
	Matrix Bl, t1, t2;

	vector<int> all;
	vector<int> basis;
	vector<int> nonbasis;


	z0 = f;
	for (int i = 0; i < n; i++) {
		f.push_back(0);
	}

	int t = 2 * n;
	int entering = t;
	int leaving = 0;
	for (int i = 0; i < n; i++) {
		all.push_back(i);
	}
	int lvindex;
	int idx;
	vector<int>::iterator iiter;

	for (int i = 0; i < n; i++) {
		if (z0[i] > 0) {
			basis.push_back(i);
		}
		else {
			nonbasis.push_back(i);
		}
	}


	Bl.set_up_as_Identity(n);
	Bl *= -1.0;
	x = b;

	//inintial leaving
	double min_x = 0;
	double min_x_val = x[0];
	for (int i = 0; i < n; i++) {
		if (x[i] < min_x_val) {
			min_x = i;
			min_x_val = x[i];
		}
	}
	double tval = -min_x_val;
	for (int i = 0; i < nonbasis.size(); i++) {
		basis.push_back(nonbasis[i] + n);
	}
	lvindex = min_x;
	iiter = basis.begin();
	advance(iiter, lvindex);
	leaving = *iiter;


	// pivot in the artificial variable
	*iiter = t; 
	u.resize(n);
	for (unsigned i = 0; i < n; i++) {
		u[i] = (x[i] < 0) ? 1 : 0;
	}
	Be = (Bl * u) * -1;
	u = u * tval;
	x = x + u;
	x[lvindex] = tval;
	Bl.set_col(lvindex, Be);


	vector<int> tlist;


	// main iterations begin here
	for (int pivots = 0; pivots < max_iteration; pivots++) {
		if (log_intermediate) {
			string str;
			for (unsigned i = 0; i < basis.size(); i++) {
				str += " " + to_string(basis[i]);
			}
			cout << "basic variables:" << str << std::endl;
			cout << "leaving: " << leaving << " t:" << t << std::endl;
		}

		// check whether done; if not, get new entering variable
		if (leaving == t) {
			//cout << "-- solved LCP successfully!" << std::endl;
			finish_lemke(A, b, x, f, basis);
			
			return true;
		}
		else if (leaving < n) {
			entering = n + leaving;
			Be.resize(n, 0.0);
			Be[leaving] = -1;
		}
		else {
			entering = leaving - n;
			Be = A.get_col(entering);
		}
		dl = Be;

		dl = solve_linear(Bl, dl);

		// find new leaving variable
		vector<int> j_;
		for (int i = 0; i < dl.size(); i++) {
			if (dl[i] > zero_tolerance) {
				j_.push_back(i);
			}
		}

		// check for no new pivots; ray termination
		if (j_.empty()) {
			//cout << "no new pivots (ray termination)" << std::endl;
			finish_lemke(A, b, x, f, basis);
			//f = Vector(n, 0);
			return false;
		}

		if (log_intermediate) {

			cout << "d:";
			for (int i = 0; i < dl.size(); i++)
				cout << " " << dl[i];
			cout << std::endl;

			cout << "j (before min ratio):";
			for (int i = 0; i < j_.size(); i++)
				cout << " " << j_[i];
			cout << std::endl;
		}

		// select elements j from x and d
		xj = select_sub_vector(x, j_);
		dj = select_sub_vector(dl, j_);

		
		result = Vector(xj.size());
		double theta = (xj[0] + zero_tolerance) / dj[0];

		
		for (int i = 0; i < result.size(); i++) {
			result[i] = (xj[i] + zero_tolerance) / dj[i];
			//cout << result[i] << " ";
			if (theta > result[i])
				theta = result[i];
			result[i] = xj[i] / dj[i];
		}
		//cout << endl << "theta " << theta << endl; 

		for (iiter = j_.begin(), idx = 0; iiter != j_.end();) {
			if (result[idx++] <= theta) {
				iiter++;
			}
			else {
				iiter = j_.erase(iiter);
			}
		}
		if (log_intermediate) {
			cout << "j (after min ratio):";
			for (unsigned i = 0; i < j_.size(); i++) {
				cout << " " << j_[i];
			}
			cout << std::endl;
		}

		// if j is empty, then likely the zero tolerance is too low
		if (j_.empty()) {
			cout << "zero tolerance too low" << std::endl;
			finish_lemke(A, b, x, f, basis);
			//f = Vector(n, 0.0);
			return false;
		}

		// check whether artificial index among these
		tlist.clear();
		for (int i = 0; i < j_.size(); i++) {
			tlist.push_back(basis[j_[i]]);
		}
		if (std::find(tlist.begin(), tlist.end(), t) != tlist.end()) {
			iiter = std::find(basis.begin(), basis.end(), t);
			lvindex = iiter - basis.begin();
		}
		else {
			lvindex = j_[0];
		}

		// set leaving = basis(lvindex)
		iiter = basis.begin();
		std::advance(iiter, lvindex);
		leaving = *iiter;

		// ** perform pivot
		double ratio = x[lvindex] / dl[lvindex];
		dl = dl * ratio;
		x = x - dl;
		x[lvindex] = ratio;
		Bl.set_col(lvindex, Be);
		*iiter = entering;
		//cout << " -- pivoting: leaving index=" << lvindex
		//	<< "  entering index=" << entering << std::endl;
	}

	finish_lemke(A, b, x, f, basis);

	return false;
}
void finish_lemke(Matrix A, const Vector& b, const Vector& x, Vector& f, vector<int>& basis) {
	
	vector<int>::iterator iiter;
	int idx;
	for (idx = 0, iiter = basis.begin(); iiter != basis.end(); iiter++, idx++) {
		f[*iiter] = x[idx];
	}

	
	f.resize(b.size());

	// check to see whether tolerances are satisfied
	if (log_intermediate) {
		Vector wl = (A * f) + b;
		double dot = 0.0;
		cout << "f\t\tAf+b" << endl;
		for (int i = 0; i < wl.size(); i++) {
			dot += wl[i] * f[i];
			cout << f[i] << "\t\t" << wl[i] << endl;
		}
		cout << "fT*(Af+b)=" << dot << endl;
	}
}


Vector solve_linear(Matrix & A, Vector b)
{
	// PA = LU
	// C = L + U - E
	
	const int n = A.w;

	Matrix C = A;

	Matrix P;
	P.set_up_as_Identity(n);

	for (int i = 0; i < n; i++) {
		
		double pivotValue = 0;
		int pivot = -1;
		for (int row = i; row < n; row++) {
			if (abs(C.values[i][row]) > pivotValue) {
				pivotValue = abs(C.values[i][row]);
				pivot = row;
			}
		}
		if (pivotValue != 0) {
			P.swap_row(pivot, i);
			C.swap_row(pivot, i);
			for (int j = i + 1; j < n; j++) {
				C.values[i][j] /= C.values[i][i];
				for (int k = i + 1; k < n; k++)
					C.values[k][j] -= C.values[i][j] * C.values[k][i];
			}
		}
	}


	// PA = LU
	// C = L + U - E
	// L -  1 0 0
	//      * 1 0
	//      * * 1
	//
	// U -  * * *
	//      0 * *
	//      0 0 *
	//
	// C -  U U U
	//      L U U
	//      L L U

	// Ax=b
	// PAx=Pb
	b = P * b;

	// LUx = Pb
	// Ly = Pb
	// y[0] = Pb[0]
	// y[1] = Pb[1] - L[0][1]*y[0]
	// y[i] = Pb[i] - (L[0-(i-1)][i]*y[0-(i-1)])
	Vector y;
	for (int i = 0; i < n; i++) {
		y.push_back(b[i]);
		for (int j = 0; j < i; j++) {
			y[i] -= C.values[j][i] * y[j];
		}
	}

	// Ux = y
	// x[n-1] = y[n-1] / U[n-1][n-1]
	// x[n-2] = (y[n-2] - x[n-1] * U[n-1][n-2]) / U[n-2][n-2]
	Vector x = Vector(n, 0.0);
	for (int i = n - 1; i >= 0; i--) {
		x[i] = y[i];
		for (int j = i + 1; j < n; j++) {
			x[i] -= C.values[j][i] * x[j];
		}
		x[i] /= C.values[i][i];
	}
	return x;
}

