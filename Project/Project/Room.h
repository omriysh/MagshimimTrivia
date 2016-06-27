#pragma once

#include <string>
#include <vector>
#include "User.h"
#include "Helper.h"

using namespace std;
class User;
class Room
{
    public:
        Room(int id, User* admin,string name, int maxUsers, int questionTime, int questionNo);
        bool joinRoom(User* user);
        void leaveRoom(User* user);
        int closeRoom(User* user);
        vector<User*> getUsers();
        string getUsersListMessage();
        int getQuestionsNo();
        int getId();
        string getName();
    private:
        vector<User*> _users;
        User* _admin;
        int _maxUsers;
        int _questionTime;
        int _questionsNo;
        string _name;
        int _id;
        
        string getUsersAsString(vector<User*> users, User* user);
        void sendMessage(string message);
        void sendMessage(User* user, string message);
};