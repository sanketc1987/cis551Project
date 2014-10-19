#include "stdio.h"
#include "string.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "strings.h"

#define EOS '\0'

#define SA struct sockaddr

#define CIS551_PORT 10551

#define ERR_SOCKET 1

#define BUFSIZE 4096

main( int argc, char *argv[] )
{
	char buf[BUFSIZE];
	int sockfd;
	struct sockaddr_in servaddr;
	FILE *server_request, *server_reply;

	/* Check if invoked correctly */
	if( argc != 2 )
	{
		fprintf(stderr, "Usage: %s <IP address>\n", argv[0] );
		exit ( 1 );
	}

	/* Open up a TCP/IP socket */
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0 )) < 0 )
	{
		perror( "socket open" );
		exit( ERR_SOCKET );
	}

	/* initialize and prepare for connections to a server */
	bzero( &servaddr, sizeof(servaddr ));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( CIS551_PORT );

	/* convert address, e.g., 127.0.0.1, to the right format */
	if( inet_pton( AF_INET, argv[1], &servaddr.sin_addr ) <= 0 )
	{
		perror( "inet_pton for address" );
		exit( 99 );
	}

	/* attempt to make the actual connection */
	if( connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0 )
	{
		perror( "connect to associative memory at server" );
		exit( 100 );
	}
	
	printf("\n Connected!\n");

	/* setup the interfaces between the new socket and stdio system */
	server_request = fdopen( sockfd, "w" );
	if( server_request == (FILE *) NULL )
	{
		perror( "fdopen of stream for server requests" );
		exit( 2 );
	}
	server_reply = fdopen( sockfd, "r" );
	if( server_reply == (FILE *) NULL )
	{
		perror( "fdopen of stream for server replies" );
		exit( 3 );
	}

	/* The main interactive loop, getting input from the user and 
	* passing to the server, and presenting replies from the server to
	* the user, as appropriate. Lots of opportunity to generalize
	* this primitive user interface...
	*/
	
	while(strcmp(fgets( buf, BUFSIZE, stdin), "exit") != 0)
	{
		if( fputs( buf, server_request ) == EOF )
		{
			perror( "write failure to associative memory at server" );
		}
		fflush( server_request );  /* buffering everywhere.... */
		
		if( fgets( buf, BUFSIZE, server_reply ) == NULL )
		{
			perror( "read failure from associative memory at server");
		}
		fputs( buf, stdout );		
	}
	
	/* shut things down */
	fclose( server_request );
	fclose( server_reply );
	close( sockfd); 

	exit( 0 );
}