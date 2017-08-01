#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "data_packet.h"
using namespace std;

int main()
{
	cout << getpid() << endl;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		perror("socket");

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9988);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		perror("accept");

	while(1)
	{
		getchar();
        DataPacket dp;
        dp << sizeof(int16_t);
        dp << 23;
       	write(fd, dp.get_data_ptr(), dp.get_data_size());

        int16_t msg_len;
		read(fd, &msg_len, sizeof(msg_len));
        int16_t msg_num;
		read(fd, &msg_num, sizeof(msg_num));
        cout << msg_num << endl;
	}

	cout << ntohs(addr.sin_port) << endl;
	close(fd);
	return 0;
}
