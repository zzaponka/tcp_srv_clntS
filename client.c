#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define DEBUG_LOG(fmt, ...) \
	do { \
		printf("%s@%d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__); \
	} while (0)

#define NUM_CLIENTS 3

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
	int sock;
	struct sockaddr_un server_addr;
	int err;
	time_t result;
	char buf[5];
	int n;

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		DEBUG_LOG("Cannot create a socket.");
        return;
	} else {
        DEBUG_LOG("Thread created, sock: %d.", sock);
    }

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, "./srvsock", strlen("./srvsock"));
	err = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err < 0) {
		perror("connect");
		DEBUG_LOG("Cannot connect to the socket.");
	}

	while (1) {
		result = time(NULL);
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, "%s", rand() % 10 ? "ABCD" : "PING");
		DEBUG_LOG("buf: |%s|.", buf);
		write(sock, buf, strlen(buf));
		sleep((rand() % 10) + 1);
	}

	return 0;
}
