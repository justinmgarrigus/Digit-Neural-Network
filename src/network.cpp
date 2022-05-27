#include <math.h> // exp
#include <stdlib.h> // rand, RAND_MAX
#include <iostream> // cout, fixed
#include <iomanip> // setprecision
#include <string> // std::string
#include "network.h"
#include "progress.h" 

long long time_millis(); 

double random01() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX); 
}

Network::Network(int* lengths, int count, double learning_rate, int batch_size) {
	this->lengths = lengths; 
	this->count = count; 
	this->learning_rate = learning_rate; 
	this->batch_size = batch_size; 
	
	nodes = new double*[count]; 
	for (int i = 0; i < count; i++) 
		nodes[i] = new double[lengths[i]]; 
	
	signals = new double*[count]; 
	deriv = new double*[count]; 
	biases = new double*[count]; 
	gradient_biases = new double*[count]; 
	for (int i = 1; i < count; i++) { // start at 1, index 0 unused 
		signals[i] = new double[lengths[i]]; 
		deriv[i] = new double[lengths[i]];
		gradient_biases[i] = new double[lengths[i]];
		
		biases[i] = new double[lengths[i]]; 
		for (int row = 0; row < lengths[i]; row++) 
			biases[i][row] = random01() * 2 - 1; 
	}
	
	weights = new double**[count-1];
	gradient_weights = new double**[count-1]; 
	for (int layer = 0; layer < count-1; layer++) {
		weights[layer] = new double*[lengths[layer]]; 
		gradient_weights[layer] = new double*[lengths[layer]]; 
		for (int row = 0; row < lengths[layer]; row++) {
			gradient_weights[layer][row] = new double[lengths[layer+1]]; 
			weights[layer][row] = new double[lengths[layer+1]]; 
			for (int index = 0; index < lengths[layer+1]; index++) 
				weights[layer][row][index] = random01() * 2 - 1; 
		}
	}
}

Network::~Network() {
	for (int i = 0; i < count; i++) 
		delete[] nodes[i];
	delete[] nodes; 
	
	for (int i = 1; i < count; i++) {
		delete[] signals[i]; 
		delete[] deriv[i]; 
		delete[] biases[i]; 
		delete[] gradient_biases[i]; 
	}
	delete[] signals; 
	delete[] deriv; 
	delete[] biases; 
	delete[] gradient_biases; 
	
	for (int layer = 0; layer < count-1; layer++) {
		for (int row = 0; row < lengths[layer]; row++) {
			delete[] weights[layer][row]; 
			delete[] gradient_weights[layer][row]; 
		}
		delete[] weights[layer]; 
		delete[] gradient_weights[layer]; 
	}
	delete[] weights; 
	delete[] gradient_weights; 
}

double Network::sigmoid(double value) {
	return 1.0 / (1.0 + exp(-value)); 
}

double Network::deriv_sigmoid(double value) {
	double sig = sigmoid(value); 
	return sig * (1.0 - sig); 
}

void Network::feedforward(double* input) {
	// Set inputs
	for (int row = 0; row < lengths[0]; row++)
		nodes[0][row] = input[row];
	
	for (int layer = 1; layer < count; layer++) { // Layer 0 (inputs) do not need to be set 
		for (int row = 0; row < lengths[layer]; row++) {
			signals[layer][row] = 0; // Summation 
			
			// Multiply input to node by cooresponding weight and add bias  
			for (int index = 0; index < lengths[layer-1]; index++)
				signals[layer][row] += nodes[layer-1][index] * weights[layer-1][index][row]; 
			signals[layer][row] += biases[layer][row];
			
			// Apply activation function
			nodes[layer][row] = sigmoid(signals[layer][row]);
		}
	}
}

double Network::backpropagate(double* input, double* expected_output) {
	// Set each value in nodes and signals 
	feedforward(input); 
	
	// 1. Output's error 
	for (int row = 0; row < lengths[count-1]; row++)
		deriv[count-1][row] = (nodes[count-1][row] - expected_output[row]) * deriv_sigmoid(signals[count-1][row]);
	
	// 2. Backpropagate the error 
	for (int layer = count-2; layer > 0; layer--) {
		for (int row = 0; row < lengths[layer]; row++) {
			// Dot product of weights * errors 
			double sum = 0; 
			for (int index = 0; index < lengths[layer+1]; index++) 
				sum += weights[layer][row][index] * deriv[layer+1][index]; 
			deriv[layer][row] = sum * deriv_sigmoid(signals[layer][row]); 
		}
	}
	
	// 3. Gradient descent (weights) 
	for (int layer = 0; layer < count-1; layer++)
		for (int row = 0; row < lengths[layer]; row++)
			for (int index = 0; index < lengths[layer+1]; index++) 
				gradient_weights[layer][row][index] += deriv[layer+1][index] * nodes[layer][row];
			
	// 4. Gradient descent (biases) 
	for (int layer = 1; layer < count; layer++)
		for (int row = 0; row < lengths[layer]; row++) 
			gradient_biases[layer][row] += deriv[layer][row]; 
		
	// Now gradient_weights and gradient_biases have been updated!
	// We should return the square of the magnitude of the output - expected vector. 
	double output_difference = 0;
	for (int row = 0; row < lengths[count-1]; row++) 
		output_difference += abs(expected_output[row] - nodes[count-1][row]);
	return pow(output_difference, 2); 
}

double Network::update_mini_batch(double** inputs, double** expected_outputs, int offset) {
	// Clear gradient_weights 
	for (int layer = 0; layer < count-1; layer++) 
		for (int row = 0; row < lengths[layer]; row++) 
			for (int index = 0; index < lengths[layer+1]; index++)
				gradient_weights[layer][row][index] = 0; 
			
	// Clear gradient_biases
	for (int layer = 1; layer < count; layer++) 
		for (int row = 0; row < lengths[layer]; row++) 
			gradient_biases[layer][row] = 0; 
	
	double error_sum = 0; 
	for (int test = 0; test < batch_size; test++)
		error_sum += backpropagate(inputs[test + offset], expected_outputs[test + offset]);
	
	// Update weights 
	for (int layer = 0; layer < count-1; layer++) 
		for (int row = 0; row < lengths[layer]; row++) 
			for (int index = 0; index < lengths[layer+1]; index++)
				weights[layer][row][index] -= (learning_rate / batch_size) * gradient_weights[layer][row][index];
			
	// Update biases 
	for (int layer = 1; layer < count; layer++)
		for (int row = 0; row < lengths[layer]; row++)
			biases[layer][row] -= (learning_rate / batch_size) * gradient_biases[layer][row];
	
	return error_sum; 
}

void Network::train(double** inputs, double** expected_outputs, int training_size, int epochs) {
	std::cout << "Training:" << std::setprecision(3) << std::fixed << std::endl; 
	Progress progress = Progress(10); 
	double error; 
	double total_time = 0; 
	for (int epoch = 0; epoch < epochs; epoch++) {
		double error_sums = 0;
		std::cout << "   Epoch " << epoch + 1 << ": "; 
		progress.initialize();
		double start_time = time_millis(); 
		for (int batch = 0; batch < training_size / batch_size; batch++) {
			error_sums += update_mini_batch(inputs, expected_outputs, batch * batch_size);
			
			progress.update(batch / ((double)training_size / batch_size)); 
		}
		progress.update(1); // Completed bar 
		
		// All training has been completed for the epoch. Display the new error to profile performance
		error = (1.0 / (2 * training_size)) * error_sums;
		std::cout << " " << error;
		
		// Display time in a nice format
		double time = time_millis() - start_time; 
		total_time += time; 
		std::string time_format = "ms"; 
		if (time > 1000) {
			time /= 1000; 
			time_format = "s";
		}
			
		std::cout << " (" << time << time_format << ")" << std::endl;
	}
	
	std::string time_format = "ms"; 
	if (total_time > 1000) {
		total_time /= 1000; 
		time_format = "s"; 
	}
	if (total_time > 60) {
		total_time /= 60; 
		time_format = "m"; 
	}
	
	std::cout << "MSE: " << std::setprecision(5) << error << ", duration: " <<  total_time << time_format << std::endl; 
}

int Network::test(double* input) {
	feedforward(input); 
	for (int i = 0; i < lengths[count-1]; i++) 
		if (nodes[count-1][i] > 0.5)
			return i; 
	return -1; 
}