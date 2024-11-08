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

std::unordered_map<int, std::unique_ptr<SOCKET>> clients;
SOCKET server_socket;
std::mutex clients_mutex;

void ServerHandle();
void ClientHandle(int);

//Create the function for using them. Example as follows:
//      - /send_message_to_client 1 "Hello, client!"
//      - /disconnect_client 1
//      - /get_clients_count
//      - /get_client_socket 1


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
    int msg_size;

    while (true)
    {
        std::unique_ptr<SOCKET>& client_socket = clients[index];  // Получаем ссылку на умный указатель клиента

        // Прием размера сообщения
        int bytesReceived = recv(*client_socket, (char*)&msg_size, sizeof(int), 0);  // * для разыменования unique_ptr
        if (bytesReceived <= 0)
        {
            std::cerr << "Client " << index << " disconnected or error occurred." << std::endl;

            std::lock_guard<std::mutex> guard(clients_mutex);
            clients.erase(index);  // Удаляем клиента — сокет закроется автоматически
            return;
        }

        // Создание буфера для сообщения
        std::string msg(msg_size, '\0');

        // Прием сообщения
        bytesReceived = recv(*client_socket, &msg[0], msg_size, 0);
        if (bytesReceived <= 0)
        {
            std::cerr << "Error receiving message from client " << index << std::endl;
            continue;
        }

        // Формирование сообщения
        std::string message = "Player " + std::to_string(index) + ": " + msg;
        std::cout << message << std::endl;

        // Отправка сообщения всем остальным клиентам
        std::lock_guard<std::mutex> guard(clients_mutex);
        for (auto& client : clients)
        {
            if (client.first != index)
            {
                int msg_len = static_cast<int>(message.size());
                send(*client.second, (char*)&msg_len, sizeof(int), 0);
                send(*client.second, message.c_str(), msg_len, 0);
            }
        }
    }
}

void ServerHandle()
{
    SOCKET client_socket;
    SOCKADDR_IN client_addr;
    int client_addr_size = sizeof(client_addr);

    while (true)
    {
        client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_size);
        if (client_socket == INVALID_SOCKET)
        {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        {
            std::lock_guard<std::mutex> guard(clients_mutex);
            int new_client_id = clients.size();

            // Создаем умный указатель на сокет клиента
            clients[new_client_id] = std::make_unique<SOCKET>(client_socket);
            std::cout << "New connect: Player " << new_client_id << std::endl;

            // Передаем в поток ID клиента
            std::thread(ClientHandle, new_client_id).detach();
        }
    }
}