#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string>

#define PORT 8080
#define MAXCLIENTS 30

int main(int argc, char** argv)
{
    int version; //store if we're doing http or basic tcp (0 - basic, 1 - http)

    //let's see what args we got passed!
    if(argc > 0)
    {
        for(int i = 1; i < argc; i++) //loop through argv
        {
            if(argv[i] == std::string("--http")) //see if the pointer to a char within the argv array is equal to --http (needs to be of type string, so not comparing pointer to pointer)
            {
                version = 1;
                break;
            }
            else if(argv[i] == std::string("--basic")) //same as previous, just --basic
            {
                version = 0;
                break;
            }
            //else, they gave a bad flag
            std::cerr << "Invalid flag. Options: --http (for http), --basic (for basic TCP)" << std::endl;
            return -1;
        }
    }

    //create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0); //(returns int) - makes socket (returns what socket it is in terms of an int)
    if(listening < 0) //check if we were able to make a socket!
    {
        std::cerr << "Can't create socket!" << std::endl;
        return -1;
    }

    //bind the socket to an IP/Port
    sockaddr_in hint{}; // This creates a structure for the ipv4 info of the socket.
    hint.sin_family = AF_INET; //set the family to ipv4
    hint.sin_port = htons(PORT); //set the port to the macro set above (use host-to-network-short to conver the int to the port)
    hint.sin_addr.s_addr = INADDR_ANY; //set the ip to any address

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) < 0) //attempt to bind socket (inticated by what number the `listening` socket is) to ip/port
    {
        std::cerr << "Can't bind to IP/Port" << std::endl;
        return -1;
    }

    //mark the socket for listening

    if(listen(listening, MAXCLIENTS) < 0 /*SOMAXCONN = maximum amount of connections, defined by sys/socket.h*/) //attempt to listen on the socket number indicated by `listening`
    {
        std::cerr << "Can't listen on the socket!" << std::endl;
        return -1;
    }

    //FD_CLR() = Remove 1 from set
    //FD_SET() = Add to set
    //FD_ZERO() = Remove everything from set
    //FD_ISSET() = Check if something is part of a set

    fd_set master; //define the set

    //definitions
    int max_sd, //max socket descriptor
        activity, //result from select()
        client_socks[MAXCLIENTS]{}, //array of client sockets
        client_socket; //temp variable, so we can store the result from accept()

    while (true)
    {
        FD_ZERO(&master); //make sure it's cleared (give us a clean slate)

        FD_SET(listening, &master); //add the listening socket (server) to the set (this always exists, as long as the program is running)

        max_sd = listening; //max socket descriptor set to the listening socket (need this for the select func)

        for (int &client_sock : client_socks) {

            if (client_sock > 0) //make sure the particular socket exists
            {
                FD_SET(client_sock, &master); //add it to the set
            }
            if (client_sock > max_sd) //if the socket is greater than our current maximum socket descriptor
            {
                max_sd = client_sock;
            }
        }

        //wait for one or more of the socket descriptors to be ready for IO
        activity = select( max_sd + 1 , &master , NULL , NULL , NULL);

        //check if something went wrong
        if (activity < 0) //error!
        {
            std::cerr << "Error while trying to select!" << std::endl;
        }

        //this stuff will occur once the select function returns something (we woke up!)
        int addrlen = sizeof(hint);

        if (FD_ISSET(listening, &master)) //if the select got that the listening (server) socket had action occur on it
        {
            client_socket = accept(listening, (sockaddr *) &hint, &addrlen); //accept the first client "waiting to get in"
            std::cout << client_socket << std::endl;

            if (client_socket < 0) {
                std::cerr << "Something went wrong when trying to accept a client socket!" << std::endl;
                break;
            }

            std::cout << "New connection: " << inet_ntoa(hint.sin_addr) << " on port " << ntohs(hint.sin_port) << std::endl;

            if (send(client_socket, "Welcome to the socket party!\n", strlen("Welcome to the socket party!\n"), 0) != strlen("Welcome to the socket party!\n")) { //greetings!
                std::cerr << "Error when sending welcome message." << std::endl; //something went wrong ;(
            }

            for (int &client_sock : client_socks) //for each index, set client_sock to the address of the index of client_socks, so that we can set the value of it!
            {
                if(client_sock == 0) //if this position is null (0)
                {
                    client_sock = client_socket; //add the new client_socket to the array of client socks
                    break;
                }
            }
        }

        else //else, there was action on a client socket (most likely a message is being sent!)
        {
            char buffer[2048]; //we need somewhere to store clients messages!

            for (int &client_sock : client_socks) //loop through the client sockets
            {
                if (client_sock != 0) //make sure we're actually going to run the stuff below on an actual socket!
                {
                    if (FD_ISSET(client_sock, &master)) //check if the select got action on the particular index in the client_socks array
                    {
                        memset(&buffer, 0, sizeof(buffer)); //make sure the buffer is clear!
                        if (read(client_sock, &buffer, 2048) == 0) //check if nothing was recieved from the client
                        {
                            getpeername(client_sock, (sockaddr*)&hint, (socklen_t*)&addrlen) < 0; //gets networking info, based off of which socket is passed (the if statement checks for errors). In addition, it sets the values of hint to the info from the passed socket

                            //print that the client disconnected
                            std::cout << "A client has disconnected! IP: " << inet_ntoa(hint.sin_addr) << " Port: " << ntohs(hint.sin_port) << std::endl;

                            close(client_sock); //close the socket
                            client_sock = 0; //set its value in the array to 0, so that we can reuse it!
                        }

                        //else, we got message from the client
                        for (int &socket : client_socks)
                        {
                            if (socket != client_sock)
                            {
                                send(socket , buffer, strlen(buffer), 0 );

                            }
                        }
                    }
                }
            }
        }
    }
    return 0;

}


