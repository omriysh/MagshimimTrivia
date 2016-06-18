#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include "TriviaServer.h"
#include "Validator.h"
#define PORT 8820
#define IFACE 0
#include <algorithm>
using namespace std;

int TriviaServer::_roomIdSequence;

TriviaServer::TriviaServer() 
{
	_roomIdSequence = 0;
	_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket"); 
	/*DataBase Build*/
}

TriviaServer::~TriviaServer()
{
	RecievedMessage* m=nullptr;
	User *u;
	SOCKET s;
	map<int, Room*>::iterator it = _roomsList.begin();
	for (it; it != _roomsList.end(); it++)
	{
		delete it->second;
	}
	map<SOCKET, User*>::iterator it2 = _connectedUsers.begin();
	for (it2; it2 != _connectedUsers.end(); it2++)
	{
		u = it2->second;
		s = it2->first;
		m = new RecievedMessage(s, SIGN_OUT_REQUEST);
		m->setUser(u);
		handleSignout(m);
		delete u;
		if (s)
		{
			closesocket(s);
		}
		delete m;
	}
	closesocket(_socket);
}

void TriviaServer::server()
{
	bindAndListen();
	thread handleMessages(&TriviaServer::handleRecievedMessages,this);
	handleMessages.detach();
	while (true)
	{
		cout << "accepting client..." << endl;
		accept();
	}
}

void TriviaServer::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	cout << "binded" << endl;

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	cout << "listening..." << endl;
}

void TriviaServer::accept()
{
	SOCKET client_socket = ::accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	cout << "Client accepted !" << endl;
	//create new thread for client	and detach from it
	std::thread tr(&TriviaServer::clientHandler, this, client_socket);
	tr.detach();
}

void TriviaServer::clientHandler(SOCKET s)
{
	bool clientConnected = true;
	while (clientConnected)
	{
		try
		{
			int code = Helper::getMessageTypeCode(s);
			RecievedMessage* m = buildRecieveMessage(s, code);
			addRecievedMessage(m);
		}
		catch (...)
		{
			RecievedMessage* m = buildRecieveMessage(s, EXIT_APPLICATION);
			addRecievedMessage(m);
			clientConnected = false;
		}
	}
}

void TriviaServer::safeDeleteUser(RecievedMessage* m)
{
	try
	{

		SOCKET soc = m->getSock();
		handleSignout(m);
		if (soc != NULL)
		{
			closesocket(soc);
		}
	}
	catch (...)
	{ }
}

void TriviaServer::addRecievedMessage(RecievedMessage* m)
{
	std::unique_lock<std::mutex> locker(_mtxRecievedMessages);
	locker.try_lock();
	cout << "RECIEVING:: " << m->getMessageCode();
	for (vector<string>::iterator it = m->getValues()->begin(); it != m->getValues()->end(); it++)
	{
		cout << " " << it->c_str();
	}
	if (m->getUser())
	{
		cout << " - User: " << m->getUser()->getUsername();
	}
	cout << endl;
	_queRcvMessages.push(m);	
	locker.unlock();
	_cond.notify_all();
	
}

RecievedMessage* TriviaServer::buildRecieveMessage(SOCKET s, int code)
{
	vector<string> values;
	int len;
	switch (code)
	{
	case SIGN_IN_REQUEST:
		for (int i = 0; i < 2; i++)
		{
			len = Helper::getIntPartFromSocket(s, 2);
			values.push_back(Helper::getStringPartFromSocket(s, len));
		}
		break;
	case SIGN_OUT_REQUEST:
		break;
	case SIGN_UP_REQUEST:
		for (int i = 0; i < 3; i++)
		{
			len = Helper::getIntPartFromSocket(s, 2);
			values.push_back(Helper::getStringPartFromSocket(s, len));
		}
		break;
	case AVAILABLE_ROOM_REQUEST:
		break;
	case ROOM_USERS_REQUEST:
	case ROOM_JOIN_REQUEST:
		values.push_back(Helper::getStringPartFromSocket(s, 4));
		break;
	case ROOM_LEAVE_REQUEST:
		break;
	case ROOM_CREATE_REQUEST:
		len = Helper::getIntPartFromSocket(s, 2);
		values.push_back(Helper::getStringPartFromSocket(s, len));
		values.push_back(Helper::getStringPartFromSocket(s, 1));
		values.push_back(Helper::getStringPartFromSocket(s, 2));
		values.push_back(Helper::getStringPartFromSocket(s, 2));
		break;
	case ROOM_CLOSE_REQUEST:
		break;
	case EXIT_APPLICATION:
		break;
	case GAME_LEAVE:
		break;
	case ANSWER:
		values.push_back(Helper::getStringPartFromSocket(s, 1));
		values.push_back(Helper::getStringPartFromSocket(s, 2));
		break;
	}

	RecievedMessage* m;
	if (!values.empty())
	{
		m = new RecievedMessage(s, code, values);
	}
	else
	{
		m = new RecievedMessage(s, code);
	}
	m->setUser(getUserBySocket(s));
	return m;
}

void TriviaServer::handleRecievedMessages()
{
	std::unique_lock<std::mutex> locker(_mtxRecievedMessages);
	while (true)
	{
		if (_queRcvMessages.empty())
		{
			_cond.wait(locker);
		}
		switch (_queRcvMessages.front()->getMessageCode())
		{
		case SIGN_IN_REQUEST:
			handleSignin(_queRcvMessages.front());
			break;
		case SIGN_OUT_REQUEST:
			handleSignout(_queRcvMessages.front());
			break;
		case SIGN_UP_REQUEST:
			handleSignup(_queRcvMessages.front());
			break;
		case AVAILABLE_ROOM_REQUEST:
			handleGetRooms(_queRcvMessages.front());
			break;
		case ROOM_USERS_REQUEST:
			handleGetUsersInRoom(_queRcvMessages.front());
			break;
		case ROOM_JOIN_REQUEST:
			handleJoinRoom(_queRcvMessages.front());
			break;
		case ROOM_LEAVE_REQUEST:
			handleLeaveRoom(_queRcvMessages.front());
			break;
		case ROOM_CREATE_REQUEST:
			handleCreateRoom(_queRcvMessages.front());
			break;
		case ROOM_CLOSE_REQUEST:
			handleCloseRoom(_queRcvMessages.front());
			break;
		case EXIT_APPLICATION:
			safeDeleteUser(_queRcvMessages.front());
			break;
		case GAME_LEAVE:
			handleLeaveGame(_queRcvMessages.front());
			break;
		case ANSWER:
			handlePlayerAnswer(_queRcvMessages.front());
			break;
		}
		delete _queRcvMessages.front();
		_queRcvMessages.pop();
	}
}

User* TriviaServer::handleSignin(RecievedMessage* m)
{
	int flag = true; // if wrong details
	map<string, string>::iterator it;
	map<SOCKET,User*>::iterator itConnected;
	User* login = nullptr;
	for (it = _dbUsers.begin(); it != _dbUsers.end(); it++)
	{
		if ((it->first) == ((*(m->getValues()))[0]) && (it->second) == ((*(m->getValues()))[1]))
		{
			flag = false;
			login = new User(it->first, m->getSock());
			for (itConnected = _connectedUsers.begin(); itConnected != _connectedUsers.end(); itConnected++)
			{
				if (itConnected->second->getUsername() == login->getUsername())
				{
					delete login;
					login = nullptr;
				}
			}
		}
	}

	if (login)
	{
		Helper::sendData(m->getSock(), to_string(SIGN_IN_SUCCESS));
		_connectedUsers[m->getSock()] = login;
	}
	else if (flag)
	{
		Helper::sendData(m->getSock(), to_string(SIGN_IN_WRONG_DETAILS));

	}
	else
	{
		Helper::sendData(m->getSock(), to_string(SIGN_IN_USER_CONNECTED));
	}
	return login;
}

bool TriviaServer::handleSignup(RecievedMessage* m)
{
	//[203##username##pass##email]
	string user = (*m->getValues())[0];
	string pass = (*m->getValues())[1];
	string email = (*m->getValues())[2];

	if (!Validator::isPasswordValid(pass))
	{
		Helper::sendData(m->getSock(), to_string(SIGN_UP_PASS_ILLEGAL));
		return false;
	}
	if (!Validator::isUsernameValid(user))
	{
		Helper::sendData(m->getSock(), to_string(SIGN_UP_USERNAME_ILLEGAL));
		return false;
	}
	//////////////////// TEMP CODE
	map<string, string>::iterator it = _dbUsers.begin();
	for (it; it != _dbUsers.end(); it++)
	{
		if (user == it->first)
		{
			Helper::sendData(m->getSock(), to_string(SIGN_UP_USERNAME_EXISTS));
			return false;
		}
	}
	_dbUsers.insert(pair<string, string>(user, pass));
	//////////////////// END TEMP CODE
	//////////////////// FUTURE CODE:
	/*if (!DataBase::isUserExists(user))
	{
	Helper::sendData(m->getSock(), to_string(SIGN_UP_USERNAME_EXISTS));
	return false;
	}
	if(!DataBase::addNewUser(user, pass, email))
	{
	Helper::sendData(m->getSock(), to_string(SIGN_UP_OTHER));
	return false;
	}
	*/
	Helper::sendData(m->getSock(), to_string(SIGN_UP_SUCCESS));
	return true;
}

void TriviaServer::handleSignout(RecievedMessage* m)
{
	User* u;
	if (u = m->getUser())
	{
		handleCloseRoom(m);
		handleLeaveRoom(m);
		//handleLeaveGame(m);
		map<SOCKET, User*>::iterator it = _connectedUsers.begin();
		for (it; it != _connectedUsers.end(); it++)
		{
			if (it->first == u->getSocket())
			{
				delete it->second;
				closesocket(it->first);
				_connectedUsers.erase(it);
				break;
			}
		}	
	}
}

bool TriviaServer::handleCreateRoom(RecievedMessage* m)
{
	User* curr = m->getUser();
	if (curr)
	{
		vector<string>* values = m->getValues();
		if (curr->createRoom(++_roomIdSequence, m->getUser(),(*values)[0] ,stoi((*values)[1], nullptr, 0), stoi((*values)[3], nullptr, 0), stoi((*values)[2], nullptr, 0)))
		{
			_roomsList[_roomIdSequence] = curr->getRoom();
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

bool TriviaServer::handleCloseRoom(RecievedMessage* m)
{
	User* user = m->getUser();
	if (user == nullptr) return false;
	Room* room = user->getRoom();
	if (room == nullptr) return false;
	int val = user->closeRoom();
	if (val == -1) return false;
	map<int, Room*>::iterator it = _roomsList.begin();
	for (it; it != _roomsList.end(); it++)
	{
		if (it->second == room) break;
	}
	_roomsList.erase(it);
	return true;
}

bool TriviaServer::handleJoinRoom(RecievedMessage* m)
{
	User* user = m->getUser();
	if (!user) return false;
	int roomID = stoi((*(m->getValues()))[0]);
	Room* room = getRoomById(roomID);
	if (!room)
	{
		Helper::sendData(m->getSock(), to_string(ROOM_JOIN_OTHER));
		return false;
	}
	user->joinRoom(room);
	return true;
}

bool TriviaServer::handleLeaveRoom(RecievedMessage* m)
{
	User* user = m->getUser();
	if (!user) return false;
	Room* room = user->getRoom();
	if (!room) return false;
	user->leaveRoom();
	user->send(to_string(ROOM_LEAVE_SUCCESS));
	return true;
}

void TriviaServer::handleGetRooms(RecievedMessage* m)
{
	stringstream message;
	message << to_string(AVAILABLE_ROOM_RESPONSE) << Helper::getPaddedNumber(_roomsList.size(), 4);
	map<int, Room*>::iterator it;
	for (it = _roomsList.begin(); it != _roomsList.end(); it++)
	{
		message << Helper::getPaddedNumber(it->first, 4) << Helper::getPaddedNumber((it->second->getName().size()), 2) << it->second->getName();
	}
	Helper::sendData(m->getSock(), message.str());
}

Room* TriviaServer::getRoomById(int id)
{
	std::map<int, Room*>::iterator it;
	it = _roomsList.find(id);
	if (it != _roomsList.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

User* TriviaServer::getUserBySocket(SOCKET s)
{
	std::map<SOCKET, User*>::iterator it;
	it = _connectedUsers.find(s);
	if (it != _connectedUsers.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void TriviaServer::handleGetUsersInRoom(RecievedMessage* m)
{
	User* user = m->getUser();
	int roomID = stoi((*(m->getValues()))[0]);
	Room* room = getRoomById(roomID);
	if (!room) Helper::sendData(m->getSock(), to_string(ROOM_USERS_RESPONSE) + "0");
	else Helper::sendData(m->getSock(), to_string(ROOM_USERS_RESPONSE) + room->getUsersListMessage());
}

void TriviaServer::handleLeaveGame(RecievedMessage* m)
{
	Game* g = m->getUser()->getGame();
	if (!m->getUser()->leaveGame())
	{
		delete g;
	}
	Helper::sendData(m->getSock(),to_string(GAME_LEAVE));
}

void TriviaServer::handleStartGame(RecievedMessage* m)
{

}

void TriviaServer::handlePlayerAnswer(RecievedMessage* m)
{
	Game* g = m->getUser()->getGame();
	if (g)
	{
		if (!(g->handleAnswerFromUser(m->getUser(), stoi((*(m->getValues()))[0]), stoi((*(m->getValues()))[1]))))
		{
			delete g;
		}
	}
}