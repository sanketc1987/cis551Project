#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SA struct sockaddr
#define LISTENQ 5
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
	struct data *sp = search(s1);
	
	if(sp!=(struct data *) NULL)
	{
		 if(strcmp(sp->username,s1)==0 && strcmp(sp->password,s2)==0)
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
      if( sp == (struct data *) NULL )
      {
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

  if( filep == (FILE *) NULL )
  {
      fputs( "Fatal error. Cannot Save", stderr );
      exit( 1 );
  }

  for( sp = Head.next;sp != (struct data *) NULL;sp = sp->next )
  {
      fprintf( filep, "%s%c%s",sp->username,DELIMITER,sp->password);
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
      while( fgets( buf, BUFSIZE, db ) > 0 )
	{
	  char *ptr;
	  //split username and password based on delimiter and insert it to the structure
	  if( (ptr = strchr( buf, DELIMITER ) ) > 0 )
	    {
	      *ptr = '\0';
	      insert( strsave( buf), strsave( ++ptr ));
	    }
	}
      fclose( db );
    }
  return;
}


int connection_fd;
void service();
socklen_t len;
struct sockaddr_in cliaddr;

int match(char *s1, char *s2) {
  while( *s1 != '\0' && *s2 != 0 && *s1 == *s2 ){
    s1++; s2++;
  }
  return( *s1 - *s2 );
}

void welcome(char *str) { printf(str); }

void goodbye(char *str) { void exit(); printf(str); exit(1); }

main(){
  char name[123], pw[123]; /* passwords are short! */
  char *good = "Welcome to The Machine!\n";
  char *evil = "Invalid identity, exiting!\n";

  service();

  printf("login: "); 
  scanf("%s", name);
  printf("password: "); 
  scanf("%s", pw);

  if( match(name,pw) == 0 )
    welcome( good );
  else
    goodbye(evil );
}

void service() 
{
  int listenfd;
  struct sockaddr_in servaddr;
  FILE *client_request, *client_reply;
  char buf[1000];
  char *ip;

  char localName[20];
  int i;
  struct hostent *he;
  struct in_addr **addr_list;

  listenfd = socket(AF_INET, SOCK_STREAM, 0 );

  if( listenfd < 0 ) {
      perror("creating socket for listenfd");
      exit(2);
  }

  bzero( &servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(10551);

  if( bind( listenfd, (SA *) &servaddr, sizeof(servaddr) ) < 0 ) {
      perror( "bind on listenfd");
      exit(3);
  }

  if( listen( listenfd, LISTENQ ) < 0 ) {
      perror( "listen on listenfd" );
      exit(4);
  }

  while(1) {
      len = sizeof( cliaddr );
      connection_fd = accept( listenfd, (SA *) &cliaddr, &len );
      fprintf(stderr, "Client connected\n");

      if( connection_fd < 0 ) {
        perror( "accept on server_fd" );
        exit(1);
      }

    client_request = fdopen( connection_fd, "r" );
    if(client_request == (FILE *) NULL) {
      perror("fdopen of client_request");
      exit( 1 );
    }

    client_reply = fdopen( connection_fd, "w" );
    if( client_reply == (FILE *) NULL ) {
      perror( "fdopen of client_reply" );
      exit( 1 );
    }

   while(fgets( buf, 1000, client_request ) != NULL) {
    	fprintf( stderr, "FROM CLIENT: %s\n", buf );
  	fputs( "Server inserted new entry\n", client_reply );
          fflush( client_reply );
    }
  }
}
