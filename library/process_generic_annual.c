/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_annual.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "query.h"
#include "dictionary.h"
#include "attribute.h"
#include "appaserver.h"
#include "appaserver_user.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "accumulate_boolean.h"
#include "aggregate_level.h"
#include "document.h"
#include "folder.h"
#include "environ.h"
#include "units.h"
#include "grace.h"
#include "date_convert.h"
#include "lookup_statistic.h"
#include "process_generic_annual.h"

PROCESS_GENERIC_ANNUAL_DATE *process_generic_annual_date_new(
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC_ANNUAL_DATE *process_generic_annual_date;
	char *get;

	if ( !post_dictionary )
	{
		char message[ 256 ];

		sprintf(message, "post_dictionary is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_annual_date = process_generic_annual_date_calloc();

	if ( ! ( get =
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				PROCESS_GENERIC_ANNUAL_DATE_BEGIN_LABEL,
				post_dictionary ) ) )
	{
		return (PROCESS_GENERIC_ANNUAL_DATE *)0;
	}

	process_generic_annual_date->begin_month_integer =
		process_generic_annual_date_month_integer(
			get /* month_day_string */ );

	process_generic_annual_date->begin_day_integer =
		process_generic_annual_date_day_integer(
			get /* month_day_string */ );

	if ( ! ( get =
			dictionary_get(
				PROCESS_GENERIC_ANNUAL_DATE_END_LABEL,
				post_dictionary ) ) )
	{
		return (PROCESS_GENERIC_ANNUAL_DATE *)0;
	}

	process_generic_annual_date->end_month_integer =
		process_generic_annual_date_month_integer(
			get /* month_day_string */ );

	process_generic_annual_date->end_day_integer =
		process_generic_annual_date_day_integer(
			get /* month_day_string */ );

	if ( !process_generic_annual_date->begin_month_integer
	||   !process_generic_annual_date->begin_day_integer
	||   !process_generic_annual_date->end_month_integer
	||   !process_generic_annual_date->end_day_integer )
	{
		return (PROCESS_GENERIC_ANNUAL_DATE *)0;
	}

	process_generic_annual_date->begin_year_integer =
		string_atoi(
			dictionary_get(
				PROCESS_GENERIC_ANNUAL_DATE_BEGIN_YEAR,
				post_dictionary ) );

	process_generic_annual_date->end_year_integer =
		string_atoi(
			dictionary_get(
				PROCESS_GENERIC_ANNUAL_DATE_END_YEAR,
				post_dictionary ) );

	if (	!process_generic_annual_date->begin_year_integer
	||	process_generic_annual_date->begin_year_integer >
		process_generic_annual_date->end_year_integer )
	{
		return (PROCESS_GENERIC_ANNUAL_DATE *)0;
	}

	process_generic_annual_date->year_integer_list =
		process_generic_annual_date_year_integer_list(
			process_generic_annual_date->begin_year_integer,
			process_generic_annual_date->end_year_integer );

	if ( !list_length( process_generic_annual_date->year_integer_list ) )
	{
		return (PROCESS_GENERIC_ANNUAL_DATE *)0;
	}

	process_generic_annual_date->day_range =
		process_generic_annual_date_day_range(
			process_generic_annual_date->begin_month_integer,
			process_generic_annual_date->begin_day_integer,
			process_generic_annual_date->end_month_integer,
			process_generic_annual_date->end_day_integer,
			PROCESS_GENERIC_ANNUAL_DATE_LEAP_YEAR );

	if ( !process_generic_annual_date->day_range )
	{
		return (PROCESS_GENERIC_ANNUAL_DATE *)0;
	}

	process_generic_annual_date->year_range =
		process_generic_annual_date_year_range(
			process_generic_annual_date->begin_year_integer,
			process_generic_annual_date->end_year_integer );

	process_generic_annual_date->begin_date =
		date_new(
			process_generic_annual_date->begin_year_integer,
			process_generic_annual_date->begin_month_integer,
			process_generic_annual_date->begin_day_integer );

	process_generic_annual_date->end_date =
		date_new(
			process_generic_annual_date->end_year_integer,
			process_generic_annual_date->end_month_integer,
			process_generic_annual_date->end_day_integer );

	return process_generic_annual_date;
}

PROCESS_GENERIC_ANNUAL_DATE *process_generic_annual_date_calloc( void )
{
	PROCESS_GENERIC_ANNUAL_DATE *process_generic_annual_date;

	if ( ! ( process_generic_annual_date =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_ANNUAL_DATE ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_annual_date;
}

int process_generic_annual_date_month_integer( char *month_day_string )
{
	char month_string[ 128 ];

	if ( !month_day_string ) return 0;

	piece( month_string, '-', month_day_string, 0 );
	return atoi( month_string );
}

int process_generic_annual_date_day_integer( char *month_day_string )
{
	char day_string[ 128 ];

	if ( !month_day_string ) return 0;

	piece( day_string, '-', month_day_string, 1 );
	return atoi( day_string );
}

LIST *process_generic_annual_date_year_integer_list(
			int begin_year_integer,
			int end_year_integer )
{
	LIST *list;
	int year;
	int *year_ptr;

	if ( !begin_year_integer
	||   begin_year_integer > end_year_integer )
	{
		return (LIST *)0;
	}

	list = list_new();

	for (	year = begin_year_integer;
		year <= end_year_integer;
		year++ )
	{
		year_ptr = malloc( sizeof ( int ) );
		*year_ptr = year;
		list_set( list, year_ptr );
	}

	return list;
}

char *process_generic_link_extension( enum output_medium output_medium )
{
	if ( output_medium == spreadsheet )
		return "csv";
	else
	if ( output_medium == gracechart )
		return "pdf";
	else
	if ( output_medium == text_file )
		return "txt";
	else
	if ( output_medium == googlechart )
		return "html";
	else
		return (char *)0;
}

PROCESS_GENERIC_LINK *process_generic_link_new(
			char *application_name,
			char *document_root,
			char *filename_stem,
			pid_t process_id,
			char *process_generic_link_extension )
{
	PROCESS_GENERIC_LINK *process_generic_link;

	if ( !application_name
	||   !document_root
	||   !filename_stem
	||   !process_generic_link_extension )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_link = process_generic_link_calloc();

	process_generic_link->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			document_root,
			filename_stem,
			application_name,
			process_id,
			(char *)0 /* session_key */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			process_generic_link_extension );

	if ( strcmp(
		process_generic_link_extension,
		"pdf" ) == 0 )
	{
		process_generic_link->agr_appaserver_link =
			appaserver_link_new(
				application_http_prefix(
					application_ssl_support_boolean(
						application_name ) ),
				application_server_address(),
				document_root,
				filename_stem,
				application_name,
				process_id,
				(char *)0 /* session_key */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"agr" /* extension */ );
	}

	return process_generic_link;
}

PROCESS_GENERIC_LINK *process_generic_link_calloc( void )
{
	PROCESS_GENERIC_LINK *process_generic_link;

	if ( ! ( process_generic_link =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_LINK ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_link;
}

int process_generic_annual_date_day_range(
			int begin_month_integer,
			int begin_day_integer,
			int end_month_integer,
			int end_day_integer,
			int process_generic_annual_date_leap_year )
{
	DATE *begin_date;
	DATE *end_date;
	char date_string[ 16 ];
	int day_range;

	sprintf(date_string,
		"%d-%d-%d",
		process_generic_annual_date_leap_year,
		begin_month_integer,
		begin_day_integer );

	begin_date =
		date_yyyy_mm_dd_new(
			date_string );

	sprintf(date_string,
		"%d-%d-%d",
		process_generic_annual_date_leap_year,
		end_month_integer,
		end_day_integer );

	end_date =
		date_yyyy_mm_dd_new(
			date_string );

	day_range = date_subtract_days( end_date, begin_date ) + 1;

	if ( day_range < 0 )
		return 0;
	else
		return day_range;
}

int process_generic_annual_date_year_range(
			int begin_year_integer,
			int end_year_integer )
{
	return (end_year_integer + 1) - begin_year_integer;
}

boolean process_generic_input_date_is_date_time(
			LIST *folder_attribute_list,
			char *date_attribute_name )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !date_attribute_name )
	{
		char message[ 256 ];

		sprintf(message, "date_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( folder_attribute =
			folder_attribute_seek(
				date_attribute_name,
				folder_attribute_list ) ) )
	{
		char message[ 256 ];

		sprintf(message,
			"folder_attribute_seek(%s) returned empty.",
			date_attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !folder_attribute->attribute )
	{
		char message[ 256 ];

		sprintf(message, "folder_attribute->attribute is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	attribute_is_date_time(
		folder_attribute->
			attribute->
			datatype_name );
}

PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix_fetch(
			char *application_name,
			PROCESS_GENERIC *process_generic,
			PROCESS_GENERIC_ANNUAL_DATE *
				process_generic_annual_date )
{
	PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix;
	int year;
	char *begin_date_string;
	char *end_date_string;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !application_name
	||   !process_generic
	||   !process_generic->process_generic_value
	||   !process_generic->
		process_generic_value->
		process_generic_datatype
	||   !process_generic->
		process_generic_value->
		process_generic_foreign_folder
	||   !process_generic->
		process_generic_value->
		process_generic_foreign_folder->
		folder
	||   !process_generic->process_generic_input
	||   !process_generic_annual_date )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_annual_matrix =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_annual_matrix_new(
			process_generic_annual_date->year_range,
			process_generic_annual_date->day_range );

	for (	year = process_generic_annual_date->begin_year_integer;
		year <= process_generic_annual_date->end_year_integer;
		year++ )
	{
		begin_date_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_generic_annual_matrix_date_string(
				process_generic_annual_date->
					begin_month_integer,
				process_generic_annual_date->
					begin_day_integer,
				year );

		end_date_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_generic_annual_matrix_date_string(
				process_generic_annual_date->
					end_month_integer,
				process_generic_annual_date->
					end_day_integer,
				year );

		process_generic->date_where =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_generic_date_where(
				process_generic->
					process_generic_value->
					date_attribute_name,
				begin_date_string,
				end_date_string,
				process_generic->
					process_generic_input->
					date_is_date_time );

		process_generic->process_generic_where =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_generic_where_new(
				process_generic->
					process_generic_value->
					process_generic_foreign_folder->
					folder->
					folder_attribute_primary_key_list
					/* where_attribute_name_list */,
				process_generic->
					process_generic_value->
					process_generic_foreign_folder->
					data_list
					/* where_attribute_data_list */,
				process_generic->
					process_generic_value->
					process_generic_foreign_folder->
					folder->
					folder_attribute_primary_list,
				process_generic->date_where );

		process_generic->system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_generic_system_string(
				application_name,
				process_generic->select,
				process_generic->value_folder_name,
				process_generic->process_generic_where->string,
				process_generic->
					process_generic_input->
					aggregate_level,
				process_generic->
					process_generic_input->
					aggregate_statistic,
				process_generic->
					process_generic_input->
					accumulate_piece,
				process_generic->
					process_generic_input->
					date_piece,
				process_generic->
					process_generic_input->
					time_piece,
				process_generic->
					process_generic_input->
					value_piece,
				end_date_string,
				process_generic->
					process_generic_value->
					units_name,
				process_generic->
					process_generic_input->
					units_converted_name,
				process_generic->
					process_generic_input->
					whisker_boolean );

		input_pipe =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_input_pipe(
				process_generic->system_string );

		free( begin_date_string );
		free( end_date_string );
		free( process_generic->system_string );

		while ( string_input( input, input_pipe, 1024 ) )
		{
			process_generic_annual_matrix_input_set(
				process_generic_annual_matrix->
					value_matrix /* in/out */,
				process_generic,
				process_generic_annual_date,
				input );
		}

		pclose( input_pipe );
	}

	return process_generic_annual_matrix;
}

void process_generic_annual_matrix_input_set(
			char ***value_matrix /* in/out */,
			PROCESS_GENERIC *process_generic,
			PROCESS_GENERIC_ANNUAL_DATE *
				process_generic_annual_date,
			char *input )
{
	PROCESS_GENERIC_POINT *process_generic_point;
	int day_offset;
	int year_offset;

	if ( !value_matrix
	||   !process_generic
	||   !process_generic->process_generic_value
	||   !process_generic->process_generic_input
	||   !process_generic_annual_date
	||   !input )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_point =
		process_generic_point_parse(
			SQL_DELIMITER,
			process_generic->
				process_generic_value->
				primary_key_list_length,
			process_generic->
				process_generic_input->
				date_piece,
			process_generic->
				process_generic_input->
				time_piece,
			process_generic->
				process_generic_input->
				value_piece,
			process_generic->
				process_generic_input->
				accumulate_piece,
			input );

	if ( process_generic_point->value_null_boolean )
	{
		process_generic_point_free( process_generic_point );
		return;
	}

	day_offset =
		process_generic_annual_matrix_day_offset(
			process_generic_annual_date->
				begin_month_integer,
			process_generic_annual_date->
				begin_day_integer,
			process_generic_point->
				date_string );

	year_offset =
		process_generic_annual_matrix_year_offset(
			process_generic_annual_date->
				begin_year_integer,
			process_generic_point->
				date_string );

	process_generic_annual_matrix_value_set(
		value_matrix /* in/out */,
		strdup( process_generic_point->value_string ),
		day_offset,
		year_offset );

	process_generic_point_free( process_generic_point );
}

PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix_new(
			int year_range,
			int day_range )
{
	PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix;
	int day;

	if ( !year_range
	||   !day_range )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_annual_matrix = process_generic_annual_matrix_calloc();

	process_generic_annual_matrix->value_matrix =
		(char ***)calloc( day_range, sizeof ( void ** ) );

	for ( day = 0; day < day_range; day++ )
	{
		process_generic_annual_matrix->value_matrix[ day ] =
			(char **)calloc( year_range, sizeof ( char * ) );
	}

	return process_generic_annual_matrix;
}

PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix_calloc( void )
{
	PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix;

	if ( ! ( process_generic_annual_matrix =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_ANNUAL_MATRIX ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_annual_matrix;
}

int process_generic_annual_matrix_day_offset(
			int begin_month_integer,
			int begin_day_integer,
			char *date_string )
{
	int day_offset;
	DATE *later_date;
	char earlier_date_string[ 16 ];
	DATE *earlier_date;

	if ( ( begin_month_integer < 1 || begin_month_integer > 12 )
	||   ( begin_day_integer < 1 || begin_day_integer > 31 )
	||   !date_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is invalid or empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	later_date =
		/* ------------------- */
		/* Trims trailing time */
		/* ------------------- */
		date_yyyy_mm_dd_new(
			date_string );

	sprintf(
		earlier_date_string,
		"%d-%d-%d",
		date_year_number( later_date ),
		begin_month_integer,
		begin_day_integer );

	earlier_date =
		date_yyyy_mm_dd_new(
			earlier_date_string );

	day_offset =
		date_subtract_days(
			later_date,
			earlier_date );

	date_free( later_date );
	date_free( earlier_date );
	return day_offset;
}

int process_generic_annual_matrix_year_offset(
			int begin_year_integer,
			char *date_string )
{
	int current_year_integer;

	if ( !date_string )
	{
		char message[ 256 ];

		sprintf(message, "date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	current_year_integer = atoi( date_string );
	return current_year_integer - begin_year_integer;
}

void process_generic_annual_matrix_value_set(
			char ***value_matrix /* in/out */,
			char *value_string,
			int day_offset,
			int year_offset )
{
	if ( !value_matrix
	||   !value_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	value_matrix [ day_offset ] [ year_offset ] = value_string;
}

char *process_generic_annual_matrix_date_string(
			int month_integer,
			int day_integer,
			int year )
{
	char date_string[ 16 ];

	if ( !month_integer
	||   !day_integer
	||   year < 1000 )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(date_string,
		"%.4d-%.2d-%.2d",
		year,
		month_integer,
		day_integer );

	return strdup( date_string );
}

PROCESS_GENERIC_ANNUAL *process_generic_annual_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *output_medium_string,
			char *document_root,
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC_ANNUAL *process_generic_annual;

	if ( !application_name
	||   !login_name
	||   !process_name
	||   !document_root )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_annual = process_generic_annual_calloc();

	process_generic_annual->process_generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			(char *)0 /* process_set_name */ );

	process_generic_annual->process_generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary,
			process_generic_annual->
				process_generic_process_set_name );

	process_generic_annual->process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			process_generic_annual->
				process_generic_process_name,
			process_generic_annual->
				process_generic_process_set_name,
			output_medium_string,
			document_root,
			post_dictionary );

	if ( ! ( process_generic_annual->process_generic_annual_date =
			process_generic_annual_date_new(
				post_dictionary ) ) )
	{
		return process_generic_annual;
	}

	process_generic_annual->process_generic_annual_matrix =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_annual_matrix_fetch(
			application_name,
			process_generic_annual->process_generic,
			process_generic_annual->
				process_generic_annual_date );

	process_generic_annual->sub_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_annual_sub_title(
			process_generic_annual->
				process_generic->
				value_folder_name,
			process_generic_annual->
				process_generic->
				process_generic_input->
				appaserver_user_date_format_string,
			process_generic_annual->
				process_generic_annual_date->
				begin_date,
			process_generic_annual->
				process_generic_annual_date->
				end_date,
			process_generic_annual->
				process_generic->
				process_generic_input->
				aggregate_level,
			process_generic_annual->
				process_generic->
				process_generic_input->
				aggregate_statistic );

	return process_generic_annual;
}

PROCESS_GENERIC_ANNUAL *process_generic_annual_calloc( void )
{
	PROCESS_GENERIC_ANNUAL *process_generic_annual;

	if ( ! ( process_generic_annual =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_ANNUAL ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_annual;
}

void process_generic_annual_matrix_output(
			int day_range,
			int year_range,
			int begin_year_integer,
			DATE *begin_date /* in/out */,
			char ***value_matrix,
			FILE *stream_file )
{
	int day;
	int year;

	if ( !day_range
	||   !year_range
	||   !begin_year_integer
	||   !begin_date
	||   !value_matrix
	||   !stream_file )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	for (	day = 0;
		day < day_range;
		day++ )
	{
		for (	year = 0;
			year < year_range;
			year++ )
		{
			if ( value_matrix [ day ] [ year ] )
			{
				fprintf(stream_file,
					".day %d-%d\n",
					date_month_integer( begin_date ),
					date_month_day_integer( begin_date ) );

				fprintf(stream_file,
					".year %d,%s\n",
					begin_year_integer + year,
					value_matrix [ day ] [ year ] );
			}
		}

		date_increment_day( begin_date );
	}
}

char *process_generic_annual_sub_title(
			char *value_folder_name,
			char *appaserver_user_date_format_string,
			DATE *begin_date,
			DATE *end_date,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic )
{
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];

	if ( !value_folder_name
	||   !begin_date
	||   !end_date )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	process_generic_sub_title(
		value_folder_name,
		appaserver_user_date_format_string,
		date_yyyy_mm_dd(
			begin_date_string /* destination */,
			begin_date ),
		date_yyyy_mm_dd(
			end_date_string /* destination */,
			end_date ),
		aggregate_level,
		aggregate_statistic );
}

