/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/token.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "token.h"

#define DEBUG_MODE 0

char *token_get( char **s )
{
        static char buffer[ 4096 ];
        char *buf_ptr = buffer;

if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: starting with **s = (%c)\n",
__FUNCTION__, __LINE__, **s );

        *s = token_skip_space( *s );

if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: after token_skip_space() with **s = (%c)\n",
__FUNCTION__, __LINE__, **s );

        if ( !**s ) return (char *)0;

	/* If single quote */
	/* --------------- */
	if ( **s == '\'' )
	{
if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: got a single quote; getting until another quote.\n",
__FUNCTION__, __LINE__ );

		/* Assign the first quote */
		/* ---------------------- */
		*buf_ptr++ = **s;
		(*s)++;

	        while ( **s && **s != '\'' ) 
		{
		        *buf_ptr++ = **s;
		        (*s)++;
		}

if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: got the matching quote or the end.\n",
__FUNCTION__, __LINE__ );

		/* Assign the second quote */
		/* ----------------------- */
		*buf_ptr++ = **s;
		(*s)++;

	        *buf_ptr = '\0';

if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: returning token of quoted string = (%s)\n",
__FUNCTION__, __LINE__, buffer );

	        return buffer;
	}

        if ( ( **s == STRING_CR ) || ( **s == STRING_LF ) )
        {
                *buf_ptr = '\0';

if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: got a CR or LF so returning token = (%s)\n",
__FUNCTION__, __LINE__, buffer );

                return buffer;
        }

        if ( ispunct( **s ) )
        {
                *buf_ptr++ = **s;
                (*s)++;
                *buf_ptr = '\0';

if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: got a punctuation so returning token = (%s)\n",
__FUNCTION__, __LINE__, buffer );

                return buffer;
        }

        while ( ( **s && isalnum( **s ) ) || **s == '_'  || **s == '.' )
        {
                *buf_ptr++ = **s;
                (*s)++;
        }

        *buf_ptr = '\0';

if ( DEBUG_MODE )
fprintf( stderr, "%s()/%d: returning token = (%s) and the next character will be (%d=%c)\n",
__FUNCTION__, __LINE__, buffer, **s, **s );

        return buffer;
}


char *token_skip_space( char *s )
{
        while ( *s
		&& ( isspace( *s )
		||   *s < 32  
        	||   *s == STRING_CR  
		||   *s == STRING_LF ) )
	{
		s++;
	}
        return s;
}

