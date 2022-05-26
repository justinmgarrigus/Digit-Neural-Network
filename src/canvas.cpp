#include <iostream>
#include <fstream> 
#include <Windows.h> 
#include "canvas.h"

Canvas::Canvas() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(handle, &info)) {
		SMALL_RECT rect = info.srWindow; 
		init(rect.Right - rect.Left, rect.Bottom - rect.Top - 1); 
	}
	else {
		std::cout << "Console buffer unable to be obtained!" << std::endl;
		exit(-1); 
	}
}

Canvas::Canvas(const int width, const int height) {
	init(width, height); 
}

void Canvas::init(const int width, const int height) {
	colors = new char*[height]; 
	for (int x = 0; x < width; x++) {
		colors[x] = new char[height];
		for (int y = 0; y < height; y++) 
			colors[x][y] = 0; 
	}
	this->width = width; 
	this->height = height; 
}

Canvas::~Canvas() {
	for (int i = 0; i < width; i++) 
		delete[] colors[i]; 
	delete[] colors;
}

void Canvas::clear() {
	for (int x = 0; x < width; x++) 
		for (int y = 0; y < height; y++) 
			colors[x][y] = 0; 
}

void Canvas::set_pixel(int x, int y, int brightness) {
	colors[x][y] = brightness; 
}

// Brightness is a percentage (1 = full bright, 0 = full dark) 
void Canvas::set_pixel(int x, int y, float brightness) {
	colors[x][y] = (int)(brightness * 5); 
}

void Canvas::display() {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) { 
			char brightness = colors[x][y]; 
			switch (brightness) {
				case 0: std::cout << " "; break; 
				case 1: std::cout << "░"; break; 
				case 2: std::cout << "▒"; break; 
				case 3: std::cout << "▓"; break; 
				default: std::cout << "█"; break; 
			}
		}
		std::cout << std::endl;
	}
}