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

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return EXIT_FAILURE;
    }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3060);

    while(inet_pton(AF_INET, (PCSTR)host.c_str(), &addr.sin_addr) <= 0) {
            std::cout << "Print server IP address:";
            std::cin >> host;
            if(host == "localhost") 
                host = "127.0.0.1";
            std::cin.ignore();
            Sleep(1000);
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return EXIT_FAILURE;
    }

    while(connect(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Finding..." << std::endl;
        Sleep(1000);
    }
    std::cout << "Connecting..." << std::endl;

    std::thread(ClientHandle, s).detach();
    std::string message;
    std::cout << "Connect successful!" << std::endl;
    /* У geline есть некоторые ошибки с вводом, поэтому это строка необходима что бы его убрать */        std::getline(std::cin, message);

    while (message != "exit") {
    std::getline(std::cin, message);

    int msg_size = message.size();
    send(s, (char*)&msg_size, sizeof(int), NULL);

    if(!message.empty()) {
        send(s, message.c_str(), message.size(), 0); // Отправляем сообщение серверу
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
//         msg_size = recv(s, (char*)&msg_size, sizeof(int), NULL);
//         if (msg_size <= 0) {
//             std::cerr << "Connection closed" << std::endl;
//             closesocket(s);
//             WSACleanup();
//             exit(SOCKET_ERROR);
//         }
//         char *msg = new char(msg_size);
//         int bytesReceived = recv(s, msg, sizeof(msg), 0);
//         if (bytesReceived > 0) {
//             msg[bytesReceived] = '\0'; // Завершаем строку
//             std::cout << "\r\n" << msg << std::endl << "Message:"; // Выводим полученное сообщение
            
//         }
//     }
// }

            recv(s, (char*)&msg_size, sizeof(int), 0);
            char *buffer = new char[msg_size + 1];
            buffer[msg_size] = '\0';
            int bytesReceived = recv(s, buffer, msg_size, 0);   
            std::cout << "\r" << std::string(50, ' ') << "\r"; // Очищаем текущий ввод
            std::cout << buffer << std::endl; // Выводим полученное сообщение
            std::cout << "Message: "; // Возвращаем пользователю строку для ввода
            std::cout.flush(); // Сброс буфера для немедленного вывода
            delete[] buffer;
        }
    }

