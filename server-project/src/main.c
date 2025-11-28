/*
 * main.c
 *
 * TCP Server
 */

#if defined WIN32
#include <winsock.h>
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
#include <time.h>
#include "protocol.h"

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement server logic

	srand(time(NULL));

	weather_request_t req;
	weather_response_t response;

	const char* citta_ammesse[NUM_CITTA] = {"bari", "roma", "milano", "napoli", "torino", "palermo", "genova", "bologna", "firenze", "venezia"};

	int port = SERVER_PORT;

	for(int i=1; i<argc; i++){
		if(strcmp(argv[i], "-p") == 0 && i+1 < argc){
			port = atoi(argv[++i]);
	    }
	}

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
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// TODO: Bind socket
	if(bind(my_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		printf("bind() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	// TODO: Set socket to listen

	if (listen (my_socket, QUEUE_SIZE) < 0){
		printf("listen() failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return-1;
	}

	// TODO: Implement connection acceptance loop
	struct sockaddr_in client_address;
	int client_socket;
	int client_lenght;

	client_lenght = sizeof(client_address);

	while (1){
		int flag = 0;

		client_socket = accept(my_socket, (struct sockaddr*) &client_address, &client_lenght);

		if(client_socket < 0){
			printf("accept() failed.\n");
			closesocket(my_socket);
			clearwinsock();
			return -1;
		}

		if(recv(client_socket, (char*)&req, sizeof(weather_request_t), 0) < 0){
			printf("recv() failed.\n");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		printf("Richiesta '%c %s' dal client ip %s\n", req.type, req.city, inet_ntoa(client_address.sin_addr));

		string_lower(req.city);

		for(int i=0; i<NUM_CITTA; i++){
			if(strcmp(req.city, citta_ammesse[i]) == 0){
				flag = 1;
			}
		}
		//se flag == 1 allora posso verificare il tipo di richiesta, altrimenti la città non è valida

		if(flag == 1){
			//funzione con switch per generare dati
			switch(req.type){
				case 't' :
					response.value = get_temperature();
					response.status = 0;
					response.type = 't';
					break;
				case 'h' :
					response.value = get_humidity();
					response.status = 0;
					response.type = 'h';
					break;
				case 'w' :
					response.value = get_wind();
					response.status = 0;
					response.type = 'w';
					break;
				case 'p' :
					response.value = get_pressure();
					response.status = 0;
					response.type = 'p';
					break;
				default :
					response.status = 2;
					response.type = '\0';
					response.value = 0.0;
			}
		}else{
			response.status = 1;
			response.type = '\0';
			response.value = 0.0;
		}

		if(send(client_socket, (char*)&response, sizeof(weather_response_t), 0) < 0){
			printf("send() failed.\n");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		closesocket(client_socket);
	}

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end


void string_lower(char* str){
	for(int i=0; str[i] != '\0'; i++){
		str[i] = tolower(str[i]);
	}
}

float get_temperature(){
	return ((float)rand() / (float)RAND_MAX) * 50.0f - 10.0f;
}

float get_humidity(){
	return 20.0f + ((float)rand() / (float)RAND_MAX) * 80.0f;
}

float get_wind(){
	return ((float)rand() / (float)RAND_MAX) * 100.0f;
}

float get_pressure(){
	return 950.0f +((float)rand() / (float)RAND_MAX) * 100.0f;
}
