/* $APPASERVER_HOME/library/security.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "sql.h"
#include "piece.h"
#include "appaserver_error.h"
#include "environ.h"
#include "folder_attribute.h"
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
			char *password_sql_injection_escape,
			enum password_function password_function )
{
	char sys_string[ 1024 ];
	char *select_clause;
	char *results;

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
		 "select.sh \"%s\"",
		 select_clause );

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
	char destination[ STRING_WHERE_BUFFER ];

	if ( !data || !*data ) return strdup( "" );

	return strdup(
		string_escape_character_array(
			destination,
			data,
			"`'$;%&=" ) );
}

SECURITY_ENTITY *security_entity_calloc( void )
{
	SECURITY_ENTITY *security_entity;

	if ( ! ( security_entity =
			calloc( 1, sizeof( SECURITY_ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty..\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return security_entity;
}

SECURITY_ENTITY *security_entity(
			char *login_name,
			boolean non_owner_forbid,
			boolean override_row_restrictions )
{
	SECURITY_ENTITY *security_entity = security_entity_calloc();

	security_entity->login_name = login_name;
	security_entity->non_owner_forbid = non_owner_forbid;
	security_entity->override_row_restrictions = override_row_restrictions;

	if ( non_owner_forbid && !override_row_restrictions )
	{
		security_entity->login_name_only = login_name;

		security_entity->full_name_only =
			security_entity_fetch(
				&security_entity->street_address_only,
				security_entity->login_name );
	}

	return security_entity;
}

char *security_entity_fetch(
			char **street_address_only,
			char *login_name )
{
	char system_string[ 1024 ];
	char full_name_only[ 128 ];
	char local_street_address[ 128 ];
	char escaped_login_name[ 128 ];
	char where[ 256 ];
	char *results;
	char *select = "full_name,street_address";

	sprintf(where,
		"login_name = '%s'",
		string_escape_full(
			escaped_login_name,
			login_name ) );

	sprintf(system_string,
		"select.sh %s entity \"%s\"",
		select,
		where );

	results = string_pipe_fetch( system_string );

	if ( !results || !*results ) return (char *)0;

	piece( full_name_only, SQL_DELIMITER, results, 0 );
	piece( local_street_address, SQL_DELIMITER, results, 1 );

	*street_address_only = strdup( local_street_address );

	return strdup( full_name_only );
}

char *security_login_name_full_name_only(
			char **street_address_only,
			char *login_name )
{
	if ( !folder_attribute_exists(
			"entity",
			"login_name" ) )
	{
		return (char *)0;
	}

	return
	security_entity_fetch(
		street_address_only,
		login_name );
}

char *security_entity_where_clause(
			char *where_clause,
			SECURITY_ENTITY *security_entity )
{
	char where_clause[ STRING_WHERE_BUFFER ];

	if ( !security_entity ) return where_clause;

	if ( security_entity->full_name_only )
	{
		sprintf(where_clause,
			" and full_name = '%s' and street_address = '%s'",
			security_entity->full_name_only,
			security_entity->street_address_only );
	}
	else
	{
		sprintf(where_clause,
			" and login_name = '%s'",
			security_entity->login_name_only );
	}

	return strdup( where_clause );
}

