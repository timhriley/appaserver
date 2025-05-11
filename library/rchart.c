/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rchart.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "date.h"
#include "document.h"
#include "application.h"
#include "chart.h"
#include "rchart.h"

LIST *rchart_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list )
{
	LIST *point_list = list_new();
	QUERY_ROW *query_row;
	QUERY_CELL *number_query_cell;
	QUERY_CELL *date_query_cell;
	QUERY_CELL *time_query_cell = {0};
	char *date_time_string;
	double value;
	RCHART_POINT *rchart_point;

	if ( list_rewind( query_fetch_row_list ) )
	do {
		query_row = list_get( query_fetch_row_list );

		if ( ! ( number_query_cell =
				query_cell_seek(
					number_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"query_cell_seek(%s) returned empty.",
				number_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( rchart_point_null_boolean(
			number_query_cell->select_datum ) )
				continue;

		if ( ! ( date_query_cell =
				query_cell_seek(
					date_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"query_cell_seek(%s) returned empty.",
				date_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( time_attribute_name )
		{
			time_query_cell =
				query_cell_seek(
					time_attribute_name,
					query_row->cell_list );
		}

		date_time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			rchart_point_date_time_string(
				date_query_cell,
				time_query_cell );

		value = rchart_point_value( number_query_cell );

		rchart_point =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			rchart_point_new(
				date_time_string,
				value );

		list_set( point_list, rchart_point );

	} while ( list_next( query_fetch_row_list ) );

	if ( !list_length( point_list ) )
	{
		list_free( point_list );
		point_list = NULL;
	}

	return point_list;
}

boolean rchart_point_null_boolean( char *select_datum )
{
	if ( select_datum && *select_datum ) 
		return 0;
	else
		return 1;
}

char *rchart_point_date_time_string(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell )
{
	char date_time_string[ 128 ];

	if ( !date_query_cell )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_query_cell is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy( date_time_string, date_query_cell->select_datum );

	if ( time_query_cell )
	{
		sprintf(
			date_time_string + strlen( date_time_string ),
			" %s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			rchart_point_hhmmss(
				time_query_cell ) );
	}

	return strdup( date_time_string );
}

char *rchart_point_hhmmss( QUERY_CELL *time_query_cell )
{
	char *select_datum = time_query_cell->select_datum;
	char destination[ 4 ];
	static char hhmmss[ 9 ];
	int how_many;

	/* Set hour: */
	/* --------- */
	if ( strlen( select_datum ) == 3 )
	{
		how_many = 1;

		sprintf(hhmmss,
			"0%s:",
			string_left(
				destination /* destination */,
				select_datum /* source */,
				how_many ) );
	}
	else
	{
		how_many = 2;

		sprintf(hhmmss,
			"%s:",
			string_left(
				destination /* destination */,
				select_datum /* source */,
				how_many ) );
	}

	/* Set minute:second */
	/* ----------------- */
	sprintf(
		hhmmss + strlen( hhmmss ),
		"%s:00",
		select_datum + how_many );

	return hhmmss;
}

RCHART_POINT *rchart_point_new(
		char *date_time_string,
		double rchart_point_value )
{
	RCHART_POINT *rchart_point;

	if ( !date_time_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_time_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rchart_point = rchart_point_calloc();

	rchart_point->date_time_string = date_time_string;
	rchart_point->value = rchart_point_value;

	return rchart_point;
}

RCHART_POINT *rchart_point_calloc( void )
{
	RCHART_POINT *rchart_point;

	if ( ! ( rchart_point = calloc( 1, sizeof ( RCHART_POINT ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rchart_point;
}

char *rchart_point_combine_string(
		char *attribute_name,
		LIST *rchart_point_list,
		boolean date_time_boolean )
{
	char combine_string[ STRING_64K ];
	char *ptr = combine_string;
	register int first_time = 1;
	RCHART_POINT *rchart_point;

	if ( !attribute_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
		"%s <- c(\n",
		attribute_name );

	if ( list_rewind( rchart_point_list ) )
	do {
		rchart_point = list_get( rchart_point_list );

		if ( string_strlen( combine_string ) + 2 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( first_time )
			first_time = 0;
		else
		if ( !list_last_boolean( rchart_point_list ) )
			ptr += sprintf( ptr, ",\n" );

		ptr += sprintf(
			ptr,
			"%s",
		/* Returns rchart_point_date_time_string or static memory */
		/* ------------------------------------------------------ */
			rchart_point_string(
				rchart_point->date_time_string,
				rchart_point->value,
				date_time_boolean ) );

	} while ( list_next( rchart_point_list ) );

	ptr += sprintf( ptr, ")" );

	if ( first_time )
		return NULL;
	else
		return strdup( combine_string );
}

char *rchart_point_string(
		char *date_time_string,
		double value,
		boolean date_time_boolean )
{
	static char point_string[ 128 ];

	if ( date_time_boolean )
	{
		if ( !date_time_string )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"date_time_string is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return date_time_string;
	}

	snprintf(
		point_string,
		sizeof ( point_string ),
		"%.2lf",
		value );

	return point_string;
}

double rchart_point_value( QUERY_CELL *number_query_cell )
{
	if ( !number_query_cell )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"number_query_cell is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return atof( number_query_cell->select_datum );
}

RCHART_PROMPT *rchart_prompt_new(
		RCHART_FILENAME *rchart_filename,
		char *filename_key,
		pid_t process_id )
{
	RCHART_PROMPT *rchart_prompt;

	if ( !rchart_filename
	||   !filename_key
	||   !process_id )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rchart_prompt = rchart_prompt_calloc();

	rchart_prompt->pdf_target =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rchart_prompt_pdf_target(
			filename_key,
			process_id );

	rchart_prompt->pdf_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_prompt_pdf_anchor_html(
			rchart_filename->pdf_prompt_filename,
			filename_key,
			rchart_prompt->pdf_target );

	rchart_prompt->R_target =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rchart_prompt_R_target(
			filename_key,
			process_id );

	rchart_prompt->R_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_prompt_R_anchor_html(
			rchart_filename->R_prompt_filename,
			filename_key,
			rchart_prompt->R_target );

	rchart_prompt->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_prompt_html(
			rchart_prompt->pdf_anchor_html,
			rchart_prompt->R_anchor_html );

	return rchart_prompt;
}

RCHART_PROMPT *rchart_prompt_calloc( void )
{
	RCHART_PROMPT *rchart_prompt;

	if ( ! ( rchart_prompt = calloc( 1, sizeof ( RCHART_PROMPT ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rchart_prompt;
}

char *rchart_prompt_pdf_anchor_html(
		char *pdf_prompt_filename,
		char *filename_key,
		char *rchart_prompt_pdf_target )
{
	static char html[ 256 ];
	char *ptr = html;

	if ( !pdf_prompt_filename
	||   !rchart_prompt_pdf_target )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
"<a href=\"%s\" target=\"%s\">View ",
			pdf_prompt_filename,
			rchart_prompt_pdf_target );

	if ( filename_key )
	{
		char buffer[ 128 ];

		ptr += sprintf( ptr,
			"%s ",
			string_initial_capital(
				buffer,
				filename_key ) );
	}

	sprintf( ptr, "chart</a>" );

	return html;
}

char *rchart_prompt_R_anchor_html(
		char *R_prompt_filename,
		char *filename_key,
		char *rchart_prompt_R_target )
{
	static char html[ 256 ];
	char *ptr = html;

	if ( !R_prompt_filename
	||   !rchart_prompt_R_target )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
"<a href=\"%s\" target=\"%s\">View ",
			R_prompt_filename,
			rchart_prompt_R_target );

	if ( filename_key )
	{
		char buffer[ 128 ];

		ptr += sprintf( ptr,
			"%s ",
			string_initial_capital(
				buffer,
				filename_key ) );
	}

	sprintf( ptr, "R chart</a>" );

	return html;
}

char *rchart_prompt_html(
		char *pdf_anchor_html,
		char *R_anchor_html )
{
	char html[ 1024 ];

	snprintf(
		html,
		sizeof ( html ),
		"<tr><td>%s<td>%s",
		pdf_anchor_html,
		R_anchor_html );

	return strdup( html );
}

RCHART_FILENAME *rchart_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *chart_filename_key )
{
	RCHART_FILENAME *rchart_filename;

	if ( !application_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rchart_filename = rchart_filename_calloc();

	rchart_filename->process_id =
		rchart_filename_process_id(
			session_key );

	rchart_filename->appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			data_directory,
			RCHART_FILENAME_STEM,
			application_name,
			rchart_filename->process_id,
			session_key,
			chart_filename_key /* begin_date_string */,
			(char *)0 /* end_date_string */,
			(char *)0 /* extension */ );

	rchart_filename->R_output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_filename_R_output_filename(
			rchart_filename->appaserver_link );

	rchart_filename->R_prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_filename_R_prompt_filename(
			rchart_filename->appaserver_link );

	rchart_filename->pdf_output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_filename_pdf_output_filename(
			rchart_filename->appaserver_link );

	rchart_filename->pdf_prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_filename_pdf_prompt_filename(
			rchart_filename->appaserver_link );

	return rchart_filename;
}

RCHART_FILENAME *rchart_filename_calloc( void )
{
	RCHART_FILENAME *rchart_filename;

	if ( ! ( rchart_filename = calloc( 1, sizeof ( RCHART_FILENAME ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rchart_filename;
}

pid_t rchart_filename_process_id( char *session_key )
{
	if ( session_key && *session_key )
		return (pid_t)0;
	else
		return getpid();
}

char *rchart_filename_R_output_filename(
		APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *output_filename;

	if ( !appaserver_link )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"R" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_output_filename(
			appaserver_link->appaserver_parameter_data_directory,
			tail_half );

	free( link_filename );
	free( tail_half );

	return output_filename;
}

char *rchart_filename_R_prompt_filename(
		APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *prompt_filename;

	if ( !appaserver_link
	||   !appaserver_link->appaserver_link_prompt )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"R" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_prompt_filename(
			appaserver_link->appaserver_link_prompt->link_half,
			tail_half );

	free( link_filename );
	free( tail_half );

	return prompt_filename;
}

char *rchart_filename_pdf_output_filename(
		APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *output_filename;

	if ( !appaserver_link )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"pdf" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_output_filename(
			appaserver_link->appaserver_parameter_data_directory,
			tail_half );

	free( link_filename );
	free( tail_half );

	return output_filename;
}

char *rchart_filename_pdf_prompt_filename(
		APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *prompt_filename;

	if ( !appaserver_link
	||   !appaserver_link->appaserver_link_prompt )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"pdf" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_prompt_filename(
			appaserver_link->appaserver_link_prompt->link_half,
			tail_half );

	free( link_filename );
	free( tail_half );

	return prompt_filename;
}

RCHART_WINDOW *rchart_window_new(
		RCHART_FILENAME *rchart_filename,
		char *sub_title,
		char *filename_key,
		pid_t process_id )
{
	RCHART_WINDOW *rchart_window;
	char *onload_open_tag;
	char *sub_title_display;

	rchart_window = rchart_window_calloc();

	if ( !process_id )
	{
		char message[ 128 ];

		sprintf(message, "process_id is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rchart_window = rchart_window_calloc();

	if ( rchart_filename )
	{
		char *pdf_target;

		pdf_target =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			rchart_prompt_pdf_target(
				filename_key,
				process_id );

		rchart_window->onload_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			chart_window_onload_string(
				rchart_filename->pdf_prompt_filename,
				pdf_target );
	}

	onload_open_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_onload_open_tag(
			JAVASCRIPT_WAIT_OVER,
			rchart_window->onload_string /* javascript_replace */,
			(char *)0 /* document_body_menu_onload_string */ );

	rchart_window->date_time_now =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_time_now( date_utc_offset() );

	rchart_window->screen_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rchart_window_screen_title(
			rchart_window->date_time_now );

	sub_title_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		chart_window_sub_title_display(
			sub_title );

	rchart_window->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		chart_window_html(
			onload_open_tag,
			rchart_window->screen_title,
			sub_title_display );

	free( onload_open_tag );
	free( sub_title_display );

	return rchart_window;
}

RCHART_WINDOW *rchart_window_calloc( void )
{
	RCHART_WINDOW *rchart_window;

	if ( ! ( rchart_window = calloc( 1, sizeof ( RCHART_WINDOW ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rchart_window;
}

char *rchart_window_screen_title( char *date_time_now )
{
	static char screen_title[ 128 ];

	if ( !date_time_now )
	{
		char message[ 128 ];

		sprintf(message, "date_time_now is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(screen_title,
		"<h1>R Chart Viewer<br>%s</h1>",
		date_time_now );

	return screen_title;
}

char *rchart_prompt_pdf_target(
		char *filename_key,
		pid_t process_id )
{
	static char target[ 128 ];

	if ( filename_key )
	{
		snprintf(
			target,
			sizeof ( target ),
			"R_pdf_%s_%d",
			filename_key,
			process_id );
	}
	else
	{
		snprintf(
			target,
			sizeof ( target ), 
			"R_pdf_%d",
			process_id );
	}

	return target;
}

char *rchart_prompt_R_target(
		char *filename_key,
		pid_t process_id )
{
	static char target[ 128 ];

	if ( filename_key )
	{
		snprintf(
			target,
			sizeof ( target ),
			"R_%s_%d",
			filename_key,
			process_id );
	}
	else
	{
		snprintf(
			target,
			sizeof ( target ),
			"R_%d",
			process_id );
	}

	return target;
}

RCHART *rchart_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *query_table_edit_where_string,
		char *chart_title,
		char *chart_filename_key )
{
	RCHART *rchart;

	if ( !application_name
	||   !session_key
	||   !data_directory
	||   !date_attribute_name
	||   !number_attribute_name
	||   !query_fetch_row_list
	||   !query_table_edit_where_string
	||   !chart_title
	||   !chart_filename_key )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rchart = rchart_calloc();

	rchart->rchart_filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		rchart_filename_new(
			application_name,
			session_key,
			data_directory,
			chart_filename_key );

	/* Returns program memory */
	/* ---------------------- */
	rchart->file_header_string = rchart_file_header_string();

	rchart->rchart_point_list =
		rchart_point_list(
			date_attribute_name,
			time_attribute_name,
			number_attribute_name,
			query_fetch_row_list );

	if ( !list_length( rchart->rchart_point_list ) ) return rchart;

	rchart->rchart_point_date_combine_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		rchart_point_combine_string(
			date_attribute_name,
			rchart->rchart_point_list,
			1 /* date_time_boolean */ );

	rchart->rchart_point_value_combine_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		rchart_point_combine_string(
			number_attribute_name,
			rchart->rchart_point_list,
			0 /* not date_time_boolean */ );

	rchart->file_trailer_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_file_trailer_string(
			date_attribute_name,
			number_attribute_name,
			query_table_edit_where_string,
			chart_title,
			rchart->rchart_filename->pdf_output_filename );
	
	rchart_write(
		rchart->rchart_filename->R_output_filename,
		rchart->file_header_string,
		rchart->rchart_point_date_combine_string,
		rchart->rchart_point_value_combine_string,
		rchart->file_trailer_string );

	rchart->script_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rchart_script_system_string(
			rchart->rchart_filename->R_output_filename );

	return rchart;
}

RCHART *rchart_calloc( void )
{
	RCHART *rchart;

	if ( ! ( rchart = calloc( 1, sizeof ( RCHART ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rchart;
}

char *rchart_file_trailer_string(
		char *date_attribute_name,
		char *number_attribute_name,
		char *query_table_edit_where_string,
		char *chart_title,
		char *pdf_output_filename )
{
	char trailer_string[ 1024 ];

	if ( !date_attribute_name
	||   !number_attribute_name
	||   !query_table_edit_where_string
	||   !chart_title
	||   !pdf_output_filename )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		trailer_string,
		sizeof ( trailer_string ),
		"dataFrame = data.frame( %s, %s )\n"
		"gg <- ggplot( dataFrame, aes( %s, %s ) ) +\n"
		"geom_point() +\n"
		"geom_line() +\n"
		"labs( title = \"%s\" ) +\n"
		"labs( subtitle = \"%s\", size = 10 )\n"
		"ggsave( \"%s\", plot = gg, width = 10, height = 7.5 )",
		date_attribute_name,
		number_attribute_name,
		date_attribute_name,
		number_attribute_name,
		chart_title,
		query_table_edit_where_string,
		pdf_output_filename );

	return strdup( trailer_string );
}

void rchart_write(
		char *R_output_filename,
		char *rchart_file_header_string,
		char *rchart_point_date_combine_string,
		char *rchart_point_value_combine_string,
		char *rchart_file_trailer_string )
{
	FILE *write_file;

	if ( !R_output_filename
	||   !rchart_file_header_string
	||   !rchart_point_date_combine_string
	||   !rchart_point_value_combine_string
	||   !rchart_file_trailer_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	write_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_write_file(
			R_output_filename );

	fprintf(
		write_file,
		"%s\n%s\n%s\n%s\n",
		rchart_file_header_string,
		rchart_point_date_combine_string,
		rchart_point_value_combine_string,
		rchart_file_trailer_string );

	fclose( write_file );
}

char *rchart_script_system_string( char *R_output_filename )
{
	char system_string[ 1024 ];

	if ( !R_output_filename )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"R_output_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"Rscript.sh %s /dev/null",
		R_output_filename );

	return strdup( system_string );
}

void rchart_free( RCHART *rchart )
{
	if ( !rchart )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"rchart is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( rchart->rchart_point_date_combine_string );
	free( rchart->rchart_point_value_combine_string );
	free( rchart->file_trailer_string );
	free( rchart->script_system_string );
	rchart_point_list_free( rchart->rchart_point_list );
	free( rchart );
}

void rchart_point_list_free( LIST *rchart_point_list )
{
	RCHART_POINT *rchart_point;

	if ( list_rewind( rchart_point_list ) )
	do {
		rchart_point = list_get( rchart_point_list );

		free( rchart_point->date_time_string );
		free( rchart_point );

	} while ( list_next( rchart_point_list ) );

	list_free_container( rchart_point_list );
}

char *rchart_file_header_string( void )
{
	return
	"library( ggplot2 )\n"
	"library( lubridate )\n"
	"library( scales )\n"
	"library( dplyr )";
}
