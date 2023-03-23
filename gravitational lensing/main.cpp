// g++ -std=c++17 main.cpp -I/opt/local/include/ /opt/local/lib/libsfml-graphics.dylib /opt/local/lib/libsfml-audio.dylib  /opt/local/lib/libsfml-window.dylib /opt/local/lib/libsfml-system.dylib -O3 -lgsl -lcblas -o main

#include <iostream>
// #include "lensSolver.hpp"
#include "renderer.hpp"

int main() {
    auto solver = new LensSolver<double>(2e41, 0.5, 1.0, 0.0, 0.0);
	Renderer<double> renderer(solver, "resources/Milky-Way.png");
	// Renderer<double> renderer(solver, "resources/original.png");
	// Renderer<double> renderer(solver, "resources/galaxy.jpg");
	// Renderer<double> renderer(solver, "resources/dark-image.jpg");
	// Renderer<double> renderer(solver, "resources/space.jpeg");
	// Renderer<double> renderer(solver, "resources/Milky Way galaxy as seen from Australia.jpg");

	int code = renderer.poll();
	delete solver;
	return code;
}