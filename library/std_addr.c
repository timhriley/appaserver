/* ---------- */
/* std_addr.c */
/* ---------- */

/* -----------------------------------------------------------------

Tim Riley
-------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include "std_addr.h"
/* #include "libjam.h" */


char *standardize( char *instring )
{
        static char buffer[ 100 ];
        char *temp, *temp_ptr = temp = strdup( instring );
        char *tok_ptr, *substitute();
        int x;

        if ( !*instring )
        {
                free( temp_ptr );
                *buffer = '\0';
                return buffer;
        }

        /* First remove any punctuation marks */
        /* ---------------------------------- */
        remove_punctuation( temp );

        *buffer = '\0';
        while ( 1 )
        {
                tok_ptr = get_token( &temp );
                if ( !*tok_ptr ) break;

                strcat( buffer, substitute( tok_ptr ) );
        }

        free( temp_ptr );

        return buffer;

} /* standardize() */


static remove_punctuation( char *s )
{
        while ( *s )
                if ( ispunct( *s ) )
                        strcpy( s, s + 1 );
                else
                        s++;

} /* remove_punctuation() */



static char *substitute( char *tok_ptr )
{
        int x;

        /* Remove 1st, 2nd, etc. */
        /* --------------------- */
        strip_number( tok_ptr );

        for( x = 0; addr_array[ x ].address_component; x++ )
                if ( strcmp(tok_ptr,
                            addr_array[ x ].address_component ) == 0 )
                        return addr_array[ x ].standard_component;

        return tok_ptr;

} /* substitute() */


static strip_number( char *tok_ptr )
{
        if ( isalpha( *tok_ptr ) ) return 0;

        while ( *tok_ptr )
                if ( isdigit( *tok_ptr ) )
                        tok_ptr++;
                else
                        break;

        *tok_ptr = '\0';
	return 0;

} /* strip_number() */


char *get_token( char **s )
{
        static char buffer[ 500 ];
        char *buf_ptr = buffer, *skip_space();

        *s = skip_space( *s );

        if ( !**s )
                return "";

        if ( ispunct( **s ) )
        {
                *buf_ptr = **s;
                buf_ptr++;
                (*s)++;
                *buf_ptr = '\0';
                return buffer;
        }

        while ( **s && isalnum( **s ) )
        {
                *buf_ptr = **s;
                buf_ptr++;
                (*s)++;
        }

        *buf_ptr = '\0';
        return buffer;

} /* get_token() */


static char *skip_space( char *s )
{
        while ( *s && isspace( *s ) ) s++;
        return s;

} /* skip_space() */

