#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "token.h"
#include "timlib.h"

char *token_get( char **s )
{
        static char buffer[ 4096 ];
        char *buf_ptr = buffer;

/*
fprintf( stderr, "%s()/%d: starting with **s = (%d)\n",
__FUNCTION__, __LINE__, **s );
*/

        *s = token_skip_space( *s );

/*
fprintf( stderr, "%s()/%d: after token_skip_space() with **s = (%d)\n",
__FUNCTION__, __LINE__, **s );
*/

        if ( !**s ) return (char *)0;

	/* If single quote */
	/* --------------- */
	if ( **s == '\'' )
	{
		/* Assign the first quote */
		/* ---------------------- */
		*buf_ptr++ = **s;
		(*s)++;

	        while ( **s && **s != '\'' ) 
		{
		        *buf_ptr++ = **s;
		        (*s)++;
		}

		/* Assign the second quote */
		/* ----------------------- */
		*buf_ptr++ = **s;
		(*s)++;

	        *buf_ptr = '\0';
/*
fprintf( stderr, "%s()/%d: returning token = (%s)\n",
__FUNCTION__, __LINE__, buffer );
*/
	        return buffer;
	}

        if ( ( **s == CR ) || ( **s == LF ) )
        {
                *buf_ptr = '\0';
/*
fprintf( stderr, "%s()/%d: returning token = (%s)\n",
__FUNCTION__, __LINE__, buffer );
*/
                return buffer;
        }

        if ( ispunct( **s ) )
        {
                *buf_ptr++ = **s;
                (*s)++;
                *buf_ptr = '\0';
/*
fprintf( stderr, "%s()/%d: returning token = (%s)\n",
__FUNCTION__, __LINE__, buffer );
*/
                return buffer;
        }

        while ( ( **s && isalnum( **s ) ) || **s == '_'  || **s == '.' )
        {
                *buf_ptr++ = **s;
                (*s)++;
        }

        *buf_ptr = '\0';
/*
fprintf( stderr, "%s()/%d: returning token = (%s) and **s = (%d)\n",
__FUNCTION__, __LINE__, buffer, **s );
*/
        return buffer;
}


char *token_skip_space( char *s )
{
        while ( *s
		&& ( isspace( *s )
		||   *s < 32  
        	||   *s == CR  
		||   *s == LF ) )
	{
		s++;
	}
        return s;
}

