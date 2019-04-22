#include <iostream>
#include <memory>
#include <stdexcept>

template <class T>
class pvector {
  public:
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
		reference(pvector *p, std::size_t idx) {
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
		pvector *parent = nullptr;
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

	nodeptr insert(nodeptr foo, std::size_t idx, int level, const T& thing, bool mut=true) {
		nodeptr ret = nullptr;

		bool can_mut = mut && foo != nullptr && foo.unique();

		std::cout << "insert(): "
			<< "index: " << idx
			<< ", level: " << level
			<< ", usage: " << foo.use_count()
			<< ", can_mut: " << can_mut << std::endl;

		if (level == 1) {
			//leaf *temp = dynamic_cast<leaf*>(foo.get());
			leaf *temp = (leaf*)(foo.get());
			leaf *asdf = nullptr;
			//leaf *asdf = new leaf();

			if (can_mut) {
				asdf = temp;
				ret = foo;

			} else {
				asdf = new leaf();
				ret = nodeptr(asdf);

				if (temp) {
					for (unsigned i = 0; i < 1 << NODE_BITS; i++) {
						asdf->values[i] = temp->values[i];
					}
				}
			}

			/*
			leaf *asdf = new leaf();
			ret = nodeptr(asdf);

			if (temp) {
				for (unsigned i = 0; i < 1 << NODE_BITS; i++) {
					asdf->values[i] = temp->values[i];
				}
			}
			*/

			std::cout << "insert(): level 1: temp: " << temp << std::endl;
			std::cout << "insert(): level 1: ret: " << ret << std::endl;
			std::cout << "insert(): level 1: asdf: " << asdf << std::endl;;

			asdf->values[key(idx, level)] = thing;

		} else {
			//internal *temp = dynamic_cast<internal*>(foo.get());
			internal *temp = (internal*)(foo.get());
			//internal *asdf = new internal();
			internal *asdf = nullptr;

			if (can_mut) {
				std::cout << "insert(): we're here" << std::endl;
				asdf = temp;
				ret = foo;

			} else {
				asdf = new internal();
				ret = nodeptr(asdf);

				for (unsigned i = 0; i < 1 << NODE_BITS; i++) {
					asdf->ptrs[i] = temp? temp->ptrs[i] : nullptr;
				}
			}

			std::cout << "insert(): can mut: " << can_mut << std::endl;;
			std::cout << "insert(): temp: " << temp << std::endl;;
			std::cout << "insert(): asdf: " << asdf << std::endl;;
			std::cout << "insert(): asdf v2: " << asdf->ptrs[key(idx, level)] << std::endl;;

			//nodeptr next = asdf->ptrs[key(idx, level)];
			//std::cout << "insert(): next: " << next << std::endl;;
			//asdf->ptrs[key(idx, level)] = insert(next, idx, level - 1, thing, can_mut);
			//asdf->ptrs[key(idx, level)] = insert(asdf->ptrs[key(idx, level)], idx, level - 1, thing, can_mut);

			if (can_mut) {
				asdf->ptrs[key(idx, level)] = insert(std::move(asdf->ptrs[key(idx, level)]), idx, level - 1, thing, can_mut);
			} else {
				asdf->ptrs[key(idx, level)] = insert(asdf->ptrs[key(idx, level)], idx, level - 1, thing, can_mut);
			}

			std::cout << "insert(): now have: " << asdf->ptrs[key(idx, level)] << std::endl;;
			std::cout << "insert(): ret: " << ret << std::endl;
			std::cout << "insert(): asdf: " << asdf << std::endl;;
			std::cout << "got here" << std::endl;;
		}

		std::cout << "returning" << std::endl;;
		return ret;
	}

	nodeptr pad(nodeptr foo, int new_level, int old_level) {
		nodeptr ret = foo;

		for (; old_level < new_level; old_level++) {
			internal *temp = new internal();
			temp->ptrs[0] = ret;

			std::cout << "pad(): padding " << old_level << " -> " << new_level << std::endl;
			ret = nodeptr(temp);
		}

		return ret;
	}

	nodeptr remove(nodeptr foo, std::size_t idx, std::size_t level) {
		nodeptr ret = nullptr;

		return ret;
	}
};

template <class T>
void pvector<T>::set(std::size_t idx, const T& thing) {
	std::size_t idx_level = base_log(idx + 1);
	std::size_t size_level = base_log(elements);

	if (idx >= elements) {
		elements = idx + 1;
	}

	//std::cout << "set(): got here" << std::endl;
	//root = pad(std::move(root), idx_level, size_level);
	root = pad(root, idx_level, size_level);
	//std::cout << "set(): and here" << std::endl;
	//root = insert(root, idx, base_log(elements), thing);
	root = insert(std::move(root), idx, base_log(elements), thing);
}

template <class T>
const T& pvector<T>::get(std::size_t idx) {
	if (idx >= elements) {
		throw "wut";
	}

	int level = base_log(elements);
	nodeptr temp = root;

	for (; level > 1; level--) {
		if (temp == nullptr || temp->is_leaf() == true) {
			throw std::out_of_range("pvector::get(): invalid index (internal): " + std::to_string(idx));
		}

		temp = ((internal*)temp.get())->ptrs[key(idx, level)];
	}

	if (temp == nullptr || temp->is_leaf() == false) {
		std::cout << temp << std::endl;
		throw std::out_of_range("pvector::get(): invalid index (leaf): " + std::to_string(idx));
	}

	return ((leaf*)temp.get())->values[key(idx, level)];
}

/*
template <class T>
pvector<T>::nodeptr pvector<T>::remove(pvector<T>::nodeptr foo, std::size_t idx, std::size_t radix) {
	nodeptr ret = nullptr;

	return ret;
}
*/

int main(int argc, char *argv[]) {
	srand(time(nullptr));

	unsigned len = (argc > 1)? atoi(argv[1]) : 128;

	pvector<unsigned long> foo;
	pvector<unsigned long> asdf;
	//std::map<unsigned, unsigned> foo;
	//std::map<unsigned, unsigned> asdf;

	unsigned long insum = 0;
	for (unsigned i = 0; i < len; i++) {
		unsigned lel = 2*i;

		foo[lel] = i;
		asdf[i] = lel;
		insum += i;
	}

	std::cout << "hmmmst've" << std::endl;

	pvector<unsigned long> meh = asdf;
	for (unsigned i = 0; i < len; i += 4) {
		meh[i] = 63*i;
	}

	std::cout << "hmmmmst?" << std::endl;
	meh[1024+len] = 1234;
	//meh[2*len] = 1234;
	std::cout << "got here" << std::endl;
	std::cout << "meh: " << meh.size()
		<< ", asdf: " << asdf.size()
		<< ", value: " << meh[1024+len]
		<< std::endl;

	for (unsigned i = 0; i < len; i++) {
		if (i % 4 == 0) {
			continue;
		}

		/*
		if (meh[i] != asdf[i]) {
			std::cout << "hmmm, " << meh[i] << " != " << asdf[i] << std::endl;
		}
		*/

		std::cout << "hmmm, " << asdf[i] << std::endl;
	}

	unsigned long outsum = 0;
	for (unsigned i = 0; i < len; i++) {
		if (foo[asdf[i]] != i) {
			std::cout << foo[asdf[i]] << " != " << i << "!" << std::endl;
			std::cout << "    " << asdf[i] << ", " << foo[asdf[i]] << std::endl;
		}

		outsum += foo[asdf[i]];
	}

	std::cout << "seems good: " << outsum << " == " << insum << " == " << (outsum == insum) << std::endl;

	//getchar();
	return 0;
}
