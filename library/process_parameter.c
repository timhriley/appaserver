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
#include "appaserver_parameter_file.h"
#include "security.h"
#include "dictionary.h"
#include "attribute.h"
#include "folder.h"
#include "process_parameter.h"

PROMPT *prompt_fetch( char *prompt_name )
{
	return
	prompt_parse(
		string_fetch_pipe(
			prompt_system_string(
				prompt_primary_where(
					prompt_name ) ) ) );
}

char *prompt_primary_where( char *prompt_name )
{
	static char where[ 256 ];

	sprintf(where,
		"prompt = '%s'",
		prompt_name );

	return where;
}

char *prompt_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\"",
		PROMPT_TABLE,
		where );

	return strdup( system_string );
}

PROMPT *prompt_new( char *prompt_name )
{
	PROMPT *prompt = calloc( 1, sizeof( PROMPT ) );

	if ( !prompt )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( PROMPT ) );
		exit( 1 );
	}

	prompt->prompt_name = prompt_name;
	return prompt;
}

PROMPT *prompt_parse( char *input )
{
	char prompt_name[ 256 ];
	char buffer[ 4096 ];
	PROMPT *prompt;

	/* See attribute_list prompt */
	/* ------------------------- */
	piece( prompt_name, SQL_DELIMITER, input, 0 );
	prompt = prompt_new( strdup( prompt_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	prompt->input_width = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	prompt->hint_message = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	prompt->upload_filename = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 4 );
	prompt->date = ( *buffer == 'y' );

	return prompt;
}

DROP_DOWN_PROMPT_DATA *drop_down_prompt_data_new(
			char *drop_down_prompt_name,
			char *drop_down_prompt_data )
{
	DROP_DOWN_PROMPT_DATA *p = calloc( 1, sizeof( DROP_DOWN_PROMPT_DATA ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( DROP_DOWN_PROMPT_DATA ) );
		exit( 1 );
	}

	p->drop_down_prompt_name = drop_down_prompt_name;
	p->drop_down_prompt_data = drop_down_prompt_data;

	return p;
}

DROP_DOWN_PROMPT_DATA *drop_down_prompt_data_parse(
			char *input )
{
	char prompt_name[ 256 ];
	char prompt_data[ 256 ];
	char display_order[ 128 ];
	DROP_DOWN_PROMPT_DATA *drop_down_prompt_data;

	/* See attribute_list drop_down_prompt_data */
	/* ---------------------------------------- */
	piece( prompt_name, SQL_DELIMITER, input, 0 );
	piece( prompt_data, SQL_DELIMITER, input, 1 );
	piece( display_order, SQL_DELIMITER, input, 3 );

	drop_down_prompt_data =
		drop_down_prompt_data_new(
			strdup( prompt_name ),
			strdup( prompt_data ) );

	drop_down_prompt_data->display_order = atoi( display_order );

	return drop_down_prompt_data;
}

char *drop_down_prompt_data_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\"",
		DROP_DOWN_PROMPT_DATA_TABLE,
		where );

	return strdup( system_string );
}

LIST *drop_down_prompt_data_system_list(
			char *system_string )
{
	LIST *list = {0};
	char input[ 1024 ];
	DROP_DOWN_PROMPT_DATA *drop_down_prompt_data;
	FILE *pipe = popen( system_string, "r" );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( drop_down_prompt_data =
				drop_down_prompt_data_parse(
					input ) ) )
		{
			if ( !list ) list = list_new();

			list_set( list, drop_down_prompt_data );
		}
	}

	pclose( pipe );
	return list;
}

DROP_DOWN_PROMPT *drop_down_prompt_new(
			char *drop_down_prompt_name )
{
	DROP_DOWN_PROMPT *p = calloc( 1, sizeof( DROP_DOWN_PROMPT ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( DROP_DOWN_PROMPT ) );
		exit( 1 );
	}

	p->drop_down_prompt_name = drop_down_prompt_name;
	return p;
}

DROP_DOWN_PROMPT *drop_down_prompt_fetch(
			char *drop_down_prompt_name )
{
	DROP_DOWN_PROMPT *drop_down_prompt;

	drop_down_prompt =
		drop_down_prompt_parse(
			string_fetch_pipe(
				drop_down_prompt_system_string(
					drop_down_prompt_primary_where(
						drop_down_prompt_name ) ) ) );

	drop_down_prompt->drop_down_prompt_data_list =
		drop_down_prompt_data_system_list(
			drop_down_prompt_data_system_string(
				drop_down_prompt_primary_where(
					drop_down_prompt_name ) ) );
	return drop_down_prompt;
}

DROP_DOWN_PROMPT *drop_down_prompt_parse(
			char *input )
{
	char drop_down_prompt_name[ 256 ];
	char buffer[ 2048 ];
	DROP_DOWN_PROMPT *drop_down_prompt;

	/* See attribute_list drop_down_prompt */
	/* ----------------------------------- */
	piece( drop_down_prompt_name, SQL_DELIMITER, input, 0 );

	drop_down_prompt =
		drop_down_prompt_new(
			strdup( drop_down_prompt_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	drop_down_prompt->hint_message = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	drop_down_prompt->optional_display = strdup( buffer );

	return drop_down_prompt;
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

char *drop_down_prompt_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\"",
		DROP_DOWN_PROMPT_TABLE,
		where );

	return strdup( system_string );
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
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" display_order",
		PROCESS_PARAMETER_SELECT,
		PROCESS_PARAMETER_TABLE,
		where );

	return strdup( system_string );
}

char *process_set_parameter_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" display_order",
		PROCESS_SET_PARAMETER_SELECT,
		PROCESS_SET_PARAMETER_TABLE,
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

	process_parameter->role_name = role_name;
	process_parameter->login_name = login_name;
	process_parameter->drillthru_dictionary = drillthru_dictionary;

	piece( buffer, SQL_DELIMITER, input, 5 );
	process_parameter->display_order = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	process_parameter->drop_down_multi_select = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 7 );
	process_parameter->drillthru = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 8 );
	process_parameter->populate_drop_down_process_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	process_parameter->populate_helper_process_name = strdup( buffer );

	if ( strcmp( process_parameter->folder_name, "null" ) != 0 )
	{
		process_parameter->folder =
			folder_primary_data_fetch(
				process_parameter->folder_name,
				process_parameter->role_name,
				process_parameter->login_name,
				process_parameter->drillthru_dictionary,
				process_parameter->
					populate_drop_down_process_name );
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
		process_parameter->drop_down_prompt =
			drop_down_prompt_fetch(
				process_parameter->
					drop_down_prompt_name );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->prompt_name, "null" ) != 0 )
	{
		process_parameter->prompt =
			prompt_fetch(
				process_parameter->
					prompt_name );
		ok_return = 1;
	}

	if ( ok_return )
		return process_parameter;
	else
		return (PROCESS_PARAMETER *)0;
}

PROCESS_PARAMETER *process_parameter_new(
			char *process_or_set_name,
			char *folder_name,
			char *attribute_name,
			char *drop_down_prompt_name,
			char *prompt_name )
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

	process_parameter->process_or_set_name = process_or_set_name;
	process_parameter->folder_name = folder_name;
	process_parameter->attribute_name = attribute_name;
	process_parameter->drop_down_prompt_name = drop_down_prompt_name;
	process_parameter->prompt_name = prompt_name;

	return process_parameter;
}

char *process_parameter_where(
			char *where,
			boolean is_preprompt )
{
	static char parameter_where[ 512 ];

	sprintf(parameter_where,
		"%s and ifnull(drillthru_yn,'n') = '%c'",
		where,
		(is_preprompt) ? 'y' : 'n' );

	return parameter_where;
}
