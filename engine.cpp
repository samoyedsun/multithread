#include "engine.h"
#include "config.h"

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
    this->cli_fd_list_.clear();

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

    while(true)
    {
        char msg_len_char[sizeof(int16_t)] = {0};
        memset(msg_len_char, 0, sizeof(int16_t));
        if (recv(client_fd, msg_len_char, sizeof(int16_t), 0) == -1)
        {
            if (errno == EAGAIN || errno == EINTR)
            {
                    cout << "recv_msg1 errno:" << errno << endl;
                    continue;
            }
            cout << "recv_msg2 errno:" << errno << endl;
            continue;
        }

        int16_t msg_len_int16 = *(int16_t *)msg_len_char;
        if (msg_len_int16 == 0)
        {
            cout << "client operator errno1:" << errno << endl;
            break;
        }

        char *msg_info = (char *)malloc(msg_len_int16);
        memset(msg_info, 0, msg_len_int16);
        if (recv(client_fd, msg_info, msg_len_int16, 0) == -1)
        {
            free(msg_info);
            if (errno == EAGAIN || errno == EINTR)
                    continue;
            cout << "client operator errno2:" << errno << endl;
            continue;
        }

        if (engine_ptr->process_handle(&msg_len_int16, msg_info) == false)
        {
            free(msg_info);
            close(client_fd);
            break;
        }

        if (msg_len_int16 == 0)
        {
            free(msg_info);
            continue;
        }

        char *msg_info_buf = (char *)malloc(msg_len_int16 + sizeof(int16_t));
        memcpy(msg_info_buf, (char *)&msg_len_int16, sizeof(int16_t));
        memcpy(msg_info_buf + sizeof(int16_t), msg_info, msg_len_int16);
        free(msg_info);
        if (send(client_fd, msg_info_buf, msg_len_int16 + sizeof(int16_t), 0) == -1)
            cout << "service operator errno:" << errno << endl;

        free(msg_info_buf);
    }
    pthread_exit((void *)1);
}

bool Engine::process_handle(int16_t *msg_len_int16, char *msg_info)
{
    int16_t msg_number = *(int16_t *)msg_info;
    char *msg_info_buf = msg_info + sizeof(int16_t);

    switch (msg_number)
    {
        case 1:
            cout << "client_msg1:" << string(msg_info_buf) << endl;
            {
                std::string name;
                int pos = string(msg_info_buf).find('#');
                name.append(msg_info_buf, pos);
                string password;
                password.append(msg_info_buf, pos + 1, strlen(msg_info_buf) - pos - 1);
                cout << "name:" << name << endl;
                cout << "password:" << password << endl;
                *msg_len_int16 = 0;
            }
            break;
        case 2:
            cout << "client_msg2:" << string(msg_info_buf) << endl;;
            for (int i = 0; i < strlen(msg_info_buf); ++i)
            {
                if (msg_info_buf[i] >= 65 && msg_info_buf[i] <= 90)
                    msg_info_buf[i] += 32;
                if (msg_info_buf[i] >= 97 && msg_info_buf[i] <= 122)
                    msg_info_buf[i] -= 32;
            }
            break;
        default:
            return false;
    }
    return true;
}
