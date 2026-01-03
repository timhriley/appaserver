/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/xml_separate.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "String.h"
#include "token.h"

int main( void )
{
	char buffer[ 2048 ];
	char *buf_ptr;
	char *token_ptr;	/* Note: token_ptr is the 4K static buffer */
				/*       inside of token_get().		 */
	boolean tag_boolean = 0;
	boolean prior_punctuation = 0;
	boolean space_boolean = 0;

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		buf_ptr = buffer;

		/* Returns static memory or null */
		/* ----------------------------- */
        	while ( ( token_ptr = token_get( &buf_ptr ) ) )
        	{
			if ( *token_ptr == '<' )
			{
				printf( "\n<" );
				tag_boolean = 1;
			}
			else
			if ( *token_ptr != '>' )
			{
				if ( ispunct( *token_ptr ) )
				{
					space_boolean = 0;
					prior_punctuation = 1;
				}
				else
				{
					space_boolean = 1;
				}

				if ( !tag_boolean
				&&   space_boolean
				&&   !prior_punctuation )
				{
					printf( " " );
				}

				printf( "%s", token_ptr );

			}
			else
			{
				printf( ">\n" );
				tag_boolean = 0;
			}

			if ( !ispunct( *token_ptr )
			&&   prior_punctuation )
			{
				prior_punctuation = 0;
			}
		}
	}

	return 0;
}

