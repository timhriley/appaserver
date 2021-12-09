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

DOCUMENT *document_new(	char *application_name,
			MENU *menu,
			boolean menu_boolean,
			char *document_title,
			char *javascript_replace,
			int folder_attribute_date_name_list_length )
{
	DOCUMENT *document = document_calloc();

	document->document_head =
		document_head_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			application_title_string(
				application_name ),
			document_head_menu_setup_string(
				menu_boolean ),
			document_head_calendar_setup_string(
			       folder_attribute_date_name_list_length ),
			document_head_javascript_include_string() );

	document->document_body =
		document_body_new(
			menu,
			menu_boolean,
			document_title,
			javascript_replace );

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

char *document_open_html(
			char *type_string,
			char *standard_string )
{
	char html[ 1024 ];

	if ( !type_string || !standard_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s",
		type_string,
		standard_string );

	return strdup( html );
}

void document_quick_output( char *application_name )
{
	DOCUMENT *document;
	char *tmp;

	document_output_content_type();

	document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			(MENU *)0 /* menu */,
			0 /* not menu_boolean */,
			(char *)0 /* document_title */,
			(char *)0 /* javascript_replace */,
			0 /* folder_attribute_date_name_list */ );

	printf("%s\n",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		( tmp = document_open_html(
				document->type_string,
				document->standard_string ) ) );

	free( tmp );

	printf( "%s\n",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		( tmp =
			document_head_open_html(
				document->document_head ) ) );

	free( tmp );

	printf( "%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_close_html() );

	printf( "%s\n",
		( tmp =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
			document_body_begin_html(
				(char *)0 /* onload_string */ ) ) );

	free( tmp );
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
			MENU *menu,
			boolean menu_boolean,
			char *document_title,
			char *javascript_replace )
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
			document_body->menu_onload_string,
			javascript_replace );

	document_body->hide_preload_html =
		document_body_hide_preload_html(
			menu_boolean );

	document_body->horizontal_menu_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		document_body_horizontal_menu_html(
			document_body->hide_preload_html,
			menu,
			menu_boolean );

	document_body->title_html =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		document_body_title_html(
			document_title );

	document_body->open_html =
		document_body_open_html(
			document_body->tag,
			document_body->horizontal_menu_html,
			document_body->title_html );

	document_body->close_html = document_body_close_html();

	return document_body;
}

char *document_body_tag(
			char *menu_onload_string,
			char *javascript_replace )
{
	char tag[ 2048 ];
	char *ptr = tag;

	ptr += sprintf(
		ptr,
		"<body leftmargin=0 topmargin=0 marginwidth=0 marginheight=0" );

	if ( ( menu_onload_string && *menu_onload_string )
	||   ( javascript_replace && *javascript_replace ) )
	{
		ptr += sprintf(
			ptr,
			"<onload=\"" );

		if ( menu_onload_string && *menu_onload_string )
		{
			ptr += sprintf(
				ptr,
				"%s",
				menu_onload_string );
		}

		if ( menu_onload_string && *menu_onload_string
		&&   javascript_replace && *javascript_replace )
		{
			ptr += sprintf(
				ptr,
				";" );
		}

		if ( javascript_replace && *javascript_replace )
		{
			ptr += sprintf(
				ptr,
				"%s",
				javascript_replace );
		}

		ptr += sprintf(
			ptr,
			"\">" );
	}

	return stdup( tag );
}

char *document_body_hide_preload_html( boolean menu_boolean )
{
	if ( !menu_boolean ) return (char *)0;

	return
"<script type=\"text/javascript\">//<![CDATA[ \ndocument.writeln(\"<style type='text/css'>#menu { display: none; }</style>\");//]]></script>\n\n";
}

char *document_body_menu_horizontal_html(
			char *hide_preload_html,
			MENU *menu,
			boolean menu_boolean )
{
	if ( !menu_boolean ) return (char *)0;

	if ( !menu )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Returns heap memory or null */
	/* --------------------------- */
	return menu_horizontal_html(
			hide_preload_html,
			menu->menu_verb_list );
}

char *document_body_title_html( char *document_title )
{
	static char title_html[ 256 ];

	if ( !document_title || !*document_title ) return (char *)0;

	sprintf(title_html,
		"<h1>%s</h1",
		document_title );

	return title_html;
}

char *document_body_open_html(
			char *tag,
			char *horizontal_menu_html,
			char *title_html )
{
}

char *document_body_close_html( void )
{
	return "</body>";
}

char *document_body_begin_html(
			char *onload_string )
{
	char html[ STRING_INPUT_BUFFER ];
	char onload_attribute[ STRING_INPUT_BUFFER ];

	*onload_attribute = '\0';

	if ( onload_string && *onload_string )
	{
		sprintf(onload_attribute,
			" onload=\"%s\"",
			onload_string );
	}

	sprintf(html,
"<body%s leftmargin=0 topmargin=0 marginwidth=0 marginheight=0>",
		onload_attribute );

	return strdup( html );
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
			DOCUMENT_HEAD *document_head )
{
	char html[ STRING_INPUT_BUFFER ];
	char *ptr = html;

	if ( !document_head )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: document_head is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "<head>\n" );

	if ( document_head->meta_string )
	{
		ptr += sprintf( ptr, "%s\n", document_head->meta_string );
	}

	if ( document_head->stylesheet_string )
	{
		ptr += sprintf( ptr, "%s\n", document_head->stylesheet_string );
	}

	if ( document_head->title_tag )
	{
		ptr += sprintf( ptr, "%s\n", document_head->title_tag );
	}

	if ( document_head->menu_setup_string )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			document_head->menu_setup_string );
	}

	if ( document_head->calendar_setup_string )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			document_head->calendar_setup_string );
	}

	if ( document_head->javascript_include_string )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			document_head->javascript_include_string );
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
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/jscal2.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/border-radius.css>\n"
"<link rel=stylesheet type=text/css href=/appaserver/zscal2/src/css/gold/gold.css>\n"
"<script type=text/javascript src=/appaserver/zscal2/src/js/jscal2.js></script>\n"
"<script type=text/javascript src=/appaserver/zscal2/src/js/lang/en.js></script>";
	}
}

DOCUMENT *document_choose_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_isa_folder_name,
			MENU *menu,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *action_string )
{
	DOCUMENT *document = document_calloc();

	document->document_head =
		document_head_new(
			title,
			document_head_menu_setup_string( 1 /* menu_boolean */ ),
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
			no_initial_capital,
			action_string );

	return document;
}

DOCUMENT_BODY *document_body_choose_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_isa_folder_name,
			MENU *menu,
			boolean menu_boolean,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *action_string )
{
	DOCUMENT_BODY *document_body;

	document_body =
		document_body_new(
			menu,
			menu_boolean,
			title,
			(char *)0 /* javascript_replace */ );

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
			no_initial_capital,
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

void document_close( void )
{
	printf( "%s\n",
		document_close_html() );
}

DOCUMENT_EDIT_TABLE *document_edit_table_calloc( void )
{
	DOCUMENT_EDIT_TABLE *document_edit_table;

	if ( ! ( document_edit_table =
			calloc( 1, sizeof( DOCUMENT_EDIT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return document_edit_table;
}

DOCUMENT_EDIT_TABLE *document_edit_table_new(
			char *edit_table_title,
			char *edit_table_message,
			char *folder_name,
			MENU *menu,
			LIST *folder_attribute_append_isa_list,
			LIST *dictionary_list,
			char *edit_table_submit_action_string,
			LIST *edit_table_heading_list,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drillthru_dictionary )
{
	DOCUMENT_EDIT_TABLE *document_edit_table =
		document_edit_table_calloc();

	document_edit_table->document_head =
		document_head_new(
			edit_table_title,
			document_head_menu_setup_string(
				(menu) ? 1 : 0 /* menu_boolean */ ),
			document_head_calendar_setup_string(
				list_length(
					folder_attribute_date_name_list(
					 folder_attribute_append_isa_list ) ) ),
			document_head_javascript_include_string() );


	document_edit_table->document_body_edit_table =
		document_body_edit_table_new(

	return document_edit_table;

