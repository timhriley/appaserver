/* src_wadingbird/wadingbird_library.c		*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "date_convert.h"
#include "date.h"
#include "wadingbird_library.h"

void wadingbird_get_species(	char **common_name,
				char *application_name,
				int species_code )
{
	static LIST *species_record_list = {0};
	static char local_common_name[ 64 ];
	char *record;
	char search_species_code[ 16 ];
	int str_len;

	if ( !species_record_list )
	{
		char sys_string[ 1024 ];
		char *species_select =
			"species_code,common_name";
		char *where = "species_code is not null";

		sprintf( sys_string,
		 	"get_folder_data	application=%s		"
		 	"			select=%s		"
		 	"			folder=species		"
		 	"			where=\"%s\"		",
		 	application_name,
		 	species_select,
			where );

		species_record_list = pipe2list( sys_string );
		if ( !list_length( species_record_list ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot get species_record_list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	sprintf(	search_species_code,
			"%d%c",
			species_code,
			FOLDER_DATA_DELIMITER );
	str_len = strlen( search_species_code );

	list_rewind( species_record_list );

	do {
		record = list_get_pointer( species_record_list );

		if ( strncmp( record, search_species_code, str_len ) == 0 )
		{
			piece(	local_common_name,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			*common_name = local_common_name;
			return;
		}

	} while( list_next( species_record_list ) );

	fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot get species for code = %d.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				species_code );
	exit( 1 );
} /* wadingbird_get_species() */

char *wadingbird_get_initial_takeoff_date_international(
				char *initial_takeoff_date_american )
{
	DATE_CONVERT *date_convert;
	static char return_date[ 16 ];

	date_convert =
		date_convert_new_date_convert(
				international,
				initial_takeoff_date_american );

	strcpy( return_date, date_convert->return_date );
	date_convert_free( date_convert );
	return return_date;
} /* wadingbird_get_initial_takeoff_date_international() */

boolean wadingbird_get_hydropattern(
				char **hydropattern,
				char *hydropattern_numeric_code,
				char *application_name )
{
	static DICTIONARY *code_dictionary = {0};
	char *results;

	if ( !code_dictionary )
	{
		code_dictionary =
			wadingbird_get_hydropattern_code_dictionary(
					application_name );
	}

	if ( ! (results =
		dictionary_get_pointer( code_dictionary,
					hydropattern_numeric_code ) ) )
	{
		return 0;
	}

	*hydropattern = results;
	return 1;
} /* wadingbird_get_hydropattern() */

DICTIONARY *wadingbird_get_hydropattern_code_dictionary(
			char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "hydropattern_numeric_code,hydropattern";

	sprintf(sys_string,
		"get_folder_data	application=%s		 "
		"			select=%s		 "
		"			folder=hydropattern	 ",
		application_name,
		select );

	return pipe2dictionary(	sys_string, FOLDER_DATA_DELIMITER );
} /* wadingbird_get_hydropattern_code_dictionary() */

SRF_CENSUS *wadingbird_srf_census_new(
				char *application_name,
				char *input_filename )
{
	SRF_CENSUS *srf_census;

	if ( ! ( srf_census = calloc( 1, sizeof( SRF_CENSUS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	srf_census->initial_takeoff_date =
		wadingbird_get_tablet_initial_takeoff_date_international(
				input_filename );

	srf_census->transect_list =
		wadingbird_get_transect_list(
			application_name,
			input_filename );

	return srf_census;
} /* wadingbird_srf_census_new() */

LIST *wadingbird_get_transect_list(
				char *application_name,
				char *input_filename )
{
	LIST *transect_list;
	char sys_string[ 1024 ];
	char *input_sys_string;
	FILE *input_pipe;
	char transect_string[ 256 ];
	int transect_number;
	TRANSECT *transect;

	input_sys_string =
		wadingbird_get_tablet_input_sys_string(
			input_filename );

	sprintf(sys_string,
		"%s | piece.e ',' %d | sort -u",
		input_sys_string,
		TABLET_TRANSECT_PIECE );

	input_pipe = popen( sys_string, "r" );
	transect_list = list_new();

	while( get_line( transect_string, input_pipe ) )
	{
		transect_number = atoi( transect_string );

		if ( transect_number )
		{
			transect = wadingbird_transect_new(
						application_name,
						transect_number,
						input_filename );
			list_append_pointer(
				transect_list,
				transect );
		}
	}
	pclose( input_pipe );
	return transect_list;
} /* wadingbird_get_transect_list() */

TRANSECT *wadingbird_transect_new(
				char *application_name,
				int transect_number,
				char *input_filename )
{
	TRANSECT *transect;

	if ( ! ( transect = calloc( 1, sizeof( TRANSECT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transect->transect_number = transect_number;

	if ( wadingbird_exists_researcher( input_filename, "L" ) )
	{
		transect->left_cell_list =
			wadingbird_given_transect_get_cell_list(
				application_name,
				transect->transect_number,
				!(transect->transect_number % 2) );
	}

	if ( wadingbird_exists_researcher( input_filename, "R" ) )
	{
		transect->right_cell_list =
			wadingbird_given_transect_get_cell_list(
				application_name,
				transect->transect_number,
				!(transect->transect_number % 2) );
	}

	return transect;
} /* wadingbird_transect_new() */

boolean wadingbird_exists_researcher(
				char *input_filename,
				char *left_right_code )
{
	FILE *input_pipe;
	char *input_sys_string;
	char sys_string[ 1024 ];
	char input_line[ 128 ];
	boolean exists_researcher;

	input_sys_string =
		wadingbird_get_tablet_input_sys_string(
			input_filename );

	sprintf(sys_string,
		"%s | piece.e ',' %d | sort -u | grep '%s'",
		input_sys_string,
		TABLET_LEFT_RIGHT_CODE_PIECE,
		left_right_code );

	input_pipe = popen( sys_string, "r" );

	exists_researcher = get_line( input_line, input_pipe );
	pclose( input_pipe );
	return exists_researcher;
} /* wadingbird_exists_researcher() */

LIST *wadingbird_given_transect_get_cell_list(
				char *application_name,
				int transect_number,
				boolean cells_increase )
{
	LIST *cell_list;
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *order;
	SRF_CELL *cell;
	char cell_number_string[ 128 ];
	FILE *input_pipe;

	sprintf( where, "transect_number = %d", transect_number );

	if ( cells_increase )
		order = "cell_number";
	else
		order = "cell_number desc";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=cell_number	"
		 "			folder=cell		"
		 "			where=\"%s\"		"
		 "			order=\"%s\"		",
		 application_name,
		 where,
		 order );

	input_pipe = popen( sys_string, "r" );
	cell_list = list_new();

	while( get_line( cell_number_string, input_pipe ) )
	{
		cell = wadingbird_srf_cell_new( atoi( cell_number_string ) );
		list_append_pointer( cell_list, cell );
	}
	pclose( input_pipe );
	return cell_list;
} /* wadingbird_given_transect_get_cell_list() */

LIST *wadingbird_get_cell_list( char *application_name )
{
	LIST *cell_list;
	char sys_string[ 1024 ];
	WADINGBIRD_CELL *cell;
	char input_buffer[ 256 ];
	char cell_number_string[ 128 ];
	char transect_number_string[ 128 ];
	FILE *input_pipe;
	char *select;

	select = "cell_number,transect_number";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=cell		",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );
	cell_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	cell_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		cell = wadingbird_cell_new( atoi( cell_number_string ) );

		piece(	transect_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		cell->transect_number = atoi( transect_number_string );

		list_append_pointer( cell_list, cell );
	}
	pclose( input_pipe );
	return cell_list;
} /* wadingbird_get_cell_list() */

SRF_CELL *wadingbird_srf_cell_new( int cell_number )
{
	SRF_CELL *cell;

	if ( ! ( cell = calloc( 1, sizeof( SRF_CELL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory alloction error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	cell->cell_number = cell_number;
	return cell;
} /* wadingbird_srf_cell_new() */

WADINGBIRD_CELL *wadingbird_cell_new( int cell_number )
{
	WADINGBIRD_CELL *cell;

	if ( ! ( cell = calloc( 1, sizeof( WADINGBIRD_CELL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory alloction error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	cell->cell_number = cell_number;
	return cell;
} /* wadingbird_cell_new() */

FILE *wadingbird_get_tablet_input_pipe( char *input_filename )
{
	char *sys_string;
	sys_string = wadingbird_get_tablet_input_sys_string( input_filename );
	return popen( sys_string, "r" );
} /* wadingbird_get_tablet_input_pipe() */

char *wadingbird_get_tablet_input_sys_string(
			char *input_filename )
{
	static char sys_string[ 1024 ];
	FILE *input_file;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}
	fclose( input_file );

/*
	sprintf(sys_string,
		"cat %s | grep -v '^F' | sort -n",
		input_filename );
*/
	sprintf(sys_string,
		"cat %s | grep -v '^F'",
		input_filename );
	return sys_string;
} /* wadingbird_get_tablet_input_sys_string() */

char *wadingbird_get_tablet_initial_takeoff_date_international(
				char *input_filename )
{
	char *initial_takeoff_date_international = {0};
	FILE *input_pipe;
	char input_string[ 1024 ];
	char initial_takeoff_date_american[ 128 ];

	input_pipe = wadingbird_get_tablet_input_pipe( input_filename );
	*initial_takeoff_date_american = '\0';

	while( get_line( input_string, input_pipe ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !*initial_takeoff_date_american )
		{
			if ( !piece(	initial_takeoff_date_american,
					',',
					input_string,
					TABLET_INITIAL_TAKEOFF_DATE_PIECE ) )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: cannot get takeoff date at piece = %d in (%s)\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					TABLET_INITIAL_TAKEOFF_DATE_PIECE,
					input_string );
				pclose( input_pipe );
				exit( 1 );
			}
	
			initial_takeoff_date_international =
			     wadingbird_get_initial_takeoff_date_international(
					initial_takeoff_date_american );

			/* ------------------------------------------------ */
			/* These generate the following error for each row: */
			/* grep: writing output: Broken pipe		    */
			/* So, let's just read every line.		    */
			/* ------------------------------------------------ */
			/* break; */
			/* goto all_done; */
		} /* if */
	} /* while */

/* all_done: */

	pclose( input_pipe );
	return initial_takeoff_date_international;

} /* wadingbird_get_tablet_initial_takeoff_date_international() */

SRF_CELL *wadingbird_get_cell(	LIST *transect_list,
				int cell_number,
				char *left_right )
{
	TRANSECT *transect;
	SRF_CELL *cell = {0};

	if ( !list_rewind( transect_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty transect_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		transect = list_get_pointer( transect_list );

		if ( strcmp( left_right, "left" ) == 0 )
		{
			if ( !list_rewind( transect->left_cell_list ) )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: empty left_cell_list.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}
			do {
				cell = list_get_pointer(
						transect->left_cell_list );
				if ( cell->cell_number == cell_number )
					return cell;
			} while( list_next( transect->left_cell_list ) );
		}
		else
		if ( strcmp( left_right, "right" ) == 0 )
		{
			if ( !list_rewind( transect->right_cell_list ) )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: empty right_cell_list.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}
			do {
				cell = list_get_pointer(
						transect->right_cell_list );
				if ( cell->cell_number == cell_number )
					return cell;
			} while( list_next( transect->right_cell_list ) );
		}
		else
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid left/right = (%s).\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				left_right );
			exit( 1 );
		}

	} while( list_next( transect_list ) );
	return cell;
} /* wadingbird_get_cell() */

HYDROPATTERN_MERGE *wadingbird_hydropattern_merge_new( void )
{
	HYDROPATTERN_MERGE *hydropattern_merge;

	if ( ! ( hydropattern_merge =
			calloc( 1, sizeof( HYDROPATTERN_MERGE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return hydropattern_merge;
} /* wadingbird_hydropattern_merge_new() */

char *wadingbird_get_preferred_hydropattern_seat(
			char *application_name,
			char *initial_takeoff_date )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *folder_name;
	char *select;

	sprintf( where, "initial_takeoff_date = '%s'", initial_takeoff_date );
	select = "preferred_hydropattern_seat";
	folder_name = "systematic_reconnaissance_flight";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_name,
		 where );

	return pipe2string( sys_string );

} /* wadingbird_get_preferred_hydropatten_seat() */

#ifdef NOT_DEFINED
boolean wadingbird_exists_preferred(
				LIST *preferred_cell_hydropattern_list,
				char *left_right,
				int cell_number )
{
	PREFERRED_CELL_HYDROPATTERN *preferred_cell_hydropattern;

	if ( !list_rewind( preferred_cell_hydropattern_list ) )
		return 0;
/*
	{
		fprintf(stderr,
		"ERROR in %s/%s()%d: empty preferred_cell_hydropattern_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
*/

	do {
		preferred_cell_hydropattern =
			list_get_pointer( 
				preferred_cell_hydropattern_list );

		if (	preferred_cell_hydropattern->cell_number ==
			cell_number )
		{
			if ( strcmp(	preferred_cell_hydropattern->left_right,
					left_right ) == 0 )
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	} while( list_next( preferred_cell_hydropattern_list ) );
	return 0;
} /* wadingbird_exists_preferred() */
#endif

LIST *wadingbird_get_hydropattern_merge_cell_list(
			char *application_name,
			char *initial_takeoff_date,
			char *preferred_hydropattern_seat )
{
	LIST *cell_list;
	char sys_string[ 1024 ];
	char where[ 256 ];
	SRF_CELL *cell;
	char input_buffer[ 1024 ];
	char cell_number_string[ 128 ];
	char buffer[ 128 ];
	FILE *input_pipe;
	char select[ 256 ];
	char *cell_hydropattern_table_name;
	char *cell_table_name;
	char *hydropattern_table_name;

	cell_hydropattern_table_name =
		get_table_name( application_name, "cell_hydropattern" );

	cell_table_name =
		get_table_name( application_name, "cell" );

	hydropattern_table_name =
		get_table_name( application_name, "hydropattern" );

	sprintf( select,
"%s.cell_number,left_right,%s.hydropattern,%s.utm_easting,%s.utm_northing,hydropattern_numeric_code",
		 cell_hydropattern_table_name,
		 cell_hydropattern_table_name,
		 cell_table_name,
		 cell_table_name );

	sprintf(where,
		"initial_takeoff_date = '%s'"
		" and %s.cell_number = %s.cell_number"
		" and %s.hydropattern = %s.hydropattern"
		" and %s.left_right = '%s'",
		initial_takeoff_date,
		cell_hydropattern_table_name,
		cell_table_name,
		cell_hydropattern_table_name,
		hydropattern_table_name,
		cell_hydropattern_table_name,
		preferred_hydropattern_seat );

	sprintf( sys_string,
		 "echo \"select %s				 "
		 "	 from %s,%s,%s				 "
		 "	 where %s				 "
		 "	 order by %s.cell_number;\"		|"
		 "sql.e						 ",
		 select,
		 cell_hydropattern_table_name,
		 cell_table_name,
		 hydropattern_table_name,
		 where,
		 cell_hydropattern_table_name );

	input_pipe = popen( sys_string, "r" );
	cell_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	cell_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		cell = wadingbird_srf_cell_new(
				atoi( cell_number_string ) );

		piece(	buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		cell->left_right = strdup( buffer );

		piece(	buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
		cell->hydropattern = strdup( buffer );

		piece(	buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );
		cell->utm_easting = atoi( buffer );

		piece(	buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );
		cell->utm_northing = atoi( buffer );

		piece(	buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );
		cell->hydropattern_numeric_code = atoi( buffer );

		list_append_pointer( cell_list, cell );
	}
	pclose( input_pipe );
	return cell_list;
} /* wadingbird_get_hydropattern_merge_cell_list() */

HYDROPATTERN_MERGE *wadingbird_get_hydropattern_merge(
				char *application_name,
				char *initial_takeoff_date )
{
	HYDROPATTERN_MERGE *hydropattern_merge;

	hydropattern_merge = wadingbird_hydropattern_merge_new();

	hydropattern_merge->preferred_hydropattern_seat =
		wadingbird_get_preferred_hydropattern_seat(
			application_name,
			initial_takeoff_date );

	hydropattern_merge->cell_list =
		wadingbird_get_hydropattern_merge_cell_list(
			application_name,
			initial_takeoff_date,
			hydropattern_merge->preferred_hydropattern_seat );

	return hydropattern_merge;
} /* wadingbird_get_hydropattern_merge() */

SRF_CELL *wadingbird_merge_seek_cell(
				LIST *cell_list,
				int cell_number )
{
	SRF_CELL *cell;

	if ( !list_rewind( cell_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty cell_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		cell = list_get_pointer( cell_list );
		if ( cell->cell_number == cell_number )
		{
			return cell;
		}
	} while( list_next( cell_list ) );
	return (SRF_CELL *)0;
} /* wadingbird_merge_seek_cell() */

int wadingbird_get_bird_estimation(
				char *application_name,
				int cell_number,
				int bird_count,
				LIST *transects_with_one_observer_list )
{
	int transect_number;
	char transect_number_string[ 16 ];

	if ( ! ( transect_number =
		wadingbird_given_cell_number_get_transect_number(
				application_name,
				cell_number ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: cannot get transect_number for cell_number = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			cell_number );
		exit( 1 );
	}

	sprintf( transect_number_string, "%d", transect_number );

	if ( list_exists_string(
		transaction_number_string,
		transects_with_one_observer_list ) )
	{
		return (int)( (double)bird_count / 0.075 );
	}
	else
	{
		return (int)( (double)bird_count / 0.15 );
	}
} /* wadingbird_get_bird_estimation() */

int wadingbird_given_cell_number_get_transect_number(
				char *application_name,
				int cell_number )
{
	static LIST *cell_list = {0};
	WADINGBIRD_CELL *cell;

	if ( !cell_list )
	{
		cell_list =
			wadingbird_get_cell_list(
				application_name );
	}

	if ( !list_rewind( cell_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty cell_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		cell = list_get_pointer( cell_list );
		if ( cell->cell_number == cell_number )
			return cell->transect_number;
	} while( list_next( cell_list ) );

	return 0;
} /* wadingbird_given_cell_number_get_transect_number() */

boolean wadingbird_exists_cell_hydropattern_for_seat(
				char *application_name,
				char *initial_takeoff_date,
				char *preferred_seat_position )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf(	where,
			"initial_takeoff_date = '%s'"
			" and left_right = '%s'",
			initial_takeoff_date,
			preferred_seat_position );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=cell_hydropattern	"
		 "			where=\"%s\"			",
		 application_name,
		 where );
	return atoi( pipe2string( sys_string ) );
} /* wadingbird_exists_cell_hydropattern_for_seat() */

int wadingbird_get_max_bird_count(
				LIST *cell_list )
{
	SRF_CELL *cell;
	int max_bird_count = -1;

	if ( !list_rewind( cell_list ) ) return max_bird_count;

	do {
		cell = list_get_pointer( cell_list );

		if ( max_bird_count < cell->bird_count )
			max_bird_count = cell->bird_count;
	} while( list_next( cell_list ) );
	return max_bird_count;
} /* wadingbird_get_max_bird_count() */

