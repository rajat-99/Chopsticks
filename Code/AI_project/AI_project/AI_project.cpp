// AI_project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "game.h"
#include <conio.h>

int main()
{
	cout << "Welcome to the Game of Chopsticks, designed and programmed by Rajat Sharma (B18CSE043).\n"
		<< "The explicit rules for the game can be found listed in the report under section 1.1\n"
		<< "The rules of gameplay are as follows:\n"
		<< "1. In each move you will be asked to either play a transfer move or a divide move. The transfer move is\n"
		<< "basically the attack move, while the divide move is the split move.\n"
		<< "2. If you choose the attack move, you will be asked the player you wish to attack, the hand you wish to\n"
		<< "attack with, and the opponent's hand you wish to attack.\n"
		<< "3. If you choose the split move, you will be asked the hand you wish to split from, and the number of\n"
		<< "fingers you wish to transfer.\n"
		<< "***********************************************************************************************************\n";
	getchar();
	char choice = 'y';
	while (choice == 'y')
	{
		int n;
		cout << "Enter the number of players you wish to play with.... "; cin >> n;
		Game g(n);
		g.playGame();
		cout << "Do you wish to play again(y/n)?... "; cin >> choice;
	}
	getchar();
	getchar();
    return 0;
}
