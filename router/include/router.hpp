#ifndef __ROUTER_HPP__
#define __ROUTER_HPP__

#include <vector>
#include <queue>

#include "../../utils/include/socket.hpp"
#include "../../utils/defs.hpp"


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
        void add_to_buffer(frame message);
        void pop_buffer();
        bool buffer_timeout();
    private:
        char* ip;
        int sender_send_fd;
        int sender_receive_fd;
        int receiver_send_fd;
        int receiver_receive_fd;
        std::vector<Socket*>sockets;
        std::queue<frame> buffer; 
        clock_t last_send;
};

#endif