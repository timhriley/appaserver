/* $APPASERVER_HOME/library/appaserver.c				*/
/* -------------------------------------------------------------------- */
/* This maintains a set of APPASERVER ADTs.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "application.h"
#include "appaserver_user.h"
#include "folder.h"
#include "list.h"
#include "relation.h"
#include "piece.h"
#include "timlib.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "hash_table.h"
#include "String.h"
#include "sql.h"
#include "aggregate_statistic.h"
#include "appaserver.h"

APPASERVER *appaserver_calloc( void )
{
	APPASERVER *appaserver;

	if ( ! ( appaserver =
			(APPASERVER *)
				calloc( 1, sizeof( APPASERVER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return appaserver;

}

LIST *appaserver_exclude_permission_record_list(
			char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=attribute,role,permission"
		 "			folder=attribute_exclude	",
		 application_name );

	return pipe2list( sys_string );
}

boolean appaserver_exclude_permission(
			LIST *exclude_permission_list,
			char *permission )
{
	char *compare_permission;

	if ( !exclude_permission_list
	||   !list_rewind( exclude_permission_list ) )
		return 0;
	do {
		compare_permission =
			list_get( exclude_permission_list );
		if ( strcmp( compare_permission, permission ) == 0 )
			return 1;
	} while( list_next( exclude_permission_list ) );
	return 0;
}

boolean appaserver_frameset_menu_horizontal(
			char *application_name,
			char *login_name )
{
	char frameset_menu_horizontal_yn;

	frameset_menu_horizontal_yn =
		application_frameset_menu_horizontal_yn(
			application_name );

	if ( login_name ){};
/*
	boolean frameset_menu_horizontal;
	boolean return_value = 1;
	frameset_menu_horizontal =
		appaserver_user_frameset_menu_horizontal(
				application_name,
				login_name );

	if ( frameset_menu_horizontal_yn != 'y'
	||   !frameset_menu_horizontal )
	{
		return_value = 0;
	}
*/

	return (frameset_menu_horizontal_yn == 'y');

}

LIST *appaserver_isa_folder_list( char *application_name )
{
	static LIST *isa_folder_list = {0};
	ISA_FOLDER *isa_folder;
	char sys_string[ 1024 ];
	char *where = "relation_type_isa_yn = 'y'";
	char input_buffer[ 1024 ];
	char folder_name[ 128 ];
	char related_folder_name[ 128 ];
	char related_attribute_name[ 128 ];
	FILE *pipe;
	char *select = "folder,related_folder,related_attribute";

	if ( isa_folder_list ) return isa_folder_list;

	isa_folder_list = list_new_list();

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			folder=relation			"
		"			select=%s			"
		"			where=\"%s\"			",
		application_name,
		select,
		where );

	pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, pipe ) )
	{
		piece( folder_name, '^', input_buffer, 0 );
		piece( related_folder_name, '^', input_buffer, 1 );
		piece( related_attribute_name, '^', input_buffer, 2 );

		isa_folder = appaserver_new_isa_folder(
					strdup( folder_name ),
					strdup( related_folder_name ),
					strdup( related_attribute_name ) );

		list_append_pointer(	isa_folder_list,
					isa_folder );
	}
	pclose( pipe );
	return isa_folder_list;
}

char *appaserver_isa_folder_list_display(
			LIST *isa_folder_list )
{
	ISA_FOLDER *isa_folder;
	char buffer[ 4096 ];
	char *buffer_ptr = buffer;
	boolean first_time = 1;

	*buffer_ptr = '\0';

	if ( list_rewind( isa_folder_list ) )
	{
		do {
			isa_folder =
				list_get(
					isa_folder_list );
			if ( first_time )
			{
				first_time = 0;
			}
			else
			{
				buffer_ptr += sprintf( buffer_ptr, "," );
			}

			buffer_ptr +=
			sprintf(
				buffer_ptr,
			"folder = %s, related_folder = %s, accounted_for = %d",
				isa_folder->folder_name,
				isa_folder->related_folder_name,
				isa_folder->accounted_for );

		} while( list_next( isa_folder_list ) );
	}
	return strdup( buffer );
}

ISA_FOLDER *appaserver_new_isa_folder(
			char *folder_name,
			char *related_folder_name,
			char *related_attribute_name )
{
	ISA_FOLDER *a = (ISA_FOLDER *)calloc( 1, sizeof( ISA_FOLDER ) );

	if ( !related_attribute_name
	||   !*related_attribute_name )
	{
		related_attribute_name = "null";
	}

	a->folder_name = folder_name;
	a->related_folder_name = related_folder_name;
	a->related_attribute_name = related_attribute_name;
	return a;
}

int appaserver_isa_folder_accounted_for(
			LIST *isa_folder_list,
			char *related_folder_name,
			char *related_attribute_name )
{
	ISA_FOLDER *isa_folder;

	if ( !list_rewind( isa_folder_list ) ) return 0;

	if ( !related_attribute_name ) related_attribute_name = "null";

	do {
		isa_folder = list_get( isa_folder_list );

		if (
			( strcmp(	isa_folder->folder_name,
					related_folder_name ) == 0
		&&        strcmp(	isa_folder->related_attribute_name,
					related_attribute_name ) == 0 ) )
		{
			if ( isa_folder->accounted_for ) return 1;
			isa_folder->accounted_for = 1;
			return 0;
		}
	} while( list_next( isa_folder_list ) );

	return 0;

}

enum aggregate_statistic appaserver_based_on_datatype_aggregate_statistic(
			char *application_name,
			char *appaserver_mount_point,
			char *datatype )
{
	enum aggregate_statistic aggregate_statistic;

	if ( appaserver_aggregation_sum_yn(
				application_name,
				appaserver_mount_point,
				datatype ) == 'y' )
	{
		aggregate_statistic =
			aggregate_statistic_evaluate(
				"sum",
				daily );
	}
	else
	{
		aggregate_statistic =
			aggregate_statistic_evaluate(
				"average",
				daily );
	}

	return aggregate_statistic;
}

char appaserver_aggregation_sum_yn(
			char *application,
			char *appaserver_mount_point,
			char *datatype )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "%s/%s/get_aggregation_sum_yn '%s' '%s'",
		 appaserver_mount_point,
		 application_first_relative_source_directory(
			application ),
		 application,
		 datatype );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return 'n';
	}
	else
	{
		return *results;
	}
}

/* Returns heap memory */
/* ------------------- */
char *appaserver_escape_street_address( char *street_address )
{
	char escape_street_address[ 1024 ];

	return strdup(
		string_escape_full(
			escape_street_address,
			street_address ) );
}

LIST *appaserver_ignore_pressed_attribute_name_list( 	
			DICTIONARY *ignore_dictionary,
			DICTIONARY *query_dictionary,
			LIST *attribute_name_list )
{
	LIST *return_list = create_list();
	char *attribute_name;

	if ( !dictionary_length( ignore_dictionary ) ) return return_list;
	if ( !list_reset( attribute_name_list ) ) return return_list;

	do {
		attribute_name = list_get( attribute_name_list );

		if ( dictionary_key_exists_index_zero(
			ignore_dictionary,
			attribute_name ) )
		{
			if ( !query_dictionary )
			{
				list_set(
					return_list, 
					attribute_name );
			}
			else
			if ( !dictionary_key_exists_index_zero(
				query_dictionary,
				attribute_name ) )
			{
				list_set(
					return_list, 
					attribute_name );
			}
			continue;
		}

	} while( list_next( attribute_name_list ) );

	return return_list;
}

