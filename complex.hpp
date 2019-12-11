// Artem Mikheev 2019
// MIT License

#ifndef COMPLEX_HPP
#define COMPLEX_HPP

// Complex numbers class with template for convenience

template <typename T>
class Complex {
	T a, b;
	Complex __rec(const Complex &v) {
		T tmp = v.a*v.a+v.b*v.b;
		return Complex(v.a/tmp, -v.b/tmp);
	}
public:
	Complex()
	: a(0),
	  b(0) {}

	Complex(T t_a, T t_b)
	: a(t_a),
	  b(t_b) {

	}

	T real() {
		return a;
	}

	T imag() {
		return b;
	}

	Complex operator+(const Complex & other) {
		return Complex(this->a + other.a, this->b + other.b);
	}

	Complex& operator+=(const Complex & other) {
		this->a += other.a;
		this->b += other.b;
		return *this;
	}

	Complex operator-(const Complex & other) {
		return Complex(this->a - other.a, this->b - other.b);
	}

	Complex& operator-=(const Complex & other) {
		this->a -= other.a;
		this->b -= other.b;
		return *this;
	}

	Complex operator*(const Complex & other) {
		return Complex(this->a*other.a - this->b*other.b, this->a*other.b + this->b*other.a);
	}

	Complex& operator*=(const Complex & other) {
		this->a = this->a*other.a - this->b*other.b;
		this->b = this->a*other.b + this->b*other.a;
		return *this;
	}

	const Complex recip() {
		T tmp = a*a+b*b;
		return Complex(a/tmp, -b/tmp);
	}

	const Complex conj() {
		return Complex(a, -b);
	}

	Complex operator/(const Complex & other) {
		return operator*(__rec(other));
	}

	Complex& operator/=(const Complex & other) {
		this = operator*(__rec(other));
		return *this;
	}
};

#endif //COMPLEX_HPP
