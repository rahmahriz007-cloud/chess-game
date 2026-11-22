# ---------------------------------------------------------------------------
#  C++ Chess Game  -  Makefile   (Linux / macOS / Windows with MSYS2 or MinGW)
#      make          build the game        ->  ./chess
#      make run      build and play
#      make demo     guided tour of the OOP features
#      make test     build and run the automatic test-suite
#      make clean    delete build files
# ---------------------------------------------------------------------------
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude

SRC_ALL  := $(wildcard src/*.cpp)
SRC_LIB  := $(filter-out src/main.cpp,$(SRC_ALL))
OBJ_ALL  := $(patsubst src/%.cpp,build/%.o,$(SRC_ALL))
OBJ_LIB  := $(patsubst src/%.cpp,build/%.o,$(SRC_LIB))

all: chess

chess: $(OBJ_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

build:
	mkdir -p build

run: chess
	./chess

demo: chess
	./chess --demo

test_chess: $(OBJ_LIB) tests/test_chess.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ tests/test_chess.cpp $(OBJ_LIB)

test: test_chess
	./test_chess

clean:
	rm -rf build chess test_chess chess.exe test_chess.exe

.PHONY: all run demo test clean
