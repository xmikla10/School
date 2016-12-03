/* 
*   Projekt do Predmetu ISA       
*   FTP KLIENT                       
*                                   
*   Autor : xmikla10                
*   Datum : 25.10.2016              
*                       
*/

/* Poznamky :
	-- spýtať sa na -a port_numver kam ma ísť
	-- 
*/

//cerr << "   " << endl;

#include "fclient.h"
#define SIZE 1024
#define FTP_PORT 21

char *localIP;
//char *pomPWD;

/***********MAIN************/
int main ( int argc, char *argv[])
{

    if ( argc < 5 )
    {
        print_help();
        return true;
    }

    TStruct data = {"", "", false, 0, false, "", false, false, false, "", "", "", ""};
    arguments(argc, argv, &data);
    
    string loginFile = data.loginFile;
    readFile(loginFile, &data);

    int sSocket = 0;
    socketFunction( data, &sSocket);
    if ( data.pasive == true )
    	passiveMode( sSocket, data);
    else
    {
	    if( data.save == false && data.down == false && data.del == false)
	    	activeMode(sSocket, data);

	    if( data.down == true )
	    	get(sSocket, data);

	    if( data.save == true )
	    	put(sSocket, data);

	    if( data.del == true )
	    	activeMode(sSocket, data);
	}
    quit(sSocket, data);
}
/***********MAIN************/

/*Pomocna funckia*/
string IntToString ( int number )
{
  std::ostringstream oss;
  oss<< number;
  return oss.str();
}

/*Funkcia pre vytvorenie pripojenia a prihlasenia sa na server*/
int socketFunction( TStruct data, int *pomSocket )
{
	struct hostent *host;
	struct sockaddr_in pomSock;
	char user[SIZE];
	char pass[SIZE];
	char username [SIZE];
	char password [SIZE];

	int sSocket = socket(AF_INET, SOCK_STREAM, 0 );

	if ( sSocket < 0)
	{
		perror("Socket cannot be created\n");
		exit(1);
	}


	host = gethostbyname(data.serverIP.c_str());
	pomSock.sin_family = AF_INET;
	pomSock.sin_port = htons(21);
	memcpy( &(pomSock.sin_addr), host->h_addr, host->h_length);

	if (connect ( sSocket, (struct sockaddr *)&pomSock, sizeof(pomSock) ) < 0 )
	{
		perror("Connection failed\n");
		exit(1);
	}
	struct sockaddr_in localAddress;
	socklen_t addressLength = sizeof(localAddress);

	getsockname(sSocket, (struct sockaddr*)&localAddress, &addressLength);
	char *ip = inet_ntoa( localAddress.sin_addr);
    editIP(ip);
	// username zo stringu do array
	strncpy(username, (data.username).c_str(), sizeof(username));
	username[sizeof(username) - 1] = 0;

	// password zo stringu do array
	strncpy(password, (data.password).c_str(), sizeof(password));
	password[sizeof(password) - 1] = 0;

	if( recvM(sSocket, &data) != 220)
	{
		perror("USER wrong response code");
		exit(1);
	}

	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> USER >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
	strcat(user, "USER ");	
	strcat(user, username);
	strcat(user, "\r\n");

	if( send(sSocket , user , strlen(user), 0) < 0)
	{
        perror("send failed");
        exit(1);
	}

	if( recvM(sSocket, &data) != 331)
	{
		perror("USER wrong response code");
		exit(1);
	}
	memset(user, '\0', sizeof(char) );
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> USER >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PASS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
	strcat(pass, "PASS ");	
	strcat(pass, password);
	strcat(pass, "\r\n");

	if( send(sSocket , pass , strlen(pass), 0) < 0)
	{
        perror("send failed");
        exit(1);
	}

	if( recvM(sSocket, &data) != 230)
	{
		perror("PASS wrong response code");
		exit(1);
	}
	memset(pass, '\0', sizeof(char) );
	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PASS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
	if ( data.pathToFile != "")
		cd(sSocket, data.pathToFile, data);

	//pwd(sSocket);
	*pomSocket = sSocket;
	return 0;
}
int recvM(int sSocket, TStruct *data)
{
	int code;
	char serverBuff[4];
	char c;
	bool multiline;
	stringstream s;
	string msg = "";
	memset(&serverBuff[0], 0, sizeof(serverBuff));
	recv(sSocket, serverBuff, 3, 0);
	s<<serverBuff;
	s>>code;

	recv(sSocket, &c, 1, 0);
	if ( c == '-' )
	{
		recv(sSocket, &c,2,0);
		multiline = true;
	}
	else
		multiline = false;
	bool cr = false;
	int next= 0;

	if ( multiline == false)
	{
		do{
			int n = (int)recv(sSocket, &c,1,0);
			if ( n < 0)
			{
				perror("recv");
				exit(1);
			}
			if ( c == '\r')
				cr = true;
				next++;
			if ( c == '\n' && cr == true && next == 1)
				break;
			else
				next = 0;
				msg.append(1, c);
		} while (1);
	}

	cr = false;
	int pom = 0;
	bool bbreak = false;
	char a;
	if ( multiline == true){
		do{
			int n = (int)recv(sSocket, &c,1,0);
			pom++;
			if ( n < 0)
			{
				perror("recv");
				exit(1);
			}
			if ( c != '-' && pom == 4)
			{
				bbreak = true;
			}
			if ( c == '\r')
			{
				msg.append(1, c);
				recv(sSocket, &a,1,0);
				if ( a == '\n' && c == '\r')
				{
					if( bbreak == true)
						break;
					else
						msg.append(1, c);
						msg.append(1, a);
						pom = 0;
				}
				else
				{
					msg.append(1, c);
					msg.append(1, a);
				}
			}
			else
			{
				msg.append(1, c);			
			}
		} while (1);
	}

	if( code == 227)
		data->pasvMessage = msg;

	return code;
}

void passiveMode(int sSocket, TStruct data)
{
	char pasv[SIZE];
	cin.clear();
	char buff[SIZE];
	char pom[SIZE];

	strcat(pasv, "PASV");
	strcat(pasv, "\r\n");	

	if( send(sSocket , pasv , strlen(pasv), 0) < 0)
	{
        perror("send failed");
        exit(1);
	}

	if( recvM(sSocket, &data) != 227)
	{
		perror("PASV wrong response code");
		exit(1);
	}

	cerr << data.pasvMessage <<endl;

	strncpy(pom, (data.pasvMessage).c_str(), sizeof(pom));
	pom[sizeof(pom) - 1] = 0;

	int a1, a2, a3, a4, p1, p2, dataPort;
	sscanf(pom, "Entering Passive Mode (%d,%d,%d,%d,%d,%d).", &a1,&a2,&a3,&a4,&p1,&p2);
    dataPort = (p1 * 256) + p2;

    cerr << "Port:"  << endl;
    cerr << dataPort  << endl;

    struct hostent *host;
    host = gethostbyname(data.serverIP.c_str());

	struct sockaddr_in servAdr;             // Internet address of server
    memset(&servAdr, 0, sizeof(servAdr));   // Clear structure
    servAdr.sin_family = AF_INET;           // Set address typedef
    memcpy(&servAdr.sin_addr, host->h_addr, host->h_length);
    servAdr.sin_port = htons(FTP_PORT);

    struct sockaddr_in servAdr2;                // Internet address of server
    memset(&servAdr2, 0, sizeof(servAdr2));     // Clear structure

    servAdr2.sin_family = AF_INET;              // Set address typedef
    memcpy(&servAdr2.sin_addr, host->h_addr, host->h_length);
    servAdr.sin_port = htons(dataPort);

    int dataSock;
    if ((dataSock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Client: generate error");
        exit(1);
    }

    if (connect(dataSock, (struct sockaddr *)&servAdr, sizeof(servAdr)) < 0)
    {
        perror("Client: connect error");
        exit(1);
    }

    read(dataSock, buff, sizeof(buff));
    write(fileno(stdout), buff, sizeof(buff));

    char list[SIZE]; 
	memset(list, '\0', SIZE * sizeof(char) );
	strcat(list,"LIST ");
	strcat(list, "\r\n");

	if( send(sSocket, list, strlen(list), 0) < 0 )
	{
		perror("send");
	}
	recvM(sSocket, &data);
	cin.clear();

}
void activeMode(int sSocket, TStruct data)
{
	char port[SIZE];
	char list[SIZE]; 
	char buff[SIZE];
	char port1 [SIZE];
	char port2 [SIZE];
	int min_port = 1025;
	int max_port = 65535;
	struct sockaddr_in serv_addr, cl_addr;
	int sd_client, sd_server, i;
	socklen_t len;	

	memset(list, '\0', SIZE * sizeof(char) );
	strcat(list,"LIST ");
	strcat(list, "\r\n");
 	
	if( 0 > (sd_server = socket(AF_INET, SOCK_STREAM, 0) ))
	{
		perror("socket");
		exit(1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr  = INADDR_ANY;
	memset(&(serv_addr.sin_zero), '\0', 8);
	
	if ( data.active == false)	
	{
		for( i = min_port; i <= max_port; i++)
		{
			serv_addr.sin_port = htons(i);
			if(0 <= bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) ) 
				break;
		}
	}
	else
	{
		serv_addr.sin_port = htons(data.portNumber);
		bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
		i = data.portNumber;
	}

	serv_addr.sin_port = htons(data.portNumber);
    bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
	
	memset(buff, '\0', SIZE*sizeof(char));
	memset(port, '\0', SIZE*sizeof(char));

	int a = 1025 / 256;
	int b = i - (256 * a);

	string s1 = IntToString (a);
	strncpy(port1, (s1).c_str(), sizeof(port1));
	port1[sizeof(port1) - 1] = 0;

	string s2 = IntToString (b);
	strncpy(port2, (s2).c_str(), sizeof(port2));
	port2[sizeof(port2) - 1] = 0;

	strcat(port, "PORT ");
 	strcat(port, localIP);
 	strcat(port, ",");
 	strcat(port, port1);
 	strcat(port, ",");
 	strcat(port, port2);
	strcat(port, "\r\n");

	if( send(sSocket , port , strlen(port), 0) < 0)
	{
        perror("send failed");
        exit(1);
	}
	
	if( recvM(sSocket, &data) != 200)
	{
		perror("PORT wrong response code");
		exit(1);
	}
//////////////////////////////////////////////////////////////
	//LIST  
	if ( data.del == false)
	{
		if( send(sSocket, list, strlen(list), 0) < 0 )
		{
			perror("send");
			exit(1);
		}

		if( 0 > listen(sd_server, 5) )
		{
			perror("listen");
			exit(1);
		}

		len = sizeof(cl_addr);
	    if( 0> (sd_client = accept(sd_server, (struct sockaddr *)&cl_addr, &len)))
	    {
	    	perror("accept");
	    	exit(1);
	    }

		memset(buff, '\0', SIZE*sizeof(char));
		char a;
		recv(sd_client, buff, SIZE*sizeof(char), 0 );
		cerr << buff << endl;

		string str;
		int i = 0;
		int x = 0;
		int z = 0;
		int index = 0;
		bool isDir; 
		bool brejk = false;
		while ( buff[i] != NULL)
		{
			if(buff[i] == 'd' && index == 0)
				isDir = true;
			if(buff[i] == '\r' && buff[i+1] == '\n')
			{
				x = i-1;
				while( brejk == false)
				{
					i--;
					if(buff[i] == ' ')
					{
						if(isdigit(buff[i-1]))
						{
							int y = (i+2)-x;
							while(1)
							{
								str.append(1, buff[i+1]);
								i++;
								if( i >= x)
								{
									str.append(1, '\n');
									break;
								}
							}
							brejk = true;
						}
					}
				}
				brejk = false;
				i++;
				i++;
			}
			else
			{
				i++;
			}
		}
		cerr << str << endl;
	}
	else
		del(data, sSocket);

	close(sd_server);
	close(sd_client);
	sd_server = 0;
	sd_client = 0;
}

void get(int sSocket, TStruct data)
{
	char port[SIZE]; 
	char retr[SIZE]; 
	char buff[SIZE];
	char port1 [SIZE];
	char port2 [SIZE];
	int min_port = 1025;
	int max_port = 65535;
	struct sockaddr_in serv_addr, cl_addr;
	int sd_client, sd_server, nb0= 1, nb1 = 1, fd, i;
	socklen_t len;	

	memset(retr, '\0', SIZE*sizeof(char));
	strcat(retr,"RETR ");
	strcat(retr, (data.fileName).c_str());
 	strcat(retr,"\r\n");

    if( 0 > (sd_server = socket(AF_INET, SOCK_STREAM, 0)))
    {
		perror("socket");
		exit(1);
    }

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr  = INADDR_ANY;
	memset(&(serv_addr.sin_zero), '\0', 8);
	len = sizeof(cl_addr);

	if ( data.active == false)	
	{
		for( i = min_port; i <= max_port; i++)
		{
			serv_addr.sin_port = htons(i);
			if(0 <= bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) ) 
				break;
		}
	}
	else
	{
		serv_addr.sin_port = htons(data.portNumber);
		bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
		i = data.portNumber;
	}

	memset(buff, '\0', SIZE*sizeof(char));
	memset(port, '\0', SIZE*sizeof(char));

	int a = 1025 / 256;
	int b = i - (256 * a);

	string s1 = IntToString (a);
	strncpy(port1, (s1).c_str(), sizeof(port1));
	port1[sizeof(port1) - 1] = 0;

	string s2 = IntToString (b);
	strncpy(port2, (s2).c_str(), sizeof(port2));
	port2[sizeof(port2) - 1] = 0;

	strcat(port, "PORT ");
 	strcat(port, localIP);
 	strcat(port, ",");
 	strcat(port, port1);
 	strcat(port, ",");
 	strcat(port, port2);
	strcat(port, "\r\n");

	//PORT
	if( send(sSocket, port, strlen(port), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	if( recvM(sSocket, &data) != 200)
	{
		perror("PORT wrong response code");
		exit(1);
	}

	char type [SIZE];
	memset(type, '\0', SIZE*sizeof(char));
	strcat(type, "TYPE I\r\n");

	if( send(sSocket, type, strlen(type), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	recvM(sSocket, &data);

	if(0 > (fd = open((data.fileName).c_str(), O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR)))
	{
		perror("open");
		exit(1);
	}

	//RETR 
	if( send(sSocket, retr, strlen(retr), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	if( 0 > listen(sd_server, 5) )
	{
		perror("listen");
		exit(1);
	}

	len = sizeof(cl_addr);
    if( 0 > (sd_client = accept(sd_server, (struct sockaddr *)&cl_addr, &len)))
    {
		perror("accept");
		exit(1);
    }
	
	memset(buff, '\0', SIZE*sizeof(char));
	while( nb0 != 0 )
	{
        nb1 = recv(sd_client, buff, sizeof(buff), 0 );
        nb0 = write(fd, buff, nb1);	
        if( nb0 == 0 )
        	nb0 = 0;
	}

	close(sd_server);
	close(sd_client);
	sd_server = 0;
	sd_client = 0;
}

void put(int sSocket, TStruct data)
{
	char port[SIZE]; 
	char stor[SIZE]; 
	char buff[SIZE];
	char port1 [SIZE];
	char port2 [SIZE];
	int min_port = 1025;
	int max_port = 65535;
	struct sockaddr_in serv_addr, cl_addr;
	int sd_client, sd_server, nb, fd, i;
	socklen_t len;		

	memset(stor, '\0', SIZE*sizeof(char));
	strcat(stor,"STOR ");
	strcat(stor, (data.fileName).c_str());
 	strcat(stor,"\r\n");

	if( 0 > (sd_server = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("socket");
		exit(1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr  = INADDR_ANY;
	memset(&(serv_addr.sin_zero), '\0', 8);

	if ( data.active == false)	
	{
		for( i = min_port; i <= max_port; i++)
		{
			serv_addr.sin_port = htons(i);
			if(0 <= bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) ) 
				break;
		}
	}
	else
	{
		serv_addr.sin_port = htons(data.portNumber);
		bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
		i = data.portNumber;
	}
	
	int a = 1025 / 256;
	int b = i - (256 * a);

	string s1 = IntToString (a);
	strncpy(port1, (s1).c_str(), sizeof(port1));
	port1[sizeof(port1) - 1] = 0;
	string s2 = IntToString (b);
	strncpy(port2, (s2).c_str(), sizeof(port2));
	port2[sizeof(port2) - 1] = 0;

	memset(port, '\0', SIZE*sizeof(char));
	strcat(port, "PORT ");
 	strcat(port, localIP);
 	strcat(port, ",");
 	strcat(port, port1);
 	strcat(port, ",");
 	strcat(port, port2);
	strcat(port, "\r\n");
	
	//PORT
	if( send(sSocket, port, strlen(port), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	if( recvM(sSocket, &data) != 200)
	{
		perror("PORT wrong response code");
		exit(1);
	}

	char type [SIZE];
	memset(type, '\0', SIZE*sizeof(char));
	strcat(type, "TYPE I\r\n");

	if( send(sSocket, type, strlen(type), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	recvM(sSocket, &data);

	//STOR
	if( send(sSocket, stor, strlen(stor), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	if( 0 > listen(sd_server, 5) )
	{
		perror("listen");
		exit(1);
	}

	len = sizeof(cl_addr);
    if( 0 > (sd_client = accept(sd_server, (struct sockaddr *)&cl_addr, &len) ))
    {
		perror("accept");
		exit(1);
    }

	FILE *file = fopen((data.fileName).c_str(), "rb");

	memset(buff, '\0', SIZE * sizeof(char));
	while( !feof(file))
	{
		int x = (int)fread(buff, 1, sizeof(buff), file);
		if ( x < 0)
		{
			fclose(file);
			exit(1);
		}

		int pom = 0;
		do
		{
			int y = (int) send( sd_client, &buff[pom], x - pom, 0);
			pom += y;
		}while ( pom < x);
	}
	fclose(file);
	close(sd_server); 
	close(sd_client); 
	sd_server=0;
	sd_client=0;
}
void del(TStruct data, int sSocket)
{
	char delet[SIZE]; 
	char buff[SIZE];
	string file = data.fileName;

	memset(delet, '\0', SIZE*sizeof(char));
	strcat(delet,"DELE ");
	strcat(delet, (data.fileName).c_str());
 	strcat(delet,"\r\n");

	if( send(sSocket, delet, strlen(delet), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	memset(buff, '\0', SIZE*sizeof(char));
	recvM(sSocket, &data);
}

/*void pwd( int sSocket)
{
	char buff[SIZE];
	char pwd[SIZE];
	char pwd1[SIZE];
	strcat(pwd,"PWD \r\n");

	if( send(sSocket , pwd , strlen(pwd), 0) < 0)
    {
        perror("send ");
        exit(1);
    }

	memset(buff, '\0', SIZE*sizeof(char));
    recv(sSocket, buff, sizeof(buff), 0 );

	int x = 0;
	bool lomitko = false;
	for ( int i = 0; i < 1024; i++)
	{
		if( buff[i] == '/' || lomitko == true)
		{
			lomitko = true;

			if(buff[i] == '"')
				break;
			else
			{
				pwd1[x] = buff[i];
				x++;
			}
		}
	}
	pomPWD = pwd1;
}*/


void cd(int sSocket, string pathToFile, TStruct data)
{
	char cwd[SIZE];
	strcat(cwd, "CWD ");
	strcat(cwd, pathToFile.c_str());
	strcat(cwd, "\r\n");

	if( send( sSocket, cwd, strlen(cwd), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	memset(cwd, '\0', sizeof(char));
	recvM(sSocket, &data);
}

void quit(int sSocket, TStruct data)
{
	char quit[SIZE];
	string msg;

	memset(quit, '\0', SIZE*sizeof(char));
	strcat(quit, "QUIT ");
	strcat(quit, "\r\n");
	if( send( sSocket, quit, strlen(quit), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	memset(quit, '\0', sizeof(char));
	recvM(sSocket, &data);
	close(sSocket);
	sSocket = 0;
}

/*Funkcia pre citanie zo suboru*/
void readFile(string pomstr, TStruct *data)
{ 
	size_t pom1;
	size_t pom2;
    string str;
    string strTotal;
	ifstream in;
	int length;
	string password;
	string username;

	in.open(pomstr.c_str());
	getline(in,str);

	while ( in ) 
	{
    	strTotal += str;
    	getline( in, str);
	}

	length = strTotal.length();
	pom1 = strTotal.find("password:");
	pom2 = strTotal.find("username:");

	if ( pom1 != string::npos && pom2 != string::npos )
	{
		password = strTotal.substr( pom1, length);
		username = strTotal.substr( 0, pom1);

		data->password = password.substr(10, password.length());
		data->username = username.substr(10, username.length());
	}
	else
	{
		perror("read file");
		exit(1);
	}
}

void editIP( char *str)
{
	char ch1 = '.';
    char ch2 = ',';
    char * newStr = new char[strlen(str)+1];
    int n = 0;
    while(*str!='\0')
    {
		if(*str == ch1)
			newStr[n] = ch2;
		else
			newStr[n] = *str;
		str++;
		n++;
    }
	newStr[n]='\0';
	localIP = newStr;
}
/*Funkcia pre spracovanie argumentov*/
int arguments(int argc, char *argv[], TStruct *data)
{
	int i = 1;

	while(  i < argc )
	{
		if (strcmp(argv[i], "-s") == 0)
        {
        	if (data->serverIP != "")
                exit(1);
        	data->serverIP = string(argv[i+1]); i++;
        }

        else if (strcmp(argv[i], "-c") == 0)
        {
        	if (data->loginFile != "")
                exit(1);
        	data->loginFile = string(argv[i+1]); i++;
        }

        else if (strcmp(argv[i], "-p") == 0)
        {
        	if (data->pasive == true )
                exit(1);
        	data->pasive = true; i++;
        }

        else if (strcmp(argv[i], "-a") == 0)
        {
        	if (data->active == true)
                exit(1);
            if (data->portNumber != 0)
                exit(1);
        	data->active = true;
        	data->portNumber = atoi(argv[i+1]); i++;
        }
        else if (strcmp(argv[i], "-u") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->save == true)
                exit(1);
        	data->save = true;
        	data->fileName = string(argv[i+1]); i++;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->down == true)
                exit(1);
        	data->down = true;
        	data->fileName = string(argv[i+1]); i++;
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->del == true)
                exit(1);
        	data->del = true;
        	data->fileName = string(argv[i+1]); i++;
        }

        else if (strcmp(argv[i], "-P") == 0)
        {
        	if (data->pathToFile != "" )
                exit(1);
        	data->pathToFile = string(argv[i+1]); i++;
        }
        else
            i++;
    }
    return 1;
}

/*funkcia na vratenie chybovej hlasky*/
void perror(const char *message)
{
	cerr << "Error: " << message << endl;
}

/*Pomocna funkcia pre vypis help*/
void print_help()
{
    cout <<"FTP client \n";
    cout <<"Author : xmikla10 \n";
    cout <<"\n";
    cout <<"Pre pouzitie FTP clienta skuste : \n";
    cout <<"\n";
    cout <<"./fclient -s 192.168.1.102 -c credentials.txt [-p]|[-a port_number] [-d|-u|-r filename] [-P path] \n";
    cout <<"\n";
    cout <<"-s server - IP adresa nebo domenove meno serveru \n";
    cout <<"\n";
    cout <<"-c filename - subor s uzivatelskym menom a heslom \n";
    cout <<"\n";
    cout <<"-a port number - aktivni mod, datove spojeni bude inicializovane z druheho portu  \n";
    cout <<"\n";
    cout <<"-p pasivni mod  \n";
    cout <<"\n";
    cout <<"-u filename subor, ktory sa ulozi na server  \n";
    cout <<"\n";
    cout <<"-d filename subor, ktory sa stiahne zo serveru \n";
    cout <<"\n";
    cout <<"-r filename subor, ktory sa vymaze zo serveru  \n";
    cout <<"\n";            
}
