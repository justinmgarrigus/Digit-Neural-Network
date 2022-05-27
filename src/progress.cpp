#include <iostream> // cout 
#include <windows.h> // Console operations 
#include "progress.h"

HANDLE Progress::hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

COORD Progress::get_console_position() {
	CONSOLE_SCREEN_BUFFER_INFO csbInfo; 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbInfo)) {
		std::cout << "Error getting screen buffer info" << std::endl; 
		exit(-1);
	}
	
	return csbInfo.dwCursorPosition; 
}	

Progress::Progress(int width) {
	this->width = width; 
}

void Progress::initialize() {
	start_position = get_console_position();
	std::cout << "["; 
	for (int i = 0; i < width; i++) 
		std::cout << " ";
	std::cout << "]"; 
	previous_fill = 0; 
}

// Percentage should be between 0 and 1 
void Progress::update(double percentage) {
	int amount_filled = (int)(percentage * width);
	if (amount_filled != previous_fill) {
		previous_fill = amount_filled; 
		COORD cout_pos = get_console_position();
		if (!SetConsoleCursorPosition(hStdout, start_position)) {
			std::cout << "Unable to set cursor position."; 
			exit(-1); 
		}
		
		char out[width + 2 + 1]; // Two brackets and null-terminating char 
		out[0] = '['; 
		
		int i; 
		for (i = 0; i < amount_filled; i++) 
			out[i+1] = '='; 
		
		for (; i < width; i++)
			out[i+1] = ' ';  
		
		out[i+1] = ']';
		out[i+2] = '\0'; 
		
		if (!WriteConsole(hStdout, out, width + 2, NULL, NULL)) {
			std::cout << "Unable to write buffer to output."; 
			exit(-1); 
		}
		
		if (!SetConsoleCursorPosition(hStdout, cout_pos)) {
			std::cout << "Unable to set cursor position."; 
			exit(-1);
		}
	}
}