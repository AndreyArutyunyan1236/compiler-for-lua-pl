CXX = g++
CXXFLAGS = -std=c++23 -I./include/ -Wall -Wextra
CXXSRC = src/lexer.cpp

all:
	$(CXX) $(CXXFLAGS) -o lua++ main.cpp $(CXXSRC) 

cc:
	compile make
