/*
# Game: Rock Paper Scissors Multiplayer
# Copyright (c) 2024 Takeda Software Foundation
# File: Client's part
*/

#include <iostream>
#include <thread>
#include <string>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <conio.h>  
#pragma comment(lib, "ws2_32.lib")

SOCKET s;

void ClientHandle(const SOCKET&);

int main()
{
    std::cin.tie(NULL);
    std::cin.sync_with_stdio(false);

    std::string host;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return EXIT_FAILURE;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3060);

    while (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        std::cout << "Print server IP address: ";
        std::cin >> host;
        if (host == "localhost")
            host = "127.0.0.1";
        std::cin.ignore();
        Sleep(1000);
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return EXIT_FAILURE;
    }

    while (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Finding..." << std::endl;
        Sleep(1000);
    }
    std::cout << "Connecting..." << std::endl;

    std::thread(ClientHandle, s).detach();
    std::string message;
    std::cout << "Connect successful!" << std::endl;
    while (true) {
        std::getline(std::cin, message);

        if (message == "exit") {
            closesocket(s);
            WSACleanup();
            exit(EXIT_SUCCESS);
        }

        int msg_size = message.size();
        send(s, (char*)&msg_size, sizeof(int), NULL);

        if (!message.empty()) {
            send(s, message.c_str(), message.size(), 0); 
        }

        Sleep(10);
    }

    closesocket(s);
    return 0;
}

void ClientHandle(const SOCKET& s)
{
    int msg_size;

    while (1) {
        recv(s, (char*)&msg_size, sizeof(int), 0);
        char* buffer = new char[msg_size + 1];
        buffer[msg_size] = '\0';
        int bytesReceived = recv(s, buffer, msg_size, 0);

        if (bytesReceived > 0) {
            std::string current_input;
            std::cout << "\r" << "\r";  
            std::cout << "\r" << buffer << std::endl; 
            std::cout.flush();
        }

        delete[] buffer;
    }
}
