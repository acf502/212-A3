/*
Name: Aaron Colin Foote
Date: 20th May 2013
User: acf502 / 4258770
File.: client.cpp for CSCI212 Assignment 3
*/

// Example TCP client - CSCI212
#include "wrappers.h"

bool user_first();
void gameover();	//displays game over output

int main(int argc, char **argv)
{
	char LOCALHOST[] = "localhost";	//default localhost and 48770 ports
	int i;
	char *ServHost = new char[MAX_SIZE];
	string command;
	int SERV_TCP_PORT = 48770;
	bool userchoice = true;
	bool userfirst = true;
	if (argc == 1)			//standard ./client
	{
		ServHost = LOCALHOST;
		cout << "localhost and default port selected" << endl;
	}
	else if (argc == 2)	// ./client + who goes first (-c or -u)
	{
		ServHost = LOCALHOST;
		command = argv[1];
		userchoice = false;
		if ((string)argv[1] == "-c")
		{
			cout << "comp goes first" << endl;
			userfirst = false;
		}
		else if ((string)argv[1] == "-u")
		{
			cout << "user goes first" << endl;
			userfirst = true;
		}
		else
		{
			cout << "Incorrect usage: ./client (-c)/(-u)" << endl;
			exit(1);
		}
	}
	else if (argc == 4)	//all arguments: ./client, host, -port, -first player
	{
		string str0 = argv[1];
		strcpy(ServHost,str0.c_str());
		if (ServHost[0] == '-')
			ServHost++;
		string str1 = argv[2];
		string str2;
		for (int i = 0; i < str1.length(); i++)
		{
			str2[i] = str1[(i+1)];
		}
		SERV_TCP_PORT = atoi(str2.c_str());
		command = argv[3];
		userchoice = false;
		if ((string)argv[3] == "-c")
		{
			cout << "comp goes first" << endl;
			userfirst = false;
		}
		else if ((string)argv[3] == "-u")
		{
			cout << "user goes first" << endl;
			userfirst = true;
		}
		else
		{
			cout << "Incorrect usage: ./client (-c)/(-u)" << endl;
			exit(1);
		}
	}
	else	//wrong arguments quits program
	{
		cout << "Incorrect usage: ./client -host -port# -turn" << endl;
		exit(1);
	}

	int Sockfd;
	sockaddr_in ServAddr;

	hostent *HostPtr;
	int Port = SERV_TCP_PORT;	//48770?
	bool Buff1 = true;
	bool Buff2 = false;
	bool hscore = false;
	
	for(int i=0; i<3; i++)	//load game packet with spaces
	{
		for(int j=0; j<3; j++)
		{
			GamePkt.Game[i][j] = ' ';
		}
	}
	GamePkt.UserScore = 0;	//initialize all of game packet to default
	GamePkt.CompScore = 0;
	GamePkt.MoveNo = 0;
	GamePkt.Status = eInitGame;

  // Get the address of the host
	if ((HostPtr = Gethostbyname(ServHost)) == NULL)	//failure to resolve hostname
	{
		cout << "could not retrieve host name!" << endl;
	}

	if(HostPtr->h_addrtype !=  AF_INET)	//failure to fine proper address type provided
	{
		perror("Unknown address type!");
		exit(1);
	}

	memset((char *) &ServAddr, 0, sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = ((in_addr*)HostPtr->h_addr_list[0])->s_addr;
	ServAddr.sin_port = htons(Port);	//Port = 48770?


  // Open a TCP socket
	Sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	if (Sockfd == 0)
		perror("Create socket");
  // Connect to the server
	Connect(Sockfd, (sockaddr*)&ServAddr, sizeof(ServAddr));

	if (userchoice == true)	//user goes first, if elected
	{
		userfirst = user_first();
	}

	if (userfirst)	//boolean true or false send to server to let it know who will be first
		write(Sockfd,(bool*)&Buff1, sizeof(bool));
	else
		write(Sockfd,(bool*)&Buff2, sizeof(bool));

	int rounds = 0;
  // Write and read a message to/from the server 
	while (rounds < 5)	//5 rounds
	{
		GamePkt.Status = ePlayGame;	//now playing game

		read(Sockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
		DrawGame(GamePkt.Game);

		bool userturn = userfirst;

		for (;;)
		{

			if (userturn)	//if user goes first every 2 turns, user now plays
			{
				read(Sockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				GetUserMove(GamePkt.Game);
				DrawGame(GamePkt.Game);
				write(Sockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
			}
			else	//otherwise computer goes first
			{
				read(Sockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
				DrawGame(GamePkt.Game);
				write(Sockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));
			}
			
			if (userturn)	//switch turn
				userturn = false;
			else
				userturn = true;

			read(Sockfd,(GamePktType*)&GamePkt,sizeof(GamePktType));	//check whether game has been finished
			if (GamePkt.Status == eUserWin || GamePkt.Status == eCompWin || GamePkt.Status == eDraw)
			{
				DrawGame(GamePkt.Game);
				break;
			}

		}

		if (userfirst)		//change who goes first next round
				userfirst = false;
			else
				userfirst = true;

		gameover();

		rounds++;	//increase rounds count
		
	}

	read(Sockfd,(Scores*)&ScorePkt, sizeof(Scores));	//read score packet
	
	int marker = 0;
	ScorePkt.Rank = -1;

	for (marker = 0; marker < 3; marker++)	//find whether user has new high score
	{
		if (GamePkt.UserScore > ScorePkt.Scores[marker])
		{
			hscore = true;
			ScorePkt.Rank = marker;
		}
	}
	
	cout << "ALL-TIME HIGH SCORES:" << endl;
	
	for (int i = 0; i < 3; i++)
	{
		cout << ScorePkt.Names[i] << " " << ScorePkt.Scores[i] << endl << endl;
	}
	
	if (ScorePkt.Rank > -1 && ScorePkt.Rank < 3)	//if rank is within range, prompt for new name to replace lowest current score
	{
		string name;
		cout << "Congratulations! You got a high score!" << endl << endl;
		cout << "You earned a new high-score on the all-time leaderboard with a score of: " << GamePkt.UserScore << "!" << endl;
		cout << "Please enter your name: ";
		getline(cin, name);
		strncpy(ScorePkt.Names[ScorePkt.Rank],name.c_str(), 20);	//change lowest current score to user's score
		ScorePkt.Scores[ScorePkt.Rank] = GamePkt.UserScore;
		cout << "\nNew high scores:" << endl << endl;
		for (int i = 0; i < 3; i++)
		{
			cout << ScorePkt.Names[i] << " " << ScorePkt.Scores[i] << endl << endl;
		}
		write(Sockfd,(Scores*)&ScorePkt, sizeof(Scores));
	}
	else
	{
		cout << "You did not achieve a high score" << endl;	//no new high score
	}
		
	close(Sockfd);
	delete []ServHost;
	return 0;
}

bool user_first()
{
	char response;

  cout << "Do you want to go first? (y/n) " << endl;	//ask if user wants to go first
	do
	{
		cin.get(response);
	} while ((response != 'y') && (response != 'Y') &&	//keep asking hile user doesn't enter y, Y, n or N
	     (response != 'n') && (response != 'N'));

	cin.clear();
	cin.ignore(10000, '\n');
	if ((response == 'y') || (response == 'Y'))	//send back user's decision
		return true;
	else
		return false;
}


void gameover()
{
	if (GamePkt.Status == eCompWin)		//send output to screen of game result, based on game's result
		cout << ServerWin << endl;
	else if (GamePkt.Status == eUserWin)
		cout << ClientWin << endl;
	else if (GamePkt.Status == eDraw)
		cout << Stalemate << endl;
	else
		cout << "Error occured in determining winner" << endl;

	cout << endl << "   G a m e  O v e r !" << endl;

	cout << "Your current score: " << GamePkt.UserScore << endl;
}
