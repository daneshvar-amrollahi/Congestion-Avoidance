#include "receiver.hpp"
#include <iostream>
#include "../../utils/defs.hpp"

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

    LFR=0;
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
            //cout<<sockets[receive_fd]->receive()<<endl;
        }
        if(FD_ISSET(STDIN_FILENO, &read_set)){
            cin>>input;
            sockets[send_fd]->send(input);
        }
        memset(buffer, 0, 1024);
    }

}  

int get_seq_num(string message) {
    string num = "";
    for (int i = 0; i < message.size(); i++)
    {
        if (message[i] == DELIMETER)
            return stoi(num);
        num += message[i];
    }
    return 0;
}

string get_data(string message) {
    string data = "";
    int i = 0;
    for (i = 0; i < message.size(); i++)
        if (message[i] == DELIMETER)
            break;
    i++;
    for (i; i < message.size(); i++)
        data += message[i];
    return data;
}

void Receiver::handle_recv_msg(std::string message) {
    if (message[0] == '$')
    {
        this->message.set_size(stoi(message.substr(1, (int)(message.size()) - 1)));
        cout<<message<<endl;
        sockets[send_fd]->send("ACK$");
    }else{
        int seq_num = get_seq_num(message);
        string data = get_data(message);
        if(seq_num!=LFR){
            cout<<"Discarded frame no." <<seq_num << " saying: " << data << endl<<LOG_DELIM;
        }else{
            cout << "Received frame no." <<seq_num << " saying: " << data << endl<<LOG_DELIM;
            this->message.store_frame(seq_num, data);
            sockets[send_fd]->send("ACK" + to_string(LFR));
            cout<<"Sending ACK"<<LFR<<"..."<<endl<<LOG_DELIM;
            LFR++;
        }
    }

}
