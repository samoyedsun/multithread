#ifndef ENGINE
#define ENGINE

#include <iostream>
#include <map>
#include <netinet/in.h>
#include "actor.h"
using namespace std;

class Engine
{
    public:
    Engine();
    ~Engine();

    public:
    void start();

    private:
    bool create_socket();
    bool bind_socket();
    bool listen_socket();
    void accept_socket();

    static void *unit_process(void* this_);

    struct sockaddr_in svc_addr_;
    struct sockaddr_in peer_addr_;

    int svc_fd_;
    map<pthread_t, int> client_info_map_;
    map<int, Actor> client_obj_map_;
};

#endif
