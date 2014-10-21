#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SA struct sockaddr
#define LISTENQ 5
#define DELIMITER '='
#define BUFSIZE 4096
#define DATABASE "database"

struct data
{
	char* username;
	char *password;
	struct data *next;
};

struct data Head;

/*
 * returns a pointer to a copy of the 
 * character string pointed to by "str".
 */

char *strsave(char *str)
{
	char *p;
	p = (char *) malloc( strlen(str)+1 );
	if( p != (char *) NULL )
		strcpy( p, str );
	return( p );
}

//Structure to hold username and password

struct data *search( char* name )
{
  struct data *sp;
  for( sp = Head.next; sp != (struct data *) NULL; sp = sp->next)
  {
      if( strcmp( sp->username, name ) == 0 )
	return( sp );
  }
  return( (struct data *) NULL );
}

//check for authentic users
int match(char *s1, char *s2)
{
	printf("\n**match() is called\n");
	if(Head.next == (struct data *) NULL)
	{
		insert(s1, s2);
		return 0;
	}
	fprintf(stdout,"USer: %s, Pw: %s\n",s1,s2);
	struct data *sp = search(s1);

	if(sp!=(struct data *) NULL)
	{
		 printf("entered username: %s\n", s1);
		 printf("usrnm in struct: %s\n", sp->username);
		 printf("entered pw: %s\n", s2);
		 printf("pw in struct: %s\n", sp->password);
		 
		 if((strcmp(sp->username,s1)==0) && (strcmp(sp->password,s2)==0))
		 	return 0;
	}
	return -1;
}

//insert or update username, password

void insert(char *name, char *password )
{
  struct data *sp;

  sp = search( name );

  //if name is not present in the structure, then add it to the structure
  if( sp == (struct data * ) NULL )
  {
      sp = (struct data *) malloc( sizeof( struct data ) );
      if( sp == (struct data *) NULL ) {
    	  printf( "Fatal error: no memory\n" );
    	  exit( 1 );
      }
      sp->next = Head.next;
      Head.next = sp;
  }

  sp->username = strsave( name );
  sp->password = strsave( password );
}

//write the structure to a file
void write( char *file)
{
  FILE *filep;
  struct data *sp;

  filep = fopen( file, "w" );

  if( filep == (FILE *) NULL ) {
      fputs( "Fatal error. Cannot Save", stderr );
      exit( 1 );
  }

  for( sp = Head.next;sp != (struct data *) NULL;sp = sp->next ) {
      fprintf( filep, "%s%c%s\n",sp->username,DELIMITER,sp->password);
  }

  fclose( filep );
}

//read the file contents to a structure
void read( char *file )
{
	FILE *db;
	char buf[BUFSIZE];

	db = fopen( file, "r" );

	if( db != (FILE *) NULL ) 
	{
		printf("\ncontents of the file:\n");
		while( fgets( buf, BUFSIZE, db ) > 0 ) 
		{
			printf("%s\n", buf);
			char *ptr;
			//split username and password based on delimiter and insert it to the structure
			if( (ptr = strchr( buf, DELIMITER ) ) > 0 ) 
			{
				*ptr = '\0';
				ptr[strlen(ptr+1)]='\0';
				fprintf(stdout,"Length in file: %d %d",strlen(buf),strlen(ptr+1));
				insert( strsave( buf), strsave( ++ptr ));
			}
		}
		fclose( db );
	}
	/*struct data *sp;
	for( sp = Head.next;sp != (struct data *) NULL;sp = sp->next ) {
	fprintf( stdout, "%s %s",sp->username, sp->password);
	fflush(stdout);
	}*/
}


int connection_fd;
void service();
socklen_t len;
struct sockaddr_in cliaddr;

void welcome(char *str) 
{ 
	if(send(connection_fd,str,strlen(str),0) == -1)
		fprintf(stderr, "Server: Failure sending message\n"); 
	//bzero(str, strlen(str));
}

void goodbye(char *str) 
{ 
	if(send(connection_fd,str,strlen(str),0) == -1)
		fprintf(stderr, "Server: Failure sending message\n");
	//bzero(str, strlen(str));
	close(connection_fd); 
	write(DATABASE);
	exit(0);
}

main()
{
  char name[123], pw[123]; /* passwords are short! */
  char *good = "Welcome to The Machine!\n";
  char *evil = "Invalid identity, exiting!\n";

  service(name,pw,good,evil);
  //read(DATABASE);

  /*printf("login: "); 
  scanf("%s", name);
  printf("password: "); 
  scanf("%s", pw);*/

  
}

void service(char *name,char *pw,char *good, char *evil) 
{
	read(DATABASE);
	int listenfd;
	struct sockaddr_in servaddr;
	FILE *client_request, *client_reply;
	char buf[BUFSIZE];

	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *ipaddr;
	getifaddrs (&ifap);

	for (ifa = ifap; ifa; ifa = ifa->ifa_next) 
	{
		if (ifa->ifa_addr->sa_family==AF_INET) 
		{
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			if(strcmp(ifa->ifa_name,"wlan0")==0)
				ipaddr = inet_ntoa(sa->sin_addr);	
		}
	}

	printf("Listening on IP: %s\n", ipaddr);

	listenfd = socket(AF_INET, SOCK_STREAM, 0 );

	if( listenfd < 0 ) {
		perror("creating socket for listenfd");
		exit(2);
	}

	bzero( &servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(10551);
	//Change to work on any IP
	//inet_pton(AF_INET, ipaddr, &(servaddr.sin_addr));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind( listenfd, (SA *) &servaddr, sizeof(servaddr) ) < 0 ) 
	{
		perror( "bind on listenfd");
		exit(3);
	}

	if( listen( listenfd, LISTENQ ) < 0 ) 
	{
		perror( "listen on listenfd" );
		exit(4);
	}

	while(1) 
	{
		len = sizeof( cliaddr );
		connection_fd = accept( listenfd, (SA *) &cliaddr, &len );
		fprintf(stderr, "Client connected\n");

		if( connection_fd < 0 ) 
		{
			perror( "accept on server_fd" );
			exit(1);
		}
		
		char *msg = "login: ";
		if(send(connection_fd,msg,strlen(msg),0) == -1)
			fprintf(stderr, "Server: Failure sending message\n");
		//bzero(msg, strlen(msg));


		//to do strcpy read username
		int n = recv(connection_fd,(char*)buf,BUFSIZE,0);
		//fflush(stdout);
		if(n<0)
			perror("Recv");
		buf[n-1]='\0';
		strcpy(name,buf);
		fprintf(stdout,"\nServer: Name: %s Length: %d\n",name,strlen(name));
		fflush(stdout);
		
		msg="password: ";
		if(send(connection_fd,msg,strlen(msg),0) == -1)
			fprintf(stderr, "Server: Failure sending message\n");
		//bzero(msg, strlen(msg));

		//to do strcpy read password
		n = recv(connection_fd,(char*)buf,BUFSIZE,0);
		//fflush(stdout);
		if(n<0)
			perror("Recv");
		buf[n-1]='\0';
		strcpy(pw,buf);
		fprintf(stdout,"Server: Password: %s Length: %d\n",pw,strlen(pw));
		fflush(stdout);

		if( match(name,pw) == 0 )
			welcome( good );
		else
			goodbye(evil );

		printf("before 2nd while\n");
			
		msg[0] = '\0';
		msg = "Enter your choice: \n1. Insert or Update the database\n 2. Exit\n" ;
		if(send(connection_fd,msg,strlen(msg),0) == -1)
			fprintf(stderr, "Server: Failure sending message\n");
	
		
		//while(1) 
		//{	
			n = recv(connection_fd,(char*)buf,BUFSIZE,0);
			//fflush(stdout);
			if(n<0)
				perror("Recv");
			buf[n-1]='\0';
			fprintf( stdout, "Serever: option selected: %s : %d\n", buf, strlen(buf));
			fflush(stdout);
			
			if(strcmp(buf, "1") == 0)
			{
				printf("1 slected\n");
				msg[0] = '\0';
				msg = "Enter the username: ";
				if(send(connection_fd,msg,strlen(msg),0) == -1)
					fprintf(stderr, "Server: Failure sending message\n");
				
				n = recv(connection_fd,(char*)buf,BUFSIZE,0);
				fflush(stdout);
				if(n<0)
					perror("Recv");
				buf[n-1]='\0';
				printf("username client entered: %s\n", buf);
				
				char *nm; 
				strcpy(nm, buf);
				
				msg[0] = '\0';
				msg = "Enter the password: ";
				if(send(connection_fd,msg,strlen(msg),0) == -1)
					fprintf(stderr, "Server: Failure sending message\n");
				//bzero(msg, strlen(msg));

				n = recv(connection_fd,(char*)buf,BUFSIZE,0);
				fflush(stdout);
				if(n<0)
					perror("Recv");
				buf[n-1]='\0';
				printf("password client entered: %s\n", buf);

				char *pass; 
				strcpy(pass, buf);

				insert(nm, pass);
				msg[0] = '\0';
				msg = "Server inserted new entry\n";
				if(send(connection_fd,msg,strlen(msg),0) == -1)
					fprintf(stderr, "Server: Failure sending message\n");
				
				write(DATABASE);
				close(connection_fd);
				//bzero(msg, strlen(msg));
			}
			else if(strcmp(buf, "2") == 0)
			{
				printf("2 slected\n");
				write(DATABASE);
				close(connection_fd);
				printf("Closing the connection\n");
				//break;
			}
		//}
		
	}
}
