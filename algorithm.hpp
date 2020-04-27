// Artem Mikheev 2019
// MIT License

#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include "vartypes.hpp"

namespace Algorithm {

	/* Various memory functions for working with memory  */

	template<typename T>
	void swap(T &t_first, T &t_second) {
		T tmp = t_first;
		t_first = t_second;
		t_second = tmp;
	}

	template<typename T>
	void copy(T t_first, T t_second, sizeT t_n) {
		for (sizeT i = 0; i < t_n; i++)
			t_second[i] = t_first[i];
	}

	template<typename T>
	void copy(T &t_first, sizeT firstLoc, T &t_second, sizeT secLoc, sizeT t_n) {
		for (sizeT i = 0; i < t_n; i++)
			t_second[secLoc + i] = t_first[firstLoc + i];
	}

	template<typename T>
	void copy(const T *t_firstIt, T *t_secondIt, sizeT t_n) {
		for (sizeT i = 0; i < t_n; i++, t_secondIt++, t_firstIt++)
			*t_secondIt = *t_firstIt;
	}

	template<typename T>
	void copy(T *t_firstItBegin, T *t_firstItEnd, T *t_second) {
		while (t_firstItBegin != t_firstItEnd) {
			*t_second = *t_firstItBegin;
			t_second++, t_firstItBegin++;
		}
	}

	template<typename T>
	void fill(T *t_startIt, sizeT t_n, T t_value) {
		for (sizeT i = 0; i < t_n; i++, t_startIt++)
			*t_startIt = t_value;
	}

	template<typename T>
	void fill(T *t_startIt, T *t_endIt, T t_value) {
		while (t_startIt != t_endIt) {
			*t_startIt = t_value;
			t_startIt++;
		}
	}

	template<typename T>
	void reverse(T *t_firstIt, T *t_secondIt) {
		while (t_firstIt != t_secondIt) {
			Algorithm::swap(*t_firstIt, *t_secondIt);
			t_firstIt++, t_secondIt++;
		}

	}

	template<typename T>
	void reverse(T &t_first, sizeT t_n) {
		int32_t end = t_n / 2;
		for (sizeT i = 0; i < end; i++)
			Algorithm::swap(t_first[i], t_first[t_n - 1 - i]);
	}

	template<typename T1, typename T2>
	bool inContainer(T1 &t_container, T2 t_val) {
		return t_container.find(t_val);
	}

	/* Functions for working with various variable types */

	template<typename T>
	T min(T first, T second) {
		return (first < second ? first : second);
	}

	template<typename T>
	T max(T first, T second) {
		return (first > second ? first : second);
	}
}

#endif //ALGORITHM_HPP
