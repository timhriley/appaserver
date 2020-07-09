/* $APPASERVER_HOME/library/document.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver document ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"

DOCUMENT *document_new( char *title, char *application_name )
{
	DOCUMENT *d = document_new_document( application_name );
	d->title = title;
	d->application_name = application_name;
	d->javascript_module_list = list_new();
	d->stylesheet_filename = "style.css";
	return d;
}

DOCUMENT *document_new_document( char *application_name )
{
	DOCUMENT *d = (DOCUMENT *)calloc( 1, sizeof( DOCUMENT ) );
	d->application_name = application_name;
	return d;
}

void document_output_body(	char *application_name,
				char *onload_control_string )
{
	return 
	document_output_body_stream(
				stdout,
				application_name,
				onload_control_string );
}

void document_output_body_stream(
				FILE *output_stream,
				char *application_name,
				char *onload_control_string )
{
	fprintf( output_stream, "<body" );

	fprintf( output_stream,
		 " bgcolor=%s",
		application_get_background_color( application_name ) );

	if ( onload_control_string
	&&   *onload_control_string )
	{
		int str_len;

		fprintf(	output_stream,
				" onload=\"%s",
				onload_control_string );

		str_len = strlen( onload_control_string );

		if ( *(onload_control_string + str_len) != ';' )
			fprintf( output_stream, ";" );

		fprintf(	output_stream,
				"\"" );
	}

	fprintf( output_stream, ">\n" );

} /* document_output_body_stream() */

void document_output_content_type( void )
{
	printf( "Content-type: text/html\n\n" );
	fflush( stdout );
}

void document_output_heading(	char *application_name,
				char *title,
				boolean output_content_type,
				char *appaserver_mount_point,
				LIST *javascript_module_list,
				char *stylesheet_filename,
				char *relative_source_directory,
				boolean with_dynarch_menu )
{
	document_output_head(	application_name,
				title,
				output_content_type,
				appaserver_mount_point,
				javascript_module_list,
				stylesheet_filename,
				relative_source_directory,
				with_dynarch_menu );
}

void document_output_head(	char *application_name,
				char *title,
				boolean output_content_type,
				char *appaserver_mount_point,
				LIST *javascript_module_list,
				char *stylesheet_filename,
				char *relative_source_directory,
				boolean with_dynarch_menu )
{
	return 
	document_output_head_stream(
				stdout,
				application_name,
				title,
				output_content_type,
				appaserver_mount_point,
				javascript_module_list,
				stylesheet_filename,
				relative_source_directory,
				with_dynarch_menu,
				1 /* with_close_head */ );
}

void document_output_dynarch_heading( FILE *output_stream )
{
		fprintf( output_stream,
"<link rel=stylesheet type=text/css href=\"/%s/src/style-%s.css\">\n"
"<link rel=stylesheet type=text/css href=\"/%s/src/skin-%s.css\">\n"
"<script type=\"text/javascript\"> _dynarch_menu_url=\"/%s/src/\"; </script>\n"
"<script type=\"text/javascript\"> _dynarch_top=\"/%s/\"; </script>\n"
"<script type=\"text/javascript\" src=\"/%s/src/hmenu.js\"> </script>\n",
			HORIZONTAL_MENU_RELATIVE_DIRECTORY,
			"template" /* application_name */,
			HORIZONTAL_MENU_RELATIVE_DIRECTORY,
			"template" /* application_name */,
			HORIZONTAL_MENU_RELATIVE_DIRECTORY,
			HORIZONTAL_MENU_RELATIVE_DIRECTORY,
			HORIZONTAL_MENU_RELATIVE_DIRECTORY );
}

void document_output_html_stream( FILE *output_stream )
{
	fprintf( output_stream,
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\">\n" );

	fprintf( output_stream,
"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n" );

	fflush( output_stream );

} /* document_output_html_stream() */

void document_output_head_stream(
				FILE *output_stream,
				char *application_name,
				char *title,
				boolean output_content_type,
				char *appaserver_mount_point,
				LIST *javascript_module_list,
				char *stylesheet_filename,
				char *relative_source_directory,
				boolean with_dynarch_menu,
				boolean with_close_head )
{
	if ( output_content_type ) document_output_content_type();

	if ( !title || !*title ) title = application_name;

	document_output_html_stream( output_stream );

	fprintf( output_stream,
"<head>\n\n" );

	fprintf( output_stream,
"<meta name=\"generator\" content=\"Appaserver: Open Source Application Server\" />\n\n" );

	fprintf( output_stream,
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" >\n" );

	if ( with_dynarch_menu )
	{
		document_output_dynarch_heading( output_stream );
	}
	else
	{
		fprintf( output_stream,
"<link rel=stylesheet type=\"text/css\" href=\"/appaserver/%s/%s\">\n",
		application_name,
		stylesheet_filename );
	}

	fprintf( output_stream,
		 "<TITLE>%s</TITLE>\n",
		 title );

	if ( appaserver_mount_point
	&&   list_length( javascript_module_list ) )
	{
		document_output_each_javascript_source(
				application_name,
				javascript_module_list,
				appaserver_mount_point,
				relative_source_directory );
	}

	/* This is necessary because a fork to "cat" might happen. */
	/* ------------------------------------------------------- */
	fflush( output_stream );

	if ( with_close_head )  document_output_close_head( output_stream );

} /* document_output_head_stream() */

void document_output_close_head( FILE *output_stream )
{
	fprintf( output_stream, "</head>\n" );
}

void document_output_each_javascript_source(
			char *application_name,
			LIST *javascript_module_list,
			char *appaserver_mount_point,
			char *relative_source_directory )
{
	DOCUMENT_JAVASCRIPT_MODULE *javascript_module;

	if ( !list_rewind( javascript_module_list ) ) return;

	do {
		javascript_module =
			list_get_pointer(
				javascript_module_list );

		document_output_javascript_source(
					application_name,
					javascript_module->javascript_filename,
					appaserver_mount_point,
					relative_source_directory );

	} while( list_next( javascript_module_list ) );

} /* document_output_each_javascript_source() */

void document_output_javascript_source(
				char *application_name,
				char *javascript_filename,
				char *appaserver_mount_point,
				char *relative_source_directory )
{
	char source_filename[ 256 ];
	char source_directory[ 128 ];
	char relative_source_directory_javascript[ 512 ];
	char source_directory_filename[ 512 ];
	int index;

	if ( relative_source_directory && *relative_source_directory )
	{
		sprintf( relative_source_directory_javascript,
			 "%s%cjavascript",
			 relative_source_directory,
			 PATH_DELIMITER );
	}
	else
	{
		strcpy( relative_source_directory_javascript, "javascript" );
	}

	strcpy( source_filename, javascript_filename );

	if ( instr( ".js", source_filename, 1 ) == -1 )
		strcat( source_filename, ".js" );

	for(	index = 0;
		piece(	source_directory,
			PATH_DELIMITER,
			relative_source_directory_javascript,
			index );
		index++ )
	{
		sprintf(source_directory_filename, 
		 	"%s/%s/%s",
		 	appaserver_mount_point,
		 	source_directory,
		 	source_filename );

		if ( timlib_file_exists( source_directory_filename ) ) break;
	}

	if ( !*source_directory )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot find javascript filename = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 source_filename );
		exit( 1 );
	}

	if ( application_constants_cat_javascript_source( application_name ) )
	{
		char buffer[ 1024 ];

		printf( "<SCRIPT language=\"JavaScript1.2\">\n" );
		sprintf( buffer, 
		 	"cat %s/%s/%s",
		 	appaserver_mount_point,
		 	source_directory,
		 	source_filename );
		fflush( stdout );
		system( buffer );
		fflush( stdout );
		printf( "</SCRIPT>\n" );
	}
	else
	{
		printf( 
"<SCRIPT language=\"JavaScript1.2\" src=\"/appaserver/%s/%s\"></SCRIPT>\n",
			source_directory,
			source_filename );
	}

} /* document_output_javascript_source() */

void document_close( void )
{
	return document_close_stream( stdout );
}

void document_close_stream( FILE *output_stream )
{
	document_close_body_stream( output_stream );
	document_close_html_stream( output_stream );
}

void document_output_closing( void )
{
	document_close_body();
	document_close_html();
}

void document_close_body( void )
{
	return document_close_body_stream( stdout );
}

void document_close_body_stream( FILE *output_stream )
{
	fprintf( output_stream, "</body>\n" );
}

void document_close_html( void )
{
	return document_close_html_stream( stdout );
}

void document_close_html_stream( FILE *output_stream )
{
	fprintf( output_stream, "</html>\n" );
}

void document_set_output_content_type( DOCUMENT *d )
{
	d->output_content_type = 1;
}

void document_set_javascript_module(
			DOCUMENT *d, 
			char *javascript_filename )
{
	DOCUMENT_JAVASCRIPT_MODULE *javascript_module;

	javascript_module =
		document_javascript_module_new(
			d->javascript_module_list,
			javascript_filename );

	list_append_pointer(
		d->javascript_module_list, javascript_module );
} /* document_set_javascript_module() */

char *document_set_onload_control_string(
					char *document_onload_control_string,
					char *s )
{
	if ( !s ) return document_onload_control_string;

	if ( document_onload_control_string )
	{
		char buffer[ 4096 ];

		sprintf( buffer,
			 "%s;%s",
			 document_onload_control_string,
			 s );

		 document_onload_control_string = strdup( buffer );
	}
	else
	{
		document_onload_control_string = strdup( s );
	}

	return document_onload_control_string;

} /* document_set_onload_control_string() */

void document_output_quick_body(	char *application_name,
					char *appaserver_mount_point )
{
	document_quick_output_body(	application_name,
					appaserver_mount_point );
}

void document_quick_output_head(	char *application_name,
					char *appaserver_mount_point )
{
	DOCUMENT *document;

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		(char *)0 /* relative_source_directory */,
		0 /* not with_dynarch_menu */ );
}

void document_quick_output_body(	char *application_name,
					char *appaserver_mount_point )
{
	DOCUMENT *document;

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		(char *)0 /* relative_source_directory */,
		0 /* not with_dynarch_menu */ );

	document_output_body(
		document->application_name,
		document->onload_control_string );

} /* document_quick_output_body() */

void document_set_folder_javascript_files(
					DOCUMENT *document,
					char *application_name,
					char *folder_name )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	LIST *filename_list;
	char *javascript_folders_table_name;
	char *javascript_files_table_name;
	char *filename;
	char select[ 512 ];

	javascript_folders_table_name =
		get_table_name(
			application_name,
			"javascript_folders" );

	javascript_files_table_name =
		get_table_name(
			application_name,
			"javascript_files" );

	sprintf(select,
		"%s.javascript_filename",
		javascript_files_table_name );

	sprintf( where,
		 "%s.folder = '%s' and 				 "
		 "%s.javascript_filename = %s.javascript_filename",
		 javascript_folders_table_name,
		 folder_name,
		 javascript_folders_table_name,
		 javascript_files_table_name );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\"		|"
		 "sql.e '^'						 ",
		 select,
		 javascript_files_table_name,
		 javascript_folders_table_name,
		 where );

	filename_list = pipe2list( sys_string );

	if ( list_rewind( filename_list ) )
	{
		do {
			filename = list_get_pointer( filename_list );
			document_set_javascript_module(
				document,
				filename );
		} while( list_next( filename_list ) );
	}
} /* document_set_folder_javascript_files() */

void document_set_process_javascript_files(
					DOCUMENT *document,
					char *application_name,
					char *process_name )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	LIST *filename_list;
	char *javascript_processes_table_name;
	char *javascript_process_sets_table_name;
	char *javascript_files_table_name;
	char *filename;
	char select[ 512 ];

	javascript_files_table_name =
		get_table_name(
			application_name,
			"javascript_files" );

	javascript_processes_table_name =
		get_table_name(
			application_name,
			"javascript_processes" );

	sprintf(select,
		"%s.javascript_filename",
		javascript_files_table_name );

	sprintf( where,
		 "%s.process = '%s' and 			 "
		 "%s.javascript_filename = %s.javascript_filename",
		 javascript_processes_table_name,
		 process_name,
		 javascript_processes_table_name,
		 javascript_files_table_name );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\"		|"
		 "sql.e '^'						 ",
		 select,
		 javascript_files_table_name,
		 javascript_processes_table_name,
		 where );

	filename_list = pipe2list( sys_string );

	if ( list_rewind( filename_list ) )
	{
		do {
			filename = list_get_pointer( filename_list );
			document_set_javascript_module(
				document,
				filename );
		} while( list_next( filename_list ) );
	}
	else
	{
		javascript_process_sets_table_name =
			get_table_name(
				application_name,
				"javascript_process_sets" );
	
		sprintf(select,
			"%s.javascript_filename",
			javascript_files_table_name );
	
		sprintf( where,
			 "%s.process_set = '%s' and 			 "
			 "%s.javascript_filename = %s.javascript_filename",
			 javascript_process_sets_table_name,
			 process_name,
			 javascript_process_sets_table_name,
			 javascript_files_table_name );
	
		sprintf( sys_string,
			 "echo \"select %s from %s,%s where %s;\"	|"
			 "sql.e '^'					 ",
			 select,
			 javascript_files_table_name,
			 javascript_process_sets_table_name,
			 where );
	
		filename_list = pipe2list( sys_string );
	
		if ( !list_rewind( filename_list ) ) return;
		do {
			filename = list_get_pointer( filename_list );
			document_set_javascript_module(
				document,
				filename );
		} while( list_next( filename_list ) );
	}
} /* document_set_process_javascript_files() */

DOCUMENT_JAVASCRIPT_MODULE *document_javascript_module_new(
					LIST *javascript_module_list,
					char *javascript_filename )
{
	DOCUMENT_JAVASCRIPT_MODULE *javascript_module;

	if ( list_rewind( javascript_module_list ) )
	{
		do {
			javascript_module =
				list_get_pointer(
					javascript_module_list );
			if ( strcmp(	javascript_module->javascript_filename,
					javascript_filename ) == 0 )
			{
				return javascript_module;
			}
		} while( list_next( javascript_module_list ) );
	}

	if ( ! (	javascript_module =
			calloc( 1, sizeof( DOCUMENT_JAVASCRIPT_MODULE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	javascript_module->javascript_filename = javascript_filename;
	return javascript_module;
} /* document_javascript_module_new() */

void document_output_dynarch_non_frame_html_head_body(
				char *application_name,
				boolean content_type,
				char *dynarch_menu_onload_control_string,
				char *additional_control_string )
{
	document_output_head(	application_name,
				(char *)0 /* title */,
				content_type,
				(char *)0 /* appaserver_mount_point */,
				(LIST *)0 /* javascript_module_list */,
				(char *)0 /* stylesheet_filename */,
				(char *)0 /* relative_source_directory */,
				1 /* with_dynarch_menu */ );

	document_output_dynarch_html_body(
				dynarch_menu_onload_control_string,
				additional_control_string );

} /* document_output_dynarch_non_frame_html_head_body() */


void document_output_dynarch_html_body(
				char *dynarch_menu_onload_control_string,
				char *additional_control_string )
{

/*
		printf(
"<body onload=\"%s%c%s%c\"\n"
"leftmargin=0 topmargin=0 marginwidth=0 marginheight=0\n"
">\n",
			dynarch_menu_onload_control_string,
			(additional_control_string) ? ';' : ' ',
			(additional_control_string)
				? additional_control_string
				: "",
			(additional_control_string) ? ';' : ' ' );
*/

		printf(
"<body onload=\"%s%c%s\"\n"
"leftmargin=0 topmargin=0 marginwidth=0 marginheight=0\n"
">\n",
			dynarch_menu_onload_control_string,
			(additional_control_string) ? ';' : ' ',
			(additional_control_string)
				? additional_control_string
				: "" );

	document_output_dynarch_hide_ul_contents();

} /* document_output_dynarch_html_body() */

void document_output_dynarch_hide_ul_contents( void )
{
	printf(
"<!-- following there's an workaround to hide the UL contents while the page is loading ;-) -->\n"
"<script type=\"text/javascript\">//<![CDATA[ \ndocument.writeln(\"<style type='text/css'>#menu { display: none; }</style>\");\n"
"     //]]></script>\n" );

} /* document_output_dynarch_hide_ul_contents() */

DOCUMENT *document_output_menu_new(
			char *application_name,
			char *database_string,
			char *login_name,
			char *session,
			char *role_name,
			char *appaserver_mount_point,
			char *onload_control_string,
			boolean exists_date_attribute )
{
	DOCUMENT *document;
	boolean with_dynarch_menu;

	document = document_new(
			application_get_title_string(
				application_name ),
			application_name );

	document->output_content_type = 1;
	document->onload_control_string = onload_control_string;

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );
	document_set_javascript_module( document, "post_change_multi_select" );

	with_dynarch_menu =
		appaserver_frameset_menu_horizontal(
					application_name,
					login_name );

	if ( with_dynarch_menu )
	{
		char sys_string[ 1024 ];

		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				with_dynarch_menu );

		if ( exists_date_attribute )
		{
			appaserver_library_output_calendar_javascript();
		}

		document_output_dynarch_html_body(
				DOCUMENT_DYNARCH_MENU_ONLOAD_CONTROL_STRING,
				document->onload_control_string );

		printf( "<ul id=menu>\n" );

		sprintf(	sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' %c %c 2>>%s",
				application_name,
				session,
				login_name,
				role_name,
				"" /* title */,
				'n' /* not content_type_yn */,
				'y' /* omit_html_head_yn */,
				appaserver_error_get_filename(
					application_name ) );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );
		printf( "</ul>\n" );
	}
	else
	{
		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				with_dynarch_menu );

		if ( exists_date_attribute )
		{
			appaserver_library_output_calendar_javascript();
		}

		document_output_body(
				document->application_name,
				document->onload_control_string );
	}

	return document;

} /* document_output_menu_new() */

