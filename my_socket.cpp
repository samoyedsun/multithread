#include "./my_socket.h"

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

MySocket::MySocket()
{
    memset((void *)&this->svc_addr_, 0, sizeof(this->svc_addr_));
    memset((void *)&this->peer_addr_, 0, sizeof(this->peer_addr_));
    this->svc_fd_ = 0;
    this->cli_fd_list_.clear();

    this->svc_addr_.sin_family = AF_INET;
    this->svc_addr_.sin_port = htons(PORT);
    this->svc_addr_.sin_addr.s_addr = inet_addr(ADDR);
}

MySocket::~MySocket()
{
    if (this->svc_fd_ > 2)
        close(this->svc_fd_);
}

bool MySocket::create_socket()
{
    if ((this->svc_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create_socket errno:%d\n", errno);
        return false;
    }

    return true;
}

bool MySocket::bind_socket()
{
    if (bind(this->svc_fd_, (struct sockaddr *)&this->svc_addr_, sizeof(this->svc_addr_)) == -1)
    {
        close(this->svc_fd_);
        printf("bind_socket errno:%d\n", errno);
        return false;
    }
    return true;
}

bool MySocket::listen_socket()
{
    if (listen(this->svc_fd_, LISTEN_NUM) == -1)
    {
        close(this->svc_fd_);
        printf("liste_socket errno:%d\n", errno);
        return false;
    }
    return true;
}

void MySocket::accept_socket()
{
    cout << "进程ID：" << getpid() << endl;
    cout << "线程ID:" << pthread_self() << endl;
    cout << "全局线程ID：" << this->gettid() << endl;

    socklen_t peer_len = sizeof(this->peer_addr_);
    while (true)
    {
        int client_fd = accept(this->svc_fd_, (struct sockaddr *)&this->peer_addr_, &peer_len);
        if (client_fd == -1)
        {
            printf("accept_socket errno:%d\n", errno);
            continue;
        }

        pthread_t tid;
        pthread_attr_t attr;
        if (pthread_attr_init(&attr) != 0)
        {
            printf("pthread_attr_init errno:%d\n", errno);
            continue;
        }

        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
        {
            printf("pthread_attr_setdetachstate errno:%d\n", errno);
            pthread_attr_destroy(&attr);
            continue;
        }

        if (pthread_create(&tid, &attr, unit_process, (void *)this) != 0)
        {
            printf("create_unit_process errno:%d\n", errno);
            pthread_attr_destroy(&attr);
            continue;
        }

        this->client_info_map_[tid] = client_fd;
        printf("accept_socket success! client file des:%d, thread id:%ld\n", client_fd, tid);
    }
}

void *MySocket::unit_process(void *this_)
{
    MySocket * sock_ptr = (MySocket *)this_;
    cout << "进程ID：" << getpid() << endl;
    cout << "线程ID:" << pthread_self() << endl;
    cout << "全局线程ID：" << sock_ptr->gettid() << endl;

    map<pthread_t, int>::iterator client_info_it;
    while (true)
    {
        client_info_it = sock_ptr->client_info_map_.find(pthread_self());
        if (client_info_it != sock_ptr->client_info_map_.end())
            break;
        printf("用户信息未找到，等待1ms再找\n");
        sleep(1);
    }

    pthread_t tid = client_info_it->first;
    pthread_detach(tid);
    int client_fd = client_info_it->second;
    cout << "tid:" << tid << endl;
    cout << "client_fd:" << client_fd<< endl;

    while(true)
    {
        char msg_len_char[INT16_SIZE] = {0};
        memset(msg_len_char, 0, INT16_SIZE);
        if (recv(client_fd, msg_len_char, INT16_SIZE, 0) == -1)
        {
            if (errno == EAGAIN || errno == EINTR)
            {
                    printf("recv_msg1 errno:%d\n", errno);
                    continue;
            }
            printf("recv_msg2 errno:%d\n", errno);
            continue;
        }

        int16_t msg_len_int16 = *(int16_t *)msg_len_char;
        if (msg_len_int16 == 0)
        {
            printf("client operator errno1:%d\n", errno);
            break;
        }

        char *msg_info = (char *)malloc(msg_len_int16);
        memset(msg_info, 0, msg_len_int16);
        if (recv(client_fd, msg_info, msg_len_int16, 0) == -1)
        {
            free(msg_info);
            if (errno == EAGAIN || errno == EINTR)
                    continue;
            printf("client operator errno2:%d\n", errno);
            continue;
        }

        if (sock_ptr->process_handle(&msg_len_int16, msg_info) == false)
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

        char *msg_info_buf = (char *)malloc(msg_len_int16 + INT16_SIZE);
        memcpy(msg_info_buf, (char *)&msg_len_int16, INT16_SIZE);
        memcpy(msg_info_buf + INT16_SIZE, msg_info, msg_len_int16);
        free(msg_info);
        if (send(client_fd, msg_info_buf, msg_len_int16 + INT16_SIZE, 0) == -1)
            printf("service operator errno:%d\n", errno);

        free(msg_info_buf);
    }
    pthread_exit((void *)1);
}

bool MySocket::process_handle(int16_t *msg_len_int16, char *msg_info)
{
    int16_t msg_number = *(int16_t *)msg_info;
    char *msg_info_buf = msg_info + INT16_SIZE;

    switch (msg_number)
    {
        case 1:
            printf("client_msg1:%s\n", msg_info_buf);
            *msg_len_int16 = 0;
            break;
        case 2:
            printf("client_msg2:%s\n", msg_info_buf);
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
