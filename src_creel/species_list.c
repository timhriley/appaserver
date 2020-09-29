/* src_creel/species_list.c				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "additional_drop_down_attribute.h"
#include "appaserver_library.h"
#include "dictionary.h"
#include "folder.h"
#include "timlib.h"
#include "query.h"
#include "piece.h"
#include "attribute.h"
#include "appaserver_parameter_file.h"

/* Prototypes */
/* ---------- */
LIST *get_species_record_list(
			char *application_name,
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			char *interview_number );

boolean get_dictionary_variables(
			char **fishing_purpose,
			char **census_date,
			char **interview_location,
			char **interview_number,
			DICTIONARY *post_dictionary );

void get_catches_species_in_clause(
			char *catches_species_in_clause,
			char *application_name,
			DICTIONARY *post_dictionary );

char *get_species_sys_string(
			char *application_name,
			char *fishing_purpose,
			DICTIONARY *post_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *fishing_purpose;
	char *sys_string;
	char *folder_name;
	DICTIONARY *post_dictionary = {0};

	output_starting_argv_stderr( argc, argv );

	if ( argc < 4 )
	{
		fprintf( stderr,
	"Usage: %s application fishing_purpose folder_name [dictionary]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	fishing_purpose = argv[ 2 ];
	folder_name = argv[ 3 ];

	if ( argc == 5 && strcmp( argv[ 4 ], "$dictionary" ) != 0 )
	{
		post_dictionary = 
			dictionary_string2dictionary( argv[ 4 ] );
	}

	if ( strcmp( folder_name, "species" ) == 0 )
	{
		sys_string = get_species_sys_string(
					application_name,
					fishing_purpose,
					post_dictionary );
	}
	else
	{
		fprintf(	stderr,
				"Error %s/%s()/%d: invalid folder_name= (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );
		exit( 1 );
	}

/* fprintf( stderr, "\ngot sys_string = (%s)\n", sys_string ); */

	system( sys_string );

	return 0;
}

char *get_species_sys_string(	char *application_name,
				char *fishing_purpose,
				DICTIONARY *post_dictionary )
{
	char sys_string[ 2048 ];
	char *species_table_name;
	ADDITIONAL_DROP_DOWN_ATTRIBUTE *additional_drop_down_attribute;
	char select_clause[ 1024 ];
	char *constant_select = "family,genus,species";
	char *first_attribute_name = "";
	char sort_clause[ 128 ];
	char *one2m_folder_name = {0};
	char catches_species_in_clause[ 1024 ];

	if ( dictionary_length( post_dictionary ) )
	{
		one2m_folder_name =
			dictionary_get(
				post_dictionary,
				ONE2M_FOLDER_NAME_FOR_PROCESS );
	}

	if ( timlib_strcmp(	one2m_folder_name,
				"catch_measurements" ) == 0 )
	{
		get_catches_species_in_clause(
			catches_species_in_clause,
			application_name,
			post_dictionary );
	}
	else
	{
		strcpy( catches_species_in_clause, "1 = 1" );
	}

	additional_drop_down_attribute =
		additional_drop_down_attribute_new(
			application_name,
			fishing_purpose,
			"species" );

	additional_drop_down_attribute_get_select_clause(
			select_clause,
			&first_attribute_name,
			additional_drop_down_attribute->
				application_name,
			constant_select,
			additional_drop_down_attribute->
				drop_down_folder_name,
			additional_drop_down_attribute );

	if ( strcmp( first_attribute_name, "florida_state_code" ) == 0 )
		strcpy( sort_clause, "sort -t'|' -k2 -n" );
	else
		strcpy( sort_clause, "sort" );

	species_table_name =
		get_table_name(	application_name,
				"species" );;

	sprintf( sys_string,
"echo \"select %s from %s where %s;\" | sql.e | sed 's/\\^|\\^/|/' | %s",
		 select_clause,
		 species_table_name,
		 catches_species_in_clause,
		 sort_clause );

	return strdup( sys_string );
}

boolean get_dictionary_variables(
			char **fishing_purpose,
			char **census_date,
			char **interview_location,
			char **interview_number,
			DICTIONARY *post_dictionary )
{
	char *primary_data_list_string;

	/* If from detail screen */
	/* --------------------- */
	if ( ( primary_data_list_string =
		dictionary_get(
			post_dictionary,
			PRIMARY_DATA_LIST_KEY ) ) )
	{
		char piece_buffer[ 128 ];

		if ( character_count(	FOLDER_DATA_DELIMITER,
					primary_data_list_string ) != 3 )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting 3 delimiters in (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 primary_data_list_string );
			exit( 1 );
		}

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			primary_data_list_string,
			0 );
		*fishing_purpose = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			primary_data_list_string,
			1 );
		*census_date = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			primary_data_list_string,
			2 );
		*interview_location = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			primary_data_list_string,
			3 );
		*interview_number = strdup( piece_buffer );

		return 1;

	} /* if from detail screen */

	if ( ! ( *fishing_purpose =
			dictionary_get(	post_dictionary,
					"fishing_purpose_0" ) ) )
	{
		return 0;
	}
	if ( ! ( *census_date =
			dictionary_get(	post_dictionary,
					"census_date_0" ) ) )
	{
		return 0;
	}
	if ( ! ( *interview_location =
			dictionary_get(	post_dictionary,
					"interview_location_0" ) ) )
	{
		return 0;
	}
	if ( ! ( *interview_number =
			dictionary_get(	post_dictionary,
					"interview_number_0" ) ) )
	{
		return 0;
	}

	return 1;
}

LIST *get_species_record_list(
			char *application_name,
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			char *interview_number )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *folder_name = "catches";
	char *select_clause = "family,genus,species";

	sprintf( where_clause,
		 "fishing_purpose = '%s' and			"
		 "census_date = '%s' and			"
		 "interview_location = '%s' and			"
		 "interview_number = %s and			"
		 "kept_count >= 1				",
		 fishing_purpose,
		 census_date,
		 interview_location,
		 interview_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select_clause,
		 folder_name,
		 where_clause );

	return pipe2list( sys_string );
}

void get_catches_species_in_clause(
			char *catches_species_in_clause,
			char *application_name,
			DICTIONARY *post_dictionary )
{
	char *fishing_purpose;
	char *census_date;
	char *interview_location;
	char *interview_number;
	LIST *species_record_list;
	LIST *data_list;
	char piece_buffer[ 128 ];
	char attribute_name[ 128 ];
	char *record;
	char *table_name;
	QUERY_OR_SEQUENCE *query_or_sequence;
	LIST *attribute_name_list;

	strcpy( catches_species_in_clause, "1 = 1" );

	if ( !get_dictionary_variables(
			&fishing_purpose,
			&census_date,
			&interview_location,
			&interview_number,
			post_dictionary ) )
	{
		return;
	}

	species_record_list =
		get_species_record_list(
			application_name,
			fishing_purpose,
			census_date,
			interview_location,
			interview_number );

	if ( !species_record_list
	||   !list_length( species_record_list ) )
	{
		strcpy( catches_species_in_clause, "1 = 2" );
		return;
	}

	table_name = get_table_name( application_name, "species" );
	attribute_name_list = list_new();

	sprintf( attribute_name, "%s.family", table_name );
	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	sprintf( attribute_name, "%s.genus", table_name );
	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	sprintf( attribute_name, "%s.species", table_name );
	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	query_or_sequence = query_or_sequence_new( attribute_name_list );

	/* Set family */
	/* ---------- */
	list_rewind( species_record_list );
	data_list = list_new();

	do {
		record = list_get_pointer( species_record_list );
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		list_append_pointer( data_list, strdup( piece_buffer ) );

	} while( list_next( species_record_list ) );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			data_list );

	/* Set genus */
	/* ---------- */
	list_rewind( species_record_list );
	data_list = list_new();

	do {
		record = list_get_pointer( species_record_list );
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		list_append_pointer( data_list, strdup( piece_buffer ) );

	} while( list_next( species_record_list ) );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			data_list );

	/* Set species */
	/* ----------- */
	list_rewind( species_record_list );
	data_list = list_new();

	do {
		record = list_get_pointer( species_record_list );
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			record,
			2 );

		list_append_pointer( data_list, strdup( piece_buffer ) );

	} while( list_next( species_record_list ) );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			data_list );

	strcpy( catches_species_in_clause,
		query_or_sequence_where_clause(
				query_or_sequence->attribute_name_list,
				query_or_sequence->data_list_list,
				0 /* not with_and_prefix */ ) );
}

