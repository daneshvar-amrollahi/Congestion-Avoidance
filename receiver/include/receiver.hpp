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
        int LFR[MAX_SENDERS];
        std::vector<Socket*>sockets;
        Message message[MAX_SENDERS];
        // Message format: ID;SEQ_NUM;DATA
        int get_seq_num(std::string message);
        std::string get_data(std::string message);
        int get_sender_id(std::string message);
        int get_packet_count(std::string message);
};

#endif