#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include<vector>
#include<string>

typedef std::string frame;

class Message
{
private:
    std::vector<frame> content;
    std::vector<bool> received_frames;
public:
    frame get_frame(int seq_num);
    void store_frame(int seq_num, frame input);
    void read_file();
    void store_file();
    int get_size();
    int get_frame_size();
    void set_size(int sz);
    bool is_frame_received(int seq_num);
};




#endif
