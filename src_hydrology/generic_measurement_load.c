/* --------------------------------------------------- 	*/
/* src_hydrology/generic_measurement_load.c    	 	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "document.h"
#include "attribute.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "folder.h"
#include "session.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define FOLDER_NAME			"measurement"
#define EXCLUDE_ATTRIBUTE_LIST		"last_validation_date,last_person_validating,last_validation_process,measurement_update_method,reason_value_missing,delta_time"

/* Prototypes */
/* ---------- */
void remove_exclude_attribute_list(
			LIST *attribute_list );

void generic_measurement_load(
			char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			LIST *attribute_list,
			char *database_string,
			char *login_name,
			char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *role_name;
	char *login_name;
	char *process_name;
	LIST *attribute_list;
	char process_title_initial_capital[ 256 ];
	char title[ 256 ];
	char *database_string = {0};

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s application session role process\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	login_name = session_get_login_name(
					application_name,
					session );

	format_initial_capital( process_title_initial_capital,
				process_name );

	document = document_new(process_title_initial_capital,
				application_name );

	if ( !appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		document_set_output_content_type( document );
	}

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "validate_date" );
	document_set_javascript_module( document, "post_generic_load" );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	sprintf( title,
		 "%s %s",
		 application_get_title_string( application_name ),
		 process_title_initial_capital );

	printf( "<h1>%s</h1>\n", title );

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			FOLDER_NAME,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	remove_exclude_attribute_list( attribute_list );

	if ( !list_rewind( attribute_list ) )
	{
		printf( 
	"<h4> Error: there are no attributes assigned folder = %s.</h4>\n",
			FOLDER_NAME );
		document_close();
		exit( 1 );
	}

	fflush( stdout );

	generic_measurement_load(
			application_name,
			session,
			role_name,
			FOLDER_NAME,
			attribute_list,
			database_string,
			login_name,
			process_name );

	document_close();
	exit( 0 );

} /* main() */

void generic_measurement_load(
			char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			LIST *attribute_list,
			char *database_string,
			char *login_name,
			char *process_name )
{
	LIST *mto1_related_folder_list;
	RELATED_FOLDER *related_folder;
	char buffer[ 256 ];
	ATTRIBUTE *attribute;
	int default_position = 1;
	int primary_key_default_position;
	ELEMENT *element;
	char post_change_javascript[ 128 ];

	mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			(char *)0 /* session */,
			folder_name,
			role_name,
			0 /* isa_flag */,
			related_folder_recursive_all,
			1 /* override_row_restrictions */,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	related_folder_mark_ignore_multi_attribute_primary_keys(
		mto1_related_folder_list );

	printf( "<form enctype=\"multipart/form-data\""
		" method=post"
		" action=\"%s/post_generic_measurement_load?%s+%s+%s+%s+%s\""
		" target=%s>\n",
		appaserver_parameter_file_get_cgi_directory(),
		application_name,
		session,
		role_name,
		folder_name,
		process_name,
		EDIT_FRAME );

	printf( "<input type=\"button\" value=\"%s\"			"
		"onclick=\"document.forms[0].submit();\">\n		"
		"<input type=\"reset\" value=\"Reset\">\n",
		SUBMIT_BUTTON_LABEL );

	printf( "<table cellspacing=0 cellpadding=0 border>\n" );

	printf(
"<tr><td>Filename<td><input name=load_filename type=file size=30 accept=\"*\" maxlength=100>\n" );

	printf(
"<tr><td>Skip Header Rows<td><input name=%s size=3><td>How many header rows to skip?\n",
		HYDROLOGY_LIBRARY_SKIP_HEADER_ROWS );

	printf(
"<tr><td>Execute yn<td><select name=really_yn>\n" );
	printf( "<option value=\"n\">No\n" );
	printf( "<option value=\"y\">Yes\n" );
	printf( "</select>\n" );

	printf( "</table>\n" );
	printf( "<br>\n" );
	printf( "<table cellspacing=0 cellpadding=0 border>\n" );

	printf("<tr><th>Attribute</th><th>Position</th><th>Constant</th>\n" );

	list_rewind( attribute_list );

	do {
		attribute = list_get_pointer( attribute_list );

		element = (ELEMENT *)0;

		if ( ( related_folder =
	       		related_folder_attribute_consumes_related_folder(
			      (LIST **)0 /* foreign_attribute_name_list */,
			      (LIST *)0 /* done_attribute_name_list */,
			      (LIST *)0 /* omit_update_attribute_name_list */,
			      mto1_related_folder_list,
			      attribute->attribute_name,
			      (LIST *)0 /* include_attribute_name_list */ ) ) )
		{
			char element_name[ 128 ];

			if ( list_length(
					related_folder->
					folder->
					primary_attribute_name_list ) == 1 )
			{
				sprintf(element_name,
					"constant_%s",
					attribute->attribute_name );

				element = element_new(
						drop_down,
						strdup( element_name ) );

				element->drop_down->option_data_list =
				   folder_get_primary_data_list(
					application_name,
					BOGUS_SESSION,
					related_folder->
						folder->
						folder_name,
					login_name,
					(DICTIONARY *)0
						/* parameter_dictionary */,
					(DICTIONARY *)0
						/* where_clause_dictionary */,
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
					related_folder->
						folder->
						populate_drop_down_process,
					related_folder->
						folder->attribute_list,
					(LIST *)0
					/* common_non_pr...bute_name_list */,
					related_folder->
						folder->
						row_level_non_owner_forbid,
					(LIST *)0
					/* preprompt_accou...e_name_list */,
					role_name,
					(char *)0 /* state */,
					(char *)0
					/* one2m_folder_name_for_processes */,
					(char *)0,
					/* appaserver_user_foreign_login_name */
					0 /* not include_root_folder */ );
			}
		}

		printf( "<tr><td>" );

		if ( attribute->primary_key_index ) printf( "*" );

		primary_key_default_position = default_position;

		printf( "%s",
			format_initial_capital( buffer,
						attribute->attribute_name ) );
		if ( attribute->hint_message
		&&   *attribute->hint_message )
		{
			printf( "(%s)", attribute->hint_message );
		}

		printf( "</td>\n" );
		printf(
	"<td><input name=position_%s type=text size=3 value=%d\n"
	" onChange=\"after_field('position_%s','constant_%s',%d)\"></td>\n",
			attribute->attribute_name,
			default_position,
			attribute->attribute_name,
			attribute->attribute_name,
			primary_key_default_position );

		/* If drop-down element */
		/* -------------------- */
		if ( element )
		{
			sprintf(post_change_javascript,
				"after_field('position_%s','constant_%s',%d)",
				attribute->attribute_name,
				attribute->attribute_name,
				primary_key_default_position );

			element_drop_down_output(
				stdout,
				element->name,
				element->drop_down->option_data_list,
				element->drop_down->option_label_list,
				element->drop_down->number_columns,
				element->drop_down->multi_select,
				-1 /* row */,
				element->drop_down->initial_data,
				element->drop_down->output_null_option,
				element->drop_down->output_not_null_option,
				element->drop_down->output_select_option,
				element->drop_down->folder_name,
				post_change_javascript,
				element->drop_down->max_drop_down_size,
				element->drop_down->multi_select_element_name,
				element->drop_down->onblur_javascript_function,
				(char *)0 /* background_color */,
				element->drop_down->date_piece_offset,
				element->drop_down->no_initial_capital,
				element->drop_down->readonly,
				0 /* tab_index */,
				element->drop_down->state,
				element->drop_down->attribute_width );
			fflush( stdout );
		}
		else
		{
			printf(
	"<td><input name=constant_%s type=text size=20 maxlength=%d"
	" onChange=\"after_field('position_%s','constant_%s',%d)",
				attribute->attribute_name,
				attribute->width,
				attribute->attribute_name,
				attribute->attribute_name,
				primary_key_default_position );

			if ( strcmp( attribute->datatype, "time" ) == 0)
				printf( " && validate_time_insert(this)" );
			printf( "\"></td>\n" );
		}

		default_position++;

	} while( list_next( attribute_list ) );

	printf( "</table>\n" );
	printf( "</form>\n" );

} /* generic_measurement_load() */

void remove_exclude_attribute_list(
			LIST *attribute_list )
{
	LIST *exclude_attribute_name_list;
	ATTRIBUTE *attribute;

	exclude_attribute_name_list =
		list_string2list(
			EXCLUDE_ATTRIBUTE_LIST,
			',' );

	if ( !list_rewind( attribute_list ) ) return;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( list_exists_string(
			exclude_attribute_name_list,
			attribute->attribute_name ) )
		{
			list_delete_current( attribute_list );
		}
	} while( list_next( attribute_list ) );

} /* remove_exclude_attribute_list() */
