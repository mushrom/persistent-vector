#pragma once

#include <iostream> // TODO: remove
#include <memory>
#include <stdexcept>

namespace persist {

template <typename T>
class vector {
  public:
	// TODO: make this a preprocessor option
	enum { NODE_BITS = 5, MASK = (1 << NODE_BITS) - 1, };

	class value {
		public:
			virtual bool is_leaf() = 0;
	};
	typedef std::shared_ptr<value> nodeptr;

	class internal : public value {
		public:
			virtual bool is_leaf() { return false; };
			nodeptr ptrs[1 << NODE_BITS] = {nullptr};
	};

	class leaf : public value {
		public:
			virtual bool is_leaf() { return true; };
			T values[1 << NODE_BITS];
	};

	class reference {
	  public:
		reference(vector *p, std::size_t idx) {
			parent = p;
			index = idx;
		}

		const T& operator=(const T& other) {
			parent->set(index, other);
			return other;
		}

		operator T(){
			return parent->get(index);
		}

	  private:
		vector *parent = nullptr;
		std::size_t index = 0;
	};

	nodeptr root = nullptr;
	std::size_t elements = 0;

	T& operator=(const T& other) {
		elements = other.elements;
		root = other.root;
	}

	std::size_t size(void){ return elements; };
	void set(std::size_t idx, const T& thing);
	const T& get(std::size_t idx);
	const T& operator[](std::size_t idx) const { return get(idx); };
	reference operator[](std::size_t idx){ return reference(this, idx); }
	void push_back(T datum);
	T pop_back(void);

	unsigned key(std::size_t index, int level) {
		return (index >> (NODE_BITS * (level - 1))) & MASK;
	}

	std::size_t base_log(std::size_t n) {
		std::size_t ret = 0;

		while (n) {
			ret += 1;
			n >>= NODE_BITS;
		}

		return ret;
	}

	nodeptr insert(nodeptr foo, std::size_t idx, int level, const T& thing);
	nodeptr pad(nodeptr foo, int new_level, int old_level);
	nodeptr remove(nodeptr foo, std::size_t idx, std::size_t level);
};

// namespace persist
}

#include <persist/imp/vector.hpp>
