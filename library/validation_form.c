/* library/validation_form.c						*/
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
#include "hashtbl.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "validation_form.h"
#include "form.h"

VALIDATION_FORM *validation_form_new( 	char *title, 
					char *target_frame,
					LIST *primary_column_name_list,
					LIST *datatype_name_list )
{
	VALIDATION_FORM *v;

	v = (VALIDATION_FORM *)calloc( 1, sizeof( VALIDATION_FORM ) );
	v->title = title;
	v->target_frame = target_frame;
	v->primary_column_name_list = primary_column_name_list;
	v->datatype_name_list = datatype_name_list;
	return v;
}

ROW *validation_form_row_new( char *primary_key )
{
	ROW *row;

	row = (ROW *)calloc( 1, sizeof( ROW ) );
	row->primary_key = strdup( primary_key );
	row->primary_data_hash_table = hash_table_new( hash_table_small );
	row->datatype_data_hash_table = hash_table_new( hash_table_small );
	return row;
}

LIST *validation_form_get_element_list( LIST *primary_column_name_list,
					LIST *datatype_name_list )
{
	LIST *element_list = list_new();
	char buffer[ 1024 ];
	ELEMENT_APPASERVER *element;
	char *primary_column_name;
	char *datatype_name;

	if ( !list_reset( primary_column_name_list ) )
	{
		fprintf(stderr,
"ERROR in validation_form_get_element_list: primary_column_name_list is empty\n" );
		exit( 1 );
	}

	if ( !list_reset( datatype_name_list ) )
	{
		fprintf(stderr,
"ERROR in validation_form_get_element_list: datatype_name_list is empty\n" );
		exit( 1 );
	}

	/* Create a line break */
	/* ------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append( 	element_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	/* For each primary column */
	/* ----------------------- */
	do {
		primary_column_name = 
			list_get_string( primary_column_name_list );

		/* Create a prompt element for the primary column */
		/* ---------------------------------------------- */
		element = 
			element_appaserver_new( 
				prompt_data,
				strdup( format_initial_capital( 
				   buffer, 
				   primary_column_name ) ) );

		list_append( 	element_list, 
				element, 
				sizeof( ELEMENT_APPASERVER ) );
	} while( list_next( primary_column_name_list ) );

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
				sizeof( ELEMENT_APPASERVER ) );

	} while( list_next( datatype_name_list ) );

	return element_list;
}

void validation_form_set_action( 	VALIDATION_FORM *validation_form, 
					char *action_string )
{
	validation_form->action_string = strdup( action_string );
}

void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					LIST *element_list,
					int table_border )
{
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

	validation_form_output_table_heading( element_list );

}

void validation_form_output_table_heading( LIST *element_list )
{
	ELEMENT_APPASERVER *element;
	char buffer[ 1024 ];
	char *heading;

	if ( element_list
	&&   list_reset( element_list ) )
	{
		printf( "<tr>" );
		do {
			element = list_get( element_list );
			heading = element_get_heading( (char **)0, element, 0 );
			if ( heading && *heading )
			{
				printf( "<th><p>%s", 
					format_initial_capital( 
						buffer, 
						heading ) );
			}
		} while( next_item( element_list ) );
		printf( "\n" );
	}
}

void validation_form_output_body( VALIDATION_FORM *validation_form )
{
	ELEMENT_APPASERVER *element;
	int row_int;
	char *data;
	VALIDATION_DATATYPE *datatype;
	ROW *row;
	char *primary_column_name;
	char *datatype_name;

	if ( !list_reset( validation_form->element_list ) )
	{
		fprintf(stderr,
	"ERROR: validation_form_output_body has empty element list.\n" );
		exit( 1 );
	}

	if ( !list_rewind( validation_form->row_list ) )
	{
		fprintf(stderr,
	"ERROR in validation_form_output_body(): row list is empty.\n" );
		exit( 1 );
	}

	if ( !list_length( validation_form->element_list ) )
	{
		fprintf(stderr,
	"ERROR in validation_form_output_body(): element list is empty.\n" );
		exit( 1 );
	}

	row_int = 1;
	do {
		row = list_get_pointer( validation_form->row_list );
		list_rewind( validation_form->element_list );
		list_rewind( validation_form->primary_column_name_list );

		/* Output the line break */
		/* --------------------- */
		element = (ELEMENT_APPASERVER *)
			list_get( validation_form->element_list );
		element_simple_output( element, row_int );
		list_next( validation_form->element_list );

		/* Output the primary columns */
		/* -------------------------- */
		do {
			primary_column_name = 
				list_get_string( validation_form->
						 primary_column_name_list );
			data =  hash_table_get_pointer(
					row->primary_data_hash_table,
					primary_column_name );

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
				list_get_pointer(
					validation_form->element_list );
			element_set_data( element, data );
			element_simple_output( element, row_int );
			list_next( validation_form->element_list );
		} while( list_next( validation_form->
				    primary_column_name_list ) );

		/* Output the datatypes */
		/* -------------------- */
		list_rewind( validation_form->datatype_name_list );
		do {
			datatype_name = 
				list_get_pointer(
					validation_form->datatype_name_list );

			datatype = hash_table_get_pointer( 
					row->datatype_data_hash_table,
					datatype_name );

			/* Output the datatype text item */
			/* ----------------------------- */
			if ( datatype )
			{
				element =
					list_get_pointer(
						validation_form->element_list );
				data = datatype->value;
				element_set_data( element, data );
				element_simple_output( element, row_int );
			}
			else
			{
				element_blank_data_output( stdout );
			}

			list_next( validation_form->element_list );

		} while( list_next( validation_form->
				    datatype_name_list ) );

		row_int++;
	} while( next_item( validation_form->row_list ) );

}

void validation_form_output_trailer( void )
{
	printf( "<tr><td>" );

	form_output_submit_button(
			(char *)0,
			"Validate",
			0 /* form_number */ );

	printf( "</table></form>\n" );
}

void validation_form_set_primary_data_hash_table(
			HASH_TABLE *primary_data_hash_table,
			LIST *primary_column_name_list,
			char *primary_data_comma_list )
{
	char primary_data[ 1024 ];
	char *primary_column_name;
	int i;

	if ( 	list_length( primary_column_name_list ) !=
		count_characters( ',', primary_data_comma_list ) + 1 )
	{
		fprintf( stderr,
"validation_form_set_primary_data_hash_table() ERROR: column list = (%s) does not match data list = (%s)\n",
			 list_display( primary_column_name_list ),
			 primary_data_comma_list );
		exit( 1 );
	}

	for( 	i = 0, list_rewind( primary_column_name_list );
		piece( primary_data, ',', primary_data_comma_list, i );
		list_next( primary_column_name_list ), i++ )
	{
		primary_column_name = 
			list_get_string( primary_column_name_list );

		hash_table_set_string( 	primary_data_hash_table,
					strdup( primary_column_name ),
					strdup( primary_data ) );
	}
}

VALIDATION_DATATYPE *validation_datatype_new( char *value, int validated )
{
	VALIDATION_DATATYPE *d;

	d = (VALIDATION_DATATYPE *)calloc( 1, sizeof( VALIDATION_DATATYPE ) );
	d->value = value;
	d->validated = validated;
	return d;
}


int row_compare( ROW *row1, ROW *row2 )
{
	return strcmp( row1->primary_key, row2->primary_key );
}

LIST *validation_form_get_row_list(	LIST *primary_column_name_list,
					char *select_sys_string )
{
	FILE *p;
	char input_buffer[ 4096 ];
	char right_half[ 4096 ];
	char buffer[ 1024 ];
	char primary_data_comma_list[ 4096 ];
	char value[ 128 ];
	int validated;
	ROW *row;
	LIST *row_list = list_new();
	char datatype_name[ 1024 ];

	p = popen( select_sys_string, "r" );

	/* Input specification: primary_data_1,...,n|datatype=value=validated */
	/* ------------------------------------------------------------------ */
	while( get_line( input_buffer, p ) )
	{
		piece( primary_data_comma_list, '|', input_buffer, 0 );
		piece( right_half, '|', input_buffer, 1 );
		piece( datatype_name, '=', right_half, 0 );
		piece( value, '=', right_half, 1 );
		validated = atoi( piece( buffer, '=', right_half, 2 ) );

		row = validation_form_row_new( 
				primary_data_comma_list );

		list_append_pointer( row_list, (char *)row );

		validation_form_set_primary_data_hash_table(
			row->primary_data_hash_table,
			primary_column_name_list,
			primary_data_comma_list );

		hash_table_set(	row->datatype_data_hash_table,
				strdup( datatype_name ),
				validation_datatype_new(
					strdup( value ),
					validated ) );
	} /* while( get_line() ) */
	pclose( p );
	return row_list;
}

