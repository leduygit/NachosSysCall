#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 255
int main()
{	
	int id;
	char *s;
	id = Open("test.txt", 0);
	Read(s, 2, id);
	Close(id);
	PrintString(s);
	return 0;
}
