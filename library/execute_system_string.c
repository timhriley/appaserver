/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/execute_system_string.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "execute_system_string.h"

char *execute_system_string_table_edit(
		const char *executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *results_string,
		char *error_string,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_SIZE_HASH_TABLE ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | %s %s %s %s %s %s \"%s\" \"%s\" 2>>%s",
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		executable,
		session_key,
		login_name,
		role_name,
		folder_name,
		target_frame,
		(results_string)
			? results_string
			: "",
		(error_string)
			? error_string
			: "",
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_table_insert(
		const char *executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *result_string,
		char *error_string,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_SIZE_HASH_TABLE ];

	if ( !folder_name
	||   !*folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !target_frame
	||   !appaserver_error_filename )
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
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | %s %s %s %s %s %s \"%s\" \"%s\" 2>>%s",
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		executable,
		session_key,
		login_name,
		role_name,
		folder_name,
		target_frame,
		(result_string)
			? result_string
			: "",
		(error_string)
			? error_string
			: "",
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_folder_isa(
		const char *executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *one_folder_name,
		char *appaserver_error_filename )
{
	char system_string[ STRING_2K ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !insert_folder_name
	||   !one_folder_name
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s %s %s 2>>%s",
		executable,
		session_key,
		login_name,
		role_name,
		insert_folder_name,
		one_folder_name,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_prompt_insert(
		const char *executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | %s %s %s %s %s 2>>%s",
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		executable,
		session_key,
		login_name,
		role_name,
		folder_name,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_prompt_lookup(
		const char *executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | %s %s %s %s %s 2>>%s",
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		executable,
		session_key,
		login_name,
		role_name,
		folder_name,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_lookup(
		const char *executable,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	if ( !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename )
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
		"echo \"%s\" | %s %s %s %s 2>>%s",
		(dictionary_string) ? dictionary_string : "",
		executable,
		login_name,
		role_name,
		folder_name,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_lookup_delete(
		const char *executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *send_string,
		char *appaserver_error_filename,
		char *one_two_state )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !send_string
	||   !appaserver_error_filename
	||   !one_two_state )
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
		"echo \"%s\" |"
		"%s %s %s %s %s %s %s 2>>%s",
		(send_string) ? send_string : "",
		executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		one_two_state,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_lookup_sort(
		const char *lookup_sort_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_64K ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename )
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
		"echo \"%s\" | %s %s %s %s %s %s one 2>>%s",
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		lookup_sort_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_choose_role(
		const char *executable,
		char *session_key,
		char *login_name,
		boolean frameset_menu_horizontal,
		char *output_filename )
{
	char system_string[ 1024 ];

	if ( !session_key
	||   !login_name
	||   !output_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %c > %s",
		executable,
		session_key,
		login_name,
		(frameset_menu_horizontal) ? 'y' : 'n',
		output_filename );

	return strdup( system_string );
}

char *execute_system_string_menu(
		const char *menu_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean application_menu_horizontal_boolean )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s %s %c %c",
		menu_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		(application_menu_horizontal_boolean) ? 'y' : 'n',
		'n' /* content_type_yn */ );

	return strdup( system_string );
}

char *execute_system_string_frameset(
		const char *executable,
		char *session_key,
		char *login_name,
		char *post_login_output_pipe_string,
		char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !session_key
	||   !login_name
	||   !post_login_output_pipe_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s 2>>%s %s",
		executable,
		session_key,
		login_name,
		appaserver_error_filename,
		post_login_output_pipe_string );

	return strdup( system_string );
}

char *execute_system_string_drillthru(
		const char *drillthru_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !state
	||   !dictionary_separate_send_string
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s %s \"%s\" \"%s\" 2>>%s",
		drillthru_executable,
		session_key,
		login_name,
		role_name,
		state,
		(folder_name) ? folder_name : "",
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_drilldown(
		const char *drilldown_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *target_frame,
		char *drilldown_base_folder_name,
		char *drilldown_primary_data_list_string,
		char *update_results_string,
		char *update_row_list_error_string,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ 4096 ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !target_frame
	||   !drilldown_base_folder_name
	||   !drilldown_primary_data_list_string
	||   !dictionary_separate_send_string
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s %s %s \"%s\" \"%s\" \"%s\" \"%s\" 2>>%s",
		drilldown_executable,
		session_key,
		login_name,
		role_name,
		target_frame,
		drilldown_base_folder_name,
		drilldown_primary_data_list_string,
		(update_results_string)
			? update_results_string
			: "",
		(update_row_list_error_string)
			? update_row_list_error_string
			: "",
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_post_prompt_insert(
		const char *executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_SIZE_HASH_TABLE ];
	char omit_content_type_yn = 'y';

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | %s %s %s %s %s %s %c 2>>%s",
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		omit_content_type_yn,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_session(
		const char *executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename )
{
	char system_string[ STRING_SIZE_HASH_TABLE ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !dictionary_separate_send_string
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s %s \"%s\" 2>>%s",
		executable,
		session_key,
		login_name,
		role_name,
		folder_name,
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}

char *execute_system_string_create_application(
		const char *application_create_executable,
		char *session_key,
		char *login_name,
		char *destination_application,
		char *application_title,
		char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !session_key
	||   !login_name
	||   !destination_application
	||   !application_title
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s process %s \"%s\" %c 2>>%s",
		application_create_executable,
		session_key,
		login_name,
		destination_application,
		application_title,
		'y' /* execute_yn */,
		appaserver_error_filename );

	return strdup( system_string );
}

