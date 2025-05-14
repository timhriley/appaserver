/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "String.h"
#include "application.h"
#include "appaserver.h"
#include "date.h"
#include "chart.h"
#include "document.h"
#include "appaserver_link.h"
#include "appaserver_error.h"
#include "google.h"

GOOGLE_FILENAME *google_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *number_attribute_name,
		char *chart_filename_key )
{
	GOOGLE_FILENAME *google_filename;

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

	google_filename = google_filename_calloc();

	google_filename->process_id =
		google_filename_process_id(
			session_key );

	google_filename->appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			data_directory,
			GOOGLE_FILENAME_STEM,
			application_name,
			google_filename->process_id,
			session_key,
			chart_filename_key /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" /* extension */ );

	google_filename->http_output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_filename_http_output_filename(
			google_filename->appaserver_link );

	google_filename->http_prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_filename_http_prompt_filename(
			google_filename->appaserver_link );

	/* If operation */
	/* ------------ */
	google_filename->appaserver_link_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			google_filename->http_prompt_filename,
			number_attribute_name /* target_frame */,
			"Link to google file" /* prompt_message */ );

	return google_filename;
}

pid_t google_filename_process_id( char *session_key )
{
	if ( session_key )
		return (pid_t)0;
	else
		return getpid();
}

GOOGLE_FILENAME *google_filename_calloc( void )
{
	GOOGLE_FILENAME *google_filename;

	if ( ! ( google_filename = calloc( 1, sizeof ( GOOGLE_FILENAME ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_filename;
}

char *google_filename_http_output_filename( APPASERVER_LINK *appaserver_link )
{
	if ( !appaserver_link
	||   !appaserver_link->appaserver_link_output )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	appaserver_link->
		appaserver_link_output->
		filename;
}

char *google_filename_http_prompt_filename( APPASERVER_LINK *appaserver_link )
{
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

	return
	appaserver_link->
		appaserver_link_prompt->
		filename;
}

char *google_filename_key( char *number_attribute_name )
{

	if ( number_attribute_name )
	{
		char filename_key[ 128 ];

		sprintf(filename_key,
			"googlefile_%s",
			number_attribute_name );

		return strdup( filename_key );
	}
	else
	{
		return "googlefile";
	}
}

GOOGLE_PROMPT *google_prompt_new(
		GOOGLE_FILENAME *google_filename,
		char *chart_filename_key,
		pid_t process_id )
{
	GOOGLE_PROMPT *google_prompt;

	if ( !google_filename
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

	google_prompt = google_prompt_calloc();

	google_prompt->target =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_prompt_target(
			chart_filename_key,
			process_id );

	google_prompt->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_prompt_html(
			google_filename->http_prompt_filename,
			chart_filename_key,
			google_prompt->target );

	return google_prompt;
}

GOOGLE_PROMPT *google_prompt_calloc( void )
{
	GOOGLE_PROMPT *google_prompt;

	if ( ! ( google_prompt = calloc( 1, sizeof ( GOOGLE_PROMPT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_prompt;
}

char *google_prompt_target(
		char *chart_filename_key,
		pid_t process_id )
{
	static char target[ 64 ];

	if ( chart_filename_key )
	{
		sprintf(target,
			"google_%s_%d",
			chart_filename_key,
			process_id );
	}
	else
	{
		sprintf(target,
			"google_%d",
			process_id );
	}

	return target;
}

char *google_prompt_html(
		char *http_prompt_filename,
		char *chart_filename_key,
		char *google_prompt_target )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !http_prompt_filename
	||   !google_prompt_target )
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
		"<tr><td><a href=\"%s\" target=%s>View ",
		http_prompt_filename,
		google_prompt_target );

	if ( chart_filename_key )
	{
		char buffer[ 64 ];

		ptr += sprintf( ptr,
			"%s ",
			string_initial_capital(
				buffer,
				chart_filename_key ) );
	}

	sprintf( ptr, "chart.</a>" );

	return strdup( html );
}

GOOGLE_WINDOW *google_window_new(
		GOOGLE_FILENAME *google_filename,
		char *sub_title,
		pid_t process_id )
{
	GOOGLE_WINDOW *google_window;
	char *sub_title_display;
	char *onload_open_tag;

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

	google_window = google_window_calloc();

	if ( google_filename )
	{
		google_window->google_prompt_target =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			google_prompt_target(
				(char *)0 /* chart_filename_key */,
				process_id );

		google_window->onload_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			chart_window_onload_string(
				google_filename->http_prompt_filename,
				google_window->google_prompt_target );
	}

	onload_open_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_onload_open_tag(
			JAVASCRIPT_WAIT_OVER,
			google_window->onload_string /* javascript_replace */,
			(char *)0 /* document_body_menu_onload_string */ );

	google_window->screen_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_window_screen_title(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_time_now(
				date_utc_offset() ) );

	sub_title_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		chart_window_sub_title_display(
			sub_title );

	google_window->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		chart_window_html(
			onload_open_tag,
			google_window->screen_title,
			sub_title_display );

	free( onload_open_tag );
	free( sub_title_display );

	return google_window;
}

GOOGLE_WINDOW *google_window_calloc( void )
{
	GOOGLE_WINDOW *google_window;

	if ( ! ( google_window = calloc( 1, sizeof ( GOOGLE_WINDOW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_window;
}

char *google_window_screen_title( char *date_time_now )
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
		"<h1>Google Chart Viewer<br>%s</h1>",
		date_time_now );

	return screen_title;
}

char *google_window_where_display(
		char *where_string )
{
	char where_display[ STRING_WHERE_BUFFER ];

	if ( !where_string
	||   strcmp( where_string, "1 = 1" ) == 0 )
	{
		where_string = "Entire Table";
	}

	if ( strlen( where_string ) + 17 >= STRING_WHERE_BUFFER )
	{
		*where_display = '\0';
	}
	else
	{
		sprintf(where_display,
			"<h2>Where: %s</h2>",
			where_string );
	}

	return strdup( where_display );
}

