// Artem Mikheev 2019
// MIT License

#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

namespace Algorithm {
	typedef unsigned int sizeT;

	/* Various memory functions for working with memory  */

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
