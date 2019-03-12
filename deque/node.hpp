#ifndef SJTU_NODE_HPP
#define SJTU_NODE_HPP

namespace sjtu {
	template<class T>
	class node {
		template<class T>
		friend class deque;
		template<class T>
		friend class blockList;
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
			if(data != NULL) delete data;
		}
		node(const node &other) : pv(NULL), nx(NULL) {
			if (other.data == NULL) data = NULL;
			else data = new T(*(other.data));
		}
		T &value() { return *data; }
	};
}

#endif