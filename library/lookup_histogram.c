/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_histogram.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "String.h"
#include "date.h"
#include "appaserver.h"
#include "application.h"
#include "document.h"
#include "application.h"
#include "appaserver_error.h"
#include "lookup_histogram.h"

LOOKUP_HISTOGRAM *lookup_histogram_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory )
{
	LOOKUP_HISTOGRAM *lookup_histogram;
	LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph;
	char *number_attribute_name;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
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

	lookup_histogram = lookup_histogram_calloc();

	lookup_histogram->chart_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		chart_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string );

	if ( !lookup_histogram->chart_input->row_list_length )
		return lookup_histogram;

	if ( !list_rewind(
		lookup_histogram->
			chart_input->
			query_chart->
			number_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"query_chart->number_attribute_name_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_histogram->lookup_histogram_graph_list = list_new();

	do {
		number_attribute_name =
			list_get(
				lookup_histogram->
					chart_input->
			       		query_chart->
					number_attribute_name_list );

		lookup_histogram_graph =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			lookup_histogram_graph_new(
				application_name,
				session_key,
				folder_name,
				data_directory,
				lookup_histogram->
					chart_input->
					query_chart->
					query_table_edit_where->
					string
					/* where_string */,
				lookup_histogram->
					chart_input->
					query_chart->
					query_fetch->
					row_list,
				number_attribute_name );

		list_set(
			lookup_histogram->lookup_histogram_graph_list,
			lookup_histogram_graph );

	} while ( list_next(
			lookup_histogram->
				chart_input->
			       	query_chart->
				number_attribute_name_list ) );

	lookup_histogram->lookup_histogram_filename =
		lookup_histogram_filename(
			lookup_histogram->
				lookup_histogram_graph_list );

	lookup_histogram->lookup_histogram_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_histogram_window_new(
			folder_name,
			lookup_histogram->lookup_histogram_filename,
			lookup_histogram->
				chart_input->
				query_chart->
				query_table_edit_where->
				string /* where_string */,
			getpid() /* process_id */ );

	return lookup_histogram;
}

LOOKUP_HISTOGRAM *lookup_histogram_calloc( void )
{
	LOOKUP_HISTOGRAM *lookup_histogram;

	if ( ! ( lookup_histogram =
			calloc( 1,
				sizeof ( LOOKUP_HISTOGRAM ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_histogram;
}

char *lookup_histogram_graph_title(
		char *folder_name,
		char *number_attribute_name )
{
	char title[ 256 ];
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !folder_name
	||   !number_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title,
		"Histogram for %s -- %s",
		string_initial_capital(
			buffer1,
			folder_name ),
		string_initial_capital(
			buffer2,
			number_attribute_name ) );

	return strdup( title );
}

LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		char *where_string,
		LIST *query_fetch_row_list,
		char *number_attribute_name )
{
	LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph;

	if ( !application_name
	||   !session_key
	||   !folder_name
	||   !data_directory
	||   !list_length( query_fetch_row_list )
        ||   !number_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_histogram_graph = lookup_histogram_graph_calloc();

	lookup_histogram_graph->title =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		lookup_histogram_graph_title(
			folder_name,
			number_attribute_name );

	lookup_histogram_graph->lookup_histogram_filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_histogram_filename_new(
			application_name,
			session_key,
			data_directory,
			number_attribute_name );

	lookup_histogram_graph->histogram =
		histogram_new(
			query_fetch_row_list,
			number_attribute_name,
			lookup_histogram_graph->title,
			where_string,
			lookup_histogram_graph->
				lookup_histogram_filename->
				appaserver_link->
				appaserver_link_output->
				filename
					/* histogram_output_pdf */ );

	if ( !lookup_histogram_graph->histogram )
	{
		char message[ 128 ];

		sprintf(message, "histogram_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_histogram_graph->lookup_histogram_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_histogram_prompt_new(
			lookup_histogram_graph->
				lookup_histogram_filename->
				appaserver_link->
				appaserver_link_prompt->
				filename
					/* http_prompt_filename */,
			number_attribute_name,
			getpid() /* process_id */ );

	return lookup_histogram_graph;
}

LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph_calloc( void )
{
	LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph;

	if ( ! ( lookup_histogram_graph =
			calloc( 1,
				sizeof ( LOOKUP_HISTOGRAM_GRAPH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_histogram_graph;
}

LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename(
		LIST *lookup_histogram_graph_list )
{
	if ( list_length( lookup_histogram_graph_list ) == 1 )
	{
		LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph =
			list_first(
				lookup_histogram_graph_list );

		return
		lookup_histogram_graph->lookup_histogram_filename;
	}
	else
	{
		return NULL;
	}
}

LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *number_attribute_name )
{
	LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename;

	if ( !application_name
	||   !session_key
	||   !data_directory
	||   !number_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_histogram_filename = lookup_histogram_filename_calloc();

	lookup_histogram_filename->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			data_directory,
			LOOKUP_HISTOGRAM_FILENAME_STEM,
			application_name,
			0 /* process_id */,
			session_key,
			number_attribute_name
				/* begin_date_string */,
			(char *)0 /* end_date_string */,
			"pdf" /* extension */ );

	return lookup_histogram_filename;
}

LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename_calloc( void )
{
	LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename;

	if ( ! ( lookup_histogram_filename =
			calloc( 1,
				sizeof ( LOOKUP_HISTOGRAM_FILENAME ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_histogram_filename;
}

LOOKUP_HISTOGRAM_PROMPT *lookup_histogram_prompt_new(
		char *http_prompt_filename,
		char *number_attribute_name,
		pid_t process_id )
{
	LOOKUP_HISTOGRAM_PROMPT *lookup_histogram_prompt;
	char *target_frame;

	if ( !http_prompt_filename
	||   !number_attribute_name
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_histogram_prompt = lookup_histogram_prompt_calloc();

	target_frame =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_prompt_target_frame(
			number_attribute_name
				/* datatype_name */,
			process_id );

	lookup_histogram_prompt->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		lookup_histogram_prompt_html(
			http_prompt_filename,
			number_attribute_name,
			target_frame );

	return lookup_histogram_prompt;
}

LOOKUP_HISTOGRAM_PROMPT *lookup_histogram_prompt_calloc( void )
{
	LOOKUP_HISTOGRAM_PROMPT *lookup_histogram_prompt;

	if ( ! ( lookup_histogram_prompt =
			calloc( 1,
				sizeof ( LOOKUP_HISTOGRAM_PROMPT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_histogram_prompt;
}

char *lookup_histogram_prompt_html(
		char *http_prompt_filename,
		char *number_attribute_name,
		char *query_prompt_target_frame )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !http_prompt_filename
	||   !number_attribute_name
	||   !query_prompt_target_frame )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<br><br>" );

	ptr += sprintf(
		ptr,
		"<a href=\"%s\" target=%s>Press to view ",
		http_prompt_filename,
		query_prompt_target_frame );

	if ( number_attribute_name )
	{
		char buffer[ 128 ];

		ptr += sprintf(
			ptr,
			"%s ",
			string_initial_capital(
				buffer,
				number_attribute_name ) );
	}

	sprintf( ptr, "histogram.</a>" );

	return strdup( html );
}

LOOKUP_HISTOGRAM_WINDOW *lookup_histogram_window_new(
		char *folder_name,
		LOOKUP_HISTOGRAM_FILENAME *
			lookup_histogram_filename,
		char *where_string,
		pid_t process_id )
{
	LOOKUP_HISTOGRAM_WINDOW *lookup_histogram_window;
	char *onload_string = {0};
	char *onload_open_tag;
	char *now;
	char *title_html;
	char *where_display;

	if ( !folder_name
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_histogram_window = lookup_histogram_window_calloc();

	if ( lookup_histogram_filename )
	{
		onload_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			lookup_histogram_window_onload_string(
				lookup_histogram_filename->
					appaserver_link->
					appaserver_link_prompt->
					filename
					/* http_prompt_filename */,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_prompt_target_frame(
					(char *)0 /* datatype_name */,
					process_id ) );
	}

	onload_open_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_onload_open_tag(
			JAVASCRIPT_WAIT_OVER,
			onload_string /* javascript_replace */,
			(char *)0 /* document_body_menu_onload_string */ );

	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	now = date_time_now( date_utc_offset() );

	title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		lookup_histogram_window_title_html(
			folder_name,
			now );

	where_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_where_display(
			where_string,
			0 /* max_length */ );

	lookup_histogram_window->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		lookup_histogram_window_html(
			onload_open_tag,
			title_html,
			where_display );

	free( onload_open_tag );
	free( now );
	free( where_display );

	return lookup_histogram_window;
}

char *lookup_histogram_window_title_html(
		char *folder_name,
		char *date_time_now )
{
	static char title_html[ 128 ];
	char buffer[ 64 ];

	if ( !folder_name
	||   !date_time_now )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title_html,
		"<h1>Histogram %s</h1>\n<h2>%s</h2>",
		string_initial_capital(
			buffer,
			folder_name ),
		date_time_now );

	return title_html;
}

LOOKUP_HISTOGRAM_WINDOW *lookup_histogram_window_calloc( void )
{
	LOOKUP_HISTOGRAM_WINDOW *lookup_histogram_window;

	if ( ! ( lookup_histogram_window =
			calloc( 1,
				sizeof ( LOOKUP_HISTOGRAM_WINDOW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_histogram_window;
}

char *lookup_histogram_window_onload_string(
		char *http_prompt_filename,
		char *query_prompt_target_frame )
{
	static char onload_string[ 256 ];

	if ( !http_prompt_filename
	||   !query_prompt_target_frame )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(onload_string,
		APPASERVER_WINDOW_OPEN_TEMPLATE,
		(http_prompt_filename) ? http_prompt_filename : "",
		(query_prompt_target_frame) ? query_prompt_target_frame : "" );

	return onload_string;
}

char *lookup_histogram_window_html(
		char *onload_open_tag,
		char *title_html,
		char *where_display )
{
	char html[ STRING_WHERE_BUFFER ];

	if ( !onload_open_tag
	||   !title_html
	||   !where_display )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( onload_open_tag ) +
		strlen( title_html ) +
		strlen( where_display ) +
		10 >= STRING_WHERE_BUFFER )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_WHERE_BUFFER );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"%s\n%s\n<h2>%s</h2>",
		onload_open_tag,
		title_html,
		where_display );

	return strdup( html );
}

