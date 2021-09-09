/* ---------------------------------------------------	*/
/* src_benthic/select_spss_ready_counts.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "benthic_species.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
/* #define ISOLATE_PROBLEM		1 */

/* Prototypes */
/* ---------- */
char *get_order_group_clause(	char *application_name,
				char *select_string,
				char sum_sweeps_yn,
				boolean with_not_exists );

char *get_from_clause(
				char *application_name,
				boolean with_not_exists );

char *get_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char sum_sweeps_yn,
				boolean with_not_exists );

char *get_select_statement(
				char *application_name,
				char sum_sweeps_yn,
				boolean with_not_exists );

char *get_sql_statement(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char sum_sweeps_yn,
				boolean with_not_exists );

char *get_sys_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char sum_sweeps_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *collection_list_string;
	char *project_list_string;
	LIST *collection_list;
	LIST *project_list;
	char sum_sweeps_yn;
	char *sys_string;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application collection_list project_list sum_sweeps_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	collection_list_string = argv[ 2 ];
	project_list_string = argv[ 3 ];
	sum_sweeps_yn = *argv[ 4 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	collection_list = list_string2list( collection_list_string, ',' );
	project_list = list_string2list( project_list_string, ',' );

	if ( !*collection_list_string
	||   strcmp( collection_list_string, "collection_name" ) == 0 )
	{
		fprintf( stderr,
			 "Error %s: no collection on command line.\n",
			 argv[ 0 ] );
		exit( 0 );
	}

	sys_string = 
		get_sys_string(
				application_name,
				collection_list,
				project_list,
				sum_sweeps_yn );

	system( sys_string );

	return 0;
}

char *get_sys_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list,
			char sum_sweeps_yn )
{
	char sys_string[ 65536 ];

	sprintf( sys_string,
	"(echo \"%s\" | sql.e ',';echo \"%s\" | sql.e ',') | sort -u",
		 get_sql_statement(
			application_name,
			collection_list,
			project_list,
			sum_sweeps_yn,
			0 /* not with_not_exists */ ),
		 get_sql_statement(
			application_name,
			collection_list,
			project_list,
			sum_sweeps_yn,
			1 /* with_not_exists */ ) );

	return strdup( sys_string );

}

char *get_sql_statement(
			char *application_name,
			LIST *collection_list,
			LIST *project_list,
			char sum_sweeps_yn,
			boolean with_not_exists )
{
	char sql_statement[ 65536 ];
	char *select_string;

	select_string =
		get_select_statement(
			application_name,
			sum_sweeps_yn,
			with_not_exists );

	sprintf( sql_statement,
		 "select %s from %s where %s %s;",
		 select_string,
		 get_from_clause(
			application_name,
			with_not_exists ),
		 get_where_clause(
			application_name,
			collection_list,
			project_list,
			sum_sweeps_yn,
			with_not_exists ),
		 get_order_group_clause(
				application_name,
				select_string,
				sum_sweeps_yn,
				with_not_exists ) );

	return strdup( sql_statement );

}

char *get_select_statement(
			char *application_name,
			char sum_sweeps_yn,
			boolean with_not_exists )
{
	char *sampling_point_table_name;
	char *species_count_table_name;
	char *sweep_table_name;
	char select_statement[ 65536 ];

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	species_count_table_name =
		get_table_name( application_name, "species_count" );

	sweep_table_name =
		get_table_name( application_name, "sweep" );

	if ( !with_not_exists )
	{
		if ( sum_sweeps_yn != 'y' )
		{
			sprintf( select_statement,
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,lpad(%s.site_number,2,'0'),%s.sweep_number,%s,sum(count_per_square_meter)",
			 	sampling_point_table_name,
			 	sampling_point_table_name,
			 	sampling_point_table_name,
			 	species_count_table_name,
				benthic_species_get_scientific_name_expression(
					species_count_table_name ) );
		}
		else
		{
			sprintf( select_statement,
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,lpad(%s.site_number,2,'0'),%s,sum(count_per_square_meter)",
			 	sampling_point_table_name,
			 	sampling_point_table_name,
			 	sampling_point_table_name,
				benthic_species_get_scientific_name_expression(
					species_count_table_name ) );
		}
	}
	else
	{
		if ( sum_sweeps_yn != 'y' )
		{
			sprintf( select_statement,
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,lpad(%s.site_number,2,'0'),concat( %s.sweep_number, ',,' )",
			 	sampling_point_table_name,
			 	sampling_point_table_name,
			 	sampling_point_table_name,
			 	sweep_table_name );
		}
		else
		{
			sprintf( select_statement,
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,concat( lpad(%s.site_number,2,'0'), ',,' )",
			 	sampling_point_table_name,
			 	sampling_point_table_name,
			 	sampling_point_table_name );
		}
	}

	return strdup( select_statement );

}

char *get_where_clause(
			char *application_name,
			LIST *collection_list,
			LIST *project_list,
			char sum_sweeps_yn,
			boolean with_not_exists )
{
	char where_clause[ 65536 ];
	char where_or_clause[ 65536 ];
	char where_collection_join[ 256 ];
	char where_pink_assessment_area_join[ 256 ];
	char where_location_join[ 256 ];
	char where_region_join[ 256 ];
	char where_species_count_sweep[ 2048 ];
	char where_throwtrap_sampling_point[ 2048 ];
	char where_sweep_throwtrap[ 2048 ];
	char *sampling_point_table_name;
	char *species_count_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *sweep_table_name;
	char *throwtrap_replicate_table_name;
	char *pink_assessment_area_table_name;
	QUERY_OR_SEQUENCE *query_or_sequence;
	LIST *attribute_name_list;
	char attribute_name[ 128 ];

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	species_count_table_name =
		get_table_name( application_name, "species_count" );

	collection_table_name =
		get_table_name( application_name, "collection" );

	location_table_name =
		get_table_name( application_name, "location" );

	region_table_name =
		get_table_name( application_name, "region" );

	pink_assessment_area_table_name =
		get_table_name( application_name, "pink_assessment_area" );

	sweep_table_name =
		get_table_name( application_name, "sweep" );

	throwtrap_replicate_table_name =
		get_table_name( application_name, "throwtrap_replicate" );

	attribute_name_list = list_new();

	sprintf(	attribute_name,
			"%s.collection_name",
			sampling_point_table_name );
	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	sprintf(	attribute_name,
			"%s.project",
			sampling_point_table_name );
	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	query_or_sequence = query_or_sequence_new( attribute_name_list );

	query_or_sequence_set_data_list(
		query_or_sequence->data_list_list,
		collection_list );

	query_or_sequence_set_data_list(
		query_or_sequence->data_list_list,
		project_list );

	strcpy( where_or_clause,
		query_or_sequence_where_clause(
			query_or_sequence->attribute_name_list,
			query_or_sequence->data_list_list,
			0 /* not with_and_prefix */ ) );


	if ( sum_sweeps_yn != 'y' )
	{
		if ( with_not_exists )
		{
			sprintf( where_species_count_sweep,
			 "not exists ( select * from %s where		"
			 "%s.anchor_date = %s.anchor_date and		"
			 "%s.anchor_time = %s.anchor_time and		"
			 "%s.location = %s.location and			"
			 "%s.site_number = %s.site_number and		"
			 "%s.sweep_number = %s.sweep_number and		"
			 "%s.replicate_number = %s.replicate_number)	",
			 species_count_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name );
		}
		else
		{
			sprintf( where_species_count_sweep,
			 "%s.anchor_date = %s.anchor_date and		"
			 "%s.anchor_time = %s.anchor_time and		"
			 "%s.location = %s.location and			"
			 "%s.site_number = %s.site_number and		"
			 "%s.sweep_number = %s.sweep_number and		"
			 "%s.replicate_number = %s.replicate_number	",
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name );
		}
	}
	else
	/* ------------------------ */
	/* sum_sweeps_yn must = 'y' */
	/* ------------------------ */
	{
		if ( with_not_exists )
		{
			sprintf( where_species_count_sweep,
			 "not exists ( select * from %s where		"
			 "%s.anchor_date = %s.anchor_date and		"
			 "%s.anchor_time = %s.anchor_time and		"
			 "%s.location = %s.location and			"
			 "%s.site_number = %s.site_number and		"
			 "%s.replicate_number = %s.replicate_number )	",
			 species_count_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name );
		}
		else
		{
			sprintf( where_species_count_sweep,
			 "%s.anchor_date = %s.anchor_date and		"
			 "%s.anchor_time = %s.anchor_time and		"
			 "%s.location = %s.location and			"
			 "%s.site_number = %s.site_number and		"
			 "%s.sweep_number = %s.sweep_number and		"
			 "%s.replicate_number = %s.replicate_number	",
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name,
			 species_count_table_name,
			 sweep_table_name );
		}
	}

	sprintf( where_sweep_throwtrap,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number and			"
		 "%s.replicate_number = %s.replicate_number		",
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name );

	sprintf( where_throwtrap_sampling_point,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 sampling_point_table_name,
		 throwtrap_replicate_table_name,
		 sampling_point_table_name,
		 throwtrap_replicate_table_name,
		 sampling_point_table_name,
		 throwtrap_replicate_table_name,
		 sampling_point_table_name,
		 throwtrap_replicate_table_name );

	sprintf( where_collection_join,
		 "%s.collection_name = %s.collection_name and	"
		 "%s.project = %s.project 			",
		 sampling_point_table_name,
		 collection_table_name,
		 sampling_point_table_name,
		 collection_table_name );


	sprintf( where_location_join,
		 "%s.location = %s.location			",
		 sampling_point_table_name,
		 location_table_name );

	sprintf( where_region_join,
		 "%s.region = %s.region				",
		 location_table_name,
		 region_table_name );

	sprintf( where_pink_assessment_area_join,
		 "%s.pink_assessment_area = %s.pink_assessment_area	",
		 location_table_name,
		 pink_assessment_area_table_name );

#ifdef ISOLATE_PROBLEM
	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s and %s and sampling_point.anchor_date = '2005-05-09'",
		 where_or_clause,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_pink_assessment_area_join,
		 where_sweep_sampling_point,
		 where_species_count_sweep );
#else
	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s and %s and %s",
		 where_or_clause,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_pink_assessment_area_join,
		 where_throwtrap_sampling_point,
		 where_sweep_throwtrap,
		 where_species_count_sweep );

#endif

	return strdup( where_clause );

}

char *get_from_clause(
			char *application_name,
			boolean with_not_exists )
{
	char *sampling_point_table_name;
	char *species_count_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *sweep_table_name;
	char *throwtrap_replicate_table_name;
	char *pink_assessment_area_table_name;
	char from_clause[ 1024 ];

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	species_count_table_name =
		get_table_name( application_name, "species_count" );

	collection_table_name =
		get_table_name( application_name, "collection" );

	location_table_name =
		get_table_name( application_name, "location" );

	region_table_name =
		get_table_name( application_name, "region" );

	pink_assessment_area_table_name =
		get_table_name( application_name, "pink_assessment_area" );

	sweep_table_name =
		get_table_name( application_name, "sweep" );

	throwtrap_replicate_table_name =
		get_table_name( application_name, "throwtrap_replicate" );

	if ( !with_not_exists )
	{
		sprintf( from_clause,
			 "%s,%s,%s,%s,%s,%s,%s,%s",
			 sweep_table_name,
			 pink_assessment_area_table_name,
			 region_table_name,
			 location_table_name,
			 collection_table_name,
			 species_count_table_name,
			 sampling_point_table_name,
			 throwtrap_replicate_table_name );
	}
	else
	{
		sprintf( from_clause,
			 "%s,%s,%s,%s,%s,%s,%s",
			 sweep_table_name,
			 pink_assessment_area_table_name,
			 region_table_name,
			 location_table_name,
			 collection_table_name,
			 sampling_point_table_name,
			 throwtrap_replicate_table_name );
	}

	return strdup( from_clause );

}

char *get_order_group_clause(	char *application_name,
				char *select_string,
				char sum_sweeps_yn,
				boolean with_not_exists )
{
	char order_group_clause[ 512 ];
	char *sampling_point_table_name;
	char *species_count_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	species_count_table_name =
		get_table_name( application_name, "species_count" );

	if ( sum_sweeps_yn == 'y' && !with_not_exists )
	{
		sprintf( order_group_clause,
"group by lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,lpad(%s.site_number,2,'0'),%s",
		 	sampling_point_table_name,
		 	sampling_point_table_name,
		 	sampling_point_table_name,
			benthic_species_get_scientific_name_expression(
				species_count_table_name ) );
	}
	else
	if ( sum_sweeps_yn != 'y' && !with_not_exists )
	{
		sprintf( order_group_clause,
"group by lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,lpad(%s.site_number,2,'0'),%s.sweep_number,%s",
			 sampling_point_table_name,
			 sampling_point_table_name,
			 sampling_point_table_name,
			 species_count_table_name,
			 benthic_species_get_scientific_name_expression(
			 	species_count_table_name ) );
	}
	else
	{
		sprintf( order_group_clause,
			 "order by %s",
			 select_string );
	}

	return strdup( order_group_clause );

}

