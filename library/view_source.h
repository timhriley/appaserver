/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/view_source.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef VIEW_SOURCE_H
#define VIEW_SOURCE_H

#include "list.h"
#include "boolean.h"
#include "folder_menu.h"
#include "menu.h"
#include "widget.h"
#include "document.h"

enum view_source_enum {
	view_source_code,
	view_source_diagrams };

typedef struct
{
	char *relative_directory_name;
	char *absolute_name;
	LIST *filename_list;
	LIST *widget_container_list;
} VIEW_SOURCE_DIRECTORY;

/* Usage */
/* ----- */
VIEW_SOURCE_DIRECTORY *view_source_directory_new(
		char *appaserver_parameter_mount_point,
		enum view_source_enum view_source_enum,
		char *relative_directory_name );

/* Process */
/* ------- */
VIEW_SOURCE_DIRECTORY *view_source_directory_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *view_source_directory_absolute_name(
		char *mount_point,
		char *relative_directory_name );

LIST *view_source_filename_list(
		char *view_source_ls_system_string );

/* Usage */
/* ----- */

/* Returns program memory or null */
/* ------------------------------ */
char *view_source_directory_ls_system_string(
		enum view_source_enum view_source_enum );

/* Usage */
/* ----- */
LIST *view_source_directory_widget_container_list(
		char *relative_directory_name,
		LIST *view_source_directory_filename_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *view_source_directory_heading_string(
		char *relative_directory_name );

/* Returns heap memory */
/* ------------------- */
char *view_source_directory_href_string(
		char *relative_directory_name,
		char *filename );

/* Usage */
/* ----- */
WIDGET_CONTAINER *view_source_directory_widget_container(
		char *filename,
		char *view_source_directory_href_string );

/* Usage */
/* ----- */
boolean view_source_directory_boolean(
		LIST *view_source_directory_list,
		const char *relative_directory_name );

typedef struct
{
	enum view_source_enum view_source_enum;
	char *document_process_title_string;
	char *document_body_html;
	LIST *widget_container_list;
	LIST *view_source_directory_list;
	LIST *application_relative_source_directory_list;
	char *widget_container_list_html;
	char *document_form_html;
} VIEW_SOURCE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
VIEW_SOURCE *view_source_new(
		char *application_name,
		char *process_name,
		char *mount_point,
		enum view_source_enum view_source_enum );

/* Process */
/* ------- */
VIEW_SOURCE *view_source_calloc(
		void );

#endif
