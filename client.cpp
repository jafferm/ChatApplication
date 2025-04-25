#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 54000

void receiveMessages(SOCKET sock) {
    char buffer[4096];
    while (true) {
        ZeroMemory(buffer, 4096);
        int bytesReceived = recv(sock, buffer, 4096, 0);
        if (bytesReceived > 0) {
            std::cout << "\nServer: " << buffer << std::endl;
            std::cout<<"Please enter your message:"<<std::endl;
        }
    }
}

int main() {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Can't create socket! Exiting..." << std::endl;
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed!" << std::endl;
        return -1;
    }

    std::cout << "Connected to the chat server!" << std::endl;

    // Start thread to receive messages
    std::thread recvThread(receiveMessages, clientSocket);
    recvThread.detach();

    // Send messages
    std::string message;
    while (true) {
        std::cout<<"Please enter your message:";
        std::getline(std::cin, message);
        if (message == "exit") break;
        send(clientSocket, message.c_str(), message.size() + 1, 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
