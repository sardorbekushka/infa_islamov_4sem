// g++ -std=c++17 main.cpp -I/opt/local/include/ /opt/local/lib/libsfml-graphics.dylib /opt/local/lib/libsfml-audio.dylib  /opt/local/lib/libsfml-window.dylib /opt/local/lib/libsfml-system.dylib -Ofast -lgsl -lcblas -o main

#include <iostream>
#include "lensSolver.hpp"
#include "renderer.hpp"

int main() {
	std::string sourceFile = "resources/images/BubbleNebulaMini.jpeg";
    auto solver = new LensSolver(5e33, 0.5, 1);
	Renderer renderer(solver, sourceFile, 900);
	int code = renderer.poll();
	delete solver;
	return code;
}