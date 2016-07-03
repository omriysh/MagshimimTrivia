#include <sstream>
#include "Game.h"
#include "Protocol.h"

Game::Game(const vector <User*> & players, int queNo, DataBase& db) : _players(players), _questions_no(queNo - 1), _db(&db), _currentTurnAnswers(0)
{
	//add this to db and get questions from db
	insertGameToDB();
	initQuestionsFromDB();
	vector<User*>::iterator it = _players.begin();
	for (it; it != _players.end(); it++) //set player's game pointer to this
	{
		(**it).setGame(this);
		_results.insert(pair<string, int>((**it).getUsername(), 0));
	}
}

Game::~Game()
{
	//delete all questions (that are created in DataBase)
	vector<Question*>::iterator it = _questions.begin();
	for (it; it != _questions.end(); it++)
	{
		delete *it;
	}
}

void Game::sendFirstQuestion()
{
	sendQuestionToAllUsers();
}

void Game::handleFinishGame()
{
	stringstream s;
	vector<User*>::iterator it = _players.begin();
	for (it = _players.begin(); it != _players.end(); it++) //create formatted message to all players
	{
		s << Helper::getPaddedNumber((**it).getUsername().length(), 2) << (**it).getUsername() << Helper::getPaddedNumber(_results[(**it).getUsername()], 2);
	}
	for (it = _players.begin(); it != _players.end(); it++) //send message to all players
	{
		try
		{
			(**it).send(to_string(GAME_END) + Helper::getPaddedNumber(_players.size(), 1) + s.str());
		}
		catch (...) {}
		(**it).setGame(nullptr);
	}
	_db->updateGameStatus(getID()); //end game in db
}

bool Game::handleNextTurn()
{
	if (_players.size() == 0) //if there are no players, the game is over
	{
		handleFinishGame();
		return false;
	}
	if (_currentTurnAnswers == _players.size()) //if all players have answered
	{
		if (_questions_no == 0) //if there are no more questions the game is over
		{
			handleFinishGame();
			return false;
		}
		//if there are more questions move on to the next one
		_questions_no--;
		_currentTurnAnswers = 0;
		sendQuestionToAllUsers();
	}
	return true;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	_currentTurnAnswers++;
	if (answerNo == _questions[_questions_no]->getCorrectAnswerIndex() + 1) //if the answer is correct, add score and update the user
	{
		_results[user->getUsername()]++;
		_db->addAnswerToPlayer(getID(), user->getUsername(), _questions[_questions_no]->getId(), _questions[_questions_no]->getAnswer()[answerNo - 1], true, time);
		user->send(to_string(ANSWER_INDICATION) + "1");
		return handleNextTurn();
	}
	//if the answer is incorrect update the player
	_db->addAnswerToPlayer(getID(), user->getUsername(), _questions[_questions_no]->getId(), _questions[_questions_no]->getAnswer()[answerNo - 1], false, time);
	user->send(to_string(ANSWER_INDICATION) + "0");
	return handleNextTurn();
}

bool Game::leaveGame(User* u)
{
	//delete player and handle next turn
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
	return _id;
}

bool Game::insertGameToDB()
{
	return _id = _db->insertNewGame(); //store the id returned from the database and return it
}


void Game::initQuestionsFromDB()
{
	//get the questions from the database into a question pointer array
	vector<Question*> temp = _db->initQuestion(_questions_no + 1);
	vector<Question*>::iterator it = temp.begin();
	for (it; it != temp.end(); it++)
	{
		_questions.push_back(*it);
	}
}

void Game::sendQuestionToAllUsers()
{
	//get the latest question and format the message
	string question = _questions[_questions_no]->getQuestion();
	string* answers = _questions[_questions_no]->getAnswer();
	stringstream s;
	s << to_string(QUESTION) << Helper::getPaddedNumber(question.length(), 3) << question;
	for (int i = 0; i < 4; i++)
	{
		s << Helper::getPaddedNumber(answers[i].length(), 3) << answers[i];
	}
	//send to every player
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