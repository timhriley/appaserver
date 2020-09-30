/* $APPASERVER_HOME/src_sparrow/bird_count_group_by.c	   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "sparrow_library.h"

/* Constants */
/* --------- */
#define REASON_NO_OBSERVATIONS_WHERE	"reason_no_observations is null"

/* Structures */
/* ---------- */
typedef struct
{
	int site_visit_count;
	int bird_count;
	double per_visit_ratio;
} BIRD_COUNT_SUBPOPULATION;

typedef struct
{
	int year;
	int site_visit_count;
	int bird_count;
	double per_visit_ratio;
	BIRD_COUNT_SUBPOPULATION *
		subpopulation_array[
			BIRD_COUNT_SUBPOPULATION_SIZE ];
} BIRD_COUNT_YEAR;

typedef struct
{
	LIST *bird_count_year_list;
} BIRD_COUNT;

/* Prototypes */
/* ---------- */
BIRD_COUNT_YEAR *bird_count_year_get_or_set(
				LIST *bird_count_year_list,
				int year );

int bird_count_match_function(	BIRD_COUNT_YEAR *bird_count_year_from_list,
				BIRD_COUNT_YEAR *bird_count_year_compare );

LIST *bird_count_sort_by_per_visit(
				LIST *bird_count_year_list );

void bird_count_set_per_visit_ratio(
				LIST *bird_count_year_list );

void bird_count_year_list_display(
				LIST *bird_count_year_list );

/*
LIST *bird_count_get_year_list(	char *application_name );
*/

void bird_count_populate_subpopulation_array_bird_count(
				LIST *bird_count_year_list,
				char *application_name );

void bird_count_populate_subpopulation_array_site_visit(
				LIST *bird_count_year_list,
				char *application_name );

BIRD_COUNT_SUBPOPULATION *bird_count_subpopulation_new(
				void );

BIRD_COUNT_YEAR *bird_count_year_new(
				int year );

BIRD_COUNT *bird_count_new(
				char *application_name );

int bird_count_get_subpopulation_offset(
				char *subpopulation_string );

BIRD_COUNT_YEAR *bird_count_year_seek(
				LIST *bird_count_year_list,
				int year );

char *get_group_by_year_location_sys_string(
				char *application_name,
				char *quad_sheet,
				boolean sort_by_per_visit );

char *get_group_by_location_sys_string(
				char *application_name,
				boolean sort_by_per_visit );

void output_group_by_year(
				char *application_name,
				boolean sort_by_per_visit );

int main( int argc, char **argv )
{
	char *application_name;
	char *group_by;
	char *quad_sheet;
	boolean sort_by_per_visit;
	char *sys_string;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
	"Usage: %s group_by quad_sheet sort_by_per_visit_yn\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	group_by = argv[ 1 ];
	quad_sheet = argv[ 2 ];
	sort_by_per_visit = ( *argv[ 3 ] == 'y' );

	if ( *quad_sheet && strcmp( quad_sheet, "quad_sheet" ) != 0 )
	{
		sys_string =
			get_group_by_year_location_sys_string(
				application_name,
				quad_sheet,
				sort_by_per_visit );

		system( sys_string );
	}
	else
	if ( strcmp( group_by, "year" ) == 0 )
	{
		output_group_by_year(
				application_name,
				sort_by_per_visit );
	}
	else
	{
		sys_string =
			get_group_by_location_sys_string(
				application_name,
				sort_by_per_visit );

		system( sys_string );
	}

	return 0;

} /* main() */


char *get_group_by_location_sys_string(
				char *application_name,
				boolean sort_by_per_visit )
{
	char *sparrow_observation;
	char *site_visit;
	char *select;
	char *group_by_clause;
	char sys_string[ 1024 ];
	char *sys_ptr = sys_string;

	site_visit =
		get_table_name(
			application_name,
			"site_visit" );

	sparrow_observation =
		get_table_name(
			application_name,
			"sparrow_observation" );

	group_by_clause = "quad_sheet";

	select = "quad_sheet, 'site_visit', count(*)";
	sys_ptr +=
		sprintf(
		sys_ptr,
	 	"(echo \"select %s from %s where %s group by %s;\" |"
		"sql.e '^';",
	 	select,
	 	site_visit,
		REASON_NO_OBSERVATIONS_WHERE,
	 	group_by_clause );

	select = "quad_sheet, 'bird_count', sum(bird_count)";
	sys_ptr += sprintf(
			sys_ptr,
		 	"echo \"select %s from %s group by %s;\" | sql.e '^')",
		 	select,
		 	sparrow_observation,
		 	group_by_clause );

	sys_ptr += sprintf(
			sys_ptr,
			"| sort | pivot_table.e site_visit,bird_count '^'" );

	sys_ptr += sprintf(
			sys_ptr,
			"| piece_arithmetic.e 2 1 divide '^'" );

	if ( sort_by_per_visit )
	{
		sys_ptr += sprintf(
				sys_ptr,
				"| sort -k4 -n -r -t'^'" );
	}

	return strdup( sys_string );

} /* get_group_by_location_sys_string() */

char *get_group_by_year_location_sys_string(
				char *application_name,
				char *quad_sheet,
				boolean sort_by_per_visit )
{
	char *sparrow_observation;
	char *site_visit;
	char *select;
	char *group_by_clause;
	char *quad_sheet_where;
	char sys_string[ 1024 ];
	char *sys_ptr = sys_string;

	site_visit =
		get_table_name(
			application_name,
			"site_visit" );

	sparrow_observation =
		get_table_name(
			application_name,
			"sparrow_observation" );

	group_by_clause = "substr( visit_date, 1, 4 )";

	quad_sheet_where =
		sparrow_library_get_observation_where(
			quad_sheet,
			(char *)0 /* begin_visit_date */,
			(char *)0 /* end_visit_date */,
			site_visit );

	select = "substr( visit_date, 1, 4 ), 'site_visit', count(*)";

	sys_ptr += sprintf(
			sys_ptr,
	"(echo \"select %s from %s where %s and %s group by %s;\" | sql.e '^';",
		 	select,
		 	site_visit,
			quad_sheet_where,
			REASON_NO_OBSERVATIONS_WHERE,
		 	group_by_clause );

	quad_sheet_where =
		sparrow_library_get_observation_where(
			quad_sheet,
			(char *)0 /* begin_visit_date */,
			(char *)0 /* end_visit_date */,
			sparrow_observation );

	select =
		"substr( visit_date, 1, 4 ), 'bird_count', sum(bird_count)";

	sys_ptr += sprintf(
			sys_ptr,
		"echo \"select %s from %s where %s group by %s;\" | sql.e '^')",
		 	select,
		 	sparrow_observation,
			quad_sheet_where,
		 	group_by_clause );

	sys_ptr += sprintf(
			sys_ptr,
			"| sort | pivot_table.e site_visit,bird_count '^'" );

	sys_ptr += sprintf(
			sys_ptr,
			"| piece_arithmetic.e 2 1 divide '^'" );

	if ( sort_by_per_visit )
	{
		sys_ptr += sprintf(
				sys_ptr,
				"| sort -k4 -n -r -t'^'" );
	}

	return strdup( sys_string );

} /* get_group_by_year_location_sys_string() */

void output_group_by_year(	char *application_name,
				boolean sort_by_per_visit )
{
	BIRD_COUNT *bird_count;

	bird_count =
		bird_count_new(
			application_name );

	if ( !list_length( bird_count->bird_count_year_list ) )
		return;

	bird_count_set_per_visit_ratio(
		bird_count->bird_count_year_list );

	if ( sort_by_per_visit )
	{
		bird_count->bird_count_year_list =
			bird_count_sort_by_per_visit(
				bird_count->
					bird_count_year_list );
	}

	bird_count_year_list_display(
		bird_count->bird_count_year_list );

} /* output_group_by_year() */

BIRD_COUNT_SUBPOPULATION *bird_count_subpopulation_new( void )
{
	BIRD_COUNT_SUBPOPULATION *bird_count_subpopulation;

	if ( ! ( bird_count_subpopulation =
		(BIRD_COUNT_SUBPOPULATION *)
			calloc( 1, sizeof( BIRD_COUNT_SUBPOPULATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocation memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return bird_count_subpopulation;

} /* bird_count_subpopulation_new() */

BIRD_COUNT_YEAR *bird_count_year_new(
				int year )
{
	BIRD_COUNT_YEAR *bird_count_year;

	if ( ! ( bird_count_year =
		(BIRD_COUNT_YEAR *)
			calloc( 1, sizeof( BIRD_COUNT_YEAR ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocation memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	bird_count_year->year = year;

	return bird_count_year;

} /* bird_count_year_new() */

BIRD_COUNT *bird_count_new( char *application_name )
{
	BIRD_COUNT *bird_count;

	if ( ! ( bird_count =
		(BIRD_COUNT *)
			calloc( 1, sizeof( BIRD_COUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocation memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	bird_count->bird_count_year_list = list_new();

	bird_count_populate_subpopulation_array_site_visit(
		bird_count->bird_count_year_list,
		application_name );

	if ( list_length( bird_count->bird_count_year_list ) )
	{
		bird_count_populate_subpopulation_array_bird_count(
			bird_count->bird_count_year_list,
			application_name );
	}

	return bird_count;

} /* bird_count_new() */

int bird_count_get_subpopulation_offset(
				char *subpopulation_string )
{
	char subpopulation_character;

	subpopulation_character = toupper( *subpopulation_string );

	return ( subpopulation_character - 'A' );

} /* bird_count_get_subpopulation_offset() */

BIRD_COUNT_YEAR *bird_count_year_seek(
				LIST *bird_count_year_list,
				int year )
{
	BIRD_COUNT_YEAR *bird_count_year;

	if ( !list_rewind( bird_count_year_list ) )
		return (BIRD_COUNT_YEAR *)0;

	do {
		bird_count_year = list_get( bird_count_year_list );

		if ( bird_count_year->year == year )
			return bird_count_year;

	} while( list_next( bird_count_year_list ) );

	return (BIRD_COUNT_YEAR *)0;

} /* bird_count_year_seek() */

BIRD_COUNT_YEAR *bird_count_year_get_or_set(
				LIST *bird_count_year_list,
				int year )
{
	BIRD_COUNT_YEAR *bird_count_year;

	if ( ! ( bird_count_year =
			bird_count_year_seek(
				bird_count_year_list,
				year ) ) )
	{
		bird_count_year = bird_count_year_new( year );

		list_append_pointer(	bird_count_year_list,
					bird_count_year );
	}

	return bird_count_year;

} /* bird_count_year_get_or_set() */

#ifdef NOT_DEFINED
LIST *bird_count_get_year_list(	char *application_name )
{
	char *site_visit;
	char *select;
	char *group_by_clause;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 32 ];
	char year_string[ 16 ];
	char site_visit_count_string[ 16 ];
	LIST *bird_count_year_list = list_new();
	BIRD_COUNT_YEAR *bird_count_year;

	site_visit =
		get_table_name(
			application_name,
			"site_visit" );

	group_by_clause = "substr( visit_date, 1, 4 )";

	select = "substr( visit_date, 1, 4 ), count(*)";

	sprintf(sys_string,
		"echo \"select %s from %s where %s group by %s;\" | sql.e '%c'",
		 	select,
		 	site_visit,
			REASON_NO_OBSERVATIONS_WHERE,
		 	group_by_clause,
			FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	year_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	site_visit_count_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		bird_count_year =
			bird_count_year_new(
				atoi( year_string ),
				atoi( site_visit_count_string ) );

		list_append_pointer(
			bird_count_year_list,
			bird_count_year );
	}

	pclose( input_pipe );

	return bird_count_year_list;

} /* bird_count_get_year_list() */
#endif

void bird_count_populate_subpopulation_array_bird_count(
				LIST *bird_count_year_list,
				char *application_name )
{
	char *select;
	char join_where[ 256 ];
	char *group_by_clause;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 32 ];
	char year_string[ 16 ];
	char subpopulation_string[ 16 ];
	char bird_count_string[ 16 ];
	int bird_count;
	BIRD_COUNT_YEAR *bird_count_year;
	char *sparrow_observation;
	char *observation_site;
	int subpopulation_offset;
	BIRD_COUNT_SUBPOPULATION *bird_count_subpopulation;

	sparrow_observation =
		get_table_name(
			application_name,
			"sparrow_observation" );

	observation_site =
		get_table_name(
			application_name,
			"observation_site" );

	select =
"substr( visit_date, 1, 4 ), sub_population, sum( bird_count )";

	sprintf( join_where,
		 "%s.quad_sheet = %s.quad_sheet and	"
		 "%s.site_number = %s.site_number	",
		 observation_site,
		 sparrow_observation,
		 observation_site,
		 sparrow_observation );

	group_by_clause = "substr( visit_date, 1, 4 ), sub_population";

	sprintf(	sys_string,
	"echo \"select %s from %s,%s where %s group by %s;\" |"
	"sql.e '%c'",
		 	select,
		 	sparrow_observation,
			observation_site,
			join_where,
		 	group_by_clause,
			FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	year_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	subpopulation_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		subpopulation_offset =
			bird_count_get_subpopulation_offset(
				subpopulation_string );

		if ( subpopulation_offset < 0
		||   subpopulation_offset >= BIRD_COUNT_SUBPOPULATION_SIZE )
		{
			continue;
		}

		if ( ! ( bird_count_year =
				bird_count_year_seek(
					bird_count_year_list,
					atoi( year_string ) ) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot seek year=%d.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 atoi( year_string ) );

			pclose( input_pipe );

			return;
		}

		piece(	bird_count_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		bird_count = atoi( bird_count_string );

		if ( bird_count_year->
			subpopulation_array[ subpopulation_offset ] )
		{
			bird_count_subpopulation =
				bird_count_year->
					subpopulation_array[
						subpopulation_offset ];
		}
		else
		{
			bird_count_subpopulation =
				bird_count_subpopulation_new();

			bird_count_year->
				subpopulation_array[ subpopulation_offset ] =
					bird_count_subpopulation;
		}

		bird_count_subpopulation->bird_count = bird_count;
		bird_count_year->bird_count += bird_count;

		if ( bird_count_subpopulation->site_visit_count )
		{
			bird_count_subpopulation->per_visit_ratio =
				(double)bird_count /
				(double)bird_count_subpopulation->
						site_visit_count;
		}

		if ( bird_count_year->site_visit_count )
		{
			bird_count_year->per_visit_ratio =
				(double)bird_count_year->bird_count /
				(double)bird_count_year->
						site_visit_count;
		}
	}

	pclose( input_pipe );

} /* bird_count_populate_subpopulation_array_bird_count() */

void bird_count_populate_subpopulation_array_site_visit(
				LIST *bird_count_year_list,
				char *application_name )
{
	char *select;
	char join_where[ 256 ];
	char *group_by_clause;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 32 ];
	char year_string[ 16 ];
	char subpopulation_string[ 16 ];
	char site_visit_string[ 16 ];
	int site_visit_count;
	char *site_visit_table;
	char *observation_site_table;
	int subpopulation_offset;
	BIRD_COUNT_SUBPOPULATION *bird_count_subpopulation;
	BIRD_COUNT_YEAR *bird_count_year;

	site_visit_table =
		get_table_name(
			application_name,
			"site_visit" );

	observation_site_table =
		get_table_name(
			application_name,
			"observation_site" );

	select =
"substr( visit_date, 1, 4 ), sub_population, count(*)";

	sprintf( join_where,
		 "%s.quad_sheet = %s.quad_sheet and	"
		 "%s.site_number = %s.site_number	",
		 observation_site_table,
		 site_visit_table,
		 observation_site_table,
		 site_visit_table );

	group_by_clause = "substr( visit_date, 1, 4 ), sub_population";

	sprintf(	sys_string,
	"echo \"select %s from %s,%s where %s and %s group by %s;\" |"
	"sql.e '%c'",
		 	select,
		 	site_visit_table,
			observation_site_table,
			join_where,
			REASON_NO_OBSERVATIONS_WHERE,
		 	group_by_clause,
			FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	year_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	subpopulation_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		subpopulation_offset =
			bird_count_get_subpopulation_offset(
				subpopulation_string );

		if ( subpopulation_offset < 0
		||   subpopulation_offset >= BIRD_COUNT_SUBPOPULATION_SIZE )
		{
			continue;
		}

		bird_count_year =
			bird_count_year_get_or_set(
				bird_count_year_list,
				atoi( year_string ) );

		piece(	site_visit_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		site_visit_count = atoi( site_visit_string );

		if ( bird_count_year->
			subpopulation_array[ subpopulation_offset ] )
		{
			bird_count_subpopulation =
				bird_count_year->
					subpopulation_array[
						subpopulation_offset ];
		}
		else
		{
			bird_count_subpopulation =
				bird_count_subpopulation_new();

			bird_count_year->
				subpopulation_array[ subpopulation_offset ] =
					bird_count_subpopulation;

		}

		bird_count_subpopulation->site_visit_count = site_visit_count;

		bird_count_year->site_visit_count += site_visit_count;

/*
		if ( site_visit_count )
		{
			bird_count_subpopulation->per_visit_ratio =
				(double)bird_count_subpopulation->bird_count /
				(double)site_visit_count;
		}
*/
	}

	pclose( input_pipe );

} /* bird_count_populate_subpopulation_array_site_visit() */

void bird_count_year_list_display(
				LIST *bird_count_year_list )
{
	BIRD_COUNT_YEAR *bird_count_year;
	int offset;

	if ( !list_rewind( bird_count_year_list ) ) return;

	do {
		bird_count_year = list_get( bird_count_year_list );

		printf( "%d",
			bird_count_year->year );

		for(	offset = 0;
			offset < BIRD_COUNT_SUBPOPULATION_SIZE;
			offset++ )
		{
			if ( bird_count_year->
				subpopulation_array[
					offset ] )
			{
				printf( "^%d^%d^%.5lf",
					bird_count_year->
						subpopulation_array[ offset ]->
						site_visit_count,
					bird_count_year->
						subpopulation_array[ offset ]->
						bird_count,
					bird_count_year->
						subpopulation_array[ offset ]->
						per_visit_ratio );
			}
			else
			{
				printf( "^0^0^0.00000" );
			}
		}

		printf( "^%d^%d^%.5lf\n",
			bird_count_year->site_visit_count,
			bird_count_year->bird_count,
			bird_count_year->per_visit_ratio );

	} while( list_next( bird_count_year_list ) );

} /* bird_count_year_list_display() */

void bird_count_set_per_visit_ratio(
				LIST *bird_count_year_list )
{
	BIRD_COUNT_YEAR *bird_count_year;

	if ( !list_rewind( bird_count_year_list ) ) return;

	do {
		bird_count_year = list_get( bird_count_year_list );

		if ( bird_count_year->site_visit_count )
		{
			bird_count_year->per_visit_ratio =
				(double)bird_count_year->bird_count /
				(double)bird_count_year->site_visit_count;
		}

	} while( list_next( bird_count_year_list ) );

} /* bird_count_set_per_visit_ratio() */

int bird_count_match_function(	BIRD_COUNT_YEAR *bird_count_year_from_list,
				BIRD_COUNT_YEAR *bird_count_year_compare )
{
	if (	bird_count_year_compare->per_visit_ratio <=
		bird_count_year_from_list->per_visit_ratio )
	{
		return -1;
	}
	else
	{
		return 1;
	}

} /* bird_count_match_function() */

LIST *bird_count_sort_by_per_visit(
				LIST *bird_count_year_list )
{
	BIRD_COUNT_YEAR *bird_count_year;
	LIST *sorted_list = list_new();

	if ( !list_rewind( bird_count_year_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty bird_count_year_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		bird_count_year = list_get( bird_count_year_list );

		list_add_pointer_in_order(
				sorted_list, 
				bird_count_year, 
				bird_count_match_function );

	} while( list_next( bird_count_year_list ) );

	return sorted_list;

} /* bird_count_sort_by_per_visit() */

