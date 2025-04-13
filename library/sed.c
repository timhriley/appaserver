/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sed.c		 			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sed.h"
#include "String.h"

void sed_free( SED *sed )
{
	free( sed );
}

SED *new_sed( char *regular_expression, char *replace )
{
	return sed_new( regular_expression, replace );
}

SED *sed_calloc( void )
{
	SED *sed = calloc( 1, sizeof ( SED ) );
	return sed;
}

SED *sed_new( char *regular_expression, char *replace )
{
	char regular_expression_buffer[ SED_BUFFER_SIZE ];
	SED *sed = sed_calloc();

	sed->replace = replace;

	string_escape_character(
		regular_expression_buffer,
		regular_expression,
		'$' );

	regcomp( &sed->regex, regular_expression_buffer, 0 );

	return sed;
}

int sed_will_replace( char *buffer, SED *sed )
{
	sed_get_begin_end(
			&sed->begin,
			&sed->end,
			&sed->regex,
			buffer );

	if ( sed->begin == -1 )
		return 0;
	else
		return 1;
}

void sed_get_begin_end(	regoff_t *begin,
			regoff_t *end,
			regex_t *regex,
			char *buffer )
{
	regmatch_t pmatch[ 1 ];
	int return_value;

	return_value = regexec( regex, buffer, 1, pmatch, 0 );

	if ( return_value == 1
	||   pmatch[ 0 ].rm_so == -1
	||   pmatch[ 0 ].rm_so == pmatch[ 0 ].rm_eo )
	{
		*begin = -1;
		*end = -1;
	}
	else
	{
		*begin = pmatch[ 0 ].rm_so;
		*end = pmatch[ 0 ].rm_eo;
	}
}

int sed_search_replace( char *buffer, SED *sed )
{
	regoff_t begin;
	regoff_t end;
	regoff_t last_begin = -1;
	char tmp[ SED_BUFFER_SIZE ];
	int did_any = 0;

	while( 1 )
	{
		sed_get_begin_end(
			&begin,
			&end,
			&sed->regex,
			buffer );

		if ( begin == -1 ) break;
		if ( begin == end ) break;
		if ( begin == last_begin ) break;

		strncpy( tmp, buffer, begin );
		*(tmp + begin) = '\0';
		strcat( tmp, sed->replace );
		strcat( tmp, buffer + end );
		/* *(tmp + strlen( buffer ) + end - 1) = '\0'; */
		strcpy( buffer, tmp );

		last_begin = begin;
		did_any = 1;
	}
	return did_any;
}

/* Returns static memory */
/* --------------------- */
char *sed_trim_double_spaces( char *string )
{
	static char return_string[ 1024 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	if ( !string
	||   strlen( string ) > 1023 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: string=[%s] is too large.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			string );
		exit( 1 );
	}

	regular_expression = "[ ][ ]";
	replace = " ";

	sed = sed_new( regular_expression, replace );

	string_strcpy(	return_string,
			string,
			1024 );

	while ( sed_will_replace( return_string, sed ) )
	{
		sed_search_replace( return_string, sed );
	}

	sed_free( sed );

	return string_rtrim( return_string );
}

