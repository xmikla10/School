/*
*   Projekt do Predmetu ISA
*   FTP CLIENT
*
*   Autor : xmikla10
*   Datum : 11.05.2016
*
*/


#include "fclient.h"
#define SIZE 1024
#define FTP_PORT 21

char *localIP;
string xyz; //pomocny string pri vypise LISTu


/***********MAIN************/
int main ( int argc, char *argv[])
{
    if ( argc < 5 || argc > 11 )
    {
        print_help();
        return true;
    }

    TStruct data = {"", "", false, 0, false, "", false, false, false, false, false, false, "", "", "", "", "", "", "", ""};
    arguments(argc, argv, &data);

    string loginFile = data.loginFile;
    readFile(loginFile, &data);

    int sSocket = 0;
    socketFunction( data, &sSocket);
    if ( data.pasive == true )
    	passiveMode( sSocket, data);
    else
	    activeMode(sSocket, data);

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
		perror("Wrong response code");
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
	pwd(sSocket, &data);

	if ( data.pathToFile != "")
		cd(sSocket, data.pathToFile, data);

	*pomSocket = sSocket;
	return 0;
}


/*Funkcia reprezentujuca Pasivny mod*/
void passiveMode(int sSocket, TStruct data)
{
	char pasv[SIZE];
	cin.clear();
	//char buff[SIZE];
	char pom[SIZE];
	memset(pasv, '\0', SIZE * sizeof(char) );
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

	strncpy(pom, (data.pasvMessage).c_str(), sizeof(pom));
	pom[sizeof(pom) - 1] = 0;

	int a1, a2, a3, a4, p1, p2, dataPort;
	sscanf(pom, "Entering Passive Mode (%d,%d,%d,%d,%d,%d).", &a1,&a2,&a3,&a4,&p1,&p2);
    dataPort = (p1 * 256) + p2;

    struct hostent *host;
    host = gethostbyname(data.serverIP.c_str());

	struct sockaddr_in datasSocket;
    datasSocket.sin_family = AF_INET;
    datasSocket.sin_port = htons(dataPort);
    memcpy(&datasSocket.sin_addr, host->h_addr, host->h_length);

    int dataSock;
    if ((dataSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Client: socket error");
        exit(1);
    }
    if (connect(dataSock, (struct sockaddr *)&datasSocket, sizeof(datasSocket)) < 0)
    {
        perror("Client: connect error");
        exit(1);
    }

	if( data.down == true )
    	pasv_get( sSocket, dataSock, data);

	else if( data.save == true )
	    pasv_put( sSocket, dataSock, data);

	else if( data.del == true )
	    del(data, sSocket);

	else if( data.mkd == true )
	    mkd(data, sSocket);

	else if( data.rmd == true )
	    rmd(data, sSocket);

  	else if( data.nlst == true )
      	pasv_nlst (sSocket, dataSock);

	else
		pasv_ls(sSocket, dataSock, data);

	cin.clear();
	memset(pasv,  '\0', SIZE * sizeof(char) );
  close(dataSock);
}


/*Funkcia reprezentujuca Aktivny mod*/
void activeMode(int sSocket, TStruct data)
{
	char port[SIZE];
	char port1 [SIZE];
	char port2 [SIZE];
	int min_port = 1025;
	int max_port = 65535;
	struct sockaddr_in serv_addr;
	int sd_server, i;

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
		min_port = data.portNumber;
		for( i = min_port; i <= max_port; i++)
		{
			serv_addr.sin_port = htons(i);
			if(0 <= bind(sd_server, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) )
				break;
		}
	}

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
	recvM(sSocket, &data);

	if( data.down == true )
	    get( sSocket, sd_server, data);

	else if( data.save == true )
	    put( sSocket, sd_server, data);

	else if( data.del == true )
	    del(data, sSocket);

	else if( data.mkd == true )
	    mkd(data, sSocket);

	else if( data.rmd == true )
	    rmd(data, sSocket);

 	 else if( data.nlst == true )
        nlst (sSocket, sd_server);

	else
		ls(sSocket, sd_server, data);

	close(sd_server);
	sd_server = 0;

}


/*Funkcia pre vypis adresarovej struktury*/
void ls (int sSocket, int sd_server, TStruct data)
{
	socklen_t len;
	struct sockaddr_in cl_addr;
	char buff[SIZE];
	int sd_client;
	char list[SIZE];

	memset(buff, '\0', SIZE * sizeof(char) );
	memset(list, '\0', SIZE * sizeof(char) );
	strcat(list,"LIST ");
	strcat(list, "\r\n");

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
    memset(list, '\0', SIZE*sizeof(char));
	memset(buff, '\0', SIZE*sizeof(char));
	recv(sd_client, buff, SIZE*sizeof(char), 0 );
	dirStructure( buff, sSocket, data,sd_server);
	close(sd_client);
	sd_client = 0;

}


/*Funkcia pre vypis adresarovej struktury pri pasivnom mode*/
void pasv_ls (int sSocket, int sd_server, TStruct data)
{
	char buff[SIZE];
	char list[SIZE];

	memset(buff, '\0', SIZE * sizeof(char) );
	memset(list, '\0', SIZE * sizeof(char) );
	strcat(list,"LIST ");
	strcat(list, "\r\n");
	if( send(sSocket, list, strlen(list), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	recv(sd_server, buff, SIZE*sizeof(char), 0 );
	memset(list, '\0', SIZE * sizeof(char) );
	dirStructure( buff, sSocket, data,sd_server);
	memset(buff, '\0', SIZE * sizeof(char) );
	memset(list, '\0', SIZE * sizeof(char) );
}


/*Funkcia pre download*/
void get (int sSocket, int sd_server, TStruct data)
{
	char retr[SIZE];
	char buff[SIZE];
	int sd_client;
	socklen_t len;
 	struct sockaddr_in cl_addr;
 	int nb0= 1, nb1 = 1, fd;

	memset(buff, '\0', SIZE*sizeof(char));
	memset(retr, '\0', SIZE*sizeof(char));
	strcat(retr,"RETR ");
	strcat(retr, (data.fileName).c_str());
 	strcat(retr,"\r\n");

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
	close(sd_client);
	sd_client = 0;

}


/*Funkcia pre download pri pasivnom mode*/
void pasv_get (int sSocket, int sd_server, TStruct data)
{
	char retr[SIZE];
	char buff[SIZE];
 	int nb0= 1, nb1 = 1, fd;

	memset(buff, '\0', SIZE*sizeof(char));
	memset(retr, '\0', SIZE*sizeof(char));
	strcat(retr,"RETR ");
	strcat(retr, (data.fileName).c_str());
 	strcat(retr,"\r\n");

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

	if( send(sSocket, retr, strlen(retr), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	memset(buff, '\0', SIZE*sizeof(char));
	while( nb0 != 0 )
	{
        nb1 = recv(sd_server, buff, sizeof(buff), 0 );
        nb0 = write(fd, buff, nb1);
        if( nb0 == 0 )
        	nb0 = 0;
	}
}


/*Funkcia pre upload*/
void put (int sSocket, int sd_server, TStruct data)
{
	char stor[SIZE];
	char buff[SIZE];
	int sd_client;
	socklen_t len;
 	struct sockaddr_in cl_addr;

 	memset(stor, '\0', SIZE*sizeof(char));
	strcat(stor,"STOR ");
	strcat(stor, (data.fileName).c_str());
 	strcat(stor,"\r\n");

	char type [SIZE];
	memset(type, '\0', SIZE*sizeof(char));
	strcat(type, "TYPE I\r\n");

	if( send(sSocket, type, strlen(type), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	recvM(sSocket, &data);

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
  if(!file)
  {
    perror("open");
    exit(1);
  }

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
	close(sd_client);
	sd_client=0;
}


/*Funkcia pre upload pri pasivnom mode*/
void pasv_put (int sSocket, int sd_server, TStruct data)
{
	char stor[SIZE];
	char buff[SIZE];

 	memset(stor, '\0', SIZE*sizeof(char));
	strcat(stor,"STOR ");
	strcat(stor, (data.fileName).c_str());
 	strcat(stor,"\r\n");

	char type [SIZE];
	memset(type, '\0', SIZE*sizeof(char));
	strcat(type, "TYPE I\r\n");

	if( send(sSocket, type, strlen(type), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	recvM(sSocket, &data);

	if( send(sSocket, stor, strlen(stor), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	FILE *file = fopen((data.fileName).c_str(), "rb");
  if(!file)
  {
    perror("open");
    exit(1);
  }
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
			int y = (int) send( sd_server, &buff[pom], x - pom, 0);
			pom += y;
		}while ( pom < x);
	}

	fclose(file);
}


/*Funkcia pre vymazanie suboru*/
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


/*Funkcia reprezentujuca unixovy prikaz CD*/
void cd(int sSocket, string pathToFile, TStruct data)
{
	char cwd[SIZE];
	memset(cwd, '\0', SIZE*sizeof(char));
	strcat(cwd, "CWD ");
	strcat(cwd, pathToFile.c_str());
	strcat(cwd, "\r\n");

	if( send( sSocket, cwd, strlen(cwd), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	memset(cwd, '\0', SIZE*sizeof(char));
	recvM(sSocket, &data);
}


/*Vypise list suborov v danom adresary */
void nlst (int sSocket, int sd_server)
{
	socklen_t len;/*Vypise list suborov v danom adresary v pasivnom mode*/
	struct sockaddr_in cl_addr;
	char buff[SIZE];
	int sd_client;
	char nlst[SIZE];

	memset(buff, '\0', SIZE * sizeof(char) );
	memset(nlst, '\0', SIZE * sizeof(char) );
	strcat(nlst,"NLST ");
	strcat(nlst, "\r\n");

	if( send(sSocket, nlst, strlen(nlst), 0) < 0 )
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
   recv(sd_client, buff, sizeof(buff), 0);

  memset(nlst, '\0', SIZE*sizeof(char));
  cerr << buff << endl;
  memset(buff, '\0', SIZE*sizeof(char));
  close(sd_client);
  sd_client = 0;
}


/*Vypise list suborov v danom adresary v pasivnom mode*/
void pasv_nlst (int sSocket, int sd_server)
{
  char buff[SIZE];
  char nlst[SIZE];

  memset(buff, '\0', SIZE * sizeof(char) );
  memset(nlst, '\0', SIZE * sizeof(char) );
  strcat(nlst,"NLST ");
  strcat(nlst, "\r\n");

  if( send(sSocket, nlst, strlen(nlst), 0) < 0 )
  {
    perror("send");
    exit(1);
  }
  if(recv(sd_server, buff, sizeof(buff), 0)< 0 )
  {
    perror("recv");
    exit(1);
  }
  else
    cerr << buff << endl;

  memset(nlst, '\0', SIZE*sizeof(char));
  memset(buff, '\0', SIZE*sizeof(char));
}


/*Funkcia pre zobrazenie adresovej struktury daneho priecinku*/
void dirStructure(char buff[1024], int sSocket, TStruct data, int sd_server)
{
	string str;
	int i = 0;
	int x = 0;
	int index = 0;
	bool brejk = false;
	bool priecinok = false;

	while ( buff[i] != '\0')
	{
		if(buff[i] == 'd' && index == 0)
		{
			priecinok = true;
		}
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
						if( data.concrete_dir != "")
						{
							str = str + (data.concrete_dir) + "/" ;
						}
						if ( priecinok == true)
						{
								data.isdir =  data.isdir + xyz + (data.concrete_dir1) + "/" ;
						}
						while(1)
						{
							str.append(1, buff[i+1]);

							if ( priecinok == true)
								(data.isdir).append(1, buff[i+1]);

							i++;
							if( i >= x)
							{
								str.append(1, '\n');
								break;
							}
						}
						if ( priecinok == true)
						{
							(data.isdir).append(1, '\n');
							priecinok = false;
						}
						priecinok = false;
						index = 0;
						brejk = true;
					}
				}
			}
			brejk = false;
			i = x +3 ;
		}
		else
		{
			index++;
			i++;
		}
	}
  /* Prepisanie dvoch lomitok na jedno*/
  char lomitko[SIZE];
  string vysledok;
  strncpy(lomitko, (str).c_str(), sizeof(lomitko));
  lomitko[sizeof(lomitko) - 1] = 0;
  int a = 0;
  while( lomitko[a] != '\0')
  {
      if( lomitko[a] == '/' && lomitko[a+1] == '/')
      {
        vysledok.append(1, '/');
        a++;
      }
      else
      {
    	 vysledok.append(1, lomitko[a]);
      }
      a++;
  }
  printf("%s", vysledok.c_str());
  memset(lomitko, '\0', SIZE*sizeof(char));
	if ( (data.isdir) != "")
	{
		selectDir(&data, (data.isdir));
		cd( sSocket, (data.concrete_dir), data);
		activeMode(sSocket, data);
		ls( sSocket, sd_server, data);
	}
	if ((data.isdir) == "")
	{
		 exit(1);
	}
}


/*Pomocna funkcia pre dirStructure(), ktora nastavuje novu hodnotu stringom v struktue
  podla aktualneho adresara*/
void selectDir( TStruct *data, string isdir)
{
	int i = 0;
	string str1 = "";
	string str2 = "";
	string str3 = "";
	char dir[SIZE];
	strncpy(dir, isdir.c_str(), sizeof(dir));
	dir[sizeof(dir) - 1] = 0;
	while( dir[i] != '\n')
	{
		str1.append(1, dir[i]);
		i++;
	}
	i++;
	while( dir[i] != '\0')
	{
		str2.append(1, dir[i]);
		i++;
	}
	data->concrete_dir = str1;
	data->isdir = str2;

	int x;
	x = 1;
	char dir1[SIZE];
	strncpy(dir1, str1.c_str(), sizeof(dir));
	while ( dir1[x] != '/')
	{
		str3.append(1, dir[x]);
		x++;
	}
	str3 = "";
	x++;
	while ( dir1[x] != '\0')
	{
		str3.append(1, dir[x]);
		x++;
	}

	data->concrete_dir1 = str3;

	memset(dir, '\0', SIZE*sizeof(char));
	memset(dir1, '\0', SIZE*sizeof(char));
}


/*Funkcia pre vytvorenie priecinku*/
void mkd(TStruct data, int sSocket)
{
	char mkd[SIZE];
 	char buff[SIZE];

	memset(mkd, '\0', SIZE*sizeof(char));
	strcat(mkd,"MKD ");
 	strcat(mkd, (data.fileName).c_str());
	strcat(mkd,"\r\n");

	if( send(sSocket, mkd, strlen(mkd), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	memset(buff, '\0', SIZE*sizeof(char));
	recvM(sSocket, &data);
}


/*Funkcia pre vymazanie  priecinku*/
void rmd(TStruct data, int sSocket)
{
	char rmd[SIZE];
 	char buff[SIZE];

	memset(rmd, '\0', SIZE*sizeof(char));
	strcat(rmd,"RMD ");
 	strcat(rmd, (data.fileName).c_str());
	strcat(rmd,"\r\n");

	if( send(sSocket, rmd, strlen(rmd), 0) < 0 )
	{
		perror("send");
		exit(1);
	}

	memset(buff, '\0', SIZE*sizeof(char));
	recvM(sSocket, &data);
}


/*funckia pre vypis cesty aktualneho adresara*/
void pwd(int sSocket, TStruct *data)
{
	char buff[SIZE];
	char pwd[SIZE];
	memset(buff, '\0', SIZE * sizeof(char));
	memset(pwd, '\0', SIZE * sizeof(char));

	strcat(pwd,"PWD \r\n");
	string str = "";


	if( send(sSocket, pwd, strlen(pwd), 0) < 0 )
	{
		perror("send");
		exit(1);
	}
	//memset(buff, '\0', SIZE * sizeof(char));
	if( recv(sSocket, buff, sizeof(buff), 0 ) < 0)
	{
		perror("recv");
		exit(1);
	}
	else
	{
		int i = 5;
		while( buff[i] != '"')
		{
			str.append(1, buff[i]);
			i++;
		}
	}
	if ( data->pathToFile != "")
	{
		xyz = data->pathToFile + str;
	}
	else
		xyz = str;

	memset(buff, '\0', SIZE * sizeof(char));
	memset(pwd, '\0', SIZE * sizeof(char));
}


/*Funkcia pre ukoncenie pripojenia*/
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


/*Funkcia na prijatie spravy*/
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


/*Pomocna funkcia pre upravu IP adresy*/
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
        	data->serverIP = string(argv[i+1]); i++; i++;
        }

        else if (strcmp(argv[i], "-c") == 0)
        {
        	if (data->loginFile != "")
                exit(1);
        	data->loginFile = string(argv[i+1]); i++; i++;
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
        	data->portNumber = atoi(argv[i+1]); i++; i++;
        }
        else if (strcmp(argv[i], "-u") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->save == true)
                exit(1);
        	data->save = true;
        	data->fileName = string(argv[i+1]); i++; i++;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->down == true)
                exit(1);
        	data->down = true;
        	data->fileName = string(argv[i+1]); i++; i++;
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->del == true)
                exit(1);
        	data->del = true;
        	data->fileName = string(argv[i+1]); i++; i++;
        }

        else if (strcmp(argv[i], "-P") == 0)
        {
        	if (data->pathToFile != "" )
                exit(1);
        	data->pathToFile = string(argv[i+1]); i++; i++;
        }
        else if (strcmp(argv[i], "-nl") == 0)
        {
        	if (data->nlst == true )
                exit(1);
        	data->nlst = true; i++;
        }
        else if (strcmp(argv[i], "-md") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->mkd == true)
                exit(1);
        	data->mkd = true;
        	data->fileName = string(argv[i+1]); i++; i++;
        }
        else if (strcmp(argv[i], "-rd") == 0)
        {
        	if (data->fileName != "")
                exit(1);
            if (data->rmd == true)
                exit(1);
        	data->rmd = true;
        	data->fileName = string(argv[i+1]); i++; i++;
        }
        else
            {
		perror("Wrong arguments");
		exit(1);
	}
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
