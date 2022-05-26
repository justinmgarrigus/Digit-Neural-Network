#ifndef NETWORK_H
#define NETWORK_H 

class Network {
	private: 
		int* lengths;
		int count;
		double learning_rate; 
		int batch_size; 
		
		double** nodes; 
		double** signals; 
		double** deriv; 
		double** biases; 
		double** gradient_biases; 
		double*** weights; 
		double*** gradient_weights; 
		
		double sigmoid(double value);
		double deriv_sigmoid(double value);
		
		void feedforward(double* input);
		double backpropagate(double* input, double* expected_output); 
		double update_mini_batch(double** inputs, double** expected_outputs, int offset); 
	
	public: 
		Network(int* lengths, int count, double learning_rate, int batch_size); 
		~Network(); 
		
		void display(); 
		void train(double** inputs, double** expected_outputs, int training_size, int epochs);
		int test(double* input);
}; 

#endif 