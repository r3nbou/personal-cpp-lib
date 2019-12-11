// Artem Mikheev 2019
// MIT License

#include <stdexcept>
#include <cmath>

#ifndef MATH_HPP
#define MATH_HPP

namespace Math {
	const long double pi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
	const long double exp = 2.718281828459045235360287471352662497757247093699959574966967627724076630353;
	const long double eps = 1e-9;

	long long gcd(long long a, long long b) {
		// Find the gcd of two numbers using Euclid's algorithm
		long long tmp;
		while (b != 0) {
			tmp = b;
			b = a%b;
			a = tmp;
		}
		return a;
	}

	long long lcm(long long a, long long b) {
		// Find the lcm of two numbers using Euclid's algorithm
		return a*b/gcd(a, b);
	}

	long long binPow(long long t_base, long long t_power) {
		// Raise integer number to power using binary exponentiation
		if (t_power < 0)
			throw std::logic_error("Use realBinPow to raise to negative power.");
		if (t_power == 0)
			return 1LL;
		if (t_power == 1)
			return t_base;
		if (t_base == 1)
			return 1LL;
		if (t_base < 0 && t_power % 2)
			return -binPow(-t_base, t_power);
		long long result = 1;
		while (t_power) {
			if (t_power % 2)
				result *= t_base, t_power--;
			t_base *= t_base;
			t_power /= 2;
		}
		return result;
	}

	long double realBinPow(long double t_base, long double t_power);

	long double nthRoot(long double t_base, int t_n) {
		// Find nth root of a number using Newton's method
		if (t_n == 0)
			throw std::logic_error("Can't take root 0 of a number.");
		if (t_n == 1)
			return t_base;
		if (t_n < 0)
			return 1.0/nthRoot(t_base, -t_n);
		if (t_base < 0  && t_n%2==0)
			throw std::logic_error("Can't take even root of negative number.");
		long double baseX = t_base;
		for (int i = 0; i < 5; i++)
			baseX /= t_n;
		long double diff = 10;
		long double kthX;
		while (diff > 1e-9) {
			kthX = ((t_n - 1.0)*baseX + t_base/realBinPow(baseX, t_n-1.0))/(1.0*t_n);
			diff = abs(kthX-baseX);
			baseX = kthX;
		}
		return baseX;
	}

	long double realBinPow(long double t_base, long double t_power) {
		// Raise real number to power using binary exponentiation
		if (t_base < 0 && abs(t_power-floor(t_power)) > eps)
			throw std::logic_error("Cannot raise negative number to real power.");
		if (abs(t_power - 0.0) < eps)
			return 1;
		if (abs(t_base - 0.0) < eps) {
			if (t_power < 0)
				throw std::logic_error("Cannot raise 0 to negative power.");
			return 0;
		}
		if (abs(t_power - 1.0) < eps)
			return t_base;
		if (t_power < 0)
			return 1.0/realBinPow(t_base, -t_power);
		if (t_power < 1) {
			int powerToRaise = t_power*1000;
			int rootToTake = 1000;
			int gcdToDiv = gcd(powerToRaise, rootToTake);
			return nthRoot(realBinPow(t_base, powerToRaise/gcdToDiv), rootToTake/gcdToDiv);
		}
		if (abs(t_power-floor(t_power)) > eps)
			return realBinPow(t_base, floor(t_power))*realBinPow(t_base, t_power-floor(t_power));
		if (t_base < 0 && abs(fmod(t_power, 2.0)-1.0) < eps)
			return -realBinPow(-t_base, t_power);
		long double result = 1;
		while (t_power > 0) {
			if (abs(fmod(t_power, 2.0)-0.0) < eps) {
				t_power /= 2.0;
				t_base *= t_base;
			} else {
				t_power -= 1.0;
				result *= t_base;
			}
		}
		return result;
	}

	unsigned int roundToNextPowerOfTwo(unsigned int value) {
		// Round uint32 to next power of 2 based on bit operations
		value--;
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value++;
		return value;
	}

	long double ln(long double x) {
		// Calculate natural logarithm using its taylor series expansion
		if (x < 0 || abs(x-0.0) < eps)
			throw std::logic_error("Can't take logarithm of non-positive number.");
		long double taylorSeriesTerm = 1;
		long double taylorSeriesMult = (x-1)/(x+1);
		long double result = 1;
		long double diff = 10;
		long double divisor = 1;
		long double prev = result;
		while(diff > 1e-9) {
			taylorSeriesTerm *= taylorSeriesMult*taylorSeriesMult;
			divisor += 2;
			result += taylorSeriesTerm/divisor;
			diff = abs(result-prev);
			prev = result;
		}
		result *= 2.0*taylorSeriesMult;
		return result;
	}

	long double log(long double a, long double x) {
		// Calculate logarithm of number using natural logarithm
		if (abs(a-1.0) < eps)
			throw std::logic_error("Can't take logarithm with base equal to 1.");
		if (a < 0 || abs(a-0.0) < eps)
			throw std::logic_error("Can't take logarithm with non-positive base.");
		return ln(x)/ln(a);
	}

	long double sine(long double a) {
		// Calculate sine using its taylor series expansion
		a = fmod(a, 2*pi);
		long double taylorSeriesTerm = a;
		long double result = a;
		long double diff = 10;
		long double prev = result;
		long double iter = 1;
		while(diff > 1e-9) {
			taylorSeriesTerm *= (-1.0*a*a)/(2*iter*(2*iter+1));
			result += taylorSeriesTerm;
			iter += 1.0;
			diff = abs(result-prev);
			prev = result;
		}
		return result;
	}

	long double cosine(long double a) {
		// Calculate cosine using its taylor series expansion
		a = fmod(a, 2*pi);
		long double taylorSeriesTerm = 1;
		long double result = 1;
		long double diff = 10;
		long double prev = result;
		long double iter = 1;
		while(diff > 1e-9) {
			taylorSeriesTerm *= (-1.0*a*a)/(2*iter*(2*iter-1));
			result += taylorSeriesTerm;
			diff = abs(result-prev);
			prev = result;
			iter += 1.0;
		}
		return result;
	}

	long double tg(long double a) {
		// Calculate tangent using sine and cosine
		long double cos = cosine(a);
		if (abs(cos - 0) < eps)
			throw std::logic_error("Can't take tg of PI/2 or 3*PI/2");
		return sine(a)/cosine(a);
	}

	long double ctg(long double a) {
		// Calculate cotangent using sine and cosine
		long double sin = sine(a);
		if (abs(sin - 0) < eps)
			throw std::logic_error("Can't take tg of PI/2 or 3*PI/2");
		return cosine(a)/sine(a);
	}

	long double arcsine(long double a) {
		// Calculate arcsine using its taylor series expansion
		if (abs(a) > 1)
			throw std::logic_error("Can't get arcsine of number not in the range [-1;+1].");
		long double result = a;
		long double taylorSeriesTerm = 1;
		long double taylorTermAPower = 1;
		long double diff = 10;
		long double prev = result;
		long double iter = 1;
		while (diff > 1e-9) {
			taylorSeriesTerm *= (iter*2*(iter*2-1))/(4*iter*iter);
			taylorTermAPower *= a*a;
			result += (taylorSeriesTerm*taylorTermAPower*a)/(2*iter+1);
			iter += 1.0;
			diff = abs(result-prev);
			prev = result;
		}
		return result;
	}

	long double arccos(long double a) {
		// Calculate arccos using its taylor series expansion
		if (abs(a) > 1)
			throw std::logic_error("Can't get arccos of number not in the range [-1;+1].");
		return (pi/2 - arcsine(a));
	}

	long double arctg(long double a) {
		// Calculate arctangent using its taylor series expansion
		bool reversed = 0;
		if (abs(a) > 1) {
			a = 1.0/a;
			reversed = true;
		}
		long double taylorSeriesTerm = 1;
		long double taylorTermAPower = 1;
		long double result = a;
		long double prev = result;
		long double diff = 10;
		long double iter = 1;
		while (diff > 1e-9) {
			taylorSeriesTerm *= -1;
			taylorTermAPower *= a*a;
			result += (taylorSeriesTerm*taylorTermAPower*a)/(2*iter+1);
			iter += 1.0;
			diff = abs(result-prev);
			prev = result;
		}
		if (reversed)
			return (pi/2.0-result);
		return result;
	}

	long double arcctg(long double a) {
		// Calculate arccotangent using arctangent
		return (pi/2.0-arctg(a));
	}

}

#endif // MATH_HPP
