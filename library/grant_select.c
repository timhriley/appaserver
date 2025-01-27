/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/grant_select.c 				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "security.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "grant_select.h"

GRANT_SELECT *grant_select_new(
		char *application_name,
		char *login_name,
		char *connect_from_host,
		boolean revoke_only_boolean )
{
	GRANT_SELECT *grant_select;

	if ( !application_name
	||   !login_name
	||   !connect_from_host )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grant_select = grant_select_calloc();

	grant_select->login_host_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grant_select_login_host_name(
			login_name,
			connect_from_host );

	grant_select->flush_privileges_sql_statement =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		grant_select_flush_privileges_sql_statement();

	if ( revoke_only_boolean )
	{
		grant_select->revoke_sql_statement =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			grant_select_revoke_sql_statement(
				grant_select->login_host_name );

		grant_select->drop_user_sql_statement =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			grant_select_drop_user_sql_statement(
				grant_select->login_host_name );

		return grant_select;
	}

	grant_select->security_generate_password =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_generate_password();

	grant_select->create_user_sql_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grant_select_create_user_sql_statement(
			grant_select->login_host_name,
			grant_select->security_generate_password );

	grant_select->on_table_sql_statement_list =
		grant_select_on_table_sql_statement_list(
			application_name,
			grant_select->login_host_name );

	grant_select->update_process_sql_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grant_select_update_process_sql_statement(
			application_name,
			grant_select->login_host_name );

	grant_select->post_execute_message =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grant_select_post_execute_message(
			application_name,
			connect_from_host,
			grant_select->security_generate_password );

	return grant_select;
}

GRANT_SELECT *grant_select_calloc( void )
{
	GRANT_SELECT *grant_select;

	if ( ! ( grant_select = calloc( 1, sizeof ( GRANT_SELECT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grant_select;
}

char *grant_select_post_execute_message(
		char *application_name,
		char *connect_from_host,
		char *security_generate_password )
{
	char message[ 1024 ];
	char *ptr = message;

	if ( !application_name
	||   !connect_from_host
	||   !security_generate_password )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	ptr += sprintf(
		ptr,
		"<p>Password generated: %s\n",
		security_generate_password );

	ptr += sprintf(
		ptr,
		"<p>Place this password in %s:/etc/appaserver_%s.config\n",
		connect_from_host,
		application_name );

	ptr += sprintf(
		ptr,
"<p>You may need to edit /etc/mysql/mysql.conf.d/mysqld.cnf and comment out \"bind-address = 127.0.0.1\"\n" );

	return strdup( message );
}

char *grant_select_login_host_name(
		char *login_name,
		char *connect_from_host )
{
	static char login_host_name[ 128 ];

	if ( !login_name
	||   !connect_from_host )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(login_host_name,
		"'%s'@'%s'",
		login_name,
		connect_from_host );

	return login_host_name;
}

char *grant_select_drop_user_sql_statement(
		char *grant_select_login_host_name )
{
	static char sql_statement[ 128 ];

	if ( !grant_select_login_host_name )
	{
		char message[ 128 ];

		sprintf(message, "grant_select_login_host_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(sql_statement,
		"drop user if exists %s;",
		grant_select_login_host_name );

	return sql_statement;
}

char *grant_select_create_user_sql_statement(
		char *grant_select_login_host_name,
		char *security_generate_password )
{
	static char sql_statement[ 128 ];

	if ( !grant_select_login_host_name
	||   !security_generate_password )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(sql_statement,
		"create user %s identified by '%s';",
		grant_select_login_host_name,
		security_generate_password );

	return sql_statement;
}

LIST *grant_select_on_table_sql_statement_list(
		char *application_name,
		char *grant_select_login_host_name )
{
	LIST *list;
	LIST *non_system_folder_name_list;
	char *folder_name;

	if ( !application_name
	||   !grant_select_login_host_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	non_system_folder_name_list =
		application_non_system_folder_name_list();

	if ( !list_rewind( non_system_folder_name_list ) ) return (LIST *)0;

	do {
		folder_name = list_get( non_system_folder_name_list );

		list_set(
			list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			grant_select_on_table_sql_statement(
				application_name,
				grant_select_login_host_name,
				folder_name ) );

	} while ( list_next( non_system_folder_name_list ) );

	return list;
}

char *grant_select_on_table_sql_statement(
		char *application_name,
		char *grant_select_login_host_name,
		char *folder_name )
{
	char sql_statement[ 128 ];

	sprintf(sql_statement,
		"grant select on %s.%s to %s;",
		application_name,
		folder_name,
		grant_select_login_host_name );

	return strdup( sql_statement );
}

char *grant_select_update_process_sql_statement(
		char *application_name,
		char *grant_select_login_host_name )
{
	static char sql_statement[ 128 ];

	sprintf(sql_statement,
		"grant update (execution_count) "
		"on %s.process "
		"to %s;",
		application_name,
		grant_select_login_host_name );

	return sql_statement;
}

char *grant_select_revoke_sql_statement(
		char *grant_select_login_host_name )
{
	static char sql_statement[ 256 ];

	sprintf(sql_statement,
		"revoke all on *.* from %s;",
		grant_select_login_host_name );

	return sql_statement;
}

char *grant_select_flush_privileges_sql_statement( void )
{
	return "flush privileges;";
}

void grant_select_revoke_display(
		char *revoke_sql_statement,
		char *drop_user_sql_statement,
		char *flush_privileges_sql_statement )
{
	FILE *output_pipe;
	char system_string[ 128 ];

	if ( !drop_user_sql_statement
	||   !revoke_sql_statement
	||   !flush_privileges_sql_statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"html_table.e \"^%s\" \"%s\"",
		"Revoke Select",
		"Sql Statement" );

	output_pipe = appaserver_output_pipe( system_string );

	fprintf(output_pipe,
		"%s\n%s\n%s\n",
		revoke_sql_statement,
		drop_user_sql_statement,
		flush_privileges_sql_statement );

	pclose( output_pipe );
}

void grant_select_display(
		char *create_user_sql_statement,
		LIST *on_table_sql_statement_list,
		char *update_process_sql_statement,
		char *flush_privileges_sql_statement )
{
	FILE *output_pipe;
	char system_string[ 128 ];
	char *on_table_sql_statement;

	if ( !create_user_sql_statement
	||   !update_process_sql_statement
	||   !flush_privileges_sql_statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"html_table.e \"^%s\" \"%s\"",
		"Grant Select",
		"Sql Statement" );

	output_pipe = appaserver_output_pipe( system_string );

	fprintf(output_pipe,
		"%s\n",
		create_user_sql_statement );

	if ( list_rewind( on_table_sql_statement_list ) )
	{
		do {
			on_table_sql_statement =
				list_get(
					on_table_sql_statement_list );

			fprintf(output_pipe,
				"%s\n",
				on_table_sql_statement );

		} while ( list_next( on_table_sql_statement_list ) );
	}

	fprintf(output_pipe,
		"%s\n%s\n",
		update_process_sql_statement,
		flush_privileges_sql_statement );

	pclose( output_pipe );
}

void grant_select_execute(
		char *create_user_sql_statement,
		LIST *on_table_sql_statement_list,
		char *update_process_sql_statement,
		char *flush_privileges_sql_statement )
{
	FILE *output_pipe;
	char *system_string;
	char *on_table_sql_statement;

	if ( !create_user_sql_statement
	||   !update_process_sql_statement
	||   !flush_privileges_sql_statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string = "sql.e 2>&1 | html_paragraph_wrapper.e";

	output_pipe = appaserver_output_pipe( system_string );

	fprintf(output_pipe,
		"%s\n",
		create_user_sql_statement );

	if ( list_rewind( on_table_sql_statement_list ) )
	{
		do {
			on_table_sql_statement =
				list_get(
					on_table_sql_statement_list );

			fprintf(output_pipe,
				"%s\n",
				on_table_sql_statement );

		} while ( list_next( on_table_sql_statement_list ) );
	}

	fprintf(output_pipe,
		"%s\n%s\n",
		update_process_sql_statement,
		flush_privileges_sql_statement );

	pclose( output_pipe );
}

void grant_select_revoke_execute(
		char *revoke_sql_statement,
		char *drop_user_sql_statement,
		char *flush_privileges_sql_statement )
{
	FILE *output_pipe;
	char *system_string;

	if ( !drop_user_sql_statement
	||   !revoke_sql_statement
	||   !flush_privileges_sql_statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string = "sql.e 2>&1 | html_paragraph_wrapper.e";

	output_pipe = appaserver_output_pipe( system_string );

	fprintf(output_pipe,
		"%s\n%s\n%s\n",
		revoke_sql_statement,
		drop_user_sql_statement,
		flush_privileges_sql_statement );

	pclose( output_pipe );
}

