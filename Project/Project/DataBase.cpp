#include <sstream>
#include "DataBase.h"

int DataBase::_lastValue = 0;

DataBase::DataBase() throw(string)
{
	int rc = sqlite3_open("TriviaDB.db", &_db);
	if (rc)
	{
		string error = sqlite3_errmsg(_db);
		sqlite3_close(_db);
		throw error;
	}
}

DataBase::~DataBase()
{
	sqlite3_close(_db);
}

bool DataBase::isUserExists(string username)
{
	char* zErrMsg = 0;
	string sheilta = "select count(*) from t_users where username=" + username + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return false;
	if (_lastValue) return true;
	return false;
}

bool DataBase::addNewUser(string username, string password, string email)
{
	char* zErrMsg = 0;
	string sheilta = "inser into t_users(username, password, email) values(" + username + ", " + password + ", " + email + ");";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return false;
	return true;
}

bool DataBase::isUserAndPassMatch(string username, string password)
{
	char* zErrMsg = 0;
	string sheilta = "select count(*) from t_users where username=" + username + " and password=" + password + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return false;
	if (_lastValue) return true;
	return false;
}

vector<Question*> DataBase::initQuestion(int questionNo)
{

}

vector<string> DataBase::getBestScores()
{

}

vector<string> DataBase::getPersonalStatus(string username)
{

}

int DataBase::insertNewGame()
{

}

bool DataBase::updateGameStatus(int id)
{

}

bool DataBase::addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime)
{

}



int DataBase::callbackCount(void* notUsed, int argc, char** argv, char** azCol)
{
	_lastValue = 0;
	for (int i = 0; i < argc; i++)
	{
		stringstream ss(argv[i]);
		ss >> _lastValue;
	}
	return 0;
}

int DataBase::callbackQuestions(void* notUsed, int argc, char** argv, char** azCol)
{

}
int DataBase::callbackBestScores(void* notUsed, int argc, char** argv, char** azCol)
{

}

int DataBase::callbackPersonalStatus(void* notUsed, int argc, char** argv, char** azCol)
{

}