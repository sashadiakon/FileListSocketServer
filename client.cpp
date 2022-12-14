// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#define PORT 1031


char* get_time(){
	time_t now = time(0);
    // convert now to string form
  	char* date_time = strtok(ctime(&now), "\n");
	return date_time;
}


void make_log(std::ofstream& logfile, const char *msg, int type ){
	char* st;
	if (type){
		st = "->";
	}
	else{
		st = "<-";
	}
	time_t now = time(0);

    // convert now to string form
  	char* date_time = get_time();
    logfile << date_time << "| Client"<< st << "Server: '" << msg  << "'" << std::endl;
	
}

int main(int argc, char const* argv[])
{
    std::ofstream logfile;
	logfile.open("client_log.txt", std::ios_base::app);

	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[255] = { 0 };
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
    char inp[200];
	int is_work = 1;
	std::string a;
	logfile<<get_time()<<"| USER CONNECTED\n";
	int msg_size;
	std::string msg;
	printf("Example of input by path that shows everything inside home folder: /home/*\n");
	printf("another example that shows only cpp files in Desktop: /home/sady/Desktop/*.cpp\n\n");
    while(is_work){
		a = "";
		int sent = 0;
		printf("Input path and filter:  ");
		msg = "";
		std::cin >> inp;
		a+=inp;
        send(sock, inp, strlen(inp), 0);
		// write log to file
		make_log(logfile, inp, 1);
		valread = read(sock, buffer, 255);
		msg_size = atoi(buffer);
		while (sent<msg_size){
			valread = read(sock, buffer, 255);
			logfile<<buffer<<"\n\n";
			sent += sizeof(buffer);
			msg += buffer;
			for (int i = 0; i < 255; i++){
				buffer[i] = 0;
			}
		}
		msg += "\n";
		printf(msg.c_str());

		// write log to file
		make_log(logfile, msg.c_str(), 0);

		for (int i = 0; i < 255; i++){
			buffer[i] = 0;
		}
    }

    // closing the connected socket
    close(client_fd);
	return 0;
}
