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
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "dictionary.h"
#include "role_folder.h"
#include "folder_attribute.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "application.h"
#include "relation.h"
#include "environ.h"
#include "query.h"
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
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			boolean folder_non_owner_forbid,
			boolean role_override_row_restrictions,
			char *login_name )
{
	ROW_SECURITY *row_security = row_security_calloc();

	row_security->row_security_role =
		/* ---------------------------------- */
		/* Returns null if not participating. */
		/* ---------------------------------- */
		row_security_role_new(
			folder_name,
			folder_attribute_primary_key_list(
				folder_attribute_append_isa_list ),
			role_override_row_restrictions );

	row_security->row_security_element_list =
		row_security_element_list_new(
			folder_attribute_append_isa_list,
			relation_mto1_non_isa_list,
			relation_join_one2m_list,
			drillthru_dictionary,
			primary_keys_non_edit,
			role_operation_list,
			ignore_select_attribute_name_list,
			state,
			login_name,
			security_entity_new(
				login_name,
				folder_non_owner_forbid,
				role_override_row_restrictions ),
			role_exclude_update_attribute_name_list,
			role_exclude_lookup_attribute_name_list,
			row_security->row_security_role );

	return row_security;
}

ROW_SECURITY_ROLE *row_security_role_calloc( void )
{
	ROW_SECURITY_ROLE *row_security_role;

	if ( ! ( row_security_role =
			calloc(
				1,
				sizeof( ROW_SECURITY_ROLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return row_security_role;
}

ROW_SECURITY_ROLE *row_security_role_new(
			char *folder_name,
			LIST *primary_key_list,
			boolean role_override_row_restrictions )
{
	ROW_SECURITY_ROLE *row_security_role;
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

	/* If supervisor role, then not participating. */
	/* ------------------------------------------- */
	if ( role_override_row_restrictions )
	{
		return (ROW_SECURITY_ROLE *)0;
	}

	row_security_role_update =
		row_security_role_update_fetch(
			folder_name /* check_folder_name */,
			primary_key_list );

	/* If no fetch, then not participating. */
	/* ------------------------------------ */
	if ( !row_security_role_update ) )
	{
		return (ROW_SECURITY_ROLE *)0;
	}

	row_security_role = row_security_role_calloc();

	row_security_role_update->row_security_role_update =
		row_security_role_update;

	return row_security_role;
}

boolean row_security_role_viewonly(
			char *delimited_string,
			ROW_SECURITY_ROLE *row_security_role )
{
	if ( !row_security_role ) return 0;

	/* attribute_not_null must be the last attribute selected. */
	/* ------------------------------------------------------- */
	return ( string_last_character( delimited_string ) == SQL_DELIMITER );
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc( void )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_folder;

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

char *row_security_role_system_string( void )
{
	char *select = "folder,attribute_not_null";
	char *from = "row_security_role_update";
	static char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh %s %s \"%s\"",
		select,
		from,
		where );

	return system_string;
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_fetch(
			char *check_folder_name,
			LIST *primary_key_list )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
	LIST *role_update_list;

	if ( !check_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: check_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	role_update_list =
		row_security_role_update_list(
			primary_key_list );

	if ( !list_rewind( role_update_list ) )
	{
		return (ROW_SECURITY_ROLE_UPDATE *)0;
	}

	do {
		row_security_role_update =
			list_get(
				role_update_list );

		if ( row_security_role_update_participating(
			check_folder_name,
			row_security_role_update->folder_name
			row_security_role_update->
				relation_one2m_recursive_list ) )
		{
			if ( strcmp(
				row_security_role_update->folder_name,
				check_folder_name ) != 0 )
			{
				row_security_role_update->join =
					row_security_role_update_join(
					   check_folder_name,
					   primary_key_list,
					   row_security_role_update->
					      relation_one2m_recursive_list );
			}

			return row_security_role_update;
		}

	} while ( list_next( role_update_list ) );

	return (ROW_SECURITY_ROLE_UPDATE *)0;
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_parse(
			char *input )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
	char folder_name[ 128 ];
	char attribute_not_null[ 128 ];

	if ( !input || !*input )
	{
		return (ROW_SECURITY_ROLE_UPDATE *)0;
	}

	row_security_role_update =
		row_security_role_update_calloc();

	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( attribute_not_null, SQL_DELIMITER, input, 1 );

	row_security_role_update->folder_name = strdup( folder_name );

	row_security_role_update->attribute_not_null =
		strdup( attribute_not_null );

	row_security_role_update->relation_one2m_recursive_list =
		relation_one2m_recursive_list(
			(LIST *)0 /* relation_list */,
			folder_name,
			0 /* not fetch_process */ );

	return row_security_role_update;
}

boolean row_security_role_update_participating(
			char *check_folder_name,
			char *folder_name,
			LIST *relation_one2m_recursive_list )
{
	if ( !check_folder_name || !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( check_folder_name, folder_name ) == 0 )
	{
		return 1;
	}

	if ( ( relation_one2m_seek(
			check_folder_name,
			relation_one2m_recursive_list ) ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *row_security_role_update_join(
			char *check_folder_name,
			LIST *primary_key_list,
			LIST *relation_one2m_recursive_list )
{
	char join[ 1024 ];
	char *ptr = join;
	char *primary_key;
	char *foreign_key;
	RELATION *relation;

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( relation =
			relation_one2m_seek(
				check_folder_name,
				relation_one2m_recursive_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: relation_one2m_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			check_folder_name );
		exit( 1 );
	}

	if ( !list_rewind( relation->foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		primary_key = list_get( primary_key_list );
		foreign_key = list_get( relation->foreign_key_list );

		if ( ptr != join ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s.%s = %s.%",
			check_folder_name,
			primary_key,
			relation->one_folder->folder_name,
			foreign_key );

		list_next( relation->foreign_key_list );

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

ROW_SECURITY_ELEMENT_LIST *row_security_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			char *login_name,
			SECURITY_ENTITY *security_entity,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			/* ------------------------- */
			/* Null if not participating */
			/* ------------------------- */
			ROW_SECURITY_ROLE *row_security_role )
{
	ROW_SECURITY_ELEMENT_LIST *row_security_element_list =
		row_security_element_list_calloc();

	if ( string_strcmp( state, APPASERVER_UPDATE_STATE ) == 0 )
	{
		row_security_element_list->regular_element_list =
			row_security_regular_element_list(
				folder_attribute_append_isa_list,
				relation_mto1_non_isa_list,
				relation_join_one2m_list,
				drillthru_dictionary,
				primary_keys_non_edit,
				role_operation_list,
				ignore_select_attribute_name_list,
				login_name,
				security_entity,
				role_exclude_update_attribute_name_list,
				role_exclude_lookup_attribute_name_list,
				row_security_role );

		if ( row_security_role )
		{
			row_security_element_list->viewonly_element_list =
				row_security_viewonly_element_list(
					folder_attribute_append_isa_list,
					relation_mto1_non_isa_list,
					relation_join_one2m_list,
					role_operation_list,
					ignore_select_attribute_name_list,
					role_exclude_lookup_attribute_name_list,
					row_security_role );
		}
	}
	else
	if ( string_strcmp( state, APPASERVER_VIEWONLY_STATE ) == 0 )
	{
		row_security_element_list->viewonly_element_list =
			row_security_viewonly_element_list(
				folder_attribute_append_isa_list,
				(LIST *)0 /* relation_mto1_non_isa_list */,
				relation_join_one2m_list,
				role_operation_list,
				ignore_select_attribute_name_list,
				role_exclude_lookup_attribute_name_list,
				row_security_role );
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

LIST *row_security_operation_element_list( LIST *role_operation_list )
{
	LIST *element_list = list_new();
	OPERATION *operation;
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( role_operation_list ) )
		return element_list;

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

LIST *row_security_role_update_folder_list(
			LIST *primary_key_list )
{
	LIST *role_update_folder_list = {0};
	char input[ 256 ];
	FILE *input_pipe =
		popen(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			row_security_role_update_system_string(),
			"r" );

	while( string_input( input, input_pipe, 256 ) )
	{
		row_security_role_update_folder =

		if ( !update_folder_list ) update_folder_list = list_new();

		list_set(
			role_update_folder_list,
			row_security_role_update_folder_parse( input ) );
	}

	pclose( input_pipe );
	return role_update_folder_list;
}

LIST *row_security_viewonly_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE *row_security_role )
{
}

LIST *row_security_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			char *delimited_string,
			ROW_SECURITY_ROLE *row_security_role )
{
	LIST *apply_element_list;

	if ( row_security_role
	&&   row_security_role_viewonly(
			delimited_string,
			row_security_role ) )
	{
		apply_element_list = viewonly_element_list;
	}
	else
	if ( regular_element_list )
	{
		apply_element_list = regular_element_list;
	}
	else
	{
		apply_element_list = viewonly_element_list;
	}

	return apply_element_list;
}

LIST *row_security_regular_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *login_name,
			SECURITY_ENTITY *security_entity,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE *row_security_role )
{
	LIST *element_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	RELATION *relation;
	APPASERVER_ELEMENT *element;
	LIST *name_list;
	char *attribute_name;
	LIST *done_attribute_name_list = list_new();

	name_list =
		folder_attribute_name_list(
			folder_attribute_append_isa_list,
			(char *)0 /* folder_name */ );

	if ( !list_rewind( name_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_name_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_list =
		/* -------------- */
		/* Always returns */
		/* -------------- */
		row_security_operation_element_list(
			role_operation_list );

	do {
		attribute_name = list_get( name_list );

		if ( relation =
			relation_consumes(
				attribute_name /* many_attribute_name */,
				relation_mto1_non_isa_list ) )
		{
			LIST *delimited_list =
				row_security_widget_delimited_list(
					relation->one_folder->folder_name
						/* widget_folder_name */,
					login_name,
					folder_attribute_primary_key_list(
						relation->
					LIST *relation_mto1_non_isa_list,
					char *security_entity_where,
					char *one_folder_name,
					DICTIONARY *drillthru_dictionary,
					PROCESS *populate_drop_down_process );

			element =
				appaserver_element_new(
					drop_down );

			free( element->drop_down );

			query =
				query_widget_new(
					relation->one_folder->folder_name,
					login_name,
			element->drop_down =
				element_drop_down_new(
					relation->one_folder->primary_key_list
						/* attribute_name_list */,
					row_security_widget_delimited_list

					query_primary_delimited_list(
		}

	} while ( list_next( name_list ) );

	return element_list;
}

{
if ( folder->populate_drop_down_process )
{
	delimited_list =
		list_pipe_fetch(
			process_choose_isa_command_line(
				folder->
					populate_drop_down_process->
					command_line,
				security_entity_where(
					security_entity,
					folder->folder_attribute_list ),
				application_name,
				login_name,
				role_name,
				one2m_isa_folder_name  ) );
}

LIST *row_security_widget_delimited_list(
			char *widget_folder_name,
			char *login_name,
			LIST *primary_key_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			char *one_folder_name,
			DICTIONARY *drillthru_dictionary,
			PROCESS *populate_drop_down_process )
{
	LIST *delimited_list;

	if ( !populate_drop_down_process )
	{
		QUERY *query =
			query_widget_new(
				char *widget_folder_name,
				char *login_name,
				LIST *folder_attribute_primary_key_list(),
				LIST *folder_attribute_list(),
				LIST *relation_mto1_non_isa_list,
				char *security_entity_where(),
				DICTIONARY *drillthru_dictionary );

		delimited_list =
			query_delimited_list(
				query->select_clause,
				query->from_clause,
				query->where_clause,
				query->order_clause,
				0 /* max_rows */ );
	}
	else
	{
		delimited_list =
			list_pipe_fetch(
				process_widget_command_line(
					populate_drop_down_process->
						command_line,
					application_name,
					security_entity_where,
					login_name,
					role_name,
					one2m_isa_folder_name  ) );
	}

	return delimited_list;
}
