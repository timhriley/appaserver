/* $APPASERVER_HOME/library/lookup_before_drop_down.c	   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "form.h"
#include "query.h"
#include "timlib.h"
#include "related_folder.h"
#include "attribute.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "lookup_before_drop_down.h"
#include "dictionary_appaserver.h"

LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down_new(
			char *application_name,
			DICTIONARY *lookup_before_drop_down_dictionary,
			char *state )
{
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;
	FOLDER *folder;
	char *base_folder_name;

	lookup_before_drop_down =
		(LOOKUP_BEFORE_DROP_DOWN *)
			calloc( 1, sizeof( LOOKUP_BEFORE_DROP_DOWN ) );

	if ( ! ( base_folder_name =
			dictionary_get_pointer(
				lookup_before_drop_down_dictionary,
				LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY ) ) )
	{
		return lookup_before_drop_down;
	}

	folder = folder_new(	application_name,
				BOGUS_SESSION,
				base_folder_name );

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

	folder->attribute_list =
		attribute_get_attribute_list(
			folder->application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	folder->pair_one2m_related_folder_list =
		related_folder_get_pair_one2m_related_folder_list(
				folder->application_name,
				folder->folder_name,
				(char *)0 /* role_name */ );

	folder->mto1_lookup_before_drop_down_related_folder_list =
		related_folder_lookup_before_drop_down_related_folder_list(
				list_new() /* related_folder_list */,
				application_name,
				folder->folder_name,
				folder->attribute_list
					/* base_folder_attribute_list */,
				0 /* recursive_level */ );

	lookup_before_drop_down->base_folder = folder;

	lookup_before_drop_down->lookup_before_drop_down_folder_list =
		lookup_before_drop_down_folder_list(
			&lookup_before_drop_down->
				omit_lookup_before_drop_down,
			folder->
			      mto1_lookup_before_drop_down_related_folder_list,
			state );

	lookup_before_drop_down_with_dictionary_set_fulfilled(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		lookup_before_drop_down_dictionary );

	return lookup_before_drop_down;

}

char *lookup_before_drop_down_get_base_folder_name(
		DICTIONARY *lookup_before_drop_down_dictionary )
{
	char *base_folder_name;

	if ( !dictionary_length( lookup_before_drop_down_dictionary ) )
	{
		return (char *)0;
	}

	base_folder_name =
		dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			lookup_before_drop_down_get_base_folder_key() );

	if ( base_folder_name && *base_folder_name )
		return base_folder_name;
	else
		return (char *)0;

}

LIST *lookup_before_drop_down_folder_list(
			boolean *omit_lookup_before_drop_down,
			LIST *mto1_lookup_before_drop_down_related_folder_list,
			char *state )
{
	RELATED_FOLDER *related_folder;
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *lookup_before_drop_down_folder;
	LIST *folder_list = {0};
	LIST *done_folder_name_list;
	LIST *omit_insert_attribute_name_list;
	LIST *omit_update_attribute_name_list;

	if ( !list_rewind( mto1_lookup_before_drop_down_related_folder_list ) )
	{
		*omit_lookup_before_drop_down = 1;
		return (LIST *)0;
	}

	if ( ( *omit_lookup_before_drop_down =
			lookup_before_drop_down_omit(
			  mto1_lookup_before_drop_down_related_folder_list ) ) )
	{
		return (LIST *)0;
	}

	done_folder_name_list = list_new();

	list_go_tail( mto1_lookup_before_drop_down_related_folder_list );

	do {
		related_folder =
			list_get_pointer(
			     mto1_lookup_before_drop_down_related_folder_list );

		if ( !related_folder->folder->lookup_before_drop_down )
			continue;

		if ( related_folder->automatic_preselection )
			continue;

		if ( list_exists_string(
			related_folder->
				folder->
				folder_name,
			done_folder_name_list ) )
		{
			continue;
		}

		/* ---------------------------------------------- */
		/* If inserting, don't preprompt if not inserting */
		/* all the foreign key attributes.		  */
		/* ---------------------------------------------- */
		omit_insert_attribute_name_list =
			attribute_omit_insert_attribute_name_list(
				related_folder->
					one2m_folder->
					attribute_list );

		if ( timlib_strcmp( state, "insert" ) == 0
		&&   list_length( omit_insert_attribute_name_list ) )
		{
			LIST *subtract_list;

			subtract_list =
				list_subtract(
					related_folder->
					       foreign_attribute_name_list,
					omit_insert_attribute_name_list );

			if (	list_length( subtract_list ) !=
				list_length( 
					related_folder->
					       foreign_attribute_name_list ) )
			{
				continue;
			}
		}

		omit_update_attribute_name_list =
			attribute_omit_update_attribute_name_list(
				related_folder->
					one2m_folder->
					attribute_list );

		if ( list_length( omit_update_attribute_name_list ) )
		{
			LIST *subtract_list;

			subtract_list =
				list_subtract(
					related_folder->
					       foreign_attribute_name_list,
					omit_update_attribute_name_list );

			if (	list_length( subtract_list ) !=
				list_length( 
					related_folder->
					       foreign_attribute_name_list ) )
			{
				continue;
			}
		}

		lookup_before_drop_down_folder =
			lookup_before_drop_down_folder_new(
				related_folder->
					folder->
					folder_name );

		if ( !folder_list ) folder_list = list_new();

		lookup_before_drop_down_append_folder_list(
			folder_list,
			lookup_before_drop_down_folder );

		list_append_pointer(
			done_folder_name_list,
			related_folder->folder->folder_name );

	} while( list_previous(
			mto1_lookup_before_drop_down_related_folder_list ) );

	return folder_list;

}

boolean lookup_before_drop_down_omit(
			LIST *mto1_lookup_before_drop_down_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind(
		mto1_lookup_before_drop_down_related_folder_list ) )
	{
		return 0;
	}

	do {
		related_folder =
			list_get_pointer( 
			     mto1_lookup_before_drop_down_related_folder_list );

		if ( related_folder->recursive_level == 0
		&&   !related_folder->omit_lookup_before_drop_down )
		{
			return 0;
		}
	} while( list_next(
			mto1_lookup_before_drop_down_related_folder_list ) );

	return 1;

}

void lookup_before_drop_down_with_dictionary_set_fulfilled(
		LIST *lookup_before_drop_down_folder_list,
		DICTIONARY *lookup_before_drop_down_dictionary )
{
	char *fulfilled_folder_name_list_string;
	LIST *fulfilled_folder_name_list;

	if ( !lookup_before_drop_down_dictionary ) return;

	fulfilled_folder_name_list_string =
		dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY );

	fulfilled_folder_name_list =
		list_string2list(
			fulfilled_folder_name_list_string,
			',' );

	lookup_before_drop_down_with_folder_name_list_set_fulfilled(
		lookup_before_drop_down_folder_list,
		fulfilled_folder_name_list );

}

LOOKUP_BEFORE_DROP_DOWN_FOLDER *lookup_before_drop_down_folder_list_seek(
		LIST *lookup_before_drop_down_folder_list,
		char *folder_name )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) )
		return (LOOKUP_BEFORE_DROP_DOWN_FOLDER *)0;

	do {
		folder =
			list_get_pointer(
				lookup_before_drop_down_folder_list );

		if ( timlib_strcmp(
				folder->folder_name,
				folder_name ) == 0 )
		{
			return folder;
		}
	} while( list_next( lookup_before_drop_down_folder_list ) );

	return (LOOKUP_BEFORE_DROP_DOWN_FOLDER *)0;

}

void lookup_before_drop_down_with_folder_name_list_set_fulfilled(
		LIST *lookup_before_drop_down_folder_list,
		LIST *folder_name_list )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) ) return;

	do {
		folder =
			list_get_pointer(
				lookup_before_drop_down_folder_list );

		if ( list_exists_string(
				folder->folder_name,
				folder_name_list ) )
		{
			folder->fulfilled = 1;
		}
	} while( list_next( lookup_before_drop_down_folder_list ) );
}

boolean lookup_before_drop_down_is_fulfilled_folder(
		LIST *lookup_before_drop_down_folder_list,
		char *folder_name )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) ) return 0;

	do {
		folder = list_get_pointer(
				lookup_before_drop_down_folder_list );

		if ( strcmp( folder->folder_name, folder_name ) == 0 )
			return folder->fulfilled;

	} while( list_next( lookup_before_drop_down_folder_list ) );

	return 0;

}

LIST *lookup_before_drop_down_get_folder_name_list(
			LIST *lookup_before_drop_down_folder_list )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;
	LIST *folder_name_list;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) )
		return (LIST *)0;

	folder_name_list = list_new();

	do {
		folder = list_get_pointer(
				lookup_before_drop_down_folder_list );

		list_append_pointer(	folder_name_list,
					folder->folder_name );

	} while( list_next( lookup_before_drop_down_folder_list ) );

	return folder_name_list;

}

LIST *lookup_before_drop_down_get_unfulfilled_folder_name_list(
		LIST *lookup_before_drop_down_folder_list )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;
	LIST *unfulfilled_folder_name_list;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) )
		return (LIST *)0;

	unfulfilled_folder_name_list = list_new();

	do {
		folder = list_get_pointer(
				lookup_before_drop_down_folder_list );

		if ( !folder->fulfilled )
		{
			list_append_pointer(	unfulfilled_folder_name_list,
						folder->folder_name );
		}
	} while( list_next( lookup_before_drop_down_folder_list ) );

	return unfulfilled_folder_name_list;

}

char *lookup_before_drop_down_get_unfulfilled_folder_name(
		LIST *lookup_before_drop_down_folder_list )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) )
		return (char *)0;

	do {
		folder = list_get_pointer(
				lookup_before_drop_down_folder_list );

		if ( !folder->fulfilled ) return folder->folder_name;

	} while( list_next( lookup_before_drop_down_folder_list ) );

	return (char *)0;

}

LOOKUP_BEFORE_DROP_DOWN_FOLDER *lookup_before_drop_down_folder_new(
					char *folder_name )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *lookup_before_drop_down_folder;

	lookup_before_drop_down_folder =
		(LOOKUP_BEFORE_DROP_DOWN_FOLDER *)
			calloc( 1, sizeof( LOOKUP_BEFORE_DROP_DOWN_FOLDER ) );

	lookup_before_drop_down_folder->folder_name = folder_name;

	return lookup_before_drop_down_folder;
}

char *lookup_before_drop_down_get_dictionary_base_name(
		DICTIONARY *lookup_before_drop_down_dictionary )
{
	return dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY );

}

char *lookup_before_drop_down_get_dictionary_insert_folder_name(
		DICTIONARY *lookup_before_drop_down_dictionary )
{
	return dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_INSERT_FOLDER_KEY );

}

void lookup_before_drop_down_set_dictionary_insert_folder_name(
		DICTIONARY *lookup_before_drop_down_dictionary,
		char *insert_folder_name )
{
	dictionary_set_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_INSERT_FOLDER_KEY,
			insert_folder_name );

}

void lookup_before_drop_down_set_dictionary_base_name(
		DICTIONARY *lookup_before_drop_down_dictionary,
		char *base_folder_name )
{
	dictionary_set_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY,
			base_folder_name );

}

char *lookup_before_drop_down_get_base_folder_key( void )
{
	char base_folder_key[ 128 ];

	sprintf(base_folder_key,
		"%s",
		LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY );
	return strdup( base_folder_key );
}

void lookup_before_drop_down_append_fulfilled_folder_name(
		DICTIONARY *lookup_before_drop_down_dictionary,
		char *fulfilled_folder_name )
{
	char *fulfilled_folder_name_list_string;
	char new_fulfilled_folder_name_list_string[ 1024 ];

	if ( ( fulfilled_folder_name_list_string =
		dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY ) ) )
	{
		sprintf(new_fulfilled_folder_name_list_string,
			"%s,%s",
			fulfilled_folder_name_list_string,
			fulfilled_folder_name );

		dictionary_set_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY,
			strdup( new_fulfilled_folder_name_list_string ) );
	}
	else
	{
		dictionary_set_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY,
			fulfilled_folder_name );
	}
}

boolean lookup_before_drop_down_exists_unfulfilled_folder(
				LIST *lookup_before_drop_down_folder_list )
{
	char *unfulfilled_folder_name;

	if ( ( unfulfilled_folder_name =
		lookup_before_drop_down_get_unfulfilled_folder_name(
			lookup_before_drop_down_folder_list ) ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void lookup_before_drop_down_add_fulfilled_folder(
			DICTIONARY *lookup_before_drop_down_dictionary,
			char *folder_name )
{
	char *fulfilled_folder_name_list_string;
	LIST *fulfilled_folder_name_list;

	if ( !lookup_before_drop_down_dictionary ) return;

	fulfilled_folder_name_list_string =
		dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY );

	fulfilled_folder_name_list =
		list_string2list(
			fulfilled_folder_name_list_string,
			',' );

	list_append_unique_string( fulfilled_folder_name_list, folder_name );

	dictionary_set_pointer(
		lookup_before_drop_down_dictionary,
		LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY,
		list_display_delimited(
			fulfilled_folder_name_list,
			',' ) );

}

char *lookup_before_drop_down_dictionary_get_base_folder_name(
			DICTIONARY *lookup_before_drop_down_dictionary )
{
	char *base_folder_name;

	if ( ! ( base_folder_name =
		dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY ) ) )
	{
		return (char *)0;
	}

	return base_folder_name;

}

void lookup_before_drop_down_set_fulfilled_folder_name(
			LIST *lookup_before_drop_down_folder_list,
			char *folder_name )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) ) return;

	do {
		folder =
			list_get_pointer(
				lookup_before_drop_down_folder_list );

		if ( strcmp( folder->folder_name, folder_name ) == 0 )
		{
			folder->fulfilled = 1;
			return;
		}
	} while( list_next( lookup_before_drop_down_folder_list ) );

}

void lookup_before_drop_down_set_dictionary_fulfilled(
			DICTIONARY *dictionary,
			LIST *lookup_before_drop_down_folder_list )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;
	LIST *fulfilled_folder_name_list;

	if ( !list_rewind( lookup_before_drop_down_folder_list ) ) return;

	fulfilled_folder_name_list = list_new();

	do {
		folder =
			list_get_pointer(
				lookup_before_drop_down_folder_list );

		if ( folder->fulfilled )
		{
			list_append_pointer(
				fulfilled_folder_name_list,
				folder->folder_name );
		}
	} while( list_next( lookup_before_drop_down_folder_list ) );

	dictionary_set_pointer(
			dictionary,
			LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY,
			list_display_delimited(
				fulfilled_folder_name_list,
				',' ) );

}

char *lookup_before_drop_down_display(
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder;

	*buf_ptr = '\0';

	if ( !lookup_before_drop_down )
	{
		buf_ptr += sprintf(
			buf_ptr,
			"\n%s(): empty lookup_before_drop_down\n",
			__FUNCTION__ );

		return strdup( buffer );
	}

	if ( lookup_before_drop_down->base_folder )
	{
		buf_ptr += sprintf(
			buf_ptr,
			"\n%s(): base = %s\n",
			__FUNCTION__,
			lookup_before_drop_down->base_folder->folder_name );
	}
	else
	{
		buf_ptr += sprintf(
			buf_ptr,
			"\n%s(): no base set\n",
			__FUNCTION__ );
	}

	buf_ptr += sprintf(
			buf_ptr,
			"omit_lookup_before_drop_down = %d\n",
			lookup_before_drop_down->omit_lookup_before_drop_down );

	if ( list_length(
		lookup_before_drop_down->
			base_folder->
			mto1_lookup_before_drop_down_related_folder_list ) )
	{
		buf_ptr += sprintf(
			buf_ptr,
		"%s(): mto1_lookup_before_drop_down_related_folder_list = %s\n",
			__FUNCTION__,
			related_folder_list_display(
			lookup_before_drop_down->
			   base_folder->
			   mto1_lookup_before_drop_down_related_folder_list,
			mto1,
			'\n' ) );
	}
	else
	{
		buf_ptr += sprintf(
			buf_ptr,
		"%s(): no mto1_lookup_before_drop_down_related_folder_list\n",
			__FUNCTION__ );
	}

	buf_ptr += sprintf(
			buf_ptr,
			"\n%s(): lookup_before_drop_down_folder_list:",
			__FUNCTION__ );

	if ( list_rewind( lookup_before_drop_down->
				lookup_before_drop_down_folder_list ) )
	{
		int first_time = 1;
		do {
			folder =
				list_get_pointer(
					lookup_before_drop_down->
					lookup_before_drop_down_folder_list );

			if ( first_time )
				first_time = 0;
			else
				buf_ptr += sprintf( buf_ptr, ";" );

			buf_ptr += sprintf(
					buf_ptr,
					" folder = %s, fulfilled = %d",
					folder->folder_name,
					folder->fulfilled );
		} while( list_next( lookup_before_drop_down->
					lookup_before_drop_down_folder_list ) );
	}

	return strdup( buffer );

}

void lookup_before_drop_down_append_folder_list(
			LIST *lookup_before_drop_down_folder_list,
			LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder )
{
	LOOKUP_BEFORE_DROP_DOWN_FOLDER *existing_folder;

	if ( list_rewind( lookup_before_drop_down_folder_list ) )
	{
		do {
			existing_folder =
				list_get_pointer(
					lookup_before_drop_down_folder_list );

			if ( strcmp(	folder->folder_name,
					existing_folder->folder_name ) == 0 )
			{
				return;
			}

		} while( list_next( lookup_before_drop_down_folder_list ) );
	}
	list_append_pointer(
			lookup_before_drop_down_folder_list,
			folder );

}

enum lookup_before_drop_down_state
	lookup_before_drop_down_get_state(
		LIST *lookup_before_drop_down_folder_list,
		DICTIONARY *lookup_before_drop_down_dictionary,
		DICTIONARY *preprompt_dictionary,
		boolean folder_lookup_before_drop_down )
{
	enum lookup_before_drop_down_state lookup_before_drop_down_state;
	char *state_string;
	char *base_folder_name;
	char *unfulfilled_folder_name;

	unfulfilled_folder_name =
		lookup_before_drop_down_get_unfulfilled_folder_name(
			lookup_before_drop_down_folder_list );

	if ( !list_length( lookup_before_drop_down_folder_list ) )
	{
		if ( folder_lookup_before_drop_down )
			return lookup_skipped;
		else
			return lookup_not_participating;
	}

	state_string =
		dictionary_fetch(
			LOOKUP_BEFORE_DROP_DOWN_STATE_KEY,
			lookup_before_drop_down_dictionary );

	if ( timlib_strcmp(	state_string,
				LOOKUP_BEFORE_DROP_DOWN_SKIPPED_STATE ) == 0 )
	{
		return lookup_skipped;
	}

	base_folder_name =
		dictionary_fetch(
			LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY,
			lookup_before_drop_down_dictionary );

	if ( !base_folder_name )
	{
		lookup_before_drop_down_state = lookup_not_participating;
	}
	else
	if ( timlib_strcmp(
			state_string,
			LOOKUP_BEFORE_DROP_DOWN_INITIAL_STATE ) == 0 )
	{
		lookup_before_drop_down_state = lookup_participating_not_root;
	}
	else
	if ( timlib_strcmp(
			state_string,
			LOOKUP_BEFORE_DROP_DOWN_NON_INITIAL_STATE ) == 0
	&&   lookup_before_drop_down_preprompt_dictionary_empty(
			preprompt_dictionary ) )
	{
		lookup_before_drop_down_state = lookup_skipped;
	}
	else
	if ( unfulfilled_folder_name && *unfulfilled_folder_name )
	{
		lookup_before_drop_down_state = lookup_participating_not_root;
	}
	else
	{
		lookup_before_drop_down_state =
			lookup_participating_is_root_all_complete;
	}

	return lookup_before_drop_down_state;

}

char *lookup_before_drop_down_state_display(
			enum lookup_before_drop_down_state
				lookup_before_drop_down_state )
{
	return lookup_before_drop_down_get_state_string(
			lookup_before_drop_down_state );
}

char *lookup_before_drop_down_get_state_string(
			enum lookup_before_drop_down_state
				lookup_before_drop_down_state )
{
	char *return_string;

	if ( lookup_before_drop_down_state == lookup_not_participating )
		return_string = "lookup_not_participating";
	else
	if ( lookup_before_drop_down_state == lookup_skipped )
		return_string = "lookup_skipped";
	else
	if ( lookup_before_drop_down_state ==
		lookup_participating_not_root )
		return_string = "lookup_participating_not_root";
	else
	if ( lookup_before_drop_down_state ==
		lookup_participating_is_root_all_complete )
		return_string = "lookup_participating_is_root_all_complete";
	else
	{
		char buffer[ 128 ];

		sprintf(buffer,
		"ERROR: got unrecognized lookup_before_drop_down_state = %d", 
			lookup_before_drop_down_state );
		return_string = strdup( buffer );
	}

	return return_string;

}

boolean lookup_before_drop_down_first_prelookup(
			DICTIONARY *lookup_before_drop_down_dictionary )
{
	if ( ! dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY ) )
	{
		return 0;
	}

	if ( !lookup_before_drop_down_dictionary ) return 0;

	if ( dictionary_get_pointer(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY ) )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

char *lookup_before_drop_down_get_insert_pair_base_folder_name(
			char *application_name,
			char *base_folder_name,
			LIST *pair_one2m_related_folder_list )
{
	RELATED_FOLDER *pair_related_folder;
	RELATED_FOLDER *related_folder;
	LIST *related_folder_list;

	if ( !list_rewind( pair_one2m_related_folder_list ) ) return (char *)0;

	do {
		pair_related_folder =
			list_get_pointer(
				pair_one2m_related_folder_list );

		related_folder_list =
			related_folder_get_mto1_related_folder_list(
				list_new_list(),
				application_name,
				(char *)0 /* session */,
				pair_related_folder->
					one2m_folder->
					folder_name,
				(char *)0 /* role_name */,
				0 /* isa_flag */,
				related_folder_no_recursive,
				1 /* override_row_restrictions */,
				(LIST *)0 /* root_primary_..._name_list */,
				0 /* recursive_level */ );

		if ( !list_rewind( related_folder_list ) ) continue;

		do {
			related_folder =
				list_get_pointer(
					related_folder_list );

			if ( strcmp(	related_folder->folder->folder_name,
					base_folder_name ) != 0
			&&   !related_folder->automatic_preselection
			&&   related_folder->folder->lookup_before_drop_down )
			{
				return	pair_related_folder->
						one2m_folder->
						folder_name;
			}

		} while( list_next( related_folder_list ) );
	} while( list_next( pair_one2m_related_folder_list ) );

	return (char *)0;
}

LIST *lookup_before_drop_down_get_non_edit_folder_name_list(
				LIST *lookup_before_drop_down_folder_list,
				LIST *mto1_related_folder_list )
{
	LIST *lookup_before_drop_down_folder_name_list;
	LIST *non_edit_folder_name_list;
	RELATED_FOLDER *related_folder;

	lookup_before_drop_down_folder_name_list =
		lookup_before_drop_down_get_folder_name_list(
			lookup_before_drop_down_folder_list );

	if ( !list_rewind( mto1_related_folder_list ) )
		return (LIST *)0;

	non_edit_folder_name_list = list_new();

	do {
		related_folder = list_get( mto1_related_folder_list );

		if ( list_length( related_folder->
					folder->
					primary_attribute_name_list ) > 1
		&&   list_exists_string(
			related_folder->folder->folder_name,
			lookup_before_drop_down_folder_name_list ) )
		{
			list_append_pointer(
				non_edit_folder_name_list,
				related_folder->folder->folder_name );
		}
	} while( list_next( mto1_related_folder_list ) );

	return non_edit_folder_name_list;
}

boolean lookup_before_drop_down_preprompt_dictionary_empty(
				DICTIONARY *preprompt_dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	enum relational_operator relational_operator;

	key_list = dictionary_get_key_list( preprompt_dictionary );

	if ( !list_rewind( key_list ) ) return 1;

	do {
		key = list_get_pointer( key_list );

		if ( timlib_strncmp(
			key,
			QUERY_RELATION_OPERATOR_STARTING_LABEL ) == 0 )
		{
			data = dictionary_safe_fetch(
					preprompt_dictionary,
					key );

			relational_operator =
				query_get_relational_operator(
					data );

			if ( relational_operator != equals ) return 0;
		}
		else
		{
			return 0;
		}
	} while( list_next( key_list ) );

	return 1;
}

