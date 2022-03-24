/* $APPASERVER_HOME/library/document.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "environ.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"

DOCUMENT *document_calloc( void )
{
	DOCUMENT *document;

	if ( ! ( document = calloc( 1, sizeof( DOCUMENT ) ) ) )
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
			char *title_html,
			char *subtitle_html,
			char *subsubtitle_html,
			char *javascript_replace,
			boolean menu_boolean,
			MENU *menu,
			char *menu_setup_string,
			char *calendar_setup_string,
			char *javascript_include_string,
			char *input_onload_string )
{
	DOCUMENT *document = document_calloc();

	document->document_head =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_head_new(
			application_name,
			application_title_string,
			menu_setup_string,
			calendar_setup_string,
			javascript_include_string );

	document->document_body =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_body_new(
			menu_boolean,
			menu,
			title_html,
			subtitle_html,
			subsubtitle_html,
			javascript_replace,
			input_onload_string );

	/* Returns program memory */
	/* ---------------------- */
	document->type_html = document_type_html();

	/* Returns program memory */
	/* ---------------------- */
	document->standard_html = document_standard_html();

	document->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_html(
			document->type_html,
			document->standard_html );

	return document;
}

DOCUMENT *document_quick_new(
			char *application_name,
			char *application_title_string )
{
	DOCUMENT *document = document_calloc();

	document->document_head =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_head_new(
			application_name,
			application_title_string,
			(char *)0 /* menu_setup_string */,
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */ );

	document->document_body =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_body_new(
			0 /* not menu_boolean */,
			(MENU *)0,
			(char *)0 /* title_html */,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			(char *)0 /* input_onload_string */ );

	/* Returns program memory */
	/* ---------------------- */
	document->type_html = document_type_html();

	/* Returns program memory */
	/* ---------------------- */
	document->standard_html = document_standard_html();

	document->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_html(
			document->type_html,
			document->standard_html );

	return document;
}

char *document_content_type_html( void )
{
	return "Content-type: text/html\n";
}

void document_output_content_type( void )
{
	static boolean did_already = 0;

	if ( !did_already )
	{
		fflush( stdout );
		printf( "%s\n", document_content_type_html() );
		fflush( stdout );
		did_already = 1;
	}
}

char *document_html(	char *type_html,
			char *standard_html )
{
	char html[ 1024 ];

	if ( !type_html || !standard_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n<html %s>\n",
		type_html,
		standard_html );

	return strdup( html );
}

void document_quick_output( char *application_name )
{
	DOCUMENT *document;

	document_output_content_type();

	document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_quick_new(
			application_name,
			application_title_string( application_name ) );

	printf( "%s\n", document->html );
	printf( "%s\n", document->document_head->html );
	printf( "%s\n", document_head_close_html() );
	printf( "%s\n", document->document_body->html );
}

DOCUMENT_BODY *document_body_calloc( void )
{
	DOCUMENT_BODY *document_body;

	if ( ! ( document_body = calloc( 1, sizeof( DOCUMENT_BODY ) ) ) )
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
			boolean menu_boolean,
			MENU *menu,
			char *title_html,
			char *subtitle_html,
			char *subsubtitle_html,
			char *javascript_replace,
			char *input_onload_string )
{
	DOCUMENT_BODY *document_body = document_body_calloc();

	document_body->menu_onload_string =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		document_body_menu_onload_string(
			menu_boolean );

	document_body->tag =
		document_body_tag(
			input_onload_string,
			document_body->menu_onload_string,
			javascript_replace );

	document_body->hide_preload_html =
		document_body_hide_preload_html(
			menu_boolean );

	document_body->horizontal_menu_html =
		/* -------------------------- */
		/* Returns menu->html or null */
		/* -------------------------- */
		document_body_horizontal_menu_html(
			menu_boolean,
			menu );

	document_body->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_html(
			document_body->tag,
			document_body->horizontal_menu_html,
			title_html,
			subtitle_html,
			subsubtitle_html );

	return document_body;
}

char *document_body_tag(
			char *input_onload_string,
			char *menu_onload_string,
			char *javascript_replace )
{
	char tag[ 2048 ];
	char *ptr = tag;
	boolean did_one = 0;

	ptr += sprintf(
		ptr,
		"<body leftmargin=0 topmargin=0 marginwidth=0 marginheight=0" );

	if ( ( input_onload_string && *input_onload_string )
	||   ( menu_onload_string && *menu_onload_string )
	||   ( javascript_replace && *javascript_replace ) )
	{
		ptr += sprintf(
			ptr,
			"<onload=\"" );

		if ( input_onload_string && *input_onload_string )
		{
			ptr += sprintf(
				ptr,
				"%s",
				input_onload_string );

			did_one = 1;
		}

		if ( menu_onload_string && *menu_onload_string )
		{
			if ( did_one ) ptr += sprintf( ptr, " && " );

			ptr += sprintf(
				ptr,
				"%s",
				menu_onload_string );

			did_one = 1;
		}

		if ( javascript_replace && *javascript_replace )
		{
			if ( did_one ) ptr += sprintf( ptr, " && " );

			ptr += sprintf(
				ptr,
				"%s",
				javascript_replace );
		}

		ptr += sprintf(
			ptr,
			"\">" );
	}

	return strdup( tag );
}

char *document_body_hide_preload_html( boolean menu_boolean )
{
	if ( !menu_boolean ) return (char *)0;

	return
"<script type=\"text/javascript\">//<![CDATA[ \ndocument.writeln(\"<style type='text/css'>#menu { display: none; }</style>\");//]]></script>\n\n";
}

char *document_body_horizontal_menu_html(
			boolean menu_boolean,
			MENU *menu )
{
	if ( !menu_boolean ) return (char *)0;
	if ( !menu ) return (char *)0;

	return menu->html;
}

char *document_body_title_html( char *application_title_string )
{
	static char title_html[ 256 ];

	if ( !application_title_string || !*application_title_string )
		return (char *)0;

	sprintf(title_html,
		"<h1>%s</h1",
		application_title_string );

	return title_html;
}

char *document_body_html(
			char *tag,
			char *horizontal_menu_html,
			char *title_html,
			char *subtitle_html,
			char *subsubtitle_html )
{
	char html[ STRING_64K ];
	char *ptr = html;

	if ( !tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: tag is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "%s", tag );

	if ( horizontal_menu_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			horizontal_menu_html );
	}

	if ( title_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			title_html );
	}

	if ( subtitle_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			subtitle_html );
	}

	if ( subsubtitle_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			subsubtitle_html );
	}

	return strdup( html );
}

char *document_body_set_onload_control_string(
			char *onload_control_string,
			char *append_onload_control_string )
{
	char control_string[ STRING_64K ];

	if ( !onload_control_string && !append_onload_control_string )
	{
		return (char *)0;
	}

	if ( !onload_control_string )
	{
		strcpy( control_string, append_onload_control_string );
	}
	else
	{
		sprintf(control_string,
			"%s && %s",
			onload_control_string,
			append_onload_control_string );

		free( onload_control_string );
	}

	return strdup( control_string );
}

char *document_body_close_html( void )
{
	return "</body>";
}

DOCUMENT_HEAD *document_head_calloc( void )
{
	DOCUMENT_HEAD *document_head;

	if ( ! ( document_head = calloc( 1, sizeof( DOCUMENT_HEAD ) ) ) )
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
	DOCUMENT_HEAD *document_head = document_head_calloc();

	/* Returns program memory */
	/* ---------------------- */
	document_head->meta_string = document_head_meta_string();

	document_head->stylesheet_string =
		document_head_stylesheet_string(
			application_name );

	document_head->title_tag =
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
"\n<meta name=\"generator\" content=\"Appaserver: Open Source Application Server\" />\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
}

char *document_head_javascript_include_string( void )
{
	return
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/trim.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/timlib.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/cookie.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/null2slash.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/form.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/form_cookie.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/keystrokes.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/verify_attribute_widths.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/push_button_submit.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/validate_date.js\"></SCRIPT>";

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
"<link rel=stylesheet type=text/css href=\"/appaserver/zmenu/src/style-template.css\">\n"
"<link rel=stylesheet type=text/css href=\"/appaserver/zmenu/src/skin-template.css\">\n"
"<script type=text/javascript> _dynarch_menu_url=\"/appaserver/zmenu/src/\"; </script>\n"
"<script type=text/javascript> _dynarch_top=\"/appaserver/zmenu/\"; </script>\n"
"<script type=text/javascript src=\"/appaserver/zmenu/src/hmenu.js\"> </script>";
	}
}

char *document_head_open_html(
			char *meta_string,
			char *stylesheet_string,
			char *title_tag,
			char *menu_setup_string,
			char *calendar_setup_string,
			char *javascript_include_string )
{
	char html[ STRING_INPUT_BUFFER ];
	char *ptr = html;

	ptr += sprintf( ptr, "<head>\n" );

	if ( meta_string )
	{
		ptr += sprintf( ptr, "%s\n", meta_string );
	}

	if ( stylesheet_string )
	{
		ptr += sprintf( ptr, "%s\n", stylesheet_string );
	}

	if ( title_tag )
	{
		ptr += sprintf( ptr, "%s\n", title_tag );
	}

	if ( menu_setup_string )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			menu_setup_string );
	}

	if ( calendar_setup_string )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			calendar_setup_string );
	}

	if ( javascript_include_string )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			javascript_include_string );
	}
	return strdup( html );
}

char *document_head_close_html( void )
{
	return "</head>";
}

char *document_head_stylesheet_string(
			char *application_name )
{
	char stylesheet_string[ 256 ];

	sprintf(stylesheet_string,
"<link rel=stylesheet type=\"text/css\" href=\"/appaserver/%s/style.css\">",
		application_name );

	return strdup( stylesheet_string );
}

char *document_head_title_tag(
			char *title_string )
{
	char title_tag[ 256 ];

	sprintf(title_tag,
		"<title>%s</title>",
		title_string );

	return strdup( title_tag );
}

void document_close( void )
{
	/* Returns program memory */
	/* ---------------------- */
	printf( "%s\n", document_close_html() );
}

char *document_close_html( void )
{
	return "</body>\n</html>";
}

char *document_body_onload_string(
			char *menu_onload_string,
			char *additional_onload_string )
{
	char string[ 1024 ];
	char *ptr = string;

	*ptr = '\0';

	if ( menu_onload_string )
	{
		ptr += sprintf( ptr, "%s", menu_onload_string );
	}

	if ( additional_onload_string )
	{
		if ( ptr != string ) ptr += sprintf( ptr, ";" );

		ptr += sprintf( ptr, "%s", additional_onload_string );
	}

	return strdup( string );
}

char *document_body_menu_onload_string( boolean menu_boolean )
{
	if ( !menu_boolean ) return (char *)0;

	return
"DynarchMenu.setup( 'menu', {electric: 250, blink: false, lazy: true, scrolling: true} )";
}

char *document_standard_html( void )
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
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/jscal2.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/border-radius.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/gold/gold.css>\n"
"<script type=text/javascript src=/appaserver/zscal2/src/js/jscal2.js></script>\n"
"<script type=text/javascript src=/appaserver/zscal2/src/js/lang/en.js></script>";
	}
}

void document_output_html_stream( FILE *output_stream )
{
	DOCUMENT *document;
	char *application_name = environment_application_name();

	document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_quick_new(
			application_name,
			application_title_string( application_name ) );

	fprintf( output_stream, "%s\n", document->html );
	fprintf( output_stream, "%s\n", document->document_head->html );
	fprintf( output_stream, "%s\n", document_head_close_html() );
	fprintf( output_stream, "%s\n", document->document_body->html );
}

char *document_head_html(
			char *meta_string,
			char *stylesheet_string,
			char *title_tag,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			char *menu_setup_string,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			char *calendar_setup_string,
			char *javascript_include_string )
{
	char html[ STRING_64K ];
	char *ptr = html;

	ptr += sprintf( ptr, "%s\n", meta_string );
	ptr += sprintf( ptr, "%s\n", stylesheet_string );
	ptr += sprintf( ptr, "%s\n", title_tag );

	if ( menu_setup_string )
	{
		ptr += sprintf( ptr, "%s\n", menu_setup_string );
	}

	if ( calendar_setup_string )
	{
		ptr += sprintf( ptr, "%s\n", calendar_setup_string );
	}

	ptr += sprintf( ptr, "%s\n", javascript_include_string );

	return strdup( html );
}

char *document_form_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html,
			char *form_html,
			char *document_body_close_html,
			char *document_close_html )
{
	char html[ STRING_FOUR_MEG ];

	if ( !document_html
	||   !document_head_html
	||   !document_head_close_html
	||   !document_body_html
	||   !form_html
	||   !document_body_close_html
	||   !document_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s",
		document_html,
		document_head_html,
		document_head_close_html,
		document_body_html,
		form_html,
		document_body_close_html,
		document_close_html );

	return strdup( html );
}

