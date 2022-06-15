#include "sender.hpp"
#include "stdlib.h"

int main(int argc, char* argv[]) {

    char* ip = argv[1];
    int port_to_router = atoi(argv[2]);
    int port_from_router = atoi(argv[3]);
    Sender sender = Sender(
        ip, 
        port_from_router,
        port_to_router
    );
    sender.run();
}