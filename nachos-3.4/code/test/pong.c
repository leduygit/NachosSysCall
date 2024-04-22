#include "syscall.h"

void main() {
    int i;
    for (i = 0; i < 1000; i++) {
        Down("PingPong");
        PrintChar('B');
        Up("PingPong");
    }
}