#include "receiver.hpp"
#include "stdlib.h"

int main(int argc, char* argv[]) {

    char* ip = argv[1];
    int port_to_router = atoi(argv[2]);
    int port_from_router = atoi(argv[3]);
    Receiver receiver = Receiver(
        ip, 
        port_to_router,
        port_from_router
    );
    receiver.run();
}