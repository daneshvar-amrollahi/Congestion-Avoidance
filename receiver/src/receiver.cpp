#include "receiver.hpp"
#include <iostream>
#include "../../utils/defs.hpp"
#include <string>

using namespace std;


Receiver::Receiver(
            char* ip,
            int port_to_router,
            int port_from_router
        ) {

    sockets=vector<Socket*>(10);
    Socket* socket;
    
    socket=new Socket(ip,port_to_router);
    sockets[socket->fd]=socket;
    send_fd=socket->fd;
    
    socket=new Socket(ip,port_from_router);
    sockets[socket->fd]=socket;
    receive_fd=socket->fd;

    for (int i = 0; i < MAX_SENDERS; i++)
        LFR[i]=0;
}

void Receiver::run() {
    int fd;
    int server_fd, room_fd=-1, max_sd, write_to;
    char buff[1049] = {0};
    char buffer[1024] = {0};
    char QandA[1024]={0};
    char tmp[1049]={0};
    string input;
    int id,bytes,room_type;
    fd_set master_set, read_set, write_set;
    FD_ZERO(&master_set);
    max_sd = receive_fd;
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(receive_fd, &master_set);
    FD_SET(send_fd, &master_set);
    write_to = server_fd;
    while (1)
    {
        read_set = master_set;
        bytes=select(max_sd + 1, &read_set, NULL, NULL, NULL);
        if (FD_ISSET(receive_fd, &read_set))
        {
            handle_recv_msg(sockets[receive_fd]->receive());            
        }
        if(FD_ISSET(STDIN_FILENO, &read_set)){
            cin>>input;
            sockets[send_fd]->send(input);
        }
        memset(buffer, 0, 1024);
    }

}  

int Receiver::get_sender_id(string message) { 
    string num = "";
    for (int i = 0; i < message.size(); i++)
    {
        if (message[i] == DELIMETER)
            return stoi(num);
        num += message[i];
    }
    return 0;
}

int Receiver::get_seq_num(string message) { 
    int i = 0;
    for (i; i < message.size(); i++)
        if (message[i] == DELIMETER)
            break;
    i++;
    string num = "";
    for (i; i < message.size(); i++)
    {
        if (message[i] == DELIMETER)
            return stoi(num);
        num += message[i];
    }
    return 0;
}

string Receiver::get_data(string message) { 
    int c = 0, i = 0;
    for (i; i < message.size(); i++)
    {
        if (message[i] == DELIMETER)
            c++;
        if (c == 2)
            break;
    }
    i++;
    string data = "";
    for (i; i < message.size(); i++)
        data += message[i];
    return data;
}

// $ID;COUNT
int Receiver::get_packet_count(std::string message) {
    int i = 0;
    for (i; i < message.size(); i++)
        if (message[i] == DELIMETER)
            break;
    i++;
    string count = "";
    for (i; i < message.size(); i++)
        count += message[i];
    return stoi(count);
}


void Receiver::handle_recv_msg(std::string message) {
    if (message[0] == '$')
    {
        int sender_id = get_sender_id(message.substr(1, (int)(message.size()) - 1));

        cout << "sender_id=" << sender_id << endl;

        this->message[sender_id].set_size(get_packet_count(message));

        cout << "recv_message=" << message << endl;
        
        string boz(1, DELIMETER);
        string message_to_send = "ACK$" + boz + to_string(sender_id);
 
        cout << "message_to_send=" << message_to_send << endl;

        sockets[send_fd]->send(message_to_send);
    }else{
        int seq_num = get_seq_num(message);
        string data = get_data(message);
        int sender_id = get_sender_id(message);
        if(seq_num!=LFR[sender_id]){

            cout << "Discarded (" << "sender=" << sender_id << ", seq_num=" << seq_num << ", data=" << data << ")" << endl << LOG_DELIM;
        }else{
            cout << "Received (" << "sender=" << sender_id << ", seq_num=" << seq_num << ", data=" << data << ")" << endl << LOG_DELIM;
            this->message[sender_id].store_frame(seq_num, data);
            
            string boz(1, DELIMETER);
            
            string message_to_send = "ACK" + boz + to_string(LFR[sender_id]) + boz + to_string(sender_id); 
            sockets[send_fd]->send(message_to_send);
            
            cout << "Sending " << message_to_send << endl << LOG_DELIM;
            //cout << "Sending ACK" << LFR[sender_id]<< " to " + sender_id << "..." << endl << LOG_DELIM;
            LFR[sender_id]++;
        }
    }

}
