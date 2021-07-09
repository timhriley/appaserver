/* ---------------------------------------------------	*/
/* src_timriley/load_institution_balance.c		*/
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
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "basename.h"
#include "application.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define FOLDER_NAME		"account_balance"
#define FIELD_LIST		"institution,account_number,date,balance"
#define FORMAT_LIST		"left,left,left,right"
#define OUTPUT_DELIMITER	'|'

/* Prototypes */
/* ---------- */
DICTIONARY *get_account_number_dictionary(
			char *application_name,
			char *institution );

void trim_control_characters(
			char *account_number );

char *get_date_string(	void );

boolean load_institution_balance(
			char *application_name,
			char *institution,
			char *input_filename,
			char execute_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char execute_yn;
	char *input_filename;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *institution;
	char *database_string = {0};
	DOCUMENT *document;
	char buffer[ 128 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s ignored process institution filename execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	institution = argv[ 3 ];
	input_filename = argv[ 4 ];
	execute_yn = *argv[ 5 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );
	document->output_content_type = 1;
	document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );

	document_output_body(
				document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n",
		format_initial_capital( buffer, process_name ) );
	fflush( stdout );

	if ( !load_institution_balance(
			application_name,
			institution,
			input_filename,
			execute_yn ) )
	{
		printf( "<h3>An error occurred.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( execute_yn == 'y' )
	{
		printf( "<h3>Process complete.</h3>\n" );
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	document_close();
	exit( 0 );
}

boolean load_institution_balance(
			char *application_name,
			char *institution,
			char *input_filename,
			char execute_yn )
{
	FILE *input_file;
	char account_number[ 128 ];
	char balance[ 128 ];
	char *date_string;
	char input_string[ 1024 ];
	FILE *output_pipe = {0};
	DICTIONARY *account_number_dictionary;

	date_string = get_date_string();

	account_number_dictionary =
		get_account_number_dictionary(
			application_name, institution );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			input_filename );
		return 0;
	}

	if ( execute_yn != 'y')
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
		 	"html_table.e '' %s '%c' %s",
			FIELD_LIST,
			OUTPUT_DELIMITER,
			FORMAT_LIST );

		output_pipe = popen( sys_string, "w" );
	}
	else
	{
		char sys_string[ 1024 ];
		char *table_name;

		table_name =
			get_table_name(
				application_name,
				"account_balance" );

		sprintf( sys_string,
		 	"insert_statement.e table=%s field=%s del='%c'	|"
			"sql.e 2>&1					|"
			"html_paragraph_wrapper.e			 ",
			table_name,
			FIELD_LIST,
			OUTPUT_DELIMITER );

		output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_file ) )
	{
		if ( ! ( piece_quote_comma(
				account_number,
				input_string,
				0 ) ) )
		{
			fclose( input_file );
			if ( output_pipe ) pclose( output_pipe );
			return 0;
		}

		trim_control_characters( account_number );

		if ( ! dictionary_key_exists(
					account_number_dictionary,
					account_number ) )
		{	
			fclose( input_file );
			if ( output_pipe ) pclose( output_pipe );

			printf( "<h3>Cannot find account = %s</h3>\n",
			account_number );

			return 0;
		}

		if ( ! ( piece_quote_comma(
				balance,
				input_string,
				1 ) ) )
		{
			fclose( input_file );
			return 0;
		}

		if ( output_pipe )
		{
			fprintf( output_pipe,
				 "%s%c%s%c%s%c%s\n",
				 institution,
				 OUTPUT_DELIMITER,
				 account_number,
				 OUTPUT_DELIMITER,
				 date_string,
				 OUTPUT_DELIMITER,
				 string_trim_number_characters(
					balance,
					"float" /* attribute_datatype */ ) );
		}

	}

	if ( output_pipe ) pclose( output_pipe );

	fclose( input_file );

	return 1;
}

char *get_date_string( void )
{
	return pipe2string( "date.e 0 | piece.e ':' 0" );
}

void trim_control_characters( char *account_number )
{
	char c;

	while( *account_number )
	{
		c = *account_number;
		if ( c < 0 || c > 127 )
		{
			*account_number = '\0';
			return;
		}
		account_number++;
	}
}

DICTIONARY *get_account_number_dictionary(
			char *application_name,
			char *institution )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf( where, "institution = '%s'", institution );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account_number	"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return pipe2dictionary( sys_string, ',' );

}
