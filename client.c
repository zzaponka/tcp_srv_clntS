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

#define NUM_CLIENTS 15

void *client_handler(void *data);

int main(int argc, char **argv)
{
	int sock;
	int conn;
	int i;
	pthread_t client_thread;

	srand(time(NULL));
	for (i = 0; i < NUM_CLIENTS; i++) {
		if (pthread_create(&client_thread, NULL, client_handler, (void *)i) < 0) {
			perror("pthread_create");
			DEBUG_LOG("Cannot create a thread.");
			return;
		} else {
			DEBUG_LOG("Client #%d has been created.", i);
		}
		sleep(5);
	}

	return 0;
}
void *client_handler(void *data)
{
	int client_id = (int )data;
	int sock;
	struct sockaddr_un server_addr;
	int err;
	time_t result;
	char datetime_str[256];
	int len;
	char buffer[256];
	int n;

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		DEBUG_LOG("Cannot create a socket.");
	}

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, "./srvsock", strlen("./srvsock"));
	err = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err < 0) {
		perror("connect");
		DEBUG_LOG("Cannot connect to the socket.");
		return;
	}

	while (1) {
		result = time(NULL);
		memset(datetime_str, '\0', sizeof(datetime_str));
		sprintf(datetime_str, "%s", ctime(&result));
		datetime_str[24] = '\0';
		DEBUG_LOG("datetime_str: |%s|.", datetime_str);
		len = strlen(datetime_str);
		DEBUG_LOG("after ctime() work, len = %d.", len);
		write(sock, datetime_str, strlen(datetime_str));
		sleep(2);
	}
	DEBUG_LOG("Return from handler!");
	return 0;
}
