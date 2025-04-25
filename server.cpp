#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<algorithm>

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define SERVER_PORT 54000
#define MAX_CLIENTS 10

using namespace std;

vector<SOCKET> clients; //store client sockets
mutex client_mutex;

void handle_client(SOCKET client_socket, int client_num){
    char buffer[4096];
    while(true){
        ZeroMemory(buffer,4096);
        int bytes_recv=recv(client_socket,buffer,4096,0);
        if(bytes_recv<=0){
            cout<<"Client Disconnected";
            break;
        }
    
        cout<<"Client "<<client_num<<"'s Message:"<<buffer<<endl;

        lock_guard <mutex> lock(client_mutex);
        for(SOCKET soc: clients){
            if(soc!=client_socket){
                send(soc,buffer,bytes_recv,0);
            }
        }
    }
    lock_guard <mutex> lock(client_mutex);
    clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());
    closesocket(client_socket);

}

int main(){
    
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        cerr << "Can't create socket! Exiting..." << std::endl;
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(listening, MAX_CLIENTS);

   cout << "Server is listening on port " << SERVER_PORT << "..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listening, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Error accepting client connection!" << std::endl;
            continue;
        }

        cout << "New client connected!" << std::endl;

        lock_guard<mutex> lock(client_mutex);
        clients.push_back(clientSocket);

        thread clientThread(handle_client, clientSocket,clients.size());
        clientThread.detach();
    }

    closesocket(listening);
    WSACleanup();
    return 0;
}