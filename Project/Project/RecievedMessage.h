#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <map>
#include <string>

#include "User.h"
#include "DataBase.h"
#include "Question.h"

using namespace std;

class RecievedMessage{
  private:
    SOCKET _sock;
    User* _user;
    int _messageCode;
    vector<string> _values;
    
  public:
    RecievedMessage(SOCKET s,int code);
    RecievedMessage(SOCKET s, int code, vector<string> values);
    SOCKET getSock();
    User* getUser();
    void setUser(User* set);
    int getMessageCode();
    vector<string>* getValues();
};