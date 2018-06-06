#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define CLIENT_NUM 20

typedef struct server {
	int listen_fd;
} server_t;

int server_listen(server_t *server);
int server_accept(server_t *server);
void *srv_handler(void *data);

int main(int argc, char **argv)
{
	int err = 0;
	server_t server = { 0 };

	err = server_listen(&server);
	if (err) {
		printf("Failed to listen.\n");
		return err;
	}

	for (;;) {
		err = server_accept(&server);
		if (err) {
			printf("Failed to accept connection.\n");
			return err;
		}
	}
	return 0;
}

int server_listen(server_t *server)
{
	int err = 0;
	struct sockaddr_un server_addr = { 0 };

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, "./srvsock", strlen("./srvsock"));

	err = (server->listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0));
	if (err == -1) {
		perror("socket");
		printf("Failed to create socket endpoint.\n");
		return err;
	}

	err = bind(server->listen_fd, (struct sockaddr *)&server_addr,
			sizeof(struct sockaddr_un));
	if (err == -1) {
		printf("Failed to bind socket to address.\n");
		return err;
	}

	err = listen(server->listen_fd, CLIENT_NUM);
	if (err == -1) {
		perror("listen");
		printf("Failed to put socket in passive mode.\n");
		return err;
	}

	return 0;
}

int server_accept(server_t *server)
{
	int err = 0;
	int conn_fd, *new_fd;
	socklen_t client_len;
	struct sockaddr_un client_addr;
	pthread_t srv_thread;

	client_len = sizeof(client_addr);
	err = (conn_fd = accept(server->listen_fd,
			(struct sockaddr *)&client_addr, &client_len));
	if (err == -1) {
		perror("accept");
		printf("Failed accepting connections.\n");
		return err;
	}

	new_fd = malloc(1);
	*new_fd = conn_fd;
	err = pthread_create(&srv_thread, NULL, srv_handler, (void *)new_fd);
	if (err < 0) {
		perror("pthread_create");
		printf("Error while creating thread!\n");
		return err;
	}

	return 0;
}

void *srv_handler(void *data)
{
	int sk = *(int *)data;

	int i = 0;
	int n = 0;
	char buffer[256];

	while (1) {
		memset(buffer, '\0', sizeof(buffer));
		n = read(sk, buffer, 255);
		if (n < 0) {
			perror("read");
			continue;
		}
		printf("%s@%d: id #%d: after read(), n = %d.\n", __FUNCTION__, __LINE__, sk, n);
		printf("%s@%d: id #%d: buffer received: |%s|.\n", __FUNCTION__, __LINE__, sk, buffer);
	}
}
