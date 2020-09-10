/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/load_fishing_permits.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "boolean.h"
#include "piece.h"
#include "creel_load_library.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define DECAL_COUNT_MAX		4

/* Prototypes */
/* ---------- */
void output_table_full(	char *input_filename );

void output_table_abbreviated(
			char *input_filename );

void insert_guide_anglers(
			char *input_filename,
			boolean replace_existing_data );

void update_guide_anglers(
			char *input_filename );

void insert_permits(	char *input_filename,
			boolean abbreviated_columns,
			boolean replace_existing_data );

void update_permits(	char *application_name,
			char *input_filename,
			boolean abbreviated_columns );

void insert_guide_boat_decal(
			char *input_filename,
			boolean replace_existing_data );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *input_filename;
	boolean replace_existing_data;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	boolean abbreviated_columns;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s process filename abbreviated_columns_yn replace_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	input_filename = argv[ 2 ];
	abbreviated_columns = (*argv[ 3 ] == 'y');
	replace_existing_data = (*argv[ 4 ] == 'y');
	execute = (*argv[ 5 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h2>Load Fishing Permits\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*input_filename
	||   strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( execute )
	{
		insert_guide_anglers(
			input_filename,
			replace_existing_data );

		if ( abbreviated_columns )
		{
			insert_permits(
				input_filename,
				abbreviated_columns,
				replace_existing_data );

			if ( replace_existing_data )
			{
				update_permits(
					application_name,
					input_filename,
					abbreviated_columns );
			}
		}
		else
		{
			if ( replace_existing_data )
			{
				update_guide_anglers(
					input_filename );
			}

			insert_permits(
				input_filename,
				abbreviated_columns,
				replace_existing_data );

			if ( replace_existing_data )
			{
				update_permits(
					application_name,
					input_filename,
					abbreviated_columns );
			}

			insert_guide_boat_decal(
				input_filename,
				replace_existing_data );
		}
	}
	else
	{
		if ( abbreviated_columns )
			output_table_abbreviated(
				input_filename );
		else
			output_table_full(
				input_filename );
	}

	if ( execute )
	{
		process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

		printf( "<p>Process complete\n" );
	}
	else
	{
		printf( "<p>Process not executed\n" );
	}

	document_close();

	return 0;
}

#define PRIMARY_KEY	"guide_angler_name"

void insert_guide_anglers(
			char *input_filename,
			boolean replace_existing_data )
{
	FILE *input_file;
	FILE *guide_anglers_insert_pipe;
	char input_string[ 1024 ];
	char first_name[ 256 ];
	char last_name[ 256 ];
	char sys_string[ 1024 ];
	char *guide_angler_name;

	sprintf(sys_string,
		"insert_statement t=%s f=%s replace=%c	|"
		"sql 2>&1				|"
		"html_paragraph_wrapper			|"
		"cat					 ",
		 "guide_anglers",
		 PRIMARY_KEY,
		(replace_existing_data) ? 'y' : 'n' );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	guide_anglers_insert_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_quote_comma(
			last_name,
			input_string,
			GUIDE_LAST_NAME_PIECE ) )
		{
			continue;
		}

		if ( !piece_quote_comma(
			first_name,
			input_string,
			GUIDE_FIRST_NAME_PIECE ) )
		{
			continue;
		}

		guide_angler_name =
			creel_load_library_get_guide_angler_name(
				first_name,
				last_name,
				(char *)0 /* application_name */,
				(char *)0 /* permit_number_string */ );

		fprintf(guide_anglers_insert_pipe,
			"%s\n",
			guide_angler_name );

	}

	fclose( input_file );
	pclose( guide_anglers_insert_pipe );
}

#define INSERT_GUIDE_ANGLERS_FIELD_LIST			\
		"guide_angler_name,"			\
		"street_address,"			\
		"city,"					\
		"state_code,"				\
		"zip_code,"				\
		"permit_issued_date,"			\
		"permit_expired_date,"			\
		"permit_code,"				\
		"phone_number,"				\
		"email_address"

#define INSERT_GUIDE_ABBREVIATED_ANGLERS_FIELD_LIST	\
		"guide_angler_name,"			\
		"permit_issued_date,"			\
		"permit_expired_date,"			\
		"permit_code"

void update_guide_anglers(
			char *input_filename )
{
	FILE *input_file;
	FILE *guide_anglers_update_pipe;
	char input_string[ 1024 ];
	char first_name[ 256 ];
	char last_name[ 256 ];
	char *guide_angler_name;
	char street_address[ 256 ];
	char city[ 256 ];
	char state_code[ 256 ];
	char zip_code[ 256 ];
	char phone_number[ 256 ];
	char email_address[ 256 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/fishing_permit_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	sprintf(sys_string,
		"update_statement.e table=%s key=%s carrot=y	|"
		"sql 2>&1					|"
		"html_paragraph_wrapper				|"
		"cat						 ",
		"guide_anglers",
		PRIMARY_KEY );

	guide_anglers_update_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_quote_comma(
			last_name,
			input_string,
			GUIDE_LAST_NAME_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece last name in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			first_name,
			input_string,
			GUIDE_FIRST_NAME_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece first name code in (%s)\n",
				input_string );
			continue;
		}

		guide_angler_name =
			creel_load_library_get_guide_angler_name(
				first_name,
				last_name,
				(char *)0 /* application_name */,
				(char *)0 /* permit_number_string */ );

		if ( !piece_quote_comma(
			street_address,
			input_string,
			GUIDE_STREET_ADDRESS_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece street address in (%s)\n",
				input_string );
			continue;
		}

		if ( *street_address )
		{
			fprintf(guide_anglers_update_pipe,
			 	"%s^street_address^%s\n",
			 	guide_angler_name,
			 	street_address );
		}

		if ( !piece_quote_comma(
			city,
			input_string,
			GUIDE_CITY_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece city in (%s)\n",
				input_string );
			continue;
		}

		if ( *city )
		{
			fprintf( guide_anglers_update_pipe,
			 	"%s^city^%s\n",
			 	guide_angler_name,
			 	city );
		}

		if ( !piece_quote_comma(
			state_code,
			input_string,
			GUIDE_STATE_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece state code in (%s)\n",
				input_string );
			continue;
		}

		if ( *state_code )
		{
			fprintf( guide_anglers_update_pipe,
			 	"%s^state_code^%s\n",
			 	guide_angler_name,
			 	state_code );
		}

		if ( !piece_quote_comma(
			zip_code,
			input_string,
			GUIDE_ZIP_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece zip code in (%s)\n",
				input_string );
			continue;
		}

		if ( *zip_code )
		{
			fprintf( guide_anglers_update_pipe,
			 	"%s^zip_code^%s\n",
			 	guide_angler_name,
			 	zip_code );
		}

		if ( !piece_quote_comma(
			phone_number,
			input_string,
			GUIDE_PHONE_NUMBER_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece phone number in (%s)\n",
				input_string );
			continue;
		}

		if ( *phone_number )
		{
			fprintf( guide_anglers_update_pipe,
			 	"%s^phone_number^%s\n",
			 	guide_angler_name,
			 	phone_number );
		}

		if ( !piece_quote_comma(
			email_address,
			input_string,
			GUIDE_EMAIL_ADDRESS_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece email address in (%s)\n",
				input_string );
			continue;
		}

		if ( *email_address )
		{
			fprintf( guide_anglers_update_pipe,
			 	"%s^email_address^%s\n",
			 	guide_angler_name,
			 	email_address );
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( guide_anglers_update_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Fishing Permit Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) ){};
	}

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};
}

#define INSERT_PERMITS_FIELD_LIST		\
		"permit_code,"			\
		"guide_angler_name,"		\
		"issued_date,"			\
		"expiration_date"

void insert_permits(	char *input_filename,
			boolean abbreviated_columns,
			boolean replace_existing_data )
{
	FILE *input_file;
	FILE *fishing_permits_output_pipe;
	char input_string[ 1024 ];
	char first_name[ 256 ];
	char last_name[ 256 ];
	char *guide_angler_name;
	char permit_code[ 256 ];
	char date_issued_mdyy[ 256 ];
	char date_issued_international[ 256 ];
	char date_expired_mdyy[ 256 ];
	char date_expired_international[ 256 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/fishing_permit_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	sprintf(sys_string,
		"insert_statement.e t=%s f=%s d='|' replace=%c	|"
		"sql 2>&1					|"
		"html_paragraph_wrapper				|"
		"cat						 ",
	 	"permits",
	 	INSERT_PERMITS_FIELD_LIST,
	 	(replace_existing_data) ? 'y' : 'n' );

	fishing_permits_output_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_quote_comma(
			last_name,
			input_string,
			GUIDE_LAST_NAME_PIECE ) )
		{
			fprintf( error_file,
				 "Can't parse last name in [%s]\n",
				 input_string );
			continue;
		}

		if ( !piece_quote_comma(
			first_name,
			input_string,
			GUIDE_FIRST_NAME_PIECE ) )
		{
			fprintf( error_file,
				 "Can't parse first name in [%s]\n",
				 input_string );
			continue;
		}

		guide_angler_name =
			creel_load_library_get_guide_angler_name(
				first_name,
				last_name,
				(char *)0 /* application_name */,
				(char *)0 /* permit_number_string */ );

		if ( abbreviated_columns )
		{
			if ( !piece_quote_comma(
				permit_code,
				input_string,
				GUIDE_ABBREVIATED_PERMIT_CODE_PIECE ) )
			{
				fprintf(error_file,
				 	"Can't parse permit code in [%s]\n",
				 	input_string );
				continue;
			}

			if ( !piece_quote_comma(
				date_issued_mdyy,
				input_string,
				GUIDE_ABBREVIATED_DATE_ISSUED_PIECE ) )
			{
				fprintf(error_file,
				 	"Can't parse date issued in [%s]\n",
				 	input_string );
				continue;
			}

			if ( !piece_quote_comma(
				date_expired_mdyy,
				input_string,
				GUIDE_ABBREVIATED_DATE_EXPIRED_PIECE ) )
			{
				fprintf(error_file,
				 	"Can't parse date expired in [%s]\n",
				 	input_string );
					continue;
			}
		}
		else
		{
			if ( !piece_quote_comma(
				permit_code,
				input_string,
				GUIDE_PERMIT_CODE_PIECE ) )
			{
				fprintf(error_file,
				 	"Can't parse permit code in [%s]\n",
				 	input_string );
				continue;
			}

			if ( !piece_quote_comma(
				date_issued_mdyy,
				input_string,
				GUIDE_DATE_ISSUED_PIECE ) )
			{
				fprintf(error_file,
				 	"Can't parse date issued in [%s]\n",
				 	input_string );
				continue;
			}

			if ( !piece_quote_comma(
				date_expired_mdyy,
				input_string,
				GUIDE_DATE_EXPIRED_PIECE ) )
			{
				fprintf(error_file,
				 	"Can't parse date expired in [%s]\n",
				 	input_string );
				continue;
			}
		}

		*date_issued_international = '\0';

		if ( !date_convert_source_american(
			date_issued_international,
			international,
			date_issued_mdyy ) )
		{
			fprintf(error_file,
			 	"Bad format for date issued in [%s]\n",
			 	input_string );
			continue;
		}

		*date_expired_international = '\0';

		if ( !date_convert_source_american(
			date_expired_international,
			international,
			date_expired_mdyy ) )
		{
			fprintf(error_file,
			 	"Bad format for date expired in [%s]\n",
			 	input_string );
			continue;
		}

		fprintf(fishing_permits_output_pipe,
			"%s|%s|%s|%s\n",
			permit_code,
			guide_angler_name,
			date_issued_international,
			date_expired_international );
	}

	fclose( input_file );
	pclose( fishing_permits_output_pipe );
	fclose( error_file );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Fishing Permit Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) ){};
	}

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};
}

#define INSERT_DECALS_FIELD_LIST	\
		"guide_angler_name,"	\
		"decal_number"

void insert_guide_boat_decal(
			char *input_filename,
			boolean replace_existing_data )
{
	FILE *input_file;
	FILE *guide_boat_decal_output_pipe;
	char input_string[ 1024 ];
	char first_name[ 256 ];
	char last_name[ 256 ];
	char *guide_angler_name;
	char decal_number_string[ 256 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;
	int decal_offset;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/fishing_permit_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	sprintf(sys_string,
		"insert_statement.e t=%s f=%s d='|' replace=%c	|"
		"sql 2>&1					|"
		"html_paragraph_wrapper				|"
		"cat						 ",
		"guide_boat_decal",
		 INSERT_DECALS_FIELD_LIST,
		 (replace_existing_data) ? 'y' : 'n' );

	guide_boat_decal_output_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_quote_comma(
			last_name,
			input_string,
			GUIDE_LAST_NAME_PIECE ) )
		{
			fprintf(error_file,
				"Can't parse last name in [%s]\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			first_name,
			input_string,
			GUIDE_FIRST_NAME_PIECE ) )
		{
			fprintf(error_file,
				"Can't parse first name in [%s]\n",
				input_string );
			continue;
		}

		guide_angler_name =
			creel_load_library_get_guide_angler_name(
				first_name,
				last_name,
				(char *)0 /* application_name */,
				(char *)0 /* permit_number_string */ );

		decal_offset = 0;

		while( 1 )
		{
			if ( !piece_quote_comma(
				decal_number_string,
				input_string,
				GUIDE_STARTING_DECAL_PIECE + decal_offset ) )
			{
				break;
			}

			if ( !*decal_number_string ) break;

			fprintf(guide_boat_decal_output_pipe,
				"%s|%s\n",
				guide_angler_name,
				decal_number_string );

			if ( ( decal_offset++ ) == DECAL_COUNT_MAX ) break;
		}
	}

	fclose( input_file );
	pclose( guide_boat_decal_output_pipe );
	fclose( error_file );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Guide Boat Decal Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) ){};
	}

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};
}

void output_table_full( char *input_filename )
{
	FILE *input_file;
	FILE *guide_anglers_output_pipe;
	char input_string[ 1024 ];
	char first_name[ 256 ];
	char last_name[ 256 ];
	char *guide_angler_name;
	char permit_code[ 256 ];
	char date_issued_mdyy[ 256 ];
	char date_issued_international[ 256 ];
	char date_expired_mdyy[ 256 ];
	char date_expired_international[ 256 ];
	char street_address[ 256 ];
	char city[ 256 ];
	char state_code[ 256 ];
	char zip_code[ 256 ];
	char phone_number[ 256 ];
	char email_address[ 256 ];
	char decal_number_string[ 256 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;
	int decal_offset;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/fishing_permit_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Guide Anglers' %s,decal '|'",
		 INSERT_GUIDE_ANGLERS_FIELD_LIST );

	guide_anglers_output_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_quote_comma(
			last_name,
			input_string,
			GUIDE_LAST_NAME_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece last name in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			first_name,
			input_string,
			GUIDE_FIRST_NAME_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece first name code in (%s)\n",
				input_string );
			continue;
		}

		guide_angler_name =
			creel_load_library_get_guide_angler_name(
				first_name,
				last_name,
				(char *)0 /* application_name */,
				(char *)0 /* permit_number_string */ );

		if ( !piece_quote_comma(
			permit_code,
			input_string,
			GUIDE_PERMIT_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece permit code in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			date_issued_mdyy,
			input_string,
			GUIDE_DATE_ISSUED_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece date issued in (%s)\n",
				input_string );
			continue;
		}

		if ( !*date_issued_mdyy )
		{
			*date_issued_international = '\0';
		}
		else
		{
			date_convert_source_american(
				date_issued_international,
				international,
				date_issued_mdyy );

			if ( !*date_issued_international )
			{
				fprintf(error_file,
				"Warning: invalid date format = (%s)\n",
					date_issued_mdyy );
				continue;
			}
		}

		if ( !piece_quote_comma(
			date_expired_mdyy,
			input_string,
			GUIDE_DATE_EXPIRED_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece date expired in (%s)\n",
				input_string );
			continue;
		}

		if ( !*date_expired_mdyy )
		{
			*date_expired_international = '\0';
		}
		else
		{
			date_convert_source_american(
				date_expired_international,
				international,
				date_expired_mdyy );

			if ( !*date_expired_international )
			{
				fprintf(error_file,
				"Warning: invalid date format = (%s)\n",
					date_expired_mdyy );
				continue;
			}
		}

		if ( !piece_quote_comma(
			street_address,
			input_string,
			GUIDE_STREET_ADDRESS_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece street address in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			city,
			input_string,
			GUIDE_CITY_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece city in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			state_code,
			input_string,
			GUIDE_STATE_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece state code in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			zip_code,
			input_string,
			GUIDE_ZIP_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece zip code in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			phone_number,
			input_string,
			GUIDE_PHONE_NUMBER_PIECE ) )
		{
			*phone_number = '\0';
		}

		if ( !piece_quote_comma(
			email_address,
			input_string,
			GUIDE_EMAIL_ADDRESS_PIECE ) )
		{
			*email_address = '\0';
		}

		fprintf(guide_anglers_output_pipe,
			"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			guide_angler_name,
			street_address,
			city,
		      	state_code,
			zip_code,
			date_issued_international,
			date_expired_international,
			permit_code,
			phone_number,
			email_address );

		decal_offset = 0;

		while( 1 )
		{
			if ( !piece_quote_comma(
				decal_number_string,
				input_string,
				GUIDE_STARTING_DECAL_PIECE + decal_offset ) )
			{
				break;
			}

			if ( !*decal_number_string ) break;

			if ( decal_offset == 0 )
			{
				fprintf(guide_anglers_output_pipe,
					"|%s\n",
					decal_number_string );
			}
			else
			{
				fprintf(guide_anglers_output_pipe,
					"||||||||||%s\n",
					decal_number_string );
			}

			if ( ( decal_offset++ ) == DECAL_COUNT_MAX ) break;
		}

		/* If no decals */
		/* ------------ */
		if ( decal_offset == 0 )
		{
			fprintf(guide_anglers_output_pipe, "\n" );
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( guide_anglers_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Guide Angler Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) ){};
	}

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};
}

void output_table_abbreviated( char *input_filename )
{
	FILE *input_file;
	FILE *guide_anglers_output_pipe;
	char input_string[ 1024 ];
	char first_name[ 256 ];
	char last_name[ 256 ];
	char *guide_angler_name;
	char permit_code[ 256 ];
	char date_issued_mdyy[ 256 ];
	char date_issued_international[ 256 ];
	char date_expired_mdyy[ 256 ];
	char date_expired_international[ 256 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/fishing_permit_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	sprintf( sys_string,
"html_table.e 'Insert into Guide Anglers' %s '|'",
		 INSERT_GUIDE_ABBREVIATED_ANGLERS_FIELD_LIST );

	guide_anglers_output_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_quote_comma(
			last_name,
			input_string,
			GUIDE_LAST_NAME_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece last name in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			first_name,
			input_string,
			GUIDE_FIRST_NAME_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece first name code in (%s)\n",
				input_string );
			continue;
		}

		guide_angler_name =
			creel_load_library_get_guide_angler_name(
				first_name,
				last_name,
				(char *)0 /* application_name */,
				(char *)0 /* permit_number_string */ );

		if ( !piece_quote_comma(
			permit_code,
			input_string,
			GUIDE_ABBREVIATED_PERMIT_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece permit code in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_quote_comma(
			date_issued_mdyy,
			input_string,
			GUIDE_ABBREVIATED_DATE_ISSUED_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece date issued in (%s)\n",
				input_string );
			continue;
		}

		if ( !*date_issued_mdyy )
		{
			*date_issued_international = '\0';
		}
		else
		{
			date_convert_source_american(
				date_issued_international,
				international,
				date_issued_mdyy );

			if ( !*date_issued_international )
			{
				fprintf(error_file,
				"Warning: invalid date format = (%s)\n",
					date_issued_mdyy );
				continue;
			}
		}

		if ( !piece_quote_comma(
			date_expired_mdyy,
			input_string,
			GUIDE_ABBREVIATED_DATE_EXPIRED_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece date expired in (%s)\n",
				input_string );
			continue;
		}

		if ( !*date_expired_mdyy )
		{
			*date_expired_international = '\0';
		}
		else
		{
			date_convert_source_american(
				date_expired_international,
				international,
				date_expired_mdyy );

			if ( !*date_expired_international )
			{
				fprintf(error_file,
				"Warning: invalid date format = (%s)\n",
					date_expired_mdyy );
				continue;
			}
		}

		fprintf(guide_anglers_output_pipe,
			"%s|%s|%s|%s\n",
			guide_angler_name,
			date_issued_international,
			date_expired_international,
			permit_code );

	}

	fclose( input_file );
	fclose( error_file );
	pclose( guide_anglers_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Guide Angler Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) ){};
	}

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};
}

void update_permits(	char *application_name,
			char *input_filename,
			boolean abbreviated_columns )
{
	FILE *input_file;
	FILE *fishing_permits_output_pipe;
	char input_string[ 1024 ];
	char first_name[ 256 ];
	char last_name[ 256 ];
	char *guide_angler_name;
	char permit_code[ 256 ];
	char date_issued_mdyy[ 256 ];
	char date_issued_international[ 256 ];
	char date_expired_mdyy[ 256 ];
	char date_expired_international[ 256 ];
	char sys_string[ 1024 ];
	char *table_name;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	table_name = get_table_name( application_name, "permits" );

	sprintf( sys_string,
"update_statement.e t=%s k=%s c=y | sql.e 2>&1",
		 table_name,
		 "permit_code" );
	fishing_permits_output_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_quote_comma(
			last_name,
			input_string,
			GUIDE_LAST_NAME_PIECE ) )
		{
			continue;
		}

		if ( !piece_quote_comma(
			first_name,
			input_string,
			GUIDE_FIRST_NAME_PIECE ) )
		{
			continue;
		}

		guide_angler_name =
			creel_load_library_get_guide_angler_name(
				first_name,
				last_name,
				(char *)0 /* application_name */,
				(char *)0 /* permit_number_string */ );

		if ( abbreviated_columns )
		{
			if ( !piece_quote_comma(
				permit_code,
				input_string,
				GUIDE_ABBREVIATED_PERMIT_CODE_PIECE ) )
			{
				continue;
			}
	
			if ( !piece_quote_comma(
				date_issued_mdyy,
				input_string,
				GUIDE_ABBREVIATED_DATE_ISSUED_PIECE ) )
			{
				continue;
			}
	
			if ( !piece_quote_comma(
				date_expired_mdyy,
				input_string,
				GUIDE_ABBREVIATED_DATE_EXPIRED_PIECE ) )
			{
				continue;
			}
		}
		else
		{
			if ( !piece_quote_comma(
				permit_code,
				input_string,
				GUIDE_PERMIT_CODE_PIECE ) )
			{
				continue;
			}
	
			if ( !piece_quote_comma(
				date_issued_mdyy,
				input_string,
				GUIDE_DATE_ISSUED_PIECE ) )
			{
				continue;
			}
	
			if ( !piece_quote_comma(
				date_expired_mdyy,
				input_string,
				GUIDE_DATE_EXPIRED_PIECE ) )
			{
				continue;
			}
		}

		*date_issued_international = '\0';
		date_convert_source_american(
				date_issued_international,
				international,
				date_issued_mdyy );

		*date_expired_international = '\0';
		date_convert_source_american(
				date_expired_international,
				international,
				date_expired_mdyy );

		fprintf(fishing_permits_output_pipe,
			"%s^guide_angler_name^%s\n",
			permit_code,
			guide_angler_name );

		if ( *date_issued_international )
		{
			fprintf(fishing_permits_output_pipe,
				"%s^issued_date^%s\n",
				permit_code,
				date_issued_international );
		}

		if ( *date_expired_international )
		{
			fprintf(fishing_permits_output_pipe,
				"%s^expiration_date^%s\n",
				permit_code,
				date_expired_international );
		}
	}

	fclose( input_file );
	pclose( fishing_permits_output_pipe );
}

