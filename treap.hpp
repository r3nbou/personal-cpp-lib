// Artem Mikheev 2019
// MIT License

#ifndef TREAP_H
#define TREAP_H

#include "random.hpp"
#include "utility.hpp"
#include "algorithm.hpp"

// Treap (tree-heap) implementation in C++ classes
// both "map" and "set" variants are available

template<typename T1, typename T2=void>
class Treap {
protected:
	xorshift64 priorityGen;

	struct node {
		T1 *key;
		T2 *elem;
		unsigned long long priority;
		unsigned int size;
		node *left, *right;

		node()
				: left(nullptr),
				  right(nullptr),
				  key(nullptr),
				  elem(nullptr),
				  size(0) {}

		node(xorshift64 &priorityGen, T1 t_key, T2 t_elem)
				: key(new T1(t_key)),
				  elem(new T2(t_elem)),
				  left(nullptr),
				  right(nullptr),
				  priority(priorityGen()),
				  size(1) {}
	};

	struct nodePair {
		node *left, *right;

		nodePair()
				: left(nullptr),
				  right(nullptr) {}

		nodePair(node *t_left, node *t_right)
				: left(t_left),
				  right(t_right) {}
	};

	node *_root;

	inline unsigned int getSize(node *t) {
		return t == nullptr ? 0 : t->size;
	}

	inline void update(node *&t) {
		t->size = getSize(t->left) + getSize(t->right) + 1;
	}

	void clearTree(node *t) {
		if (t == nullptr)
			return;
		clearTree(t->left);
		clearTree(t->right);
		delete t;
	}

	node *copyTree(node *t) {
		if (t == nullptr)
			return nullptr;
		node *result = new node(priorityGen, *t->key, *t->elem);
		result->priority = t->priority;
		result->left = copyTree(t->left);
		result->right = copyTree(t->right);
		return result;
	}

	node *findMax(node *t) {
		while (t != nullptr && t->right != nullptr)
			t = t->right;
		return t;
	}

	node *findMin(node *t) {
		while (t != nullptr && t->left != nullptr)
			t = t->left;
		return t;
	}

	node *find(node *t, T1 t_key) {
		if (t == nullptr || *t->key == t_key)
			return t;
		if (t_key < *t->key)
			return find(t->left, t_key);
		else
			return find(t->right, t_key);
	}

	nodePair split(node *t, T1 t_key) {
		if (t == nullptr)
			return nodePair(nullptr, nullptr);
		if (t_key < *t->key) {
			nodePair returnSplit = split(t->left, t_key);
			update(returnSplit.left);
			update(returnSplit.right);
			t->left = returnSplit.right;
			update(t);
			return {returnSplit.left, t};
		}
		nodePair returnSplit = split(t->right, t_key);
		update(returnSplit.left);
		update(returnSplit.right);
		t->right = returnSplit.left;
		update(t);
		return {t, returnSplit.right};
	}

	node *merge(nodePair nodes) {
		if (nodes.left == nullptr || nodes.right == nullptr)
			return nodes.left == nullptr ? nodes.right : nodes.left;
		if (nodes.left->priority > nodes.right->priority) {
			nodes.left->right = merge({nodes.left->right, nodes.right});
			update(nodes.left->right);
			update(nodes.left);
			return nodes.left;
		}
		nodes.right->left = merge({nodes.left, nodes.right->left});
		update(nodes.right->left);
		update(nodes.right);
		return nodes.right;
	}

	node *leftRotate(node *t) {
		node *x = t->right;
		node *y = x->left;
		t->right = y;
		update(t);
		x->left = t;
		update(x);
		return x;
	}

	node *rightRotate(node *t) {
		node *x = t->left;
		node *y = x->right;
		t->left = y;
		update(t);
		x->right = t;
		update(x);
		return x;
	}

	node *insert(node *t, T1 t_key, T2 t_elem) {
		if (t == nullptr)
			return new node(priorityGen, t_key, t_elem);
		if (t_key == *t->key) {
			t->elem = new T2(t_elem);
		} else if (t_key < *t->key) {
			t->left = insert(t->left, t_key, t_elem);
			update(t);
			if (t->left->priority > t->priority)
				t = rightRotate(t);
		} else {
			t->right = insert(t->right, t_key, t_elem);
			update(t);
			if (t->right->priority > t->priority)
				t = leftRotate(t);
		}
		return t;
	}

	node *remove(node *t, T1 t_key) {
		if (t == nullptr)
			return nullptr;
		if (*t->key == t_key) {
			node *result = merge({t->left, t->right});
			if (t->key != nullptr)
				delete t->key;
			if (t->elem != nullptr)
				delete t->elem;
			return result;
		} else if (t_key < *t->key) {
			t->left = remove(t->left, t_key);
			update(t);
		} else {
			t->right = remove(t->right, t_key);
			update(t);
		}
		return t;
	}

public:
	Treap()
			: _root(nullptr) {}

	~Treap() {
		clearTree(_root);
	}

	Treap(node *t_root) {
		_root = t_root;
	}

	bool empty() {
		return _root == nullptr;
	}

	unsigned int size() {
		return getSize(_root);
	}

	Treap(const Treap &other) {
		_root = copyTree(other._root);
		priorityGen = other.priorityGen;
	}

	Treap &operator=(const Treap &other) {
		_root = copyTree(other._root);
		priorityGen = other.priorityGen;
		return *this;
	}

	void insert(T1 t_key, T2 t_elem) {
		_root = insert(_root, t_key, t_elem);
		return;
	}

	void merge(const Treap &other) {
		_root = merge({_root, other._root});
		return;
	}

	Pair<Treap, Treap> split(T1 t_key) {
		node *tmp = copyTree(_root);
		nodePair returnSplit = split(tmp, t_key);
		return {returnSplit.left, returnSplit.right};
	}

	void remove(T1 t_key) {
		_root = remove(_root, t_key);
	}

	T2 getMin() {
		return *findMin(_root)->elem;
	}

	T2 getMax() {
		return *findMax(_root)->elem;
	}

	T2 *find(T1 t_key) {
		node *findResult = find(_root, t_key);
		return findResult == nullptr ? nullptr : new T2(*findResult->elem);
	}
};


template<typename T>
class Treap<T, void> {
protected:
	xorshift64 priorityGen;

	struct node {
		T *key;
		unsigned long long priority;
		node *left, *right;
		unsigned int size;

		node()
				: left(nullptr),
				  right(nullptr),
				  size(0),
				  key(nullptr) {}

		node(xorshift64 &priorityGen, T t_key)
				: key(new T(t_key)),
				  priority(priorityGen()),
				  left(nullptr),
				  right(nullptr),
				  size(1) {}
	};

	struct nodePair {
		node *left, *right;

		nodePair()
				: left(nullptr),
				  right(nullptr) {}

		nodePair(node *t_left, node *t_right)
				: left(t_left),
				  right(t_right) {}
	};

	node *_root;

	inline unsigned int getSize(node *t) {
		return t == nullptr ? 0 : t->size;
	}

	inline void update(node *&t) {
		t->size = getSize(t->left) + getSize(t->right) + 1;
	}

	void clearTree(node *t) {
		if (t == nullptr)
			return;
		clearTree(t->left);
		clearTree(t->right);
		delete t;
	}

	node *copyTree(node *t) {
		if (t == nullptr)
			return nullptr;
		node *result = new node(priorityGen, *t->key);
		result->priority = t->priority;
		result->height = t->height;
		result->left = copyTree(t->left);
		result->right = copyTree(t->right);
		return result;
	}

	node *findMax(node *t) {
		while (t != nullptr && t->right != nullptr)
			t = t->right;
		return t;
	}

	node *findMin(node *t) {
		while (t != nullptr && t->left != nullptr)
			t = t->left;
		return t;
	}

	node *find(node *t, T t_key) {
		if (t == nullptr || *t->key == t_key)
			return t;
		if (t_key < *t->key)
			return find(t->left, t_key);
		else
			return find(t->right, t_key);
	}

	nodePair split(node *t, T t_key) {
		if (t == nullptr)
			return nodePair(nullptr, nullptr);
		if (t_key < *t->key) {
			nodePair returnSplit = split(t->left, t_key);
			update(returnSplit.left);
			update(returnSplit.right);
			t->left = returnSplit.right;
			update(t);
			return {returnSplit.left, t};
		}
		nodePair returnSplit = split(t->right, t_key);
		update(returnSplit.left);
		update(returnSplit.right);
		t->right = returnSplit.left;
		update(t);
		return {t, returnSplit.right};
	}

	node *merge(nodePair nodes) {
		if (nodes.left == nullptr || nodes.right == nullptr)
			return nodes.left == nullptr ? nodes.right : nodes.left;
		if (nodes.left->priority > nodes.right->priority) {
			nodes.left->right = merge({nodes.left->right, nodes.right});
			update(nodes.left->right);
			update(nodes.left);
			return nodes.left;
		}
		nodes.right->left = merge({nodes.left, nodes.right->left});
		update(nodes.right->left);
		update(nodes.right);
		return nodes.right;
	}

	node *leftRotate(node *t) {
		node *x = t->right;
		node *y = x->left;
		t->right = y;
		update(t);
		x->left = t;
		update(x);
		return x;
	}

	node *rightRotate(node *t) {
		node *x = t->left;
		node *y = x->right;
		t->left = y;
		update(t);
		x->right = t;
		update(x);
		return x;
	}

	node *insert(node *t, T t_key) {
		if (t == nullptr)
			return new node(priorityGen, t_key);
		if (t_key < *t->key) {
			t->left = insert(t->left, t_key);
			update(t);
			if (t->left->priority > t->priority)
				t = rightRotate(t);
		} else if (t_key > *t->key) {
			t->right = insert(t->right, t_key);
			update(t);
			if (t->right->priority > t->priority)
				t = leftRotate(t);
		}
		return t;
	}

	node *remove(node *t, T t_key) {
		if (t == nullptr)
			return nullptr;
		if (*t->key == t_key) {
			node *result = merge({t->left, t->right});
			if (t->key != nullptr)
				delete t->key;
			return result;
		} else if (t_key < *t->key) {
			t->left = remove(t->left, t_key);
			update(t);
		} else {
			t->right = remove(t->right, t_key);
			update(t);
		}
		return t;
	}

public:
	Treap()
			: priorityGen((unsigned long long) time(nullptr) * 65537 * 11 * 19),
			  _root(nullptr) {}

	~Treap() {
		clearTree(_root);
	}

	Treap(node *t_root) {
		_root = t_root;
	}

	bool empty() {
		return _root == nullptr;
	}

	unsigned int size() {
		return getSize(_root);
	}

	Treap(const Treap &other) {
		_root = copyTree(other._root);
		priorityGen = other.priorityGen;
	}

	Treap &operator=(const Treap &other) {
		_root = copyTree(other._root);
		priorityGen = other.priorityGen;
		return *this;
	}

	void insert(T t_key) {
		_root = insert(_root, t_key);
		return;
	}

	void merge(const Treap &other) {
		_root = merge({_root, other._root});
		return;
	}

	Pair<Treap, Treap> split(T t_key) {
		node *tmp = copyTree(_root);
		nodePair returnSplit = split(tmp, t_key);
		return {returnSplit.left, returnSplit.right};
	}

	void remove(T t_key) {
		_root = remove(_root, t_key);
	}

	T getMin() {
		return *findMin(_root)->key;
	}

	T getMax() {
		return *findMax(_root)->key;
	}

	T *find(T t_key) {
		node *findResult = find(_root, t_key);
		return findResult == nullptr ? nullptr : new T(*findResult->key);
	}
};

#endif //TREAP_H
