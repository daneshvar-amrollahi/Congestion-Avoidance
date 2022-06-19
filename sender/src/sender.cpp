#include "sender.hpp"
#include "../../utils/defs.hpp"
#include<stdio.h>

using namespace std;

typedef string frame;


Sender::Sender(
            int id,
            char* ip, 
            int port_from_router,
            int port_to_router
        ) {
    this->id=id;
    sockets=vector<Socket*>(10);
    Socket* socket;
    
    socket=new Socket(ip,port_to_router);
    sockets[socket->fd]=socket;
    send_fd=socket->fd;
    
    socket=new Socket(ip,port_from_router);
    sockets[socket->fd]=socket;
    receive_fd=socket->fd;

    message.read_file();

    sent_times = vector<clock_t> (message.get_size());

    LFS = 0;
    LAR = -1;
}

void Sender::send_new_frames()
{
    while (LFS - LAR <= SWS)
    {
        cout << "Sending frame " << LFS << "..." << endl<<LOG_DELIM;
        sent_times[LFS] = clock();
        sockets[send_fd]->send(get_next_frame());
        sleep(1.5);
    }
}

int get_seq_num(string message) {
    int i = 0;
    for (i; i < message.size(); i++)
        if (message[i] == DELIMETER)
            break;
    i++;
    string count = "";
    for (i; i < message.size(); i++){
        if(message[i]==DELIMETER)
            break;
        count += message[i];
    }
    return stoi(count);
}


bool Sender::time_out() {
    if(LAR + 1 >=message.get_size())
        return false;
    clock_t current_time = clock();
    float time_elapsed = (current_time - sent_times[LAR + 1]) / CLOCKS_PER_SEC;
    return time_elapsed > PACKET_LOST_THRESHOLD;
}


void Sender::retransmit() {
    for (int i = LAR + 1; i < LFS; i++)
    {
        sent_times[i] = clock();
        cout << "Retransmitting frame " << i << "..." << endl<<LOG_DELIM;
        sockets[send_fd]->send(create_frame(i));
    }
}

int get_sender_id(string message){
    string num="";
    int i;
    for(i= message.size()-1;i>=0;i--){
        if(message[i]==DELIMETER)
            break;
    }
    i++;
    for (i ; i < message.size() ; i++){
        num+=message[i];
    }
    
    return stoi(num);
}



void Sender::run() {
    int max_sd;
    int bytes;
    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    max_sd = max(receive_fd,send_fd);
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(send_fd, &master_set);
    FD_SET(receive_fd, &master_set);

    int starting_time;

    sleep(5);

    starting_time = clock();
    sockets[send_fd]->send("$" + to_string(this->id) + DELIMETER + to_string(message.get_size()));

    while (1)
    {

        read_set = master_set;
        bytes=select(max_sd + 1, &read_set, NULL, NULL, NULL);
        if (FD_ISSET(receive_fd, &read_set))
        {
            string recv_message = sockets[receive_fd]->receive();
            if(get_sender_id(recv_message)!=this->id){
                continue;
            } 
            cout<<"recv_message = " << recv_message<<endl<<LOG_DELIM;
            if (recv_message.substr(0,4) == FIRST_ACK){
                send_new_frames();
            }
            else 
            {
                int seq_num = get_seq_num(recv_message);
                LAR = seq_num;
                if(!all_frames_sent()){
                        send_new_frames();
                }
            }  
        }
        if (time_out())
        {
            cout << "TIMEOUT occured for " << LAR + 1 << endl<<LOG_DELIM;
            retransmit();
        }
        if(LAR==message.get_size()-1){
            cout<<"TRANSMIT IS OVER"<<endl<<LOG_DELIM;
            break;
        }
    }

    int exec_time = (clock()-starting_time)/CLOCKS_PER_SEC;

    cout << "EXECUTION TIME: " << exec_time << "s" << endl;
}  

frame Sender::create_frame(int seq_num)
{
    return to_string(id) + DELIMETER + to_string(seq_num) + DELIMETER + message.get_frame(seq_num); 
}

frame Sender::get_next_frame() {

    string ret = create_frame(LFS);
    LFS++;
    return ret;
}

bool Sender::all_frames_sent(){
    return LFS>message.get_size()-1;
}