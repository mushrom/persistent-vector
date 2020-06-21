# GNU make
CXXFLAGS=-std=c++11 -Wall -g -Og -MD -Iinclude -Itests/include

TEST_SRC=$(wildcard tests/*.cpp)
TEST_DEP=$(TEST_SRC:.cpp=.d)
TEST_PROG=$(TEST_SRC:.cpp=.test)
TEST_RESULTS=$(TEST_SRC:.cpp=.result)

test: $(TEST_RESULTS)

-include $(TEST_DEP)

tests/%.test: tests/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

tests/%.result : tests/%.test
	$< | tee $@

.PHONY: clean
clean:
	rm -f $(TEST_PROG) $(TEST_RESULTS) $(TEST_DEP)
