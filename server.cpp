/*
# Game: Rock Paper Scissors Multiplayer
# Copyright (c) 2024 Takeda Software Foundation
# File: Server's part
*/


#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <string>

std::mutex mtx;
std::unordered_map<int, SOCKET> clients;

SOCKET server_socket;

void ServerHandle();
void ClientHandle(int);

int main()
{
    WSADATA Wdata;
        if(WSAStartup(MAKEWORD(2, 2), &Wdata) != NO_ERROR)
        {
            std::cerr << "Failed to start WSA" << std::endl;
            return 1;
        }

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(3060);
    int sizeofAddr = sizeof(addr);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    if(bind(server_socket, (SOCKADDR*)&addr, sizeofAddr) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }
    if(listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen" << std::endl;
        return 1;
    }

    std::cout << "Server started on port 3060" << std::endl;

    ServerHandle();
    closesocket(server_socket);
    WSACleanup();
    std::cerr << "Server was closed" << std::endl;
    std::cin.get();
    return EXIT_SUCCESS;
}

void ClientHandle(int index)
{
    char msg[256];
    while (1)
    {
        for (int i = 0; i < clients.size(); i++) 
        {
            int bytesReceived = recv(clients[index], msg, sizeof(msg), 0);
            if (bytesReceived <= 0) {
                std::cout << "Client " << index << " disconnected or error occurred." << std::endl;
                closesocket(clients[index]);
                clients.erase(index);
                return;
            }

            msg[bytesReceived] = '\0'; 
            std::cout << "Received from " << index << ": " << msg << std::endl;
            if(clients.size() == 0)
                return;
                
            if (i != index) {
                std::string fullMessage = "Player "+std::to_string(index) + ": " + msg;
                send(clients[i], fullMessage.c_str(), fullMessage.size(), 0);
            }
        }
    }
    
}
void ServerHandle()
{
    SOCKET client_socket;
    SOCKADDR_IN client_addr;
    int client_addr_size = sizeof(client_addr);

    while (1) 
    {
        client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_size);
        
        if (client_socket == INVALID_SOCKET) 
        {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        clients[clients.size()] = client_socket;
        std::thread(ClientHandle, (int)clients.size()-1).detach();
    }
}