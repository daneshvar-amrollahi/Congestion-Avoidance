#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string>

class Socket
{
private:
    struct sockaddr_in address;
public:
    int fd;
    Socket(char *ip, int port);
    int send(std::string data);
    std::string receive();
};
