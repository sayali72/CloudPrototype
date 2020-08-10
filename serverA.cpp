//Author: Sayali Borase
//Project Cloud Prototype
//File: serverA.cpp
//serverA listen at port 4000
//Port 4005 is dedicated for synchronization

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

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

// Server side - This is serverA
#define MAXLINE 4096 /* Max text line length*/
#define SERV_PORT 4000 /* Port*/
#define SERV_PORT_SYNC 4005 /* Port used for synchronization  */
#define LISTENQ 2 /* Maximum number of client connections*/
#define MAX_LIMIT 100 /* To generate unique key */
#define MIN_LIMIT 900 /* To generate unique key */
#define MUTEX "/mutex_lock"


int checkUserPresent(string& filename, string& username);
int synchronizeUserInfo();

// Function to check if user already present in userInfo.txt
int checkUserPresent(string& filename, string& username)
{
	ifstream inFile;
	char fileWord[50];
	int cnt = 0;

    inFile.open(filename, ios::in);

	while(inFile.good())
	{
		inFile >> fileWord;
		if(inFile.good() && (fileWord == username))
			cnt++;
	}
	inFile.close();
	return cnt;
}

// Function to synchronize user login details after user "exits"
int synchronizationSetup()
{
	// Open a new port allowing serverB to connect
	// Port remains open for synchronization
	cout << "Settin up socket for synchronization at serverA --port 4005" << endl;

	int listenfdSync, connfdSync, bindStatusSync;
  	socklen_t clilenSync;
  	struct sockaddr_in cliaddrSync, servaddrSync;

	listenfdSync = socket (AF_INET, SOCK_STREAM, 0);
  	if (listenfdSync < 0)
  	{
    	cerr << "Error in establishing the server socket";
    	exit(1);
  	}

	// Preparation of the socket address
	servaddrSync.sin_family = AF_INET;
	servaddrSync.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddrSync.sin_port = htons(SERV_PORT_SYNC);

  	// Bind the socket
  	bindStatusSync = bind (listenfdSync, (struct sockaddr *) &servaddrSync, sizeof(servaddrSync));
  	if(bindStatusSync < 0)
  	{
    	cerr << "Error in binding the server socket";
      	exit(1);
  	}
  	listen (listenfdSync, 1);
    clilenSync = sizeof(cliaddrSync);

    // Accept a connection
    connfdSync = accept (listenfdSync, (struct sockaddr *) &cliaddrSync, &clilenSync);
    if(connfdSync < 0)
    {
		cerr << "Error in accepting connection";
      	exit(1);
    }
    return connfdSync;
}

// Function to avoid zombie processes
// Parent process installs a signal handler for the SIGCHLD signal
// On receipt of SIGCHLD, child entry is cleared
void waitParent(int signum)
{
    wait(NULL);
}

int main (int argc, char **argv)
{
	int connfdSync = synchronizationSetup();

    pthread_mutex_t* mutex;
    int des_mutex;
    time_t t;

	int listenfd, connfd, bindStatus;
  	int bytesRead = 0, bytesWritten = 0;
	int key, attempts = 3, userPresent = 0;
	bool loginFailed = true, readAllowed = true, writeAllowed = true;

	string user, pass, writeDeleteFlag;
	string userText, passText, writeDeleteFlagText, userFileText;
	string serverMsg;
	string loginFile = "userInfoA.txt";

  	pid_t childpid;
  	socklen_t clilen;
  	char buffer[MAXLINE];
  	struct sockaddr_in cliaddr, servaddr;

    des_mutex = shm_open(MUTEX, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG); // Allocate shared memory

    if (des_mutex < 0)
    {
        cerr << "Failure on shm_open on des_mutex";
        exit(1);
    }

    if (ftruncate(des_mutex, sizeof(pthread_mutex_t)) == -1)
    {
        cerr << "Error on ftruncate to sizeof pthread_attr";
        exit(-1);
    }

    mutex = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, des_mutex, 0);

    // Set mutex shared between processes
    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex, &mutexAttr);

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

  	// Create a socket for the soclet
  	// If listenfd < 0 there was an error in the creation of the socket
	listenfd = socket (AF_INET, SOCK_STREAM, 0);
  	if (listenfd < 0)
  	{
    	cerr << "Error in establishing the server socket";
    	exit(1);
  	}

	// Preparation of the socket address
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

  	// Bind the socket
  	bindStatus = bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  	if(bindStatus < 0)
  	{
    	cerr << "Error in binding the server socket";
      	exit(1);
  	}
  	cout << "Bind to port: " << SERV_PORT << endl;

  	// Listen to the socket by creating a connection queue, then wait for clients
  	listen (listenfd, LISTENQ);

  	cout << "Server running...waiting for connections" << endl;

  	for ( ; ; )
  	{
    	clilen = sizeof(cliaddr);

    	// Accept a connection
    	connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
    	if(connfd < 0)
    	{
      		exit(1);
    	}
    	cout << "Connection accepted from " << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << endl;

    	// Support multiple clients
    	if ( (childpid = fork ()) == 0 )
    	{
    		// If it’s 0, it’s child process
      		cout << "Child process created for dealing with client requests" << endl;

            // Avoid creating zombie processes
            if(childpid != 0)
            {
                signal(SIGCHLD, waitParent);
                while(1);
            }

      		// Close listening socket
      		close (listenfd); // Child closes listening socket

      		// Ask username and password, disconnect when login attempts exceed 3
      		do
      		{
	      		serverMsg = "Please enter username";
	      		memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
	      		strcpy(buffer, serverMsg.c_str());
	      		bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
	      		cout << "> Awaiting client response for username..." << endl;

				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
				bytesRead += recv(connfd, (char*)&buffer, sizeof(buffer), 0);
				if(!strcmp(buffer, "exit"))
	        	{
	          		cout << "Client has quit the session" << endl;
	          		break;
	        	}
				cout << "Client message: " << buffer << endl;
				user = buffer; // Store client username in a variable

				serverMsg = "Please enter password";
	      		memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
	      		strcpy(buffer, serverMsg.c_str());
	      		bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
	      		cout << "> Awaiting client response for password..." << endl;

				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
				bytesRead += recv(connfd, (char*)&buffer, sizeof(buffer), 0);
				if(!strcmp(buffer, "exit"))
	        	{
	          		cout << "Client has quit the session" << endl;
	          		break;
	        	}
				cout << "Client message: " << buffer << endl;
				pass = buffer; // Store client password in a variable

				// Check if the user details already present in file. If so, verify the login credentials.
				// If not (new user) add/append user details into file
				if(attempts == 3)
					userPresent = checkUserPresent(loginFile, user);

				if(userPresent == 0)
				{
					// User is not present in a file (New user)
					// Add user details to file
					// Generate a unique key
                    srand((int)time(&t) % getpid()); // seed based on current process ID
					key = rand() % (MAX_LIMIT - MIN_LIMIT) + MIN_LIMIT;

					// Generate a new file
					ofstream outUserFile;
					string userFileName = user + "fileA.txt";
					outUserFile.open(userFileName, ios::out);
					if(outUserFile.is_open())
					{
						outUserFile << "file is empty";
					}
					outUserFile.close();

					// Initialize the writeDeleteFlag to OFF
					writeDeleteFlag = "WRITE_FLAG_OFF";

					ofstream outFile;
					outFile.open(loginFile, std::ios_base::app | std::ios_base::out);
					if(outFile.is_open())
					{
						outFile << user << " " << pass << " " << key << " " << userFileName << " " << writeDeleteFlag << endl;
					}
					outFile.close();

					cout << ">";
					cout << "Client " << user << " is logged in" << endl;

					// Ssend key to the client - New user
					serverMsg = "Welcome " + user + ",You are now logged in! A file is created for you and this is its unique key: " + to_string(key);
					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
	      			strcpy(buffer, serverMsg.c_str());
	      			bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);

	      			loginFailed = false; // Successful login
	      			// IMP - New user must "exit", so server can synchronize with remote server
	      			// Call synchronize()
				}
				else
				{
					// User is already present in a file (returning user)
					// Verify login details
					ifstream inFile;
	    			inFile.open(loginFile, ios::in);
	    			while(inFile >> userText >> passText >> key >> userFileText >> writeDeleteFlag)
	    			{
	    				if((user == userText) && (pass == passText))
	    				{
	    					loginFailed = false; // Successful login

	    					cout << "> ";
							cout << "Client " << user << " is logged in" << endl;

	    					serverMsg = "Welcome " + user + " ,You are now logged in!";
							memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
	      					strcpy(buffer, serverMsg.c_str());
	      					bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
	    				}
	    			}
	    			inFile.close();

	    			if(loginFailed)
	    			{
		    			// Incorrect username or password, set loginFailed = true
		    			attempts--;
		    			cout << "> ";
						cout << "Incorrect login details. Remaining login ,attempts: " << attempts << endl;

		    			serverMsg = "Incorrect login details. Remaining login attempts: " + to_string(attempts) + "\n";
						memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
		      			strcpy(buffer, serverMsg.c_str());
		      			bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
		    		}
	    		}

	    	}while(attempts > 0 && loginFailed == true);

	    	userPresent = 0;

	    	// If remaining attempt = 0, the disconnect
	    	if(attempts == 0)
	    	{
	    	 	cout << "> Client is disconnected!" << endl;
	    	 	serverMsg = "You have been disconnected";
				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
		      	strcpy(buffer, serverMsg.c_str());
		      	bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
		      	//break;
	    	}
		}

		if(!loginFailed)
		{
			// Ask user to select one of the operation
			while(1)
			{
				FILE *fp1, *fp2;
				serverMsg = "\nEnter name of operation followed by filename: 1) read 2) write 3) delete 4) exit";
				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
			    strcpy(buffer, serverMsg.c_str());
			    bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
				cout << "> Awaiting client " << user << " response... for choices" << endl;

				memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
				bytesRead += recv(connfd, (char*)&buffer, sizeof(buffer), 0);

				if(!strcmp(buffer, "exit"))
		        {
		        	cout << "Client " << user  << " has quit the session" << endl;

		        	// Check if new user - synchronize user login details with serverB
		        	// Check if user made changes to the file - synchronize file content with serverB
		        	cout << "Please wait! Server is synchronising" << endl;

		        	//pthread_mutex_lock(mutex);

		        	memset(&buffer, 0, sizeof(buffer)); // Clear the buffer

		        	fp1 = fopen(loginFile.c_str(), "r");
		        	while(!feof(fp1))
						fread(buffer, sizeof(buffer), 1, fp1);

					// Send login file to serverB
					send(connfdSync, (char*)&buffer, strlen(buffer), 0);

					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
		        	fp2 = fopen("alicefileA.txt", "rb");
		        	while(!feof(fp2))
						fread(buffer, sizeof(buffer), 1, fp2);

					// Send user file to serverB
					send(connfdSync, (char*)&buffer, strlen(buffer), 0);

					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
					recv(connfdSync, (char*)&buffer, sizeof(buffer), 0);
					if(!strcmp(buffer, "synchronization complete"))
	        			cout << "Synchronization complete" << endl;

	        		//pthread_mutex_unlock(mutex);

	        		cout << ">";
		        	break;
		        }

		        cout << "Client " << user << " message: " << buffer << endl;

		        string temp, strError;
		       	string arr[2];
		       	int i = 0;
		       	ifstream inFile;
		       	ofstream outFile;
		       	FILE *fp;

				temp = buffer;
				stringstream ss(temp);
				while(ss.good() && i < 2)
				{
					ss >> arr[i];
					++i;
				}

				// Read operation
				if (arr[0] == "read")
				{
					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer

					// Check writeDeleteFlag
					// Critical section check - if client 1 has writeDeleteFlag ON on his file, then client 2 cannot read client 1 file.
					// Critical section check - client 1 and client 2 can read each others file at the same time, no need to check here

					//pthread_mutex_unlock(mutex);
					inFile.open(loginFile, ios::in);
					if(inFile.is_open())
					{
						while (inFile >> userText >> passText >> key >> userFileText >> writeDeleteFlag)
						{
							if(userFileText == arr[1] && writeDeleteFlag == "WRITE_FLAG_ON")
							{
								strError = "Sorry, you cannot access the file at the moment."; // Permission denied
								readAllowed = false;
							}
						}
					}
					inFile.close();

					// Critical section - clear
					//pthread_mutex_unlock(mutex);

					if(readAllowed)
					{
						fp = fopen(arr[1].c_str(), "rb");
						while(!feof(fp))
							fread(buffer, sizeof(buffer), 1, fp);

						// Send file contents to the user
						bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
					}
					else
					{
						// Send error message
						strcpy(buffer, strError.c_str());
			    		bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
					}
				}

				// Write opration
				if(arr[0] == "write")
				{
					int keyVal;

					// Ask user for a key to check if user have permission to access file
					serverMsg = "Please enter a key";
					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
					strcpy(buffer, serverMsg.c_str());
			    	bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
					cout << "> Awaiting client " << user << " response... for key" << endl;

					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
					bytesRead += recv(connfd, (char*)&buffer, sizeof(buffer), 0);
					cout << "Client " << user << " message: " << buffer << endl;
					keyVal = atoi(buffer);

					inFile.open(loginFile, ios::in);
					if(inFile.is_open())
					{
						while (inFile >> userText >> passText >> key >> userFileText >> writeDeleteFlag)
						{
							if(userFileText == arr[1] && key != keyVal)
							{
								strError = "You have no permission to write to this file!";
								writeAllowed = false;
							}
						}
					}
					inFile.close();


					if(writeAllowed)
					{
						// Update the value of writeDeleteFlag = WRITE_FLAG_ON
						// Send this file to other server for Synchronization (No read should be permitted to other users)
						fstream fFile, temp;
						string line;
						writeDeleteFlag = "WRITE_FLAG_ON";

						fFile.open("userInfoA.txt", ios::in | ios::app | ios::out);
						temp.open("temp.txt", ios::in | ios::app | ios::out);

						while(getline(fFile, line))
						{
							size_t found = line.find("alice");
							if(found == string::npos)
							{
								temp << line << endl;
							}
						}
						temp << user << " " << pass << " " << keyVal << " " << arr[1] << " " << writeDeleteFlag << endl;
						fFile.close();
						temp.close();
						remove("userInfoA.txt");
						rename("temp.txt", "userInfoA.txt");

						// Ask user to enter file content
						serverMsg = "Enter a file content";
						memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
						strcpy(buffer, serverMsg.c_str());
			    		bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
			    		cout << "Awaiting client " << user << " response... for file content" << endl;

			    		// Receive the file content and write to the file
			    		memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
						bytesRead += recv(connfd, (char*)&buffer, sizeof(buffer), 0);

						//fp = fopen(arr[1].c_str(), "w+b");
						//fwrite(buffer, sizeof(buffer), 1, fp);

						outFile.open(arr[1].c_str(), ios::out);
						if(outFile.is_open())
						{
							outFile << buffer;
						}
						outFile.close();

						// Acknowldge the write operation
						memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
						serverMsg = "Write operation successful";
						strcpy(buffer, serverMsg.c_str());
						bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);

						// Write complete
						// Update the value of writeDeleteFlag = WRITE_FLAG_OFF
						// Send this file to other server for Synchronization (Read operation can be permitted to other users)
						writeDeleteFlag = "WRITE_FLAG_OFF";

						fFile.open("userInfoA.txt", ios::in | ios::app | ios::out);
						temp.open("temp.txt", ios::in | ios::app | ios::out);

						while(getline(fFile, line))
						{
							size_t found = line.find("alice");
							if(found == string::npos)
							{
								temp << line << endl;
							}
						}
						temp << user << " " << pass << " " << keyVal << " " << arr[1] << " " << writeDeleteFlag << endl;
						fFile.close();
						temp.close();
						remove("userInfoA.txt");
						rename("temp.txt", "userInfoA.txt");
					}
					else
					{
						// Invalid key
						memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
						strcpy(buffer, strError.c_str());
			    		bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
					}
				}

				// Delete operatio
				if(arr[0] == "delete")
				{
					int keyVal;

					// Critical section check - each client can modify (update/delete) contents of its own file
					// Ask user for a key to check if user have permission to access file
					serverMsg = "Please enter a key";
					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
					strcpy(buffer, serverMsg.c_str());
			    	bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
					cout << "> Awaiting client " << user << " response... for key" << endl;

					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
					bytesRead += recv(connfd, (char*)&buffer, sizeof(buffer), 0);
					cout << "Client " << user << " message: " << buffer << endl;
					keyVal = atoi(buffer);

					inFile.open(loginFile, ios::in);
					if(inFile.is_open())
					{
						while (inFile >> userText >> passText >> key >> userFileText >> writeDeleteFlag)
						{
							if(userFileText == arr[1] && key != keyVal)
							{
								strError = "You have no permission to update this file!";
								writeAllowed = false;
							}
						}
					}
					inFile.close();

					// Critical section - clear
					if(writeAllowed)
					{
						// Update the value of writeDeleteFlag = WRITE_FLAG_ON
						// Send this file to other server for Synchronization (No read should be permitted to other users)
						fstream fFile, temp;
						string line;
						writeDeleteFlag = "WRITE_FLAG_ON";

						fFile.open("userInfoA.txt", ios::in | ios::app | ios::out);
						temp.open("temp.txt", ios::in | ios::app | ios::out);

						while(getline(fFile, line))
						{
							size_t found = line.find("alice");
							if(found == string::npos)
							{
								temp << line << endl;
							}
						}
						temp << user << " " << pass << " " << keyVal << " " << arr[1] << " " << writeDeleteFlag << endl;
						fFile.close();
						temp.close();
						remove("userInfoA.txt");
						rename("temp.txt", "userInfoA.txt");


						memset(&buffer, 0, sizeof(buffer)); // Clear the buffer

						// Display content of file to user
						fp = fopen(arr[1].c_str(), "r");
						while(!feof(fp))
							fread(buffer, sizeof(buffer), 1, fp);

						// Send file content to user
						bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
			    		cout << "Awaiting client " << user << " response... for file content" << endl;
			    		fclose(fp);

						// Receive the file content and write back to the file
			    		memset(&buffer, 0, sizeof(buffer)); //clear the buffer
						bytesRead += recv(connfd, (char*)&buffer, sizeof(buffer), 0);

						//fp = fopen(arr[1].c_str(), "w+b");
						//fwrite(buffer, sizeof(buffer), 1, fp);
						//fclose(fp);
						outFile.open(arr[1].c_str(), ios::out);
						if(outFile.is_open())
						{
							outFile << buffer;
						}
						outFile.close();

						// Acknowldge the update/delete operation
						memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
						serverMsg = "Update/delete operation successful";
						strcpy(buffer, serverMsg.c_str());
						bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);

						// Delete complete
						// Update the value of writeDeleteFlag = WRITE_FLAG_OFF
						// Send this file to other server for Synchronization (Read operation can be permitted to other users)
						writeDeleteFlag = "WRITE_FLAG_OFF";

						fFile.open("userInfoA.txt", ios::in | ios::app | ios::out);
						temp.open("temp.txt", ios::in | ios::app | ios::out);

						while(getline(fFile, line))
						{
							size_t found = line.find("alice");
							if(found == string::npos)
							{
								temp << line << endl;
							}
						}
						temp << user << " " << pass << " " << keyVal << " " << arr[1] << " " << writeDeleteFlag << endl;
						fFile.close();
						temp.close();
						remove("userInfoA.txt");
						rename("temp.txt", "userInfoA.txt");
					}

					// Invalid key
					else
					{
					memset(&buffer, 0, sizeof(buffer)); // Clear the buffer
					strcpy(buffer, strError.c_str());
			    	bytesWritten += send(connfd, (char*)&buffer, strlen(buffer), 0);
			    	}
				}
			}
    	}
   	}

	// Close socket of the server
   	// Parent closes connected server
   	close(connfdSync);
   	close(connfd);

	#ifdef _WIN32
	WSACleanup();
	#endif

    // Destroy mutex and cleanup
    pthread_mutexattr_destroy(&mutexAttr);
    pthread_mutex_destroy(mutex);
    shm_unlink(MUTEX);

   	cout << "******** Session details ********" << endl;
   	cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;

    exit(0);

	return 0;
}



