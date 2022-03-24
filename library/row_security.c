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
#include "element.h"
#include "attribute.h"
#include "query.h"
#include "role_operation.h"
#include "appaserver.h"
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

ROW_SECURITY *row_security_new(
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			LIST *exclude_update_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			boolean role_override_row_restrictions,
			char *login_name,
			char *security_entity_where )
{
	ROW_SECURITY *row_security = row_security_calloc();

	row_security->row_security_role =
		/* --------------------------------- */
		/* Returns null if not participating */
		/* --------------------------------- */
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
			post_change_javascript,
			drillthru_dictionary,
			primary_keys_non_edit,
			role_operation_list,
			ignore_select_attribute_name_list,
			state,
			login_name,
			security_entity_where,
			exclude_update_attribute_name_list,
			exclude_lookup_attribute_name_list,
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

	row_security_role->folder_name = folder_name;

	row_security_role->role_override_row_restrictions =
		role_override_row_restrictions;

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
		free( row_security_role );
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
			return row_security_role_update->attribute_not_null;
		}
		else
		if ( string_strcmp(
			relation->
				many_folder->
				folder_name,
			row_security_role_update->folder_name  ) == 0 )
		{
			return row_security_role_update->attribute_not_null;
		}
	} while ( list_next( update_list ) );

	fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot seek attribute_not_null.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__ );
	exit( 1 );
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
			folder_name );

	return row_security_role_update;
}

RELATION *row_security_role_relation(
			char *folder_name,
			LIST *update_list )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
	RELATION *relation;

	if ( !list_rewind( update_list ) ) return (RELATION *)0;

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
			return row_security_role_update->folder_name;
		}

	} while ( list_next( update_list ) );

	return (char *)0;
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

	if ( !relation->many_folder )
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
			"%s.%s = %s.%s",
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
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			char *login_name,
			char *security_entity_where,
			LIST *exclude_update_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
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

	if ( strcmp( state, ELEMENT_UPDATE_STATE ) == 0 )
	{
		row_security_element_list->regular =
			row_security_element_list_regular_new(
				folder_attribute_append_isa_list,
				relation_mto1_non_isa_list,
				relation_join_one2m_list,
				post_change_javascript,
				drillthru_dictionary,
				primary_keys_non_edit,
				role_operation_list,
				ignore_select_attribute_name_list,
				login_name,
				security_entity_where,
				exclude_update_attribute_name_list,
				exclude_lookup_attribute_name_list,
				row_security_role );

		if ( row_security_role )
		{
			row_security_element_list->viewonly =
				row_security_element_list_viewonly_new(
					folder_attribute_append_isa_list,
					relation_mto1_non_isa_list,
					relation_join_one2m_list,
					role_operation_list,
					ignore_select_attribute_name_list,
					security_entity_where,
					exclude_lookup_attribute_name_list );
		}
	}
	else
	if ( string_strcmp( state, ELEMENT_VIEWONLY_STATE ) == 0 )
	{
		row_security_element_list->viewonly =
			row_security_element_list_viewonly_new(
				folder_attribute_append_isa_list,
				(LIST *)0 /* relation_mto1_non_isa_list */,
				relation_join_one2m_list,
				role_operation_list,
				ignore_select_attribute_name_list,
				security_entity_where,
				exclude_lookup_attribute_name_list );
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
	ROLE_OPERATION *role_operation;
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( role_operation_list ) )
		return element_list;

	do {
		role_operation = 
			list_get(
				role_operation_list );

		list_set(
			element_list,
			appaserver_element_new(
				table_data,
				(char *)0 /* name */ ) );

		if ( viewonly
		&&   operation_delete_boolean(
			role_operation->operation->operation_name ) )
		{
			continue;
		}

		element =
			appaserver_element_new(
				checkbox,
				role_operation->operation->operation_name
					/* element_name */ );

		element->checkbox->element_name =
			role_operation->operation->operation_name;

		element->checkbox->prompt_string =
			role_operation->operation->operation_name;

		if ( operation_delete_boolean(
			role_operation->
				operation->
				operation_name ) )
		{
			element->checkbox->on_click =
				ROW_SECURITY_DELETE_WARNING_JAVASCRIPT;
		}

		list_set( element_list, element );

	} while( list_next( role_operation_list ) );

	return element_list;
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

	while ( string_input( input, input_pipe, 1024 ) )
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

ROW_SECURITY_ELEMENT_LIST_REGULAR *
	row_security_element_list_regular_calloc(
			void )
{
	ROW_SECURITY_ELEMENT_LIST_REGULAR *regular;

	if ( ! ( regular =
		    calloc( 1,
			    sizeof( ROW_SECURITY_ELEMENT_LIST_REGULAR ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return regular;
}

ROW_SECURITY_ELEMENT_LIST_REGULAR *
	row_security_element_list_regular_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *login_name,
			char *security_entity_where,
			LIST *exclude_update_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			/* ------------------------- */
			/* Null if not participating */
			/* ------------------------- */
			ROW_SECURITY_ROLE *row_security_role )
{
	char *attribute_name;
	FOLDER_ATTRIBUTE *folder_attribute;
	ROW_SECURITY_RELATION *row_security_relation;
	ROW_SECURITY_ATTRIBUTE *row_security_attribute;

	ROW_SECURITY_ELEMENT_LIST_REGULAR *regular =
		 row_security_element_list_regular_calloc();

	if ( !list_length( folder_attribute_append_isa_list ) )
	{
		return regular;
	}

	regular->element_list =
		/* -------------- */
		/* Always succeed */
		/* -------------- */
		row_security_operation_element_list(
			role_operation_list,
			0 /* not viewonly */ );

	regular->attribute_name_list =
		/* ------------ */
		/* Will succeed */
		/* ------------ */
		folder_attribute_name_list(
			folder_attribute_append_isa_list );

	list_rewind( regular->attribute_name_list );

	do {
		attribute_name =
			list_get(
				regular->
					attribute_name_list );

		if ( list_string_exists(
			attribute_name,
			ignore_select_attribute_name_list ) )
		{
			continue;
		}

		if ( list_string_exists(
			attribute_name,
			exclude_lookup_attribute_name_list ) )
		{
			continue;
		}

		folder_attribute =
			/* ------------ */
			/* Will succeed */
			/* ------------ */
			folder_attribute_seek(
				attribute_name,
				folder_attribute_append_isa_list );

		if ( folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( primary_keys_non_edit
		&&   folder_attribute->primary_key_index )
		{
			goto skip_relation;
		}

		if ( list_string_exists(
			attribute_name,
			exclude_update_attribute_name_list ) )
		{
			goto skip_relation;
		}

		if ( !list_length( relation_mto1_non_isa_list ) )
		{
			goto skip_relation;
		}

		if ( ( row_security_relation =
			row_security_relation_new(
				attribute_name,
				relation_mto1_non_isa_list,
				post_change_javascript,
				drillthru_dictionary,
				login_name,
				security_entity_where,
				0 /* not_viewonly */,
				regular->row_security_relation_list ) ) )
		{
			if ( !regular->row_security_relation_list )
			{
				regular->
					row_security_relation_list =
						list_new();
			}

			list_set(
				regular->row_security_relation_list,
				row_security_relation );

			list_set_list(
				regular->element_list,
				row_security_relation->element_list );

			continue;
		}

skip_relation:

		if ( ( row_security_attribute =
			row_security_attribute_new(
				attribute_name,
				primary_keys_non_edit,
				folder_attribute->primary_key_index,
				folder_attribute->
					attribute->
					datatype_name,
				folder_attribute->
					attribute->
					width,
				exclude_update_attribute_name_list,
				post_change_javascript,
				regular->row_security_relation_list ) ) )
		{
			if ( !regular->row_security_attribute_list )
			{
				regular->
					row_security_attribute_list =
						list_new();
			}

			list_set(
				regular->row_security_attribute_list,
				row_security_attribute );

			list_set_list(
				regular->element_list,
				row_security_attribute->element_list );
		}

	} while ( list_next( regular->attribute_name_list ) );

	return regular;
}

ROW_SECURITY_RELATION *row_security_relation_calloc( void )
{
	ROW_SECURITY_RELATION *row_security_relation;

	if ( ! ( row_security_relation =
			calloc( 1, sizeof( ROW_SECURITY_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return row_security_relation;
}

ROW_SECURITY_RELATION *row_security_relation_new(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list,
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			boolean viewonly,
			LIST *row_security_relation_list )
{
	ROW_SECURITY_RELATION *row_security_relation;
	RELATION *relation;

	if ( !attribute_name || !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( relation_mto1_non_isa_list ) )
	{
		return (ROW_SECURITY_RELATION *)0;
	}

	if ( row_security_relation_attribute_name_exists(
			attribute_name,
			row_security_relation_list ) )
	{
		return (ROW_SECURITY_RELATION *)0;
	}

	if ( ! ( relation =
			relation_consumes(
				attribute_name,
				relation_mto1_non_isa_list ) ) )
	{
		return (ROW_SECURITY_RELATION *)0;
	}

	relation->consumes_taken = 1;

	if ( !relation->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: relation->one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation->one_folder->folder_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: one_folder->folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( relation->foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_security_relation = row_security_relation_calloc();
	row_security_relation->relation = relation;

	row_security_relation->attribute_name_list =
		row_security_relation_attribute_name_list(
			relation->foreign_key_list );

	row_security_relation->element_list = list_new();

	row_security_relation->table_data_appaserver_element =
		appaserver_element_new(
			table_data,
			(char *)0 /* element_name */ );

	row_security_relation->element_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		row_security_relation_element_name(
			relation->one_folder->folder_name,
			relation->related_attribute_name,
			relation->foreign_key_list );

	row_security_relation->drop_down_appaserver_element =
		appaserver_element_new(
			drop_down,
			row_security_relation->element_name );

	row_security_relation->query_widget =
		query_widget_new(
			relation->one_folder->folder_name
				/* widget_folder_name */,
			login_name,
			relation->one_folder->folder_attribute_list,
			relation->one_folder->relation_mto1_non_isa_list,
			security_entity_where,
			drillthru_dictionary );

	if ( !row_security_relation->query_widget )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_widget_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	free( row_security_relation->
		drop_down_appaserver_element->
		drop_down );

	row_security_relation->drop_down_appaserver_element->drop_down =
		element_drop_down_new(
			row_security_relation->element_name,
			(LIST *)0 /* attribute_name_list */,
			row_security_relation->query_widget->delimited_list,
			(LIST *)0 /* display_list */,
			relation->one_folder->no_initial_capital,
			1 /* output_null_option */,
			1 /* output_not_null_option */,
			1 /* output_select_option */,
			element_drop_down_display_size(
				list_length(
					row_security_relation->
						query_widget->
						delimited_list ) ),
			-1 /* tab_order */,
			0 /* not multi_select */,
			post_change_javascript,
			0 /* not read only */,
			0 /* not recall */ );

	if ( !row_security_relation->drop_down_appaserver_element->drop_down )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: element_drop_down_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_security_relation->element_list = list_new();

	list_set(
		row_security_relation->element_list,
		row_security_relation->table_data_appaserver_element );

	list_set(
		row_security_relation->element_list,
		row_security_relation->drop_down_appaserver_element );

	return row_security_relation;
}

boolean row_security_relation_attribute_name_exists(
			char *attribute_name,
			LIST *row_security_relation_list )
{
	ROW_SECURITY_RELATION *row_security_relation;

	if ( !list_rewind( row_security_relation_list ) ) return 0;

	do {
		row_security_relation =
			list_get(
				row_security_relation_list );

		if ( list_string_exists(
			attribute_name,
			row_security_relation->attribute_name_list ) )
		{
			return 1;
		}

	} while ( list_next( row_security_relation_list ) );

	return 0;
}

LIST *row_security_relation_attribute_name_list(
			LIST *foreign_key_list )
{
	return foreign_key_list;
}

char *row_security_relation_element_name(
			char *one_folder_name,
			char *related_attribute_name,
			LIST *foreign_key_list )
{
	char element_name[ 128 ];

	if ( !one_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ( !related_attribute_name || !*related_attribute_name )
	&&   !list_length( foreign_key_list ) )
	{
		strcpy( element_name, one_folder_name );
	}
	else
	if ( list_length( foreign_key_list ) )
	{
		sprintf(element_name,
			"%s",
			list_display_delimited(
				foreign_key_list,
				',' ) );
	}
	else
	{
		sprintf(element_name,
			"%s (%s)",
			one_folder_name,
			related_attribute_name );
	}

	return strdup( element_name );
}

ROW_SECURITY_ATTRIBUTE *row_security_attribute_calloc( void )
{
	ROW_SECURITY_ATTRIBUTE *row_security_attribute;

	if ( ! ( row_security_attribute =
			calloc( 1, sizeof( ROW_SECURITY_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return row_security_attribute;
}

ROW_SECURITY_ATTRIBUTE *row_security_attribute_new(
			char *attribute_name,
			boolean primary_keys_non_edit,
			int primary_key_index,
			char *datatype_name,
			int attribute_width,
			LIST *exclude_update_attribute_name_list,
			char *post_change_javascript,
			LIST *row_security_relation_list )
{
	ROW_SECURITY_ATTRIBUTE *row_security_attribute;

	if ( !attribute_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !datatype_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: datatype_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( row_security_relation_attribute_name_exists(
		attribute_name,
		row_security_relation_list ) )
	{
		return (ROW_SECURITY_ATTRIBUTE *)0;
	}

	row_security_attribute = row_security_attribute_calloc();

	row_security_attribute->table_data_appaserver_element =
		appaserver_element_new(
			table_data,
			(char *)0 /* element_name */ );

	if ( ( primary_keys_non_edit && primary_key_index )
	||     list_string_exists(
			attribute_name,
			exclude_update_attribute_name_list )
	||     attribute_is_timestamp( datatype_name ) )
	{
		row_security_attribute->attribute_appaserver_element =
			appaserver_element_new(
				non_edit_text,
				attribute_name );

		free( row_security_attribute->
			attribute_appaserver_element->
			non_edit_text );

		row_security_attribute->
			attribute_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					attribute_name,
					(char *)0 /* message */ );

		row_security_attribute->element_non_edit_text =
			row_security_attribute->
				attribute_appaserver_element->
				non_edit_text;
	}
	else
	if ( attribute_is_text( datatype_name ) )
	{
		row_security_attribute->attribute_appaserver_element =
			appaserver_element_new(
				text,
				attribute_name );

		free( row_security_attribute->
			attribute_appaserver_element->
			text );

		row_security_attribute->
			attribute_appaserver_element->
			text =
				element_text_new(
					attribute_name,
					datatype_name,
					attribute_width,
					1 /* null_to_slash */,
					1 /* prevent_carrot */,
					post_change_javascript /* on_change */,
					(char *)0 /* on_focus */,
					(char *)0 /* on_keyup */,
					-1 /* tab_order */,
					0 /* not recall */ );

		row_security_attribute->element_text =
			row_security_attribute->
				attribute_appaserver_element->
				text;
	}
	else
	if ( attribute_is_notepad( datatype_name ) )
	{
		row_security_attribute->attribute_appaserver_element =
			appaserver_element_new(
				notepad,
				attribute_name );

		free( row_security_attribute->
			attribute_appaserver_element->
			notepad );

		row_security_attribute->
			attribute_appaserver_element->
			notepad =
				element_notepad_new(
					attribute_name,
					attribute_width /* attribute_size */,
					ELEMENT_NOTEPAD_COLUMNS,
					ELEMENT_NOTEPAD_ROWS,
					1 /* null_to_slash */,
					post_change_javascript,
					-1 /* tab_order */ );

		row_security_attribute->element_notepad =
			row_security_attribute->
				attribute_appaserver_element->
				notepad;
	}
	else
	if ( attribute_is_yes_no( attribute_name ) )
	{
		row_security_attribute->attribute_appaserver_element =
			appaserver_element_new(
				checkbox,
				attribute_name );

		free( row_security_attribute->
			attribute_appaserver_element->
			checkbox );

		row_security_attribute->
			attribute_appaserver_element->
			checkbox =
				element_checkbox_new(
					attribute_name,
					(char *)0 /* element_name */,
					(char *)0 /* prompt_string */,
					post_change_javascript /* on_click */,
					-1 /* tab_order */,
					(char *)0 /* image_source */,
					0 /* not recall */ );

		row_security_attribute->element_checkbox =
			row_security_attribute->
				attribute_appaserver_element->
				checkbox;
	}
	else
	if ( attribute_is_password( datatype_name ) )
	{
		row_security_attribute->attribute_appaserver_element =
			appaserver_element_new(
				password,
				attribute_name );

		free( row_security_attribute->
			attribute_appaserver_element->
			password );

		row_security_attribute->
			attribute_appaserver_element->
			password =
				element_password_new(
					attribute_name,
					attribute_width
					       /* attribute_width_max_length */,
					ELEMENT_TEXT_MAX_DISPLAY_SIZE,
					-1 /* tab_order */ );

		row_security_attribute->element_password =
			row_security_attribute->
				attribute_appaserver_element->
				password;
	}
	else
	if ( attribute_is_upload( datatype_name ) )
	{
		row_security_attribute->attribute_appaserver_element =
			appaserver_element_new(
				upload,
				attribute_name );

		free( row_security_attribute->
			attribute_appaserver_element->
			upload );

		row_security_attribute->
			attribute_appaserver_element->
			upload =
				element_upload_new(
					attribute_name,
					-1 /* tab_order */,
					0 /* not recall */ );

		row_security_attribute->element_upload =
			row_security_attribute->
				attribute_appaserver_element->
				upload;
	}

	if ( !row_security_attribute->attribute_appaserver_element )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: attribute_appaserver_element is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_security_attribute->element_list = list_new();

	list_set(
		row_security_attribute->element_list,
		row_security_attribute->table_data_appaserver_element );

	list_set(
		row_security_attribute->element_list,
		row_security_attribute->attribute_appaserver_element );

	return row_security_attribute;
}

ROW_SECURITY_ELEMENT_LIST_VIEWONLY *
	row_security_element_list_viewonly_calloc( void )
{
	ROW_SECURITY_ELEMENT_LIST_VIEWONLY *
		row_security_element_list_viewonly;

	if ( ! ( row_security_element_list_viewonly =
		   calloc(
			1,
			sizeof( ROW_SECURITY_ELEMENT_LIST_VIEWONLY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return row_security_element_list_viewonly;
}

ROW_SECURITY_ELEMENT_LIST_VIEWONLY *
	row_security_element_list_viewonly_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *security_entity_where,
			LIST *exclude_lookup_attribute_name_list )
{
	ROW_SECURITY_ELEMENT_LIST_VIEWONLY *
		row_security_element_list_viewonly =
			row_security_element_list_viewonly_calloc();

	return row_security_element_list_viewonly;
}

boolean row_security_role_participating(
			char *row_security_role_root_folder_name,
			RELATION *row_security_role_relation )
{
	if ( !row_security_role_relation ) return 0;

	return
	( strcmp(
		row_security_role_relation->many_folder->folder_name,
		row_security_role_root_folder_name ) == 0 );
}
