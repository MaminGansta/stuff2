
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
	int top = 0, bottom = 0;

	Fraction() = default;
	Fraction(int t) : top(t), bottom(1) {}
	Fraction(const Fraction& f) : top(f.top), bottom(f.bottom) {}
	Fraction(int t, int b)
	{
		int g = gcd(t, b);
		top = t / g;
		bottom = b / g;
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
		return Fraction(temp_top / g, temp_bottom / g);
	}

	Fraction operator- (Fraction f)
	{
		int l = lcm(bottom, f.bottom);
		int t1 = top * (float(l) / bottom);
		int t2 = f.top * (float(l) / f.bottom);
		int temp_top = t1 - t2;
		int temp_bottom = l;

		int g = gcd(temp_top, temp_bottom);
		return Fraction(temp_top / g, temp_bottom / g);
	}


	Fraction operator* (Fraction other)
	{
		return Fraction(top * other.top, bottom * other.bottom);
	}

	Fraction operator/ (Fraction other)
	{
		int temp_top = top * other.bottom;
		int temp_bottom = bottom * other.top;

		int g = gcd(temp_top, temp_bottom);
		return Fraction(temp_top / g, temp_bottom / g);
	}

	Fraction operator-()
	{
		return Fraction(-top, -bottom);
	}

	bool operator< (Fraction f)
	{
		return top * f.bottom < f.top* bottom;
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
void output(wchar_t buffer[], T a, T b);

template <>
void output<float>(wchar_t buffer[], float a, float b)
{
	swprintf_s(buffer, 32, L"X = (%.4f, %.4f)", a, b);
}

template <>
void output<Fraction>(wchar_t buffer[], Fraction a, Fraction b)
{
	swprintf_s(buffer, 32, L"X = (%d/%d, %d/%d)", a.top, a.bottom, b.top, b.bottom);
}



template <typename T>
struct Mat
{
	T* mat;
	size_t row, column;


	Mat() : mat(nullptr), row(0), column(0) {}
	Mat(size_t row, size_t column) : row(row), column(column) {
		mat = new T[row * column];
	}

	~Mat() {
		delete[] mat;
	}

	Mat(const Mat<T>& other)
	{
		row = other.row;
		column = other.column;
		mat = new T[row * column];
		memmove(mat, other.mat, sizeof(T) * row * column);
	}
	
	Mat(Mat<T>&& other)
	{
		mat = other.mat;
		row = other.row;
		column = other.column;
		other.mat = nullptr;
	}
	
	Mat<T>& operator= (Mat<T>&& other)
	{
		delete[] mat;
		mat = other.mat;
		row = other.row;
		column = other.column;
		other.mat = nullptr;
		return *this;
	}

	T* operator [] (size_t i)
	{
		assert(i < row);
		return mat + i * column;
	}

	

};

typedef Mat<float> Matf;
typedef Mat<Fraction> Mat_fraction;