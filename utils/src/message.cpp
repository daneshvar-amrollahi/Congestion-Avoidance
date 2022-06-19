#include "../include/message.hpp"
#include "../defs.hpp"
#include <fstream>

using namespace std;
 
void Message::read_file(){
    
    // content={"this","is","a","dumb","test","and","I","am","bored"};

    ifstream fin(FILE_ADDRESS);
	char c[CHUNK_SIZE];
	while (fin.read(c, CHUNK_SIZE))
    {
        string chunk = "";
        for (int i = 0; i < CHUNK_SIZE; i++)
            chunk += c[i];
        content.push_back(chunk);
    }
}

void Message::store_file(){
    //TBC
}

frame Message::get_frame(int seq_num){
    return content[seq_num];
}

void Message::store_frame(int seq_num , frame input){
    content[seq_num]=input;
    received_frames[seq_num]=true;
}

int Message::get_size(){
    return content.size();
}
int Message::get_frame_size(){
    return 1;
}

void Message::set_size(int sz) {
    content = vector<frame> (sz);
    received_frames = vector<bool> (sz,false);
}

bool Message::is_frame_received(int seq_num){
    return received_frames[seq_num];
}