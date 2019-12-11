// Artem Mikheev 2019
// MIT License

#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "utility.hpp"

// Queue class based on listNode helper class

template<typename T>
class Queue {
	listNode<T> * _head;
	listNode<T> *_last;
public:
	Queue()
			: _head(nullptr),
			  _last(nullptr) {}

	~Queue()
	{
		if (!this->empty()) this->clear();
	}

	Queue(const Queue & other)
	{
		_head = nullptr;
		_last = nullptr;
		listNode<T> * currentNode = other._head;
		while (currentNode != nullptr) {
			this->push(*currentNode->elem);
			currentNode = currentNode->next;
		}
	}
	Queue& operator=(const Queue & other)
	{
		listNode<T> * currentNode = other._head;
		while (currentNode != nullptr) {
			this->push(*currentNode->elem);
			currentNode = currentNode->next;
		}
		return *this;
	}

	inline bool empty() {
		return this->_head == nullptr;
	}

	void pop() {
		if (this->empty()) throw std::runtime_error("Popping from empty Queue.");
		listNode<T> * tmpNode = this->_head->next;
		delete this->_head;
		this->_head = tmpNode;
	}

	T top() {
		if (this->empty()) throw std::runtime_error("Getting element from empty Queue.");
		return *this->_head->elem;
	}

	void push(T t_val) {
		listNode<T> * tmpNode = new listNode<T>(t_val);
		if (!this->empty()) {
			this->_last->next = tmpNode;
		} else {
			this->_head = tmpNode;
		}
		this->_last = tmpNode;
	}

	void clear() {
		while (!this->empty()) this->pop();
	}
};

#endif //QUEUE_HPP
