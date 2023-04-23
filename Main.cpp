#include <iostream>
#include <conio.h>
#include <windows.h>
#include "fcntl.h"
#include "io.h"
using namespace std;

float lokacija = 1;
float *pLokacija = &lokacija;
//lokacija = 1, Meni
//lokacija = 1.1, prva opcija
//lokacija = 1.2, druga opcija itd
//lokacija = 1.1.1, prva opcija u prvoj opciji itd
char Bar = char(0x16), B = char(219), E = char(0x08);
char const BAR[5] = {B, Bar, Bar, Bar, B};
int bojaReal = 13, boja;
int *pBojaReal = &bojaReal;
bool tipSelekcije = true;
bool *pTipSelekcije = &tipSelekcije;
bool tipFormatDatuma = true;
bool *pTipFormatDatuma = &tipFormatDatuma;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

void ShowConsoleCursor(bool showFlag)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(h, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(h, &cursorInfo);
}

int meni(int brojOpcija);
void odabranaBoja(){SetConsoleTextAttribute(h, bojaReal);}
void vratiBoju(){SetConsoleTextAttribute(h, 15);}
void printajNaslov();
void teg();
void printajGrafik()
{
cout << "\t  _____  _____  _____ __  __      _ ______ _____  " << "\n";
cout << "\t |  __ \\|  __ \\|_   _|  \\/  |    | |  ____|  __ \\ " << "\n";
cout << "\t | |__) | |__) | | | | \\  / |    | | |__  | |__) |" << "\n";
cout << "\t |  ___/|  _  /  | | | |\\/| |_   | |  __| |  _  / " << "\n";
cout << "\t | |    | | \\ \\ _| |_| |  | | |__| | |____| | \\ \\ " << "\n";
cout << "\t |_|    |_|  \\_\\_____|_|  |_|\\____/|______|_|  \\_\\" << "\n";
}
void opcijePromjena();
int selekcijaLogika(char const** OPCIJE, int brojOpcija, float lokacija, char const* naslov)
{
	int odabir = 1;
	if(*pTipSelekcije == true)
	{
		int key = 0;
		while(key != 13)
		{
			system("CLS");
			ShowConsoleCursor(false);
			if(lokacija == 1.0)
				printajNaslov();
			else
				vratiBoju();
			cout << "\t" << naslov << "\n";
			
			cout << "\n";

			int p = 0;
			while(p < brojOpcija)
			{
				if(odabir != p+1){	cout << "\t      " << OPCIJE[p];	}	
				else{	odabranaBoja(); cout << "\t" << BAR << " " << OPCIJE[p]; vratiBoju();	}
				cout << "\n"; p++;	
				
			}
			if(lokacija != 1.0)
					cout << "\n\tNAZAD [ESC]\n";
			key = getch();
	
			if(key == 80)			odabir++;
			else if(key == 72)		odabir--;
	
			if(odabir > brojOpcija)	odabir = 1;
			else if(odabir < 1)		odabir = brojOpcija;
			
			if(lokacija != 1.0)
				if(key == 27) return -1;
		}
		return odabir;
	}
	else
	{
		int i;
		do
		{
			system("CLS");
			ShowConsoleCursor(true);
			odabranaBoja();
			if(lokacija == 1.0) printajNaslov();
			cout << "\t" << naslov << "\n";
			odabranaBoja();
			cout << "\n";
			if(cin.fail() || odabir < 1 || odabir > brojOpcija)
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			for(i = 0; i < brojOpcija; i++) cout << "\t" << i + 1 << ". " << OPCIJE[i] << "\n";
			if(lokacija != 1) cout << "\t" << i + 1 << ". Nazad\n";	
			
			cout << "\t"; cin >> odabir;
			if(odabir == brojOpcija + 1)
			{
				return -1;
				break;
			}
		}while(cin.fail() || odabir < 1 || odabir > brojOpcija);
		return odabir;
	}
}



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

void printajNaslov()
{
odabranaBoja();
//cout << "      _____                    _____                    _____                    _____                _____                    _____                    _____                    _____          " << "\n";
//cout << "     /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\              /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\         " << "\n";
//cout << "    /::\\    \\                /::\\    \\                /::\\    \\                /::\\    \\            /::\\    \\                /::\\    \\                /::\\____\\                /::\\    \\        " << "\n";
//cout << "    \\:::\\    \\              /::::\\    \\              /::::\\    \\              /::::\\    \\           \\:::\\    \\              /::::\\    \\              /::::|   |               /::::\\    \\       " << "\n";
//cout << "     \\:::\\    \\            /::::::\\    \\            /::::::\\    \\            /::::::\\    \\           \\:::\\    \\            /::::::\\    \\            /:::::|   |              /::::::\\    \\      " << "\n";
//cout << "      \\:::\\    \\          /:::/\\:::\\    \\          /:::/\\:::\\    \\          /:::/\\:::\\    \\           \\:::\\    \\          /:::/\\:::\\    \\          /::::::|   |             /:::/\\:::\\    \\     " << "\n";
//cout << "       \\:::\\    \\        /:::/__\\:::\\    \\        /:::/__\\:::\\    \\        /:::/__\\:::\\    \\           \\:::\\    \\        /:::/__\\:::\\    \\        /:::/|::|   |            /:::/__\\:::\\    \\    " << "\n";
//cout << "       /::::\\    \\      /::::\\   \\:::\\    \\      /::::\\   \\:::\\    \\      /::::\\   \\:::\\    \\          /::::\\    \\      /::::\\   \\:::\\    \\      /:::/ |::|   |           /::::\\   \\:::\\    \\   " << "\n";
//cout << "      /::::::\\    \\    /::::::\\   \\:::\\    \\    /::::::\\   \\:::\\    \\    /::::::\\   \\:::\\    \\        /::::::\\    \\    /::::::\\   \\:::\\    \\    /:::/  |::|   | _____    /::::::\\   \\:::\\    \\  " << "\n";
//cout << "     /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\  /:::/\\:::\\   \\:::\\____\\  /:::/\\:::\\   \\:::\\    \\      /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\  /:::/   |::|   |/\\    \\  /:::/\\:::\\   \\:::\\    \\ " << "\n";
//cout << "    /:::/  \\:::\\____\\/:::/__\\:::\\   \\:::\\____\\/:::/  \\:::\\   \\:::|    |/:::/__\\:::\\   \\:::\\____\\    /:::/  \\:::\\____\\/:::/  \\:::\\   \\:::\\____\\/:: /    |::|   /::\\____\\/:::/  \\:::\\   \\:::\\____\\" << "\n";
//cout << "   /:::/    \\::/    /\\:::\\   \\:::\\   \\::/    /\\::/   |::::\\  /:::|____|\\:::\\   \\:::\\   \\::/    /   /:::/    \\::/    /\\::/    \\:::\\  /:::/    /\\::/    /|::|  /:::/    /\\::/    \\:::\\  /:::/    /" << "\n";
//cout << "  /:::/    / \\/____/  \\:::\\   \\:::\\   \\/____/  \\/____|:::::\\/:::/    /  \\:::\\   \\:::\\   \\/____/   /:::/    / \\/____/  \\/____/ \\:::\\/:::/    /  \\/____/ |::| /:::/    /  \\/____/ \\:::\\/:::/    / " << "\n";
//cout << " /:::/    /            \\:::\\   \\:::\\    \\            |:::::::::/    /    \\:::\\   \\:::\\    \\      /:::/    /                    \\::::::/    /           |::|/:::/    /            \\::::::/    /  " << "\n";
//cout << "/:::/    /              \\:::\\   \\:::\\____\\           |::|\\::::/    /      \\:::\\   \\:::\\____\\    /:::/    /                      \\::::/    /            |::::::/    /              \\::::/    /   " << "\n";
//cout << "\\::/    /                \\:::\\   \\::/    /           |::| \\::/____/        \\:::\\   \\::/    /    \\::/    /                       /:::/    /             |:::::/    /               /:::/    /    " << "\n";
//cout << " \\/____/                  \\:::\\   \\/____/            |::|  ~|               \\:::\\   \\/____/      \\/____/                       /:::/    /              |::::/    /               /:::/    /     " << "\n";
//cout << "                           \\:::\\    \\                |::|   |                \\:::\\    \\                                       /:::/    /               /:::/    /               /:::/    /      " << "\n";
//cout << "                            \\:::\\____\\               \\::|   |                 \\:::\\____\\                                     /:::/    /               /:::/    /               /:::/    /       " << "\n";
//cout << "                             \\::/    /                \\:|   |                  \\::/    /                                     \\::/    /                \\::/    /                \\::/    /        " << "\n";
//cout << "                              \\/____/                  \\|___|                   \\/____/                                       \\/____/                  \\/____/                  \\/____/         " << "\n\n\n";
cout << "  ___  _      _                            _   _                  ___         __                             _   _       " << endl;
cout << " / __|(_) ___| |_  ___  _ __    ___ __ _  | | | | _ _   ___  ___ |_ _| _ _   / _| ___  _ _  _ __   __ _  __ (_) (_) __ _ " << endl;
cout << " \\__ \\| |(_-<|  _|/ -_)| '  \\  |_ // _` | | |_| || ' \\ / _ \\(_-<  | | | ' \\ |  _|/ _ \\| '_|| '  \\ / _` |/ _|| | | |/ _` |" << endl;
cout << " |___/|_|/__/ \\__|\\___||_|_|_| /__|\\__,_|  \\___/ |_||_|\\___//__/ |___||_||_||_|  \\___/|_|  |_|_|_|\\__,_|\\__||_|_/ |\\__,_|" << endl;
cout << "                                                                                                              |__/       " << endl;
vratiBoju();
}

int meni(int brojOpcija)
{
	lokacija = 1;
	char const* OPCIJE[brojOpcija];
	OPCIJE[0] = "Opcija 1";
	OPCIJE[1] = "Opcija 2";
	OPCIJE[2] = "Opcija 3";
	OPCIJE[3] = "Opcija 4";
	OPCIJE[4] = "Opcija 5";
	OPCIJE[5] = "Opcije";
	OPCIJE[6] = "EXIT";	
	selekcijaLogika(OPCIJE, brojOpcija, lokacija, "");
}

void opcijePromjena()
{
	ulaz:
	lokacija = 1.6;
	int brojOpcija = 3, key = 0, odabir = 1;
	char const* OPCIJE[brojOpcija];
	OPCIJE[0] = "Promjeni Boju";
	OPCIJE[1] = "Promjeni Nacin Unosa";
	OPCIJE[2] = "Promjeni Format Datuma";
	
	char const* OPCIJE1[2];
	OPCIJE1[0] = "Unos Preko ARROW KEYS";
	OPCIJE1[1] = "Unos Preko BROJEVA";
	
	char const* DATUM[2];
	DATUM[0] = "DD/MM/GGGG";
	DATUM[1] = "MM/DD/GGGG";
	
	int choice = selekcijaLogika(OPCIJE, brojOpcija, lokacija, "OPCIJE:");
	if(choice == -1) goto izlaz;
		
	switch(choice)
	{
		
		case 1:
		{
			case1:
			lokacija = 1.61;
			int odabir1 = *pBojaReal+1, key1 = 0, boja;
			int odabir2;
			if(*pTipSelekcije)
			{
				while(key1 != 13)
				{
					boja = odabir1 - 1;
					system("CLS");
					SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\tOdaberite Boju:";
					cout << "\n\n"; SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\t[<-]"; SetConsoleTextAttribute(h, boja);
					cout << "   (" << boja << "/15) Primjer TEXTA  "; SetConsoleTextAttribute(h, *pBojaReal);
					cout << "[->]\n\n";
					SetConsoleTextAttribute(h, boja);
					printajGrafik();
					SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\n";
					cout << "\tNAZAD [ESC]\n";
					key1 = getch();
				
					if(key1 == 77) odabir1++;
					else if(key1 == 75) odabir1--;
	
					if(odabir1 > 16) odabir1 = 2;
					else if(odabir1 < 2) odabir1 = 16;
				
					if(key1 == 13)
					{
						*pBojaReal = boja;
						goto case1;
						break;
					}
					if(key1 == 27) goto ulaz;
				}
			}
			else
			{
				int i;
				do
				{
					system("CLS");
					SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\tOdaberite Boju:\n";
					printajGrafik();
					cout << "\n";
					for(i = 0; i < 15; i++)
					{
						SetConsoleTextAttribute(h, *pBojaReal);
						cout << "\t" << i + 1;
						cout << ".\t "; SetConsoleTextAttribute(h, i+1);
						cout << "PRIMJER TEXTA" << "\n";
					}
					SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\t" << i + 1 << ".\t NAZAD\n\t";
					if(cin.fail() || odabir2 < 1 || odabir2 > 15)
					{
						cin.clear();
						cin.ignore(1000, '\n');
					}
					cin >> odabir2;	
					if(odabir2 == 16) break;
				}while(cin.fail() || odabir2 < 1 || odabir2 > 15);
				
				if(odabir2 == 16) goto ulaz;
				else if(odabir2 != 16)
				{
					*pBojaReal = odabir2;
					goto case1;
				}
			}
			break;		
		}
		case 2:
		{
			lokacija = 1.62;
			case2:
			int choice1 = selekcijaLogika(OPCIJE1, 2, lokacija, "NACIN UNOSA:"); 
			if(choice1 == -1) goto ulaz;
			if(choice1 == 1) *pTipSelekcije = true;
			else *pTipSelekcije = false;
			goto case2;
			break;
		}
		case 3:
		{
			char const* tempDatum;
			lokacija = 1.63;
			case3:
			if(*pTipFormatDatuma) tempDatum = "TRENUTNI FORMAT: DD/MM/GGGG";
			else tempDatum = "TRENUTNI FORMAT: MM/DD/GGGG";
			int odabirFormatDatuma = selekcijaLogika(DATUM, 2, lokacija, tempDatum);
			if(odabirFormatDatuma == -1) goto ulaz;
			if(odabirFormatDatuma == 1)	*pTipFormatDatuma = true;
			else *pTipFormatDatuma = false;
			goto case3;
			break;
		}
		default:
		{
			break; izlaz:{}
		}
	}
}
