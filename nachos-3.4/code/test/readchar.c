#include "syscall.h"

int main() {
    char c, s[10];
    c = ReadChar();
    PrintChar(c);
    PrintChar('\n');

    // test readString
    ReadString(s, 10);
    PrintString(s);
    
    Exit(0);
}
