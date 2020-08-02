/* ---------------------------------------------------------------	*/
/* src_appaserver/post_generic_load.c					*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "column.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "attribute.h"
#include "environ.h"
#include "application.h"
#include "boolean.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "post2dictionary.h"
#include "appaserver_user.h"
#include "dictionary.h"
#include "related_folder.h"
#include "session.h"
#include "role.h"
#include "date.h"
#include "generic_load.h"

/* Constants */
/* --------- */
#define POSITION_PREFIX		"position_"
#define CONSTANT_PREFIX		"constant_"

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void post_state_one(		char *application_name,
				char *session,
				char *role_name,
				char *folder_name,
				LIST *attribute_list,
				char *login_name );

void post_state_two(		char *application_name,
				char *session,
				char *role_name,
				char *folder_name,
				char *appaserver_data_directory,
				char *login_name );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name, *state;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	LIST *attribute_list;
	char buffer[ 256 ];
	char *database_string = {0};
	DOCUMENT *document;
	int with_dynarch_menu;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application session role folder state\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	state = argv[ 5 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );

	login_name = session_get_login_name(
				application_name,
				session );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !login_name || !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				"insert" ) )
	{
		session_access_failed_message_and_exit(
					application_name, session, login_name );
	}

	if ( !appaserver_user_exists_role(
					application_name,
					login_name,
					role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new(
				application_get_title_string(
					application_name ),
				application_name );

	document->output_content_type = 1;

	with_dynarch_menu =
		appaserver_frameset_menu_horizontal(
					application_name,
					login_name );

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "validate_date" );
	document_set_javascript_module( document, "post_generic_load" );

	if ( strcmp( state, "one" ) == 0 && with_dynarch_menu )
	{
		char sys_string[ 1024 ];

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			with_dynarch_menu );

		document_output_dynarch_html_body(
				DOCUMENT_DYNARCH_MENU_ONLOAD_CONTROL_STRING,
				document->onload_control_string );

		printf( "<ul id=menu>\n" );

		sprintf(sys_string,
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
		with_dynarch_menu = 0;

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			with_dynarch_menu );

		document_output_body(
			document->application_name,
			document->onload_control_string );
	}

	sprintf( title,
		 "%s Load %s",
		 application_get_title_string( application_name ),
		 format_initial_capital( buffer,
					 folder_name ) );

	printf( "<h1>%s</h1>\n", title );

	attribute_list =
	attribute_get_attribute_list(
		application_name,
		folder_name,
		(char *)0 /* attribute_name */,
		(LIST *)0 /* mto1_isa_related_folder_list */,
		role_name );

	if ( !list_rewind( attribute_list ) )
	{
		printf( 
	"<h4> Error: there are no attributes assigned folder = %s.</h4>\n",
			folder_name );
		document_close();
		exit( 1 );
	}

	fflush( stdout );

	if ( strcmp( state, "one" ) == 0 )
	{
		post_state_one(	application_name,
				session,
				role_name,
				folder_name,
				attribute_list,
				login_name );
	}
	else
	if ( strcmp( state, "two" ) == 0 )
	{
		post_state_two(	application_name,
				session,
				role_name,
				folder_name,
				appaserver_parameter_file->
					appaserver_data_directory,
				login_name );
	}

	document_close();
	return 0;

} /* main() */

void post_state_one(	char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			LIST *attribute_list,
			char *login_name )
{
	LIST *mto1_related_folder_list;
	RELATED_FOLDER *related_folder;
	char buffer[ 256 ];
	ATTRIBUTE *attribute;
	int default_position = 1;
	int primary_key_default_position;
	APPASERVER_ELEMENT *element;
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
		" action=\"%s/post_generic_load?%s+%s+%s+%s+two\""
		" target=%s>\n",
		appaserver_parameter_file_get_cgi_directory(),
		application_name,
		session,
		role_name,
		folder_name,
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
		SKIP_HEADER_ROWS );

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

		element = (APPASERVER_ELEMENT *)0;

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

			related_folder->
			folder->
			primary_attribute_name_list =
				folder_get_primary_attribute_name_list(
					related_folder->
						folder->
						attribute_list );

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

		if ( attribute->primary_key_index )
		{
			printf( "*" );
			primary_key_default_position = default_position;
		}
		else
		{
			primary_key_default_position = 0;
		}

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
	"<td><input name=%s%s type=text size=3 value=%d\n"
	" onChange=\"after_field('%s%s','%s%s',%d)\"></td>\n",
			POSITION_PREFIX,
			attribute->attribute_name,
			default_position,
			POSITION_PREFIX,
			attribute->attribute_name,
			CONSTANT_PREFIX,
			attribute->attribute_name,
			primary_key_default_position );

		/* If drop-down element */
		/* -------------------- */
		if ( element )
		{
			sprintf(post_change_javascript,
				"after_field('%s%s','%s%s',%d)",
				POSITION_PREFIX,
				attribute->attribute_name,
				CONSTANT_PREFIX,
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
		/* ------------------ */
		/* Else field element */
		/* ------------------ */
		{
			printf(
	"<td><input name=%s%s type=text size=20 maxlength=%d"
	" onChange=\"after_field('%s%s','%s%s',%d)",
				CONSTANT_PREFIX,
				attribute->attribute_name,
				attribute->width,
				POSITION_PREFIX,
				attribute->attribute_name,
				CONSTANT_PREFIX,
				attribute->attribute_name,
				primary_key_default_position );

			if ( strcmp( attribute->datatype, "time" ) == 0)
				printf( " && validate_time_insert(this)" );
			printf( "\"></td>\n" );
		}

		if ( !attribute->primary_key_index )
		{
			printf(
	"<td><input name=ignore_%s type=checkbox value=yes"
	" onChange=\"after_ignore(this, '%s%s','%s%s')\">"
	"Ignore</td>\n",
				attribute->attribute_name,
				POSITION_PREFIX,
				attribute->attribute_name,
				CONSTANT_PREFIX,
				attribute->attribute_name );
		}

		default_position++;

	} while( list_next( attribute_list ) );

	printf( "</table>\n" );
	printf( "</form>\n" );

} /* post_state_one() */

void post_state_two(	char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			char *appaserver_data_directory,
			char *login_name )
{
	DICTIONARY *post_dictionary;
	DICTIONARY *position_dictionary;
	DICTIONARY *constant_dictionary;
	GENERIC_LOAD *generic_load;
	char *load_filename;
	char delimiter = {0};
	char *really_yn;
	char *skip_header_rows;
	int skip_header_rows_integer = 0;
	FILE *input_file;
	boolean with_sort_unique;

	post_dictionary =
		post2dictionary(stdin,
				appaserver_data_directory,
				session );

	really_yn =
		dictionary_get_pointer(
			post_dictionary,
			"really_yn" );

	load_filename =
		dictionary_get_pointer(
			post_dictionary,
			"load_filename" );

	if ( !load_filename
	||   !( input_file = fopen( load_filename, "r" ) ) )
	{
		printf( "<p>ERROR: please transmit a file.\n" );
		document_close();
		exit( 0 );
	}
	fclose( input_file );

	if ( ( skip_header_rows =
		dictionary_get_pointer(
			post_dictionary,
			SKIP_HEADER_ROWS ) ) )
	{
		skip_header_rows_integer = atoi( skip_header_rows );
	}

	generic_load = generic_load_new();

	generic_load->folder =
		generic_load_get_database_folder(
			application_name,
			session,
			folder_name,
			role_name,
			1 /* with_mto1_related_folders */ );

	position_dictionary =
		dictionary_get_without_prefix(
			post_dictionary,
			POSITION_PREFIX );

	constant_dictionary =
		dictionary_get_without_prefix(
			post_dictionary,
			CONSTANT_PREFIX );

	if ( ! ( generic_load->generic_load_folder_list =
			generic_load_get_folder_list(
				generic_load->folder,
				position_dictionary,
				constant_dictionary,
				login_name ) )
	||   !list_length( generic_load->generic_load_folder_list ) )
	{
		printf( "<h4>ERROR: Invalid input.</h4>\n" );
		document_close();
		exit( 0 );
	}

	generic_load_attribute_set_is_participating(
		generic_load->generic_load_folder_list,
		&delimiter,
		application_name,
		load_filename,
		skip_header_rows_integer );

	if ( *really_yn != 'y' )
	{
		GENERIC_LOAD_FOLDER *generic_load_folder;
		int input_record_count = 0;

		list_rewind( generic_load->generic_load_folder_list );

		do {
			generic_load_folder =
				list_get_pointer(
					generic_load->
						generic_load_folder_list );

			if ( !generic_load_has_participating(
				generic_load_folder->
					generic_load_attribute_list ) )
			{
				continue;
			}

			with_sort_unique =
				!list_at_head(
					generic_load->
						generic_load_folder_list );

			input_record_count =
				generic_load_output_test_only_report(
					load_filename,
					delimiter,
					generic_load_folder->
						generic_load_attribute_list,
					generic_load_folder->
						primary_key_date_offset,
					generic_load_folder->
						primary_key_time_offset,
					application_name,
					list_at_head(
						generic_load->
						generic_load_folder_list )
					/* display_errors */,
					generic_load_folder->folder_name,
					skip_header_rows_integer,
					with_sort_unique );

		} while( list_next( generic_load->
					generic_load_folder_list ) );

		printf( "<p>Test complete -- Input count: %d\n",
			input_record_count );
	}
	else
	{
		generic_load_output_to_database(
				application_name,
				load_filename,
				delimiter,
				generic_load->generic_load_folder_list,
				skip_header_rows_integer );
	}

} /* post_state_two() */
