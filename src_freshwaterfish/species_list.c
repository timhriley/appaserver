/* src_freshwaterfish/species_list.c			   */
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
#include "appaserver_parameter_file.h"

/* Prototypes */
/* ---------- */
boolean get_dictionary_variables(
			char **one2m_folder_name_for_process,
			DICTIONARY *post_dictionary );

char *get_vegetation_species_sys_string(
			char *application_name,
			char *species_category );

char *get_non_vegetation_species_sys_string(
			char *application_name );

char *get_species_sys_string(
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *sys_string = {0};
	char *dictionary_string;
	DICTIONARY *post_dictionary;
	char *one2m_folder_name_for_process = {0};

	output_starting_argv_stderr( argc, argv );

	if ( argc != 3 )
	{
		fprintf( stderr,
	"Usage: %s application dictionary\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	dictionary_string = argv[ 2 ];

	post_dictionary = 
		dictionary_string2dictionary( dictionary_string );

	get_dictionary_variables(
			&one2m_folder_name_for_process,
			post_dictionary );

	if ( one2m_folder_name_for_process )
	{
		if ( strcmp(	one2m_folder_name_for_process,
				"surface_vegetation" ) == 0 )
		{
			sys_string =
				get_vegetation_species_sys_string(
					application_name,
					"surface_vegetation" );
		}
		else
		if ( strcmp(	one2m_folder_name_for_process,
				"submergent_vegetation" ) == 0 )
		{
			sys_string =
				get_vegetation_species_sys_string(
					application_name,
					"submergent_vegetation" );
		}
		else
		if ( strcmp(	one2m_folder_name_for_process,
				"measurement" ) == 0 )
		{
			sys_string =
				get_non_vegetation_species_sys_string(
					application_name );
		}
		else
		{
			sys_string =
				get_species_sys_string(
					application_name );
		}
	}
	else
	{
		sys_string =
			get_species_sys_string(
				application_name );
	}

	system( sys_string );

	return 0;
}

boolean get_dictionary_variables(
			char **one2m_folder_name_for_process,
			DICTIONARY *post_dictionary )
{
	if ( ! ( *one2m_folder_name_for_process =
			dictionary_get(
				ONE2M_FOLDER_NAME_FOR_PROCESS,
				post_dictionary ) ) )
	{
		return 0;
	}

	return 1;
}


char *get_vegetation_species_sys_string(
				char *application_name,
				char *species_category )
{
	char sys_string[ 2048 ];
	char *species_table_name;
	char species_category_in_clause[ 128 ];
	char select[ 128 ];
	char from[ 128 ];
	char where[ 256 ];
	char order[ 256 ];

	sprintf( species_category_in_clause,
		 "in ('%s')",
		 species_category );

	species_table_name =
		get_table_name(	application_name,
				"species" );;

	sprintf( select,
		 "%s.scientific_name",
		 species_table_name );

	sprintf( from,
		 "%s",
		 species_table_name );

	sprintf( where,
		 "species_category %s",
		 species_category_in_clause );

	sprintf( order,
		 "order by display_order,%s.scientific_name",
		 species_table_name );

	sprintf( sys_string,
"echo \"select %s from %s where %s %s;\" | sql.e",
		 select,
		 from,
		 where,
		 order );

/* fprintf( stderr, "%s\n", sys_string ); */

	return strdup( sys_string );

}

char *get_non_vegetation_species_sys_string(
				char *application_name )
{
	char sys_string[ 2048 ];
	char *species_table_name;
	char select[ 128 ];
	char from[ 128 ];
	char order[ 256 ];

	species_table_name =
		get_table_name(	application_name,
				"species" );;

	sprintf( select,
		 "%s.scientific_name,%s.species_category",
		 species_table_name,
		 species_table_name );

	sprintf( from,
		 "%s",
		 species_table_name );

	sprintf( order,
		 "order by display_order,%s.scientific_name",
		 species_table_name );

	sprintf( sys_string,
"echo \"select %s from %s %s;\"					|"
"sql.e								|"
"grep -v '\\^vegetation'					|"
"piece.e '^' 0							|"
"cat								 ",
		 select,
		 from,
		 order );

/* fprintf( stderr, "%s\n", sys_string ); */

	return strdup( sys_string );

}

char *get_species_sys_string( char *application_name )
{
	char sys_string[ 2048 ];
	char *species_table_name;
	char select[ 128 ];
	char from[ 128 ];
	char order[ 256 ];

	species_table_name =
		get_table_name(	application_name,
				"species" );;

	sprintf( select,
		 "%s.scientific_name",
		 species_table_name );

	strcpy( from, species_table_name );

	sprintf( order,
		 "order by display_order,%s.scientific_name",
		 species_table_name );

	sprintf( sys_string,
"echo \"select %s from %s %s;\" | sql.e",
		 select,
		 from,
		 order );

/* fprintf( stderr, "%s\n", sys_string ); */

	return strdup( sys_string );

}

