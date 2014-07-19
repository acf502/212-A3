/*
Name: Aaron Colin Foote
Date: 20th May 2013
User: acf502 / 4258770
File.: wrappers.h for CSCI212 Assignment 3
*/

#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

#include <unistd.h>     // unix system fns
#include <sys/types.h>  // a variety of typedefs
#include <sys/socket.h> // socket stuff
#include <netinet/in.h> // functions for converting
#include <netdb.h>      // dotted decimal strings
#include <arpa/inet.h> 	// to IP numbers
#include <signal.h>     // signal handling
#include <sys/signal.h> // signal handling
#include <errno.h>      // error reports
#include <cstdlib>      // things like exit()
#include <cctype>       // char stuff...
#include <cstring>      // cstring stuff...
#include <iostream>     // stream stuff...
#include <fstream>

using namespace std;

const int MAX_SIZE = 80;        // send/receive buffer size

enum StatusType{eInitGame,ePlayGame,eUserWin,eCompWin,eDraw};

struct GamePktType{
	char Game[3][3];
	int UserScore, CompScore;
	int MoveNo;
	StatusType Status;
} GamePkt;

struct Scores{
	char Names[3][20];
	int Scores[3];
	int Rank;
} ScorePkt;

const char cUser = 'X';
const char cComputer = 'O';

const char ServerWin[MAX_SIZE] = "     Y O U  L O S E !";
const char ClientWin[MAX_SIZE] = "      Y O U  W I N !";
const char Stalemate[MAX_SIZE] = "   G A M E  D R A W !";

//============= Wrapper Function Prototypes ===============

int Socket(int Domain, int Type, int Protocol);
void Bind(int Sockfd, sockaddr *MyAddr, int AddrLen);
void Listen(int Sockfd, int Backlog);
void Connect(int Sockfd, sockaddr *ServAddr, int Size);
int Accept(int Sockfd, sockaddr *Addr, int *AddrLen);
hostent *Gethostbyname(char *ServHost);

//Game functions

void InitGame(char Game[3][3]);
void DrawGame(char Game[3][3]);
void GetUserMove(char Game[3][3]);
void GetCompMove(char Game[3][3]);
int comp_find_win(char Game[3][3], char symbol);
int comp_middle(char Game[3][3]);
int comp_corner(char Game[3][3]);
int comp_side(char Game[3][3]);
bool GameWon(char Game[3][3], char TstCh); // returns T if row of TstCh found

#endif
