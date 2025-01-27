/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/table_rectification.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "environ.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "html.h"
#include "process.h"
#include "security.h"
#include "rectification.h"

void table_rectification_missing_folder(
		LIST *rectification_mismatch_list );

void table_rectification_missing_table(
		LIST *rectification_mismatch_list );

void table_rectification_missing_column(
		LIST *rectification_mismatch_list );

void table_rectification_missing_attribute(
		LIST *rectification_mismatch_list );

void table_rectification_datatype_mismatch(
		LIST *rectification_mismatch_list );

void table_rectification_width_mismatch(
		LIST *rectification_mismatch_list );

void table_rectification_key_mismatch(
		LIST *rectification_mismatch_list );

void table_rectification_not_in_role_table(
		LIST *rectification_not_in_role_table_name_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *process_name;
	RECTIFICATION *rectification;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s session login process\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	process_name = argv[ 3 ];

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	rectification =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		rectification_fetch(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				session_key ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				login_name ) );

	table_rectification_missing_column(
		rectification->
			rectification_mismatch_list );

	table_rectification_datatype_mismatch(
		rectification->
			rectification_mismatch_list );

	table_rectification_width_mismatch(
		rectification->
			rectification_mismatch_list );

	table_rectification_key_mismatch(
		rectification->
			rectification_mismatch_list );

	table_rectification_missing_attribute(
		rectification->
			rectification_mismatch_list );

	table_rectification_missing_table(
		rectification->
			rectification_mismatch_list );

	table_rectification_missing_folder(
		rectification->
			rectification_mismatch_list );

	table_rectification_not_in_role_table(
		rectification->not_in_role_table_name_list );

	process_increment_count( process_name );
	document_close();

	return 0;
}

void table_rectification_missing_column(
		LIST *rectification_mismatch_list )
{
	HTML_TABLE *html_table;
	LIST *cell_list;
	RECTIFICATION_MISMATCH *rectification_mismatch;

	html_table =
		html_table_new(
		   "Missing Mysql Column",
		   (char *)0 /* sub_title */,
		   (char *)0 /* sub_sub_title */ );

	html_table->column_list = list_new();
	html_table->row_list = list_new();

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Table",
			0 /* not right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Mysql Column",
			0 ) );

	if ( !list_rewind( rectification_mismatch_list ) )
	{
		html_table_output(
			html_table,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
		return;
	}

	do {
		rectification_mismatch =
			list_get(
				rectification_mismatch_list );

		if ( rectification_mismatch->missing_column_name )
		{
			cell_list = list_new();

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->folder_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						missing_column_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				html_table->row_list,
				html_row_new( cell_list ) );
		}

	} while( list_next( rectification_mismatch_list ) );

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void table_rectification_datatype_mismatch(
		LIST *rectification_mismatch_list )
{
	HTML_TABLE *html_table;
	LIST *cell_list;
	RECTIFICATION_MISMATCH *rectification_mismatch;
	char *datatype_name;

	html_table =
		html_table_new(
		   "Datatype Mismatch",
		   (char *)0 /* sub_title */,
		   (char *)0 /* sub_sub_title */ );

	html_table->column_list = list_new();
	html_table->row_list = list_new();

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Table",
			0 /* not right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Column",
			0 ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Column Datatype",
			0 ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Mysql Column Datatype",
			0 ) );

	if ( !list_rewind( rectification_mismatch_list ) )
	{
		html_table_output(
			html_table,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
		return;
	}

	do {
		rectification_mismatch =
			list_get(
				rectification_mismatch_list );

		if ( rectification_mismatch->datatype_boolean )
		{
			cell_list = list_new();

			if ( !rectification_mismatch->rectification_attribute
			||   !rectification_mismatch->rectification_column )
			{
				char message[ 256 ];

				sprintf(message,
				"rectification_mismatch is incomplete." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->folder_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						rectification_attribute->
						attribute_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			if ( rectification_mismatch->
					rectification_attribute->
					datatype_name )
			{
				datatype_name =
					rectification_mismatch->
						rectification_attribute->
						datatype_name;
			}
			else
			{
				datatype_name = "Missing";
			}

			list_set(
				cell_list,
				html_cell_new(
					datatype_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						rectification_column->
						datatype_text,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				html_table->row_list,
				html_row_new( cell_list ) );
		}

	} while( list_next( rectification_mismatch_list ) );

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void table_rectification_width_mismatch(
		LIST *rectification_mismatch_list )
{
	HTML_TABLE *html_table;
	LIST *cell_list;
	RECTIFICATION_MISMATCH *rectification_mismatch;

	html_table =
		html_table_new(
		   "Width Mismatch",
		   (char *)0 /* sub_title */,
		   (char *)0 /* sub_sub_title */ );

	html_table->column_list = list_new();
	html_table->row_list = list_new();

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Table",
			0 /* not right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Column",
			0 ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Column Width",
			1 /* right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Mysql Column Width",
			1 /* right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Column Float Decimal",
			1 /* right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Mysql Column Float Decimal",
			1 /* right_justify_boolean */ ) );

	if ( !list_rewind( rectification_mismatch_list ) )
	{
		html_table_output(
			html_table,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
		return;
	}

	do {
		rectification_mismatch =
			list_get(
				rectification_mismatch_list );

		if ( rectification_mismatch->width_boolean
		||   rectification_mismatch->float_boolean )
		{
			if ( !rectification_mismatch->rectification_attribute
			||   !rectification_mismatch->rectification_column )
			{
				char message[ 256 ];

				sprintf(message,
				"rectification_mismatch is incomplete." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			cell_list = list_new();

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->folder_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						rectification_attribute->
						attribute_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
				     strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_itoa(
					     rectification_mismatch->
						rectification_attribute->
						width ) ),
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
				     strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_itoa(
					     rectification_mismatch->
						rectification_column->
						width ) ),
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
				     strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_itoa(
					     rectification_mismatch->
						rectification_attribute->
						float_decimal_places ) ),
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
				     strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_itoa(
					     rectification_mismatch->
						rectification_column->
						float_decimal_places ) ),
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				html_table->row_list,
				html_row_new( cell_list ) );
		}

	} while( list_next( rectification_mismatch_list ) );

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void table_rectification_key_mismatch(
		LIST *rectification_mismatch_list )
{
	HTML_TABLE *html_table;
	LIST *cell_list;
	RECTIFICATION_MISMATCH *rectification_mismatch;

	html_table =
		html_table_new(
		   "Key Mismatch",
		   (char *)0 /* sub_title */,
		   (char *)0 /* sub_sub_title */ );

	html_table->column_list = list_new();
	html_table->row_list = list_new();

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Table",
			0 /* not right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Mysql Column",
			0 ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Primary Key Index",
			0 ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Mysql Column Key",
			0 ) );

	if ( !list_rewind( rectification_mismatch_list ) )
	{
		html_table_output(
			html_table,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
		return;
	}

	do {
		rectification_mismatch =
			list_get(
				rectification_mismatch_list );

		if ( rectification_mismatch->key_boolean )
		{
			cell_list = list_new();

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->folder_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						rectification_attribute->
						attribute_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
				   strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_itoa(
					   rectification_mismatch->
						rectification_attribute->
						primary_key_index ) ),
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
				   strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_itoa(
					   rectification_mismatch->
						rectification_column->
						primary_key_index  ) ),
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				html_table->row_list,
				html_row_new( cell_list ) );
		}

	} while( list_next( rectification_mismatch_list ) );

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void table_rectification_missing_attribute(
		LIST *rectification_mismatch_list )
{
	HTML_TABLE *html_table;
	LIST *cell_list;
	RECTIFICATION_MISMATCH *rectification_mismatch;

	html_table =
		html_table_new(
		   "Missing Appaserver Column",
		   (char *)0 /* sub_title */,
		   (char *)0 /* sub_sub_title */ );

	html_table->column_list = list_new();
	html_table->row_list = list_new();

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Table",
			0 /* not right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Mysql Column",
			0 ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Execute",
			0 ) );

	if ( !list_rewind( rectification_mismatch_list ) )
	{
		html_table_output(
			html_table,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
		return;
	}

	do {
		rectification_mismatch =
			list_get(
				rectification_mismatch_list );

		if ( rectification_mismatch->missing_attribute_name )
		{
			cell_list = list_new();

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->folder_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						missing_attribute_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						drop_column_tag,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				html_table->row_list,
				html_row_new( cell_list ) );
		}

	} while( list_next( rectification_mismatch_list ) );

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void table_rectification_missing_table(
		LIST *rectification_mismatch_list )
{
	HTML_TABLE *html_table;
	LIST *cell_list;
	RECTIFICATION_MISMATCH *rectification_mismatch;

	html_table =
		html_table_new(
		   "Missing Mysql Table",
		   (char *)0 /* sub_title */,
		   (char *)0 /* sub_sub_title */ );

	html_table->column_list = list_new();
	html_table->row_list = list_new();

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Table",
			0 /* not right_justify_boolean */ ) );

	if ( !list_rewind( rectification_mismatch_list ) )
	{
		html_table_output(
			html_table,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
		return;
	}

	do {
		rectification_mismatch =
			list_get(
				rectification_mismatch_list );

		if ( rectification_mismatch->missing_table )
		{
			cell_list = list_new();

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->folder_name,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				html_table->row_list,
				html_row_new( cell_list ) );
		}

	} while( list_next( rectification_mismatch_list ) );

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void table_rectification_missing_folder(
		LIST *rectification_mismatch_list )
{
	HTML_TABLE *html_table;
	LIST *cell_list;
	RECTIFICATION_MISMATCH *rectification_mismatch;

	html_table =
		html_table_new(
		   "Mysql Table Not In Role",
		   (char *)0 /* sub_title */,
		   (char *)0 /* sub_sub_title */ );

	html_table->column_list = list_new();
	html_table->row_list = list_new();

	list_set(
		html_table->column_list,
		html_column_new(
			"Appaserver Table",
			0 /* not right_justify_boolean */ ) );

	list_set(
		html_table->column_list,
		html_column_new(
			"Execute",
			0 ) );

	if ( !list_rewind( rectification_mismatch_list ) )
	{
		html_table_output(
			html_table,
			HTML_TABLE_ROWS_BETWEEN_HEADING );
		return;
	}

	do {
		rectification_mismatch =
			list_get(
				rectification_mismatch_list );

		if ( rectification_mismatch->missing_folder )
		{
			cell_list = list_new();

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->missing_folder,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				cell_list,
				html_cell_new(
					rectification_mismatch->
						drop_table_tag,
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );

			list_set(
				html_table->row_list,
				html_row_new( cell_list ) );
		}

	} while( list_next( rectification_mismatch_list ) );

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void table_rectification_not_in_role_table(
		LIST *rectification_not_in_role_table_name_list )
{
	fflush( stdout );
	printf( "<h1>Appaserver Table Not In Role</h1>\n" );
	fflush( stdout );

	list_html_table_display(
		rectification_not_in_role_table_name_list,
		"Appaserver Table" /* heading_string */ );

	fflush( stdout );
}

