/* ----------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/table_rectification.c */
/* ----------------------------------------------------- */
/* 						       	 */
/* Freely available software: see Appaserver.org	 */
/* ----------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "timlib.h"
#include "query.h"
#include "document.h"
#include "folder.h"
#include "application.h"
#include "list.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "session.h"
#include "piece.h"
#include "role.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *column_name;
	char *datatype;
	int width;
	int float_decimal_places;
} COLUMN;
	
/* Constants */
/* --------- */
#define PROCESS_NAME		"table_rectification"

/* Prototypes */
/* ---------- */
void output_tables_can_be_dropped(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					LIST *folder_list,
					LIST *table_name_list );

char *get_test_drop_table_control_string(
					char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *table_name );

LIST *get_table_name_list(		void );

char *get_attribute_datatype_string(	ATTRIBUTE *attribute );

int attributes_the_same(		ATTRIBUTE *attribute,
					COLUMN *column );

COLUMN *new_column(			char *column_name );

void output_columns_not_used_anymore(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					LIST *folder_list );

void output_attributes_not_in_tables(	char *application_name,
					LIST *folder_list );

void output_columns_different_from_attributes(
					char *application_name,
					LIST *folder_list );

char *get_drop_table_control_string(
					char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *table_name );

char *get_drop_column_control_string(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *folder_name,
					char *attribute_name );

LIST *get_table_column_name_list(
					char *application_name,
					char *folder_name );

LIST *get_table_column_list(
					char *application_name,
					char *folder_name );

LIST *get_folder_list(			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	LIST *folder_list;
	LIST *table_name_list;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s ignored session login_name role\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];

	appaserver_parameter_file = appaserver_parameter_file_new();
	document = document_new( "", application_name );

/* No longer prompting for Application.
	document_set_output_content_type( document );
*/

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

	folder_list = get_folder_list( application_name );
	table_name_list = get_table_name_list();

	output_columns_not_used_anymore(
		application_name,
		session,
		login_name,
		role_name,
		folder_list );

	printf( "<br>\n" );

	output_attributes_not_in_tables(
		application_name,
		folder_list );

	printf( "<br>\n" );

	output_columns_different_from_attributes(
		application_name,
		folder_list );

	printf( "<br>\n" );

	output_tables_can_be_dropped(
		application_name,
		session,
		login_name,
		role_name,
		folder_list,
		table_name_list );

	document_close();

	process_increment_execution_count(
		application_name,
		PROCESS_NAME,
		appaserver_parameter_file_get_dbms() );

	exit( 0 );

} /* main() */

LIST *get_table_name_list( void )
{
	char sys_string[ 128 ];
	strcpy( sys_string, "echo \"show tables;\" | sql" );
	return pipe2list( sys_string );
}

LIST *get_folder_list( char *application_name )
{
	FOLDER *folder;
	LIST *folder_list;

	folder_list =
		folder_get_folder_list(
			application_name,
			1 /* override_row_restrictions */,
			(char *)0 /* role_name */ );

	if ( !list_rewind( folder_list ) ) return (LIST *)0;

	do {
		folder = list_get_pointer( folder_list );

		folder->attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	} while( list_next( folder_list ) );

	return folder_list;
} /* get_folder_list() */

LIST *get_every_folder_list( char *application_name )
{
	FOLDER *folder;
	LIST *folder_list;
	LIST *folder_name_list;
	char *folder_name;

	folder_name_list = folder_get_folder_name_list( application_name );

	if ( !list_rewind( folder_name_list ) ) return( LIST *)0;

	folder_list = list_new_list();
	do {
		folder_name = list_get_pointer( folder_name_list );

		if ( strcmp( folder_name, "null" ) == 0 ) continue;

		folder = folder_new_folder(
				application_name,
				BOGUS_SESSION,
				folder_name );

		list_append_pointer( folder_list, folder );

	} while( list_next( folder_name_list ) );
	return folder_list;

} /* get_every_folder_list() */

LIST *get_table_column_list(
				char *application_name,
				char *folder_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	char piece_buffer1[ 512 ];
	char piece_buffer2[ 128 ];
	char *table_name;
	FILE *input_pipe;
	LIST *column_list;
	COLUMN *column;

	table_name = get_table_name( application_name, folder_name );

	sprintf(sys_string,
		"echo \"show columns from %s;\" | sql",
		table_name );

	input_pipe = popen( sys_string, "r" );

	column_list = list_new_list();

	while( get_line( input_buffer, input_pipe  ) )
	{
		piece( piece_buffer1, '^', input_buffer, 0 );
		column = new_column( strdup( piece_buffer1 ) );

		piece( piece_buffer1, '^', input_buffer, 1 );
		column->datatype = strdup( piece_buffer1 );

		list_append_pointer( column_list, column );

		if ( !piece( piece_buffer2, '(', piece_buffer1, 1 ) )
			continue;

		column->width = atoi( piece_buffer2 );

		if ( piece( piece_buffer1, ',', piece_buffer2, 1 ) )
		{
			column->float_decimal_places = atoi( piece_buffer1 );
		}
	}
	pclose( input_pipe );
	return column_list;
} /* get_table_column_list() */

LIST *get_table_column_name_list(
				char *application_name,
				char *folder_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = get_table_name( application_name, folder_name );

	sprintf(sys_string,
		"echo \"show columns from %s;\" | sql.e | piece.e '^' 0",
		table_name );

	return pipe2list( sys_string );
} /* get_table_column_name_list() */

char *get_drop_column_control_string(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *folder_name,
					char *attribute_name )
{
	static char control_string[ 1024 ];

	sprintf(control_string,
	"<a href=\"drop_column?%s+%s+%s+%s+%s+%s+y\" target=_new>Drop</a>\n",
		application_name,
		session,
		login_name,
		role_name,
		folder_name,
		attribute_name );

	return control_string;

} /* get_drop_column_control_string() */

void output_tables_can_be_dropped(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					LIST *folder_list,
					LIST *table_name_list )
{
	char *table_name;
	HTML_TABLE *html_table;
	LIST *heading_list;

	if ( !list_length( folder_list ) )
	{
		printf( "<p>ERROR: cannot get folder list for application\n" );
		document_close();
		exit( 1 );
	}

	html_table = new_html_table(
		"Table Rectification -- Table Can Be Dropped",
		(char *)0 /* sub_title */ );

	heading_list = new_list();
	list_append_string( heading_list, "Table" );

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	list_rewind( table_name_list );

	do {
		table_name = list_get_pointer( table_name_list );

		if ( timlib_exists_string( table_name, "_application" ) )
			continue;

		if ( !(boolean)folder_seek_folder(
				folder_list,
				table_name /* folder_name */ ) )
		{
			html_table_set_data(
				html_table->data_list,
				get_drop_table_control_string(
					application_name,
					session,
					login_name,
					role_name,
					table_name ) );

			html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			html_table->data_list = list_new();
		}

	} while( list_next( table_name_list ) );

	html_table_close();

} /* output_tables_can_be_dropped() */

void output_attributes_not_in_tables(	char *application_name,
					LIST *folder_list )
{
	FOLDER *folder;
	LIST *table_column_name_list;
	LIST *residual_attribute_name_list;
	HTML_TABLE *html_table;
	LIST *heading_list;

	if ( !list_rewind( folder_list ) )
	{
		printf( "<p>ERROR: cannot get folder list for application\n" );
		document_close();
		exit( 1 );
	}

	html_table = new_html_table(
		"Table Rectification -- Attributes Not In Table",
		(char *)0 /* sub_title */ );

	heading_list = new_list();
	list_append_string( heading_list, "Table" );
	list_append_string( heading_list, "Column" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	do {
		folder = list_get_pointer( folder_list );

		table_column_name_list =
			get_table_column_name_list(
				application_name,
				folder->folder_name );

		residual_attribute_name_list =
			list_subtract_string_list(
				folder_get_attribute_name_list(
					folder->attribute_list ),
				table_column_name_list );
					

		if ( list_rewind( residual_attribute_name_list ) )
		{
			char *residual_attribute_name;

			do {
				residual_attribute_name =
					list_get_pointer(
						residual_attribute_name_list );

				html_table_set_data(
					html_table->data_list,
					folder->folder_name );
				html_table_set_data(
					html_table->data_list,
					residual_attribute_name );

				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );

				html_table->data_list = list_new();

			} while( list_next( residual_attribute_name_list ) );
		}

		list_free_string_list( table_column_name_list );

	} while( list_next( folder_list ) );

	html_table_close();

} /* output_attributes_not_in_tables() */

void output_columns_not_used_anymore(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					LIST *folder_list )
{
	FOLDER *folder;
	LIST *table_column_name_list;
	LIST *residual_attribute_name_list;
	HTML_TABLE *html_table;
	LIST *heading_list;

	if ( !list_rewind( folder_list ) )
	{
		printf( "<p>ERROR: cannot get folder list for application\n" );
		document_close();
		exit( 1 );
	}

	html_table = new_html_table(
		"Table Rectification -- Columns Not In FOLDER_ATTRIBUTE",
		(char *)0 /* sub_title */ );

	heading_list = new_list();
	list_append_string( heading_list, "Table" );
	list_append_string( heading_list, "Column" );
	list_append_string( heading_list, "Drop" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	do {
		folder = list_get_pointer( folder_list );

		table_column_name_list =
			get_table_column_name_list(
				application_name,
				folder->folder_name );

		residual_attribute_name_list =
			list_subtract_string_list(
				table_column_name_list,
				folder_get_attribute_name_list(
					folder->attribute_list ) );
					

		if ( list_rewind( residual_attribute_name_list ) )
		{
			char *residual_attribute_name;

			do {
				residual_attribute_name =
					list_get_pointer(
						residual_attribute_name_list );

				html_table_set_data(
					html_table->data_list,
					folder->folder_name );

				html_table_set_data(
					html_table->data_list,
					residual_attribute_name );

				html_table_set_data(
					html_table->data_list,
					get_drop_column_control_string(
						application_name,
						session,
						login_name,
						role_name,
						folder->folder_name,
						residual_attribute_name ) );

				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );

				html_table->data_list = list_new();

			} while( list_next( residual_attribute_name_list ) );
		}

		list_free_string_list( table_column_name_list );

	} while( list_next( folder_list ) );

	html_table_close();
} /* output_columns_not_used_anymore() */

void output_columns_different_from_attributes(
					char *application_name,
					LIST *folder_list )
{
	FOLDER *folder;
	LIST *table_column_list;
	HTML_TABLE *html_table;
	LIST *heading_list;
	COLUMN *column;
	ATTRIBUTE *attribute;

	if ( !list_rewind( folder_list ) )
	{
		printf( "<p>ERROR: cannot get folder list for application\n" );
		document_close();
		exit( 1 );
	}

	html_table = new_html_table(
		"Table Rectification -- Columns Different from Attributes",
		(char *)0 /* sub_title */ );

	heading_list = new_list();
	list_append_string( heading_list, "Table" );
	list_append_string( heading_list, "Column" );
	list_append_string( heading_list, "Column Datatype" );
	list_append_string( heading_list, "Attribute Datatype" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	do {
		folder = list_get_pointer( folder_list );

		table_column_list =
			get_table_column_list(
				application_name,
				folder->folder_name );

		if ( !list_rewind( table_column_list ) ) continue;

		do {
			column =
				list_get_pointer(
					table_column_list );

			attribute =
				attribute_seek_attribute(
					column->column_name,
					folder->attribute_list );

			if ( !attribute ||
			      attributes_the_same(
					attribute,
					column ) )
			{
				continue;
			}

			html_table_set_data(
				html_table->data_list,
				folder->folder_name );

			html_table_set_data(
				html_table->data_list,
				column->column_name );

			html_table_set_data(
				html_table->data_list,
				column->datatype );

			html_table_set_data(
				html_table->data_list,
				get_attribute_datatype_string(
					attribute ) );

			html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				(LIST *)0 /* justify_list */ );

			html_table->data_list = list_new();
		} while( list_next( table_column_list ) );

	} while( list_next( folder_list ) );

	html_table_close();
} /* output_columns_different_from_attributes() */

COLUMN *new_column( char *column_name )
{
	COLUMN *column = (COLUMN *)calloc( 1, sizeof( COLUMN ) );

	if ( !column )
	{
		fprintf(stderr,
			"MEMORY ALLOCATION ERROR in %s/%s()\n",
			__FILE__, __FUNCTION__ );
		exit( 1 );
	}

	column->column_name = column_name;
	return column;
} /* new_column() */

int attributes_the_same(	ATTRIBUTE *attribute,
				COLUMN *column )
{
	if ( timlib_strncmp( column->datatype, "date" ) == 0 )
	{
		if ( timlib_strncmp( attribute->datatype, "date" ) == 0
		||   timlib_strncmp( attribute->datatype, "current" ) == 0 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ( strcmp( attribute->datatype, "integer" ) == 0 )
	{
		if ( timlib_strncmp( column->datatype, "int" ) == 0 )
			return 1;
		else
			return 0;
	}

	if ( strcmp( column->datatype, "text" ) == 0 )
	{
		if ( strcmp( attribute->datatype, "text" ) == 0
		||   strcmp( attribute->datatype, "notepad" ) == 0 )
			return 1;
		else
			return 0;
	}

	if ( attribute->width != column->width )
		return 0;

	if ( timlib_strncmp( column->datatype, "float" ) == 0 )
	{
		if ( attribute->float_decimal_places ==
		     column->float_decimal_places ) 
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return 1;

} /* attributes_the_same() */

char *get_attribute_datatype_string( ATTRIBUTE *attribute )
{
	char datatype_string[ 128 ];

	if ( strcmp( attribute->datatype, "float" ) == 0 )
	{
		sprintf( datatype_string,
			 "double(%d,%d)",
			 attribute->width,
			 attribute->float_decimal_places );
	}
	else
	if ( strcmp( attribute->datatype, "text" ) == 0 )
	{
		sprintf( datatype_string,
			 "char(%d)",
			 attribute->width );
	}
	else
	{
		sprintf( datatype_string,
			 "%s(%d)",
			 attribute->datatype,
			 attribute->width );
	}

	return strdup( datatype_string );

} /* get_attribute_datatype_string() */

char *get_drop_table_control_string(
						char *application_name,
						char *session,
						char *login_name,
						char *role_name,
						char *table_name )
{
	static char control_string[ 1024 ];

	sprintf(control_string,
	"<a href=\"drop_table?%s+%s+%s+%s+%s+y\" target=_new>Drop %s</a>\n",
		application_name,
		session,
		login_name,
		role_name,
		table_name,
		table_name );

	return control_string;

} /* get_drop_table_control_string() */

char *get_test_drop_table_control_string(
						char *application_name,
						char *session,
						char *login_name,
						char *role_name,
						char *table_name )
{
	static char control_string[ 1024 ];

	sprintf(control_string,
	"<a href=\"drop_table?%s+%s+%s+%s+%s+n\" target=_new>Test</a>\n",
		application_name,
		session,
		login_name,
		role_name,
		table_name );
	return control_string;
} /* get_test_drop_table_control_string() */

LIST *get_application_name_list( void )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"echo \"show tables;\"			|"
		"sql					|"
		"grep '_application$'			|"
		"sed 's/_application$//'		|"
		"sort -u				 " );

	return pipe2list( sys_string );
	/* return pipe2list( "echo everglades" ); */
} /* get_application_name_list() */

