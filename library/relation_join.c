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
			DICTIONARY *dictionary,
			LIST *relation_join_one2m_list,
			LIST *primary_key_list )
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

		if ( !list_length( relation_join_one2m->foreign_key_list ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		relation_join->relation_join_folder =
			relation_join_folder_new(
				relation_join_one2m->many_folder,
				relation_join_folder->foreign_key_list,
				primary_key_list );
	return relation_join;
}

void relation_join_one2m_list_set(
			DICTIONARY *dictionary,
			LIST *relation_join_one2m_list,
			LIST *primary_key_list )
{
	RELATION *relation_join_one2m;

	if ( !list_rewind( relation_join_one2m_list ) ) return;

	do {
		relation_join_one2m = list_get( relation_join_one2m_list );

		relation_join_one2m_set(
			dictionary,
			relation_join_one2m,
			primary_key_list );

	} while ( list_next( relation_join_one2m_list ) );
}

void relation_join_one2m_set(
			DICTIONARY *dictionary,
			RELATION *relation_join_one2m,
			LIST *primary_key_list )
{
	char *related_data_list_string;
	char *where_clause;
	char sys_string[ 1024 ];
	LIST *select_list;

	select_list = list_subtract(
			related_folder->
				one2m_folder->
				primary_attribute_name_list,
			primary_attribute_name_list );

	where_clause =
		dictionary_get_attribute_where_clause(
			row_dictionary,
			related_folder_foreign_attribute_name_list( 
				primary_attribute_name_list,
				related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list ) );

	if ( strcmp( where_clause, "1 = 1" ) == 0 )
	{
		where_clause =
			dictionary_get_attribute_where_clause(
				row_dictionary,
				primary_attribute_name_list );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=select		|"
		 "joinlines.e '%c'				 ",
		 application_name,
		 list_display( select_list ),
		 related_folder->one2m_folder->folder_name,
		 where_clause,
		 MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER );

	related_data_list_string = pipe2string( sys_string );

	if ( related_data_list_string && *related_data_list_string )
	{
		dictionary_set_pointer(
			row_dictionary,
		 	related_folder->
				one2m_folder->
				folder_name,
		 	related_data_list_string );
	}
	else
	{
		dictionary_set_pointer(
			row_dictionary,
		 	related_folder->
				one2m_folder->
				folder_name,
		 	"" );
	}
}

