#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {


	template<class T>
	void swap(T &a, T &b) {
		T tmp = a;
		a = b;
		b = tmp;
	}
	template<typename T, class Compare = std::less<T>>
	class priority_queue {
	private:
		struct node {
			T value;
			int distance;
			node *leftChild, *rightChild;
			node(T _value, node *_leftChild = NULL, node *_rightChild = NULL) : value(_value), leftChild(_leftChild), rightChild(_rightChild), distance(0) {}
			node(node *other) : value(other->value), distance(other->distance) {
				if (other->leftChild != NULL) {
					leftChild = new node(other->leftChild);
				}
				else leftChild = NULL;
				if (other->rightChild != NULL) {	
					rightChild = new node(other->rightChild);
				}
				else rightChild = NULL;
			}
			~node() {}
			void clear() {
				if (leftChild != NULL) {
					leftChild->clear();
					delete leftChild;
				}
				if (rightChild != NULL) {
					rightChild->clear();
					delete rightChild;
				}
			}
			void maintain() {
				int leftChildDistance = leftChild == NULL ? -1 : leftChild->distance;
				int rightChildDistance = rightChild == NULL ? -1 : rightChild->distance;
				if (leftChildDistance < rightChildDistance) {
					swap(leftChild, rightChild);
					distance = leftChildDistance + 1;
				}
				else distance = rightChildDistance + 1;
			}
		};
		node *root;
		size_t currentSize;
	public:
		priority_queue() : root(NULL), currentSize(0) {}
		priority_queue(const priority_queue &other) : currentSize(other.currentSize) {
			if (other.root != NULL) {
				root = new node(other.root);
			}
			else root = NULL;
		}
		~priority_queue() {
			currentSize = 0;
			if (root != NULL) {
				root->clear();
				delete root;
			}
		}
		priority_queue &operator=(const priority_queue &other) {
			if (this == &other) return *this;
			currentSize = other.currentSize;
			if (root != NULL) {
				root->clear();
				delete root;
			}
			if (other.root != NULL) {
				root = new node(other.root);
			}
			else root = NULL;
			return *this;
		}
		node* mergeNode(node *a, node *b) {
			if (a == NULL) return b;
			if (b == NULL) return a;
			static Compare comparator;
			if (comparator(a->value, b->value)) swap(a, b);
			a->rightChild = mergeNode(a->rightChild, b);
			a->maintain();
			return a;
		}
		const T & top() const {
			if (currentSize == 0) throw container_is_empty();
			return root->value;
		}
		void push(const T &e) {
			++currentSize;
			node *p = new node(e);
			root = mergeNode(root, p);
		}
		void pop() {
			if (currentSize == 0) throw container_is_empty();
			currentSize--;
			node *tmp = root;
			root = mergeNode(root->leftChild, root->rightChild);
			delete tmp;
		}
		size_t size() const {
			return currentSize;
		}
		bool empty() const {
			return currentSize == 0;
		}
		void merge(priority_queue &other) {
			root = mergeNode(root, other.root);
			currentSize += other.currentSize;
			other.root = NULL;
			other.currentSize = 0;
		}
	};
}
#endif
