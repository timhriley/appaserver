/* get_sequence.c */
/* -------------- */

#define SEQUENCE_FILE	"/home/riley/utility/sequence.dat"

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	int sequence, get_sequence( char *sequence_file );

	sequence = get_sequence( SEQUENCE_FILE );

	if ( !sequence )
	{
		fprintf( stderr, 
			 "ERROR in %s: cannot write to %s\n",
			 argv[ 0 ],
			 SEQUENCE_FILE );
		exit( 1 );
	}
	printf( "%d\n", sequence );

	/* To suppress Wunused-parameters warning */
	/* -------------------------------------- */
	argc = 0;

	return 0;

} /* main() */


int get_sequence( char *sequence_file )
{
	FILE *f;
	int sequence;

	f = fopen( sequence_file, "r" );

	/* If no file, then initialize with 1 */
	/* ---------------------------------- */
	if ( !f )
	{
		sequence = 1;
	}
	else
	{
		fscanf( f, "%d", &sequence );
		fclose( f );
	}

	/* Save the new sequence value */
	/* --------------------------- */
	f = fopen( sequence_file, "w" );
	if ( !f ) return 0;

	fprintf( f, "%d\n", sequence + 1 );
	fclose( f );
	return sequence;

} /* get_sequence() */


