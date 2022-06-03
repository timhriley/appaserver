/* $APPASERVER_HOME/library/button.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "frameset.h"
#include "javascript.h"
#include "piece.h"
#include "appaserver_parameter.h"
#include "element.h"
#include "application.h"
#include "appaserver_library.h"
#include "pair_one2m.h"
#include "button.h"

BUTTON *button_calloc( void )
{
	BUTTON *button;

	if ( ! ( button = calloc( 1, sizeof( BUTTON ) ) ) )
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

BUTTON *button_new(	char *label,
			char *action_string )
{
	BUTTON *button = button_calloc();

	if ( !label || !action_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	button->html =
		button_html(
			label,
			action_string );

	return button;
}

char *button_html(	char *label,
			char *action_string )
{
	char html[ 1024 ];

	if ( !label || !action_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
"<input type=button value=\"%s\" onClick=\"%s\">",
		label,
		action_string );

	return strdup( html );
}

char *button_reset_html(
			char *javascript_replace,
			int form_number )
{
	char html[ 1024 ];
	char *ptr = html;

	ptr += sprintf(
		ptr,
"<td><input type=\"button\" value=\"Reset\" onClick=\"form_reset(document.forms[%d], '%c')",
		form_number,
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

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
			char *form_multi_select_all_javascript,
			char *recall_save_javascript,
			char *form_verify_notepad_widths_javascript,
			int form_number )
{
	BUTTON *button = button_calloc();

	button->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_submit_html(
			form_multi_select_all_javascript,
			recall_save_javascript,
			form_verify_notepad_widths_javascript,
			form_number );

	return button;
}

char *button_submit_html(
			char *form_multi_select_all_javascript,
			char *recall_save_javascript,
			char *form_verify_notepad_widths_javascript,
			int form_number )
{
	char html[ 4096 ];
	char *ptr = html;
	boolean got_one = 0;

	ptr += sprintf(
		ptr,
"<input type=button value=\"%s\" onClick=\"",
		BUTTON_SUBMIT_LABEL );

	if ( form_multi_select_all_javascript )
	{
		ptr += sprintf(
			ptr,
			"%s",
			form_multi_select_all_javascript );

		got_one = 1;
	}

	if ( recall_save_javascript )
	{
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
		"document.forms[%d].submit();\">",
		form_number );

	return strdup( html );
}

BUTTON *button_back_to_drillthru( char *action_string )
{
	BUTTON *button = button_calloc();

	button->html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		button_back_to_drillthru_html(
			action_string );

	return button;
}

char *button_back_to_drillthru_html( char *action_string )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !action_string || !*action_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: action_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Close form[0] */
	/* ------------- */
	ptr += sprintf(
		ptr,
		"</form>\n" );

	/* Open form[1] */
	/* ------------ */
	ptr += sprintf(
		ptr,
"<form enctype=\"multipart/form-data\" method=post action=\"%s\" target=%s>\n"
"<input type=button value=\"Back to Drillthru\" onClick=\"document.forms[1].submit()\"\n",
		action_string,
		FRAMESET_PROMPT_FRAME );

	return strdup( html );
}

BUTTON *button_back_to_top( void )
{
	BUTTON *button = button_calloc();

	/* Safely returns heap memory */
	/* -------------------------- */
	button->html = button_back_to_top_html();

	return button;
}

char *button_back_to_top_html( void )
{
	char html[ 256 ];

	sprintf(html,
"<td><a onClick=\"%s\"><img src=\"/%s/top.png\"></a>",
		"window.scrollTo(0,0)",
		BUTTON_IMAGE_RELATIVE_DIRECTORY );

	return strdup( html );
}

BUTTON *button_recall( char *recall_load_javascript )
{
	BUTTON *button = button_calloc();

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	button->html =
		button_recall_html(
			recall_load_javascript );

	if ( !button->html )
	{
		free( button );
		return (BUTTON *)0;
	}

	return button;
}

char *button_recall_html(
			char *recall_load_javascript )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !recall_load_javascript || !*recall_load_javascript )
	{
		return (char *)0;
	}

	ptr += sprintf( ptr,
"<td><input type=button value=Recall onClick=\"%s\">",
		recall_load_javascript );

	return strdup( html );
}

BUTTON *button_back( void )
{
	BUTTON *button = button_calloc();

	/* Returns program memory */
	/* ---------------------- */
	button->html = button_back_html();

	return button;
}

char *button_back_html( void )
{
	return
"<td><input type=\"button\" value=\"Back\" onClick=\"history.back()\">";
}

BUTTON *button_forward( void )
{
	BUTTON *button = button_calloc();

	/* Returns program memory */
	/* ---------------------- */
	button->html = button_forward_html();

	return button;
}

char *button_forward_html( void )
{
	return
"<td><input type=\"button\" value=\"Forward\" onClick=\"timlib_history_forward()\">";

}

BUTTON *button_reset(	char *javascript_replace,
			int form_number )
{
	BUTTON *button = button_calloc();

	button->html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		button_reset_html(
			javascript_replace,
			form_number );

	return button;
}

BUTTON *button_drillthru_skip( void )
{
	BUTTON *button = button_calloc();

	/* Safely returns heap memory */
	/* -------------------------- */
	button->html = button_drillthru_skip_html();

	return button;
}

char *button_drillthru_skip_html( void )
{
	char html[ 1024 ];

	sprintf(html,
"<td><input type=\"button\" value=\"Skip\" title=\"%s\" onClick=\"form_reset(document.forms[0], '%c'); document.forms[0].submit()\">",
		BUTTON_SKIP_MESSAGE,
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	return strdup( html );
}

BUTTON *button_help(	char *application_name,
			char *html_help_file_anchor )
{
	BUTTON *button = button_calloc();

	button->html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		button_help_html(
			application_name,
			html_help_file_anchor );

	return button;
}

char *button_help_html(	char *application_name,
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
			PATH_DELIMITER,
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
"<td><input type=button value=\"Help\" onclick='window.open(\"%s\",\"help_window\",\"width=600,height=350,resizable=yes,scrollbars=yes\")'>",
		full_pathname );

	return strdup( html );
}

LIST *button_insert_pair_one2m_submit_list(
			LIST *pair_one2m_folder_list )
{
	PAIR_ONE2M_FOLDER *pair_one2m_folder;
	LIST *button_list;
	BUTTON *button;

	if ( !list_rewind( pair_one2m_folder_list ) )
	{
		return (LIST *)0;
	}

	button_list = list_new();

	do {
		pair_one2m_folder =
			list_get( 
				pair_one2m_folder_list );

		button = button_calloc();

		if ( !pair_one2m_folder->folder_button_string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_button_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		button->html =
			pair_one2m_folder->
				folder_button_string;

		list_set(
			button_list,
			button );

	} while( list_next( pair_one2m_folder_list ) );

	return button_list;
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
		"<table border=0\n<tr>\n" );

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
			"%s\n",
			button->html );

	} while ( list_next( button_list ) );

	ptr += sprintf(
		ptr,
		"</table>\n" );

	return strdup( html );
}

void button_list_free( LIST *button_list )
{
	BUTTON *button;

	if ( !list_rewind( button_list ) ) return;

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

		free( button->html );
		free( button );

	} while ( list_next( button_list ) );

	list_free_container( button_list );
}

