/*#pragma once

#include <vector>
#include <map>
#include <string>
#include "User.h"
#include "DataBase.h"
#include "Question.h"

using namespace std;

class User;
class Question;
class DataBase;
class Game{
  private:
    vector<Question*> _questions;
    vector<User*> _players;
    int _questions_no;
    DataBase* _db;
    map<string, int> _results;
    int _currentTurnAnswers;
    
    bool insertGameToDB();
    void initQuestionsFromDB();
    void sendQuestionToAllUsers();
  
  public:
    Game(const vector <User*> & players, int queNo, DataBase& db);
    ~Game();
    void sendFirstQuestion();
    void handleFinishGame();
    void handleNextTurn();
    bool handleAnswerFromUser(User* u, int curr, int no);
    bool leaveGame(User* u);
    int getID();
};*/