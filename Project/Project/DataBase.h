#pragma once

#include <string>
#include <vector>
#include <exception>
#include "Question.h"
#include "sqlite3.h"

using namespace std;

class Question;
class DataBase
{
public:
	DataBase();
	~DataBase();
	bool isUserExists(string username);
	bool addNewUser(string username, string password, string email);
	bool isUserAndPassMatch(string username, string password);
	vector<Question*> initQuestion(int questionNo);
	vector<string> getBestScores();
	vector<string> getPersonalStatus(string username);
	int insertNewGame();
	bool updateGameStatus(int id);
	bool addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime);
private:
	sqlite3* _db;
	static int _lastValue;
	static vector<int> _ids;
	static string _questionData;
	static vector<string> _userNames;
	static int callbackCount(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackId(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackQuestions(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackBestScores(void* notUsed, int argc, char** argv, char** azCol);
	static int callbackPersonalStatus(void* notUsed, int argc, char** argv, char** azCol);
};