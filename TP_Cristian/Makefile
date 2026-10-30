CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

all: cliente servidor_tiempo

cliente: cliente.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

servidor_tiempo: servidor_tiempo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f cliente servidor_tiempo

.PHONY: all clean