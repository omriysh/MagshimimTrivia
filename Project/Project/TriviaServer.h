#pragma once
#include <string>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <sstream>
#include "DataBase.h"
#include "RecievedMessage.h"
#include "Helper.h"
#include "Protocol.h"
#include <condition_variable>
using namespace std;


class TriviaServer {
public:
	TriviaServer();
	~TriviaServer();
	void server();

private:
	SOCKET _socket;
	map<SOCKET, User*> _connectedUsers;
	//DataBase _db;
	map<int, Room*> _roomsList;

	map<string, string> _dbUsers;

	std::condition_variable _cond;
	mutex _mtxRecievedMessages;
	queue<RecievedMessage*> _queRcvMessages;

	static int _roomIdSequence;

	void bindAndListen();
	void accept();
	void clientHandler(SOCKET s);
	void safeDeleteUser(RecievedMessage* m);

	User* handleSignin(RecievedMessage* m);
	bool handleSignup(RecievedMessage* m);
	void handleSignout(RecievedMessage* m);

	void handleLeaveGame(RecievedMessage* m);
	void handleStartGame(RecievedMessage* m);
	void handlePlayerAnswer(RecievedMessage* m);

	bool handleCreateRoom(RecievedMessage* m);
	bool handleCloseRoom(RecievedMessage* m);
	bool handleJoinRoom(RecievedMessage* m);
	bool handleLeaveRoom(RecievedMessage* m);
	void handleGetUsersInRoom(RecievedMessage* m);
	void handleGetRooms(RecievedMessage* m);

	void handleGetBestScores(RecievedMessage* m);
	void handleGetPersomaStatus(RecievedMessage* m);

	void handleRecievedMessages();
	void addRecievedMessage(RecievedMessage* m);
	RecievedMessage* buildRecieveMessage(SOCKET s, int code);

	User* getUserByName(string str);
	User* getUserBySocket(SOCKET s);
	Room* getRoomById(int id);
};

