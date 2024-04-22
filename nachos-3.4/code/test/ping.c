#include "syscall.h"

void main() {
    int i;
    for (i = 0; i < 1000; i++) {
        Down("Ping");
        PrintChar('A');
        Up("Pong");
    }
}