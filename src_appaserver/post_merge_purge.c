/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_merge_purge.c			*/
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
#include "session.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "post2dictionary.h"
#include "appaserver_user.h"
#include "dictionary.h"
#include "related_folder.h"
#include "role.h"
#include "query.h"
#include "folder_menu.h"

/* Constants */
/* --------- */
#define DEBUG_MODE			0
#define TABLE_LINES_TO_QUEUE		50
#define KEEP_LABEL			"keep"
#define PURGE_LABEL			"purge"

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void output_delete_database(
				char *application_name,
				char *session,
				char *role_name,
				char *appaserver_data_directory,
				FOLDER *folder,
				char *purge_data );

void output_update_database(
				char *application_name,
				FOLDER *folder,
				char *keep_data_list_string,
				char *purge_data_list_string );

void output_test_only_related_folder(
				char *application_name,
				RELATED_FOLDER *related_folder,
				LIST *where_attribute_name_list,
				LIST *purge_data_list );

void output_test_only_report(
				char *application_name,
				FOLDER *folder,
				char *purge_data_list_string );

void post_state_one(		char *application_name,
				char *session,
				char *role_name,
				char *folder_name,
				char *login_name,
				char *process_name );

void post_state_two(		char *application_name,
				char *session,
				char *role_name,
				char *folder_name,
				char *appaserver_data_directory );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name, *state;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char buffer1[ 256 ];
	char buffer2[ 256 ];
	char *database_string = {0};
	DOCUMENT *document;
	int with_dynarch_menu;
	char *process_name;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application session role folder process state\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	process_name = argv[ 5 ];
	state = argv[ 6 ];

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

	if ( !DEBUG_MODE )
	{
		if ( session_remote_ip_address_changed(
			application_name,
			session ) )
		{
			session_message_ip_address_changed_exit(
				application_name,
				login_name );
		}

		if ( !login_name
		||   !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				"insert" )
		|| !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				"update" ) )
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
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new(
				application_title_string(
					application_name ),
				application_name );

	document->output_content_type = 1;

	with_dynarch_menu =
		appaserver_frameset_menu_horizontal(
					application_name,
					login_name );

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
			application_relative_source_directory(
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
		if ( system( sys_string ) ){};
		fflush( stdout );
		printf( "</ul>\n" );
	}
	else
	{
		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );
	}

	sprintf( title,
		 "%s %s %s",
		 application_title_string( application_name ),
		 format_initial_capital( buffer1,
					 process_name ),
		 format_initial_capital( buffer2,
					 folder_name ) );

	printf( "<h1>%s</h1>\n", title );

	fflush( stdout );

	if ( strcmp( state, "one" ) == 0 )
	{
		post_state_one(	application_name,
				session,
				role_name,
				folder_name,
				login_name,
				process_name );
	}
	else
	if ( strcmp( state, "two" ) == 0 )
	{
		post_state_two(	application_name,
				session,
				role_name,
				folder_name,
				appaserver_parameter_file->
					appaserver_data_directory );
	}

	document_close();
	return 0;
} /* main() */

void post_state_one(	char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			char *login_name,
			char *process_name )
{
	ELEMENT_APPASERVER *element;
	FOLDER *folder;
	char buffer[ 256 ];
	LIST *option_data_list;
	LIST *option_label_list;

	folder = folder_new_folder( 	application_name,
					session,
					folder_name );

	folder->attribute_list =
		folder_get_attribute_list(
				application_name,
				folder->folder_name );

	printf( "<form enctype=\"multipart/form-data\""
		" method=post"
		" action=\"%s/post_merge_purge?%s+%s+%s+%s+%s+two\""
		" target=%s>\n",
		appaserver_parameter_file_get_cgi_directory(),
		application_name,
		session,
		role_name,
		folder_name,
		process_name,
		EDIT_FRAME );

	printf( "<table border=1>\n" );

	/* Build keep drop-down */
	/* -------------------- */
	element = element_appaserver_new(
			drop_down,
			KEEP_LABEL );

	element->drop_down->option_data_list =
		folder_get_primary_data_list(
			application_name,
			session,
			folder->folder_name,
			login_name,
			(DICTIONARY *)0 /* parameter_dictionary */,
			(DICTIONARY *)0 /* where_clause_dictionary */,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			(PROCESS *)0 /* populate_drop_down_process */,
			folder->attribute_list,
			(LIST *)0
			/* common_non_pr...bute_name_list */,
			folder->row_level_non_owner_forbid,
			(LIST *)0
			/* preprompt_accou...e_name_list */,
			role_name,
			(char *)0 /* state */,
			(char *)0
			/* one2m_folder_name_for_processes */,
			(char *)0
			/* appaserver_user_foreign_login_name */,
			0 /* not include_root_folder */ );

	printf( "<tr><td>Keep %s\n",
		format_initial_capital( buffer, folder_name ) );

	printf( "<td>" );
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
				(char *)0 /* post_change_javascript */,
				element->drop_down->max_drop_down_size,
				element->drop_down->multi_select_element_name,
				element->drop_down->onblur_javascript_function,
				(char *)0 /* background_color */,
				element->drop_down->date_piece_offset,
				1 /* no_initial_capital */,
				element->drop_down->readonly,
				0 /* tab_index */,
				element->drop_down->state,
				element->drop_down->attribute_width );
			fflush( stdout );

	/* Build purge drop-down */
	/* -------------------- */
	element = element_appaserver_new(
			drop_down,
			PURGE_LABEL );

	element->drop_down->option_data_list =
		folder_get_primary_data_list(
			application_name,
			session,
			folder->folder_name,
			login_name,
			(DICTIONARY *)0
				/* parameter_dictionary */,
			(DICTIONARY *)0
				/* where_clause_dictionary */,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			(PROCESS *)0 /* populate_drop_down_process */,
			folder->attribute_list,
			(LIST *)0
			/* common_non_pr...bute_name_list */,
			folder->row_level_non_owner_forbid,
			(LIST *)0
			/* preprompt_accou...e_name_list */,
			role_name,
			(char *)0 /* state */,
			(char *)0
			/* one2m_folder_name_for_processes */,
			(char *)0
			/* appaserver_user_foreign_login_name */,
			0 /* not include_root_folder */ );

	printf( "<tr><td>Purge %s\n",
		format_initial_capital( buffer, folder_name ) );

	printf( "<td>" );
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
				(char *)0 /* post_change_javascript */,
				element->drop_down->max_drop_down_size,
				element->drop_down->multi_select_element_name,
				element->drop_down->onblur_javascript_function,
				(char *)0 /* background_color */,
				element->drop_down->date_piece_offset,
				1 /* no_initial_capital */,
				element->drop_down->readonly,
				0 /* tab_index */,
				element->drop_down->state,
				element->drop_down->attribute_width );
			fflush( stdout );

	/* Build the execute_yn drop-down */
	/* ------------------------------ */
	element =
		element_appaserver_new(
			drop_down,
			"execute_yn" );

	option_data_list = list_new();
	list_set_pointer( option_data_list, "y" );
	list_set_pointer( option_data_list, "n" );

	element->drop_down->option_data_list = option_data_list;

	option_label_list = list_new();
	list_set_pointer( option_label_list, "Yes" );
	list_set_pointer( option_label_list, "No" );

	element->drop_down->option_label_list = option_label_list;

	printf( "<tr><td>Execute\n" );

	printf( "<td>" );
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
				(char *)0 /* post_change_javascript */,
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

	printf( "<tr><td colspan=2>					"
		"<input type=\"button\" value=\"%s\"			"
		"onclick=\"document.forms[0].submit();\">\n		"
		"<input type=\"reset\" value=\"Reset\">\n",
		SUBMIT_BUTTON_LABEL );

	printf( "</table>\n" );
	printf( "</form>\n" );

} /* post_state_one() */

void post_state_two(	char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			char *appaserver_data_directory )
{
	DICTIONARY *post_dictionary;
	char *execute_yn;
	char *keep_data;
	char *purge_data;
	FOLDER *folder;

	folder = folder_with_load_new( 	application_name,
					session,
					folder_name,
					role_new(
						application_name,
						role_name ) );

	if ( !folder )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: folder_with_load_new() returned NULL.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_dictionary =
		post2dictionary(stdin,
				appaserver_data_directory,
				session );

	keep_data =
		dictionary_get_pointer(
			post_dictionary,
			KEEP_LABEL );

	if ( !keep_data || timlib_strcmp( keep_data, "select" ) == 0 )
	{
		printf( "<h3>Warning: please select the keep data.</h3>\n" );
		return;
	}

	purge_data =
		dictionary_get_pointer(
			post_dictionary,
			PURGE_LABEL );

	if ( !purge_data || timlib_strcmp( purge_data, "select" ) == 0 )
	{
		printf( "<h3>Warning: please select the purge data.</h3>\n" );
		return;
	}

	if ( timlib_strcmp( keep_data, purge_data ) == 0 )
	{
		printf( "<h3>Warning: please select different values.</h3>\n" );
		return;
	}

	if ( !list_rewind( folder->one2m_recursive_related_folder_list ) )
	{
		printf(
		"<h3>Warning: you should simply delete %s.</h3>\n",
		purge_data );
		return;
	}

	execute_yn =
		dictionary_get_pointer(
			post_dictionary,
			"execute_yn" );

	if ( !execute_yn || *execute_yn != 'y' )
	{
		output_test_only_report(
				application_name,
				folder,
				purge_data /* purge_data_list_string */ );
	}
	else
	{
		output_update_database(
				application_name,
				folder,
				keep_data /* keep_data_list_string */,
				purge_data /* purge_data_list_string */ );

		output_delete_database(
				application_name,
				session,
				role_name,
				appaserver_data_directory,
				folder,
				purge_data );

		printf( "<h3>Process complete.</h3>\n" );
	}

} /* post_state_two() */

void output_test_only_report(
				char *application_name,
				FOLDER *folder,
				char *purge_data_list_string )
{
	RELATED_FOLDER *related_folder;
	LIST *purge_data_list;
	LIST *where_attribute_name_list;

	if ( !list_rewind( folder->one2m_recursive_related_folder_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty one2m_recursive_related_folder_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	purge_data_list =
		list_string2list(
			purge_data_list_string,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	if ( !folder->primary_attribute_name_list )
	{
		folder->primary_attribute_name_list =
			folder_get_primary_attribute_name_list(
				folder->attribute_list );
	}

	do {
		related_folder =
			list_get_pointer(
				folder->one2m_recursive_related_folder_list );

		where_attribute_name_list =
			related_folder_foreign_attribute_name_list( 
				folder->primary_attribute_name_list,
				related_folder->
					related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );

		output_test_only_related_folder(
			application_name,
			related_folder,
			where_attribute_name_list,
			purge_data_list );

	} while( list_next( folder->one2m_recursive_related_folder_list ) );

} /* output_test_only_report() */

void update_database_related_folder(
			char *application_name,
			RELATED_FOLDER *related_folder,
			LIST *where_attribute_name_list,
			LIST *keep_data_list,
			LIST *purge_data_list )
{
	char *table_name;
	LIST *primary_attribute_name_list;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *where;
	char *set_attribute_name;
	char *new_data;
	char *executable;

	if ( !list_length( where_attribute_name_list )
	||   list_length( where_attribute_name_list ) !=
	     list_length( keep_data_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: invalid input.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			related_folder->one2m_folder->attribute_list );

	table_name = get_table_name(
			application_name,
			related_folder->one2m_folder->folder_name );

	if ( DEBUG_MODE )
		executable = "cat";
	else
		executable = "sql.e";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s	|"
		 "%s 2>&1				|"
		 "html_paragraph_wrapper.e		 ",
		 table_name,
		 list_display_delimited( primary_attribute_name_list, ',' ),
		 executable );

	output_pipe = popen( sys_string, "w" );

	where = query_login_name_where_clause(
			related_folder->one2m_folder,
			where_attribute_name_list,
			purge_data_list,
			(char *)0 /* login_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 application_name,
		 list_display_delimited( primary_attribute_name_list, ',' ),
		 related_folder->one2m_folder->folder_name,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf(	output_pipe,
				"%s",
				input_buffer );

		list_rewind( where_attribute_name_list );
		list_rewind( keep_data_list );

		do {
			set_attribute_name =
				list_get(
					where_attribute_name_list );

			new_data =
				list_get(
					keep_data_list );

			fprintf(output_pipe,
				"|%s=%s",
				set_attribute_name,
				new_data );

			list_next( keep_data_list );

		} while( list_next( where_attribute_name_list ) );
		fprintf( output_pipe, "\n" );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* update_database_related_folder() */

void output_test_only_related_folder(
			char *application_name,
			RELATED_FOLDER *related_folder,
			LIST *where_attribute_name_list,
			LIST *purge_data_list )
{
	LIST *attribute_name_list;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	FILE *input_pipe;
	char buffer[ 128 ];
	char input_buffer[ 1024 ];
	char *where;

	attribute_name_list =
		folder_get_attribute_name_list(
			related_folder->one2m_folder->attribute_list );

	sprintf( sys_string,
		 "queue_top_bottom_lines.e %d	|"
		 "html_table.e '^^%s' '%s' '%c'  ",
		 TABLE_LINES_TO_QUEUE,
		 format_initial_capital(
			buffer,
			related_folder->one2m_folder->folder_name ),
		 list_display_delimited( attribute_name_list, ',' ),
		 FOLDER_DATA_DELIMITER );

	output_pipe = popen( sys_string, "w" );

	where = query_login_name_where_clause(
			related_folder->one2m_folder,
			where_attribute_name_list,
			purge_data_list,
			(char *)0 /* login_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 application_name,
		 list_display_delimited( attribute_name_list, ',' ),
		 related_folder->one2m_folder->folder_name,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
		fprintf( output_pipe, "%s\n", input_buffer );

	pclose( input_pipe );
	pclose( output_pipe );

} /* output_test_only_related_folder() */

void output_update_database(
				char *application_name,
				FOLDER *folder,
				char *keep_data_list_string,
				char *purge_data_list_string )
{
	RELATED_FOLDER *related_folder;
	LIST *keep_data_list;
	LIST *purge_data_list;
	LIST *where_attribute_name_list;

	if ( !list_rewind( folder->one2m_recursive_related_folder_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty one2m_recursive_related_folder_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	keep_data_list =
		list_string2list(
			keep_data_list_string,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	purge_data_list =
		list_string2list(
			purge_data_list_string,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	if ( !folder->primary_attribute_name_list )
	{
		folder->primary_attribute_name_list =
			folder_get_primary_attribute_name_list(
				folder->attribute_list );
	}

	do {
		related_folder =
			list_get_pointer(
				folder->one2m_recursive_related_folder_list );

		where_attribute_name_list =
			related_folder_foreign_attribute_name_list( 
				folder->primary_attribute_name_list,
				related_folder->
					related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );

		update_database_related_folder(
			application_name,
			related_folder,
			where_attribute_name_list,
			keep_data_list,
			purge_data_list );

	} while( list_next( folder->one2m_recursive_related_folder_list ) );

} /* output_update_database() */

void output_delete_database(
				char *application_name,
				char *session,
				char *role_name,
				char *appaserver_data_directory,
				FOLDER *folder,
				char *purge_data )
{
	char sys_string[ 1024 ];
	RELATED_FOLDER *related_folder;
	LIST *where_attribute_name_list;
	char *table_name;
	char *executable;

	if ( !list_rewind( folder->one2m_recursive_related_folder_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty one2m_recursive_related_folder_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	if ( !folder->primary_attribute_name_list )
	{
		folder->primary_attribute_name_list =
			folder_get_primary_attribute_name_list(
				folder->attribute_list );
	}

	if ( DEBUG_MODE )
		executable = "html_paragraph_wrapper.e";
	else
		executable = "sql.e";

	do {
		related_folder =
			list_get_pointer(
				folder->one2m_recursive_related_folder_list );

		where_attribute_name_list =
			related_folder_foreign_attribute_name_list( 
				folder->primary_attribute_name_list,
				related_folder->
					related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list );

		table_name =
			get_table_name(
				application_name,
			 	related_folder->
					one2m_folder->
					folder_name );

		sprintf( sys_string,
			 "echo \"%s\"					|"
			 "delete_statement.e table=%s field=%s del='%c'	|"
			 "%s						 ",
			 purge_data,
			 table_name,
			 list_display( where_attribute_name_list ),
			 MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			 executable );

		if ( system( sys_string ) ){};

		folder_menu_refresh_row_count(
			application_name,
				related_folder->
				one2m_folder->
				folder_name,
			session,
			appaserver_data_directory,
			role_name );

	} while( list_next( folder->one2m_recursive_related_folder_list ) );

	where_attribute_name_list = folder->primary_attribute_name_list;

	table_name =
		get_table_name(
			application_name,
			folder->folder_name );

	sprintf( sys_string,
		 "echo \"%s\"					|"
		 "delete_statement.e table=%s field=%s del='%c'	|"
		 "%s						 ",
		 purge_data,
		 table_name,
		 list_display( where_attribute_name_list ),
		 MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
		 executable );

	if ( system( sys_string ) ){};

	folder_menu_refresh_row_count(
		application_name,
		folder->folder_name,
		session,
		appaserver_data_directory,
		role_name );

} /* output_delete_database() */
