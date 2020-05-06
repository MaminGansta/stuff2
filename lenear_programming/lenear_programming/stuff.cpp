
// great common divisor
int gcd(int a, int b)
{
	while (b)
	{
		a %= b;
		std::swap(a, b);
	}
	return a;
}

// least common multiple
int lcm(int a, int b)
{
	return abs(a * b) / gcd(a, b);
}


struct Fraction
{
	int top = 0, bottom = 1;

	Fraction() = default;
	Fraction(int t) : top(t), bottom(1) {}
	Fraction(const Fraction& f) : top(f.top), bottom(f.bottom) {}
	Fraction(int t, int b)
	{
		int g = gcd(t, b);
		top = t / g;
		bottom = b / g;

		if (bottom < 0)
		{
			top = -top;
			bottom = -bottom;
		}
	}
	Fraction& operator= (const Fraction& f) { top = f.top;  bottom = f.bottom; return *this; }
	Fraction& operator-= (const Fraction& f)
	{
		int l = lcm(bottom, f.bottom);
		int t1 = top * (float(l) / bottom);
		int t2 = f.top * (float(l) / f.bottom);
		top = t1 - t2;
		bottom = l;

		int g = gcd(top, bottom);
		top /= g;
		bottom /= g;

		if (bottom < 0)
		{
			top = -top;
			bottom = -bottom;
		}

		return *this;
	}
	Fraction& operator+= (const Fraction& f)
	{
		int l = lcm(bottom, f.bottom);
		int t1 = top * (float(l) / bottom);
		int t2 = f.top * (float(l) / f.bottom);
		top = t1 + t2;
		bottom = l;

		int g = gcd(top, bottom);
		top /= g;
		bottom /= g;

		if (bottom < 0)
		{
			top = -top;
			bottom = -bottom;
		}

		return *this;
	}

	Fraction operator+ (Fraction f)
	{
		int l = lcm(bottom, f.bottom);
		int t1 = top * (float(l) / bottom);
		int t2 = f.top * (float(l) / f.bottom);
		int temp_top = t1 + t2;
		int temp_bottom = l;

		int g = gcd(temp_top, temp_bottom);
		
		Fraction res(temp_top / g, temp_bottom / g);
		if (res.bottom < 0)
		{
			res.top = -res.top;
			res.bottom = -res.bottom;
		}
		return res;
	}

	Fraction operator- (Fraction f)
	{
		int l = lcm(bottom, f.bottom);
		int t1 = top * (float(l) / bottom);
		int t2 = f.top * (float(l) / f.bottom);
		int temp_top = t1 - t2;
		int temp_bottom = l;

		int g = gcd(temp_top, temp_bottom);
		Fraction res(temp_top / g, temp_bottom / g);
		if (res.bottom < 0)
		{
			res.top = -res.top;
			res.bottom = -res.bottom;
		}
		return res;
	}


	Fraction operator* (Fraction other)
	{
		Fraction res(top * other.top, bottom * other.bottom);
		if (res.bottom < 0)
		{
			res.top = -res.top;
			res.bottom = -res.bottom;
		}
		return res;
	}

	Fraction operator/ (Fraction other)
	{
		int temp_top = top * other.bottom;
		int temp_bottom = bottom * other.top;

		if (temp_bottom == 0)
		{
			doutput("Fraction: zero division\n");
			temp_bottom = 1;
			temp_top = MAXINT32;
		}

		int g = gcd(temp_top, temp_bottom);
		Fraction f(temp_top / g, temp_bottom / g);
		if (f.bottom < 0)
		{
			f.top = -f.top;
			f.bottom = -f.bottom;
		}
		return f;
	}

	Fraction operator-()
	{
		return Fraction(-top, bottom);
	}

	bool operator< (Fraction f)
	{
		return top * f.bottom < f.top* bottom;
	}

	bool operator<= (Fraction f)
	{
		return top * f.bottom <= f.top* bottom;
	}

	bool operator== (Fraction f)
	{
		return top * f.bottom == f.top* bottom;
	}

	// int
	Fraction operator+ (int other)
	{
		return Fraction(top + other * bottom, bottom);
	}

	Fraction operator* (int other)
	{
		return Fraction(top * other, bottom);
	}

	bool operator== (int other)
	{
		return other * bottom == top;
	}

	bool operator< (int other)
	{
		return top < other * bottom;
	}

	bool operator> (int other)
	{
		return  top > other * bottom;
	}

	bool operator!= (int other)
	{
		return other * bottom != top;
	}

	operator int()
	{
		return top;
	}

	// float
	float operator* (float other)
	{
		return tof() * other;
	}

	float operator+ (float other)
	{
		return tof() + other;
	}


	bool operator> (float f)
	{
		return tof() > f;
	}

	float tof()
	{
		return float(top) / bottom;
	}
};


// output for float and Fraction

template <typename T>
int to_str(wchar_t buffer[], T a);

template <>
int to_str<int>(wchar_t buffer[], int a)
{
	return swprintf_s(buffer, 32, L"%d ", a);
}

template <>
int to_str<float>(wchar_t buffer[], float a)
{
	return swprintf_s(buffer, 32, L"%.1f ", a);
}

template <>
int to_str<Fraction>(wchar_t buffer[], Fraction a)
{
	if (a.bottom == 1)
		return swprintf_s(buffer, 32, L"%d ", a.top);
	else
		return swprintf_s(buffer, 32, L"%d/%d ", a.top, a.bottom);
}


// ============== Matrix ===================

template <typename T>
struct Mat
{
	T* mat;
	size_t row, column;
	size_t col_capacity;


	Mat() : mat(nullptr), row(0), column(0), col_capacity(0) {}
	Mat(size_t row, size_t column) : row(row), column(column), col_capacity(column)
	{
		mat = new T[row * column];
		for (int i = 0; i < column * row; i++)
			mat[i] = T();
	}

	~Mat()
	{
		delete[] mat;
	}

	Mat(const Mat<T>& other)
	{
		row = other.row;
		column = other.column;
		col_capacity = other.col_capacity;
		mat = new T[row * col_capacity];
		memmove(mat, other.mat, sizeof(T) * row * col_capacity);
	}
	
	Mat(Mat<T>&& other)
	{
		mat = other.mat;
		row = other.row;
		column = other.column;
		col_capacity = other.col_capacity;

		other.mat = nullptr;
	}
	
	Mat<T>& operator= (Mat<T>&& other)
	{
		delete[] mat;
		mat = other.mat;
		row = other.row;
		column = other.column;
		col_capacity = other.col_capacity;

		other.mat = nullptr;
		return *this;
	}

	T* operator [] (size_t i)
	{
		assert(i < row);
		return mat + i * col_capacity;
	}

	

};

typedef Mat<float> Matf;
typedef Mat<Fraction> Mat_fraction;