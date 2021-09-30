/* $APPASERVER_HOME/library/document.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
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

char *document_type_string( void )
{
	return
	"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\">";
}

char *document_html_standard_url( void )
{
	return
	"http://www.w3.org/1999/xhtml";
}

DOCUMENT *document_new(	void )
{
	DOCUMENT *document = document_calloc();

	/* Returns program memory */
	/* ---------------------- */
	document->type_string = document_type_string();

	/* Returns program memory */
	/* ---------------------- */
	document->standard_string = document_standard_string();

	return document;
}

void document_output_content_type( void )
{
	printf( "Content-type: text/html\n\n" );
	fflush( stdout );
}

void document_output(
			FILE *output_stream,
			char *type_string,
			char *standard_string )
{
	fprintf(output_stream,
		"%s\n%s\n",
		type_string,
		standard_string );
}

void document_quick_output( char *application_name )
{
	DOCUMENT *document;

	document = document_new();

	document->document_head =
		document_head_new(
			application_title_string( application_name ),
			(char *)0 /* menu_setup_string */,
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */ );

	document_output_content_type();

	document_output(
		stdout,
		document->type_string,
		document->standard_string );

	document_head_output(
		stdout,
		document->document_head );

	document_body_tag_output(
		stdout,
		(char *)0 /* onload_string */ );
}

DOCUMENT_BODY *document_body_new(
			char *onload_string,
			MENU *menu )
{
	DOCUMENT_BODY *document_body = document_body_calloc();

	document_body->onload_string = onload_string;
	document_body->menu = menu;

	return document_body;
}

void document_body_tag_output(
			FILE *output_stream,
			char *onload_string )
{
	char onload_attribute[ 1024 ];

	*onload_attribute = '\0';

	if ( onload_string && *onload_string )
	{
		sprintf(onload_attribute,
			" onload=\"%s\"",
			onload_string );
	}

	fprintf(output_stream,
"<body%s leftmargin=0 topmargin=0 marginwidth=0 marginheight=0>",
		onload_attribute );
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
			char *title_string,
			char *menu_setup_string,
			char *calendar_setup_string,
			char *javascript_include_string )
{
	DOCUMENT_HEAD *document_head = document_head_calloc();

	document_head->title_string = title_string;
	document_head->menu_setup_string = menu_setup_string;
	document_head->calendar_setup_string = calendar_setup_string;
	document_head->javascript_include_string = javascript_include_string;

	/* Returns program memory */
	/* ---------------------- */
	document_head->meta_string = document_head_meta_string();

	document_head->stylesheet_string =
		document_head_stylesheet_string(
			environment_application_name() );

	document_head->title_tag =
		document_head_title_tag(
			title_string );

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
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/form.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/form_cookie.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/keystrokes.js\"></SCRIPT>"
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/javascript/verify_attribute_widths.js\"></SCRIPT>";

}

char *document_head_menu_setup_string( void )
{
	return
"<link rel=stylesheet type=text/css href=\"/appaserver/zmenu/src/style-template.css\">\n"
"<link rel=stylesheet type=text/css href=\"/appaserver/zmenu/src/skin-template.css\">\n"
"<script type=text/javascript> _dynarch_menu_url=\"/appaserver/zmenu/src/\"; </script>\n"
"<script type=text/javascript> _dynarch_top=\"/appaserver/zmenu/\"; </script>\n"
"<script type=text/javascript src=\"/appaserver/zmenu/src/hmenu.js\"> </script>";

}

void document_head_output(
			FILE *output_stream,
			DOCUMENT_HEAD *document_head )
{
	fprintf( output_stream, "<head>\n" );
	fprintf( output_stream, "%s\n", document_head->meta_string );
	fprintf( output_stream, "%s\n", document_head->stylesheet_string );
	fprintf( output_stream, "%s\n", document_head->title_tag );

	if ( document_head->menu_setup_string )
	{
		fprintf(output_stream,
			"%s\n",
			document_head->menu_setup_string );
	}

	if ( document_head->calendar_setup_string )
	{
		fprintf(output_stream,
			"%s\n",
			document_head->calendar_setup_string );
	}

	if ( document_head->javascript_include_string )
	{
		fprintf(output_stream,
			"%s\n",
			document_head->javascript_include_string );
	}

	fprintf( output_stream, "</head>\n" );
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

void document_close( FILE *output_stream )
{
	fprintf( output_stream, "</body>\n" );
	fprintf( output_stream, "</html>\n" );
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

char *document_body_menu_onload_string( void )
{
	return
"DynarchMenu.setup( 'menu', {electric: 250, blink: false, lazy: true, scrolling: true} )";
}

char *document_body_hide_preload_message( void )
{
	return
"<!-- following there's an workaround to hide the UL contents while the page is loading ;-) -->\n"
"<script type=\"text/javascript\">//<![CDATA[ \ndocument.writeln(\"<style type='text/css'>#menu { display: none; }</style>\");\n"
"//]]></script>\n\n";
}

char *document_standard_string( void )
{
	return
"xmlns=\"http://www.w3.org/1999/xhtml\"";
}

/* Returns program memory */
/* ---------------------- */
char *document_head_calendar_setup_string( void )
{
	return
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/jscal2.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/border-radius.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/gold/gold.css>\n"
"<script type=text/javascript src=/appaserver/zscal2/src/js/jscal2.js></script>\n"
"<script type=text/javascript src=/appaserver/zscal2/src/js/lang/en.js></script>";

}

void document_body_horizontal_menu_output(
			FILE *output_stream,
			char *hide_preload_message,
			MENU *menu )
{
	if ( menu && list_length( menu->menu_verb_list ) )
	{
		menu_verb_horizontal_output(
			output_stream,
			hide_preload_message,
			menu->menu_verb_list );
	}
}

DOCUMENT *document_choose_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_isa_folder_name,
			MENU *menu,
			LIST *primary_key_list,
			LIST *delimited_list,
			char *action_string )
{
	DOCUMENT *document = document_calloc();

	document->document_head =
		document_head_new(
			title,
			document_head_menu_setup_string(),
			(char *)0 /* calendar_setup_string */,
			document_head_javascript_include_string() );

	document->document_body =
		document_body_choose_isa_new(
			title,
			prompt_message,
			one2m_isa_folder_name,
			menu,
			primary_key_list,
			delimited_list,
			action_string );

	return document;
}

DOCUMENT_BODY *document_body_choose_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_isa_folder_name,
			MENU *menu,
			LIST *primary_key_list,
			LIST *delimited_list,
			char *action_string )
{
	DOCUMENT_BODY *document_body;

	document_body =
		document_body_new(
			document_body_onload_string(
				document_body_menu_onload_string(),
				(char *)0 /* additional_onload_string */ ),
			menu );

	if ( !document_body )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: document_body_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	document_body->form_prompt_isa =
		form_prompt_isa_new(
			title,
			prompt_message,
			one2m_isa_folder_name,
			primary_key_list,
			delimited_list,
			action_string );

	if ( !document_body->form_prompt_isa )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_prompt_isa_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return document_body;
}

