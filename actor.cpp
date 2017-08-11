#include "actor.h"
#include "data_packet.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

Actor::Actor()
{
    this->name = "name";
    this->password = "123456";
}

Actor::~Actor()
{

}

bool Actor::process(int client_fd, void* recv_packet)
{
    DataPacket &request = *(DataPacket *)recv_packet;
    int16_t msg_number;
    request >> msg_number;
    std::cout << "msg_number : " << msg_number << std::endl;

    switch (msg_number)
    {
        case 1:
            std::cout << "msg_number : " << msg_number << std::endl;
            break;

        case 2:
            std::cout << "msg_number : " << msg_number << std::endl;
            break;

        case 3:
            std::cout << "msg_number : " << msg_number << std::endl;
            break;

        default:
            std::cout << "非法指令：" << msg_number << std::endl;

    }

    DataPacket respond;
    int16_t len = sizeof(int16_t);
    int16_t num = msg_number + 1;
    respond << len;
    respond << num;
    return this->send_msg(client_fd, &respond);
}

bool Actor::send_msg(int client_fd, void* send_packet)
{
    DataPacket &respond = *(DataPacket *)send_packet;
    int ret = send(client_fd, respond.get_data_ptr(), respond.get_data_size(), 0);
    if (-1 == ret)
    {
        std::cout << "service operator errno:" << errno << std::endl;
        return false;
    }
    std::cout << "respond end" << respond.get_data_size() << std::endl;
    return true;
}
