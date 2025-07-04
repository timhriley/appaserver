/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/select.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "environ.h"
#include "folder.h"
#include "session.h"
#include "query.h"
#include "name_arg.h"
#include "folder.h"
#include "folder_attribute.h"
#include "appaserver.h"

void setup_arg(	NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(
		char **application_name,
		char **folder_name,
		char **select,
		char **where_clause,
		char **order_clause,
		char **group_clause,
		char **quick_yes_no,
		char **maxrows,
		NAME_ARG *arg );

char *select_multiple_table_names(
		char *application_name,
		char *multi_folder_name_list_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	char *table_name;
	char *select = {0};
	char *order_clause = {0};
	char *group_clause = {0};
	char *quick_yes_no = {0};
	char *maxrows = {0};
	long maxrows_long = 0L;
	char *where_clause;
	char system_string[ STRING_SYSTEM_BUFFER ];
	char order_by_clause[ 4096 ];
	char group_by_clause[ 4096 ];
	char where_clause_escaped[ STRING_WHERE_BUFFER ];
	FOLDER *folder;
	char sql_executable[ 128 ];
	long row_access_count = 0L;
	char input_buffer[ STRING_64K ];
	FILE *input_pipe;
        NAME_ARG *arg = init_arg( argv[ 0 ] );

	/* appaserver_error_stderr( argc, argv ); */

	setup_arg( arg, argc, argv );

	fetch_parameters(
		&application_name,
		&folder_name,
		&select,
		&where_clause,
		&order_clause,
		&group_clause,
		&quick_yes_no,
		&maxrows,
		arg );

	if ( !folder_name || !*folder_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( maxrows && *maxrows ) maxrows_long = atol( maxrows );

	session_environment_set( application_name );

	if( string_strcmp( select, "primary" ) == 0 )
	{
		folder =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_fetch(
				folder_name,
				(LIST *)0 /* exclude_lookup_name_list */,
				1 /* fetch_folder_attribute_list */,
				1 /* fetch_attribute */,
				0 /* not cache_boolean */ );

		if ( !list_length( folder->folder_attribute_primary_key_list ) )
		{
			char msg[ 1024 ];

			sprintf( msg, 
"ERROR %s/%s(): blank folder_attribute_primary_key_list for folder = (%s)",
				 __FILE__,
				 __FUNCTION__,
				 folder_name );

			appaserver_error_message_file(
				application_name,
				(char *)0 /* login_name */,
				msg );

			exit( 1 );
		}

		select = 
			list_display_delimited(
				folder->folder_attribute_primary_key_list,
				',' );
	}
	else
	if ( string_strcmp( select, "*" ) == 0
	||   string_strcmp( select, "all" ) == 0 )
	{
		folder =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_fetch(
				folder_name,
				(LIST *)0 /* exclude_lookup_name_list */,
				1 /* fetch_folder_attribute_list */,
				1 /* fetch_attribute */,
				0 /* not cache_boolean */ );

		if ( !list_length( folder->folder_attribute_name_list ) )
		{
			char msg[ 1024 ];

			sprintf( msg, 
	"ERROR %s/%s(): blank folder_attribute_name_list for folder = (%s)",
				 __FILE__,
				 __FUNCTION__,
				 folder->folder_name );

			appaserver_error_message_file(
				application_name,
				(char *)0 /* login_name */,
				msg );

			exit( 1 );
		}

		select = 
			list_display_delimited(
				folder->folder_attribute_name_list,
				',' );
	}
	else
	if( string_strcmp( select, "count" ) == 0 )
	{
		select = "count(*)";
		order_clause = "none";
	}

	/* Set final where clause */
	/* ---------------------- */
	if ( string_strncmp( where_clause, "where " ) == 0 )
	{
		strcpy( where_clause_escaped, where_clause + 6 );
	}
	else
	{
		strcpy( where_clause_escaped, where_clause );
	}

	escape_dollar_sign( where_clause_escaped );

	if ( !*where_clause_escaped )
	{
		strcpy( where_clause_escaped, "1 = 1" );
	}

	/* Set group by clause */
	/* ------------------- */
	if ( group_clause && *group_clause )
	{
		sprintf(group_by_clause, 
		 	"group by %s",
		 	group_clause );
	}
	else
	{
		*group_by_clause = '\0';
	}

	/* Set order by clause */
	/* ------------------- */
	if ( string_strcmp( order_clause, "select" ) == 0 )
	{
		sprintf(order_by_clause, 
		 	"order by %s",
		 	select );
	}
	else
	if ( order_clause && string_strcmp( order_clause, "none" ) != 0 )
	{
		sprintf(order_by_clause, 
		 	"order by %s",
		 	order_clause );
	}
	else
	{
		*order_by_clause = '\0';
	}

	if ( string_strcmp( quick_yes_no, "yes" ) == 0 )
	{
		sprintf(sql_executable,
			"sql_quick.e '%c'",
			SQL_DELIMITER );
	}
	else
	{
		sprintf(sql_executable,
			"sql.e '%c'",
			SQL_DELIMITER );
	}

	table_name =
		select_multiple_table_names(
			application_name,
			folder_name
				/* multi_folder_name_list_string */ );

	if ( !*table_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty table_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !select )
	{
		char msg[ 1024 ];

		sprintf( msg, 
			 "ERROR: %s/%s(%s,%s) has no select clause\n",
			 __FILE__,
			 __FUNCTION__,
			 application_name,
			 folder_name );

		appaserver_error_message_file(
			application_name,
			(char *)0 /* login_name */,
			msg );

		exit( 1 );
	}

	sprintf(system_string,
	 	"echo \"select %s				 "
		" 	from %s					 "
		" 	where %s				 "
		" 	%s					 "
		"	%s;\"					|"
	 	"%s 						|"
	 	"cat 						 ",
	 	select, 
		table_name,
		where_clause_escaped,
		group_by_clause,
		order_by_clause,
		sql_executable );

	input_pipe = popen( system_string, "r" );

	while( string_input( input_buffer, input_pipe, STRING_INPUT_BUFFER ) )
	{
		row_access_count++;

		if ( maxrows_long
		&&   row_access_count > maxrows_long )
		{
			char message[ STRING_64K ];

			printf(
"Warning: Output truncated. Max rows of %ld exceeded\n", maxrows_long );

			sprintf(message,
"Warning in %s/%s()/%d: Output truncated. Max rows of %ld exceeded in (from %s where %s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				maxrows_long,
				table_name,
				where_clause_escaped );

			msg( application_name, message );

			pclose( input_pipe );
			exit( 0 );
		}

		printf( "%s\n", input_buffer );
	}

	pclose( input_pipe );

	return 0;
}

void fetch_parameters(	char **application_name,
			char **folder_name,
			char **select,
			char **where_clause,
			char **order_clause,
			char **group_clause,
			char **quick_yes_no,
			char **maxrows,
			NAME_ARG *arg )
{
	*application_name = fetch_arg( arg, "application" );
	*folder_name = fetch_arg( arg, "folder" );
	*select = fetch_arg( arg, "select" );
	*where_clause = fetch_arg( arg, "where_clause" );
	*order_clause = fetch_arg( arg, "order_clause" );
	*group_clause = fetch_arg( arg, "group_clause" );
	*quick_yes_no = fetch_arg( arg, "quick" );
	*maxrows = fetch_arg( arg, "maxrows" );
}

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

	set_comment( arg, "Valid options for select are:" );

	set_comment( arg,
	"\"primary\",\"all\",\"count\",\"*\",column_comma_list" );

	set_comment( arg, "" );

	set_comment( arg, "Valid options for order_clause are" );
	set_comment( arg,
		"\"select\",\"none\",column_comma_list [descending]" );

        ticket = add_valid_option( arg, "application" );

        ticket = add_valid_option( arg, "folder" );

        ticket = add_valid_option( arg, "select" );
        set_default_value( arg, ticket, "*" );

        ticket = add_valid_option( arg, "where_clause" );
        set_default_value( arg, ticket, "1=1" );

        ticket = add_valid_option( arg, "quick" );
	add_valid_value( arg, ticket, "yes" );
	add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

        ticket = add_valid_option( arg, "order_clause" );
        set_default_value( arg, ticket, "none" );

        ticket = add_valid_option( arg, "group_clause" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "maxrows" );
        set_default_value( arg, ticket, "" );

        ins_all( arg, argc, argv );
}

char *get_multible_table_names(	char *application_name,
				char *multi_folder_name_list_string )
{
	char multi_table_name[ 1024 ];
	char *ptr = multi_table_name;
	char folder_name[ 128 ];
	char *table_name;
	int i;

	for(	i = 0;
		piece(	folder_name,
			',',
			multi_folder_name_list_string,
			i );
		i++ )
	{
		if ( i ) ptr += sprintf( ptr, "," );

		/* Returns static memory */
		/* --------------------- */
		table_name = folder_table_name( application_name, folder_name );

		ptr += sprintf( ptr, "%s", table_name );
	}

	return strdup( multi_table_name );
}

char *select_multiple_table_names(
		char *application_name,
		char *multi_folder_name_list_string )
{
	char multi_table_name[ 1024 ];
	char *ptr = multi_table_name;
	char folder_name[ 128 ];
	char *table_name;
	int i;

	for(	i = 0;
		piece(	folder_name,
			',',
			multi_folder_name_list_string,
			i );
		i++ )
	{
		if ( i ) ptr += sprintf( ptr, "," );

		table_name =
			/* ---------------------- */
			/* Returns static memory */
			/* ---------------------- */
			folder_table_name(
				application_name,
				folder_name );

		ptr += sprintf( ptr, "%s", table_name );
	}

	return strdup( multi_table_name );
}
