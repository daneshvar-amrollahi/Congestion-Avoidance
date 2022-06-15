#include "router.hpp"
#include "../../utils/defs.hpp"


using namespace std;

Router::Router(
            char* ip,
            int port_to_sender,
            int port_from_sender,
            int port_to_receiver,
            int port_from_receiver
        ) {
    sockets=vector<Socket*>(10);
    Socket* socket;
    
    socket=new Socket(ip,port_to_sender);
    sockets[socket->fd]=socket;
    sender_send_fd=socket->fd;

    socket=new Socket(ip,port_from_sender);
    sockets[socket->fd]=socket;
    sender_receive_fd=socket->fd;
    
    socket=new Socket(ip,port_to_receiver);
    sockets[socket->fd]=socket;
    receiver_send_fd=socket->fd;
    
    socket=new Socket(ip,port_from_receiver);
    sockets[socket->fd]=socket;
    receiver_receive_fd=socket->fd;
}

void Router::add_to_buffer(frame message) {
    if (buffer.size() <= MIN_DROP_THRESHOLD)
    {
        buffer.push_back(message);
        cout << "Buffered " << message << endl << LOG_DELIM;
        return;
    }
    if (buffer.size() >= MIN_DROP_THRESHOLD && buffer.size() < MAX_DROP_THRESHOLD)
    {
        float prob = float(rand())/RAND_MAX;
        float drop_prob = RED_DROP_RATE * (( (int)buffer.size() ) - MIN_DROP_THRESHOLD);
        if(prob <= drop_prob){
            cout<<"Oops I dropped packet no."<< message[0] <<" :)))"<<endl<<LOG_DELIM;
        }else{
            buffer.push_back(message);
            cout << "Buffered " << message << endl << LOG_DELIM;
        }
    }
    return;
}



void Router::run() {
    srand((unsigned)time(NULL));
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
    max_sd = receiver_receive_fd;
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(sender_receive_fd, &master_set);
    FD_SET(sender_send_fd, &master_set);
    FD_SET(receiver_receive_fd, &master_set);
    FD_SET(receiver_send_fd, &master_set);
    write_to = server_fd;
    string recieved_message;
    while (1)
    {
        read_set = master_set;
        bytes=select(max_sd + 1, &read_set, NULL, NULL, NULL);
        if (FD_ISSET(sender_receive_fd, &read_set))
        {
            recieved_message=sockets[sender_receive_fd]->receive();
            add_to_buffer(recieved_message);
        }
        if (FD_ISSET(receiver_receive_fd, &read_set))
        {
            recieved_message=sockets[receiver_receive_fd]->receive();
            sockets[sender_send_fd]->send(recieved_message);
            cout<<"Transmitting message \""<< recieved_message <<"\" from receiver to sender..."<<endl<<LOG_DELIM;
        }
        memset(buffer, 0, 1024);
    }

}  