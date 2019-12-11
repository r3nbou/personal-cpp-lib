// Artem Mikheev 2019
// MIT License

#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include "algorithm.hpp"

// AVL tree implementation in C++ classes without any dependencies
// both "map" and "set" variants are available

template<typename T1, typename T2=void>
class AVLtree {
protected:
	struct node {
		T1 *key;
		T2 *elem;
		unsigned int height;
		unsigned int size;
		node *left, *right;

		node()
				: left(nullptr),
				  right(nullptr),
				  key(nullptr),
				  elem(nullptr),
				  height(0),
				  size(0) {}

		node(T1 t_key, T2 t_elem)
				: key(new T1(t_key)),
				  elem(new T2(t_elem)),
				  left(nullptr),
				  right(nullptr),
				  height(1),
				  size(1) {}

	};

	node *_root;

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
		node *result = new node(*t->key, *t->elem);
		result->height = t->height;
		result->size = t->size;
		result->left = copyTree(t->left);
		result->right = copyTree(t->right);
		return result;
	}

	inline unsigned int getHeight(node *t) {
		return (t == nullptr ? 0 : t->height);
	}

	inline unsigned int getSize(node *t) {
		return (t == nullptr ? 0 : t->size);
	}

	inline void update(node *&t) {
		t->height = Algorithm::max(getHeight(t->left), getHeight(t->right)) + 1;
		t->size = getSize(t->left) + getSize(t->right) + 1;
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

	void rotate(node *&t) {
		if (t == nullptr)
			return;

		if (getHeight(t->left) - getHeight(t->right) == 2) {
			if (getHeight(t->left->left) >= getHeight(t->left->right)) {
				node *l = t->left;
				t->left = l->right;
				update(t);
				l->right = t;
				t = l;
			} else {
				node *l = t->left;
				node *m = l->right;
				l->right = m->left;
				update(l);
				t->left = m->right;
				update(t);
				m->left = l;
				m->right = t;
				t = m;
			}
		} else if (getHeight(t->right) - getHeight(t->left) == 2) {
			if (getHeight(t->right->right) >= getHeight(t->right->left)) {
				node *r = t->right;
				t->right = r->left;
				update(t);
				r->left = t;
				t = r;
			} else {
				node *r = t->right;
				node *m = r->left;
				r->left = m->right;
				update(r);
				t->right = m->left;
				update(t);
				m->left = t;
				m->right = r;
				t = m;
			}
		}
		update(t);
	}

	node *find(node *t, T1 t_key) {
		if (t == nullptr || *t->key == t_key)
			return t;
		if (t_key < *t->key)
			return find(t->left, t_key);
		else
			return find(t->right, t_key);
	}

	node *insert(node *t, T1 t_key, T2 t_elem) {
		if (t == nullptr) {
			t = new node(t_key, t_elem);
			return t;
		} else if (*t->key == t_key) {
			t->elem = new T2(t_elem);
			return t;
		} else if (t_key < *t->key) {
			t->left = insert(t->left, t_key, t_elem);
		} else {
			t->right = insert(t->right, t_key, t_elem);
		}
		rotate(t);
		return t;
	}

	node *remove(node *t, T1 t_key) {
		if (t == nullptr)
			return nullptr;
		else if (t_key < *t->key)
			t->left = remove(t->left, t_key);
		else if (t_key > *t->key)
			t->right = remove(t->right, t_key);
		else {
			if (t->left != nullptr && t->right != nullptr) {
				node *tempNode = findMax(t->left);
				if (t->key != nullptr)
					delete t->key;
				if (t->elem != nullptr)
					delete t->elem;
				t->key = tempNode->key;
				t->elem = tempNode->elem;
				t->left = remove(t->left, *t->key);
			} else {
				node *tempNode;
				if (t->left != nullptr)
					tempNode = t->left;
				else if (t->right != nullptr)
					tempNode = t->right;
				else
					tempNode = nullptr;
				delete t;
				t = tempNode;
			}
		}
		rotate(t);
		return t;
	}

public:
	AVLtree()
			: _root(nullptr) {}

	~AVLtree() {
		clearTree(_root);
	}

	AVLtree(node *otherRoot) {
		_root = otherRoot;
		return;
	}

	bool empty() {
		return _root == nullptr;
	}

	unsigned int size() {
		return getSize(_root);
	}

	AVLtree(const AVLtree &other) {
		_root = copyTree(other._root);
	}

	AVLtree &operator=(const AVLtree &other) {
		_root = copyTree(other._root);
	}

	void insert(T1 t_key, T2 t_elem) {
		_root = insert(_root, t_key, t_elem);
	}

	void remove(T1 t_key) {
		_root = remove(_root, t_key);
	}

	T2 min() {
		return *findMin(_root)->elem;
	}

	T2 max() {
		return *findMax(_root)->elem;
	}

	T2 *find(T1 t_key) {
		node *findResult = find(_root, t_key);
		return findResult == nullptr ? nullptr : new T2(*findResult->elem);
	}
};

template<typename T>
class AVLtree<T, void> {
protected:
	struct node {
		T *key;
		node *left, *right;
		unsigned int height;
		unsigned int size;

		node()
				: left(nullptr),
				  right(nullptr),
				  key(nullptr),
				  height(0),
				  size(0) {}

		node(T t_key)
				: key(new T(t_key)),
				  left(nullptr),
				  right(nullptr),
				  height(1),
				  size(1) {}
	};

	node *_root;

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
		node *result = new node(*t->key);
		result->height = t->height;
		result->left = copyTree(t->left);
		result->right = copyTree(t->right);
		return result;
	}

	inline unsigned int getHeight(node *t) {
		return (t == nullptr ? 0 : t->height);
	}

	inline unsigned int getSize(node *t) {
		return (t == nullptr ? 0 : t->size);
	}

	inline void update(node *&t) {
		t->height = Algorithm::max(getHeight(t->left), getHeight(t->right)) + 1;
		t->size = getSize(t->left) + getSize(t->right) + 1;
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

	void rotate(node *&t) {
		if (t == nullptr)
			return;

		if (getHeight(t->left) - getHeight(t->right) == 2) {
			if (getHeight(t->left->left) >= getHeight(t->left->right)) {
				node *l = t->left;
				t->left = l->right;
				update(t);
				l->right = t;
				t = l;
			} else {
				node *l = t->left;
				node *m = l->right;
				l->right = m->left;
				update(l);
				t->left = m->right;
				update(t);
				m->left = l;
				m->right = t;
				t = m;
			}
		} else if (getHeight(t->right) - getHeight(t->left) == 2) {
			if (getHeight(t->right->right) >= getHeight(t->right->left)) {
				node *r = t->right;
				t->right = r->left;
				update(t);
				r->left = t;
				t = r;
			} else {
				node *r = t->right;
				node *m = r->left;
				r->left = m->right;
				update(r);
				t->right = m->left;
				update(t);
				m->left = t;
				m->right = r;
				t = m;
			}
		}
		update(t);
		return;
	}

	node *find(node *t, T t_key) {
		if (t == nullptr || *t->key == t_key)
			return t;
		if (t_key < *t->key)
			return find(t->left, t_key);
		else
			return find(t->right, t_key);
	}

	node *insert(node *t, T t_key) {
		if (t == nullptr) {
			t = new node(t_key);
			return t;
		} else if (*t->key == t_key) {
			return t;
		} else if (t_key < *t->key) {
			t->left = insert(t->left, t_key);
		} else {
			t->right = insert(t->right, t_key);
		}
		rotate(t);
		return t;
	}

	node *remove(node *t, T t_key) {
		if (t == nullptr)
			return nullptr;
		else if (t_key < *t->key)
			t->left = remove(t->left, t_key);
		else if (t_key > *t->key)
			t->right = remove(t->right, t_key);
		else {
			if (t->left != nullptr && t->right != nullptr) {
				node *tempNode = findMax(t->left);
				if (t->key != nullptr)
					delete t->key;
				t->key = tempNode->key;
				t->left = remove(t->left, *t->key);
			} else {
				node *tempNode = t;
				if (t->left != nullptr)
					t = t->left;
				else if (t->right != nullptr)
					t = t->right;
				else
					t = nullptr;
				delete tempNode;
			}
		}
		rotate(t);
		return t;
	}

public:
	AVLtree()
			: _root(nullptr) {}

	~AVLtree() {
		clearTree(_root);
	}

	AVLtree(node *otherRoot) {
		_root = otherRoot;
		return;
	}

	AVLtree(const AVLtree &other) {
		_root = copyTree(other._root);
	}

	bool empty() {
		return _root == nullptr;
	}

	unsigned int size() {
		return getSize(_root);
	}

	AVLtree &operator=(const AVLtree &other) {
		_root = copyTree(other._root);
	}

	void insert(T t_key) {
		_root = insert(_root, t_key);
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

#endif // AVL_TREE_HPP
