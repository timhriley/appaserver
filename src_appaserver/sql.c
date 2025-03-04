/* $APPASERVER_HOME/src_appaserver/sql.c				*/
/* -------------------------------------------------------------------- */
/* This compiles to sql, sql.e, and sql_quick.e.			*/
/* To override the database_name found in				*/
/* /etc/appaserver.config, pass as parameters:				*/
/* 1) the delimiter							*/
/* 2) the database_name							*/
/* --OR--								*/
/* 1) the database_name							*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "timlib.h"
#include "appaserver_parameter.h"
#include "sql.h"
#include "environ.h"
#include "basename.h"

int main( int argc, char **argv )
{
	APPASERVER_PARAMETER *appaserver_parameter;
	char system_string[ 8192 ];
	char delimiter = SQL_DELIMITER;
	char *base_name;
	char *quick_flag;
	char *override_database = {0};
	char *database_connection = {0};
	char null_string_filter[ 512 ];

	/* --------------------------------------------	*/
	/* Usage: sql.e delimiter [database]		*/
	/* --OR--					*/
	/* Usage: sql.e [database]			*/
	/* -------------------------------------------- */

	if ( argc == 3 )
	{
		if ( strlen( argv[ 1 ] ) == 1 )
			delimiter = *argv[ 1 ];

		/* Account for legacy usage */
		/* ------------------------ */
		if ( *argv[ 2 ]
		&&   strcmp( argv[ 2 ], "ignored" ) != 0
		&&   strcmp( argv[ 2 ], "mysql" ) != 0 )
		{
			override_database = argv[ 2 ];
		}
	}
	else
	if ( argc == 2 )
	{
		if ( strlen( argv[ 1 ] ) == 1 )
			delimiter = *argv[ 1 ];
		else
			override_database = argv[ 1 ];
	}

	base_name = basename_base_name( argv[ 0 ], 1 );

	if ( strcmp( base_name, "sql_quick" ) == 0 )
		quick_flag = "--quick";
	else
		quick_flag = "";

	if ( override_database && *override_database )
	{
		environment_set(
			"DATABASE",
			override_database );

		environment_set(
			"APPASERVER_DATABASE",
			override_database );
	}

	appaserver_parameter = appaserver_parameter_new();

	if ( !appaserver_parameter )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: appaserver_parameter_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( appaserver_parameter->MYSQL_HOST ) 
	{
		environ_set_environment(
			"MYSQL_HOST",
			appaserver_parameter->MYSQL_HOST );
	}

	if ( appaserver_parameter->MYSQL_TCP_PORT ) 
	{
		environ_set_environment(
			"MYSQL_TCP_PORT",
			appaserver_parameter->MYSQL_TCP_PORT );
	}

	database_connection =
		environment_get(
		"APPASERVER_DATABASE" );

	if ( !database_connection || !*database_connection )
	{
		database_connection =
			environment_get( "DATABASE" );
	}

	if ( !database_connection || !*database_connection )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no database connection.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	snprintf(
		null_string_filter,
		sizeof ( null_string_filter ),
		 "mysql_remove_null.e '%c'",
		 delimiter );

	if ( appaserver_parameter->mysql_password_syntax )
	{
		snprintf(
		system_string,
		sizeof ( system_string ),
"mysql --defaults-extra-file=%s %s -u%s %s %s		|"
"escape_character.e '%c'				|"
"tr '\011' '%c'						|"
"%s							 ",
	 	appaserver_parameter->filename,
	 	appaserver_parameter->flags,
	 	appaserver_parameter->mysql_user,
	 	quick_flag,
	 	database_connection,
	 	delimiter,
	 	delimiter,
		null_string_filter );
	}
	else
	{
		snprintf(
		system_string,
		sizeof ( system_string ),
"(							 "
"	echo \"connect %s;\"				;"
"	cat -						 "
") 							|"
"mysql %s -p%s -u%s %s					|"
"escape_character.e '%c'				|"
"tr '\011' '%c'						|"
"%s							 ",
	 	database_connection,
	 	appaserver_parameter->flags,
	 	appaserver_parameter->password,
	 	appaserver_parameter->mysql_user,
	 	quick_flag,
	 	delimiter,
	 	delimiter,
		null_string_filter );
	}

	if ( system( system_string ) ) {};

	return 0;
}

