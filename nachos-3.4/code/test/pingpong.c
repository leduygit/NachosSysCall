#include "syscall.h"

void main() {
    SpaceId PingID, PongID;
    int pingSem, pongSem;
    PrintString("PingPong\n");
    pingSem = CreateSemaphore("Ping", 1);
    pongSem = CreateSemaphore("Pong", 0);
    PingID = Exec("./test/ping");
    PongID = Exec("./test/pong");
    Join(PingID);
    Join(PongID);
}