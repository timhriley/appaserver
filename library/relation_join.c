/* --------------------------------------------- */
/* $APPASERVER_HOME/library/relation_join.c	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "foreign_attribute.h"
#include "appaserver_library.h"
#include "relation_join.h"

RELATION_JOIN *relation_join_calloc( void )
{
	RELATION_JOIN *relation_join;

	if ( ! ( relation_join = calloc( 1, sizeof( RELATION_JOIN ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation_join;
}

RELATION_JOIN *relation_join_new(
			DICTIONARY *row_dictionary,
			char *application_name,
			LIST *relation_join_one2m_list,
			char *one_folder_name,
			LIST *one_primary_key_list )
{
	RELATION_JOIN *relation_join;
	RELATION *relation_join_one2m;

	if ( !list_rewind( relation_join_one2m_list ) )
		return (RELATION_JOIN *)0;

	relation_join = relation_join_calloc();

	do {
		relation_join_one2m = list_get( relation_join_one2m_list );

		if ( !relation_join_one2m->many_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: many folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		relation_join->relation_join_folder =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			relation_join_folder_new(
				row_dictionary,
				application_name,
				relation_join_one2m,
				one_folder_name,
				one_primary_key_list );

		if ( relation_join->relation_join_folder->delimited_string )
		{
			dictionary_set(
				row_dictionary,
				relation_join_one2m->many_folder->folder_name,
				relation_join->
					relation_join_folder->
					delimited_string );
		}

	} while ( list_next( relation_join_one2m_list ) );

	return relation_join;
}

RELATION_JOIN_FOLDER *relation_join_folder_calloc( void )
{
	RELATION_JOIN_FOLDER *relation_join_folder;

	if ( ! ( relation_join_folder =
			calloc( 1, sizeof( RELATION_JOIN_FOLDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation_join_folder;
}

RELATION_JOIN_FOLDER *relation_join_folder_new(
			DICTIONARY *row_dictionary,
			char *application_name,
			RELATION *relation_join_one2m,
			char *one_folder_name,
			LIST *one_primary_key_list )
{
	RELATION_JOIN_FOLDER *relation_join_folder =
		relation_join_folder_calloc();

	if ( !relation_join_one2m->many_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: many_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length(
		relation_join_one2m->
			many_folder->
			primary_key_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: many_folder->primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( one_primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	relation_join_folder->select_name_list =
		list_subtract(
			relation_join_one2m->many_folder->primary_key_list,
			one_primary_key_list );

	if ( !list_length( relation_join_folder->select_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: list_subtract() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	relation_join_folder->where =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		relation_join_folder_where_new(
			row_dictionary,
			relation_join_one2m,
			one_folder_name,
			one_primary_key_list );

	relation_join_folder->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_join_folder_system_string(
			application_name,
			list_delimited(
				relation_join_folder->
					select_name_list,
				',' ),
			relation_join_one2m->many_folder->folder_name,
			relation_join_folder->where->string,
			MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER );

	relation_join_folder->delimited_string =
		relation_join_folder_delimited_string(
			relation_join_folder->system_string );

	return relation_join_folder;
}

RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_calloc( void )
{
	RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where;

	if ( ! ( relation_join_folder_where =
			calloc( 1, sizeof( RELATION_JOIN_FOLDER_WHERE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation_join_folder_where;
}

char *relation_join_folder_system_string(
			char *application_name,
			char *select_name_list_string,
			char *folder_name,
			char *where,
			int multi_attribute_data_label_delimiter )
{
	char system_string[ 1024 ];

	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( system_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=select		|"
		 "joinlines.e '%c'				 ",
		 application_name,
		 select_name_list_string,
		 folder_name,
		 where,
		 multi_attribute_data_label_delimiter );

	return strdup( system_string );
}

char *relation_join_folder_delimited_string(
			char *system_string )
{
	return string_pipe_fetch( system_string );
}

RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_new(
			DICTIONARY *row_dictionary,
			RELATION *relation_join_one2m,
			char *one_folder_name,
			LIST *one_primary_key_list )
{
	RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where =
		relation_join_folder_where_calloc();

	relation_join_folder_where->dictionary_data_list =
		dictionary_data_list(
			one_primary_key_list,
			row_dictionary );

	if ( !list_length( relation_join_folder_where->dictionary_data_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: dictionary_data_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display( one_primary_key_list ) );
		exit( 1 );
	}

	relation_join_folder_where->foreign_attribute_list =
		foreign_attribute_list(
			relation_join_one2m->many_folder->folder_name,
			one_folder_name );

	relation_join_folder_where->foreign_attribute_name_list =
		foreign_attribute_name_list(
			relation_join_folder_where->foreign_attribute_list );

	relation_join_folder_where->relation_foreign_key_list =
		relation_foreign_key_list(
			one_primary_key_list,
			relation_join_one2m->related_attribute_name,
			relation_join_folder_where->
				foreign_attribute_name_list );

	if ( !list_length(
		relation_join_folder_where->
			relation_foreign_key_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: relation_foreign_key_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display( one_primary_key_list ) );
		exit( 1 );
	}

	if (	list_length( relation_join_folder_where->
				dictionary_data_list ) !=
		list_length( relation_join_folder_where->
				relation_foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length = %d != length = %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( relation_join_folder_where->
				dictionary_data_list ),
			list_length( relation_join_folder_where->
				relation_foreign_key_list ) );
		exit( 1 );
	}

	relation_join_folder_where->string =
		relation_join_folder_where_string(
			relation_join_folder_where->relation_foreign_key_list,
			relation_join_folder_where->dictionary_data_list );

	return relation_join_folder_where;
}

char *relation_join_folder_where_string(
			LIST *relation_foreign_key_list,
			LIST *dictionary_data_list )
{
	char where[ 1024 ];
	char *ptr = where;

	if ( !list_rewind( relation_foreign_key_list ) ) return (char *)0;
	if ( !list_rewind( dictionary_data_list ) ) return (char *)0;

	while ( 1 )
	{
		if ( ptr != where )
		{
			ptr += sprintf( ptr, " and " );
		}

		ptr += sprintf(
			ptr,
			"%s = '%s'",
			(char *)list_get( relation_foreign_key_list ),
			(char *)list_get( dictionary_data_list ) );

		if ( !list_next( dictionary_data_list ) ) break;
		if ( !list_next( relation_foreign_key_list ) ) break;
	}

	return strdup( where );
}

void relation_join_free( RELATION_JOIN *relation_join )
{
	list_free_container(
		relation_join->
			relation_join_folder->
			where->
			dictionary_data_list );

	list_free_container(
		relation_join->
			relation_join_folder->
			where->
			foreign_attribute_list );

	list_free_container(
		relation_join->
			relation_join_folder->
			where->
			foreign_attribute_name_list );

	list_free_container(
		relation_join->
			relation_join_folder->
			where->
			relation_foreign_key_list );

	free(	relation_join->
			relation_join_folder->
			where->
			string );

	free(	relation_join->
			relation_join_folder->
			where );

	list_free_container(
		relation_join->
			relation_join_folder->
			select_name_list );

	list_free_container(
		relation_join->
			relation_join_folder->
			select_name_list );

	free(	relation_join->
			relation_join_folder->
			system_string );

	free( relation_join->relation_join_folder );
	free( relation_join );
}
