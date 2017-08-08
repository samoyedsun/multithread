#ifndef ACTOR
#define ACTOR

#include "data_packet.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

class Actor
{
    public:
        Actor();
        ~Actor();

        bool process(int client_fd, void* recv_packet);
        bool send_msg(int client_fd, void* send_packet);

    private:
        std::string name;
        std::string password;
};

#endif
