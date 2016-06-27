#include <algorithm>
#include "Room.h"
Room::Room(int id, User* admin, string name, int maxUsers, int questionTime, int questionNo) : _id(id), _admin(admin), _name(name),_maxUsers(maxUsers), _questionTime(questionTime), _questionsNo(questionNo) {}

bool Room::joinRoom(User* user)
{
    if (_users.size() < (unsigned int)_maxUsers)
    {
		_users.push_back(user);
        return true;
    }
    return false;
}

void Room::leaveRoom(User* user)
{
    _users.erase(remove(_users.begin(), _users.end(), user),_users.end());
}

int Room::closeRoom(User* user)
{
    return _id;
}

vector<User*> Room::getUsers()
{
    return _users;
}

int Room::getQuestionsNo()
{
    return _questionsNo;
}

int Room::getId()
{
    return _id;
}

string Room::getName()
{
    return _name;
}


string Room::getUsersListMessage()
{
	string val = to_string(_users.size());
	string userName;
	for (int i = 0; i < _users.size(); i++)
	{
		userName = _users[i]->getUsername();
		val = val + Helper::getPaddedNumber(userName.length(), 2) + userName;
	}
	return val;
}