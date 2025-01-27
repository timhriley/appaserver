/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_clone.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "timlib.h"
#include "application.h"
#include "role.h"
#include "application_clone.h"

APPLICATION_CLONE *application_clone_new(
		char *current_application_name,
		char *login_name,
		char *destination_application_name,
		char *application_clone_sql_gz_filename,
		char *application_title,
		char *document_root,
		char *data_directory,
		char *upload_directory,
		char *log_directory,
		char *mount_point )
{
	APPLICATION_CLONE *application_clone;

	if ( !current_application_name
	||   !login_name
	||   !destination_application_name
	||   !application_title
	||   !document_root
	||   !data_directory
	||   !upload_directory
	||   !log_directory
	||   !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	application_clone = application_clone_calloc();

	application_clone->application_create =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_create_new(
			destination_application_name,
			application_title,
			document_root,
			data_directory,
			upload_directory,
			log_directory,
			mount_point );

	if ( application_clone->application_create->application_name_invalid
	||   application_clone->application_create->application_exists_boolean )
	{
		return application_clone;
	}

	if ( application_clone_sql_gz_filename )
	{
		application_clone->gz_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_clone_gz_system_string(
				destination_application_name,
				application_clone_sql_gz_filename );
	}
	else
	{
		application_clone->zaptable_list =
			application_clone_zaptable_list();

		application_clone->database_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_clone_database_system_string(
				current_application_name,
				destination_application_name,
				application_clone->zaptable_list );
	}

	application_clone->application =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_fetch(
			current_application_name );

	application_clone->application_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_application_system_string(
			APPLICATION_TABLE,
			destination_application_name,
			application_title,
			application_clone->application->ssl_support_boolean );

	application_clone->appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_clone_appaserver_user(
			login_name );

	if ( !application_clone->appaserver_user->database_password )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"appaserver_user->database_password is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	application_clone->insert_user_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_insert_user_system_string(
			APPASERVER_USER_TABLE,
			destination_application_name,
			application_clone->appaserver_user );

	application_clone->insert_role_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_insert_role_system_string(
			ROLE_SYSTEM,
			ROLE_SUPERVISOR,
			destination_application_name,
			login_name );

	application_clone->upgrade_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_upgrade_system_string(
			destination_application_name );

	return application_clone;
}

APPLICATION_CLONE *application_clone_calloc( void )
{
	APPLICATION_CLONE *application_clone;

	if ( ! ( application_clone =
			calloc( 1,
				sizeof ( APPLICATION_CLONE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_clone;
}

char *application_clone_database_system_string(
		char *current_application_name,
		char *destination_application_name,
		LIST *application_create_zaptable_list )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"mysqldump_clone.sh %s %s %s",
		current_application_name,
		destination_application_name,
		list_display_delimited(
			application_create_zaptable_list,
			' ' ) );

	return strdup( system_string );
}

char *application_clone_gz_system_string(
		char *destination_application_name,
		char *sql_gz_filename )
{
	char system_string[ 256 ];

	if ( !sql_gz_filename )
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
		"zcat %s | sql.e %s",
		sql_gz_filename,
		destination_application_name );

	return strdup( system_string );
}

char *application_clone_sql_gz_filename( char *mount_point )
{
	static char filename[ 128 ];

	sprintf(filename,
		"%s/template/mysqldump_template.sql.gz",
		mount_point );

	if ( !timlib_file_exists( filename ) )
	{
		char message[ 256 ];

		sprintf(message,
			"timlib_file_exists(%s) returned empty.",
			filename );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return filename;
}

char *application_clone_application_system_string(
		const char *application_table,
		char *destination_application_name,
		char *application_title,
		boolean ssl_support_boolean )
{
	char update_statement[ 256 ];
	char system_string[ 512 ];

	if ( !destination_application_name
	||   !application_title )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		update_statement,
		sizeof ( update_statement ),
		"update %s set "
		"application='%s',"
		"application_title='%s',"
		"ssl_support_yn='%c';",
		application_table,
		destination_application_name,
		application_title,
		(ssl_support_boolean)
			? 'y'
			: 'n' );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | sql.e %s",
		update_statement,
		destination_application_name );

	return strdup( system_string );
}

char *application_clone_insert_role_system_string(
		const char *role_system,
		const char *role_supervisor,
		char *destination_application_name,
		char *login_name )
{
	char system_string[ 1024 ];
	char *ptr = system_string;

	ptr += sprintf( ptr,
		"echo \"%s^%s\" | "
		"insert_statement.e "
		"table=role_appaserver_user "
		"field=login_name,role "
		"delimiter='^' | "
		"sql.e %s && ",
		login_name,
		role_system,
		destination_application_name );

	ptr += sprintf( ptr,
		"echo \"%s^%s\" | "
		"insert_statement.e "
		"table=role_appaserver_user "
		"field=login_name,role "
		"delimiter='^' | "
		"sql.e %s",
		login_name,
		role_supervisor,
		destination_application_name );

	return strdup( system_string );
}

char *application_clone_insert_user_system_string(
		const char *appaserver_user_table,
		char *destination_application_name,
		APPASERVER_USER *appaserver_user )
{
	char system_string[ 1024 ];
	char *field =
		"login_name,"
		"person_full_name,"
		"password,"
		"user_date_format";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s^%s^%s^%s\" | "
		"insert_statement.e "
		"table=%s "
		"field=%s "
		"delimiter='^' | "
		"sql.e %s",
		appaserver_user->login_name,
		(appaserver_user->full_name)
			? appaserver_user->full_name
			: "",
		appaserver_user->database_password,
		(appaserver_user->user_date_format)
			? appaserver_user->user_date_format
			: "",
		appaserver_user_table,
		field,
		destination_application_name );

	return strdup( system_string );
}

LIST *application_clone_zaptable_list( void )
{
	LIST *list = list_new();

	list_set( list, "login_default_role" );
	list_set( list, "role_appaserver_user" );
	list_set( list, "appaserver_user" );
	list_set( list, "session" );

	return list;
}

void application_clone_system(
		boolean execute_boolean,
		char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( execute_boolean )
	{
		if ( system( system_string ) ){}
	}
	else
	{
		printf( "<p>%s\n", system_string );
	}
}

char *application_clone_upgrade_system_string(
		char *current_application_name )
{
	char system_string[ 128 ];

	if ( !current_application_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"current_application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"upgrade-appaserver %s 2>&1 | "
		"html_paragraph_wrapper.e",
		current_application_name );

	return strdup( system_string );
}

APPASERVER_USER *application_clone_appaserver_user( char *login_name )
{
	APPASERVER_USER *appaserver_user;

	if ( !login_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ( appaserver_user =
		appaserver_user_fetch(
			login_name,
			0 /* not fetch_role_name_list */ ) ) )
	{
		return appaserver_user;
	}

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_new(
			login_name );

	appaserver_user->database_password =
		security_generate_password();

	return appaserver_user;
}

