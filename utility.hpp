// Artem Mikheev 2019
// MIT License

#ifndef UTILITY_HPP
#define UTILITY_HPP

// Various utility classes for use in code and other structures

// Template structure for creating linked-list based structures

template<typename T>
struct listNode {
	T * elem;
	listNode * next = nullptr;
	listNode(T t_val): elem(new T(t_val)) {}
	listNode (const listNode & other)
	{
		elem = other.elem;
		next = other.next;
	}

	listNode& operator=(const listNode & other)
	{
		elem = other.elem;
		next = other.next;
	}
};

// Simple helper classes for pairs and triples of various element types

template<typename T1, typename T2>
class Pair {
public:
	T1 first;
	T2 second;

	Pair()
	: first(T1()),
	  second(T2()) {}

	Pair(T1 t_first, T2 t_second)
	: first (t_first),
	  second(t_second) {}

};

template<typename T1, typename T2, typename T3>
class Triple {
public:
	T1 first;
	T2 second;
	T3 third;

	Triple()
	: first(T1()),
	  second(T2()),
	  third(T3()) {}

	Triple(T1 t_first, T2 t_second, T3 t_third)
	: first (t_first),
	  second(t_second),
	  third(t_third) {}
};

#endif //UTILITY_HPP
