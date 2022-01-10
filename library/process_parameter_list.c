/* $APPASERVER_HOME/library/process_parameter_list.c			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver process_parameter_list ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "query.h"
#include "application.h"
#include "process_parameter_list.h"
#include "element.h"
#include "attribute.h"
#include "folder.h"
#include "list.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "populate_helper_process.h"

PROCESS_PARAMETER_LIST *process_parameter_list_new( 
				char *login_name,
				char *application_name,
				char *process_name,
				boolean override_row_restrictions,
				boolean is_preprompt,
				DICTIONARY *parameter_dictionary,
				char *role_name,
				char *session,
				boolean with_populate_drop_downs )
{
	PROCESS_PARAMETER_LIST *p;
	PROCESS_PARAMETER *process_parameter;
	PARAMETER_FOLDER *parameter_folder;
	PARAMETER_ATTRIBUTE *parameter_attribute;
	PARAMETER_PROMPT *parameter_prompt;
	PARAMETER_DROP_DOWN_PROMPT *parameter_drop_down_prompt;
	char *parameter_record;
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char prompt[ 128 ];
	char drop_down_prompt[ 128 ];
	char input_width[ 128 ];
	char prompt_hint_message[ 512 ];
	char drop_down_prompt_hint_message[ 512 ];
	char drop_down_prompt_optional_display[ 64 ];
	char drop_down_multi_select_yn[ 8 ];
	char upload_filename_yn[ 8 ];
	char prompt_date_yn[ 8 ];
	char populate_drop_down_process[ 128 ];
	char populate_helper_process_name[ 128 ];
	LIST *parameter_record_list;

	p =	(PROCESS_PARAMETER_LIST *)
		calloc( 1, sizeof( PROCESS_PARAMETER_LIST ) );

	if ( !p )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc failed\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	p->application_name = application_name;
	p->process_name = process_name;
	p->distinct_folder_list = list_new_list();
	p->process_parameter_list = list_new_list();

	parameter_record_list =
		process_parameter_list_process_parameter_record_list(
				&p->is_process_set,
				application_name,
				process_name,
				appaserver_error_get_filename(
					application_name ),
				is_preprompt );

	if ( !list_rewind( parameter_record_list ) ) return p;

	do {
		parameter_record = list_get_pointer( parameter_record_list );

		process_parameter_parse_parameter_record(
				folder_name,
				attribute_name,
				prompt,
				drop_down_prompt,
				input_width,
				prompt_hint_message,
				drop_down_prompt_hint_message,
				drop_down_prompt_optional_display,
				drop_down_multi_select_yn,
				populate_drop_down_process,
				upload_filename_yn,
				prompt_date_yn,
				populate_helper_process_name,
				parameter_record );

		process_parameter = process_parameter_new();

		process_parameter->populate_helper_process =
			strdup( populate_helper_process_name );

		if ( strcmp( folder_name, "null" ) == 0 )
			*folder_name = '\0';

		if ( strcmp( attribute_name, "null" ) == 0 )
			*attribute_name = '\0';

		if ( strcmp( drop_down_prompt, "null" ) == 0 )
			*drop_down_prompt = '\0';

		if ( strcmp( prompt, "null" ) == 0 )
			*prompt = '\0';

		/* If drop_down_prompt */
		/* ------------------- */
		if ( *drop_down_prompt )
		{
			parameter_drop_down_prompt =
				parameter_drop_down_prompt_new(
				application_name,
				strdup( drop_down_prompt ),
				strdup( drop_down_prompt_hint_message ),
				strdup(
				drop_down_prompt_optional_display ),
				drop_down_multi_select_yn,
				with_populate_drop_downs,
				populate_drop_down_process,
				parameter_dictionary );

			process_parameter_set_drop_down_prompt(
				process_parameter,
				parameter_drop_down_prompt );
		}
		else
		/* --------- */
		/* If prompt */
		/* --------- */
		if ( !*folder_name
		&&   !*attribute_name
		&&   !*populate_drop_down_process
		&&   *prompt )
		{
			parameter_prompt =
				parameter_prompt_new(
					prompt,
					atoi( input_width ),
					strdup( prompt_hint_message ),
					*upload_filename_yn,
					*prompt_date_yn );

			process_parameter_set_prompt(
				process_parameter,
				parameter_prompt );
		}
		else
		/* ------------ */
		/* If attribute */
		/* ------------ */
		if ( *attribute_name )
		{
			parameter_attribute =
				parameter_attribute_new(
					application_name,
					strdup( attribute_name ) );
			process_parameter_set_attribute(
				process_parameter,
				parameter_attribute );
		}
		else
		/* --------- */
		/* If folder */
		/* --------- */
		if ( *folder_name )
		{
			if ( *populate_drop_down_process )
			{
				parameter_folder =
				parameter_folder_new(
				     login_name,
				     application_name,
				     session,
				     strdup( folder_name ),
				     strdup( prompt ),
				     drop_down_multi_select_yn,
				     p->distinct_folder_list,
				     override_row_restrictions,
				     parameter_dictionary,
				     role_name,
				     with_populate_drop_downs,
				     strdup( populate_drop_down_process ),
				     process_name );
			}
			else
			{
				parameter_folder =
				parameter_folder_new(
				     login_name,
				     application_name,
				     session,
				     strdup( folder_name ),
				     strdup( prompt ),
				     drop_down_multi_select_yn,
				     p->distinct_folder_list,
				     override_row_restrictions,
				     parameter_dictionary,
				     role_name,
				     with_populate_drop_downs,
				     (char *)0
					/* populate_drop_down_process */,
				     process_name );
			}
	
			process_parameter_set_folder(
					process_parameter,
					parameter_folder );
		}
		else
		/* If prompt with populate_drop_down_process */
		/* ----------------------------------------- */
		if ( *prompt
		&&   *populate_drop_down_process )
		{
			PROCESS *process;

			process =
			process_new_process(
				application_name,
				session, 
				strdup( populate_drop_down_process ),
				(DICTIONARY *)0,
				role_name,
				0 /* not with_check_executable_ok */ );

			if ( !process )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot load process = %s\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 populate_drop_down_process );
				exit( 1 );
			}

			process_parameter->parameter_process =
				parameter_process_new(
					strdup( prompt ),
					process,
				     	*drop_down_multi_select_yn );
			process_parameter->type = "process";
		}
		else
		{
			fprintf( stderr,
"ERROR in %s/%s(): invalid input process parameter record = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 parameter_record );
			exit( 1 );
		}

		list_append_pointer(
				p->process_parameter_list, 
				process_parameter );

	} while( list_next( parameter_record_list ) );

	return p;
}

/* This is a statically built drop down */
/* ------------------------------------ */
LIST *process_parameter_get_drop_down_prompt_element_list(
			PARAMETER_DROP_DOWN_PROMPT *parameter_drop_down_prompt,
			char *post_change_javascript )
{
	LIST *element_list;
	APPASERVER_ELEMENT *element;
	char *element_name;
	char buffer[ 1024 ];
	char *multi_select_element_name = {0};

	if ( parameter_drop_down_prompt->optional_display
	&&   *parameter_drop_down_prompt->optional_display )
	{
		element_name = parameter_drop_down_prompt->optional_display;
	}
	else
	{
		element_name = parameter_drop_down_prompt->drop_down_prompt;
	}

	element_list = list_new();

	/* Make the line break */
	/* ------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	/* Make the prompt */
	/* --------------- */
	element = element_appaserver_new( prompt, element_name );

	list_append_pointer( 	element_list, 
				element );

	/* Make the drop-down element */
	/* -------------------------- */
	if ( parameter_drop_down_prompt->multi_select )
	{
		char element_name_buffer[ 512 ];

		multi_select_element_name = element_name;

		sprintf(	element_name_buffer,
				"%s%s", 
				QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL, 
				element_name );
		element_name = strdup( element_name_buffer );
	}

	element = element_appaserver_new( drop_down, element_name );

	element->drop_down->option_data_list =
		parameter_drop_down_prompt->
			drop_down_prompt_data_list;

	element->drop_down->multi_select =
		parameter_drop_down_prompt->multi_select;

	element->drop_down->multi_select_element_name =
		multi_select_element_name;

	element->drop_down->post_change_javascript =
		post_change_javascript;

	list_append_pointer( 	element_list, 
				element );

	if ( parameter_drop_down_prompt->hint_message
	&&   *parameter_drop_down_prompt->hint_message )
	{
		element = 
			element_appaserver_new( 
				non_edit_text,
				parameter_drop_down_prompt->hint_message );

		list_append_pointer( 	element_list, 
					element );
	}

	/* Create a hidden query relational operator equals */
	/* ------------------------------------------------ */
	sprintf( buffer, 
		 "%s%s",
		 QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 parameter_drop_down_prompt->drop_down_prompt );

	element = element_appaserver_new( hidden, strdup( buffer ) );

	element->hidden->data = EQUAL_OPERATOR;

	list_append_pointer(
		element_list, 
		element );

	return element_list;
}

/* This is a folder without a prompt added */
/* --------------------------------------- */
LIST *process_parameter_get_folder_element_list(
			char *post_change_javascript,
			char *folder_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			boolean multi_select,
			char *populate_helper_process_name,
			char *document_root_directory,
			char *application_name,
			char *session,
			char *login_name )
{
	LIST *element_list;
	APPASERVER_ELEMENT *element;
	char element_name_buffer[ 512 ];
	char *element_name;
	POPULATE_HELPER_PROCESS *
		populate_helper_process = {0};

	element_list = list_new_list();

	/* Set the line break */
	/* ------------------ */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	element_name = folder_name;

	element = element_appaserver_new( prompt, element_name );

	list_append_pointer(
			element_list, 
			element );

	if ( multi_select )
	{
		sprintf(	element_name_buffer,
				"%s%s", 
				QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL, 
				list_display_delimited(
					primary_key_list,
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );
		element_name = strdup( element_name_buffer );
	}
	else
	{
		element_name =
			list_display_delimited(
					primary_key_list,
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
	}

	element = element_appaserver_new( drop_down, element_name );

	if ( multi_select )
	{
		element->drop_down->multi_select_element_name =
			list_display_delimited(
				primary_key_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
	}

	element->drop_down->option_data_list = primary_data_list;

	element->drop_down->multi_select = multi_select;

	element->drop_down->post_change_javascript =
		post_change_javascript;

	element->drop_down->no_initial_capital =
		process_parameter_get_no_initial_capital(
			application_name,
			folder_name );

	list_append_pointer( 	element_list, 
				element );

	/* Create a hidden query relational operator equals */
	/* ------------------------------------------------ */
	element_name =
		 list_display_delimited_prefixed(
				primary_key_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			  	QUERY_RELATION_OPERATOR_STARTING_LABEL );

	element = element_appaserver_new( hidden, element_name );

	element->hidden->data = EQUAL_OPERATOR;

	list_append_pointer(
		element_list, 
		element );

	if ( populate_helper_process_name
	&&   *populate_helper_process_name )
	{
		PROCESS *process;
		char executable[ 1024 ];

		process =
			process_new_process(
				application_name,
				session, 
				populate_helper_process_name,
				(DICTIONARY *) 0 /* dictionary */,
				(char *)0 /* role_name */,
				0 /* not with_check_executable_ok */ );

		if ( !process )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: could not find helper PROCESS = %s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 populate_helper_process_name );

			goto return_element_list;
		}

		process_convert_parameters(
				&process->executable,
			 	application_name,
				session,
				(char *)0 /* state */,
				login_name,
				(char *)0 /* folder */,
				(char *)0 /* role_name */,
				(char *)0 /* target_frame */,
				(DICTIONARY *)0 /* parameter_dictionary */,
				(DICTIONARY *)0 /* where_clause_dictionary */,
				process->attribute_list,
				(LIST *)0 /* prompt_list */,
				(LIST *)0 /* primary_key_list */,
				(LIST *)0 /* primary_data_list */,
				0 /* row */,
				(char *)0 /* process_name */,
				(PROCESS_SET *)0 /* process_set */,
				(char *)0
				/* one2m_folder_name_for_processes */,
				(char *)0 /* operation_row_count_string */,
				(char *)0 /* prompt */ );

		populate_helper_process =
			populate_helper_process_new(
				populate_helper_process_name,
				document_root_directory,
				application_name,
				getpid() );

		strcpy( executable, process->executable );
		sprintf( executable + strlen( executable ),
			 " >%s",
			populate_helper_process->output_filename );
/*
			 populate_helper_process_output_filename(
				populate_helper_process->,
					populate_helper_process_name,
				document_root_directory,
				application_name,
				getpid() ) );
*/

		if ( system( executable ) ) {};

		element =
			element_appaserver_new(
				anchor,
				populate_helper_process->
					populate_helper_process_name );

		element->anchor->prompt =
			populate_helper_process->
				populate_helper_process_name;

		element->anchor->href =
			populate_helper_process->
				prompt;
/*
			populate_helper_process_prompt(
				populate_helper_process->key,
				application_name,
				getpid() );
*/

		list_append_pointer( 	element_list, 
					element );

	}

	return element_list;
}

/* This is a folder with a prompt added. */
/* ------------------------------------- */
APPASERVER_ELEMENT *process_parameter_get_folder_prompt_element(
				char *login_name,
				char *application_name,
				boolean override_row_restrictions,
				boolean multi_select,
				char *folder_name,
				char *prompt_string,
				LIST *primary_data_list,
				char *role_name )
{
	APPASERVER_ELEMENT *element;
	char element_name[ 256 ];

	if ( multi_select )
	{
		sprintf(	element_name, 
				"%s%s", 
				QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL, 
				prompt_string );
	}
	else
	{
		strcpy( element_name, prompt_string );
	}

	element = element_appaserver_new( drop_down, strdup( element_name ) );

	if ( multi_select )
	{
		strcpy( element_name, prompt_string );

		element->drop_down->multi_select_element_name =
			strdup( element_name );
	}

	if ( primary_data_list )
	{
		element->drop_down->option_data_list = primary_data_list;
	}
	else
	{
		element->drop_down->option_data_list =
			process_parameter_get_primary_data_list(
					login_name,
					application_name,
					folder_name,
					(LIST *)0 /* distinct_folder_list */,
					override_row_restrictions,
					role_name,
					(char *)0 /* state */ );
	}

	element->drop_down->multi_select = multi_select;

	element->drop_down->no_initial_capital =
		process_parameter_get_no_initial_capital(
			application_name,
			folder_name );

	return element;
}

APPASERVER_ELEMENT *process_parameter_get_process_set_output_drop_down_element(
					char *process_set_name,
					char *application_name,
					char *role_name )
{
	APPASERVER_ELEMENT *element;
	char element_name[ 256 ];
	LIST *members4set_list;
	LIST *option_data_list;
	LIST *option_label_list;
	char *record;
	char option_data[ 128 ];
	char option_label[ 128 ];

	strcpy( element_name, process_set_name );

	element = element_appaserver_new( drop_down, strdup( element_name ) );

	members4set_list =
			process_parameter_get_process_members4set_list(
				application_name,
				process_set_name,
				role_name );

	if ( !list_rewind( members4set_list ) ) return element;

	option_data_list = list_new_list();
	option_label_list = list_new_list();

	do {
		record = list_get_pointer( members4set_list );

		piece( option_data, '^', record, 0 );
		list_append_pointer(	option_data_list,
					strdup( option_data ) );

		if ( piece( option_label, '^', record, 1 )
		&&   *option_label )
		{
			list_append_pointer(	option_label_list,
						strdup( option_label ) );
		}
		else
		{
			list_append_pointer(	option_label_list,
						strdup( option_data ) );
		}

	} while( list_next( members4set_list ) );

	element->drop_down->option_data_list = option_data_list;
	element->drop_down->option_label_list = option_label_list;

	return element;

}

APPASERVER_ELEMENT *process_parameter_get_attribute_element(
					PROCESS_PARAMETER *process_parameter )
{
	APPASERVER_ELEMENT *element;
	char *element_name;

	element_name = process_parameter->parameter_attribute->attribute_name;

	element = element_appaserver_new( text_item, strdup( element_name ) );

	element_text_item_set_attribute_width( 	element->text_item,
						process_parameter->
						parameter_attribute->
						width );
	return element;
}

void process_parameter_set_prompt(	
				PROCESS_PARAMETER *process_parameter,
				PARAMETER_PROMPT *parameter_prompt )
{
	process_parameter->type = "prompt";
	process_parameter->parameter_prompt = parameter_prompt;
}
void process_parameter_set_attribute( 	
				PROCESS_PARAMETER *process_parameter,
				PARAMETER_ATTRIBUTE *parameter_attribute )
{
	process_parameter->type = "attribute";
	process_parameter->parameter_attribute = parameter_attribute;
}

void process_parameter_set_folder( 	
				PROCESS_PARAMETER *process_parameter,
				PARAMETER_FOLDER *parameter_folder )
{
	process_parameter->type = "folder";
	process_parameter->parameter_folder = parameter_folder;
}

PARAMETER_PROMPT *parameter_prompt_new(	char *prompt,
					int input_width,
					char *hint_message,
					char upload_filename_yn,
					char date_yn )
{
	PARAMETER_PROMPT *p = (PARAMETER_PROMPT *)
				calloc( 1, sizeof( PARAMETER_PROMPT ) );
	p->prompt = strdup( prompt );
	p->width = input_width;
	p->hint_message = hint_message;
	p->upload_filename_yn = upload_filename_yn;
	p->date_yn = date_yn;
	return p;
}

PARAMETER_ATTRIBUTE *parameter_attribute_new(	char *application_name,
						char *attribute_name )
{
	ATTRIBUTE *attribute;
	PARAMETER_ATTRIBUTE *p = (PARAMETER_ATTRIBUTE *)
				calloc( 1, sizeof( PARAMETER_ATTRIBUTE ) );

	p->attribute_name = attribute_name;
	attribute =
		attribute_load_attribute(
			application_name,
			attribute_name );

	p->width = attribute->width;
	p->hint_message = attribute->hint_message;
	return p;
}

PARAMETER_DROP_DOWN_PROMPT *parameter_drop_down_prompt_new(
					char *application_name,
					char *drop_down_prompt,
					char *hint_message,
					char *optional_display,
					char *drop_down_multi_select_yn,
					boolean with_populate_drop_downs,
					char *populate_drop_down_process_string,
					DICTIONARY *parameter_dictionary )
{
	PARAMETER_DROP_DOWN_PROMPT *p = (PARAMETER_DROP_DOWN_PROMPT *)
			calloc( 1, sizeof( PARAMETER_DROP_DOWN_PROMPT ) );

	if ( with_populate_drop_downs )
	{
		p->drop_down_prompt_data_list =
			process_parameter_get_drop_down_prompt_data_list(
				application_name,
				drop_down_prompt,
				populate_drop_down_process_string,
				parameter_dictionary );
	}

	p->drop_down_prompt = drop_down_prompt;
	p->hint_message = hint_message;
	p->optional_display = optional_display;
	p->multi_select = ( *drop_down_multi_select_yn == 'y' );

	return p;
}

LIST *process_parameter_get_drop_down_prompt_data_list(
					char *application_name,
					char *drop_down_prompt,
					char *populate_drop_down_process,
					DICTIONARY *parameter_dictionary )
{
	char sys_string[ 1024 ];

	if ( populate_drop_down_process && *populate_drop_down_process )
	{
		char *command_line;

		command_line =
			process_parameter_command_line(
				application_name,
				populate_drop_down_process,
				parameter_dictionary );

		if ( command_line )
			return pipe2list( command_line );
		else
			return (LIST *)0;
	}
	else
	{
		char where[ 512 ];

		sprintf( where,
			 "drop_down_prompt = '%s'",
			 drop_down_prompt );

		sprintf( sys_string,
		 	"get_folder_data				"
		 	"		application=%s 			"
		 	"		folder=drop_down_prompt_data	"
		 	"		select=drop_down_prompt_data	"
		 	"		where=\"%s\"			"
		 	"		order=display_order		",
			application_name,
			where );
	}

	return pipe2list( sys_string );

}

void process_parameter_set_drop_down_prompt(
					PROCESS_PARAMETER *process_parameter,
					PARAMETER_DROP_DOWN_PROMPT *p )
{
	process_parameter->type = "drop_down_prompt";
	process_parameter->parameter_drop_down_prompt = p;
}

PARAMETER_FOLDER *parameter_folder_new(
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *prompt,
			char *drop_down_multi_select_yn,
			LIST *distinct_folder_list,
			boolean override_row_restrictions,
			DICTIONARY *parameter_dictionary,
			char *role_name,
			boolean with_populate_drop_downs,
			char *populate_drop_down_process_string,
			char *process_name )
{
	PARAMETER_FOLDER *p = (PARAMETER_FOLDER *)
				calloc( 1, sizeof( PARAMETER_FOLDER ) );
	p->folder_name = folder_name;

	if ( with_populate_drop_downs )
	{
		p->primary_data_list =
			process_parameter_get_primary_key_and_data_list(
			&p->primary_key_list,
			login_name,
			application_name,
			session,
			folder_name,
			distinct_folder_list,
			override_row_restrictions,
			parameter_dictionary,
			role_name,
			populate_drop_down_process_string,
			process_name,
			prompt );
	}

	if ( *prompt && ( strcmp( prompt, "null" ) != 0 ) )
		p->prompt = prompt;

	p->multi_select = ( *drop_down_multi_select_yn == 'y' );

	return p;
}

LIST *process_parameter_get_primary_key_and_data_list(
			LIST **primary_key_list,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			LIST *distinct_folder_list,
			boolean override_row_restrictions,
			DICTIONARY *parameter_dictionary,
			char *role_name,
			char *populate_drop_down_process_string,
			char *process_name,
			char *prompt )
{
	FOLDER *folder;
	PROCESS *local_populate_drop_down_process;

	folder = process_parameter_get_and_set_distinct_folder(
			distinct_folder_list,
			application_name,
			folder_name,
			override_row_restrictions,
			role_name );

	folder_convert_date_attributes_to_database_format(
			parameter_dictionary,
			application_name,
			folder->attribute_list );

	*primary_key_list = 
		folder_get_primary_key_list(
			folder->attribute_list );

	if ( populate_drop_down_process_string )
	{
		local_populate_drop_down_process =
			process_new_process(
				application_name,
				session,
				populate_drop_down_process_string,
				parameter_dictionary,
				role_name,
				0 /* not with_check_executable_ok */ );
	}
	else
	{
		local_populate_drop_down_process =
			folder->populate_drop_down_process;
	}

	return folder_get_process_primary_data_list(
			application_name,
			folder_name,
			login_name,
			parameter_dictionary
				/* preprompt_dictionary */,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			local_populate_drop_down_process,
			folder->attribute_list,
			(LIST *)0 /* common_non_primary_key_list */,
			folder->row_level_non_owner_forbid,
			(LIST *)0
			/* preprompt_accounted_for_attribute_name_list */,
			role_name,
			(char *)0 /* state */,
			(char *)0 /* one2m_folder_name_for_processes */,
			process_name,
			prompt );

}

LIST *process_parameter_get_primary_data_list(
			char *login_name,
			char *application_name,
			char *folder_name,
			LIST *distinct_folder_list,
			boolean override_row_restrictions,
			char *role_name,
			char *state )
{
	FOLDER *folder;

	folder = 
		process_parameter_get_and_set_distinct_folder(
			distinct_folder_list,
			application_name,
			folder_name,
			override_row_restrictions,
			role_name );

	return folder_primary_data_list(
			application_name,
			BOGUS_SESSION,
			folder_name,
			login_name,
			(DICTIONARY *)0 /* parameter_dictionary */,
			(DICTIONARY *)0 /* where_clause_dictionary */,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			folder->populate_drop_down_process,
			folder->attribute_list,
			(LIST *)0 /* common_non_primary_key_list */,
			folder->row_level_non_owner_forbid,
			(LIST *)0
			/* preprompt_accounted_for_attribute_name_list */,
			role_name,
			state,
			(char *)0 /* one2m_folder_name_for_processes */,
			(char *)0 /* appaserver_user_foreign_login_name */,
			0 /* not include_root_folder */ );
}

LIST *process_parameter_get_process_members4set_list(
			char *application_name,
			char *process_set,
			char *role_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "process_members4set.sh %s %s %s",
		 application_name,
		 process_set,
		 role_name );
	return pipe2list( sys_string );
}

FOLDER *process_parameter_get_and_set_distinct_folder(
				LIST *distinct_folder_list,
				char *application_name,
				char *folder_name,
				boolean override_row_restrictions,
				char *role_name )
{
	FOLDER *folder;

	if ( distinct_folder_list && list_reset( distinct_folder_list ) )
	{
		do {
			folder = (FOLDER *)list_get( distinct_folder_list );
			if ( strcmp( folder->folder_name, folder_name ) == 0 )
			{
				folder_load(
				&folder->insert_rows_number,
				&folder->lookup_email_output,
				&folder->row_level_non_owner_forbid,
				&folder->row_level_non_owner_view_only,
				&folder->populate_drop_down_process,
				&folder->post_change_process,
				&folder->folder_form,
				&folder->notepad,
				&folder->html_help_file_anchor,
				&folder->post_change_javascript,
				&folder->
					lookup_before_drop_down,
				&folder->data_directory,
				&folder->index_directory,
				&folder->no_initial_capital,
				&folder->subschema_name,
				&folder->create_view_statement,
				folder->application_name,
				folder->session,
				folder->folder_name,
				override_row_restrictions,
				role_name,
				(LIST *)0 /* mto1_related_folder_list */ );

				return folder;
			}
		} while( list_next( distinct_folder_list ) );
	}

	folder = folder_new_folder(	application_name,
					BOGUS_SESSION,
					folder_name );

	folder->attribute_list =
			attribute_get_attribute_list(
				folder->application_name,
				folder->folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				role_name );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->post_change_javascript,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			&folder->create_view_statement,
			folder->application_name,
			folder->session,
			folder->folder_name,
			override_row_restrictions,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	if ( distinct_folder_list )
	{
		list_append_pointer(	distinct_folder_list,
					folder );
	}
	return folder;
}

PROCESS_PARAMETER *process_parameter_new()
{
	PROCESS_PARAMETER *p = (PROCESS_PARAMETER *)
				calloc( 1, sizeof( PROCESS_PARAMETER ) );
	return p;
}

LIST *process_parameter_get_prompt_element_list(
				PROCESS_PARAMETER *process_parameter,
				char *hint_message,
				char upload_filename_yn,
				char date_yn,
				char *post_change_javascript )
{
	LIST *element_list;
	APPASERVER_ELEMENT *element;
	char element_name[ 256 ];

	element_list = list_new();

	/* Start with a line break */
	/* ----------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	/* Create the prompt element */
	/* ------------------------- */
	sprintf( element_name, 
		 "%s",
		 process_parameter->parameter_prompt->prompt );

	element = element_appaserver_new( prompt, strdup( element_name ) );

	list_append_pointer( 	element_list, 
				element );

	/* Create the relational operator element */
	/* -------------------------------------- */
	sprintf( element_name, 
		 "%s%s",
		 QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 process_parameter->parameter_prompt->prompt );

	element = element_appaserver_new( hidden, strdup( element_name ) );

	element->hidden->data = EQUAL_OPERATOR;

	list_append_pointer(
		element_list, 
		element );

	/* Create the from operand (note: no "to" operand) */
	/* ----------------------------------------------- */
	sprintf( element_name, "%s%s", 
		 QUERY_FROM_STARTING_LABEL,
		 process_parameter->parameter_prompt->prompt );

	if ( upload_filename_yn == 'y' )
	{
		element = element_appaserver_new(
				upload_filename,
				strdup( element_name ) );
		element->upload_filename->attribute_width =
		 	process_parameter->parameter_prompt->width;
	}
	else
	if ( process_parameter_list_element_name_boolean(
					element_name ) )
	{
		element =
			element_get_yes_no_element(
				strdup( element_name ),
				(char *)0 /* prepend_folder_name */,
				post_change_javascript,
				0 /* not with_is_null */,
				0 /* not with_not_null */ );
	}
	else
	if ( date_yn == 'y' )
	{
		element =
			element_appaserver_new(
				element_date,
				strdup( element_name ) );

		element_text_item_set_attribute_width(
				element->text_item,
	 			process_parameter->
					parameter_prompt->
						width );

		post_change_javascript =
			attribute_append_post_change_javascript(
				post_change_javascript,
				"validate_date(this)",
				1 /* place_first */ );

		element->text_item->post_change_javascript =
			post_change_javascript;
	}
	else
	{
		element =
			element_appaserver_new(
				text_item,
				strdup( element_name ) );

		element_text_item_set_attribute_width(
				element->text_item,
	 			process_parameter->
					parameter_prompt->
						width );
		element->text_item->post_change_javascript =
			post_change_javascript;
	}

	list_append_pointer( 	element_list,
				element );

	/* Create the hint message */
	/* ----------------------- */
	if ( *hint_message )
	{
		element = 
			element_appaserver_new( 
				non_edit_text,
				hint_message );

		element->non_edit_text->column_span=99;

		list_append_pointer( 	element_list, 
					element );
	}
	return element_list;
}


/* ----------------------- */
/* If folder with a prompt */
/* ----------------------- */
LIST *process_parameter_get_folder_prompt_element_list(
				char *login_name,
				char *application_name,
				boolean override_row_restrictions,
				char *post_change_javascript,
				char *prompt_string,
				boolean multi_select,
				char *folder_name,
				LIST *primary_data_list,
				char *role_name )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new_list();
	char *hint_message;

	/* Start with a line break */
	/* ----------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	/* Create the prompt element */
	/* ------------------------- */
	element = 
		element_appaserver_new(
			prompt,
			prompt_string );

	list_append_pointer( 	element_list, 
				element );

	element = process_parameter_get_folder_prompt_element(
				login_name,
				application_name,
				override_row_restrictions,
				multi_select,
				folder_name,
				prompt_string,
				primary_data_list,
				role_name );

	element->drop_down->post_change_javascript =
		post_change_javascript;

	list_append_pointer( 	element_list,
				element );

	if ( ( hint_message =
			process_parameter_get_prompt_hint_message(
				application_name,
				prompt_string,
				(PROCESS_PARAMETER *)0 ) ) )
	{
		element = 
			element_appaserver_new( 
				non_edit_text,
				hint_message );

		list_append_pointer( 	element_list, 
					element );
	}
	return element_list;
}


LIST *process_parameter_get_attribute_element_list(
				boolean *exists_date_element,
				boolean *exists_time_element,
				char *application_name,
				PROCESS_PARAMETER *process_parameter,
				char *post_change_javascript )
{
	LIST *element_list;
	APPASERVER_ELEMENT *element;
	char element_name[ 128 ];
	ATTRIBUTE *attribute;

	element_list = list_new();

	attribute = 
		attribute_load_attribute(
			application_name,
			process_parameter->
				parameter_attribute->
				attribute_name );

	/* Start with a line break */
	/* ----------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	/* Create the prompt element */
	/* ------------------------- */
	sprintf( element_name, 
		 "%s",
		 process_parameter->
			parameter_attribute->
			attribute_name );

	element = element_appaserver_new( prompt, strdup( element_name ) );

	list_append_pointer( 	element_list, 
				element );

	/* Create the relational operator element */
	/* -------------------------------------- */
	sprintf( element_name, 
		 "%s%s",
		 QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 process_parameter->
			parameter_attribute->
			attribute_name );

	element = element_appaserver_new( drop_down, strdup( element_name ) );

	element->drop_down->option_data_list =
			get_relation_operator_list(
					attribute->datatype );

	list_append_pointer( 	element_list, 
				element );

	/* Create the from operand */
	/* ----------------------- */
	sprintf( element_name, "%s%s", 
		 QUERY_FROM_STARTING_LABEL,
		 process_parameter->parameter_attribute->attribute_name );

	if ( strcmp( attribute->datatype, "date" ) == 0
	||   strcmp( attribute->datatype, "current_date") == 0 )
	{
		element =
			element_appaserver_new(
				element_date,
				strdup( element_name ) );

		post_change_javascript =
			attribute_append_post_change_javascript(
				post_change_javascript,
				"validate_date(this)",
				1 /* place_first */ );

		*exists_date_element = 1;
	}
	else
	if ( strcmp( attribute->datatype, "time" ) == 0
	||   strcmp( attribute->datatype, "current_time") == 0 )
	{
		element =
			element_appaserver_new(
				element_time,
				strdup( element_name ) );

		post_change_javascript =
			attribute_append_post_change_javascript(
				post_change_javascript,
				"validate_time(this)",
				1 /* place_first */ );

		*exists_time_element = 1;
	}
	else
	{
		element = element_appaserver_new(
				text_item,
				strdup( element_name ) );
	}

	element_text_item_set_attribute_width(
				element->text_item,
		 		process_parameter->parameter_attribute->width );

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		element->text_item->post_change_javascript =
			post_change_javascript;
	}

	list_append_pointer( 	element_list, 
				element );

	/* Create the between "and" label */
	/* ------------------------------ */
	element = element_appaserver_new(
			prompt,
			"and" );

	list_append_pointer( 	element_list, 
				element );

	/* Create the to operand */
	/* --------------------- */
	sprintf( element_name, 
		 "%s%s", 
		 QUERY_TO_STARTING_LABEL,
		 process_parameter->parameter_attribute->attribute_name );

	if ( strcmp( attribute->datatype, "date" ) == 0
	||   strcmp( attribute->datatype, "current_date") == 0 )
	{
		element = element_appaserver_new(
				element_date,
				strdup( element_name ) );
	}
	else
	if ( strcmp( attribute->datatype, "time" ) == 0
	||   strcmp( attribute->datatype, "current_time") == 0 )
	{
		element =
			element_appaserver_new(
				element_time,
				strdup( element_name ) );
	}
	else
	{
		element =
			element_appaserver_new(
				text_item,
				strdup( element_name ) );
	}

	element_text_item_set_attribute_width( 	
				element->text_item,
		 		process_parameter->parameter_attribute->width );

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		element->text_item->post_change_javascript =
			post_change_javascript;
	}

	list_append_pointer( 	element_list,
				element );

	/* Create the hint message */
	/* ----------------------- */
	if ( *attribute->hint_message )
	{
		element =
			element_appaserver_new( 
				non_edit_text,
				attribute->hint_message );

		list_append_pointer( 	element_list, 
					element );
	}
	return element_list;
}

LIST *process_parameter_get_process_set_output_element_list(
				char *process_set_name,
				char *application_name,
				char *role_name,
				char *post_change_javascript,
				char *prompt_display_text )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new();

	/* Start with a line break */
	/* ----------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	if ( prompt_display_text
	&&   *prompt_display_text )
	{
		element = element_appaserver_new( prompt, prompt_display_text );
	}
	else
	{
		element = element_appaserver_new( prompt, "process" );
	}

	list_append_pointer( 	element_list, 
				element );

	element = process_parameter_get_process_set_output_drop_down_element(
				process_set_name,
				application_name,
				role_name );

	element->drop_down->post_change_javascript =
		post_change_javascript;

	list_append_pointer( 	element_list,
				element );

	return element_list;
}


char *process_parameter_list_display( PROCESS_PARAMETER_LIST *p )
{
	char buffer[ 4096 ];
	char *buffer_pointer = buffer;
	PROCESS_PARAMETER *process_parameter;
	int first_time = 1;

	if ( !list_rewind( p->process_parameter_list ) ) return "";

	*buffer_pointer = '\0';

	do {
		process_parameter = (PROCESS_PARAMETER *)
					list_get( p->process_parameter_list );
		
		if ( first_time )
		{
			first_time = 0;
			buffer_pointer +=
				sprintf( buffer_pointer,
					 "%s",
					 process_parameter->type );
		}
		else
		{
			buffer_pointer +=
				sprintf( buffer_pointer,
					 ",%s",
					 process_parameter->type );
		}
	} while( list_next( p->process_parameter_list ) );

	return strdup( buffer );
}

/* Output format: folder^attribute^prompt^input_width^hint_message^drop_down_multi_select_yn^populate_drop_down_process^upload_filename_yn^prompt_date_yn^populate_help_process */

LIST *process_parameter_list_process_parameter_record_list(
					boolean *is_process_set,
					char *application_name,
					char *process_name,
					char *error_file,
					boolean is_preprompt )
{
	LIST *return_list;

	if ( !*process_name )
	{
		char msg[ 1024 ];

		sprintf( msg,
			 "Error in %s/%s(): parameter \"process\" is empty.",
			 __FILE__,
			 __FUNCTION__ );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	return_list = process_parameter_get_records4process_list(
				application_name,
				process_name,
				is_preprompt,
				error_file );

	if ( list_length( return_list ) )
	{
		*is_process_set = 0;
		return return_list;
	}

	return_list = process_parameter_get_records4process_set_list(
				application_name,
				process_name,
				is_preprompt,
				error_file );

	if ( list_length( return_list ) ) *is_process_set = 1;
	return return_list;
}

int process_parameter_list_element_name_boolean( char *element_name )
{
	int str_len = strlen( element_name );

	return ( strncmp( element_name + str_len - 3, "_yn", 3 ) == 0 );
}

char *process_parameter_get_prompt_hint_message(
					char *application_name,
					char *prompt,
					PROCESS_PARAMETER *process_parameter )
{
	char sys_string[ 512 ];
	char where_clause[ 128 ];
	char *results = {0};

	if ( process_parameter
	&&   process_parameter->parameter_prompt )
	{
		return process_parameter->parameter_prompt->hint_message;
	}

	if ( !prompt || !*prompt ) return (char *)0;

	sprintf( where_clause, "prompt = '%s'", prompt );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=hint_message		"
		 "			folder=prompt			"
		 "			where=\"%s\"			",
		 application_name,
		 where_clause );

	pipe2string( sys_string );

	if ( results && *results )
		return results;
	else
		return (char *)0;
}

boolean process_parameter_get_prompt_date(
				char *application_name,
				char *prompt,
				PROCESS_PARAMETER *process_parameter )
{
	if ( process_parameter
	&&   process_parameter->parameter_prompt )
	{
		return (process_parameter->parameter_prompt->date_yn == 'y' );
	}
	else
	{
		char sys_string[ 512 ];
		char where_clause[ 128 ];
		char *results;
	
		if ( !prompt || !*prompt ) return 0;
	
		sprintf( where_clause, "prompt = '%s'", prompt );
	
		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=date_yn		"
			 "			folder=prompt		"
			 "			where=\"%s\"		",
			 application_name,
			 where_clause );
	
		results = pipe2string( sys_string );
	
		if ( results && *results )
			return (*results == 'y');
		else
			return 0;
	}
}

PARAMETER_PROCESS *parameter_process_new(
				char *prompt,
				PROCESS *populate_drop_down_process,
				char drop_down_multi_select )
{
	PARAMETER_PROCESS *p = (PARAMETER_PROCESS *)
			calloc( 1, sizeof( PARAMETER_PROCESS ) );

	if ( !p )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc failed\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	p->prompt = prompt;
	p->populate_drop_down_process = populate_drop_down_process;
	p->multi_select = ( drop_down_multi_select == 'y' );

	return p;
}

LIST *process_parameter_get_folder_process_element_list(
				char *login_name,
				char *session,
				char *application_name,
				char *role_name,
				char *prompt_string,
				PROCESS *populate_drop_down_process,
				boolean multi_select,
				char *state )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new_list();

	/* Start with a line break */
	/* ----------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	/* Create the prompt element */
	/* ------------------------- */
	element = element_appaserver_new(
			prompt,
			prompt_string );

	list_append_pointer( 	element_list, 
				element );

	element = process_parameter_get_folder_process_element(
				login_name,
				application_name,
				multi_select,
				prompt_string,
				role_name,
				populate_drop_down_process,
				session,
				state );

	list_append_pointer( 	element_list,
				element );
	return element_list;
}

LIST *process_parameter_get_prompt_process_element_list(
				char *login_name,
				char *session,
				char *application_name,
				char *role_name,
				char *prompt_string,
				PROCESS *populate_drop_down_process,
				boolean multi_select,
				DICTIONARY *parameter_dictionary )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new_list();

	/* Start with a line break */
	/* ----------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer( 	element_list, 
				element );

	/* Create the prompt element */
	/* ------------------------- */
	element = element_appaserver_new(
			prompt,
			prompt_string );

	list_append_pointer( 	element_list, 
				element );

	element = process_parameter_get_prompt_process_element(
				login_name,
				application_name,
				multi_select,
				prompt_string,
				role_name,
				populate_drop_down_process,
				session,
				parameter_dictionary );

	list_append_pointer( 	element_list,
				element );
	return element_list;
}

APPASERVER_ELEMENT *process_parameter_get_folder_process_element(
				char *login_name,
				char *application_name,
				boolean multi_select,
				char *prompt_string,
				char *role_name,
				PROCESS *populate_drop_down_process,
				char *session,
				char *state )
{
	APPASERVER_ELEMENT *element;
	char element_name[ 256 ];

	if ( !populate_drop_down_process )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: populate_drop_down_process is null\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( multi_select )
	{
		sprintf(	element_name, 
				"%s%s", 
				QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL, 
				prompt_string );
	}
	else
	{
		strcpy( element_name, prompt_string );
	}

	element = element_appaserver_new( drop_down, strdup( element_name ) );

	if ( multi_select )
	{
		strcpy( element_name, prompt_string );

		element->drop_down->multi_select_element_name =
			strdup( element_name );
	}

	element->drop_down->option_data_list =
		folder_drop_down_process_list(	
			application_name,
			session,
			(char *)0 /* folder_name */,
			login_name,
			populate_drop_down_process,
			role_name,
			(DICTIONARY *)0 /* parameter_dictionary */,
			(DICTIONARY *)0 /* where_clause_dictionary */,
			state,
			(char *)0
			/* one2m_folder_name_for_processes */,
			(LIST *)0 /* attribute_list */,
			0 /* piece_multi_attribute_data_label_delimiter */,
			(char *)0 /* process_name */,
			(char *)0 /* prompt */ );

	element->drop_down->multi_select = multi_select;
	return element;

}

APPASERVER_ELEMENT *process_parameter_get_prompt_process_element(
				char *login_name,
				char *application_name,
				boolean multi_select,
				char *prompt_string,
				char *role_name,
				PROCESS *populate_drop_down_process,
				char *session,
				DICTIONARY *parameter_dictionary )
{
	APPASERVER_ELEMENT *element;
	char element_name[ 256 ];

	if ( !populate_drop_down_process )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: populate_drop_down_process is null\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( multi_select )
	{
		sprintf(	element_name, 
				"%s%s", 
				QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL, 
				prompt_string );
	}
	else
	{
		strcpy( element_name, prompt_string );
	}

	element = element_appaserver_new( drop_down, strdup( element_name ) );

	if ( multi_select )
	{
		strcpy( element_name, prompt_string );

		element->drop_down->multi_select_element_name =
			strdup( element_name );
	}

	element->drop_down->option_data_list =
		process_parameter_get_prompt_process_list(
					application_name,
					session,
					(char *)0 /* folder_name */,
					login_name,
					populate_drop_down_process,
					role_name,
					parameter_dictionary );

	element->drop_down->multi_select = multi_select;

	return element;
}

LIST *process_parameter_get_prompt_process_list(
				char *application_name,
				char *session,
				char *folder_name,
				char *login_name,
				PROCESS *populate_drop_down_process,
				char *role_name,
				DICTIONARY *parameter_dictionary )
{
	if ( !populate_drop_down_process )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: null pointer for populate_drop_down_process\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	process_convert_parameters(
			&populate_drop_down_process->executable,
			application_name,
			session,
			(char *)0 /* state */,
			login_name,
			folder_name,
			role_name,
			(char *)0 /* target_frame */,
			parameter_dictionary,
			parameter_dictionary /* where_clause_dictionary */,
			(LIST *)0 /* attribute_list */,
			(LIST *)0 /* prompt_list */,
			(LIST *)0 /* primary_key_list */,
			(LIST *)0 /* primary_data_list */,
			0 /* row */,
			(char *)0 /* process_name */,
			(PROCESS_SET *)0 /* process_set */,
			(char *)0 /* one2m_folder_name_for_processes */,
			(char *)0 /* operation_row_count_string */,
			(char *)0 /* prompt */ );

	return process2list( populate_drop_down_process->executable );

}

LIST *process_parameter_list_get_process_folder_name_list(
			char *application_name,
			char *process_name )
{

	PROCESS_PARAMETER_LIST *process_parameter_list;
	PROCESS_PARAMETER *process_parameter;
	LIST *folder_name_list;

	process_parameter_list = 
		process_parameter_list_new(
				(char *)0 /* login_name */,
				application_name,
				process_name,
				0 /* override_row_restrictions */,
				0 /* not is_preprompt */,
				(DICTIONARY *)0,
				(char *)0 /* role_name */,
				(char *)0 /* session */,
				0 /* not with_populate_drop_downs */ );

	if ( !list_rewind( process_parameter_list->process_parameter_list ) )
	{
		return (LIST *)0;
	}

	folder_name_list = list_new();
	do {
		process_parameter =
			list_get_pointer( 
				process_parameter_list->
					process_parameter_list );

		if ( strcmp( process_parameter->type, "folder" ) == 0 )
		{
			list_append_pointer(
					folder_name_list,
					process_parameter->
						parameter_folder->
							folder_name );
		}

	} while( list_next( process_parameter_list->process_parameter_list ) );
	return folder_name_list;
}

void process_parameter_list_dictionary_set_drop_down_prompt_optional_display(
			DICTIONARY *dictionary,
			LIST *process_parameter_list )
{
	PROCESS_PARAMETER *process_parameter;
	PARAMETER_DROP_DOWN_PROMPT *parameter_drop_down_prompt;
	char *drop_down_prompt_data;

	if ( !list_rewind( process_parameter_list ) ) return;

	do {
		process_parameter =
			list_get_pointer( 
				process_parameter_list );

		if ( strcmp(	process_parameter->type,
				"drop_down_prompt" ) != 0 )
		{
			continue;
		}

		parameter_drop_down_prompt =
			process_parameter->
				parameter_drop_down_prompt;

		if ( parameter_drop_down_prompt->optional_display
		&&   *parameter_drop_down_prompt->optional_display )
		{
			if ( ! ( drop_down_prompt_data =
				dictionary_get_index_zero(
					dictionary,
					parameter_drop_down_prompt->
						optional_display ) ) )
			{
				continue;
			}
			dictionary_set_index_zero(
				dictionary,
				parameter_drop_down_prompt->
					drop_down_prompt,
				drop_down_prompt_data );
		}
	} while( list_next( process_parameter_list ) );
}

LIST *process_parameter_list_get_preprompt_folder_name_list(
				char *application_name,
				char *process_name )
{
	LIST *preprompt_folder_name_list = {0};
	LIST *parameter_record_list;
	boolean is_process_set;
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char prompt[ 128 ];
	char drop_down_prompt[ 128 ];
	char input_width[ 128 ];
	char prompt_hint_message[ 512 ];
	char drop_down_prompt_hint_message[ 512 ];
	char drop_down_prompt_optional_display[ 64 ];
	char drop_down_multi_select_yn[ 8 ];
	char upload_filename_yn[ 8 ];
	char prompt_date_yn[ 8 ];
	char populate_drop_down_process[ 128 ];
	char populate_helper_process[ 128 ];
	char *parameter_record;

	parameter_record_list =
		process_parameter_list_process_parameter_record_list(
					&is_process_set,
					application_name,
					process_name,
					appaserver_error_get_filename(
						application_name ),
					1 /* is_preprompt */ );

	if ( !list_reset( parameter_record_list ) )
		return preprompt_folder_name_list;

	preprompt_folder_name_list = list_new();

	do {
		parameter_record = list_get_pointer( parameter_record_list );

/*
{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d: got parameter_record = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
parameter_record );
m( msg );
}
*/
		process_parameter_parse_parameter_record(
				folder_name,
				attribute_name,
				prompt,
				drop_down_prompt,
				input_width,
				prompt_hint_message,
				drop_down_prompt_hint_message,
				drop_down_prompt_optional_display,
				drop_down_multi_select_yn,
				populate_drop_down_process,
				upload_filename_yn,
				prompt_date_yn,
				populate_helper_process,
				parameter_record );

/*
{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d: got preprompt folder_name = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
folder_name );
m( msg );
}
*/
		if (	*folder_name
		&&	strcmp( folder_name, "null" ) != 0 )
		{
			list_append_pointer(
				preprompt_folder_name_list,
				strdup( folder_name ) );
		}

	} while( list_next( parameter_record_list ) );
	return preprompt_folder_name_list;

}

void process_parameter_parse_parameter_record(
				char *folder_name,
				char *attribute_name,
				char *prompt,
				char *drop_down_prompt,
				char *input_width,
				char *prompt_hint_message,
				char *drop_down_prompt_hint_message,
				char *drop_down_prompt_optional_display,
				char *drop_down_multi_select_yn,
				char *populate_drop_down_process,
				char *upload_filename_yn,
				char *prompt_date_yn,
				char *populate_helper_process,
				char *parameter_record )
{

/* format: folder^attribute^prompt^input_width^hint_message^drop_down_multi_select_yn^populate_drop_down_process^upload_filename_yn^prompt_date_yn,populate_helper_process*/

		/* -------------------------------------------------- */
		/* See the output to parameter_records4process.sh     */
		/*			and			      */
		/* See the output to parameter_records4process_set.sh */
		/* -------------------------------------------------- */
		piece( folder_name, '^', parameter_record, 0 );
		piece( attribute_name, '^', parameter_record, 1 );
		piece( prompt, '^', parameter_record, 2 );
		piece( drop_down_prompt, '^', parameter_record, 3 );
		piece( input_width, '^', parameter_record, 4 );
		piece( prompt_hint_message, '^', parameter_record, 5 );
		piece( drop_down_prompt_hint_message, '^', 
			parameter_record, 6 );
		piece( drop_down_prompt_optional_display, '^', 
			parameter_record, 7 );
		piece( drop_down_multi_select_yn, '^',
			parameter_record, 8 );
		piece( populate_drop_down_process, '^',
			parameter_record, 9 );
		piece( upload_filename_yn, '^',
			parameter_record, 10 );
		piece( prompt_date_yn, '^',
			parameter_record, 11 );
		piece( populate_helper_process, '^',
			parameter_record, 12 );

}

LIST *process_parameter_get_records4process_list(
				char *application_name,
				char *process_name,
				boolean is_preprompt,
				char *error_file )
{
	LIST *records4process_list = {0};
	LIST *preprompt_records4process_list = {0};
	char sys_string[ 1024 ];

	if ( is_preprompt )
	{
		if ( preprompt_records4process_list )
			return preprompt_records4process_list;
	
		sprintf( sys_string, 
			 "parameter_records4process.sh %s '%s' %c 2>>%s",
			 application_name,
			 process_name,
			 (is_preprompt) ? 'y' : 'n',
			 error_file );
	
		preprompt_records4process_list = pipe2list( sys_string );
		return preprompt_records4process_list;
	}
	else
	{
		if ( records4process_list ) return records4process_list;
	
		sprintf( sys_string, 
			 "parameter_records4process.sh %s '%s' %c 2>>%s",
			 application_name,
			 process_name,
			 (is_preprompt) ? 'y' : 'n',
			 error_file );

		records4process_list = pipe2list( sys_string );
		return records4process_list;
	}
}

LIST *process_parameter_get_records4process_set_list(
				char *application_name,
				char *process_name,
				boolean is_preprompt,
				char *error_file )
{
	LIST *records4process_set_list = {0};
	LIST *preprompt_records4process_set_list = {0};
	char sys_string[ 1024 ];

	if ( is_preprompt )
	{
		if ( preprompt_records4process_set_list )
			return preprompt_records4process_set_list;
	
		sprintf( sys_string,
			 "parameter_records4process_set.sh %s '%s' %c 2>>%s",
			 application_name,
			 process_name,
			 (is_preprompt) ? 'y' : 'n',
			 error_file );
	
		preprompt_records4process_set_list = pipe2list( sys_string );
		return preprompt_records4process_set_list;
	}
	else
	{
		if ( records4process_set_list )
			return records4process_set_list;
	
		sprintf( sys_string,
			 "parameter_records4process_set.sh %s '%s' %c 2>>%s",
			 application_name,
			 process_name,
			 (is_preprompt) ? 'y' : 'n',
			 error_file );
	
		records4process_set_list = pipe2list( sys_string );
		return records4process_set_list;
	}
}

LIST *process_parameter_get_preprompt_attribute_name_list(
				char *application_name,
				char *process_name,
				char *appaserver_error_file )
{
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char prompt[ 128 ];
	char drop_down_prompt[ 128 ];
	char input_width[ 128 ];
	char prompt_hint_message[ 512 ];
	char drop_down_prompt_hint_message[ 512 ];
	char drop_down_prompt_optional_display[ 64 ];
	char drop_down_multi_select_yn[ 8 ];
	char upload_filename_yn[ 8 ];
	char prompt_date_yn[ 8 ];
	char populate_drop_down_process[ 128 ];
	char populate_helper_process[ 128 ];
	LIST *parameter_record_list;
	boolean is_process_set = {0};
	LIST *preprompt_attribute_name_list = {0};
	char *parameter_record;

	parameter_record_list =
		process_parameter_list_process_parameter_record_list(
					&is_process_set,
					application_name,
					process_name,
					appaserver_error_file,
					1 /* is_preprompt */ );

	if ( !list_rewind( parameter_record_list ) )
		return (LIST *)0;

	do {
		parameter_record = list_get_pointer( parameter_record_list );

		process_parameter_parse_parameter_record(
				folder_name,
				attribute_name,
				prompt,
				drop_down_prompt,
				input_width,
				prompt_hint_message,
				drop_down_prompt_hint_message,
				drop_down_prompt_optional_display,
				drop_down_multi_select_yn,
				populate_drop_down_process,
				upload_filename_yn,
				prompt_date_yn,
				populate_helper_process,
				parameter_record );

		if ( strcmp( attribute_name, "null" ) == 0 )
			*attribute_name = '\0';

		if ( *attribute_name )
		{
			if ( !preprompt_attribute_name_list )
				preprompt_attribute_name_list = list_new();

			list_append_pointer(
				preprompt_attribute_name_list,
				strdup( attribute_name ) );

		}
	} while( list_next( parameter_record_list ) );
	return preprompt_attribute_name_list;
}

boolean process_parameter_get_no_initial_capital(
			char *application_name,
			char *folder_name )
{
	FOLDER *folder;

	folder = folder_new_folder(	application_name,
					BOGUS_SESSION,
					folder_name );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->post_change_javascript,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			&folder->create_view_statement,
			folder->application_name,
			folder->session,
			folder->folder_name,
			0 /* not override_row_restrictions */,
			(char *)0 /* role_name */,
			(LIST *)0 /* mto1_related_folder_list */ );

	return folder->no_initial_capital;
}

/* Returns heap memory. */
/* -------------------- */
char *process_parameter_command_line(
				char *application_name,
				char *populate_drop_down_process_string,
				DICTIONARY *parameter_dictionary )
{
	PROCESS *process;
	char *return_string;

	process = process_new(
			application_name,
			populate_drop_down_process_string
				/* process_name */,
			1 /* with_check_executable_ok */ );

	if ( !process )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: process_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	process_convert_parameters(
			&process->executable,
			application_name,
			(char *)0 /* session */,
			(char *)0 /* state */,
			(char *)0 /* login_name */,
			(char *)0 /* folder_name */,
			(char *)0 /* role_name */,
			(char *)0 /* target_frame */,
			parameter_dictionary,
			(DICTIONARY *)0 /* where_clause_dictionary */,
			(LIST *)0 /* attribute_list */,
			(LIST *)0 /* prompt_list */,
			(LIST *)0 /* primary_key_list */,
			(LIST *)0 /* primary_data_list */,
			0 /* row */,
			(char *)0 /* parameter_process_name */,
			(PROCESS_SET *)0 /* process_set */,
			(char *)0 /* one2m_folder_name_for_processes */,
			(char *)0 /* operation_row_count_string */,
			(char *)0 /* prompt */ );

	return_string = strdup( process->executable );
	process_free( process );
	return return_string;

}

