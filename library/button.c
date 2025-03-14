/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/button.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "frameset.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "widget.h"
#include "application.h"
#include "javascript.h"
#include "post_choose_folder.h"
#include "button.h"

BUTTON *button_calloc( void )
{
	BUTTON *button;

	if ( ! ( button = calloc( 1, sizeof ( BUTTON ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return button;
}

BUTTON *button_new( char *label )
{
	BUTTON *button = button_calloc();

	button->label = label;

	return button;
}

char *button_html(
		char *label,
		char *action_string,
		char *hover_message,
		int row_number )
{
	char html[ STRING_4K ];
	char *ptr = html;
	char *display;

	if ( !label
	||   !action_string )
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
		"<button type=button onClick=\"%s\"",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		javascript_replace(
		       action_string /* post_change_javascript */,
		       (char *)0 /* state */,
		       row_number ) );

	if ( hover_message )
	{
		ptr += sprintf( ptr,
			" title=\"%s\"",
			hover_message );
	}

	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	display = button_display( label );

	ptr += sprintf( ptr,
		">%s</button>",
		display );

	free( display );

	return strdup( html );
}

char *button_reset_html(
		char *form_name,
		char widget_multi_left_right_delimiter,
		char *javascript_replace,
		char *button_reset_message )
{
	char html[ STRING_4K ];
	char *ptr = html;

	if ( !form_name ) form_name = "document.forms[0]";

	ptr += sprintf(
		ptr,
"<input type=button value=\"Reset\" title=\"%s\" onClick=\"form_reset(%s, '%c')",
		(button_reset_message) ? button_reset_message : "",
		form_name,
		widget_multi_left_right_delimiter );

	if ( javascript_replace && *javascript_replace )
	{
		ptr += sprintf(
			ptr,
			" && %s",
			javascript_replace );
	}

	ptr += sprintf( ptr, "\">" );

	return strdup( html );
}

BUTTON *button_submit(
		char *form_name,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *form_verify_notepad_widths_javascript )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_submit_html(
			form_name,
			BUTTON_SUBMIT_DISPLAY,
			form_multi_select_all_javascript,
			recall_save_javascript,
			form_verify_notepad_widths_javascript,
			BUTTON_SUBMIT_MESSAGE );

	return button;
}

char *button_submit_html(
		char *form_name,
		char *button_submit_display,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *form_verify_notepad_widths_javascript,
		char *button_submit_message )
{
	char html[ STRING_4K ];
	char *ptr = html;
	boolean got_one = 0;

	if ( !button_submit_display
	||   !button_submit_message )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !form_name ) form_name = "document.forms[0]";

	ptr += sprintf(
		ptr,
"<input type=button value=\"%s\" title=\"%s\" onClick=\"",
		button_submit_display,
		button_submit_message );

	if ( form_multi_select_all_javascript )
	{
		if (	strlen( html ) +
			strlen( form_multi_select_all_javascript )
			>= STRING_4K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_4K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s",
			form_multi_select_all_javascript );

		got_one = 1;
	}

	if ( recall_save_javascript )
	{
		if (	strlen( html ) +
			strlen( recall_save_javascript )
			+ 5 >= STRING_4K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_4K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( got_one )
		{
			ptr += sprintf( ptr, " && " );
		}

		ptr += sprintf(
			ptr,
			"%s",
			recall_save_javascript );

		got_one = 1;
	}

	if ( form_verify_notepad_widths_javascript )
	{
		if (	strlen( html ) +
			strlen( form_verify_notepad_widths_javascript )
			+ 5 >= STRING_4K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_4K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( got_one )
		{
			ptr += sprintf( ptr, " && " );
		}

		ptr += sprintf(
			ptr,
			"%s",
			form_verify_notepad_widths_javascript );

		got_one = 1;
	}

	if ( got_one )
	{
		ptr += sprintf( ptr, " && " );
	}

	ptr += sprintf(
		ptr,
		"%s && ",
		JAVASCRIPT_WAIT_START );

	ptr += sprintf(
		ptr,
		"%s.submit();\">",
		form_name );

	return strdup( html );
}

BUTTON *button_restart_drillthru( char *post_choose_folder_action_string )
{
	BUTTON *button;

	if ( !post_choose_folder_action_string )
	{
		char message[ 128 ];

		sprintf(message, "post_choose_folder_action_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_anchor_html(
			"Restart Drillthru" /* display */,
			post_choose_folder_action_string
				/* href_action_string */,
			(char *)0 /* onclick_action_string */,
			BUTTON_DRILLTHRU_MESSAGE
				/* hover_message */ );

	return button;
}

BUTTON *button_to_top( void )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_to_top_html(
			BUTTON_IMAGE_RELATIVE_DIRECTORY,
			BUTTON_TO_TOP_MESSAGE );

	return button;
}

char *button_to_top_html(
		char *button_image_relative_directory,
		char *button_to_top_message )
{
	char html[ 1024 ];

	sprintf(html,
"<a onClick=\"%s\" title=\"%s\"><img src=\"/appaserver_home/%s/top.png\"></a>",
		"window.scrollTo(0,0)",
		button_to_top_message,
		button_image_relative_directory );

	return strdup( html );
}

BUTTON *button_recall( char *recall_load_javascript )
{
	BUTTON *button;

	if ( !recall_load_javascript ) return NULL;

	button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_input_html(
			"Recall" /* display */,
			recall_load_javascript /* action_string */,
			BUTTON_RECALL_MESSAGE );

	return button;
}

BUTTON *button_back( void )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_input_html(
			"Back" /* display */,
			"history.back()" /* action_string */,
			BUTTON_BACK_MESSAGE );

	return button;
}

BUTTON *button_forward( void )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_input_html(
			"Forward" /* display */,
			"timlib_history_forward()" /* action_string */,
			BUTTON_FORWARD_MESSAGE );

	return button;
}

BUTTON *button_reset(
		char *form_name,
		char *javascript_replace )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_reset_html(
			form_name,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER,
			javascript_replace,
			BUTTON_RESET_MESSAGE );

	return button;
}

BUTTON *button_drillthru_skip( void )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_drillthru_skip_html(
			BUTTON_SKIP_MESSAGE,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER );

	return button;
}

char *button_drillthru_skip_html(
		char *button_skip_message,
		char widget_multi_left_right_delimiter )
{
	char html[ 1024 ];

	sprintf(html,
"<input type=button value=\"Skip Drillthru\" title=\"%s\" onClick=\"form_reset(document.forms[0], '%c'); document.forms[0].submit()\">",
		button_skip_message,
		widget_multi_left_right_delimiter );

	return strdup( html );
}

BUTTON *button_help(
		char *application_name,
		char *html_help_file_anchor )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_help_html(
			application_name,
			html_help_file_anchor );

	return button;
}

char *button_help_html(
		char *application_name,
		char *html_help_file_anchor )
{
	char full_pathname[ 512 ];
	char *appaserver_mount_point;
	char *relative_source_directory;
	char source_directory[ 128 ];
	char source_directory_filename[ 512 ];
	int index;
	char html[ 1024 ];

	if ( !html_help_file_anchor || !*html_help_file_anchor )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: html_help_file_anchor is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	appaserver_mount_point = appaserver_parameter_mount_point();

	relative_source_directory =
		application_relative_source_directory(
			application_name );

	for(	index = 0;
		piece(	source_directory,
			':',
			relative_source_directory,
			index );
		index++ )
	{
		sprintf(source_directory_filename, 
		 	"%s/%s/%s",
		 	appaserver_mount_point,
		 	source_directory,
		 	html_help_file_anchor );

		if ( timlib_file_exists( source_directory_filename ) ) break;
	}

	if ( !*source_directory )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: cannot html_help_file_anchor = [%s] in any of (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 html_help_file_anchor,
			 relative_source_directory );
		exit( 1 );
	}

	if ( *html_help_file_anchor == '/' )
	{
		strcpy( full_pathname, html_help_file_anchor );
	}
	else
	{
		sprintf(	full_pathname,
				"/appaserver/%s/%s",
				source_directory,
				html_help_file_anchor );
	}

	sprintf(html,
"<input type=button value=\"Help\" onclick='window.open(\"%s\",\"help_window\",\"width=600,height=350,resizable=yes,scrollbars=yes\")'>",
		full_pathname );

	return strdup( html );
}

char *button_list_html( LIST *button_list )
{
	char html[ STRING_64K ];
	char *ptr = html;
	BUTTON *button;

	if ( !list_rewind( button_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: button_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n%s\n",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_table_open_tag(
			0 /* not border_boolean */,
			0 /* cell_spacing */,
			0 /* cell_padding */ ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_table_row_tag( (char *)0 /* background_color */ ) );

	do {
		button = list_get( button_list );

		if ( !button->html )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: button->html is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		ptr += sprintf(
			ptr,
			"%s%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_table_data_tag(
				0 /* not align_right */,
				0 /* not column_span */,
				0 /* width */,
				(char *)0 /* style */ ),
			button->html );

	} while ( list_next( button_list ) );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		widget_table_close_tag() );

	return strdup( html );
}

void button_list_free( LIST *button_list )
{
	BUTTON *button;

	if ( list_rewind( button_list ) )
	do {
		button = list_get( button_list );

		if ( button->html ) free( button->html );

		free( button );

	} while ( list_next( button_list ) );

	list_free_container( button_list );
}

char *button_display( char *label )
{
	char display[ 128 ];
	char *ptr = display;
	boolean beginning = 1;

	if ( !label )
	{
		char message[ 128 ];

		sprintf(message, "label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	while ( *label )
	{
		if ( beginning )
		{
			if ( !isspace( *label )
			&& ( !ispunct( *label )
			&&   *label != '\'' ) )
			{
				*ptr++ = toupper( *label++ );
				beginning = 0;
			}
			else
			{
				*ptr++ = *label++;
			}
		}
		else
		if ( *label == '_' || *label == '|' || *label == '^' )
		{
			beginning = 1;
			*ptr++ = '<';
			*ptr++ = 'b';
			*ptr++ = 'r';
			*ptr++ = '>';
			label++;
		}
		else
		if ( isspace( *label )
		|| ( ispunct( *label ) && *label != '\'' ) )
		{
			beginning = 1;
			*ptr++ = *label++;
		}
		else
		{
			*ptr++ = *label++;
		}
	}

	*ptr = '\0';

	return strdup( display );
}

char *button_input_html(
		char *display,
		char *action_string,
		char *hover_message )
{
	char html[ STRING_4K ];
	char *ptr = html;

	if ( !display
	||   !action_string )
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
		"<input type=button value=\"%s\"",
		display );

	ptr += sprintf( ptr,
		" onClick=\"%s\"",
		action_string );

	if ( hover_message )
	{
		ptr += sprintf( ptr,
			" title=\"%s\"",
			hover_message );
	}

	ptr += sprintf( ptr, ">" );

	return strdup( html );
}

char *button_anchor_html(
		char *display,
		char *href_action_string,
		char *onclick_action_string,
		char *hover_message )
{
	char html[ STRING_4K ];
	char *ptr = html;

	if ( !display
	||   !href_action_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr, "<button type=button" );

	if ( onclick_action_string )
	{
		ptr += sprintf(
			ptr,
			" onClick=\"%s\"",
			onclick_action_string );
	}

	if ( hover_message )
	{
		ptr += sprintf(
			ptr,
			" title=\"%s\"",
			hover_message );
	}

	ptr += sprintf(
		ptr,
		"><a href=\"%s\">%s</a></button>",
		href_action_string,
		display );

	return strdup( html );
}

