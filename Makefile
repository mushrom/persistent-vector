CXXFLAGS=-std=c++17 -Wall -g -O0

persist_vector: persist_vector.o
	$(CXX) -o $@ $<

.PHONY: clean
clean:
	rm -f persist_vector *.o