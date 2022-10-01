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
    	int16_t c = getchar();
        if (c == 10) continue;
        if (9 == (c -= 48)) break;

        DataPacket dp;
        dp << (int16_t)sizeof(int16_t);
        dp << c;
        cout << "send size:" << dp.get_data_size() << endl;
       	write(fd, dp.get_data_ptr(), dp.get_data_size());
        char msg_len_char[sizeof(int16_t)];
		read(fd, &msg_len_char, sizeof(int16_t));
        int16_t msg_len = *((int16_t *)msg_len_char);
        cout << "recv msg_len : " << msg_len << endl;


        DataPacket dp2(msg_len);
		read(fd, dp2.get_data_ptr(), msg_len);
        int16_t msg_num;
        dp2 >> msg_num;
        cout << "recv msg_num : " << msg_num << endl;
	}

	cout << ntohs(addr.sin_port) << endl;
	close(fd);
	return 0;
}
