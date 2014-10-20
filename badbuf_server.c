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
	fprintf(stdout,"USer: %s, Pw: %s\n",s1,s2);
	struct data *sp = search(s1);
	
	/*for( sp = Head.next;sp != (struct data *) NULL;sp = sp->next )
	{
      		fprintf( stdout, "%s %s",sp->username, sp->password);
      		fflush(stdout);
  	}*/
	if(sp!=(struct data *) NULL)
	{
		 if((strcmp(sp->username,s1)==0) && (strcmp(sp->password,s2)==0))
		 	return 0;
		 /*int n1 = strcmp(sp->username,s1);
		 int n2 = strcmp(sp->password,s2);
		 int len1 = strlen(sp->password);
		 int len2 = strlen(s2);
		 fprintf(stdout,"%d %d\n",n1,n2);*/
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

  if( db != (FILE *) NULL ) {
    while( fgets( buf, BUFSIZE, db ) > 0 ) {
  	  char *ptr;
  	  //split username and password based on delimiter and insert it to the structure
  	  if( (ptr = strchr( buf, DELIMITER ) ) > 0 ) {
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

void welcome(char *str) { send(connection_fd,str,sizeof(str),0); }

void goodbye(char *str) { send(connection_fd,str,sizeof(str),0); close(connection_fd); }

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
  char buf[1000];

  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *ipaddr;
  getifaddrs (&ifap);

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr->sa_family==AF_INET) {
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
    char *msg = "login:";
    send(connection_fd,msg,7,0);
    
    
    //to do strcpy read username
    int n = recv(connection_fd,(char*)buf,sizeof(buf),0);
    fflush(stdout);
    if(n<0)
    	perror("Recv");
    buf[n-1]='\0';
    strcpy(name,buf);
    fprintf(stdout,"Name: %s Length: %d\n",name,strlen(name));
    fflush(stdout);
    
    msg="password:";
    send(connection_fd,msg,10,0);
    
    //to do strcpy read password
    n = recv(connection_fd,(char*)buf,sizeof(buf),0);
    fflush(stdout);
    if(n<0)
    	perror("Recv");
    buf[n-1]='\0';
    strcpy(pw,buf);
    fprintf(stdout,"Password: %s Length: %d\n",pw,strlen(pw));
    fflush(stdout);
    
    if( match(name,pw) == 0 )
    	welcome( good );
    else
    	goodbye(evil );
    printf("before 2nd while\n");
    /*while(fgets( buf, 1000, client_request ) != NULL) 
    {
    	fputs( "Enter your choice:", client_reply );
    	fputs("1. Insert or Update the database\n 2. Exit\n",client_reply);
    	fgets(buf, 1000, client_request);
    	fprintf( stderr, "FROM CLIENT: %s\n", buf );
    	fputs( "Server inserted new entry\n", client_reply );
      	fflush( client_reply );
        
	/*scanf("%d",choice);
	switch(choice)
	{
		case 1:
			printf("Enter the username\n");
			scanf("%s",name);
			printf("Enter the password\n");
			scanf("%s",pw);
			insert(name,pw);
			break;
		case 2:
			close(connection_fd);
			return;
			break;
		default:
			printf("Invalid Choice\n");
					
	}
     	
  	
    }*/
	//TO DO clean connections 
	//close(connection_fd);
  }
}
