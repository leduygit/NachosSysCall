#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main() {
    int i, id, j, temp, count;
    char s[MAX_LENGTH];

    // Create the file "ascii.txt" if it doesn't exist
    CreateF("ascii.txt");

    // Open the file with write permissions
    id = OpenF("ascii.txt", 0);
    if (id < 0) {
        PrintString("Failed to open file\n");
        return 1; // Return with error code
    }

    // Write ASCII characters to the file
    for (i = 32; i < 127; i++) {
        // Convert integer i to ASCII decimal representation manually
        temp = i;
        count = 0;
        while (temp != 0) {
            temp /= 10;
            count++;
        }
        temp = i;
        for (j = count - 1; j >= 0; j--) {
            s[j] = (temp % 10) + '0';
            temp /= 10;
        }
        s[count] = ' ';
        s[count + 1] = i;
        s[count + 2] = '\n';
        s[count + 3] = '\0';
        // Write the string to the file
        WriteF(s, count + 3, id);
    }

    // Close the file
    CloseF(id);

    // print notification
    PrintString("Write ASCII table to file ascii.txt successfully\n");

    return 0;
}
