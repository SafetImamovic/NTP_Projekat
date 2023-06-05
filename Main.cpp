//sistem za uclanivanje u sportski klub ili teretuanu, odredjivanje termina treninga itd
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include "fcntl.h"
#include "io.h"
#include <thread>
#include <chrono>
#include <iomanip>
#include "Includes/grafici.h"
#include <string>
#include <ctime>
using namespace std;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

struct tm;
tm noviLokal;

struct POSTAVKE
{
	int bojaReal = 9, boja;
	bool tipSelekcije = true;
	bool tipFormatDatuma = true;
	bool prikaziGrafik = true;
};

struct KORISNIK
{
	int ID;
	tm VrijemeUclanjivanja;
	char Ime[20];
	char Prezime[30];
	char Spol[20];
	string Dan, Mje, God;
	string DatumRodjenja;
	int Dob;
	char AdresaStanovanja[40];
	char BrojTelefona[20];
};

vector<KORISNIK> Korisnici;
vector<KORISNIK>* pKorisnici = &Korisnici;

POSTAVKE globalPOSTAVKE;
POSTAVKE* pGlobalPOSTAVKE = &globalPOSTAVKE;
POSTAVKE tempGlobalnePOSTAVKE;
POSTAVKE* pTempGlobalnePOSTAVKE = &tempGlobalnePOSTAVKE;

void odabranaBoja(int bojaReal){SetConsoleTextAttribute(h, bojaReal);}
void vratiBoju(){SetConsoleTextAttribute(h, 15);}

void ShowConsoleCursor(bool showFlag)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(h, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(h, &cursorInfo);
}

char const* LOKACIJA = "1.0.0.0";
//lokacija = 1, Meni lokacija = 1.1, prva opcija lokacija = 1.2, druga opcija itd lokacija = 1.1.1, prva opcija u prvoj opciji itd

bool sacuvano = true;
bool* pSacuvano = &sacuvano;

void unsaved(char const* lokacija)
{
	if((pGlobalPOSTAVKE->bojaReal != pTempGlobalnePOSTAVKE->bojaReal && lokacija == "1.6.1.0")
	||(pGlobalPOSTAVKE->tipSelekcije != pTempGlobalnePOSTAVKE->tipSelekcije && lokacija == "1.6.2.0")
	||(pGlobalPOSTAVKE->tipFormatDatuma != pTempGlobalnePOSTAVKE->tipFormatDatuma && lokacija == "1.6.3.0")
	||(pGlobalPOSTAVKE->prikaziGrafik != pTempGlobalnePOSTAVKE->prikaziGrafik && lokacija == "1.6.4.0"))
	{
		cout << "\n\tPromjenu Postavke\n\tTreba Sacuvati.\n\tINDIKATOR ";
		if(pGlobalPOSTAVKE->bojaReal != 4)
			odabranaBoja(4);
		else
			odabranaBoja(2);
		cout << BAR;
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "\n";
		*pSacuvano = false;
	}
	else if((pGlobalPOSTAVKE->bojaReal == pTempGlobalnePOSTAVKE->bojaReal && lokacija == "1.6.1.0")
	||(pGlobalPOSTAVKE->tipSelekcije == pTempGlobalnePOSTAVKE->tipSelekcije && lokacija == "1.6.2.0")
	||(pGlobalPOSTAVKE->tipFormatDatuma == pTempGlobalnePOSTAVKE->tipFormatDatuma && lokacija == "1.6.3.0")
	||(pGlobalPOSTAVKE->prikaziGrafik == pTempGlobalnePOSTAVKE->prikaziGrafik && lokacija == "1.6.4.0"))
		*pSacuvano = true;
}

int meni(int brojOpcija);

void POSTAVKEPromjena();

typedef void (*pFunkcija)();

fstream checkPostavkepostavkeFile;
ifstream inPostavkepostavkeFile;
ifstream* pInPostavkepostavkeFile = &inPostavkepostavkeFile;
ofstream postavkeFile;
ofstream* pPostavkeFile = &postavkeFile;

fstream checkKorisniciFile;
ifstream inKorisniciFile;
ifstream* pInKorisniciFile = &inKorisniciFile;
ofstream korisniciFile;
ofstream* pKorisniciFile = &korisniciFile;


void printajNaslov();
void teg();

void parsePOSTAVKE()
{
	string header;
    getline(*pInPostavkepostavkeFile, header);
    
    string line;
    getline(*pInPostavkepostavkeFile, line, ','); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->bojaReal = atoi(line.c_str()); pTempGlobalnePOSTAVKE->bojaReal = atoi(line.c_str()); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->tipSelekcije = atoi(line.c_str()); pTempGlobalnePOSTAVKE->tipSelekcije = atoi(line.c_str()); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->tipFormatDatuma = atoi(line.c_str()); pTempGlobalnePOSTAVKE->tipFormatDatuma = atoi(line.c_str()); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->prikaziGrafik = atoi(line.c_str()); pTempGlobalnePOSTAVKE->prikaziGrafik = atoi(line.c_str()); line = "";
}



void parseKORISNICI()
{
	KORISNIK tempKorisnik;
	
	string header;
	getline(*pInKorisniciFile, header);
	
	string line;
	while(getline(*pInKorisniciFile, line))
	{
		string ID, d, mj, g, h, m, s, ime, prezime, spol, dan, mje, god, adresa, brojtel;
		string tempString, discard;
		
		stringstream inputString(line);
		getline(inputString, discard, ',');
		getline(inputString, ID, ',');
		
		tempKorisnik.ID = atoi(ID.c_str());
		
		getline(inputString, d, ','); getline(inputString, mj, ','); getline(inputString, g, ',');
		getline(inputString, h, ','); getline(inputString, m, ','); getline(inputString, s, ',');
		
		tempKorisnik.VrijemeUclanjivanja.tm_mday = atoi(d.c_str());
		tempKorisnik.VrijemeUclanjivanja.tm_mon = atoi(mj.c_str()) - 1;
		tempKorisnik.VrijemeUclanjivanja.tm_year = atoi(g.c_str()) - 1900;
		tempKorisnik.VrijemeUclanjivanja.tm_hour = atoi(h.c_str());
		tempKorisnik.VrijemeUclanjivanja.tm_min = atoi(m.c_str());
		tempKorisnik.VrijemeUclanjivanja.tm_sec = atoi(s.c_str());
		
		getline(inputString, ime, ',');
		getline(inputString, prezime, ',');
		getline(inputString, spol, ',');
		getline(inputString, dan, ',');
		getline(inputString, mje, ',');
		getline(inputString, god, ',');
		getline(inputString, adresa, ',');
		getline(inputString, brojtel, ',');
		
		strcpy(tempKorisnik.Ime, ime.c_str());
		strcpy(tempKorisnik.Prezime, prezime.c_str());
		strcpy(tempKorisnik.Spol, spol.c_str());
		tempKorisnik.Dan = dan;
		tempKorisnik.Mje = mje;
		tempKorisnik.God = god;
		strcpy(tempKorisnik.AdresaStanovanja, adresa.c_str());
		strcpy(tempKorisnik.BrojTelefona, brojtel.c_str());
		
		Korisnici.push_back(tempKorisnik);
		line = "";
	}
	
}

void obrisiKorisnika()
{
	int velicina = Korisnici.size();
	
}

int selekcijaLogika(char const** OPCIJE, int brojOpcija, char const*& LOKACIJA, char const* naslov, pFunkcija pGrafik)
{
	int odabir = 1;
	if(pGlobalPOSTAVKE->tipSelekcije == true)
	{
		int key = 0;
		while(key != 13)
		{
			system("CLS");
			ShowConsoleCursor(false);
			odabranaBoja(pGlobalPOSTAVKE->bojaReal); pGrafik();
			vratiBoju();
			cout << "\t(" << odabir << "/" << brojOpcija << ") ";
			cout << naslov << "\n\n";
			int p = 0;
			while(p < brojOpcija)
			{
				if(odabir != p+1){	cout << "\t      " << OPCIJE[p];	}	
				else{	odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << "\t" << BAR << " " << OPCIJE[p];	}
				if(LOKACIJA == "1.6.0.0" && *pSacuvano == false && p == 4 ||
				LOKACIJA == "1.0.0.0" && *pSacuvano == false && p == 5)
				{
					if(pGlobalPOSTAVKE->bojaReal != 4)
						odabranaBoja(4);
					else
						odabranaBoja(2);
					cout << " " << BAR << " - NESACUVANE POSTAVKE";
				}
				vratiBoju();
				cout << "\n"; p++;	
			}
			if(LOKACIJA != "1.0.0.0")
			{
				cout << "\n\tNAZAD [ESC]\n";
				cout << "\n";
				odabranaBoja(pGlobalPOSTAVKE->bojaReal); if(pGlobalPOSTAVKE->prikaziGrafik) printajTeg(); vratiBoju();
			}
			unsaved(LOKACIJA);
			
			key = getch();
	
			if(key == 80)			odabir++;
			else if(key == 72)		odabir--;
	
			if(odabir > brojOpcija)	odabir = 1;
			else if(odabir < 1)		odabir = brojOpcija;
			
			if(LOKACIJA != "1.0.0.0")
			{
				if(key == 27)
				{
					return -1;
				}
			}
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
			odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			pGrafik();
			
			cout << "\t" << naslov << "\n";
			odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "\n";
			if(cin.fail() || odabir < 1 || odabir > brojOpcija)
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			for(i = 0; i < brojOpcija; i++)
			{
				cout << "\t" << i + 1 << ". " << OPCIJE[i];
				if(LOKACIJA == "1.6.0.0" && *pSacuvano == false && i == 4
				|| LOKACIJA == "1.0.0.0" && *pSacuvano == false && i == 5)
				{
					if(pGlobalPOSTAVKE->bojaReal != 4)
						odabranaBoja(4);
					else
						odabranaBoja(2);
					cout << " " << BAR << " - NESACUVANE POSTAVKE";
					
				}
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << "\n";
			}
			
			if(LOKACIJA != "1.0.0.0") 
			{
				cout << "\t" << i + 1 << ". Nazad\n";
				
				if(pGlobalPOSTAVKE->prikaziGrafik)
				{
					cout << "\n";
					printajTeg();
				}
			}
				unsaved(LOKACIJA);	
				
			cout << "\n\tOdabir: ";
			cin >> odabir;
			
			if(odabir == brojOpcija + 1)
			{
				return -1;
				break;
			}
			
		}while(cin.fail() || odabir < 1 || odabir > brojOpcija);
		return odabir;
	}
}

string CharArrToString(char *arr)
{
	int i = 0;
	string ret = "";
	while(arr[i] != '\0')
	{
		ret += arr[i];
		i++;
	}
	return ret;
}

void sortAlpha()
{
	int velicina = Korisnici.size();
	vector<KORISNIK> tempKorisnici;
	vector<KORISNIK>* pTempKorisnici = &tempKorisnici;
	
	for(int i = 0; i < velicina; i++)
		pTempKorisnici->push_back(Korisnici[i]);

	string temp, str1, str2;
	for(int i = 0; i < velicina; i++)
	{
		for(int j = 0; j < velicina; j++)
		{
			//if(CharArrToString(tempKorisnici[j].Ime) < CharArrToString(tempKorisnici[j + 1].Ime))
		}
	}
}

unosKorisnikaClassic(pFunkcija pGrafik)
{
	int lokacija = 0;
	int velicina = Korisnici.size();
	KORISNIK tempKorisnik;

	tempKorisnik.ID = velicina+1;
	tempKorisnik.Ime[0] = NULL;
	tempKorisnik.Prezime[0] = NULL;
	tempKorisnik.Spol[0] = NULL;
	tempKorisnik.DatumRodjenja = "";
	tempKorisnik.AdresaStanovanja[0] = NULL;
	tempKorisnik.BrojTelefona[0] = NULL;
	
	char key;
	while(true)
	{
		lokacija = 0;
		while(true)
		{
			system("CLS");
		
			cout << "\tNAZAD "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ESC] "; vratiBoju();
			cout << "- NAPRIJED "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ENTER]\n\n"; vratiBoju();
			cout << "\tUnos "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << velicina + 1; vratiBoju();
			cout << ". Korisnika";
			
			tempKorisnik.ID = velicina + 1;
			
			if(tempKorisnik.Ime[0] != NULL)
			{
				cout << "\n\n\t\t(1/6) Uneseno Ime: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Ime; vratiBoju();
			}
				
			if(tempKorisnik.Prezime[0] != NULL)
			{
				cout << "\n\t\t(2/6) Uneseno Prezime: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Prezime; vratiBoju();
			}
				
			if(tempKorisnik.Spol[0] != NULL)
			{
				cout << "\n\t\t(3/6) Uneseni Spol: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Spol; vratiBoju();
			}
				
			if(tempKorisnik.DatumRodjenja != "")
			{
				cout << "\n\t\t(4/6) Uneseni Datum Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.DatumRodjenja; vratiBoju();
			}
			
			if(tempKorisnik.AdresaStanovanja[0] != NULL)
			{
				cout << "\n\t\t(5/6) Unesena Adresa Stanovanja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.AdresaStanovanja; vratiBoju();
			}
				
			if(tempKorisnik.BrojTelefona[0] != NULL)
			{
				cout << "\n\t\t(6/6) Uneseni Broj Telefona: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.BrojTelefona; vratiBoju();
			}
			
			cout << "\n\n";
			
			if(lokacija == 6)
				lokacija++;
				
				
			switch(lokacija)
			{
				case 0:
				{
					cout << "\t\tUnesite Ime: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(tempKorisnik.Ime, 20); vratiBoju();
					lokacija++;
					break;
				}
				case 1:
				{
					cout << "\t\tUnesite Prezime: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(tempKorisnik.Prezime, 30); vratiBoju();
					lokacija++;
					break;
				}
				case 2:
				{
					cout << "\t\tUnesite Spol: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(tempKorisnik.Spol, 20); vratiBoju();
					lokacija++;
					break;
				}
				case 3:
				{
					cout << "\t\tUnesite Datum Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					getline(cin, tempKorisnik.DatumRodjenja); vratiBoju();
					lokacija++;
					break;
				}
				case 4:
				{
					cout << "\t\tUnesite Adresu Stanovanja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(tempKorisnik.AdresaStanovanja, 40); vratiBoju();
					lokacija++;
					break;
				}
				case 5:
				{
					cout << "\t\tUnesite Broj Telefona: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(tempKorisnik.BrojTelefona, 20); vratiBoju();
					lokacija++;
					break;
				}
			}
			if(lokacija == 7)
				break;
			
		}
		cout << "\n\n\t\tPonoviti Unos? NE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- DA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();

		key = _getch();
			if(key == 27)
				break;
	}
	Korisnici.push_back(tempKorisnik);
}

unosKorisnika(pFunkcija pGrafik)
{
	mainUlaz:
	int velicina = Korisnici.size();
	KORISNIK tempKorisnik;
	string tempString = "";
	char key;
	
	char const* SPOL[4];
	SPOL[0] = "Muski";
	SPOL[1] = "Zenski";
	SPOL[2] = "Drugo";
	SPOL[3] = "Radije ne bi Rekli";
	LOKACIJA = "1.1.1.0";
	
	int odabir = 0, lokacija = 0;
	
	tempKorisnik.ID = velicina+1;
	tempKorisnik.Ime[0] = NULL;
	tempKorisnik.Prezime[0] = NULL;
	tempKorisnik.Spol[0] = NULL;
	tempKorisnik.Dan = "";
	tempKorisnik.Mje = "";
	tempKorisnik.God = "";
	tempKorisnik.DatumRodjenja = "";
	tempKorisnik.AdresaStanovanja[0] = NULL;
	tempKorisnik.BrojTelefona[0] = NULL;

	string ime, prezime, spol, datum, adresa, brojtel, dan, mje, god;
	char keyCheck;
	int tempNum;
	while(true)
	{
		system("CLS");
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "\tUnos Korisnika:\n\n"; vratiBoju();
		cout << "\tNAZAD "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- NAPRIJED "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();
		cout << "\tUnos "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << velicina + 1; vratiBoju();
		cout << ". Korisnika"; 
		
		cout << "\n\n\t\t"; 
		if(lokacija == 0){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseno Ime: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Ime[0] != NULL) cout << tempKorisnik.Ime;
		else							cout << "N/A";
		vratiBoju();

		cout << "\n\t\t";
		if(lokacija == 1){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseno Prezime: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Prezime[0] != NULL) cout << tempKorisnik.Prezime;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t";
		if(lokacija == 3){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }	
		cout << "\tUneseni Spol: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Spol[0] != NULL) cout << tempKorisnik.Spol;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t";
		if(lokacija == 4){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseni Dan Rodjenja: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Dan != "") cout << tempKorisnik.Dan;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t";
		if(lokacija == 5){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseni Mjesec Rodjenja: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Mje != "") cout << tempKorisnik.Mje;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t";
		if(lokacija == 6){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUnesena Godina Rodjenja: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.God != "") cout << tempKorisnik.God;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t"; 
		if(lokacija == 7){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUnesena Adresa Stanovanja: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.AdresaStanovanja[0] != NULL) cout << tempKorisnik.AdresaStanovanja;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t"; 
		if(lokacija == 8){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseni Broj Telefona: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.BrojTelefona[0] != NULL) cout << tempKorisnik.BrojTelefona;
		else							cout << "N/A";
		vratiBoju();
		cout << "\n\n";
		if(lokacija == 0)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "1"; vratiBoju();
			cout << "/8) Unesite Ime: ";
		}
			
		if(lokacija == 1)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "2"; vratiBoju();
			cout << "/8) Unesite Prezime: ";
		}
			
		if(lokacija == 2)
			odabir = selekcijaLogika(SPOL, 4, LOKACIJA, "SPOL", printajNULL);
		
		if(lokacija == 3)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "3"; vratiBoju();
			cout << "/8) Unesite Spol: ";
		}
			
		if(lokacija == 4)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "4"; vratiBoju();
			cout << "/8) Unesite Dan Rodjenja: ";
		}
			
		if(lokacija == 5)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "5"; vratiBoju();
			cout << "/8) Unesite Mjesec Rodjenja: ";
		}		
			
		if(lokacija == 6)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "6"; vratiBoju();
			cout << "/8) Unesite Godinu Rodjenja: ";
		}	
			
		if(lokacija == 7)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "7"; vratiBoju();
			cout << "/8) Unesite Adresu Stanovanja: ";
		}	
			
		if(lokacija == 8)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "8"; vratiBoju();
			cout << "/8) Unesite Broj Telefona: ";
		}	
			
		if(lokacija == 9)
		{
			cout << "\t\tDa li ste zadovoljni unosom?\n\t\t";
			cout << "\tNE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ESC] "; vratiBoju();
			cout << "- DA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ENTER]\n\n"; vratiBoju();
		}
			

		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(lokacija != 9 && (lokacija < 4 || lokacija > 5))
		{
			cout << tempString;
		}
		if(lokacija == 4)
		{
			cout << tempString;
			tempNum = atoi(tempString.c_str());
			if(tempNum > 31 || tempNum < 0)
				cout << "\tDan Nije Validan";
		}
		if(lokacija == 5)
		{
			cout << tempString;
			tempNum = atoi(tempString.c_str());
			if(tempNum > 12 || tempNum < 0)
				cout << "\tMjesec Nije Validan";
		}
			
		vratiBoju();
		
		if(lokacija == 2)
		{
			if(odabir != -1)
			{
				lokacija++;
				if(odabir == 1)
				{
					strcpy(tempKorisnik.Spol, "Muski");
					if(tempKorisnik.Spol[0] == NULL)
						tempString = "";
					else if(lokacija == 3)
						tempString = "Muski";
						
				}
				else if(odabir == 2)
				{
					strcpy(tempKorisnik.Spol, "Zenski");
					if(tempKorisnik.Spol[0] == NULL)
						tempString = "";
					else if(lokacija == 3)
						tempString = "Zenski";
				}
				else if(odabir == 3)
				{
					if(tempKorisnik.Spol[0] == NULL)
						tempString = "";
					else if(lokacija == 3)
						tempString = spol;
				}
				else if(odabir == 4)
				{
					strcpy(tempKorisnik.Spol, "Radije ne bi Rekli");
					if(tempKorisnik.Spol[0] == NULL)
						tempString = "";
					else if(lokacija == 3)
						tempString = "Radije ne bi Rekli";
				}
			}
			if(odabir == -1)
			{
				tempString = prezime;
				lokacija--;
			}		
		}
		else
		{
			key = _getch();

			if(key == 27)
			{
				if(lokacija == 0)
					break;
				else
				{
					if(lokacija == 1)
						tempString = ime;
					if(lokacija == 4)
						tempString = spol;
					if(lokacija == 5)
						tempString = dan;
					if(lokacija == 6)
						tempString = mje;
					if(lokacija == 7)
						tempString = god;
					if(lokacija == 8)
						tempString = adresa;
					if(lokacija == 9)
						tempString = brojtel;
					if(lokacija == 10)
						tempString = brojtel;
					lokacija--;
				}
			}
			else if((key == 8 && odabir != 1 && odabir != 2 && odabir != 4) || key == 8 && (lokacija >= 4 ))
			{
				if(tempString.size() != 0)
					tempString.pop_back();
			}
			else if(key == 13)
			{
				if(lokacija == 0)
				{
					strcpy(tempKorisnik.Ime, tempString.c_str());
					ime = tempString;
					
				}
				if(lokacija == 1)
				{
					strcpy(tempKorisnik.Prezime, tempString.c_str());
					prezime = tempString;
				}
				
				if(lokacija == 3)
				{
					strcpy(tempKorisnik.Spol, tempString.c_str());
					spol = tempString;
				}
				
				if(lokacija == 4)
				{
					tempNum = atoi(tempString.c_str());
					if(tempNum <= 31 && tempNum >= 0)
					{
						tempKorisnik.Dan = tempString;
						dan = tempString;
					}
					else
						lokacija--;
				}
				
				if(lokacija == 5)
				{
					tempNum = atoi(tempString.c_str());
					if(tempNum <= 12 && tempNum >= 0)
					{
						tempKorisnik.Mje = tempString;
						mje = tempString;
					}
					else
						lokacija--;	
				}
				
				if(lokacija == 6)
				{
					tempKorisnik.God = tempString;
					god = tempString;	
				}
				
				if(lokacija == 7)
				{
					strcpy(tempKorisnik.AdresaStanovanja, tempString.c_str());
					adresa = tempString;	
				}
				
				if(lokacija == 8)
				{
					strcpy(tempKorisnik.BrojTelefona, tempString.c_str());
					brojtel = tempString;
				}
				
				if(lokacija == 9)
				{
					time_t sad;
					
					sad = time(NULL); //uzme vrijeme od OS
					tempKorisnik.VrijemeUclanjivanja = *localtime(&sad);
					
					Korisnici.push_back(tempKorisnik);
					pKorisniciFile->open("KorisniciData.csv", ios::app);
					
					//	struct KORISNIK
					//	{
					//		int ID;
					//		tm VrijemeUclanjivanja;
					//		char Ime[20];
					//		char Prezime[30];
					//		char Spol[20];
					//		string Dan, Mje, God;
					//		string DatumRodjenja;
					//		int Dob;
					//		char AdresaStanovanja[40];
					//		char BrojTelefona[20];
					//	};
					
					*pKorisniciFile << " " << "," << tempKorisnik.ID << ","
									<< tempKorisnik.VrijemeUclanjivanja.tm_mday << ","
									<< tempKorisnik.VrijemeUclanjivanja.tm_mon + 1<< ","
									<< tempKorisnik.VrijemeUclanjivanja.tm_year + 1900<< ","
									<< tempKorisnik.VrijemeUclanjivanja.tm_hour << ","
									<< tempKorisnik.VrijemeUclanjivanja.tm_min << ","
									<< tempKorisnik.VrijemeUclanjivanja.tm_sec << ","
									<< tempKorisnik.Ime << ","
									<< tempKorisnik.Prezime << ","
									<< tempKorisnik.Spol << ","
									<< tempKorisnik.Dan << ","
									<< tempKorisnik.Mje << ","
									<< tempKorisnik.God << ","
									<< tempKorisnik.AdresaStanovanja << ","
									<< tempKorisnik.BrojTelefona << endl;
					pKorisniciFile->close();
					break;
				}
				
				if(tempKorisnik.Prezime[0] == NULL && lokacija == 1)
					tempString = "";
				else if(lokacija == 0)
					tempString = prezime;

				if(tempKorisnik.Dan == "" && lokacija == 4)
					tempString = "";
				else if(lokacija == 3)
					tempString = dan;
					
				if(tempKorisnik.Mje == "" && lokacija == 5)
					tempString = "";
				else if(lokacija == 4)
					tempString = mje;
					
				if(tempKorisnik.God == "" && lokacija == 6)
					tempString = "";
				else if(lokacija == 5)
					tempString = god;
					
				if(tempKorisnik.AdresaStanovanja[0] == NULL && lokacija == 7)
					tempString = "";
				else if(lokacija == 6)
					tempString = adresa;
				
				if(tempKorisnik.BrojTelefona[0] == NULL && lokacija == 8)
					tempString = "";
				else if(lokacija == 7)
					tempString = brojtel;
				
				lokacija++;
			}
			else if(!isalpha(key))
			{
				if(lokacija == 4)
				{

				}
				tempString += key;
			}
			else if((lokacija < 4 || lokacija > 6) && lokacija != 8)
			{
				if(!(odabir != 3 && lokacija == 3))
					tempString += key;
			}
		}
	}
	if(lokacija == 0)
	{
		cout << "\n\n\t\tDa li zelite izaci iz unosa Korisnika?\n\t\t";
		cout << "\tDA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- NE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();
	}
	else if(lokacija == 9)
	{
		cout << "\n\n\t\tDa li zelite unijeti sljedeceg Korisnika?\n\t\t";
		cout << "\tNE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- DA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();
	}
	
		if(lokacija != 7)
		{
			key = _getch();
			if(key == 27)
				goto izlaz;
			else if(key == 13)
				goto mainUlaz;
		}
	
	izlaz:{}
}

void prikaziKorisnike()
{
	char key;
	
	string DATUM, VRIJEME;
	while(true)
	{
		while(true)
		{
			time_t sad;
			sad = time(NULL); //uzme vrijeme od OS
			noviLokal = *localtime(&sad);
			
			string datum = "", god = "";
			system("CLS");
			cout << "Pregled Korisnika: \n\n";
			cout << "Trenutni Datum: ";
			odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			if(pGlobalPOSTAVKE->tipFormatDatuma)
			{
				cout << noviLokal.tm_mday << "/" << noviLokal.tm_mon + 1 << "/" << noviLokal.tm_year + 1900;
			}
			else
			{
				cout << noviLokal.tm_mon + 1 << "/" << noviLokal.tm_mday << "/" << noviLokal.tm_year + 1900;
			}
			vratiBoju();
			cout << "\nTrenutno Vrijeme: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << noviLokal.tm_hour << ":" << noviLokal.tm_min << ":" << noviLokal.tm_sec; vratiBoju();
			cout << "\nPritisnite "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ENTER]";	vratiBoju();
			cout << " da "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "Osvjezite Vrijeme";	vratiBoju();
				cout << "\n\n";
			//_sleep(500);
			
			int velicina = pKorisnici->size();
			for(int i = 0; i < velicina; i++)
			{
				god = Korisnici[i].God;
				
				if(god != "")
				Korisnici[i].Dob = noviLokal.tm_year + 1900 - atoi(god.c_str());
				else
					Korisnici[i].Dob = NULL;
				
				cout << "ID: " << Korisnici[i].ID;
				cout << "\nDatum Uclanjivanja: ";
				if(pGlobalPOSTAVKE->tipFormatDatuma)
				{
					cout << Korisnici[i].VrijemeUclanjivanja.tm_mday << "/" << Korisnici[i].VrijemeUclanjivanja.tm_mon + 1 << "/" << Korisnici[i].VrijemeUclanjivanja.tm_year + 1900;
				}
				else
				{
					cout << Korisnici[i].VrijemeUclanjivanja.tm_mon + 1 << "/" << Korisnici[i].VrijemeUclanjivanja.tm_mday << "/" << Korisnici[i].VrijemeUclanjivanja.tm_year + 1900;
				}
				cout << "\nVrijeme Uclanjivanja: ";
				cout << Korisnici[i].VrijemeUclanjivanja.tm_hour << ":" << Korisnici[i].VrijemeUclanjivanja.tm_min << ":" << Korisnici[i].VrijemeUclanjivanja.tm_sec;
				cout << "\nIme: ";
				if(Korisnici[i].Ime[0] != NULL)			cout << Korisnici[i].Ime;
				else									cout << "N/A";
				cout << "\nPrezime: ";
				if(Korisnici[i].Prezime[0] != NULL)		cout << Korisnici[i].Prezime;
				else									cout << "N/A";
				cout << "\nSpol: " << Korisnici[i].Spol;
				cout << "\nDatum Rodjenja: ";
				if(Korisnici[i].Dan != "" || Korisnici[i].Mje != "" || Korisnici[i].God != "")
				{
					if(pGlobalPOSTAVKE->tipFormatDatuma)
					{
						cout << Korisnici[i].Dan << "/" << Korisnici[i].Mje << "/" << Korisnici[i].God;
					}
					else
					{
						cout << Korisnici[i].Mje << "/" << Korisnici[i].Dan << "/" << Korisnici[i].God;
					}
				}
				else	cout << "N/A";
				
				cout << "\nDob: ";
				if(Korisnici[i].Dob != NULL)				cout << Korisnici[i].Dob;
				else									cout << "N/A";
				cout << "\nAdresa Stanovanja: ";
				if(Korisnici[i].AdresaStanovanja[0] != NULL)	cout << Korisnici[i].AdresaStanovanja;
				else											cout << "N/A";
				cout << "\nBroj Telefona: ";
				if(Korisnici[i].BrojTelefona[0] != NULL)	cout << Korisnici[i].BrojTelefona;
				else										cout << "N/A";
				cout << endl << endl << endl;
			}
			cout << "NAZAD [ESC]";

			key = _getch();
			if(key == 27){goto izlaz;}

		}
	}
	izlaz:{}
}

int main()
{
	checkPostavkepostavkeFile.open("PostavkeData.csv");
	inPostavkepostavkeFile.open("PostavkeData.csv");
	
	postavkeFile.open("PostavkeData.csv", ios::app);
	
	if(!(checkPostavkepostavkeFile.good()))
	{
		*pPostavkeFile << "Postavke,Boja,Tip Selekcije,Format Datuma,Prikazi Grafik" << endl;
		*pPostavkeFile << " " << "," << pGlobalPOSTAVKE->bojaReal
			   << "," << pGlobalPOSTAVKE->tipSelekcije 
			   << "," << pGlobalPOSTAVKE->tipFormatDatuma
			   << "," << pGlobalPOSTAVKE->prikaziGrafik << endl;
	}
	else
		parsePOSTAVKE();
		
	checkKorisniciFile.open("KorisniciData.csv");
	inKorisniciFile.open("KorisniciData.csv");
	korisniciFile.open("KorisniciData.csv", ios::app);

	if(!(checkKorisniciFile.good()))
	{
		*pKorisniciFile << "Korisnici,ID,D.uc.,Mj.uc.,G.uc.,H.uc.,M.uc.,S.uc.,Ime,Prezime,Spol,D.rod.,Mj.rod.,G.rod.,Adresa st.,Broj tel." << endl;
	}
	else
		parseKORISNICI();
	
	int brojOpcija = 7;
	int odabir;
	do
	{
		odabir = meni(brojOpcija);
		switch(odabir)
		{
			case 1:
			{
				pKorisniciFile->close();
				LOKACIJA = "1.1.0.0";
				char const* UNOS[2];
				UNOS[0] = "Classic";
				UNOS[1] = "Modern";
				
				int choice;
				choice = selekcijaLogika(UNOS, 2, LOKACIJA, "UNOS KORISNIKA: VRSTA UNOSA:", printajNULL);
				if(choice == 1)
					unosKorisnikaClassic(printajTeg);
				else if(choice == 2)
					unosKorisnika(printajTeg);
				else if(choice == -1)
				{}
				pKorisniciFile->open("KorisniciData.csv", ios::app);
				break;
			}
			case 2:
			{
				prikaziKorisnike();
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
				pPostavkeFile->close();
				POSTAVKEPromjena();
				pPostavkeFile->open("PostavkeData.csv", ios::app);
				break;
			}
			default:
			{
				
				break;
			}
	}
	}while(odabir != brojOpcija);
	
	checkPostavkepostavkeFile.close();
	pInPostavkepostavkeFile->close();
	pPostavkeFile->close();
	system("PAUSE");
	return 0;
}

int meni(int brojOpcija)
{
	char const* POSTAVKE[brojOpcija];
	POSTAVKE[0] = "Unos Korisnika";
	POSTAVKE[1] = "Pregled Korisnika";
	POSTAVKE[2] = "Opcija 3";
	POSTAVKE[3] = "Opcija 4";
	POSTAVKE[4] = "Opcija 5";
	POSTAVKE[5] = "Postavke";
	POSTAVKE[6] = "EXIT";	
	LOKACIJA = "1.0.0.0";
	selekcijaLogika(POSTAVKE, brojOpcija, LOKACIJA, "MENI:", printajNaslov);
}

void POSTAVKEPromjena()
{
	ulaz:
	LOKACIJA = "1.6.0.0";
	int brojOpcija = 6, key = 0, odabir = 1;
	char const* POSTAVKE[brojOpcija];
	POSTAVKE[0] = "Promjeni Boju";
	POSTAVKE[1] = "Promjeni Nacin Unosa";
	POSTAVKE[2] = "Promjeni Format Datuma";
	POSTAVKE[3] = "Prikazi Grafik";
	POSTAVKE[4] = "Sacuvaj Postavke";
	POSTAVKE[5] = "FACTORY RESET";
	
	char const* POSTAVKE1[2];
	POSTAVKE1[0] = "Unos Preko ARROW KEYS";
	POSTAVKE1[1] = "Unos Preko BROJEVA";
	
	char const* DATUM[2];
	DATUM[0] = "DD/MM/GGGG";
	DATUM[1] = "MM/DD/GGGG";
	
	char const* RESET[2];
	RESET[0] = "DA";
	RESET[1] = "NE";
	
	char const* GRAFIK[2];
	GRAFIK[0] = "PRIKAZI GRAFIK";
	GRAFIK[1] = "NE PRIKAZI GRAFIK";
	
	int choice = selekcijaLogika(POSTAVKE, brojOpcija, LOKACIJA, "POSTAVKE:", printajNULL);
	if(choice == -1) goto izlaz;
		
	switch(choice)
	{
		
		case 1:
		{
			case1:
			LOKACIJA = "1.6.1.0";
			int odabir1 = pGlobalPOSTAVKE->bojaReal+1, key1 = 0, boja;
			int odabir2 = pGlobalPOSTAVKE->bojaReal+1;
			if(pGlobalPOSTAVKE->tipSelekcije)
			{
				while(key1 != 13)
				{
					boja = odabir1 - 1;
					system("CLS");
					SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
					cout << "\tODABERITE BOJU:";
					cout << "\n\n"; SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
					cout << "\t[<-]"; SetConsoleTextAttribute(h, boja);
					cout << "   (" << boja << "/15) Primjer TEXTA  "; SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
					cout << "[->]\n\n";
					SetConsoleTextAttribute(h, boja);
					printajGrafik();
					printajTeg();
					SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
					cout << "\n";
					cout << "\tNAZAD [ESC]\n";
					
					unsaved(LOKACIJA);
					
					key1 = getch();
				
					if(key1 == 77) odabir1++;
					else if(key1 == 75) odabir1--;
	
					if(odabir1 > 16) odabir1 = 2;
					else if(odabir1 < 2) odabir1 = 16;
				
					if(key1 == 13)
					{
						pGlobalPOSTAVKE->bojaReal = boja;
						
						goto case1;
						break;
					}
					if(key1 == 27)
					{
						if(pTempGlobalnePOSTAVKE->bojaReal == pGlobalPOSTAVKE->bojaReal)
							*pSacuvano = true;
						goto ulaz;
					}
				}
			}
			else
			{
				int i;
				do
				{
					system("CLS");
					SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
					cout << "\tODABERITE BOJU:\n\n";
					printajGrafik();
					cout << "\n\n";
					for(i = 0; i < 8; i++)
					{
						SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
						
						cout << "\t" << i + 1 << ".\t"; SetConsoleTextAttribute(h, i+1);
						
						cout << "PRIMJER TEXTA";
						if(i == pGlobalPOSTAVKE->bojaReal - 1)
							cout << "\t" << BAR;
						else
							cout << "\t     ";
						//cout << "\t\t";
						if(i > 6)
							break;
						SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
						
						cout << "\t" << i + 1 + 8 << ".\t"; SetConsoleTextAttribute(h, i+9);
						cout << "PRIMJER TEXTA";
						if(i+8 == pGlobalPOSTAVKE->bojaReal - 1)
							cout << "\t" << BAR;
						else
							cout << "\t     ";
						cout << "\n";
					}
					SetConsoleTextAttribute(h, pGlobalPOSTAVKE->bojaReal);
					cout << "\t" << 2*i+2 << ".\tNAZAD\n";
					if(cin.fail() || odabir2 < 1 || odabir2 > 15)
					{
						cin.clear();
						cin.ignore(1000, '\n');
					}
					unsaved(LOKACIJA);
					cout << "\n\t";
					cin >> odabir2;	
					if(odabir2 == 16) break;
				}while(cin.fail() || odabir2 < 1 || odabir2 > 15);
				
				if(odabir2 == 16) goto ulaz;
				else if(odabir2 != 16)
				{
					pGlobalPOSTAVKE->bojaReal = odabir2;
					goto case1;
				}
			}
			break;		
		}
		case 2:
		{
			LOKACIJA = "1.6.2.0";
			int odabirTipSelekcije;
			while(true)
			{
				odabirTipSelekcije = selekcijaLogika(POSTAVKE1, 2, LOKACIJA, "NACIN UNOSA:", printajNULL); 
				if(odabirTipSelekcije == -1) goto ulaz;
				if(odabirTipSelekcije == 1) pGlobalPOSTAVKE->tipSelekcije = true;
				else pGlobalPOSTAVKE->tipSelekcije = false;
			}
			break;
		}
		case 3:
		{
			char const* tempDatum;
			int odabirFormatDatuma;
			LOKACIJA = "1.6.3.0";
			while(true)
			{
				if(pGlobalPOSTAVKE->tipFormatDatuma) tempDatum = "TRENUTNI FORMAT: DD/MM/GGGG";
				else tempDatum = "TRENUTNI FORMAT: MM/DD/GGGG";
				odabirFormatDatuma = selekcijaLogika(DATUM, 2, LOKACIJA, tempDatum, printajNULL);
				if(odabirFormatDatuma == -1) goto ulaz;
				if(odabirFormatDatuma == 1)	pGlobalPOSTAVKE->tipFormatDatuma = true;
				else pGlobalPOSTAVKE->tipFormatDatuma = false;
			}
			break;
		}
		case 4:
		{
			char const* tempGrafik;
			int odabirPrikaziGrafik;
			LOKACIJA = "1.6.4.0";
			while(true)
			{
				if(pGlobalPOSTAVKE->prikaziGrafik) tempGrafik = "FORMAT: PRIKAZUJE";
				else tempGrafik = "FORMAT: NE PRIKAZUJE";
				odabirPrikaziGrafik = selekcijaLogika(GRAFIK, 2, LOKACIJA, tempGrafik, printajNULL);
				if(odabirPrikaziGrafik == -1) goto ulaz;
				if(odabirPrikaziGrafik == 1) pGlobalPOSTAVKE->prikaziGrafik = true;
				else pGlobalPOSTAVKE->prikaziGrafik = false;
			}
			break;
		}
		case 5:
		{
			LOKACIJA = "1.6.5.0";
			if(*pSacuvano == true)
				cout << "\n\tNema Promjena. [ENTER]\n";
			else
			{
				pPostavkeFile->open("PostavkeData.csv", ios::trunc);
				pPostavkeFile->close();
				pPostavkeFile->open("PostavkeData.csv", ios::app);
				*pPostavkeFile << "Postavke,Boja,Tip Selekcije,Format Datuma,Prikazi Grafik" << endl;
				*pPostavkeFile << " " << "," << pGlobalPOSTAVKE->bojaReal
			   	<< "," << pGlobalPOSTAVKE->tipSelekcije 
			   	<< "," << pGlobalPOSTAVKE->tipFormatDatuma
			   	<< "," << pGlobalPOSTAVKE->prikaziGrafik << endl;
				pPostavkeFile->close();
				
				cout << "\n\tPostavke Sacuvane!\n";
				cout << "\tPromjene: \n\n";
				
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				
				if(pTempGlobalnePOSTAVKE->bojaReal != pGlobalPOSTAVKE->bojaReal)
					cout << "\t\tBoja\n";
				if(pTempGlobalnePOSTAVKE->tipSelekcije != pGlobalPOSTAVKE->tipSelekcije)
					cout << "\t\tNacin Unosa\n";
				if(pTempGlobalnePOSTAVKE->tipFormatDatuma != pGlobalPOSTAVKE->tipFormatDatuma)
					cout << "\t\tFormat Datuma\n";
				if(pTempGlobalnePOSTAVKE->prikaziGrafik != pGlobalPOSTAVKE->prikaziGrafik)
					cout << "\t\tPrikaz Grafika\n";
				
				if(pGlobalPOSTAVKE->tipSelekcije == true)
					vratiBoju();
				else
					odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << "\n\t[ENTER]";
				
				pTempGlobalnePOSTAVKE->bojaReal = pGlobalPOSTAVKE->bojaReal;
				pTempGlobalnePOSTAVKE->tipSelekcije = pGlobalPOSTAVKE->tipSelekcije;
				pTempGlobalnePOSTAVKE->tipFormatDatuma = pGlobalPOSTAVKE->tipFormatDatuma;
				pTempGlobalnePOSTAVKE->prikaziGrafik = pGlobalPOSTAVKE->prikaziGrafik;
				
				*pSacuvano = true;
			}
			char key = _getch();
			if(key == 13)
				goto ulaz;
		}
		case 6:
		{
			char const* tempReset = "VRACA SVE POSTAVKE NA TVORNICKE. DA LI STE SIGURNI?";
			LOKACIJA = "1.6.6.0";
			int reset = selekcijaLogika(RESET, 2, LOKACIJA, tempReset, printajNULL);
			if(reset == 1)
			{
				pGlobalPOSTAVKE->bojaReal = 9;
				pGlobalPOSTAVKE->tipSelekcije = true;
				pGlobalPOSTAVKE->tipFormatDatuma = true;
				pGlobalPOSTAVKE->prikaziGrafik = true;
				
				pPostavkeFile->open("PostavkeData.csv", ios::trunc);
				pPostavkeFile->close();
				pPostavkeFile->open("PostavkeData.csv", ios::app);
				*pPostavkeFile << "Postavke,Boja,Tip Selekcije,Format Datuma,Prikazi Grafik" << endl;
				*pPostavkeFile << " " << "," << pGlobalPOSTAVKE->bojaReal
			   	<< "," << pGlobalPOSTAVKE->tipSelekcije 
			   	<< "," << pGlobalPOSTAVKE->tipFormatDatuma
			   	<< "," << pGlobalPOSTAVKE->prikaziGrafik << endl;
				pPostavkeFile->close();
				
				pTempGlobalnePOSTAVKE->bojaReal = pGlobalPOSTAVKE->bojaReal;
				
				break;
			}
			else
				goto ulaz;
		}
		default:
		{
			break;
		}
		izlaz:{}
				
	}
}

