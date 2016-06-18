#pragma once

#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include "Room.h"
#include "Game.h"

using namespace std;

class Room;
class Game;

class User
{
    public:
        User(string username, SOCKET sock);
        void send(string message);
        string getUsername();
        SOCKET getSocket();
        Room* getRoom();
        Game* getGame();
        void setGame(Game* game);
        void clearRoom();
        bool createRoom(int id, User* _admin, string name,int maxUsers, int questionTime, int questionNo);
        bool joinRoom(Room* room);
        void leaveRoom();
        int closeRoom();
        bool leaveGame();
    private:
        string _username;
        Room* _currRoom;
        Game* _currGame;
        SOCKET _sock;
};