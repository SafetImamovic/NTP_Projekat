//sistem za uclanivanje u sportski klub ili teretuanu, odredjivanje termina treninga itd
#include <iostream>
#include <conio.h>
#include <windows.h>
#include "fcntl.h"
#include "io.h"
#include <thread>
#include <chrono>
#include <iomanip>
#include "Includes/grafici.h"
#include <cstring>
using namespace std;

char const* LOKACIJA = "1.0.0.0";
//lokacija = 1, Meni lokacija = 1.1, prva opcija lokacija = 1.2, druga opcija itd lokacija = 1.11, prva opcija u prvoj opciji itd

int bojaReal = 9, boja;
int *pBojaReal = &bojaReal;
bool tipSelekcije = true;
bool *pTipSelekcije = &tipSelekcije;
bool tipFormatDatuma = true;
bool *pTipFormatDatuma = &tipFormatDatuma;
bool prikaziGrafik = true;
bool *pPrikaziGrafik = &prikaziGrafik;

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

void opcijePromjena();

typedef void (*pFunkcija)();

int selekcijaLogika(char const** OPCIJE, int brojOpcija, char const*& LOKACIJA, char const* naslov, pFunkcija pGrafik)
{
	int odabir = 1;
	if(*pTipSelekcije == true)
	{
		int key = 0;
		while(key != 13)
		{
			system("CLS");
			ShowConsoleCursor(false);
			odabranaBoja(); pGrafik(); vratiBoju();
			cout << "\t(" << odabir << "/" << brojOpcija << ") ";
			cout << naslov << "\n\n";
			int p = 0;
			while(p < brojOpcija)
			{
				if(odabir != p+1){	cout << "\t      " << OPCIJE[p];	}	
				else{	odabranaBoja(); cout << "\t" << BAR << " " << OPCIJE[p]; vratiBoju();	}
				cout << "\n"; p++;	
			}
			if(LOKACIJA != "1.0.0.0")
			{
				cout << "\n\tNAZAD [ESC]\n";
				cout << "\n";
				odabranaBoja(); if(*pPrikaziGrafik) printajTeg(); vratiBoju();
			}
					
			key = getch();
	
			if(key == 80)			odabir++;
			else if(key == 72)		odabir--;
	
			if(odabir > brojOpcija)	odabir = 1;
			else if(odabir < 1)		odabir = brojOpcija;
			
			if(LOKACIJA != "1.0.0.0")
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
			pGrafik();
			odabranaBoja();
			cout << "\t" << naslov << "\n";
			odabranaBoja();
			cout << "\n";
			if(cin.fail() || odabir < 1 || odabir > brojOpcija)
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			for(i = 0; i < brojOpcija; i++) cout << "\t" << i + 1 << ". " << OPCIJE[i] << "\n";
			if(LOKACIJA != "1.0.0.0") 
			{
				cout << "\t" << i + 1 << ". Nazad\n";
				
				if(*pPrikaziGrafik)
				{
					cout << "\n";
					printajTeg();
				}
				else{}
			}	
			
			cout << "\n";
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
	LOKACIJA = "1.0.0.0";
	selekcijaLogika(OPCIJE, brojOpcija, LOKACIJA, "MENI:", printajNaslov);
}

void opcijePromjena()
{
	ulaz:
	LOKACIJA = "1.6.0.0";
	int brojOpcija = 5, key = 0, odabir = 1;
	char const* OPCIJE[brojOpcija];
	OPCIJE[0] = "Promjeni Boju";
	OPCIJE[1] = "Promjeni Nacin Unosa";
	OPCIJE[2] = "Promjeni Format Datuma";
	OPCIJE[3] = "Prikazi Grafik";
	OPCIJE[4] = "FACTORY RESET";
	
	char const* OPCIJE1[2];
	OPCIJE1[0] = "Unos Preko ARROW KEYS";
	OPCIJE1[1] = "Unos Preko BROJEVA";
	
	char const* DATUM[2];
	DATUM[0] = "DD/MM/GGGG";
	DATUM[1] = "MM/DD/GGGG";
	
	char const* RESET[2];
	RESET[0] = "DA";
	RESET[1] = "NE";
	
	char const* GRAFIK[2];
	GRAFIK[0] = "PRIKAZI GRAFIK";
	GRAFIK[1] = "NE PRIKAZI GRAFIK";
	
	int choice = selekcijaLogika(OPCIJE, brojOpcija, LOKACIJA, "OPCIJE:", printajNULL);
	if(choice == -1) goto izlaz;
		
	switch(choice)
	{
		
		case 1:
		{
			case1:
			LOKACIJA = "1.6.1.0";
			int odabir1 = *pBojaReal+1, key1 = 0, boja;
			int odabir2;
			if(*pTipSelekcije)
			{
				while(key1 != 13)
				{
					boja = odabir1 - 1;
					system("CLS");
					SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\tODABERITE BOJU:";
					cout << "\n\n"; SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\t[<-]"; SetConsoleTextAttribute(h, boja);
					cout << "   (" << boja << "/15) Primjer TEXTA  "; SetConsoleTextAttribute(h, *pBojaReal);
					cout << "[->]\n\n";
					SetConsoleTextAttribute(h, boja);
					printajGrafik();
					printajTeg();
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
					cout << "\tODABERITE BOJU:\n\n";
					printajGrafik();
					cout << "\n\n";
					for(i = 0; i < 8; i++)
					{
						SetConsoleTextAttribute(h, *pBojaReal);
						
						cout << "\t" << i + 1 << ".\t"; SetConsoleTextAttribute(h, i+1);
						
						cout << "PRIMJER TEXTA";
						if(i == *pBojaReal - 1)
							cout << "\t" << BAR;
						else
							cout << "\t     ";
						//cout << "\t\t";
						if(i > 6)
							break;
						SetConsoleTextAttribute(h, *pBojaReal);
						
						cout << "\t" << i + 1 + 8 << ".\t"; SetConsoleTextAttribute(h, i+9);
						cout << "PRIMJER TEXTA";
						if(i+8 == *pBojaReal - 1)
							cout << "\t" << BAR;
						else
							cout << "\t     ";
						cout << "\n";
					}
					SetConsoleTextAttribute(h, *pBojaReal);
					cout << "\t" << 2*i+2 << ".\tNAZAD\n\t";
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
			LOKACIJA = "1.6.2.0";
			case2:
			int choice1 = selekcijaLogika(OPCIJE1, 2, LOKACIJA, "NACIN UNOSA:", printajNULL); 
			if(choice1 == -1) goto ulaz;
			if(choice1 == 1) *pTipSelekcije = true;
			else *pTipSelekcije = false;
			goto case2;
			break;
		}
		case 3:
		{
			char const* tempDatum;
			LOKACIJA = "1.6.3.0";
			case3:
			if(*pTipFormatDatuma) tempDatum = "TRENUTNI FORMAT: DD/MM/GGGG";
			else tempDatum = "TRENUTNI FORMAT: MM/DD/GGGG";
			int odabirFormatDatuma = selekcijaLogika(DATUM, 2, LOKACIJA, tempDatum, printajNULL);
			if(odabirFormatDatuma == -1) goto ulaz;
			if(odabirFormatDatuma == 1)	*pTipFormatDatuma = true;
			else *pTipFormatDatuma = false;
			goto case3;
			break;
		}
		case 4:
		{
			char const* tempGrafik;
			LOKACIJA = "1.6.4.0";
			case4:
			if(*pPrikaziGrafik) tempGrafik = "FORMAT: PRIKAZUJE";
			else tempGrafik = "FORMAT: NE PRIKAZUJE";
			int odabirPrikaziGrafik = selekcijaLogika(GRAFIK, 2, LOKACIJA, tempGrafik, printajNULL);
			if(odabirPrikaziGrafik == -1) goto ulaz;
			if(odabirPrikaziGrafik == 1) *pPrikaziGrafik = true;
			else *pPrikaziGrafik = false;
			goto case4;
			break;
		}
		case 5:
		{
			char const* tempReset = "VRACA SVE POSTAVKE NA TVORNICKE. DA LI STE SIGURNI?";
			LOKACIJA = "1.6.5.0";
			int reset = selekcijaLogika(RESET, 2, LOKACIJA, tempReset, printajNULL);
			if(reset == 1)
			{
				*pBojaReal = 9;
				*pTipSelekcije = true;
				*pTipFormatDatuma = true;
				*pPrikaziGrafik = true;
				break;
			}
			else
				goto ulaz;
		}
		default:
		{
			break; izlaz:{}
		}
	}
}

