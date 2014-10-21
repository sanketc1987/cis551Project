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
	int sockfd, n;
	struct sockaddr_in servaddr;

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

	printf("\nAfter setting up interfaces\n");
	/* The main interactive loop, getting input from the user and 
	* passing to the server, and presenting replies from the server to
	* the user, as appropriate. Lots of opportunity to generalize
	* this primitive user interface...
	*/
	//display login
		
	while(strcmp(buf, "2") != 0)
	{
		strcpy(buf, "");
		//receive the message from server 
		n = recv(sockfd,(char*)buf,BUFSIZE,0);
		if(n<0)
			perror("Recv");
		buf[n]='\0';
		fprintf(stdout, buf);
		fflush(stdout);
		
		//read user input from keyboard
		n = read(fileno(stdin),buf,BUFSIZE);
		if(n<0)
			perror("Read");
		buf[n-1]='\0';
		send(sockfd,buf,n,0);

	}
		
	/* shut things down */
	//fclose( server_request );
	//fclose( server_reply );
	close( sockfd); 
	printf("\nExit client\n");
	exit( 0 );
}
