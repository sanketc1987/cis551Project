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
