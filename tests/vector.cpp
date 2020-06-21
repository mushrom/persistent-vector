#include <persist/vector.hpp>

#include <iostream>
#include <cstdio>
#include <test.hpp>

using namespace tester;

int main(int argc, char *argv[]) {
	typedef persist::vector<long> T;

	context<T> ctx("vector", [](uint32_t u){ return T(); });
	ctx.check("insert", [](T& data){
		for (long x = 0; x < 128; x++) {
			data.push_back(x);
		}

		return data.size() == 128;
	});

	ctx.check("valid", [](T& data){
		bool valid = true;
		for (long x = 0; x < 128; x++) {
			valid = valid && data[x] == x;
		}

		return valid;
	});

	ctx.check("remove", [](T& data){
		return data.size() == 128;
	});

	ctx.check("sparse_insert", [](T& data) {
		for (long x = 0; x < 0x80; x++) {
			data[x + 0x80000000] = x;
		}

		return data.size() == 0x80000080;
	});

	ctx.check("sparse_valid", [](T& data) {
		bool valid = true;
		for (long x = 0; x < 0x80; x++) {
			valid = valid && data[x + 0x80000000] == x;
		}

		return valid;
	});

	ctx.check("persistence", [](T& data) {
		T temp = data;

		for (long x = 0; x < 0x80; x++) {
			temp[x] = x*x + 1;
		}

		bool valid = true;
		for (long x = 0; x < 0x80; x++) {
			valid = valid
				&& temp[x] == x*x + 1
				&& data[x] == x
				&& temp[x] != data[x]
				&& temp[x + 0x80000000] == data[x + 0x80000000];
		}

		return valid;
	});

	return 0;
}
