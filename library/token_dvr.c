/* token_dvr.c */
/* ----------- */

#include <stdio.h>
#include <stdlib.h>
#include "token.h"

int main()
{
	/* test1(); */
	/* test2(); */
	return 0;
}

#ifdef NOT_DEFINED
test1()
{
        char *buffer = "while ( gets( input_buffer ) ) nothing();";
	char *temp = buffer;
	char *tok_ptr;

        while ( 1 )
        {
                tok_ptr = get_token( &temp );
                if ( !*tok_ptr )
			break;
		else
			printf( "token: %s\n", tok_ptr );
	}
}

test2()
{
        char *buffer = "select 'hello there', hello.there from dual;";
	char *temp = buffer;
	char *tok_ptr;

        while ( 1 )
        {
                tok_ptr = get_token( &temp );
                if ( !*tok_ptr )
			break;
		else
			printf( "token: %s\n", tok_ptr );
	}
}
#endif
