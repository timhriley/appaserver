/* --------------------------------------------- */
/* $APPASERVER_HOME/library/relation.c		 */
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
#include "environ.h"
#include "foreign_attribute.h"
#include "folder_attribute.h"
#include "relation.h"

RELATION *relation_calloc( void )
{
	RELATION *relation;

	if ( ! ( relation = calloc( 1, sizeof( RELATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation;
}

RELATION *relation_new(	char *many_folder_name,
			char *one_folder_name )
{
	RELATION *relation = relation_calloc();

	relation->many_folder_name = many_folder_name;
	relation->one_folder_name = one_folder_name;

	return relation;
}

RELATION *relation_parse(
			char *input,
			boolean fetch_folder,
			boolean fetch_folder_attribute_list,
			boolean fetch_process )
{
	char many_folder_name[ 128 ];
	char one_folder_name[ 128 ];
	char related_attribute_name[ 128 ];
	char pair_one2m_order[ 128 ];
	char omit_one2m_detail[ 128 ];
	char relation_type_isa[ 128 ];
	char copy_common_attributes[ 128 ];
	char automatic_preselection[ 128 ];
	char drop_down_multi_select[ 128 ];
	char join_one2m_each_row[ 128 ];
	char omit_lookup_before_drop_down[ 128 ];
	char ajax_fill_drop_down[ 128 ];
	char hint_message[ 4096 ];
	RELATION *relation;

	if ( !input || !*input ) return (RELATION *)0;

	/* See: RELATION_SELECT_COLUMNS */
	/* ---------------------------- */
	piece( many_folder_name, SQL_DELIMITER, input, 0 );
	piece( one_folder_name, SQL_DELIMITER, input, 1 );

	relation =
		relation_new(
			strdup( many_folder_name ),
			strdup( one_folder_name ) );

	piece( related_attribute_name, SQL_DELIMITER, input, 2 );
	relation->related_attribute_name = strdup( related_attribute_name );

	piece( pair_one2m_order, SQL_DELIMITER, input, 3 );
	relation->pair_one2m_order = atoi( pair_one2m_order );

	piece( omit_one2m_detail, SQL_DELIMITER, input, 4 );
	relation->omit_one2m_detail = ( *omit_one2m_detail == 'y' );

	piece( relation_type_isa, SQL_DELIMITER, input, 5 );
	relation->relation_type_isa = ( *relation_type_isa == 'y' );

	piece( copy_common_attributes, SQL_DELIMITER, input, 6 );
	relation->copy_common_attributes = ( *copy_common_attributes == 'y' );

	piece( automatic_preselection, SQL_DELIMITER, input, 7 );
	relation->automatic_preselection = ( *automatic_preselection == 'y' );

	piece( drop_down_multi_select, SQL_DELIMITER, input, 8 );
	relation->drop_down_multi_select = ( *drop_down_multi_select == 'y' );

	piece( join_one2m_each_row, SQL_DELIMITER, input, 9 );
	relation->join_one2m_each_row = ( *join_one2m_each_row == 'y' );

	piece( omit_lookup_before_drop_down, SQL_DELIMITER, input, 10 );
	relation->omit_lookup_before_drop_down =
		( *omit_lookup_before_drop_down == 'y' );

	piece( ajax_fill_drop_down, SQL_DELIMITER, input, 11 );
	relation->ajax_fill_drop_down = ( *ajax_fill_drop_down == 'y' );

	piece( hint_message, SQL_DELIMITER, input, 12 );
	relation->hint_message = strdup( hint_message );

	if ( fetch_folder )
	{
		relation->many_folder =
			folder_fetch(
				many_folder_name,
				(char *)0 /* role_name */,
				(LIST *)0 /* exclude_attribute_name_list */,
				fetch_folder_attribute_list,
				0 /* not fetch_relation_mto1_non_isa_list */,
				0 /* not fetch_relation_mto1_isa_list */,
				0 /* not fetch_relation_one2m_list */,
				0 /* not fetch_relation_one2m_recursive_list */,
				fetch_process,
				0 /* not fetch_role_folder_list */,
				0 /* not fetch_row_level_restriction */,
				0 /* not fetch_role_operation_list */ );

		relation->one_folder =
			folder_fetch(
				one_folder_name,
				(char *)0 /* role_name */,
				(LIST *)0 /* exclude_lookup_..._name_list */,
				fetch_folder_attribute_list,
				0 /* not fetch_relation_mto1_non_isa_list */,
				0 /* not fetch_relation_mto1_isa_list */,
				0 /* not fetch_relation_one2m_list */,
				0 /* not fetch_relation_one2m_recursive_list */,
				fetch_process,
				0 /* not fetch_role_folder_list */,
				0 /* not fetch_row_level_restriction */,
				0 /* not fetch_role_operation_list */ );
	}

	if ( fetch_folder && fetch_folder_attribute_list )
	{
		relation->foreign_key_list =
			relation_foreign_key_list(
				folder_attribute_primary_key_list(
				     relation->
						one_folder->
						folder_attribute_list )
				     		/* primary_foreign_key_list */,
				relation->related_attribute_name,
				foreign_attribute_name_list(
					foreign_attribute_list(
						relation->
							many_folder->
							folder_name,
						relation->
							one_folder->
							folder_name ) ) );
	}
	return relation;
}

LIST *relation_system_list(
			char *system_string,
			boolean fetch_folder,
			boolean fetch_attribute_list,
			boolean fetch_process )
{
	char input[ 65536 ];
	FILE *input_pipe = popen( system_string, "r" );
	LIST *relation_list = list_new();

	while ( string_input( input, input_pipe, 65536 ) )
	{
		list_set(
			relation_list,
			relation_parse(
				input,
				fetch_folder,
				fetch_attribute_list,
				fetch_process ) );
	}
	pclose( input_pipe );
	return relation_list;
}

char *relation_system_string(
			char *where,
			char *order )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" \"%s\"",
		RELATION_SELECT_COLUMNS,
		RELATION_TABLE,
		(where) ? where : "",
		(order ) ? order : "" );

	return strdup( system_string );
}

char *relation_display(	RELATION *relation )
{
	char display[ 65536 ];

	sprintf(display,
		"\nRelation: %s -> %s, foreign_key_list = [%s];\n",
		relation->many_folder->folder_name,
		relation->one_folder->folder_name,
		list_display( relation->foreign_key_list ) );

	return strdup( display );
}

LIST *relation_foreign_key_list(
			/* ------------------------------------------- */
			/* Send in folder_attribute_primary_key_list() */
			/* ------------------------------------------- */
			LIST *primary_foreign_key_list,
			char *related_attribute_name,
			LIST *foreign_attribute_name_list )
{
	LIST *return_list = {0};

	/* -------------------------- */
	/* If using FOREIGN_ATTRIBUTE */
	/* -------------------------- */
	if ( list_length( foreign_attribute_name_list ) )
	{
		return_list = foreign_attribute_name_list;
	}
	else
	/* ----------------------------------- */
	/* If populated related_attribute_name */
	/* ----------------------------------- */
	if ( related_attribute_name
	&&   *related_attribute_name
	&&   string_strcmp( related_attribute_name, "null" ) != 0 )
	{
		primary_foreign_key_list =
			list_duplicate(
				primary_foreign_key_list );

		list_replace_last( 
			primary_foreign_key_list,
			related_attribute_name );

		return_list = primary_foreign_key_list;
	}
	else
	{
		/* ---------------------------- */
		/* If exact one-to-one matching */
		/* ---------------------------- */
		return_list = primary_foreign_key_list;
	}
	return return_list;
}

LIST *relation_mto1_non_isa_list(
			char *many_folder_name )
{
	char where[ 256 ];

	sprintf(where,
		"folder = '%s' and ifnull(relation_type_isa,'n') != 'y'",
		many_folder_name );

	return
		relation_system_list(
			relation_system_string(
				where,
				(char *)0 /* order */ ),
			/* ---------------------------- */
			/* Setting both will set	*/
			/* foreign_attribute_name_list	*/
			/* ---------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */,
			0 /* not fetch_process */ );
}

LIST *relation_one2m_list( char *one_folder_name )
{
	char where[ 128 ];

	sprintf(where,
		"related_folder = '%s'",
		one_folder_name );

	return
		relation_system_list(
			relation_system_string(
				where,
				(char *)0 /* order */ ),
			/* ---------------------------------- */
			/* foreign_attribute_name_list is set */
			/* ---------------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */,
			0 /* not fetch_process */ );
}

LIST *relation_pair_one2m_list(
			LIST *relation_one2m_list )
{
	RELATION *relation;
	LIST *pair_one2m_list = {0};

	if ( !list_rewind( relation_one2m_list ) ) return (LIST *)0;

	do {
		relation = list_get( relation_one2m_list );

		if ( relation->pair_one2m_order )
		{
			if ( !pair_one2m_list ) pair_one2m_list = list_new();

			list_set( pair_one2m_list, relation );
		}

	} while ( list_next( relation_one2m_list ) );

	return pair_one2m_list;
}

LIST *relation_join_one2m_list(
			LIST *relation_one2m_recursive_list,
			DICTIONARY *ignore_dictionary )
{
	RELATION *relation;
	char key[ 128 ];
	LIST *join_one2m_list = {0};

	if ( !list_rewind( relation_one2m_recursive_list ) )
		return (LIST *)0;

	do {
		relation = list_get( relation_one2m_recursive_list );

		if ( relation->join_one2m_each_row )
		{
			sprintf(key,
			 	"%s_0",
			 	relation->
					one_folder->
					folder_name );

			if ( dictionary_key_exists( ignore_dictionary, key ) )
			{
				continue;
			}

			if ( !join_one2m_list ) join_one2m_list = list_new();

			list_set( join_one2m_list, relation );
		}

	} while ( list_next( relation_one2m_recursive_list ) );

	return join_one2m_list;
}

LIST *relation_one2m_recursive_list(
			LIST *relation_list,
			char *one_folder_name )
{
	LIST *local_relation_list;
	RELATION *relation;
	char where[ 128 ];

	sprintf(where, "related_folder = '%s'", one_folder_name );

	local_relation_list =
		relation_system_list(
			relation_system_string(
				where,
				(char *)0 /* order */ ),
			/* ---------------------------------- */
			/* foreign_attribute_name_list is set */
			/* ---------------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */,
			0 /* not fetch_process */ );

	if ( !list_rewind( local_relation_list ) )
		return relation_list;

	if ( !relation_list ) relation_list = list_new();

	do {
		relation = list_get( local_relation_list );

		list_set( relation_list, relation );

		if ( ( relation->is_primary_key_subset =
			relation_is_primary_key_subset(
				relation->foreign_key_list,
				relation->
				    many_folder->
				    primary_key_list
				    /* many_primary_key_list */ ) ) )
		{
			relation_list =
				relation_one2m_recursive_list(
					relation_list,
					relation->
						many_folder->
						folder_name );
		}

	} while ( list_next( local_relation_list ) );

	list_free_container( local_relation_list );

	return relation_list;
}

LIST *relation_mto1_isa_list(
			LIST *relation_list,
			char *many_folder_name,
			boolean fetch_process )
{
	LIST *local_relation_list;
	RELATION *relation;
	char where[ 256 ];

	sprintf(where,
		"folder = '%s' and ifnull(relation_type_isa,'n') = 'y'",
		many_folder_name );

	local_relation_list =
		relation_system_list(
			relation_system_string(
				where,
				(char *)0 /* order */ ),
			/* ---------------------------------- */
			/* foreign_attribute_name_list is set */
			/* ---------------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */,
			fetch_process );

	if ( !list_rewind( local_relation_list ) )
		return relation_list;

	if ( !relation_list ) relation_list = list_new();

	do {
		relation = list_get( local_relation_list );

		if ( !relation->relation_type_isa ) continue;

		list_set( relation_list, relation );

		relation_list =
			relation_mto1_isa_list(
				relation_list,
				relation->
					one_folder->
					folder_name,
				fetch_process );

	} while ( list_next( local_relation_list ) );

	list_free_container( local_relation_list );
	return relation_list;
}

boolean relation_is_primary_key_subset(
			LIST *foreign_key_list,
			LIST *many_primary_key_list )
{
	return list_is_subset_of(
			foreign_key_list,
			many_primary_key_list );
}

char *relation_list_display(
			LIST *relation_list )
{
	char display[ 65536 ];
	char *ptr = display;
	RELATION *relation;

	if ( !list_rewind( relation_list ) ) return "";

	*ptr = '\0';

	do {
		relation = list_get( relation_list );

		if ( ptr != display ) ptr += sprintf( ptr, "; " );

		ptr += sprintf(
			ptr,
			"%s",
			relation_display( relation ) );

	} while ( list_next( relation_list ) );

	return strdup( display );
}

LIST *relation_one2m_pair_list(
			char *one_folder_name )
{
	char where[ 256 ];

	sprintf(where,
		"related_folder = '%s' and	"
		"ifnull(pair_1tom_order,0) > 0	",
		one_folder_name );

	return
		relation_system_list(
			relation_system_string(
				where,
				"pair_1tom_order" /* order */ ),
			/* ---------------------------------- */
			/* foreign_attribute_name_list is set */
			/* ---------------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */,
			0 /* not fetch_process */ );
}

LIST *relation_mto1_drillthru_list(
			LIST *relation_list,
			char *base_folder_name,
			LIST *fulfilled_folder_name_list )
{
	LIST *local_relation_list;
	RELATION *relation;
	char where[ 256 ];

	sprintf(where,
		"folder = '%s' and ifnull(relation_type_isa,'n') = 'n'",
		base_folder_name );

	local_relation_list =
		relation_system_list(
			relation_system_string(
				where,
				(char *)0 /* order */ ),
			1 /* fetch_folder */,
			0 /* not fetch_attribute_list */,
			0 /* not fetch_process */ );

	if ( !list_rewind( local_relation_list ) )
		return relation_list;

	do {
		relation = list_get( local_relation_list );

		if ( relation->one_folder->drilldown
		&&   !list_exists_string(
			relation->one_folder->folder_name,
			fulfilled_folder_name_list ) )
		{
			if ( !relation_list ) relation_list = list_new();

			list_set( relation_list, relation );

			relation_list =
				relation_mto1_drillthru_list(
					relation_list,
					relation->
						one_folder->
						folder_name,
					fulfilled_folder_name_list );
		}

	} while ( list_next( local_relation_list ) );

	list_free_container( local_relation_list );
	return relation_list;
}

RELATION *relation_consumes(
			char *many_attribute_name,
			LIST *relation_mto1_list )
{
	RELATION *relation;

	if ( !list_rewind( relation_mto1_list ) ) return (RELATION *)0;

	do {
		relation =
			list_get(
				relation_mto1_list );

		if ( relation->consumes_taken ) continue;

		if ( !relation->foreign_key_list )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( list_string_exists(
			many_attribute_name,
			relation->foreign_key_list ) )
		{
			return relation;
		}

	} while( list_next( relation_mto1_list ) );

	return (RELATION *)0;
}

RELATION *relation_one2m_seek(
			char *folder_name,
			LIST *relation_one2m_list )
{
	RELATION *relation;

	if ( !list_rewind( relation_one2m_list ) )
	{
		return (RELATION *)0;
	}

	do {
		relation =
			list_get(
				relation_one2m_list );

		if ( !relation->many_folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: many_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(
			folder_name,
			relation->many_folder->folder_name ) == 0 )
		{
			return relation;
		}

	} while ( list_next( relation_one2m_list ) );

	return (RELATION *)0;
}

RELATION *relation_mto1_seek(
			char *folder_name,
			LIST *relation_mto1_list )
{
	RELATION *relation;

	if ( !list_rewind( relation_mto1_list ) )
	{
		return (RELATION *)0;
	}

	do {
		relation =
			list_get(
				relation_mto1_list );

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
			return relation;
		}

	} while ( list_next( relation_mto1_list ) );

	return (RELATION *)0;
}

LIST *relation_mto1_folder_name_list(
			LIST *relation_mto1_list )
{
	RELATION *relation;
	LIST *folder_name_list;

	if ( !list_rewind( relation_mto1_list ) )
	{
		return (LIST *)0;
	}

	folder_name_list = list_new();

	do {
		relation =
			list_get(
				relation_mto1_list );

		list_set(
			folder_name_list,
			relation->one_folder_name );

	} while ( list_next( relation_mto1_list ) );

	return folder_name_list;
}

LIST *relation_one2m_folder_name_list(
			LIST *relation_one2m_list )
{
	RELATION *relation;
	LIST *folder_name_list;

	if ( !list_rewind( relation_one2m_list ) )
	{
		return (LIST *)0;
	}

	folder_name_list = list_new();

	do {
		relation =
			list_get(
				relation_one2m_list );

		list_set(
			folder_name_list,
			relation->many_folder_name );

	} while ( list_next( relation_one2m_list ) );

	return folder_name_list;
}

void relation_set_one_folder_primary_delimited_list(
			LIST *relation_mto1_list )
{
	RELATION *relation;

	if ( !list_rewind( relation_mto1_list ) ) return;

	do {
		relation = list_get( relation_mto1_list );

		if ( !relation->one_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !list_length(
			relation->
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

		relation->one_folder->delimited_list =
			folder_delimited_list(
				folder_table_name(
					environment_application_name(),
					relation->one_folder->folder_name ),
				relation->one_folder->primary_key_list
					/* attribute_name_list */,
				(char *)0 /* where_clause */ );

	} while ( list_next( relation_mto1_list ) );
}

boolean relation_exists_multi_select(
			LIST *relation_mto1_non_isa_list )
{
	RELATION *relation;

	if ( !list_rewind( relation_mto1_non_isa_list ) ) return 0;

	do {
		relation =
			list_get(
				relation_mto1_non_isa_list );

		if ( relation->drop_down_multi_select ) return 1;

	} while ( list_next( relation_mto1_non_isa_list ) );

	return 0;
}

