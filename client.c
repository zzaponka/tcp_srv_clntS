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
		} else {
            printf("%s@%d: client #%d has been created.\n", __FUNCTION__, __LINE__, i);
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
		printf("%s@%d: cannot create a socket!\n", __FUNCTION__, __LINE__);
	}

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, "./srvsock", strlen("./srvsock"));
	err = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err < 0) {
		perror("connect");
		printf("Error while creating socket.\n");
		return;
	}

	while (1) {
		result = time(NULL);
		memset(datetime_str, '\0', sizeof(datetime_str));
		sprintf(datetime_str, "%s", ctime(&result));
		datetime_str[24] = '\0';
		printf("%s@%d: datetime_str: |%s|.\n", __FUNCTION__, __LINE__, datetime_str);
		len = strlen(datetime_str);
		printf("%s@%d: after ctime() work, len = %d.\n", __FUNCTION__, __LINE__, len);
		write(sock, datetime_str, strlen(datetime_str));
		sleep(2);
	}

    printf("%s@%d: RETURN FROM HANDLER!\n", __FUNCTION__, __LINE__);
	return 0;
}
