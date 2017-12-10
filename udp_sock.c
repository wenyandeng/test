#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define LOCAL_PORT 8890
#define LOCAL_IP   "127.0.0.1"
#define udp_printf printf

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
unsigned short g_port = 80;
unsigned short g_mode = 0;
char g_addr[64] = {0x00};
#define LOCAL_ADDR "0.0.0.0"
void show_help()
{
	return;
}
int server_mode()
{
	int s_fd;
	struct sockaddr_in local_sock;
	struct sockaddr_in remote_sock;
	
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
		int	n = recvfrom(s_fd, recvbuff, sizeof(recvbuff) - 1,  0, (struct sockaddr *)&remote_sock, &sock_len);
		if (n > 0){
			if (3 == n && strncmp(recvbuff, "EOF", n)) {
				udp_printf("Stop server\n");
				break;
			}
			write(1, recvbuff, n);
			my_printf((struct sockaddr *)&remote_sock, recvbuff, n);
			int	send_len = sendto(s_fd, recvbuff, n,  0, (struct sockaddr *)&remote_sock, sock_len);
			memset(recvbuff, 0x00, sizeof(recvbuff));
			continue;
		}
		fprintf(stderr, "Nothing recived!!\n");
	}

	close(s_fd);
	return 0;
}
int client_mode()
{
	int s_fd;
	struct sockaddr_in remote_sock;
	
	s_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_fd < 0) {
		udp_printf("socket create failed!!\n");
		return -1;
	}

	memset(&remote_sock, 0, sizeof(remote_sock));
	remote_sock.sin_family = AF_INET;
	remote_sock.sin_port   = htons(g_port);
	remote_sock.sin_addr.s_addr = inet_addr(g_addr);

	unsigned int sock_len = sizeof(remote_sock);
	int len;
	char sendbuf[1024] = {0};

	for(;;) {
		memset(sendbuf, 0x00, sizeof(sendbuf));
		len = read(STDIN_FILENO, sendbuf, sizeof(sendbuf));
		if (len <= 0) {
			continue;
		}
		sendto(s_fd, sendbuf, len,  0, (struct sockaddr *)&remote_sock, sock_len);
		memset(sendbuf, 0x00, sizeof(sendbuf));
		int	n = recvfrom(s_fd, sendbuf, sizeof(sendbuf),  0, (struct sockaddr *)&remote_sock, &sock_len);
		my_printf(NULL, sendbuf, n);
	}

	close(s_fd);
	return 0;
}

int main(int argc, char *argv[])
{
	int opt;
	opterr = 0;
	while ((opt = getopt(argc, argv, "lc:s:p::")) != -1) {
		switch (opt) {
			case 'p':
				g_port = atoi(optarg);
				break;
			case 'c':
				g_mode = 1;
				memcpy(g_addr, optarg, (strlen(optarg) < sizeof(g_addr))?strlen(optarg):sizeof(g_addr));
				break;
			case 's':
				g_mode = 2;
				memcpy(g_addr, optarg, (strlen(optarg) < sizeof(g_addr))?strlen(optarg):sizeof(g_addr));
				break;
			case 'l':
				memcpy(g_addr, LOCAL_ADDR, strlen(LOCAL_ADDR)); 
				g_mode = 1;
				break;
			default:
				break;
		}
	}

	if (!g_mode) {
		udp_printf("Must select mode, -c is udp client, -s is udp server.\n");
		return 0;
	}

	if (1 == g_mode) {
		udp_printf("Start client mode. Connect: %s:%d.\n", g_addr, g_port);
		//client_mode();
	} else if (2 == g_mode) {
		udp_printf("Start server mode. Listen ip = %s,port = %d.\n", g_addr, g_port);
		//server_mode();
	}
	return 0;
}
