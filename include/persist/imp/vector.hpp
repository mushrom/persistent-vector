#pragma once

namespace persist {

template <typename T>
class vector<T>::value {
	public:
		virtual bool is_leaf() = 0;
};

template <typename T>
class vector<T>::internal : public vector<T>::value {
	public:
		virtual bool is_leaf() { return false; };
		nodeptr ptrs[1 << NODE_BITS] = {nullptr};
};

template <typename T>
class vector<T>::leaf : public vector<T>::value {
	public:
		virtual bool is_leaf() { return true; };
		T values[1 << NODE_BITS];
};

template <typename T>
class vector<T>::reference {
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

template <typename T>
T& vector<T>::operator=(const T& other) {
	elements = other.elements;
	root = other.root;
}

template <typename T>
std::size_t vector<T>::size(void) {
	return elements;
};

template <typename T>
const T& vector<T>::operator[](std::size_t idx) const {
	return get(idx);
};

template <typename T>
typename vector<T>::reference vector<T>::operator[](std::size_t idx) {
	return reference(this, idx);
}

template <typename T>
void vector<T>::push_back(T datum) {
	set(elements, datum);
}

template <typename T>
typename vector<T>::nodeptr
vector<T>::insert(vector<T>::nodeptr foo,
                  std::size_t idx,
                  int level,
                  const T& thing)
{
	nodeptr ret = nullptr;

	/*
	std::cout << "insert(): "
		<< "index: " << idx
		<< ", level: " << level
		<< ", usage: " << foo.use_count() << std::endl;
		*/

	if (level == 1) {
		//leaf *temp = dynamic_cast<leaf*>(foo.get());
		leaf *temp = (leaf*)(foo.get());
		leaf *asdf = new leaf();
		ret = nodeptr(asdf);

		if (temp) {
			for (unsigned i = 0; i < 1 << NODE_BITS; i++) {
				asdf->values[i] = temp->values[i];
			}
		}

		asdf->values[key(idx, level)] = thing;

	} else {
		internal *temp = (internal*)(foo.get());
		internal *asdf = new internal();
		ret = nodeptr(asdf);

		for (unsigned i = 0; i < 1 << NODE_BITS; i++) {
			asdf->ptrs[i] = temp? temp->ptrs[i] : nullptr;
		}

		nodeptr next = asdf->ptrs[key(idx, level)];
		asdf->ptrs[key(idx, level)] = insert(next, idx, level - 1, thing);
	}

	return ret;
}

template <typename T>
typename vector<T>::nodeptr
vector<T>::pad(nodeptr foo, int new_level, int old_level) {
	nodeptr ret = foo;

	for (; old_level < new_level; old_level++) {
		internal *temp = new internal();
		temp->ptrs[0] = ret;

		//std::cout << "pad(): padding " << old_level << " -> " << new_level << std::endl;
		ret = nodeptr(temp);
	}

	return ret;
}

template <typename T>
typename vector<T>::nodeptr
vector<T>::remove(nodeptr foo,
                  std::size_t idx,
                  std::size_t level)
{
	nodeptr ret = nullptr;

	return ret;
}

template <typename T>
std::size_t vector<T>::base_log(std::size_t n) {
	std::size_t ret = 0;

	while (n) {
		ret += 1;
		n >>= NODE_BITS;
	}

	return ret;
}

template <typename T>
unsigned vector<T>::key(std::size_t index, int level) {
	return (index >> (NODE_BITS * (level - 1))) & MASK;
}

template <typename T>
void vector<T>::set(std::size_t idx, const T& thing) {
	std::size_t idx_level = base_log(idx + 1);
	std::size_t size_level = base_log(elements);

	if (idx >= elements) {
		elements = idx + 1;
	}

	//std::cout << "set(): got here" << std::endl;
	root = pad(root, idx_level, size_level);
	//std::cout << "set(): and here" << std::endl;
	root = insert(root, idx, base_log(elements), thing);
}

template <typename T>
const T& vector<T>::get(std::size_t idx) {
	if (idx >= elements) {
		throw "wut";
	}

	int level = base_log(elements);
	nodeptr temp = root;

	for (; level > 1; level--) {
		if (temp == nullptr || temp->is_leaf() == true) {
			throw std::out_of_range("vector::get(): invalid index (internal): " + std::to_string(idx));
		}

		temp = ((internal*)temp.get())->ptrs[key(idx, level)];
	}

	if (temp == nullptr || temp->is_leaf() == false) {
		std::cout << temp << std::endl;
		throw std::out_of_range("vector::get(): invalid index (leaf): " + std::to_string(idx));
	}

	return ((leaf*)temp.get())->values[key(idx, level)];
}

/*
template <typename T>
pvector<T>::nodeptr pvector<T>::remove(pvector<T>::nodeptr foo, std::size_t idx, std::size_t radix) {
nodeptr ret = nullptr;

return ret;
}
*/

// namespace persist
}
