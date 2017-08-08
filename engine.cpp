#include "engine.h"
#include "config.h"
#include "data_packet.h"

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

Engine::Engine()
{
    memset((void *)&this->svc_addr_, 0, sizeof(this->svc_addr_));
    memset((void *)&this->peer_addr_, 0, sizeof(this->peer_addr_));
    this->svc_fd_ = 0;

    this->svc_addr_.sin_family = AF_INET;
    this->svc_addr_.sin_port = htons(PORT);
    this->svc_addr_.sin_addr.s_addr = inet_addr(ADDR); }

Engine::~Engine()
{
    if (this->svc_fd_ > 2)
        close(this->svc_fd_);
}

void Engine::start()
{
    if (this->create_socket() &&
            this->bind_socket() &&
            this->listen_socket())
    {
        this->accept_socket();
    }
}

bool Engine::create_socket()
{
    if ((this->svc_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout << "create_socket errno:" << errno << endl;
        return false;
    }

    return true;
}

bool Engine::bind_socket()
{
    if (bind(this->svc_fd_, (struct sockaddr *)&this->svc_addr_, sizeof(this->svc_addr_)) == -1)
    {
        close(this->svc_fd_);
        cout << "bind_socket errno:" << errno << endl;
        return false;
    }
    return true;
}

bool Engine::listen_socket()
{
    if (listen(this->svc_fd_, LISTEN_NUM) == -1)
    {
        close(this->svc_fd_);
        cout << "liste_socket errno:" << errno << endl;
        return false;
    }
    return true;
}

void Engine::accept_socket()
{
    cout << "server start success!" << endl;

    socklen_t peer_len = sizeof(this->peer_addr_);
    while (true)
    {
        int client_fd = accept(this->svc_fd_, (struct sockaddr *)&this->peer_addr_, &peer_len);
        if (client_fd == -1)
        {
            cout << "accept_socket errno:" << errno << endl;
            continue;
        }

        pthread_t tid;
        pthread_attr_t attr;
        if (pthread_attr_init(&attr) != 0)
        {
            cout << "pthread_attr_init errno:" << errno << endl;
            continue;
        }

        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
        {
            cout << "pthread_attr_setdetachstate errno:" << errno << endl;
            pthread_attr_destroy(&attr);
            continue;
        }

        if (pthread_create(&tid, &attr, unit_process, (void *)this) != 0)
        {
            cout << "create_unit_process errno:" << errno << endl;
            pthread_attr_destroy(&attr);
            continue;
        }

        this->client_info_map_[tid] = client_fd;
        cout << "accept client request!" << endl;

        char ip[INET6_ADDRSTRLEN];
        if (inet_ntop(this->peer_addr_.sin_family, (void *)&this->peer_addr_.sin_addr, ip, sizeof(ip)))
        {
            cout << "ip:" << string(ip) << endl;
            cout << "port:" << this->peer_addr_.sin_port<< endl;
        }
    }
}

void *Engine::unit_process(void *this_)
{
    Engine *engine_ptr = (Engine *)this_;
    cout << "process id:" << getpid() << endl;
    cout << "pthread id:" << pthread_self() << endl;

    map<pthread_t, int>::iterator client_info_iter;
    while (true)
    {
        client_info_iter = engine_ptr->client_info_map_.find(pthread_self());
        if (client_info_iter != engine_ptr->client_info_map_.end())
            break;
        cout << "no find target, sleep 1 ms ..." << endl;
        sleep(1);
    }

    pthread_t tid = client_info_iter->first;
    pthread_detach(tid);
    int client_fd = client_info_iter->second;
    cout << "tid:" << tid << endl;
    cout << "client_fd:" << client_fd<< endl;

    engine_ptr->client_obj_map_[client_fd] = Actor();
    Actor &actor_obj = engine_ptr->client_obj_map_[client_fd];

    while(true)
    {
        char msg_len_char[sizeof(int16_t)] = {0};
        if (recv(client_fd, msg_len_char, sizeof(int16_t), 0) == -1)
        {
            if (errno == EINTR)
            {
                    cout << "recv signal break errno:" << errno << endl;
                    continue;
            }
            cout << "recv unknown error1:" << errno << endl;
            break;
        }

        int16_t msg_len_int16 = *((int16_t *)msg_len_char);
        cout << "msg_len_int16_t : " << msg_len_int16 << endl;
        if (msg_len_int16 == 0)
        {
            cout << "client disconnect:" << errno << endl;
            break;
        }

        DataPacket recv_packet(msg_len_int16);
        if (recv(client_fd, recv_packet.get_data_ptr(), msg_len_int16, 0) == -1)
        {
            if (errno == EINTR)
                    continue;
            cout << "recv unknown error2:" << errno << endl;
            continue;
        }

        if (actor_obj.process(client_fd, &recv_packet) == false)
        {
            close(client_fd);
            break;
        }

        if (msg_len_int16 == 0)
        {
            continue;
        }

    }
    pthread_exit((void *)1);
}
