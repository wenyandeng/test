#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define LOCAL_PORT 8890
#define LOCAL_IP   "127.0.0.1"
#define udp_printf printf
int process_msg(void)
{
	return 0;
}

void my_printf(const struct sockaddr* sock, const char* buf, unsigned int len)
{
	if (sock != NULL){
		udp_printf("sock :\n");
		struct sockaddr_in* sock_inet = (struct sockaddr_in *)sock;
		udp_printf("sock addr:[%s:%d]\n", (sock_inet->sin_addr).s_addr?inet_ntoa(sock_inet->sin_addr):"0.0.0.0", 
											ntohs(sock_inet->sin_port));
	}	

	unsigned int i = 0;
	udp_printf("========================================================\n");
	while (i <= len && (*buf != '\0')) {
		udp_printf("%02x", *(buf + i));
		i++;
		if (0 == i % 8) {
			// blank
			udp_printf("  ");
		}
		if (0 == i % 16) {
			//\r\n
			udp_printf("\r\n");
		}
	}
	udp_printf("========================================================\n");
	return;
}
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
	char recvbuff[1024] = {0};

	for(;;) {
		int	n = recvfrom(s_fd, recvbuff, sizeof(recvbuff),  0, (struct sockaddr *)&remote_sock, &sock_len);
		if (n > 0){
			write(1, recvbuff, n);
			write(s_fd, recvbuff, n);
			continue;
		}
		fprintf(stderr, "Nothing recived!!\n");
	}

	close(s_fd);
	return 0;
}
