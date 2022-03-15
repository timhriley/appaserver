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
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "sql.h"
#include "environ.h"
#include "basename.h"

/* Constants */
/* --------- */

int main( int argc, char **argv )
{
	APPASERVER_PARAMETER *h;
	char sys_string[ 8192 ];
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

	base_name = basename_get_base_name( argv[ 0 ], 1 );

	if ( strcmp( base_name, "sql_quick" ) == 0 )
		quick_flag = "--quick";
	else
		quick_flag = "";

	if ( override_database && *override_database )
	{
		environment_set(
			"DATABASE",
			override_database );
	}

	h = appaserver_parameter_new();

	if ( !h )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: appaserver_parameter_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( h->MYSQL_HOST ) 
		environ_set_environment(
			"MYSQL_HOST", h->MYSQL_HOST );

	if ( h->MYSQL_TCP_PORT ) 
		environ_set_environment(
			"MYSQL_TCP_PORT", h->MYSQL_TCP_PORT );

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

	sprintf( null_string_filter,
		 "mysql_remove_null.e '%c'",
		 delimiter );

	if ( h->mysql_password_syntax )
	{
		sprintf( sys_string,
"mysql --defaults-extra-file=%s %s -u%s %s %s		|"
"tr '\011' '%c'						|"
"%s							|"
"cat							 ",
	 	h->parameter_file_full_path,
	 	h->flags,
	 	h->user,
	 	quick_flag,
	 	database_connection,
	 	delimiter,
		null_string_filter );
	}
	else
	{
		sprintf( sys_string,
"(							 "
"	echo \"connect %s;\"				;"
"	cat -						 "
") 							|"
"mysql %s -p%s -u%s %s					|"
"tr '\011' '%c'						|"
"%s							|"
"cat							 ",
	 	database_connection,
	 	h->flags,
	 	h->password,
	 	h->user,
	 	quick_flag,
	 	delimiter,
		null_string_filter );
	}

	if ( system( sys_string ) ) {};

	return 0;
}

