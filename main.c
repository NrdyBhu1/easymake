#include <stdio.h>
#include "easycolor.h"

int main(void)
{
	ezcolor_set(COLOR_RED, 0); printf("Hello, ");
	ezcolor_set(COLOR_GREEN, 0); printf("world!\n");
	
	ezcolor_set(COLOR_DARK_WHITE, 0); //Reset to default
	return 0;
}
