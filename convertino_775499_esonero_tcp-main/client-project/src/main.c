/*
 * main.c
 *
 * TCP Client
 */

#if defined WIN32
#include <winsock.h>
#include <windows.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "protocol.h"

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement client logic

	#if defined WIN32
		SetConsoleOutputCP(65001);
	#endif

	weather_request_t request;
	weather_response_t res;

	char* richiesta = NULL;
	char* server_ip = "127.0.0.1";
	int server_port = SERVER_PORT;

	for(int i=1; i<argc; i++){
		if(strcmp(argv[i], "-r") == 0 && i+1 < argc){
			richiesta = argv[++i];
		}else if(strcmp(argv[i], "-s") == 0 && i+1 < argc){
			server_ip = argv[++i];
		}else if(strcmp(argv[i], "-p") == 0 && i+1 < argc){
			server_port = atoi(argv[++i]);
		}
	}

	if(richiesta == NULL){
		printf("Argomenti da riga di comando non validi\n");
		printf("Uso corretto: -r ""<type>"" ""<city>"" \n");
		return -1;
	}

	size_t max_lenght = (sizeof(request.city) - 1) + 2;

	if(strlen(richiesta) > max_lenght){
	    printf("Stringa troppo lunga\n");
	    return -1;
	}

	request.type = richiesta[0];
	strncpy(request.city, richiesta + 2, sizeof(request.city) - 1);
	request.city[63] = '\0';

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;

	// TODO: Create socket
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(my_socket < 0){
		printf("socket creation failed.\n");
		return -1;
	}

	// TODO: Configure server address
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

	// TODO: Connect to server
	if(connect(my_socket,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		printf( "failed to connect.\n" );
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	// TODO: Implement communication logic
	if(send(my_socket, (char*)&request, sizeof(weather_request_t), 0) < 0){
		printf("send() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	if(recv(my_socket, (char*)&res, sizeof(weather_response_t), 0) < 0){
		printf("recv() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	printf("Ricevuto risultato dal server ip %s. ", inet_ntoa(server_addr.sin_addr));

	formatta_citta(request.city);

	//controllo sullo status della risposta
	switch(res.status){
		case 0 :
			switch(res.type){
				case 't' :
					printf("%s: Temperatura = %.1f°C", request.city, res.value);
					break;
				case 'h' :
					printf("%s: Umidità = %.1f%%", request.city, res.value);
					break;
				case 'w' :
					printf("%s: Vento = %.1f km/h", request.city, res.value);
					break;
				case 'p' :
					printf("%s: Pressione = %.1f hPa", request.city, res.value);
					break;
				}
			break;

		case 1 :
			printf("Città non disponibile\n");
			break;
		case 2 :
			printf("Richiesta non valida\n");
			break;
	}

	// TODO: Close socket
	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end

void formatta_citta(char * str){

	//stringa in lowercase

	for(int i=0; str[i] != '\0'; i++){
		str[i] = tolower(str[i]);
	}

	//primo carattere in uppercase

	str[0] = toupper(str[0]);
}
