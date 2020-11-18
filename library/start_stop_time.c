#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void start_time( char *key )
{
	char sys_str[ 1024 ];
	sprintf( sys_str, "start_time.e %s_%d", key, getpid() );
	if ( system( sys_str ) ){};
}


void stop_time( char *key )
{
	char sys_str[ 1024 ];
	sprintf( sys_str, "stop_time.e %s_%d", key, getpid() );
	if ( system( sys_str ) ){};
}

