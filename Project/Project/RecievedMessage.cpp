#include "RecievedMessage.h"

RecievedMessage::RecievedMessage(SOCKET s, int code) : _sock(s), _messageCode(code) {}
RecievedMessage::RecievedMessage(SOCKET s, int code, vector<string> values) : _sock(s), _messageCode(code), _values(values) {}

SOCKET RecievedMessage::getSock()
{
    return _sock;
}

User* RecievedMessage::getUser()
{
    return _user;
}

void RecievedMessage::setUser(User* set)
{
    _user = set;
}

int RecievedMessage::getMessageCode()
{
    return _messageCode;
}

vector<string>* RecievedMessage::getValues()
{
    return &_values;
}