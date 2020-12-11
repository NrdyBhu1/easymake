#ifndef EASYCOLOR_H
#define EASYCOLOR_H

#ifdef _WIN32
	#include <windows.h>
#endif

#include <stdio.h>

#define COLOR_BLACK				  0
#define COLOR_BLUE			 	  1
#define COLOR_GREEN				  2
#define COLOR_CYAN				   3
#define COLOR_RED				    4
#define COLOR_MAGENTA			    5
#define COLOR_YELLOW		   	  6
#define COLOR_DARK_WHITE		     7
#define COLOR_BRIGHT_BLACK		   8
#define COLOR_BRIGHT_BLUE		    9
#define COLOR_BRIGHT_GREEN		   10
#define COLOR_BRIGHT_CYAN		    11
#define COLOR_BRIGHT_RED		     12
#define COLOR_BRIGHT_MAGENTA	     13
#define COLOR_BRIGHT_YELLOW		  14
#define COLOR_WHITE				  15

#define BG_COLOR_BLACK	   		0
#define BG_COLOR_BLUE   		 	16
#define BG_COLOR_GREEN  		 	32
#define BG_COLOR_CYAN   		 	48
#define BG_COLOR_RED    		 	64
#define BG_COLOR_MAGENTA  	   	80
#define BG_COLOR_YELLOW 	 		96
#define BG_COLOR_DARK_WHITE	  	112
#define BG_COLOR_BRIGHT_BLACK    	128
#define BG_COLOR_BRIGHT_BLUE	 	144
#define BG_COLOR_BRIGHT_GREEN		160
#define BG_COLOR_BRIGHT_CYAN	 	176
#define BG_COLOR_BRIGHT_RED	  	192
#define BG_COLOR_BRIGHT_MAGENTA  	208
#define BG_COLOR_BRIGHT_YELLOW   	224
#define BG_COLOR_WHITE		   	240

#define ANSI_COLOR_BLACK				  "\x1b[30m"
#define ANSI_COLOR_BLUE				   "\x1b[34m"
#define ANSI_COLOR_GREEN				  "\x1b[32m"
#define ANSI_COLOR_CYAN				   "\x1b[36m"
#define ANSI_COLOR_RED					"\x1b[31m"
#define ANSI_COLOR_MAGENTA				"\x1b[35m"
#define ANSI_COLOR_YELLOW				 "\x1b[33m"
#define ANSI_COLOR_DARK_WHITE			 "\x1b[37m"
#define ANSI_COLOR_BRIGHT_BLACK		   "\x1b[90m"
#define ANSI_COLOR_BRIGHT_BLUE			"\x1b[94m"
#define ANSI_COLOR_BRIGHT_GREEN		   "\x1b[92m"
#define ANSI_COLOR_BRIGHT_CYAN			"\x1b[96m"
#define ANSI_COLOR_BRIGHT_RED			 "\x1b[91m"
#define ANSI_COLOR_BRIGHT_MAGENTA		 "\x1b[95m"
#define ANSI_COLOR_BRIGHT_YELLOW		  "\x1b[93m"
#define ANSI_COLOR_WHITE				  "\x1b[97m"

void
ezcolor_set(int foreground, int background)
{
	#ifdef _WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, foreground + background);
	#else
		switch (foreground) {
			case COLOR_BLACK: {
				printf(ANSI_COLOR_BLACK "");
			}
			case COLOR_BLUE: {
				printf(ANSI_COLOR_BLUE "");
			}
			case COLOR_GREEN: {
				printf(ANSI_COLOR_GREEN "");
			}
			case COLOR_CYAN: {
				printf(ANSI_COLOR_CYAN "");
			}
			case COLOR_RED: {
				printf(ANSI_COLOR_RED "");
			}
			case COLOR_MAGENTA: {
				printf(ANSI_COLOR_MAGENTA "");
			}
			case COLOR_YELLOW: {
				printf(ANSI_COLOR_YELLOW "");
			}
			case COLOR_DARK_WHITE: {
				printf(ANSI_COLOR_DARK_WHITE "");
			}
			case COLOR_BRIGHT_BLACK: {
				printf(ANSI_COLOR_BRIGHT_BLACK "");
			}
			case COLOR_BRIGHT_BLUE: {
				printf(ANSI_COLOR_BRIGHT_BLUE "");
			}
			case COLOR_BRIGHT_GREEN: {
				printf(ANSI_COLOR_BRIGHT_GREEN "");
			}
			case COLOR_BRIGHT_CYAN: {
				printf(ANSI_COLOR_BRIGHT_CYAN "");
			}
			case COLOR_BRIGHT_RED: {
				printf(ANSI_COLOR_BRIGHT_RED "");
			}
			case COLOR_BRIGHT_MAGENTA: {
				printf(ANSI_COLOR_BRIGHT_MAGENTA "");
			}
			case COLOR_BRIGHT_YELLOW: {
				printf(ANSI_COLOR_BRIGHT_YELLOW "");
			}
			case COLOR_WHITE: {
				printf(ANSI_COLOR_WHITE "");
			}
		}
	#endif
}

#endif
