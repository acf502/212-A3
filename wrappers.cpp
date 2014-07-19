/*
Name: Aaron Colin Foote
Date: 20th May 2013
User: acf502 / 4258770
File.: wrappers.cpp for CSCI212 Assignment 3
*/
//  wrappers.cpp - Wrapper functions for CSCI212 socket programs


#include <sys/socket.h> // for socket(), bind(), and connect()
#include <arpa/inet.h>  // for sockaddr_in and inet_ntoa()
#include <string.h>     // for memset()
#include <stdio.h>      // for perror()
#include <stdlib.h>     // for exit()
#include <unistd.h>     // for close()
#include <errno.h>      // error reports
#include <netdb.h>	// dotted decimal strings

#include "wrappers.h"


//============= Wrapper Functions ========================

int Socket( int Domain, int Type, int Protocol )
{
	int Sockfd;
	if ((Sockfd = socket(Domain, Type, Protocol)) < 0)
	{
		perror( "socket creation" );
		exit(1);
	}
	return Sockfd;
}

void Bind( int Sockfd, sockaddr *MyAddr, int AddrLen )
{
	if (bind( Sockfd, MyAddr, AddrLen ) < 0)
	{
		perror( "socket bind" );
		exit(1);
	}
}

void Listen( int Sockfd, int Backlog )
{
	if (listen(Sockfd, Backlog ) < 0)
	{
		perror( "socket listen" );
		exit(1);
	}
}

void Connect(int Sockfd, sockaddr *ServAddr, int Size)
{
	if(connect(Sockfd, ServAddr, Size) < 0)
	{
		perror( "Can't connect to server!");
		exit(1);
	}
}

int Accept( int Sockfd, sockaddr *Addr, int *AddrLen )
{
	int Connfd;
	if ((Connfd = accept(Sockfd, Addr, AddrLen)) < 0)
	{
		if ((errno != EPROTO) && (errno != ECONNABORTED) && (errno != EINTR))
			perror( "socket accept" );
	}
	return Connfd;
}

struct hostent *Gethostbyname(char *ServHost)
{
	hostent *HostPtr;
	if((HostPtr = gethostbyname(ServHost)) == NULL)
	{
		perror( "gethostbyname error");
		exit(1);
	}
	return HostPtr;
}

//My own Function Definitions

void InitGame(char Game[3][3])
{
	for (int i = 0; i<3; i++)
	{
		for (int j = 0; j<3; j++)
		{
			Game[i][j] = ' ';
		}
	}
}

void DrawGame(char Game[3][3])	//Displays current game's board
{
	std::cout << "    1     2     3" << std::endl;
	cout << "       |     |" << endl;
	cout << "a   " << Game[0][0] << "  |  " << Game[0][1] << "  |  " << Game[0][2] << endl;
	cout << "   ----|-----|----" << endl;
	cout << "b   " << Game[1][0] << "  |  " << Game[1][1] << "  |  " << Game[1][2] << endl;
	cout << "   ----|-----|----" << endl;
	cout << "c   " << Game[2][0] << "  |  " << Game[2][1] << "  |  " << Game[2][2] << endl;
	cout << "       |     |" << endl;
	cout << endl << endl;
}

void GetUserMove(char Game[3][3])
{
	char row = ' ';
	int col = 0, rownum;	//row letter needs to be converted 
	bool repeat = true;
	while (repeat)
	{
		cout << "Enter move => ";	//get user move's row
		cin.get(row);
		cin >> col;	//get user's column
		cin.clear();
		cin.ignore(10000, '\n');
		if (((row != 'a' && row != 'b' && row != 'c') || (col != 1 && col != 2 && col != 3)))
		{
			repeat = true;
			cout << "Invalid move! Try again!" << endl;
		}
		else
			repeat = false;

		switch (row)	//convert letter(row) to number
		{
			case 'a': 
				rownum = 0;
				break;
			case 'b':
				rownum = 1;
				break;
			case 'c':
				rownum = 2;
				break;
			default:
				rownum = 0;
				break;
		};

		if (Game[rownum][(col-1)] != ' ')	//finally check if space is valid, if not then repeat user input
		{
			repeat = true;
			cout << "Invalid move! Try again!" << endl;
		}
		else
			repeat = false;
	}
	
	Game[rownum][(col-1)] = cUser;
}

void GetCompMove(char Game[3][3])
{
	int selection = 0, row = 0, col = 0;	//initialise to 0

  // Use first strategy rule that returns valid selection
	selection = comp_find_win(Game, cComputer);	//search for immediate win
	if (selection == 0)
		selection = comp_find_win(Game, cUser);	//search to block opponent
	if (selection == 0)
		selection = comp_middle(Game);	//identify if middle square is available
	if (selection == 0)
		selection = comp_corner(Game);	//identify if any corner is available
	if (selection == 0)
		selection = comp_side(Game);	//identify free side square that's available

	row = (selection - 1) / 3;	//square 7, 4, 1 = row 3, 2, 1 (6/3 = 2, 3/3 = 1, 0/3 = 0) etc...
	col = (selection - 1) % 3;	//square 3, 2, 1 = column 3, 2, 1 (2%3 = 2, 1%3 = 1, 0%3 = 0) etc...

	Game[row][col] = cComputer;	//computer takes optimised move

}

int comp_find_win(char Game[3][3], char symbol)	//comp looks for immediate win, block if not possible
{
	int selection, row, col, result = 0;

	for (selection = 1; selection <= 9; selection++)
	{
		row = (selection - 1) / 3;	//square 7, 4, 1 = row 3, 2, 1 (6/3 = 2, 3/3 = 1, 0/3 = 0) etc...
		col = (selection - 1) % 3;	//square 3, 2, 1 = column 3, 2, 1 (2%3 = 2, 1%3 = 1, 0%3 = 0) etc...

		if (Game[row][col] == ' ')	//if square is vacant
		{
			Game[row][col] = symbol;	//place symbol in square
			if (GameWon(Game, symbol))	//check for win
				result = selection;		//set return value of where winning move is available
			Game[row][col] = ' ';		//return game board to normal state
		}
	}
	
	return result;	//return winning move (if comp), return where to block opponent (if testing for comp to block user)
}

int comp_middle(char Game[3][3])	//if the middle is vacant, take middle
{
	if (Game[1][1] == ' ')
		return 5;
	else
		return 0;
}

int comp_corner(char Game[3][3])	//test for individual corners to take
{
	if (Game[0][0] == ' ')
		return 1;
	if (Game[0][2] == ' ')
		return 3;
	if (Game[2][0] == ' ')
		return 7;
	if (Game[2][2] == ' ')
		return 9;

	return 0;
}

int comp_side(char Game[3][3])		//find any remaining sides to take
{
	if (Game[0][1] == ' ')
		return 2;
	if (Game[1][0] == ' ')
		return 4;
	if (Game[1][2] == ' ')
		return 6;
	if (Game[2][1] == ' ')
		return 8;

	return 0;
}

bool GameWon(char Game[3][3], char TstCh)	//checks for all possible win arrangements
{
	int row, col;


	// returns true if row of TstCh found

	for (row = 0; row < 3; row++)	//checks rows match
	{
		if ((Game[row][0] == TstCh) && (Game[row][1] == TstCh) && (Game[row][2] == TstCh))
			return true;
	}

	for (col = 0; col < 3; col++)	//checks columns match
	{
		if ((Game[0][col] == TstCh) && (Game[1][col] == TstCh) && (Game[2][col] == TstCh))
			return true;
	}

	if ((Game[0][0] == TstCh) && (Game[1][1] == TstCh) && (Game[2][2] == TstCh))	//test each diagonal
		return true;

	if ((Game[0][2] == TstCh) && (Game[1][1] == TstCh) && (Game[2][0] == TstCh))
		return true;

	return false;
}
