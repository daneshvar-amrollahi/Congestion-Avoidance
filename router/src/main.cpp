#include "router.hpp"
#include "stdlib.h"

int main(int argc, char* argv[]) {

    srand((unsigned)time(NULL));
    char* ip = argv[1];
    int port_to_sender = atoi(argv[2]);
    int port_from_sender = atoi(argv[3]);
    int port_to_receiver = atoi(argv[4]);
    int port_from_receiver = atoi(argv[5]);
    Router router = Router(
        ip, 
        port_to_sender,
        port_from_sender,
        port_to_receiver,
        port_from_receiver
    );
    router.run();
}