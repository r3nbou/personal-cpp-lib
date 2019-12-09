// Artem Mikheev 2019
// MIT License

#include "math.hpp"
#include "algorithm.hpp"
#include <stdexcept>

#ifndef VECTOR_HPP
#define VECTOR_HPP

// Vector class implemented on C-style arrays with growth based on powers of 2

typedef unsigned int sizeT;

template<typename T>
class Vector {
	const sizeT _defaultStartSize = 32;
	T * _baseArray = nullptr;
	sizeT _currentSize = 0;
	sizeT _currentMaxSize = 0;
public:
	Vector()
	:  _currentSize(0),
	  _currentMaxSize(_defaultStartSize)
	{
		_baseArray = new T[_currentMaxSize];
	}

	Vector(std::initializer_list<T> values)
		: _currentSize(values.size())
	{
		_currentMaxSize = (_currentSize & (-_currentSize)) == _currentSize
							? _currentSize
							: roundToNextPowerOfTwo(_currentSize);
		_baseArray = new T[_currentMaxSize];
		
		auto it = values.begin();
		for (sizeT i = 0;i < values.size();++i) {
			_baseArray[i] = *it++;
		}
	}

	template <sizeT n> Vector(const T (&arr)[n])
	{
		_currentSize = n;
		_currentMaxSize = (n & (-n)) == n ? n : Math::roundToNextPowerOfTwo(n);
		_baseArray = new T[_currentMaxSize];
		copy(arr, _baseArray, n);
	}

	Vector(sizeT t_vectorSize)
	: _currentSize(t_vectorSize)
	{
		_currentMaxSize = ((t_vectorSize & (-t_vectorSize)) == t_vectorSize
							? t_vectorSize
							: Math::roundToNextPowerOfTwo(t_vectorSize)),
		_baseArray = new T[_currentMaxSize];
	}

	Vector(sizeT t_vectorSize, T t_default)
	: _currentSize(t_vectorSize)
	{
		_currentMaxSize = ((t_vectorSize & (-t_vectorSize)) == t_vectorSize
							? t_vectorSize
							: Math::roundToNextPowerOfTwo(t_vectorSize)),
		_baseArray = new T[_currentMaxSize];
        fill(_baseArray, _currentMaxSize, t_default);
	}

	~Vector()
	{
		if (_baseArray != nullptr) {
			_currentSize = 0;
			_currentMaxSize = 0;
			delete[] _baseArray;
		}
	}

	Vector(const Vector & other)
	: _currentSize(other._currentSize)
	, _currentMaxSize(other._currentMaxSize)
	{
		if (_baseArray != nullptr)
			delete [] _baseArray;
		_baseArray = new T[_currentMaxSize];
		copy(other._baseArray, _baseArray, _currentSize);
	}

	Vector& operator=(const Vector & other)
	{
		_currentMaxSize = other._currentMaxSize;
		_currentSize = other._currentSize;
		if (_baseArray != nullptr)
			delete [] _baseArray;
		_baseArray = new T[_currentMaxSize];
		copy(other._baseArray, _baseArray, _currentSize);
		return *this;
	}

	T& operator[](sizeT index) const
	{
		if (index < 0 || index >= _currentSize) throw std::runtime_error("Accessing element at invalid location in vector.");
		return _baseArray[index];
	}

	T at(sizeT index) const
	{
		if (index < 0 || index >= _currentSize) throw std::runtime_error("Accessing element at invalid location in vector.");
		return _baseArray[index];
	}

	Vector operator+(const Vector & other)
	{
		Vector result(_currentSize+other._currentSize);
		copy(_baseArray, result._baseArray, _currentSize);
		copy(other._baseArray, result._baseArray+_currentSize, other._currentSize);
		return result;
	}

	Vector& operator+=(const Vector &other)
	{
		Vector result = *this+other;
		*this = result;
		return *this;
	}

	bool operator==(const Vector &other)
	{
		if (_currentSize != other._currentSize) return false;
		for (sizeT i = 0; i < _currentSize; i++)
			if (_baseArray[i] != other._baseArray[i]) return false;
		return true;
	}

	sizeT size() const
	{
		return _currentSize;
	}

	void resize(sizeT t_n)
	{
		Vector result(t_n);
		if (t_n < _currentSize)
			copy(_baseArray, result._baseArray, t_n);
		else
			copy(_baseArray, result._baseArray, _currentSize);
		result._currentSize = t_n;
		*this = result;
	}

	void clear()
	{
		if (_baseArray != nullptr) {
			_currentSize = 0;
			_currentMaxSize = 0;
			delete[] _baseArray;
		}
	}

	void push(T t_value)
	{
		if (_currentMaxSize == 0)
			Vector();
		if (_currentSize == _currentMaxSize)
			resize(_currentMaxSize*2);
		_baseArray[_currentSize++] = t_value;
	}

	void pop()
	{
		if (_currentSize == 0) throw std::runtime_error("Trying to pop element from empty vector.");
		_currentSize--;
		_baseArray[_currentSize] = T();
	}
};

#endif