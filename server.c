#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#pragma comment(lib,"ws2_32.lib")

#define N 100
#define LEN 265

const char* parseCommand(const char* buff);
const char* parse_slash(const char* buff);

int main(void) {
	int client_count = 1;
	char dataResived[2048];
	WSADATA wsaData;
	int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in thisServerAddress;
	thisServerAddress.sin_family = AF_INET;
	thisServerAddress.sin_port = htons(8080);
	thisServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind(server, (struct sockaddr*)&thisServerAddress, sizeof(thisServerAddress));
	listen(server, 10);
	while (1) {//waiting for new connections 
		char history[N][LEN];
		int history_count = 0;
		int history_insex = 0;
		char welcomeNSG[256];
		char NickNAME[20]="";
		char errorMSG[] = "Invalid command\n";
		sprintf(welcomeNSG, "You are client number:%d)\n", client_count);
		SOCKET client = accept(server, NULL, NULL);
		printf("Client %d) conected\n", client_count);
		send(client, welcomeNSG, strlen(welcomeNSG), 0);
		client_count++;
		while (1) { //working with current client
			int bytes=recv(client, dataResived, sizeof(dataResived)-1, 0);
			if (bytes <= 0) {
				printf("Client %d disconnected\n",client_count-1);
				closesocket(client);
				break;
			}
			dataResived[bytes] = '\0';
			const char* responceCommand = parseCommand(dataResived);
			if (responceCommand == NULL) {
				send(client, errorMSG, strlen(errorMSG), 0);
				continue;
			}
			if (strncmp(responceCommand, "NICK", 4) == 0) {
				char* NickNmaeP = NickNAME;
				int e = 0;
				char* nick = parse_slash(dataResived);
				while (*nick != '\0') {//creating nickname
					while (isspace(*nick)) {
						nick++;
					}
					if (*nick == '\0') {
						send(client, errorMSG, strlen(errorMSG), 0);
						e = 1;
						break;
					}
					int bufferOverload = 0;
					while (isalpha(*nick) && bufferOverload < 20) {
						*NickNmaeP++ = *nick++;
						bufferOverload++;
					}
					*NickNmaeP = '\0';
					break;
				}
				if (e == 1) {
					continue;
				}
				char msgNick[120];
				sprintf(msgNick, "Your nick is now:%s\n", NickNAME);
				send(client, msgNick, strlen(msgNick), 0);
				continue;
			}
			if (strncmp(responceCommand, "MSG",3) == 0) {
				char history0[256] = "";
				char* curHistory = history0;
				char* temp = parse_slash(dataResived);
				if (history_count < 100) {
					history_count++;
				}
				while (*temp != '\0') {//reading all data 
					*curHistory++ = *temp++;
				}
				*curHistory = '\0';
				if (strlen(NickNAME) == 0) {
					char historyResp[30] = "";
					sprintf(history[history_insex], "UNKNOWN clinet:%s\n",history0);
					sprintf(historyResp, "Message saved total:%d\n", history_count);
					send(client, historyResp, strlen(historyResp), 0);
				}
				else {
					char historyResp0[30] = "";
					sprintf(history[history_insex], "%s:%s\n", NickNAME, history0);
					sprintf(historyResp0, "Message saved total:%d\n", history_count);
					send(client, historyResp0, strlen(historyResp0), 0);
				}
				history_insex++;
				if (history_insex == 100) {//refreshing buffer
					history_insex = 0;
				}
				continue;
			}
			if (strncmp(responceCommand, "HISTORY", 7) == 0) {
				if (history_count == 0) {
					char error03[] = "History is empty\n";
					send(client, error03, strlen(error03), 0);
				}
				else if (history_count<100) {// history not full
					char greating[4096] = "All history of yourself:\n";
					for (int i = 0; i < history_count; i++) {
						strcat(greating, history[i]);
					}
					send(client, greating, strlen(greating), 0);
				}
				else {//history is full
					char greating[4096] = "All history:\n";
					for (int i = history_insex; i < 100; i++) {
						strcat(greating, history[i]);
					}
					for (int i = 0; i < history_insex; i++) {
						strcat(greating, history[i]);
					}
					send(client, greating, strlen(greating), 0);
				}
				continue;
			}
			if (strncmp(responceCommand,"QUIT", 4) == 0) {
				char exitingMSG[] = "You closed the conection\n";
				send(client,exitingMSG, strlen(exitingMSG), 0);
				closesocket(client);
				break;
			}
			send(client, responceCommand, strlen(responceCommand), 0);
			
		}
	}
	WSACleanup();
	return 0;
}

const char* parseCommand(const char* buff) {
	if (strlen(buff) == 0) {
		return NULL;
	}
	char temp_word[50] = "";
	char* temp_wordP = temp_word;
	while (*buff != '\0') {
		while (isspace(*buff) && *buff != '/') {
			buff++;
		}
		if (*buff == '\0') {
			return NULL;
		}
		buff++;//skip '/'
		if (*buff == '\0') {
			return NULL;
		}
		while (isalpha(*buff)) {
			*temp_wordP++ = *buff++;
		}
		break;
	}
	if (strncmp(temp_word, "nick", 4) == 0) {
		return "NICK";
	}
	else if (strncmp(temp_word, "msg", 3) == 0) {
		return "MSG";
	}
	else if (strncmp(temp_word, "quit", 4) == 0) {
		return "QUIT";
	}
	else if (strncmp(temp_word, "history", 7) == 0) {
		return "HISTORY";
	}
	else {
		return "UNKNOWN";
	}
}

const char* parse_slash(const char* buff) {//deleting evrething before '\'
	while (*buff != '\0') {
		while (isspace(*buff) && *buff != '/') {
			buff++;
		}
		if (*buff == '\0') {
			return NULL;
		}
		buff++;
		if (*buff == '\0') {
			return buff;
		}
		while (isalpha(*buff)) {
			buff++;
		}
		return buff;
		break;
	}
}