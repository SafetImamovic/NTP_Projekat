//sistem za uclanivanje u sportski klub ili teretuanu, odredjivanje termina treninga itd
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <thread>
#include <chrono>
#include <iomanip>
#include "Includes/grafici.h" // putanja do header fajla koji sadrzi ASCII grafike koje smo koristili
#include <string>
#include <ctime>
#include <map>
using namespace std;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);//windows handle koji kontrolise output stream, HANDLE je data tip koji 
											//predstavlja windows API i koristi se za pristup mnogim sistemskim resursima

struct tm; //struktura koja sadrzi vremneske atribute kao sati, minute, sekunde, datum itd.
tm noviLokal;

struct POSTAVKE
{
	int bojaReal = 9, boja;
	bool tipSelekcije = true;
	bool tipFormatDatuma = true;
	bool prikaziGrafik = true;
};

struct PAKETI
{
	int ID;
	string Ime;
	int BrojSedmicnihSesija;
	int CijenaPoSesiji;
	int UkupnaCijena;
};

struct DETALJI
{
	bool Placeno;
	float Razlika;
	int brPlacenihMjeseci;
	vector<PAKETI> PlaceniPaketi;
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
	DETALJI Evidencija;
};

vector<KORISNIK> Korisnici; //vektor svih korinsika u sistemu
vector<KORISNIK>* pKorisnici = &Korisnici; //pointer na vektor korisnika, pointeri se koriste da se globalno mijenja

POSTAVKE globalPOSTAVKE; //objekat svih postavke
POSTAVKE* pGlobalPOSTAVKE = &globalPOSTAVKE; //pointer na to
POSTAVKE tempGlobalnePOSTAVKE; //temporalne postavke
POSTAVKE* pTempGlobalnePOSTAVKE = &tempGlobalnePOSTAVKE; //odgovarajuci pointer

vector<PAKETI> Paketi; //vektor svih paketa
vector<PAKETI>* pPaketi = &Paketi; //pointer na vektor paketa

void odabranaBoja(int bojaReal){SetConsoleTextAttribute(h, bojaReal);} //ova funkcija mijenja nacin bojenja konzole
void vratiBoju(){SetConsoleTextAttribute(h, 15);} // funkcija vraca na originalnu boju tj bijelu, znaci svaki print ili cout 
												//Izmedju ove dvije funkcije ce biti obojen odredjenom bojom. Bijela boja == 15

void PrikaziKursor(bool showFlag)
{
    CONSOLE_CURSOR_INFO cursorInfo; //daje informacije u kursoru/selektoru
    GetConsoleCursorInfo(h, &cursorInfo);
    cursorInfo.bVisible = showFlag; // postavi vidljivost kursora/selektora
    SetConsoleCursorInfo(h, &cursorInfo);
}

char const* LOKACIJA = "1.0.0.0";
//lokacija = 1, Meni lokacija = 1.1, prva opcija lokacija = 1.2, druga opcija itd lokacija = 1.1.1, prva opcija unutar prve opcije itd.

bool sacuvano = true; //Provjera da li su sacuvane postavke
bool* pSacuvano = &sacuvano; //Pointer na provjeru

void unsaved(char const* lokacija)
{
	if((pGlobalPOSTAVKE->bojaReal != pTempGlobalnePOSTAVKE->bojaReal && lokacija == "11.1.0.0")
	||(pGlobalPOSTAVKE->tipSelekcije != pTempGlobalnePOSTAVKE->tipSelekcije && lokacija == "11.2.0.0")
	||(pGlobalPOSTAVKE->tipFormatDatuma != pTempGlobalnePOSTAVKE->tipFormatDatuma && lokacija == "11.3.0.0")
	||(pGlobalPOSTAVKE->prikaziGrafik != pTempGlobalnePOSTAVKE->prikaziGrafik && lokacija == "11.4.0.0"))//provjerava da li su postavke promijenjene na svojim originalnim lokacijama
	{
		cout << "\n\tPromjenu Postavke\n\tTreba Sacuvati.\n\tINDIKATOR ";
		if(pGlobalPOSTAVKE->bojaReal != 4)//ako boja nije crvena, boja za indikator sacuvanja promjene ce biti crvena
			odabranaBoja(4);
		else	//ako je crvena boja indikatora ce biti zelena
			odabranaBoja(2);
		cout << BAR;
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "\n";
		*pSacuvano = false;
	}
	else if((pGlobalPOSTAVKE->bojaReal == pTempGlobalnePOSTAVKE->bojaReal && lokacija == "11.1.0.0")
	||(pGlobalPOSTAVKE->tipSelekcije == pTempGlobalnePOSTAVKE->tipSelekcije && lokacija == "11.2.0.0")
	||(pGlobalPOSTAVKE->tipFormatDatuma == pTempGlobalnePOSTAVKE->tipFormatDatuma && lokacija == "11.3.0.0")
	||(pGlobalPOSTAVKE->prikaziGrafik == pTempGlobalnePOSTAVKE->prikaziGrafik && lokacija == "11.4.0.0"))
		*pSacuvano = true;
}

int meni(int brojOpcija);

void POSTAVKEPromjena();

typedef void (*pFunkcija)();//definisan tip podatka kao pointer na funckiju koja nema arg. i return tip void

fstream checkPostavkepostavkeFile; //tip podatka za provjeru fajla, da li vec postoji
ifstream inPostavkepostavkeFile; //cita podatke sa csv fajla
ifstream* pInPostavkepostavkeFile = &inPostavkepostavkeFile; //pointer na to
ofstream postavkeFile; //pise podatke postavki u fajl
ofstream* pPostavkeFile = &postavkeFile; //pointer na to

fstream checkKorisniciFile;
ifstream inKorisniciFile;
ifstream* pInKorisniciFile = &inKorisniciFile;
ofstream korisniciFile;
ofstream* pKorisniciFile = &korisniciFile;

fstream checkPaketiFile;
ifstream inPaketiFile;
ifstream* pInPaketiFile = &inPaketiFile;
ofstream paketiFile;
ofstream* pPaketiFile = &paketiFile;


void printajNaslov();
void teg();

void parsePOSTAVKE()//funkcija koja snima podatke vezane za postavke iz pohranjene memorije u RAM tj. iz csv fajla za postavke
{
	string header;
    getline(*pInPostavkepostavkeFile, header); //ignorise prvi red fajla jer su to naslovi
    
    string line; //posto je csv fajl "Comma seperated file", celije su odvojene zarezima, pa ce granice za getline biti ','
    getline(*pInPostavkepostavkeFile, line, ','); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->bojaReal = atoi(line.c_str()); pTempGlobalnePOSTAVKE->bojaReal = atoi(line.c_str()); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->tipSelekcije = atoi(line.c_str()); pTempGlobalnePOSTAVKE->tipSelekcije = atoi(line.c_str()); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->tipFormatDatuma = atoi(line.c_str()); pTempGlobalnePOSTAVKE->tipFormatDatuma = atoi(line.c_str()); line = "";
	getline(*pInPostavkepostavkeFile, line, ','); pGlobalPOSTAVKE->prikaziGrafik = atoi(line.c_str()); pTempGlobalnePOSTAVKE->prikaziGrafik = atoi(line.c_str()); line = "";
}

void parseKORISNICI()//funkcija koja snima podatke vezane za korisnike iz pohranjene memorije u RAM tj. iz csv fajla za korisnike
{
	KORISNIK tempKorisnik;
	
	string header; 
	getline(*pInKorisniciFile, header); //ignorise prvi red kao kod postavki
	
	string line;
	while(getline(*pInKorisniciFile, line)) //sve dok cita iz fajla obavlja donji block
	{
		string ID, d, mj, g, h, m, s, ime, prezime, spol, dan, mje, god, adresa, brojtel;
		string tempString, discard;
		
		stringstream inputString(line);//stringstream je klasa koja nam dopusta da u sustini koristimo string kao i/o stream, kao cin itd.
		getline(inputString, discard, ',');
		getline(inputString, ID, ',');
		
		tempKorisnik.ID = atoi(ID.c_str());
		
		getline(inputString, d, ','); getline(inputString, mj, ','); getline(inputString, g, ',');
		getline(inputString, h, ','); getline(inputString, m, ','); getline(inputString, s, ',');
		
		//dodjeljuje vrijeme i datum temporalnom objektu iz fajla
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
		
		Korisnici.push_back(tempKorisnik); //postavlja taj objekat u glavnu strukturu za korisnike
		line = "";
	}
}

void parsePAKETI()
{
	PAKETI tempPaketi;
	
	string header;
	getline(*pInPaketiFile, header);
	
	string line;
	while(getline(*pInPaketiFile, line))
	{
		string ID, ime, brojSsesija, cijenaPoS, ukupnaCijena;
		string tempString, discard;
		
		stringstream inputString(line);
		getline(inputString, discard, ',');
		getline(inputString, ID, ',');
		
		tempPaketi.ID = atoi(ID.c_str());
		
		getline(inputString, ime, ',');
		getline(inputString, brojSsesija, ',');
		getline(inputString, cijenaPoS, ',');
		getline(inputString, ukupnaCijena, ',');
		
		tempPaketi.Ime=ime;
		tempPaketi.BrojSedmicnihSesija= atoi(brojSsesija.c_str());
		tempPaketi.CijenaPoSesiji= atoi(cijenaPoS.c_str());
		tempPaketi.UkupnaCijena= atoi(ukupnaCijena.c_str());
		
		Paketi.push_back(tempPaketi);
		line = "";
	}
}

//ova funkcija se bavi interfejsom za odabir. Ima dva moda: tip selekcije sa strijelicama i sa unosom odabira manuelno
//prvi arg je niz svih opcija, broj opcija, lokacija kao npr. "1.1.0.0", naslov i pointer na funckiju
int selekcijaLogika(char const** OPCIJE, int brojOpcija, char const*& LOKACIJA, char const* naslov, pFunkcija pGrafik)
{
	int odabir = 1;
	if(pGlobalPOSTAVKE->tipSelekcije == true)//ako je true to oznacava tip selekcije sa strijelicama
	{
		int key = 0;
		while(key != 13)//13 predstavlja ASCII kod za ENTER
		{
			system("CLS");// ocisti konzolu
			PrikaziKursor(false);
			odabranaBoja(pGlobalPOSTAVKE->bojaReal); pGrafik();
			vratiBoju();
			cout << "\t(" << odabir << "/" << brojOpcija << ") ";
			cout << naslov << "\n\n";
			int p = 0;
			while(p < brojOpcija)
			{
				if(odabir != p+1){	cout << "\t      " << OPCIJE[p];	}	
				else{	odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << "\t" << BAR << " " << OPCIJE[p];	}
				if(LOKACIJA == "11.0.0.0" && *pSacuvano == false && p == 4 ||
				LOKACIJA == "1.0.0.0" && *pSacuvano == false && p == 10)
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
	else //oznacava tip selekcije unosom odabira
	{
		int i;
		do
		{
			system("CLS");
			PrikaziKursor(true);
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
				cout << "\t" << i + 1 << ".\t" << OPCIJE[i];
				if(LOKACIJA == "11.0.0.0" && *pSacuvano == false && i == 4
				|| LOKACIJA == "1.0.0.0" && *pSacuvano == false && i == 10)
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

//Ova funckija filtrira Korisnike dinamicki na osnovu termina koju korisnik unosi, trazi parcijalnu podudarnost npr.
//ako imamo korisnika sa imenom Hamo, i ako pretrazivac ukuca string Ha, Hamo ce se automatski pojavit jer je Ha podstring od Hamo itd.
//Ovo vazi za sve atribute strukture
vector<KORISNIK>* pretraga(string &termin, string &tempTermin)//povratni tip je pointer na vektor objekata strukture KORISNIK jer on trazi sve objekte koji imaju tu parcijalnu podudarnost i vraca je
{
	vector<KORISNIK>* ptr = new vector<KORISNIK>();//dinamicki alocirani pointer na vektor Korisnika
	time_t sad; //tip podatka koji sadrzi podatke o vremenu i datumu
	sad = time(NULL); //uzme vrijeme od OS
	noviLokal = *localtime(&sad);//prida vremenske podatke objektu
	
	string datum = "", god = "";
	system("CLS");
	cout << "NAZAD "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);// ova funckija mijenja boju kao i prije
	cout << "[ESC] "; vratiBoju();//vraca kao i prije
	
	cout << "\nTrenutni Datum: ";
	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
	//u zavisnosti od formata datuma koji se moze podesit unutar programa, ako je true prikazuje DD/MM/GGGG ili false MM/DD/GGGG
	if(pGlobalPOSTAVKE->tipFormatDatuma)	{cout << noviLokal.tm_mday << "/" << noviLokal.tm_mon + 1 << "/" << noviLokal.tm_year + 1900;}
	else	{cout << noviLokal.tm_mon + 1 << "/" << noviLokal.tm_mday << "/" << noviLokal.tm_year + 1900;}
	vratiBoju();
	cout << "\nTrenutno Vrijeme: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
	cout << noviLokal.tm_hour << ":" << noviLokal.tm_min << ":" << noviLokal.tm_sec; vratiBoju();
	cout << "\nPritisnite "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
	cout << "[ENTER]";	vratiBoju();
	cout << " da "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
	cout << "Osvjezite Vrijeme";vratiBoju();
	cout << "\n\nPregled Korisnika:"; 	 
	cout << "\n\nPretraga: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
	//prikazuje taj pretrazivacki termin
	cout << termin << endl; vratiBoju();
	cout << "----------------------------------------" << endl;
	string ime, prezime, spol, adresa;
	bool imaMatch = false;
	string godRod;
	vector<KORISNIK> KorisniciLower;//vektor tipa korisnik koji cuva sve iste atribute samo sto je sve promijenjeno u mala slova
									//zbog parcijalne podudarnosti. Cim korisnik ukuca termin, stvara se kopija koja je sacinjena malim slovima
									//i taj novi termin se poredi sa malim slovima nove strukture tj. namjestamo da filtrira neovisno o kapitalizaciji
	for(int i = 0; i < Korisnici.size(); i++)
	{
		time_t sad;
					
		sad = time(NULL); //uzme vrijeme od OS
		noviLokal = *localtime(&sad);
		
		godRod = Korisnici[i].God;
					
		if(godRod != "")//racuna dob korisnika
			Korisnici[i].Dob = noviLokal.tm_year + 1900 - atoi(godRod.c_str());
		else
			Korisnici[i].Dob = 0;
		
		KorisniciLower.push_back(Korisnici[i]);
		
		ime = CharArrToString(KorisniciLower[i].Ime);
		prezime = CharArrToString(KorisniciLower[i].Prezime);
		spol = CharArrToString(KorisniciLower[i].Spol);
		adresa = CharArrToString(KorisniciLower[i].AdresaStanovanja);
		//proces koji pretvara svako slovo iz normalnog objekta u malo slovo u novi objekat
		for(int j = 0; j < ime.size(); j++)
			KorisniciLower[i].Ime[j] = tolower(KorisniciLower[i].Ime[j]);
			
		for(int j = 0; j < prezime.size(); j++)
			KorisniciLower[i].Prezime[j] = tolower(KorisniciLower[i].Prezime[j]);
			
		for(int j = 0; j < spol.size(); j++)
			KorisniciLower[i].Spol[j] = tolower(KorisniciLower[i].Spol[j]);
			
		for(int j = 0; j < adresa.size(); j++)
			KorisniciLower[i].AdresaStanovanja[j] = tolower(KorisniciLower[i].AdresaStanovanja[j]);
	}
	
	vector<size_t> Pozicija;//vektor koji cuva size_t tip, to je tip koji se koristi za predstavljanje velicine objekata posto se stringovi mogu tretirati kao vektori
	Pozicija.resize(16);
	vector<size_t> PozicijaNew;
	PozicijaNew.resize(16);
	cout << endl;
	for(int i = 0; i < Korisnici.size(); i++)
	{
		//.find metoda trazi podudarnost termina sa odredjenim stringom i ako pronadje vrati poziciju gdje krece podudarnost
		//ili vrati string::npos, znaci no position, i to cuvamo u vektoru
		Pozicija[0] = to_string(KorisniciLower[i].ID).find(tempTermin);
		Pozicija[1] = to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday).find(tempTermin);
		Pozicija[2] = to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon + 1).find(tempTermin);
		Pozicija[3] = to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_year + 1900).find(tempTermin);
		Pozicija[4] = to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_hour).find(tempTermin);
		Pozicija[5] = to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_min).find(tempTermin);
		Pozicija[6] = to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_sec).find(tempTermin);
		Pozicija[7] = CharArrToString(KorisniciLower[i].Ime).find(tempTermin);
		Pozicija[8] = CharArrToString(KorisniciLower[i].Prezime).find(tempTermin);
		Pozicija[9] = CharArrToString(KorisniciLower[i].Spol).find(tempTermin);
		Pozicija[10] = KorisniciLower[i].Dan.find(tempTermin);
		Pozicija[11] = KorisniciLower[i].Mje.find(tempTermin);
		Pozicija[12] = KorisniciLower[i].God.find(tempTermin);
		Pozicija[13] = to_string(KorisniciLower[i].Dob).find(tempTermin);
		Pozicija[14] = CharArrToString(KorisniciLower[i].AdresaStanovanja).find(tempTermin);
		Pozicija[15] = CharArrToString(KorisniciLower[i].BrojTelefona).find(tempTermin);

		//provjerava da li je neki element prazan, tj da li nepostoji podudarnost stringova.
		//i ako ima podudarnost onda cuva size_t u novi vektor i taj vektor u sebi sadrzi samo
		//elemente koji imaju podudarnost sa terminom
		for(int j = 0; j < 16; j++)
		{
			if(Pozicija[j] != string::npos)
				PozicijaNew[j] = Pozicija[j];
		}

		KorisniciLower[i].VrijemeUclanjivanja.tm_year += 1900;
		KorisniciLower[i].VrijemeUclanjivanja.tm_mon += 1;
		
		//ovaj if poredi da li nije bar jedan od ovih substringova vratila string::npos, tj da ne postoji podudarnost
		if(Pozicija[0] != string::npos || Pozicija[1] != string::npos  || Pozicija[2] != string::npos || Pozicija[3] != string::npos || Pozicija[4] != string::npos ||
		Pozicija[5] != string::npos || Pozicija[6] != string::npos || Pozicija[7] != string::npos || Pozicija[8] != string::npos || Pozicija[9] != string::npos ||
		Pozicija[10] != string::npos || Pozicija[11] != string::npos || Pozicija[12] != string::npos || Pozicija[13] != string::npos || Pozicija[14] != string::npos ||
		Pozicija[15] != string::npos)
		{
			//donji kod formatira ispis tako sto oboji podudaran termin sa odredjenom bojom, jer ima pocetni index podudarnosti, velicinu termina, i velicinu rijeci sa kojom tesira podudarnost
			ptr->push_back(Korisnici[i]); //pohranjuje objekte sa kojim je pronadjena podudarnost i vraca je kasnije u kodu
			cout << "ID: ";
			if(to_string(KorisniciLower[i].ID).find(tempTermin) != string::npos)
			{
				for(int j = 0; j < PozicijaNew[0]; j++)												cout << to_string(KorisniciLower[i].ID)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				for(int j = PozicijaNew[0]; j < PozicijaNew[0] + termin.size(); j++)						cout << to_string(KorisniciLower[i].ID)[j];	vratiBoju();
				for(int j = PozicijaNew[0] + termin.size(); j < to_string(KorisniciLower[i].ID).size(); j++)					cout << to_string(KorisniciLower[i].ID)[j];
			}
			else
				cout << Korisnici[i].ID;
			cout << endl;
			cout << "Datum Uclanjivanja: ";
			if(pGlobalPOSTAVKE->tipFormatDatuma)
			{
				if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[1]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[1]; j < PozicijaNew[1] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday)[j];	vratiBoju();
					for(int j = PozicijaNew[1] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday)[j];
				}
				else
					cout << KorisniciLower[i].VrijemeUclanjivanja.tm_mday;
				cout << "/";
				if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[2]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[2]; j < PozicijaNew[2] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon)[j];	vratiBoju();
					for(int j = PozicijaNew[2] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon)[j];
				}
				else
					cout << KorisniciLower[i].VrijemeUclanjivanja.tm_mon;
			}
			else
			{
				if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[2]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[2]; j < PozicijaNew[2] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon)[j];	vratiBoju();
					for(int j = PozicijaNew[2] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mon)[j];
				}
				else
					cout << KorisniciLower[i].VrijemeUclanjivanja.tm_mon;
				cout << "/";
				if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[1]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[1]; j < PozicijaNew[1] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday)[j];	vratiBoju();
					for(int j = PozicijaNew[1] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_mday)[j];
				}
				else
					cout << KorisniciLower[i].VrijemeUclanjivanja.tm_mday;
			}
			cout << "/";
			if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_year).find(tempTermin) != string::npos)
			{
				for(int j = 0; j < PozicijaNew[3]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_year)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				for(int j = PozicijaNew[3]; j < PozicijaNew[3] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_year)[j];	vratiBoju();
				for(int j = PozicijaNew[3] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_year).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_year)[j];
			}
			else
				cout << KorisniciLower[i].VrijemeUclanjivanja.tm_year;
			cout << endl;
			cout << "Vrijeme Uclanjivanja: ";
			if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_hour).find(tempTermin) != string::npos)
			{
				for(int j = 0; j < PozicijaNew[4]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_hour)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				for(int j = PozicijaNew[4]; j < PozicijaNew[4] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_hour)[j];	vratiBoju();
				for(int j = PozicijaNew[4] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_hour).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_hour)[j];
			}
			else
				cout << KorisniciLower[i].VrijemeUclanjivanja.tm_hour;
			cout << ":";
			if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_min).find(tempTermin) != string::npos)
			{
				for(int j = 0; j < PozicijaNew[5]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_min)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				for(int j = PozicijaNew[5]; j < PozicijaNew[5] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_min)[j];	vratiBoju();
				for(int j = PozicijaNew[5] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_min).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_min)[j];
			}
			else
				cout << KorisniciLower[i].VrijemeUclanjivanja.tm_min;
			cout << ":";
			if(to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_sec).find(tempTermin) != string::npos)
			{
				for(int j = 0; j < PozicijaNew[6]; j++)												cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_sec)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				for(int j = PozicijaNew[6]; j < PozicijaNew[6] + termin.size(); j++)						cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_sec)[j];	vratiBoju();
				for(int j = PozicijaNew[6] + termin.size(); j < to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_sec).size(); j++)					cout << to_string(KorisniciLower[i].VrijemeUclanjivanja.tm_sec)[j];
			}
			else
				cout << KorisniciLower[i].VrijemeUclanjivanja.tm_sec;
			cout << endl;
			cout << "Ime: ";
			if(Korisnici[i].Ime[0] != '\0')
			{
				if(CharArrToString(KorisniciLower[i].Ime).find(tempTermin) != string::npos)
				{
		    		for(int j = 0; j < PozicijaNew[7]; j++)												cout << Korisnici[i].Ime[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[7]; j < PozicijaNew[7] + termin.size(); j++)							cout << Korisnici[i].Ime[j];	vratiBoju();
					for(int j = PozicijaNew[7] + termin.size(); j < CharArrToString(KorisniciLower[i].Ime).size(); j++)		cout << Korisnici[i].Ime[j];
				}
				else
					cout << Korisnici[i].Ime;
			}
			else
				cout << "N/A";
			cout << endl;			
			cout << "Prezime: ";
			if(Korisnici[i].Prezime[0] != '\0')
			{
				if(CharArrToString(KorisniciLower[i].Prezime).find(tempTermin) != string::npos)
				{
		    		for(int j = 0; j < PozicijaNew[8]; j++)												cout << Korisnici[i].Prezime[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[8]; j < PozicijaNew[8] + termin.size(); j++)						cout << Korisnici[i].Prezime[j];	vratiBoju();
					for(int j = PozicijaNew[8] + termin.size(); j < CharArrToString(KorisniciLower[i].Prezime).size(); j++)	cout << Korisnici[i].Prezime[j];
				}
				else
					cout << Korisnici[i].Prezime;
			}
			else
				cout << "N/A";
			cout << endl;
			
			cout << "Spol: ";
			if(CharArrToString(KorisniciLower[i].Spol).find(tempTermin) != string::npos)
			{
	    		for(int j = 0; j < PozicijaNew[9]; j++)												cout << Korisnici[i].Spol[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				for(int j = PozicijaNew[9]; j < PozicijaNew[9] + termin.size(); j++)						cout << Korisnici[i].Spol[j];	vratiBoju();
				for(int j = PozicijaNew[9] + termin.size(); j < CharArrToString(KorisniciLower[i].Spol).size(); j++)	cout << Korisnici[i].Spol[j];	cout;
			}
			else
				cout << Korisnici[i].Spol;
			cout << endl;
			cout << "Datum Rodjenja: ";
			if(pGlobalPOSTAVKE->tipFormatDatuma && KorisniciLower[i].Dan != "" && KorisniciLower[i].Mje != "" && KorisniciLower[i].God != "")
			{
				if((KorisniciLower[i].Dan).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[10]; j++)												cout << (KorisniciLower[i].Dan)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[10]; j < PozicijaNew[10] + termin.size(); j++)						cout << (KorisniciLower[i].Dan)[j];	vratiBoju();
					for(int j = PozicijaNew[10] + termin.size(); j < (KorisniciLower[i].Dan).size(); j++)					cout << (KorisniciLower[i].Dan)[j];
				}
				else
					cout << KorisniciLower[i].Dan;
				cout << "/";
				if((KorisniciLower[i].Mje).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[11]; j++)												cout << (KorisniciLower[i].Mje)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[11]; j < PozicijaNew[11] + termin.size(); j++)						cout << (KorisniciLower[i].Mje)[j];	vratiBoju();
					for(int j = PozicijaNew[11] + termin.size(); j < (KorisniciLower[i].Mje).size(); j++)					cout << (KorisniciLower[i].Mje)[j];
				}
				else
					cout << KorisniciLower[i].Mje;
				cout << "/";
			}
			else if(pGlobalPOSTAVKE->tipFormatDatuma == false && KorisniciLower[i].Dan != "" && KorisniciLower[i].Mje != "" && KorisniciLower[i].God != "")
			{
				if((KorisniciLower[i].Mje).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[11]; j++)												cout << (KorisniciLower[i].Mje)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[11]; j < PozicijaNew[11] + termin.size(); j++)						cout << (KorisniciLower[i].Mje)[j];	vratiBoju();
					for(int j = PozicijaNew[11] + termin.size(); j < (KorisniciLower[i].Mje).size(); j++)					cout << (KorisniciLower[i].Mje)[j];
				}
				else
					cout << KorisniciLower[i].Mje;
				cout << "/";
				if((KorisniciLower[i].Dan).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[10]; j++)												cout << (KorisniciLower[i].Dan)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[10]; j < PozicijaNew[10] + termin.size(); j++)						cout << (KorisniciLower[i].Dan)[j];	vratiBoju();
					for(int j = PozicijaNew[10] + termin.size(); j < (KorisniciLower[i].Dan).size(); j++)					cout << (KorisniciLower[i].Dan)[j];
				}
				else
					cout << KorisniciLower[i].Dan;
				cout << "/";
			}
			else
				cout << "N/A";
			
			if((KorisniciLower[i].God).find(tempTermin) != string::npos)
			{
				for(int j = 0; j < PozicijaNew[12]; j++)												cout << (KorisniciLower[i].God)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				for(int j = PozicijaNew[12]; j < PozicijaNew[12] + termin.size(); j++)						cout << (KorisniciLower[i].God)[j];	vratiBoju();
				for(int j = PozicijaNew[12] + termin.size(); j < (KorisniciLower[i].God).size(); j++)					cout << (KorisniciLower[i].God)[j];
			}
			else
				cout << KorisniciLower[i].Mje;
			cout << endl;
			cout << "Starost: ";
			if(Korisnici[i].Dob != 0)
			{
				if(to_string(KorisniciLower[i].Dob).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[13]; j++)												cout << to_string(KorisniciLower[i].Dob)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[13]; j < PozicijaNew[13] + termin.size(); j++)						cout << to_string(KorisniciLower[i].Dob)[j];	vratiBoju();
					for(int j = PozicijaNew[13] + termin.size(); j < to_string(KorisniciLower[i].Dob).size(); j++)					cout << to_string(KorisniciLower[i].Dob)[j];
				}
				else
					cout << Korisnici[i].Dob;
			}
			else
				cout << "N/A";
			cout << endl;
			cout << "Adresa Stanovanja: ";
			if(Korisnici[i].AdresaStanovanja[0] != '\0')
			{
				if((CharArrToString(KorisniciLower[i].AdresaStanovanja)).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[14]; j++)												cout << CharArrToString(Korisnici[i].AdresaStanovanja)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[14]; j < PozicijaNew[14] + termin.size(); j++)						cout << CharArrToString(Korisnici[i].AdresaStanovanja)[j];	vratiBoju();
					for(int j = PozicijaNew[14] + termin.size(); j < CharArrToString(KorisniciLower[i].AdresaStanovanja).size(); j++)					cout << CharArrToString(Korisnici[i].AdresaStanovanja)[j];
				}
				else
					cout << Korisnici[i].AdresaStanovanja;
			}
			else
				cout << "N/A";
			cout << endl;
			cout << "Broj Telefona: ";
			if(Korisnici[i].BrojTelefona[0] != '\0')
			{
				if((CharArrToString(KorisniciLower[i].BrojTelefona)).find(tempTermin) != string::npos)
				{
					for(int j = 0; j < PozicijaNew[15]; j++)												cout << CharArrToString(KorisniciLower[i].BrojTelefona)[j];	odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					for(int j = PozicijaNew[15]; j < PozicijaNew[15] + termin.size(); j++)						cout << CharArrToString(KorisniciLower[i].BrojTelefona)[j];	vratiBoju();
					for(int j = PozicijaNew[15] + termin.size(); j < CharArrToString(KorisniciLower[i].BrojTelefona).size(); j++)					cout << CharArrToString(KorisniciLower[i].BrojTelefona)[j];
				}
				else
					cout << Korisnici[i].BrojTelefona;	
			}
			else
				cout << "N/A";
			cout << endl << endl << "----------------------------------------" << endl;
		}
	}
	return ptr; // vraca pointer na vektor objekata tipa KORISNIK
}

//ova funkcija sluzi kao finalni filter nakon podudarnosti termina. U slucaju da podudarnost minimalno daje 2 ili vise objekta, ova funkcija pruza sistem odabira izmedju pronadjenih
//objekata na osnovu njihovog indeksa u vektoru
//Tj. ako imamo dva 'safeta', oni imaju razlicit ID, pa kad se filter na podudarnost zasiti onda se oslanja na odabiru pomocu ID
int odabirKorisnika(vector<KORISNIK>* pFiltrirano)
{
	system("CLS");
	char key;
	string termin = "";
	size_t velicina = pFiltrirano->size(); //pFiltrirano predstavlja pointer na vektor objekata koji je deklarisan za pohranu objekata tipa Korisnik koji imaju podudarnost sa odredjenim terminom
	bool pronadjen = true;
	if(velicina == 1)//ako je broj objekata 1 to znaci da nije potrebna daljnja filtracija jer je korisnik vec pronadjen
	{
		return (pFiltrirano)->at(0).ID;
	}
	else if(velicina == 0)//nije pronadjen korisnik
	{
		return 0;
	}
	else//2 ili vise pronadjena u kojem slucaju odabir pravimo pomocu Indeksa
	{
		while(true)
		{
			system("CLS");
			cout << "Unesite Trazeni ID Korisnika:\n\n";
			for(int i = 0; i < velicina; i++)
			{
				//ispis atributa imaju dva moda u slucaju da atribut nije prazan printa vrijednost atributa, a ako jest onda printa "N/A" -> 'Unknown'
				//to vazi za skoro svaki atribut
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << BAR << "\tID: " << (pFiltrirano)->at(i).ID << "\n";
				vratiBoju();
				
				cout << "\tDatum Uclanjivanja: ";
				
				if(pGlobalPOSTAVKE->tipFormatDatuma)
					cout << (pFiltrirano)->at(i).VrijemeUclanjivanja.tm_mday << "/" << (pFiltrirano)->at(i).VrijemeUclanjivanja.tm_mon + 1;
				else
					cout << (pFiltrirano)->at(i).VrijemeUclanjivanja.tm_mon + 1 << "/" << (pFiltrirano)->at(i).VrijemeUclanjivanja.tm_mday;
				cout << "/";
				cout << (pFiltrirano)->at(i).VrijemeUclanjivanja.tm_year + 1900 << "\n";
				cout << "\tIme: ";
				if((pFiltrirano)->at(i).Ime[0] != '\0')
					cout << (pFiltrirano)->at(i).Ime << "\n";
				else
					cout << "N/A\n";
				cout << "\tPrezime: ";
				if((pFiltrirano)->at(i).Prezime[0] != '\0')
					cout << (pFiltrirano)->at(i).Prezime << "\n";
				else
					cout << "N/A\n";
				cout << "\tSpol: ";
				if((pFiltrirano)->at(i).Spol[0] != '\0')
					cout << (pFiltrirano)->at(i).Spol << "\n";
				else
					cout << "N/A\n";
				
				cout << "\tDatum Rodjenja: ";
				if((pFiltrirano)->at(i).Dan != "" || (pFiltrirano)->at(i).Mje != "" || (pFiltrirano)->at(i).God != "")
				{
					if(pGlobalPOSTAVKE->tipFormatDatuma)
						cout << (pFiltrirano)->at(i).Dan << "/" << (pFiltrirano)->at(i).Mje;
					else
						cout << (pFiltrirano)->at(i).Mje << "/" << (pFiltrirano)->at(i).Dan;
					cout << "/";
					cout << (pFiltrirano)->at(i).God << "\n";
				}
				else
					cout << "N/A\n";
				cout << "\tAdresa Stanovanja: ";
				if((pFiltrirano)->at(i).AdresaStanovanja[0] != '\0')
					cout << (pFiltrirano)->at(i).AdresaStanovanja << "\n";
				else
					cout << "N/A\n";
				cout << "\tBroj Telefona: ";
				if((pFiltrirano)->at(i).BrojTelefona[0] != '\0')
					cout << (pFiltrirano)->at(i).BrojTelefona << "\n";
				else
					cout << "N/A\n";
	
				cout << "\n";
			}
			cout << "\tUnos: " << termin;
			if(pronadjen == false)
				cout << "\n\n\tTrazeni ID Nije Pronadjen";
			key = _getch();//funkcija iz conio koja se koristi za citanje jednog karaktera sa konzole bez prikazivanja na ekranu
			if(key == 27)//ascii za ESC
				return 0;
			else if(key == 8)//ascii za BACKSPACE, tj. ako korisnik pritisne backspace uklonit ce zadnji element termina za pretrazivanje i kombinovano sa system("CLS") daje 
							//daje efekat brisanja kao na nekom search enginu
			{
				if(termin.size() > 0)// samo vazi ako je termin neprazan
				{
					termin.pop_back();
				}
			}
			else if(key == 13)// ascii za ENTER
			{
				for(int j = 0; j < velicina; j++)
				{
					if(pFiltrirano->at(j).ID == atoi(termin.c_str()))//ako su ID korisnika i termin podudarni onda:
					{
						return pFiltrirano->at(j).ID; //vraca pronadjeni ID trazenog korisnika
						break;
					}
					else
					{
						pronadjen = false;
						continue;
					}
				}
			}
			else//sve dok ne pritisne esc, backspace ili enter, termin se puni sa charovima koje korisnik unese i onda se koristi za test podudarnosti
			{
				termin += key;
			}
		}
	}
}

//glavna funckija za pretragu, u ovoj se funckiji stvaraju stringovi za podudarnost, sacinjene malim slovima i originalnim unoso.
int pretragaMain(vector<KORISNIK>* pFiltrirano)
{
	ulaz:
	int odabir;
	size_t FilterVelicina;
	string termin = "";
	string tempTermin = "";
	pretraga(termin, tempTermin);
	char key, x;
	while(true)
	{
		key = _getch();
		if(key == 27)
			goto izlaz;
		else if(key == 8)
		{
			if(termin.size() > 0)
			{
				termin.pop_back();
				tempTermin.pop_back();
			}
		}
		else if(key == 13)
		{
			system("CLS");
			pFiltrirano = pretraga(termin, tempTermin);//kada korisnik pritisne ENTER, funkcija pretraga ce vratit pointer na vektor svih objekata 
													//ciji su neki atributi podudarni sa terminom, tj tempTerminom
			FilterVelicina = pFiltrirano->size();
			//radi testiranja da zahtijeva unos prije nego sto ocisti ekran
			//cout << FilterVelicina;
			//	x = _getch();
			//	if(x == 27)
			
			break;
		}
		else
		{
			termin += key;
			tempTermin += tolower(key);
		}
		pretraga(termin, tempTermin);//salje termin i tempTermin, koji je samo verzija sa malim slovima termina, u funckiju za pretragu iznad
	}
	odabir = odabirKorisnika(pFiltrirano);
	if(odabir == 0)//ako je odabir 0 tj nema podudarnih objekata onda goto ide na ulaz funkcije
		goto ulaz;
	else
		return odabir;//u ostalom vraca broj indeksa trazenog korisnika
	izlaz:{}
}

//klasicni unos korisnika koristenjem cin, cin.getline(), getline(cin) itd.
unosKorisnikaClassic(pFunkcija pGrafik)//ovaj argument sluzi za prenos grafik funkcija koje su definisane u header fajlu Grafici.h
{
	int lokacija = 0;//oznacava nivo unosa, ime = 0; prezime = 1; itd;
					//lokacija 9 u ovom slucaju predstavlja kraj
	int velicina = Korisnici.size();
	KORISNIK tempKorisnik;

	//ocisti atribute temporanlog objekta
	tempKorisnik.ID = velicina+1;

	tempKorisnik.Ime[0] = 0;
	tempKorisnik.Prezime[0] = 0;
	tempKorisnik.Spol[0] = 0;
	tempKorisnik.DatumRodjenja = "";
	tempKorisnik.AdresaStanovanja[0] = 0;
	tempKorisnik.BrojTelefona[0] = 0;

	
	char key;
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

		//prikaz atributu, ako je atribut prazan onda printa "N/A"
			cout << "\n\n\t\t(1/8) Uneseno Ime: ";
			if(tempKorisnik.Ime[0] != 0)

			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Ime; vratiBoju();
			}
			else
				cout << "N/A";
			
			cout << "\n\t\t(2/8) Uneseno Prezime: ";
			if(tempKorisnik.Prezime[0] != 0)
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Prezime; vratiBoju();
			}
			else
				cout << "N/A";

			cout << "\n\t\t(3/8) Uneseni Spol: ";
			if(tempKorisnik.Spol[0] != 0)
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Spol; vratiBoju();
			}
			else
				cout << "N/A";
					
			cout << "\n\t\t(4/8) Uneseni Dan Rodjenja: ";
			if(tempKorisnik.Dan != "")
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Dan; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(5/8) Uneseni Mjesec Rodjenja: ";
			if(tempKorisnik.Mje != "")
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.Mje; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(6/8) Unesena Godina Rodjenja: ";
			if(tempKorisnik.God != "")
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.God; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(7/8) Unesena Adresa Stanovanja: ";
			if(tempKorisnik.AdresaStanovanja[0] != 0)

			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.AdresaStanovanja; vratiBoju();
			}
			else
				cout << "N/A";
			cout << "\n\t\t(8/8) Uneseni Broj Telefona: ";
			if(tempKorisnik.BrojTelefona[0] != 0)

			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempKorisnik.BrojTelefona; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\n";
			
			if(lokacija == 8)
				lokacija++;
			
			//klasican unos
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
					cout << "\t\tUnesite Dan Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin >> tempKorisnik.Dan; vratiBoju();
					lokacija++;
					break;
				}
				case 4:
				{
					cout << "\t\tUnesite Mjesec Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin >> tempKorisnik.Mje; vratiBoju();
					lokacija++;
					break;
				}
				case 5:
				{
					cout << "\t\tUnesite Godinu Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin >> tempKorisnik.God; vratiBoju();
					lokacija++;
					cin.clear();
					cin.ignore(1000, '\n');
					break;
				}
				case 6:
				{
					cout << "\t\tUnesite Adresu Stanovanja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(tempKorisnik.AdresaStanovanja, 40); vratiBoju();
					lokacija++;
					break;
				}
				case 7:
				{
					cout << "\t\tUnesite Broj Telefona: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(tempKorisnik.BrojTelefona, 20); vratiBoju();
					lokacija++;
					break;
				}
			}
			if(lokacija == 9)
			{
				time_t sad;
					
				sad = time(NULL); //uzme vrijeme od OS
				tempKorisnik.VrijemeUclanjivanja = *localtime(&sad);
				
				Korisnici.push_back(tempKorisnik);
				pKorisniciFile->open("KorisniciData.csv", ios::app);
				
				//pise podatke korisnika u csv fajl "KorisniciData" u append modu, jer zelimo samo da nadodamo novu liniju u fajlu a ne da pisemo iz pocetka
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
			else if(key == 27)
				system("CLS");
	}
	Korisnici.push_back(tempKorisnik);//pohrani temporalnog korisnika u glavni vektor Korisnika
}


unosPaketaClassic(pFunkcija pGrafik)
{
	int lokacija = 0;
	int velicina = Paketi.size();
	PAKETI tempPaketi;

	tempPaketi.ID = velicina+1;
	tempPaketi.Ime = "";
	tempPaketi.BrojSedmicnihSesija = 0;
	tempPaketi.CijenaPoSesiji = 0;
	tempPaketi.UkupnaCijena = 0;
	
	char key;
	while(true)
	{
		
		ulaz:
			cin.ignore(1000, '\n');
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
			cout << ". Paketa";
			
			tempPaketi.ID = velicina + 1;
			
			if(tempPaketi.Ime != "")
			{
				cout << "\n\n\t\t(1/4) Uneseno Ime Paketa: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempPaketi.Ime; vratiBoju();
			}
				
			if(tempPaketi.BrojSedmicnihSesija != 0)
			{
				cout << "\n\t\t(2/4) Uneseni Broj Sedmicnih Sesija: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempPaketi.BrojSedmicnihSesija; vratiBoju();
			}
				
			if(tempPaketi.CijenaPoSesiji != 0)
			{
				cout << "\n\t\t(3/4) Unesena Cijena Po Sesiji: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempPaketi.CijenaPoSesiji; vratiBoju();
			}
			
			if(tempPaketi.UkupnaCijena != 0)
			{
				cout << "\n\t\t(4/4) Unesena Ukupna Cijena: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << tempPaketi.UkupnaCijena; vratiBoju();
			}
			
			cout << "\n\n";
			
			if(lokacija == 4)
				lokacija++;
				
				
			switch(lokacija)
			{
				case 0:
				{
					cout << "\t\tUnesite Ime Paketa: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					getline(cin, tempPaketi.Ime); vratiBoju();
					cin.clear();
					lokacija++;
					break;
				}
				case 1:
				{
					cout << "\t\tUnesite Broj Sedmicnih Sesija: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin>>tempPaketi.BrojSedmicnihSesija; vratiBoju();
					if(cin.fail())
					{
						cin.clear();
						cin.ignore(1000, '\n');
						goto ulaz;
					}
					lokacija++;
					break;
				}
				case 2:
				{
						
					cout << "\t\tUnesite Cijenu Po Sesiji: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin>>tempPaketi.CijenaPoSesiji; vratiBoju();
					if(cin.fail())
					{
						cin.clear();
						cin.ignore(1000, '\n');
						goto ulaz;
					}
					lokacija++;
					break;
				}
				case 3:
				{
					cout << "\t\tUkupna Cijena: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin>>tempPaketi.UkupnaCijena; vratiBoju();
					if(cin.fail())
					{
						cin.clear();
						cin.ignore(1000, '\n');
						goto ulaz;
					}
					lokacija++;
					break;
				}
			}
			if(lokacija == 5){
			
				Paketi.push_back(tempPaketi);
					pPaketiFile->open("PaketiData.csv", ios::app);
					
					*pPaketiFile << " " << "," << tempPaketi.ID << ","
									<< tempPaketi.Ime << ","
									<< tempPaketi.BrojSedmicnihSesija << ","
									<< tempPaketi.CijenaPoSesiji << ","
									<< tempPaketi.UkupnaCijena << endl;
					pPaketiFile->close();
					break;
		}
		}
		cout << "\n\n\t\tPonoviti Unos? NE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- DA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();

		key = _getch();
			if(key == 27)
				break;
			//else if(key == 13)
			
	}
	Paketi.push_back(tempPaketi);
}

void UplataClanarina()
{
	
}

//moderniji nacin unosa korisnika. Program radi na sistemu _getch() koji stvara string termin, na ovom nacinu rada moguce je pritisnuti esc ili enter kako 
//bi omoguilo korisniku izlaz iz unosa korisnika. Takodjer je ugradjen sistem medjumemorije koje kada korisnik unese jedan atribut, moze da se vrati na taj 
//atribut a da on ostane vec unesen u polje s tim da korisnik moze to promijeniti npr. na unosu Ime:, korisnik unese ime 'Hamo' i pritisne enter da udje u unos
//prezimena, ako korisnik u tom trenutku pritisne esc i vrati se na unos imena, tada ce 'Hamo' ostati na unosu, tj. zapamtit ce unos.
//ovo je implementirano jer je mnogo lakse prepraviti pogreske pri unosu, mada postoji i funkcija za editovanje korisnike i brisanje analogno
unosKorisnika(pFunkcija pGrafik)
{
	mainUlaz:
	int velicina = Korisnici.size();
	KORISNIK tempKorisnik;
	string tempString = "";
	char key;
	
	//deklarisan niz char const pointera koju sluze kao opcije za odabir Spola
	char const* SPOL[4];
	SPOL[0] = "Muski";
	SPOL[1] = "Zenski";
	SPOL[2] = "Drugo";
	SPOL[3] = "Radije ne bi Rekli";
	LOKACIJA = "1.1.1.0";
	
	int odabir = 0, lokacija = 0;
	
	//ociste se vrijednosti atributa
	tempKorisnik.ID = velicina+1;
	tempKorisnik.Ime[0] = 0;
	tempKorisnik.Prezime[0] = 0;
	tempKorisnik.Spol[0] = 0;
	tempKorisnik.Dan = "";
	tempKorisnik.Mje = "";
	tempKorisnik.God = "";
	tempKorisnik.DatumRodjenja = "";
	tempKorisnik.AdresaStanovanja[0] = 0;
	tempKorisnik.BrojTelefona[0] = 0;

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
		//prikaz atributa, ako su prazni printa "N/A"
		cout << "\n\n\t\t"; 
		if(lokacija == 0){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseno Ime: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Ime[0] != 0) cout << tempKorisnik.Ime;
		else							cout << "N/A";
		vratiBoju();

		cout << "\n\t\t";
		if(lokacija == 1){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseno Prezime: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Prezime[0] != 0) cout << tempKorisnik.Prezime;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t";
		if(lokacija == 3){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }	
		cout << "\tUneseni Spol: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.Spol[0] != 0) cout << tempKorisnik.Spol;
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
		if(tempKorisnik.AdresaStanovanja[0] != 0) cout << tempKorisnik.AdresaStanovanja;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t"; 
		if(lokacija == 8){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseni Broj Telefona: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempKorisnik.BrojTelefona[0] != 0) cout << tempKorisnik.BrojTelefona;
		else							cout << "N/A";
		vratiBoju();
		cout << "\n\n";
		//u odnosu na lokaciju printa odredjenu poruku. nivo unosa.
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
		//ovaj kondicional se odnosi na lokacije gdje se nalazi dan, mjesec i godina datuma rodjenja
		if(lokacija != 9 && (lokacija < 4 || lokacija > 5))
		{
			cout << tempString;
		}
		if(lokacija == 4)//poredi da li je dan validan, tj da li je veci od 31. ili manji od 0, svjesni smo da svaki mjesec nema 31 dan ali ipak je neki sigurnosni pristup
		{
			cout << tempString;
			tempNum = atoi(tempString.c_str());
			if(tempNum > 31 || tempNum < 0)
				cout << "\tDan Nije Validan";
		}
		if(lokacija == 5)//poredi da li je mjesec validan, tj da li je veci od 12 ili manji od 0
		{
			cout << tempString;
			tempNum = atoi(tempString.c_str());
			if(tempNum > 12 || tempNum < 0)
				cout << "\tMjesec Nije Validan";
		}
			
		vratiBoju();
		
		if(lokacija == 2)//odnosi se na odabir spola. Ako korisnik odabere ili Muski, Zenski ili Radije ne bi Rekli opciju tada na sljedecoj lokaciji ne mogu mijenjati
						//taj string termin, ali naravano, moguce se je vratit na odabir spola i odabrati drugi spol, ili cak da korisnik unese.
		{
			if(odabir != -1)
			{
				lokacija++;
				if(odabir == 1)
				{
					strcpy(tempKorisnik.Spol, "Muski");
					if(tempKorisnik.Spol[0] == 0)
						tempString = "";
					else if(lokacija == 3)
						tempString = "Muski";
						
				}
				else if(odabir == 2)
				{
					strcpy(tempKorisnik.Spol, "Zenski");
					if(tempKorisnik.Spol[0] == 0)
						tempString = "";
					else if(lokacija == 3)
						tempString = "Zenski";
				}
				else if(odabir == 3)
				{
					if(tempKorisnik.Spol[0] == 0)
						tempString = "";
					else if(lokacija == 3)
						tempString = spol;
				}
				else if(odabir == 4)
				{
					strcpy(tempKorisnik.Spol, "Radije ne bi Rekli");
					if(tempKorisnik.Spol[0] == 0)
						tempString = "";
					else if(lokacija == 3)
						tempString = "Radije ne bi Rekli";
				}
			}
			if(odabir == -1)//povrat -1 predstavlja esc iz selekcijaLogika funkcije
			{
				tempString = prezime;
				lokacija--;
			}		
		}
		else
		{
			key = _getch();

			if(key == 27)//esc
			{
				if(lokacija == 0)
					break;
				else//ovdje se obavlja proces pohranjivanja podataka u "medjumemoriju" u zavisnosti od lokacije. npr. ako smo na lokaciji za unos prezimena i unesemo esc
					//tada ce tempString imati vrijednost ime. Onda kada se vratimo na unos imena vec ce u unosu biti spremljeno odgovarajuce ime korisnika itd.
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
			else if(key == 13)//kontra logika, na unosu ENTER tempString dobija vrijednost sljedeceg odgovarajuceg atributa u zavisnosti od lokacije
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
					//opet pisanje u fajl
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

				//ako je odgovarajuci atribut prazan, onda ce tempString biti prazan
				if(tempKorisnik.Prezime[0] == 0 && lokacija == 1)

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
					
				if(tempKorisnik.AdresaStanovanja[0] == 0 && lokacija == 7)
					tempString = "";
				else if(lokacija == 6)
					tempString = adresa;
				
				if(tempKorisnik.BrojTelefona[0] == 0 && lokacija == 8)
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

unosPaketa(pFunkcija pGrafik)
{
	mainUlaz:
	int velicina = Paketi.size();
	PAKETI tempPaketi;
	string tempString = "";
	char key;
	
	int odabir = 0, lokacija = 0;
	
	tempPaketi.ID = velicina+1;
	tempPaketi.Ime[0] = 0;
	tempPaketi.BrojSedmicnihSesija = 0;
	tempPaketi.CijenaPoSesiji = 0;
	tempPaketi.UkupnaCijena = 0;

	string ImePaketa, BrojSedmicnihSesija, CijenaPoSesiji, UkupnaCijena;
	char keyCheck;
	int tempNum;
	while(true)
	{
		system("CLS");
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "\tUnos Paketa:\n\n"; vratiBoju();
		cout << "\tNAZAD "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- NAPRIJED "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();
		cout << "\tUnos "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << velicina + 1; vratiBoju();
		cout << ". Paketa";
		
		cout << "\n\n\t\t";
		if(lokacija == 0){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseno Ime Paketa: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempPaketi.Ime[0] != 0) cout << tempPaketi.Ime;
		else							cout << "N/A";
		vratiBoju();

		cout << "\n\t\t";
		if(lokacija == 1){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUneseni Broj Sedmicnih Sesija: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempPaketi.BrojSedmicnihSesija != 0) cout << tempPaketi.BrojSedmicnihSesija;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t";
		if(lokacija == 3){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }	
		cout << "\tUnesena Cijena Po Sesiji: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(	tempPaketi.CijenaPoSesiji != 0) cout << 	tempPaketi.CijenaPoSesiji;
		else							cout << "N/A";
		vratiBoju();
		
		cout << "\n\t\t";
		if(lokacija == 4){ odabranaBoja(pGlobalPOSTAVKE->bojaReal); cout << BAR; vratiBoju(); }
		cout << "\tUnesena Ukupna Cijena: ";
		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		if(tempPaketi.UkupnaCijena != 0) cout << tempPaketi.UkupnaCijena;
		else							cout << "N/A";
		vratiBoju();
		
		
		cout << "\n\n";
		if(lokacija == 0)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "1"; vratiBoju();
			cout << "/4) Unesite Ime Paketa: ";
		}
			
		if(lokacija == 1)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "2"; vratiBoju();
			cout << "/4) Unesite Broj Sesija Po Sedmici: ";
		}
			
		
		if(lokacija == 2)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "3"; vratiBoju();
			cout << "/4) Unesite Cijenu Po Sesiji: ";
		}
			
		if(lokacija == 3)
		{
			cout << "\t\t("; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "4"; vratiBoju();
			cout << "/4) Unesite Ukupnu Cijenu: ";
		}

			
		if(lokacija == 4)
		{
			cout << "\t\tDa li ste zadovoljni unosom?\n\t\t";
			cout << "\tNE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ESC] "; vratiBoju();
			cout << "- DA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ENTER]\n\n"; vratiBoju();
		}
			

		odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			
		vratiBoju();
		
		if(lokacija == 2)
		{
			if(odabir != -1)
			{
				lokacija++;
			
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
						tempString = ImePaketa;
					if(lokacija == 2)
						tempString = BrojSedmicnihSesija;
					if(lokacija == 3)
						tempString = CijenaPoSesiji;
					if(lokacija == 4)
						tempString = UkupnaCijena;
					lokacija--;
				}
			}
			else if((key == 4 && odabir != 1 && odabir != 2 && odabir != 3) || key == 4 && (lokacija >= 3 ))
			{
				if(tempString.size() != 0)
					tempString.pop_back();
			}
			else if(key == 13)
			{
				if(lokacija == 0)
				{
					tempPaketi.Ime= atoi(tempString.c_str());
					ImePaketa = tempString;
					
				}
				if(lokacija == 2)
				{
				tempPaketi.BrojSedmicnihSesija= atoi(tempString.c_str());
					BrojSedmicnihSesija = tempString;
				}
				
				if(lokacija == 3)
				{
					tempPaketi.CijenaPoSesiji= atoi(tempString.c_str());
					CijenaPoSesiji = tempString;
				}
				
				if(lokacija == 4)
				{
					tempPaketi.UkupnaCijena= atoi(tempString.c_str());
					UkupnaCijena = tempString;
				}
				

				
				if(lokacija == 5)
				{
					Paketi.push_back(tempPaketi);
					pPaketiFile->open("PaketiData.csv", ios::app);
					
					*pPaketiFile << " " << "," << tempPaketi.ID << ","
									<< tempPaketi.Ime << ","
									<< tempPaketi.BrojSedmicnihSesija << ","
									<< tempPaketi.CijenaPoSesiji << ","
									<< tempPaketi.UkupnaCijena << endl;
					pPaketiFile->close();
					break;
				}
				
				if(tempPaketi.BrojSedmicnihSesija == 0 && lokacija == 1)
					tempString = "";
				else if(lokacija == 0)
					tempString = BrojSedmicnihSesija;

				if(tempPaketi.CijenaPoSesiji == 0 && lokacija == 2)
					tempString = "";
				else if(lokacija == 1)
					tempString = CijenaPoSesiji;
					
				if(tempPaketi.UkupnaCijena == 0 && lokacija == 3)
					tempString = "";
				else if(lokacija == 2)
					tempString = UkupnaCijena;
				
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
		cout << "\n\n\t\tDa li zelite izaci iz unosa Paketa?\n\t\t";
		cout << "\tDA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- NE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();
	}
	else if(lokacija == 5)
	{
		cout << "\n\n\t\tDa li zelite unijeti sljedeci Paket?\n\t\t";
		cout << "\tNE "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ESC] "; vratiBoju();
		cout << "- DA "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
		cout << "[ENTER]\n\n"; vratiBoju();
	}
	
		if(lokacija != 4)
		{
			key = _getch();
			if(key == 27)
				goto izlaz;
			else if(key == 13)
				goto mainUlaz;
		}
	
	izlaz:{}
}
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
			if(pGlobalPOSTAVKE->tipFormatDatuma)	{cout << noviLokal.tm_mday << "/" << noviLokal.tm_mon + 1 << "/" << noviLokal.tm_year + 1900;}
			else	{cout << noviLokal.tm_mon + 1 << "/" << noviLokal.tm_mday << "/" << noviLokal.tm_year + 1900;}
			vratiBoju();
			cout << "\nTrenutno Vrijeme: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << noviLokal.tm_hour << ":" << noviLokal.tm_min << ":" << noviLokal.tm_sec; vratiBoju();
			cout << "\nPritisnite "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "[ENTER]";	vratiBoju();
			cout << " da "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
			cout << "Osvjezite Vrijeme";	vratiBoju();
			cout << "\n\n";
			
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
				if(pGlobalPOSTAVKE->tipFormatDatuma)	{cout << Korisnici[i].VrijemeUclanjivanja.tm_mday << "/" << Korisnici[i].VrijemeUclanjivanja.tm_mon + 1 << "/" << Korisnici[i].VrijemeUclanjivanja.tm_year + 1900;}
				else	{cout << Korisnici[i].VrijemeUclanjivanja.tm_mon + 1 << "/" << Korisnici[i].VrijemeUclanjivanja.tm_mday << "/" << Korisnici[i].VrijemeUclanjivanja.tm_year + 1900;}
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
					if(pGlobalPOSTAVKE->tipFormatDatuma)	{cout << Korisnici[i].Dan << "/" << Korisnici[i].Mje << "/" << Korisnici[i].God;}
					else	{cout << Korisnici[i].Mje << "/" << Korisnici[i].Dan << "/" << Korisnici[i].God;}
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
				
			cout<<endl << endl<< endl;
			}
			cout << "NAZAD [ESC]";
			key = _getch();
			if(key == 27){goto izlaz;}
		}
	}
	izlaz:{}
}

void tabelaPrint(vector<map<string, string>>& KorisniciData, vector<string>& keys) {
    int KorisniciData_size = KorisniciData.size();
    int column_count = keys.size();
    int* col_sizes = new int[column_count];
    for (int i = 0; i < column_count; i++) {
        col_sizes[i] = keys.at(i).length();
        for (int j = 0; j < KorisniciData_size; j++) {
            if (col_sizes[i] < KorisniciData[j][keys.at(i)].length()) {
                col_sizes[i] = KorisniciData[j][keys.at(i)].length();
            }
        }
    }
odabranaBoja(pGlobalPOSTAVKE->bojaReal);
    for (int i = 0; i < column_count; i++) {
        cout << setw(col_sizes[i]) << left << keys.at(i) << "|";
    }
    cout << endl;

    for (int i = 0; i < KorisniciData_size; i++) {
        for (int j = 0; j < column_count; j++) {
            cout << setw(col_sizes[j]) << setfill('-') << "" << setfill(' ') << "|";
        }
        cout << endl;
        for (int j = 0; j < column_count; j++) {
            cout << setw(col_sizes[j]) << left << KorisniciData[i][keys.at(j)]  << "|";
        }
        cout << endl;
    }
}

vector<string> tokens(string s, string delimiter) {
    vector<string> v;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        v.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    v.push_back(s);
    return v;
}

void printajTabeluKorisnika(){
	LOKACIJA = "1.2.0.0";
	system("CLS");
	
	cout<<endl;
	
	map<string, string> myMap;
    vector<map<string, string>> KorisniciData;
    string text;
    ifstream read;
    read.open("KorisniciData.csv");
    
	odabranaBoja(pGlobalPOSTAVKE->bojaReal);

    getline(read, text);
    vector<string> keys = tokens(text, ",");
    vector<string> row;
    
    while (getline(read, text)) {
        row = tokens(text, ",");
        myMap.clear();
        for (int i = 0; i < keys.size(); i++) {
            myMap.insert(pair<string, string>(keys.at(i), row[i]));
        }
        KorisniciData.push_back(myMap);
    }

    tabelaPrint(KorisniciData, keys);
    vratiBoju();
    
    cout<< endl << endl;
    
    cout << "NAZAD [ESC]";

			char key= _getch();
			if(key == 27){system("CLS");}
    
}

void printajTabeluPaketa(){
	system("CLS");
	
	cout<<endl;
	
	map<string, string> myMap;
    vector<map<string, string>> PaketiData;
    string text;
    ifstream read;
    read.open("PaketiData.csv");
    
odabranaBoja(pGlobalPOSTAVKE->bojaReal);

    getline(read, text);
    vector<string> keys = tokens(text, ",");
    vector<string> row;
    
    while (getline(read, text)) {
        row = tokens(text, ",");
        myMap.clear();
        for (int i = 0; i < keys.size(); i++) {
            myMap.insert(pair<string, string>(keys.at(i), row[i]));
        }
        PaketiData.push_back(myMap);
    }

    tabelaPrint(PaketiData, keys);
    vratiBoju();
    
    cout<< endl << endl;
    
    cout << "NAZAD [ESC]";

			char key= _getch();
			if(key == 27){system("CLS");}
    
}

void OsobniPaketi() //trening paketi koje vlasnik zadaje a takodjer imaju i unaprijed definisani paketi koje korisnik moze da importuje
{
	pPaketi;
	LOKACIJA = "1.6.0.0";
	char const* PAKET[4];
	PAKET[0] = "Importuj Tvornicke Pakete";
	PAKET[1] = "Napravi Novi Paket";
	PAKET[2] = "Edituj Paket";
	PAKET[3] = "Obrisi Paket";
	PAKET[4] = "Sacuvaj Promjene";
	
	selekcijaLogika(PAKET, 5, LOKACIJA, "PAKETI:", printajNULL);
}

void UplataClanarinaMain(vector<KORISNIK>* pFiltrirano)
{
	int odabir;
	int odabir2;
	LOKACIJA = "10.0.0.0";
	char const* CLANARINA[1]; //niz opcija
	CLANARINA[0] = "Rok Kraja Clanarine";
	CLANARINA[1] = "Odabir Trening Paketa";
	odabir = selekcijaLogika(CLANARINA, 2, LOKACIJA, "CLANARINA:", printajNULL);
	system("CLS");
	if(odabir == 1)
	{
		int id = pretragaMain(pFiltrirano); //dobavlja trazeni ID Korisnika
		cout << "\n\n";
		time_t sad;
		sad = time(NULL); //uzme vrijeme od OS
		time_t korisnik = mktime(&(Korisnici[0].VrijemeUclanjivanja)); //pretvara tm struct u time_t format radi lakseg poredjenja vremena i trazenja razlike
		noviLokal = *localtime(&sad); // pridodaje time_t sadasnje vrijeme u struct NoviLokal, tako da mozemo imati pristup vremenskim jedinicama zasebno kao sek, min, sat itd.
		//cout << ctime(&korisnik); //Prikazuje vrijeme uclanjivanja korisnika
		//cout << ctime(&sad); // Prikazuje sadasnje vrijeme
		
		Korisnici[id - 1].Evidencija.Razlika = difftime(korisnik, sad)/(3600*24);//trazi razliku u sekundama i dijeljenjem sa 3600*24 konvertujemo u dane
		//cout << Korisnici[id - 1].Evidencija.Razlika;
		cout << "Korisnik " << id << ". ima jos: " << 30 - static_cast<int>(Korisnici[id - 1].Evidencija.Razlika) << " dana do isteka clanarine\n";
		cout << "Tacnije: " << 30 - Korisnici[id - 1].Evidencija.Razlika << " dana do isteka clanarine";
	}
	else if(odabir == 2)
	{
		int id = pretragaMain(pFiltrirano);
		printajTabeluPaketa();
		do
		{
			if(cin.fail())
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> odabir2;
		}while(cin.fail() || odabir2 < 0 || odabir2 > pPaketi->size());
		Korisnici[id - 1].Evidencija.PlaceniPaketi.push_back(pPaketi->at(odabir2 - 1));
	}
	
	cout << "\n\nNAZAD [ESC]";
	char key = _getch();
	if(key == 27){system("CLS");}
}

//funckija za brisanje korisnika na odnosu indeksa
void obrisiKorisnika(vector<KORISNIK>* pFiltrirano)
{
	char key;
	int velicina = Korisnici.size();
	LOKACIJA = "1.5.0.0";
	system("CLS");
	int id = pretragaMain(pFiltrirano); //dobavlja trazeni ID Korisnika
	cout << "\n\n";
	cout << id << endl;
	cout << Korisnici[id - 1].Ime;
	string line;
	cout << "Da li zelite obrisati Korisnika?\nDA = [ENTER] - NE = [ESC]";
	key = _getch();
	if(key == 13)
	{
		Korisnici.erase(Korisnici.begin() + id - 1);
		//petlja smanjuje id svakog korisnika poslike obrisanog za 1 tako da nadoknadi id koji nedostaje
		for(int i = id - 1; i < velicina - 1; i++)
			Korisnici[i].ID--;
		//otvara fajl KorisniciData.csv u Truncated modu, brise podatke sa fajla.
		pKorisniciFile->open("KorisniciData.csv", ios::trunc);
		pKorisniciFile->close();
		//otvara fajl u Append modu i dodjeljujemu naslovni line i takodjer sve korisnike bez obrisanog
		pKorisniciFile->open("KorisniciData.csv", ios::app);
		
		*pKorisniciFile << "Korisnici,ID,D.uc.,Mj.uc.,G.uc.,H.uc.,M.uc.,S.uc.,Ime,Prezime,Spol,D.rod.,Mj.rod.,G.rod.,Adresa st.,Broj tel." << endl;
		
		for(int i = 0; i < velicina - 1; i++)
		{
			*pKorisniciFile << " " << "," << Korisnici[i].ID << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_mday << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_mon + 1<< ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_year + 1900<< ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_hour << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_min << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_sec << ","
							<< Korisnici[i].Ime << ","
							<< Korisnici[i].Prezime << ","
							<< Korisnici[i].Spol << ","
							<< Korisnici[i].Dan << ","
							<< Korisnici[i].Mje << ","
							<< Korisnici[i].God << ","
							<< Korisnici[i].AdresaStanovanja << ","
							<< Korisnici[i].BrojTelefona << endl;
		}
	}
	else if(key == 27)
		system("CLS");
	pKorisniciFile->close();
}

//funkcija koja pruza korisniku programa opciju da promijeni vrijednosti atributa nekoh korisnika
void editujKorisnika(vector<KORISNIK>* pFiltrirano)
{
	char key;
	
	LOKACIJA = "1.4.0.0";
	system("CLS");
	int id = pretragaMain(pFiltrirano); //dobavlja trazeni ID Korisnika
	int velicina = Korisnici.size();
	cout << "\n\n";
	cout << id << endl;
	cout << Korisnici[id - 1].Ime;
	string line;
	int lokacija = 0;
	cout << "Da li zelite editovati Korisnika?\nDA = [ENTER] - NE = [ESC]";
	key = _getch();
	if(key == 13)
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
			cout << id; vratiBoju();
			cout << ". Korisnika";
			
			//printa atribute korisnika koji su od ranije uneseni, ako nema onda printa "N/A"
			Korisnici[id - 1].ID = id;
			
			cout << "\n\n\t\t(1/8) Uneseno Ime: ";
			if(Korisnici[id - 1].Ime[0] != NULL)
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].Ime; vratiBoju();
			}
			else
				cout << "N/A";
			
			cout << "\n\t\t(2/8) Uneseno Prezime: ";
			if(Korisnici[id - 1].Prezime[0] != NULL)
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].Prezime; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(3/8) Uneseni Spol: ";
			if(Korisnici[id - 1].Spol[0] != NULL)
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].Spol; vratiBoju();
			}
			else
				cout << "N/A";
					
			cout << "\n\t\t(4/8) Uneseni Dan Rodjenja: ";
			if(Korisnici[id - 1].Dan != "")
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].Dan; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(5/8) Uneseni Mjesec Rodjenja: ";
			if(Korisnici[id - 1].Mje != "")
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].Mje; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(6/8) Unesena Godina Rodjenja: ";
			if(Korisnici[id - 1].God != "")
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].God; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(7/8) Unesena Adresa Stanovanja: ";
			if(Korisnici[id - 1].AdresaStanovanja[0] != NULL)
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].AdresaStanovanja; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\t\t(8/8) Uneseni Broj Telefona: ";
			if(Korisnici[id - 1].BrojTelefona[0] != NULL)
			{
				odabranaBoja(pGlobalPOSTAVKE->bojaReal);
				cout << Korisnici[id - 1].BrojTelefona; vratiBoju();
			}
			else
				cout << "N/A";
				
			cout << "\n\n";
			
			if(lokacija == 8)
				lokacija++;
				
			//Ovdje dopusta unos atributa korisnika tako sto overwrite-a vrijednosti tog objekta
			switch(lokacija)
			{
				case 0:
				{
					cout << "\t\tUnesite Ime: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(Korisnici[id - 1].Ime, 20); vratiBoju();
					lokacija++;
					break;
				}
				case 1:
				{
					cout << "\t\tUnesite Prezime: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(Korisnici[id - 1].Prezime, 30); vratiBoju();
					lokacija++;
					break;
				}
				case 2:
				{
					cout << "\t\tUnesite Spol: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(Korisnici[id - 1].Spol, 20); vratiBoju();
					lokacija++;
					break;
				}
				case 3:
				{
					cout << "\t\tUnesite Dan Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin >> Korisnici[id - 1].Dan; vratiBoju();
					lokacija++;
					break;
				}
				case 4:
				{
					cout << "\t\tUnesite Mjesec Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin >> Korisnici[id - 1].Mje; vratiBoju();
					lokacija++;
					break;
				}
				case 5:
				{
					cout << "\t\tUnesite Godinu Rodjenja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin >> Korisnici[id - 1].God; vratiBoju();
					lokacija++;
					cin.clear();
					cin.ignore(1000, '\n');
					break;
				}
				case 6:
				{
					cout << "\t\tUnesite Adresu Stanovanja: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(Korisnici[id - 1].AdresaStanovanja, 40); vratiBoju();
					lokacija++;
					break;
				}
				case 7:
				{
					cout << "\t\tUnesite Broj Telefona: "; odabranaBoja(pGlobalPOSTAVKE->bojaReal);
					cin.getline(Korisnici[id - 1].BrojTelefona, 20); vratiBoju();
					lokacija++;
					break;
				}
			}
			if(lokacija == 9)
			{
				//opet brise podatke iz fajla
				pKorisniciFile->open("KorisniciData.csv", ios::trunc);
				pKorisniciFile->close();
				pKorisniciFile->open("KorisniciData.csv", ios::app);
				//ali sada upisuje naslov line i citav vektor korisnika, s tom promjenom
				*pKorisniciFile << "Korisnici,ID,D.uc.,Mj.uc.,G.uc.,H.uc.,M.uc.,S.uc.,Ime,Prezime,Spol,D.rod.,Mj.rod.,G.rod.,Adresa st.,Broj tel." << endl;
				
				for(int i = 0; i < velicina; i++)
				{
					*pKorisniciFile << " " << "," << Korisnici[i].ID << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_mday << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_mon + 1<< ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_year + 1900<< ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_hour << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_min << ","
							<< Korisnici[i].VrijemeUclanjivanja.tm_sec << ","
							<< Korisnici[i].Ime << ","
							<< Korisnici[i].Prezime << ","
							<< Korisnici[i].Spol << ","
							<< Korisnici[i].Dan << ","
							<< Korisnici[i].Mje << ","
							<< Korisnici[i].God << ","
							<< Korisnici[i].AdresaStanovanja << ","
							<< Korisnici[i].BrojTelefona << endl;
				}
				break;
			}
			
		}
	}
	else if(key == 27)
		system("CLS");
	pKorisniciFile->close();
}

int main()
{
	//data tip za provjeru csv fajla, da li postoji
	checkPostavkepostavkeFile.open("PostavkeData.csv");
	//data tip za citanje iz fajla
	inPostavkepostavkeFile.open("PostavkeData.csv");
	//data tip za pisanje u fajl
	postavkeFile.open("PostavkeData.csv", ios::app);
	
	if(!(checkPostavkepostavkeFile.good()))//ako fajl ne postoji, kreira fajl, unese naslov za svaki atribut i uvom slucaju za postavke samo drugu i zadnju liniju za pohranu postavki
	{
		*pPostavkeFile << "Postavke,Boja,Tip Selekcije,Format Datuma,Prikazi Grafik" << endl;
		*pPostavkeFile << " " << "," << pGlobalPOSTAVKE->bojaReal
			   << "," << pGlobalPOSTAVKE->tipSelekcije 
			   << "," << pGlobalPOSTAVKE->tipFormatDatuma
			   << "," << pGlobalPOSTAVKE->prikaziGrafik << endl;
	}
	else
		parsePOSTAVKE();//funckija koja pohranjuje iz HDD/SSD u RAM
		
		
	checkPaketiFile.open("PaketiData.csv");
	inPaketiFile.open("PaketiData.csv");
	paketiFile.open("PaketiData.csv", ios::app);
	
	if(!(checkPaketiFile.good()))
	{
		*pPaketiFile << "Paketi,ImePaketa,BrojSesijaPoSedmici,CijenaSesije,UkupnaCijena" << endl;

	}
	else
		parsePAKETI();
		
	checkKorisniciFile.open("KorisniciData.csv");
	inKorisniciFile.open("KorisniciData.csv");
	korisniciFile.open("KorisniciData.csv", ios::app);

	if(!(checkKorisniciFile.good()))
	{
		*pKorisniciFile << "Korisnici,ID,D.uc.,Mj.uc.,G.uc.,H.uc.,M.uc.,S.uc.,Ime,Prezime,Spol,D.rod.,Mj.rod.,G.rod.,Adresa st.,Broj tel." << endl;
	}
	else
		parseKORISNICI();
		
		
	vector<KORISNIK>* pFiltrirano;
	int brojOpcija = 12;
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
				printajTabeluKorisnika();
				break;
			}
			case 3:
			{
				char y;
				if(Korisnici.size() != 0)
					pretragaMain(pFiltrirano);
				else
				{
					system("CLS");
					cout << "Baza Podataka je Prazna\n\nNAZAD = [ESC]";
					y = _getch();
					if(y == 27)
						system("CLS");
				}
				break;
			}
			case 4:
			{
				char y;
				if(Korisnici.size() != 0)
				{
					pKorisniciFile->close();
					editujKorisnika(pFiltrirano);
					pKorisniciFile->open("KorisniciData.csv", ios::app);
				}
				else
				{
					system("CLS");
					cout << "Baza Podataka je Prazna\n\nNAZAD = [ESC]";
					y = _getch();
					if(y == 27)
						system("CLS");
				}
				break;
			}
			case 5:
			{
				char y;
				if(Korisnici.size() != 0)
				{
					pKorisniciFile->close();
					obrisiKorisnika(pFiltrirano);
					pKorisniciFile->open("KorisniciData.csv", ios::app);
				}
				else
				{
					system("CLS");
					cout << "Baza Podataka je Prazna\n\nNAZAD = [ESC]";
					y = _getch();
					if(y == 27)
						system("CLS");
				}
				break;
			}
			case 6:
			{
				//cin.ignore(1000, '\n');
				pPaketiFile->close();
				unosPaketaClassic(printajTeg);
				pPaketiFile->open("PaketiData.csv", ios::app);
				
				break;
			}
			case 7:
			{
				printajTabeluPaketa();
				break;
			}
			case 8:
			{
				break;
			}
			case 9:
			{
				break;
			}
			case 10:
			{
				char y;
				if(Korisnici.size() != 0)
					UplataClanarinaMain(pFiltrirano);
				else
				{
					system("CLS");
					cout << "Baza Podataka je Prazna\n\nNAZAD = [ESC]";
					y = _getch();
					if(y == 27)
						system("CLS");
				}
				break;
			}
			case 11:
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
	delete pFiltrirano;//brise dinamicko alokaciju za vektor filtriranih korisnika
	system("PAUSE");
	return 0;
}

int meni(int brojOpcija)
{
	char const* POSTAVKE[brojOpcija]; //niz opcija
	POSTAVKE[0] = "Unos Korisnika";

	POSTAVKE[1] = "Tabelarni Ispis Korisnika";
	POSTAVKE[2] = "Pretraga Korisnika";
	POSTAVKE[3] = "Editovanje Korisnika";
	POSTAVKE[4] = "Brisanje Korisnika";
	POSTAVKE[5] = "Unos Trening Paketa";
	POSTAVKE[6] = "Tabelarni Ispis Paketa";
	POSTAVKE[7] = "";
	POSTAVKE[8] = "Opcija 9";
	POSTAVKE[9] = "Evidencija o Clanarini";
	POSTAVKE[10] = "Postavke";
	POSTAVKE[11] = "EXIT";
	LOKACIJA = "1.0.0.0";
	selekcijaLogika(POSTAVKE, brojOpcija, LOKACIJA, "MENI:", printajNaslov);
}

void POSTAVKEPromjena()
{
	ulaz:
	LOKACIJA = "11.0.0.0";
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
		//slicno kao selekcijaLogika samo sto dopusta horizontalnu promjenu opcija <- -> za odabir boje
		case 1:
		{
			case1:
			LOKACIJA = "11.1.0.0";
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
			LOKACIJA = "11.2.0.0";
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
			LOKACIJA = "11.3.0.0";
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
			LOKACIJA = "11.4.0.0";
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
			LOKACIJA = "11.5.0.0";
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
			LOKACIJA = "11.6.0.0";
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
