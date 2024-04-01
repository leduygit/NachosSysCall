#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{   
    int i, id;
    char s[MAX_LENGTH];

    // Create the file "ascii.txt" if it doesn't exist
    Create("ascii.txt");

    id = Open("ascii.txt", 0);
    if (id < 0) {
        PrintString("Failed to open file\n");
        return 1; // Return with error code
    }

    for (i = 0; i < 128; i++)
    {
        s[0] = i;
        Write(s, 0, id);
    }

    // Close the file
    Close(id);

    return 0;
}
