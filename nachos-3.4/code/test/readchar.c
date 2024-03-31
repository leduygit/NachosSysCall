#include "syscall.h"

int main() {
    char c;
    c = ReadChar();
    PrintChar(c);
    Exit(0);
}