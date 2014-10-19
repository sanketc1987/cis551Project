#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SA struct sockaddr
#define LISTENQ 5

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

void service() {
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
