/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_one2m.c	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "foreign_attribute.h"
#include "folder_attribute.h"
#include "appaserver.h"
#include "relation_mto1.h"
#include "relation_one2m.h"

LIST *relation_one2m_list(
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list,
		boolean include_isa_boolean )
{
	LIST *one2m_list = {0};
	LIST *seek_one2m_list;
	RELATION *relation;
	RELATION_ONE2M *relation_one2m;

	if ( !one_folder_name
	||   !list_length( one_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	seek_one2m_list =
		relation_seek_one2m_list(
			one_folder_name );

	if ( list_rewind( seek_one2m_list ) )
	do {
		relation = list_get( seek_one2m_list );

		if ( !include_isa_boolean
		&&   relation->relation_type_isa )
		{
			continue;
		}

		relation_one2m =
			relation_one2m_new(
				role_name,
				one_folder_name,
				one_folder_primary_key_list,
				relation );

		if ( !one2m_list ) one2m_list = list_new();

		list_set( one2m_list, relation_one2m );

	} while ( list_next( seek_one2m_list ) );

	return one2m_list;
}

RELATION_ONE2M *relation_one2m_calloc( void )
{
	RELATION_ONE2M *relation_one2m;

	if ( ! ( relation_one2m = calloc( 1, sizeof ( RELATION_ONE2M ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation_one2m;
}

RELATION_ONE2M *relation_one2m_new(
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list,
		RELATION *relation )
{
	RELATION_ONE2M *relation_one2m;

	if ( !one_folder_name
	||   !list_length( one_folder_primary_key_list )
	||   !relation )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_one2m = relation_one2m_calloc();

	relation_one2m->one_folder_name = one_folder_name;
	relation_one2m->many_folder_name = relation->many_folder_name;

	relation_one2m->one_folder_primary_key_list =
		one_folder_primary_key_list;

	relation_one2m->relation = relation;

	relation_one2m->many_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			relation->many_folder_name,
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	relation_one2m->foreign_attribute_list =
		foreign_attribute_list(
			relation->many_folder_name,
			one_folder_name,
			relation->related_attribute_name );

	if ( list_length( relation_one2m->foreign_attribute_list ) )
	{
		relation_one2m->foreign_attribute_name_list =
			foreign_attribute_name_list(
				relation_one2m->
					foreign_attribute_list );
	}

	relation_one2m->relation_foreign_key_list =
		relation_foreign_key_list(
			one_folder_primary_key_list,
			relation->related_attribute_name,
			relation_one2m->foreign_attribute_name_list );

	relation_one2m->relation_translate_list =
		relation_translate_list(
			one_folder_primary_key_list,
			relation_one2m->relation_foreign_key_list );

	relation_one2m->relation_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_name(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			relation_one2m->
				relation_foreign_key_list );

	relation_one2m->relation_foreign_key_exists_primary =
		relation_foreign_key_exists_primary(
			relation_one2m->
				many_folder->
				folder_attribute_primary_key_list,
			relation_one2m->relation_foreign_key_list );

	relation_one2m->relation_prompt =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_prompt(
			relation->many_folder_name,
			relation->related_attribute_name,
			relation_one2m->foreign_attribute_name_list,
			relation_one2m->relation_foreign_key_exists_primary );

	relation_one2m->primary_key_subset =
		relation_one2m_primary_key_subset(
			relation_one2m->relation_foreign_key_list,
			relation_one2m->
				many_folder->
				folder_attribute_primary_key_list );

	relation_one2m->foreign_key_none_primary =
		relation_one2m_foreign_key_none_primary(
			relation_one2m->relation_foreign_key_list,
			relation_one2m->
				many_folder->
				folder_attribute_primary_key_list );

	return relation_one2m;
}

LIST *relation_one2m_pair_list(
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list )
{
	LIST *seek_one2m_list;
	RELATION *relation;
	RELATION_ONE2M *relation_one2m;
	LIST *one2m_list = {0};

	if ( !one_folder_name
	||   !list_length( one_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	seek_one2m_list =
		relation_seek_one2m_list(
			one_folder_name );

	if ( list_rewind( seek_one2m_list ) )
	do {
		relation = list_get( seek_one2m_list );

		if ( !relation->pair_one2m_order ) continue;

		relation_one2m =
			relation_one2m_new(
				role_name,
				one_folder_name,
				one_folder_primary_key_list,
				relation );

		if ( !one2m_list ) one2m_list = list_new();

		list_set( one2m_list, relation_one2m );

	} while ( list_next( seek_one2m_list ) );

	return one2m_list;
}

LIST *relation_one2m_recursive_list(
		LIST *one2m_list,
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list )
{
	LIST *seek_one2m_list;
	RELATION *relation;
	RELATION_ONE2M *relation_one2m;

	if ( !one_folder_name
	||   !list_length( one_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	seek_one2m_list =
		relation_seek_one2m_list(
			one_folder_name );

	if ( list_rewind( seek_one2m_list ) )
	do {
		relation = list_get( seek_one2m_list );

		relation_one2m =
			relation_one2m_new(
				role_name,
				one_folder_name,
				one_folder_primary_key_list,
				relation );

		if ( !one2m_list ) one2m_list = list_new();

		list_set( one2m_list, relation_one2m );

		if ( relation_one2m->primary_key_subset )
		{
			one2m_list =
				relation_one2m_recursive_list(
					one2m_list,
					role_name,
					relation->many_folder_name
					/* one_folder_name */,
					relation_one2m->
					   many_folder->
					   folder_attribute_primary_key_list
					   /* one_folder_primary_key_list */ );
		}

	} while ( list_next( seek_one2m_list ) );

	return one2m_list;
}

boolean relation_one2m_primary_key_subset(
		LIST *relation_foreign_key_list,
		LIST *many_folder_primary_key_list )
{
	return
	list_is_subset_of(
		relation_foreign_key_list /* subset */,
		many_folder_primary_key_list /* set */ );
}

boolean relation_one2m_foreign_key_none_primary(
		LIST *relation_foreign_key_list,
		LIST *many_folder_primary_key_list )
{
	char *foreign_key;

	if ( !list_rewind( relation_foreign_key_list )
	||   !list_length( many_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		foreign_key = list_get( relation_foreign_key_list );

		if ( list_string_exists(
			foreign_key,
			many_folder_primary_key_list ) )
		{
			return 0;
		}

	} while ( list_next( relation_foreign_key_list ) );

	return 1;
}

LIST *relation_one2m_folder_name_list(
		LIST *relation_one2m_list )
{
	RELATION_ONE2M *relation_one2m;
	LIST *folder_name_list = list_new();

	if ( list_rewind( relation_one2m_list ) )
	do {
		relation_one2m =
			list_get(
				relation_one2m_list );

		list_set(
			folder_name_list,
			relation_one2m->many_folder_name );

	} while ( list_next( relation_one2m_list ) );

	if ( !list_length( folder_name_list ) )
	{
		list_free( folder_name_list );
		folder_name_list = NULL;
	}

	return folder_name_list;
}

LIST *relation_one2m_join_list(
		/* ----------------------------------- */
		/* Set to cache all folders for a role */
		/* ----------------------------------- */
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list,
		DICTIONARY *no_display_dictionary )
{
	RELATION *relation;
	RELATION_ONE2M *relation_one2m;
	LIST *one2m_list = {0};
	LIST *seek_one2m_list;

	seek_one2m_list =
		relation_seek_one2m_list(
			one_folder_name );

	if ( list_rewind( seek_one2m_list ) )
	do {
		relation = list_get( seek_one2m_list );

		if ( !relation->join_one2m_each_row ) continue;

		if ( dictionary_key_exists(
				no_display_dictionary,
				relation->many_folder_name ) )
		{
			continue;
		}

		relation_one2m =
			relation_one2m_new(
				role_name,
				one_folder_name,
				one_folder_primary_key_list,
				relation );

		if ( !one2m_list ) one2m_list = list_new();

		list_set( one2m_list, relation_one2m );

	} while ( list_next( seek_one2m_list ) );

	return one2m_list;
}

char *relation_one2m_where_string(
		const char *relation_foreign_key
			/* probably related_table */,
		char *one_folder_name )
{
	static char where_string[ 128 ];

	if ( !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where_string,
		sizeof ( where_string ),
		"%s = '%s'",
		relation_foreign_key,
		one_folder_name );

	return where_string;
}

char *relation_one2m_list_display( LIST *relation_one2m_list )
{
	char display[ 2048 ];
	char *ptr = display;
	RELATION_ONE2M *relation_one2m;

	if ( !list_rewind( relation_one2m_list ) ) return "NULL";

	do {
		relation_one2m = list_get( relation_one2m_list );

		if ( ptr != display ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"many_folder_name=%s, "
			"relation_foreign_key_list=%s\n\n",
			relation_one2m->many_folder_name,
			list_display(
				relation_one2m->
					relation_foreign_key_list ) );

	} while ( list_next( relation_one2m_list ) );

	return strdup( display );
}

LIST *relation_one2m_omit_update_list(
		LIST *relation_mto1_recursive_list,
		LIST *relation_one2m_list )
{
	RELATION_MTO1 *relation_mto1;
	RELATION_ONE2M *relation_one2m;
	LIST *list = list_new();

	if ( list_rewind( relation_mto1_recursive_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_recursive_list );

		if ( list_rewind(
			relation_mto1->
				relation_one2m_list ) )
		do {
			relation_one2m =
				list_get(
					relation_mto1->
						relation_one2m_list );

			if ( relation_one2m->relation->omit_update )
			{
				list_set( list, relation_one2m );
			}

		} while ( list_next( 
				relation_mto1->
					relation_one2m_list ) );

	} while ( list_next( relation_mto1_recursive_list ) );

	if ( list_rewind( relation_one2m_list ) )
	do {
		relation_one2m = list_get( relation_one2m_list );

		if ( relation_one2m->relation->omit_update )
		{
			list_set( list, relation_one2m );
		}

	} while ( list_next( relation_one2m_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

