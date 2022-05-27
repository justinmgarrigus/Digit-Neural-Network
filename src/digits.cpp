#include <iostream>
#include <iomanip> 
#include <fstream> 
#include <cstring> 
#include <iomanip>
#include <stdlib.h> 
#include <Windows.h> 
#include <cstdio> 
#include <WinCon.h> 
#include <winuser.h>
#include <ctime> 
#include <chrono> 
#include <math.h> 
#include "canvas.h"
#include "network.h"

long long start_time; 
long long time_millis() {
	using namespace std::chrono; 
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
	);
	return ms.count() - start_time; // How many milliseconds have passed since we started the program. 
}

struct input_package {
	int count;
	int width, height; 
	double** images; 
	double** labels; 
	
	input_package(char* image_file_name, char* label_file_name) {
		std::ifstream image_file(image_file_name, std::ios::in | std::ios::binary);
		if (!image_file.is_open()) {
			std::cout << "Error: " << image_file_name << " could not be opened" << std::endl; 
			exit(-1);
		}
		
		if (read_int(&image_file) != 2051) {
			std::cout << "Error: image file formatted incorrectly" << std::endl;
			exit(-1);
		}
		 
		count = read_int(&image_file); 
		width = read_int(&image_file);
		height = read_int(&image_file);
			
		std::ifstream label_file(label_file_name, std::ios::in | std::ios::binary); 
		if (!label_file.is_open()) {
			std::cout << "Error: " << label_file_name << " could not be opened" << std::endl; 
			exit(-1);
		}
		
		if (read_int(&label_file) != 2049) {
			std::cout << "Error: label file formatted incorrectly" << std::endl;
			exit(-1);
		}
		
		if (read_int(&label_file) != count) {
			std::cout << "Error: size of label file doesn't match size of image file" << std::endl; 
			exit(-1); 
		}
		
		images = new double*[count]; 
		for (int x = 0; x < count; x++) {
			images[x] = new double[width * height]; 
			for (int i = 0; i < width * height; i++) 
				images[x][i] = (unsigned char)image_file.get() / 255.0;
		}			
		
		labels = new double*[count]; 
		for (int x = 0; x < count; x++) {
			labels[x] = new double[10]; 
			unsigned char label = label_file.get();
			for (int i = 0; i < 10; i++) {
				if (i == label) labels[x][i] = 1; 
				else labels[x][i] = 0; 
			}
		}
	}
	
	~input_package() {
		for (int i = 0; i < count; i++) { 
			delete[] images[i]; 
			delete[] labels[i]; 
		}
		delete[] images; 
		delete[] labels; 
	}
	
	int buffer_int(char* buffer) {
		return (int)(
			(unsigned char)(buffer[0]) << 24 | 
			(unsigned char)(buffer[1]) << 16 | 
			(unsigned char)(buffer[2]) << 8  | 
			(unsigned char)(buffer[3])); 
	}

	int read_int(std::ifstream* file) {
		char buffer[sizeof(int)]; 
		file->read(buffer, sizeof(int)); 
		return buffer_int(buffer); 
	}
};

int main(int argc, char* argv[]) {
	start_time = time_millis(); 
	std::cout << std::unitbuf; 
	SetConsoleOutputCP(CP_UTF8); 
	setvbuf(stdout, nullptr, _IOFBF, 1000);
	
	if (argc != 5) {
		std::cout << "Usage: ./digits <train_image_file.idx3-ubyte> <train_label_file.idx1-ubyte> <test_image_file.idx3-ubyte> <test_label_file.idx1-ubyte>" << std::endl; 
		exit(-1);
	}
	else {
		struct input_package *data = new input_package(argv[1], argv[2]);
		
		// Create and train network
		int size = 3; 
		int lengths[size] = {784, 30, 10}; 
		Network *network = new Network(lengths, size, 3.0, 10);
		network->train(data->images, data->labels, data->count, 10); 
		
		delete data; 
		data = new input_package(argv[3], argv[4]); 
		
		Canvas *canvas = new Canvas(data->width, data->height);
		for (int i = 0; i < data->count; i++) {
			std::cout << "Press any key to continue..."; 
			std::cin.ignore(); 
			std::cout << std::endl; 
			
			for (int y = 0; y < data->height; y++)
				for (int x = 0; x < data->width; x++)
					canvas->set_pixel(x, y, (float)data->images[i][y * data->width + x]); 
			canvas->display(); 
			
			for (int d = 0; d < 10; d++) 
				if (data->labels[i][d] == 1)
					std::cout << "Label: " << d << std::endl; 
				
			std::cout << "Predicted digit: " << network->test(data->images[i]) << std::endl; 
		}

	
		delete network; 
		delete data; 
		delete canvas;
	}
}