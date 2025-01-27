/* mnemonic2words.c				*/
/* -------------------------------------------- */
/* Input: 	mnemonic			*/
/* Output:	the initial word is capitalized	*/
/*		and a space replaces each 	*/
/*		underbar.		 	*/
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

#define MAX_SIZE 1024

char *mnemonic2words( char *mnemonic );

char *mnemonic2words( char *mnemonic )
{
	static char a[ MAX_SIZE + 1 ];
	char *ptr = a;
	int first_letter_of_word = 1;

	while( *mnemonic )
	{
		if ( *mnemonic == '_' )
		{
			*ptr++ = ' ';
			mnemonic++;
			first_letter_of_word = 1;
		}
		else
		if ( first_letter_of_word )
		{
			*ptr++ = toupper( *mnemonic++ );
			first_letter_of_word = 0;
		}
		else
		{
			*ptr++ = tolower( *mnemonic++ );
		}
	}

	*ptr = '\0';
	return a;

} /* mnemonic2words() */


main( int argc, char **argv )
{
	char buffer[ MAX_SIZE + 1 ];

	if ( argc == 1 )
	{
		while( get_line( buffer, stdin ) )
			printf( "%s\n", mnemonic2words( buffer ) );
	}
	else
	{
		while( --argc )
			printf( "%s\n", mnemonic2words( *++argv ) );
	}

} /* main() */

