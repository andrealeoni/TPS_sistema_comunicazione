#include "stdafx.h"
#include "stdlib.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include "UDP.h"
#define dim 111
#define dimerr 20
#define dimbuffer 1024
unsigned char ack[2] = { 06, '\0' };
int n_commands=0;
char coma[2] = { ';', '\0' };
char space[2] = { ' ' , '\0' };
UDP sMask(21102);
unsigned long ip_address;
unsigned short port_number;
char stringend = '\0';
char str[1000] = { " > C:\\Users\\g.rivolta\\Desktop\\Executor\\Executor\\CMDResults.txt" };
unsigned char cd[65] = { "cd > C:\\Users\\g.rivolta\\Desktop\\Executor\\Executor\\CMDResults.txt" };
unsigned char n[dimbuffer];
unsigned char cmdhistory[100000];
unsigned char tmphistory[100000];
char error[50] = { "Comando errato" };
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void addNewComand(unsigned char comand[], unsigned char comandlist[])//Rivolta
{
	if(n_commands!=0)
		{
			strcat((char*)comandlist, (char*)coma);
		}
	strcat((char*)comandlist, (char*)comand);
}

void intToString(int num, char str[]) //Merlo e Grandesso
{
	int num1, num2 = 0, i = 0, div2, div1 = 10;
	num1 = num%div1;
	str[i] = (char)(num1 + 48);
	i++;
	while (num != num1)
	{
		num2 = num%div1;
		div2 = div1;
		div1 = div1 * 10;
		num1 = num%div1;
		for (int j = i; j > 0; j--)
		{
			str[j] = str[j - 1];
		}
		str[0] = (char)(((num1 - num2) / div2) + 48);
		i++;
	}
	str[i] = '\0';
}

void socket_send(unsigned char str[]) //Rivolta
{
	sMask.send(ip_address, port_number, str, strlen((char*)str));
	sMask.receive(&ip_address, &port_number, (unsigned char*)ack, sizeof(ack));
}

void conta() //Merlo
{
	int contarighe = 0;
	char strTemp[1024];
	ifstream results;
	results.open("CMDResults.txt");
	do
	{
		results.getline(strTemp, 1023);
		contarighe++;
	} while ((!results.eof()));
	intToString(contarighe, (char*)n);
	sMask.send(ip_address, port_number, n, strlen((char*)n)); //invio del numero di righe
	sMask.receive(&ip_address, &port_number, (unsigned char*)ack, sizeof(ack));
}

void fromFile(unsigned char str[]) //Rivolta
{
	char strResult[1024];
	fstream result;
	result.open("CMDResults.txt", ios::out);
	if (result.eof())
	{
		result.close();
		result.open("CMDResults.txt", ios::out);
		result << error;
		result.close();
	}
	else
	{
		result.open("CMDResults.txt", ios::in);
		if (result.is_open())
		{
			while (!result.eof())
			{
				result.getline(strResult, 1023);
				strResult[strlen(strResult) - 1] = '\0';
				socket_send((unsigned char*)strResult);
			}
		}
		result.close();
	}
}

int main(void)	//Di Giorgio
{
	fstream White_List, Results;
	char DOScommand_list[dim];
	cout << "Servizio attivo..." << "\r\n";
	//while (true)
	{
		Results.open("CMDResults.txt", ios::out);
		Results << "";
		Results.close();
		int j = 0;
		int k = 0, w = 0;
		bool flag = false;
		//sMask.receive(&ip_address, &port_number, n, sizeof(n));
		//sMask.send(ip_address, port_number, (unsigned char*)ack, sizeof(ack));
		n[0] = 'c';
		n[1] = 'd';
		n[2] = '.';
		n[3] = '.';
		n[4] = '\0';
		White_List.open("WhiteList.txt", ios::in);
		if (White_List.is_open())
		{
			while ((!White_List.eof()) && (!flag))
			{
				White_List >> DOScommand_list;
				k = 0;
				int cont = 0;
				flag = false;
				while (DOScommand_list[w] != '\0')
				{
					w++;
					cont++;
					if ((DOScommand_list[w] >= 97) && (DOScommand_list[w] <= 122)) //97 = a 122 = z
					{
						DOScommand_list[w] = toupper(DOScommand_list[w]);
					}
				}
				w = 0;
				for (int i = 0; i < cont; i++)
				{
					if (DOScommand_list[i] != ';')
					{

						if (DOScommand_list[i] == n[k])
						{
							w++;
						}
						k++;
					}
					else
					{
						if (k == w)
						{
							flag = true;
						}
					}
				}
			}
		}
		if (flag)
		{
			cout << "Comando corretto" << endl;
		}
		//Rivolta
		if (n_commands == 0)
			strcpy((char*)cmdhistory, (char*)n);
		else
			strcat((char*)cmdhistory, (char*)n);
		strcat((char*)cmdhistory, coma);
		strcat((char*)cmdhistory, space);
		strcpy((char*)tmphistory, (char*)cmdhistory);
		if (((n[0] == cd[0]) || (n[0] == (cd[0] + 32))) && ((n[1] == cd[1]) || (n[1] == (cd[1] + 32))))
		{
			//addNewComand(n, cmdhistory);
			addNewComand(cd, tmphistory);
			system((char*)tmphistory);
			cout << "CD" << endl;
		}
		else
		{
			strcat((char*)tmphistory, str);
			system((char*)tmphistory);

			if (!flag)  //Comando non contenuto nella white list
			{
				char errore[dimerr] = { "Comando sbagliato" };
				int val = 1;
				sMask.send(ip_address, port_number, (unsigned char*)val, sizeof(unsigned int));
				sMask.receive(&ip_address, &port_number, (unsigned char*)ack, sizeof(ack));
				sMask.send(ip_address, port_number, (unsigned char*)errore, sizeof(errore));
				sMask.receive(&ip_address, &port_number, (unsigned char*)ack, sizeof(ack));
				cout << "Comando errato" << endl;
			}
			else	//Comando contenuto nella white list
			{
				n_commands++;
				conta();
				fromFile(n);
			}
		}
	}
}