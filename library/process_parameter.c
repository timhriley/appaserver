/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_parameter.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "security.h"
#include "dictionary.h"
#include "folder.h"
#include "folder_attribute.h"
#include "query.h"
#include "relation_mto1.h"
#include "widget.h"
#include "process_parameter.h"

PROCESS_PARAMETER_PROMPT *process_parameter_prompt_fetch(
		char *prompt_name )
{
	PROCESS_PARAMETER_PROMPT *process_parameter_prompt;
	char *tmp;

	if ( !prompt_name
	||   strcmp( prompt_name, "null" ) == 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_parameter_prompt =
	    /* -------------- */
	    /* Safely returns */
	    /* -------------- */
	    process_parameter_prompt_parse(
		prompt_name,
		string_fetch_pipe(
			( tmp =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_parameter_prompt_system_string(
				PROCESS_PARAMETER_PROMPT_SELECT,
				PROCESS_PARAMETER_PROMPT_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_prompt_primary_where(
					prompt_name ) ) ) ) );

	free( tmp );

	return process_parameter_prompt;
}

char *process_parameter_prompt_primary_where( char *prompt_name )
{
	static char where[ 256 ];

	sprintf(where,
		"prompt = '%s'",
		prompt_name );

	return where;
}

char *process_parameter_prompt_system_string(
		char *process_parameter_prompt_select,
		char *process_parameter_prompt_table,
		char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		process_parameter_prompt_select,
		process_parameter_prompt_table,
		where );

	return strdup( system_string );
}

PROCESS_PARAMETER_PROMPT *process_parameter_prompt_calloc( void )
{
	PROCESS_PARAMETER_PROMPT *process_parameter_prompt;

	if ( ! ( process_parameter_prompt =
			calloc( 1, sizeof ( PROCESS_PARAMETER_PROMPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_parameter_prompt;
}

PROCESS_PARAMETER_PROMPT *process_parameter_prompt_new(
			char *prompt_name )
{
	PROCESS_PARAMETER_PROMPT *process_parameter_prompt;

	if ( !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_parameter_prompt = process_parameter_prompt_calloc();
	process_parameter_prompt->prompt_name = prompt_name;

	return process_parameter_prompt;
}

PROCESS_PARAMETER_PROMPT *process_parameter_prompt_parse(
		char *prompt_name,
		char *input )
{
	char buffer[ 4096 ];
	PROCESS_PARAMETER_PROMPT *process_parameter_prompt;

	if ( !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_parameter_prompt =
		process_parameter_prompt_new(
			prompt_name );

	/* See PROCESS_PARAMETER_PROMPT_SELECT */
	/* ----------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	process_parameter_prompt->input_width = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
	{
		process_parameter_prompt->hint_message = strdup( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 2 );
	process_parameter_prompt->upload_filename_boolean = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 3 );
	process_parameter_prompt->date_boolean = ( *buffer == 'y' );

	return process_parameter_prompt;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_calloc(
		void )
{
	PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *p;

	if ( ! ( p  =
		    calloc(
			1,
			sizeof ( PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_new(
		char *drop_down_prompt_name,
		char *drop_down_prompt_data )
{
	PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
		process_parameter_drop_down_prompt_data =
			process_parameter_drop_down_prompt_data_calloc();

	process_parameter_drop_down_prompt_data->
		drop_down_prompt_name =
			drop_down_prompt_name;

	process_parameter_drop_down_prompt_data->
		drop_down_prompt_data =
			drop_down_prompt_data;

	return process_parameter_drop_down_prompt_data;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
	process_parameter_drop_down_prompt_data_parse(
		char *drop_down_prompt_name,
		char *input )
{
	char prompt_data[ 128 ];
	char display_order[ 128 ];
	PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *
		process_parameter_drop_down_prompt_data;

	if ( !drop_down_prompt_name
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_SELECT */
	/* -------------------------------------------------- */
	piece( prompt_data, SQL_DELIMITER, input, 0 );
	piece( display_order, SQL_DELIMITER, input, 1 );

	process_parameter_drop_down_prompt_data =
		process_parameter_drop_down_prompt_data_new(
			drop_down_prompt_name,
			strdup( prompt_data ) );

	process_parameter_drop_down_prompt_data->
		display_order =
			atoi( display_order );

	return process_parameter_drop_down_prompt_data;
}

LIST *process_parameter_drop_down_prompt_data_list(
		char *drop_down_prompt_name,
		char *where )
{
	if ( !drop_down_prompt_name
	||   !where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	process_parameter_drop_down_prompt_data_system_list(
		drop_down_prompt_name,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_parameter_drop_down_prompt_data_system_string(
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_SELECT,
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE,
			where ) );
}

FILE *process_parameter_drop_down_prompt_data_pipe(
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

	return popen( system_string, "r" );
}

LIST *process_parameter_drop_down_prompt_data_system_list(
		char *drop_down_prompt_name,
		char *system_string )
{
	LIST *list;
	char input[ 1024 ];
	FILE *pipe;

	if ( !drop_down_prompt_name
	||   !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe = process_parameter_drop_down_prompt_data_pipe( system_string );

	list = list_new();

	while ( string_input( input, pipe, 1024 ) )
	{
		list_set(
			list,
			process_parameter_drop_down_prompt_data_parse(
				drop_down_prompt_name,
				input ) );
	}

	pclose( pipe );
	return list;
}

char *process_parameter_drop_down_prompt_data_system_string(
		char *process_parameter_drop_down_prompt_data_select,
		char *process_parameter_drop_down_prompt_data_table,
		char *process_parameter_drop_down_where )
{
	static char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		process_parameter_drop_down_prompt_data_select,
		process_parameter_drop_down_prompt_data_table,
		process_parameter_drop_down_where );

	return system_string;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_calloc(
		void )
{
	PROCESS_PARAMETER_DROP_DOWN_PROMPT *p;

	if ( ! ( p = calloc(
			1,
			sizeof ( PROCESS_PARAMETER_DROP_DOWN_PROMPT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_new(
		char *drop_down_prompt_name )
{
	PROCESS_PARAMETER_DROP_DOWN_PROMPT *process_parameter_drop_down_prompt =
		process_parameter_drop_down_prompt_calloc();

	process_parameter_drop_down_prompt->
		drop_down_prompt_name =
			drop_down_prompt_name;

	return process_parameter_drop_down_prompt;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_fetch(
		char *drop_down_prompt_name,
		boolean multi_select_boolean )
{
	char *where;
	char *tmp;
	PROCESS_PARAMETER_DROP_DOWN_PROMPT *
		process_parameter_drop_down_prompt;

	if ( !drop_down_prompt_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: drop_down_prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_parameter_drop_down_prompt_primary_where(
			drop_down_prompt_name );

	process_parameter_drop_down_prompt =
		process_parameter_drop_down_prompt_parse(
			drop_down_prompt_name,
			multi_select_boolean,
			where,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			string_pipe_input(
			     ( tmp =
			     /* ------------------- */
			     /* Returns heap memory */
			     /* ------------------- */
			       process_parameter_drop_down_prompt_system_string(
		   		   PROCESS_PARAMETER_DROP_DOWN_PROMPT_SELECT,
		   		   PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE,
		   		   where ) ) ) );

	free( tmp );

	return process_parameter_drop_down_prompt;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_parse(
		char *drop_down_prompt_name,
		boolean multi_select_boolean,
		char *where,
		char *input )
{
	char buffer[ STRING_64K ];
	PROCESS_PARAMETER_DROP_DOWN_PROMPT *process_parameter_drop_down_prompt;

	if ( !drop_down_prompt_name
	||   !where
	||   !input )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_parameter_drop_down_prompt =
		process_parameter_drop_down_prompt_calloc();

	process_parameter_drop_down_prompt->drop_down_prompt_name =
		drop_down_prompt_name;

	process_parameter_drop_down_prompt->multi_select_boolean =
		multi_select_boolean;

	/* See PROCESS_PARAMETER_DROP_DOWN_PROMPT_SELECT */
	/* --------------------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer )
	{
		process_parameter_drop_down_prompt->hint_message =
			strdup( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
	{
		process_parameter_drop_down_prompt->optional_display =
			strdup( buffer );
	}

	process_parameter_drop_down_prompt->data_list =
		process_parameter_drop_down_prompt_data_list(
		   drop_down_prompt_name,
		   where );

	process_parameter_drop_down_prompt->data_option_list =
		process_parameter_drop_down_prompt_data_option_list(
			process_parameter_drop_down_prompt->data_list );

	return process_parameter_drop_down_prompt;
}

char *drop_down_prompt_primary_where( char *drop_down_prompt_name )
{
	static char where[ 256 ];

	sprintf(where,
		"drop_down_prompt = '%s'",
		drop_down_prompt_name );

	return where;
}

char *process_parameter_drop_down_prompt_system_string(
		char *process_parameter_drop_down_prompt_select,
		char *process_parameter_drop_down_prompt_table,
		char *process_parameter_drop_down_prompt_primary_where )
{
	char system_string[ 2048 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		process_parameter_drop_down_prompt_select,
		process_parameter_drop_down_prompt_table,
		process_parameter_drop_down_prompt_primary_where );

	return strdup( system_string );
}

FILE *process_parameter_input_pipe( char *system_string )
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

	return popen( system_string, "r" );
}

LIST *process_parameter_system_list(
		char *application_name,
		char *login_name,
		char *role_name,
		DICTIONARY *drillthru_dictionary,
		char *system_string )
{
	LIST *list;
	char input[ 1024 ];
	PROCESS_PARAMETER *process_parameter;
	FILE *input_pipe;

	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	list = list_new();

	input_pipe = process_parameter_input_pipe( system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		process_parameter =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_parameter_parse(
				application_name,
				login_name,
				role_name,
				drillthru_dictionary,
				input );

		list_set( list, process_parameter );
	}

	pclose( input_pipe );

	return list;
}

char *process_parameter_system_string(
		const char *process_parameter_select,
		const char *process_parameter_table,
		char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" display_order",
		process_parameter_select,
		process_parameter_table,
		where );

	return strdup( system_string );
}

char *process_parameter_set_system_string(
		const char *process_set_parameter_select,
		const char *process_set_parameter_table,
		char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" display_order",
		process_set_parameter_select,
		process_set_parameter_table,
		where );

	return strdup( system_string );
}

PROCESS_PARAMETER *process_parameter_parse(
		char *application_name,
		char *login_name,
		char *role_name,
		DICTIONARY *drillthru_dictionary,
		char *input )
{
	char process_or_set_name[ 128 ];
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char drop_down_prompt_name[ 128 ];
	char prompt_name[ 128 ];
	char buffer[ 128 ];
	PROCESS_PARAMETER *process_parameter;

	process_parameter = process_parameter_calloc();

	/* ------------------------------------------------------------ */
	/* See PROCESS_PARAMETER_SELECT or PROCESS_SET_PARAMETER_SELECT */
	/* ------------------------------------------------------------ */
	/* Note: all of the primary keys have at least a null string	*/
	/* placeholder.							*/
	/* ------------------------------------------------------------ */
	piece( process_or_set_name, SQL_DELIMITER, input, 0 );
	piece( folder_name, SQL_DELIMITER, input, 1 );
	piece( attribute_name, SQL_DELIMITER, input, 2 );
	piece( drop_down_prompt_name, SQL_DELIMITER, input, 3 );
	piece( prompt_name, SQL_DELIMITER, input, 4 );

	process_parameter->process_or_set_name = strdup( process_or_set_name );
	process_parameter->folder_name = strdup( folder_name );
	process_parameter->attribute_name = strdup( attribute_name );

	process_parameter->drop_down_prompt_name =
		strdup( drop_down_prompt_name );

	process_parameter->prompt_name = strdup( prompt_name );

	piece( buffer, SQL_DELIMITER, input, 5 );
	process_parameter->display_order = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	process_parameter->drop_down_multi_select = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 7 );
	process_parameter->drillthru = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer )
	{
		process_parameter->
			populate_drop_down_process_name =
				strdup( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer )
	{
		process_parameter->
			populate_helper_process_name =
				strdup( buffer );
	}

	if ( process_parameter->populate_drop_down_process_name )
	{
		/* ---------------------------------- */
		/* May just need the scalar variables */
		/* ---------------------------------- */
		if ( application_name )
		{
			process_parameter->process_parameter_drop_down =
			    process_parameter_drop_down_process_fetch(
				process_parameter->folder_name,
				process_parameter->prompt_name,
				process_parameter->drop_down_multi_select
					/* multi_select_boolean */,
				process_parameter->
					populate_drop_down_process_name );
		}
	}
	else
	if ( strcmp( folder_name, "null" ) != 0
	/* ---------------------------------- */
	/* May just need the scalar variables */
	/* ---------------------------------- */
	&& application_name
	&& login_name
	&& role_name )
	{
		process_parameter->process_parameter_drop_down =
			process_parameter_drop_down_folder_fetch(
				application_name,
				login_name,
				role_name,
				process_parameter->folder_name,
				drillthru_dictionary,
				process_parameter->
					drop_down_multi_select
					/* multi_select_boolean */ );
	}
	else
	if ( strcmp( attribute_name, "null" ) != 0 )
	{
		process_parameter->process_parameter_attribute =
			process_parameter_attribute_fetch(
				process_parameter->
					attribute_name );
	}
	else
	if (	strcmp( prompt_name, "null" ) != 0
	&&	attribute_is_yes_no(
			prompt_name
				/* attribute_name */ ) )
	{
		process_parameter->process_parameter_yes_no =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_parameter_yes_no_fetch(
				process_parameter->prompt_name );
	}
	else
	if ( strcmp( drop_down_prompt_name, "null" ) != 0 )
	{
		process_parameter->process_parameter_drop_down_prompt =
			process_parameter_drop_down_prompt_fetch(
				process_parameter->drop_down_prompt_name,
				process_parameter->drop_down_multi_select
					/* multi_select_boolean */ );
	}
	else
	if (	strcmp( prompt_name, "null" ) != 0
	&&	strcmp( drop_down_prompt_name, "null" ) == 0 )
	{
		process_parameter->process_parameter_prompt =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_parameter_prompt_fetch(
				process_parameter->prompt_name );
	}

	return process_parameter;
}

PROCESS_PARAMETER *process_parameter_calloc( void )
{
	PROCESS_PARAMETER *process_parameter;

	if ( ! ( process_parameter =
			calloc( 1, sizeof ( PROCESS_PARAMETER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_parameter;
}

char *process_parameter_where(
		char *process_name,
		boolean is_drillthru )
{
	static char parameter_where[ 256 ];

	sprintf(parameter_where,
		"process = '%s' and ifnull(drillthru_yn,'n') = '%c'",
		process_name,
		(is_drillthru) ? 'y' : 'n' );

	return parameter_where;
}

char *process_parameter_set_where(
		char *process_set_name,
		boolean is_drillthru )
{
	static char parameter_where[ 512 ];

	sprintf(parameter_where,
		"process_set = '%s' and ifnull(drillthru_yn,'n') = '%c'",
		process_set_name,
		(is_drillthru) ? 'y' : 'n' );

	return parameter_where;
}

PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_process_fetch(
		char *folder_name,
		char *prompt_name,
		boolean multi_select_boolean,
		char *populate_drop_down_process_name )
{
	PROCESS_PARAMETER_DROP_DOWN *process_parameter_drop_down;

	if ( !populate_drop_down_process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_parameter_drop_down = process_parameter_drop_down_calloc();

	process_parameter_drop_down->multi_select_boolean =
		multi_select_boolean;

	process_parameter_drop_down->name =
		/* -------------------------------------- */
		/* Returns prompt_name, "" or heap memory */
		/* -------------------------------------- */
		process_parameter_drop_down_name(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			folder_name,
			prompt_name,
			(LIST *)0 /* folder_attribute_primary_key_list */ );

	process_parameter_drop_down->process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_fetch(
			populate_drop_down_process_name,
			(char *)0 /* document_root */,
			(char *)0 /* relative_source_directory */,
			1 /* check_executable_inside_filesystem */,
			appaserver_parameter_mount_point() );

	process_parameter_drop_down->delimited_list =
		list_pipe(
			process_parameter_drop_down->
				process->
				command_line );

	process_parameter_drop_down->folder_no_initial_capital =
		folder_no_initial_capital(
			folder_name );

	process_parameter_drop_down->widget_drop_down_option_list =
		widget_drop_down_option_list(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			WIDGET_DROP_DOWN_LABEL_DELIMITER,
			WIDGET_DROP_DOWN_EXTRA_DELIMITER,
			WIDGET_DROP_DOWN_DASH_DELIMITER,
			process_parameter_drop_down->delimited_list,
			process_parameter_drop_down->
				folder_no_initial_capital );

	return process_parameter_drop_down;
}

PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_folder_fetch(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *drillthru_dictionary,
		boolean multi_select_boolean )
{
	PROCESS_PARAMETER_DROP_DOWN *process_parameter_drop_down;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_parameter_drop_down = process_parameter_drop_down_calloc();

	process_parameter_drop_down->multi_select_boolean =
		multi_select_boolean;

	process_parameter_drop_down->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );


	process_parameter_drop_down->fetch_mto1 =
		process_parameter_drop_down_fetch_mto1(
			(boolean)dictionary_length( drillthru_dictionary )
				/* drillthru_dictionary_length */ );

	if ( process_parameter_drop_down->fetch_mto1 )
	{
		process_parameter_drop_down->relation_mto1_list =
			relation_mto1_list(
				/* ----------------------------------- */
				/* Set to cache all folders for a role */
				/* ----------------------------------- */
				role_name,
				folder_name
					/* many_folder_name */,
				process_parameter_drop_down->
					folder->
					folder_attribute_primary_key_list
					/* many_folder_primary_key_list */ );
	}

	process_parameter_drop_down->name =
		/* -------------------------------------- */
		/* Returns prompt_name, "" or heap memory */
		/* -------------------------------------- */
		process_parameter_drop_down_name(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			(char *)0 /* folder_name */,
			(char *)0 /* prompt_name */,
			process_parameter_drop_down->
				folder->
				folder_attribute_primary_key_list );

	process_parameter_drop_down->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	process_parameter_drop_down->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			0 /* not fetch_role_attribute_exclude_list */ );

	process_parameter_drop_down->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			process_parameter_drop_down->
				folder_row_level_restriction->
				non_owner_forbid,
			process_parameter_drop_down->
				role->
				override_row_restrictions );

	process_parameter_drop_down->
		query_drop_down =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_new(
				application_name,
				login_name,
				(char *)0 /* many_folder_name */,
				folder_name /* one_folder_name */,
				(char *)0 /* state */,
				process_parameter_drop_down->
					folder->
					folder_attribute_list,
				process_parameter_drop_down->
					folder->
					populate_drop_down_process_name,
				process_parameter_drop_down->
					relation_mto1_list,
				drillthru_dictionary
					/* process_command_line_dictionary */,
				process_parameter_drop_down->
					security_entity );

	process_parameter_drop_down->widget_drop_down_option_list =
		widget_drop_down_option_list(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			WIDGET_DROP_DOWN_LABEL_DELIMITER,
			WIDGET_DROP_DOWN_EXTRA_DELIMITER,
			WIDGET_DROP_DOWN_DASH_DELIMITER,
			process_parameter_drop_down->
				query_drop_down->
				delimited_list,
			process_parameter_drop_down->
				folder->
				no_initial_capital );

	return process_parameter_drop_down;
}

PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_calloc(
		void )
{
	PROCESS_PARAMETER_DROP_DOWN *process_parameter_drop_down;

	if ( ! ( process_parameter_drop_down =
			calloc( 1, sizeof ( PROCESS_PARAMETER_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_parameter_drop_down;
}

char *process_parameter_drop_down_name(
		const char attribute_multi_key_delimiter,
		char *folder_name,
		char *prompt_name,
		LIST *folder_attribute_primary_key_list )
{
	if (	prompt_name
	&&	strcmp( prompt_name, "null" ) != 0 )
	{
		return prompt_name;
	}

	if (	folder_name
	&&	strcmp( folder_name, "null" ) != 0 )
	{
		folder_attribute_primary_key_list =
			folder_attribute_fetch_primary_key_list(
				folder_name );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_display_delimited(
		folder_attribute_primary_key_list,
		(char)attribute_multi_key_delimiter );
}

LIST *process_parameter_drop_down_attribute_name_list(
		char *folder_name,
		char *prompt_name )
{
	if ( folder_name && strcmp( folder_name, "null" ) != 0 )
	{
		return
		folder_attribute_fetch_primary_key_list(
			folder_name );
	}
	else
	if ( prompt_name && strcmp( prompt_name, "null" ) != 0 )
	{
		LIST *attribute_name_list = list_new();

		list_set( attribute_name_list, prompt_name );

		return attribute_name_list;
	}
	else
	{
		return (LIST *)0;
	}
}

boolean process_parameter_drop_down_fetch_mto1(
		boolean drillthru_dictionary_length )
{
	return drillthru_dictionary_length;
}

LIST *process_parameter_upload_filename_list(
		LIST *process_parameter_list )
{
	LIST *list = {0};
	PROCESS_PARAMETER *process_parameter;

	if ( list_rewind( process_parameter_list ) )
	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( process_parameter->process_parameter_prompt
		&&   process_parameter->
			process_parameter_prompt->
			upload_filename_boolean )
		{
			if ( !list ) list = list_new();

			list_set(
				list,
				process_parameter->
					process_parameter_prompt->
					prompt_name );
		}

	} while ( list_next( process_parameter_list ) );

	return list;
}

LIST *process_parameter_folder_name_list(
		LIST *process_parameter_list )
{
	LIST *list = {0};
	PROCESS_PARAMETER *process_parameter;

	if ( list_rewind( process_parameter_list ) )
	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( strcmp( process_parameter->folder_name, "null" ) != 0 )
		{
			if ( !list ) list = list_new();

			list_set(
				list,
				process_parameter->folder_name );
		}

	} while ( list_next( process_parameter_list ) );

	return list;
}

boolean process_parameter_date_boolean( LIST *process_parameter_list )
{
	PROCESS_PARAMETER *process_parameter;

	if ( list_rewind( process_parameter_list ) )
	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( process_parameter->process_parameter_prompt
		&&   process_parameter->process_parameter_prompt->date_boolean )
		{
			return 1;
		}

	} while ( list_next( process_parameter_list ) );

	return 0;
}

boolean process_parameter_drillthru_boolean(
		char *process_name,
		char *process_set_name )
{
	char *system_string;

	if ( !process_name && !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "both parameters are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( process_name )
	{
		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_parameter_system_string(
				"count(1)" /* select */,
				PROCESS_PARAMETER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_where(
					process_name,
					1 /* is_drillthru */ ) );
	}
	else
	{
		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_parameter_set_system_string(
				"count(1)" /* select */,
				PROCESS_SET_PARAMETER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_set_where(
					process_set_name,
					1 /* is_drillthru */ ) );
	}

	return (boolean)atoi( string_pipe_fetch( system_string ) );
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_set_member(
		char *process_set_default_prompt,
		char *prompt_display_text,
		LIST *member_name_list )
{
	PROCESS_PARAMETER_DROP_DOWN_PROMPT *process_parameter_drop_down_prompt;
	char *drop_down_prompt_name;

	if ( !process_set_default_prompt
	||   !list_length( member_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	drop_down_prompt_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		process_parameter_drop_down_prompt_set_member_name(
			process_set_default_prompt,
			prompt_display_text );

	if ( !drop_down_prompt_name )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: process_parameter_drop_down_prompt_set_member_name() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_parameter_drop_down_prompt =
		process_parameter_drop_down_prompt_new(
			drop_down_prompt_name );

	process_parameter_drop_down_prompt->data_list =
		process_parameter_drop_down_prompt_member_data_list(
			drop_down_prompt_name,
			member_name_list );

	return process_parameter_drop_down_prompt;
}

char *process_parameter_drop_down_prompt_set_member_name(
		char *process_set_process_label,
		char *prompt_display_text )
{
	if ( prompt_display_text && *prompt_display_text )
		return prompt_display_text;
	else
		return process_set_process_label;
}

LIST *process_parameter_drop_down_prompt_member_data_list(
		char *drop_down_prompt_name,
		LIST *member_name_list )
{
	LIST *member_data_list;

	if ( !list_rewind( member_name_list ) ) return (LIST *)0;

	member_data_list = list_new();

	do {
		list_set(
			member_data_list,
			process_parameter_drop_down_prompt_data_new(
				drop_down_prompt_name,
				list_get( member_name_list ) ) );

	} while ( list_next( member_name_list ) );

	return member_data_list;
}

char *process_parameter_drop_down_prompt_primary_where(
		char *drop_down_prompt_name )
{
	static char where[ 256 ];

	sprintf(where,
		"drop_down_prompt = '%s'",
		drop_down_prompt_name );

	return where;
}

LIST *process_parameter_set_member_append(
		LIST *process_parameter_list,
		char *process_set_default_prompt,
		char *prompt_display_text,
		LIST *member_name_list )
{
	PROCESS_PARAMETER *process_parameter;

	if ( !process_parameter_list
	||   !process_set_default_prompt )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( member_name_list ) ) return (LIST *)0;

	process_parameter = process_parameter_calloc();

	if ( ( process_parameter->process_parameter_drop_down_prompt =
		process_parameter_drop_down_prompt_set_member(
			process_set_default_prompt,
			prompt_display_text,
			member_name_list ) ) )
	{
		if ( !prompt_display_text )
		{
			list_set_head(
				process_parameter_list,
				process_parameter );
		}
		else
		{
			list_set(
				process_parameter_list,
				process_parameter );
		}
	}

	return process_parameter_list;
}

PROCESS_PARAMETER_ATTRIBUTE *process_parameter_attribute_fetch(
		char *attribute_name )
{
	PROCESS_PARAMETER_ATTRIBUTE *process_parameter_attribute;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_parameter_attribute = process_parameter_attribute_calloc();

	process_parameter_attribute->attribute =
		attribute_fetch(
			attribute_name );

	return process_parameter_attribute;
}

PROCESS_PARAMETER_ATTRIBUTE *process_parameter_attribute_calloc( void )
{
	PROCESS_PARAMETER_ATTRIBUTE *process_parameter_attribute;

	if ( ! ( process_parameter_attribute =
			calloc(	1,
				sizeof ( PROCESS_PARAMETER_ATTRIBUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_parameter_attribute;
}

LIST *process_parameter_drop_down_prompt_data_option_list( LIST *data_list )
{
	PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA *data;
	LIST *option_list = {0};

	if ( list_rewind( data_list ) )
	do {
		data = list_get( data_list );

		if ( !data->drop_down_prompt_data
		||   !*data->drop_down_prompt_data )
		{
			char message[ 256 ];

			sprintf(message,
	"for drop_down_prompt_name = [%s], drop_down_prompt_data is empty.",
				data->drop_down_prompt_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !option_list ) option_list = list_new();

		list_set(
			option_list,
			widget_drop_down_option_new(
				data->drop_down_prompt_data
					/* string */ ) );

	} while ( list_next( data_list ) );

	return option_list;
}

LIST *process_parameter_list(
		char *process_name,
		char *process_set_name,
		boolean is_drillthru )
{
	char *system_string;

	if ( !process_name && !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "both parameters are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( process_name )
	{
		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_parameter_system_string(
				PROCESS_PARAMETER_SELECT,
				PROCESS_PARAMETER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_where(
					process_name,
					is_drillthru ) );
	}
	else
	{
		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_parameter_set_system_string(
				PROCESS_SET_PARAMETER_SELECT,
				PROCESS_SET_PARAMETER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_set_where(
					process_set_name,
					is_drillthru ) );
	}

	return
	process_parameter_system_list(
		(char *)0 /* application_name */,
		(char *)0 /* login_name */,
		(char *)0 /* role_name */,
		(DICTIONARY *)0 /* drillthru_dictionary */,
		system_string );
}

LIST *process_parameter_date_name_list( LIST *process_parameter_list )
{
	LIST *list = {0};
	PROCESS_PARAMETER *process_parameter;

	if ( list_rewind( process_parameter_list ) )
	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( process_parameter->process_parameter_prompt
		&&   process_parameter->
			process_parameter_prompt->
			date_boolean )
		{
			if ( !list ) list = list_new();

			list_set(
				list,
				process_parameter->
					process_parameter_prompt->
					prompt_name );
		}

	} while ( list_next( process_parameter_list ) );

	return list;
}

LIST *process_parameter_drop_down_prompt_list( LIST *process_parameter_list )
{
	LIST *list = {0};
	PROCESS_PARAMETER *process_parameter;

	if ( list_rewind( process_parameter_list ) )
	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( string_strcmp(
			process_parameter->drop_down_prompt_name,
			"null" ) != 0 )
		{
			if ( !list ) list = list_new();

			list_set(
				list,
				process_parameter->drop_down_prompt_name );
		}

	} while ( list_next( process_parameter_list ) );

	return list;
}

LIST *process_parameter_prompt_list( LIST *process_parameter_list )
{
	LIST *list = {0};
	PROCESS_PARAMETER *process_parameter;

	if ( list_rewind( process_parameter_list ) )
	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( string_strcmp(
			process_parameter->prompt_name,
			"null" ) != 0 )
		{
			if ( !list ) list = list_new();

			list_set(
				list,
				process_parameter->prompt_name );
		}

	} while ( list_next( process_parameter_list ) );

	return list;
}

LIST *process_parameter_drop_down_prompt_data_in_clause_list(
		const char *process_parameter_drop_down_prompt_data_table,
		LIST *process_parameter_drop_down_prompt_list )
{
	char where[ 512 ];
	char system_string[ 1024 ];

	if ( !list_length( process_parameter_drop_down_prompt_list ) )
		return (LIST *)0;

	snprintf(
		where,
		sizeof ( where ),
		"drop_down_prompt_data in (%s)",
		list_single_quotes_around_string(
			process_parameter_drop_down_prompt_list ) );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh %s %s \"%s\"",
		"drop_down_prompt_data",
		process_parameter_drop_down_prompt_data_table,
		where );

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: system_string=[%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
system_string );
msg( (char *)0, message );
}
	return list_pipe_fetch( system_string );
}

LIST *process_parameter_set_list( char *process_set_name )
{
	char *system_string;

	if ( !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "process_set_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		process_parameter_set_system_string(
			PROCESS_SET_PARAMETER_SELECT,
			PROCESS_SET_PARAMETER_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_set_primary_where( process_set_name )
				/* process_parameter_set_where */ );

	return
	process_parameter_system_list(
		(char *)0 /* application_name */,
		(char *)0 /* login_name */,
		(char *)0 /* role_name */,
		(DICTIONARY *)0 /* drillthru_dictionary */,
		system_string );
}

PROCESS_PARAMETER_YES_NO *process_parameter_yes_no_fetch(
		char *prompt_name )
{
	PROCESS_PARAMETER_YES_NO *process_parameter_yes_no;

	if (	!prompt_name
	||	strcmp( prompt_name, "null" ) == 0 )
	{
		char message[ 128 ];

		sprintf(message, "prompt_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_parameter_yes_no = process_parameter_yes_no_calloc();
	process_parameter_yes_no->prompt_name = prompt_name;

	process_parameter_yes_no->process_parameter_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_parameter_prompt_fetch(
			prompt_name );

	return process_parameter_yes_no;
}

PROCESS_PARAMETER_YES_NO *process_parameter_yes_no_calloc( void )
{
	PROCESS_PARAMETER_YES_NO *process_parameter_yes_no;

	if ( ! ( process_parameter_yes_no =
			calloc( 1,
				sizeof ( PROCESS_PARAMETER_YES_NO ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_parameter_yes_no;
}

