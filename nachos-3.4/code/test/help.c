#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 255
int main()
{	
	int id;
    char s[MAX_LENGTH];
    id = Open("mota.txt", 0);
    // read until end of file
    while (Read(s, MAX_LENGTH, id) > 0)
    {
        Write(s, MAX_LENGTH, 1);
    }
    Close(id);
    return 0;
}
