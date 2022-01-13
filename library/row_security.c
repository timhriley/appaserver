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
#include "dictionary.h"
#include "role_folder.h"
#include "folder_attribute.h"
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

ROW_SECURITY *row_security_edit_table_new(
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
			char *login_name,
			char *security_entity_where )
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

	/* If supervisor role, then not participating. */
	/* ------------------------------------------- */
	if ( role_override_row_restrictions )
	{
		return (ROW_SECURITY_ROLE *)0;
	}

	row_security_role = row_security_role_calloc();

	row_security_role->update_list =
		row_security_role_update_list();

	if ( !list_length( row_security_role->update_list ) )
	{
		free( row_security_role );
		return (ROW_SECURITY_ROLE *)0;
	}

	row_security_role->root_folder_name =
		row_security_role_root_folder_name(
			folder_name,
			row_security_role->update_list );

	row_security_role->relation =
		row_security_role_relation(
			folder_name,
			row_security_role->update_list );

	row_security_role->participating =
		row_security_role_participating(
			row_security_role->root_folder_name,
			row_security_role->relation );

	if ( !row_security_role->participating )
	{
		return (ROW_SECURITY_ROLE *)0;
	}

	row_security_role->attribute_not_null =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		row_security_role_update_attribute_not_null(
			row_security_role->root_folder_name,
			row_security_role->relation,
			row_security_role->update_list );

	if ( row_security_role->relation )
	{
		row_security_role->join =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			row_security_role_join(
				folder_name,
				primary_key_list,
				row_security_role->relation );
	}

	return row_security_role;
}

char *row_security_role_update_attribute_not_null(
			char *root_folder_name,
			RELATION *relation,
			LIST *update_list )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

	if ( relation && !relation->many_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: many_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		row_security_role_update =
			list_get(
				update_list );

		if ( string_strcmp(
			root_folder_name,
			row_security_role_update->folder_name ) == 0 )
		{
			return row_security_role_update->attribute_not_null );
		}
		else
		if ( string_strcmp(
			relation->
				many_folder->
				folder_name,
			row_security_role_update->folder_name  ) == 0 )
		{
			return row_security_role_update->attribute_not_null );
		}
	} while ( list_next( update_list ) );

	fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot seek attribute_not_null.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__ );
	exit( 1 );
}

boolean row_security_role_viewonly(
			DICTIONARY *row_dictionary,
			char *attribute_not_null )
{
	char *data;

	if ( !attribute_not_null )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: attribute_not_null is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	data =
		dictionary_get(
			row_dictionary,
			attribute_not_null );

	if ( data && *data )
		return 1;
	else
		return 0;
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

RELATION *row_security_role_relation(
			char *folder_name,
			LIST *update_list )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
	RELATION *relation;

	if ( !list_rewind( update_list ) )
		return (RELATION *)0;

	do {
		row_security_role_update =
			list_get(
				update_list );

		if ( !list_rewind( row_security_role_update->
					relation_one2m_recursive_list ) )
		{
			continue;
		}

		do {
			relation =
				list_get(
					row_security_role_update->
						relation_one2m_recursive_list );

			if ( !relation->many_folder )
			{
				fprintf(stderr,
				"ERROR in %s/%s()/%d: many_folder is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( strcmp(	relation->many_folder->folder_name,
					folder_name ) == 0 )
			{
				return relation;
			}

		} while ( list_next(
				row_security_role_update->
					relation_one2m_recursive_list ) );

	} while ( list_next( update_list ) );

	return (RELATION *)0;
}

char *row_security_role_root_folder_name(
			char *folder_name,
			LIST *update_list )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

	if ( !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		row_security_role_update =
			list_get(
				update_list );

		if ( !row_security_role_update->folder_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(	folder_name,
				row_security_role_update->folder_name ) == 0 )
		{
			return 1;
		}

	} while ( list_next( update_list ) );

	return 0;
}

boolean row_security_role_update_participating(
			char *root_folder_name,
			RELATION *relation )
{
	if ( root_folder_name || relation )
		return  1;
	else
		return 0;
}

char *row_security_role_join(
			char *folder_name,
			LIST *primary_key_list,
			RELATION *relation )
{
	char join[ 1024 ];
	char *ptr = join;
	char *primary_key;
	char *foreign_key;

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
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

	if ( !relation->many_folder ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: many_folder is empty.\n",
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
			folder_name,
			primary_key,
			relation->many_folder->folder_name,
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

	if ( !state )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: state is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( state, APPASERVER_UPDATE_STATE ) == 0 )
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

LIST *row_security_operation_element_list(
			LIST *role_operation_list,
			boolean viewonly )
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

		list_set(
			element_list,
			appaserver_element_new(
				table_data,
				(char *)0 /* name */ ) );

		if ( viewonly
		&&   operation_delete_name(
			operation->operation_name ) )
		{
			continue;
		}

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
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role )
{
	if ( !row_security_role )
	{
		if ( regular_element_list )
			return regular_element_list;
		else
			return viewonly_element_list;
	}
	else
	if ( row_security_role_viewonly(
			row_dictionary,
			row_security_role->attribute_not_null ) )
	{
		return viewonly_element_list;
	}
	else
	{
		return regular_element_list;
	}
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

	element_list = list_copy( operation_element_list );

	do {
		attribute_name = list_get( name_list );

		if ( relation =
			relation_consumes(
				attribute_name /* many_attribute_name */,
				relation_mto1_non_isa_list ) )
		{
			QUERY *query;

			if ( !relation->one_folder )
			{
				fprintf(stderr,
				"ERROR in %s/%s()/%d: one_folder is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( !list_length( relation->
						one_folder->
						folder_attribute_list ) )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute_list is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			element =
				appaserver_element_new(
					drop_down );

			free( element->drop_down );

			query =
				query_widget_new(
					relation->one_folder->folder_name
						/* widget_folder_name */,
					login_name,
					relation->
						one_folder->
						folder_attribute_list,
					relation->
						one_folder->
						relation_mto1_non_isa_list,

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

LIST *row_security_role_update_list( void )
{
	LIST *update_list = {0};
	FILE *input_pipe;
	char input[ 1024 ];

	input_pipe =
		popen(
			row_security_role_update_system_string(),
			"r" );

	while ( string_input( input, input_pipe ) )
	{
		if ( !update_list ) update_list = list_new();

		list_set(
			update_list,
			row_security_role_update_parse(
				input ) );
	}

	pclose( input_pipe );

	return update_list;
}

char *row_security_role_update_system_string( void )
{
	static char system_string[ 256 ];

	sprintf(system_string,
		"select.sh %s %s",
		"folder,attribute_not_null",
		"row_security_role_update" );

	return system_string;
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_new(
			char *folder_name,
			char *attribute_not_null );

