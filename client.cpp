/*
# Game: Rock Paper Scissors Multiplayer
# Copyright (c) 2024 Takeda Software Foundation
# File: Client's part
*/


#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <thread>
#include <WinSock2.h>
#include <string>
#include <Ws2tcpip.h>

SOCKET s;

void ClientHandle(const SOCKET&);

int main()
{
    std::cin.tie(NULL);
    std::cin.sync_with_stdio(false);

    std::string host;

    std::cout << "Print server IP address:";
    std::cin >> host;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return EXIT_FAILURE;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3060);

    while(inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
            std::cout << "Print server IP address:";
            std::cin >> host;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return EXIT_FAILURE;
    }

    while(connect(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Finding..." << std::endl;
        Sleep(1000);
    }
    std::cerr << "Connecting..." << std::endl;

    std::thread(ClientHandle, s).detach();
    std::string message;
    message.resize(256);
    std::cerr << "Connect successful!" << std::endl;
    std::getline(std::cin, message);
    while (message != "exit") {
    std::cout << "Message:";
    std::getline(std::cin, message);

    if(!message.empty()) {
        send(s, message.c_str(), message.length(), 0); // Отправляем сообщение серверу
    }
    message.clear();
}
    
    closesocket(s);
    return 0;
}

void ClientHandle(const SOCKET& s)
{
    char msg[256];
    while (1) {
        int bytesReceived = recv(s, msg, sizeof(msg), 0);
        if (bytesReceived > 0) {
            msg[bytesReceived] = '\0'; // Завершаем строку
            std::cout << "\r\n" << msg << std::endl << "Message:"; // Выводим полученное сообщение
            
        }
    }
}
