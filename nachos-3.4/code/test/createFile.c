#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 255
int main()
{	
	int id;
	if (CreateF("test.txt") == 0) PrintString("Create file success\n");
	id = OpenF("test.txt", 0);
	if (id >= 0 && id <= 10) PrintString("Open file success\n");
	WriteF("Hello world", 11, id);
 	CloseF(id);
	return 0;
}