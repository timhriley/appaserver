/* $APPASERVER_HOME/library/validation_form.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver validation_form ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "element.h"
#include "piece.h"
#include "hash_table.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "validation_form.h"
#include "form.h"

VALIDATION_FORM *validation_form_calloc( void )
{
	VALIDATION_FORM *validation_form;

	if ( ! ( validation_form =
			calloc( 1, sizeof( VALIDATION_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return validation_form;
}

VALIDATION_FORM *validation_form_new(
			char *title, 
			char *target_frame,
			LIST *primary_name_list,
			LIST *datatype_name_list,
			char *action_string,
			boolean table_border,
			char *data_process )
{
	VALIDATION_FORM *validation_form = validation_form_calloc();

	validation_form->title = title;
	validation_form->target_frame = target_frame;
	validation_form->primary_name_list = primary_name_list;
	validation_form->datatype_name_list = datatype_name_list;
	validation_form->action_string = action_string;
	validation_form->action_string = action_string;
	validation_form->action_string = action_string;

	validation_form->element_list = 
		validation_form_element_list( 
		   validation_form->primary_name_list,
		   validation_form->datatype_name_list );

	validation_form->row_list =
		validation_form_row_list(
			validation_form->primary_name_list,
			data_process );

	return validation_form;
}

VALIDATION_FORM_ROW *validation_form_row_calloc( void )
{
	VALIDATION_FORM_ROW *validation_form_row;

	if ( ! ( validation_form_row =
			calloc( 1, sizeof( VALIDATION_FORM_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return validation_form_row;
}

VALIDATION_FORM_ROW *validation_form_row_new(
			char *primary_data_column_list_string )
{
	VALIDATION_FORM_ROW *validation_form_row =
		validation_form_row_calloc();

	validation_form_row->primary_data_column_list_string =
		primary_data_column_list_string;

	validation_form_row->primary_data_hash_table =
		hash_table_new(
			hash_table_small );

	validation_form_row->datatype_data_hash_table =
		hash_table_new(
			hash_table_small );

	return validation_form_row;
}

LIST *validation_form_element_list(
			LIST *primary_name_list,
			LIST *datatype_name_list )
{
	LIST *element_list = list_new();
	char buffer[ 1024 ];
	APPASERVER_ELEMENT *element;
	char *primary_name;
	char *datatype_name;

	if ( !list_reset( primary_name_list ) )
	{
		fprintf(stderr,
"ERROR in validation_form_get_element_list: primary_name_list is empty\n" );
		exit( 1 );
	}

	if ( !list_reset( datatype_name_list ) )
	{
		fprintf(stderr,
"ERROR in validation_form_get_element_list: datatype_name_list is empty\n" );
		exit( 1 );
	}

	/* Create a table row */
	/* ------------------ */
	element = appaserver_element_new( table_row );
	list_set( element_list, element );

	/* For each primary column */
	/* ----------------------- */
	do {
		primary_name = 
			list_get( primary_name_list );

		/* Create a prompt element for the primary column */
		/* ---------------------------------------------- */
		element =
			appaserver_element( 
				non_edit_text );

		element->non_edit_text->html =
			element_non_edit_text_html(
				format_initial_capital( 
					buffer, 
					primary_name ) );

		list_set( element_list, element );

	} while( list_next( primary_name_list ) );

	/* For each datatype */
	/* ------------------ */
	do {
		datatype_name = list_get_string( datatype_name_list );

		/* Create a prompt element for the value */
		/* ------------------------------------- */
		element = 
			element_appaserver_new( 
				prompt_data,
				strdup( format_initial_capital( 
				   buffer, 
				   datatype_name ) ) );
		list_append(	element_list, 
				element, 
				sizeof( APPASERVER_ELEMENT ) );

	} while( list_next( datatype_name_list ) );

	return element_list;
}

char *validation_form_heading(
			char *table_title,
			char *action_string,
			char *target_frame,
			LIST *element_list,
			int table_border )
{
	char heading[ STRING_INPUT_BUFFER ];
	char *ptr = heading;

	printf( "<h1>%s</h1>\n", title );

	/* Output form tag */
	/* --------------- */
	printf( "<form method=post" );
	printf( " action=%s", action_string );
	printf( " target=\"%s\"", target_frame );
	printf( ">\n" );

	/* Output table tag */
	/* ---------------- */
	printf(
"	<table cellspacing=0 cellpadding=0" );

	if ( table_border ) printf( " border" );

	printf( ">\n" );

	validation_form_table_heading( element_list );
}

char *validation_form_table_heading( LIST *element_list )
{
	char buffer[ 1024 ];
	char *heading;

	if ( list_rewind( element_list ) )
	{
		printf( "<tr>" );
		do {
			heading =
				element_heading(
					list_get( element_list ) );

			if ( heading && *heading )
			{
				printf( "<th><p>%s", 
					format_initial_capital( 
						buffer, 
						heading ) );
			}
		} while( list_next( element_list ) );
		printf( "\n" );
	}
}

char *validation_form_body(
			VALIDATION_FORM *validation_form )
{
	APPASERVER_ELEMENT *element;
	int row_int;
	char *data;
	VALIDATION_DATATYPE_DATA *datatype_data;
	VALIDATION_FORM_ROW *row;
	char *primary_column_name;
	char *datatype_name;
	char body[ STRING_INPUT_BUFFER ];
	char *ptr = body;

	if ( !list_length( validation_form->element_list ) ) return (char *)0;
	if ( !list_rewind( validation_form->row_list ) ) return (char *)0;

	row_int = 1;

	do {
		row = list_get( validation_form->row_list );

		list_rewind( validation_form->element_list );
		list_rewind( validation_form->primary_name_list );

		/* Output the line break */
		/* --------------------- */
		element = list_get( validation_form->element_list );
		element_simple_output( element, row_int );
		list_next( validation_form->element_list );

		/* Output the primary columns */
		/* -------------------------- */
		do {
			primary_column_name = 
				list_get(
					validation_form->
						 primary_column_name_list );

			data =
				hash_table_get(
					row->primary_data_hash_table,
					primary_name );

			if ( !data )
			{
				fprintf( stderr,
			"Warning in %s/%s()/%d: cannot find data for %s\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 primary_column_name );

				data = "";
			}

			element =
				list_get(
					validation_form->element_list );

			element_set_data( element, data );

			element_simple_output( element, row_int );

			list_next( validation_form->element_list );

		} while( list_next( validation_form->primary_name_list ) );

		/* Output the datatypes */
		/* -------------------- */
		list_rewind( validation_form->datatype_name_list );

		do {
			datatype_name = 
				list_get(
					validation_form->datatype_name_list );

			datatype =
				hash_table_get( 
					row->datatype_data_hash_table,
					datatype_name );

			/* Output the datatype text item */
			/* ----------------------------- */
			if ( datatype )
			{
				element =
					list_get(
						validation_form->
							element_list );

				data = datatype->datatype_value;
				element_set_data( element, data );
				element_simple_output( element, row_int );
			}
			else
			{
				element_blank_data_output( stdout );
			}

			list_next( validation_form->element_list );

		} while( list_next( validation_form->datatype_name_list ) );

		row_int++;

	} while( next_item( validation_form->row_list ) );
}

char *validation_form_trailer( void )
{
	char trailer[ 1024 ];
	char *ptr = trailer;
	char *tmp;

	ptr += sprintf( ptr, "<tr><td>" );

	ptr += sprintf(
		ptr,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp = form_button_submit(
			(char *)0,
			"Validate",
			0 /* form_number */ ) );

	free( tmp );

	ptr += sprintf( ptr, "</table></form>\n" );

	return strdup( trailer );
}

void validation_form_row_set_primary_data_hash_table(
			HASH_TABLE *primary_data_hash_table,
			LIST *primary_name_list,
			char *primary_data_comma_list_string )
{
	char primary_data[ 1024 ];
	char *primary_name;
	int i;

	if ( 	list_length( primary_name_list ) !=
		count_characters( ',', primary_data_comma_list_string ) + 1 )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: column and data are out of balance .\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	for( 	i = 0, list_rewind( primary_name_list );
		piece( primary_data, ',', primary_data_comma_list, i );
		list_next( primary_name_list ), i++ )
	{
		hash_table_set(
			primary_data_hash_table,
			list_get( primary_name_list ),
			strdup( primary_data ) );
	}
}

VALIDATION_DATATYPE_DATA *validation_datatype_data_calloc( void )
{
	VALIDATION_DATATYPE_DATA *validation_datatype_data;

	if ( ! ( validation_datatype_data =
			calloc( 1, sizeof( VALIDATION_DATATYPE_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return validation_datatype_data;
}

VALIDATION_DATATYPE_DATA *validation_datatype_data_new(
			char *datatype_value,
			boolean validated )
{
	VALIDATION_DATATYPE_DATA *validation_datatype_data =
		validation_datatype_data_calloc();

	validation_datatype_data->datatype_value = datatype_value;
	validation_datatype_data->validated = validated;

	return validation_datatype_data;
}


int row_compare( ROW *row1, ROW *row2 )
{
	return strcmp( row1->primary_key, row2->primary_key );
}

LIST *validation_form_row_list(
			LIST *primary_name_list,
			char *system_string )
{
	FILE *p;
	char input_buffer[ 4096 ];
	char right_half[ 4096 ];
	char buffer[ 1024 ];
	char primary_data_comma_list_string[ 4096 ];
	char datatype_name[ 1024 ];
	char datatype_value[ 128 ];
	boolean validated;
	VALIDATION_FORM_ROW *validation_form_row;
	LIST *row_list = list_new();

	p = popen( system_string, "r" );

	/* Input specification: primary_data_1,...,n|datatype=value=validated */
	/* ------------------------------------------------------------------ */
	while( get_line( input_buffer, p ) )
	{
		piece( primary_data_comma_list_string, '|', input_buffer, 0 );
		piece( right_half, '|', input_buffer, 1 );
		piece( datatype_name, '=', right_half, 0 );
		piece( value, '=', right_half, 1 );

		validated =
			(boolean)
				atoi( piece(
					buffer,
					'=',
					right_half,
					2 ) );

		validation_form_row =
			validation_form_row_new( 
				primary_data_comma_list_string );

		list_set( row_list, validation_form_row );

		validation_form_row_set_primary_data_hash_table(
			validation_form_row->primary_data_hash_table,
			primary_column_name_list,
			primary_data_comma_list );

		validation_form_row_set_datatype_data_hash_table(
			validation_form_row->datatype_data_hash_table,
			strdup( datatype_name ),
			strdup( datatype_value ),
			validated );

	} /* while( get_line() ) */

	pclose( p );
	return row_list;
}

void validation_form_row_set_datatype_data_hash_table(
			HASH_TABLE *datatype_data_hash_table,
			char *datatype_name,
			char *datatype_value,
			boolean validated )
{
	hash_table_set(	datatype_data_hash_table,
			datatype_name,
			validation_datatype_data_new(
				datatype_value,
				validated ) ) );
}

VALIDATION_DATATYPE_DATA *validation_datatype_data_calloc( void )
{
	VALIDATION_DATATYPE_DATA *validation_datatype_data;

	if ( ! ( validation_datatype_data =
			calloc( 1, sizeof( VALIDATION_DATATYPE_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return validation_datatype_data;
}

VALIDATION_DATATYPE_DATA *validation_datatype_data_new(
			char *datatype_data,
			boolean validated )
{
	VALIDATION_DATATYPE_DATA *validation_datatype_data =
		validation_datatype_data_calloc();

	validation_datatype_data->datatype_data = datatype_data;
	validation_datatype_data->validated = validated;

	return validation_datatype_data;
}

