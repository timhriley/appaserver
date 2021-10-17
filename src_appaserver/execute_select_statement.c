/* ---------------------------------------------------	*/
/* src_appaserver/execute_select_statement.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "list.h"
#include "environ.h"
#include "process.h"
#include "basename.h"
#include "session.h"
#include "piece.h"
#include "token.h"
#include "application.h"
#include "appaserver_link.h"
#include "attribute.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define TABLE_QUEUE_TOP_BOTTOM	1000
#define SPOOL_STATEMENT_TEMPLATE "%s/%s_execute_select_statement_%s.sql"

/* Prototypes */
/* ---------- */
void output_file(			char *filename );

LIST *get_asteric_column_name_list(	char *application_name,
					char *role_name,
					char *table_name );

char *get_select_statement_title(
				char *input_filename );

char *spool_from_table(		char *application_name,
				char *select_statement_title,
				char *select_statement_login_name,
				char *session,
				char *appaserver_data_directory );

boolean is_non_selectable_attribute(
				char *application_name,
				char *role_name,
				char *attribute_name );

LIST *get_non_selectable_attribute_name_list(
				char *application_name,
				char *role_name );

LIST *get_selectable_folder_name_list(
				char *application_name,
				char *login_name );

boolean is_selectable_folder(	char *application_name,
				char *login_name,
				char *table_name );

LIST *get_column_name_list(	char **error_message,
				FILE *input_file,	
				char *application_name,
				char *role_name );

boolean from_clause_valid(	char **table_name,
				char *application_name,
				FILE *input_file,
				char *login_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *output_medium;
	char *session;
	char *role_name;
	char *input_filename;
	FILE *input_file;
	char buffer[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char output_sys_string[ 1024 ];
	char sys_string[ 4096 ];
	char asteric_sed[ 1024 ];
	LIST *column_name_list;
	char *output_filename;
	char *ftp_filename = {0};
	char *error_message = {0};
	char *select_statement_title;
	char *select_statement_login_name;
	char *table_name;
	char *first_column;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf( stderr, 
"Usage: %s ignored process login_name session role output_medium filename select_statement_title select_statement_login_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	session = argv[ 4 ];
	role_name = argv[ 5 ];
	output_medium = argv[ 6 ];
	input_filename = argv[ 7 ];
	select_statement_title = argv[ 8 ];
	select_statement_login_name = argv[ 9 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );

	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};

	printf( "</h2>\n" );
	fflush( stdout );

	if ( ( !*input_filename
	||   	strcmp( input_filename, "filename" ) == 0 )
	&&   ( !*select_statement_title
	||      strcmp( select_statement_title,
			"select_statement_title" ) == 0 ) )
	{
		printf(
	"<h3>Please choose a select statement.</h3>\n" );
		document_close();
		exit( 0 );
	}
	else
	if ( *select_statement_title
	&&   strcmp(	select_statement_title,
			"select_statement_title" ) != 0
	&&   ( !*input_filename
	||      strcmp( input_filename,
			"filename" ) == 0 ) )
	{
		input_filename =
			spool_from_table(
				application_name,
				select_statement_title,
				select_statement_login_name,
				session,
				appaserver_parameter_file->
					appaserver_data_directory );
	}
	else
	if ( *input_filename
	&&   strcmp( input_filename, "filename" ) != 0
	&&   *select_statement_title
	&&   strcmp(	select_statement_title,
			"select_statement_title" ) != 0 )
	{
		input_filename =
			spool_from_table(
				application_name,
				select_statement_title,
				select_statement_login_name,
				session,
				appaserver_parameter_file->
					appaserver_data_directory );
	}

	if ( get_file_size( input_filename ) == 0 )
	{
		printf( "<h3>Error: the statement is empty.</h3>\n" );
		document_close();
		exit( 0 );
	}

	fflush( stdout );
	printf( "<p>" );
	output_file( input_filename );
	printf( "<p><br>" );
	printf( "\n" );
	fflush( stdout );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf(stderr,
			"Error in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_filename );
		exit( 1 );
	}

	if ( ! ( column_name_list =
			get_column_name_list(
				&error_message,
				input_file,
				application_name,
				role_name ) )
	|| !list_length( column_name_list ) )
	{
		if ( error_message )
		{
			printf( "<h3>Error: %s.</h3>\n", error_message );
		}
		else
		{
			printf(
			"<h3>Error: cannot decern the column names.</h3>\n" );
		}
		fclose( input_file );
		document_close();
		exit( 0 );
	}

	fclose( input_file );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf(stderr,
			"Error in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_filename );
		exit( 1 );
	}

	if ( !from_clause_valid(	&table_name,
					application_name,
					input_file,
					login_name ) )
	{
		printf( "<h3>Error: invalid from clause.</h3>\n" );
		document_close();
		fclose( input_file );
		exit( 0 );
	}

	fclose( input_file );

	first_column = list_get_first_pointer( column_name_list );

	if ( *first_column == '*' )
	{
		column_name_list =
			get_asteric_column_name_list(
				application_name,
				role_name,
				table_name );

		sprintf( asteric_sed,
		 	 "sed 's/^select[ ][ ]*\\*/select %s/'",
		 	 list_display( column_name_list ) );
	}
	else
	{
		strcpy( asteric_sed, "cat" );
	}

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0
	||   strcmp( output_medium, "table" ) == 0 )
	{
		/* Need to escape commas in select statement. */
		/* ------------------------------------------ */
		char heading_comma_list[ 2048 ];

		strcpy(	heading_comma_list,
			list_display_delimited( column_name_list, '%' ) );

		search_replace_string( heading_comma_list, ",", "\\," );
		search_replace_character( heading_comma_list, '%', ',' );

		sprintf(output_sys_string,
			"queue_top_bottom_lines.e %d	|"
			"html_table.e '' \"%s\" '%c'	 ",
			TABLE_QUEUE_TOP_BOTTOM,
		 	heading_comma_list,
		 	FOLDER_DATA_DELIMITER );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0
	|| ( strcmp( output_medium, "spreadsheet" ) == 0 ) )
	{
		pid_t process_id = getpid();
		FILE *output_file;
		APPASERVER_LINK *appaserver_link;

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				process_name /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"csv" );

		output_filename = appaserver_link->output->filename;
		ftp_filename = appaserver_link->prompt->filename;

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			char buffer1[ 1024 ];
			char buffer2[ 1024 ];

			fprintf( output_file,
				 "%s\n",
				 format_initial_capital(
				 buffer1,
		 		 list_display_double_quote_comma_delimited(
					buffer2,
					column_name_list ) ) );

			fclose( output_file );
		}

		sprintf( output_sys_string,
			 "double_quote_comma_delimited.e '%c' >> %s",
			 FOLDER_DATA_DELIMITER,
			 output_filename );
	}
	else
	{
		strcpy( output_sys_string, "cat" );
	}

	/* Remove any semicolons to prevent SQL injection */
	/* ---------------------------------------------- */
	sprintf( sys_string,
		 "cat \"%s\"				|"
		 "grep -v '^#'				|"
		 "sed 's/;//g'				|"
		 "%s					|"
		 "sql.e '%c' 2>&1			|"
		 "%s",
		 input_filename,
		 asteric_sed,
		 FOLDER_DATA_DELIMITER,
		 output_sys_string );

	if ( system( sys_string ) ) {};

	if ( strcmp( output_medium, "text_file" ) == 0
	|| ( strcmp( output_medium, "spreadsheet" ) == 0 ) )
	{
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

	if ( !*select_statement_title
	||   strcmp(	select_statement_title,
			"select_statement_title" ) == 0 )
	{
		char *select_statement_title;
		char sys_string[ 1024 ];
		char *table_name;
		char *field;

		table_name =
			get_table_name( application_name,
					"select_statement" );

		field =
		"select_statement,select_statement_title,login_name";

		select_statement_title =
			get_select_statement_title(
				input_filename );

		sprintf( sys_string,
"cat %s								|"
"joinlines.e ' '						|"
"sed 's/$/^%s/'							|"
"sed 's/$/^%s/'							|"
"insert_statement.e table=%s field=%s delimiter='^' replace=y	|"
"sql.e 2>&1							|"
"html_paragraph_wrapper.e					 ",
			 input_filename,
			 select_statement_title,
			 login_name,
			 table_name,
			 field );

		if ( system( sys_string ) ) {};
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );

}

#define beginning_state		0
#define select_state		1
#define from_state		2
#define subsequent_state	3

LIST *get_column_name_list(	char **error_message,
				FILE *input_file,
				char *application_name,
				char *role_name )
{
	char buffer[ 2048 ];
	char column_name[ 2048 ];
	int state = beginning_state;
	int parenthesis_nest;
	char *buf_ptr;
	LIST *column_name_list = list_new();
	char *token_ptr;	/* Note: token_ptr is the 4K static buffer */
				/*       inside of get_token().		 */

	*column_name = '\0';
	parenthesis_nest = 0;

	while( get_line( buffer, input_file ) )
	{
		if ( *buffer == '#' ) continue;

		buf_ptr = buffer;

        	while( 1 )
        	{
                	token_ptr = get_token( &buf_ptr );

                	if ( !*token_ptr ) break;

			if ( strcasecmp( token_ptr, "password" ) == 0 )
			{
				*error_message =
					"Cannot select passwords";
				return (LIST *)0;
			}
			else
			if ( state == beginning_state
			&&   strcasecmp( token_ptr, "select" ) != 0 )
			{
				*error_message = "Not a select statement";
				return (LIST *)0;
			}
			else
			if ( state == beginning_state )
			{
				state = select_state;
			}
			else
			if ( *token_ptr == ';' )
			{
				return column_name_list;
			}
			else
			if ( strcasecmp( token_ptr, "from" ) == 0 )
			{
				list_append_pointer(
					column_name_list,
					strdup( column_name ) );

				return column_name_list;
			}
			else
			if ( *token_ptr == '(' )
			{
				strcat( column_name, token_ptr );
				parenthesis_nest++;
			}
			else
			if ( *token_ptr == ')' )
			{
				strcat( column_name, token_ptr );
				parenthesis_nest--;
			}
			else
			if ( *token_ptr == ',' && !parenthesis_nest )
			{
				list_append_pointer(
					column_name_list,
					strdup( column_name ) );
				*column_name = '\0';
			}
			else
			{
				strcat( column_name, token_ptr );

				if ( is_non_selectable_attribute(
					application_name,
					role_name,
					column_name ) )
				{
					char message[ 128 ];

					sprintf( message,
"You are trying to select an excluded column: %s, logged in as role: %s",
						 column_name,
						 role_name );
					*error_message = strdup( message );
					return (LIST *)0;
				}
			}
		}
	}

	if ( state == beginning_state )
		return (LIST *)0;
	else
		return column_name_list;

}

boolean from_clause_valid(	char **table_name,
				char *application_name,
				FILE *input_file,
				char *login_name )
{
	char buffer[ 2048 ];
	int state = beginning_state;
	char *buf_ptr;
	char *token_ptr;	/* Note: token_ptr is the 4K static buffer */
				/*       inside of get_token().		 */

	while( get_line( buffer, input_file ) )
	{
		if ( *buffer == '#' ) continue;

		buf_ptr = buffer;

        	while( 1 )
        	{
                	token_ptr = get_token( &buf_ptr );
                	if ( !*token_ptr ) break;

			if ( strcasecmp( token_ptr, "from" ) == 0 )
			{
				state = from_state;
			}
			else
			if ( strcasecmp( token_ptr, "where" ) == 0 )
			{
				state = subsequent_state;
			}
			else
			if ( strcasecmp( token_ptr, "group" ) == 0 )
			{
				state = subsequent_state;
			}
			else
			if ( strcasecmp( token_ptr, "order" ) == 0 )
			{
				state = subsequent_state;
			}
			else
			if ( *token_ptr == ';' )
			{
				state = subsequent_state;
			}
			else
			if ( *token_ptr == ',' )
			{
				continue;
			}
			else
			if ( state == from_state )
			{
				if ( !is_selectable_folder(
					application_name,
					login_name,
					token_ptr ) )
				{
					return 0;
				}
				else
				{
					*table_name = strdup( token_ptr );
					return 1;
				}
			}
		}
	}
	return 0;

}

boolean is_selectable_folder(	char *application_name,
				char *login_name,
				char *table_name )
{
	char *folder_name;
	static LIST *selectable_folder_name_list = {0};

	if ( !*table_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty table_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !selectable_folder_name_list )
	{
		selectable_folder_name_list =
			get_selectable_folder_name_list(
				application_name,
				login_name );
	}

	folder_name = get_folder_name( application_name, table_name );

	if ( !*folder_name ) return 0;

	return list_exists_string(
			folder_name,
			selectable_folder_name_list );

}

LIST *get_selectable_folder_name_list(
				char *application_name,
				char *login_name )
{
	char *role_appaserver_user_table_name;
	char *role_folder_table_name;
	char where_clause[ 512 ];
	char sys_string[ 512 ];

	role_appaserver_user_table_name =
		get_table_name(	application_name,
				"role_appaserver_user" );

	role_folder_table_name =
		get_table_name(	application_name,
				"role_folder" );

	sprintf( where_clause,
		 "login_name = '%s' and				"
		 "%s.role = %s.role and				"
		 "permission in ('lookup','update')		",
		 login_name,
		 role_appaserver_user_table_name,
		 role_folder_table_name );

	sprintf( sys_string,
		 "echo \"select distinct folder from %s,%s where %s;\" | sql.e",
		 role_appaserver_user_table_name,
		 role_folder_table_name,
		 where_clause );

	return pipe2list( sys_string );

}

boolean is_non_selectable_attribute(	char *application_name,
					char *role_name,
					char *attribute_name )
{
	static LIST *non_selectable_attribute_name_list = {0};

	if ( !non_selectable_attribute_name_list )
	{
		non_selectable_attribute_name_list =
			get_non_selectable_attribute_name_list(
				application_name,
				role_name );
	}

	return list_exists_string(
			attribute_name,
			non_selectable_attribute_name_list );
}

LIST *get_non_selectable_attribute_name_list(
				char *application_name,
				char *role_name )
{
	char *attribute_exclude_table_name;
	char where_clause[ 512 ];
	char sys_string[ 512 ];

	attribute_exclude_table_name =
		get_table_name(	application_name,
				"attribute_exclude" );

	sprintf( where_clause,
		 "%s.role = '%s' and				"
		 "permission = 'lookup'				",
		 attribute_exclude_table_name,
		 role_name );

	sprintf( sys_string,
	"echo \"select attribute from %s where %s;\" | sql.e",
		 attribute_exclude_table_name,
		 where_clause );

	return pipe2list( sys_string );

}

char *spool_from_table(		char *application_name,
				char *select_statement_title,
				char *select_statement_login_name,
				char *session,
				char *appaserver_data_directory )
{
	static char input_filename[ 128 ];
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf(input_filename,
	 	SPOOL_STATEMENT_TEMPLATE,
	 	appaserver_data_directory,
		application_name,
	 	session );

	sprintf(where,
		"select_statement_title = '%s' and login_name = '%s'",
		select_statement_title,
		select_statement_login_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		  "
		 "			select=select_statement	  "
		 "			folder=select_statement	  "
		 "			where=\"%s\"		| "
		 "cat > %s					  ",
		 application_name,
		 where,
		 input_filename );

	if ( system( sys_string ) ) {};

	return input_filename;

}

char *get_select_statement_title(
				char *input_filename )
{
	static char statement_title[ 256 ];

	strcpy( statement_title,
		basename_get_base_name(
			input_filename,
			1 /* strip_extension */ ) );

	right_trim_session( statement_title );

	return statement_title;

}

LIST *get_asteric_column_name_list(	char *application_name,
					char *role_name,
					char *folder_name )
{
	LIST *attribute_list;
	LIST *name_list;
	LIST *non_selectable_attribute_name_list;

	attribute_list =
		attribute_get_list(
			application_name,
			folder_name,
			role_name );

	name_list =
		attribute_name_list_extract(
			attribute_list );

	non_selectable_attribute_name_list =
		get_non_selectable_attribute_name_list(
			application_name,
			role_name );

	return list_subtract_list(
			name_list,
			non_selectable_attribute_name_list );

}

void output_file( char *filename )
{
	FILE *file;
	char input_buffer[ 4096 ];

	if ( ! ( file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		exit( 1 );
	}

	while( get_line( input_buffer, file ) )
		printf( "%s\n", input_buffer );

	fclose( file );

}

