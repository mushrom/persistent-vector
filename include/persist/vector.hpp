#pragma once

#include <iostream> // TODO: remove
#include <memory>
#include <stdexcept>

namespace persist {

template <typename T>
class vector {
	public:
		class value;
		class internal;
		class leaf;
		class reference;

		typedef std::shared_ptr<value> nodeptr;

		T& operator=(const T& other);
		std::size_t size(void);;
		const T& operator[](std::size_t idx) const;
		reference operator[](std::size_t idx);
		void push_back(T datum);
		T pop_back(void);

	private:
		// TODO: make this a preprocessor option
		enum { NODE_BITS = 5, MASK = (1 << NODE_BITS) - 1, };

		nodeptr root = nullptr;
		std::size_t elements = 0;

		void set(std::size_t idx, const T& thing);
		const T& get(std::size_t idx);

		nodeptr insert(nodeptr foo, std::size_t idx, int level, const T& thing);
		nodeptr pad(nodeptr foo, int new_level, int old_level);
		nodeptr remove(nodeptr foo, std::size_t idx, std::size_t level);
		std::size_t base_log(std::size_t n);
		unsigned key(std::size_t index, int level);
};

// namespace persist
}

#include <persist/imp/vector.hpp>
