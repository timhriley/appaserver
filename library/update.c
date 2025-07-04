/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/update.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "sql.h"
#include "dictionary.h"
#include "process.h"
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "attribute.h"
#include "folder.h"
#include "date.h"
#include "role_folder.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "widget.h"
#include "query.h"
#include "spool.h"
#include "security.h"
#include "appaserver_parameter.h"
#include "dictionary_separate.h"
#include "relation_copy.h"
#include "update.h"

UPDATE *update_calloc( void )
{
	UPDATE *update;

	if ( ! ( update = calloc( 1, sizeof ( UPDATE ) ) ) )
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

	if ( ! ( update_root = calloc( 1, sizeof ( UPDATE_ROOT ) ) ) )
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

UPDATE *update_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean override_row_restrictions,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction,
		LIST *relation_one2m_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process )
{
	UPDATE *update;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"for folder=%s, folder_attribute_append_isa_list is empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( multi_row_dictionary )
	||   !dictionary_length( file_dictionary ) )
	{
		return NULL;
	}

 	update = update_calloc();

	update->relation_one2m_list = relation_one2m_list;
	update->relation_mto1_list = relation_mto1_list;
	update->relation_mto1_isa_list = relation_mto1_isa_list;

	if ( folder_row_level_restriction )
	{
		update->security_entity =
			security_entity_new(
				login_name,
				folder_row_level_restriction->
					non_owner_forbid ||
				folder_row_level_restriction->
					non_owner_viewonly,
				override_row_restrictions );
	}

	update->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	update->appaserver_parameter_mount_point =
		/* ------------------------------------------------ */
		/* Returns component of global_appaserver_parameter */
		/* ------------------------------------------------ */
		appaserver_parameter_mount_point();

	update->update_row_list =
		/* -------------------------- */
		/* Returns null if no updates */
		/* -------------------------- */
		update_row_list_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			multi_row_dictionary,
			file_dictionary,
			relation_one2m_list,
			relation_mto1_list,
			relation_mto1_isa_list,
			folder_attribute_append_isa_list,
			post_change_process,
			update->security_entity,
			update->appaserver_error_filename,
			update->appaserver_parameter_mount_point );

	if ( !update->update_row_list )
	{
		free( update );
		return NULL;
	}

	update->results_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_results_string(
			update->update_row_list->cell_count,
			update->update_row_list->folder_name_list_string );

	return update;
}

UPDATE_ROW *update_row_calloc( void )
{
	UPDATE_ROW *update_row;

	update_row = calloc( 1, sizeof ( UPDATE_ROW ) );

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
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process,
		SECURITY_ENTITY *security_entity,
		char *appaserver_error_filename,
		LIST *update_attribute_list )
{
	UPDATE_ROOT *update_root;

	if ( !application_name
	||   !login_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list )
	||   !appaserver_error_filename
	||   !list_length( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_root = update_root_calloc();
	update_root->folder_name = folder_name;

	update_root->folder_attribute_primary_key_list =
		folder_attribute_primary_key_list(
			folder_name,
			folder_attribute_append_isa_list );

	update_root->folder_attribute_name_list =
		folder_attribute_name_list(
			folder_name,
			folder_attribute_append_isa_list );

	update_root->update_changed_list =
		update_changed_list_new(
			folder_name,
			update_root->folder_attribute_primary_key_list,
			update_root->folder_attribute_name_list,
			update_attribute_list,
			(security_entity)
				? security_entity->where
				: (char *)0 );

	if ( !update_root->update_changed_list )
	{
		free( update_root );
		return NULL;
	}

	update_root->update_changed_primary_key_boolean =
		update_changed_primary_key_boolean(
			update_root->update_changed_list->list
				/* update_changed_list */ );

	if ( post_change_process )
	{
		update_root->update_where_list_primary_data_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_where_list_primary_data_string(
				SQL_DELIMITER,
				update_root->
					update_changed_list->
					where_list
					/* update_where_list */ );

		update_root->update_command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_command_line(
				APPASERVER_UPDATE_STATE,
				UPDATE_PREUPDATE_PREFIX,
				post_change_process->command_line,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_change_process->process_name,
				appaserver_error_filename,
				update_root->
					update_where_list_primary_data_string,
				update_attribute_list );

		update_root->update_changed_list->update_command_line =
			update_root->update_command_line;
	}

	return update_root;
}

UPDATE_ROW *update_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		LIST *relation_one2m_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process,
		SECURITY_ENTITY *security_entity,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		int row_number )
{
	UPDATE_ROW *update_row;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename
	||   !row_number )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( multi_row_dictionary )
	||   !dictionary_length( file_dictionary ) )
	{
		return NULL;
	}

	update_row = update_row_calloc();
	update_row->row_number = row_number;

	update_row->update_attribute_list =
		update_attribute_list(
			multi_row_dictionary,
			file_dictionary,
			folder_attribute_append_isa_list,
			row_number );

	if ( !list_length( update_row->update_attribute_list ) )
	{
		free( update_row );
		return NULL;
	}

	update_row->update_root =
		update_root_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			folder_attribute_append_isa_list,
			post_change_process,
			security_entity,
			appaserver_error_filename,
			update_row->update_attribute_list );

	if ( update_row->update_root
	&&   update_row->update_root->update_changed_primary_key_boolean
	&&   list_length( relation_one2m_list ) )
	{
#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: calling update_one2m_list_new() with folder_name=%s;\nupdate_changed_list=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	folder_name,
	update_changed_list_display( update_row->update_root->update_changed_list ) );
msg( (char *)0, message );
}
#endif

		update_row->update_one2m_list =
			update_one2m_list_new(
				application_name,
				session_key,
				login_name,
				folder_name,
				role_name,
				row_number,
				relation_one2m_list,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				update_row->update_root->update_changed_list );
	}

	if ( list_length( relation_mto1_isa_list ) )
	{
		update_row->update_mto1_isa_list =
			update_mto1_isa_list(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name /* many_folder_name */,
				row_number,
				relation_mto1_isa_list,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				(update_row->update_root)
				   ? update_row->
					update_root->
					update_changed_primary_key_boolean
				   : 0,
				update_row->update_attribute_list );
	}

	if ( !update_row->update_root
	&&   !update_row->update_one2m_list
	&&   !list_length( update_row->update_mto1_isa_list ) )
	{
		free( update_row );
		return NULL;
	}

	return update_row;
}

UPDATE_ATTRIBUTE *update_attribute_calloc( void )
{
	UPDATE_ATTRIBUTE *update_attribute;

	if ( ! ( update_attribute =
			calloc( 1, sizeof ( UPDATE_ATTRIBUTE ) ) ) )
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

	if ( ! ( update_mto1_isa = calloc( 1, sizeof ( UPDATE_MTO1_ISA ) ) ) )
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

	if ( ! ( update_one2m = calloc( 1, sizeof ( UPDATE_ONE2M ) ) ) )
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
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		int row_number,
		RELATION_MTO1 *relation_mto1_isa,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		boolean update_changed_primary_key_boolean,
		LIST *update_attribute_list )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !many_folder_name
	||   !relation_mto1_isa
	||   !relation_mto1_isa->one_folder
	||   !list_length(
		relation_mto1_isa->
			one_folder->
			folder_attribute_primary_key_list )
	||   !list_length(
		relation_mto1_isa->
			one_folder->
			folder_attribute_primary_key_list )
	||   !list_length(
		relation_mto1_isa->
			one_folder->
			folder_attribute_list )
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !list_length( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: relation_mto1_isa->one_folder_name=[%s]\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	relation_mto1_isa->one_folder_name );
msg( (char *)0, message );
}
#endif

	update_mto1_isa = update_mto1_isa_calloc();
	update_mto1_isa->relation_mto1_isa = relation_mto1_isa;

	update_mto1_isa->update_attribute_list =
		update_mto1_isa_update_attribute_list(
			many_folder_name,
			relation_mto1_isa->
				one_folder->
				folder_attribute_list
				/* one_folder_attribute_list */,
			relation_mto1_isa->relation_translate_list,
			update_attribute_list
				/* many_update_attribute_list */ );

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: update_attribute_list=[%s]\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	update_attribute_list_display( update_mto1_isa->update_attribute_list ) );
msg( (char *)0, message );
}
#endif

	if ( !list_length( update_mto1_isa->update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message,
"for one_folder_name=%s, update_mto1_isa_update_attribute_list() returned empty.",
			relation_mto1_isa->one_folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_mto1_isa->update_changed_list =
		update_changed_list_new(
			relation_mto1_isa->one_folder_name,
			relation_mto1_isa->
				one_folder->
				folder_attribute_primary_key_list,
			relation_mto1_isa->
				one_folder->
				folder_attribute_name_list,
			update_mto1_isa->update_attribute_list,
			(char *)0 /* security_entity_where */ );

	if ( !update_mto1_isa->update_changed_list )
	{
		free( update_mto1_isa );
		return NULL;
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: update_changed_list=[%s]\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	update_changed_list_display( update_mto1_isa->update_changed_list ) );
msg( (char *)0, message );
}
#endif

	if ( relation_mto1_isa->one_folder->post_change_process_name
	&&   !relation_mto1_isa->one_folder->post_change_process )
	{
		relation_mto1_isa->one_folder->post_change_process =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_fetch(
				relation_mto1_isa->
					one_folder->
					post_change_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source */,
				1 /* check_executable */,
				appaserver_parameter_mount_point );
	}

	if ( relation_mto1_isa->one_folder->post_change_process )
	{
		update_mto1_isa->update_where_list_primary_data_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_where_list_primary_data_string(
				SQL_DELIMITER,
				update_mto1_isa->
					update_changed_list->
					where_list
					/* update_where_list */ );

		update_mto1_isa->update_command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_command_line(
				APPASERVER_UPDATE_STATE,
				UPDATE_PREUPDATE_PREFIX,
				relation_mto1_isa->
					one_folder->
					post_change_process->
					command_line,
				session_key,
				login_name,
				role_name,
				relation_mto1_isa->one_folder_name,
				relation_mto1_isa->
					one_folder->
					post_change_process_name,
				appaserver_error_filename,
				update_mto1_isa->
					update_where_list_primary_data_string,
				update_mto1_isa->update_attribute_list );

		update_mto1_isa->update_changed_list->update_command_line =
			update_mto1_isa->update_command_line;
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: update_changed_primary_key_boolean=%d\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	update_changed_primary_key_boolean );
msg( (char *)0, message );
}
#endif

	if ( update_changed_primary_key_boolean )
	{
		update_mto1_isa->relation_one2m_list =
			relation_one2m_list(
				relation_mto1_isa->one_folder_name,
				relation_mto1_isa->
					one_folder->
					folder_attribute_primary_key_list,
				1 /* include_isa_boolean */ );

		if ( list_length( update_mto1_isa->relation_one2m_list ) )
		{
#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: recursively calling update_one2m_list_new() with one_folder_name=%s;\nupdate_changed_list=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	relation_mto1_isa->one_folder_name,
	update_changed_list_display( update_mto1_isa->update_changed_list ) );
msg( (char *)0, message );
}
#endif

			update_mto1_isa->update_one2m_list =
				update_one2m_list_new(
					application_name,
					session_key,
					login_name,
					relation_mto1_isa->one_folder_name,
					role_name,
					row_number,
					update_mto1_isa->relation_one2m_list,
					appaserver_error_filename,
					appaserver_parameter_mount_point,
					update_mto1_isa->update_changed_list );
		}
	}

	return update_mto1_isa;
}

UPDATE_ROW_LIST *update_row_list_calloc( void )
{
	UPDATE_ROW_LIST *update_row_list;

	if ( ! ( update_row_list =
			calloc( 1, sizeof ( UPDATE_ROW_LIST ) ) ) )
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
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		LIST *relation_one2m_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process,
		SECURITY_ENTITY *security_entity,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point )
{
	UPDATE_ROW_LIST *update_row_list;
	int row_number;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( multi_row_dictionary )
	||   !dictionary_length( file_dictionary ) )
	{
		return NULL;
	}

	update_row_list = update_row_list_calloc();

	if ( ( update_row_list->dictionary_highest_index =
		dictionary_highest_index(
			multi_row_dictionary ) ) < 1 )
	{
		free( update_row_list );
		return NULL;
	}

	update_row_list->list = list_new();

	for(	row_number = 1;
		row_number <= update_row_list->dictionary_highest_index;
		row_number++ )
	{
		(void)relation_copy_new(
			multi_row_dictionary /* in/out */,
			folder_attribute_append_isa_list,
			relation_mto1_list,
			row_number );

		list_set(
			update_row_list->list,
			update_row_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				multi_row_dictionary,
				file_dictionary,
				relation_one2m_list,
				relation_mto1_isa_list,
				folder_attribute_append_isa_list,
				post_change_process,
				security_entity,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				row_number ) );
	}

	if ( list_length( update_row_list->list ) )
	{
		update_row_list_update_changed_list_list_set(
			update_row_list->list
				/* update_row_list in/out */ );

		update_row_list->cell_count =
			update_row_list_cell_count(
				update_row_list->list
					/* update_row_list */ );

		update_row_list->folder_name_list =
			update_row_list_folder_name_list(
				update_row_list->list
					/* update_row_list */ );

		update_row_list->folder_name_list_string =
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			update_row_list_folder_name_list_string(
				UPDATE_FOLDER_NAME_DELIMITER,
				update_row_list->folder_name_list );

		return update_row_list;
	}
	else
	{
		return NULL;
	}
}

LIST *update_mto1_isa_list(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		int row_number,
		LIST *relation_mto1_isa_list,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		boolean update_changed_primary_key_boolean,
		LIST *update_attribute_list )
{
	LIST *list = list_new();
	RELATION_MTO1 *relation_mto1_isa;
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !many_folder_name
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !list_length( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: with many_folder_name=%s; relation_mto1_isa_list=[%s]; update_attribute_list=[%s]\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	relation_mto1_list_display( relation_mto1_isa_list ),
	update_attribute_list_display( update_attribute_list ) );
msg( (char *)0, message );
}
#endif

	if ( list_rewind( relation_mto1_isa_list ) )
	do {
		relation_mto1_isa =
			list_get(
				relation_mto1_isa_list );

		update_mto1_isa =
			update_mto1_isa_new(
				application_name,
				session_key,
				login_name,
				role_name,
				many_folder_name,
				row_number,
				relation_mto1_isa,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				update_changed_primary_key_boolean,
				update_attribute_list );

		if ( update_mto1_isa ) list_set( list, update_mto1_isa );

	} while ( list_next( relation_mto1_isa_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

UPDATE_ONE2M *update_one2m_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		char *many_folder_name,
		LIST *many_primary_key_list,
		LIST *many_folder_attribute_list,
		LIST *relation_foreign_key_list,
		LIST *relation_translate_list,
		PROCESS *many_post_change_process,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list )
{
	UPDATE_ONE2M *update_one2m;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !row_number
	||   !many_folder_name
	||   !list_length( many_primary_key_list )
	||   !list_length( many_folder_attribute_list )
	||   !list_length( relation_foreign_key_list )
	||   !list_length( relation_translate_list )
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !update_changed_list )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_one2m = update_one2m_calloc();

	update_one2m->many_folder_name = many_folder_name;

	update_one2m->select_name_list =
		update_one2m_select_name_list(
			many_primary_key_list,
			relation_foreign_key_list );

	update_one2m->select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_one2m_select_string(
			update_one2m->select_name_list );

	update_one2m->where_query_cell_list =
		update_one2m_where_query_cell_list(
			update_changed_list->where_list
				/* update_where_list */,
			relation_translate_list,
			many_folder_attribute_list );

	update_one2m->query_cell_where_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_cell_where_string(
			update_one2m->where_query_cell_list
				/* query_row_cell_list */ );

	if ( !update_one2m->query_cell_where_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"query_cell_where_string() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_one2m->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			update_one2m->select_string,
			many_folder_name
				/* from_string */,
			update_one2m->query_cell_where_string,
			(char *)0 /* group_by_string */,
			(char *)0 /* order_string */,
			0 /* max_rows */ );

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: with many_folder_name=%s;\nquery_system_string=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	update_one2m->query_system_string );
msg( (char *)0, message );
}
#endif

	update_one2m->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			many_folder_name,
			many_folder_attribute_list,
			date_convert_international
				/* date_convert_format_enum */,
			update_one2m->select_name_list,
			update_one2m->query_system_string,
			1 /* input_save_boolean */ );

	if ( !list_length( update_one2m->query_fetch->row_list ) )
	{
		/* Todo: free heap memory */
		free( update_one2m );
		return NULL;
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: calling update_one2m_fetch_new() with many_folder_name=%s;many_primary_key_list=%s;foreign_key_list=%s; list_length(query_fetch->row_list)=%d\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	list_display( many_primary_key_list ),
	relation_translate_foreign_delimited_string( relation_translate_list ),
	list_length( update_one2m->query_fetch->row_list ) );
msg( (char *)0, message );
}
#endif

	update_one2m->update_one2m_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		update_one2m_fetch_new(
			application_name,
			session_key,
			login_name,
			role_name,
			row_number,
			many_folder_name,
			many_primary_key_list,
			many_folder_attribute_list,
			relation_translate_list,
			many_post_change_process,
			appaserver_error_filename,
			appaserver_parameter_mount_point,
			update_changed_list,
			update_one2m->query_fetch->row_list );

	return update_one2m;
}

char *update_changed_list_sql_statement_string(
		char *table_name,
		char *set_string,
		char *where_string )
{
	char sql_statement_string[ STRING_128K ];

	if ( !table_name
	||   !set_string
	||   !where_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if (	strlen( table_name ) +
		strlen( set_string ) +
		strlen( where_string ) +
		20 >= sizeof ( sql_statement_string ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			STRING_OVERFLOW_TEMPLATE,
			(int)sizeof ( sql_statement_string ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		sql_statement_string,
		sizeof ( sql_statement_string ),
		"update %s set %s where %s;",
		table_name,
		set_string,
		where_string );

	return strdup( sql_statement_string );
}

char *update_row_list_execute(
		const char *sql_executable,
		char *application_name,
		UPDATE_ROW_LIST *update_row_list,
		char *appaserver_error_filename,
		boolean update_root_boolean )
{
	char error_string[ STRING_64K ];
	char *ptr = error_string;
	UPDATE_ROW *update_row;
	char *update_error_string;

	if ( !application_name
	||   !update_row_list )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_row_list->list ) )
	do {
		update_row = list_get( update_row_list->list );

		update_error_string =
			update_changed_list_list_execute(
				sql_executable,
				application_name,
				update_row->update_changed_list_list,
				appaserver_error_filename,
				update_root_boolean );

		if ( update_error_string )
		{
			if ( ptr != error_string )
			{
				ptr += sprintf( ptr, "\n<br>" );
			}

			ptr += sprintf(
				ptr,
				"%s",
				update_error_string );
		}

	} while ( list_next( update_row_list->list ) );

	if ( ptr == error_string )
		return (char *)0;
	else
		return strdup( error_string );
}

char *update_command_line(
		const char *appaserver_update_state,
		const char *update_preupdate_prefix,
		char *post_change_process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *process_name,
		char *appaserver_error_filename,
		char *primary_data_list_string,
		LIST *update_attribute_list )
{
	char command_line[ STRING_8K ];
	char preupdate_attribute_name[ 128 ];
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !post_change_process_command_line
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !process_name
	||   !appaserver_error_filename
	||   !primary_data_list_string
	||   !list_rewind( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy(
		command_line,
		post_change_process_command_line,
		STRING_8K );

	string_replace_command_line(
		command_line,
		session_key,
		PROCESS_SESSION_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_NAME_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		role_name,
		PROCESS_ROLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		folder_name,
		PROCESS_FOLDER_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		folder_name,
		PROCESS_TABLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		process_name,
		PROCESS_NAME_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		(char *)appaserver_update_state,
		PROCESS_STATE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		primary_data_list_string,
		PROCESS_PRIMARY_PLACEHOLDER );

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

		if ( !update_attribute->file_datum )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: file_datum is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		string_with_space_search_replace(
			command_line,
			update_attribute->folder_attribute->attribute_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_double_quotes_around(
				update_attribute->post_datum ) );

		if ( update_changed_boolean(
			QUERY_IS_NULL,
			update_attribute->
				file_datum,
			update_attribute->
				post_datum ) )
		{
			sprintf(preupdate_attribute_name,
				"%s%s",
				update_preupdate_prefix,
				update_attribute->
					folder_attribute->
					attribute_name );

			string_with_space_search_replace(
				command_line,
				preupdate_attribute_name,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_double_quotes_around(
					update_attribute->
						file_datum ) );
		}

	} while ( list_next( update_attribute_list ) );

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	string_escape_dollar( command_line );
}

char *update_command_line_list_execute( LIST *command_line_list )
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

LIST *update_one2m_changed_list(
		char *many_folder_name,
		LIST *update_changed_list,
		LIST *relation_foreign_key_list )
{
	UPDATE_CHANGED *changed_primary;
	UPDATE_CHANGED *changed_foreign;
	LIST *one2m_changed_list;
	char *changed_attribute_name;

	if ( !many_folder_name
	||   !list_length( relation_foreign_key_list )
	||   !list_rewind( update_changed_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	one2m_changed_list = list_new();

	do {
		changed_primary =
			list_get(
				update_changed_list );

		if ( !changed_primary->update_attribute
		||   !changed_primary->update_attribute->folder_attribute )
		{
			char message[ 128 ];

			sprintf(message, "changed_primary is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( changed_primary->
			update_attribute->
			folder_attribute->
			primary_key_index )
		{
			changed_foreign = update_changed_calloc();

			changed_foreign->update_attribute =
				update_attribute_calloc();

			changed_attribute_name =
				/* ------------------------------------- */
				/* Returns component of foreign_key_list */
				/* ------------------------------------- */
				update_one2m_changed_attribute_name(
					changed_primary->
						update_attribute->
						folder_attribute->
						primary_key_index,
					relation_foreign_key_list );

			changed_foreign->update_attribute->folder_attribute =
				folder_attribute_fetch(
					many_folder_name,
					changed_attribute_name,
					1 /* fetch_attribute */ );

			/* This is for debugging; file_datum isn't used. */
			/* --------------------------------------------- */
			changed_foreign->
				update_attribute->
				file_datum =
					changed_primary->
						update_attribute->
						file_datum;

			changed_foreign->
				update_attribute->
				post_datum =
					changed_primary->
						update_attribute->
						post_datum;

			changed_foreign->set_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				update_changed_set_string(
					QUERY_IS_NULL,
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
					       post_datum );

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
		LIST *relation_foreign_key_list )
{
	UPDATE_WHERE *where_primary;
	UPDATE_WHERE *where_foreign;
	char *foreign_key;
	LIST *one2m_where_list;

	if ( !many_folder_name
	||   !list_length( update_where_list )
	||   !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( update_where_list ) !=
	     list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"list length mismatch: %d vs. %d.",
			list_length( update_where_list ),
			list_length( relation_foreign_key_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_rewind( update_where_list );
	list_rewind( relation_foreign_key_list );

	one2m_where_list = list_new();

	do {
		foreign_key = list_get( relation_foreign_key_list );

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

		where_foreign = update_where_calloc();

		where_foreign->update_attribute = update_attribute_calloc();

		where_foreign->
			update_attribute->
			folder_attribute =
				folder_attribute_fetch(
					many_folder_name,
					foreign_key,
					1 /* fetch_attribute */ );

		where_foreign->
			update_attribute->
			file_datum =
				where_primary->
					update_attribute->
					file_datum;

		where_foreign->
			update_attribute->
			post_datum =
				where_primary->
					update_attribute->
					post_datum;

		where_foreign->string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_where_string(
				foreign_key /* attribute_name */,
				where_foreign->
					update_attribute->
					folder_attribute->
					attribute->
					datatype_name,
				where_foreign->
					update_attribute->
					file_datum );

		list_set( one2m_where_list, where_foreign );

		list_next( update_where_list );

	} while ( list_next( relation_foreign_key_list ) );

	return one2m_where_list;
}

char *update_one2m_changed_attribute_name(
		int primary_key_index,
		LIST *relation_foreign_key_list )
{
	char *attribute_name;

	if ( ! ( attribute_name =
			/* ------------------ */
			/* index is one based */
			/* ------------------ */
			list_index_seek(
				primary_key_index,
				relation_foreign_key_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: list_index_seek(%d) for [%s] returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			primary_key_index,
			list_display( relation_foreign_key_list ) );
		exit( 1 );
	}

	return attribute_name;
}

UPDATE_CHANGED *update_changed_seek(
		LIST *update_changed_list,
		char *attribute_name )
{
	UPDATE_CHANGED *update_changed;

	if ( !attribute_name ) return NULL;

	if ( list_rewind( update_changed_list ) )
	do {
		update_changed = list_get( update_changed_list );

		if ( !update_changed->update_attribute
		||   !update_changed->update_attribute->folder_attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"update_changed_is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

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

	return NULL;
}

UPDATE_CHANGED_LIST *update_changed_list_new(
		char *folder_name,
		LIST *primary_key_list,
		LIST *folder_attribute_name_list,
		LIST *update_attribute_list,
		char *security_entity_where )
{
	UPDATE_CHANGED_LIST *update_changed_list;
	char *attribute_name;
	UPDATE_CHANGED *update_changed;
	char *primary_key;
	UPDATE_WHERE *update_where;

	if ( !folder_name
	||   !list_rewind( primary_key_list )
	||   !list_rewind( folder_attribute_name_list )
	||   !list_length( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_changed_list = update_changed_list_calloc();
	update_changed_list->folder_name = folder_name;

	do {
		attribute_name =
			list_get(
				folder_attribute_name_list );

		update_changed =
			update_changed_new(
				folder_name,
				attribute_name,
				update_attribute_list );

		if ( update_changed )
		{
			if ( !update_changed_list->list )
			{
				update_changed_list->list = list_new();
			} 

			list_set(
				update_changed_list->list,
				update_changed );
		}

	} while ( list_next( folder_attribute_name_list ) );

	if ( !list_length( update_changed_list->list ) ) return NULL;

	update_changed_list->set_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_changed_list_set_string(
			update_changed_list->list /* changed_list */ );

	update_changed_list->where_list = list_new();

	do {
		primary_key =
			list_get(
				primary_key_list );

		if ( ! ( update_where =
				update_where_new(
					folder_name,
					primary_key,
					update_attribute_list ) ) )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
"for folder_name=%s, update_attribute_list = [%s], update_where_new(%s) returned empty.",
				folder_name,
				update_attribute_list_display(
					update_attribute_list ),
				primary_key );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			update_changed_list->where_list,
			update_where );

	} while ( list_next( primary_key_list ) );

	update_changed_list->update_where_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_where_list_string(
			security_entity_where,
			update_changed_list->where_list );


	update_changed_list->sql_statement_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_changed_list_sql_statement_string(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_table_name(
				folder_name ),
			update_changed_list->set_string,
			update_changed_list->update_where_list_string );

	return update_changed_list;
}

UPDATE_CHANGED_LIST *update_changed_list_calloc( void )
{
	UPDATE_CHANGED_LIST *update_changed_list;

	if ( ! ( update_changed_list =
			calloc( 1,
				sizeof ( UPDATE_CHANGED_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return update_changed_list;
}

char *update_changed_list_set_string( LIST *changed_list )
{
	char set_string[ STRING_128K ];
	char *ptr = set_string;
	UPDATE_CHANGED *update_changed;

	if ( !list_rewind( changed_list ) )
	{
		char message[ 128 ];

		sprintf(message, "changed_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		update_changed = list_get( changed_list );

		if ( !update_changed->set_string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: set_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if (	strlen( set_string ) +
			strlen( update_changed->set_string ) + 1
			>= STRING_128K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != set_string ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			update_changed->set_string );

	} while ( list_next( changed_list ) );

	return strdup( set_string );
}

UPDATE_WHERE *update_where_new(
		char *folder_name,
		char *primary_key,
		LIST *update_attribute_list )
{
	UPDATE_WHERE *update_where = update_where_calloc();

	if ( !folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( ! ( update_where->update_attribute =
			update_attribute_seek(
				folder_name,
				primary_key /* attribute_name */,
				update_attribute_list ) ) )
	{
		free( update_where );
		return NULL;
	}

	if ( !update_where->update_attribute->folder_attribute
	||   !update_where->update_attribute->folder_attribute->attribute )
	{
		char message[ 128 ];

		sprintf(message,
		"update_where->update_attribute is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_where->string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_where_string(
			primary_key,
			update_where->
				update_attribute->
				folder_attribute->
				attribute->
				datatype_name,
			update_where->
				update_attribute->
				file_datum );

	return update_where;
}

UPDATE_WHERE *update_where_calloc( void )
{
	UPDATE_WHERE *update_where;

	if ( ! ( update_where = calloc( 1, sizeof ( UPDATE_WHERE ) ) ) )
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

char *update_where_string(
		char *primary_key,
		char *datatype_name,
		char *datum )
{
	char string[ 1024 ];
	char *format_string;
	char *sql_injection_escape;

	if ( !primary_key
	||   !datum )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if (	strlen( primary_key ) +
		strlen( datum ) + 5 >= 1024 )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			1024 );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( attribute_is_number( datatype_name ) )
		format_string = "%s = %s";
	else
		format_string = "%s = '%s'";

	sql_injection_escape =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			datum );

	snprintf(
		string,
		sizeof ( string ),
		format_string,
		primary_key,
		sql_injection_escape );

	return strdup( string );
}

char *update_where_list_string(
		char *security_entity_where,
		LIST *update_where_list )
{
	UPDATE_WHERE *update_where;
	char string[ 2048 ];
	char *ptr = string;

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

		if ( !update_where->string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: update_where->string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if (	strlen( string ) +
			strlen( update_where->string ) + 5 >= 2048 )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				2048 );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf( ptr, "%s", update_where->string );

	} while ( list_next( update_where_list ) );

	if ( security_entity_where )
	{
		sprintf(string + strlen( string ),
			" and %s",
			security_entity_where );
	}

	return strdup( string );
}

UPDATE_CHANGED *update_changed_new(
		char *folder_name,
		char *attribute_name,
		LIST *update_attribute_list )
{
	boolean changed_boolean;
	UPDATE_CHANGED *update_changed;

	if ( !attribute_name
	||   !list_length( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_changed = update_changed_calloc();

	if ( ! ( update_changed->update_attribute =
			update_attribute_seek(
				folder_name,
				attribute_name,
				update_attribute_list ) ) )
	{
		free( update_changed );
		return NULL;
	}

	changed_boolean =
		update_changed_boolean(
			QUERY_IS_NULL,
			update_changed->
				update_attribute->
				file_datum,
			update_changed->
				update_attribute->
				post_datum );

	if ( !changed_boolean )
	{
		free( update_changed );
		return NULL;
	}

	if ( !update_changed->update_attribute->folder_attribute
	||   !update_changed->update_attribute->folder_attribute->attribute )
	{
		char message[ 128 ];

		sprintf(message,
		"update_changed->update_attribute is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_changed->set_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_changed_set_string(
			QUERY_IS_NULL,
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
				post_datum );

	return update_changed;
}

UPDATE_CHANGED *update_changed_calloc( void )
{
	UPDATE_CHANGED *update_changed;

	if ( ! ( update_changed = calloc( 1, sizeof ( UPDATE_CHANGED ) ) ) )
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
		const char *query_is_null,
		char *file_datum,
		char *post_datum )
{
	char *unescape;
	int return_value = 0;

	if ( !post_datum )
	{
		char message[ 128 ];

		sprintf(message, "post_datum is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ( !file_datum || !*file_datum ) && !*post_datum ) return 0;

	if (	!*file_datum
	&&	strcmp( post_datum, query_is_null ) == 0 )
	{
		return 0;
	}

	unescape =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_unescape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			post_datum );

	if ( string_strcmp( file_datum, unescape ) != 0 )
	{
		return_value = 1;
	}

	if ( post_datum != unescape ) free( unescape );

	return return_value;
}

char *update_changed_set_string(
		const char *query_is_null,
		char *attribute_name,
		char *datatype_name,
		char *post_datum )
{
	char *format_string;
	char set_string[ STRING_64K ];

	if ( !attribute_name
	||   !post_datum )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*set_string = '\0';

	if ( !*post_datum
	||   strcmp( post_datum, "/" ) == 0
	||   strcmp( post_datum, query_is_null ) == 0 )
	{
		sprintf(set_string,
			"%s = null",
			attribute_name );
	}
	else
	if ( datatype_name && attribute_is_number( datatype_name ) )
		format_string = "%s = %s";
	else
		format_string = "%s = '%s'";

	if ( !*set_string )
	{
		snprintf(
			set_string,
			sizeof ( set_string ),
			format_string,
			attribute_name,
			post_datum );
	}

	return strdup( set_string );
}

LIST *update_changed_primary_data_list( LIST *update_changed_list )
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
			post_datum )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: post_datum is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			data_list,
			update_changed->
				update_attribute->
				post_datum );

	} while ( list_next( update_changed_list ) );

	return data_list;
}

boolean update_changed_primary_key_boolean( LIST *update_changed_list )
{
	UPDATE_CHANGED *update_changed;

	if ( list_rewind( update_changed_list ) )
	do {
		update_changed = list_get( update_changed_list );

		if ( !update_changed->update_attribute
		||   !update_changed->update_attribute->folder_attribute )
		{
			char message[ 128 ];

			sprintf(message,
	"update_attribute or update_attribute->folder_attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
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

char *update_where_list_primary_data_string(
		char sql_delimiter,
		LIST *update_where_list )
{
	UPDATE_WHERE *update_where;
	char primary_data_delimited[ 1024 ];
	char *ptr = primary_data_delimited;
	char *datum;

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
			file_datum )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: file_datum is empty.\n",
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
			QUERY_IS_NULL,
			update_where->
				update_attribute->
				file_datum,
			update_where->
				update_attribute->
				post_datum ) )
		{
			datum =
				update_where->
					update_attribute->
					post_datum;
		}
		else
		{
			datum =
				update_where->
					update_attribute->
					file_datum;
		}

		ptr += sprintf(
			ptr,
			"%s",
			datum );

	} while ( list_next( update_where_list ) );

	return strdup( primary_data_delimited );
}

LIST *update_attribute_list(
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		LIST *folder_attribute_append_isa_list,
		int row_number )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *attribute_list;

	if ( !dictionary_length( file_dictionary ) ) return (LIST *)0;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_attribute_append_isa_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	attribute_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		list_set(
			attribute_list,
			update_attribute_new(
				multi_row_dictionary,
				file_dictionary,
				row_number,
				folder_attribute ) );

	} while ( list_next( folder_attribute_append_isa_list ) );

	return attribute_list;
}

UPDATE_ATTRIBUTE *update_attribute_new(
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		int row_number,
		FOLDER_ATTRIBUTE *folder_attribute )
{
	UPDATE_ATTRIBUTE *update_attribute;
	char *key;

	if ( !folder_attribute
	||   !folder_attribute->attribute_name
	||   !folder_attribute->attribute
	||   !folder_attribute->attribute->datatype_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute is incomplete.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_attribute = update_attribute_calloc();
	update_attribute->folder_attribute = folder_attribute;

	key =
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			folder_attribute->attribute_name
				/* widget_name */,
			row_number );

	update_attribute->post_datum =
		dictionary_get(
			key,
			multi_row_dictionary );

	if ( !update_attribute->post_datum )
	{
		free( update_attribute );
		return NULL;
	}

	update_attribute->file_datum =
		dictionary_get(
			key,
			file_dictionary );

	if ( ( !update_attribute->file_datum || !*update_attribute->file_datum )
	&&   attribute_is_date_time(
		folder_attribute->
			attribute->
			datatype_name ) )
	{
		update_attribute->post_datum =
			/* ---------------------------------- */
			/* Returns heap memory or date_string */
			/* ---------------------------------- */
			date_append_time(
				update_attribute->post_datum
					/* date_string */,
				folder_attribute->attribute->width );
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

		if ( !update_attribute->file_datum
		&&   !update_attribute->post_datum )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: both file_data and post_data are empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( update_changed_boolean(
			QUERY_IS_NULL,
			update_attribute->file_datum,
			update_attribute->post_datum ) )
		{
			list_set(
				data_list,
				update_attribute->
					post_datum );
		}
		else
		{
			list_set(
				data_list,
				update_attribute->
					file_datum );
		}

	} while ( list_next( update_attribute_list ) );

	return data_list;
}

UPDATE_ATTRIBUTE *update_attribute_seek(
		char *folder_name,
		char *attribute_name,
		LIST *update_attribute_list )
{
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_attribute_list ) )
	do {
		update_attribute = list_get( update_attribute_list );

		if ( !update_attribute->folder_attribute )
		{
			char message[ 128 ];

			sprintf(message,
			"update_attribute->folder_attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( strcmp(
			update_attribute->
				folder_attribute->
				folder_name,
			folder_name ) != 0 )
		{
			continue;
		}

		if ( strcmp(
			update_attribute->folder_attribute->attribute_name,
			attribute_name ) == 0 )
		{
			return update_attribute;
		}

	} while ( list_next( update_attribute_list ) );

	return NULL;
}

UPDATE_ONE2M_ROW *update_one2m_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		char *many_folder_name,
		LIST *many_primary_key_list,
		LIST *many_folder_attribute_list,
		LIST *relation_translate_list,
		PROCESS *many_post_change_process,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list,
		LIST *query_row_cell_list )
{
	UPDATE_ONE2M_ROW *update_one2m_row;
	boolean changed_primary_key_boolean;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !many_folder_name
	||   !list_length( relation_translate_list )
	||   !list_length( many_folder_attribute_list )
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !update_changed_list
	||   !list_length( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: received many_folder_name=%s;many_primary_key_list=%s;foreign_key_list=%s;\nupdate_changed_list=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	list_display( many_primary_key_list ),
	relation_translate_foreign_delimited_string( relation_translate_list ),
	update_changed_list_display( update_changed_list ) );
msg( (char *)0, message );
}
#endif

	update_one2m_row = update_one2m_row_calloc();
	update_one2m_row->many_folder_name = many_folder_name;

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: calling update_one2m_row_update_attribute_list() with many_folder_name=%s;many_primary_key_list=%s;foreign_key_list=%s;\nupdate_changed_list=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	list_display( many_primary_key_list ),
	relation_translate_foreign_delimited_string( relation_translate_list ),
	update_changed_list_display( update_changed_list ) );
msg( (char *)0, message );
}
#endif

	update_one2m_row->update_attribute_list =
		update_one2m_row_update_attribute_list(
			many_primary_key_list,
			many_folder_attribute_list,
			relation_translate_list,
			update_changed_list->list
				/* update_changed_list */,
			query_row_cell_list );

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: calling update_one2m_row_update_changed_list() with many_folder_name=%s;\nupdate_attribute_list=[%s]\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	update_attribute_list_display( update_one2m_row->update_attribute_list ) );
msg( (char *)0, message );
}
#endif

	update_one2m_row->update_changed_list =
		update_one2m_row_update_changed_list(
			many_folder_name,
			many_folder_attribute_list,
			update_one2m_row->update_attribute_list );

	if ( !update_one2m_row->update_changed_list )
	{
		char message[ 128 ];

		sprintf(message,
		"update_one2m_row_update_changed_list_new(%s) returned empty.",
			many_folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( many_post_change_process )
	{
		update_one2m_row->query_cell_primary_data_list =
			query_cell_primary_data_list(
				query_row_cell_list );

		if ( !list_length(
			update_one2m_row->
				query_cell_primary_data_list ) )
		{
			char message[ 128 ];

			sprintf(message,
			"query_cell_primary_data_list() returned empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_one2m_row->command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_one2m_row_command_line(
				APPASERVER_UPDATE_STATE,
				UPDATE_PREUPDATE_PREFIX,
				many_post_change_process->
					command_line,
				session_key,
				login_name,
				role_name,
				many_folder_name,
				many_post_change_process->process_name,
				appaserver_error_filename,
				update_one2m_row->query_cell_primary_data_list,
				update_one2m_row->update_attribute_list );

		update_one2m_row->update_changed_list->update_command_line =
			update_one2m_row->command_line;
	}

	changed_primary_key_boolean =
		update_changed_primary_key_boolean(
			update_one2m_row->
				update_changed_list->
				list /* update_changed_list */ );

	if ( changed_primary_key_boolean )
	{
		update_one2m_row->relation_one2m_list =
			relation_one2m_list(
				many_folder_name,
				folder_attribute_primary_key_list(
					many_folder_name,
					many_folder_attribute_list ),
				1 /* include_isa_boolean */ );

		if ( list_length( update_one2m_row->relation_one2m_list ) )
		{
#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: recursively calling update_one2m_list_new() with many_folder_name=%s;\nupdate_changed_list=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	update_changed_list_display( update_one2m_row->update_changed_list ) );
msg( (char *)0, message );
}
#endif

			update_one2m_row->update_one2m_list =
				update_one2m_list_new(
					application_name,
					session_key,
					login_name,
					many_folder_name,
					role_name,
					row_number,
					update_one2m_row->relation_one2m_list,
					appaserver_error_filename,
					appaserver_parameter_mount_point,
					update_one2m_row->update_changed_list );
		}
	}

	return update_one2m_row;
}

LIST *update_one2m_row_update_attribute_list(
		LIST *many_primary_key_list,
		LIST *many_folder_attribute_list,
		LIST *relation_translate_list,
		LIST *update_changed_list,
		LIST *query_row_cell_list )
{
	LIST *update_attribute_list;
	QUERY_CELL *query_cell;
	char *primary_key;
	UPDATE_CHANGED *update_changed;
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !list_length( many_primary_key_list )
	||   !list_length( many_folder_attribute_list )
	||   !list_length( relation_translate_list )
	||   !list_length( update_changed_list )
	||   !list_rewind( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: received many_primary_key_list=%s; foreign_key_list=%s\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	list_display( many_primary_key_list ),
	relation_translate_foreign_delimited_string( relation_translate_list ) );
msg( (char *)0, message );
}
#endif

	update_attribute_list = list_new();

	do {
		query_cell = list_get( query_row_cell_list );

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: query_cell=%s\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	query_cell_display( query_cell ) );
msg( (char *)0, message );
}
#endif

		primary_key =
			relation_translate_primary_key(
				relation_translate_list,
				query_cell->attribute_name
					/* foreign_key */ );

		update_changed =
			update_changed_seek(
				update_changed_list,
				primary_key /* attribute_name */ );

		update_attribute =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			update_one2m_row_update_attribute(
				many_folder_attribute_list,
				query_cell->attribute_name,
				query_cell->select_datum,
				/* -------------------------------------- */
				/* Returns component of parameter or null */
				/* -------------------------------------- */
				update_one2m_row_post_datum(
					update_changed ) );

		list_set(
			update_attribute_list,
			update_attribute );

	} while ( list_next( query_row_cell_list ) );

	return update_attribute_list;
}

UPDATE_ATTRIBUTE *update_one2m_row_update_attribute(
		LIST *many_folder_attribute_list,
		char *attribute_name,
		char *query_cell_select_datum,
		char *update_one2m_row_post_datum )
{
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !list_length( many_folder_attribute_list )
	||   !attribute_name
	||   !query_cell_select_datum )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_attribute = update_attribute_calloc();

	if ( ! ( update_attribute->folder_attribute =
			folder_attribute_seek(
				attribute_name,
				many_folder_attribute_list ) ) )
	{
		char message[ 128 ];

		sprintf(message,
		"folder_attribute_seek(attribute_name=%s) returned empty.",
			attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_attribute->post_datum =
	update_attribute->file_datum =
		query_cell_select_datum;

	if ( update_one2m_row_post_datum )
	{
		update_attribute->post_datum =
			update_one2m_row_post_datum;
	}

	return update_attribute;
}

UPDATE_ONE2M_ROW *update_one2m_row_calloc( void )
{
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( ! ( update_one2m_row =
			calloc( 1, sizeof ( UPDATE_ONE2M_ROW ) ) ) )
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

char *update_one2m_row_where_string( LIST *update_attribute_list )
{
	char where_string[ STRING_4K ];
	char *ptr = where_string;
	UPDATE_ATTRIBUTE *update_attribute;
	char *string;

	if ( !list_rewind( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "update_attribute_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		update_attribute = list_get( update_attribute_list );

		if ( !update_attribute->folder_attribute
		||   !update_attribute->folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message, "update_attribute is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_where_string(
				update_attribute->
					folder_attribute->
					attribute_name,
				update_attribute->
					folder_attribute->
					attribute->
					datatype_name,
				update_attribute->post_datum );

		if (	strlen( where_string ) +
			strlen( string ) + 5 >= STRING_4K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_4K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != where_string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf( ptr, "%s", string );

		free( string );

	} while ( list_next( update_attribute_list ) );

	return strdup( where_string );
}

UPDATE_ONE2M_FETCH *update_one2m_fetch_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		char *many_folder_name,
		LIST *many_primary_key_list,
		LIST *many_folder_attribute_list,
		LIST *relation_translate_list,
		PROCESS *many_post_change_process,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list,
		LIST *query_fetch_row_list )
{
	UPDATE_ONE2M_FETCH *update_one2m_fetch;
	QUERY_ROW *query_row;
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !many_folder_name
	||   !list_length( many_primary_key_list )
	||   !list_length( many_folder_attribute_list )
	||   !list_length( relation_translate_list )
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !update_changed_list
	||   !list_rewind( query_fetch_row_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: received many_folder_name=%s;many_primary_key_list=%s;foreign_key_list=%s;list_length(query_fetch_row_list)=%d\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	list_display( many_primary_key_list ),
	relation_translate_foreign_delimited_string( relation_translate_list ),
	list_length( query_fetch_row_list ) );
msg( (char *)0, message );
}
#endif

	update_one2m_fetch = update_one2m_fetch_calloc();

	update_one2m_fetch->row_number = row_number;
	update_one2m_fetch->many_folder_name = many_folder_name;

	update_one2m_fetch->update_one2m_row_list = list_new();

	do {
		query_row = list_get( query_fetch_row_list );

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: calling update_one2m_row_new() with many_folder_name=%s;many_primary_key_list=%s;foreign_key_list=%s;\nupdate_changed_list=%s;query_row=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	list_display( many_primary_key_list ),
	relation_translate_foreign_delimited_string( relation_translate_list ),
	update_changed_list_display( update_changed_list ),
	query_row->input );
msg( (char *)0, message );
}
#endif
		update_one2m_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			update_one2m_row_new(
				application_name,
				session_key,
				login_name,
				role_name,
				row_number,
				many_folder_name,
				many_primary_key_list,
				many_folder_attribute_list,
				relation_translate_list,
				many_post_change_process,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				update_changed_list,
				query_row->cell_list );

		list_set(
			update_one2m_fetch->update_one2m_row_list,
			update_one2m_row );

	} while ( list_next( query_fetch_row_list ) );

	return update_one2m_fetch;
}

UPDATE_ONE2M_FETCH *update_one2m_fetch_calloc( void )
{
	UPDATE_ONE2M_FETCH *update_one2m_fetch;

	if ( ! ( update_one2m_fetch =
			calloc( 1, sizeof ( UPDATE_ONE2M_FETCH ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_one2m_fetch;
}

LIST *update_one2m_row_list_fetch_list(
			char *folder_table_name,
			LIST *primary_key_list,
			char *update_where_list_string )
{
	char system_string[ 1024 ];

	if ( !folder_table_name
	||   !list_length( primary_key_list )
	||   !update_where_list_string )
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
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited( primary_key_list, ',' ),
		folder_table_name,
		update_where_list_string );

	return list_pipe_fetch( system_string );
}

void update_row_command_line_execute(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list )
{
	if ( update_root && update_root->update_command_line )
	{
		security_system(
			SECURITY_FORK_CHARACTER,
			SECURITY_FORK_STRING,
			update_root->update_command_line );
	}

	if ( update_one2m_list )
	{
		update_one2m_list_command_line_execute(
			update_one2m_list->list
				/* update_one2m_list */ );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		update_mto1_isa_list_command_line_execute(
			update_mto1_isa_list );
	}
}

char *update_sql_statement_string(
		LIST *update_column_name_list,
		LIST *update_data_list,
		LIST *primary_key_list,
		LIST *primary_data_list,
		LIST *folder_attribute_list,
		char *table_name )
{
	char statement_string[ STRING_4K ];
	char *ptr = statement_string;
	boolean first_time = 1;
	char *update_column_name;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *datatype_name = {0};
	char *set_string;
	char *where_string;

	if ( !list_length( update_column_name_list )
	||   !list_length( primary_data_list )
	||   !list_length( primary_key_list )
	||   !list_length( primary_data_list )
	||   !table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( update_column_name_list ) !=
	     list_length( update_data_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align: (%s) <> (%s)",
			list_display_delimited(
				update_column_name_list,
				',' ),
			list_display_delimited(
				update_data_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( primary_key_list ) !=
	     list_length( primary_data_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align: (%s) <> (%s)",
			list_display_delimited(
				primary_key_list,
				',' ),
			list_display_delimited(
				primary_data_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"update %s set ",
		table_name );

	list_rewind( update_data_list );
	list_rewind( update_column_name_list );

	do {
		update_column_name = list_get( update_column_name_list );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, "," );

		if ( ( folder_attribute =
			folder_attribute_seek(
				update_column_name,
				folder_attribute_list ) ) )
		{
			if ( !folder_attribute->attribute )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
				"folder_attribute->attribute is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			datatype_name =
				folder_attribute->
					attribute->
					datatype_name;
		}

		set_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_changed_set_string(
				QUERY_IS_NULL,
				update_column_name,
				datatype_name,
				list_get( update_data_list )
					/* post_data */ );

		if (	strlen( statement_string ) +
			strlen( set_string ) >= STRING_4K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_4K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s",
			set_string );

		free( set_string );

		list_next( update_data_list );

	} while ( list_next( update_column_name_list ) );

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_attribute_where_string(
			folder_attribute_list,
			primary_key_list,
			primary_data_list );

	if (	strlen( statement_string ) +
		strlen( where_string ) + 8 >= STRING_4K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_4K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		" where %s;",
		where_string );

	return strdup( statement_string );
}

char *update_system_string(
		const char *sql_executable,
		char *appaserver_error_filename )
{
	static char system_string[ 256 ];

	if ( !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_error_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"tee -a %s | %s 2>>%s",
		appaserver_error_filename,
		sql_executable,
		appaserver_error_filename );

	return system_string;
}

char *update_one2m_row_command_line(
		char *appaserver_update_state,
		char *update_preupdate_prefix,
		char *post_change_process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		char *post_change_process_name,
		char *appaserver_error_filename,
		LIST *query_cell_primary_data_list,
		LIST *update_one2m_row_update_attribute_list )
{
	char *display_delimited;

	if ( !appaserver_update_state
	||   !update_preupdate_prefix
	||   !post_change_process_command_line
	||   !session_key
	||   !login_name
	||   !role_name
	||   !many_folder_name
	||   !post_change_process_name
	||   !appaserver_error_filename
	||   !list_length( query_cell_primary_data_list )
	||   !list_length( update_one2m_row_update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	display_delimited =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			query_cell_primary_data_list,
			ATTRIBUTE_MULTI_KEY_DELIMITER );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	update_command_line(
		appaserver_update_state,
		update_preupdate_prefix,
		post_change_process_command_line,
		session_key,
		login_name,
		role_name,
		many_folder_name,
		post_change_process_name,
		appaserver_error_filename,
		display_delimited /* primary_data_list_string */,
		update_one2m_row_update_attribute_list );
}

void update_one2m_row_list_pipe_execute(
		LIST *update_one2m_row_list,
		FILE *output_pipe )
{
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( !output_pipe )
	{
		char message[ 128 ];

		sprintf(message, "output_pipe is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_one2m_row_list ) )
	do {
		update_one2m_row =
			list_get(
				update_one2m_row_list );

		fprintf(output_pipe,
			"%s\n",
			update_one2m_row->
				update_changed_list->
				sql_statement_string );

		if ( update_one2m_row->update_one2m_list )
		{
			update_one2m_list_sql_statement_execute(
				update_one2m_row->
					update_one2m_list->
					list
					/* update_one2m_row_list */,
				output_pipe );
		}

	} while ( list_next( update_one2m_row_list ) );
}

void update_one2m_row_list_command_execute(
		LIST *update_one2m_row_list )
{
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( list_rewind( update_one2m_row_list ) )
	do {
		update_one2m_row =
			list_get(
				update_one2m_row_list );

		if ( update_one2m_row->command_line )
		{
			security_system(
				SECURITY_FORK_CHARACTER,
				SECURITY_FORK_STRING,
				update_one2m_row->
			     		command_line );
		}

		if ( update_one2m_row->update_one2m_list )
		{
			update_one2m_list_command_line_execute(
				update_one2m_row->
					update_one2m_list->
					list
					/* update_one2m_row_list */ );
		}

	} while ( list_next( update_one2m_row_list ) );
}

void update_one2m_list_sql_statement_execute(
		LIST *update_one2m_list,
		FILE *output_pipe )
{
	UPDATE_ONE2M *update_one2m;

	if ( !output_pipe )
	{
		char message[ 128 ];

		sprintf(message, "output_pipe is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( update_one2m_list ) ) return;

	do {
		update_one2m = list_get( update_one2m_list );

		update_one2m_row_list_pipe_execute(
			update_one2m->
				update_one2m_fetch->
				update_one2m_row_list,
			output_pipe );

	} while ( list_next( update_one2m_list ) );
}

void update_one2m_list_command_line_execute( LIST *update_one2m_list )
{
	UPDATE_ONE2M *update_one2m;

	if ( list_rewind( update_one2m_list ) )
	do {
		update_one2m = list_get( update_one2m_list );

		update_one2m_row_list_command_execute(
			update_one2m->
				update_one2m_fetch->
				update_one2m_row_list );

	} while ( list_next( update_one2m_list ) );
}

void update_mto1_isa_list_sql_statement_execute(
		LIST *update_mto1_isa_list,
		FILE *output_pipe )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !output_pipe )
	{
		char message[ 128 ];

		sprintf(message, "output_pipe is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_mto1_isa_list ) )
	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		update_mto1_isa_sql_statement_execute(
			update_mto1_isa,
			output_pipe );

	} while ( list_next( update_mto1_isa_list ) );
}

void update_mto1_isa_sql_statement_execute(
		UPDATE_MTO1_ISA *update_mto1_isa,
		FILE *appaserver_output_pipe )
{
	if ( !appaserver_output_pipe )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_output_pipe is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !update_mto1_isa
	||   !update_mto1_isa->update_changed_list
	||   !update_mto1_isa->update_changed_list->sql_statement_string )
	{
		char message[ 128 ];

		sprintf(message, "update_mto1_isa is empty or incomplete." );

		pclose( appaserver_output_pipe );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(
		appaserver_output_pipe,
		"%s\n",
		update_mto1_isa->
			update_changed_list->
			sql_statement_string );

	if ( update_mto1_isa->update_one2m_list )
	{
		UPDATE_ONE2M *update_one2m;

		list_rewind( update_mto1_isa->update_one2m_list->list );

		do {
			update_one2m =
				list_get(
					update_mto1_isa->
						update_one2m_list->
						list );

			update_one2m_row_list_pipe_execute(
				update_one2m->
					update_one2m_fetch->
					update_one2m_row_list,
				appaserver_output_pipe );

		} while ( list_next(
				update_mto1_isa->
					update_one2m_list->list ) );
	}
}

void update_mto1_isa_list_command_line_execute( LIST *update_mto1_isa_list )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( list_rewind( update_mto1_isa_list ) )
	do {
		update_mto1_isa =
			list_get(
				update_mto1_isa_list );

		update_mto1_isa_command_line_execute(
			update_mto1_isa );

	} while ( list_next( update_mto1_isa_list ) );
}

void update_mto1_isa_command_line_execute( UPDATE_MTO1_ISA *update_mto1_isa )
{
	if ( !update_mto1_isa )
	{
		char message[ 128 ];

		sprintf(message, "update_mto1_isa is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( update_mto1_isa->update_command_line )
	{
		security_system(
			SECURITY_FORK_CHARACTER,
			SECURITY_FORK_STRING,
			update_mto1_isa->update_command_line );
	}

	if ( update_mto1_isa->update_one2m_list
	&&   list_rewind( update_mto1_isa->update_one2m_list->list ) )
	{
		UPDATE_ONE2M *update_one2m;

		do {
			update_one2m =
				list_get(
					update_mto1_isa->
						update_one2m_list->list );

			update_one2m_row_list_command_execute(
				update_one2m->
					update_one2m_fetch->
					update_one2m_row_list );

		} while ( list_next(
				update_mto1_isa->
					update_one2m_list->
					list ) );
	}
}

char *update_row_execute(
		const char *sql_executable,
		char *application_name,
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list,
		char *appaserver_error_filename )
{
	FILE *output_pipe;

	if ( !application_name
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( update_root )
	{
		char *update_error_string;

		update_error_string =
			update_root_execute(
				sql_executable,
				update_root->
					update_changed_list->
					sql_statement_string
					/* update_sql_statement_string */,
				appaserver_error_filename );

		if ( update_error_string )
		{
			appaserver_error_message_file(
				application_name,
				(char *)0 /* login_name */,
				update_error_string /* message */ );

			return update_error_string;
		}
	}

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			update_system_string(
				sql_executable,
				appaserver_error_filename ) );

	if ( update_one2m_list )
	{
		update_one2m_list_sql_statement_execute(
			update_one2m_list->list,
			output_pipe );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		update_mto1_isa_list_sql_statement_execute(
			update_mto1_isa_list,
			output_pipe );
	}

	pclose( output_pipe );

	return (char *)0;
}

LIST *update_one2m_row_list_folder_name_list( LIST *update_one2m_row_list )
{
	LIST *folder_name_list = list_new();
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( list_rewind( update_one2m_row_list ) )
	do {
		update_one2m_row =
			list_get(
				update_one2m_row_list );

		list_set(
			folder_name_list,
			update_one2m_row->update_changed_list->folder_name );

		if ( update_one2m_row->update_one2m_list )
		{
			list_unique_list(
				folder_name_list,
				update_one2m_list_folder_name_list(
				    update_one2m_row->
					update_one2m_list->
					list /* update_one2m_list */ ) );
		}

	} while ( list_next( update_one2m_row_list ) );

	if ( !list_length( folder_name_list ) )
	{
		list_free( folder_name_list );
		folder_name_list = NULL;
	}

	return folder_name_list;
}

int update_one2m_row_list_cell_count( LIST *update_one2m_row_list )
{
	int cell_count = 0;
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( list_rewind( update_one2m_row_list ) )
	do {
		update_one2m_row =
			list_get(
				update_one2m_row_list );

		cell_count +=
			list_length(
				update_one2m_row->
					update_changed_list->
					list /* update_changed_list */ );

		if ( update_one2m_row->update_one2m_list )
		{
			cell_count +=
				update_one2m_list_cell_count(
					update_one2m_row->
						update_one2m_list->
						list /* update_one2m_list */ );
		}

	} while ( list_next( update_one2m_row_list ) );

	return cell_count;
}

LIST *update_one2m_list_folder_name_list( LIST *update_one2m_list )
{
	LIST *folder_name_list = list_new();
	UPDATE_ONE2M *update_one2m;

	if ( list_rewind( update_one2m_list ) )
	do {
		update_one2m = list_get( update_one2m_list );

		list_set(
			folder_name_list,
			update_one2m->many_folder_name );

		if ( update_one2m->update_one2m_fetch )
		{
			list_unique_list(
				folder_name_list,
				update_one2m_row_list_folder_name_list(
					update_one2m->
						update_one2m_fetch->
						update_one2m_row_list ) );
		}

	} while ( list_next( update_one2m_list ) );

	if ( !list_length( folder_name_list ) )
	{
		list_free( folder_name_list );
		folder_name_list = NULL;
	}

	return folder_name_list;
}

int update_one2m_list_cell_count( LIST *update_one2m_list )
{
	int cell_count = 0;
	UPDATE_ONE2M *update_one2m;

	if ( !list_rewind( update_one2m_list ) ) return 0;

	do {
		update_one2m = list_get( update_one2m_list );

		cell_count +=
			update_one2m_row_list_cell_count(
				update_one2m->
					update_one2m_fetch->
					update_one2m_row_list );

	} while ( list_next( update_one2m_list ) );

	return cell_count;
}

LIST *update_row_folder_name_list(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list )
{
	LIST *folder_name_list = list_new();

	if ( update_root )
	{
		list_set(
			folder_name_list,
			update_root->folder_name );
	}

	if ( update_one2m_list )
	{
		list_unique_list(
			folder_name_list /* in/out */,
			update_one2m_list_folder_name_list(
				update_one2m_list->list
					/* update_one2m_list */ ) );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		list_unique_list(
			folder_name_list /* in/out */,
			update_mto1_isa_list_folder_name_list(
				update_mto1_isa_list ) );
	}

	return folder_name_list;
}

int update_mto1_isa_cell_count( UPDATE_MTO1_ISA *update_mto1_isa )
{
	int cell_count;

	if ( !update_mto1_isa
	||   !update_mto1_isa->update_changed_list
	||   !list_length(
		update_mto1_isa->
			update_changed_list->
			list /* update_changed_list */ ) )
	{
		char message[ 128 ];

		sprintf(message, "update_mto1_isa is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_count =
		list_length(
			update_mto1_isa->
				update_changed_list->
				list /* update_changed_list */ );

	if ( update_mto1_isa->update_one2m_list )
	{
		cell_count +=
			update_one2m_list_cell_count(
				update_mto1_isa->update_one2m_list->list
					/* update_one2m_list */ );
	}

	return cell_count;
}

LIST *update_mto1_isa_folder_name_list( UPDATE_MTO1_ISA *update_mto1_isa )
{
	LIST *folder_name_list;

	if ( !update_mto1_isa
	||   !update_mto1_isa->relation_mto1_isa
	||   !update_mto1_isa->relation_mto1_isa->one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "update_mto1_isa is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	folder_name_list = list_new();

	list_set(
		folder_name_list,
		update_mto1_isa->relation_mto1_isa->one_folder_name );

	if ( update_mto1_isa->update_one2m_list )
	{
		list_unique_list(
			folder_name_list /* in/out */,
			update_one2m_list_folder_name_list(
				update_mto1_isa->update_one2m_list->list
					/* update_one2m_list */ ) );
	}

	return folder_name_list;
}

LIST *update_mto1_isa_list_folder_name_list( LIST *update_mto1_isa_list )
{
	LIST *folder_name_list;
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !list_rewind( update_mto1_isa_list ) ) return (LIST *)0;

	folder_name_list = list_new();

	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		list_unique_list(
			folder_name_list,
			update_mto1_isa_folder_name_list(
				update_mto1_isa ) );

	} while ( list_next( update_mto1_isa_list ) );

	return folder_name_list;
}

int update_mto1_isa_list_cell_count( LIST *update_mto1_isa_list )
{
	int cell_count = 0;
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( list_rewind( update_mto1_isa_list ) )
	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		cell_count +=
			update_mto1_isa_cell_count(
				update_mto1_isa );

	} while ( list_next( update_mto1_isa_list ) );

	return cell_count;
}

LIST *update_row_list_folder_name_list( LIST *update_row_list )
{
	LIST *folder_name_list = list_new();
	UPDATE_ROW *update_row;

	if ( list_rewind( update_row_list ) )
	do {
		update_row = list_get( update_row_list );

		list_unique_list(
			folder_name_list /* in/out */,
			update_row_folder_name_list(
				update_row->update_root,
				update_row->update_one2m_list,
				update_row->update_mto1_isa_list ) );

	} while ( list_next( update_row_list ) );

	if ( !list_length( folder_name_list ) )
	{
		list_free( folder_name_list );
		folder_name_list = NULL;
	}

	return folder_name_list;
}

char *update_row_list_folder_name_list_string(
		const char update_folder_name_delimiter,
		LIST *update_row_list_folder_name_list )
{
	char delimiter_string[ 3 ];

	delimiter_string[ 0 ] = update_folder_name_delimiter;
	delimiter_string[ 1 ] = ' ';
	delimiter_string[ 2 ] = '\0';

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_display_string_delimited(
		update_row_list_folder_name_list,
		delimiter_string );
}

void update_row_list_command_line_execute(
		UPDATE_ROW_LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	UPDATE_CHANGED_LIST *update_changed_list;

	if ( !update_row_list
	||   !list_rewind( update_row_list->list ) )
	{
		return;
	}

	do {
		update_row = list_get( update_row_list->list );

		if ( list_rewind( update_row->update_changed_list_list ) )
		do {
			update_changed_list =
				list_get(
					update_row->
						update_changed_list_list );

			if ( update_changed_list->update_command_line )
			{
				security_system(
					SECURITY_FORK_CHARACTER,
					SECURITY_FORK_STRING,
					update_changed_list->
						update_command_line );
			}

		} while ( list_next( update_row->update_changed_list_list ) );

	}  while ( list_next( update_row_list->list ) );
}

char *update_results_string(
		int cell_count,
		char *update_folder_name_list_string )
{
	char results_string[ 1024 ];
	char buffer[ 512 ];

	if ( !cell_count ) return NULL;

	if ( !update_folder_name_list_string )
	{
		char message[ 128 ];

		sprintf(message, "update_folder_name_list_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( cell_count == 1 )
	{
		sprintf(results_string,
			"Changed 1 cell in %s",
			string_initial_capital(
				buffer,
				update_folder_name_list_string ) );
	}
	else
	{
		sprintf(results_string,
			"Changed %d cells in %s",
			cell_count,
			string_initial_capital(
				buffer,
				update_folder_name_list_string ) );
	}

	return strdup( results_string );
}

UPDATE_ONE2M_LIST *update_one2m_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *folder_name,
		char *role_name,
		int row_number,
		LIST *relation_one2m_list,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list )
{
	UPDATE_ONE2M_LIST *update_one2m_list;
	RELATION_ONE2M *relation_one2m;
	UPDATE_ONE2M *update_one2m;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !folder_name
	||   !role_name
	||   !row_number
	||   !list_rewind( relation_one2m_list )
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !update_changed_list )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: received folder_name=%s and update_changed_list=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	folder_name,
	update_changed_list_display( update_changed_list ) );
msg( (char *)0, message );
}
#endif

	update_one2m_list = update_one2m_list_calloc();

	update_one2m_list->folder_name = folder_name;
	update_one2m_list->row_number = row_number;
	update_one2m_list->list = list_new();

	do {
		relation_one2m = list_get( relation_one2m_list );

		if ( !relation_one2m->many_folder )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_one2m->many_folder is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_length(
			relation_one2m->
				many_folder->
				folder_attribute_list ) )
		{
			char message[ 128 ];

			sprintf(message,
		"for many_folder_name=[%s], folder_attribute_list is empty.",
				relation_one2m->many_folder_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_length(
			relation_one2m->
				relation_translate_list ) )
		{
			char message[ 128 ];

			sprintf(message,
		"for many_folder_name=[%s], relation_translate_list is empty.",
				relation_one2m->many_folder_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		/* Cache PROCESS *post_change_process */
		/* ---------------------------------- */
		if ( relation_one2m->
			many_folder->
			post_change_process_name
		&&   !relation_one2m->
			many_folder->
			post_change_process )
		{
			relation_one2m->
			    many_folder->
			    post_change_process =
				process_fetch(
				relation_one2m->
					many_folder->
					post_change_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source */,
				1 /* check_inside */,
				appaserver_parameter_mount_point );
		}

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: calling update_one2m_new() with: relation_one2m->many_folder_name=%s;primary_key_list=%s;foreign_key_list=%s;\nupdate_changed_list=%s\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	relation_one2m->many_folder_name,
	list_display( relation_one2m->many_folder->folder_attribute_primary_key_list ),
	list_display( relation_one2m->relation_foreign_key_list ),
	update_changed_list_display( update_changed_list ) );
msg( (char *)0, message );
}
#endif

		update_one2m =
			update_one2m_new(
				application_name,
				session_key,
				login_name,
				role_name,
				row_number,
				relation_one2m->many_folder_name,
				relation_one2m->
					many_folder->
					folder_attribute_primary_key_list
					/* many_primary_key_list */,
				relation_one2m->
					many_folder->
					folder_attribute_list
					/* many_folder_attribute_list */,
				relation_one2m->
					relation_foreign_key_list,
				relation_one2m->relation_translate_list,
				relation_one2m->
					many_folder->
					post_change_process,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				update_changed_list );

		if ( update_one2m )
		{
			list_set(
				update_one2m_list->list,
				update_one2m );
		}

	} while ( list_next( relation_one2m_list ) );

	if ( !list_length( update_one2m_list->list ) )
	{
		list_free( update_one2m_list->list );
		free( update_one2m_list );
		update_one2m_list = NULL;
	}

	return update_one2m_list;
}

UPDATE_ONE2M_LIST *update_one2m_list_calloc( void )
{
	UPDATE_ONE2M_LIST *update_one2m_list;

	if ( ! ( update_one2m_list =
			calloc( 1,
				sizeof ( UPDATE_ONE2M_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return update_one2m_list;
}

LIST *update_one2m_where_query_cell_list(
		LIST *update_where_list,
		LIST *relation_translate_list,
		LIST *many_folder_attribute_list )
{
	LIST *query_cell_list;
	UPDATE_WHERE *update_where;
	RELATION_TRANSLATE *relation_translate;
	QUERY_CELL *query_cell;

	if ( !list_rewind( update_where_list )
	||   !list_rewind( relation_translate_list )
	||   !list_length( many_folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_cell_list = list_new();

	do {
		update_where = list_get( update_where_list );

		relation_translate =
			list_get(
				relation_translate_list );

		query_cell =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_cell_parse(
				many_folder_attribute_list,
				0 /* date_convert_format_enum */,
				relation_translate->
					foreign_key
					/* attribute_name */,
				update_where->
					update_attribute->
					file_datum );

		list_set( query_cell_list, query_cell );

		list_next( relation_translate_list );

	} while ( list_next( update_where_list ) );

	return query_cell_list;
}

LIST *update_mto1_isa_update_attribute_list(
		char *many_folder_name,
		LIST *one_folder_attribute_list,
		LIST *relation_translate_list,
		LIST *update_attribute_list )
{
	LIST *list = list_new();
	FOLDER_ATTRIBUTE *folder_attribute;
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !many_folder_name
	||   !list_length( one_folder_attribute_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( one_folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				one_folder_attribute_list );

		update_attribute =
			update_mto1_isa_update_attribute(
				many_folder_name,
				folder_attribute
					/* one_folder_attribute */,
				relation_translate_list,
				update_attribute_list );

		if ( update_attribute )
		{
			list_set( list, update_attribute );
		}

	} while ( list_next( one_folder_attribute_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

UPDATE_ATTRIBUTE *update_mto1_isa_update_attribute(
		char *many_folder_name,
		FOLDER_ATTRIBUTE *one_folder_attribute,
		LIST *relation_translate_list,
		LIST *update_attribute_list )
{
	char *folder_name;
	char *attribute_name;
	UPDATE_ATTRIBUTE *seek;
	UPDATE_ATTRIBUTE *new;

	if ( one_folder_attribute->primary_key_index )
	{
		folder_name = many_folder_name;

		attribute_name =
			relation_translate_foreign_key(
				relation_translate_list,
				one_folder_attribute->attribute_name
					/* primary_key */ );
	}
	else
	{
		folder_name = one_folder_attribute->folder_name;
		attribute_name = one_folder_attribute->attribute_name;
	}

	if ( ! ( seek =
			update_attribute_seek(
				folder_name,
				attribute_name,
				update_attribute_list ) ) )
	{
		return NULL;
	}

	new = update_attribute_calloc();

	new->folder_attribute = one_folder_attribute;
	new->post_datum = seek->post_datum;
	new->file_datum = seek->file_datum;

	return new;
}

char *update_attribute_list_display( LIST *update_attribute_list )
{
	UPDATE_ATTRIBUTE *update_attribute;
	char display[ 65536 ];
	char *ptr = display;

	*ptr = '\0';

	if ( list_rewind( update_attribute_list ) )
	do {
		update_attribute =
			list_get(
				update_attribute_list );

		if ( ptr != display ) ptr += sprintf( ptr, "; " );

		ptr += sprintf(
			ptr,
			"%s\n",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_attribute_display(
				update_attribute->folder_attribute->folder_name,
				update_attribute->
					folder_attribute->
					attribute_name,
				update_attribute->post_datum,
				update_attribute->file_datum ) );

	} while ( list_next( update_attribute_list ) );

	return strdup( display );
}

char *update_attribute_display(
		char *folder_name,
		char *attribute_name,
		char *post_datum,
		char *file_datum )
{
	char display[ 4096 ];

	sprintf(display,
	"folder=%s, attribute_name=%s, post_datum=[%s], file_datum=[%s]",
		folder_name,
		attribute_name,
		post_datum,
		file_datum );

	return strdup( display );
}

char *update_changed_display( UPDATE_CHANGED *update_changed )
{
	char display[ STRING_64K ];
	char *ptr = display;

	if ( !update_changed 
	||   !update_changed->update_attribute
	||   !update_changed->update_attribute->folder_attribute )
	{
		char message[ 128 ];

		sprintf(message, "update_changed is empty or incomlete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"folder=%s, "
		"attribute=%s, "
		"primary=%d, "
		"post_datum=%s, "
		"file_datum=%s",
		update_changed->
			update_attribute->
			folder_attribute->
			folder_name,
		update_changed->
			update_attribute->
			folder_attribute->
			attribute_name,
		update_changed->
			update_attribute->
			folder_attribute->
			primary_key_index,
		update_changed->
			update_attribute->
			post_datum,
		update_changed->
			update_attribute->
			file_datum );

	return strdup( display );
}

char *update_changed_list_display(
		UPDATE_CHANGED_LIST *update_changed_list )
{
	char display[ STRING_65K ];
	char *ptr = display;
	UPDATE_CHANGED *update_changed;

	*ptr = '\0';

	if ( list_rewind( update_changed_list->list ) )
	do {
		update_changed =
			list_get(
				update_changed_list->
					list );

		if ( ptr != display ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"[%s]",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			update_changed_display(
				update_changed ) );

	} while ( list_next( update_changed_list->list ) );

	ptr +=
		sprintf(
			ptr,
			" update_where_list_string=%s, ",
			update_changed_list->
				update_where_list_string );

	ptr +=
		sprintf(
			ptr,
			" sql_statement_string=%s\n\n",
			update_changed_list->
				sql_statement_string );

	return strdup( display );
}

char *update_root_execute(
		const char *sql_executable,
		char *update_sql_statement_string,
		char *appaserver_error_filename )
{
	char system_string[ 128 ];
	SPOOL *spool;
	LIST *list;
	char *update_error_string;

	if ( !update_sql_statement_string
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"tee_process.e '%s %s' | %s",
		"update_log.sh",
		appaserver_error_filename,
		sql_executable );

	/* Safely returns */
	/* -------------- */
	spool =
		spool_new(
			system_string,
			1 /* capture_stderr_boolean */ );

	fprintf(spool->output_pipe,
		"%s\n",
		update_sql_statement_string );

	pclose( spool->output_pipe );
	list = spool_list( spool->output_filename );
	update_error_string = list_first( list );

	return update_error_string;
}

void update_mto1_isa_list_display(
		LIST *update_mto1_isa_list,
		FILE *output_stream )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !output_stream )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"output_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_mto1_isa_list ) )
	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		update_mto1_isa_display(
			update_mto1_isa,
			output_stream );

	} while ( list_next( update_mto1_isa_list ) );
}

void update_mto1_isa_display(
		UPDATE_MTO1_ISA *update_mto1_isa,
		FILE *output_stream )
{
	UPDATE_ONE2M *update_one2m;

	if ( !output_stream )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"output_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !update_mto1_isa
	||   !update_mto1_isa->update_changed_list
	||   !update_mto1_isa->update_changed_list->sql_statement_string )
	{
		char message[ 128 ];

		sprintf(message, "update_mto1_isa is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	appaserver_error_message_file(
		(char *)0 /* application_name */,
		(char *)0 /* login_name */,
		update_mto1_isa->
			update_changed_list->
			sql_statement_string );

	if ( update_mto1_isa->update_one2m_list
	&&   list_rewind( update_mto1_isa->update_one2m_list->list ) )
	do {
		update_one2m =
			list_get(
				update_mto1_isa->
					update_one2m_list->
					list );

		if ( !update_one2m->update_one2m_fetch )
		{
			char message[ 128 ];

			sprintf(message,
				"update_one2m->update_one2m_fetch is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_one2m_row_list_display(
			update_one2m->
				update_one2m_fetch->
				update_one2m_row_list,
			output_stream );

	} while ( list_next( update_mto1_isa->update_one2m_list->list ) );
}

void update_mto1_isa_list_update_changed_list_list(
		LIST *update_mto1_isa_list,
		LIST *update_changed_list_list /* in/out */ )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( list_rewind( update_mto1_isa_list ) )
	do {
		update_mto1_isa = list_get( update_mto1_isa_list );

		update_mto1_isa_update_changed_list_list(
			update_mto1_isa,
			update_changed_list_list /* in/out */ );

	} while ( list_next( update_mto1_isa_list ) );
}

void update_mto1_isa_update_changed_list_list(
		UPDATE_MTO1_ISA *update_mto1_isa,
		LIST *update_changed_list_list /* in/out */ )
{
	UPDATE_ONE2M *update_one2m;

	if ( !update_mto1_isa
	||   !update_mto1_isa->update_changed_list
	||   !update_mto1_isa->update_changed_list->sql_statement_string )
	{
		char message[ 128 ];

		sprintf(message, "update_mto1_isa is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_changed_list_list_getset(
		update_changed_list_list /* in/out */,
		update_mto1_isa->
			update_changed_list );

	if ( update_mto1_isa->update_one2m_list
	&&   list_rewind( update_mto1_isa->update_one2m_list->list ) )
	do {
		update_one2m =
			list_get(
				update_mto1_isa->
					update_one2m_list->
					list );

		if ( !update_one2m->update_one2m_fetch )
		{
			char message[ 128 ];

			sprintf(message,
				"update_one2m->update_one2m_fetch is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_one2m_row_list_update_changed_list_list(
			update_one2m->
				update_one2m_fetch->
				update_one2m_row_list,
			update_changed_list_list /* in/out */ );

	} while ( list_next( update_mto1_isa->update_one2m_list->list ) );
}

void update_one2m_row_list_display(
		LIST *update_one2m_row_list,
		FILE *output_stream )
{
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( list_rewind( update_one2m_row_list ) )
	do {
		update_one2m_row = list_get( update_one2m_row_list );

		if ( !update_one2m_row->update_changed_list )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"update_one2m_row->update_changed_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		fprintf(output_stream,
			"%s\n",
			update_one2m_row->
				update_changed_list->
				sql_statement_string );

		if ( update_one2m_row->update_one2m_list )
		{
			update_one2m_list_display(
				update_one2m_row->
					update_one2m_list->
					list
					/* update_one2m_row_list */,
				output_stream );
		}

	} while ( list_next( update_one2m_row_list ) );
}

void update_one2m_row_list_update_changed_list_list(
		LIST *update_one2m_row_list,
		LIST *update_changed_list_list /* in/out */ )
{
	UPDATE_ONE2M_ROW *update_one2m_row;

	if ( list_rewind( update_one2m_row_list ) )
	do {
		update_one2m_row = list_get( update_one2m_row_list );

		if ( !update_one2m_row->update_changed_list )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"update_one2m_row->update_changed_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_changed_list_list_getset(
			update_changed_list_list /* in/out */,
			update_one2m_row->
				update_changed_list );

		if ( update_one2m_row->update_one2m_list )
		{
			update_one2m_list_update_changed_list_list(
				update_one2m_row->
					update_one2m_list->
					list /* update_one2m_row_list */,
				update_changed_list_list /* in/out */ );
		}

	} while ( list_next( update_one2m_row_list ) );
}

void update_one2m_list_display(
		LIST *update_one2m_list,
		FILE *output_stream )
{
	UPDATE_ONE2M *update_one2m;

	if ( !output_stream )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"output_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_one2m_list ) )
	do {
		update_one2m = list_get( update_one2m_list );

		if ( !update_one2m->update_one2m_fetch
		||   !update_one2m->update_one2m_fetch->update_one2m_row_list )
		{
			char message[ 128 ];

			sprintf(message,
		"update_one2m->update_one2m_fetch is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_one2m_row_list_display(
			update_one2m->
				update_one2m_fetch->
				update_one2m_row_list,
			output_stream );

	} while ( list_next( update_one2m_list ) );
}

void update_one2m_list_update_changed_list_list(
		LIST *update_one2m_list,
		LIST *update_changed_list_list /* in/out */ )
{
	UPDATE_ONE2M *update_one2m;

	if ( list_rewind( update_one2m_list ) )
	do {
		update_one2m = list_get( update_one2m_list );

		if ( !update_one2m->update_one2m_fetch
		||   !update_one2m->update_one2m_fetch->update_one2m_row_list )
		{
			char message[ 128 ];

			sprintf(message,
		"update_one2m->update_one2m_fetch is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_one2m_row_list_update_changed_list_list(
			update_one2m->
				update_one2m_fetch->
				update_one2m_row_list,
			update_changed_list_list /* in/out */ );

	} while ( list_next( update_one2m_list ) );
}

void update_row_display(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list,
		FILE *output_stream )
{
	if ( update_root )
	{
		if ( !update_root->update_changed_list
		||   !update_root->
			update_changed_list->
			sql_statement_string )
		{
			char message[ 128 ];

			sprintf(message,
		"update_roow->update_changed_list is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		fprintf(output_stream,
			"%s\n",
			update_root->
				update_changed_list->
				sql_statement_string );
	}

	if ( update_one2m_list )
	{
		if ( !update_one2m_list->list )
		{
			char message[ 128 ];

			sprintf(message,
				"update_one2m_list->list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_one2m_list_display(
			update_one2m_list->list
				/* update_one2m_list */,
			output_stream );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		update_mto1_isa_list_display(
			update_mto1_isa_list,
			output_stream );
	}
}

void update_row_list_update_changed_list_list_set(
		LIST *update_row_list /* in/out */ )
{
	UPDATE_ROW *update_row;
	LIST *update_changed_list_list;

	if ( list_rewind( update_row_list ) )
	do {
		update_row = list_get( update_row_list );

		update_changed_list_list =
			update_row_update_changed_list_list(
				update_row->update_root,
				update_row->update_one2m_list,
				update_row->update_mto1_isa_list );

		update_row->update_changed_list_list =
			update_changed_list_list;

	} while ( list_next( update_row_list ) );
}

LIST *update_row_update_changed_list_list(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list )
{
	LIST *update_changed_list_list = list_new();

	if ( update_root )
	{
		if ( !update_root->update_changed_list
		||   !update_root->
			update_changed_list->
			sql_statement_string )
		{
			char message[ 128 ];

			sprintf(message,
		"update_roow->update_changed_list is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			update_changed_list_list,
			update_root->update_changed_list );
	}

	if ( update_one2m_list )
	{
		if ( !update_one2m_list->list )
		{
			char message[ 128 ];

			sprintf(message,
				"update_one2m_list->list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_one2m_list_update_changed_list_list(
			update_one2m_list->list
				/* update_one2m_list */,
			update_changed_list_list /* in/out */ );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		update_mto1_isa_list_update_changed_list_list(
			update_mto1_isa_list,
			update_changed_list_list /* in/out */ );
	}

	return update_changed_list_list;
}

void update_row_list_display(
		UPDATE_ROW_LIST *update_row_list,
		FILE *output_stream,
		boolean update_root_boolean )
{
	UPDATE_ROW *update_row;

	if ( !output_stream )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"output_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( !update_row_list )
	{
		char message[ 128 ];

		sprintf(message, "update_row_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_row_list->list ) )
	do {
		update_row = list_get( update_row_list->list );

		if ( !update_root_boolean ) update_row->update_root = NULL;

		update_row_display(
			update_row->update_root,
			update_row->update_one2m_list,
			update_row->update_mto1_isa_list,
			output_stream );

	} while ( list_next( update_row_list->list ) );
}

void update_statement_execute(
		const char *sql_executable,
		char *application_name,
		char *update_statement )
{
	char *error_filename;
	char *system_string;
	FILE *output_pipe;

	if ( !application_name
	||   !update_statement )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	error_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filename(
			application_name );

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		update_system_string(
			sql_executable,
			error_filename );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	fprintf(
		output_pipe,
		"%s\n",
		update_statement );

	pclose( output_pipe );

	free( error_filename );
}

UPDATE_CHANGED_LIST *update_one2m_row_update_changed_list(
		char *many_folder_name,
		LIST *many_folder_attribute_list,
		LIST *update_attribute_list )
{
	LIST *primary_key_list;
	LIST *attribute_name_list;
	UPDATE_CHANGED_LIST *update_changed_list;

	if ( !many_folder_name
	||   !list_length( many_folder_attribute_list )
	||   !list_length( update_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_key_list =
		folder_attribute_primary_key_list(
			many_folder_name,
			many_folder_attribute_list );

	attribute_name_list =
		folder_attribute_name_list(
			many_folder_name,
			many_folder_attribute_list );

#ifdef UPDATE_DEBUG_MODE
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: calling update_changed_list_new() with many_folder_name=%s, primary_key_list=[%s], update_attribute_list=[%s]\n\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	many_folder_name,
	list_display( primary_key_list ),
	update_attribute_list_display( update_attribute_list ) );
msg( (char *)0, message );
}
#endif

	update_changed_list =
		update_changed_list_new(
			many_folder_name,
			primary_key_list,
			attribute_name_list,
			update_attribute_list,
			(char *)0 /* security_entity_where */ );

	if ( !update_changed_list )
	{
		char message[ 4096 ];

		snprintf(
			message,
			sizeof ( message ),
"update_change_list_new(%s,[primary_key=%s],update_attribute_list=[%s]) returned empty.",
			many_folder_name,
			list_display( primary_key_list ),
			update_attribute_list_display(
				update_attribute_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return update_changed_list;
}

char *update_one2m_select_string( LIST *select_name_list )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	list_display_delimited(
		select_name_list,
		',' );
}

LIST *update_one2m_select_name_list(
		LIST *many_primary_key_list,
		LIST *relation_foreign_key_list )
{
	LIST *copy = list_copy( many_primary_key_list );

	return
	list_set_list(
		copy,
		relation_foreign_key_list );
}

char *update_one2m_row_post_datum( UPDATE_CHANGED *update_changed_seek )
{
	if ( update_changed_seek )
	{
		return
		update_changed_seek->
			update_attribute->
			post_datum;
	}
	else
	{
		return NULL;
	}
}

void update_changed_list_list_getset(
		LIST *update_changed_list_list /* in/out */,
		UPDATE_CHANGED_LIST *parameter_update_changed_list )
{
	UPDATE_CHANGED_LIST *update_changed_list;
	boolean list_boolean;

	if ( !update_changed_list_list
	||   !parameter_update_changed_list )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_changed_list_list ) )
	do {
		update_changed_list =
			list_get( update_changed_list_list );

		list_boolean =
			update_changed_list_boolean(
				update_changed_list,
				parameter_update_changed_list );

		if ( list_boolean ) return;

	} while ( list_next( update_changed_list_list ) );

	list_set(
		update_changed_list_list,
		parameter_update_changed_list );
}

boolean update_changed_list_boolean(
		UPDATE_CHANGED_LIST *update_changed_list,
		UPDATE_CHANGED_LIST *parameter_update_changed_list )
{
	int cmp;
	boolean list_boolean = 0;

	cmp = strcmp(
		update_changed_list->
			sql_statement_string,
		parameter_update_changed_list->
			sql_statement_string );

	if ( cmp == 0 ) list_boolean = 1;

	return list_boolean;
}

char *update_changed_list_list_execute(
		const char *sql_executable,
		char *application_name,
		LIST *update_changed_list_list,
		char *appaserver_error_filename,
		boolean update_root_boolean )
{
	char *update_error_string;
	char *system_string;
	FILE *output_pipe;
	UPDATE_CHANGED_LIST *update_changed_list;

	if ( !application_name
	||   !update_changed_list_list
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		update_system_string(
			sql_executable,
			appaserver_error_filename );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	if ( list_rewind( update_changed_list_list ) )
	do {
		update_changed_list =
			list_get(
				update_changed_list_list );

		if ( list_first_boolean( update_changed_list_list ) )
		{
		     if ( update_root_boolean )
		     {
			update_error_string =
				update_root_execute(
					SQL_EXECUTABLE,
					update_changed_list->
						sql_statement_string,
					appaserver_error_filename );

			if ( update_error_string )
			{
				appaserver_error_message_file(
					application_name,
					(char *)0 /* login_name */,
					update_error_string /* message */ );

				pclose( output_pipe );
				return update_error_string;
			}
		     }
		}
		else
		{
			fprintf(
				output_pipe,
				"%s\n",
				update_changed_list->sql_statement_string );
		}

	} while ( list_next( update_changed_list_list ) );

	pclose( output_pipe );

	return NULL;
}

int update_row_list_cell_count( LIST *update_row_list )
{
	int cell_count = 0;
	UPDATE_ROW *update_row;

	if ( list_rewind( update_row_list ) )
	do {
		update_row = list_get( update_row_list );

		cell_count +=
			update_row_cell_count(
				update_row );

	} while ( list_next( update_row_list ) );

	return cell_count;
}

int update_row_cell_count( UPDATE_ROW *update_row )
{
	int cell_count = 0;
	UPDATE_CHANGED_LIST *update_changed_list;

	if ( !update_row )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"update_row is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( update_row->update_changed_list_list ) )
	do {
		update_changed_list =
			list_get(
				update_row->
					update_changed_list_list );

		cell_count +=
			list_length(
				update_changed_list->
					list );

	} while ( list_next( update_row->update_changed_list_list ) );

	return cell_count;
}

