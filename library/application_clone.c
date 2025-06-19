/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_clone.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "file.h"
#include "application.h"
#include "role.h"
#include "entity_self.h"
#include "login_default_role.h"
#include "application_clone.h"

APPLICATION_CLONE *application_clone_new(
		char *current_application_name,
		char *login_name,
		char *destination_application_name,
		char *application_clone_sql_gz_filespecification,
		char *application_title,
		boolean post_signup_boolean,
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

	if ( application_clone->
		application_create->
		application_name_invalid
	||   application_clone->
		application_create->
		application_log_exists_boolean )
	{
		return application_clone;
	}

	if ( application_clone_sql_gz_filespecification )
	{
		application_clone->gz_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_clone_gz_system_string(
				destination_application_name,
				application_clone_sql_gz_filespecification );
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
			login_name,
			post_signup_boolean );

	if ( !application_clone->appaserver_user->password )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"appaserver_user->password is empty." );

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
			application_clone->
				appaserver_user->
				full_name,
			application_clone->
				appaserver_user->
				street_address,
			application_clone->
				appaserver_user->
				login_name,
			application_clone->
				appaserver_user->
				password,
			application_clone->
				appaserver_user->
				user_date_format );

	application_clone->insert_role_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_insert_role_system_string(
			ROLE_SYSTEM,
			ROLE_SUPERVISOR,
			destination_application_name,
			application_clone->
				appaserver_user->
				full_name,
			application_clone->
				appaserver_user->
				street_address );

	application_clone->insert_default_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_insert_default_system_string(
			LOGIN_DEFAULT_ROLE_TABLE,
			ROLE_SUPERVISOR,
			destination_application_name,
			application_clone->
				appaserver_user->
				full_name,
			application_clone->
				appaserver_user->
				street_address );

	application_clone->insert_entity_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_insert_entity_system_string(
			ENTITY_TABLE,
			destination_application_name,
			application_clone->
				appaserver_user->
				full_name,
			application_clone->
				appaserver_user->
				street_address );

	application_clone->insert_self_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_clone_insert_entity_system_string(
			ENTITY_SELF_TABLE,
			destination_application_name,
			application_clone->
				appaserver_user->
				full_name,
			application_clone->
				appaserver_user->
				street_address );

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
	char system_string[ 4096 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
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
		char *sql_gz_filespecification )
{
	char system_string[ 256 ];

	if ( !destination_application_name
	||   !sql_gz_filespecification )
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
		system_string,
		sizeof ( system_string ),
		"zcat %s |"
		"mysqldump_sed.sh |"
		"sql.e %s",
		sql_gz_filespecification,
		destination_application_name );

	return strdup( system_string );
}

char *application_clone_sql_gz_filespecification(
		const char *application_clone_sql_gz,
		char *mount_point )
{
	static char filespecification[ 128 ];

	if ( !mount_point )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"mount_point is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		filespecification,
		sizeof ( filespecification ),
		"%s/%s",
		mount_point,
		application_clone_sql_gz );

	if ( !file_exists_boolean( filespecification ) )
	{
		char message[ 256 ];

		sprintf(message,
			"file_exists_boolean(%s) returned empty.",
			filespecification );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return filespecification;
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

char *application_clone_insert_default_system_string(
		const char *login_default_role_table,
		const char *role_supervisor,
		char *destination_application_name,
		char *full_name,
		char *street_address )
{
	char system_string[ 1024 ];
	char *field;

	if ( !destination_application_name
	||   !full_name
	||   !street_address )
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

	field = "full_name,street_address,role";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s^%s^%s\" | "
		"insert_statement.e "
		"table=%s "
		"field=%s "
		"delimiter='^' | "
		"sql.e %s",
		full_name,
		street_address,
		role_supervisor,
		login_default_role_table,
		field,
		destination_application_name );

	return strdup( system_string );
}

char *application_clone_insert_entity_system_string(
		const char *table_name,
		char *destination_application_name,
		char *full_name,
		char *street_address )
{
	char system_string[ 1024 ];
	char *field;

	if ( !destination_application_name
	||   !full_name
	||   !street_address )
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

	field = "full_name,street_address";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s^%s\" | "
		"insert_statement.e "
		"table=%s "
		"field=%s "
		"delimiter='^' | "
		"sql.e %s",
		full_name,
		street_address,
		table_name,
		field,
		destination_application_name );

	return strdup( system_string );
}

char *application_clone_insert_role_system_string(
		const char *role_system,
		const char *role_supervisor,
		char *destination_application_name,
		char *full_name,
		char *street_address )
{
	char system_string[ 1024 ];
	char *ptr = system_string;
	char *field;

	if ( !destination_application_name
	||   !full_name
	||   !street_address )
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


	field = "full_name,street_address,role";

	ptr += sprintf( ptr,
		"echo \"%s^%s^%s\" | "
		"insert_statement.e "
		"table=role_appaserver_user "
		"field=%s "
		"delimiter='^' | "
		"sql.e %s && ",
		full_name,
		street_address,
		role_system,
		field,
		destination_application_name );

	ptr += sprintf( ptr,
		"echo \"%s^%s^%s\" | "
		"insert_statement.e "
		"table=role_appaserver_user "
		"field=%s "
		"delimiter='^' | "
		"sql.e %s",
		full_name,
		street_address,
		role_supervisor,
		field,
		destination_application_name );

	return strdup( system_string );
}

char *application_clone_insert_user_system_string(
		const char *appaserver_user_table,
		char *destination_application_name,
		char *full_name,
		char *street_address,
		char *login_name,
		char *password,
		char *user_date_format )
{
	char system_string[ 1024 ];
	char *field;

	if ( !destination_application_name
	||   !full_name
	||   !street_address
	||   !login_name
	||   !password
	||   !user_date_format )
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

	field =	"full_name,"
		"street_address,"
		"login_name,"
		"password,"
		"user_date_format";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s^%s^%s^%s^%s\" | "
		"insert_statement.e "
		"table=%s "
		"field=%s "
		"delimiter='^' | "
		"sql.e %s",
		full_name,
		street_address,
		login_name,
		password,
		user_date_format,
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
	list_set( list, "self" );
	list_set( list, "entity" );
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

APPASERVER_USER *application_clone_appaserver_user(
		char *login_name,
		boolean post_signup_boolean )
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

	if ( !post_signup_boolean )
	{
		appaserver_user =
			appaserver_user_login_fetch(
				login_name,
				0 /* not fetch_role_name_list */ );

		if ( !appaserver_user )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"appaserver_user_login_fetch(%s) returned empty.",
				login_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return appaserver_user;
	}

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_new(
			"Fullname Placeholder" /* full_name */,
			"1234 Set Me" /* street_address */ );

	appaserver_user->login_name = login_name;

	appaserver_user->password =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_generate_password();

	appaserver_user->user_date_format = "american";

	return appaserver_user;
}

boolean application_clone_post_signup_boolean( char *process_name )
{
	if ( process_name
	&&   strcmp( process_name, "process" ) == 0 )
	{
		return 1;
	}

	return 0;
}

