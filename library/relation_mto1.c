/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_mto1.c	 			*/
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
#include "relation_one2m.h"
#include "relation_mto1.h"

LIST *relation_mto1_list(
		char *role_name,
		char *many_folder_name,
		LIST *many_folder_primary_key_list )
{
	LIST *seek_mto1_list;
	LIST *mto1_list = {0};
	RELATION *relation;
	RELATION_MTO1 *relation_mto1;

	if ( !many_folder_name
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

	seek_mto1_list =
		relation_seek_mto1_list(
			many_folder_name );

	if ( list_rewind( seek_mto1_list ) )
	do {
		relation = list_get( seek_mto1_list );

		if ( relation->relation_type_isa ) continue;

		if ( !mto1_list ) mto1_list = list_new();

		relation_mto1 =
			relation_mto1_new(
				role_name,
				many_folder_name,
				many_folder_primary_key_list,
				relation );

		list_set( mto1_list, relation_mto1 );

	} while ( list_next( seek_mto1_list ) );

	if ( !list_length( mto1_list ) )
	{
		list_free( mto1_list );
		mto1_list = NULL;
	}

	return mto1_list;
}

LIST *relation_mto1_recursive_list(
		LIST *relation_mto1_list,
		char *role_name,
		char *many_folder_name,
		LIST *many_folder_primary_key_list,
		boolean single_foreign_key_only )
{
	LIST *seek_mto1_list;
	RELATION *relation;
	RELATION_MTO1 *relation_mto1;

	if ( !many_folder_name
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

	seek_mto1_list =
		relation_seek_mto1_list(
			many_folder_name );

	if ( list_rewind( seek_mto1_list ) )
	do {
		relation = list_get( seek_mto1_list );

		if ( relation->relation_type_isa ) continue;

		relation_mto1 =
			relation_mto1_new(
				role_name,
				many_folder_name,
				many_folder_primary_key_list,
				relation );

		if ( !relation_mto1_list ) relation_mto1_list = list_new();

		if ( single_foreign_key_only )
		{
			if ( list_length(
				relation_mto1->
					relation_foreign_key_list ) == 1 )
			{
				list_set( relation_mto1_list, relation_mto1 );
			}
		}
		else
		{
			list_set( relation_mto1_list, relation_mto1 );
		}

		relation_mto1_list =
			relation_mto1_recursive_list(
				relation_mto1_list,
				role_name,
				relation->one_folder_name
					/* many_folder_name */,
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list
					/* many_folder_primary_key_list */,
				single_foreign_key_only );

	} while ( list_next( seek_mto1_list ) );

	return relation_mto1_list;
}

LIST *relation_mto1_isa_list(
		LIST *mto1_list,
		char *role_name,
		char *many_folder_name,
		LIST *many_folder_primary_key_list,
		boolean fetch_relation_one2m_list,
		boolean fetch_relation_mto1_list )
{
	LIST *seek_mto1_list;
	RELATION *relation;
	RELATION_MTO1 *relation_mto1;

	if ( !many_folder_name
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

	seek_mto1_list =
		relation_seek_mto1_list(
			many_folder_name );

	if ( list_rewind( seek_mto1_list ) )
	do {
		relation = list_get( seek_mto1_list );

		if ( !relation->relation_type_isa ) continue;

		relation_mto1 =
			relation_mto1_new(
				role_name,
				many_folder_name,
				many_folder_primary_key_list,
				relation );

		if ( !mto1_list ) mto1_list = list_new();

		list_set( mto1_list, relation_mto1 );

		if ( fetch_relation_one2m_list )
		{
			if ( !relation_mto1->one_folder
			||   !list_length(
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list ) )
			{
				char message[ 128 ];

				sprintf(message,
			"relation_mto1->one_folder is empty or incomplete." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			relation_mto1->relation_one2m_list =
				relation_one2m_list(
					role_name,
					relation_mto1->one_folder_name,
					relation_mto1->
					    one_folder->
					    folder_attribute_primary_key_list
					    /* one_folder_primary_key_list */,
					0 /* not include_isa_boolean */ );
		}

		if ( fetch_relation_mto1_list )
		{
			relation_mto1->relation_mto1_list =
				relation_mto1_list(
				    role_name,
				    relation_mto1->one_folder_name
					/* many_folder_name */,
				    relation_mto1->
					one_folder->
					folder_attribute_primary_key_list
					   /* many_folder_primary_key_list */ );
		}

		mto1_list =
			relation_mto1_isa_list(
				mto1_list,
				role_name,
				relation->one_folder_name
					/* many_folder_name */,
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list
					/* many_folder_primary_key_list */,
				fetch_relation_one2m_list,
				fetch_relation_mto1_list );

	} while ( list_next( seek_mto1_list ) );

	return mto1_list;
}

LIST *relation_mto1_drillthru_list(
		LIST *relation_mto1_list,
		char *role_name,
		char *base_folder_name,
		LIST *base_folder_primary_key_list,
		LIST *fulfilled_folder_name_list )
{
	LIST *seek_mto1_list;
	RELATION *relation;
	RELATION_MTO1 *relation_mto1;

	if ( !base_folder_name
	||   !list_length( base_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	seek_mto1_list =
		relation_seek_mto1_list(
			base_folder_name
				/* many_folder_name */ );

	if ( list_rewind( seek_mto1_list ) )
	do {
		relation = list_get( seek_mto1_list );

		if ( list_exists_string(
			relation->one_folder_name,
			fulfilled_folder_name_list ) )
		{
			continue;
		}

		if ( relation->omit_drillthru ) continue;

		relation_mto1 =
			relation_mto1_new(
				role_name,
				base_folder_name
					/* many_folder_name */,
				base_folder_primary_key_list
					/* many_folder_primary_key_list */,
				relation );

		if ( !relation_mto1->one_folder->drillthru ) continue;

		if ( !relation_mto1_list ) relation_mto1_list = list_new();

		list_set( relation_mto1_list, relation_mto1 );

		relation_mto1_list =
			relation_mto1_drillthru_list(
				relation_mto1_list,
				role_name,
				relation->one_folder_name
					/* base_folder_name */,
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list
					/* base_folder_primary_key_list */,
				fulfilled_folder_name_list );

	} while ( list_next( seek_mto1_list ) );

	return relation_mto1_list;
}

RELATION_MTO1 *relation_mto1_consumes(
		char *many_attribute_name,
		LIST *relation_mto1_list )
{
	RELATION_MTO1 *relation_mto1;

	if ( !list_length( relation_mto1_list ) )
	{
		return NULL;
	}

	relation_mto1 =
		relation_mto1_attribute_consumes(
			many_attribute_name,
			relation_mto1_list );

	if ( relation_mto1 ) return relation_mto1;

	return NULL;
}

RELATION_MTO1 *relation_mto1_isa_consumes(
		char *many_attribute_name,
		LIST *relation_mto1_isa_list )
{
	RELATION_MTO1 *relation_mto1;
	RELATION_MTO1 *relation_mto1_to_one;

	if ( list_rewind( relation_mto1_isa_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_isa_list );

		relation_mto1_to_one =
			relation_mto1_attribute_consumes(
				many_attribute_name,
				relation_mto1->relation_mto1_list );

		if ( relation_mto1_to_one )
		{
			return relation_mto1_to_one;
		}

	} while ( list_next( relation_mto1_isa_list ) );

	return NULL;
}

RELATION_MTO1 *relation_mto1_attribute_consumes(
		char *many_attribute_name,
		LIST *relation_mto1_list )
{
	RELATION_MTO1 *relation_mto1;
	boolean string_exists;

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 =
			list_get(
				relation_mto1_list );

		if ( !relation_mto1->relation_foreign_key_list )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_foreign_key_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		string_exists =
			list_string_exists(
				many_attribute_name,
				relation_mto1->relation_foreign_key_list );

		if ( string_exists )
		{
			return relation_mto1;
		}

		if ( list_length( relation_mto1->relation_mto1_list ) )
		{
			relation_mto1 =
				relation_mto1_attribute_consumes(
					many_attribute_name,
					relation_mto1->relation_mto1_list );

			if ( relation_mto1 ) return relation_mto1;
		}

	} while( list_next( relation_mto1_list ) );

	return NULL;
}

LIST *relation_mto1_folder_name_list( LIST *relation_mto1_list )
{
	RELATION_MTO1 *relation_mto1;
	LIST *folder_name_list = list_new();

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 =
			list_get(
				relation_mto1_list );

		list_set(
			folder_name_list,
			relation_mto1->one_folder_name );

	} while ( list_next( relation_mto1_list ) );

	if ( !list_length( folder_name_list ) )
	{
		list_free( folder_name_list );
		folder_name_list = NULL;
	}

	return folder_name_list;
}

RELATION_MTO1 *relation_mto1_automatic_preselection(
		LIST *relation_mto1_list )
{
	RELATION_MTO1 *relation_mto1;

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 =
			list_get(
				relation_mto1_list );

		if ( relation_mto1->
			relation->
			automatic_preselection )
		{
			return relation_mto1;
		}

	} while ( list_next( relation_mto1_list ) );

	return NULL;
}

boolean relation_mto1_exists_multi_select(
		LIST *relation_mto1_list )
{
	RELATION_MTO1 *relation_mto1;

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 =
			list_get(
				relation_mto1_list );

		if ( relation_mto1->
			relation->
			drop_down_multi_select )
		{
			return 1;
		}

	} while ( list_next( relation_mto1_list ) );

	return 0;
}

RELATION_MTO1 *relation_mto1_ajax_fill_seek(
		LIST *relation_mto1_to_one_list )
{
	RELATION_MTO1 *relation_mto1;

	if ( list_rewind( relation_mto1_to_one_list ) )
	do {
		relation_mto1 =
			list_get(
				relation_mto1_to_one_list );

		if ( relation_mto1->relation->ajax_fill_drop_down )
		{
			return relation_mto1;
		}

	} while ( list_next( relation_mto1_to_one_list ) );

	return NULL;
}

RELATION_MTO1 *relation_mto1_seek(
		char *one_folder_name,
		LIST *relation_mto1_list )
{
	RELATION_MTO1 *relation_mto1;

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

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_list );

		if ( strcmp(	relation_mto1->one_folder_name,
				one_folder_name ) == 0 )
		{
			return relation_mto1;
		}

	} while( list_next( relation_mto1_list ) );

	return NULL;
}

LIST *relation_mto1_foreign_key_less_equal_list(
		LIST *relation_mto1_list,
		int max_foreign_key_list_length )
{
	RELATION_MTO1 *relation_mto1;
	LIST *list = {0};

	if ( !max_foreign_key_list_length
	||   !list_rewind( relation_mto1_list ) )
	{
		return NULL;
	}

	do {
		relation_mto1 = list_get( relation_mto1_list );

		if ( list_length(
			relation_mto1->
				relation_foreign_key_list ) <=
			max_foreign_key_list_length )
		{
			if ( !list ) list = list_new();

			list_set( list, relation_mto1 );
		}

	} while ( list_next( relation_mto1_list ) );

	return list;
}

RELATION_MTO1 *relation_mto1_new(
		char *role_name,
		char *many_folder_name,
		LIST *many_folder_primary_key_list,
		RELATION *relation )
{
	RELATION_MTO1 *relation_mto1;

	if ( !many_folder_name
	||   !list_length( many_folder_primary_key_list )
	||   !relation
	||   !relation->one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_mto1 = relation_mto1_calloc();

	relation_mto1->many_folder_name = many_folder_name;
	relation_mto1->one_folder_name = relation->one_folder_name;

	relation_mto1->many_folder_primary_key_list =
		many_folder_primary_key_list;

	relation_mto1->relation = relation;

	relation_mto1->one_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			relation->one_folder_name,
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	if ( !list_length(
		relation_mto1->
			one_folder->
			folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
	"folder_fetch(%s) returned an empty folder_attribute_primary_key_list.",
			relation->one_folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	relation_mto1->foreign_attribute_list =
		foreign_attribute_list(
			many_folder_name,
			relation->one_folder_name,
			relation->related_attribute_name );

	if ( list_length( relation_mto1->foreign_attribute_list ) )
	{
		relation_mto1->foreign_attribute_name_list =
			foreign_attribute_name_list(
				relation_mto1->foreign_attribute_list );
	}

	relation_mto1->relation_foreign_key_list =
		relation_foreign_key_list(
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list,
			relation_mto1->relation->related_attribute_name,
			relation_mto1->foreign_attribute_name_list );

	relation_mto1->relation_translate_list =
		relation_translate_list(
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list,
			relation_mto1->relation_foreign_key_list );

	relation_mto1->relation_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_name(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			relation_mto1->relation_foreign_key_list );

	relation_mto1->relation_foreign_key_exists_primary =
		relation_foreign_key_exists_primary(
			many_folder_primary_key_list,
			relation_mto1->relation_foreign_key_list );

	relation_mto1->relation_prompt =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_prompt(
			relation->one_folder_name,
			relation->related_attribute_name,
			(LIST *)0 /* foreign_attribute_name_list */,
			relation_mto1->relation_foreign_key_exists_primary );

	return relation_mto1;
}

RELATION_MTO1 *relation_mto1_calloc( void )
{
	RELATION_MTO1 *relation_mto1;

	if ( ! ( relation_mto1 = calloc( 1, sizeof ( RELATION_MTO1 ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return relation_mto1;
}

char *relation_mto1_where_string(
		const char *folder_primary_key,
		char *many_folder_name )
{
	static char where_string[ 128 ];

	if ( !many_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "many_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where_string,
		"%s = '%s'",
		folder_primary_key,
		many_folder_name );

	return where_string;
}

char *relation_mto1_list_display( LIST *relation_mto1_list )
{
	char display[ 2048 ];
	char *ptr = display;
	RELATION_MTO1 *relation_mto1;

	if ( !list_rewind( relation_mto1_list ) ) return "NULL";

	do {
		relation_mto1 = list_get( relation_mto1_list );

		if ( !relation_mto1->relation
		||   !relation_mto1->one_folder )
		{
			char message[ 128 ];

			sprintf(message, "relation_mto1 is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"one_folder_name=%s, "
			"related_attribute_name=%s, "
			"ajax_fill_drop_down=%d, "
			"folder_attribute_name_list=%s\n, "
			"relation_foreign_key_list=%s\n",
			relation_mto1->one_folder_name,
			relation_mto1->relation->related_attribute_name,
			relation_mto1->relation->ajax_fill_drop_down,
			list_display(
				relation_mto1->
					one_folder->
					folder_attribute_name_list ),
			list_display(
				relation_mto1->
					relation_foreign_key_list ) );

		if ( list_length( relation_mto1->relation_one2m_list ) )
		{
			ptr += sprintf(
				ptr,
				"RELATION_ONE2M: %s\n",
				relation_one2m_list_display(
					relation_mto1->relation_one2m_list ) );
		}
		else
		{
			ptr += sprintf( ptr, "no RELATION_ONE2M list.\n" );
		}

		if ( list_length( relation_mto1->relation_mto1_list ) )
		{
			ptr += sprintf(
				ptr,
				"RELATION_MTO1: %s\n",
				relation_mto1_list_display(
					relation_mto1->relation_mto1_list ) );
		}
		else
		{
			ptr += sprintf( ptr, "no RELATION_MTO1 list.\n" );
		}

		ptr += sprintf( ptr, "\n" );

	} while ( list_next( relation_mto1_list )  );

	return strdup( display );
}

LIST *relation_mto1_to_one_fetch_list(
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list )
{
	if ( !role_name
	||   !one_folder_name
	||   !list_length(
		one_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	relation_mto1_list(
		role_name,
		one_folder_name
			/* many_folder_name */,
		one_folder_primary_key_list
			/* many_folder_primary_key_list */ );
}

LIST *relation_mto1_without_omit_drillthru_list( LIST *relation_mto1_list )
{
	LIST *list = list_new();
	RELATION_MTO1 *relation_mto1;

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_list );

		if ( !relation_mto1->relation->omit_drillthru )
		{
			list_set( list, relation_mto1 );
		}

	} while ( list_next( relation_mto1_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *relation_mto1_to_one_list(
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list )
{
	LIST *mto1_list = list_new();
	RELATION_MTO1 *relation_mto1;

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_list );

		list_set( mto1_list, relation_mto1 );

	} while ( list_next( relation_mto1_list ) );


	if ( list_rewind( relation_mto1_isa_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_isa_list );

		list_set_list(
			mto1_list,
			relation_mto1->relation_mto1_list );

	} while ( list_next( relation_mto1_isa_list ) );

	if ( !list_length( mto1_list ) )
	{
		list_free( mto1_list );
		mto1_list = NULL;
	}

	return mto1_list;
}
