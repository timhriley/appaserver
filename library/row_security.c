/* $APPASERVER_HOME/library/row_security.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "dictionary.h"
#include "role_folder.h"
#include "attribute.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "application.h"
#include "list.h"
#include "query.h"
#include "relation.h"
#include "environ.h"
#include "row_security.h"

ROW_SECURITY *row_security_calloc( void )
{
	ROW_SECURITY *row_security;

	if ( ! ( row_security = calloc( 1, sizeof( ROW_SECURITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return row_security;
}

ROW_SECURITY *row_security_edit_table(
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean edit_table_primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			LIST *role_exclude_lookup_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list )
{
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc( void )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

	if ( ! ( row_security_role_update =
			calloc(	1,
				sizeof( ROW_SECURITY_ROLE_UPDATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return row_security_role_update;
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_new(
			char *folder_name,
			LIST *primary_key_list,
			boolean role_override_row_restrictions )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
	ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder;

	if ( ! ( row_security_role_update_folder =
			row_security_role_update_folder_fetch(
				folder_name,
				primary_key_list ) ) )
	{
		return (ROW_SECURITY_ROLE_UPDATE *)0;
	}

	row_security_role_update = row_security_role_update_calloc();

	row_security_role_update->folder_name = folder_name;
	row_security_role_update->primary_key_list = primary_key_list;

	row_security_role_update->role_override_restrictions =
		role_override_restrictions;

	row_security_role_update->row_security_role_update_folder =
		row_security_role_update_folder;

	return row_security_role_update;
}

boolean row_security_role_update_viewonly(
			boolean role_override_restrictions )
{
	return 1 - role_override_restrictions;
}

ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_calloc( void )
{
	ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder;

	if ( ! ( row_security_role_update_folder =
			calloc(	1,
				sizeof( ROW_SECURITY_ROLE_UPDATE_FOLDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return row_security_role_update_folder;
}

char *row_security_role_update_system_string( char *folder_name )
{
	char *select = "folder,attribute_not_null";
	char *from = "row_security_role_update";
	char where[ 128 ];
	static char system_string[ 1024 ];

	sprintf(where,
		"folder = '%s'",
		folder_name );

	sprintf(system_string,
		"select.sh %s %s \"%s\"",
		select,
		from,
		where );

	return system_string );
}

ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_fetch(
			char *folder_name,
			LIST *primary_key_list )
{
	ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder;

	row_security_role_update_folder =
		row_security_role_update_folder_parse(
		string_pipe_fetch(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			row_security_role_update_system_string(
				folder_name ) ) );

	if ( !row_security_role_update_folder )
	{
		return (ROW_SECURITY_ROLE_UPDATE_FOLDER *)0;
	}

	row_security_role_update_folder->folder_name = folder_name;
	row_security_role_update_folder->primary_key_list = primary_key_list;

	return row_security_role_update_folder;
}

ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_parse(
			char *input )
{
	ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder;
	char one_folder_name[ 128 ];
	char attribute_not_null[ 128 ];

	if ( !input || !*input )
	{
		return (ROW_SECURITY_ROLE_UPDATE_FOLDER *)0;
	}

	row_security_role_update_folder =
		row_security_role_update_folder_calloc();

	piece( one_folder_name, SQL_DELIMITER, input, 0 );
	piece( attribute_not_null, SQL_DELIMITER, input, 1 );

	row_security_role_update_folder->one_folder_name =
		strdup( one_folder_name );

	row_security_role_update_folder->attribute_not_null =
		strdup( attribute_not_null );

	row_security_role_update_folder->relation_one2m_recursive_list =
		relation_one2m_recursive_list(
			(LIST *)0 /* relation_list */,
			one_folder_name,
			0 /* not fetch_process */ );

	return row_security_role_update_folder;
}

boolean row_security_role_update_folder_participating(
			char *folder_name,
			char *one_folder_name,
			LIST *relation_one2m_recursive_list )
{
	RELATION *relation;

	if ( !folder_name || !one_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( folder_name, one_folder_name ) == 0 )
	{
		return 1;
	}

	if ( !list_rewind( relation_one2m_recursive_list ) )
	{
		return 0;
	}

	do {

		relation =
			list_get(
				relation_one2m_recursive_list );

		if ( !relation->one_folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(
			folder_name,
			relation->one_folder->folder_name ) == 0 )
		{
			return 1;
		}

	} while ( list_next( relation_one2m_recursive_list ) );

	return 0;
}

char *row_security_role_update_folder_join(
			char *folder_name,
			char *one_folder_name,
			LIST *primary_key_list )
{
	char join[ 1024 ];
	char *ptr = join;
	char *key;

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		key = list_get( primary_key_list );

		if ( ptr != join ) ptr += sprintf( ptr, " and" );

		ptr += sprintf(
			ptr,
			"%s.%s = %s.%",
			folder_name,
			key,
			one_folder_name,
			key );

	} while ( list_next( primary_key_list ) );

	return strdup( join );
}

ROW_SECURITY_ELEMENT_LIST *row_security_element_list_calloc( void )
{
	ROW_SECURITY_ELEMENT_LIST *row_security_element_list;

	if ( ! ( row_security_element_list =
			calloc( 1, sizeof( ROW_SECURITY_ELEMENT_LIST ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return row_security_element_list;
}

ROW_SECURITY_ELEMENT_LIST *row_security_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE_UPDATE *role_update )
{
	ROW_SECURITY_ELEMENT_LIST *row_security_element_list =
		row_security_element_list_calloc();

	if ( string_strcmp( state, APPASERVER_UPDATE_STATE ) == 0 )
	{
		row_security_element_list->regular_element_list =
			row_security_update_state_regular_element_list(
				folder_attribute_append_isa_list,
				relation_mto1_non_isa_list,
				relation_join_one2m_list,
				drillthru_dictionary,
				primary_keys_non_edit,
				role_operation_list,
				ignore_select_attribute_name_list,
				role_exclude_update_attribute_name_list,
				role_exclude_lookup_attribute_name_list,
				role_update );

		if ( role_update )
		{
			row_security_element_list->viewonly_element_list =
				row_security_update_state_viewonly_element_list(
					folder_attribute_append_isa_list,
					relation_mto1_non_isa_list,
					relation_join_one2m_list,
					role_operation_list,
					ignore_select_attribute_name_list,
					role_exclude_lookup_attribute_name_list,
					role_update );
		}
	}
	else
	if ( string_strcmp( state, APPASERVER_VIEW_ONLY_STATE ) == 0 )
	{
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid state of (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			state );
		exit( 1 );
	}

	return row_security_element_list;
}

LIST *row_security_update_state_regular_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE_UPDATE *role_update )
{
	LIST *element_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *folder_attribute_name_list;
	LIST *primary_key_list;

	element_list =
		/* -------------- */
		/* Always returns */
		/* -------------- */
		row_security_operation_element_list(
			role_operation_list );

	return element_list;
}

LIST *row_security_operation_element_list( LIST *role_operation_list )
{
	LIST *element_list = list_new();
	OPERATION *operation;
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( role_operation_list ) )
		return element_list();

	do {
		operation = 
			list_get(
				role_operation_list );

		if ( operation->empty_placeholder )
		{
			element =
				appaserver_element_new(
					blank_tag );
		}
		else
		{
			element =
				appaserver_element_new(
					checkbox );

			element->checkbox->name = operation->operation_name;

			element->checkbox->prompt_string =
				operation->operation_name;

			element->checkbox->action_string =
				operation->delete_warning_javascript;

			element->checkbox->value = "yes";
		}

		list_set( element_list, element );

	} while( list_next( role_operation_list ) );

	return element_list;
}
