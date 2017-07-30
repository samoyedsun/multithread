#include "my_socket.h"

int main()
{
    MySocket server;

    if (server.create_socket() == false ||
            server.bind_socket() == false ||
            server.listen_socket() == false)
        return 0;

    server.accept_socket();
	return 0;
}

