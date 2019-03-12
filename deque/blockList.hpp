#ifndef SJTU_BLOCKLIST_HPP
#define SJTU_BLOCKLIST_HPP

#include "exceptions.hpp"
#include "node.hpp"

#include <cstddef>
const size_t maximalSize = 300;
namespace sjtu {
	template<class T>
	class blockList {
		template<class T>
		friend class deque;
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
		~blockList() {}
		void clear() {
			elementSize = 0;
			node<T> *pointer = hd, *tmp;
			int cnt = 0;
			while (pointer != NULL) {
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
		node<T>* end() const{
			return tl->pv;
		}
		node<T>* lastPointer(node<T> *pointer) {
			return pointer->pv;
		}
		node<T>* nextPointer(node<T> *pointer) {
			return pointer->nx;
		}
		node<T>* moveLeft(node<T> *pointer, int delta) {
			while(delta > 0 && pointer != hd) {
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
}

#endif
