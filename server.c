#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define PORT 7777
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

	printf("%s@%d: called.\n", __FUNCTION__, __LINE__);
	err = server_listen(&server);
	if (err) {
		printf("Failed to listen on address 127.0.0.0:%d.\n", PORT);

		return err;
	}

	for (;;) {
		err = server_accept(&server);
		printf("%s@%d: after server_accept()...\n", __FUNCTION__, __LINE__);
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

	printf("%s@%d: called.\n", __FUNCTION__, __LINE__);

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, "./srvsock", strlen("./srvsock"));

	printf("%s@%d: calling socket()...\n", __FUNCTION__, __LINE__);
	err = (server->listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0));
	if (err == -1) {
		perror("socket");
		printf("Failed to create socket endpoint.\n");

		return err;
	} else {
		printf("%s@%d: socket obtained: %d.\n", __FUNCTION__, __LINE__,
				server->listen_fd);
	}

	err = bind(server->listen_fd, (struct sockaddr *)&server_addr,
			sizeof(struct sockaddr_un));
	if (err == -1) {
		printf("Failed to bind socket to address.\n");

		return err;
	} else {
		printf("%s@%d: after binding to the socket, err = %d.\n", __FUNCTION__, __LINE__, err);
	}

	printf("%s@%d: calling listen()...\n", __FUNCTION__, __LINE__);
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
	char buf[1024];

	printf("%s@%d: called.\n", __FUNCTION__, __LINE__);
	client_len = sizeof(client_addr);
	printf("%s@%d: before accept() call...\n", __FUNCTION__, __LINE__);
	err = (conn_fd = accept(server->listen_fd,
			(struct sockaddr *)&client_addr, &client_len));
	printf("%s@%d: after accept() call, err= %d.\n", __FUNCTION__, __LINE__, err);
	if (err == -1) {
		perror("accept");
		printf("Failed accepting connections.\n");

		return err;
	}
	printf("%s@%d: client connected! After accept() call, err = %d and conn_fd = %d.\n", __FUNCTION__, __LINE__, err, conn_fd);
	new_fd = malloc(1);
	*new_fd = conn_fd;
	printf("%s@%d: after descriptor allocation, conn_fd = %d, *new_fd = %d.\n", __FUNCTION__, __LINE__, conn_fd, *new_fd);
	printf("%s@%d: calling pthread_create()...\n", __FUNCTION__, __LINE__);
	err = pthread_create(&srv_thread, NULL, srv_handler, (void *)new_fd);
	printf("%s@%d: after pthread_create() call err = %d.\n", __FUNCTION__, __LINE__, err);
	if (err < 0) {
		perror("pthread_create");
		printf("%s@%d: error while creating thread!\n", __FUNCTION__, __LINE__);

		return err;
	} else {
		printf("%s@%d: after pthread_create(), err = %d.\n", __FUNCTION__, __LINE__, err);
	}
#if 0
	memset(buf, 0, sizeof(buf));
	read(conn_fd, buf, sizeof(buf));
	printf("%s@%d: after read() call, buf is |%s|.\n", __FUNCTION__, __LINE__, buf);
	err = close(conn_fd);
	if (err == -1) {
		perror("close");
		printf("Failed to close connection.\n");

		return err;
	} else {
		printf("%s@%d: after close() call, err = %d.\n", __FUNCTION__, __LINE__, err);
	}
#endif

	return 0;
}

void *srv_handler(void *data)
{
	int sk = *(int *)data;

	int i = 0;
	int n = 0;
	char buffer[256];

	// printf("%s@%d: called.\n", __FUNCTION__, __LINE__);
	printf("%s@%d: incoming *(int *)data = %d, sk = %d.\n", __FUNCTION__, __LINE__, *(int *)data, sk);
	while (1) {
		memset(buffer, '\0', sizeof(buffer));
		n = read(sk, buffer, 255);
		if (n < 0) {
			perror("read");
			break;
		}
		// printf("%s@%d: after checking n for negative value after read...\n", __FUNCTION__, __LINE__);
		printf("%s@%d: id #%d: after read(), n = %d.\n", __FUNCTION__, __LINE__, sk, n);
		printf("%s@%d: id #%d: buffer received: |%s|.\n", __FUNCTION__, __LINE__, sk, buffer);
		n = write(sk, "THNX", strlen("THNX"));
		// printf("%s@%d: after write() call...\n", __FUNCTION__, __LINE__);
		if (n < 0) {
			perror("write");
		}
		// printf("%s@%d: after checking n for negative value after write...\n", __FUNCTION__, __LINE__);
	}

	// close(sk);
	// printf("%s@%d: returning...\n", __FUNCTION__, __LINE__);
}
