#ifndef __ROUTER_HPP__
#define __ROUTER_HPP__

#include <vector>

#include "../../utils/include/socket.hpp"


class Router {
    public:
        Router(
            char* ip,
            int port_to_sender,
            int port_from_sender,
            int port_to_receiver,
            int port_from_receiver
        );
        void run();
    private:
        char* ip;
        int sender_send_fd;
        int sender_receive_fd;
        int receiver_send_fd;
        int receiver_receive_fd;
        std::vector<Socket*>sockets;
};

#endif