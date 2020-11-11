#pragma once
#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <iterator>
#include <cmath>
#include <ctime>
#include <iomanip>
using namespace std;

class Player
{
private:
	pair<int, int> hands;
public:
	Player()
	{
		hands = make_pair(1, 1);
	}
	void reset()
	{
		hands = make_pair(1, 1);
	}
	bool isAlive()
	{
		return (hands.first || hands.second);
	}
	int getHandCount(int hand)
	{
		if (hand == 1)
			return hands.first;
		else if (hand == 2)
			return hands.second;
		else
			return 0;
	}
	void addFingers(int fingers, int hand)
	{
		if (hand == 1)
			hands.first = (hands.first + fingers) % 5;
		else
			hands.second = (hands.second + fingers) % 5;
	}
	bool isDivide(int hand, int fingers)
	{
		int hand1, hand2;
		if (hand == 1)
		{
			hand1 = hands.first - fingers;
			hand2 = (hands.second + fingers) % 5;
		}
		else
		{
			hand1 = (hands.first + fingers) % 5;
			hand2 = hands.second - fingers;
		}
		if (hand1 == 0 || hand2 == 0)
			return false;
		else if (hands.first == hand2 && hands.second == hand1)
			return false;
		else
			return true;

	}
	void divideFingers(int hand, int fingers)
	{
		if (hand == 1)
		{
			hands.first = hands.first - fingers;
			hands.second = (hands.second + fingers) % 5;
		}
		else
		{
			hands.first = (hands.first + fingers) % 5;
			hands.second = hands.second - fingers;
		}
	}

};

struct State
{
	vector<Player> players;
};

struct Move
{
	string mType;
	vector<int> args;
};

struct Score
{
	vector<int> scores;
};

bool checkValidMove(State s, Move m, int curr)
{
	if (m.mType == "T")
	{
		int pTo = m.args[0], hFrom = m.args[1], hTo = m.args[2];
		if (s.players[pTo].isAlive() && s.players[curr].getHandCount(hFrom) && s.players[pTo].getHandCount(hTo))
			return true;
		else
			return false;
	}
	else if (m.mType == "D")
	{
		int hFrom = m.args[0], fingers = m.args[1];
		if (s.players[curr].getHandCount(hFrom) < fingers)
			return false;
		else
			return s.players[curr].isDivide(hFrom, fingers);
	}
	else
		return false;
}

State performMove(State s, Move m, int curr)
{
	State _s;
	_s.players.assign(s.players.begin(), s.players.end());
	if(m.mType == "T")
	{
		int pTo = m.args[0], hFrom = m.args[1], hTo = m.args[2];
		int fingers = _s.players[curr].getHandCount(hFrom);
		_s.players[pTo].addFingers(fingers, hTo);
	}
	else
	{
		int hFrom = m.args[0], fingers = m.args[1];
		_s.players[curr].divideFingers(hFrom, fingers);
	}
	return _s;
}

vector<pair<Move, State>> getSuccessor(State s, int curr)
{
	vector<pair<Move, State>> successors;
	int total = (int)s.players.size();
	for(int pTo = 0; pTo < total; ++pTo)
		for(int hFrom = 1; hFrom <= 2; ++hFrom)
			for (int hTo = 1; hTo <= 2; ++hTo)
			{
				if (pTo != curr)
				{
					Move m;
					m.mType = "T";
					m.args.push_back(pTo);
					m.args.push_back(hFrom);
					m.args.push_back(hTo);
					if (checkValidMove(s, m, curr))
					{
						State _s = performMove(s, m, curr);
						successors.push_back(make_pair(m, _s));
					}
				}
			}
	for(int hFrom = 1; hFrom <= 2; ++hFrom)
		for (int fingers = 1; fingers <= 3; ++fingers)
		{
			Move m; 
			m.mType = "D";
			m.args.push_back(hFrom);
			m.args.push_back(fingers);
			if (checkValidMove(s, m, curr))
			{
				State _s = performMove(s, m, curr);
				successors.push_back(make_pair(m, _s));
			}
		}
	return successors;
}

bool isEnd(State s)
{
	int alive_count = 0;
	for (auto it : s.players)
		if (it.isAlive()) ++alive_count;	
	return alive_count <= 1;
}

Score getScores(State s)
{
	Score scr;
	if (isEnd(s))
	{
		for (auto it : s.players)
			if (it.isAlive())
				scr.scores.push_back(10);
			else
				scr.scores.push_back(0);
	}
	else
	{
		for (auto it : s.players)
			scr.scores.push_back(it.getHandCount(1) + it.getHandCount(2));
	}
	return scr;
}

Move getNull()
{
	Move null_move;
	null_move.mType = "";
	return null_move;
}

map<vector<int>, int> visited;
map<vector<int>, pair<Move,Score>> strategy;

pair<Move, Score> maxN(State s, int curr, int depth)
{
	if (s.players[curr].isAlive() == false)
	{
		return maxN(s, (curr + 1) % (int(s.players.size())), depth);
	}
	if (depth == 8)
	{
		Score evaluated_scores = getScores(s);
		Move null_move = getNull();
		return make_pair(null_move, evaluated_scores);
	}
	auto u = s.players;
	vector<int> curr_state;
	for (auto it : u)
	{
		curr_state.push_back(it.getHandCount(1));
		curr_state.push_back(it.getHandCount(2));
	}
	curr_state.push_back(curr);
	if (strategy.find(curr_state) != strategy.end())
	{
		return strategy[curr_state];
	}
	if (visited.find(curr_state) != visited.end())
	{
		Score evaluated_scores = getScores(s);
		Move null_move = getNull();
		return make_pair(null_move, evaluated_scores);
	}
	visited[curr_state] = 1;
	auto successors = getSuccessor(s, curr);
	pair<Move, Score> res;
	int mx_score = -1, total = (int)s.players.size();
	for (auto it : successors)
	{
		Score curr_score;
		if (isEnd(it.second) == true)
			curr_score = getScores(it.second);
		else
		{
			auto v = maxN(it.second, (curr + 1) % total, depth + 1);
			curr_score = v.second;
		}
		if (curr_score.scores[curr] > mx_score)
		{
			mx_score = curr_score.scores[curr];
			res = make_pair(it.first, curr_score);
		}
	}
	return res;
}

class Game
{
private:
	vector<Player> players;
	static int curr, total, alive, hooman;
public:
	Game()
	{
		players.resize(2);
		curr = 0;
		hooman = 0;
		total = 2;
		alive = 2;
		visited.clear();
		strategy.clear();
	}
	Game(int n)
	{
		players.resize(n);
		curr = 0;
		hooman = 0;
		total = n;
		alive = n;
		visited.clear();
		strategy.clear();
	}
	void reset()
	{

	}
	void makeMove()
	{
		if (players[curr].isAlive() == false)
		{
			while (players[curr].isAlive() == false) curr = (curr + 1) % total;
		}
		if (alive == 1)
			return;
		else if(curr == hooman)
		{
			bool isValid = false;
			while (isValid == false)
			{
				string mType;
				cout << "Player " << curr << ": Choose your move (T/D)... ";
				cin >> mType;
				if (mType == "T")
				{
					int pTo, handFrom, handTo;
					cout << "Enter the player you want to transfer to... ";
					cin >> pTo;
					if (players[pTo].isAlive() == false)
						cout << "The selected player is invalid!\n";
					else
					{
						cout << "Enter your hand you want to transfer from... ";
						cin >> handFrom;
						if (players[curr].getHandCount(handFrom) == 0)
							cout << "Selected hand is invalid!\n";
						else
						{
							cout << "Enter the hand you wish to transfer to... ";
							cin >> handTo;
							if (players[pTo].getHandCount(handTo) == 0)
								cout << "Cannot transfer to invalid hand\n";
							else
							{
								int fingers = players[curr].getHandCount(handFrom);
								players[pTo].addFingers(fingers, handTo);
								cout << "Player " << curr << " transferred " << fingers << " to Player " << pTo << "\n";
								isValid = true;
							}
						}
					}
				}
				else if (mType == "D")
				{
					int handFrom, fingers;
					cout << "Enter your hand you want to split from... ";
					cin >> handFrom;
					if (players[curr].getHandCount(handFrom) == 0)
						cout << "Selected hand is invalid!\n";
					else
					{
						cout << "Enter the number of fingers you want to transfer... ";
						cin >> fingers;
						if (players[curr].getHandCount(handFrom) < fingers)
							cout << "Not enough fingers\n";
						else if (players[curr].isDivide(handFrom, fingers) == false)
							cout << "Invalid move\n";
						else
						{
							players[curr].divideFingers(handFrom, fingers);
							cout << "Player " << curr << " split fingers to "
								<< "(" << players[curr].getHandCount(1) << "," << players[curr].getHandCount(2) << ")\n";
							isValid = true;
						}
					}
				}
				if (isValid == false)
					cout << "Please choose a valid move!\n";
			}
		}
		else
		{
			visited.clear();
			State s, f;
			auto u = s.players;
			vector<int> curr_state;
			for (auto it : u)
			{
				curr_state.push_back(it.getHandCount(1));
				curr_state.push_back(it.getHandCount(2));
			}
			curr_state.push_back(curr);
			s.players.assign(players.begin(), players.end());
			clock_t time_req;
			time_req = clock();
			auto v = maxN(s, curr, 0);
			time_req = clock() - time_req;
			setprecision(9);
			cout << v.first.mType << " " << v.first.args[0] << " " << (double)time_req / CLOCKS_PER_SEC;
			f = performMove(s, v.first, curr);
			for (int i = 0; i < total; ++i)
				players[i] = f.players[i];
		}
		curr = (curr + 1) % total;
	}
	bool checkStatus()
	{
		int aliveCount = 0;
		for (int i = 0; i < total; ++i)
		{
			int hand1 = players[i].getHandCount(1);
			int hand2 = players[i].getHandCount(2);
			if (hand1 || hand2)
				aliveCount++;
		}
		alive = aliveCount;
		if (alive == 1)
			return true;
		return false;
	}
	void showStatus()
	{
		cout << "\n\nCurrent Status:\n"
			<< "***********************\n";
		for (int i = 0; i < total; ++i)
		{
			int hand1 = players[i].getHandCount(1);
			int hand2 = players[i].getHandCount(2);
			if (hand1 || hand2)
				cout << "Player " << i << " is at (" << hand1 << "," << hand2 << ") \n";
			else
				cout << "Player " << i << " lost both his hands\n";
		}
		cout << "************************\n\n";
	}
	int getVictor()
	{
		for (int i = 0; i < total; ++i)
			if (players[i].isAlive())
				return i;
		return -1;
	}
	void playGame()
	{
		strategy.clear();
		while (checkStatus() == false)
		{
			makeMove();
			showStatus();
		}
		cout << "The winner is " << getVictor();
	}
};
int Game::curr = 0;
int Game::total = 0;
int Game::alive = 0;
int Game::hooman = 0;