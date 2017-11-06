#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define LOCAL_PORT 8890
#define LOCAL_IP   "127.0.0.1"
#define udp_printf printf

int main(int argc, char *argv[])
{
	struct sockaddr_in local_sock;
	struct sockaddr_in remote_sock;
	
	int s_fd;
	
	s_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_fd < 0) {
		udp_printf("socket create failed!!\n");
		return -1;
	}

	memset(&local_sock, 0, sizeof(local_sock));
	local_sock.sin_family = AF_INET;
	local_sock.sin_port   = htons(LOCAL_PORT);
	local_sock.sin_addr.s_addr = inet_addr(LOCAL_IP);

	int err;

	err = bind(s_fd, (struct sockaddr *)&local_sock, sizeof(local_sock));
	if (err < 0) {
		udp_printf("bind sock err!!!\n");
		return -1;
	}

	unsigned int sock_len = sizeof(remote_sock);
	char recvbuff[1204] = {0};
	int n = 0;
	for(;;) {
		n = recvfrom(s_fd, recvbuff, sizeof(recvbuff),  0, (struct sockaddr *)&remote_sock, &sock_len);
		write(1, recvbuff, sizeof(recvbuff));
		sleep(10);
	}

	close(s_fd);

	return 0;
}
