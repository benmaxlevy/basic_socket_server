#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    //create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0); //(returns int) - makes socket (returns what socket it is in terms of an int)
    if(listening < 0) //check if we were able to make a socket!
    {
        std::cerr << "Can't create socket!" << std::endl;
        return -1;
    }

    //bind the socket to an IP/Port

    /* This creates a structure for the ipv4 info of the socket. */sockaddr_in hint;
    hint.sin_family = AF_INET; //set the family to ipv4
    hint.sin_port = htons(PORT); //set the port to the macro set above (use host-to-network-short to conver the int to the port)
    hint.sin_addr.s_addr = INADDR_ANY; //set the ip to any address

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) < 0) //attempt to bind socket (inticated by what number the `listening` socket is) to ip/port
    {
        std::cerr << "Can't bind to IP/Port" << std::endl;
        return -1;
    }

    //mark the socket for listening

    if(listen(listening, SOMAXCONN) < 0 /*SOMAXCONN = maximum amount of connections, defined by sys/socket.h*/) //attempt to listen on the socket number indicated by `listening`
    {
        std::cerr << "Can't listen on the socket!" << std::endl;
        return -1;
    }

    //accept a call
    sockaddr_in client; //same as `listening`, but for data of the client

    socklen_t clientSize = sizeof(client);

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    if(clientSocket < 0) //attempt to accept from the client socket to the server socket (listening)
    {
        std::cerr << "Can't accept the client to the server!" << std::endl;
        return -1;
    }

    //close the listening socket

    close(listening); //close the server socket

    //define stuff for getnameinfo
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memset(host, 0, sizeof(host)); //cleanup
    memset(service, 0, sizeof(service)); //cleanup

    int result = getnameinfo((sockaddr*)&client, // attmept to get info on the client
                             sizeof(client),
                             host,
                             sizeof(host),
                             service,
                             sizeof(service),
                             0);

    if(result) //see if the getnameinfo was successful
    {
        std::cout << host << " connected on " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, sizeof(host)); //ip->string
        std::cout << host << " connected on " << ntohs(client.sin_port) /*converts ip to string*/ << std::endl;
    }

    //while receiving, display message

    char buffer[4096];
    while(true)
    {
        //clear buffer
        memset(buffer, 0, sizeof(buffer));
        //wait for a message
        int bytesRecv = recv(clientSocket, buffer, sizeof(clientSocket), 0); //bytes recieved
        if(bytesRecv < 0)
        {
            std::cerr << "There was a connection issue!" << std::endl;
            break;
        }
        if(bytesRecv == 0)
        {
            std::cout << "The client has disconnected" << std::endl;
        }

        std::cout << "Received:" << std::string(buffer, 0, bytesRecv) << std::endl; //display message at server

        send(clientSocket, buffer, bytesRecv + 1, 0); //send message back
    }

    //close socket
    close(clientSocket);

    return 0;
}