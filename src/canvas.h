#ifndef CANVAS
#define CANVAS 

class Canvas {
	private: 
		char** colors;
		
		void init(int width, int height); 
	
	public: 
		int width, height; 
	
		Canvas(); 
		Canvas(int width, int height); 
		~Canvas(); 
		
		void clear(); 
		void set_pixel(int x, int y, int brightness); 
		void set_pixel(int x, int y, float brightness); 
		void display(); 
}; 

#endif 