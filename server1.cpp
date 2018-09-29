#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <thread>
#include <list>
#include <vector>
using namespace std;

struct client_type
{
    int sd;

};


vector<client_type> clients;


void sendAll (char* msg) {
    for(int i = 0; i < clients.size(); i++) {
      send(clients[i].sd, (char*)&msg, strlen(msg), 0);
    }
}


void handleClient(client_type client, fd_set tmpfd){
    char msg[1500];
    if(FD_ISSET(client.sd, &tmpfd)){
      cout << "waiting to receive" << endl;
      int bytesRead = recv(client.sd, (char*)&msg, sizeof(msg), 0);
      cout << "im the receiver" << endl;
      sendAll(msg);
    }
}





int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    //grab the port number
    int port = atoi(argv[1]);
    //buffer to send and receive messages with
    char msg[1500];
    int num_clients = -1;
    int temp_id = -1;
    bool firsttime = true;

    const char OPTION_VALUE = 1;
    const int MAX_CLIENTS = 5;
    std::vector<client_type> client(MAX_CLIENTS);

    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    std::thread my_thread[MAX_CLIENTS];

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr,
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to address" << endl;
        exit(0);
    }
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to
    //handle the new connection with client


    //lets keep track of the session time
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    //also keep track of the amount of data sent as well
    int bytesRead, bytesWritten = 0;
    int maxsd = serverSd;
    struct timeval tv;

    fd_set servfd, tmpfd;

    while(1)
    {
      cout << maxsd << endl;
      tv.tv_usec = 0;
      tv.tv_sec = 10;
        memset(&msg, 0, sizeof(msg));

        int retval = select(maxsd + 1, &tmpfd, NULL, NULL, &tv);
        if(retval == -1) {
          cout << "error p2" << endl;
          break;
        }
        else if (retval > 0) {
          if(FD_ISSET(serverSd, &tmpfd)) {

          //Reset the number of clients
            num_clients = -1;

          //Create a temporary id for the next client
            temp_id = -1;
            int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
            cout << "new client connected!" << endl;
            client_type client;
            client.sd = newSd;
            clients.push_back(client);
            if(client.sd > maxsd) {
              maxsd = client.sd;
            }
            cout << newSd << endl;
            if(newSd < 0)
            {
                cerr << "Error accepting request from client!" << endl;
                exit(1);
            }
            cout << "connected clients: " << endl;
          }
          for (int i = 0; i < clients.size(); i++) {
            cout << clients[i].sd << endl;

            handleClient(clients[i], tmpfd);

        // send(clients[i].sd, (char*)&msg, strlen(msg), 0);
          }
        }
        else {
          cout << "no message in 10 sek" << endl;
        }




        /*for (int i = 0; i < MAX_CLIENTS; i++)
        {

            if (client[i].id == port &&  temp_id == -1)
            {
                client[i].id = newSd;
                client[i].id = i;
                temp_id = i;
                //std::cout << "Client #" << client[temp_id].id << " just connected!" << std::endl;
            }

            if (client[i].id != -1)
                num_clients++;

            //std::cout << client[i].socket << std::endl;
        }

        if (MAX_CLIENTS > 5)
        {
          cout << "server is full" << endl;



            //Create a thread process for that client
          //  my_thread[temp_id] = std::thread(process_client, std::ref(client[temp_id]), std::ref(client), std::ref(my_thread[temp_id]));
        }
        else
        {
          //Send the id to that client
          if(firsttime == true){
            std::cout << "someOne tried to connect!" << std::endl;
            firsttime = false;
          }

      cout << ">";
      string data;
      getline(cin, data);
      memset(&msg, 0, sizeof(msg)); //clear the buffer
      strcpy(msg, data.c_str());
        }



        //receive a message from the client (listen)
        cout << "Awaiting client response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
        if(!strcmp(msg, "exit"))
        {
            cout << "Client has quit the session" << endl;
            break;
        }
        cout << "Client: " << msg << endl;
        cout << ">";
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg)); //clear the buffer
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            //send to the client that server has closed the connection
            send(newSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        //send the message to client
        bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);*/
    }



    //we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
  //  close(newSd);
    close(serverSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec)
        << " secs" << endl;
    cout << "Connection closed..." << endl;
    return 0;
}
