#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include "Log.h"

#define PORT 5675
#define BYTES 1024

struct sockaddr_in webServ_addr;
int sockID;

void startServer(const unsigned int port);
void service_client(int id);

int main()
{
	startServer(PORT);
	// define socket
	while (true)
	{
		struct sockaddr_in client_addr;
		socklen_t clientS_len = sizeof(client_addr);
		int clientID = accept(sockID, (struct sockaddr*)&client_addr, &clientS_len);
		pid_t pid = fork();
		if (pid == 0)
		{
			service_client(clientID);
			exit(0);
		}
	}

}

void service_client(int id)
{
	const char* path_var = "/";
	char path[10000];
	char data_to_send[BYTES];
	char msg[10000];
	char *reqline[3];
	memset(msg, 0, (sizeof(msg) / sizeof(char)));
	int conn_state = recv(id, msg, (sizeof(msg) / sizeof(char)), 0);
	if (conn_state < 0)
	{
		printf("recv Error\n");
		return;
	}
	else if (conn_state == 0)
	{
		printf("client dissconnected\n");
		return;
	}
	printf("from client %s\n", msg);
	reqline[0] = strtok(msg, " \t\n");
	if (strncmp(reqline[0], "GET\0", 3) == 0)
	{
		reqline[1] = strtok(NULL, " \t");
		reqline[2] = strtok(NULL, " \t\n");
		printf("req0: %s\nreq1: %s\nreq2: %s\n", reqline[0], reqline[1], reqline[2]);
		if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0)
		{
			write(id, "HTTP/1.0 400 Bad Request\n", 25);	
		}
		else 
		{
			if (strncmp(reqline[1], "/\0", 2) == 0)
			{
				reqline[1] = "/index.html";
			}
			strcpy(path, path_var);
			strcpy(&path[strlen(path_var)], reqline[1]);
			
			int fd;
			int bytes_read;
			if ((fd = open(path, O_RDONLY)) != 0)
			{
				send(id, "HTTP/1.0 200 OK\n\n", 17, 0);
				while((bytes_read = read(fd, data_to_send, BYTES)) > 0)
				{
					write(id, data_to_send, bytes_read);
					memset(data_to_send, 0, (sizeof(data_to_send) / sizeof(char)));
				}
			}
			else
			{
				write(id, "HTTP/1.0 404 Not Found\n", 23);
			}
		}
	}
	shutdown(id, SHUT_RDWR);
	close(id);
}

void startServer(const unsigned int port)
{
	webServ_addr.sin_family = AF_INET;
	webServ_addr.sin_port = htons(port);
	webServ_addr.sin_addr.s_addr = INADDR_ANY;

	sockID = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// bind web server to port
	if (bind(sockID, (struct sockaddr*)&webServ_addr, sizeof(webServ_addr)) < 0)
	{
		printf("Web server failed to bind to port!!!\n");
		exit(-1);
	}
	
	listen(sockID, 1000);	
}
