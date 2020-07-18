/* xargs.c 					*/
/* -------------------------------------------- */
/* Input: on the command line: an executable	*/
/*	  with input expected on the command	*/
/*	  line.					*/
/*	  on the stream: the arguments to the	*/
/*	  executable.				*/
/* Work:  the executable is executed for each	*/
/*	  line from the stream, with the line	*/
/*	  replacing {} on the command line.	*/
/* Output:the executable's output.	 	*/
/* Note:  this is equivalent to 		*/
/*	  xargs -n1 $executable {}		*/
/*	  It is useful if your version doesn't	*/
/*	  support the {}.			*/
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#define MAX_SYS_STR	4096
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

/* Prototypes */
/* ---------- */
char *build_sys_str( char *sys_str, 
		     int *trace_mode,
		     int argc, 
		     char **argv, 
		     char *substitution_buffer );

void xargs( int argc, char **argv, char *substitution_buffer );

int main( int argc, char **argv )
{
	char buffer[ 1024 ];

	if ( argc < 2 )
	{
		fprintf(stderr,
			"Usage: %s executable [parameters] {}\n",
			argv[ 0 ] );
		exit( 1 );
	}

	while( timlib_get_line( buffer, stdin, 1024 ) )
	{
		xargs( argc, argv, buffer );
	}

	return 0;
} /* main() */

void xargs( int argc, char **argv, char *substitution_buffer )
{
	char sys_str[ MAX_SYS_STR ];
	int trace_mode;

	if ( !build_sys_str( sys_str, 
			     &trace_mode,
			     argc, 
			     argv, 
			     substitution_buffer ) )
	{
		fprintf( stderr,
			 "ERROR: %s: buffer of size %d not big enough.\n",
			 argv[ 0 ],
			 MAX_SYS_STR );
		exit( 1 );
	}
	else
	{
		if ( trace_mode ) fprintf( stderr, "%s\n", sys_str );

		if ( system( sys_str ) ){};
	}

} /* xargs() */


char *build_sys_str(
			char *sys_str, 
			int *trace_mode,
			int argc, 
			char **argv, 
			char *replace_buffer )
{
	int i, str_len;
	char *parameter;

	i = 1;

	/* See if trace mode */
	/* ----------------- */
	if ( strcmp( argv[ 1 ], "-t" ) == 0 )
	{
		*trace_mode = 1;
		i++;
	}
	else
		*trace_mode = 0;

	/* Capture the executable */
	/* ---------------------- */
	strcpy( sys_str, argv[ i ] );

	while( ++i <= argc - 1 )
	{
		parameter = argv[ i ];

		/* Capture any arguments */
		/* --------------------- */
		str_len = strlen( sys_str );

		/* Note adding a space */
		/* ------------------- */
		if ( strlen( parameter ) + str_len + 1 > MAX_SYS_STR )
		{
			return (char *)0;
		}

		sprintf( sys_str + strlen( sys_str ), 
			 " %s", 
			 parameter );
	}

	return search_replace_string(	sys_str,
					"{}",
					replace_buffer );
} /* build_sys_str() */

