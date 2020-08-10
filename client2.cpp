//Author: Sayali Borase
//CSCI 6300 - Project Cloud Prototype
//File: client2.cpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <unistd.h>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#pragma comment(lib, "ws2_32")

using namespace std;

// Client side - This is client 2

#define MAXLINE 4096 /* Max text line length*/

int main(int argc, char** argv)
{
	int sockfd, status, port;
	int bytesRead = 0, bytesWritten = 0;
	struct sockaddr_in servaddr;
	char buffer[MAXLINE];
	string clientMsg, str;

	// Initialize WinSock
	#ifdef _WIN32
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		exit(1);
	}
	#endif

	// We will have to provide ip address of the server to connect
	if (argc != 3)
	{
		cerr << "Usage: IP address port";
		exit(1);
	}

	// Grab the ip address
	char *serverIp = argv[1];
	struct hostent* host = gethostbyname(serverIp);

	// Grab the port
	port = atoi(argv[2]);

	// Create a socket for the client
	// If sockfd<0 there was an error in the creation of the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cerr << "Error in establishing the client 1 socket";
		exit(1);
	}

	// Creation of the socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	//servaddr.sin_addr.s_addr = inet_addr("161.45.162.69");
	servaddr.sin_port = htons(port); //Using the htons for big-endian

	// Connection of the client to the socket
	status = connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
	if (status < 0)
	{
		cerr << "Error in connecting to server socket";
		close(sockfd);
		exit(1);
	}
	cout << "Connected to the server" << endl;

	while(1)
	{
		string s1 = "logged in";
		string s2 = "disconnected";
		string s3 = "Incorrect login";
		string s4 = "username";
		string s5 = "password";
		string s6 = "key";
		string s7 = "file content";

		memset(&buffer, 0, sizeof(buffer)); //clear the buffer
	    bytesRead += recv(sockfd, (char*)&buffer, sizeof(buffer), 0);

		if( strstr(buffer, s1.c_str()) )
		{
		    cout << "Server message: " << buffer << endl;

		    /*bytesRead += recv(sockfd, (char*)&buffer, sizeof(buffer), 0);
			cout << "Server message: " << buffer << endl;*/

		    cout << "> ";
			getline(cin, clientMsg);
			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			strcpy(buffer, clientMsg.c_str());
			if(clientMsg == "exit")
			{
				send(sockfd, (char*)&buffer, strlen(buffer), 0);
				break;
			}
			bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);
		}

	    else if( strstr(buffer, s2.c_str()) )
	    {
	    	cout << "Server message: " << buffer << endl;
	    	break;
	    }

	   else if( strstr(buffer, s3.c_str()) )
	    {
	    	cout << "Server message: " << buffer;

			cout << "> ";
			getline(cin, clientMsg);
			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			strcpy(buffer, clientMsg.c_str());
			if(clientMsg == "exit")
			{
				send(sockfd, (char*)&buffer, strlen(buffer), 0);
				break;
			}
			bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);

			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
	    	bytesRead += recv(sockfd, (char*)&buffer, sizeof(buffer), 0);

			cout << "Server message: "<< buffer << endl;

			cout << "> ";
			getline(cin, clientMsg);
			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			strcpy(buffer, clientMsg.c_str());
			if(clientMsg == "exit")
			{
				send(sockfd, (char*)&buffer, strlen(buffer), 0);
				break;
			}
			bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);
	    }

	    else if ( strstr(buffer, s4.c_str()) )
	    {
		    // Send responce to server
		    cout << "Server message: " << buffer << endl;

			cout << "> ";
			getline(cin, clientMsg);
			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			strcpy(buffer, clientMsg.c_str());
			if(clientMsg == "exit")
			{
				send(sockfd, (char*)&buffer, strlen(buffer), 0);
				break;
			}
			bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);
		}

		else if ( strstr(buffer, s5.c_str()) )
	    {
		    // Send responce to server
		    cout << "Server message: " << buffer << endl;

			cout << "> ";
			getline(cin, clientMsg);
			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			strcpy(buffer, clientMsg.c_str());
			if(clientMsg == "exit")
			{
				send(sockfd, (char*)&buffer, strlen(buffer), 0);
				break;
			}
			bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);
		}

		else if ( strstr(buffer, s6.c_str()) )
	    {
		    // Send responce to server
		    cout << "Server message: " << buffer << endl;

			cout << "> ";
			getline(cin, clientMsg);
			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			strcpy(buffer, clientMsg.c_str());
			if(clientMsg == "exit")
			{
				send(sockfd, (char*)&buffer, strlen(buffer), 0);
				break;
			}
			bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);

			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
	    	bytesRead += recv(sockfd, (char*)&buffer, sizeof(buffer), 0);
	    	cout << "Server message: " << buffer << endl;

	    	if (strstr(buffer, s7.c_str()))
	    	{
	    		cout << "> ";
				getline(cin, clientMsg);
				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
				strcpy(buffer, clientMsg.c_str());
				if(clientMsg == "exit")
				{
					send(sockfd, (char*)&buffer, strlen(buffer), 0);
					break;
				}
				bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);

				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
	    		bytesRead += recv(sockfd, (char*)&buffer, sizeof(buffer), 0);
	    		cout << "Server message: " << buffer << endl;
	    	}

	    		cout << "> ";
				getline(cin, clientMsg);
				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
				strcpy(buffer, clientMsg.c_str());
				if(clientMsg == "exit")
				{
					send(sockfd, (char*)&buffer, strlen(buffer), 0);
					break;
				}
				bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);
		}

		else
		{
			cout << "Server message: " << buffer << endl;

			cout << "> ";
			getline(cin, clientMsg);
			memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			strcpy(buffer, clientMsg.c_str());
			if(clientMsg == "exit")
			{
				send(sockfd, (char*)&buffer, strlen(buffer), 0);
				break;
			}
			bytesWritten += send(sockfd, (char*)&buffer, strlen(buffer), 0);
		}
	}

	close(sockfd);

	#ifdef _WIN32
	WSACleanup();
	#endif

	cout << "******** Session details ********" << endl;
	cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;

	return 0;
}
