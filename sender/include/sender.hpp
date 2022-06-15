#ifndef __SENDER_HPP__
#define __SENDER_HPP__

#include<vector>
#include "../../utils/include/message.hpp"
#include "../../utils/include/socket.hpp"

class Sender {
    public:
        Sender(
            char* ip, 
            int port_from_router,
            int port_to_router
        );
        void run();
        frame create_frame(int seq_num);
        frame get_next_frame();
        bool all_frames_sent();
        void send_new_frames();
        bool time_out();
        void retransmit();
    private:
        char* ip;
        int port;
        int send_fd;
        int receive_fd;
        std::vector<Socket*>sockets;
        Message message;
        int LFS;
        int LAR;
        std::vector<clock_t> sent_times;
};

#endif