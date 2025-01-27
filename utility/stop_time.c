/* $APPASERVER_HOME/utility/stop_time.c			*/
/* ---------------------------------------------------- */
/* Use this program to stop timing a process.		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "timlib.h"

void remove_tmp_file( char *tmp_file_name );
void display_stop_time_to_stderr( char *key, long start_time, time_t now );

int main( int argc, char **argv )
{
	FILE *tmp_file;
	char tmp_file_name[ 128 ];
	char input_buffer[ 128 ];
	time_t now, start_time;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s key\n", argv[ 0 ] );
		exit( 1 );
	}

	now = time( (long *)0 );

	sprintf( tmp_file_name, "/tmp/time_%s.tmp", argv[ 1 ] );
	tmp_file = fopen( tmp_file_name, "r" );
	if ( !tmp_file )
	{
		fprintf( stderr, 
		"%s: Cannot open \"%s\" for read. Did you \"start_time.e\"?\n",
			 argv[ 0 ],
			 tmp_file_name );
		exit( 1 );
	}

	if ( !get_line( input_buffer, tmp_file ) )
	{
		fprintf( stderr, 
	"%s: %s is empty. Did \"start_time.e\" complete successfully?\n",
			 argv[ 0 ],
			 tmp_file_name );
		exit( 1 );
	}

	fclose( tmp_file );
	start_time = atol( input_buffer );
	display_stop_time_to_stderr( argv[ 1 ], start_time, now );
	remove_tmp_file( tmp_file_name );

	return 0;
}

void remove_tmp_file( char *tmp_file_name )
{
	char sys_str[ 512 ];

	sprintf( sys_str, "/bin/rm -f %s", tmp_file_name );
	if ( system( sys_str ) ){}
}

void display_stop_time_to_stderr( char *key, long start_time, time_t now )
{
	double minutes_taken;
	long seconds_taken;
	struct tm *struct_tm;

	struct_tm = localtime( &now );

	fprintf( stderr, 
		 "Stop time  (%s):\t%d:%.2d",
		 key,
		 struct_tm->tm_hour,
		 struct_tm->tm_min );

	seconds_taken = now - start_time;
	minutes_taken = (double)( (double)seconds_taken / 60.0 );

	fprintf( stderr, "   Minutes: %.2lf\n", minutes_taken );
}

