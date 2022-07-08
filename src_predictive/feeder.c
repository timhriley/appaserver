/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "date_convert.h"
#include "sql.h"
#include "session.h"
#include "basename.h"
#include "sed.h"
#include "feeder.h"

#ifdef NOT_DEFINED
int feeder_load_file_begin_sequence_number(
			int feeder_load_file_line_count ); 
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"reference_number.sh %s %d",
		environment_application(),
		line_count );

	return atoi( pipe2string( system_string ) );
}

int feeder_load_file_line_count(
			char *feeder_load_filename,
			int date_piece_offset )
{
	char input_string[ 4096 ];
	char american_date[ 128 ];
	char *international_date;
	FILE *input_file;
	int line_count = 0;

	if ( ! ( input_file = fopen( feeder_load_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_filename );
		exit( 1 );
	}

	while( string_input( input_string, input_file, 4096 ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
				american_date,
				input_string,
				date_piece_offset ) )
		{
			continue;
		}

		if ( ( international_date =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_row_international_date(
				american_date ) ) )
		{
			line_count++;
			free( international_date );
		}
	}

	fclose( input_file );
	return line_count;
}
#endif

LIST *feeder_phrase_list( void )
{
	LIST *list;
	FILE *input_pipe;
	char input[ 1024 ];

	input_pipe =
		popen(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_phrase_system_string(
				FEEDER_PHRASE_SELECT,
				FEEDER_PHRASE_TABLE ),
			"r" );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			feeder_phrase_parse( input ) );
	}

	pclose( input_pipe );

	return list;
}

char *feeder_phrase_system_string(
			char *select,
			char *table )
{
	char system_string[ 1024 ];

	if ( !select || !table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s",
		select,
		table );

	return strdup( system_string );
}

FEEDER_PHRASE *feeder_phrase_parse( char *input )
{
	char feeder_phrase[ 128 ];
	char buffer[ 128 ];
	FEEDER_PHRASE *phrase;

	if ( !input || !*input ) return (FEEDER_PHRASE *)0;

	/* See FEEDER_PHRASE_SELECT */
	/* ------------------------ */
	piece( feeder_phrase, SQL_DELIMITER, input, 0 );

	phrase = feeder_phrase_new( strdup( feeder_phrase ) );

	if ( piece( buffer, SQL_DELIMITER, input, 1 ) )
	{
		phrase->nominal_account = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 2 ) )
	{
		phrase->full_name = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 3 ) )
	{
		phrase->street_address = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 4 ) )
	{
		phrase->feeder_phrase_ignore = ( *buffer == 'y' );
	}

	return phrase;
}

FEEDER_PHRASE *feeder_phrase_new( char *phrase )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( !phrase || !*phrase )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: phrase is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_phrase = feeder_phrase_calloc();
	feeder_phrase->phrase = phrase;

	return feeder_phrase;
}

FEEDER_PHRASE *feeder_phrase_calloc( void )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( ! ( feeder_phrase = calloc( 1, sizeof( FEEDER_PHRASE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_phrase;
}

char *feeder_load_row_trim_bank_date( char *description )
{
	static char sans_bank_date_description[ 512 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	timlib_strcpy(	sans_bank_date_description,
			description,
			512 );

	/* Why do I need to append the EOL character for sed() to work? */
	/* ------------------------------------------------------------ */
	sprintf( sans_bank_date_description +
		 strlen( sans_bank_date_description ),
		 "$" );

	if ( sed_will_replace( sans_bank_date_description, sed ) )
	{
		sed_search_replace( sans_bank_date_description, sed );
	}
	else
	{
		trim_right( sans_bank_date_description, 1 );
	}

	sed_free( sed );

	return timlib_rtrim( sans_bank_date_description );
}

int feeder_load_row_line_number( int line_number )
{
	return line_number;
}

FEEDER_LOAD_ROW *feeder_load_row_new(
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			char *input,
			int line_number )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	char buffer[ 512 ];

	if ( !date_column
	||   !description_column
	||   !input
	||   !*input
	||   !line_number )
	{
		return (FEEDER_LOAD_ROW *)0;
	}

	feeder_load_row = feeder_load_row_calloc();

	feeder_load_row->line_number =
		feeder_load_row_line_number(
			line_number );

	feeder_load_row->american_date =
		strdup(
			piece_quote_comma(
				buffer,
				input,
				date_column - 1 ) );

	if ( ! ( feeder_load_row->international_date =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_row_international_date(
				feeder_load_row->american_date ) ) )
	{
		free( feeder_load_row->american_date );
		free( feeder_load_row );
		return (FEEDER_LOAD_ROW *)0;
	}

	feeder_load_row->description =
		strdup(
			piece_quote_comma(
				buffer,
				input,
				description_column - 1 ) );

	if ( !credit_column )
	{
		feeder_load_row->amount =
			atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );
	}
	else
	{
		feeder_load_row->debit =
			atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );

		feeder_load_row->credit =
			atof(
				piece_quote_comma(
					buffer,
					input,
					credit_column - 1 ) );

		feeder_load_row->amount =
			feeder_load_row_amount(
				feeder_load_row->debit,
				feeder_load_row->credit );
	}

	if ( balance_column )
	{
		feeder_load_row->balance =
			atof(
				piece_quote_comma(
					buffer,
					input,
					balance_column - 1 ) );
	}

	feeder_load_row->description_embedded =
		feeder_load_row_description_embedded(
			feeder_load_row->description,
			line_number );

	return feeder_load_row;
}

FEEDER_LOAD_ROW *feeder_load_row_calloc( void )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( ! ( feeder_load_row = calloc( 1, sizeof( FEEDER_LOAD_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_row;
}

double feeder_load_row_amount(
			double debit,
			double credit )
{
	return debit - credit;
}

char *feeder_load_row_description_embedded(
			char *description,
			int line_number )
{

	return
	/* ----------------------------------------- */
	/* Returns feeder_load_row_description_build */
	/* ----------------------------------------- */
	feeder_load_row_description_crop(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_row_description_build(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			sed_trim_double_spaces(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_load_row_trim_bank_date(
					description ) ),
			line_number ),
		FEEDER_DESCRIPTION_SIZE );
}

char *feeder_load_row_description_build(
			char *sed_trim_double_spaces,
			int line_number )
{
	char build[ 512 ];

	if ( !sed_trim_double_spaces
	||   !line_number )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(build,
		"%s %d",
		sed_trim_double_spaces,
		line_number );

	return strdup( build );
}

char *feeder_load_row_description_crop(
			char *feeder_load_row_description_build,
			int feeder_description_size )
{
	*( feeder_load_row_description_build + feeder_description_size ) = '\0';

	return feeder_load_row_description_build;
}

double feeder_load_row_last_running_balance(
			boolean reverse_order,
			LIST *feeder_load_row_list )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( !list_length( feeder_load_row_list ) ) return 0.0;

	if ( !reverse_order )
		feeder_load_row = list_last( feeder_load_row_list );
	else
		feeder_load_row = list_first( feeder_load_row_list );

	return feeder_load_row->balance;
}

char *feeder_load_row_international_date( char *american_date )
{
	char international_date[ 128 ];

	date_convert_source_american(
		international_date,
		international,
		american_date );

	if ( date_convert_is_valid_international(
		international_date ) )
	{
		return strdup( international_date );
	}
	else
	{
		return (char *)0;
	}
}

FEEDER_LOAD_FILE *feeder_load_file_fetch(
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order )
{
	FEEDER_LOAD_FILE *feeder_load_file;
	FEEDER_LOAD_ROW *feeder_load_row;
	FILE *file;
	char input[ 1024 ];
	int line_number = 0;

	if ( !feeder_load_filename
	||   !date_column
	||   !description_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( file = fopen( feeder_load_filename, "r" ) ) )
	{
		return (FEEDER_LOAD_FILE *)0;
	}

	feeder_load_file = feeder_load_file_calloc();

	if ( ! ( feeder_load_file->line_count =
			feeder_load_file_line_count(
				feeder_load_filename,
				date_column - 1
					/* date_piece_offset */ ) ) )
	{
		return (FEEDER_LOAD_FILE *)0;
	}

	feeder_load_file->begin_sequence_number =
		feeder_load_file_begin_sequence_number(
			feeder_load_file->line_count ); 

	feeder_load_file->feeder_load_row_list = list_new();

	while ( string_input( input, file, 1024 ) )
	{
		timlib_remove_character( input, '\\' );

		feeder_load_row =
			feeder_load_row_new(
				date_column,
				description_column,
				debit_column,
				credit_column,
				balance_column,
				input,
				++line_number );

		if ( !feeder_load_row ) continue;

		if ( reverse_order )
		{
			list_set_first(
				feeder_load_file->feeder_load_row_list,
				feeder_load_row );
		}
		else
		{
			list_set(
				feeder_load_file->feeder_load_row_list,
				feeder_load_row );
		}
	}

	fclose( file );

	return feeder_load_file;
}

FEEDER_LOAD_FILE *feeder_load_file_calloc( void )
{
	FEEDER_LOAD_FILE *feeder_load_file;

	if ( ! ( feeder_load_file = calloc( 1, sizeof( FEEDER_LOAD_FILE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_file;
}

FEEDER_LOAD_EVENT *feeder_load_event_new(
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *feeder_account,
			double ending_balance,
			double feeder_load_row_last_running_balance,
			char *feeder_load_sha256sum )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !feeder_load_date_time
	||   !login_name
	||   !basename_filename
	||   !feeder_account
	||   !feeder_load_sha256sum )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_load_event = feeder_load_event_calloc();

	feeder_load_event->feeder_load_date_time = feeder_load_date_time;
	feeder_load_event->login_name = login_name;
	feeder_load_event->basename_filename = basename_filename;
	feeder_load_event->feeder_account = feeder_account;
	feeder_load_event->sha256sum = feeder_load_sha256sum;

	feeder_load_event->ending_balance =
		feeder_load_event_ending_balance(
			ending_balance,
			feeder_load_row_last_running_balance );

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_calloc( void )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( ! ( feeder_load_event =
			calloc( 1, sizeof( FEEDER_LOAD_EVENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_event;
}

double feeder_load_event_ending_balance(
			double ending_balance,
			double last_running_balance )
{
	if ( !ending_balance
	&&   !last_running_balance )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: both ending_balance and last_running_balance are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ending_balance
	&&   last_running_balance )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: both ending_balance and last_running_balance are populated.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ending_balance )
		return ending_balance;
	else
		return last_running_balance;
}

void feeder_load_event_insert(
			char *feeder_load_event_table,
			char *feeder_load_select,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance )
{
	char *system_string;
	FILE *insert_pipe;

	if ( !feeder_load_event_table
	||   !feeder_load_select
	||   !feeder_load_date_time
	||   !login_name
	||   !basename_filename
	||   !sha256sum
	||   !feeder_account
	||   !ending_balance )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	system_string =
		feeder_load_event_insert_system_string(
			feeder_load_event_table,
			feeder_load_select,
			SQL_DELIMITER );

	insert_pipe = popen( system_string, "w" );

	feeder_load_event_insert_pipe(
		insert_pipe,
		SQL_DELIMITER,
		feeder_load_date_time,
		login_name,
		basename_filename,
		sha256sum,
		feeder_account,
		ending_balance );

	pclose( insert_pipe );
}

char *feeder_load_event_insert_system_string(
			char *feeder_load_event_table,
			char *feeder_load_select,
			char sql_delimiter )
{
	char system_string[ 1024 ];

	if ( !feeder_load_event_table
	||   !feeder_load_select
	||   !sql_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
	 	"insert_statement table=%s field=%s del='%c' 		  |"
	 	"sql 2>&1						  |"
	 	"html_paragraph_wrapper.e				   ",
	 	feeder_load_event_table,
	 	feeder_load_select,
	 	sql_delimiter );

	return strdup( system_string );
}

void feeder_load_event_insert_pipe(
			FILE *insert_pipe,
			char sql_delimiter,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance )
{
	if ( !insert_pipe
	||   !sql_delimiter
	||   !feeder_load_date_time
	||   !login_name
	||   !basename_filename
	||   !sha256sum
	||   !feeder_account
	||   !ending_balance )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		pclose( insert_pipe );
		exit( 1 );
	}

	fprintf(insert_pipe,
		"%s%c%s%c%s%c%s%c%s%c%.2lf\n",
	 	feeder_load_date_time,
		sql_delimiter,
	 	login_name,
		sql_delimiter,
		basename_filename,
		sql_delimiter,
		sha256sum,
		sql_delimiter,
		feeder_account,
		sql_delimiter,
		ending_balance );
}

char *feeder_load_basename_filename( char *feeder_load_filename )
{
	char filename[ 256 ];
	int session_characters_to_trim;
	char *start_trimming_here;

	string_strcpy(
		filename,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		basename_filename( feeder_load_filename ),
		256 );

	/* Trim off _$SESSION */
	/* ------------------ */
	session_characters_to_trim = SESSION_MAX_SESSION_SIZE + 1;

	start_trimming_here =
		filename + strlen( filename ) - session_characters_to_trim;

	*start_trimming_here = '\0';

	return strdup( filename );
}

FEEDER_LOAD *feeder_load_new(
			char *process_name,
			char *login_name,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order,
			double ending_balance )
{
	FEEDER_LOAD *feeder_load;

	if ( !process_name
	||   !login_name
	||   !feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0
	||   !feeder_load_filename
	||   !date_column
	||   !description_column
	||   !debit_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_load = feeder_load_calloc();

	feeder_load->feeder_load_file =
		feeder_load_file_fetch(
			feeder_load_filename,
			date_column,
			description_column,
			debit_column,
			credit_column,
			balance_column,
			reverse_order );

	if ( !feeder_load->feeder_load_file
	||   !list_length(
		feeder_load->
			feeder_load_file->
			feeder_load_row_list ) )
	{
		free( feeder_load );
		return (FEEDER_LOAD *)0;
	}

	feeder_load->feeder_phrase_list =
		feeder_phrase_list();

	feeder_load->feeder_load_row_last_running_balance =
		feeder_load_row_last_running_balance(
			reverse_order,
			feeder_load->feeder_load_file->feeder_load_row_list );

	feeder_load->basename_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_basename_filename(
			feeder_load_filename );

	feeder_load->sha256sum =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_sha256sum(
			feeder_load_filename );

	feeder_load->feeder_load_event =
		feeder_load_event_new(
			date_display_19(
				date_now_new(
					date_utc_offset()
						/* feeder_load_date_time */ ) ),
			login_name,
			feeder_account,
			feeder_load->basename_filename,
			ending_balance,
			feeder_load->feeder_load_row_last_running_balance,
			feeder_load->sha256sum );

	return feeder_load;
}

FEEDER_LOAD *feeder_load_calloc( void )
{
	FEEDER_LOAD *feeder_load;

	if ( ! ( feeder_load = calloc( 1, sizeof( FEEDER_LOAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load;
}

char *feeder_load_sha256sum( char *feeder_load_filename )
{
	if ( !feeder_load_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	timlib_sha256sum( feeder_load_filename );
}

FEEDER_PHRASE *feeder_phrase_seek(
			char *feeder_description,
			LIST *feeder_phrase_list )
{
	FEEDER_PHRASE *feeder_phrase;
	char feeder_component[ 128 ];
	int piece_number;

	if ( !list_rewind( feeder_phrase_list ) ) return (FEEDER_PHRASE *)0;

	do {
		feeder_phrase = list_get( feeder_phrase_list );

		for(	piece_number = 0;
			piece(	feeder_component,
				'|',
				feeder_phrase->feeder_phrase,
				piece_number );
			piece_number++ )
		{
			if ( string_exists_substr(
				feeder_description /* string */,
				feeder_component /* substring */ ) )
			{
				return feeder_phrase;
			}
		}

	} while ( list_next( feeder_phrase_list ) );

	return (FEEDER_PHRASE *)0;
}

LIST *feeder_load_table_row_list(
			char *feeder_account,
			char *feeder_load_file_minimum_date )
{
	LIST *list;
	char *system_string;
	FILE *input_pipe;
	char input[ 512 ];

	if ( !feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0
	||   !feeder_load_file_minimum_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_table_row_system_string(
			FEEDER_LOAD_TABLE_ROW_SELECT,
			FEEDER_LOAD_ROW_TABLE,
			/* ------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_load_table_row_where(
				feeder_account,
				feeder_load_file_minimum_date ) );

	list = list_new()

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 512 ) )
	{
		list_set(
			list,
			feeder_load_table_row_parse(
				input ) );
	}

	pclose( input_pipe );
	free( system_string );

	return list;
}

char *feeder_load_table_row_where(
			char *feeder_account,
			char *feeder_load_file_minimum_date )
{
	static char where[ 128 ];

	if ( !feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0
	||   !feeder_load_file_minimum_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"feeder_account = '%s' and "
		"feeder_date >= '%s'",
		feeder_account,
		feeder_load_file_minimum_date );

	return where;
}

char *feeder_load_table_row_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !select
	||   !table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		table,
		where );

	return strdup( system_string );
}

FEEDER_LOAD_TABLE_ROW *feeder_load_table_row_parse( char *input )
{
	FEEDER_LOAD_TABLE_ROW *feeder_load_table_row;
	char buffer[ 128 ];

	if ( !input || !*input ) return (FEEDER_LOAD_TABLE_ROW *)0;

	feeder_load_table_row = feeder_load_table_row_calloc();

	/* See FEEDER_LOAD_TABLE_ROW_SELECT */
	/* -------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_load_table_row->feeder_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_load_table_row->feeder_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_load_table_row->transaction_date_time = strdup( buffer );

	return feeder_load_table_row;
}

FEEDER_LOAD_TABLE_ROW *feeder_load_table_row_calloc( void )
{
	FEEDER_LOAD_TABLE_ROW *feeder_load_table_row;

	if ( ! ( feeder_load_table_row =
			calloc( 1, sizeof( FEEDER_LOAD_TABLE_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_table_row;
}

FEEDER_LOAD_TABLE_ROW *feeder_load_table_row_seek(
			char *international_date,
			char *description_embedded,
			LIST *feeder_load_table_row_list )
{
	FEEDER_LOAD_TABLE_ROW *feeder_load_table_row;

	if ( !international_date
	||   !description_embeded )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_load_table_row_list ) )
		return (FEEDER_LOAD_TABLE_ROW *)0;

	do {
		feeder_load_table_row =
			list_get(
				feeder_load_table_row_list );

		if ( strcmp(	feeder_load_table_row->feeder_date,
				internation_date ) == 0
		&&   strcmp(	feeder_load_table_row->feeder_description,
				description_embedded ) == 0 )
		{
			return feeder_load_table_row;
		}

	} while ( list_next( feeder_load_table_row_list ) );

	return (FEEDER_LOAD_TABLE_ROW *)0;
}

