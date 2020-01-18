#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <ctype.h>

#define INVALID '0'
#define HIT_Paddle1 1000
#define HIT_Paddle2 2000

void CampoPong ();
void Message (int message, int score, HANDLE handle);
char GetRandomDirection(void);
void printScore (int score[2], HANDLE handle);
COORD Increment(char c);
void MovePaddle(struct tagPaddle *Paddle,char input,HANDLE Console);
void PrintPaddle(struct tagPaddle Paddle, HANDLE Console);
char CheckInput();
void DeletePaddle(struct tagPaddle Paddle, HANDLE Console);
int checkHit (COORD nextCoords,struct tagPaddle *Paddle1,struct tagPaddle *Paddle2);
COORD ReturnBallDirection(COORD nextCoords,struct tagPaddle *Paddle1,struct tagPaddle *Paddle2);

struct tagPaddle
{
	COORD Upper;
	COORD Lower;
}Paddle1,Paddle2;

int main()
{
	COORD coords, direcao;
	COORD nextCoords;
	HANDLE handle;
	CONSOLE_CURSOR_INFO cursor;

	FILE *File;

	int random, codCor, ms = 75, score[2] = {0,0}, maxScore= 10,opc;
	char c,aux,input, inputAux;

	time_t t ;
	struct tm tm ;

	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	File = fopen ("MatchHistory.txt", "a");

	Paddle1.Upper.X = 1;
	Paddle1.Lower.X = 1;

	Paddle1.Upper.Y = 8;
	Paddle1.Lower.Y = 4;

	Paddle2.Upper.X = 78;
	Paddle2.Lower.X = 78;

	Paddle2.Upper.Y = 8;
	Paddle2.Lower.Y = 4;


	printf ("Campo de (r)elva ou (t)erra? ");
	scanf ("%c", &c);

	if (c == 'r')
		codCor = 175;
	if (c == 't')
	{
		codCor = 111;
		ms = 125;
	}

	coords.X = GetRandomNumber(20, 60);
	coords.Y = GetRandomNumber(5, 16);

	c = GetRandomDirection();
	direcao = Increment(c);

	system("cls");

	CampoPong(codCor);
	printScore(score, handle);
	SetConsoleTextAttribute(handle, codCor);
	PrintPaddle(Paddle1, handle);
	PrintPaddle(Paddle2, handle);


	while (1)

	{
		cursor.dwSize = 1;
		cursor.bVisible = FALSE;
		SetConsoleCursorInfo (handle, &cursor);
		SetConsoleTextAttribute(handle, codCor);

		if (_kbhit() != 0)
		{
			if ((inputAux = CheckInput() ) == INVALID )
				continue;

			input = inputAux;

			switch(input)
			{
			case 'W':
			case 'S':
			case 'A':
			case 'D':
				MovePaddle(&Paddle1, input, handle);
				break;
			case '8':
			case '2':
			case '4':
			case '6':
				MovePaddle(&Paddle2, input, handle);
				break;


			}
		}


		SetConsoleCursorPosition(handle, coords);
		printf (" ");


		nextCoords.X = coords.X+ direcao.X;
		nextCoords.Y = coords.Y+direcao.Y;

		switch ( opc = checkHit(nextCoords,&Paddle1,&Paddle2) )
		{
		case -1:
			direcao.Y  = -direcao.Y;
			break;
		case 0:
			break;
		case 1:
		case 2:
			score[checkHit(nextCoords,&Paddle1,&Paddle2)-1]++;
			Message (checkHit(nextCoords,&Paddle1,&Paddle2), score, handle);
			coords.X = GetRandomNumber(20, 60);
			coords.Y = GetRandomNumber(5, 16);
			c = GetRandomDirection();
			direcao = Increment(c);
			break;
		default:		//Acertou no paddle
				direcao = ReturnBallDirection(nextCoords,&Paddle1,&Paddle2,direcao);
			
		}

		if (score[0] == maxScore || score[1] == maxScore)
		{
			if (score[0] > score[1])
				Message (3, score, handle);
			else
				Message (4, score, handle);
			break;
		}

		nextCoords.X = coords.X+ direcao.X;
		nextCoords.Y = coords.Y+direcao.Y;
		coords = nextCoords;
		SetConsoleCursorPosition(handle, coords);
		printf ("%c", 254);

		Sleep(ms);
	}
	t = time(NULL);
	tm = *localtime(&t);

	fprintf (File, "%d - %d %d/%d/%d %d:%d\n", score[0], score[1], tm.tm_mday, tm.tm_mon + 1,tm.tm_year + 1900 , tm.tm_hour, tm.tm_min);


	return 0;
}


void CampoPong (int codCor)
{
	int idx, idx2;
	COORD coords;
	HANDLE handle;
	handle = GetStdHandle(STD_OUTPUT_HANDLE);


	for (idx = 0; idx < 22; idx++)
	{
		for (idx2 = 0; idx2 < 80; idx2++)
		{
			if (idx == 0 || idx == 21)
			{
				if ( idx2 == 0 || idx2 == 79)
				{
					printf ("+");
				}
				else
					printf ("-");
			}
			else
			{
				if ( idx2 == 0 )
				{
					printf ("|");

				}
				else
				{
					if (idx2 == 79)
					{
						coords.X = idx2;
						coords.Y = idx;
						SetConsoleCursorPosition(handle, coords);
						SetConsoleTextAttribute(handle, 15 | 0);
						printf ("|");
					}
					else
					{
						SetConsoleTextAttribute(handle, codCor);
						printf (" ");
					}
				}
			}

		}

	}

}

int checkHit (COORD nextCoords,struct tagPaddle *Paddle1,struct tagPaddle *Paddle2)
{
	if (nextCoords.X == 0)
		return 2;
	if (nextCoords.X == 79)
		return 1;
	if (nextCoords.Y == 0 || nextCoords.Y == 21)
		return -1;
	if(	nextCoords.X == Paddle1->Upper.X)
		if(EntreAB(nextCoords.Y,Paddle1->Upper.Y,Paddle1->Upper.Y + 4) == 1)
			return HIT_Paddle1;
	if( nextCoords.X == Paddle2->Upper.X )
		if(EntreAB(nextCoords.Y,Paddle2->Upper.Y,Paddle2->Upper.Y + 4) == 1)
			return HIT_Paddle2;
	
	
	return 0; // Sem Hits
}

int GetRandomNumber(int min, int max)
{
	int num;

	num = ( rand() % (max - min + 1) ) + min; 

	return num;
}

void Message (int message, int score[2], HANDLE handle)
{
	COORD coords;

	coords.X = 0;
	coords.Y = 23;
	SetConsoleTextAttribute(handle, 15 | 0);
	switch (message)
	{
	case 1:
	case 2:	
		printScore(score, handle);

		printf ("Player %d scores\n", message);
		printf ("Press spacebar to continue...");

		while (1)
		{
			if (getch() == ' ')
				break;
		}
		printf("\r                                  ");
		coords.X = 0;
		coords.Y = 23;
		SetConsoleCursorPosition(handle, coords);
		printf("                                  ");

		break;
	case 3: 
	case 4:
		coords.X = 0;
		coords.Y = 23;
		SetConsoleCursorPosition(handle, coords);
		printf ("Player %d Wins!\n", message-2);

		break;
	default:
		break;
	}
}


char GetRandomDirection(void)
{ 
	int num; 

	int min=1;
	int max=9;
	srand ( (unsigned)time ( NULL));
	do
	{

		num = ( rand() % (max - min + 1) ) + min; 
	}
	while( num==8 || num==5 || num==2 || num==7|| num==9|| num==1|| num==3);

	num+= 48;

	return (char)num;

} 


COORD Increment(char c)
{
	COORD pos = { 0 , 0};

	switch(c)
	{
	case '1':
	case '4':
	case '7':
		pos.X = -1;
		break;
	case '3':
	case '6':
	case '9':
		pos.X = 1;
		break;
	default:
		pos.X = 0;
		break;
	}

	switch(c)
	{
	case '1':
	case '2':
	case '3':
		pos.Y = 1;
		break;
	case '7':
	case '8':
	case '9':
		pos.Y = -1;
		break;
	default:
		pos.Y = 0;
		break;

	}

	return pos;

}



void printScore (int score[2], HANDLE handle)
{
	COORD coords = { 0 , 0};

	coords.X = 31;

	coords.Y = 22;
	SetConsoleCursorPosition(handle, coords);
	printf ("%d      -      %d\n", score[0], score[1]);
}
void MovePaddle(struct tagPaddle *Paddle,char input,HANDLE Console)
{
	int cnt=0;

	switch(input)
	{
	case 'W':
	case '8':
		if( Paddle->Upper.Y - 1 < 1 )
			break;
		else
		{
			DeletePaddle(*Paddle,Console);
			Paddle->Upper.Y -= 1;	
			Paddle->Lower.Y -=1;
			break;
		}
	case 'S':
	case '2':
		if( Paddle->Upper.Y + 5 > 20 )
			break;
		else
		{
			DeletePaddle(*Paddle,Console);
			Paddle->Upper.Y += 1;	
			Paddle->Lower.Y +=1;
			break;
		}
	case 'A':
	case '4':
		if (Paddle->Upper.X <= 40)		//È o da esquerda
			if(Paddle->Upper.X - 1 < 2 ) // Para ver se está a sair dos limites
				break;
			else
			{
			DeletePaddle(*Paddle,Console);
			Paddle->Upper.X -= 1;	
			Paddle->Lower.X -= 1;
			break;
			}
		else				// É o da direita
			if(Paddle->Upper.X - 1 < 41)	// Para ver se está a sair dos limites
				break;
			else
			{
			  DeletePaddle(*Paddle,Console);
			  Paddle->Upper.X -= 1;	
			  Paddle->Lower.X -= 1;
				break;
			}
	case 'D':
	case '6':
		if (Paddle->Upper.X <= 40)		// Esquerda
			if(Paddle->Upper.X + 1 > 40)
				break;
			else
			{
			DeletePaddle(*Paddle,Console);
			Paddle->Upper.X += 1;	
			Paddle->Lower.X += 1;
			break;
			}
		else					//Direita
			if(Paddle->Upper.X + 1 > 78)
				break;
			else
			{
			  DeletePaddle(*Paddle,Console);
			  Paddle->Upper.X += 1;	
			  Paddle->Lower.X += 1;
				break;
			}

	default: puts("I shouldn't be here");
	}

	PrintPaddle(*Paddle,Console);

}
void PrintPaddle(struct tagPaddle Paddle, HANDLE Console)
{
	int cnt=0;

	COORD cursor;

	cursor.Y = Paddle.Upper.Y;
	cursor.X = Paddle.Upper.X;

	SetConsoleCursorPosition(Console,cursor);

	for(cnt=0; cnt < 5 ; cnt++)
	{
		cursor.Y = Paddle.Upper.Y + cnt; // Imprimir de cima para baixo aumentando o Y
		SetConsoleCursorPosition(Console,cursor);
		printf("%c", 219);
	}

}
void DeletePaddle(struct tagPaddle Paddle, HANDLE Console)
{
	int cnt=0;

	COORD cursor;

	cursor.Y = Paddle.Upper.Y;
	cursor.X = Paddle.Upper.X;

	for(cnt=0; cnt < 5 ; cnt++)
	{
		cursor.Y = Paddle.Upper.Y + cnt;
		SetConsoleCursorPosition(Console,cursor);
		printf(" ");
	}

}
char CheckInput()
{
	char c;

	c= getch();

	switch( toupper(c) )
	{
	case 'W':
	case 'S':
	case '8':
	case '2':
	case 'A':
	case '4':
	case 'D':
	case '6':
		return toupper(c);

	default:
		return INVALID;
	}
}
COORD ReturnBallDirection(COORD nextCoords,struct tagPaddle *Paddle1,struct tagPaddle *Paddle2,COORD direction)
{
	int caso,aux;

	if(nextCoords.X == Paddle1->Upper.X)	// Está a acertar no paddle 1
	{
		caso = nextCoords.Y - Paddle1->Upper.Y + 1;
		
		switch(caso)
		{
		case 3:
			direction.Y = 0;
			direction.X = 1;
			break;
		default:
			direction.X = 1;
			if(direction.Y==0)
			{
				if(caso == 1 || caso == 2)
					direction.Y = -1;
				else
					direction.Y =  1;
			}
			break;
		}

	}
	else	// Está a acertar no paddle 2
	{
		caso = nextCoords.Y - Paddle2->Upper.Y + 1;

 		switch(caso)
		{
		case 3:
			direction.Y = 0;
			direction.X = -1;
			break;
		default:
			direction.X = -1;
			if(direction.Y==0)
			{
				if(caso == 1 || caso == 2)
					direction.Y = 1;
				else
					direction.Y = -1;
			}
			break;
		}

	}


		
	return direction;
}
int EntreAB(int x,int min,int max)
{
	if(x>max||x<min)
		return -1;
	else
		return 1;

}