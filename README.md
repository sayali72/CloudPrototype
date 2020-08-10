# CloudPrototype

1. Project has four source files - serverA, serverB, client1 and client2.

2. Language and Complier: 
C++, GNU C++ Compiler

3. Source code:
serverA.cpp, serverB.cpp, client1.cpp, client2.cpp, and makefile for all source files.
Program is designed to work on separate machines and compatible on Windows 10, Windows server 2008 and LINUX (Linux is preferred).
Program will work ONLY in LAN.

4. server1 listen at port 4000.
server2 listen at port 6000.
Both the servers also listen at 4005. This port is dedicated specifically for Synchronization purpose.
The port numbers can be altered in a serverA.cpp or serverB.cpp
Note: Use host "hostname" to confirm the IP address of machin on which server is hosted

5. client1(user1) and client2(user2) login details are stored as userInfoA.txt on serverA and so on (login details are maintained in single file on a server).
user1 file content on serverA will be stored as user1fileA.txt and user2 file content on serverB will be stored as user2fileB.txt

6. To compiler a severA and a serverB(from Terminal): 
make or g++ serverA.cpp -lpthread -lrt -o serverA
make or g++ serverA.cpp -lpthread -lrt -o serverB

7. To run both server(no arguments needed):
./serverA
./serverB
Note: server should always be executed first (to bind to the socket).

8. To compile a client(from Terminal):
make

9. To run a client(MUST provide 2 arguments):
./client1 "IP address" 4000
In order to establish socket connection, Client needs to provide the IP address and the port number of the server.

10. For first time login, server generates a key. This key will be required during authentication for read/write/delete operation. Server also generates an empty file for user.
(User can perom write/delete on this file.)

11. Client gets disconnected, after three unsuccessful login attempts.

11. After success login, Server provide 4 choices: i)Read ii)Write iii)Delete iv)Exit
	- Read operation: Client need to choose "Read" option followed by a "filename".
	  Client will be denied access if the file is being written by the owner.
	
	- Write operation: Client need to choose "Write" option followed by a "filename".
	  After successful authentication using Key, server fetch and display content to the client.
	  Client then need to provide the content to be written to the file.
	  
	- Delete operation: Client need to choose "Write" option followed by a "filename".
	  After successful authentication using Key,  server fetch and display content to the client.
	  Client then need to provide the content to be deleted from the file.

	- Exit operation: Client need to choose "Exit" option.
	  This option cause server to initiate synchronization with other server.





