#ifndef __RECEIVER_HPP__
#define __RECEIVER_HPP__

#include <string>
#include <vector>

#include "../../utils/include/socket.hpp"
#include "../../utils/include/message.hpp"

class Receiver {
    public:
        Receiver(
            char* ip,
            int port_to_router,
            int port_from_router
        );
        void run();
        void handle_recv_msg(std::string message);

    private:
        char* ip;
        int port;
        int send_fd;
        int receive_fd;
        int LFR;
        //int LAF;
        std::vector<Socket*>sockets;
        Message message;
};

#endif