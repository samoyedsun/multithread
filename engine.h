#ifndef MY_SOCKET
#define MY_SOCKET

#include <iostream>
#include <queue>
#include <list>
#include <map>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

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

    static void *unit_process(void*);
    bool process_handle(int16_t *msg_len_int16, char *msg_info);

    inline pid_t gettid()
    {
        return syscall(SYS_gettid);
    }

    struct sockaddr_in svc_addr_;
    struct sockaddr_in peer_addr_;

    queue<void *> msg_queue_send_;
    queue<void *> msg_queue_recv_;

    int svc_fd_;
    map<pthread_t, int> client_info_map_;
    list<int> cli_fd_list_;
};

#endif
