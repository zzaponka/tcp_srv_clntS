#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

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
			printf("%s@%d: cannot create a thread.\n", __FUNCTION__, __LINE__);

			return;
		}
		sleep(5);

	}
	pthread_exit(NULL);

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
	printf("%s@%d: called, client_id = %d.\n", __FUNCTION__, __LINE__, client_id);
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		printf("%s@%d: cannot create a socket!\n", __FUNCTION__, __LINE__);
	}
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, "./srvsock", strlen("./srvsock"));
	printf("%s@%d: after creating socket.\n", __FUNCTION__, __LINE__);
	err = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	printf("%s@%d: after connect() call, err = %d.\n", __FUNCTION__, __LINE__, err);

	if (err < 0) {
		perror("connect");
		printf("%s@%d: after creating socket.\n", __FUNCTION__, __LINE__);

		return;
	}
	printf("%s@%d: after calling connect()...\n", __FUNCTION__, __LINE__);
	while (1) {
		result = time(NULL);
		printf("%s@%d: after getting time, it looks like: |%s|.\n", __FUNCTION__, __LINE__, ctime(&result));
		memset(datetime_str, '\0', sizeof(datetime_str));
		sprintf(datetime_str, "%s", ctime(&result));
		datetime_str[24] = '\0';
		printf("%s@%d: datetime_str: |%s|.\n", __FUNCTION__, __LINE__, datetime_str);
		len = strlen(datetime_str);
		printf("%s@%d: after ctime() work, len = %d.\n", __FUNCTION__, __LINE__, len);
		write(sock, datetime_str, strlen(datetime_str));
		sleep(2);
	}

	printf("%s@%d: after sending date-time...\n", __FUNCTION__, __LINE__);

	memset(buffer, '\0', sizeof(buffer));
	n = 0;
	n = read(sock, buffer, 255);
	printf("%s@%d: buffer received: |%s|.\n", __FUNCTION__, __LINE__, buffer);

	printf("%s@%d: returning...\n", __FUNCTION__, __LINE__);

	return 0;
}
