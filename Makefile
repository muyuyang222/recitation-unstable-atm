CXX=clang++
CXX_FLAGS=-std=c++20 -Iincludes -Wall -Wextra -O0 -g -fsanitize=address -O0

exec: bin/exec
tests: bin/tests

bin/exec: ./src/driver.cc ./includes/atm.hpp ./src/atm.cc
	$(CXX) $(CXX_FLAGS) ./src/driver.cc ./src/atm.cc -o $@

bin/tests: ./tests/tests.cc ./includes/atm.hpp  ./src/atm.cc
	$(CXX) $(CXX_FLAGS) ./tests/tests.cc ./src/atm.cc -o $@

.DEFAULT_GOAL := exec
.PHONY: exec tests

clean:
	rm -rf bin/*
