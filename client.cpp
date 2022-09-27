// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#define PORT 8080

int main(int argc, char const* argv[])
{
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((client_fd
		= connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
    char inp[100];
	int is_work = 1;
    while(is_work){
		printf("Input filter:");
        std::cin >> inp;
        send(sock, inp, strlen(inp), 0);
        printf("Message sent\n");
        valread = read(sock, buffer, 1024);
        printf("%s\n", buffer);  
    }
    // closing the connected socket
    close(client_fd);
	return 0;
}
