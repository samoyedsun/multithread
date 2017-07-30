#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
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
        char *msg_buf = (char *)malloc(sizeof("samoyedsun") + sizeof(int16_t) * 2);
        *(int16_t *)msg_buf = sizeof("samoyedsun") + sizeof(int16_t);
        *(int16_t *)(msg_buf + sizeof(int16_t)) = 2;
        memcpy(msg_buf + sizeof(int16_t) * 2, "samoyedsun", sizeof("samoyedsun"));
		write(fd, msg_buf, sizeof("samoyedsun") + sizeof(int16_t) * 2);
        free(msg_buf);

        char msg_len[2] = {0};
		read(fd, msg_len, 2);
        char msg_num[2] = {0};
		read(fd, msg_num, 2);
        char msg_info[100] = {0};
		read(fd, msg_info, *(int16_t *)msg_len - 2);
	//	read(fd, msg_info, sizeof("samoyedsun"));
        printf("server_info:%d\t%d\t%s\t%ld \n", *(int16_t *)msg_len,
                *(int16_t *)msg_num, msg_info, sizeof("samoyedsun"));
	}
	cout << ntohs(addr.sin_port) << endl;
	close(fd);
	return 0;
}
