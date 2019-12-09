#ifndef Vector_hpp
#define Vector_hpp

#include "custom_types.h"
#include "maths.h"
#include "algorithms.h"
#include <stdexcept>

using Maths::roundToNextPowerOfTwo;

//vector class implemented on C-style arrays

template<typename T>
class vector {
	static sizeT _defaultStartSize;
	T * _baseArray = nullptr;
	sizeT _currentSize = 0;
	sizeT _currentMaxSize = 0;
public:
	vector()
	:  _currentSize(0),
	  _currentMaxSize(_defaultStartSize)
	{
		_baseArray = new T[_currentMaxSize];
	}

	vector(std::initializer_list<T> values)
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

	template <sizeT n> vector(const T (&arr)[n])
	{
		_currentSize = n;
		_currentMaxSize = (n & (-n)) == n ? n : roundToNextPowerOfTwo(n);
		_baseArray = new T[_currentMaxSize];
		copy(arr, _baseArray, n);
	}

	vector(sizeT t_vectorSize)
	: _currentSize(t_vectorSize)
	{
		_currentMaxSize = ((t_vectorSize & (-t_vectorSize)) == t_vectorSize
							? t_vectorSize
							: roundToNextPowerOfTwo(t_vectorSize)),
		_baseArray = new T[_currentMaxSize];
	}

	vector(sizeT t_vectorSize, T t_default)
	: _currentSize(t_vectorSize)
	{
		_currentMaxSize = ((t_vectorSize & (-t_vectorSize)) == t_vectorSize
							? t_vectorSize
							: roundToNextPowerOfTwo(t_vectorSize)),
		_baseArray = new T[_currentMaxSize];
        fill(_baseArray, _currentMaxSize, t_default);
	}

	~vector()
	{
		if (_baseArray != nullptr) {
			_currentSize = 0;
			_currentMaxSize = 0;
			delete[] _baseArray;
		}
	}

	vector(const vector & other)
	: _currentSize(other._currentSize)
	, _currentMaxSize(other._currentMaxSize)
	{
		if (_baseArray != nullptr)
			delete [] _baseArray;
		_baseArray = new T[_currentMaxSize];
		copy(other._baseArray, _baseArray, _currentSize);
	}

	vector& operator=(const vector & other)
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

	vector operator+(const vector & other)
	{
		vector result(_currentSize+other._currentSize);
		copy(_baseArray, result._baseArray, _currentSize);
		copy(other._baseArray, result._baseArray+_currentSize, other._currentSize);
		return result;
	}

	vector& operator+=(const vector &other)
	{
		vector result = *this+other;
		*this = result;
		return *this;
	}

	bool operator==(const vector &other)
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
		vector result(t_n);
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
			vector();
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

template<typename T>
sizeT vector<T>::_defaultStartSize = 32U;

#endif

