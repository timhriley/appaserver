/* $APPASERVER_HOME/library/update.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "attribute.h"
#include "folder.h"
#include "role_folder.h"
#include "relation.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "sql.h"
#include "appaserver_parameter.h"
#include "dictionary_separate.h"
#include "update.h"

UPDATE *update_calloc( void )
{
	UPDATE *update;

	if ( ! ( update = calloc( 1, sizeof( UPDATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update;
}

UPDATE_ROOT *update_root_calloc( void )
{
	UPDATE_ROOT *update_root;

	if ( ! ( update_root = calloc( 1, sizeof( UPDATE_ROOT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_root;
}

UPDATE *update_new(	char *application_name,
			char *login_name,
			DICTIONARY *multi_row_dictionary,
			DICTIONARY *file_dictionary,
			ROLE *role,
			FOLDER *folder )
{
	UPDATE *update;

	if ( !application_name
	||   !login_name
	||   !role
	||   !folder
	||   !list_length( folder->folder_attribute_append_isa_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( multi_row_dictionary ) ) return (UPDATE *)0;

	if ( !dictionary_length( file_dictionary ) ) return (UPDATE *)0;

	if ( !role_folder_update(
		folder->role_folder_list ) )
	{
		return (UPDATE *)0;
	}

 	update = update_calloc();

	update->security_entity =
		security_entity_new(
			login_name,
			folder->non_owner_forbid,
			role->override_row_restrictions );

	update->update_row_list =
		/* -------------------------- */
		/* Returns null if no updates */
		/* -------------------------- */
		update_row_list_new(
			application_name,
			login_name,
			multi_row_dictionary,
			file_dictionary,
			folder->folder_name,
			folder->folder_attribute_append_isa_list,
			folder->relation_one2m_recursive_list,
			folder->relation_mto1_isa_list,
			folder->post_change_process,
			update->security_entity,
			appaserver_error_filename( application_name ) );

	if ( !update->update_row_list )
	{
		free( update );
		return (UPDATE *)0;
	}

	update->update_sql_statement_list =
		update_sql_statement_list_new(
			update->update_row_list );

	return update;
}

UPDATE_ROW *update_row_calloc( void )
{
	UPDATE_ROW *update_row;

	update_row = calloc( 1, sizeof( UPDATE_ROW ) );

	if ( !update_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return update_row;
}

UPDATE_ROOT *update_root_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			char *appaserver_error_filename,
			LIST *update_attribute_list )
{
	UPDATE_ROOT *update_root;
	char *tmp;

	if ( !application_name
	||   !login_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list )
	     /* -------------- */
	     /* Always returns */
	     /* -------------- */
	||   !security_entity
	||   !list_length( update_attribute_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_root = update_root_calloc();

	update_root->folder_attribute_append_isa_name_list =
		folder_attribute_append_isa_name_list(
			folder_name,
			folder_attribute_append_isa_list );

	if ( !list_length( update_root->
				folder_attribute_append_isa_name_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: folder_attribute_append_isa_name_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}

	update_root->update_changed_list =
		update_changed_list(
			update_root->folder_attribute_append_isa_name_list,
			update_attribute_list );

	if ( !list_length( update_root->update_changed_list ) )
	{
		free( update_root );
		return (UPDATE_ROOT *)0;
	}

	update_root->update_changed_primary_key =
		update_changed_primary_key(
			update_root->update_changed_list );

	update_root->folder_attribute_primary_key_list =
		folder_attribute_primary_key_list(
			folder_attribute_append_isa_list );

	update_root->update_where_list =
		update_where_list(
			update_root->folder_attribute_primary_key_list,
			update_root->update_attribute_list );

	if ( !list_length( update_root->update_where_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_where_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display_delimited(
				update_root->folder_attribute_primary_key_list,
				',' ) );
		exit( 1 );
	}

	update_root->where_clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_root_where_clause(
			/* -------------------------------------- */
			/* Returns heap memory.			  */
			/* Executes free( update_where->clause ). */
			/* -------------------------------------- */
			(  tmp =
				update_where_list_clause(
					update_root->update_where_list ) ),
			security_entity,
			folder_attribute_append_isa_list );

	free( tmp );

	update_root->update_sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_sql_statement(
			folder_table_name(
				application_name,
				folder_name ),
			/* -------------------------------------------- */
			/* Returns heap memory or null.			*/
			/* Executes free( update_changed->set_clause ). */
			/* -------------------------------------------- */
			( tmp =
				update_changed_list_set_clause(
					update_root->update_changed_list ) ),
			update_root->where_clause );

	if ( tmp ) free( tmp );

	if ( post_change_process )
	{
		update_root->update_command_line =
			update_command_line(
				post_change_process->command_line,
				login_name,
				appaserver_error_filename,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				( tmp =
					update_where_primary_data_list_string(
						update_root->
							update_where_list,
						SQL_DELIMITER ) ),
				update_root->update_attribute_list,
				APPASERVER_UPDATE_STATE,
				UPDATE_PREUPDATE_PREFIX );

		if ( tmp ) free( tmp );
	}

	return update_root;
}

UPDATE_ROW *update_row_new(
			char *application_name,
			char *login_name,
			DICTIONARY *dictionary_separate_row,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			char *appaserver_error_filename,
			int row_number )
{
	UPDATE_ROW *update_row = update_row_calloc();

	update_row->row_number = row_number;

	update_row->update_attribute_list =
		update_attribute_list(
			dictionary_separate_row,
			file_dictionary,
			row_number,
			folder_attribute_append_isa_list );

	if ( !list_length( update_row->update_attribute_list ) )
	{
		free( update_row );
		return (UPDATE_ROW *)0;
	}

	update_row->update_root =
		update_root_new(
			application_name,
			login_name,
			folder_name,
			folder_attribute_append_isa_list,
			post_change_process,
			security_entity,
			appaserver_error_filename,
			update_row->update_attribute_list );

	if ( update_row->update_root
	&&   update_row->update_root->update_changed_primary_key
	&&   list_length( relation_one2m_recursive_list ) )
	{
		update_row->update_one2m_list =
			update_one2m_list(
				application_name,
				login_name,
				appaserver_error_filename,
				update_row->update_root->update_changed_list,
				update_row->update_root->update_where_list,
				relation_one2m_recursive_list );
	}

	if ( list_length( relation_mto1_isa_list ) )
	{
		update_row->update_mto1_isa_list =
			update_mto1_isa_list(
				application_name,
				login_name,
				appaserver_error_filename,
				update_row->update_attribute_list,
				(update_row->update_root)
					? update_row->
						update_root->
						update_changed_primary_key
					: 0,
				relation_mto1_isa_list );
	}

	if ( !update_row->update_root
	&&   !list_length( update_row->update_one2m_list )
	&&   !list_length( update_row->update_mto1_isa_list ) )
	{
		free( update_row );
		return (UPDATE_ROW *)0;
	}

	update_row->cell_count =
		update_row_cell_count(
			update_row->update_root,
			update_row->update_one2m_list,
			update_row->update_mto1_isa_list );

	return update_row;
}

int update_row_list_cell_count( UPDATE_ROW_LIST *update_row_list )
{
	int cell_count = 0;
	UPDATE_ROW *update_row;

	if ( !update_row_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_row_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_row_list->list ) ) return 0;

	do {
		update_row = list_get( update_row_list->list );

		if ( !update_row->cell_count )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: update_row->cell_count is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		cell_count += update_row->cell_count;

	} while( list_next( update_row_list->list ) );

	return cell_count;
}

int update_row_cell_count(
			UPDATE_ROOT *update_root,
			LIST *update_one2m_list,
			LIST *update_mto1_isa_list )
{
	UPDATE_ONE2M *update_one2m;
	UPDATE_MTO1_ISA *update_mto1_isa;
	int cell_count = 0;

	if ( update_root )
	{
		cell_count =
			list_length(
				update_root->
					update_changed_list );
	}

	if ( list_rewind( update_one2m_list ) )
	{
		do {
			update_one2m =
				list_get(
					update_one2m_list );

			cell_count +=
				( list_length(
					update_one2m->
						changed_list ) *
				  list_length(
					update_one2m->
						update_one2m_row_list->list ) );

		} while( list_next( update_one2m_list ) );
	}

	if ( list_rewind( update_mto1_isa_list ) )
	{
		do {
			update_mto1_isa =
				list_get(
					update_mto1_isa_list );

			cell_count +=
				list_length(
					update_mto1_isa->
						update_changed_list );

			if ( list_rewind( update_mto1_isa->update_one2m_list ) )
			{
				do {
					update_one2m =
						list_get(
						     update_mto1_isa->
							    update_one2m_list );

					cell_count +=
						( list_length(
						     update_one2m->
							 changed_list ) *
				  		  list_length(
						     update_one2m->
							 update_one2m_row_list->
								list ) );
				} while ( list_next(
						update_mto1_isa->
							update_one2m_list ) );
			}

		} while( list_next( update_mto1_isa_list ) );
	}

	return cell_count;
}

UPDATE_ATTRIBUTE *update_attribute_calloc( void )
{
	UPDATE_ATTRIBUTE *update_attribute;

	if ( ! ( update_attribute =
			calloc( 1, sizeof( UPDATE_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_attribute;
}

UPDATE_MTO1_ISA *update_mto1_isa_calloc( void )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( ! ( update_mto1_isa = calloc( 1, sizeof( UPDATE_MTO1_ISA ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_mto1_isa;
}

UPDATE_ONE2M *update_one2m_calloc( void )
{
	UPDATE_ONE2M *update_one2m;

	if ( ! ( update_one2m = calloc( 1, sizeof( UPDATE_ONE2M ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_one2m;
}

UPDATE_MTO1_ISA *update_mto1_isa_new(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_attribute_list,
			boolean update_changed_primary_key,
			RELATION *relation_mto1_isa )
{
	UPDATE_MTO1_ISA *update_mto1_isa;
	char *tmp1;
	char *tmp2;

	if ( !application_name
	||   !login_name
	||   !appaserver_error_filename
	||   !list_length( update_attribute_list )
	||   !relation_mto1_isa
	||   !relation_mto1_isa->one_folder
	||   !list_length(
		relation_mto1_isa->
			one_folder->
			folder_attribute_list )
	||   !list_length(
		relation_mto1_isa->
			one_folder->
			primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_mto1_isa = update_mto1_isa_calloc();

	update_mto1_isa->update_changed_list =
		update_changed_list(
			relation_mto1_isa->one_folder->folder_attribute_list,
			update_attribute_list );

	if ( !list_length( update_mto1_isa->update_changed_list ) )
	{
		free( update_mto1_isa );
		return (UPDATE_MTO1_ISA *)0;
	}

	update_mto1_isa->update_where_list =
		update_where_list(
			relation_mto1_isa->
				one_folder->
				primary_key_list,
			update_attribute_list );

	if ( !list_length( update_mto1_isa->update_where_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: update_where_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display_delimited(
				relation_mto1_isa->
					one_folder->
					primary_key_list,
				'^' ) );
		exit( 1 );
	}

	update_mto1_isa->update_sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_sql_statement(
			folder_table_name(
				application_name,
				relation_mto1_isa->one_folder->folder_name ),
			/* -------------------------------------------- */
			/* Returns heap memory or null.			*/
			/* Executes free( update_changed->set_clause ). */
			/* -------------------------------------------- */
			( tmp1 =
				update_changed_list_set_clause(
				       update_mto1_isa->update_changed_list ) ),
			/* -------------------------------------- */
			/* Returns heap memory.			  */
			/* Executes free( update_where->clause ). */
			/* -------------------------------------- */
			( tmp2 =
				update_where_list_clause(
				       update_mto1_isa->update_where_list ) ) );

	if ( tmp1 ) free( tmp1 );
	if ( tmp2 ) free( tmp2 );

	if ( relation_mto1_isa->one_folder->post_change_process )
	{
		update_mto1_isa->update_command_line =
			update_command_line(
				relation_mto1_isa->
					one_folder->
					post_change_process->
					command_line,
				login_name,
				appaserver_error_filename,
				( tmp1 =
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					update_where_primary_data_list_string(
						update_mto1_isa->
							update_where_list,
						SQL_DELIMITER ) ),
				update_attribute_list,
				APPASERVER_UPDATE_STATE,
				UPDATE_PREUPDATE_PREFIX );

		if ( tmp1 ) free( tmp1 );
	}

	if ( update_changed_primary_key )
	{
		if ( relation_mto1_isa->
			one_folder->
			relation_one2m_recursive_list )
		{
			fprintf(stderr,
"Warning in %s/%s()/%d: relation_one2m_recursive_list is already set. No need to set it again.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
		}

		relation_mto1_isa->
			one_folder->
			relation_one2m_recursive_list =
				relation_one2m_recursive_list(
					(LIST *)0,
					relation_mto1_isa->
						one_folder->
						folder_name );

		update_mto1_isa->update_one2m_list =
			update_one2m_list(
				application_name,
				login_name,
				appaserver_error_filename,
				update_mto1_isa->update_changed_list,
				update_mto1_isa->update_where_list,
				relation_mto1_isa->
					one_folder->
					relation_one2m_recursive_list );
	}

	return update_mto1_isa;
}

UPDATE_ROW_LIST *update_row_list_calloc( void )
{
	UPDATE_ROW_LIST *update_row_list;

	if ( ! ( update_row_list =
			calloc( 1, sizeof( UPDATE_ROW_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_row_list;
}

UPDATE_ROW_LIST *update_row_list_new(
			char *application_name,
			char *login_name,
			DICTIONARY *multi_row_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			char *appaserver_error_filename )
{
	UPDATE_ROW_LIST *update_row_list = update_row_list_calloc();
	LIST *name_list;

	if ( ( update_row_list->dictionary_highest_row =
		dictionary_highest_row(
			multi_row_dictionary ) ) < 1 )
	{
		free( update_row_list );
		return (UPDATE_ROW_LIST *)0;
	}

	update_row_list->list = list_new();

	name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			folder_attribute_append_isa_list );

	for(	update_row_list->row_number = 1;
		update_row_list->row_number <=
			update_row_list->dictionary_highest_row;
		update_row_list->row_number++ )
	{
		update_row_list->dictionary_separate_row =
			dictionary_separate_row(
				name_list,
				multi_row_dictionary,
				update_row_list->row_number );

		list_set(
			update_row_list->list,
			update_row_new(
				application_name,
				login_name,
				update_row_list->dictionary_separate_row,
				file_dictionary,
				folder_name,
				folder_attribute_append_isa_list,
				relation_one2m_recursive_list,
				relation_mto1_isa_list,
				post_change_process,
				security_entity,
				appaserver_error_filename,
				update_row_list->row_number ) );
	}

	if ( list_length( update_row_list->list ) )
	{
		update_row_list->cell_count =
			update_row_list_cell_count(
				update_row_list );
	}

	return update_row_list;
}

LIST *update_mto1_isa_list(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_attribute_list,
			boolean update_changed_primary_key,
			LIST *relation_mto1_isa_list )
{
	LIST *list;

	if ( !list_rewind( relation_mto1_isa_list ) ) return (LIST *)0;

	list = list_new();

	do {
		list_set(
			list,
			update_mto1_isa_new(
				application_name,
				login_name,
				appaserver_error_filename,
				update_attribute_list,
				update_changed_primary_key,
				list_get( relation_mto1_isa_list ) ) );

	} while ( list_next( relation_mto1_isa_list ) );

	return list;
}

LIST *update_one2m_list(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_changed_list,
			LIST *update_where_list,
			LIST *relation_one2m_recursive_list )
{
	LIST *one2m_list;

	if ( !list_rewind( relation_one2m_recursive_list ) )
		return (LIST *)0;

	one2m_list = list_new();

	do {
		list_set(
			one2m_list,
			update_one2m_new(
				application_name,
				login_name,
				appaserver_error_filename,
				update_changed_list,
				update_where_list,
				list_get(
					relation_one2m_recursive_list ) ) );

	} while ( list_next( relation_one2m_recursive_list ) );

	return one2m_list;
}

UPDATE_ONE2M *update_one2m_new(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_changed_list,
			LIST *update_where_list,
			RELATION *relation_one2m )
{
	UPDATE_ONE2M *update_one2m;
	char *tmp;

	if ( !application_name
	||   !login_name
	||   !appaserver_error_filename
	||   !list_length( update_changed_list )
	||   !list_length( update_where_list )
	||   !relation_one2m
	||   !relation_one2m->many_folder
	||   !relation_one2m->many_folder->folder_name
	||   !list_length( relation_one2m->foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_one2m = update_one2m_calloc();

	update_one2m->changed_list =
		update_one2m_changed_list(
			relation_one2m->many_folder->folder_name,
			update_changed_list,
			relation_one2m->foreign_key_list );

	if ( !list_length( update_one2m->changed_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: update_one2m_changed_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display( relation_one2m->foreign_key_list ) );
		exit( 1 );
	}

	if ( list_length( update_where_list ) !=
	     list_length( relation_one2m->foreign_key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: list length mismatch: %d vs. %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( update_where_list ),
			list_length( relation_one2m->foreign_key_list ) );
		exit( 1 );
	}

	update_one2m->where_list =
		update_one2m_where_list(
			relation_one2m->many_folder->folder_name
				/* many_folder_name */,
			update_where_list,
			relation_one2m->foreign_key_list );

	update_one2m->update_where_list_clause =
		/* -------------------------------------- */
		/* Returns heap memory.			  */
		/* Executes free( update_where->clause ). */
		/* -------------------------------------- */
		update_where_list_clause(
			update_one2m->where_list );

	update_one2m->update_one2m_row_list =
		update_one2m_row_list_new(
			application_name,
			relation_one2m->many_folder->folder_name
				/* many_folder_name */,
			relation_one2m->foreign_key_list
				/* primary_key_list */,
			update_one2m->changed_list,
			update_one2m->update_where_list_clause );

	if ( !update_one2m->update_one2m_row_list )
	{
		free( update_one2m );
		return (UPDATE_ONE2M *)0;
	}

	update_one2m->sql_statement_list =
		update_one2m_sql_statement_list(
			folder_table_name(
				application_name,
				relation_one2m->many_folder->folder_name ),
			/* -------------------------------------------- */
			/* Returns heap memory or null.			*/
			/* Executes free( update_changed->set_clause ). */
			/* -------------------------------------------- */
			( tmp =
				update_changed_list_set_clause(
					update_one2m->changed_list ) ),
			update_one2m->update_one2m_row_list );

	if ( !list_length( update_one2m->sql_statement_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: update_one2m_sql_statement_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( tmp ) free( tmp );

	if ( relation_one2m->many_folder->post_change_process )
	{
		update_one2m->command_line_list =
			update_one2m_command_line_list(
				relation_one2m->
					many_folder->
					post_change_process->
					command_line,
				login_name,
				appaserver_error_filename,
				update_one2m->update_one2m_row_list );
	}

	return update_one2m;
}

LIST *update_one2m_primary_delimited_list(
			char *folder_table_name,
			LIST *foreign_key_list,
			char *update_where_list_clause )
{
	char system_string[ 1024 ];

	if ( !folder_table_name
	||   !list_length( foreign_key_list )
	||   !update_where_list_clause )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh %s %s \"%s\" none",
		list_display_delimited( foreign_key_list, ',' ),
		folder_table_name,
		update_where_list_clause );

	return list_pipe_fetch( system_string );
}

char *update_sql_statement(
			char *folder_table_name,
			char *set_clause,
			char *where_clause )
{
	char sql_statement[ STRING_64K ];

	if ( !folder_table_name
	||   !set_clause
	||   !where_clause )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(sql_statement,
		"update %s set %s where %s",
		folder_table_name,
		set_clause,
		where_clause );

	return strdup( sql_statement );
}

char *update_root_where_clause(
			char *update_where_clause,
			SECURITY_ENTITY *security_entity,
			LIST *folder_attribute_append_isa_list )
{
	char *entity_where;

	if ( !security_entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: security_entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ( entity_where =
			security_entity_where(
				security_entity,
				folder_attribute_append_isa_list ) ) )
	{
		char where_clause[ 4096 ];

		sprintf(where_clause,
			"%s and %s",
			update_where_clause,
			entity_where );

		free( update_where_clause );
		return strdup( where_clause );
	}
	else
	{
		return update_where_clause;
	}
}

void update_row_list_command_line_execute(
			LIST *command_line_list )
{
	char *command_line;

	if ( !list_rewind( command_line_list ) ) return;

	do {
		command_line =
			list_get(
				command_line_list );

		if ( system( command_line ) ){}

	} while ( list_next( command_line_list ) );
}

char *update_row_list_execute(
			UPDATE_ROW_LIST *update_row_list,
			char *appaserver_error_filename )
{
	char error_message_list_string[ STRING_64K ];
	char *ptr = error_message_list_string;
	UPDATE_ROW *update_row;
	char *message_string;
	FILE *output_pipe;
	char system_string[ 128 ];

	if ( !update_row_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_row_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_row_list->list ) ) return (char *)0;

	sprintf(system_string,
		"sql 2>>%s",
		appaserver_error_filename );

	output_pipe = popen( system_string, "w" );

	do {
		update_row = list_get( update_row_list->list );

		message_string = (char *)0;

		if ( update_row->update_root )
		{
			if ( ( message_string =
				/* --------------------------------- */
				/* Returns sql_error_message or null */
				/* --------------------------------- */
				update_row_root_sql_statement_execute(
					update_row->
						update_root->
						update_sql_statement ) ) )
			{
				if ( ptr != error_message_list_string )
				{
					ptr += sprintf( ptr, "\n<br>" );
				}

				ptr += sprintf(
					ptr,
					"%s",
					message_string );
			}
		}

		if ( message_string ) continue;

		if ( list_length( update_row->update_one2m_list ) )
		{
			update_row_one2m_sql_statement_execute(
				output_pipe,
				update_row->update_one2m_list );
		}

		if ( list_length( update_row->update_mto1_isa_list ) )
		{
			update_row_mto1_isa_sql_statement_execute(
				output_pipe,
				update_row->update_mto1_isa_list );
		}

		update_row_command_line_execute(
			update_row->update_root,
			update_row->update_one2m_list,
			update_row->update_mto1_isa_list );

	} while ( list_next( update_row_list->list ) );

	pclose( output_pipe );

	if ( ptr == error_message_list_string )
		return (char *)0;
	else
		return strdup( error_message_list_string );
}

char *update_command_line(
			char *command_line,
			char *login_name,
			char *appaserver_error_filename,
			char *update_where_primary_data_list_string,
			LIST *update_attribute_list,
			char *appaserver_update_state,
			char *update_preupdate_prefix )
{
	char line[ STRING_8K ];
	char buffer[ 1024 ];
	char preupdate_attribute_name[ 128 ];
	UPDATE_ATTRIBUTE *update_attribute;
	char *data;

	if ( !command_line
	||   !login_name
	||   !update_where_primary_data_list_string
	||   !list_rewind( update_attribute_list )
	||   !appaserver_update_state
	||   !update_preupdate_prefix
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strcpy( line, command_line, STRING_16K );

	string_search_replace(
		line,
		"$login_name",
		double_quotes_around(
			buffer,
			login_name ) );

	string_search_replace(
		line,
		"$state",
		double_quotes_around(
			buffer,
			appaserver_update_state ) );

	string_search_replace(
		line,
		"$primary_data_list",
		double_quotes_around(
			buffer,
			update_where_primary_data_list_string ) );

	do {
		update_attribute =
			list_get(
				update_attribute_list );

		if ( !update_attribute->folder_attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !update_attribute->file_data )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: file_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( update_changed_boolean(
			update_attribute->
				file_data,
			update_attribute->
				post_data ) )
		{
			data =
				update_attribute->
					post_data;
		}
		else
		{
			data =
				update_attribute->
					file_data;
		}

		string_search_replace(
			line,
			update_attribute->folder_attribute->attribute_name,
			double_quotes_around(
				buffer,
				data ) );

		sprintf(preupdate_attribute_name,
			"%s%s",
			update_preupdate_prefix,
			update_attribute->folder_attribute->attribute_name );

		string_search_replace(
			line,
			preupdate_attribute_name,
			double_quotes_around(
				buffer,
				update_attribute->
					file_data ) );

	} while ( list_next( update_attribute_list ) );

	sprintf(line + strlen( line ),
		" 2>>%s",
		appaserver_error_filename );

	return strdup( line );
}

char *update_command_line_list_execute(
			LIST *command_line_list )
{
	char *message_string;
	char *message_list_string = {0};

	if ( !list_rewind( command_line_list ) ) return (char *)0;

	do {
		if ( ( message_string =
			string_pipe_fetch(
				list_get(
					command_line_list ) ) ) )
		{
			message_list_string =
				string_append(
					message_list_string,
					message_string,
					"<br>" );
		}
	} while ( list_next( command_line_list ) );
					
	return message_list_string;
}

char *update_one2m_sql_statement(
			char *folder_table_name,
			char *update_changed_list_set_clause,
			char *update_one2m_row_where )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	update_sql_statement(
		folder_table_name,
		update_changed_list_set_clause,
		update_one2m_row_where );
}

char *update_one2m_command_line(
			char *command_line,
			char *login_name,
			char *appaserver_error_filename,
			UPDATE_ONE2M_ROW *update_one2m_row,
			char *appaserver_update_state,
			char *update_preupdate_prefix )
{
	if ( !command_line
	||   !login_name
	||   !appaserver_error_filename
	||   !update_one2m_row
	||   !list_rewind( update_one2m_row->update_attribute_list )
	||   !appaserver_update_state
	||   !update_preupdate_prefix )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	update_command_line(
		command_line,
		login_name,
		appaserver_error_filename,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			update_attribute_data_list(
				update_one2m_row->update_attribute_list ),
			SQL_DELIMITER )
				/* primary_data_list_string */,
		update_one2m_row->update_attribute_list,
		appaserver_update_state,
		update_preupdate_prefix );
}

LIST *update_one2m_sql_statement_list(
			char *folder_table_name,
			char *update_changed_list_set_clause,
			UPDATE_ONE2M_ROW_LIST *update_one2m_row_list )
{
	LIST *sql_statement_list;
	char *tmp;

	if ( !folder_table_name
	||   !update_changed_list_set_clause
	||   !update_one2m_row_list
	||   !list_length( update_one2m_row_list->list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sql_statement_list = list_new();

	list_rewind( update_one2m_row_list->list );

	do {
		list_set(
			sql_statement_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_one2m_sql_statement(
				folder_table_name,
				update_changed_list_set_clause,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				( tmp = update_one2m_row_where(
					   list_get(
						update_one2m_row_list->
							list ) ) ) ) );

		if ( tmp ) free( tmp );

	} while ( list_next( update_one2m_row_list->list ) );

	return sql_statement_list;
}

LIST *update_one2m_changed_list(
			char *many_folder_name,
			LIST *update_changed_list,
			LIST *foreign_key_list )
{
	UPDATE_CHANGED *changed_primary;
	UPDATE_CHANGED *changed_foreign;
	LIST *one2m_changed_list;
	char *changed_attribute_name;

	if ( !many_folder_name
	||   !list_length( foreign_key_list )
	||   !list_rewind( update_changed_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	one2m_changed_list = list_new();

	do {
		changed_primary =
			list_get(
				update_changed_list );

		if ( changed_primary->
			update_attribute->
			folder_attribute->
			primary_key_index )
		{
			changed_foreign = update_changed_calloc();

			changed_foreign->update_attribute =
				update_attribute_calloc();

			changed_attribute_name =
				/* ------------------------------------ */
				/* Returns list_get( foreign_key_list ) */
				/* ------------------------------------ */
				update_one2m_changed_attribute_name(
					changed_primary->
						update_attribute->
						folder_attribute->
						primary_key_index,
					foreign_key_list );

			if ( !changed_attribute_name )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: update_one2m_changed_attribute_name() returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			changed_foreign->update_attribute->folder_attribute =
				folder_attribute_new(
					many_folder_name,
					changed_attribute_name );

			changed_foreign->
				update_attribute->
				folder_attribute->
				primary_key_index =
					changed_primary->
						update_attribute->
						folder_attribute->
						primary_key_index;

			changed_foreign->
				update_attribute->
				folder_attribute->
				attribute =
					attribute_new(
						changed_attribute_name );

			changed_foreign->
				update_attribute->
				folder_attribute->
				attribute->
				datatype_name =
					changed_primary->
						update_attribute->
						folder_attribute->
						attribute->
						datatype_name;

			/* This is for debugging; file_data isn't used. */
			/* -------------------------------------------- */
			changed_foreign->
				update_attribute->
				file_data =
					changed_primary->
						update_attribute->
						file_data;

			changed_foreign->
				update_attribute->
				post_data =
					changed_primary->
						update_attribute->
						post_data;

			changed_foreign->set_clause =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				update_changed_set_clause(
					changed_foreign->
						update_attribute->
						folder_attribute->
						attribute_name,
					changed_foreign->
						update_attribute->
						folder_attribute->
						attribute->
						datatype_name,
					changed_foreign->
					       update_attribute->
					       post_data );

			list_set(
				one2m_changed_list,
				changed_foreign );
		}
	} while ( list_next( update_changed_list ) );

	return one2m_changed_list;
}

LIST *update_one2m_where_list(
			char *many_folder_name,
			LIST *update_where_list,
			LIST *foreign_key_list )
{
	UPDATE_WHERE *where_primary;
	UPDATE_WHERE *where_foreign;
	char *foreign_key;
	LIST *one2m_where_list;

	if ( !many_folder_name
	||   !list_length( update_where_list )
	||   !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( update_where_list ) !=
	     list_length( foreign_key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: list length mismatch: %d vs. %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( update_where_list ),
			list_length( foreign_key_list ) );
		exit( 1 );
	}

	list_rewind( update_where_list );
	list_rewind( foreign_key_list );

	one2m_where_list = list_new();

	do {
		where_primary = list_get( update_where_list );

		if ( !where_primary->update_attribute
		||   !where_primary->update_attribute->folder_attribute
		||   !where_primary->
			update_attribute->
			folder_attribute->
			attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		foreign_key = list_get( foreign_key_list );

		where_foreign = update_where_calloc();

		where_foreign->update_attribute = update_attribute_calloc();

		where_foreign->
			update_attribute->
			folder_attribute =
				folder_attribute_new(
					many_folder_name,
					foreign_key );

		where_foreign->
			update_attribute->
			folder_attribute->
			attribute =
				attribute_new(
					foreign_key );

		where_foreign->
			update_attribute->
			folder_attribute->
			attribute->
			datatype_name =
				where_primary->
					update_attribute->
					folder_attribute->
					attribute->
					datatype_name;

		where_foreign->
			update_attribute->
			file_data =
				where_primary->
					update_attribute->
					file_data;

		where_foreign->
			update_attribute->
			post_data =
				where_primary->
					update_attribute->
					post_data;

		where_foreign->clause =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_where_clause(
				foreign_key,
				where_foreign->
					update_attribute->
					folder_attribute->
					attribute->
					datatype_name,
				where_foreign->
					update_attribute->
					file_data );

		list_set( one2m_where_list, where_foreign );

		list_next( foreign_key_list );

	} while ( list_next( update_where_list ) );

	return one2m_where_list;
}

char *update_one2m_changed_attribute_name(
			int primary_key_index,
			LIST *foreign_key_list )
{
	char *attribute_name;

	if ( ! ( attribute_name =
			/* ------------------ */
			/* index is one based */
			/* ------------------ */
			list_index_seek(
				primary_key_index,
				foreign_key_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: list_index_seek(%d) for [%s] returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			primary_key_index,
			list_display( foreign_key_list ) );
		exit( 1 );
	}

	return attribute_name;
}

UPDATE_CHANGED *update_changed_seek(
			char *attribute_name,
			LIST *update_changed_list )
{
	UPDATE_CHANGED *update_changed;

	if ( !list_rewind( update_changed_list ) )
		return (UPDATE_CHANGED *)0;

	do {
		update_changed = list_get( update_changed_list );

		if ( strcmp(
			update_changed->
				update_attribute->
				folder_attribute->
				attribute_name,
			attribute_name ) == 0 )
		{
			return update_changed;
		}

	} while ( list_next( update_changed_list ) );

	return (UPDATE_CHANGED *)0;
}

LIST *update_one2m_command_line_list(
			char *command_line,
			char *login_name,
			char *appaserver_error_filename,
			UPDATE_ONE2M_ROW_LIST *update_one2m_row_list )
{
	LIST *command_line_list;

	if ( !command_line
	||   !login_name
	||   !update_one2m_row_list
	||   !list_rewind( update_one2m_row_list->list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	command_line_list = list_new();

	do {
		list_set(
			command_line_list,
			update_one2m_command_line(
				command_line,
				login_name,
				appaserver_error_filename,
				list_get( update_one2m_row_list->list ),
				APPASERVER_UPDATE_STATE,
				UPDATE_PREUPDATE_PREFIX ) );

	} while ( list_next( update_one2m_row_list->list ) );

	return command_line_list;
}

LIST *update_where_list(
			LIST *primary_key_list,
			LIST *update_attribute_list )
{
	LIST *where_list;

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where_list = list_new();

	do {
		list_set(
			where_list,
			update_where_new(
				(char *)list_get( primary_key_list ),
				update_attribute_list ) );

	} while ( list_next( primary_key_list ) );

	return where_list;
}

UPDATE_WHERE *update_where_new(
			char *primary_key,
			LIST *update_attribute_list )
{
	UPDATE_WHERE *update_where = update_where_calloc();

	if ( ! ( update_where->update_attribute =
			update_attribute_seek(
				primary_key,
				update_attribute_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: update_attribute_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			primary_key );
		exit( 1 );
	}

	if ( !update_where->update_attribute->folder_attribute
	||   !update_where->update_attribute->folder_attribute->attribute )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_where->clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_where_clause(
			primary_key,
			update_where->
				update_attribute->
				folder_attribute->
				attribute->
				datatype_name,
			update_where->
				update_attribute->
				file_data );

	return update_where;
}

UPDATE_WHERE *update_where_calloc( void )
{
	UPDATE_WHERE *update_where;

	if ( ! ( update_where = calloc( 1, sizeof( UPDATE_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_where;
}

char *update_where_clause(
			char *key,
			char *datatype_name,
			char *file_data )
{
	char clause[ 1024 ];

	if ( !key
	||   !datatype_name
	||   !file_data )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( attribute_is_number( datatype_name ) )
	{
		sprintf(clause,
			"%s = %s",
			key,
			file_data );
	}
	else
	{
		sprintf(clause,
			"%s = '%s'",
			key,
			file_data );
	}

	return strdup( clause );
}

char *update_where_list_clause( LIST *update_where_list )
{
	UPDATE_WHERE *update_where;
	char clause[ STRING_16K ];
	char *ptr = clause;

	if ( !list_rewind( update_where_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_where_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		update_where = list_get( update_where_list );

		if ( !update_where->clause )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: clause is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != clause ) ptr += sprintf( ptr, " and " );

		ptr += sprintf( ptr, "%s", update_where->clause );

		free( update_where->clause );

	} while ( list_next( update_where_list ) );

	return strdup( clause );
}

LIST *update_changed_list(
			LIST *folder_attribute_append_isa_name_list,
			LIST *update_attribute_list )
{
	UPDATE_CHANGED *update_changed;
	char *attribute_name;
	LIST *list = {0};

	if ( !list_rewind( folder_attribute_append_isa_name_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: folder_attribute_append_isa_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		attribute_name =
			list_get(
				folder_attribute_append_isa_name_list );

		update_changed =
			update_changed_new(
				attribute_name,
				update_attribute_list );

		if ( update_changed )
		{
			if ( !list ) list = list_new();

			list_set( list, update_changed );
		}

	} while ( list_next( folder_attribute_append_isa_name_list ) );

	return list;
}

UPDATE_CHANGED *update_changed_new(
			char *attribute_name,
			LIST *update_attribute_list )
{
	UPDATE_CHANGED *update_changed = update_changed_calloc();

	if ( !attribute_name
	||   !list_length( update_attribute_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( update_changed->update_attribute =
			update_attribute_seek(
				attribute_name,
				update_attribute_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: update_attribute_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			attribute_name );
		exit( 1 );
	}

	update_changed->update_changed_boolean =
		update_changed_boolean(
			update_changed->
				update_attribute->
				file_data,
			update_changed->
				update_attribute->
				post_data );

	if ( !update_changed->update_changed_boolean )
	{
		free( update_changed );
		return (UPDATE_CHANGED *)0;
	}

	if ( !update_changed->update_attribute->folder_attribute
	||   !update_changed->update_attribute->folder_attribute->attribute )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_changed->set_clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_changed_set_clause(
			update_changed->
				update_attribute->
				folder_attribute->
				attribute_name,
			update_changed->
				update_attribute->
				folder_attribute->
				attribute->
				datatype_name,
			update_changed->
				update_attribute->
				post_data );

	return update_changed;
}

UPDATE_CHANGED *update_changed_calloc( void )
{
	UPDATE_CHANGED *update_changed;

	if ( ! ( update_changed = calloc( 1, sizeof( UPDATE_CHANGED ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_changed;
}

boolean update_changed_boolean(
			char *file_data,
			char *post_data )
{
	if ( !post_data || !*post_data )
	{
		return 0;
	}

	return
	( string_strcmp(
		file_data,
		post_data ) != 0 );
}

char *update_changed_set_clause(
			char *attribute_name,
			char *datatype_name,
			char *post_data )
{
	char set_clause[ STRING_16K ];

	if ( !attribute_name
	||   !post_data )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( post_data, "/" ) == 0 )
	{
		sprintf(set_clause,
			"%s = null",
			attribute_name );
	}
	else
	if ( datatype_name && attribute_is_number( datatype_name ) )
	{
		sprintf(set_clause,
			"%s = %s",
			attribute_name,
			post_data );
	}
	else
	{
		sprintf(set_clause,
			"%s = '%s'",
			attribute_name,
			post_data );
	}

	return strdup( set_clause );
}

LIST *update_changed_primary_data_list(
			LIST *update_changed_list )
{
	LIST *data_list;
	UPDATE_CHANGED *update_changed;

	if ( !list_rewind( update_changed_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		update_changed = list_get( update_changed_list );

		if ( !update_changed->update_attribute
		||   !update_changed->
			update_attribute->
			post_data )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: post_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			data_list,
			update_changed->
				update_attribute->
				post_data );

	} while ( list_next( update_changed_list ) );

	return data_list;
}

char *update_changed_list_set_clause(
			LIST *update_changed_list )
{
	char set_clause[ STRING_64K ];
	char *ptr = set_clause;
	UPDATE_CHANGED *update_changed;

	if ( !list_rewind( update_changed_list ) ) return (char *)0;

	do {
		update_changed = list_get( update_changed_list );

		if ( !update_changed->set_clause )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: set_clause is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != set_clause ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			update_changed->set_clause );

		free( update_changed->set_clause );

	} while ( list_next( update_changed_list ) );

	return strdup( set_clause );
}

boolean update_changed_primary_key(
			LIST *update_changed_list )
{
	UPDATE_CHANGED *update_changed;

	if ( !list_rewind( update_changed_list ) ) return 0;

	do {
		update_changed = list_get( update_changed_list );

		if ( !update_changed->update_attribute
		||   !update_changed->update_attribute->folder_attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( update_changed->
			update_attribute->
			folder_attribute->
			primary_key_index )
		{
			return 1;
		}

	} while ( list_next( update_changed_list ) );

	return 0;
}

char *update_where_primary_data_list_string(
			LIST *update_where_list,
			char sql_delimiter )
{
	UPDATE_WHERE *update_where;
	char primary_data_delimited[ 1024 ];
	char *ptr = primary_data_delimited;
	char *data;

	if ( !list_rewind( update_where_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_where_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		update_where = list_get( update_where_list );

		if ( !update_where->update_attribute
		||   !update_where->
			update_attribute->
			file_data )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: file_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != primary_data_delimited )
		{
			ptr += sprintf(
				ptr,
				"%c",
				sql_delimiter );
		}

		if ( update_changed_boolean(
			update_where->
				update_attribute->
				file_data,
			update_where->
				update_attribute->
				post_data ) )
		{
			data =
				update_where->
					update_attribute->
					post_data;
		}
		else
		{
			data =
				update_where->
					update_attribute->
					file_data;
		}

		ptr += sprintf(
			ptr,
			"%s",
			data );

	} while ( list_next( update_where_list ) );

	return strdup( primary_data_delimited );
}

LIST *update_attribute_list(
			DICTIONARY *dictionary_separate_row,
			DICTIONARY *file_dictionary,
			int row_number,
			LIST *folder_attribute_append_isa_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *attribute_list;

	if ( !dictionary_length( file_dictionary ) ) return (LIST *)0;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_append_isa_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	attribute_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		list_set(
			attribute_list,
			update_attribute_new(
				dictionary_separate_row,
				file_dictionary,
				row_number,
				folder_attribute ) );

	} while ( list_next( folder_attribute_append_isa_list ) );

	return attribute_list;
}

UPDATE_ATTRIBUTE *update_attribute_new(
			DICTIONARY *dictionary_separate_row,
			DICTIONARY *file_dictionary,
			int row_number,
			FOLDER_ATTRIBUTE *folder_attribute )
{
	UPDATE_ATTRIBUTE *update_attribute = update_attribute_calloc();

	if ( !folder_attribute
	||   !folder_attribute->attribute_name
	||   !folder_attribute->attribute
	||   !folder_attribute->attribute->datatype_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_attribute->folder_attribute = folder_attribute;
	update_attribute->row_number = row_number;

	update_attribute->post_data =
		dictionary_get(
			folder_attribute->attribute_name,
			dictionary_separate_row );

	update_attribute->file_data =
		dictionary_row(
			folder_attribute->attribute_name,
			row_number,
			file_dictionary );

	if ( !update_attribute->file_data )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: dictionary_row(%s/%d) returned empty for dictionary = \n[%s].\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_attribute->attribute_name,
			row_number,
			dictionary_display_delimiter(
				file_dictionary,
				SQL_DELIMITER ) );
		exit( 1 );
	}

	return update_attribute;
}

LIST *update_attribute_data_list( LIST *update_attribute_list )
{
	LIST *data_list;
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !list_rewind( update_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	data_list = list_new();

	do {
		update_attribute = list_get( update_attribute_list );

		if ( !update_attribute->file_data
		&&   !update_attribute->post_data )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: both file_data and post_data are empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( update_changed_boolean(
			update_attribute->file_data,
			update_attribute->post_data ) )
		{
			list_set(
				data_list,
				update_attribute->
					post_data );
		}
		else
		{
			list_set(
				data_list,
				update_attribute->
					file_data );
		}

	} while ( list_next( update_attribute_list ) );

	return data_list;
}

UPDATE_ATTRIBUTE *update_attribute_seek(
			char *attribute_name,
			LIST *update_attribute_list )
{
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	do {
		update_attribute = list_get( update_attribute_list );

		if ( !update_attribute->folder_attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(
			update_attribute->folder_attribute->attribute_name,
			attribute_name ) == 0 )
		{
			return update_attribute;
		}

	} while ( list_next( update_attribute_list ) );

	return (UPDATE_ATTRIBUTE *)0;
}

UPDATE_ONE2M_ROW *update_one2m_row_new(
			char *many_folder_name,
			LIST *primary_key_list,
			LIST *update_one2m_changed_list,
			LIST *primary_data_list )
{
	UPDATE_ONE2M_ROW *update_one2m_row;
	char *primary_key;
	UPDATE_ATTRIBUTE *update_attribute;
	UPDATE_CHANGED *update_changed;

	if ( !many_folder_name
	||   !list_length( primary_key_list )
	||   !list_length( update_one2m_changed_list )
	||   !list_length( primary_data_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( primary_key_list ) !=
	     list_length( primary_data_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: list length mismatch: %d vs. %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( primary_key_list ),
			list_length( primary_data_list ) );
		exit( 1 );
	}

	update_one2m_row = update_one2m_row_calloc();

	update_one2m_row->update_attribute_list = list_new();

	list_rewind( primary_data_list );
	list_rewind( primary_key_list );

	do {
		primary_key = list_get( primary_key_list );

		update_attribute = update_attribute_calloc();

		update_attribute->folder_attribute =
			folder_attribute_new(
				many_folder_name,
				primary_key );

		update_attribute->file_data =
			list_get( primary_data_list );

		if ( ( update_changed =
			update_changed_seek(
				primary_key,
				update_one2m_changed_list ) ) )
		{
			update_attribute->
				post_data =
					update_changed->
						update_attribute->
						post_data;
		}
		else
		{
			update_attribute->
				post_data =
					update_attribute->
						file_data;
		}

		list_set(
			update_one2m_row->update_attribute_list,
			update_attribute );

		list_next( primary_data_list );

	} while( list_next( primary_key_list ) );

	return update_one2m_row;
}

UPDATE_ONE2M_ROW *update_one2m_row_calloc( void )
{
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( ! ( update_one2m_row =
			calloc( 1, sizeof( UPDATE_ONE2M_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_one2m_row;
}

UPDATE_ONE2M_ROW_LIST *update_one2m_row_list_new(
			char *application_name,
			char *many_folder_name,
			LIST *primary_key_list,
			LIST *update_one2m_changed_list,
			char *update_where_list_clause )
{
	char *update_one2m_row_list_fetch;
	UPDATE_ONE2M_ROW_LIST *update_one2m_row_list;

	if ( !application_name
	||   !many_folder_name
	||   !list_length( primary_key_list )
	||   !list_length( update_one2m_changed_list )
	||   !update_where_list_clause )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_one2m_row_list = update_one2m_row_list_calloc();

	update_one2m_row_list->fetch_list =
		update_one2m_row_list_fetch_list(
			folder_table_name(
				application_name,
				many_folder_name ),
			primary_key_list,
			update_where_list_clause );

	if ( !list_rewind( update_one2m_row_list->fetch_list ) )
	{
		free( update_one2m_row_list );
		return (UPDATE_ONE2M_ROW_LIST *)0;
	}

	update_one2m_row_list->list = list_new();

	do {
		update_one2m_row_list_fetch =
			list_get(
				update_one2m_row_list->
					fetch_list );

		list_set(
			update_one2m_row_list->list,
			update_one2m_row_new(
				many_folder_name,
				primary_key_list,
				update_one2m_changed_list,
				list_string_extract(
					update_one2m_row_list_fetch,
					SQL_DELIMITER )
						/* primary_data_list */ ) );

	} while ( list_next( update_one2m_row_list->fetch_list ) );

	return update_one2m_row_list;
}

UPDATE_ONE2M_ROW_LIST *update_one2m_row_list_calloc( void )
{
	UPDATE_ONE2M_ROW_LIST *update_one2m_row_list;

	if ( ! ( update_one2m_row_list =
			calloc( 1, sizeof( UPDATE_ONE2M_ROW_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_one2m_row_list;
}

LIST *update_one2m_row_list_fetch_list(
			char *folder_table_name,
			LIST *primary_key_list,
			char *update_where_list_clause )
{
	char system_string[ 1024 ];

	if ( !folder_table_name
	||   !list_length( primary_key_list )
	||   !update_where_list_clause )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" none",
		list_display_delimited( primary_key_list, ',' ),
		folder_table_name,
		update_where_list_clause );

	return list_pipe_fetch( system_string );
}

char *update_one2m_row_where( UPDATE_ONE2M_ROW *update_one2m_row )
{
	char where[ 1024 ];
	char *ptr = where;
	UPDATE_ATTRIBUTE *update_attribute;
	char *tmp;

	if ( !list_rewind( update_one2m_row->update_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		update_attribute =
			list_get(
				update_one2m_row->
					update_attribute_list );

		if ( !update_attribute->folder_attribute
		||   !update_attribute->folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !update_attribute->file_data )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: file_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = update_where_clause(
				     update_attribute->
					folder_attribute->
					attribute_name,
				     update_attribute->
					folder_attribute->
					attribute->
					datatype_name,
				     update_attribute->
					file_data ) ) );

		if ( tmp ) free( tmp );

	} while ( list_next( update_one2m_row->update_attribute_list ) );

	return strdup( where );
}

char *update_row_root_sql_statement_execute(
			char *update_sql_statement )
{
	char system_string[ STRING_64K ];

	sprintf(system_string,
		"echo \"%s\" | sql 2>&1",
		update_sql_statement );

	return string_pipe_fetch( system_string );
}

void update_row_one2m_sql_statement_execute(
			FILE *output_pipe,
			LIST *update_one2m_list )
{
	UPDATE_ONE2M *update_one2m;

	if ( !list_rewind( update_one2m_list ) ) return;

	do {
		update_one2m = list_get( update_one2m_list );

		if ( list_rewind( update_one2m->sql_statement_list ) )
		{
			do {
				fprintf(output_pipe,
					"%s\n",
					(char *)list_get(
						update_one2m->
							sql_statement_list ) );

				fflush( output_pipe );

			} while ( list_next(
					update_one2m->sql_statement_list ) );
		}

	} while ( list_next( update_one2m_list ) );
}

void update_row_mto1_isa_sql_statement_execute(
			FILE *output_pipe,
			LIST *update_mto1_isa_list )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !list_rewind( update_mto1_isa_list ) ) return;

	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		fprintf(output_pipe,
			"%s\n",
			update_mto1_isa->update_sql_statement );

		fflush( output_pipe );

		if ( list_length( update_mto1_isa->update_one2m_list ) )
		{
			update_row_one2m_sql_statement_execute(
				output_pipe,
				update_mto1_isa->update_one2m_list );
		}

	} while ( list_next( update_mto1_isa_list ) );
}

void update_row_command_line_execute(
			UPDATE_ROOT *update_root,
			LIST *update_one2m_list,
			LIST *update_mto1_isa_list )
{
	if ( update_root && update_root->update_command_line )
	{
		if ( system( update_root->update_command_line ) ){}
	}

	if ( list_length( update_one2m_list ) )
	{
		update_row_one2m_command_line_execute(
			update_one2m_list );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		update_row_mto1_isa_command_line_execute(
			update_mto1_isa_list );
	}
}

void update_row_one2m_command_line_execute(
			LIST *update_one2m_list )
{
	UPDATE_ONE2M *update_one2m;

	if ( !list_rewind( update_one2m_list ) ) return;

	do {
		update_one2m = list_get( update_one2m_list );

		if ( list_rewind( update_one2m->command_line_list ) )
		{
			do {
				if ( system(
					(char *)list_get(
						update_one2m->
							command_line_list ) ) )
				{
				}
			} while ( list_next(
					update_one2m->command_line_list ) );
		}

	} while ( list_next( update_one2m_list ) );
}

void update_row_mto1_isa_command_line_execute(
			LIST *update_mto1_isa_list )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !list_rewind( update_mto1_isa_list ) ) return;

	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		if ( system( update_mto1_isa->update_command_line ) ){}

		if ( list_length( update_mto1_isa->update_one2m_list ) )
		{
			update_row_one2m_command_line_execute(
				update_mto1_isa->update_one2m_list );
		}

	} while ( list_next( update_mto1_isa_list ) );
}

UPDATE_SQL_STATEMENT_LIST *update_sql_statement_list_new(
			UPDATE_ROW_LIST *update_row_list )
{
	UPDATE_SQL_STATEMENT_LIST *update_sql_statement_list;
	UPDATE_ROW *update_row;

	if ( !update_row_list
	||   !list_rewind( update_row_list->list ) )
	{
		return (UPDATE_SQL_STATEMENT_LIST *)0;
	}

	update_sql_statement_list = update_sql_statement_list_calloc();

	update_sql_statement_list->list = list_new();

	do {
		update_row = list_get( update_row_list->list );

		list_set(
			update_sql_statement_list->list,
			update_sql_statement_new(
				update_row->update_root,
				update_row->update_one2m_list,
				update_row->update_mto1_isa_list ) );

	} while ( list_next( update_row_list->list ) );

	return update_sql_statement_list;
}

UPDATE_SQL_STATEMENT_LIST *update_sql_statement_list_calloc( void )
{
	UPDATE_SQL_STATEMENT_LIST *update_sql_statement_list;

	if ( ! ( update_sql_statement_list =
			calloc( 1, sizeof( UPDATE_SQL_STATEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_sql_statement_list;
}

UPDATE_SQL_STATEMENT *update_sql_statement_new(
			UPDATE_ROOT *update_root,
			LIST *update_one2m_list,
			LIST *update_mto1_isa_list )
{
	UPDATE_SQL_STATEMENT *update_sql_statement =
		update_sql_statement_calloc();

	if ( update_root )
	{
		update_sql_statement->root =
			update_sql_statement_root(
				update_root );

		update_sql_statement->command_line_root =
			update_sql_statement_command_line_root(
				update_root );
	}

	if ( list_length( update_one2m_list ) )
	{
		update_sql_statement->one2m_list =
			update_sql_statement_one2m_list(
				update_one2m_list );

		update_sql_statement->command_line_one2m_list =
			update_sql_statement_command_line_one2m_list(
				update_one2m_list );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		update_sql_statement->mto1_isa_list =
			update_sql_statement_mto1_isa_list(
				update_mto1_isa_list );

		update_sql_statement->command_line_mto1_isa_list =
			update_sql_statement_command_line_mto1_isa_list(
				update_mto1_isa_list );
	}

	if ( update_sql_statement->root
	||   list_length( update_sql_statement->one2m_list )
	||   list_length( update_sql_statement->mto1_isa_list ) )
	{
		return update_sql_statement;
	}
	else
	{
		free( update_sql_statement );
		return (UPDATE_SQL_STATEMENT *)0;
	}
}

UPDATE_SQL_STATEMENT *update_sql_statement_calloc( void )
{
	UPDATE_SQL_STATEMENT *update_sql_statement;

	if ( ! ( update_sql_statement =
			calloc( 1, sizeof( UPDATE_SQL_STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_sql_statement;
}

char *update_sql_statement_root( UPDATE_ROOT *update_root )
{
	if ( !update_root
	&&   !update_root->update_sql_statement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_sql_statement is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_root->update_sql_statement;
}

char *update_sql_statement_command_line_root( UPDATE_ROOT *update_root )
{
	if ( !update_root )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_root is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_root->update_command_line;
}

LIST *update_sql_statement_one2m_list( LIST *update_one2m_list )
{
	LIST *list;
	UPDATE_ONE2M *update_one2m;

	if ( !list_rewind( update_one2m_list ) ) return (LIST *)0;

	list = list_new();

	do {
		update_one2m = list_get( update_one2m_list );

		list_set_list(
			list,
			update_one2m->sql_statement_list );

	} while ( list_next( update_one2m_list ) );

	return list;
}

LIST *update_sql_statement_command_line_one2m_list( LIST *update_one2m_list )
{
	LIST *list = {0};
	UPDATE_ONE2M *update_one2m;

	if ( !list_rewind( update_one2m_list ) ) return (LIST *)0;


	do {
		update_one2m = list_get( update_one2m_list );

		if ( list_length( update_one2m->command_line_list ) )
		{
			if ( !list ) list = list_new();

			list_set_list(
				list,
				update_one2m->command_line_list );
		}

	} while ( list_next( update_one2m_list ) );

	return list;
}

LIST *update_sql_statement_mto1_isa_list( LIST *update_mto1_isa_list )
{
	LIST *list;
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !list_rewind( update_mto1_isa_list ) ) return (LIST *)0;

	list = list_new();

	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		list_set(
			list,
			update_mto1_isa->update_sql_statement );

		if ( list_length( update_mto1_isa->update_one2m_list ) )
		{
			list_set_list(
				list,
				update_sql_statement_one2m_list(
					update_mto1_isa->update_one2m_list ) );
		}

	} while ( list_next( update_mto1_isa_list ) );

	return list;
}

LIST *update_sql_statement_command_line_mto1_isa_list(
			LIST *update_mto1_isa_list )
{
	LIST *list;
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !list_rewind( update_mto1_isa_list ) ) return (LIST *)0;

	list = list_new();

	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		if ( update_mto1_isa->update_command_line )
		{
			list_set(
				list,
				update_mto1_isa->update_command_line );
		}

		if ( list_length( update_mto1_isa->update_one2m_list ) )
		{
			list_set_list(
				list,
				update_sql_statement_command_line_one2m_list(
					update_mto1_isa->update_one2m_list ) );
		}

	} while ( list_next( update_mto1_isa_list ) );

	return list;
}

