/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_map_operation.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "application.h"
#include "appaserver_error.h"
#include "frameset.h"
#include "google_map_operation.h"

GOOGLE_MAP_OPERATION *google_map_operation_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *operation_name,
		char *latitude_string,
		char *longitude_string,
		char *utm_easting_string,
		char *utm_northing_string,
		char *primary_data_list_string,
		pid_t parent_process_id,
		int operation_row_checked_count,
		char *data_directory )
{
	GOOGLE_MAP_OPERATION *google_map_operation;

	if ( !parent_process_id )
	{
		char message[ 128 ];

		sprintf(message, "parent_process_id is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !operation_name
	||   !primary_data_list_string
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

	if ( !operation_row_checked_count ) return NULL;

	google_map_operation = google_map_operation_calloc();

	google_map_operation->operation_semaphore =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		operation_semaphore_new(
			application_name,
			operation_name,
			data_directory,
			parent_process_id,
			operation_row_checked_count );

	google_map_operation->google_filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_filename_new(
			application_name,
			session_key,
			data_directory,
		       	(char *)0 /* number_attribute_name */,
			(char *)0 /* chart_filename_key */ );

	if ( !google_map_operation->
			google_filename->
			appaserver_link_anchor_html )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"appaserver_link_anchor_html is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	google_map_operation->output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_map_operation_output_file(
			google_map_operation->
				operation_semaphore->
				group_first_time,
			google_map_operation->
				google_filename->
				http_output_filename );

	if ( google_map_operation->operation_semaphore->group_first_time )
	{
		google_map_operation->google_map_key =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			google_map_key(
				GOOGLE_MAP_KEY_LABEL );

		google_map_operation->title_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			google_map_operation_title_string(
				operation_name );

		google_map_operation->google_map_canvas_style_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			google_map_canvas_style_html(
				GOOGLE_MAP_CANVAS_LABEL,
				0 /* absolute_position_top */,
				0 /* absolute_position_left */,
				GOOGLE_MAP_WIDTH,
				GOOGLE_MAP_HEIGHT );

		google_map_operation->google_map_loader_script_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			google_map_loader_script_html(
				google_map_operation->google_map_key );

		google_map_operation->google_map_script_open_tag =
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			google_map_script_open_tag();

		google_map_operation->google_map_create_marker_javascript =
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			google_map_create_marker_javascript();

		google_map_operation->google_map_initialize_javascript =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			google_map_initialize_javascript(
				GOOGLE_MAP_CANVAS_LABEL,
				GOOGLE_MAP_CENTER_LATITUDE,
				GOOGLE_MAP_CENTER_LONGITUDE,
				GOOGLE_MAP_STARTING_ZOOM );

		google_map_operation->document_open_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			google_map_operation_document_open_html(
				application_name,
				google_map_operation->title_string,
				google_map_operation->
					google_map_canvas_style_html,
				google_map_operation->
					google_map_loader_script_html,
				google_map_operation->
					google_map_script_open_tag,
				google_map_operation->
					google_map_create_marker_javascript,
				google_map_operation->
					google_map_initialize_javascript );

		fprintf(
			google_map_operation->output_file,
			"%s\n",
			google_map_operation->document_open_html );
	}

	google_map_operation->google_map_click_message =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_map_click_message(
			primary_data_list_string /* message */ );

	if ( string_atoi( latitude_string ) )
	{
		google_map_operation->google_map_point =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			google_map_point_latitude_longitude_new(
				latitude_string,
				longitude_string,
				google_map_operation->
					google_map_click_message );
	}
	else
	{
		google_map_operation->google_map_point =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			google_map_point_utm_new(
				utm_easting_string,
				utm_northing_string,
				google_map_operation->
					google_map_click_message );
	}

	fprintf(
		google_map_operation->output_file,
		"\n%s\n",
		google_map_operation->google_map_point->javascript );

	if ( google_map_operation->operation_semaphore->group_last_time )
	{
		fprintf(google_map_operation->output_file,
			"%s\n",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			google_map_script_close_tag() );

		fprintf(google_map_operation->output_file,
			"%s\n",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag() );

		google_map_operation->google_map_body_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			google_map_body_html(
				GOOGLE_MAP_CANVAS_LABEL );

		fprintf(google_map_operation->output_file,
			"%s\n",
			google_map_operation->google_map_body_html );

		fprintf(google_map_operation->output_file,
			"%s\n",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );
	}

	fclose( google_map_operation->output_file );

	if ( google_map_operation->operation_semaphore->group_last_time )
	{
		google_map_operation->application_menu_horizontal_boolean =
			application_menu_horizontal_boolean(
				application_name );

		google_map_operation->menu_horizontal_boolean =
			menu_horizontal_boolean(
				target_frame /* current_frame */,
				FRAMESET_PROMPT_FRAME,
				google_map_operation->
					application_menu_horizontal_boolean );

		if ( google_map_operation->menu_horizontal_boolean )
		{
			google_map_operation->folder_menu =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				folder_menu_fetch(
					application_name,
					session_key,
					role_name,
					data_directory,
					0 /* not folder_menu_remove */ );

			google_map_operation->menu =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				menu_new(
				    application_name,
				    session_key,
				    login_name,
				    role_name,
				    1 /* application_menu_horizontal_boolean */,
				    google_map_operation->
					folder_menu->
					count_list );
		}

		google_map_operation->document_head_menu_setup_string =
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(google_map_operation->menu)
					? 1
					: 0 /* menu_boolean */ );

		google_map_operation->document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			document_new(
				application_name,
				application_title_string( application_name ),
				google_map_operation->title_string,
				(char *)0 /* sub_title_html */,
				(char *)0 /* sub_sub_title_html */,
				(char *)0 /* notepad */,
				(char *)0 /* onload_javascript_string */,
				(google_map_operation->menu)
					? google_map_operation->
						menu->
						html
					: (char *)0,
				google_map_operation->
					document_head_menu_setup_string,
				(char *)0 /* calendar_setup_string */,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				javascript_include_string(
					application_name,
					(LIST *)0
					/* javascript_filename_list */ ) );

		google_map_operation->document_form_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			document_form_html(
				google_map_operation->document->html,
				google_map_operation->
					document->
					document_head->
					html,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				document_head_close_tag(),
				google_map_operation->
					document->
					document_body->
					html,
				(char *)0 /* form_html */,
				(char *)0 /* document_body_close_tag */,
				(char *)0 /* document_close_tag */ );
	}

	return google_map_operation;
}

GOOGLE_MAP_OPERATION *google_map_operation_calloc( void )
{
	GOOGLE_MAP_OPERATION *google_map_operation;

	if ( ! ( google_map_operation =
			calloc( 1,
				sizeof ( GOOGLE_MAP_OPERATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_map_operation;
}

FILE *google_map_operation_output_file(
			boolean group_first_time,
			char *http_output_filename )
{
	FILE *output_file;
	char *mode;

	if ( !http_output_filename )
	{
		char message[ 128 ];

		sprintf(message, "http_output_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( group_first_time )
		mode = "w";
	else
		mode = "a";

	if ( ! ( output_file = fopen( http_output_filename, mode ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"fopen(%s) returned empty.",
			http_output_filename );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return output_file;
}

char *google_map_operation_title_string( char *operation_name )
{
	static char title_string[ 64 ];

	if ( !operation_name )
	{
		char message[ 128 ];

		sprintf(message, "operation_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	string_initial_capital(
		title_string /* destination */,
		operation_name );
}

char *google_map_operation_document_open_html(
		char *application_name,
		char *google_map_operation_title_string,
		char *google_map_canvas_style_html,
		char *google_map_loader_script_html,
		char *google_map_script_open_tag,
		char *google_map_create_marker_javascript,
		char *google_map_initialize_javascript )
{
	char html[ STRING_64K ];
	char *ptr = html;
	DOCUMENT_HEAD *document_head;

	if ( !application_name
	||   !google_map_operation_title_string
	||   !google_map_canvas_style_html
	||   !google_map_loader_script_html
	||   !google_map_script_open_tag
	||   !google_map_create_marker_javascript
	||   !google_map_initialize_javascript )
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
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		document_html(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_type_html(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_standard_string() ) );

	document_head =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_head_new(
			application_name,
			google_map_operation_title_string,
			(char *)0 /* menu_setup_string */,
			(char *)0 /* calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	ptr += sprintf(
		ptr,
		"%s\n",
		document_head->html );

	ptr += sprintf(
		ptr,
		"%s\n",
		google_map_canvas_style_html );

	ptr += sprintf(
		ptr,
		"%s\n",
		google_map_loader_script_html );

	ptr += sprintf(
		ptr,
		"%s\n",
		google_map_script_open_tag );

	ptr += sprintf(
		ptr,
		"%s\n",
		google_map_create_marker_javascript );

	ptr += sprintf(
		ptr,
		"%s",
		google_map_initialize_javascript );

	return strdup( html );
}
