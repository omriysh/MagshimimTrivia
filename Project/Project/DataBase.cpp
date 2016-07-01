#include <sstream>
#include "DataBase.h"

int DataBase::_lastValue = 0;
vector<int> DataBase::_ids;
string DataBase::_questionData = "";
vector<string> DataBase::_userNames;

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
	vector<Question*> ans;
	char* zErrMsg = 0;
	string sheilta = "select question_id from t_questions order by random() limit " + to_string(questionNo) + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackId, 0, &zErrMsg);
	if (rc != SQLITE_OK) return ans;
	for (int i = 0; i < _ids.size(); i++)
	{
		sheilta = "select question from t_questions where question_id=" + to_string(_ids[i]) + ";";
		int rc = sqlite3_exec(_db, sheilta.c_str(), callbackQuestions, 0, &zErrMsg);
		if (rc != SQLITE_OK) return ans;
		string question = _questionData;
		sheilta = "select correct_ans from t_questions where question_id=" + to_string(_ids[i]) + ";";
		int rc = sqlite3_exec(_db, sheilta.c_str(), callbackQuestions, 0, &zErrMsg);
		if (rc != SQLITE_OK) return ans;
		string correctAns = _questionData;
		vector<string> answers;
		for (int j = 2; j <= 4; j++)
		{
			sheilta = "select ans" + to_string(j) + " from t_questions where question_id=" + to_string(_ids[i]) + ";";
			int rc = sqlite3_exec(_db, sheilta.c_str(), callbackQuestions, 0, &zErrMsg);
			if (rc != SQLITE_OK) return ans;
			answers.push_back(_questionData);
		}
		Question* q = new Question(_ids[i], question, correctAns, answers[0], answers[1], answers[2]);
		ans.push_back(q);
	}
	return ans;
}

vector<string> DataBase::getBestScores()
{
	//return 3 scores or empty strings
	vector<string> ans;
	char* zErrMsg = 0;
	string sheilta = "select username from t_users;";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackBestScores, 0, &zErrMsg);
	if (rc != SQLITE_OK) return ans;
	map<string, int> scores;
	for (int i = 0; i < _userNames.size(); i--)
	{
		vector<string> status = getPersonalStatus(_userNames[i]);
		scores.insert(pair<string, int>(_userNames[i], stoi(status[1])));
	}
	for (int i = 0; i < 3; i++)
	{
		map<string, int>::iterator it = scores.begin();
		if (it != scores.end())
		{
			pair<string, int> max = *it;
			for (it++; it != scores.end(); it++)
			{
				if (it->second > max.second) max = *it;
			}
			ans.push_back(Helper::getPaddedNumber(max.first.size(), 2) + max.first + Helper::getPaddedNumber(max.second, 6));
		}
		else
		{
			ans.push_back("00");
		}
	}
	return ans;
}

vector<string> DataBase::getPersonalStatus(string username)
{
	//number of games (4), number of right answers (6)
	//number of wrong answers (6), average answer time (4, 2 whole 2 fraction)
	//return padded
	vector<string> ans;
	char* zErrMsg = 0;
	string sheilta = "select count(distinct game_id) from t_players_answers where username=" + username + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return ans;
	ans.push_back(Helper::getPaddedNumber(_lastValue, 4));
	string sheilta = "select count(*) from t_players_answers where is_correct=1 and username=" + username + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return ans;
	ans.push_back(Helper::getPaddedNumber(_lastValue, 6));
	string sheilta = "select count(*) from t_players_answers where is_correct=0 and username=" + username + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return ans;
	ans.push_back(Helper::getPaddedNumber(_lastValue, 6));
	string sheilta = "select answer_time from t_players_value where username=" + username + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackPersonalStatus, 0, &zErrMsg);
	if (rc != SQLITE_OK) return ans;
	ans.push_back(Helper::getPaddedNumber(_lastValue, 4));
	return ans;
}

int DataBase::insertNewGame()
{
	char* zErrMsg = 0;
	string sheilta = "insert into t_games(status, start_time) valeus(0, now);";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return -1;
	string sheilta = "select count(*) from t_games;";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return -1;
	return _lastValue;
}

bool DataBase::updateGameStatus(int id)
{
	char* zErrMsg = 0;
	string sheilta = "update t_games set status=1 and start_time=now where game_id=" + to_string(id) + ";";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return false;
	return true;
}

bool DataBase::addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime)
{
	char* zErrMsg = 0;
	string sheilta = "isert into t_players_answers(game_id, username, question_id, player_answer, is_correct, answer_time) values(" + to_string(gameId) + ", " + username + ", " + to_string(questionId) + ", " + answer + ", " + (isCorrect ? "true" : "false") + ", " + to_string(answerTime) + ");";
	int rc = sqlite3_exec(_db, sheilta.c_str(), callbackCount, 0, &zErrMsg);
	if (rc != SQLITE_OK) return false;
	return true;
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

int DataBase::callbackId(void* notUsed, int argc, char** argv, char** azCol)
{
	while (_ids.size()) _ids.erase(_ids.end());
	for (int i = 0; i < argc; i++)
	{
		int id;
		stringstream ss(argv[i]);
		ss >> id;
		_ids.push_back(id);
	}
	return 0;
}

int DataBase::callbackQuestions(void* notUsed, int argc, char** argv, char** azCol)
{
	_questionData = "";
	for (int i = 0; i < argc; i++)
	{
		_questionData = argv[i];
	}
	return 0;
}

int DataBase::callbackBestScores(void* notUsed, int argc, char** argv, char** azCol)
{
	while (_userNames.size()) _userNames.erase(_userNames.end());
	for (int i = 0; i < argc; i++)
	{
		_userNames.push_back(argv[i]);
	}
	return 0;
}

int DataBase::callbackPersonalStatus(void* notUsed, int argc, char** argv, char** azCol)
{
	_lastValue = 0;
	float sum = 0;
	for (int i = 0; i < argc; i++)
	{
		stringstream ss(argv[i]);
		ss >> sum;
	}
	sum /= argc;
	_lastValue = (int)sum * 100 + sum - (int)sum;
}