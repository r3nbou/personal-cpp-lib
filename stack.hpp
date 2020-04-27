// Artem Mikheev 2019
// GNU GPLv3 License

#ifndef STACK_HPP
#define STACK_HPP

#include "utility.hpp"

// Stack class based on listNode helper struct

template<typename T>
class Stack {
	listNode<T> * _cur;
public:
	Stack()
	: _cur(nullptr) {}

	~Stack()
	{
		if (!this->empty()) this->clear();
	}

	Stack(const Stack & other) {
		Stack<T> working;
		listNode<T> *curNode = other._cur;
		while (curNode != nullptr) {
			working.push(*curNode->elem);
			curNode = curNode->next;
		}
		while (!working.empty())
			this->push(working.top()), working.pop();
	}

	Stack& operator=(const Stack & other) {
		Stack<T> working;
		listNode<T> *curNode = other._cur;
		while (curNode != nullptr) {
			working.push(*curNode->elem);
			curNode = curNode->next;
		}
		while (!working.empty())
			this->push(working.top()), working.pop();
		return *this;
	}

	inline bool empty() {
		return this->_cur == nullptr;
	}

	void pop() {
		if (this->empty()) throw std::runtime_error("Popping from empty stack.");
		listNode<T> * tmpNode = this->_cur->next;
		delete this->_cur;
		this->_cur = tmpNode;
	}

	T top() {
		if (this->empty()) throw std::runtime_error("Getting element from empty stack.");
		return *this->_cur->elem;
	}

	void push(T t_val) {
		listNode<T> * tmpNode = new listNode<T>(t_val);
		tmpNode->next = this->_cur;
		this->_cur = tmpNode;
	}

	void clear() {
		while (!this->empty()) this->pop();
	}
};

#endif //STACK_HPP
