#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 255
int main()
{	
	int id;
    char s[MAX_LENGTH];
    id = OpenF("mota.txt", 0);
    // read until end of file
    while (ReadF(s, MAX_LENGTH, id) > 0) {
        PrintString(s);
    }
    CloseF(id);
    return 0;
}
