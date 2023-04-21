#include <iostream>
#include <conio.h>
#include <windows.h>
#include "fcntl.h"
#include "io.h"
using namespace std;

int boja = 13;
int *pBoja = &boja;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
int meni(int brojOpcija);
void odabranaBoja(){SetConsoleTextAttribute(h, boja);}
void vratiBoju(){SetConsoleTextAttribute(h, 15);}
void printajNaslov();
void teg();
void printajGrafik();
void opcijePromjena();

char Bar = char(0x16), B = char(219), E = char(0x08);
char const BAR[5] = {B, Bar, Bar, Bar, B};

int main()
{
	int brojOpcija = 7;
	int odabir;
	do
	{
		odabir = meni(brojOpcija);
			switch(odabir)
		{
			case 1:
		{
			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			break;
		}
		case 4:
		{
			break;
		}
		case 5:
		{
			break;
		}
		case 6:
		{
			opcijePromjena();
			break;
		}
		default:
		{
			break;
		}
	}
	}while(odabir != brojOpcija);
	

	
	system("PAUSE");
	return 0;
}

void printajGrafik()
{
}

void printajNaslov()
{
odabranaBoja();
cout << "      _____                    _____                    _____                    _____                _____                    _____                    _____                    _____          " << "\n";
cout << "     /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\              /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\         " << "\n";
cout << "    /::\\    \\                /::\\    \\                /::\\    \\                /::\\    \\            /::\\    \\                /::\\    \\                /::\\____\\                /::\\    \\        " << "\n";
cout << "    \\:::\\    \\              /::::\\    \\              /::::\\    \\              /::::\\    \\           \\:::\\    \\              /::::\\    \\              /::::|   |               /::::\\    \\       " << "\n";
cout << "     \\:::\\    \\            /::::::\\    \\            /::::::\\    \\            /::::::\\    \\           \\:::\\    \\            /::::::\\    \\            /:::::|   |              /::::::\\    \\      " << "\n";
cout << "      \\:::\\    \\          /:::/\\:::\\    \\          /:::/\\:::\\    \\          /:::/\\:::\\    \\           \\:::\\    \\          /:::/\\:::\\    \\          /::::::|   |             /:::/\\:::\\    \\     " << "\n";
cout << "       \\:::\\    \\        /:::/__\\:::\\    \\        /:::/__\\:::\\    \\        /:::/__\\:::\\    \\           \\:::\\    \\        /:::/__\\:::\\    \\        /:::/|::|   |            /:::/__\\:::\\    \\    " << "\n";
cout << "       /::::\\    \\      /::::\\   \\:::\\    \\      /::::\\   \\:::\\    \\      /::::\\   \\:::\\    \\          /::::\\    \\      /::::\\   \\:::\\    \\      /:::/ |::|   |           /::::\\   \\:::\\    \\   " << "\n";
cout << "      /::::::\\    \\    /::::::\\   \\:::\\    \\    /::::::\\   \\:::\\    \\    /::::::\\   \\:::\\    \\        /::::::\\    \\    /::::::\\   \\:::\\    \\    /:::/  |::|   | _____    /::::::\\   \\:::\\    \\  " << "\n";
cout << "     /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\  /:::/\\:::\\   \\:::\\____\\  /:::/\\:::\\   \\:::\\    \\      /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\  /:::/   |::|   |/\\    \\  /:::/\\:::\\   \\:::\\    \\ " << "\n";
cout << "    /:::/  \\:::\\____\\/:::/__\\:::\\   \\:::\\____\\/:::/  \\:::\\   \\:::|    |/:::/__\\:::\\   \\:::\\____\\    /:::/  \\:::\\____\\/:::/  \\:::\\   \\:::\\____\\/:: /    |::|   /::\\____\\/:::/  \\:::\\   \\:::\\____\\" << "\n";
cout << "   /:::/    \\::/    /\\:::\\   \\:::\\   \\::/    /\\::/   |::::\\  /:::|____|\\:::\\   \\:::\\   \\::/    /   /:::/    \\::/    /\\::/    \\:::\\  /:::/    /\\::/    /|::|  /:::/    /\\::/    \\:::\\  /:::/    /" << "\n";
cout << "  /:::/    / \\/____/  \\:::\\   \\:::\\   \\/____/  \\/____|:::::\\/:::/    /  \\:::\\   \\:::\\   \\/____/   /:::/    / \\/____/  \\/____/ \\:::\\/:::/    /  \\/____/ |::| /:::/    /  \\/____/ \\:::\\/:::/    / " << "\n";
cout << " /:::/    /            \\:::\\   \\:::\\    \\            |:::::::::/    /    \\:::\\   \\:::\\    \\      /:::/    /                    \\::::::/    /           |::|/:::/    /            \\::::::/    /  " << "\n";
cout << "/:::/    /              \\:::\\   \\:::\\____\\           |::|\\::::/    /      \\:::\\   \\:::\\____\\    /:::/    /                      \\::::/    /            |::::::/    /              \\::::/    /   " << "\n";
cout << "\\::/    /                \\:::\\   \\::/    /           |::| \\::/____/        \\:::\\   \\::/    /    \\::/    /                       /:::/    /             |:::::/    /               /:::/    /    " << "\n";
cout << " \\/____/                  \\:::\\   \\/____/            |::|  ~|               \\:::\\   \\/____/      \\/____/                       /:::/    /              |::::/    /               /:::/    /     " << "\n";
cout << "                           \\:::\\    \\                |::|   |                \\:::\\    \\                                       /:::/    /               /:::/    /               /:::/    /      " << "\n";
cout << "                            \\:::\\____\\               \\::|   |                 \\:::\\____\\                                     /:::/    /               /:::/    /               /:::/    /       " << "\n";
cout << "                             \\::/    /                \\:|   |                  \\::/    /                                     \\::/    /                \\::/    /                \\::/    /        " << "\n";
cout << "                              \\/____/                  \\|___|                   \\/____/                                       \\/____/                  \\/____/                  \\/____/         " << "\n\n\n";
cout << "  ___  _      _                            _   _                  ___         __                             _   _       " << endl;
cout << " / __|(_) ___| |_  ___  _ __    ___ __ _  | | | | _ _   ___  ___ |_ _| _ _   / _| ___  _ _  _ __   __ _  __ (_) (_) __ _ " << endl;
cout << " \\__ \\| |(_-<|  _|/ -_)| '  \\  |_ // _` | | |_| || ' \\ / _ \\(_-<  | | | ' \\ |  _|/ _ \\| '_|| '  \\ / _` |/ _|| | | |/ _` |" << endl;
cout << " |___/|_|/__/ \\__|\\___||_|_|_| /__|\\__,_|  \\___/ |_||_|\\___//__/ |___||_||_||_|  \\___/|_|  |_|_|_|\\__,_|\\__||_|_/ |\\__,_|" << endl;
cout << "                                                                                                              |__/       " << endl;
vratiBoju();
}

int meni(int brojOpcija)
{
	
	char const* OPCIJE[brojOpcija];
	OPCIJE[0] = "Opcija 1";
	OPCIJE[1] = "Opcija 2";
	OPCIJE[2] = "Opcija 3";
	OPCIJE[3] = "Opcija 4";
	OPCIJE[4] = "Opcija 5";
	OPCIJE[5] = "Opcije";
	OPCIJE[6] = "EXIT";	
	
	int key = 0, odabir = 1;
	while(key != 13)
	{
		system("CLS");
			printajNaslov();
		cout << "\n\n\n";

		int p = 0;
		while(p < brojOpcija)
		{
			if(odabir != p+1){	cout << "\t      " << OPCIJE[p];	}	
			else{	odabranaBoja(); cout << "\t" << BAR << " " << OPCIJE[p]; vratiBoju();	}
			cout << "\n"; p++;	
		}
		key = getch();
	
		if(key == 80)			odabir++;
		else if(key == 72)		odabir--;
	
		if(odabir > brojOpcija)	odabir = 1;
		else if(odabir < 1)		odabir = brojOpcija;
	}
	return odabir;
}

void opcijePromjena()
{
	int brojOpcija = 2, key = 0, odabir = 1;
	char const* OPCIJE[brojOpcija];
	OPCIJE[0] = "Promjeni Boju";
	OPCIJE[1] = "Nazad";
	while(key != 13)
	{
		system("CLS");
		int p = 0;
		while(p < brojOpcija)
		{
			if(odabir != p+1){	cout << "\t      " << OPCIJE[p];	}	
			else{	odabranaBoja(); cout << "\t" << BAR << " " << OPCIJE[p]; vratiBoju();	}
			cout << "\n"; p++;	
		}
		key = getch();
	
		if(key == 80)			odabir++;
		else if(key == 72)		odabir--;
	
		if(odabir > brojOpcija)	odabir = 1;
		else if(odabir < 1)		odabir = brojOpcija;
	}
	switch(odabir)
	{
		case 1:
		{
			int odabir1 = 2, key1 = 0;
			while(key1 != 13)
			{
				boja = odabir1 - 1;
				system("CLS");
				cout << "\t[<-]"; odabranaBoja();
				cout << "   (" << boja << "/15) Primjer TEXTA  "; vratiBoju();
				cout << "[->]\n";
				key1 = getch();
				
				if(key1 == 77)			{
				odabir1++;}
				else if(key1 == 75)		{
				odabir1--;}
	
				if(odabir1 > 16)			odabir1 = 2;
				else if(odabir1 < 2)		odabir1 = 16;
			}
			break;			
		}
		default:
		{
			break;
		}
	}
}
