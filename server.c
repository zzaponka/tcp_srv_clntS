#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define DEBUG_LOG(fmt, ...) \
	do { \
		printf("%s@%d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__); \
	} while (0)

#define CLIENT_NUM 5

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

    DEBUG_LOG("Creating server...");
	err = server_listen(&server);
	if (err) {
		DEBUG_LOG("Failed to listen.");
		return err;
	} else {
        DEBUG_LOG("Listening successfully, err = %d.", err);
    }

	for (;;) {
		err = server_accept(&server);
		if (err) {
			DEBUG_LOG("Failed to accept connection.");
			return err;
		} else {
			DEBUG_LOG("Accepted successfully, err = %d.", err);
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
		DEBUG_LOG("Failed to create socket endpoint.");
		return err;
	}

	err = bind(server->listen_fd, (struct sockaddr *)&server_addr,
			sizeof(struct sockaddr_un));
	if (err == -1) {
		perror("bind");
		DEBUG_LOG("Failed to bind socket to address.");
		return err;
	}

	err = listen(server->listen_fd, CLIENT_NUM);
	if (err == -1) {
		perror("listen");
		DEBUG_LOG("Failed to put socket in passive mode.");
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
		DEBUG_LOG("Failed accepting connections.");
		return err;
	}

	new_fd = malloc(sizeof(int));
	*new_fd = conn_fd;
	err = pthread_create(&srv_thread, NULL, srv_handler, (void *)new_fd);
	if (err < 0) {
		perror("pthread_create");
		DEBUG_LOG("Error while creating thread!");
		return err;
	} else {
        DEBUG_LOG("Created thread successfully.");
    }

	return 0;
}

void *srv_handler(void *data)
{
	int sk = *(int *)data;

	int i = 0;
	int n = 0;
	char buf[256];

    DEBUG_LOG("Received sk = %d.\n", sk);

	while (1) {
		memset(buf, '\0', sizeof(buf));
		n = read(sk, buf, sizeof(buf) - 1);
		if (n < 0) {
			perror("read");
			break;
		}
		DEBUG_LOG("id #%d: buffer received: |%s|, n = %d.", sk, buf, n);
        if (strstr(buf, "PING")) {
            DEBUG_LOG("Ping received!");
        } else {
            DEBUG_LOG("Skipping...");
        }
	}
}
