#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#pragma comment(lib,"ws2_32.lib")

int main(void) {
	char responce[2048];
	WSADATA wsaData;
	int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int res=connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (res == -1) {
		printf("Connection failed\n");
		return 1;
	}
	int resp = recv(client, responce, sizeof(responce)-1, 0);
	if (resp == -1) {
		printf("Error\n");
	}
	else if (resp > 1) {
		responce[resp] = '\0';
		printf("Server responce:\n");
		printf("%s\n", responce);
	}
	while (1) {
		char msg_client[2048];
		char responce0[2048];
		printf("Enter your request to the server\n");
		fgets(msg_client, sizeof(msg_client), stdin);
		msg_client[strcspn(msg_client, "\n")] = '\0';
		send(client, msg_client,strlen(msg_client), 0);
		int resp0= recv(client, responce0, sizeof(responce)-1, 0);
		if (resp0 <= 0) {
			printf("Error\n");
			break;
		}
		else if (resp0 > 1) {
			responce0[resp0] = '\0';
			if (strstr(responce0, "You closed") != NULL) {
				printf("You closed the connection\n");
				break;
			}
			else {
				printf("Server responce:\n");
				printf("%s\n", responce0);
			}
		}
		
	}
	closesocket(client);
	WSACleanup();
	return 0;
}