# GNU make
CXXFLAGS=-std=c++11 -Wall -g -Og -MD -Iinclude -Itests/include

TEST_SRC=$(wildcard tests/*.cpp)
TEST_DEP=$(TEST_SRC:.cpp=.d)
TEST_PROG=$(TEST_SRC:.cpp=.test)

.PHONY: test
test: $(TEST_PROG)
	@for thing in $(TEST_PROG); do $$thing; done

-include $(TEST_DEP)

tests/%.test: tests/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(TEST_PROG) $(TEST_RESULTS) $(TEST_DEP)
