/* $APPASERVER_HOME/library/related_folder.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver related_folder ADT.				*/
/* It is used as an interface to the RELATION folder.			*/ 
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "related_folder.h"
#include "folder.h"
#include "query.h"
#include "element.h"
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "list_usage.h"
#include "application.h"
#include "document.h"
#include "form.h"
#include "role_folder.h"
#include "appaserver_parameter_file.h"

LIST *related_folder_get_primary_data_list(
				char *application_name,
				char *related_folder_name,
				LIST *select_attribute_name_list,
				LIST *where_attribute_name_list,
				LIST *primary_data_list )
{
	char sys_string[ 1024 ];
	LIST *return_list = list_new_list();
	char *lookup_data_delimited_string;
	LIST *lookup_data_string_list;

	if ( !list_rewind( primary_data_list ) )
		return return_list;

	do {
		lookup_data_delimited_string =
 			list_get_string( primary_data_list );

		lookup_data_string_list =
			list_string2list(
				lookup_data_delimited_string,
				FOLDER_DATA_DELIMITER );

		sprintf(sys_string,
			"get_folder_data	application=%s		"
			"			folder=%s		"
			"			select=%s		"
			"			where=\"%s\"		",
			application_name,
			related_folder_name,
			list_display( select_attribute_name_list ),
			list_usage_attribute_data_list2where_clause(
				where_attribute_name_list,
				lookup_data_string_list ) );

		escape_dollar_sign( sys_string );

		list_append_string_list( return_list, pipe2list( sys_string ) );

	} while( list_next( primary_data_list ) );

	return return_list;
}

RELATED_FOLDER *related_folder_new(
					char *application_name,
					char *session,
					char *related_folder_name,
					char *related_attribute_name )
{
	RELATED_FOLDER *related_folder;

	related_folder = related_folder_calloc();

	related_folder->folder = folder_new_folder(
					application_name,
					session,
					related_folder_name );

	related_folder->related_attribute_name = related_attribute_name;
	return related_folder;

} /* related_folder_new() */

RELATED_FOLDER *related_folder_calloc( void )
{
	RELATED_FOLDER *related_folder = (RELATED_FOLDER *)
				calloc( 1, sizeof( RELATED_FOLDER ) );
	if ( !related_folder )
	{
		fprintf(stderr,
			"ERROR: cannot allocate a new related_folder.\n" );
		exit( 1 );
	}

	return related_folder;

} /* related_folder_calloc() */

LIST *related_folder_foreign_attribute_name_list(
			LIST *primary_attribute_name_list,
			char *related_attribute_name,
			LIST *folder_foreign_attribute_name_list )
{
	LIST *return_list = {0};

	if ( list_length( folder_foreign_attribute_name_list ) )
	{
		return_list = folder_foreign_attribute_name_list;
	}
	else
	if ( !list_length( primary_attribute_name_list ) )
	{
		return_list = list_new();
	}
	else
	if ( related_attribute_name
	&&   *related_attribute_name
	&&   strcmp( related_attribute_name, "null" ) != 0 )
	{
		primary_attribute_name_list = 
			string_list_duplicate(
				primary_attribute_name_list );

		list_replace_last_string( 
			primary_attribute_name_list,
			related_attribute_name );

		return_list = primary_attribute_name_list;
	}
	else
	{
		return_list = primary_attribute_name_list;
	}

	return return_list;
}

RELATED_FOLDER *related_folder_attribute_consumes_related_folder(
		LIST **foreign_attribute_name_list,
		LIST *done_attribute_name_list,
		LIST *omit_update_attribute_name_list,
		LIST *mto1_related_folder_list,
		char *attribute_name,
		/* ---------------------------------------------------- */
		/* Don't list_display( include_attribute_name_list );	*/
		/* See output_prompt_insert_form.c			*/
		/* ---------------------------------------------------- */
		LIST *include_attribute_name_list )
{
	RELATED_FOLDER *related_folder;
	LIST *local_foreign_attribute_name_list = {0};
	LIST *subtract_list;

	if ( !list_rewind( mto1_related_folder_list ) )
	{
		return (RELATED_FOLDER *)0;
	}

	do {
		related_folder =
			list_get_pointer(
				mto1_related_folder_list );

		if ( related_folder->ignore_output ) continue;

		if ( related_folder->folder_foreign_attribute_name_list )
		{
			local_foreign_attribute_name_list =
				related_folder->
					folder_foreign_attribute_name_list;

			if ( !list_exists_string(
				local_foreign_attribute_name_list,
				attribute_name ) )
			{
				continue;
			}
		}
		else
		{
			local_foreign_attribute_name_list =
				related_folder_foreign_attribute_name_list(
			   	folder_get_primary_attribute_name_list(
						related_folder->folder->
							attribute_list ),
			   	related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );
		}

		subtract_list =
			list_subtract(
				local_foreign_attribute_name_list,
				omit_update_attribute_name_list );

		if ( ( list_length( local_foreign_attribute_name_list ) > 1 )
		&&  	list_length( subtract_list ) !=
			list_length( local_foreign_attribute_name_list ) )
		{
			continue;
		}

		if ( list_exists_string(
			local_foreign_attribute_name_list,
			attribute_name ) )
		{
			if ( list_length( include_attribute_name_list ) )
			{
				/* See output_prompt_insert_form.c */
				/* ------------------------------- */
				list_push( include_attribute_name_list );

				subtract_list =
					list_subtract(
					     local_foreign_attribute_name_list,
					     include_attribute_name_list );

				/* See output_prompt_insert_form.c */
				/* ------------------------------- */
				list_pop( include_attribute_name_list );

				if ( list_length( subtract_list ) )
					continue;
			}

			if ( done_attribute_name_list )
			{
				list_append_string_list(
					done_attribute_name_list,
					local_foreign_attribute_name_list );
			}

			if ( foreign_attribute_name_list )
			{
				*foreign_attribute_name_list =
					local_foreign_attribute_name_list;
			}

			return related_folder;
		}

	} while( list_next( mto1_related_folder_list ) );

	return (RELATED_FOLDER *)0;
}

LIST *related_folder_drop_down_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			char *folder_name,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *foreign_attribute_name_list,
			boolean omit_drop_down_new_push_button,
			boolean omit_ignore_push_buttons,
			DICTIONARY *preprompt_dictionary,
			char *ignore_or_no_display_push_button_prefix,
			char *ignore_or_no_display_push_button_heading,
			char *post_change_javascript,
			char *hint_message,
			LIST *role_folder_insert_list,
			char *form_name,
			int max_drop_down_size,
			LIST *common_non_primary_attribute_name_list,
			boolean is_primary_attribute,
			boolean row_level_non_owner_view_only,
			boolean row_level_non_owner_forbid,
			char *related_attribute_name,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *state,
			char *one2m_folder_name_for_processes,
			int tab_index,
			boolean set_first_initial_data,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			char *appaserver_user_foreign_login_name,
			boolean prepend_folder_name,
			boolean omit_lookup_before_drop_down )
{
	char buffer[ 256 ];
	char element_name[ 256 ];
	LIST *return_list;
	ELEMENT_APPASERVER *element;
	char relation_operator_equals[ 256 ];
	boolean set_option_data_option_label_list;

	return_list = list_new_list();

	/* Create the line break */
	/* --------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer(
			return_list, 
			element );

	if ( !omit_ignore_push_buttons
	&&   ignore_or_no_display_push_button_prefix
	&&   *ignore_or_no_display_push_button_prefix
	&&   ignore_or_no_display_push_button_heading
	&&   *ignore_or_no_display_push_button_heading )
	{
		sprintf(element_name,
		 	"%s%s",
		 	ignore_or_no_display_push_button_prefix,
		 	list_display_delimited(
			  	foreign_attribute_name_list,
			  	MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );

		element = element_appaserver_new(
				toggle_button, 
				strdup( element_name ) );

		element_toggle_button_set_heading(
			element->toggle_button,
			ignore_or_no_display_push_button_heading );

		list_set(
			return_list, 
			element );
	}

	if ( role_folder_insert_list
	&&   !omit_drop_down_new_push_button )
	{
		ELEMENT_APPASERVER *new_button_element;

		if ( ( new_button_element =
			related_folder_get_new_button_element(
				folder_name,
				role_folder_insert_list,
				form_name ) ) )
		{
			list_append_pointer(	return_list,
						new_button_element );
		}
		else
		{
			list_append_pointer(	return_list,
						element_appaserver_new(
							empty_column,
							"" ) );
		}
	}

	/* Create the prompt element */
	/* ------------------------- */
	if ( is_primary_attribute )
	{
		sprintf( element_name,
		 	 "*%s",
		 	 strdup( folder_name ) );
	}
	else
	{
		sprintf( element_name,
		 	 "%s",
		 	 strdup( folder_name ) );
	}

	if ( related_attribute_name
	&&   *related_attribute_name
	&&   strcmp( related_attribute_name, "null" ) != 0 )
	{
		sprintf( element_name + strlen( element_name ),
			 " (%s)",
			 related_attribute_name );
	}

	element =
		element_appaserver_new(
			prompt,
			strdup( format_initial_capital_not_parens(
					buffer, 
					element_name ) ) );

	list_append_pointer(
			return_list, 
			element );

	/* Create the drop down element */
	/* ---------------------------- */
	if ( prepend_folder_name && folder_name && *folder_name )
	{
		sprintf(element_name, 
		 	"%s.%s",
			folder_name,
		 	list_display_delimited(
			  	foreign_attribute_name_list,
			  	MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));
	}
	else
	{
		sprintf(element_name, 
		 	"%s",
		 	list_display_delimited(
			  	foreign_attribute_name_list,
			  	MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));
	}

	if ( drop_down_multi_select )
	{
		char drop_down_element_name[ 128 ];

		sprintf(	drop_down_element_name,
				"%s%s",
				QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL,
				element_name );

		element = element_appaserver_new(
				drop_down,
				strdup( drop_down_element_name ) );

		element->drop_down->multi_select = 1;
		element->drop_down->multi_select_element_name =
			strdup( element_name );
	}
	else
	{
		element = element_appaserver_new(
				drop_down,
				strdup( element_name ) );

		element->drop_down->output_null_option =
			output_null_option;

		element->drop_down->output_not_null_option =
			output_not_null_option;

		element->drop_down->output_select_option =
			output_select_option;
	}

	element->drop_down->post_change_javascript =
		post_change_javascript;

	element->drop_down->max_drop_down_size = max_drop_down_size;
	element->drop_down->no_initial_capital = no_initial_capital;
	element->tab_index = tab_index;

	list_append_pointer(
			return_list, 
			element );

	set_option_data_option_label_list = 1;

	if ( !drop_down_multi_select
	&&   !omit_lookup_before_drop_down
	&&   ajax_fill_drop_down_related_folder
	&&   !*ajax_fill_drop_down_related_folder
			/* Can only execute this once.		     */
			/* Therefore, only one ajaxed mto1 relation. */ )
	{
		FOLDER *folder;

		folder = folder_with_load_new(
				application_name,
				session,
				folder_name,
				(ROLE *)0 );

		if ( ( *ajax_fill_drop_down_related_folder =
		       related_folder_get_ajax_fill_drop_down_related_folder(
			folder->mto1_related_folder_list ) ) )
		{
			char *onclick_function;

			/* Create the fill button element */
			/* ------------------------------ */
			element =
				element_appaserver_new(
					push_button, 
					(char *)0 /* element_name */ );

			element->push_button->label =
				RELATED_FOLDER_AJAX_FILL_LABEL;

			onclick_function =
				related_folder_get_ajax_onclick_function(
					foreign_attribute_name_list );

			element->push_button->onclick_function =
				onclick_function;

			list_append_pointer(
				return_list, 
				element );

			set_option_data_option_label_list = 0;
		}
	}

	if ( set_option_data_option_label_list )
	{
		element_drop_down_set_option_data_option_label_list(
			&element->drop_down->option_data_list,
			&element->drop_down->option_label_list,
			folder_related_primary_data_list(
				application_name,
				session,
				folder_name,
				login_name,
				preprompt_dictionary
					/* parameter_dictionary */,
				preprompt_dictionary
					/* where_clause_dictionary */,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
				populate_drop_down_process,
				attribute_list,
				common_non_primary_attribute_name_list,
				( row_level_non_owner_view_only ||
				  row_level_non_owner_forbid )
					/* filter_only_login_name */,
				role_name,
				state,
				one2m_folder_name_for_processes,
				appaserver_user_foreign_login_name ) );
	}

	if ( set_first_initial_data
	&&   list_length( element->drop_down->option_data_list ) )
	{
		element->drop_down->initial_data =
			list_get_first_pointer(
				element->drop_down->option_data_list );
	}

	/* Create the hint message */
	/* ----------------------- */
	if ( hint_message && *hint_message )
	{
		element = 
			element_appaserver_new(
				non_edit_text,
				hint_message );

		list_append_pointer(
				return_list, 
				element );
	}

	/* Create the hidden equals operator */
	/* --------------------------------- */
	sprintf( relation_operator_equals,
		 "%s%s",
		 QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 element_name );

	element =
		element_appaserver_new(
			hidden,
			strdup( relation_operator_equals ) );

	element->hidden->data = "equals";

	list_set(
		return_list, 
		element );

	return return_list;
}

void related_folder_set_ignore_output_for_duplicate(
					LIST *related_folder_list )
{
	LIST *folder_attribute_name_list;
	RELATED_FOLDER *related_folder;
	char folder_attribute_name[ 128 ];

	if ( !list_rewind( related_folder_list ) ) return;

	folder_attribute_name_list = list_new();

	do {
		related_folder = list_get_pointer( related_folder_list );

		sprintf(folder_attribute_name,
			"%s^%s",
			related_folder->folder->folder_name,
			(related_folder->related_attribute_name)
				? related_folder->related_attribute_name
				: "" );

		if ( list_exists_string( folder_attribute_name_list,
					 folder_attribute_name ) )
		{
			related_folder->ignore_output = 1;
		}
		else
		{
			list_append_pointer(
					folder_attribute_name_list,
					strdup( folder_attribute_name ) );
		}
	} while( list_next( related_folder_list ) );

	list_free_string_list( folder_attribute_name_list );

} /* related_folder_set_ignore_output_for_duplicate() */

LIST *related_folder_subtract_related_attribute_name_list(
					LIST *foreign_attribute_name_list,
					LIST *mto1_related_folder_list )
{
	LIST *attribute_name_list = list_new_list();
	char *attribute_name;
	RELATED_FOLDER *related_folder;
	LIST *primary_attribute_name_list = list_new_list();

	if ( !list_rewind( foreign_attribute_name_list ) )
		return attribute_name_list;

	if ( !mto1_related_folder_list
	||   !list_rewind( mto1_related_folder_list ) )
	{
		return foreign_attribute_name_list;
	}

	do {
		related_folder =
			(RELATED_FOLDER *)
				list_get_pointer(
					mto1_related_folder_list );

		list_append_list(
			primary_attribute_name_list,
			folder_get_primary_attribute_name_list(
				related_folder->folder->
					attribute_list ) );

	} while( list_next( mto1_related_folder_list ) );

	do {
		attribute_name =
			list_get_pointer( foreign_attribute_name_list );

		if ( !list_exists_string(
			primary_attribute_name_list,
			attribute_name ) )
		{
			list_append_pointer(
					attribute_name_list,
					attribute_name );
		}
	} while( list_next( foreign_attribute_name_list ) );
	return attribute_name_list;
}

char *related_folder_list_display(	LIST *related_folder_list,
					enum relation_type relation_type,
					char delimiter )
{
	RELATED_FOLDER *related_folder;
	int first_time = 1;
	char buffer[ 65536 ], *buf_ptr = buffer;

	if ( !list_length( related_folder_list ) ) return "";

	list_push( related_folder_list );

	*buf_ptr = '\0';

	if ( list_rewind( related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					related_folder_list );

			if ( first_time )
				first_time = 0;
			else
				buf_ptr += sprintf( buf_ptr, "%c", delimiter );

			buf_ptr += sprintf(	buf_ptr,
						"%s",
						related_folder_display(
							related_folder,
							relation_type ) );

		} while( list_next( related_folder_list ) );
	}

	list_pop( related_folder_list );

	return strdup( buffer );
}

char *related_folder_display(	RELATED_FOLDER *related_folder,
				enum relation_type relation_type )
{
	char buffer[ 4096 ];
	char *buf_ptr = buffer;
	FOLDER *folder;

	*buf_ptr = '\0';

	if ( relation_type == one2m )
	{
		folder = related_folder->one2m_folder;
	}
	else
	{
		folder = related_folder->folder;
	}

	buf_ptr +=
		sprintf(
		buf_ptr,
"\n%s (%s): isa = %d, ignore = %d, pair_1tom_order = %d, recursive = %d, lookup_before_drop_down = %d, join_1tom_each_row = %d, omit_lookup_before_drop_down = %d, ajax_fill_drop_down = %d, drop_down_multi_select = %d, recursive_level = %d, row_level_non_owner_view_only = %d, row_level_non_owner_forbid = %d, insert_permission = %d, update_permission = %d, lookup_permission = %d, folder_foreign_attribute_name_list = %s",
			folder->folder_name,
			(related_folder->related_attribute_name)
				? related_folder->related_attribute_name
				: "null",
			related_folder->relation_type_isa,
			related_folder->ignore_output,
			related_folder->pair_1tom_order,
			related_folder->prompt_mto1_recursive,
			folder->lookup_before_drop_down,
			related_folder->join_1tom_each_row,
			related_folder->omit_lookup_before_drop_down,
			related_folder->ajax_fill_drop_down,
			related_folder->drop_down_multi_select,
			related_folder->recursive_level,
			folder->row_level_non_owner_view_only,
			folder->row_level_non_owner_forbid,
			folder->insert_permission,
			folder->update_permission,
			folder->lookup_permission,
			(related_folder->folder_foreign_attribute_name_list)
				? list_display(
					related_folder->
					    folder_foreign_attribute_name_list )
				: "null" );

	if ( list_length(
		folder->
		primary_attribute_name_list ) )
	{
		buf_ptr +=
		sprintf( buf_ptr,
			 ", primary = (%s)",
			 list_display(
			 	folder->
			 	primary_attribute_name_list ) );
	}

	if ( list_length(
		related_folder->
		foreign_attribute_name_list ) )
	{
		buf_ptr +=
		sprintf( buf_ptr,
			 ", foreign = (%s)",
			 list_display(
			 	related_folder->
			 	foreign_attribute_name_list ) );
	}

	if ( list_length(
		related_folder->
		parent_primary_attribute_name_list ) )
	{
		buf_ptr +=
		sprintf( buf_ptr,
			 ", parent_primary_attribute_name_list = (%s)",
			 list_display(
			 	related_folder->
			 	parent_primary_attribute_name_list ) );
	}

	if ( dictionary_length(
		folder->
		primary_data_dictionary ) )
	{
		buf_ptr +=
			sprintf(
			buf_ptr,
			", primary_data_dictionary = (%s)",
			dictionary_display(
				folder->primary_data_dictionary ) );
	}

	if ( list_length( folder->mto1_related_folder_list ) )
	{
		buf_ptr +=
			sprintf(
			buf_ptr,
			", length( mto1_related_folder_list ) = %d",
			list_length(
				folder->
				mto1_related_folder_list ) );
/*
		buf_ptr +=
			sprintf(
			buf_ptr,
			", mto1_related_folder_list = (%s)",
			related_folder_list_display(
				folder->mto1_related_folder_list, mto1, ';' ) );
*/
	}

	return strdup( buffer );
}

LIST *related_folder_remove_duplicate_mto1_related_folder_list(
			LIST *related_folder_list )
{
	LIST *folder_name_list = list_new_list();
	LIST *new_related_folder_list = list_new_list();
	RELATED_FOLDER *related_folder;

	if ( list_rewind( related_folder_list ) )
	{
		do {
			related_folder =
				(RELATED_FOLDER *)
					list_get_pointer(
						related_folder_list );
			if ( !related_folder->related_attribute_name
			&&   list_exists_string(
					folder_name_list,
					related_folder->folder->folder_name ) )
			{
				continue;
			}
			list_append_pointer(
					new_related_folder_list,
					related_folder );
			list_append_pointer(
					folder_name_list,
					related_folder->folder->folder_name );
		} while( list_next( related_folder_list ) );
	}

	return new_related_folder_list;
} /* related_folder_remove_duplicate_mto1_related_folder_list() */

LIST *related_folder_get_insert_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			/* --------------------------- */
			/* sets related_folder->folder */
			/* --------------------------- */
			RELATED_FOLDER *related_folder,
			char *application_name,
			char *session,
			char *login_name,
			LIST *foreign_attribute_name_list,
			int row_dictionary_list_length,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			boolean prompt_data_element_only,
			char *post_change_javascript,
			int max_drop_down_size,
			boolean row_level_non_owner_forbid,
			boolean override_row_restrictions,
			boolean is_primary_attribute,
			char *role_name,
			int max_query_rows_for_drop_downs,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *one2m_folder_name_for_processes )
{
	LIST *element_list = list_new();
	ELEMENT_APPASERVER *element;
	char element_heading[ 128 ];
	char primary_attribute_asteric[ 2 ] = {0};

	if ( ajax_fill_drop_down_related_folder )
	{
		*ajax_fill_drop_down_related_folder = (RELATED_FOLDER *)0;
	}

	if ( is_primary_attribute )
		*primary_attribute_asteric = '*';
	else
		*primary_attribute_asteric = '\0';

	if ( related_folder->related_attribute_name
	&&   *related_folder->related_attribute_name
	&&   strcmp(	related_folder->related_attribute_name,
				"null" ) != 0 )
	{
		sprintf( element_heading,
			 "%s%s (%s)",
			 primary_attribute_asteric,
			 related_folder->folder->folder_name,
			 related_folder->related_attribute_name );
	}
	else
	{
		sprintf( element_heading,
			 "%s%s",
			 primary_attribute_asteric,
			 related_folder->folder->folder_name );
	}

	if ( row_dictionary_list_length > max_query_rows_for_drop_downs
	||   related_folder->automatic_preselection )
	{
		prompt_data_element_only = 1;
	}

	if ( prompt_data_element_only )
	{
		element = element_appaserver_new(
				prompt_data,
				list_display_delimited(
					foreign_attribute_name_list,
			  	MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );

		element->prompt_data->heading = strdup( element_heading );

		if ( related_folder->automatic_preselection )
		{
			element->prompt_data->format_initial_capital = 1;
		}

		list_append_pointer( element_list, element );
	
		/* ------------------------- */
		/* Create a hidden element   */
		/* so delete will work.      */
		/* ------------------------- */
		element = element_appaserver_new(
				hidden,
				list_display_delimited(
					foreign_attribute_name_list,
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );

		list_append_pointer(
				element_list, 
				element );

		return element_list;

	} /* if prompt_data_element_only */

	/* Create the drop down element */
	/* ---------------------------- */
	element =
		element_appaserver_new(
			drop_down, 
			list_display_delimited(
				foreign_attribute_name_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );

	element->drop_down->post_change_javascript =
		post_change_javascript;

	element->drop_down->max_drop_down_size =
		max_drop_down_size;

	element->drop_down->multi_select =
		drop_down_multi_select;

	element->drop_down->no_initial_capital =
		no_initial_capital;

	element->drop_down->heading = strdup( element_heading );

	element->drop_down->state = "insert";

	folder_load(
		&related_folder->folder->insert_rows_number,
		&related_folder->folder->lookup_email_output,
		&related_folder->folder->row_level_non_owner_forbid,
		&related_folder->folder->row_level_non_owner_view_only,
		&related_folder->folder->populate_drop_down_process,
		&related_folder->folder->post_change_process,
		&related_folder->folder->folder_form,
		&related_folder->folder->notepad,
		&related_folder->folder->html_help_file_anchor,
		&related_folder->folder->
			post_change_javascript,
		&related_folder->folder->lookup_before_drop_down,
		&related_folder->folder->data_directory,
		&related_folder->folder->index_directory,
		&related_folder->folder->no_initial_capital,
		&related_folder->folder->subschema_name,
		&related_folder->folder->create_view_statement,
		related_folder->folder->application_name,
		related_folder->folder->session,
		related_folder->folder->folder_name,
		override_row_restrictions,
		role_name,
		(LIST *)0 /* mto1_related_folder_list */ );

	related_folder->folder->mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			(char *)0 /* session */,
			related_folder->folder->folder_name,
			(char *)0 /*role_name */,
			0 /* isa_flag */,
			related_folder_no_recursive,
			1 /* override_row_restrictions */,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	list_append_pointer(
			element_list, 
			element );

	if ( !related_folder->omit_lookup_before_drop_down
	&&   ajax_fill_drop_down_related_folder
	&& ( *ajax_fill_drop_down_related_folder =
		    related_folder_get_ajax_fill_drop_down_related_folder(
			related_folder->folder->mto1_related_folder_list ) ) )
	{
		char *onclick_function;

		/* Create the fill button element */
		/* ------------------------------ */
		element =
			element_appaserver_new(
				push_button, 
				(char *)0 /* element_name */ );

		element->push_button->label =
			RELATED_FOLDER_AJAX_FILL_LABEL;

		onclick_function =
			related_folder_get_ajax_onclick_function(
				related_folder->
					folder->
					primary_attribute_name_list );

		element->push_button->onclick_function =
			onclick_function;

		list_append_pointer(
			element_list, 
			element );
	}
	else
	{
		element_drop_down_set_option_data_option_label_list(
			&element->drop_down->option_data_list,
			&element->drop_down->option_label_list,
			folder_primary_data_list(
				application_name,
				session,
				related_folder->folder->folder_name,
				login_name,
				parameter_dictionary,
				where_clause_dictionary,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
				related_folder->
					folder->
					populate_drop_down_process,
				related_folder->
					folder->
					attribute_list,
				related_folder->
					common_non_primary_attribute_name_list,
				row_level_non_owner_forbid
					/* filter_only_login_name */,
				(LIST *)0
				/* exclude_attribute_name_list */,
				role_name,
				"insert" /* state */,
				one2m_folder_name_for_processes,
				(char *)0
				       /* appaserver_user_foreign_login_name */,
				1 /* include_root_folder */ ) );
	}

	return element_list;
}

LIST *related_folder_update_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *login_name,
			RELATED_FOLDER *related_folder,
			LIST *foreign_attribute_name_list,
			char update_yn,
			int row_dictionary_list_length,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			char *state,
			boolean prompt_data_element_only,
			char *post_change_javascript,
			int max_drop_down_size,
			boolean row_level_non_owner_forbid,
			boolean override_row_restrictions,
			boolean is_primary_attribute,
			char *role_name,
			int max_query_rows_for_drop_downs,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *one2m_folder_name_for_processes,
			boolean omit_lookup_before_drop_down )
{
	LIST *element_list = list_new();
	ELEMENT_APPASERVER *element;
	char element_heading[ 128 ];
	DICTIONARY *local_preprompt_dictionary = {0};
	DICTIONARY *parameter_dictionary;
	char primary_attribute_asteric[ 2 ] = {0};
	LIST *primary_attribute_name_list;
	boolean set_option_data_option_label_list;

	if ( ajax_fill_drop_down_related_folder )
	{
		*ajax_fill_drop_down_related_folder = (RELATED_FOLDER *)0;
	}

	if (  update_yn != 'y' )
	{
		prompt_data_element_only = 1;
	}

	if ( is_primary_attribute )
		*primary_attribute_asteric = '*';
	else
		*primary_attribute_asteric = '\0';

	if ( list_length( related_folder->folder_foreign_attribute_name_list ) )
	{
		local_preprompt_dictionary = (DICTIONARY *)0;
	}
	else
	{
		local_preprompt_dictionary =
			dictionary_copy_dictionary( preprompt_dictionary );
	}

	if ( related_folder->related_attribute_name
	&&   *related_folder->related_attribute_name
	&&   strcmp(	related_folder->related_attribute_name,
				"null" ) != 0 )
	{
		sprintf( element_heading,
			 "%s%s (%s)",
			 primary_attribute_asteric,
			 related_folder->folder->folder_name,
			 related_folder->related_attribute_name );
	}
	else
	{
		sprintf( element_heading,
			 "%s%s",
			 primary_attribute_asteric,
			 related_folder->folder->folder_name );
	}

	if ( row_dictionary_list_length > max_query_rows_for_drop_downs )
		prompt_data_element_only = 1;

	if ( prompt_data_element_only )
	{
		element =
			element_appaserver_new( 
				prompt_data,
				list_display_delimited(
			  	   foreign_attribute_name_list,
			  	   MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );

		element->prompt_data->heading = strdup( element_heading );

		if ( related_folder->automatic_preselection )
			element->prompt_data->format_initial_capital = 1;

		list_append_pointer(
				element_list, 
				element );
	
		/* ------------------------- */
		/* Create a hidden element   */
		/* so delete will work.      */
		/* ------------------------- */
		element = element_appaserver_new( 
			hidden,
			list_display_delimited(
			  foreign_attribute_name_list,
			  MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));

		list_append_pointer(
				element_list, 
				element );

		return element_list;

	} /* if prompt_data_element_only */

	/* Create the drop down element */
	/* ---------------------------- */
	element =
		element_appaserver_new(
			drop_down, 
			list_display_delimited(
		  	foreign_attribute_name_list,
		  	MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));

	element->drop_down->post_change_javascript =
		post_change_javascript;

	element->drop_down->max_drop_down_size =
		max_drop_down_size;

	element->drop_down->multi_select =
		drop_down_multi_select;

	element->drop_down->no_initial_capital =
		no_initial_capital;

	element->drop_down->heading = strdup( element_heading );

	element->drop_down->state = state;

	if ( !is_primary_attribute )
		element->drop_down->output_null_option = 1;

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			related_folder->folder->attribute_list );

	/* If single primary key, then don't filter anything. */
	/* -------------------------------------------------- */
	if ( list_length( primary_attribute_name_list ) == 1
	&&   local_preprompt_dictionary )
	{
		char *primary_attribute_name;
		char tmp[ 512 ];

		primary_attribute_name =
			list_get_first_pointer(
				primary_attribute_name_list );

		dictionary_remove_key(	local_preprompt_dictionary,
					primary_attribute_name );

		sprintf( tmp, "%s_0", primary_attribute_name );
		dictionary_remove_key(	local_preprompt_dictionary,
					tmp );
	}

	folder_load(
		&related_folder->folder->insert_rows_number,
		&related_folder->folder->lookup_email_output,
		&related_folder->folder->row_level_non_owner_forbid,
		&related_folder->folder->row_level_non_owner_view_only,
		&related_folder->folder->populate_drop_down_process,
		&related_folder->folder->post_change_process,
		&related_folder->folder->folder_form,
		&related_folder->folder->notepad,
		&related_folder->folder->html_help_file_anchor,
		&related_folder->folder->
			post_change_javascript,
		&related_folder->folder->lookup_before_drop_down,
		&related_folder->folder->data_directory,
		&related_folder->folder->index_directory,
		&related_folder->folder->no_initial_capital,
		&related_folder->folder->subschema_name,
		&related_folder->folder->create_view_statement,
		related_folder->folder->application_name,
		related_folder->folder->session,
		related_folder->folder->folder_name,
		override_row_restrictions,
		role_name,
		(LIST *)0 /* mto1_related_folder_list */ );

	parameter_dictionary = dictionary_copy( local_preprompt_dictionary );

	dictionary_append_dictionary(	parameter_dictionary,
					query_dictionary );

	set_option_data_option_label_list = 1;

	list_append_pointer(
			element_list, 
			element );

	if ( omit_lookup_before_drop_down
	&&   ajax_fill_drop_down_related_folder )
	{
		FOLDER *folder;

		folder = folder_with_load_new(
				application_name,
				session,
				related_folder->folder->folder_name,
				(ROLE *)0 );

		if ( ( *ajax_fill_drop_down_related_folder =
		       related_folder_get_ajax_fill_drop_down_related_folder(
			folder->mto1_related_folder_list ) ) )
		{
			char *onclick_function;

			/* Create the fill button element */
			/* ------------------------------ */
			element =
				element_appaserver_new(
					push_button, 
					(char *)0 /* element_name */ );

			element->push_button->label =
				RELATED_FOLDER_AJAX_FILL_LABEL;

			onclick_function =
				related_folder_get_ajax_onclick_function(
					foreign_attribute_name_list );

			element->push_button->onclick_function =
				onclick_function;

			list_append_pointer(
				element_list, 
				element );

			set_option_data_option_label_list = 0;
		}
	}

	if ( set_option_data_option_label_list )
	{
		element_drop_down_set_option_data_option_label_list(
			&element->drop_down->option_data_list,
			&element->drop_down->option_label_list,
			folder_primary_data_list(
				application_name,
				session,
				related_folder->folder->folder_name,
				login_name,
				parameter_dictionary,
				local_preprompt_dictionary
					/* where_clause_dictionary */,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
				related_folder->
					folder->
					populate_drop_down_process,
				related_folder->
					folder->
					attribute_list,
				related_folder->
				common_non_primary_attribute_name_list,
				row_level_non_owner_forbid
					/* filter_only_login_name */,
				(LIST *)0
					/* exclude_attribute_name_list */,
				role_name,
				state,
				one2m_folder_name_for_processes,
				(char *)0
				     /* appaserver_user_foreign_login_name */,
				1 /* include_root_folder */ ) );
	}

	return element_list;
}

LIST *related_folder_get_edit_lookup_element_list(
				LIST *foreign_attribute_name_list )
{
	ELEMENT_APPASERVER *element;
	LIST *element_list = list_new();

	element =
		element_appaserver_new( 
			prompt_data,
			list_display_delimited(
				foreign_attribute_name_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );

	list_append_pointer(
			element_list, 
			element );

	/* ------------------------- */
	/* Create a hidden element   */
	/* so delete will work.      */
	/* ------------------------- */
	element = element_appaserver_new( 
		hidden,
		list_display_delimited(
		  foreign_attribute_name_list,
		  MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));

	list_append_pointer(
			element_list, 
			element );
	return element_list;
}

LIST *related_folder_get_mto1_related_folder_list(
		LIST *related_folder_list,
		char *application_name,
		char *session,
		char *folder_name,
		char *role_name,
		boolean isa_flag,
		enum related_folder_recursive_request_type
			recursive_request_type,
		boolean override_row_restrictions,
		LIST *root_primary_attribute_name_list,
		int recursive_level )
{
	RELATED_FOLDER *related_folder;
	LIST *local_related_folder_list;

	if ( !related_folder_list ) related_folder_list = list_new_list();

	local_related_folder_list =
		related_folder_get_related_folder_list(
			application_name,
			session,
			folder_name,
			mto1,
			related_folder_list
				/* existing_related_folder_list */ );

	if (!list_rewind( local_related_folder_list ) )
	{
		return related_folder_list;
	}

	do {
		related_folder = list_get_pointer( local_related_folder_list );

		if ( folder_name
		&&   strcmp(	related_folder->
					one2m_folder->
					folder_name,
				folder_name ) != 0 )
		{
			continue;
		}

		related_folder->recursive_level = recursive_level;

		if ( !related_folder->folder->attribute_list )
		{
			related_folder->folder->attribute_list =
			attribute_get_attribute_list(
				related_folder->folder->application_name,
				related_folder->folder->folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				role_name );

			related_folder->folder->primary_attribute_name_list =
				attribute_get_primary_attribute_name_list(
					related_folder->
						folder->
						attribute_list );
		}

		folder_load(
			&related_folder->folder->insert_rows_number,
			&related_folder->folder->lookup_email_output,
			&related_folder->folder->row_level_non_owner_forbid,
			&related_folder->folder->row_level_non_owner_view_only,
			&related_folder->folder->populate_drop_down_process,
			&related_folder->folder->post_change_process,
			&related_folder->folder->folder_form,
			&related_folder->folder->notepad,
			&related_folder->folder->html_help_file_anchor,
			&related_folder->folder->
				post_change_javascript,
			&related_folder->folder->lookup_before_drop_down,
			&related_folder->folder->data_directory,
			&related_folder->folder->index_directory,
			&related_folder->folder->no_initial_capital,
			&related_folder->folder->subschema_name,
			&related_folder->folder->create_view_statement,
			application_name,
			session,
			related_folder->folder->folder_name,
			override_row_restrictions,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

		related_folder->foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
				related_folder->
					folder->
						primary_attribute_name_list,
				related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );

		if ( isa_flag != related_folder->relation_type_isa )
		{
			continue;
		}
		else
		if ( recursive_request_type == related_folder_no_recursive
		&&   !isa_flag )
		{
			list_append_pointer(
				related_folder_list,
				related_folder );
			continue;
		}
		else
		if ( !isa_flag
		&&	recursive_request_type !=
			related_folder_no_recursive )
		{
			if ( root_primary_attribute_name_list )
			{
				if ( !list_is_subset_of(
				attribute_get_primary_attribute_name_list(
					related_folder->folder->attribute_list),
				root_primary_attribute_name_list ) )
				{
					continue;
				}
			}
		}

		if ( isa_flag )
		{
			if ( related_folder->relation_type_isa )
			{
				list_append_pointer(
					related_folder_list,
					related_folder );
			}
			else
			{
				continue;
			}
		}
		else
		{
			if ( !related_folder->relation_type_isa )
			{
				list_append_pointer(
					related_folder_list,
					related_folder );
			}
		}

		if ( (	recursive_request_type ==
			related_folder_prompt_recursive_only
		&&      related_folder->prompt_mto1_recursive )
		|| (    recursive_request_type ==
			related_folder_recursive_all ) )
		{
			LIST *local_root_primary_attribute_name_list;

			local_root_primary_attribute_name_list =
			      attribute_get_primary_attribute_name_list(
			      related_folder->folder->attribute_list );

			related_folder_list =
				related_folder_get_mto1_related_folder_list(
				   related_folder_list,
				   application_name,
				   session,
				   related_folder->folder->folder_name,
				   role_name,
				   isa_flag,
				   recursive_request_type,
				   override_row_restrictions,
				   local_root_primary_attribute_name_list,
				   recursive_level + 1 );
		}
	} while( list_next( local_related_folder_list ) );

	return related_folder_list;

}

LIST *related_folder_1tom_related_folder_list(
		char *application_name,
		char *session,
		char *folder_name,
		char *role_name,
		enum related_folder_list_usage related_folder_list_usage,
		LIST *primary_data_list,
		LIST *related_folder_list,
		boolean omit_isa_relations,
		enum related_folder_recursive_request_type
			recursive_request_type,
		LIST *parent_primary_attribute_name_list,
		LIST *original_primary_attribute_name_list,
		char *prior_related_attribute_name )
{
	RELATED_FOLDER *related_folder;
	LIST *local_related_folder_list;

	local_related_folder_list =
		related_folder_get_related_folder_list(
			application_name,
			session,
			folder_name,
			one2m,
			(LIST *)0 /* existing_related_folder_list */ );

	if (!list_rewind( local_related_folder_list ) )
	{
		return related_folder_list;
	}

	do {
		related_folder = list_get_pointer( local_related_folder_list );

		if ( related_folder_list_usage == detail
		&&   related_folder->omit_1tom_detail )
		{
			continue;
		}

		if ( omit_isa_relations
		&&   related_folder->relation_type_isa )
		{
			continue;
		}

		folder_load(
			&related_folder->
				one2m_folder->
				insert_rows_number,
			&related_folder->
				one2m_folder->
				lookup_email_output,
			&related_folder->
				one2m_folder->
				row_level_non_owner_forbid,
			&related_folder->
				one2m_folder->
				row_level_non_owner_view_only,
			&related_folder->
				one2m_folder->
				populate_drop_down_process,
			&related_folder->
				one2m_folder->
				post_change_process,
			&related_folder->
				one2m_folder->
				folder_form,
			&related_folder->
				one2m_folder->
				notepad,
			&related_folder->
				one2m_folder->
				html_help_file_anchor,
			&related_folder->
				one2m_folder->
				post_change_javascript,
			&related_folder->
				one2m_folder->
				lookup_before_drop_down,
			&related_folder->
				one2m_folder->
				data_directory,
			&related_folder->
				one2m_folder->
				index_directory,
			&related_folder->
				one2m_folder->
				no_initial_capital,
			&related_folder->
				one2m_folder->
				subschema_name,
			&related_folder->
				one2m_folder->
				create_view_statement,
			application_name,
			BOGUS_SESSION,
			related_folder->
				one2m_folder->
				folder_name,
			1 /* override_row_restrictions */,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

		related_folder->folder_foreign_attribute_name_list =
			related_folder_fetch_folder_foreign_attribute_name_list(
				application_name,
				related_folder->
					one2m_folder->
						folder_name,
				folder_name );

		if ( !related_folder->one2m_folder->attribute_list )
		{
			related_folder->one2m_folder->attribute_list =
				attribute_get_attribute_list(
				related_folder->
					one2m_folder->
					application_name,
				related_folder->
					one2m_folder->
					folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				role_name );
		}

		if ( !related_folder->
			one2m_folder->
			primary_attribute_name_list )
		{
			related_folder->
			one2m_folder->
			primary_attribute_name_list =
				folder_get_primary_attribute_name_list(
					related_folder->
						one2m_folder->
						attribute_list );
		}


		if ( !related_folder->
			one2m_folder->
			attribute_name_list )
		{
			related_folder->
			one2m_folder->
			attribute_name_list =
				folder_get_attribute_name_list(
					related_folder->
						one2m_folder->
						attribute_list );
		}

		if ( !related_folder->folder->attribute_list )
		{
			related_folder->folder->attribute_list =
				attribute_get_attribute_list(
				related_folder->
					folder->
					application_name,
				related_folder->
					folder->
					folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				role_name );
		}

		if ( !related_folder->folder->primary_attribute_name_list )
		{
			related_folder->
			folder->
			primary_attribute_name_list =
				folder_get_primary_attribute_name_list(
					related_folder->
						folder->
						attribute_list );
		}

		if ( !related_folder->folder->attribute_name_list )
		{
			related_folder->
			folder->
			attribute_name_list =
				folder_get_attribute_name_list(
					related_folder->
						folder->
						attribute_list );
		}

		related_folder->parent_primary_attribute_name_list =
			parent_primary_attribute_name_list;

		if ( list_length( parent_primary_attribute_name_list ) )
		{
			related_folder->foreign_attribute_name_list =
				related_folder_foreign_attribute_name_list(
					parent_primary_attribute_name_list,
					related_folder->
						related_attribute_name,
					related_folder->
					  folder_foreign_attribute_name_list );
		}
		else
		{
			related_folder->foreign_attribute_name_list =
				related_folder_foreign_attribute_name_list(
					related_folder->
						folder->
						primary_attribute_name_list,
					related_folder->
						related_attribute_name,
					related_folder->
					  folder_foreign_attribute_name_list );
		}

		if ( related_folder_list_usage != detail
		&&   related_folder_is_one2one_firewall(
			related_folder->foreign_attribute_name_list,
			related_folder->one2m_folder->attribute_list ) )
		{
			continue;
		}

		related_folder->
			primary_data_list =
				primary_data_list;

		if ( list_length(
			related_folder->folder_foreign_attribute_name_list ) )
		{
			related_folder->is_primary_key_subset = 0;
		}
		else
		{
			related_folder->is_primary_key_subset =
				list_is_subset_of(
					related_folder->
						foreign_attribute_name_list,
					related_folder->
						one2m_folder->
						primary_attribute_name_list );
		}

		related_folder_one2m_append_unique(
			related_folder_list,
			related_folder );

		if ( recursive_request_type == related_folder_no_recursive )
			continue;

		/* If at a leaf */
		/* ------------ */
		if ( !related_folder->is_primary_key_subset )
			continue;

		if ( related_folder_list_usage == update
		||   recursive_request_type == related_folder_recursive_all )
		{
			char *use_related_attribute_name;

			if ( related_folder->related_attribute_name
			&&   *related_folder->related_attribute_name
			&&   strcmp(	related_folder->related_attribute_name,
					"null" ) != 0 )
			{
				use_related_attribute_name =
					related_folder->related_attribute_name;
			}
			else
			{
				use_related_attribute_name =
					prior_related_attribute_name;
			}

			if ( list_length( primary_data_list ) )
			{
				related_folder_list =
				  related_folder_1tom_related_folder_list(
				   application_name,
				   session,
				   related_folder->
					one2m_folder->
						folder_name,
				   role_name,
				   related_folder_list_usage,
			   	   related_folder_get_primary_data_list(
					application_name,
					related_folder->
						one2m_folder->
							folder_name,
					related_folder->
						one2m_folder->
						primary_attribute_name_list,
					related_folder->
						foreign_attribute_name_list,
					primary_data_list ),
				   related_folder_list,
				   omit_isa_relations,
				   recursive_request_type,
				   related_folder->
					foreign_attribute_name_list,
				   original_primary_attribute_name_list,
					use_related_attribute_name );
			}
			else
			{
				related_folder_list =
				    related_folder_1tom_related_folder_list(
					application_name,
					session,
					related_folder->
						one2m_folder->
							folder_name,
					role_name,
					related_folder_list_usage,
					primary_data_list,
					related_folder_list,
					omit_isa_relations,
					recursive_request_type,
					related_folder->
						one2m_folder->
						primary_attribute_name_list,
					original_primary_attribute_name_list,
					use_related_attribute_name );
			}
		}

	} while( list_next( local_related_folder_list ) );

	return related_folder_list;
}

static LIST *global_related_folder_list = {0};

LIST *related_folder_get_global_related_folder_list(
			char *application_name,
			char *session,
			char delimiter )
{
	LIST *related_record_list;
	LIST *related_folder_list;
	char sys_string[ 1024 ];
	char buffer[ 4096 ];
	char *related_record;
	RELATED_FOLDER *related_folder;
	FOLDER *folder;
	char folder_name[ 128 ];
	char related_folder_name[ 128 ];
	char related_attribute_name[ 128 ];

	related_folder_list = list_new();

	sprintf(sys_string,
		"%s/src_appaserver/related_folder_list.sh %s \"%c\" 2>>%s",
		appaserver_parameter_file_get_appaserver_mount_point(),
		application_name,
		delimiter,
		appaserver_error_get_filename( application_name ) );

	related_record_list = pipe2list( sys_string );

	if ( !list_rewind( related_record_list ) )
		return related_folder_list;

	do {
		related_record = list_get_pointer( related_record_list );

		if ( character_count(
			delimiter, 
			related_record ) != 
			    RELATED_FOLDER_HINT_MESSAGE_PIECE )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: bad input: (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				related_record );
			exit( 1 );
		}

		piece(	folder_name,
			delimiter,
			related_record,
			RELATED_FOLDER_FOLDER_PIECE );

		piece(	related_folder_name,
			delimiter,
			related_record,
			RELATED_FOLDER_RELATED_FOLDER_PIECE );

		piece(	related_attribute_name,
			delimiter,
			related_record,
			RELATED_FOLDER_RELATED_ATTRIBUTE_PIECE );

		related_folder =
			related_folder_new(
				application_name,
				session,
				strdup( related_folder_name ),
				strdup( related_attribute_name ) );

		related_folder->folder_foreign_attribute_name_list =
			related_folder_fetch_folder_foreign_attribute_name_list(
				application_name,
				folder_name,
				related_folder_name );

		folder = folder_new_folder(
				application_name,
				session,
				strdup( folder_name ) );

		related_folder->one2m_folder = folder;

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_PAIR_1TOM_PIECE );
		related_folder->pair_1tom_order = atoi( buffer );

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_OMIT_1TOM_DETAIL_PIECE );
		related_folder->omit_1tom_detail = (*buffer == 'y');

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_MTO1_RECURSIVE_PIECE );
		related_folder->prompt_mto1_recursive = (*buffer == 'y');

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_RELATION_TYPE_ISA_PIECE );
		related_folder->relation_type_isa = (*buffer == 'y');

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_COPY_COMMON_ATTRIBUTES_PIECE );
		related_folder->copy_common_attributes =
				(*buffer == 'y');

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_AUTOMATIC_PRESELECTION_PIECE );
		related_folder->automatic_preselection =
				(*buffer == 'y');

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_DROP_DOWN_MULTI_SELECT_PIECE );
		related_folder->drop_down_multi_select =
				(*buffer == 'y');

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_JOIN_1TOM_EACH_ROW_PIECE );
		related_folder->join_1tom_each_row =
				(*buffer == 'y');

/* Want to retire omit_lookup_before_drop_down.
		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_OMIT_LOOKUP_BEFORE_DROP_DOWN_PIECE );
		related_folder->omit_lookup_before_drop_down =
				(*buffer == 'y');
*/

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_AJAX_FILL_DROP_DOWN_PIECE );
		related_folder->ajax_fill_drop_down =
				(*buffer == 'y');

		piece(	buffer,
			delimiter,
			related_record,
			RELATED_FOLDER_HINT_MESSAGE_PIECE );
		related_folder->hint_message = strdup( buffer );

		list_append_pointer(	related_folder_list,
					related_folder );

	} while( list_next( related_record_list ) );
	list_free_string_list( related_record_list );
	return related_folder_list;
} /* related_folder_get_global_related_folder_list() */

LIST *related_folder_mto1_related_folder_list(
			char *application_name,
			char *folder_name,
			LIST *existing_related_folder_list )
{
	LIST *related_folder_list;
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *new_related_folder;
	FOLDER *folder;

	if ( !global_related_folder_list )
	{
		global_related_folder_list =
			related_folder_get_global_related_folder_list(
				application_name,
				BOGUS_SESSION,
				RELATED_FOLDER_DELIMITER );
	}

	related_folder_list = list_new_list();

	if ( !list_rewind( global_related_folder_list ) )
		return (LIST *)0;

	do {
		related_folder = list_get_pointer( global_related_folder_list );

		folder = related_folder->folder;

		if ( related_folder_exists_related_folder_list(
					folder->folder_name,
					related_folder->related_attribute_name,
					existing_related_folder_list,
					mto1 /* relation_type */ ) )
		{
			continue;
		}

		if ( folder_name
		&&   strcmp(	related_folder->
					one2m_folder->
						folder_name,
				folder_name ) != 0 )
		{
			continue;
		}

		new_related_folder = related_folder_calloc();
		memcpy(	new_related_folder,
			related_folder,
			sizeof( RELATED_FOLDER ) );

		list_append_pointer(	related_folder_list,
					new_related_folder );

	} while( list_next( global_related_folder_list ) );

	return related_folder_list;

} /* related_folder_mto1_related_folder_list() */

LIST *related_folder_get_related_folder_list(
			char *application_name,
			char *session,
			char *folder_name,
			enum relation_type relation_type,
			LIST *existing_related_folder_list )
{
	LIST *related_folder_list;
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *new_related_folder;
	FOLDER *folder;

	if ( !global_related_folder_list )
	{
		global_related_folder_list =
			related_folder_get_global_related_folder_list(
						application_name,
						session,
						RELATED_FOLDER_DELIMITER );
	}

	related_folder_list = list_new_list();

	if ( !list_rewind( global_related_folder_list ) )
		return related_folder_list;

	do {
		related_folder = list_get_pointer( global_related_folder_list );

		if ( relation_type == one2m )
			folder = related_folder->one2m_folder;
		else
			folder = related_folder->folder;

		if ( related_folder_exists_related_folder_list(
					folder->folder_name,
					related_folder->related_attribute_name,
					existing_related_folder_list,
					relation_type ) )
		{
			continue;
		}

		if ( relation_type == one2m )
		{
			if ( folder_name
			&&   strcmp(	related_folder->folder->folder_name,
					folder_name ) != 0 )
			{
				continue;
			}
		}
		else
		{
			if ( folder_name
			&&   strcmp(	related_folder->
						one2m_folder->
							folder_name,
					folder_name ) != 0 )
			{
				continue;
			}
		}

		new_related_folder = related_folder_calloc();
		memcpy(	new_related_folder,
			related_folder,
			sizeof( RELATED_FOLDER ) );

		list_append_pointer(	related_folder_list,
					new_related_folder );

	} while( list_next( global_related_folder_list ) );

	return related_folder_list;

} /* related_folder_get_related_folder_list() */

boolean related_folder_exists_one2m_related_folder_list(
					char *related_folder_name,
					char *related_attribute_name,
					LIST *existing_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	FOLDER *folder;

	if ( list_rewind( existing_related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					existing_related_folder_list );

			folder = related_folder->one2m_folder;

			if ( strcmp(	folder->folder_name,
					related_folder_name ) == 0
			&&   strcmp(	related_folder->related_attribute_name,
					related_attribute_name ) == 0 )
			{
				return 1;
			}

		} while( list_next( existing_related_folder_list ) );
	}

	return 0;
	
} /* related_folder_exists_one2m_related_folder_list() */

boolean related_folder_exists_related_folder_list(
					char *related_folder_name,
					char *related_attribute_name,
					LIST *existing_related_folder_list,
					enum relation_type relation_type )
{
	RELATED_FOLDER *related_folder;
	FOLDER *folder;

	if ( list_rewind( existing_related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					existing_related_folder_list );

			if ( relation_type == one2m )
				folder = related_folder->one2m_folder;
			else
				folder = related_folder->folder;

			if ( strcmp(	folder->folder_name,
					related_folder_name ) == 0
			&&   strcmp(	related_folder->related_attribute_name,
					related_attribute_name ) == 0 )
			{
				return 1;
			}

		} while( list_next( existing_related_folder_list ) );
	}

	return 0;
	
} /* related_folder_exists_related_folder_list() */

LIST *related_folder_subtract_duplicate_related_folder_list(
			LIST *new_related_folder_list,
			LIST *related_folder_list )
{
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *new_related_folder;
	int all_done = 0;

	while( !all_done )
	{
		all_done = 1;
		if ( !list_rewind( new_related_folder_list ) ) break;
		if ( !list_length( related_folder_list ) ) break;
	
		do {
			new_related_folder =
				(RELATED_FOLDER *)
					list_get_pointer(
						new_related_folder_list );

			list_rewind( related_folder_list );
			do {
				related_folder =
					(RELATED_FOLDER *)
						list_get_pointer(
						related_folder_list );
	
				if ( strcmp(
					related_folder->folder->folder_name,
					new_related_folder->
							folder->
							folder_name ) == 0  )
				{
					list_delete_current(
						new_related_folder_list );
					list_rewind( new_related_folder_list );
					all_done = 0;
					break;
				}
	
			} while( list_next( related_folder_list ) );
			if ( !list_length( new_related_folder_list ) ) break;

		} while( list_next( new_related_folder_list ) );

		if ( !all_done ) break;

	} /* while not all_done */

	return new_related_folder_list;

} /* related_folder_subtract_duplicate_related_folder_list() */

LIST *related_folder_get_related_folder_name_list( LIST *related_folder_list )
{
	return related_folder_get_mto1_related_folder_name_list(
			related_folder_list );
}

LIST *related_folder_get_one2m_related_folder_name_list(
				LIST *one2m_related_folder_list )
{
	LIST *related_folder_name_list = list_new();
	RELATED_FOLDER *related_folder;

	if ( list_rewind( one2m_related_folder_list ) )
	{
		do {
			related_folder = list_get( one2m_related_folder_list );

			list_append_pointer(	related_folder_name_list,
						related_folder->
							one2m_folder->
							folder_name );

		} while( list_next( one2m_related_folder_list ) );
	}

	return related_folder_name_list;

} /* related_folder_get_one2m_related_folder_name_list() */

LIST *related_folder_get_mto1_multi_key_name_list(
				LIST *mto1_related_folder_list )
{
	LIST *related_folder_name_list;
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) )
		return (LIST *)0;

	related_folder_name_list = list_new();

	do {
		related_folder = list_get( mto1_related_folder_list );

		if ( list_length(
			related_folder->
				folder->
				primary_attribute_name_list ) > 1 )
		{
			list_append_pointer(	related_folder_name_list,
						related_folder->
							folder->
							folder_name );
		}

	} while( list_next( mto1_related_folder_list ) );

	return related_folder_name_list;

} /* related_folder_get_mto1_multi_key_name_list() */

LIST *related_folder_get_mto1_related_folder_name_list(
				LIST *mto1_related_folder_list )
{
	LIST *related_folder_name_list = list_new();
	RELATED_FOLDER *related_folder;

	if ( list_rewind( mto1_related_folder_list ) )
	{
		do {
			related_folder = list_get( mto1_related_folder_list );

			list_append_pointer(	related_folder_name_list,
						related_folder->
							folder->
							folder_name );
		} while( list_next( mto1_related_folder_list ) );
	}

	return related_folder_name_list;

} /* related_folder_get_mto1_related_folder_name_list() */

LIST *related_folder_get_isa_related_folder_list(
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				boolean override_row_restrictions,
				enum related_folder_recursive_request_type
					recursive_request_type )
{
	LIST *mto1_isa_related_folder_list;
	RELATED_FOLDER *related_folder;

	mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			session,
			folder_name,
			role_name,
			1 /* isa_flag */,
			recursive_request_type,
			override_row_restrictions,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	if ( !list_rewind( mto1_isa_related_folder_list ) )
	{
		list_free( mto1_isa_related_folder_list );
		return (LIST *)0;
	}

	do {
		related_folder =
			list_get_pointer(
				mto1_isa_related_folder_list );

		related_folder->folder->primary_attribute_name_list =
			folder_get_primary_attribute_name_list(
				related_folder->folder->
					attribute_list );

	} while( list_next( mto1_isa_related_folder_list ) );
	return mto1_isa_related_folder_list;
} /* related_folder_get_isa_related_folder_list() */

boolean related_folder_exists_1tom_relations(
				char *application_name,
				char *session,
				char *folder_name,
				LIST *original_primary_attribute_name_list,
				char *related_attribute_name )
{
	LIST *one2m_related_folder_list;
	RELATED_FOLDER *related_folder;
	LIST *subtracted_attribute_name_list;
	LIST *original_foreign_attribute_name_list;

	original_foreign_attribute_name_list =
		related_folder_foreign_attribute_name_list(
			original_primary_attribute_name_list,
			related_attribute_name,
			(LIST *)0 /* folder_foreign_attribute_name_list */ );

	one2m_related_folder_list =
		related_folder_get_related_folder_list(
			application_name,
			session,
			folder_name,
			one2m,
			(LIST *)0 /* existing_related_folder_list */ );

	if ( !list_rewind( one2m_related_folder_list ) ) return 0;

	do {
		related_folder = list_get( one2m_related_folder_list );

		if ( !related_folder->
			one2m_folder->
			attribute_name_list )
		{
			related_folder->
				one2m_folder->
				attribute_list =
				attribute_get_attribute_list(
					related_folder->
						one2m_folder->
						application_name,
					related_folder->
						one2m_folder->
						folder_name,
					(char *)0 /* attribute_name */,
					(LIST *)0
					/* mto1_isa_related_folder_list */,
					(char *)0 /* role_name */ );
		}

		if ( !related_folder->
			one2m_folder->
			primary_attribute_name_list )
		{
			related_folder->
				one2m_folder->
				primary_attribute_name_list =
				attribute_get_primary_attribute_name_list(
					related_folder->
						one2m_folder->
						attribute_list );
		}

		if ( related_folder->related_attribute_name
		&&   *related_folder->related_attribute_name
		&&   strcmp(	related_folder->related_attribute_name,
				"null" ) != 0 )
		{
			original_foreign_attribute_name_list =
				related_folder_foreign_attribute_name_list(
					original_primary_attribute_name_list,
					related_folder->
						related_attribute_name,
					related_folder->
					  folder_foreign_attribute_name_list );
		}

		related_folder->foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
				related_folder->
					one2m_folder->
					primary_attribute_name_list,
				related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );

		subtracted_attribute_name_list =
			list_subtract_string_list(
				original_foreign_attribute_name_list,
				related_folder->
					foreign_attribute_name_list );

		if ( list_length( subtracted_attribute_name_list ) == 0 )
			return 1;

	} while( list_next( one2m_related_folder_list ) );

	return 0;

} /* related_folder_exists_1tom_relations() */

void related_folder_reset_ignore_output( LIST *related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( list_rewind( related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					related_folder_list );

			related_folder->ignore_output = 0;
		} while( list_next( related_folder_list ) );
	}
} /* related_folder_reset_ignore_output() */

LIST *related_folder_get_mto1_folder_name_list(
					LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	LIST *folder_name_list = list_new();

	if ( mto1_related_folder_list
	&&   list_length( mto1_related_folder_list ) )
	{
		do {
			related_folder =
				(RELATED_FOLDER *)
					list_get_pointer(
						mto1_related_folder_list );

			list_append_pointer(
				folder_name_list,
				related_folder->folder->folder_name);

		} while( list_next( mto1_related_folder_list ) );
	}

	return folder_name_list;
} /* related_folder_get_mto1_folder_name_list() */


void related_folder_populate_common_non_primary_attribute_name_list(
				LIST *common_non_primary_attribute_name_list,
				char *application_name,
				char *related_folder_name,
				char *one2m_related_folder_name )
{
	LIST *related_folder_attribute_list = list_new_list();
	LIST *one2m_related_folder_attribute_list = list_new_list();
	ATTRIBUTE *related_attribute;
	ATTRIBUTE *related_to_attribute;

	attribute_append_attribute_list(
		related_folder_attribute_list,
		application_name,
		related_folder_name,
		(char *)0 /* attribute_name */,
		(char *)0 /* role_name */,
		attribute_fetch_either );

	attribute_append_attribute_list(
		one2m_related_folder_attribute_list,
		application_name,
		one2m_related_folder_name,
		(char *)0 /* attribute_name */,
		(char *)0 /* role_name */,
		attribute_fetch_either );

	if ( !list_rewind( related_folder_attribute_list ) ) return;

	do {
		related_attribute =
			list_get_pointer(
				related_folder_attribute_list );

		if ( !list_rewind( one2m_related_folder_attribute_list ) )
			return;

		do {
			related_to_attribute =
				list_get_pointer(
					one2m_related_folder_attribute_list );

			if ( strcmp( related_to_attribute->
					attribute_name,
				     related_attribute->
					attribute_name ) == 0
			&&   related_to_attribute->display_order
			&&   related_attribute->display_order )
			{
				list_append_pointer( 
					common_non_primary_attribute_name_list,
					related_attribute->
						attribute_name );
			}
		} while( list_next( one2m_related_folder_attribute_list ) );

	} while( list_next( related_folder_attribute_list ) );
}

/* Note: this only applies when state=insert. */
/* ------------------------------------------ */
LIST *related_folder_get_common_non_primary_attribute_name_list(
					char *application_name,
					char *folder_name,
					LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	LIST *return_list = list_new_list();

	if ( !mto1_related_folder_list
	||   !list_rewind( mto1_related_folder_list ) )
	{
		return return_list;
	}

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( !related_folder->copy_common_attributes )
			continue;

		related_folder->
			common_non_primary_attribute_name_list =
				list_new_list();

		related_folder_populate_common_non_primary_attribute_name_list(
				related_folder->
					common_non_primary_attribute_name_list,
				application_name,
				folder_name,
				related_folder->folder->folder_name );

		list_append_string_list(
			return_list,
			related_folder->
				common_non_primary_attribute_name_list );

	} while( list_next( mto1_related_folder_list ) );
	return return_list;
}

ELEMENT_APPASERVER *related_folder_get_new_button_element(
				char *folder_name,
				LIST *role_folder_insert_list,
				char *form_name )
{
	char element_name[ 128 ];
	ELEMENT_APPASERVER *element;

	if ( role_folder_insert_list
	&&   list_exists_string( role_folder_insert_list, folder_name ) )
	{
		sprintf( element_name,
			 "%s%s",
			 VERTICAL_NEW_PUSH_BUTTON_PREFIX,
			 folder_name );
	
		element = element_appaserver_new(
					toggle_button, 
					strdup( element_name ) );
	
		element_toggle_button_set_heading(
			element->toggle_button,
			VERTICAL_NEW_PUSH_BUTTON_LABEL );

		element->toggle_button->form_name = form_name;
		element->toggle_button->onchange_submit_yn = 'y';
	
		return element;
	}
	return (ELEMENT_APPASERVER *)0;
} /* related_folder_get_new_button_element() */

LIST *related_folder_get_mto1_common_non_primary_related_folder_list(
			char *application_name,
			char *session,
			char *folder_name,
			boolean override_row_restrictions )
{
	LIST *mto1_related_folder_list;
	RELATED_FOLDER *related_folder;
	LIST *return_list = list_new_list();

	mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new(),
			application_name,
			session,
			folder_name,
			(char *)0 /* role_name */,
			0 /* isa_flag */,
			related_folder_no_recursive,
			override_row_restrictions,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	if ( !mto1_related_folder_list
	||   !list_rewind( mto1_related_folder_list ) )
	{
		return return_list;
	}

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( !related_folder->copy_common_attributes )
			continue;

		related_folder->
			common_non_primary_attribute_name_list =
				list_new_list();

		related_folder_populate_common_non_primary_attribute_name_list(
				related_folder->
					common_non_primary_attribute_name_list,
				application_name,
				folder_name,
				related_folder->folder->folder_name );

		list_append_pointer(
			return_list,
			related_folder );

	} while( list_next( mto1_related_folder_list ) );
	return return_list;

} /* related_folder_get_mto1_common_non_primary_related_folder_list() */

LIST *related_folder_list_get_preselection_dictionary_list(
				char *application_name,
				char *session,
				char *login_name,
				char *folder_name,
				DICTIONARY *query_dictionary,
				LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return (LIST *)0;

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( related_folder->automatic_preselection )
		{
			return
			     related_folder_get_preselection_dictionary_list(
				application_name,
				session,
				folder_name,
				related_folder->folder->folder_name,
				query_dictionary,
				login_name );
		}

	} while( list_next( mto1_related_folder_list ) );
	return (LIST *)0;

} /* related_folder_list_get_preselection_dictionary_list() */

boolean related_folder_exists_prompt_mto1_recursive(
				LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !mto1_related_folder_list
	||   !list_rewind( mto1_related_folder_list ) )
	{
		return 0;
	}

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( related_folder->prompt_mto1_recursive )
			return 1;

	} while( list_next( mto1_related_folder_list ) );
	return 0;

} /* related_folder_exists_prompt_mto1_recursive() */

boolean related_folder_exists_automatic_preselection(
				LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !mto1_related_folder_list
	||   !list_rewind( mto1_related_folder_list ) )
	{
		return 0;
	}

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( related_folder->automatic_preselection )
			return 1;

	} while( list_next( mto1_related_folder_list ) );
	return 0;

} /* related_folder_exists_automatic_preselection() */

LIST *related_folder_get_preselection_dictionary_list(
				char *application_name,
				char *session,
				char *folder_name,
				char *related_folder_name,
				DICTIONARY *query_dictionary,
				char *login_name )
{
	QUERY *query;
	LIST *related_folder_dictionary_list = {0};
	FOLDER *related_folder;
	LIST *related_primary_attribute_name_list;

	related_folder = folder_new_folder(
					application_name,
					session,
					related_folder_name );

	folder_load(
			&related_folder->insert_rows_number,
			&related_folder->lookup_email_output,
			&related_folder->row_level_non_owner_forbid,
			&related_folder->row_level_non_owner_view_only,
			&related_folder->populate_drop_down_process,
			&related_folder->post_change_process,
			&related_folder->folder_form,
			&related_folder->notepad,
			&related_folder->html_help_file_anchor,
			&related_folder->
				post_change_javascript,
			&related_folder->lookup_before_drop_down,
			&related_folder->data_directory,
			&related_folder->index_directory,
			&related_folder->no_initial_capital,
			&related_folder->subschema_name,
			&related_folder->create_view_statement,
			related_folder->application_name,
			related_folder->session,
			related_folder->folder_name,
			0 /* not override_row_restrictions */,
			(char *)0 /* role_name */,
			(LIST *)0 /* mto1_related_folder_list */ );

	related_folder->attribute_list =
		attribute_get_attribute_list(
			related_folder->application_name,
			related_folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	related_primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			related_folder->attribute_list );

	if ( related_folder->populate_drop_down_process )
	{
		related_folder_dictionary_list =
		    folder_get_process_dictionary_list(
			application_name,
			session,
			related_folder->folder_name,
			login_name,
			related_folder->populate_drop_down_process,
			(char *)0 /* role_name */,
			query_dictionary /* parameter_dictionary */,
			(char *)0 /* state */,
			folder_name /*one2m_folder_name_for_processes */,
			related_folder->attribute_list,
			0
			/* piece_multi_attribute_data_label_delimiter */,
			(char *)0 /* process_name */,
			(char *)0 /* prompt */,
			related_primary_attribute_name_list );
	}
	else
	{
		query = query_edit_table_new(
				query_dictionary,
				application_name,
				login_name,
				related_folder->folder_name,
				(ROLE *)0 );

		related_folder_dictionary_list =
			query_row_dictionary_list(
				query->folder->application_name,
				query->query_output->select_clause,
				query->query_output->from_clause,
				query->query_output->where_clause,
				query->query_output->order_clause,
				query->max_rows,
				query->folder->append_isa_attribute_list,
				query->login_name );
	}

	return related_folder_subtract_preselection_existing_dictionary_list(
			related_folder_dictionary_list,
			application_name,
			session,
			folder_name,
			query_dictionary,
			login_name,
			related_primary_attribute_name_list );

} /* related_folder_get_preselection_dictionary_list() */

LIST *related_folder_subtract_preselection_existing_dictionary_list(
			LIST *related_folder_dictionary_list,
			char *application_name,
			char *session,
			char *primary_folder_name,
			DICTIONARY *query_dictionary,
			char *login_name,
			LIST *related_primary_attribute_name_list )
{
	LIST *return_dictionary_list;
	QUERY *query;
	LIST *primary_folder_dictionary_list;
	FOLDER *primary_folder;
	LIST *related_key_data_list;
	DICTIONARY *primary_dictionary;
	DICTIONARY *related_dictionary;
	int ok_add;

	primary_folder = folder_new_folder(
					application_name,
					session,
					primary_folder_name );

	primary_folder->attribute_list =
		attribute_get_attribute_list(
			primary_folder->application_name,
			primary_folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	query = query_insert_new(
			application_name,
			login_name,
			primary_folder->folder_name,
			query_dictionary );

	primary_folder_dictionary_list =
		query_row_dictionary_list(
				query->folder->application_name,
				query->query_output->select_clause,
				query->query_output->from_clause,
				query->query_output->where_clause,
				query->query_output->order_clause,
				query->max_rows,
				query->folder->append_isa_attribute_list,
				query->login_name );

	if ( !primary_folder_dictionary_list
	||   !list_length( primary_folder_dictionary_list )
	||   !list_length( related_folder_dictionary_list ) )
	{
		return related_folder_dictionary_list;
	}

	return_dictionary_list = list_new_list();

	list_rewind( related_folder_dictionary_list );

	do {
		related_dictionary =
			list_get_pointer(
				related_folder_dictionary_list );

		related_key_data_list =
			dictionary_get_key_data_list(
				related_dictionary,
				related_primary_attribute_name_list,
				'^' );

		related_key_data_list =
			piece_list(
				related_key_data_list,
				'[',
				0 );

		list_rewind( primary_folder_dictionary_list );

		ok_add = 1;
		do {
			primary_dictionary =
				list_get_pointer(
				primary_folder_dictionary_list );

			if ( related_folder_exists_all_key_data_list(
				primary_dictionary,
				related_key_data_list,
				'^' ) )
			{
				ok_add = 0;
				break;
			}

		} while( list_next( primary_folder_dictionary_list ) );

		if ( ok_add )
		{
			list_append_pointer(
				return_dictionary_list,
				related_dictionary );
		}

	} while( list_next( related_folder_dictionary_list ) );

	return return_dictionary_list;

} /* related_folder_subtract_preselection_existing_dictionary_list() */

boolean related_folder_exists_all_key_data_list(
					DICTIONARY *dictionary,
					LIST *key_data_list,
					char delimiter )
{
	char key[ 128 ];
	char data[ 1024 ];
	char *key_data;
	char *dictionary_data;

	if ( !list_rewind( key_data_list ) ) return 0;

	do {
		key_data = list_get_pointer( key_data_list );

 		piece( key, delimiter, key_data, 0 );
 		piece( data, delimiter, key_data, 1 );

		if ( !( dictionary_data =
			dictionary_get_data(
				dictionary,
				key ) ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s(): cannot find key = (%s) in dictionary = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 key,
				 dictionary_display( dictionary ) );
			exit( 1 );
		}
		else
		{
			if ( timlib_strcmp( dictionary_data, data ) != 0 )
			{
				return 0;
			}
		}
	} while( list_next( key_data_list ) );
	return 1;
} /* related_folder_exists_key_all_data_list() */

boolean related_folder_exists_pair_1tom( LIST *related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !related_folder_list
	||   !list_rewind( related_folder_list ) )
	{
		return 0;
	}

	do {
		related_folder = list_get_pointer( related_folder_list );

		if ( related_folder->pair_1tom_order ) return 1;

	} while( list_next( related_folder_list ) );
	return 0;

} /* related_folder_exists_pair_1tom() */

char *related_folder_get_hint_message(
				char *attribute_hint_message,
				char *related_folder_hint_message,
				char *folder_notepad )
{
	char *hint_message;

/*
	if ( attribute_hint_message
	&&   *attribute_hint_message )
	{
		hint_message = attribute_hint_message;
	}
	else
*/

	if ( related_folder_hint_message
	&&   *related_folder_hint_message )
	{
		hint_message =
			related_folder_hint_message;
	}
	else
	if ( attribute_hint_message
	&&   *attribute_hint_message )
	{
		hint_message = attribute_hint_message;
	}
	else
	{
		hint_message = folder_notepad;
	}
	return hint_message;
} /* related_folder_get_hint_message() */

LIST *related_folder_get_pair_one2m_related_folder_list(
			char *application_name,
			char *folder_name,
			char *role_name )
{
	LIST *one2m_related_folder_list;
	LIST *pair_one2m_related_folder_list = {0};
	RELATED_FOLDER *related_folder;

	one2m_related_folder_list =
		related_folder_1tom_related_folder_list(
			application_name,
			BOGUS_SESSION,
			folder_name,
			role_name,
			other,
			(LIST *)0 /* primary_data_list */,
			list_new_list() /* related_folder_list */,
			0 /* dont omit_isa_relations */,
			related_folder_no_recursive,
			(LIST *)0 /* parent_primary_attribute_name_list */,
			(LIST *)0 /* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

	if ( list_rewind( one2m_related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					one2m_related_folder_list );

			if ( related_folder->pair_1tom_order )
			{
				if ( !pair_one2m_related_folder_list )
				{
					pair_one2m_related_folder_list =
						list_new();
				}

/*
				list_add_pointer_in_order(
					pair_1tom_related_folder_list, 
					related_folder, 
					related_folder_pair_match_function );
*/

				list_append_pointer(
					pair_one2m_related_folder_list,
					related_folder );
			}
		} while( list_next( one2m_related_folder_list ) );
	}

	return pair_one2m_related_folder_list;

} /* related_folder_get_pair_one2m_related_folder_list() */

void related_folder_populate_primary_data_dictionary(
			LIST *mto1_related_folder_list,
			char *application_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) )
	{
		return;
	}

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( !list_length(	related_folder->
						folder->
						primary_attribute_name_list ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: for folder_name = %s, empty primary_attribute_name_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 related_folder->folder->folder_name );
			exit( 1 );
		}

		related_folder->folder->primary_data_dictionary =
			folder_get_primary_data_dictionary(
					application_name,
					related_folder->folder->folder_name,
					related_folder->
						folder->
						primary_attribute_name_list );

	} while( list_next( mto1_related_folder_list ) );

} /* related_folder_populate_primary_data_dictionary() */

boolean related_folder_exists_mto1_folder_name(
				LIST *mto1_related_folder_list,
				char *folder_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) )
	{
		return 0;
	}

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( strcmp(	folder_name,
				related_folder->
					folder->
					folder_name ) == 0 )
		{
			return 1;
		}

	} while( list_next( mto1_related_folder_list ) );

	return 0;

} /* related_folder_exists_mto1_folder_name() */

boolean related_folder_exists_mto1_folder_name_list(
				LIST *mto1_related_folder_list,
				LIST *folder_name_list )
{
	char *folder_name;

	if ( !list_rewind( folder_name_list ) ) return 0;

	do {
		folder_name = list_get_pointer( folder_name_list );

		if ( related_folder_exists_mto1_folder_name(
				mto1_related_folder_list,
				folder_name ) )
		{
			return 1;
		}

	} while( list_next( folder_name_list ) );

	return 0;

} /* related_folder_exists_mto1_folder_name_list() */

char *related_folder_append_where_clause_related_join(
					FOLDER *folder,
					char *application_name,
					char *source_where_clause,
					RELATED_FOLDER *related_folder )
{
	if ( !folder || !related_folder )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty folder or related folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( folder->primary_attribute_name_list ) )
	{
		folder->primary_attribute_name_list =
			folder_get_primary_attribute_name_list(
					folder->attribute_list );
	}

	if ( !list_length( related_folder->foreign_attribute_name_list ) )
	{
		related_folder->foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
				folder->primary_attribute_name_list,
				related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );
	}

	return query_append_where_clause_related_join(
			application_name,
			source_where_clause,
			folder->primary_attribute_name_list,
			related_folder->foreign_attribute_name_list,
			folder->folder_name,
			related_folder->one2m_folder->folder_name );

} /* related_folder_append_where_clause_related_join() */

void related_folder_set_join_where_clause(
					LIST *one2m_related_folder_list,
					FOLDER *folder,
					char *application_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( one2m_related_folder_list ) )
		return;

	do {
		related_folder =
			list_get_pointer(
				one2m_related_folder_list );

		related_folder->join_where_clause =
			related_folder_append_where_clause_related_join(
					folder,
					application_name,
					(char *)0 /* source_where_clause */,
					related_folder );

	} while( list_next( one2m_related_folder_list ) );

} /* related_folder_set_join_where_clause() */

LIST *related_folder_get_lookup_before_drop_down_related_folder_list(
				LIST *related_folder_list,
				char *application_name,
				char *folder_name,
				LIST *base_folder_attribute_list,
				int recursive_level )
{
	RELATED_FOLDER *related_folder;
	LIST *local_related_folder_list;

	if ( !folder_name )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !related_folder_list ) related_folder_list = list_new_list();

	local_related_folder_list =
		related_folder_get_related_folder_list(
			application_name,
			BOGUS_SESSION,
			folder_name,
			mto1,
			(LIST *)0 /* existing_related_folder_list */ );

	if (!list_rewind( local_related_folder_list ) )
	{
		return related_folder_list;
	}

	do {
		related_folder = list_get_pointer( local_related_folder_list );

		related_folder->recursive_level = recursive_level;

		folder_load(
			&related_folder->folder->insert_rows_number,
			&related_folder->folder->lookup_email_output,
			&related_folder->folder->row_level_non_owner_forbid,
			&related_folder->folder->row_level_non_owner_view_only,
			&related_folder->folder->populate_drop_down_process,
			&related_folder->folder->post_change_process,
			&related_folder->folder->folder_form,
			&related_folder->folder->notepad,
			&related_folder->folder->html_help_file_anchor,
			&related_folder->folder->
				post_change_javascript,
			&related_folder->folder->lookup_before_drop_down,
			&related_folder->folder->data_directory,
			&related_folder->folder->index_directory,
			&related_folder->folder->no_initial_capital,
			&related_folder->folder->subschema_name,
			&related_folder->folder->create_view_statement,
			application_name,
			BOGUS_SESSION,
			related_folder->folder->folder_name,
			1 /* override_row_restrictions */,
			(char *)0 /* role_name */,
			(LIST *)0 /* mto1_related_folder_list */ );

		if ( !related_folder->folder->lookup_before_drop_down )
		{
			continue;
		}

		if ( !related_folder->folder->attribute_list )
		{
			related_folder->folder->attribute_list =
			attribute_get_attribute_list(
				related_folder->folder->application_name,
				related_folder->folder->folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				(char *)0 /* role_name */ );


			related_folder->folder->primary_attribute_name_list =
				attribute_get_primary_attribute_name_list(
					related_folder->
						folder->
						attribute_list );
		}

		if ( !related_folder->one2m_folder->attribute_list )
		{
			related_folder->one2m_folder->attribute_list =
			attribute_get_attribute_list(
				related_folder->
					one2m_folder->
					application_name,
				related_folder->
					one2m_folder->
					folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				(char *)0 /* role_name */ );

			related_folder->
				one2m_folder->
				primary_attribute_name_list =
				attribute_get_primary_attribute_name_list(
					related_folder->
						one2m_folder->
						attribute_list );
		}

		related_folder->foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
				related_folder->
					folder->
					primary_attribute_name_list,
				related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );

		if ( related_folder_is_one2one_firewall(
			related_folder->foreign_attribute_name_list,
			base_folder_attribute_list ) )
		{
			continue;
		}

		list_append_pointer(
			related_folder_list,
			related_folder );

		related_folder_list =
		related_folder_get_lookup_before_drop_down_related_folder_list(
				related_folder_list,
				application_name,
				related_folder->folder->folder_name,
				related_folder->folder->attribute_list
					/* base_folder_attribute_list */,
				recursive_level + 1 );

	} while( list_next( local_related_folder_list ) );

	return related_folder_list;

} /* related_folder_get_lookup_before_drop_down_related_folder_list() */

void related_folder_set_no_ignore_output(
				LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return;

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		related_folder->ignore_output = 0;

	} while( list_next( mto1_related_folder_list ) );

} /* related_folder_set_no_ignore_output() */

RELATED_FOLDER *related_folder_mto1_seek(
				LIST *mto1_related_folder_list,
				char *folder_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) )
		return (RELATED_FOLDER *)0;

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( strcmp(	related_folder->folder->folder_name,
				folder_name ) == 0 )
		{
			return related_folder;
		}

	} while( list_next( mto1_related_folder_list ) );

	return (RELATED_FOLDER *)0;

} /* related_folder_mto1_seek() */

RELATED_FOLDER *related_folder_one2m_seek(
				LIST *one2m_related_folder_list,
				char *folder_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( one2m_related_folder_list ) )
		return (RELATED_FOLDER *)0;

	do {
		related_folder = list_get_pointer( one2m_related_folder_list );

		if ( strcmp(	related_folder->
					one2m_folder->
					folder_name,
				folder_name ) == 0 )
		{
			return related_folder;
		}

	} while( list_next( one2m_related_folder_list ) );

	return (RELATED_FOLDER *)0;

} /* related_folder_one2m_seek() */

LIST *related_folder_get_join_1tom_related_folder_list(
				LIST *one2m_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	LIST *related_folder_list = {0};

	if ( !list_rewind( one2m_related_folder_list ) )
		return (LIST *)0;

	do {
		related_folder = list_get_pointer( one2m_related_folder_list );

		if ( related_folder->join_1tom_each_row )
		{
			if ( !related_folder_list )
				related_folder_list = list_new();

			list_append_pointer(	related_folder_list,
						related_folder );
		}

	} while( list_next( one2m_related_folder_list ) );

	return related_folder_list;

} /* related_folder_get_join_1tom_related_folder_list() */

LIST *related_folder_get_non_edit_multi_element_list(
				char *folder_name )
{
	LIST *return_list;
	ELEMENT_APPASERVER *element;

	return_list = list_new_list();

	element = element_appaserver_new(
			non_edit_multi_select,
			folder_name );

	list_append_pointer(
			return_list, 
			element );

	return return_list;

} /* related_folder_get_non_edit_multi_element_list() */

boolean related_folder_mto1_exists_drop_down_multi_select(
			LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return 0;

	do {
		related_folder =
			list_get_pointer(
				mto1_related_folder_list );

		if ( related_folder->drop_down_multi_select ) return 1;

	} while( list_next( mto1_related_folder_list ) );

	return 0;

} /* related_folder_mto1_exists_drop_down_multi_select() */

LIST *related_folder_fetch_non_primary_foreign_data_list(
					char *application_name,
					LIST *select_list,
					char *folder_name,
					LIST *primary_attribute_name_list,
					LIST *primary_data_list )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 list_display( select_list ),
		 folder_name,
		 list_usage_attribute_data_list2where_clause(
				primary_attribute_name_list,
				primary_data_list ) );

	return list_string_to_list(
			pipe2string( sys_string ),
			FOLDER_DATA_DELIMITER );

} /* related_folder_fetch_non_primary_foreign_data_list() */

void related_folder_one2m_append_unique(
			LIST *one2m_related_folder_list,
			RELATED_FOLDER *related_folder )
{
	if ( list_length( related_folder->folder_foreign_attribute_name_list ) )
	{
		list_append_pointer(
			one2m_related_folder_list,
			related_folder );
	}
	else
	if ( !related_folder_exists_one2m_related_folder_list(
			related_folder->
				one2m_folder->
				folder_name,
			related_folder->related_attribute_name,
			one2m_related_folder_list ) )
	{
		list_append_pointer(
			one2m_related_folder_list,
			related_folder );
	}

} /* related_folder_one2m_append_unique() */

void related_folder_mto1_append_unique(
			LIST *mto1_related_folder_list1,
			LIST *mto1_related_folder_list2 )
{
	RELATED_FOLDER *related_folder2;

	if ( !list_rewind( mto1_related_folder_list2 ) ) return;

	do {
		related_folder2 = list_get( mto1_related_folder_list2 );

		if ( !related_folder_exists_related_folder_list(
				related_folder2->folder->folder_name,
				related_folder2->related_attribute_name,
				mto1_related_folder_list1,
				mto1 ) )
		{
			list_append_pointer(
				mto1_related_folder_list1,
				related_folder2 );
		}
	} while( list_next( mto1_related_folder_list2 ) );

} /* related_folder_mto1_append_unique() */

char *related_folder_get_appaserver_user_foreign_login_name(
			LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	LIST *inside_mto1_related_folder_list;
	char *appaserver_user_foreign_login_name = {0};

	if ( !list_rewind( mto1_related_folder_list ) ) return (char *)0;

	do {
		related_folder = list_get( mto1_related_folder_list );

		if ( strcmp(	related_folder->folder->folder_name,
				"appaserver_user" ) == 0 )
		{
			goto return_foreign_login_name;
		}

		if ( !list_rewind(	related_folder->
						folder->
						mto1_related_folder_list ) )
		{
			continue;
		}

		inside_mto1_related_folder_list =
			related_folder->folder->mto1_related_folder_list;

		do {
			related_folder =
				list_get(
					inside_mto1_related_folder_list );
		
			if ( strcmp(	related_folder->folder->folder_name,
					"appaserver_user" ) == 0 )
			{
				goto return_foreign_login_name;
			}
		} while( list_next( inside_mto1_related_folder_list ) );

	} while( list_next( mto1_related_folder_list ) );

	return (char *)0;

return_foreign_login_name:

	if ( list_length( related_folder->
				foreign_attribute_name_list ) )
	{
		appaserver_user_foreign_login_name =
			list_get_first_pointer(
				related_folder->
				foreign_attribute_name_list );
	}

	if ( !appaserver_user_foreign_login_name
	||   strcmp(	appaserver_user_foreign_login_name,
			"null" ) == 0 )
	{
		appaserver_user_foreign_login_name = "login_name";
	}
	return appaserver_user_foreign_login_name;

} /* related_folder_get_appaserver_user_foreign_login_name() */

RELATED_FOLDER *related_folder_get_view_only_related_folder(
			LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) )
		return (RELATED_FOLDER *)0;

	do {
		related_folder = list_get( mto1_related_folder_list );

		if ( related_folder->folder->row_level_non_owner_view_only )
			return related_folder;

	} while( list_next( mto1_related_folder_list ) );

	return (RELATED_FOLDER *)0;

} /* related_folder_get_view_only_related_folder() */

void related_folder_populate_one2m_foreign_attribute_dictionary(
			DICTIONARY *foreign_attribute_dictionary,
			char *last_primary_attribute_name,
			char *related_attribute_name )
{
	if ( related_attribute_name
	&&   *related_attribute_name
	&&   strcmp( related_attribute_name, "null" ) != 0 )
	{
		char *data;

		if ( !dictionary_exists_key(
			foreign_attribute_dictionary,
			related_attribute_name ) )
		{
			dictionary_set_pointer(
				foreign_attribute_dictionary,
				related_attribute_name,
				last_primary_attribute_name );
		}

		if ( ( data = dictionary_get_pointer(
				foreign_attribute_dictionary,
				last_primary_attribute_name ) ) )
		{
			if ( !dictionary_exists_key(
				foreign_attribute_dictionary,
				related_attribute_name ) )
			{
				dictionary_set_pointer(
					foreign_attribute_dictionary,
					related_attribute_name,
					data );
			}
		}
	}

} /* related_folder_populate_one2m_foreign_attribute_dictionary() */

void related_folder_list_populate_one2m_foreign_attribute_dictionary(
			DICTIONARY *foreign_attribute_dictionary,
			char *last_primary_attribute_name,
			LIST *one2m_recursive_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( one2m_recursive_related_folder_list ) ) return;

	do {
		related_folder =
			list_get_pointer(
				one2m_recursive_related_folder_list );

/*
		if ( list_length(
			related_folder->
				folder_foreign_attribute_name_list ) )
		{
			related_populate_folder_foreign_attribute_dictionary(
				foreign_attribute_dictionary,
				related_folder->
					folder_foreign_attribute_name_list,
				related_folder->
					foreign_attribute_name_list );
			continue;
		}
*/

		related_folder_populate_one2m_foreign_attribute_dictionary(
			foreign_attribute_dictionary,
			last_primary_attribute_name,
			related_folder->related_attribute_name );

	} while( list_next( one2m_recursive_related_folder_list ) );

} /* related_folder_list_populate_one2m_foreign_attribute_dictionary() */

void related_folder_list_populate_mto1_isa_foreign_attribute_dictionary(
			DICTIONARY *foreign_attribute_dictionary,
			char *last_primary_attribute_name,
			LIST *mto1_isa_related_folder_list,
			char *application_name )
{
	RELATED_FOLDER *related_folder;
	FOLDER *folder;
	LIST *related_primary_attribute_name_list;
	char *last_related_primary_attribute_name;

	if ( !list_rewind( mto1_isa_related_folder_list ) )
	{
		return;
	}

	if ( !last_primary_attribute_name )
	{
		fprintf( stderr,
		"ERROR in %s/%s/%d: empty last_primary_attribute_name.\n", 
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		related_folder = list_get( mto1_isa_related_folder_list );

		if ( !related_folder->related_attribute_name
		||   !*related_folder->related_attribute_name
		||   strcmp(	related_folder->related_attribute_name,
				"null" ) == 0 )
		{
			continue;
		}

		folder = related_folder->folder;

		if ( !folder->attribute_list )
		{
			folder->attribute_list =
			attribute_get_attribute_list(
				application_name,
				folder->folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				(char *)0 /* role_name */ );
		}

		related_primary_attribute_name_list =
			attribute_get_primary_attribute_name_list(
				folder->attribute_list );

		if ( !list_length( related_primary_attribute_name_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s/%d: no primary attributes for folder = (%s)\n", 
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 folder->folder_name );
			exit( 1 );
		}

		last_related_primary_attribute_name =
			list_get_last_pointer(
				related_primary_attribute_name_list );

		dictionary_set_pointer(
			foreign_attribute_dictionary,
			last_related_primary_attribute_name,
			last_primary_attribute_name );

	} while( list_next( mto1_isa_related_folder_list ) );

} /* related_folder_list_populate_mto1_isa_foreign_attribute_dictionary() */

int related_folder_pair_match_function(
				RELATED_FOLDER *related_folder_from_list,
				RELATED_FOLDER *related_folder_compare )
{
	if (	related_folder_from_list->pair_1tom_order <=
		related_folder_compare->pair_1tom_order )
	{
		return -1;
	}
	else
	{
		return 1;
	}

} /* related_folder_pair_match_function() */

boolean related_folder_is_one2one_firewall(
					LIST *foreign_attribute_name_list,
					LIST *attribute_list )
{
	char *attribute_name;
	ATTRIBUTE *attribute;

	if ( !list_rewind( foreign_attribute_name_list ) ) return 0;

	do {
		attribute_name =
			list_get_pointer(
				foreign_attribute_name_list );

		if ( ! ( attribute =
				attribute_seek(
					attribute_list,
					attribute_name ) ) )
		{
/* Not participating.
			fprintf( stderr,
"Warning in %s/%s()/%d: cannot seek attribute_name = (%s) in attribute_list = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 attribute_name,
				 attribute_list_display( attribute_list ) );
*/
			continue;
		}

		if ( attribute->omit_insert && attribute->omit_update )
			return 1;

	} while( list_next( foreign_attribute_name_list ) );

	return 0;

} /* related_folder_is_one2one_firewall() */

LIST *related_folder_fetch_folder_foreign_attribute_record_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder_name;
	char *order;

	folder_name = "foreign_attribute";

	if ( !folder_exists_folder(	application_name,
					folder_name ) )
	{
		return (LIST *)0;
	}

	select = "folder,related_folder,foreign_attribute";
	order = "foreign_key_index";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			order=%s	",
		 application_name,
		 select,
		 folder_name,
		 order );

	return pipe2list( sys_string );
}

LIST *related_folder_fetch_folder_foreign_attribute_name_list(
				char *application_name,
				char *folder_name,
				char *related_folder_name )
{
	static LIST *folder_foreign_attribute_record_list = {0};
	LIST *folder_foreign_attribute_name_list = {0};
	char *record;
	char local_folder_name[ 128 ];
	char local_related_folder_name[ 128 ];
	char foreign_attribute_name[ 128 ];

	if ( !folder_foreign_attribute_record_list )
	{
		folder_foreign_attribute_record_list =
		     related_folder_fetch_folder_foreign_attribute_record_list(
			application_name );
	}

	if ( !list_rewind( folder_foreign_attribute_record_list ) )
		return (LIST *)0;

	do {
		record =
			list_get_pointer(
				folder_foreign_attribute_record_list );

		piece(	local_folder_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcmp( local_folder_name, folder_name ) != 0 ) continue;

		piece(	local_related_folder_name,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( strcmp(	local_related_folder_name,
				related_folder_name ) != 0 )
		{
			continue;
		}

		piece(	foreign_attribute_name,
			FOLDER_DATA_DELIMITER,
			record,
			2 );

		if ( !folder_foreign_attribute_name_list )
		{
			folder_foreign_attribute_name_list = list_new();
		}

		list_append_pointer(
			folder_foreign_attribute_name_list,
			strdup( foreign_attribute_name ) );

	} while( list_next( folder_foreign_attribute_record_list ) );

	return folder_foreign_attribute_name_list;
}

void related_folder_mark_ignore_multi_attribute_primary_keys(
				LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return;

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( list_length( related_folder->
					folder->
					primary_attribute_name_list ) != 1 )
		{
			related_folder->ignore_output = 1;
		}
	} while( list_next( mto1_related_folder_list ) );
}

void related_populate_folder_foreign_attribute_dictionary(
				DICTIONARY *foreign_attribute_dictionary,
				LIST *folder_foreign_attribute_name_list,
				LIST *primary_attribute_name_list )
{
	char *foreign_attribute_name;
	char *primary_attribute_name;

	if ( !list_length( folder_foreign_attribute_name_list ) )
		return;

	if ( list_length( folder_foreign_attribute_name_list ) !=
	     list_length( primary_attribute_name_list ) )
	{
		fprintf( stderr,
	    "ERROR in %s/%s()/%d: list lengths are not the same: %d vs %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( folder_foreign_attribute_name_list ),
	     		 list_length( primary_attribute_name_list ) );
		exit( 1 );
	}

	list_rewind( folder_foreign_attribute_name_list );
	list_rewind( primary_attribute_name_list );

	do {
		foreign_attribute_name =
			list_get_pointer( 
				folder_foreign_attribute_name_list );

		primary_attribute_name =
			list_get_pointer( 
				primary_attribute_name_list );

		dictionary_set_pointer(
			foreign_attribute_dictionary,
			foreign_attribute_name,
			primary_attribute_name );

		list_next( primary_attribute_name_list );

	} while( list_next( folder_foreign_attribute_name_list ) );
}

RELATED_FOLDER *related_folder_get_ajax_fill_drop_down_related_folder(
			LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return 0;

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		if ( related_folder->ajax_fill_drop_down )
		{
			return related_folder;
		}

	} while( list_next( mto1_related_folder_list ) );

	return (RELATED_FOLDER *)0;
}

char *related_folder_get_ajax_onclick_function(
				LIST *attribute_name_list )
{
	char onclick_function[ 512 ];

	sprintf( onclick_function,
		 "fork_ajax_window( '%s_$row' )",
		 list_display_delimited(
			attribute_name_list,
			'^' ) );

	return strdup( onclick_function );

} /* related_folder_get_ajax_onclick_function() */

LIST *related_folder_mto1_isa_related_folder_list(
			LIST *existing_related_folder_list,
			char *application_name,
			char *folder_name,
			char *role_name,
			int recursive_level )
{
	RELATED_FOLDER *related_folder;
	LIST *local_related_folder_list;

	if ( !existing_related_folder_list )
	{
		existing_related_folder_list = list_new();
	}

	local_related_folder_list =
		related_folder_mto1_related_folder_list(
			application_name,
			folder_name,
			existing_related_folder_list );

	if (!list_rewind( local_related_folder_list ) )
	{
		return existing_related_folder_list;
	}

	do {
		related_folder =
			list_get_pointer(
				local_related_folder_list );

		if ( !related_folder->relation_type_isa )
		{
			continue;
		}

		if ( timlib_strcmp(
			related_folder->
				one2m_folder->
				folder_name,
			folder_name ) != 0 )
		{
			continue;
		}

		related_folder->recursive_level = recursive_level;

		related_folder->folder->attribute_list =
			folder_attribute_list(
				related_folder->
					folder->
					application_name,
				related_folder->
					folder->
					folder_name,
				role_name );

		related_folder->folder->primary_attribute_name_list =
			attribute_get_primary_attribute_name_list(
				related_folder->
					folder->
					attribute_list );

		folder_load(
			&related_folder->folder->insert_rows_number,
			&related_folder->folder->lookup_email_output,
			&related_folder->folder->row_level_non_owner_forbid,
			&related_folder->folder->row_level_non_owner_view_only,
			&related_folder->folder->populate_drop_down_process,
			&related_folder->folder->post_change_process,
			&related_folder->folder->folder_form,
			&related_folder->folder->notepad,
			&related_folder->folder->html_help_file_anchor,
			&related_folder->folder->
				post_change_javascript,
			&related_folder->folder->lookup_before_drop_down,
			&related_folder->folder->data_directory,
			&related_folder->folder->index_directory,
			&related_folder->folder->no_initial_capital,
			&related_folder->folder->subschema_name,
			&related_folder->folder->create_view_statement,
			application_name,
			BOGUS_SESSION,
			related_folder->folder->folder_name,
			0 /* not override_row_restrictions */,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

		related_folder->foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
				related_folder->
					folder->
						primary_attribute_name_list,
				related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );

		list_append_pointer(	existing_related_folder_list,
					related_folder );

		existing_related_folder_list =
			related_folder_mto1_isa_related_folder_list(
				   existing_related_folder_list,
				   application_name,
				   related_folder->folder->folder_name,
				   role_name,
				   recursive_level + 1 );

	} while( list_next( local_related_folder_list ) );

	return existing_related_folder_list;
}

char *related_folder_mto1_list_display(
			LIST *related_folder_list )
{
	RELATED_FOLDER *related_folder;
	int first_time = 1;
	char buffer[ 65536 ], *buf_ptr = buffer;

	if ( !list_length( related_folder_list ) ) return "";

	list_push( related_folder_list );

	*buf_ptr = '\0';

	list_rewind( related_folder_list );

	do {
		related_folder =
			list_get_pointer(
				related_folder_list );

		if ( first_time )
			first_time = 0;
		else
			buf_ptr += sprintf( buf_ptr, ";" );

		buf_ptr += sprintf(	buf_ptr,
					"%s\n",
					related_folder_display(
						related_folder,
						mto1 ) );

	} while( list_next( related_folder_list ) );

	list_pop( related_folder_list );

	return strdup( buffer );
}

