/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/drop_table.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "folder.h"
#include "shell_script.h"
#include "drop_table.h"

DROP_TABLE *drop_table_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *table_name )
{
	DROP_TABLE *drop_table;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	drop_table = drop_table_calloc();

	drop_table->session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			DROP_TABLE_EXECUTABLE
				/* process_or_set_name */ );

	drop_table->execute_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		drop_table_execute_system_string(
			table_name );

	drop_table->shell_script =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		drop_table_shell_script(
			application_name,
			drop_table->execute_system_string );

	drop_table->appaserver_parameter_data_directory =
		/* ------------------------------------------------ */
		/* Returns component of global_appaserver_parameter */
		/* ------------------------------------------------ */
		appaserver_parameter_data_directory();

	drop_table->process_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		drop_table_process_filename(
			table_name,
			drop_table->
				appaserver_parameter_data_directory );

	return drop_table;
}

DROP_TABLE *drop_table_calloc( void )
{
	DROP_TABLE *drop_table;

	if ( ! ( drop_table = calloc( 1, sizeof ( DROP_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drop_table;
}

char *drop_table_href_string(
		const char *drop_table_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *table_name )
{
	char href_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		href_string,
		sizeof ( href_string ),
		"%s/%s?%s+%s+%s+%s+%s+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean(
				application_name ) ),
		drop_table_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		drop_table_executable /* process_name */,
		table_name,
		"y" /* content_type_yn */,
		"y" /* execute_yn */ );

	return strdup( href_string );
}

char *drop_table_execute_system_string( char *table_name )
{
	static char system_string[ 256 ];

	if ( !table_name )
	{
		char message[ 128 ];

		sprintf(message, "table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"drop table %s;\" |"
		"tee_appaserver.sh |"
		"sql.e 2>&1",
		table_name );

	return system_string;
}

char *drop_table_shell_script(
		char *application_name,
		char *execute_system_string )
{
	char shell_script[ 1024 ];
	char *ptr = shell_script;

	if ( !application_name
	||   !execute_system_string )
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
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name ) );

	ptr += sprintf(
		ptr,
		"%s\n",
		execute_system_string );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	return strdup( shell_script );
}

char *drop_table_process_filename(
		char *table_name,
		char *data_directory )
{
	static char process_filename[ 128 ];

	if ( !table_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		process_filename,
		sizeof ( process_filename ),
		"%s/drop_table_%s.sh",
		data_directory,
		table_name );

	return process_filename;
}

