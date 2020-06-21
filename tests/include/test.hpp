#include <iostream>
#include <stdexcept>
#include <string>

namespace tester {

template <typename T>
class context {
	public:
		typedef T (*test_constructor)(uint32_t seed);
		typedef bool (*test_func)(T& thing);

		context(std::string n,
		        test_constructor cn,
		        uint32_t s = 1234)
			: name(n), seed(s)
		{
			data = cn(seed);
		}

		void check(std::string name, test_func fn);

		T data;
		std::string name;
		uint32_t seed;
};

template <typename T>
void context<T>::check(std::string testname, test_func fn) {
	std::string refname = name + "." + testname;
	std::cout << refname << ": ";

	if (!fn(data)) {
		std::cout << "failed" << std::endl;
		throw std::logic_error(refname);
	}

	std::cout << "passed" << std::endl;
}

// namespace tester
}
