/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_map_operation.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef GOOGLE_MAP_OPERATION_H
#define GOOGLE_MAP_OPERATION_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "operation.h"
#include "folder_menu.h"
#include "menu.h"
#include "document.h"
#include "google.h"
#include "google_map.h"

typedef struct
{
	OPERATION_SEMAPHORE *operation_semaphore;
	GOOGLE_FILENAME *google_filename;
	FILE *output_file;
	char *google_map_key;
	char *title_string;
	char *google_map_canvas_style_html;
	char *google_map_loader_script_html;
	char *google_map_script_open_tag;
	char *google_map_create_marker_javascript;
	char *google_map_initialize_javascript;
	char *document_open_html;
	char *google_map_click_message;
	GOOGLE_MAP_POINT *google_map_point;
	char *google_map_body_html;
	boolean application_menu_horizontal_boolean;
	boolean menu_horizontal_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *document_head_menu_setup_string;
	DOCUMENT *document;
	char *document_form_html;
} GOOGLE_MAP_OPERATION;

/* Usage */
/* ----- */
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
		char *data_directory );

/* Process */
/* ------- */
GOOGLE_MAP_OPERATION *google_map_operation_calloc(
		void );

/* Safely returns */
/* -------------- */
FILE *google_map_operation_output_file(
		boolean group_first_time,
		char *http_output_filename );

/* Returns static memory */
/* --------------------- */
char *google_map_operation_title_string(
		char *operation_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_operation_document_open_html(
		char *application_name,
		char *google_map_operation_title_string,
		char *google_map_canvas_style_html,
		char *google_map_loader_script_html,
		char *google_map_script_open_tag,
		char *google_map_create_marker_javascript,
		char *google_map_initialize_javascript );

#endif
