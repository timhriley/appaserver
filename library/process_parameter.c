/* $APPASERVER_HOME/library/process_parameter.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "list.h"
#include "environ.h"
#include "piece.h"
#include "basename.h"
#include "column.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "security.h"
#include "dictionary.h"
#include "attribute.h"
#include "folder.h"
#include "folder_attribute.h"
#include "query.h"
#include "process_parameter.h"

PROCESS_PARAMETER_PROMPT *process_parameter_prompt_fetch(
			char *prompt_name )
{
	if ( !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	process_parameter_prompt_parse(
		prompt_name,
		string_fetch_pipe(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_parameter_prompt_system_string(
				PROCESS_PARAMETER_PROMPT_SELECT,
				PROCESS_PARAMETER_PROMPT_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_prompt_primary_where(
					prompt_name ) ) ) );
}

char *process_parameter_prompt_primary_where(
			char *prompt_name )
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
	static char system_string[ 128 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		process_parameter_prompt_select,
		process_parameter_prompt_table,
		where );

	return system_string;
}

PROCESS_PARAMETER_PROMPT *process_parameter_prompt_calloc( void )
{
	PROCESS_PARAMETER_PROMPT *process_parameter_prompt;

	if ( ! ( process_parameter_prompt =
			calloc( 1, sizeof( PROCESS_PARAMETER_PROMPT ) ) ) )
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
	process_parameter_prompt->hint_message = strdup( buffer );

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
			sizeof( PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA ) ) ) )
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
		char *process_parameter_drop_down_prompt_primary_where )
{
	return
	process_parameter_drop_down_prompt_data_system_list(
		drop_down_prompt_name,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_parameter_drop_down_prompt_data_system_string(
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_SELECT,
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE,
			process_parameter_drop_down_prompt_primary_where ) );
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

	list = list_new();
	pipe = popen( system_string, "r" );

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
	static char system_string[ 128 ];

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
			sizeof( PROCESS_PARAMETER_DROP_DOWN_PROMPT ) ) ) )
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
			char *drop_down_prompt_name )
{
	char *where;
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
			where,
			string_fetch_pipe(
			      /* --------------------- */
			      /* Returns static memory */
			      /* --------------------- */
			      process_parameter_drop_down_prompt_system_string(
		   		   PROCESS_PARAMETER_DROP_DOWN_PROMPT_SELECT,
		   		   PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE,
		   		   where ) ) );

	process_parameter_drop_down_prompt->data_list =
		process_parameter_drop_down_prompt_data_list(
			drop_down_prompt_name,
			where );

	return process_parameter_drop_down_prompt;
}

PROCESS_PARAMETER_DROP_DOWN_PROMPT *
	process_parameter_drop_down_prompt_parse(
			char *drop_down_prompt_name,
			char *process_parameter_drop_down_prompt_primary_where,
			char *input )
{
	char buffer[ 2048 ];
	PROCESS_PARAMETER_DROP_DOWN_PROMPT *process_parameter_drop_down_prompt;

	if ( !drop_down_prompt_name
	||   !process_parameter_drop_down_prompt_primary_where
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
		process_parameter_drop_down_prompt_new(
			drop_down_prompt_name );

	/* See PROCESS_PARAMETER_DROP_DOWN_PROMPT_SELECT */
	/* --------------------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	process_parameter_drop_down_prompt->hint_message = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	process_parameter_drop_down_prompt->optional_display = strdup( buffer );

	process_parameter_drop_down_prompt->data_list =
		process_parameter_drop_down_prompt_data_list(
		   drop_down_prompt_name,
		   process_parameter_drop_down_prompt_primary_where );

	return process_parameter_drop_down_prompt;
}

char *drop_down_prompt_primary_where(
			char *drop_down_prompt_name )
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
	static char system_string[ 128 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		process_parameter_drop_down_prompt_select,
		process_parameter_drop_down_prompt_table,
		process_parameter_drop_down_prompt_primary_where );

	return system_string;
}

LIST *process_parameter_system_list(
			char *system_string,
			char *role_name,
			char *login_name,
			DICTIONARY *drillthru_dictionary )
{
	LIST *list = {0};
	char input[ 1024 ];
	PROCESS_PARAMETER *process_parameter;
	FILE *pipe = popen( system_string, "r" );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( process_parameter =
				process_parameter_parse(
					input,
					role_name,
					login_name,
					drillthru_dictionary ) ) )
		{
			if ( !list ) list = list_new();

			list_set( list, process_parameter );
		}
	}

	pclose( pipe );
	return list;
}

char *process_parameter_system_string(
			char *process_parameter_select,
			char *process_parameter_table,
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
			char *process_set_parameter_select,
			char *process_set_parameter_table,
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
			char *input,
			char *role_name,
			char *login_name,
			DICTIONARY *drillthru_dictionary )
{
	char process_or_set_name[ 128 ];
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char drop_down_prompt_name[ 128 ];
	char prompt_name[ 128 ];
	char buffer[ 128 ];
	PROCESS_PARAMETER *process_parameter;
	boolean ok_return = 0;

	/* See PROCESS_PARAMETER_SELECT or PROCESS_SET_PARAMETER_SELECT */
	/* ------------------------------------------------------------ */
	piece( process_or_set_name, SQL_DELIMITER, input, 0 );
	piece( folder_name, SQL_DELIMITER, input, 1 );
	piece( attribute_name, SQL_DELIMITER, input, 2 );
	piece( drop_down_prompt_name, SQL_DELIMITER, input, 3 );
	piece( prompt_name, SQL_DELIMITER, input, 4 );

	process_parameter =
		process_parameter_new(
			strdup( process_or_set_name ),
			strdup( folder_name ),
			strdup( attribute_name ),
			strdup( drop_down_prompt_name ),
			strdup( prompt_name ) );

	piece( buffer, SQL_DELIMITER, input, 5 );
	process_parameter->display_order = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	process_parameter->drop_down_multi_select = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 7 );
	process_parameter->drillthru = ( *buffer == 'y' );

	if ( piece( buffer, SQL_DELIMITER, input, 8 ) )
	{
		process_parameter->
			populate_drop_down_process_name =
				strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 9 ) )
	{
		process_parameter->
			populate_helper_process_name =
				strdup( buffer );
	}

	if ( process_parameter->populate_drop_down_process_name )
	{
		process_parameter->process_parameter_drop_down =
			process_parameter_drop_down_process_fetch(
				process_parameter->folder_name,
				process_parameter->prompt_name,
				process_parameter->drop_down_multi_select,
				process_parameter->
					populate_drop_down_process_name );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->folder_name, "null" ) != 0
	&&   !process_parameter->populate_drop_down_process_name )
	{
		process_parameter->process_parameter_drop_down =
			process_parameter_drop_down_folder_fetch(
				login_name,
				role_name,
				drillthru_dictionary,
				process_parameter->folder_name,
				process_parameter->drop_down_multi_select );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->attribute_name, "null" ) != 0 )
	{
		process_parameter->attribute =
			attribute_fetch(
				process_parameter->
					attribute_name );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->drop_down_prompt_name, "null" ) != 0 )
	{
		process_parameter->process_parameter_drop_down_prompt =
			process_parameter_drop_down_prompt_fetch(
				process_parameter->drop_down_prompt_name );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->prompt_name, "null" ) != 0 )
	{
		process_parameter->process_parameter_prompt =
			process_parameter_prompt_fetch(
				process_parameter->
					prompt_name );
		ok_return = 1;
	}

	if ( ok_return )
		return process_parameter;
	else
		return (PROCESS_PARAMETER *)0;
}

PROCESS_PARAMETER *process_parameter_calloc( void )
{
	PROCESS_PARAMETER *process_parameter;

	if ( ! ( process_parameter =
			calloc( 1, sizeof( PROCESS_PARAMETER ) ) ) )
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

PROCESS_PARAMETER *process_parameter_new(
			char *process_or_set_name,
			char *folder_name,
			char *attribute_name,
			char *drop_down_prompt_name,
			char *prompt_name )
{
	PROCESS_PARAMETER *process_parameter = process_parameter_calloc();

	process_parameter->process_or_set_name = process_or_set_name;
	process_parameter->folder_name = folder_name;
	process_parameter->attribute_name = attribute_name;
	process_parameter->drop_down_prompt_name = drop_down_prompt_name;
	process_parameter->prompt_name = prompt_name;

	return process_parameter;
}

char *process_parameter_where(
			char *process_name,
			boolean is_drillthru )
{
	static char parameter_where[ 512 ];

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
			boolean drop_down_multi_select,
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

	process_parameter_drop_down->attribute_name_list =
		process_parameter_drop_down_attribute_name_list(
			folder_name,
			prompt_name );

	if ( !list_length( process_parameter_drop_down->attribute_name_list ) )
	{
		free( process_parameter_drop_down );
		return (PROCESS_PARAMETER_DROP_DOWN *)0;
	}

	process_parameter_drop_down->drop_down_name =
		list_display_delimited(
			process_parameter_drop_down->attribute_name_list,
			'^' );

	if ( ! ( process_parameter_drop_down->process =
			process_fetch(
				populate_drop_down_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source_directory */,
				1 /* check_executable_inside_filesystem */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: process_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			populate_drop_down_process_name );
		exit( 1 );
	}

	process_parameter_drop_down->delimited_list =
		list_pipe_fetch(
			process_parameter_drop_down->
				process->
				command_line );

	process_parameter_drop_down->drop_down_multi_select =
		drop_down_multi_select;

	return process_parameter_drop_down;
}

PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_folder_fetch(
			char *login_name,
			char *role_name,
			DICTIONARY *drillthru_dictionary,
			char *folder_name,
			boolean drop_down_multi_select )
{
	PROCESS_PARAMETER_DROP_DOWN *process_parameter_drop_down;

	if ( !login_name
	||   !role_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_parameter_drop_down = process_parameter_drop_down_calloc();

	process_parameter_drop_down->role =
		role_fetch(
			role_name,
			0 /* not fetch_attribute_exclude_list */ );

	process_parameter_drop_down->fetch_mto1 =
		process_parameter_drop_down_fetch_mto1(
			drillthru_dictionary );

	process_parameter_drop_down->folder =
		folder_fetch(
			folder_name,
			/* Not fetching role_folder_list */
			(char *)0 /* role_name */,
			(LIST *)0 /* role_exclude_lookup_attribute_name_list */,
			/* Also sets folder_attribute_primary_list */
			/* and primary_key_list */
			1 /* fetch_folder_attribute_list */,
			process_parameter_drop_down->fetch_mto1
				/* fetch_relation_mto1_non_isa_list */,
			/* Also sets folder_attribute_append_isa_list */
			0 /* fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* fetch_relation_one2m_recursive_list */,
			0 /* not fetch_process */,
			0 /* not fetch_role_folder_list */,
			1 /* fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

	process_parameter_drop_down->drop_down_name =
		list_display_delimited(
			process_parameter_drop_down->
				folder->
				primary_key_list,
			'^' );

	process_parameter_drop_down->security_entity =
		/* -------------- */
		/* Always returns */
		/* -------------- */
		security_entity_new(
			login_name,
			process_parameter_drop_down->
				folder->
				non_owner_forbid,
			process_parameter_drop_down->
				role->
				override_row_restrictions );

	process_parameter_drop_down->
		query_widget =
			query_widget_fetch(
				folder_name /* widget_folder_name */,
				login_name,
				process_parameter_drop_down->
					folder->
					folder_attribute_list,
				process_parameter_drop_down->
					folder->
					relation_mto1_non_isa_list,
				security_entity_where(
					process_parameter_drop_down->
						security_entity,
					process_parameter_drop_down->
						folder->
						folder_attribute_list ),
				drillthru_dictionary );

	process_parameter_drop_down->delimited_list =
		process_parameter_drop_down->
			query_widget->
			delimited_list;

	process_parameter_drop_down->drop_down_multi_select =
		drop_down_multi_select;

	return process_parameter_drop_down;
}

PROCESS_PARAMETER_DROP_DOWN *
	process_parameter_drop_down_calloc(
			void )
{
	PROCESS_PARAMETER_DROP_DOWN *process_parameter_drop_down;

	if ( ! ( process_parameter_drop_down =
			calloc( 1, sizeof( PROCESS_PARAMETER_DROP_DOWN ) ) ) )
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
			char *folder_name,
			char *prompt_name )
{
	if ( !folder_name && !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( folder_name )
	{
		return
		list_display_delimited(
			folder_attribute_fetch_primary_key_list(
				folder_name ),
			'^' );
	}
	else
	{
		return prompt_name;
	}
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
			DICTIONARY *drillthru_dictionary )
{
	return (boolean)dictionary_length( drillthru_dictionary );
}

LIST *process_parameter_folder_name_list(
			char *process_name,
			char *process_set_name,
			boolean is_drillthru )
{
	LIST *process_parameter_list;
	LIST *folder_name_list = {0};
	PROCESS_PARAMETER *process_parameter;
	char *system_string;

	if ( !process_name && !process_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( process_name )
	{
		system_string =
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

	process_parameter_list =
		process_parameter_system_list(
			system_string,
			(char *)0 /* login_name */,
			(char *)0 /* role_name */,
			(DICTIONARY *)0 /* drillthru_dictionary */ );

	if ( !list_rewind( process_parameter_list ) ) return (LIST *)0;

	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( process_parameter->folder_name
		&&   *process_parameter->folder_name )
		{
			if ( !folder_name_list )
			{
				folder_name_list = list_new();
			}

			list_set(
				folder_name_list,
				process_parameter->folder_name );
		}

	} while ( list_next( process_parameter_list ) );

	return folder_name_list;
}

boolean process_parameter_date_boolean( LIST *process_parameter_list )
{
	PROCESS_PARAMETER *process_parameter;

	if ( !list_rewind( process_parameter_list ) ) return 0;

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

boolean process_parameter_has_drillthru(
			char *process_name,
			char *process_set_name )
{
	char *system_string;

	if ( !process_name && !process_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( process_name )
	{
		system_string =
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
	static char where[ 128 ];

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

