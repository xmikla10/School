#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cctype>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <fcntl.h>

using std::cout;
using std::endl;
using std::string;
using std::cerr;
using std::fstream;
using std::ofstream;
using namespace std;
using std::stringstream;
using std::getline;

/***********STRUKTURA************/
typedef struct Data
{
    string serverIP; // -s
    string loginFile; // -c
    bool active; // -a
    int portNumber; // -a
    bool pasive; // -p
    string fileName; // -u, -d, -r
    bool save; // -u
    bool down; // -d
    bool del; // -r
    bool nlst;
    bool mkd;
    bool rmd;
    string pathToFile; // -P
    string username;
    string password;
    string pasvMessage;
    string isdir;
    string concrete_dir;
    string strpwd;
    string concrete_dir1;

} TStruct;
/***********STRUKTURA************/

/***********FUNKCIE************/
/*Pomocna funkcia pre vypis napovedy*/
void print_help();
/*Funkcia pre spracovanie argumentov*/
int arguments(int argc, char *argv[], TStruct *data);
/*Funkcia pre citanie zo suboru*/
void readFile(string pomstr, TStruct *data);
/*Funkcia na vratenie chybovej hlasky*/
void perror(const char *message);
/*Funkcia pre vytvorenie pripojenia a prihlasenia sa na server*/
int socketFunction( TStruct data, int *pomSocket );
/*Funkcia reprezentujuca Aktivny mod*/
void activeMode(int sSocket, TStruct data);
/*Funkcia reprezentujuca Pasivny mod*/
void passiveMode(int sSocket, TStruct data);
/*Funkcia pre vymazanie suboru*/
void del(TStruct data, int sSocket);
/*Funkcia pre ukoncenie pripojenia*/
void quit(int sSocket, TStruct data);
/*Pomocna funkcia pre upravu IP adresy*/
void editIP( char *str);
/*Funkcia pre zistenie pwd*/
void pwd( int sSocket);
/*Funkcia reprezentujuca unixovy prikaz CD*/
void cd(int sSocket, string pathToFile, TStruct data);
/*Funkcia na prijatie spravy*/
int recvM(int sSocket, TStruct *data);
/*Funkcia pre download*/
void get (int sSocket, int sd_server, TStruct data);
/*Funkcia pre upload*/
void put (int sSocket, int sd_server, TStruct data);
/*Funkcia pre vypis adresarovej struktury*/
void ls (int sSocket, int sd_server, TStruct data);
/*Funkcia pre zobrazenie adresovej struktury daneho priecinku*/
void dirStructure(char buff[1024], int sSocket, TStruct data, int sd_server);
/*pomocna funkcia pre dirStructure(), ktora nastavuje novu hodnotu stringom v struktue
  podla aktualneho adresara*/
void selectDir( TStruct *data, string isdir);
/*funckia pre vypis cesty aktualneho adresara*/
void pwd(int sSocket, TStruct *data);
/*Funkcia pre vypis adresarovej struktury pri pasivnom mode*/
void pasv_ls (int sSocket, int sd_server, TStruct data);
/*Funkcia pre download pri pasivnom mode*/
void pasv_get (int sSocket, int sd_server, TStruct data);
/*Funkcia pre upload pri pasivnom mode*/
void pasv_put (int sSocket, int sd_server, TStruct data);
/*Vypise list suborov v danom adresary */
void nlst (int sSocket, int sd_server);
/*Vypise list suborov v danom adresary v pasivnom mode*/
void pasv_nlst (int sSocket, int sd_server);
/*Funkcia pre vytvorenie priecinku*/
void mkd(TStruct data, int sSocket);
/*Funkcia pre vymazanie  priecinku*/
void rmd(TStruct data, int sSocket);
/***********FUNKCIE************/
