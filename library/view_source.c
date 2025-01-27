/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/view_source.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "application.h"
#include "widget.h"
#include "frameset.h"
#include "button.h"
#include "view_source.h"

VIEW_SOURCE *view_source_new(
		char *application_name,
		char *process_name,
		char *mount_point,
		enum view_source_enum view_source_enum )
{
	VIEW_SOURCE *view_source;
	char *relative_source_directory_name;
	VIEW_SOURCE_DIRECTORY *view_source_directory;
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !process_name
	||   !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	view_source = view_source_calloc();

	view_source->document_process_title_string =
		/* --------------------------- */
		/* Returns static memory or "" */
		/* --------------------------- */
		document_process_title_string(
			process_name );

	view_source->document_body_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_html(
			(char *)0 /* menu_html */,
			view_source->document_process_title_string,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			document_process_sub_title_string(),
			(char *)0 /* sub_sub_title_string */,
			(char *)0 /* body_onload_open_tag */,
			(char *)0 /* document_body_hide_preload_html */,
			(char *)0 /* notepad_html */ );

	view_source->widget_container_list = list_new();
	view_source->view_source_directory_list = list_new();

	view_source->application_relative_source_directory_list =
		application_relative_source_directory_list(
			application_name );

	if ( list_rewind(
			view_source->
				application_relative_source_directory_list ) )
	do {
		relative_source_directory_name =
			list_get(
				view_source->
				   application_relative_source_directory_list );

		view_source_directory =
			view_source_directory_new(
				mount_point,
				view_source_enum,
				relative_source_directory_name );

		if ( view_source_directory )
		{
			list_set(
				view_source->view_source_directory_list,
				view_source_directory );

			list_set_list(
				view_source->widget_container_list,
				view_source_directory->widget_container_list );
		}

	} while ( list_next( 
			view_source->
				application_relative_source_directory_list ) );

	if ( !view_source_directory_boolean(
		view_source->view_source_directory_list,
		"src_appaserver" ) )
	{
		view_source_directory =
			view_source_directory_new(
				mount_point,
				view_source_diagrams,
				"src_appaserver" );

		if ( view_source_directory )
		{
			list_set(
				view_source->view_source_directory_list,
				view_source_directory );

			list_set_list(
				view_source->widget_container_list,
				view_source_directory->widget_container_list );
		}
	}

	if ( !view_source_directory_boolean(
		view_source->view_source_directory_list,
		"template" ) )
	{
		view_source_directory =
			view_source_directory_new(
				mount_point,
				view_source_enum,
				"template" );

		if ( view_source_directory )
		{
			list_set(
				view_source->view_source_directory_list,
				view_source_directory );

			list_set_list(
				view_source->widget_container_list,
				view_source_directory->widget_container_list );
		}
	}

	if ( !view_source_directory_boolean(
		view_source->view_source_directory_list,
		"utility" ) )
	{
		view_source_directory =
			view_source_directory_new(
				mount_point,
				view_source_enum,
				"utility" );

		if ( view_source_directory )
		{
			list_set(
				view_source->view_source_directory_list,
				view_source_directory );

			list_set_list(
				view_source->widget_container_list,
				view_source_directory->widget_container_list );
		}
	}

	if ( !view_source_directory_boolean(
		view_source->view_source_directory_list,
		"library" ) )
	{
		view_source_directory =
			view_source_directory_new(
				mount_point,
				view_source_enum,
				"library" );

		if ( view_source_directory )
		{
			list_set(
				view_source->view_source_directory_list,
				view_source_directory );

			list_set_list(
				view_source->widget_container_list,
				view_source_directory->widget_container_list );
		}
	}

	list_set(
		view_source->widget_container_list,
		(widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button =
		button_to_top();

	view_source->widget_container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			view_source->widget_container_list );

	if ( !view_source->widget_container_list_html )
	{
		char message[ 128 ];

		sprintf(message,
			"widget_container_list_html() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	view_source->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			(char *)0 /* document_html */,
			(char *)0 /* document_head_html */,
			(char *)0 /* document_head_close_tag */,
			view_source->document_body_html,
			view_source->widget_container_list_html
				/* form_html */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	return view_source;
}

VIEW_SOURCE *view_source_calloc( void )
{
	VIEW_SOURCE *view_source;

	if ( ! ( view_source = calloc( 1, sizeof ( VIEW_SOURCE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return view_source;
}

char *view_source_directory_absolute_name(
		char *mount_point,
		char *relative_directory_name )
{
	static char absolute_name[ 128 ];

	if ( !mount_point
	||   !relative_directory_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(absolute_name,
		"%s/%s",
		mount_point,
		relative_directory_name );

	return absolute_name;
}

LIST *view_source_directory_widget_container_list(
		char *relative_directory_name,
		LIST *filename_list )
{
	LIST *widget_container_list;
	char *heading_string;
	char *filename;
	char *href_string;

	if ( !relative_directory_name
	||   !list_rewind( filename_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container_list = list_new();

	heading_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		view_source_directory_heading_string(
			relative_directory_name );

	list_set(
		widget_container_list,
		widget_container_new(
			non_edit_text,
			heading_string ) );

	list_set(
		widget_container_list,
		widget_container_new(
			unordered_list_open, (char *)0 ) );

	do {
		filename =
			list_get(
				filename_list );

		list_set(
			widget_container_list,
			widget_container_new(
				list_item, (char *)0 ) );

		href_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			view_source_directory_href_string(
				relative_directory_name,
				filename );

		list_set(
			widget_container_list,
			view_source_directory_widget_container(
				filename,
				href_string ) );

	} while ( list_next( filename_list ) );

	list_set(
		widget_container_list,
		widget_container_new(
			unordered_list_close, (char *)0 ) );

	return widget_container_list;
}

char *view_source_directory_href_string(
		char *relative_directory_name,
		char *filename )
{
	char href_string[ 256 ];

	if ( !relative_directory_name
	||   !filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(href_string,
		"/appaserver_home/%s/%s",
		relative_directory_name,
		filename );

	return strdup( href_string );
}

WIDGET_CONTAINER *view_source_directory_widget_container(
		char *filename,
		char *view_source_directory_href_string )
{
	WIDGET_CONTAINER *widget_container;

	if ( !filename
	||   !view_source_directory_href_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			anchor, (char *)0 );

	widget_container->anchor->hypertext_reference =
		view_source_directory_href_string;

	widget_container->anchor->target_frame =
		"_new";

	widget_container->anchor->prompt =
		filename;

	return widget_container;
}

char *view_source_directory_ls_system_string(
		enum view_source_enum view_source_enum )
{
	if ( view_source_enum == view_source_code )
	{
		return
"ls -1 makefile *.[ch] *.sh *.js *.awk 2>/dev/null | sort";
	}
	else
	if ( view_source_enum == view_source_diagrams )
	{
		return
"ls -1 *.xls *.jpg *.doc *.pdf *.png *.txt *.ppt *.xml 2>/dev/null | sort";
	}
	else
	{
		return (char *)0;
	}
}

LIST *view_source_directory_filename_list( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return list_pipe_fetch( system_string );
}

VIEW_SOURCE_DIRECTORY *view_source_directory_new(
		char *mount_point,
		enum view_source_enum view_source_enum,
		char *relative_directory_name )
{
	VIEW_SOURCE_DIRECTORY *view_source_directory;
	char *ls_system_string;

	if ( !mount_point
	||   !relative_directory_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	view_source_directory = view_source_directory_calloc();

	view_source_directory->relative_directory_name =
		relative_directory_name;

	view_source_directory->absolute_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		view_source_directory_absolute_name(
			mount_point,
			relative_directory_name );

	if ( chdir( view_source_directory->absolute_name ) != 0 )
	{
		char message[ 128 ];

		sprintf(message,
			"chdir(%s) failed to execute.",
			view_source_directory->absolute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( ls_system_string =
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			view_source_directory_ls_system_string(
				view_source_enum ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"view_source_directory_ls_sytem_string(%d) empty.",
			view_source_enum );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	view_source_directory->filename_list =
		view_source_directory_filename_list(
			ls_system_string );

	if ( !list_length( view_source_directory->filename_list ) )
	{
		return NULL;
	}

	view_source_directory->widget_container_list =
		view_source_directory_widget_container_list(
			relative_directory_name,
			view_source_directory->filename_list );

	return view_source_directory;
}

VIEW_SOURCE_DIRECTORY *view_source_directory_calloc( void )
{
	VIEW_SOURCE_DIRECTORY *view_source_directory;

	if ( ! ( view_source_directory =
			calloc( 1,
				sizeof( VIEW_SOURCE_DIRECTORY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return view_source_directory;
}

char *view_source_directory_heading_string( char *relative_directory_name )
{
	char heading_string[ 128 ];

	if ( !relative_directory_name )
	{
		char message[ 128 ];

		sprintf(message, "relative_directory_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(heading_string,
		"<h3>%s</h3>",
		relative_directory_name );

	return
	strdup(
		string_initial_capital(
			heading_string,
			heading_string ) );
}

boolean view_source_directory_boolean(
		LIST *view_source_directory_list,
		const char *relative_directory_name )
{
	VIEW_SOURCE_DIRECTORY *view_source_directory;

	if ( list_rewind( view_source_directory_list ) )
	do {
		view_source_directory =
			list_get(
				view_source_directory_list );

		if ( strcmp(
			relative_directory_name,
			view_source_directory->
				relative_directory_name ) == 0 )
		{
			return 1;
		}

	} while ( list_next( view_source_directory_list ) );

	return 0;
}
