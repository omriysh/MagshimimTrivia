#pragma once

#include <vector>
#include <map>
#include <string>
#include "User.h"
#include "DataBase.h"

using namespace std;

class Question{
  private:
    string _question;
    string _answers[4];
    int _correctAnswerIndex;
    int _id;
    
  public:
    Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4);
    string getQuestion();
    string* getAnswer();
    int getCorrectAnswerIndex();
    int getId();
};