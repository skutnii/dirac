/*
 * Rational.hpp
 *
 *	Rational number arithmetic
 *
 *  Created on: Dec 28, 2022
 *      Author: skutnii
 */

#ifndef SRC_ALGEBRA_RATIONAL_HPP_
#define SRC_ALGEBRA_RATIONAL_HPP_

#include <cstdlib>
#include <algorithm>

namespace dirac {

namespace algebra {

/**
 * Naive rational number implementation.
 * Does not care about overflows
 * and can be used in not too complex computations.
 */
class Rational {
public:
	/**
	 * Numerator
	 */
	long long int num() const { return _num; };

	/**
	 * Denominator
	 */
	unsigned long long int den() const { return _den; };

	/**
	 * Absolute value of the numerator
	 */
	unsigned long long int absNum() const {
		return static_cast<unsigned long long int>(std::abs(_num));
	}

	Rational() : _num{ 0 }, _den{ 1 } {}
	Rational(long long int n, unsigned long long int d)
		: _num{ n }, _den{ d } { normalize(); }

	Rational(long long int n) : Rational{ n, 1 } {}
	Rational(int n) : Rational{ n, 1 } {}

	Rational(const Rational& other) :
		_num{ other._num }, _den{ other._den } {};
	Rational(Rational&& other) :
		_num{ other._num }, _den{ other._den } {};

	Rational& operator=(const Rational& other) {
		_num = other._num;
		_den = other._den;
		return *this;
	};

	/**
	 * Whether the callee is a valid finite rational number.
	 */
	bool finite() const { return (_den != 0); }

	/**
	 * Equality check
	 */
	bool operator==(const Rational& other) const {
		if (!finite() || !other.finite())
			return false;

		return (other._den * _num == _den * other._num);
	}

	/**
	 * Addition
	 */
	Rational operator+(const Rational& r2) const {
		return Rational{
			_num * static_cast<long long int>(r2._den)
					+ r2._num * static_cast<long long int>(_den),
			_den * r2._den };
	}

	/**
	 * Subtraction
	 */
	Rational operator-(const Rational& r2) const {
		return Rational{
			_num * static_cast<long long int>(r2._den)
					- r2._num * static_cast<long long int>(_den),
			_den * r2._den };
	}

	/**
	 * Multiplication
	 */
	Rational operator*(const Rational& r2) const {
		return Rational{ _num * r2._num, _den * r2._den };
	}

	/**
	 * Division
	 */
	Rational operator/(const Rational& r2) const {
		if (r2._num < 0)
			return Rational{
					-_num * static_cast<long long int>(r2._den),
					_den * r2.absNum() };

		return Rational{
			_num * static_cast<long long int>(r2._den),
			_den * r2._num };
	}

	/**
	 * Negation (unary minus)
	 */
	Rational operator-() const {
		Rational res;
		res._num = -_num;
		res._den = _den;
		return res;
	}

	/**
	 * Greater-than operator
	 */
	bool operator>(const Rational& other) const {
		if (!finite() || !other.finite())
			return false;

		return (num() * static_cast<long long int>(other.den())
				> other.num() * static_cast<long long int>(den()));
	}

	/**
	 * Lesser-than operator
	 */
	bool operator<(const Rational& other) const {
		if (!finite() || !other.finite())
			return false;

		return (num() * static_cast<long long int>(other.den())
				< other.num() * static_cast<long long int>(den()));
	}

	/**
	 * Mutating addition
	 */
	Rational& operator+=(const Rational& other) {
		if (other._num != 0) {
			_num = _num * static_cast<long long int>(other._den)
				+ other._num * static_cast<long long int>(_den);
			_den = _den * other._den;
			normalize();
		}

		return *this;
	}

	/**
	 * Mutating subtraction
	 */
	Rational& operator-=(const Rational& other) {
		if (other._num != 0) {
			_num = _num * static_cast<long long int>(other._den)
						- other._num * static_cast<long long int>(_den);
			_den = _den * other._den;
			normalize();
		}

		return *this;
	}

	/**
	 * Mutating multiplication
	 */
	Rational& operator*=(const Rational& other) {
		_num = _num * other._num;
		_den = _den * other._den;
		normalize();

		return *this;
	}

	/**
	 * Mutating division
	 */
	Rational& operator/=(const Rational& other) {
		_num = _num * static_cast<long long int>(other._den);
		_den = _den * static_cast<unsigned long long int>(other._num);
		normalize();

		return *this;
	}

private:
	/**
	 * Normalizes the number by removing common factors
	 * from the numerator and denominator
	 */
	void normalize();

	/**
	 * Removes the common factor specified by the argument
	 * from the numerator and denominator
	 */
	void removeFactor(unsigned long long int f);

	long long int _num;
	unsigned long long int _den;
};

}

}



#endif /* SRC_ALGEBRA_RATIONAL_HPP_ */
