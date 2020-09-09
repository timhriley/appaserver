/* src_benthic/benthic_species_list.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "dictionary.h"
#include "folder.h"
#include "timlib.h"
#include "query.h"
#include "piece.h"
#include "attribute.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "query.h"

/* Prototypes */
/* ---------- */
LIST *get_species_record_list(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int replicate_number,
			int sweep_number );

boolean get_dictionary_variables(
			char **anchor_date,
			char **anchor_time,
			char **location,
			char **site_number_string,
			char **replicate_number_string,
			char **sweep_number_string,
			DICTIONARY *post_dictionary );

char *get_species_where_clause(
			char *application_name,
			DICTIONARY *post_dictionary );

char *get_species_sys_string(
			char *application_name,
			DICTIONARY *post_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *sys_string;
	DICTIONARY *post_dictionary = {0};

	output_starting_argv_stderr( argc, argv );

	if ( argc < 2 )
	{
		fprintf( stderr,
	"Usage: %s application [dictionary]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];

	if ( argc == 3 && strcmp( argv[ 2 ], "$dictionary" ) != 0 )
	{
		post_dictionary = 
			dictionary_string2dictionary( argv[ 2 ] );
	}

	sys_string = get_species_sys_string(
				application_name,
				post_dictionary );

/*
fprintf( stderr, "\ngot sys_string = (%s)\n", sys_string );
*/

	system( sys_string );

	return 0;
} /* main() */

char *get_species_sys_string(	char *application_name,
				DICTIONARY *post_dictionary )
{
	char sys_string[ 2048 ];
	char *benthic_species_table_name;
	char *select;
	char *one2m_folder_name = {0};
	char *species_where_clause;

	select =
"family_name, genus_name, concat( species_name, '|' , family_name ), genus_name, species_name, common_name";

	if ( post_dictionary )
	{
		one2m_folder_name =
			dictionary_get(
				post_dictionary,
				ONE2M_FOLDER_NAME_FOR_PROCESS );
	}

	if ( one2m_folder_name
	&&   strcmp(	one2m_folder_name,
			"species_measurement" ) == 0 )
	{
		species_where_clause =
			get_species_where_clause(
				application_name,
				post_dictionary );
	}
	else
	{
		species_where_clause =  "1 = 1";
	}

	benthic_species_table_name =
		get_table_name(	application_name,
				"benthic_species" );;

	sprintf( sys_string,
"echo \"select %s from %s where %s;\" | sql.e | sort",
		 select,
		 benthic_species_table_name,
		 species_where_clause );

	return strdup( sys_string );

} /* get_species_sys_string() */

boolean get_dictionary_variables(
			char **anchor_date,
			char **anchor_time,
			char **location,
			char **site_number_string,
			char **replicate_number_string,
			char **sweep_number_string,
			DICTIONARY *post_dictionary )
{
	if ( ! ( *anchor_date =
			dictionary_get_index_zero(
					post_dictionary,
					"anchor_date" ) ) )
	{
		return 0;
	}
	if ( ! ( *anchor_time =
			dictionary_get_index_zero(
					post_dictionary,
					"anchor_time" ) ) )
	{
		return 0;
	}
	if ( ! ( *location =
			dictionary_get_index_zero(
					post_dictionary,
					"location" ) ) )
	{
		return 0;
	}
	if ( ! ( *site_number_string =
			dictionary_get_index_zero(
					post_dictionary,
					"site_number" ) ) )
	{
		return 0;
	}
	if ( ! ( *replicate_number_string =
			dictionary_get_index_zero(
					post_dictionary,
					"replicate_number" ) ) )
	{
		return 0;
	}
	if ( ! ( *sweep_number_string =
			dictionary_get_index_zero(
					post_dictionary,
					"sweep_number" ) ) )
	{
		return 0;
	}

	return 1;
} /* get_dictionary_variables() */

LIST *get_species_record_list(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int replicate_number,
			int sweep_number )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *folder_name = "species_count";
	char *select_clause = "family_name, genus_name, species_name";

	sprintf( where_clause,
		 "anchor_date = '%s' and			"
		 "anchor_time = '%s' and			"
		 "location = '%s' and				"
		 "site_number = %d and				"
		 "replicate_number = %d and			"
		 "sweep_number = %d and				"
		 "count_per_square_meter >= 1			",
		 anchor_date,
		 anchor_time,
		 location,
		 site_number,
		 replicate_number,
		 sweep_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select_clause,
		 folder_name,
		 where_clause );

	return pipe2list( sys_string );

} /* get_species_record_list() */

char *get_species_where_clause(
			char *application_name,
			DICTIONARY *post_dictionary )
{
	char *anchor_date;
	char *anchor_time;
	char *location;
	char *site_number_string;
	char *replicate_number_string;
	char *sweep_number_string;
	LIST *species_record_list;
	char *species_record;
	LIST *attribute_name_list;
	LIST *query_drop_down_list;
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( !get_dictionary_variables(
			&anchor_date,
			&anchor_time,
			&location,
			&site_number_string,
			&replicate_number_string,
			&sweep_number_string,
			post_dictionary ) )
	{
		return "1 = 1";
	}

	species_record_list =
		get_species_record_list(
			application_name,
			anchor_date,
			anchor_time,
			location,
			atoi( site_number_string ),
			atoi( replicate_number_string ),
			atoi( sweep_number_string ) );

	if ( !list_rewind( species_record_list ) ) return "1 = 1";

	attribute_name_list = list_new();
	list_append_pointer( attribute_name_list, "family_name" );
	list_append_pointer( attribute_name_list, "genus_name" );
	list_append_pointer( attribute_name_list, "species_name" );

	query_drop_down = query_drop_down_new( "benthic_species" );
	query_drop_down_list = list_new();
	list_append_pointer( query_drop_down_list, query_drop_down );

	do {
		species_record = list_get( species_record_list );

		query_drop_down_row =
			query_drop_down_row_new(
				attribute_name_list,
				(LIST *)0 /* attribute_list */,
				list_string2list( species_record, '^' )
					/* data_list */ );

		list_append_pointer(
			query_drop_down->query_drop_down_row_list,
			query_drop_down_row );

	} while( list_next( species_record_list ) );

	return query_drop_down_where(
			query_drop_down_list,
			application_name,
			(char *)0 /* folder_name */ );
}

