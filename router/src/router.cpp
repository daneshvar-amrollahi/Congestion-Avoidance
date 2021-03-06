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

    last_send=clock();
}

int Router::get_sender_id(string message) { 
    string num = "";
    for (int i = 0; i < message.size(); i++)
    {
        if (message[i] == DELIMETER)
            return stoi(num);
        num += message[i];
    }
    return 0;
}

int Router::get_seq_num(string message) { 
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

string Router::get_data(string message) { 
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


void Router::add_to_buffer(frame message) {
    if(message[0]=='$'){
        sockets[receiver_send_fd]->send(message);
        cout<<"Transmitting message \""<< message <<"\" from sender to receiver..."<<endl<<LOG_DELIM;
        return;
    }

    int seq_num = get_seq_num(message);
    string data = get_data(message);
    int sender_id = get_sender_id(message);
    if (buffer.size() <= MIN_DROP_THRESHOLD)
    {
        buffer.push(message);
        cout << "Buffered (" << "sender=" << sender_id << ", seq_num=" << seq_num << ", data=" << data << ")" << endl << LOG_DELIM;
        return;
    }
    else if (buffer.size() >= MIN_DROP_THRESHOLD && buffer.size() < MAX_DROP_THRESHOLD)
    {
        float prob = float(rand())/RAND_MAX;
        float drop_prob = RED_DROP_RATE * (( (int)buffer.size() ) - MIN_DROP_THRESHOLD);
        if(prob <= drop_prob){
            cout<<"Oops I dropped ("<< "sender=" << sender_id << ", seq_num=" << seq_num << ", data=" << data << ")" <<" :)))"<<endl<<LOG_DELIM;
        }else{
            buffer.push(message);
            cout << "Buffered (" << "sender=" << sender_id << ", seq_num=" << seq_num << ", data=" << data << ")" << endl << LOG_DELIM;
        }
    }else{
        cout<<"Oops the buffer is full -> dropped ("<< "sender=" << sender_id << ", seq_num=" << seq_num << ", data=" << data << ")" <<" :)))"<<endl<<LOG_DELIM;
    }
    return;
}



void Router::run() {
    srand((unsigned)time(NULL));
    int max_sd;
    int bytes;
    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    max_sd = receiver_receive_fd;
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(sender_receive_fd, &master_set);
    FD_SET(sender_send_fd, &master_set);
    FD_SET(receiver_receive_fd, &master_set);
    FD_SET(receiver_send_fd, &master_set);
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
        if(buffer_timeout()){
            pop_buffer();
        }
    }

}

void Router::pop_buffer(){
    if(!buffer.empty()){
        frame message = buffer.front();
        int seq_num = get_seq_num(message);
        string data = get_data(message);
        int sender_id = get_sender_id(message);

        sockets[receiver_send_fd]->send(message);
        buffer.pop();

        cout<<"Transmitting (" << "sender=" << sender_id << ", seq_num=" << seq_num << ", data=" << data << ")"  <<" from buffer..."<<endl<<LOG_DELIM;
    }
}

bool Router::buffer_timeout(){
    if((clock()-last_send)/(CLOCKS_PER_SEC/1000)>BUFFER_SEND_THRESHOLD){
        last_send=clock();
        cout<<"BUFFER-PROCESSING DONE"<<endl<<LOG_DELIM;
        return true;
    }
    return false;
}