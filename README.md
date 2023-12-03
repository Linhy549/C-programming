# Huiying Lin - Project 2 

## System documentation
1. A high-level data flow diagram for the system

2. A list of routines and their brief descriptions
    - Server starts and listening the incoming clients
    - Clients are requires to enter user-id to login.
    - If login successfully, clients can have services such as "Check online user", "Sends Private message".
    - The server would sends out message to incoming clients when it reaches out the maximun of 6 clients.
    - The user can exit from the client, then the socket with server is going to terminate. 
3. Implementation details
> The server and client are commnicated each others by sokcet. The server keep listening for incoming clients to connect. Once a client connected to server, then the server would start to authenticate the user id. The client will gets the users information and sends to server. If the user id is valid, then server would create a Recv-thread to keep getting data from client to support different services, such as Login, LogOut, Login_List, Msg. These services has been generated as pre-define protocol as the following: <LOG></LOG>, for example. Same thing happen in client side, once it's login, then Recv-thread has been creating to keep getting data from server.  
## Test documentation
1. How you tested your program
    - Since the program has been divided into different parts
        - Design and implements Consle-Based UI
        - Test the implementation of CRC and hamming detetion
        - Test the error insertion module
        - Test the data communication between server and clients
        - Test the the different prototypes
    - Above 1 is critical for the correction of the program. Once each part being succesfully working then, merge all the code together and test the whole functionality of it.
    - Finally, test the large data set if it works propertly.
2. Testing outputs
- Tag language commands and responses
- CRC error detection
- Hamming detection and correction
- One to one chat

## User documentation
1. Where is your source
> The program are includes:
> - Two main programs: client.c and server.c
> - Support subprograms: encDec.h, phy_layer.c, data_layer.c, app_layer,c
2. How to run your program
> Need to complie server.c and client.c programs before making them run.
> - gcc -o client clietn.c app_layer.c phy_layer.c data_layer.c
>- gcc -o server server.c
3.  Vidoe Records
> Recorded a demo of this project:
> - ew
> - fe
