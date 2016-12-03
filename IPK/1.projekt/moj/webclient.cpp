/* Autor : xmikla10																					*
* Datum : 25.3.2016																					*
* Vytvořte program (klient) s využitím rozhraní schránek (sockets API), který implementuje stažení	*
* zadaného objektu pomocí URL z WWW serveru s využitím  protokolu	HTTP 1.0 nebo HTTP 1.1 do 		*
* souboru	uloženého v lokálním souborovém systému.												*
* Program vytvořte v jazyce C/C++, který je přeložitelný na studentském unixovém serveru			*
* eva.fit.vutbr.cz (FreeBSD) včetně funkčního Makefile souboru (program přeložitelný po zadání		*
* příkazu	make) . Program	 využívá spojovanou službu (protokol TCP). Jméno přeloženého programu	*
* klienta	bude webclient.																			*
* Program	 předpokládá jeden povinný paramet a to	URL identifikující objekt, který bude uložen do	*
* aktuálního adresáře. Pokud v dotazu URLnení	 uvedeno jméno souboru, obsah bude uložen do		*
* souboru	 index.html . Oznámení o chybách (stavové kódy 4xx, 5xx), které mohou nastat, budou		*
* vytištěny na standardní chybový výstup (stderr). Ze stavových kódů 3xx implementujte minimálně	*	
* podporu	pro	kódy 301 a 302. Maximální počet přesměrování uvažujte 5.							
*/

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
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

#include <limits.h>

#define PORT 80
#define OK "ok\n"

#ifndef HTTP
	#define HTTP
#endif

using std::cout;
using std::endl;
using std::string;
using std::ofstream;
using std::cerr;
using namespace std;

const char *SAVE_TO_INDEX = "index.html";
const int DEFAULT_PORT = 80;

typedef struct Data
{
	int port;
	string server_name;
	string path;
	string outfile;
	string pom;
	string pom2;
	string url;
	string start;
} TStruct;

typedef struct Data2
{
	string url;
} TStruct2;

void print_help();
bool function_arguments(string pom_url, TStruct *data);
void perror(const char *message);
int createANDconnect(int *_socket, TStruct data);
int communikateServer(TStruct data, int *chunked, int socket, string *pom);
int writeAndSave (TStruct data, string msg, int chunked);
string strip(string in);
void strrev(char *str);
int redir(string *x);
int findLocation(string *x, TStruct2 *data2);

int main ( int argc, char *argv[])
{

	if (argc != 2)
  	{
  		print_help();
  		return true;
  	}

	std::string pom_url(argv[1]);

	int a = 0;
	int _socket = 0;
	int chunked = 0;
	string pom = "";
	TStruct data = {PORT, "", "/", "index+.html","","","",""};
	TStruct2 data2 = {""};

  	function_arguments(pom_url,&data);

  	createANDconnect( &_socket, data);

	communikateServer(data, &chunked, _socket, &pom);
	//redir(&pom);

	if (redir(&pom) == 2 )
	{
		if (findLocation(&pom, &data2) == 1 )
		{
			pom_url = (data2.url).c_str();
			for (int i = 0; i < 5; i++) 
			{
				TStruct data = {PORT, "", "/", "index.html","","","",""};
				function_arguments(pom_url, &data);
					int min = (data.outfile).length() -1;
					data.outfile = data.outfile.substr(0, min);
				createANDconnect( &_socket, data);
				pom.clear();
				communikateServer(data, &chunked, _socket, &pom);
				writeAndSave(data, pom, chunked);
				int aux = redir(&pom);
				if (aux == 1 || aux == 3 )
				{
					break;
				}
				else
				{
					findLocation(&pom, &data2); //redirect
					pom_url = (data2.url).c_str();
				}
			}
			a = 1;
		}
	}
	if ( a == 0)
  		writeAndSave(data, pom, chunked);
}

int findLocation(string *x, TStruct2 *data2)
{
	string pom = *x;
	int tmp = pom.find("Location:");
	pom.erase(0, tmp+9);
	tmp = (pom).find("\n");
	data2->url = pom.erase(tmp);
	data2->url = data2->url.substr(1,data2->url.length());
	return 1;
}


int redir(string *x)
{
	string pom = *x;
	pom.erase(0, 9);

	int tmp = (pom).find("\n");
	(pom).erase(tmp);

	if( pom.find("200") != string::npos )
		return 1;

	else if( pom.find("301") != string::npos )
		return 2;

	else if( pom.find("302") != string::npos )
		return 2;
	else
	{
		perror("nieco doplnit\n");
		return 3;
	}
}


//funkcia na vytvorenie socketu a pripojenie na server
int createANDconnect(int *_socket, TStruct data)
{
	int sSocket;
	struct sockaddr_in server_socket;
	struct hostent *host;


	if (( sSocket = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
		perror("socket cannot be created\n");

	//metoda c_str() vrací konstantní pole znaků. 
	if((host = gethostbyname(data.server_name.c_str())) == NULL)
	{
		perror("gethostbyname problem\n");
	}

	server_socket.sin_family = AF_INET;
	server_socket.sin_port = htons(data.port);

	//Zkopíruje n bajtů z paměti označené pomocí src do dest.
	// Vrací ukazatel na dest. Paměti se nesmějí překrývat.
	/*Vrací 0 v případě, že jsou všechny bajty stejné; hodnotu větší jak 0,
	 když s1 je větší jak s2; hodnotu menší jak 0, když s1 je menší jak s2.*/
	memcpy( &(server_socket.sin_addr), host->h_addr, host->h_length); //server IP

	if (connect ( sSocket, (struct sockaddr *)&server_socket, sizeof(server_socket) ) != 0 )
		perror("connection failed\n");

	*_socket = sSocket;

	return 0;
}
int communikateServer(TStruct data, int *chunked, int socket, string *pom)
{
	char buffer[1024];

	#ifdef HTTP
 			string http = 	"GET " + data.path + " HTTP/1.1\r\n"
  							"Host: " + data.server_name + "\r\n"
  							"Connection: close\r\n\r\n";
  	#else
 			string http = 	"GET " + data.path + " HTTP/1.0\r\n"
 							"Host: " + data.server_name + "\r\n\r\n";

 	#endif

	if (write(socket, http.c_str(), http.size()) < 0 )
	{
		perror("cannot send message to server\n");
	}

	int x;

	while ((x = read(socket, buffer, 1024)) > 0)
	{
		if (x < 0) perror("cannot read\n");
		else (*pom).append(buffer, x);
	} 
	//cout << *pom << endl;
	close(socket);

	if ((*pom).find("Transfer-Encoding: chunked\r\n") != string::npos)
		*chunked = 1;
	else
		*chunked = 0;

	return 0;

}

int writeAndSave (TStruct data, string pom, int chunked)
{
	size_t x;
	string chunk_number = "";
	string pom1 = "";
	long int number = -1;

	x = pom.find("\r\n\r\n");
	if (x != string::npos)
	{
		pom.erase( 0, x+4 );
	}

	/*if( chunked == 1)
	{
		while (number != 0) 
		{
			if (( x = pom.find("\r\n")) != string::npos) 
			{
				chunk_number = pom.substr(0, x);
				chunk_number.append("\0");
				number = strtol(chunk_number.c_str(), NULL, 16);
				pom.erase(0, x+2);
				pom1.append(pom, 0, number);
				pom.erase(0, x+2);
			}
		}
		pom = pom1;
	}
*/

	//cout << pom << endl;
	if(data.outfile.empty())	
		data.outfile = "index.html";
	ofstream outfile;
	outfile.open(data.outfile.c_str(), ios::out | ios::binary);
	outfile.write(pom.c_str(), pom.length());
	outfile.close();
	return 0;
}


//funkcia na vratenie chybovej hlasky
void perror(const char *message)
{
	cerr << "Error: " << message << endl;
	//exit();
	/* use : perror("text\n"); */
}
//funkcia na spracovanie argumentov
bool function_arguments(string pom_url, TStruct *data)
{

	size_t url;
	//pomocna premenna pre najdenie ":"
	size_t pom1;
	//pomocna premenna pre najdenie "/"
	size_t pom2;
	//pomocna premenna pre najdenie "/"
	size_t pom3;
	//pomocna premenna pre najdenie " "
	size_t pom4;
	//pomocna premenna pre najdenie "%20"
	size_t pom5;
	size_t strPom;

	if((url = pom_url.find("http://")) != string::npos)
	{
		pom_url.erase(url, 7);
	}
	else
	{
		perror("missing http://\n");
	}

	pom1 = pom_url.find(":");
	pom2 = pom_url.find("/");

		//ak je port :
		if( pom1 != string::npos)
		{
			// ulozi server name
			data->server_name = pom_url.substr( 0, pom1);
			// ulozi path
			data->pom = pom_url.substr( pom1, pom2);
			string port_pom = pom_url;
			data->pom2 = pom_url;
			// ulozi path
			data->path = pom_url.erase( 0, pom2);
			// ulozi port
			data->port = atoi(strip(port_pom).c_str());
		/*	cout << data->server_name << endl; 
			cout << data->path << endl; 
			cout << data->port << endl;*/
		}
		//ak nieje port a je len /
		else
		{
			pom3 = pom_url.find("/");
			//ak najde dalsie lomitko cize tam je path nejaky
			if ( pom3 != string::npos)
			{
 				data->server_name = pom_url.substr( 0, pom3);
 				data->path = pom_url.erase( 0, pom3);

				pom4 = data->path.find(" ");
				if (pom4 != string::npos)
				{
					data->path.replace(pom4, 1, "%20");
				}
			}
			//ak nieje ziadna path tak len ulozi meno serveru a dalej
			// to uklada do index.html
			else
				data->server_name = pom_url;
		}

	if ( data->path == "/")
	{
  		data->outfile = "index.html";
	}
	// ak path nieje len lomitko
  	else
  	{
  		string str = data->path; 
  		std::reverse(str.begin(), str.end());
  		strPom = str.find("/");
  		string revers = str.substr( 0, strPom);
  		std::reverse(revers.begin(), revers.end());
  		data->outfile = revers;

  		pom5 = data->outfile.find("%20");
  		if (pom5 != string::npos)
  		{
			data->outfile.replace(pom5, 3, " ");
  		}
  	}
  			/*cout << data->server_name << endl; 
			cout << data->path << endl; 
			cout << data->port << endl;*/
  	return true;
}

string strip(string in)
{
    string final;
    int pom = in.size();
    for(int i = 0; i < pom; i++) 
    {
        if(isdigit(in[i])) 
        	final += in[i];
    }
    return final;
}

void strrev(char *str)
{
    if (str == NULL)
        return;
    std::reverse(str, str + strlen(str));
}


void print_help()
{
 	cout <<"WEBCLIENT \n";
 	cout <<"Author : xmikla10 \n";
 	cout <<"\n";
	cout <<"For use Webclient, you can try : \n";
	cout <<"\n";
	cout <<"webclient http://www.fit.vutbr.cz \n";
	cout <<"Stáhne defaultní stránku a uloží do souboru index.html \n";
	cout <<"\n";
	cout <<"webclient http://www.fit.vutbr.cz:80/common/img/fit_logo_cz.gif \n";
	cout <<"Stáhne a uloží obrázek fit_logo_cz.gif do souboru \n";
	cout <<"\n";
	cout <<"webclient http://www.fit.vutbr.cz/study/courses/IPK/public/some text.txt \n";
	cout <<"Stáhne a uloží soubor some text.txt do souboru \n";
	cout <<"\n";
	cout <<"webclient http://www.fit.vutbr.cz/study/courses/IPK/public/test/redir.php \n";
	cout <<"Stáhne a uloží soubor, na který byla přesměrována odpověď \n";
	cout <<"\n";
}