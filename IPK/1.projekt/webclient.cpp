/* 
* 	1.projekt do Predmetu IPK		
* 	WEBCLIENT 						
*									
* 	Autor : xmikla10				
*	Datum : 25.3.2016				
*						
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
#define HTTP

using std::cout;
using std::endl;
using std::string;
using std::ofstream;
using std::cerr;
using namespace std;

/*Hlavna struktura pre ukladanie portu, mena serveru, atd..*/
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

/*Pomocna struktura pre URL pri presmerovani*/
typedef struct Data2
{
	string url;
} TStruct2;

/*Pomocna funkcia pre vypis help*/
void print_help();
/*funkcia na spracovanie argumentov*/
bool function_url(string pom_url, TStruct *data);
/*funkcia na vratenie chybovej hlasky*/
void perror(const char *message);
/*funkcia na vytvorenie socketu a pripojenie na server*/
int createANDconnect(int *_socket, TStruct data);
/*Funkcia pre zasielanie sprav a komunikaciu so serverom*/
int communikateServer(TStruct data, int *chunked, int socket, string *pom);
/*Funkcia na zapis do suboru*/
int saveTOoutfile (TStruct data, string msg, int chunked);
/*Pomocna funkcia*/
string strip(string in);
/*Pomocna funkcia*/
void strrev(char *str);
/*Funkcia pre stavove kody 200, 301, 302, 4xx, 5xx */
int redir(string *x);
/*Funkcia pre najdenie Location pri presmerovani*/
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

  	function_url(pom_url,&data);
  	createANDconnect( &_socket, data);
	communikateServer(data, &chunked, _socket, &pom);

	if (redir(&pom) == 2 )
	{
		if (findLocation(&pom, &data2) == 1 )
		{
			pom_url = (data2.url).c_str();
			for (int i = 0; i < 5; i++) 
			{
				TStruct data = {PORT, "", "/", "index.html","","","",""};
				function_url(pom_url, &data);
				int min = (data.outfile).length() -1;
				data.outfile = data.outfile.substr(0, min);
				createANDconnect( &_socket, data);
				pom.clear();
				communikateServer(data, &chunked, _socket, &pom);
				saveTOoutfile(data, pom, chunked);
				int aux = redir(&pom);

				if (aux == 1 || aux == 3 )
					break;
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
  		saveTOoutfile(data, pom, chunked);
}

/*Funkcia pre najdenie Location pri presmerovani*/
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

/*funkcia na vytvorenie socketu a pripojenie na server*/
int createANDconnect(int *_socket, TStruct data)
{
	struct hostent *host;
	struct sockaddr_in server_socket;

	int sSocket = socket(AF_INET, SOCK_STREAM, 0 );

	if ( sSocket < 0)
		perror("Socket cannot be created\n");

	//metoda c_str() vrací konstantní pole znaků. 
	host = gethostbyname(data.server_name.c_str());

	if(host == NULL)
	{
		perror("Gethostbyname problem\n");
	}

	server_socket.sin_family = AF_INET;
	server_socket.sin_port = htons(data.port);

	memcpy( &(server_socket.sin_addr), host->h_addr, host->h_length);

	if (connect ( sSocket, (struct sockaddr *)&server_socket, sizeof(server_socket) ) != 0 )
		perror("Connection failed\n");

	*_socket = sSocket;

	return 0;
}

/*Funkcia pre zasielanie sprav a komunikaciu so serverom*/
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
		perror("Cannot send message to server\n");

	int x;
	while ((x = read(socket, buffer, 1024)) > 0)
	{
		if (x < 0) 
			perror("Cannot read\n");
		else 
			(*pom).append(buffer, x);
	} 

	close(socket);

	string chunk = *pom;
	if (chunk.find("Transfer-Encoding: chunked\r\n") != string::npos)
		*chunked = 1;
	else
		*chunked = 0;

	return 0;

}

/*Funkcia na zapis do suboru*/
/*Funkica takisto spracuje chunked data*/
int saveTOoutfile (TStruct data, string pom, int chunked)
{
	size_t x = pom.find("\r\n\r\n");
	long int number = -1;

	if (x != string::npos)
	{
		pom.erase( 0, x + 4 );
	}

	if( chunked == 1)
	{
		string pom1 = "";

		while (number != 0) 
		{
			x = pom.find("\r\n");

			if (( x = pom.find("\r\n")) != string::npos) 
			{
				string chunk_number = "";
				chunk_number = pom.substr( 0, x );
				chunk_number.append( "\0" );
				number = strtol( chunk_number.c_str(), NULL, 16 );
				pom.erase( 0, x + 2 );
				pom1.append( pom, 0, number );
				pom.erase( 0, number + 2 );
			}
		}
		pom = pom1;
	}

	if(data.outfile.empty())	
		data.outfile = "index.html";

	ofstream outfile;
	outfile.open(data.outfile.c_str(), ios::out);
	outfile.write(pom.c_str(), pom.length());
	outfile.close();
	return 0;
}


/*funkcia na vratenie chybovej hlasky*/
void perror(const char *message)
{
	cerr << "Error: " << message << endl;
	/* use : perror("text\n"); */
}
/*funkcia na spracovanie argumentov*/
bool function_url(string pom_url, TStruct *data)
{
	//pomocna premenna pre url
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
	//pomocna premenna
	size_t strPom;

	if((url = pom_url.find("http://")) != string::npos)
		pom_url.erase(url, 7);
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
  	return true;
}

/*Pomocna funkcia*/
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

/*Pomocna funkcia*/
void strrev(char *str)
{
    if (str == NULL)
        return;
    std::reverse(str, str + strlen(str));
}

/*Funkcia pre stavove kody 200, 301, 302, 4xx, 5xx */
int redir(string *x)
{
	string pom = *x;
	pom.erase(0, 9);
	int tmp = (pom).find("\n");
	pom.erase(tmp);

	if( pom.find("200") != string::npos ) return 1;
	else if( pom.find("301") != string::npos ) return 2;
	else if( pom.find("302") != string::npos ) return 2;
	/*4xx*/
	else if( pom.find("400") != string::npos ) { cerr << "400" << endl; exit(1); } // Bad Request
	else if( pom.find("401") != string::npos ) { cerr << "401" << endl; exit(1); } // Unauthorized
	else if( pom.find("402") != string::npos ) { cerr << "402" << endl; exit(1); } // Payment Required
	else if( pom.find("403") != string::npos ) { cerr << "403" << endl; exit(1); } // Forbidden
	else if( pom.find("404") != string::npos ) { cerr << "404" << endl; exit(1); } // Not Found
	else if( pom.find("405") != string::npos ) { cerr << "405" << endl; exit(1); } // Method Not Allowed
	else if( pom.find("406") != string::npos ) { cerr << "406" << endl; exit(1); } // Not Acceptable
	else if( pom.find("407") != string::npos ) { cerr << "407" << endl; exit(1); } // Proxy Authentication Required
	else if( pom.find("408") != string::npos ) { cerr << "408" << endl; exit(1); } // Request Timeout
	else if( pom.find("409") != string::npos ) { cerr << "409" << endl; exit(1); } // Conflict
	else if( pom.find("410") != string::npos ) { cerr << "410" << endl; exit(1); } // Gone
	else if( pom.find("411") != string::npos ) { cerr << "411" << endl; exit(1); } // Length Required
	else if( pom.find("412") != string::npos ) { cerr << "412" << endl; exit(1); } // Precondition Failed
	else if( pom.find("413") != string::npos ) { cerr << "413" << endl; exit(1); } // Request Entity Too Large
	else if( pom.find("414") != string::npos ) { cerr << "414" << endl; exit(1); } // Request-URI Too Long
	else if( pom.find("415") != string::npos ) { cerr << "415" << endl; exit(1); } // Unsupported Media Type
	else if( pom.find("416") != string::npos ) { cerr << "416" << endl; exit(1); } // Requested Range Not Satisfiable
	else if( pom.find("417") != string::npos ) { cerr << "417" << endl; exit(1); } // Expectation Failed
	/*5xx*/
	else if( pom.find("500") != string::npos ) { cerr << "500" << endl; exit(1); } // Internal Server Error
	else if( pom.find("501") != string::npos ) { cerr << "501" << endl; exit(1); } // Not Implemented
	else if( pom.find("502") != string::npos ) { cerr << "502" << endl; exit(1); } // Bad Gateway
	else if( pom.find("503") != string::npos ) { cerr << "503" << endl; exit(1); } // Service Unavailable
	else if( pom.find("504") != string::npos ) { cerr << "504" << endl; exit(1); } // Gateway Timeout
	else if( pom.find("505") != string::npos ) { cerr << "505" << endl; exit(1); } // HTTP Version Not Supported
	else
	{
		perror("Something another\n");
		exit(1);
	}
}

/*Pomocna funkcia pre vypis help*/
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