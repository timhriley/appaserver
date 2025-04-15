/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/document.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "environ.h"
#include "application.h"
#include "appaserver_error.h"
#include "javascript.h"
#include "appaserver.h"
#include "date.h"
#include "widget.h"
#include "document.h"

DOCUMENT *document_calloc( void )
{
	DOCUMENT *document;

	if ( ! ( document = calloc( 1, sizeof ( DOCUMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return document;
}

char *document_type_html( void )
{
	return
	"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\">";
}

DOCUMENT *document_new(
		char *application_name,
		char *application_title_string,
		char *title_string,
		char *sub_title_string,
		char *sub_sub_title_string,
		char *notepad,
		char *onload_javascript_string,
		char *menu_html,
		char *menu_setup_string,
		char *calendar_setup_string,
		char *javascript_include_string )
{
	DOCUMENT *document;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	document = document_calloc();
	document->javascript_include_string = javascript_include_string;

	/* Returns program memory */
	/* ---------------------- */
	document->type_html = document_type_html();

	/* Returns program memory */
	/* ---------------------- */
	document->standard_string = document_standard_string();

	document->html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		document_html(
			document->type_html,
			document->standard_string );

	document->document_head =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_head_new(
			application_name,
			application_title_string,
			menu_setup_string,
			calendar_setup_string,
			javascript_include_string );

	document->document_body =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_body_new(
			menu_html,
			title_string,
			sub_title_string,
			sub_sub_title_string,
			notepad,
			onload_javascript_string );

	return document;
}

DOCUMENT *document_basic_new(
		char *application_name,
		LIST *javascript_filename_list,
		char *application_title_string )
{
	DOCUMENT *document = document_calloc();

	/* Returns program memory */
	/* ---------------------- */
	document->type_html = document_type_html();

	/* Returns program memory */
	/* ---------------------- */
	document->standard_string = document_standard_string();

	document->html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		document_html(
			document->type_html,
			document->standard_string );

	document->javascript_include_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		javascript_include_string(
			application_name,
			javascript_filename_list );

	document->document_head =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_head_new(
			application_name,
			application_title_string,
			(char *)0 /* menu_setup_string */,
			(char *)0 /* calendar_setup_string */,
			document->javascript_include_string );

	document->document_body =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_body_new(
			(char *)0 /* menu_html */,
			(char *)0 /* title_string */,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			(char *)0 /* notepad */,
			(char *)0 /* onload_javascript_string */ );

	return document;
}

char *document_content_type_html( void )
{
	return "Content-type: text/html\n";
}

void document_content_type_output( void )
{
	static boolean did_already;

	if ( did_already ) return;

	printf( "%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_content_type_html() );
	fflush( stdout );

	did_already = 1;
}

char *document_html(
		char *type_html,
		char *standard_string )
{
	static char html[ 256 ];

	if ( !type_html || !standard_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n<html %s>",
		type_html,
		standard_string );

	return html;
}

char *document_process_title_string( char *title_string )
{
	static char string[ 128 ];

	if ( !title_string ) return (char *)0;

	return
	string_initial_capital(
		string,
		title_string );
}

char *document_process_sub_title_string( void )
{
	return
	date_now_yyyy_mm_dd_hhmm(
		date_utc_offset() );
}

void document_process_output(
		char *application_name,
		LIST *javascript_filename_list,
		char *title_string )
{
	DOCUMENT *document;
	char *sub_title_string;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message,
			"ERROR in %s/%s()/%d: application_name is empty.",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		fprintf(stderr,
			"%s\n",
			message );

		exit( 1 );
	}

	document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_basic_new(
			application_name,
			javascript_filename_list,
			application_title_string( application_name ) );

	printf( "%s\n%s\n%s\n%s\n",
		document->html,
		document->document_head->html,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_close_tag(),
		document->document_body->html );

	title_string =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		document_process_title_string(
			title_string );

	if ( title_string )
	{
		sub_title_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			document_process_sub_title_string();

		printf(	"<h1>%s</h1>\n<h2>%s</h2>\n",
			title_string,
			sub_title_string );

	}
	fflush( stdout );
}

DOCUMENT_BODY *document_body_calloc( void )
{
	DOCUMENT_BODY *document_body;

	if ( ! ( document_body = calloc( 1, sizeof ( DOCUMENT_BODY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return document_body;
}

DOCUMENT_BODY *document_body_new(
		char *menu_html,
		char *title_string,
		char *sub_title_string,
		char *sub_sub_title_string,
		char *notepad,
		char *onload_javascript_string )
{
	DOCUMENT_BODY *document_body = document_body_calloc();

	if ( menu_html )
	{
		document_body->menu_onload_string =
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_menu_onload_string();

		document_body->hide_preload_html =
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_hide_preload_html();
	}

	document_body->onload_open_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_onload_open_tag(
			JAVASCRIPT_WAIT_OVER,
			onload_javascript_string,
			document_body->menu_onload_string );

	if ( notepad )
	{
		document_body->notepad_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			document_body_notepad_html(
				notepad );
	}

	document_body->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_html(
			menu_html,
			title_string,
			sub_title_string,
			sub_sub_title_string,
			document_body->onload_open_tag
				/* body_onload_open_tag */,
			document_body->hide_preload_html,
			document_body->notepad_html );

	return document_body;
}

char *document_body_onload_open_tag(
		const char *javascript_wait_over,
		char *onload_javascript_string,
		char *menu_onload_string )
{
	char tag[ 4096 ];
	char *ptr = tag;

	ptr += sprintf(
		ptr,
		"<body leftmargin=0 topmargin=0 marginwidth=0 marginheight=0" );

	ptr += sprintf(
		ptr,
		" onload=\"%s",
		javascript_wait_over );

	if ( menu_onload_string )
	{
		ptr += sprintf(
			ptr,
			" && %s",
			menu_onload_string );
	}

	if ( onload_javascript_string )
	{
		ptr += sprintf(
			ptr,
			" && %s",
			onload_javascript_string );
	}

	ptr += sprintf( ptr, "\">" );

	return strdup( tag );
}

char *document_body_hide_preload_html( void )
{
	return
"<script type=\"text/javascript\">//<![CDATA[ \ndocument.writeln(\"<style type='text/css'>#menu { display: none; }</style>\");//]]></script>\n\n";
}

char *document_body_html(
		char *menu_html,
		char *title_string,
		char *sub_title_string,
		char *sub_sub_title_string,
		char *body_onload_open_tag,
		char *hide_preload_html,
		char *document_body_notepad_html )
{
	char html[ STRING_64K ];
	char *ptr = html;

	*ptr = '\0';

	if ( body_onload_open_tag )
	{
		ptr += sprintf( ptr, "%s", body_onload_open_tag );
	}

	if ( menu_html && hide_preload_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s\n%s",
			hide_preload_html,
			menu_html );
	}

	if ( title_string && *title_string )
	{
		ptr += sprintf(
			ptr,
			"\n<h1>%s</h1>",
			title_string );
	}

	if ( sub_title_string && *sub_title_string )
	{
		ptr += sprintf(
			ptr,
			"\n<h2>%s</h2>",
			sub_title_string );
	}

	if ( document_body_notepad_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			document_body_notepad_html );
	}

	if ( sub_sub_title_string && *sub_sub_title_string )
	{
		ptr += sprintf(
			ptr,
			"\n<h3>%s</h3>",
			sub_sub_title_string );
	}

	return strdup( html );
}

char *document_body_set_onload_javascript_string(
		char *javascript_string,
		char *append_javascript_string )
{
	char onload_javascript_string[ STRING_64K ];

	if ( !javascript_string && !append_javascript_string ) return NULL;
	if ( !javascript_string ) return append_javascript_string;
	if ( !append_javascript_string ) return javascript_string;

	sprintf(onload_javascript_string,
		"%s && %s",
		javascript_string,
		append_javascript_string );

	return strdup( onload_javascript_string );
}

char *document_body_open_tag( void )
{
	return "<body>";
}

char *document_body_close_tag( void )
{
	return "</body>";
}

DOCUMENT_HEAD *document_head_calloc( void )
{
	DOCUMENT_HEAD *document_head;

	if ( ! ( document_head = calloc( 1, sizeof ( DOCUMENT_HEAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return document_head;
}

DOCUMENT_HEAD *document_head_new(
		char *application_name,
		char *application_title_string,
		char *menu_setup_string,
		char *calendar_setup_string,
		char *javascript_include_string )
{
	DOCUMENT_HEAD *document_head;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	document_head = document_head_calloc();

	/* Returns program memory */
	/* ---------------------- */
	document_head->meta_string = document_head_meta_string();

	document_head->stylesheet_string =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_stylesheet_string();

	if ( !application_title_string ) application_title_string = "";

	document_head->title_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_head_title_tag(
			application_title_string );

	document_head->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_head_html(
			document_head->meta_string,
			document_head->stylesheet_string,
			document_head->title_tag,
			menu_setup_string,
			calendar_setup_string,
			javascript_include_string );

	return document_head;
}

char *document_head_meta_string( void )
{
	return
"<meta name=\"generator\" content=\"Appaserver: Open Source Application Server\" />\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
"<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />";
}

char *document_head_javascript_string( void )
{
	return
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/trim.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/form.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/timlib.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/cookie.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/null2slash.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/form_cookie.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/keystrokes.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/verify_attribute_widths.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/push_button_submit.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/post_change_multi_select.js\"></SCRIPT>\n"
"<SCRIPT language=JavaScript1.2 src=\"/appaserver_home/javascript/validate_date.js\"></SCRIPT>";
}

char *document_head_menu_setup_string( boolean menu_boolean )
{
	if ( !menu_boolean )
	{
		return (char *)0;
	}
	else
	{
		return
"<link rel=stylesheet type=text/css href=\"/appaserver_home/zmenu/src/style-template.css\">\n"
"<link rel=stylesheet type=text/css href=\"/appaserver_home/zmenu/src/skin-template.css\">\n"
"<script language=JavaScript1.2> _dynarch_menu_url=\"/appaserver_home/zmenu/src/\";</script>\n"
"<script language=JavaScript1.2> _dynarch_top=\"/appaserver_home/zmenu/\";</script>\n"
"<script language=JavaScript1.2 src=\"/appaserver_home/zmenu/src/hmenu.js\"></script>";
	}
}

char *document_head_open_tag( void )
{
	return "<head>";
}

char *document_head_close_tag( void )
{
	return "</head>";
}

char *document_head_stylesheet_string( void )
{
	return
"<link rel=stylesheet type=\"text/css\" href=\"/appaserver_home/template/style.css\">";
}

char *document_head_title_tag( char *title_string )
{
	char title_tag[ 256 ];

	if ( !title_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: title_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title_tag,
		"<title>%s</title>",
		title_string );

	return strdup( title_tag );
}

void document_close( void )
{
	fflush( stdout );
	printf( "%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		document_both_close_tag() );
	fflush( stdout );
}

char *document_script_close_tag( void )
{
	return "</script>";
}

char *document_close_tag( void )
{
	return "</html>";
}

char *document_body_menu_onload_string( void )
{
	return
"DynarchMenu.setup( 'menu', {electric: 250, blink: false, lazy: false, scrolling: true} )";
}

char *document_standard_string( void )
{
	return
"xmlns=\"http://www.w3.org/1999/xhtml\"";
}

char *document_head_calendar_setup_string(
		int folder_attribute_date_name_list_length )
{
	if ( !folder_attribute_date_name_list_length )
	{
		return (char *)0;
	}
	else
	{
		return
"<link rel=stylesheet type=text/css href=/appaserver_home/zscal2/src/css/jscal2.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver_home/zscal2/src/css/border-radius.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver_home/zscal2/src/css/gold/gold.css>\n"
"<script language=Javascript1.2 src=\"/appaserver_home/zscal2/src/js/jscal2.js\"></script>\n"
"<script language=Javascript1.2 src=\"/appaserver_home/zscal2/src/js/lang/en.js\"></script>";
	}
}

void document_head_output_html_stream( FILE *output_stream )
{
	DOCUMENT *document;
	char *application_name = environment_application_name();

	document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_basic_new(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			application_title_string( application_name ) );

	fprintf(output_stream,
		"%s\n%s\n",
		document->html,
		document->document_head->html );
}

char *document_head_html(
		char *meta_string,
		char *stylesheet_string,
		char *title_tag,
		char *menu_setup_string,
		char *calendar_setup_string,
		char *javascript_include_string )
{
	char html[ STRING_128K ];
	char *ptr = html;

	if ( !meta_string
	||   !stylesheet_string
	||   !title_tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_open_tag() );

	ptr += sprintf( ptr, "%s\n", meta_string );
	ptr += sprintf( ptr, "%s\n", stylesheet_string );
	ptr += sprintf( ptr, "%s", title_tag );

	if ( menu_setup_string )
	{
		ptr += sprintf( ptr, "\n%s", menu_setup_string );
	}

	if ( calendar_setup_string )
	{
		ptr += sprintf( ptr, "\n%s", calendar_setup_string );
	}

	if ( javascript_include_string )
	{
		ptr += sprintf( ptr, "\n%s", javascript_include_string );
	}

	return strdup( html );
}

char *document_form_html(
		char *document_html,
		char *document_head_html,
		char *document_head_close_tag,
		char *document_body_html,
		char *form_html,
		char *document_body_close_tag,
		char *document_close_tag )
{
	char html[ STRING_768K ];
	char *ptr = html;
	char *capacity_message;

	*ptr = '\0';

	if ( document_html
	&&   document_head_html
	&&   document_head_close_tag )
	{
		ptr += sprintf(
			ptr,
			"%s\n%s\n%s",
			document_html,
			document_head_html,
			document_head_close_tag );
	}

	if ( document_body_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			document_body_html );
	}

	if ( form_html )
	{
		if (	strlen( html ) +
			strlen( form_html ) +
			1 >= STRING_768K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_768K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"\n%s",
			form_html );
	}

	if ( document_body_close_tag )
	{
		if (	strlen( html ) +
			strlen( document_body_close_tag ) +
			1 >= STRING_768K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_768K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"\n%s",
			document_body_close_tag );
	}

	if ( document_close_tag )
	{
		if (	strlen( html ) +
			strlen( document_close_tag ) +
			1 >= STRING_768K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_768K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"\n%s",
			document_close_tag );
	}


	capacity_message =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		document_capacity_message(
			__FUNCTION__ /* function_name */,
			STRING_768K /* max_strlen */,
			APPASERVER_CAPACITY_THRESHOLD,
			(char *)0 /* label */,
			strlen( html ) /* strlen_html */ );

	if ( capacity_message )
	{
		appaserver_error_message_file(
			(char *)0 /* application_name */,
			(char *)0 /* login_name */,
			capacity_message );
	}

	return strdup( html );
}

void document_head_output_stylesheet(
		FILE *output_stream,
		char *application_name,
		char *stylesheet_filename )
{
	fprintf(output_stream,
"<link rel=stylesheet type=\"text/css\" href=\"/appaserver/%s/%s\">\n",
		application_name,
		stylesheet_filename );
}

char *document_body_notepad_html( char *notepad )
{
	char notepad_html[ 65536 ];

	if ( !notepad ) return NULL;

	snprintf(
		notepad_html,
		sizeof ( notepad_html ),
		"<p style=\"margin: 2%c;\">%s",
		'%',
		notepad );

	return strdup( notepad_html );
}

char *document_capacity_message(
		const char *function_name,
		const int max_strlen,
		const double appaserver_capacity_threshold,
		char *label,
		int strlen_html )
{
	static char capacity_message[ 256 ];
	char *ptr = capacity_message;
	double capacity;

	capacity =
		document_capacity(
			max_strlen,
			strlen_html );

	if ( capacity < appaserver_capacity_threshold ) return NULL;

	ptr += sprintf(
		ptr,
		"%s(): ",
		function_name );

	if ( label )
	{
		ptr += sprintf(
			ptr,
			"For %s, ",
			label );
	}

	sprintf(ptr,
		"returning html_length=%d, which is capacity=%.0lf%c",
		strlen_html,
		capacity,
		'%' );

	return capacity_message;
}

double document_capacity(
		const int max_strlen,
		int strlen_html )
{
	return
	( (double)strlen_html /
	  (double)max_strlen ) * 100.0;
}

char *document_both_close_tag( void )
{
	static char both_close_tag[ 32 ];

	snprintf(
		both_close_tag,
		sizeof ( both_close_tag ),
		"%s\n%s",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_body_close_tag(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_close_tag() );

	return both_close_tag;
}

