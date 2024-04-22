#include "syscall.h"

void main() {
    SpaceId PingID, PongID;
    PrintString("PingPong\n");
    int semID = CreateSemaphore("PingPong", 1);
    PingID = Exec("./test/ping");
    PongID = Exec("./test/pong");
    Join(PingID);
    Join(PongID);
}