/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/security.c			   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "appaserver_error.h"
#include "environ.h"
#include "folder_attribute.h"
#include "role_folder.h"
#include "spool.h"
#include "security.h"

boolean security_password_match(
		char *database_password,
		char *encrypt_password )
{
	if ( !database_password || !*database_password ) return 0;
	if ( !encrypt_password || !*encrypt_password ) return 0;

	return
	( strcmp(
		encrypt_password,
		database_password ) == 0 );
}

enum password_function
	security_database_password_function(
		char *database_password )
{
	int str_len;

	str_len = strlen( database_password );

	if ( str_len == 16 )
	{
		return old_password_function;
	}
	else
	if ( str_len == 41 && *database_password == '*' )
	{
		return regular_password_function;
	}
	if ( str_len == 56 )
	{
		return sha2_function;
	}
	else
	{
		return no_encryption;
	}
}

char *security_encrypt_password(
		char *post_login_password,
		enum password_function password_function )
{
	char *select_clause;
	char system_string[ 1024 ];
	char *fetch;

	if ( !post_login_password ) return (char *)0;

	if ( ! ( select_clause = 
			security_encrypt_select_clause(
				password_function,
				post_login_password ) ) )
	{
		char message[ 128 ];

		sprintf(message, "security_encrypt_select_clause() empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\"",
		select_clause );

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( fetch =
			spool_pipe(
				system_string,
				0 /* not capture_stderr_boolean */ ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"password function %d not supported.",
			password_function );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return fetch;
}

enum password_function
	security_mysql_version_password_function(
		char *mysql_version )
{
	double version;

	if ( !mysql_version || !isdigit( *mysql_version ) )
		return no_encryption;

	version = atof( mysql_version );

	if ( version < 5.0 )
		return old_password_function;
	else
	if ( version <= 5.6 )
		return regular_password_function;
	else
		return sha2_function;
}

char *security_mysql_version( void )
{
	static char *version = {0};

	if ( !version )
	{
		version = string_pipe_fetch( "mysql_version.sh" );
	}

	return version;
}

/* ---------------------------------------------------- */
/* Next step is openssl_256				*/
/* echo $password | openssl dgst -sha256 | column.e 1	*/
/* ---------------------------------------------------- */
char *security_encrypt_select_clause(
		enum password_function password_function,
		char *post_login_password )
{
	char *escape;
	char select_clause[ 128 ];

	if ( !post_login_password )
	{
		char message[ 128 ];

		sprintf(message, "post_login_password is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	escape =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_escape_single_quotes(
			post_login_password );

	if ( password_function == no_encryption )
	{
		snprintf(
			select_clause,
			sizeof ( select_clause ),
			"'%s'",
			escape );
	}
	else
	if ( password_function == old_password_function )
	{
		snprintf(
			select_clause,
			sizeof ( select_clause ),
			"old_password('%s')",
			escape );
	}
	else
	if ( password_function == regular_password_function )
	{
/* Retired by MySQL
		sprintf(select_clause,
			"password('%s')",
			escape );
*/
		snprintf(
			select_clause,
			sizeof ( select_clause ),
			"CONCAT('*', UPPER(SHA1(UNHEX(SHA1('%s')))))",
			escape );
	}
	else
	if ( password_function == sha2_function )
	{
		snprintf(
			select_clause,
			sizeof ( select_clause ),
			"sha2('%s',224 )",
			escape );
	}
	else
	{
		char message[ 128 ];

		sprintf(message,
			"Invalid password_function=%d.",
			password_function );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return strdup( select_clause );
}

boolean security_password_encrypted( char *password )
{
	enum password_function password_function;

	password_function =
		security_database_password_function(
			password );

	if ( password_function == no_encryption )
		return 0;
	else
		return 1;
}

char *security_replace_special_characters( char *data )
{
	if ( !data || !*data ) return data;

	string_search_replace( data, "\\", "/" );
	string_search_replace( data, "\"", "'" );

	return data;
}

char *security_sql_injection_escape(
		const char *security_escape_character_string,
		char *datum )
{
	char destination[ STRING_64K ];
	char *return_value;

	if ( !datum || !*datum ) return strdup( "" );

	if ( strlen( datum ) >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return_value =
	strdup(
		/* -------------------------- */
		/* Safely returns destination */
		/* -------------------------- */
		string_escape_character_array(
			destination,
			datum,
			(char *)security_escape_character_string ) );

	return return_value;
}

char *security_sql_injection_unescape(
		const char *security_escape_character_string,
		char *datum )
{
	char destination[ STRING_64K ];

	if ( !datum || !*datum ) return strdup( "" );

	if ( strlen( datum ) >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup(
		string_unescape_character_array(
			destination,
			datum,
			(char *)security_escape_character_string ) );
}

char *security_sql_injection_escape_quote_delimit( char *datum )
{
	char destination[ STRING_64K ];
	char *ptr = destination;

	if ( !datum || !*datum ) return strdup( "''" );

	if ( strlen( datum ) >= STRING_64K )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: datum length = %d > max length = %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			(int)strlen( datum ),
			STRING_64K );
		exit( 1 );
	}

	ptr += sprintf( ptr, "'" );

	/* Safely returns destination */
	/* -------------------------- */
	string_escape_character_array(
		ptr /* destination */,
		datum,
		SECURITY_ESCAPE_CHARACTER_STRING );

	ptr += sprintf( ptr, "'" );

	return strdup( destination );
}

LIST *security_sql_injection_escape_list( LIST *data_list )
{
	LIST *escape_list = list_new();

	if ( list_rewind( data_list ) )
	do {
		list_set(
			escape_list,
			/* ------------------- */
			/* Returns heap memory */
			/* -------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				(char *)list_get( data_list ) ) );

	} while ( list_next( data_list ) );

	return escape_list;
}

unsigned long security_random( unsigned long up_to )
{
	return (unsigned long)( (drand48() * up_to) + 1 );
}

char *security_generate_password( void )
{
	char *password;

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	password = string_pipe( "generate_password.sh 2>/dev/null" );

	if ( !password )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: generate_password.sh returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return password;
}

char *security_remove_semicolon( char *datum )
{
	int inside_quote = 0;
	char destination[ STRING_64K ];
	char *ptr = destination;

	if ( strlen( datum ) >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !string_character_boolean( datum, ';' ) ) return datum;

	while ( *datum )
	{
		if ( *datum == '\'' )
		{
			inside_quote = 1 - inside_quote;
		}

		if ( *datum == ';' )
		{
			if ( inside_quote )
			{
				*ptr++ = ';';
			}
		}
		else
		{
			*ptr++ = *datum;
		}
		datum++;
	}

	return strdup( destination );
}

void security_system(
		const char *security_error_character_string,
		char *system_string )
{
	char destination[ STRING_64K ];

	if ( !system_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Safely returns destination */
	/* -------------------------- */
	(void)string_escape(
		destination,
		system_string /* source */,
		(char *)security_error_character_string );

	if ( strcmp( destination, system_string ) != 0 )
	{
		char message[ STRING_65K ];

		snprintf(
			message,
			sizeof ( message ),
			"Cannot execute=[%s].",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( system( system_string ) ){}
}

