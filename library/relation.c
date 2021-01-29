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
#include "relation.h"

RELATION *relation_calloc( void )
{
	RELATION *relation;

	if ( ! ( relation = calloc( 1, sizeof( RELATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation;
}

RELATION *relation_new(	char *mto1_folder_name,
			char *one2m_folder_name,
			boolean fetch_folder,
			boolean fetch_attribute_list )
{
	RELATION *relation = relation_calloc();

	relation->mto1_folder_name = mto1_folder_name;
	relation->one2m_folder_name = one2m_folder_name;

	if ( fetch_folder )
	{
		relation->mto1_folder =
			folder_fetch(
				mto1_folder_name,
				fetch_attribute_list,
				0 /* not fetch_one2m_recursive_relation_list */,
				0 /* not fetch_mto1_relation_list */,
				0 /* not fetch_mto1_isa_relation_list */ );

		relation->one2m_folder =
			folder_fetch(
				one2m_folder_name,
				fetch_attribute_list,
				0 /* not fetch_one2m_recursive_relation_list */,
				0 /* not fetch_mto1_relation_list */,
				0 /* not fetch_mto1_isa_relation_list */ );
	}

	return relation;
}

RELATION *relation_parse(
			char *input,
			boolean fetch_folder,
			boolean fetch_attribute_list )
{
	char mto1_folder_name[ 128 ];
	char one2m_folder_name[ 128 ];
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
	piece( mto1_folder_name, SQL_DELIMITER, input, 0 );
	piece( one2m_folder_name, SQL_DELIMITER, input, 1 );

	relation =
		relation_new(
			strdup( mto1_folder_name ),
			strdup( one2m_folder_name ),
			fetch_folder,
			fetch_attribute_list );

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

	if ( fetch_folder && fetch_attribute_list )
	{
		relation->foreign_attribute_name_list =
			relation_foreign_attribute_name_list(
				attribute_primary_name_list(
				     relation->
						one2m_folder->
						attribute_list )
				     /* primary_foreign_attribute_name_list */,
				relation->related_attribute_name,
				foreign_attribute_list(
					relation->
						mto1_folder->
						folder_name )
					/* foreign_attribute_list_folder */ );
	}
	return relation;
}

LIST *relation_system_list(
			char *sys_string,
			boolean fetch_folder,
			boolean fetch_attribute_list )
{
	char input[ 65536 ];
	FILE *input_pipe = popen( sys_string, "r" );
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

char *relation_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '%s' %s \"%s\"",
		RELATION_SELECT_COLUMNS,
		RELATION_TABLE,
		where );

	return strdup( sys_string );
}

char *relation_display(	RELATION *relation )
{
	char display[ 65536 ];

	sprintf(display,
		"\nRelation: %s -> %s, foreign_attribute_name_list = [%s];\n",
		relation->mto1_folder->folder_name,
		relation->one2m_folder->folder_name,
		list_display(
			relation->foreign_attribute_name_list ) );

	return strdup( display );
}

LIST *relation_one2m_recursive_relation_list(
			LIST *relation_list,
			char *one2m_folder_name )
{
	LIST *local_relation_list;
	RELATION *relation;

	if ( !relation_list ) relation_list = list_new();

	local_relation_list =
		/* ---------------------------------- */
		/* foreign_attribute_name_list is set */
		/* ---------------------------------- */
		relation_one2m_fetch_relation_list(
			one2m_folder_name );

	if ( !list_rewind( local_relation_list ) )
		return relation_list;

	do {
		relation = list_get( local_relation_list );

		if ( relation_list_exists(
			relation_list,
			relation->mto1_folder->folder_name,
			relation->one2m_folder->folder_name ) )
		{
			continue;
		}

		list_set( relation_list, relation );

		if ( ( relation->is_primary_key_subset =
			relation_is_primary_key_subset(
				relation->foreign_attribute_name_list,
				relation->
				    mto1_folder->
				    primary_attribute_name_list
				    /* mto1_primary_attribute_name_list */ ) ) )
		{
			relation_list =
				relation_one2m_recursive_relation_list(
					relation_list,
					relation->
						mto1_folder->
						folder_name
						/* one2m_folder_name */ );
		}
	} while ( list_next( local_relation_list ) );

	return relation_list;
}

boolean relation_list_exists(
			LIST *relation_list,
			char *mto1_folder_name,
			char *one2m_folder_name )
{
	RELATION *relation;

	if ( !list_rewind( relation_list ) ) return 0;

	do {
		relation =
			list_get(
				relation_list );

		if ( string_strcmp(
			relation->mto1_folder_name,
			mto1_folder_name ) == 0
		&&   string_strcmp(
			relation->one2m_folder_name,
			one2m_folder_name ) == 0 )
		{
			return 1;
		}

	} while ( list_next( relation_list ) );

	return 0;
}

LIST *relation_foreign_attribute_name_list(
			/* ----------------------------------- */
			/* Send in primary_attribute_name_list */
			/* ----------------------------------- */
			LIST *primary_foreign_attribute_name_list,
			char *related_attribute_name,
			LIST *foreign_attribute_list_folder )
{
	LIST *return_list = {0};

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
	/* -------------------------- */
	/* If using FOREIGN_ATTRIBUTE */
	/* -------------------------- */
	if ( list_length( foreign_attribute_list_folder ) )
	{
		return_list =
			foreign_attribute_name_list(
				foreign_attribute_list_folder );
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

LIST *relation_one2m_fetch_relation_list(
			char *one2m_folder_name )
{
	char where[ 128 ];

	sprintf(where,
		"related_folder = '%s'",
		one2m_folder_name );

	return
		relation_system_list(
			relation_sys_string(
				where ),
			/* ---------------------------- */
			/* Setting both will set	*/
			/* foreign_attribute_name_list	*/
			/* ---------------------------- */
			1 /* fetch_folder */,
			1 /* fetch_attribute_list */ );
}

LIST *relation_mto1_relation_list(
			LIST *relation_list,
			char *mto1_folder_name )
{
	return (LIST *)0;
}

LIST *relation_mto1_isa_relation_list(
			LIST *relation_list,
			char *mto1_folder_name )
{
	return (LIST *)0;
}

boolean relation_is_primary_key_subset(
			LIST *foreign_attribute_name_list,
			LIST *mto1_primary_attribute_name_list )
{
	return list_is_subset_of(
			foreign_attribute_name_list,
			mto1_primary_attribute_name_list );
}
