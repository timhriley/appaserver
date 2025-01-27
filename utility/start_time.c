/* $APPASERVER_HOME/utility/start_time.c		*/
/* ---------------------------------------------------- */
/* Use this program to start timing a process.		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

void display_start_time_to_stderr( char *key, time_t now );

int main( int argc, char **argv )
{
	FILE *tmp_file;
	char tmp_file_name[ 128 ];
	time_t now;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s key\n", argv[ 0 ] );
		exit( 1 );
	}

	now = time( (long *)0 );

	sprintf( tmp_file_name, "/tmp/time_%s.tmp", argv[ 1 ] );
	tmp_file = fopen( tmp_file_name, "w" );
	if ( !tmp_file )
	{
		fprintf( stderr, 
			 "%s: Cannot open %s for write.\n",
			 argv[ 0 ],
			 tmp_file_name );
		exit( 1 );
	}

	fprintf( tmp_file, "%ld\n", now );
	fclose( tmp_file );

	display_start_time_to_stderr( argv[ 1 ], now );

	return 0;
}


void display_start_time_to_stderr( char *key, time_t now )
{
	struct tm *struct_tm;

	struct_tm = localtime( &now );
	fprintf( stderr, 
		 "Start time (%s):\t%d:%.2d\n",
		 key,
		 struct_tm->tm_hour,
		 struct_tm->tm_min );

} /* display_start_time_to_stderr() */

