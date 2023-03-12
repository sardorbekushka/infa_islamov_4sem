// g++ -std=c++17 main.cpp -I/opt/local/include/ /opt/local/lib/libsfml-graphics.dylib /opt/local/lib/libsfml-audio.dylib  /opt/local/lib/libsfml-window.dylib /opt/local/lib/libsfml-system.dylib -O3 -lgsl -lcblas -o

#include <iostream>
// #include "lensSolver.hpp"
#include "renderer.hpp"

int main() {
    auto solver = new LensSolver<double>(1e30, 0.5, 1.0, 60, 0.0, 0.0);
	Renderer<double> renderer(solver, "resources/Milky-Way.png");

	int code = renderer.poll();
	delete solver;
	return code;
}