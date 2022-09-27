// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <glob.h>
#define PORT 8080

#include <iostream>
using namespace std;
using std::string;
using std::cout;

string get_files(char patterns[100]){
    glob_t globbuf;
    string files = "Files and directories list: \n";
    glob(patterns, 0, NULL, &globbuf);
	if (globbuf.gl_pathc>0){
		for (int i = 0; i < globbuf.gl_pathc; i ++){
			files += globbuf.gl_pathv[i];
			files += "\n";
		}
	}
	else{
		files = "There are no files or directories that fit to your filter\n";
	}
    return files;
    
}


int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[255] = { 0 };
	char* hello = "Hello from server";

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}


	
	int is_work = 1;
	while (1){
		if ((new_socket
		= accept(server_fd, (struct sockaddr*)&address,
				(socklen_t*)&addrlen))
			< 0) 
		{
			perror("error in accept");
			exit(EXIT_FAILURE);
		}
		string files;
		while (is_work) {
			valread = read(new_socket, buffer, 255);
			if (valread == -1)
			{
				perror("Error in read");
				exit(EXIT_FAILURE);
			}
			if (valread == 0)
			{
				perror("user disconected");
				is_work = 0;
			}
			files = get_files(buffer);
			printf("Receaved msg: %s\n", buffer);
			send(new_socket, files.c_str(), files.size(), 0);
			printf(files.c_str()); 
    	}
		// close client socket
		close(new_socket);
		printf("Server closed connection with client");

	}
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
	return 0;
}