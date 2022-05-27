#ifndef PROGRESS_H 
#define PROGRESS_H

#include <windows.h> 

// A progress bar.
class Progress {
	private: 
		static HANDLE hStdout;
		static COORD get_console_position(); 
		
		int width; 
		COORD start_position;
		int previous_fill; 
	
	public: 
		Progress(int width); 
		void initialize(); 
		void update(double percentage);
}; 

#endif 