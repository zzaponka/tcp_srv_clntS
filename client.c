#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "debug.h"

#define TIMEOUT_CLNT 2500
#define TIMEOUT_SLEEP 90
#define NUM_CLIENTS 512

void *client_handler(void *data);

int main(int argc, char **argv)
{
	int sock;
	int conn;
	int i;
	pthread_t client_thread;

	srand(time(NULL));
	for (i = 0; i < NUM_CLIENTS; i++) {
		if (pthread_create(&client_thread, NULL, client_handler, NULL) < 0) {
			perror("pthread_create");
			DEBUG_LOG("Cannot create a thread.");
		} else {
			DEBUG_LOG("Client #%d has been created.", i);
		}
	}
	pthread_exit(NULL);

	return 0;
}

void *client_handler(void *data)
{
	int sk;
	struct sockaddr_un server_addr;
	int err;
	char buf[16];
	int n;

	struct pollfd ufds[1];
	int res;

	if ((sk = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		DEBUG_LOG_SK("Cannot create a socket.");
		return;
	}

	ufds[0].fd = sk;
	ufds[0].events = POLLIN;

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, "./srvsock", strlen("./srvsock"));
	err = connect(sk, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err < 0) {
		perror("connect");
		DEBUG_LOG_SK("Cannot connect to the socket.");
		return;
	}

	while (1) {
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, "%s", rand() % 10 ? "ABCD" : "PING");
		n = write(sk, buf, strlen(buf));
		res = poll(ufds, 1, TIMEOUT_CLNT);
		if (res == -1) {
			perror("poll");
			DEBUG_LOG_SK("error while polling.");
		} else if (res == 0) {
			DEBUG_LOG_SK("timeout while polling.");
		} else if (ufds[0].revents & POLLIN) {
			memset(buf, '\0', sizeof(buf));
			n = read(sk, buf, sizeof(buf) - 1);
			if (n < 0) {
				perror("read");
				DEBUG_LOG_SK("error while reading.");
				continue;
			}
			DEBUG_LOG_SK("id #%d: buffer received: |%s|, n = %d.", sk, buf, n);
			if (strstr(buf, "BYE")) {
				DEBUG_LOG_SK("Server says good bye!");
				return;
			}
		}
		sleep((rand() % TIMEOUT_SLEEP) + 1);
	}

	return 0;
}
