/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_creel/catches_per_species.c    	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "application.h"
#include "query.h"
#include "piece.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "hydrology_library.h"
#include "appaserver_link_file.h"
#include "creel_library.h"

/* Constants */
/* --------- */
#define QUEUE_TOP_BOTTOM_LINES	5000

/* Prototypes */
/* ---------- */
void output_catches_per_species(
				char *application_name,
				char *begin_date,
				char *end_date,
				char *family,
				char *genus,
				char *species,
				char *fishing_purpose,
				char *aggregate_level,
				char *output_medium,
				char *document_root_directory,
				pid_t process_id,
				char *title,
				char *sub_title,
				char *process_name,
				char *fishing_area_list_string,
				char *interview_location );

FILE *get_input_pipe(		char **heading,
				int *fishing_purpose_piece,
				int *fishing_area_piece,
				int *interview_location_piece,
				int *interview_number_piece,
				int *florida_state_code_piece,
				int *family_piece,
				int *genus_piece,
				int *species_piece,
				int *common_name_piece,
				int *kept_piece,
				boolean *omit_output_day,
				boolean *omit_output_day_sum,
				boolean *omit_output_month_sum,
				char *application_name,
				char *begin_date,
				char *end_date,
				char *fishing_purpose,
				char *aggregate_level,
				char *family,
				char *genus,
				char *species,
				char *fishing_area_list_string,
				char *interview_location );

void parse_input_buffer(	char **year,
				char **month,
				char **day,
				char **census_date,
				char **fishing_purpose,
				char **fishing_area,
				char **interview_location,
				char **interview_number,
				char **florida_state_code,
				char **family,
				char **genus,
				char **species,
				char **common_name,
				int *kept_count,
				int *released_count,
				char *input_buffer,
				int fishing_purpose_piece,
				int fishing_area_piece,
				int interview_location_piece,
				int interview_number_piece,
				int florida_state_code_piece,
				int family_piece,
				int genus_piece,
				int species_piece,
				int common_name_piece,
				int kept_piece,
				char *aggregate_level );

/* Returns row_count */
/* ----------------- */
int perform_output(		FILE *output_pipe,
				FILE *input_pipe,
				int fishing_purpose_piece,
				int fishing_area_piece,
				int interview_location_piece,
				int interview_number_piece,
				int florida_state_code_piece,
				int family_piece,
				int genus_piece,
				int species_piece,
				int common_name_piece,
				int kept_piece,
				boolean omit_output_day,
				boolean omit_output_day_sum,
				boolean omit_output_month_sum,
				char *aggregate_level );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_date,
				char *end_date,
				char *process_name,
				char *fishing_purpose,
				char *aggregate_level,
				char *family,
				char *genus,
				char *species );


int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fishing_purpose;
	char *family;
	char *genus;
	char *species;
	char *aggregate_level;
	char *output_medium;
	char *begin_date;
	char *end_date;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 512 ];
	char sub_title[ 512 ];
	DOCUMENT *document;
	char *fishing_area_list_string;
	char *interview_location;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 13 )
	{
		fprintf(stderr,
"Usage: %s ignored process begin_date end_date family genus species fishing_purpose aggregate_level output_medium fishing_area interview_location\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* application_name = argv[ 1 ]; */
	process_name = argv[ 2 ];
	begin_date = argv[ 3 ];
	end_date = argv[ 4 ];
	family = argv[ 5 ];
	genus = argv[ 6 ];
	species = argv[ 7 ];
	fishing_purpose = argv[ 8 ];
	aggregate_level = argv[ 9 ];
	output_medium = argv[ 10 ];
	fishing_area_list_string = argv[ 11 ];
	interview_location = argv[ 12 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	if (	!*output_medium
	||	strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = "table";
	}

	document = document_new( "", application_name );
	document_set_output_content_type( document );
	
	document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		printf( "<h3>ERROR: invalid date(s).</h3>\n" );
		document_close();
		exit( 0 );
	}

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date,
			end_date,
			process_name,
			fishing_purpose,
			aggregate_level,
			family,
			genus,
			species );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date.sh" ) ) {};
	fflush( stdout );
	printf( "</h2>\n" );

	output_catches_per_species(
			application_name,
			begin_date,
			end_date,
			family,
			genus,
			species,
			fishing_purpose,
			aggregate_level,
			output_medium,
			appaserver_parameter_file->
				document_root,
			getpid(),
			title,
			sub_title,
			process_name,
			fishing_area_list_string,
			interview_location );

	document_close();
	return 0;

} /* main() */

void output_catches_per_species(
			char *application_name,
			char *begin_date,
			char *end_date,
			char *family,
			char *genus,
			char *species,
			char *fishing_purpose,
			char *aggregate_level,
			char *output_medium,
			char *document_root_directory,
			pid_t process_id,
			char *title,
			char *sub_title,
			char *process_name,
			char *fishing_area_list_string,
			char *interview_location )
{
	FILE *input_pipe;
	FILE *output_pipe = {0};
	FILE *output_file;
	char sys_string[ 1024 ];
	char *output_filename;
	char *ftp_filename;
	char *heading;
	int fishing_purpose_piece = -1;
	int fishing_area_piece = -1;
	int interview_location_piece = -1;
	int interview_number_piece = -1;
	int florida_state_code_piece = -1;
	int family_piece = -1;
	int genus_piece = -1;
	int species_piece = -1;
	int common_name_piece = -1;
	int kept_piece = -1;
	boolean omit_output_day = 0;
	boolean omit_output_day_sum = 0;
	boolean omit_output_month_sum = 0;
	APPASERVER_LINK_FILE *appaserver_link_file;
	int row_count;

	input_pipe = get_input_pipe(
			&heading,
			&fishing_purpose_piece,
			&fishing_area_piece,
			&interview_location_piece,
			&interview_number_piece,
			&florida_state_code_piece,
			&family_piece,
			&genus_piece,
			&species_piece,
			&common_name_piece,
			&kept_piece,
			&omit_output_day,
			&omit_output_day_sum,
			&omit_output_month_sum,
			application_name,
			begin_date,
			end_date,
			fishing_purpose,
			aggregate_level,
			family,
			genus,
			species,
			fishing_area_list_string,
			interview_location );

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	if ( strcmp( output_medium, "text_file" ) == 0
	||   strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_filename =
			appaserver_link_get_output_filename(
				appaserver_link_file->
					output_file->
					document_root_directory,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf(
			"<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fprintf( output_file, "%s\n", title );
			fprintf( output_file, "%s\n", sub_title );
			fprintf( output_file, "%s\n", heading );
			fclose( output_file );
		}

		sprintf( sys_string,
	 	 	"cat >> %s",
	 	 	output_filename );
		output_pipe = popen( sys_string, "w" );

		row_count =
		    perform_output(
			output_pipe,
			input_pipe,
			fishing_purpose_piece,
			fishing_area_piece,
			interview_location_piece,
			interview_number_piece,
			florida_state_code_piece,
			family_piece,
			genus_piece,
			species_piece,
			common_name_piece,
			kept_piece,
			omit_output_day,
			omit_output_day_sum,
			omit_output_month_sum,
			aggregate_level );

		pclose( input_pipe );
		pclose( output_pipe );

		ftp_filename =
			appaserver_link_get_link_prompt(
				appaserver_link_file->
					link_prompt->
					prepend_http_boolean,
				appaserver_link_file->
					link_prompt->
					http_prefix,
				appaserver_link_file->
					link_prompt->server_address,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		printf( "<p>Generated %d rows.<br>\n", row_count );

		appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "table" ) == 0 )
	{
		sprintf(sys_string,
	 	 "queue_lines %d | html_table.e '' '%s' ','",
			QUEUE_TOP_BOTTOM_LINES,
			heading );

		output_pipe = popen( sys_string, "w" );

		row_count =
		    perform_output(
			output_pipe,
			input_pipe,
			fishing_purpose_piece,
			fishing_area_piece,
			interview_location_piece,
			interview_number_piece,
			florida_state_code_piece,
			family_piece,
			genus_piece,
			species_piece,
			common_name_piece,
			kept_piece,
			omit_output_day,
			omit_output_day_sum,
			omit_output_month_sum,
			aggregate_level );

		pclose( input_pipe );
		pclose( output_pipe );
	}
	else
	{
		pclose( input_pipe );
		printf( "<H3>Error: invalid output medium.</h3>\n" );
	}

} /* output_catches_per_species() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *begin_date,
			char *end_date,
			char *process_name,
			char *fishing_purpose,
			char *aggregate_level,
			char *family,
			char *genus,
			char *species )
{
	char fishing_purpose_display[ 128 ];
	char aggregate_level_display[ 128 ];
	char species_display[ 256 ];

	*fishing_purpose_display = '\0';
	*aggregate_level_display = '\0';
	*species_display = '\0';

	format_initial_capital( title, process_name );

	if ( *family && strcmp( family, "family" ) != 0 )
	{
		sprintf(	title + strlen( title ),
				" for %s",
				family );
	}
	if ( *genus && strcmp( genus, "genus" ) != 0 )
	{
		sprintf(	title + strlen( title ),
				"/%s",
				genus );
	}
	if ( *species && strcmp( species, "species" ) != 0 )
	{
		sprintf(	title + strlen( title ),
				" %s",
				species );
	}

	if ( fishing_purpose
	&&   *fishing_purpose
	&&   strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		sprintf(	fishing_purpose_display,
				"%s ",
				fishing_purpose );
	}

	if ( aggregate_level
	&&   *aggregate_level
	&&   strcmp( aggregate_level, "aggregate_level" ) != 0 )
	{
		sprintf(	aggregate_level_display,
				"%s ",
				aggregate_level );
	}

	sprintf(	sub_title,
			"%s%sFrom: %s To: %s",
			fishing_purpose_display,
			aggregate_level_display,
			begin_date,
			end_date );

	format_initial_capital( sub_title, sub_title );

} /* get_title_and_sub_title() */

/* Returns row_count */
/* ----------------- */
int perform_output(	FILE *output_pipe,
			FILE *input_pipe,
			int fishing_purpose_piece,
			int fishing_area_piece,
			int interview_location_piece,
			int interview_number_piece,
			int florida_state_code_piece,
			int family_piece,
			int genus_piece,
			int species_piece,
			int common_name_piece,
			int kept_piece,
			boolean omit_output_day,
			boolean omit_output_day_sum,
			boolean omit_output_month_sum,
			char *aggregate_level )
{
	char *year;
	char *month;
	char *day;
	char *census_date;
	char *interview_number;
	char *fishing_purpose;
	char *fishing_area;
	char *interview_location;
	char *florida_state_code;
	char *family;
	char *genus;
	char *species;
	char *common_name;
	int kept_count;
	int released_count;
	char input_buffer[ 1024 ];
	char old_year[ 16 ];
	char old_month[ 16 ];
	char old_day[ 16 ];
	int year_kept = 0;
	int year_released = 0;
	int month_kept = 0;
	int month_released = 0;
	int day_kept = 0;
	int day_released = 0;
	boolean output_end_of_line = 0;
	boolean new_year;
	boolean new_month;
	boolean new_day;
	int row_count = 0;

	*old_year = '\0';

	while( get_line( input_buffer, input_pipe ) )
	{
		row_count++;

		parse_input_buffer(
				&year,
				&month,
				&day,
				&census_date,
				&fishing_purpose,
				&fishing_area,
				&interview_location,
				&interview_number,
				&florida_state_code,
				&family,
				&genus,
				&species,
				&common_name,
				&kept_count,
				&released_count,
				input_buffer,
				fishing_purpose_piece,
				fishing_area_piece,
				interview_location_piece,
				interview_number_piece,
				florida_state_code_piece,
				family_piece,
				genus_piece,
				species_piece,
				common_name_piece,
				kept_piece,
				aggregate_level );

		/* If first time */
		/* ------------- */
		if ( !*old_year )
		{
			strcpy( old_year, year );
			strcpy( old_month, month );
			strcpy( old_day, day );
		}

		new_year = 0;
		new_month = 0;
		new_day = 0;

		if ( strcmp( old_year, year ) != 0 )
		{
			new_year = 1;
		}
		else
		if ( strcmp( old_month, month ) != 0 )
		{
			new_month = 1;
		}
		else
		if ( strcmp( old_day, day ) != 0 )
		{
			new_day = 1;
		}

		if ( new_year )
		{
			if ( !omit_output_day_sum )
			{
				fprintf( output_pipe,
				 	",%d,%d,%d",
				 	day_kept,
				 	day_released,
				 	day_kept + day_released );
			}

			if ( !omit_output_month_sum )
			{
				fprintf( output_pipe,
				 	",%d,%d,%d",
				 	month_kept,
				 	month_released,
				 	month_kept + month_released );
			}

			fprintf( output_pipe,
				 ",%d,%d,%d\n",
				 year_kept,
				 year_released,
				 year_kept + year_released );

			strcpy( old_year, year );
			strcpy( old_month, month );
			strcpy( old_day, day );

			day_kept = 0;
			day_released = 0;
			month_kept = 0;
			month_released = 0;
			year_kept = 0;
			year_released = 0;

			output_end_of_line = 0;
		}
		else
		if ( new_month )
		{
			if ( !omit_output_day_sum )
			{
				fprintf( output_pipe,
				 	",%d,%d,%d",
				 	day_kept,
				 	day_released,
				 	day_kept + day_released );
			}

			if ( !omit_output_month_sum )
			{
				fprintf( output_pipe,
				 	",%d,%d,%d",
				 	month_kept,
				 	month_released,
				 	month_kept + month_released );
			}

			fprintf( output_pipe, ",,,\n" );

			strcpy( old_month, month );
			strcpy( old_day, day );

			day_kept = 0;
			day_released = 0;
			month_kept = 0;
			month_released = 0;

			output_end_of_line = 0;
		}
		else
		if ( new_day )
		{
			if ( !omit_output_day_sum )
			{
				fprintf( output_pipe,
				 	",%d,%d,%d",
				 	day_kept,
				 	day_released,
				 	day_kept + day_released );
			}

			fprintf( output_pipe,
				 ",,,,,,\n" );

			strcpy( old_day, day );

			day_kept = 0;
			day_released = 0;

			output_end_of_line = 0;
		}

		if ( output_end_of_line )
		{
			fprintf( output_pipe, ",,,,,,,,,\n" );
		}

		output_end_of_line = 1;

		day_kept += kept_count;
		day_released += released_count;
		month_kept += kept_count;
		month_released += released_count;
		year_kept += kept_count;
		year_released += released_count;

		fprintf( output_pipe,
			 "%s",
			 census_date );

		if ( fishing_purpose_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	fishing_purpose );
		}

		if ( fishing_area_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	fishing_area );
		}

		if ( interview_location_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	interview_location );
		}

		if ( interview_number_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	interview_number );
		}

		if ( florida_state_code_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	florida_state_code );
		}

		if ( family_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	family );
		}

		if ( genus_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	genus );
		}

		if ( species_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	species );
		}

		if ( common_name_piece != -1 )
		{
			fprintf( output_pipe,
			 	",%s",
			 	common_name );
		}

		if ( !omit_output_day )
		{
			fprintf( output_pipe,
		 		",%d,%d,%d",
		 		kept_count,
				released_count,
				kept_count + released_count );
		}
	}

	if ( row_count )
	{
		if ( !omit_output_day_sum )
		{
			fprintf( output_pipe,
			 	",%d,%d,%d",
			 	day_kept,
			 	day_released,
			 	day_kept + day_released );
		}
	
		if ( !omit_output_month_sum )
		{
			fprintf( output_pipe,
			 	",%d,%d,%d",
			 	month_kept,
			 	month_released,
			 	month_kept + month_released );
		}
	
		fprintf( output_pipe,
			 ",%d,%d,%d\n",
			 year_kept,
			 year_released,
			 year_kept + year_released );
	}

	return row_count;

} /* perform_output() */

static char *catches_table_name = "catches";

FILE *get_input_pipe(	char **heading,
			int *fishing_purpose_piece,
			int *fishing_area_piece,
			int *interview_location_piece,
			int *interview_number_piece,
			int *florida_state_code_piece,
			int *family_piece,
			int *genus_piece,
			int *species_piece,
			int *common_name_piece,
			int *kept_piece,
			boolean *omit_output_day,
			boolean *omit_output_day_sum,
			boolean *omit_output_month_sum,
			char *application_name,
			char *begin_date,
			char *end_date,
			char *fishing_purpose,
			char *aggregate_level,
			char *family,
			char *genus,
			char *species,
			char *fishing_area_list_string,
			char *interview_location )
{
	char sys_string[ 65536 ];
	char select[ 1024 ];
	char catches_join_where[ 1024 ];
	char species_where[ 512 ];
	char fishing_purpose_where[ 256 ];
	char where[ 65536 ];
	char *species_table_name;
	char *from;
	char *group = {0};
	char *order = {0};

	species_table_name = get_table_name( application_name, "species" );

	if (	!*aggregate_level ||
		strcmp( aggregate_level, "aggregate_level" ) == 0 )
	{
		*heading =
"Census Date,Fishing Purpose,Fishing Area,Interview Location,Interview Number,Florida State Code,Family,Genus,Species,Common Name,Kept,Released,Caught,Day Kept,Day Released,Day Caught,Month Kept,Month Released,Month Caught,Year Kept,Year Released,Year Caught";

		sprintf(select,
"%s.census_date,%s.fishing_purpose,fishing_area,fishing_trips.interview_location,%s.interview_number,%s.florida_state_code,%s.family,%s.genus,%s.species,%s.common_name,kept_count,released_count",
		 	catches_table_name,
		 	catches_table_name,
		 	catches_table_name,
		 	species_table_name,
		 	catches_table_name,
		 	catches_table_name,
		 	catches_table_name,
		 	species_table_name );

		order = "select";

		*fishing_purpose_piece = 1;
		*fishing_area_piece = 2;
		*interview_location_piece = 3;
		*interview_number_piece = 4;
		*florida_state_code_piece = 5;
		*family_piece = 6;
		*genus_piece = 7;
		*species_piece = 8;
		*common_name_piece = 9;
		*kept_piece = 10;
	}
	else
	if ( strcmp( aggregate_level, "daily" ) == 0 )
	{
		*heading =
"Census Date,Kept,Released,Caught,Month Kept,Month Released,Month Caught,Year Kept,Year Released,Year Caught";

		strcpy(select,
"catches.census_date,sum(kept_count),sum(released_count)" );

		group = "catches.census_date";

		*fishing_purpose_piece = -1;
		*interview_number_piece = -1;
		*florida_state_code_piece = -1;
		*family_piece = -1;
		*genus_piece = -1;
		*species_piece = -1;
		*common_name_piece = -1;
		*kept_piece = 1;
		*omit_output_day_sum = 1;
	}
	else
	if ( strcmp( aggregate_level, "monthly" ) == 0 )
	{
		*heading =
"Census Month,Month Kept,Month Released,Month Caught,Year Kept,Year Released,Year Caught";

		strcpy(select,
"substr(catches.census_date,1,7),sum(kept_count),sum(released_count)" );

		group = "substr(catches.census_date,1,7)";

		*fishing_purpose_piece = -1;
		*interview_number_piece = -1;
		*florida_state_code_piece = -1;
		*family_piece = -1;
		*genus_piece = -1;
		*species_piece = -1;
		*common_name_piece = -1;
		*kept_piece = 1;
		*omit_output_day = 1;
		*omit_output_day_sum = 1;
	}
	else
	if ( strcmp( aggregate_level, "annually" ) == 0 )
	{
		*heading =
"Census Year,Year Kept,Year Released,Year Caught";

		strcpy(select,
"substr(catches.census_date,1,4),sum(kept_count),sum(released_count)" );

		group = "substr(catches.census_date,1,4)";

		*fishing_purpose_piece = -1;
		*interview_number_piece = -1;
		*florida_state_code_piece = -1;
		*family_piece = -1;
		*genus_piece = -1;
		*species_piece = -1;
		*common_name_piece = -1;
		*kept_piece = 1;
		*omit_output_day = 1;
		*omit_output_day_sum = 1;
		*omit_output_month_sum = 1;
	}

	strcpy( species_where, "1 = 1" );

	if ( *family && strcmp( family, "family" ) != 0 )
	{
		sprintf(species_where,
			"%s.family = '%s'",
			catches_table_name,
			family );
	}

	if ( *genus && strcmp( genus, "genus" ) != 0 )
	{
		sprintf(species_where + strlen( species_where ),
			"and %s.genus = '%s'",
			catches_table_name,
			genus );
	}

	if ( *species && strcmp( species, "species" ) != 0 )
	{
		sprintf(species_where + strlen( species_where ),
			"and %s.species = '%s'",
			catches_table_name,
			species );
	}

	sprintf( catches_join_where,
		 "%s.family = %s.family and		"
		 "%s.genus = %s.genus and		"
		 "%s.species = %s.species 		",
		 catches_table_name,
		 species_table_name,
		 catches_table_name,
		 species_table_name,
		 catches_table_name,
		 species_table_name );

	if ( !*fishing_purpose
	||   strcmp( fishing_purpose, "fishing_purpose" ) == 0 )
	{
		strcpy( fishing_purpose_where, "1 = 1" );
	}
	else
	{
		sprintf( fishing_purpose_where,
			 "catches.fishing_purpose = '%s'",
			 fishing_purpose );
	}

	sprintf(where,
"catches.census_date between '%s' and '%s' and %s and %s and %s and %s %s and %s",
		 begin_date,
		 end_date,
		 catches_join_where,
		 creel_library_get_fishing_trips_join_where(
			"catches" /* related_folder */ ),
		 fishing_purpose_where,
		 species_where,
		 /* ------------------------ */
		 /* Returns preceeding "and" */
		 /* ------------------------ */
		 creel_library_get_fishing_area_where(
			fishing_area_list_string ),
		 creel_library_get_interview_location_where(
			interview_location ) );

	from = "catches,species,fishing_trips";

	if ( group )
	{
		sprintf(sys_string,
		 	"get_folder_data	application=%s		"
		 	"			select=\"%s\"		"
		 	"			folder=%s		"
		 	"			where=\"%s\"		"
		 	"			group=\"%s\"		",
		 	application_name,
		 	select,
			from,
		 	where,
		 	group );
	}
	else
	if ( order )
	{
		sprintf(sys_string,
		 	"get_folder_data	application=%s		"
		 	"			select=\"%s\"		"
		 	"			folder=%s		"
		 	"			where=\"%s\"		"
		 	"			order=\"%s\"		",
		 	application_name,
		 	select,
			from,
		 	where,
		 	order );
	}
	else
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty group and order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return popen( sys_string, "r" );

} /* get_input_pipe() */

void parse_input_buffer(	char **year,
				char **month,
				char **day,
				char **census_date,
				char **fishing_purpose,
				char **fishing_area,
				char **interview_location,
				char **interview_number,
				char **florida_state_code,
				char **family,
				char **genus,
				char **species,
				char **common_name,
				int *kept_count,
				int *released_count,
				char *input_buffer,
				int fishing_purpose_piece,
				int fishing_area_piece,
				int interview_location_piece,
				int interview_number_piece,
				int florida_state_code_piece,
				int family_piece,
				int genus_piece,
				int species_piece,
				int common_name_piece,
				int kept_piece,
				char *aggregate_level )
{
	static char local_year[ 16 ];
	static char local_month[ 16 ];
	static char local_day[ 16 ];
	static char local_census_date[ 16 ];
	static char local_fishing_purpose[ 128 ];
	static char local_fishing_area[ 128 ];
	static char local_interview_location[ 128 ];
	static char local_interview_number[ 128 ];
	static char local_florida_state_code[ 16 ];
	static char local_family[ 128 ];
	static char local_genus[ 128 ];
	static char local_species[ 128 ];
	static char local_common_name[ 128 ];
	char count_buffer[ 128 ];

	*year = local_year;
	*month = local_month;
	*day = local_day;
	*census_date = local_census_date;
	*fishing_purpose = local_fishing_purpose;
	*fishing_area = local_fishing_area;
	*interview_location = local_interview_location;
	*interview_number = local_interview_number;
	*florida_state_code = local_florida_state_code;
	*family = local_family;
	*genus = local_genus;
	*species = local_species;
	*common_name = local_common_name;

	piece(	local_census_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	if ( !*aggregate_level ||
	     strcmp( aggregate_level, "aggregate_level" ) == 0 ||
	     strcmp( aggregate_level, "daily" ) == 0 )
	{
		if ( character_count( '-', local_census_date ) != 2 )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid census_date in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			exit( 1 );
		}

		piece( local_year, '-', local_census_date, 0 );

		strcpy( local_month, local_census_date );
		piece_inverse( local_month, '-', 2 );

		strcpy( local_day, local_census_date );
	}
	else
	if ( strcmp( aggregate_level, "monthly" ) == 0 )
	{
		if ( character_count( '-', local_census_date ) != 1 )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid census_date in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			exit( 1 );
		}

		piece( local_year, '-', local_census_date, 0 );
		strcpy( local_month, local_census_date );
		*local_day = '\0';
	}
	else
	if ( strcmp( aggregate_level, "annually" ) == 0 )
	{
		strcpy( local_year, local_census_date );
		*local_month = '\0';
		*local_day = '\0';
	}

	if ( fishing_purpose_piece != -1 )
	{
		piece(	local_fishing_purpose,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			fishing_purpose_piece );
	}

	if ( fishing_area_piece != -1 )
	{
		piece(	local_fishing_area,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			fishing_area_piece );
	}

	if ( interview_location_piece != -1 )
	{
		piece(	local_interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			interview_location_piece );
	}

	if ( interview_number_piece != -1 )
	{
		piece(	local_interview_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			interview_number_piece );
	}

	if ( florida_state_code_piece != -1 )
	{
		piece(	local_florida_state_code,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			florida_state_code_piece );
	}

	if ( family_piece != -1 )
	{
		piece(	local_family,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			family_piece );
	}

	if ( genus_piece != -1 )
	{
		piece(	local_genus,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			genus_piece );
	}

	if ( species_piece != -1 )
	{
		piece(	local_species,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			species_piece );
	}

	if ( common_name_piece != -1 )
	{
		piece(	local_common_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			common_name_piece );
	}

	piece(	count_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		kept_piece );
	*kept_count = atoi( count_buffer );

	piece(	count_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		kept_piece + 1 );
	*released_count = atoi( count_buffer );

} /* parse_input_buffer() */

