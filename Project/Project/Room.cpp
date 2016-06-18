#include <algorithm>
#include "Room.h"

using namespace std;

Room::Room(int id, User* admin, string name, int maxUsers, int questionTime, int questionNo) : _id(id), _admin(admin), _name(name), _maxUsers(maxUsers), _questionTime(questionTime), _questionsNo(questionNo) { _users.push_back(admin); }

bool Room::joinRoom(User* user)
{
	if (_users.size() < (unsigned int)_maxUsers)
	{
		_users.push_back(user);
		stringstream message;
		message << to_string(ROOM_JOIN_SUCCESS) << Helper::getPaddedNumber(_questionsNo, 2) << Helper::getPaddedNumber(_questionTime, 2);
		user->send(message.str());
		vector<User*>::iterator it = _users.begin();
		for (it; it != _users.end(); it++)
		{
			(**it).send(to_string(ROOM_USERS_RESPONSE) + getUsersListMessage());
		}
		return true;
	}
	user->send(to_string(ROOM_JOIN_ROOM_FULL));
	return false;
}

void Room::leaveRoom(User* user)
{
    _users.erase(remove(_users.begin(), _users.end(), user),_users.end());
	vector<User*>::iterator it = _users.begin();
	for (it; it != _users.end(); it++)
	{
		(**it).send(to_string(ROOM_USERS_RESPONSE) + getUsersListMessage());
	}
}

int Room::closeRoom(User* user)
{
	vector<User*>::iterator it = _users.begin();
	for (it; it != _users.end(); it++)
	{
		(**it).send(to_string(ROOM_CLOSE_RESPONSE));
	}
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
	for (unsigned int i = 0; i < _users.size(); i++)
	{
		userName = _users[i]->getUsername();
		val = val + Helper::getPaddedNumber(userName.length(), 2) + userName;
	}
	return val;
}