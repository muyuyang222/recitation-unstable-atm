CXX=clang++
CXX_FLAGS=-std=c++20 -Iincludes -Wall -Wextra -O0 -g -fsanitize=address

ARCH := $(shell uname -m)

ifeq ($(ARCH),x86_64)
    ATM_OBJ=./x86_64/atm.o
else ifeq ($(ARCH),arm64)
    ATM_OBJ=./arm64/atm.o
else ifeq ($(ARCH),aarch64)
    ATM_OBJ=./aarm64/atm.o
else
    $(error Unsupported architecture: $(ARCH))
endif

exec: bin/exec
tests: bin/tests

bin/exec: ./src/driver.cc ./includes/atm.hpp $(ATM_OBJ)
	$(CXX) $(CXX_FLAGS) ./src/driver.cc $(ATM_OBJ) -o $@

bin/tests: ./tests/tests.cc ./includes/atm.hpp $(ATM_OBJ)
	$(CXX) $(CXX_FLAGS) ./tests/tests.cc $(ATM_OBJ) -o $@

.DEFAULT_GOAL := exec
.PHONY: exec tests clean

clean:
	rm -rf bin/*
