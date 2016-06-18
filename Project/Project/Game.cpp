#include <sstream>
#include "Game.h"
#include "Protocol.h"

Game::Game(const vector <User*> & players, int queNo, DataBase& db) : _players(players), _questions_no(queNo)/*, _db(db)*/
{
	//DATABASE STUFF
	vector<User*>::iterator it = _players.begin();
	for (it; it != _players.end(); it++)
	{
		(**it).setGame(this);
		_results.insert(pair<string, int>((**it).getUsername(), 0));
	}
}

Game::~Game()
{
	vector<Question*>::iterator it = _questions.begin();
	for (it; it != _questions.end(); it++)
	{
		delete *it;
		_questions.erase(it);
	}
	vector<User*>::iterator it2 = _players.begin();
	for (it2; it2 != _players.end(); it2++)
	{
		delete *it2;
		_players.erase(it2);
	}
}

void Game::sendFirstQuestion()
{
	sendQuestionToAllUsers();
}

void Game::handleFinishGame()
{
	//DATABASE STUFF
	stringstream s;
	vector<User*>::iterator it = _players.begin();
	for (it = _players.begin(); it != _players.end(); it++)
	{
		s << Helper::getPaddedNumber((**it).getUsername().length(), 2) << (**it).getUsername() << Helper::getPaddedNumber(_results[(**it).getUsername()], 2);
	}
	for (it = _players.begin(); it != _players.end(); it++)
	{
		try
		{
			(**it).send(to_string(GAME_END) + Helper::getPaddedNumber(_players.size(), 1) + s.str());
		}
		catch (...) {}
		(**it).setGame(nullptr);
	}
}

bool Game::handleNextTurn()
{
	if (_players.size() == 0)
	{
		handleFinishGame();
		return false;
	}
	if (_currentTurnAnswers == _players.size())
	{
		if (_questions_no == _questions.size())
		{
			handleFinishGame();
			return false;
		}
		_questions_no++;
		sendQuestionToAllUsers();
	}
	return true;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	_currentTurnAnswers++;
	if (answerNo == _questions[_questions_no]->getCorrectAnswerIndex())
	{
		_results[user->getUsername()]++;
		//DATABASE STUFF
		user->send(to_string(ANSWER_INDICATION) + "1");
		return handleNextTurn();
	}
	//DATABASE STUFF
	user->send(to_string(ANSWER_INDICATION) + "0");
	return handleNextTurn();
}

bool Game::leaveGame(User* u)
{
	vector<User*>::iterator it = _players.begin();
	for (it; it != _players.end(); it++)
	{
		if (u == *it)
		{
			_players.erase(it);
			return handleNextTurn();
		}
	}
	return false;
}

int Game::getID()
{
	//DATABASE STUFF
	return 1;
}

bool Game::insertGameToDB()
{
	//DATABASE STUFF
	return true;
}

void Game::initQuestionsFromDB()
{
	//DATABASE STUFF
}

void Game::sendQuestionToAllUsers()
{
	string question = _questions[_questions_no]->getQuestion();
	string* answers = _questions[_questions_no]->getAnswer();
	stringstream s;
	s << to_string(QUESTION) << Helper::getPaddedNumber(question.length(), 3) << question;
	for (int i = 0; i < 4; i++)
	{
		s << Helper::getPaddedNumber(answers[i].length(), 3) << answers[i];
	}
	vector<User*>::iterator it = _players.begin();
	for (it; it != _players.end(); it++)
	{
		try
		{
			(**it).send(s.str());
		}
		catch (...) {}
	}
}