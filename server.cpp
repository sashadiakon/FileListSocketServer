


#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <glob.h>
#include <ctime>
#include <iostream>


#define PORT 1031

using namespace std;
using std::string;
using std::cout;

string get_files(char patterns[100]){
    glob_t globbuf;
    string files = "Files and directories list: \n\t";
    glob(patterns, 0, NULL, &globbuf);
	if (globbuf.gl_pathc>0){
		for (int i = 0; i < globbuf.gl_pathc; i ++){
			files += globbuf.gl_pathv[i];
			files += "\n\t";
		}
	}
	else{
		files = "There are no files or directories that fit to your filter\n";
	}
    return files;
    
}



void make_log(std::ofstream& logfile, const char *msg, int s, int type ){
	char* st;
	if (type){
		st = "->";
	}
	else{
		st = "<-";
	}
	time_t now = time(0);

    // convert now to string form
  	char* date_time = strtok(ctime(&now), "\n");
    logfile << date_time << "| SERVER"<< st << "socket-" << s << ": " << msg  << std::endl;
	
}


int main(int argc, char const* argv[])
{
    std::ofstream logfile;
	logfile.open("server_log.txt", std::ios_base::app);

	int server_fd, valread;
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
	if (listen(server_fd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}


	string user_msg;
	int is_work = 1;
	while (1){
		
		int new_socket;
		if ((new_socket
		= accept(server_fd, (struct sockaddr*)&address,
				(socklen_t*)&addrlen))
			< 0) 
		{
			perror("error in accept");
			exit(EXIT_FAILURE);
		}
		int is_work = 1;
		string msg;
		printf("user connected");
		while (is_work) {
			valread = read(new_socket, buffer, 255);
			// write log to file
			make_log(logfile, buffer, new_socket, 0);
			
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
			user_msg = buffer;
			if (user_msg == "Who"){
				msg = "Author of the program: Oleksandr Diakon, variant number - 6, File list";

			}
			else {
				msg = get_files(buffer);
			}
			printf("Receaved msg: %s\n", buffer);
			cout<<msg.c_str()<<endl;
			send(new_socket, msg.c_str(), msg.size(), 0);
			// write log to file
			make_log(logfile, msg.c_str(), new_socket, 1);

			printf("Sent file list\n");

			// Clear buffer
			for (int i = 0; i < 255; i++){
				buffer[i] = 0;
			}
    	}
		// close client socket
		close(new_socket);
		printf("Server closed connection with client");

	}
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);

	logfile.close();

	return 0;
}
