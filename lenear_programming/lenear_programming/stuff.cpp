
int gcd(int a, int b)
{
	while (b)
	{
		a %= b;
		std::swap(a, b);
	}

	return a;
}

struct Fraction
{
	int top = 0, bottom = 0;

	Fraction() = default;
	Fraction(int t) : top(t), bottom(t) {}
	Fraction(int t, int b) : top(t), bottom(b) {}
	Fraction(const Fraction& f) : top(f.top), bottom(f.bottom) {}
	Fraction& operator= (const Fraction& f) { top = f.top;  bottom = f.bottom; return *this; }
	Fraction& operator-= (const Fraction& f)
	{
		float  a = gcd(bottom, f.bottom);
		top *= (a / bottom) - f.top * (a / f.bottom);
		bottom = a;
		return *this;
	}

	Fraction operator+ (Fraction other)
	{
		float a = gcd(bottom, other.bottom);
		return Fraction( top * (a / bottom) + other.top * (a / other.bottom), a );
	}

	Fraction operator+ (int other)
	{
		return Fraction( top + other * bottom, bottom );
	}

	float operator+ (float other)
	{
		return tof() + other;
	}

	Fraction operator- (Fraction other)
	{
		// gcd
		float  a = gcd(bottom, other.bottom);
		return Fraction( top * (a / bottom) - other.top * (a / other.bottom), a );

	}

	Fraction operator* (Fraction other)
	{
		return Fraction( top * other.top, bottom * other.bottom );
	}

	Fraction operator/ (Fraction other)
	{
		return Fraction( top * other.bottom, bottom * other.top );
	}

	Fraction operator* (int other)
	{
		return Fraction( top * other, bottom );
	}

	float operator* (float other)
	{
		return tof() * other;
	}

	Fraction operator-()
	{
		return Fraction(-top, -bottom);
	}

	bool operator< (Fraction f)
	{
		return top * f.bottom < f.top* bottom;
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