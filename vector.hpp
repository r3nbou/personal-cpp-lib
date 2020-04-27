// Artem Mikheev 2019
// GNU GPLv3 License

#include "math.hpp"
#include "algorithm.hpp"
#include "vartypes.hpp"
#include <stdexcept>
#include <utility>

#ifndef VECTOR_HPP
#define VECTOR_HPP

// Vector class implemented on C-style arrays with growth based on powers of 2
// Strange code explanations:
// t_vectorSize & (-t_vectorSize) ---- checks if the vector size is already a power of 2

template<typename T>
class Vector {
	static const sizeT _defaultStartSize = 32;
	T * _baseArray = nullptr;
	sizeT _currentSize = 0;
	sizeT _currentMaxSize = 0;
public:
	const sizeT *size = &_currentSize;

	Vector(std::initializer_list<T> values)
		: _currentSize(values.size())
	{
		_currentMaxSize = (_currentSize & (-_currentSize)) == _currentSize
							? _currentSize
							: Math::roundToNextPowerOfTwo(_currentSize);
		_baseArray = new T[_currentMaxSize];
		
		auto it = values.begin();
		for (sizeT i = 0; i < values.size(); i++) {
			_baseArray[i] = *it++;
		}
	}

	template <sizeT n> Vector(const T (&arr)[n])
	{
		_currentSize = n;
		_currentMaxSize = (n & (-n)) == n ? n : Math::roundToNextPowerOfTwo(n);
		_baseArray = new T[_currentMaxSize];
		Algorithm::copy(arr, _baseArray, n);
	}

	Vector(sizeT t_vectorSize = 0, T t_default = {})
	: _currentSize(t_vectorSize)
	{
		_currentMaxSize = Algorithm::max((t_vectorSize & (-t_vectorSize)) == t_vectorSize
							? t_vectorSize
							: Math::roundToNextPowerOfTwo(t_vectorSize), _defaultStartSize),
		_baseArray = new T[_currentMaxSize];
        Algorithm::fill(_baseArray, _currentMaxSize, t_default);
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
		Algorithm::copy(other._baseArray, _baseArray, _currentSize);
	}

	Vector& operator=(const Vector & other)
	{
		_currentMaxSize = other._currentMaxSize;
		_currentSize = other._currentSize;
		if (_baseArray != nullptr)
			delete [] _baseArray;
		_baseArray = new T[_currentMaxSize];
		Algorithm::copy(other._baseArray, _baseArray, _currentSize);
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

	T back() const
	{
		return _baseArray[_currentSize - 1];
	}

	Vector operator+(const Vector & other)
	{
		Vector result(_currentSize+other._currentSize);
		Algorithm::copy(_baseArray, result._baseArray, _currentSize);
		Algorithm::copy(other._baseArray, result._baseArray+_currentSize, other._currentSize);
		return result;
	}

	Vector& operator+=(const Vector &other)
	{
		int oldSize = _currentSize;
		resize(_currentSize + other._currentSize);
		for (int i = oldSize; i < _currentSize; i++)
			_baseArray[i] = other._baseArray[i];
		return *this;
	}

	bool operator==(const Vector &other)
	{
		if (_currentSize != other._currentSize) return false;
		for (sizeT i = 0; i < _currentSize; i++)
			if (_baseArray[i] != other._baseArray[i]) return false;
		return true;
	}

	void resize(sizeT t_n, T t_default = {})
	{
		if (t_n > _currentMaxSize) {
			T *tmpArray = new T[t_n];
			Algorithm::copy(_baseArray, tmpArray, _currentSize);
			Algorithm::fill(tmpArray + _currentSize, tmpArray + t_n, t_default);
			_currentSize = t_n;
			_baseArray = tmpArray;
		} else {
			_currentSize = t_n;
		}
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

#endif // VECTOR_HPP