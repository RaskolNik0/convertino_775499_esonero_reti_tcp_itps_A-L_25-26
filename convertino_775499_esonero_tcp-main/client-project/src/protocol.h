/*
 * protocol.h
 *
 * Header file
 * Definitions, constants and function prototypes for the server
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// Shared application parameters
#define SERVER_PORT 27015  // Server port (change if needed)
#define BUFFER_SIZE 512    // Buffer size for messages
#define QUEUE_SIZE 5   // Size of pending connections queue
#define NUM_CITTA 10

typedef struct {
	char type; 			// Weather data type: 't', 'h', 'w', 'p'
	char city[64];		// City name (null-terminated string)
}weather_request_t;

typedef struct {
	unsigned int status;	// Response status code
	char type;				// Echo of request type
	float value;			// Weather data type
}weather_response_t;

// Function prototypes
// Add here the signatures of the functions implemented by students

float get_temperature(void);
float get_humidity(void);
float get_wind(void);
float get_pressure(void);
void string_lower(char* str);
void formatta_citta(char *);

#endif /* PROTOCOL_H_ */
