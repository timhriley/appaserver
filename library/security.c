/* $APPASERVER_HOME/library/security.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "query.h"
#include "appaserver_error.h"
#include "environ.h"
#include "security.h"

boolean security_password_match(
			char *database_password,
			char *injection_escaped_encrypted_password )
{
	if ( !database_password || !*database_password ) return 0;

	if ( string_strcmp( database_password, "null" ) == 0 ) return 0;

	if ( !injection_escaped_encrypted_password
	||   !*injection_escaped_encrypted_password )
	{
		return 0;
	}

	return ( strcmp(
			injection_escaped_encrypted_password,
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

char *security_encrypted_password(
			char *application_name,
			char *password_sql_injection_escape,
			enum password_function password_function )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *select_clause;
	char *results;
	char *table_name;

	table_name = get_table_name( application_name, "application" );

	sprintf( where, "application = '%s'", application_name );

	if ( ! ( select_clause = 
			security_encryption_select_clause(
				password_function,
				password_sql_injection_escape ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: security_encryption_select_clause() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select_clause,
		 table_name,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: password function not supported = %d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 password_function );
		exit( 1 );
	}

	return results;
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
		version = pipe2string( "mysql_version.sh" );
	}

	return version;
}

/* ---------------------------------------------------- */
/* Next step is openssl_256				*/
/* echo $password | openssl dgst -sha256 | column.e 1	*/
/* ---------------------------------------------------- */
char *security_encryption_select_clause(
			enum password_function password_function,
			char *password )
{
	char select_clause[ 128 ];

	if ( password_function == no_encryption )
	{
		sprintf(select_clause,
			"'%s'",
			password );
	}
	else
	if ( password_function == old_password_function )
	{
		sprintf(select_clause,
			"old_password('%s')",
			password );
	}
	else
	if ( password_function == regular_password_function )
	{
		sprintf(select_clause,
			"password('%s')",
			password );
	}
	else
	if ( password_function == sha2_function )
	{
		sprintf(select_clause,
			"sha2('%s',224 )",
			password );
	}
	else
	{
		sprintf(select_clause,
			"'%s'",
			password );
	}

	return strdup( select_clause );
}

boolean security_password_encrypted(
			char *password )
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

char *security_replace_special_characters(
			char *data )
{
	if ( !data || !*data ) return data;

	search_replace_string( data, "\\", "/" );
	search_replace_string( data, "\"", "'" );

	return data;
}

char *security_sql_injection_escape( char *data )
{
	char destination[ QUERY_WHERE_BUFFER ];

	if ( !data || !*data ) return data;

	return strdup(
		string_escape_character_array(
			destination,
			data,
			"`'$;%&=" ) );
}

