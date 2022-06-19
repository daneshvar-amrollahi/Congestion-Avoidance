#include "sender.hpp"
#include "stdlib.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int id = atoi(argv[1]);
    char* ip = argv[2];
    int port_to_router = atoi(argv[3]);
    int port_from_router = atoi(argv[4]);
    Sender sender = Sender(
        id,
        ip, 
        port_from_router,
        port_to_router
    );
    sender.run();
}