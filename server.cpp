/*
Name: Aaron Colin Foote
Date: 20th May 2013
User: acf502 / 4258770
File.: server.cpp for CSCI212 Assignment 3
*/

// Example iterative TCP server - CSCI212
#include "wrappers.h"

void filecreator();	//can be switched on by uncommenting all instances. Used to load high score data in scores.dat 
bool fileexist(const char *file);

int main(int argc, char **argv)
{
	int SERV_TCP_PORT;
	if (argc == 1)
	{
		SERV_TCP_PORT = 48770;
	}
	else if (argc == 2)
	{
		string str1 = argv[1];
		string str2;
		for (int i = 0; i < str1.length(); i++)
		{
			str2[i] = str1[(i+1)];
		}
		SERV_TCP_PORT = atoi(str2.c_str());
	}
	else
	{
		cout << "Incorrect usage: ./server -host" << endl;
		exit(1);
	}
		
	int Sockfd, NewSockfd, ClntLen;
	sockaddr_in ClntAddr, ServAddr;
	int Port = SERV_TCP_PORT;
	bool Buff = false;
	int Len;
	bool fcreate = fileexist("scores.dat");
	if (!fcreate)
		filecreator();
	
  // Open a TCP socket (an Internet stream socket)
	Sockfd = Socket(AF_INET, SOCK_STREAM, 0); // socket() wrapper fn

  // Bind the local address, so that the client can send to server
	memset((char*)&ServAddr, 0, sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ServAddr.sin_port = htons(Port);

	// set SO_REUSEADDR on a socket to true (1):
	int optval = 1;
	setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	Bind(Sockfd, (sockaddr*) &ServAddr, sizeof(ServAddr));

  // Listen to the socket
	Listen(Sockfd, 5);


	for(;;)
	{

		int games = 1;
		bool userfirst = false;
		GamePkt.UserScore = 0;
		GamePkt.CompScore = 0;
		bool end = false;
		ScorePkt.Rank = -1;

    // Wait for a connection from a client; this is an iterative server
		ClntLen = sizeof(ClntAddr);
		NewSockfd = Accept(Sockfd, (sockaddr*)&ClntAddr, &ClntLen);

		if(NewSockfd < 0)
		{
			perror("Can't bind to local address!");
		}
		read(NewSockfd,(bool*)&Buff, sizeof(bool));
		if (Buff)		
		{
			userfirst = true;
		}

		while (games < 6)
		{
			char Game[3][3];  // 3 x 3 Game array
			int moves = 0;
			int userwin = 0;

			InitGame(GamePkt.Game);	// Initialize the Game

			GamePkt.Status = ePlayGame;

			write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));

			for(GamePkt.MoveNo = 1; GamePkt.MoveNo <= 9; GamePkt.MoveNo++)
			{
		
				if (userfirst)	//if user goes first in first game
				{
					write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));	//(or is an even amount of times following their game with their first turn)
					read(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				}
				else				//then user gets first turn
				{
					GetCompMove(GamePkt.Game);
					write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));	//(or is an even amount of times following their game with their first turn)
					read(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				}

				if (GameWon(GamePkt.Game, cUser))	// if Game Won by User 
				{
					GamePkt.Status = eUserWin;
					GamePkt.UserScore++;	// update scores, based on moves taken
					userwin = 1;
					break;
				}
				else if (GameWon(GamePkt.Game, cComputer))	// if Game Won by Computer 
				{
					GamePkt.Status = eCompWin;
					GamePkt.CompScore++;	// update scores, based on moves taken
					userwin = 2;
					break;
				}
				else	//check if game is at 9 moves if no winner yet
				{
					if (GamePkt.MoveNo == 9)	//if 9 moves, game is a draw
					{
						GamePkt.Status = eDraw;
						write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
						break;
					}
					else
						write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				}

				GamePkt.MoveNo++;

				if (userfirst)	//give opponent their turn, repeat checks for game won
				{
					GetCompMove(GamePkt.Game);
					write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));	//(or is an even amount of times following their game with their first turn)
					read(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				}
				else	//(or is an even amount of times following their game with their first turn)
				{
					write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));	
					read(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				}

				if (GameWon(GamePkt.Game, cUser))
				{
					GamePkt.UserScore++;
					userwin = 1;
					break;
				}
				else if (GameWon(GamePkt.Game, cComputer))
				{
					GamePkt.CompScore++;
					userwin = 2;
					break;
				}
				else
				{
					if (GamePkt.MoveNo == 9)
					{
						GamePkt.Status = eDraw;
						write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
						break;
					}
					else
						write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				}
				
			}

			if (userwin == 1)	//int to determine win, lose or draw
			{
				GamePkt.Status = eUserWin;
				write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
			}
			else if (userwin == 2)
			{
				GamePkt.Status = eCompWin;
				write(NewSockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
			}

			games++;

			if (userfirst)	//change who goes first this round
				userfirst = false;
			else
				userfirst = true;

		}
		
		ifstream fin("scores.dat", ios::binary);	//open scores file
		fin.read((char*)&ScorePkt, sizeof(ScorePkt));	//load file contents into score struct
		if (!fin.good())	//check file opened correctly
   		{
			cout << "Could not open high scores file" << endl;
			exit(1);
		}
		fin.close();
		int scorecount = 0;
		bool hscore = false;

		write(NewSockfd,(Scores*)&ScorePkt, sizeof(Scores));	//send score packet to client

		if (ScorePkt.Rank > -1 && ScorePkt.Rank < 3)	//if rank is a new high score, save new struct to file
		{
			ofstream fout("scores.dat", ios::binary);
			read(NewSockfd,(Scores*)&ScorePkt, sizeof(Scores));
   			fout.write((char*)&ScorePkt, sizeof(ScorePkt));
    			fout.close();	
		}		

		close(NewSockfd);	//close connection

	}
	return 0; // should never happen
}

void filecreator()	//creates a sample score packet and file if one doesn't already exist
{
	strcpy(ScorePkt.Names[0],"Ticker");
	strcpy(ScorePkt.Names[1],"Tacker");
	strcpy(ScorePkt.Names[2],"Toer");
	ScorePkt.Scores[0] = 3;
	ScorePkt.Scores[1] = 2;
	ScorePkt.Scores[2] = 1;
	ScorePkt.Rank = 0;

	ofstream fout("scores.dat", ios::binary);
	if (!fout.good())
   	{
		cout << "Could not open high scores file" << endl;
		exit(1);
	}
	else
	{
		fout.write((char*)&ScorePkt, sizeof(ScorePkt));
	}
	fout.close();
	
	ifstream fin("scores.dat", ios::binary);
	if (!fin.good())
   	{
		cout << "Could not open high scores file" << endl;
		exit(1);
	}
	else
	{
		fin.read((char*)&ScorePkt, sizeof(ScorePkt));
	}
	fin.close();
}

bool fileexist(const char *file)
{
	ifstream fin(file);
	return fin;
}
