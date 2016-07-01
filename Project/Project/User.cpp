#include "User.h"
#include "Helper.h"

User::User(string username, SOCKET sock) : _username(username), _sock(sock)
{
    _currRoom = nullptr;
    _currGame = nullptr;
}

void User::send(string message)
{
    Helper::sendData(_sock, message);
}

string User::getUsername()
{
    return _username;
}

SOCKET User::getSocket()
{
    return _sock;
}

Room* User::getRoom()
{
    return _currRoom;
}

Game* User::getGame()
{
    return _currGame;
}

void User::setGame(Game* game)
{
    _currGame = game;
}

void User::setRoom(Room* room)
{
	_currRoom = room;
}


void User::clearRoom()
{
    _currRoom = nullptr;
}

bool User::createRoom(int id, User* _admin, string name,int maxUsers, int questionTime, int questionNo)
{
    if (_currRoom != nullptr)
    {
        send("1141");
        return false;
    }
    else
    {
        Room* room = new Room(id, _admin, name,maxUsers, questionTime, questionNo);
        _currRoom = room;
        send("1140");
    }
    return true;
}

bool User::joinRoom(Room* room)
{
	if (_currRoom == nullptr)
	{
		_currRoom = room;
		room->joinRoom(this);
	}
    else return false;
    return true;
}

void User::leaveRoom()
{
    if (_currRoom) _currRoom->leaveRoom(this);
    _currRoom = nullptr;
}

int User::closeRoom()
{
    if (!_currRoom) return -1;
    _currRoom->Room::closeRoom(this);
    int roomNum = _currRoom->getId();
    delete _currRoom;
	_currRoom = nullptr;
    return roomNum;
}

bool User::leaveGame()
{
	Game* g = _currGame;
    if (!_currGame) return false;
    _currGame = nullptr;
	return g->leaveGame(this);
}