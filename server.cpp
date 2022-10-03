


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
#include <filesystem> 


#define PORT 1031

using namespace std;
using std::string;
namespace fs = std::filesystem;


string get_files(char patterns[100]){
    glob_t globbuf;
    string files = "Files and directories list: ";
    glob(patterns, 0, NULL, &globbuf);
	if (globbuf.gl_pathc>0){
		std::error_code ec;

		for (int i = 0; i < globbuf.gl_pathc; i ++){
			files += "\n\t";
			files += globbuf.gl_pathv[i];
			// Check is file directory
			const fs::path path(globbuf.gl_pathv[i]);
			if (fs::is_directory(path, ec)){
				files += "/";
			}
			
		}
	}
	else{
		files = "There are no files or directories that fit to your filter";
	}
    return files;
    
}


char* get_time(){
	time_t now = time(0);
    // convert now to string form
  	char* date_time = strtok(ctime(&now), "\n");
	return date_time;
}

void make_log(std::ofstream& logfile, const char *msg, int s, int type ){
	char* st;
	if (type){
		st = "->";
	}
	else{
		st = "<-";
	}
	char* date_time = get_time();
    logfile << date_time << "| SERVER"<< st << "socket-" << s << ": '" << msg  << "'" << std::endl;
	
}

void sendall(int client, char *buffer, const char *message, int messageLength, int bufferSize = 255)
{
	send(client, to_string(messageLength).c_str(), sizeof(messageLength), 0);
	int sendPosition = 0;
	while (messageLength) {
		int chunkSize = messageLength > bufferSize ? bufferSize : messageLength;
		memcpy(buffer, message + sendPosition, chunkSize);

		chunkSize = send(client, buffer, chunkSize, 0);
		messageLength -= chunkSize;
		sendPosition += chunkSize;
	}

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

	// Creating a socket 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// 
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
		// connect new socket(user)
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
		printf("user connected\n");
		logfile<<get_time()<<"| USER CONNECTED\n";
		while (is_work) {
			for (int i = 0; i < 255; i++){
				buffer[i] = 0;
			}
			// read client request
			valread = read(new_socket, buffer, 255);
			printf("Receaved msg: %s\n", buffer);
			// write log to file
			make_log(logfile, buffer, new_socket, 0);
			
			if (valread == -1)
			{
				perror("Error in read");
				exit(EXIT_FAILURE);
			}
			if (valread == 0)
			{
				perror("user disconnected");
				logfile<<get_time()<<"| USER DISCONNECTED\n";
				is_work = 0;
				continue;
			}
			user_msg = buffer;
			// Check different cases
			if (user_msg == "Who"){
				msg = "Author of the program: Oleksandr Diakon, variant number - 6, File list";
			}
			else {
				msg = get_files(buffer);
			}
			// send msg to the user
			sendall(new_socket, buffer, msg.c_str(), msg.size());
			// write log to file
			make_log(logfile, msg.c_str(), new_socket, 1);

			printf("Sent file list\n");

			// Clear buffer
			
    	}
		// close client socket
		close(new_socket);

	}
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);

	logfile.close();

	return 0;
}

