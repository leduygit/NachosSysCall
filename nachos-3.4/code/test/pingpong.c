#include "syscall.h"

void main() {
    SpaceId PingID, PongID;
    PrintString("PingPong\n");
    PingID = Exec("./test/ping");
    PongID = Exec("./test/pong");
    Join(PingID);
    Join(PongID);
}