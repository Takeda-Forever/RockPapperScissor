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

std::unique_ptr<SOCKET> s;

void ClientHandle();

int main()
{
    std::cin.tie(NULL);
    std::cin.sync_with_stdio(false);
    std::string host;
    WSADATA wsaData;
    
    // Инициализация WSA
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return EXIT_FAILURE;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3060);

    // Ввод IP-адреса для соединения
    while (true) {
        std::cout << "Print server IP address: ";
        std::cin >> host;
        if (host == "localhost")
            host = "127.0.0.1";
        
        // Проверка IP-адреса
        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) > 0) {
            break;  // Валидный IP-адрес, выходим из цикла
        }

        std::cerr << "Invalid IP address, please try again." << std::endl;
        
    }

    // Инициализация сокета
    s = std::make_unique<SOCKET>(socket(AF_INET, SOCK_STREAM, 0));
    if (*s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Подключение к серверу
    while (connect(*s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect, retrying..." << std::endl;
        Sleep(1000);
    }

    std::cout << "Connected to server!" << std::endl;

    std::thread(ClientHandle).detach();  // Запуск потока для приёма данных от сервера
    std::string message;

    while (true) {
        std::getline(std::cin, message);

        // Проверка на команду "exit"
        if (message == "exit") {
            closesocket(*s);
            WSACleanup();
            std::cout << "Connection closed. Exiting..." << std::endl;
            return EXIT_SUCCESS;
        }

        // Отправка сообщения
        int msg_size = message.size();
        if (send(*s, (char*)&msg_size, sizeof(int), 0) == SOCKET_ERROR) {
            std::cerr << "Error sending message size" << std::endl;
            continue;
        }

        if (!message.empty()) {
            if (send(*s, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
                std::cerr << "Error sending message" << std::endl;
            }
        }

        Sleep(10);
    }

    return 0;
}

void ClientHandle()
{
    int msg_size;
    while (true) {
        // Приём размера сообщения
        int bytesReceived = recv(*s, (char*)&msg_size, sizeof(int), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error receiving message size or connection lost." << std::endl;
            closesocket(*s);
            WSACleanup();
            return;
        }

        // Приём сообщения
        std::string msg(msg_size, '\0');
        bytesReceived = recv(*s, &msg[0], msg_size, 0);

        if (bytesReceived > 0) {
            std::cout << "\r" << msg << std::endl;
            std::cout.flush();
        } else {
            std::cerr << "Error receiving message or connection lost." << std::endl;
            return;
        }
    }
}
