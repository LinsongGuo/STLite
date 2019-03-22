#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
const size_t maximalSize = 300;

#include <cstddef>

namespace sjtu {

	template<class T>
	class node;
	template<class T>
	class blockList;
	template<class T>
	class deque;

	template<class T>
	class node {
		friend class deque<T>;
		friend class blockList<T>;
	private:
		T *data;
		node *pv, *nx;
	public:
		node() : data(NULL), pv(NULL), nx(NULL) {}
		node(node *_pv, node *_nx) : data(NULL), pv(_pv), nx(_nx) {}
		node(T value, node *_pv = NULL, node *_nx = NULL) : pv(_pv), nx(_nx) {
			data = new T(value);
		}
		~node() {
			if (data != NULL) delete data;
		}
		node(const node &other) : pv(NULL), nx(NULL) {
			if (other.data == NULL) data = NULL;
			else data = new T(*(other.data));
		}
		T &value() { return *data; }
	};

	template<class T>
	class blockList {
		friend class deque<T>;
	private:
		size_t elementSize;
		node<T> *hd, *tl;
		blockList *prev, *next;
	public:
		blockList() : elementSize(0), prev(NULL), next(NULL) {
			hd = new node<T>();
			tl = new node<T>();
			hd->nx = tl;
			tl->pv = hd;
		}
		blockList(size_t s, blockList *_prev, blockList *_next) : elementSize(s), prev(_prev), next(_next) {
			hd = new node<T>();
			tl = new node<T>();
			hd->nx = tl;
			tl->pv = hd;
		}
		blockList(blockList &other) : elementSize(other.elementSize), prev(NULL), next(NULL) {
			hd = new node<T>();
			tl = new node<T>();
			node<T> *p = hd, *q = (other.hd)->nx, *tmp;
			int cnt = 0;
			while (q != other.tl) {
				tmp = p;
				p = p->nx = new node<T>(*q);
				p->pv = tmp;
				q = q->nx;
			}
			p->nx = tl;
			tl->pv = p;
		}
		~blockList() {
			delete hd;
			delete tl;
		}
		void clear() {
			elementSize = 0;
			node<T> *pointer = hd->nx, *tmp;
			int cnt = 0;
			while (pointer != tl) {
				tmp = pointer;
				pointer = pointer->nx;
				delete tmp;
			}
		}
		void disconnect() {
			prev->next = next;
			next->prev = prev;
		}
		size_t size() {
			return elementSize;
		}
		bool empty() {
			return elementSize == 0;
		}
		bool full() {
			return elementSize == maximalSize;
		}
		node<T>* begin() {
			return hd->nx;
		}
		node<T>* cbegin() const {
			return hd->nx;
		}
		node<T>* end() const {
			return tl->pv;
		}
		node<T>* lastPointer(node<T> *pointer) {
			return pointer->pv;
		}
		node<T>* nextPointer(node<T> *pointer) {
			return pointer->nx;
		}
		node<T>* moveLeft(node<T> *pointer, int delta) {
			while (delta > 0 && pointer != hd) {
				pointer = pointer->pv;
				delta--;
			}
			return pointer;
		}
		node<T>* moveRight(node<T> *pointer, int delta) {
			while (delta > 0 && pointer != tl) {
				pointer = pointer->nx;
				delta--;
			}
			return pointer;
		}
		int index(node<T> *pointer) {
			int count = 0;
			while (pointer != hd) {
				pointer = pointer->pv;
				count++;
			}
			return count - 1;
		}
		bool belong(node<T> *pointer) {
			node<T> *p = hd->nx;
			while (p != tl) {
				if (p == pointer) return 1;
				p = p->nx;
			}
			return 0;
		}
		void special() {
			elementSize++;
			node<T> *pointer = new node<T>();
			pointer->pv = hd;
			pointer->nx = tl;
			hd->nx = pointer;
			tl->pv = pointer;
		}
		T &elementAt(int pos) {
			node<T> *pointer = hd->nx;
			if (pos < 0 || pos >= elementSize) throw index_out_of_bound();
			while (pos > 0 && pointer != tl) {
				pointer = pointer->nx;
				pos--;
			}
			return pointer->value();
		}
		void split(node<T> *pointer) {
			int pos = index(pointer);
			blockList *p = new blockList(elementSize - pos, this, next);
			next->prev = p;
			next = p;
			p->hd->nx = pointer;
			p->tl->pv = tl->pv;
			tl->pv->nx = p->tl;
			tl->pv = pointer->pv;
			pointer->pv->nx = tl;
			pointer->pv = p->hd;

			elementSize = pos;
		}
		void merge() {
			blockList *q = next;
			q->next->prev = this;
			next = q->next;

			tl->pv->nx = q->hd->nx;
			q->hd->nx->pv = tl->pv;
			tl->pv = q->tl->pv;
			q->tl->pv->nx = tl;
			elementSize += q->elementSize;
			//q->clear();
			delete q;
		}
		node<T>* insert(node<T> *pointer, const T& value) {
			elementSize++;
			node<T> *p = new node<T>(value, pointer->pv, pointer);
			pointer->pv->nx = p;
			pointer->pv = p;
			return p;
		}
		node<T>* insertInTheBeginning(const T &value) {
			elementSize++;
			node<T> *p = new node<T>(value, hd, hd->nx);
			hd->nx->pv = p;
			hd->nx = p;
			return p;
		}
		node<T>* insertInTheEnd(const T &value) {
			elementSize++;
			node<T> *p = new node<T>(value, tl->pv, tl);
			tl->pv->nx = p;
			tl->pv = p;
			return p;
		}
		node<T>* erase(node<T> *pointer) {
			--elementSize;
			node<T> *p = pointer->nx;
			pointer->pv->nx = pointer->nx;
			pointer->nx->pv = pointer->pv;
			delete pointer;
			if (p == tl) return next->hd->nx;
			return p;
		}
		void eraseInTheBeginning() {
			--elementSize;
			node<T> *p = hd->nx;
			hd->nx = p->nx;
			p->nx->pv = hd;
			delete p;
		}
		void eraseInTheEnd() {
			--elementSize;
			node<T> *p = tl->pv;
			tl->pv = p->pv;
			p->pv->nx = tl;
			delete p;
		}
	};


	template<class T>
	class deque {
		blockList<T> *head, *tail;
		size_t currentSize;
	public:
		class const_iterator;
		class iterator {
			friend class deque<T>;
		private:
			blockList<T> *block, *own;
			node<T> *pointer;
		public:
			iterator() : own(NULL), block(NULL), pointer(NULL) {}
			iterator(blockList<T> *_own, blockList<T> *_block, node<T> *_pointer) : own(_own), block(_block), pointer(_pointer) {}
			iterator operator+(const int &n) const {
				if (n == 0) return *this;
				if (n < 0) return operator- (-n);
				blockList<T> *b = block;
				node<T> *p = pointer;
				int pos = b->index(p), count = 0;
				while (count <= n && b != NULL) {
					if (pos + n - count < b->size()) return iterator(own, b, b->moveRight(p, n - count));
					count += (b->size() - pos);
					b = b->next;
					p = b->begin();
					pos = 0;
				}
				throw invalid_iterator();
			}
			iterator operator-(const int &n) const {
				if (n == 0) return *this;
				if (n < 0) return operator+ (-n);
				blockList<T> *b = block;
				node<T> *p = pointer;
				int pos = b->index(p), count = 0;
				while (count <= n && b != own) {
					if (pos - (n - count) >= 0) return iterator(own, b, b->moveLeft(p, n - count));
					count += (pos + 1);
					b = b->prev;
					p = b->end();
					pos = b->size() - 1;
				}
				throw invalid_iterator();
			}
			int operator-(const iterator &other) const {
				if (own != other.own) throw invalid_iterator();
				blockList<T> *p = this->block, *q = other.block;
				int indexp = p->index(this->pointer), indexq = q->index(other.pointer);

				p = p->prev, q = q->prev;
				if (p != NULL) {
					while (p != own) {
						indexp += p->size();
						p = p->prev;
					}
				}
				if (q != NULL) {
					while (q != own) {
						indexq += q->size();
						q = q->prev;
					}
				}
				return indexp - indexq;
			}
			iterator operator+=(const int &n) {
				*this = *this + n;
				return *this;
			}
			iterator operator-=(const int &n) {
				*this = *this - n;
				return *this;
			}
			iterator operator++(int) {
				iterator tmp = *this;
				if (pointer != block->end()) {
					pointer = block->nextPointer(pointer);
					return tmp;
				}
				else {
					block = block->next;
					if (block == NULL) throw invalid_iterator();
					pointer = block->begin();
					return tmp;
				}
			}
			iterator& operator++() {
				if (pointer != block->end()) {
					pointer = block->nextPointer(pointer);
				}
				else {
					block = block->next;
					if (block == NULL) throw invalid_iterator();
					pointer = block->begin();
				}
				return *this;
			}
			iterator operator--(int) {
				iterator other = *this;
				if (pointer != block->begin()) {
					pointer = block->lastPointer(pointer);
					return other;
				}
				else {
					block = block->prev;
					if (block == own) throw invalid_iterator();
					pointer = block->end();
					return other;
				}
			}
			iterator& operator--() {
				if (pointer != block->begin()) {
					pointer = block->lastPointer(pointer);
				}
				else {
					block = block->prev;
					if (block == own) throw invalid_iterator();
					pointer = block->end();
				}
				return *this;
			}
			T& operator*() const {
				if (block->next == NULL) throw invalid_iterator();
				return pointer->value();
			}
			T* operator->() const noexcept {
				if (block->next == NULL) throw invalid_iterator();
				return pointer->data;
			}
			bool operator==(const iterator &other) const {
				return own == other.own && block == other.block && pointer == other.pointer;
			}
			bool operator==(const const_iterator &other) const {
				return own == other.own && block == other.block && pointer == other.pointer;
			}
			bool operator!=(const iterator &other) const {
				return own != other.own || block != other.block || pointer != other.pointer;
			}
			bool operator!=(const const_iterator &other) const {
				return own != other.own || block != other.block || pointer != other.pointer;
			}
		};
		class const_iterator {
		private:
			blockList<T> *block, *own;
			node<T> *pointer;
		public:
			const_iterator() : own(NULL), block(NULL), pointer(NULL) {}
			const_iterator(blockList<T> *_own, blockList<T> *_block, node<T> *_pointer) : own(_own), block(_block), pointer(_pointer) {}
			const_iterator operator+(const int &n) const {
				if (n == 0) return *this;
				if (n < 0) return operator- (-n);
				blockList<T> *b = block;
				node<T> *p = pointer;
				int pos = b->index(p), count = 0;
				while (count <= n && b != NULL) {
					if (pos + n - count < b->size()) return const_iterator(own, b, b->moveRight(p, n - count));
					count += (b->size() - pos);
					b = b->next;
					p = b->begin();
					pos = 0;
				}
				throw invalid_iterator();
			}
			const_iterator operator-(const int &n) const {
				if (n == 0) return *this;
				if (n < 0) return operator+ (-n);
				blockList<T> *b = block;
				node<T> *p = pointer;
				int pos = b->index(p), count = 0;
				while (count <= n && b != own) {
					if (pos - (n - count) >= 0) return const_iterator(own, b, b->moveLeft(p, n - count));
					count += (pos + 1);
					b = b->prev;
					p = b->end();
					pos = b->size() - 1;
				}
				throw invalid_iterator();
			}
			int operator-(const const_iterator &other) const {
				if (own != other.own) throw invalid_iterator();
				blockList<T> *p = this->block, *q = other.block;
				int indexp = p->index(this->pointer), indexq = q->index(other.pointer);

				p = p->prev, q = q->prev;
				if (p != NULL) {
					while (p != own) {
						indexp += p->size();
						p = p->prev;
					}
				}
				if (q != NULL) {
					while (q != own) {
						indexq += q->size();
						q = q->prev;
					}
				}
				return indexp - indexq;
			}
			const_iterator operator+=(const int &n) {
				*this = *this + n;
				return *this;
			}
			const_iterator operator-=(const int &n) {
				*this = *this - n;
				return *this;
			}
			const_iterator operator++(int) {
				iterator tmp = *this;
				if (pointer != block->end()) {
					pointer = block->nextPointer(pointer);
					return tmp;
				}
				else {
					block = block->next;
					if (block == NULL) throw invalid_iterator();
					pointer = block->begin();
					return tmp;
				}
			}
			const_iterator& operator++() {
				if (pointer != block->end()) {
					pointer = block->nextPointer(pointer);
				}
				else {
					block = block->next;
					if (block == NULL) throw invalid_iterator();
					pointer = block->begin();
				}
				return *this;
			}
			const_iterator operator--(int) {
				const_iterator other = *this;
				if (pointer != block->begin()) {
					pointer = block->lastPointer(pointer);
					return other;
				}
				else {
					block = block->prev;
					if (block == own) throw invalid_iterator();
					pointer = block->end();
					return other;
				}
			}
			const_iterator& operator--() {
				if (pointer != block->begin()) {
					pointer = block->lastPointer(pointer);
				}
				else {
					block = block->prev;
					if (block == own) throw invalid_iterator();
					pointer = block->end();
				}
				return *this;
			}
			T& operator*() const {
				if (block->next == NULL) throw invalid_iterator();
				return pointer->value();
			}
			T* operator->() const noexcept {
				if (block->next == NULL) throw invalid_iterator();
				return pointer->data;
			}
			bool operator==(const iterator &other) const {
				return own == other.own && block == other.block && pointer == other.pointer;
			}
			bool operator==(const const_iterator &other) const {
				return own == other.own && block == other.block && pointer == other.pointer;
			}
			bool operator!=(const iterator &other) const {
				return own != other.own || block != other.block || pointer != other.pointer;
			}
			bool operator!=(const const_iterator &other) const {
				return own != other.own || block != other.block || pointer != other.pointer;
			}
		};

		deque() : currentSize(0) {
			head = new blockList<T>();
			tail = new blockList<T>();
			head->next = tail;
			tail->prev = head;
			tail->special();
		}
		deque(const deque &other) : currentSize(other.currentSize) {
			head = new blockList<T>();
			blockList<T> *p = head, *tmp;
			blockList<T> *q = (other.head)->next;
			while (q != other.tail) {
				tmp = p;
				p = p->next = new blockList<T>(*q);
				p->prev = tmp;
				q = q->next;
			}
			tail = new blockList<T>();
			tail->special();
			p->next = tail;
			tail->prev = p;
		}
		~deque() {
			currentSize = 0;
			blockList<T> *p = head, *tmp;
			while (p != NULL) {
				tmp = p;
				p = p->next;
				tmp->clear();
				delete tmp;
			}
		}
		deque &operator=(const deque &other) {
			if (this == &other) return *this;
			currentSize = other.currentSize;
			blockList<T> *p = head, *first = head->next, *tmp;
			blockList<T> *q = (other.head)->next;
			while (q != other.tail) {
				tmp = p;
				p = p->next = new blockList<T>(*q);
				p->prev = tmp;
				q = q->next;
			}
			p->next = tail;
			tail->prev = p;

			p = first;
			while (p != tail) {
				tmp = p;
				p = p->next;
				tmp->clear();
				delete tmp;
			}
			return *this;
		}
		T & at(const size_t &pos) {
			int count = 0;
			blockList<T> *p = head->next;
			while (count <= pos && p != tail) {
				if (count + p->size() > pos) {
					return p->elementAt(pos - count);
				}
				count += p->size();
				p = p->next;
			}
			throw index_out_of_bound();
		}
		const T & at(const size_t &pos) const {
			int count = 0;
			blockList<T> *p = head->next;
			while (count <= pos && p != tail) {
				if (count + p->size() > pos) {
					return p->elementAt(pos - count);
				}
				count += p->size();
				p = p->next;
			}
			throw index_out_of_bound();
		}
		T & operator[](const int &pos) {
			int count = 0;
			blockList<T> *p = head->next;
			while (count <= pos && p != tail) {
				if (count + p->size() > pos) {
					return p->elementAt(pos - count);
				}
				count += p->size();
				p = p->next;
			}
			throw index_out_of_bound();
		}
		const T & operator[](const int &pos) const {
			int count = 0;
			blockList<T> *p = head->next;
			while (count <= pos && p != tail) {
				if (count + p->size() > pos) {
					return p->elementAt(pos - count);
				}
				count += p->size();
				p = p->next;
			}
			throw index_out_of_bound();
		}
		const T & front() const {
			if (currentSize == 0) throw container_is_empty();
			return head->next->begin()->value();
		}
		const T & back() const {
			if (currentSize == 0) throw container_is_empty();
			return tail->prev->end()->value();
		}
		iterator begin() {
			return iterator(head, head->next, head->next->begin());
		}
		const_iterator cbegin() const {
			return const_iterator(head, head->next, head->next->cbegin());
		}
		iterator end() {
			return iterator(head, tail, tail->begin());
		}
		const_iterator cend() const {
			return const_iterator(head, tail, tail->begin());
		}
		bool empty() const {
			return currentSize == 0;
		}
		size_t size() const {
			return currentSize;
		}
		void clear() {
			currentSize = 0;
			blockList<T> *p = head->next, *tmp;
			while (p != tail) {
				tmp = p;
				p = p->next;
				tmp->clear();
				delete tmp;
			}
			head->next = tail;
			tail->prev = head;
		}
		iterator insert(iterator iter, const T &value) {
			blockList<T> *b = iter.block;
			node<T> *p = iter.pointer;
			if (iter.own != head || !b->belong(p)) throw invalid_iterator();
			currentSize++;
			if (b == tail) {
				b = b->prev;
				if (b == head || b->full()) {
					b = b->next = new blockList<T>(0, b, tail);
					tail->prev = b;
				}
				return iterator(head, b, b->insertInTheEnd(value));
			}
			if (b->full()) {
				b->split(p);
				return iterator(head, b, b->insertInTheEnd(value));
			}
			else {
				return iterator(head, b, b->insert(p, value));
			}
		}
		iterator erase(iterator iter) {
			if (currentSize == 0) throw container_is_empty();
			blockList<T> *b = iter.block;
			node<T> *p = iter.pointer;
			if (iter.own != head || !b->belong(p)) throw invalid_iterator();
			currentSize--;
			bool isend = (p == b->end());
			p = b->erase(p);
			if (b->empty()) {
				blockList<T> *bnext = b->next;
				b->disconnect();
				b->clear();
				delete b;
				return iterator(head, bnext, p);
			}
			if (b->next != tail && b->size() + b->next->size() <= maximalSize) {
				b->merge();
				isend = false;
			}
			return iterator(head, isend ? b->next : b, p);
		}
		void push_back(const T &value) {
			currentSize++;
			blockList<T> *p = tail->prev;
			if (p == head || p->full()) {
				p = p->next = new blockList<T>(0, p, tail);
				p->insertInTheEnd(value);
				tail->prev->next = p;
				tail->prev = p;
			}
			else {
				p->insertInTheEnd(value);
			}
		}
		void pop_back() {
			if (currentSize == 0) throw container_is_empty();
			currentSize--;
			blockList<T> *p = tail->prev;
			p->eraseInTheEnd();
			if (p->empty()) {
				p->disconnect();
				p->clear();
				delete p;
			}
			else if (p->prev != head && p->prev->size() + p->size() <= maximalSize) p->prev->merge();
		}
		void push_front(const T &value) {
			currentSize++;
			blockList<T> *p = head->next;
			if (p == tail || p->full()) {
				p = p->prev = new blockList<T>(0, head, p);
				p->insertInTheBeginning(value);
				head->next->prev = p;
				head->next = p;
			}
			else {
				p->insertInTheBeginning(value);
			}
		}
		void pop_front() {
			if (currentSize == 0) throw container_is_empty();
			currentSize--;
			blockList<T> *p = head->next;
			p->eraseInTheBeginning();
			if (p->empty()) {
				p->disconnect();
				p->clear();
				delete p;
			}
			else if (p->next != tail && p->next->size() + p->size() <= maximalSize) {
				p->merge();
			}
		}
	};
}

#endif