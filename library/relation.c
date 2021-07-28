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
#include "foreign_attribute.h"
#include "attribute.h"
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
			boolean fetch_attribute_list )
{
	char many_folder_name[ 128 ];
	char one_folder_name[ 128 ];
	char related_attribute_name[ 128 ];
	char pair_1tom_order[ 128 ];
	char omit_1tom_detail[ 128 ];
	char prompt_mto1_recursive[ 128 ];
	char relation_type_isa[ 128 ];
	char copy_common_attributes[ 128 ];
	char automatic_preselection[ 128 ];
	char drop_down_multi_select[ 128 ];
	char join_1tom_each_row[ 128 ];
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

	piece( pair_1tom_order, SQL_DELIMITER, input, 3 );
	relation->pair_1tom_order = atoi( pair_1tom_order );

	piece( omit_1tom_detail, SQL_DELIMITER, input, 4 );
	relation->omit_1tom_detail = ( *omit_1tom_detail == 'y' );

	piece( prompt_mto1_recursive, SQL_DELIMITER, input, 5 );
	relation->prompt_mto1_recursive = ( *prompt_mto1_recursive == 'y' );

	piece( relation_type_isa, SQL_DELIMITER, input, 6 );
	relation->relation_type_isa = ( *relation_type_isa == 'y' );

	piece( copy_common_attributes, SQL_DELIMITER, input, 7 );
	relation->copy_common_attributes = ( *copy_common_attributes == 'y' );

	piece( automatic_preselection, SQL_DELIMITER, input, 8 );
	relation->automatic_preselection = ( *automatic_preselection == 'y' );

	piece( drop_down_multi_select, SQL_DELIMITER, input, 9 );
	relation->drop_down_multi_select = ( *drop_down_multi_select == 'y' );

	piece( join_1tom_each_row, SQL_DELIMITER, input, 10 );
	relation->join_1tom_each_row = ( *join_1tom_each_row == 'y' );

	piece( omit_lookup_before_drop_down, SQL_DELIMITER, input, 11 );
	relation->omit_lookup_before_drop_down =
		( *omit_lookup_before_drop_down == 'y' );

	piece( ajax_fill_drop_down, SQL_DELIMITER, input, 12 );
	relation->ajax_fill_drop_down = ( *ajax_fill_drop_down == 'y' );

	piece( hint_message, SQL_DELIMITER, input, 13 );
	relation->hint_message = strdup( hint_message );

	if ( fetch_folder )
	{
		relation->many_folder =
			folder_fetch(
			     many_folder_name,
			     fetch_attribute_list,
			     0 /* not fetch_row_level_restriction */ );

		relation->one_folder =
			folder_fetch(
			     one_folder_name,
			     fetch_attribute_list,
			     0 /* not fetch_row_level_restriction */ );
	}

	if ( fetch_folder && fetch_attribute_list )
	{
		relation->foreign_attribute_name_list =
			relation_foreign_attribute_name_list(
				attribute_primary_name_list(
				     relation->
						one_folder->
						attribute_list )
				     /* primary_foreign_attribute_name_list */,
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
			boolean fetch_attribute_list )
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
				fetch_attribute_list ) );
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
		"\nRelation: %s -> %s, foreign_attribute_name_list = [%s];\n",
		relation->many_folder->folder_name,
		relation->one_folder->folder_name,
		list_display(
			relation->foreign_attribute_name_list ) );

	return strdup( display );
}

#ifdef NOT_DEFINED
boolean relation_list_exists(
			LIST *relation_list,
			char *many_folder_name,
			char *one_folder_name )
{
	RELATION *relation;

	if ( !list_rewind( relation_list ) ) return 0;

	do {
		relation =
			list_get(
				relation_list );

		if ( string_strcmp(
			relation->many_folder_name,
			many_folder_name ) == 0
		&&   string_strcmp(
			relation->one_folder_name,
			one_folder_name ) == 0 )
		{
			return 1;
		}

	} while ( list_next( relation_list ) );

	return 0;
}
#endif

LIST *relation_foreign_attribute_name_list(
			/* ----------------------------------- */
			/* Send in primary_attribute_name_list */
			/* ----------------------------------- */
			LIST *primary_foreign_attribute_name_list,
			char *related_attribute_name,
			LIST *foreign_attribute_name_list )
{
	LIST *return_list = {0};

	/* -------------------------- */
	/* If using FOREIGN_ATTRIBUTE */
	/* -------------------------- */
	if ( list_length( foreign_attribute_list_folder ) )
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
		primary_foreign_attribute_name_list =
			string_list_duplicate(
				primary_foreign_attribute_name_list );

		list_replace_last_string( 
			primary_foreign_attribute_name_list,
			related_attribute_name );

		return_list = primary_foreign_attribute_name_list;
	}
	else
	{
		/* ---------------------------- */
		/* If exact one-to-one matching */
		/* ---------------------------- */
		return_list = primary_foreign_attribute_name_list;
	}
	return return_list;
}

LIST *relation_fetch_one2m_list(
			char *one_folder_name,
			char *where,
			char *order )
{
	char relation_where[ 128 ];
	char full_where[ 256 ];

	sprintf(relation_where,
		"related_folder = '%s'",
		one_folder_name );

	if ( where && *where )
	{
		sprintf(full_where,
			"%s and %s",
			where,
			relation_where );
	}
	else
	{
		strcpy( full_where, relation_where );
	}

	return
		relation_system_list(
			relation_system_string(
				full_where,
				order ),
			/* ---------------------------- */
			/* Setting both will set	*/
			/* foreign_attribute_name_list	*/
			/* ---------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */ );
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
			1 /* fetch_attribute_list */ );
}

LIST *relation_one2m_recursive_list(
			LIST *relation_list,
			char *one_folder_name )
{
	LIST *local_relation_list;
	RELATION *relation;
	char where[ 128 ];

	sprintf(where,
		"related_folder = '%s'",
		one_folder_name );

	local_relation_list =
		relation_system_list(
			relation_system_string(
				where,
				(char *)0 /* order */ ),
			/* ---------------------------------- */
			/* foreign_attribute_name_list is set */
			/* ---------------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */ );

	if ( !list_rewind( local_relation_list ) )
		return relation_list;

	if ( !relation_list ) relation_list = list_new();

	do {
		relation = list_get( local_relation_list );

		list_set( relation_list, relation );

		if ( ( relation->is_primary_key_subset =
			relation_is_primary_key_subset(
				relation->foreign_attribute_name_list,
				relation->
				    many_folder->
				    primary_attribute_name_list
				    /* mto1_primary_attribute_name_list */ ) ) )
		{
			relation_list =
				relation_one2m_recursive_list(
					relation_list,
					relation->
						many_folder->
						folder_name
						/* translates to	*/
						/* one_folder_name	*/ );
		}
	} while ( list_next( local_relation_list ) );

	list_free_container( local_relation_list );
	return relation_list;
}

LIST *relation_mto1_isa_recursive_list(
			LIST *relation_list,
			char *many_folder_name )
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
			1 /* fetch_attribute_list */ );

	if ( !list_rewind( local_relation_list ) )
		return relation_list;

	if ( !relation_list ) relation_list = list_new();

	do {
		relation = list_get( local_relation_list );

		if ( !relation->relation_type_isa ) continue;

		list_set( relation_list, relation );

		relation_list =
			relation_mto1_isa_recursive_list(
				relation_list,
				relation->
					one_folder->
					folder_name
					/* translates to	*/
					/* many_folder_name	*/ );

	} while ( list_next( local_relation_list ) );

	list_free_container( local_relation_list );
	return relation_list;
}

boolean relation_is_primary_key_subset(
			LIST *foreign_attribute_name_list,
			LIST *mto1_primary_attribute_name_list )
{
	return list_is_subset_of(
			foreign_attribute_name_list,
			mto1_primary_attribute_name_list );
}

char *relation_list_display(
			LIST *relation_list )
{
	char display[ 65536 ];
	char *ptr = display;
	RELATION *relation;
	boolean first_time = 1;

	if ( !list_rewind( relation_list ) ) return "";

	*ptr = '\0';

	do {
		relation = list_get( relation_list );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, "; " );

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
	char *where = "ifnull(pair_1tom_order,0) > 0";

	return
		relation_system_list(
			relation_system_string(
				where,
				"pair_1tom_order" /* order */ ),
			/* ---------------------------------- */
			/* foreign_attribute_name_list is set */
			/* ---------------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */ );
}

#ifdef NOT_DEFINED
/* Transition between the two systems */
/* ---------------------------------- */
#include "related_folder.h"
LIST *relation_mto1_related_folder_list(
			LIST *mto1_relation_list )
{
	RELATION *relation;
	RELATED_FOLDER *related_folder;
	LIST *mto1_related_folder_list = {0};

	if ( !list_rewind( mto1_relation_list ) ) return (LIST *)0;

	do {
		relation = list_get( mto1_relation_list );

		related_folder =
			related_folder_new(
				(char *)0 /* application_name */,
				(char *)0 /* session */,
				relation->one_folder_name,
				relation->related_attribute_name );

		related_folder->foreign_attribute_name_list =
			relation->foreign_attribute_name_list;

		related_folder->pair_1tom_order =
			relation->pair_1tom_order;

		related_folder->omit_1tom_detail =
			relation->omit_1tom_detail;

		related_folder->prompt_mto1_recursive =
			relation->prompt_mto1_recursive;

		related_folder->relation_type_isa =
			relation->relation_type_isa;

		related_folder->copy_common_attributes =
			relation->copy_common_attributes;

		related_folder->automatic_preselection =
			relation->automatic_preselection;

		related_folder->drop_down_multi_select =
			relation->drop_down_multi_select;

		related_folder->join_1tom_each_row =
			relation->join_1tom_each_row;

		related_folder->omit_lookup_before_drop_down =
			relation->omit_lookup_before_drop_down;

		related_folder->ajax_fill_drop_down =
			relation->ajax_fill_drop_down;

		related_folder->hint_message =
			relation->hint_message;

		related_folder->folder->attribute_list =
			relation->one_folder->attribute_list;

		if ( !mto1_related_folder_list )
			mto1_related_folder_list =
				list_new();

		list_set( mto1_related_folder_list, related_folder );

	} while ( list_next( mto1_relation_list ) );

	return mto1_related_folder_list;
}
#endif

