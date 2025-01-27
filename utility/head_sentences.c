/* head_sentence.c	  */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	int c;
	int number_sentences;
	int sentence_count = 0;
	int inside_double_quote = 0;

	if ( argc != 2 )
	{
		fprintf( stderr, 
		  	 "Usage: %s number_sentences\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	number_sentences = atoi( argv[ 1 ] );

	while( ( c = fgetc( stdin ) ) != EOF )
	{
		putchar( c );

		if ( c == '.'
		||   c == '?'
		||   c == '!' )
		{
			if ( ++sentence_count == number_sentences )
			{
				if ( inside_double_quote )
				{
					while( ( c = fgetc( stdin ) ) != EOF )
					{
						putchar( c );
						if ( c == '"' ) break;
					}
				}
				break;
			}
		}
		if ( c == '"' )
		{
			inside_double_quote = 1 - inside_double_quote;
		}
	}
	return 0;

} /* main() */

