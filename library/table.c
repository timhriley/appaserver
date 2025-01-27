/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/table.c			   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "table.h"

TABLE_CELL *table_cell_calloc( void )
{
	TABLE_CELL *table_cell;

	if ( ! ( table_cell =
			calloc( 1, sizeof ( TABLE_CELL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return table_cell;
}

TABLE_CELL *table_cell_string_new(
		char *data_string,
		TABLE_COLUMN *table_column )
{
	TABLE_CELL *table_cell = table_cell_calloc();

	table_cell->data_string = data_string;
	table_cell->table_column = table_column;

	return table_cell;
}

TABLE_CELL *table_cell_double_new(
		double data_double,
		TABLE_COLUMN *table_column )
{
	TABLE_CELL *table_cell = table_cell_calloc();

	table_cell->data_double = data_double;
	table_cell->table_column = table_column;

	return table_cell;
}

TABLE_ROW *table_row_new( int row_number )
{
	TABLE_ROW *table_row;

	if ( ! ( table_row =
			calloc( 1, sizeof ( TABLE_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	table_row->row_number = row_number;
	return table_row;
}

TABLE_CELL *table_cell_string_set(
		LIST *table_row_cell_list,
		char *data_string,
		TABLE_COLUMN *table_column )
{
	TABLE_CELL *table_cell;

	table_cell =
		table_cell_string_new(
			data_string,
			table_column );

	list_set( table_row_cell_list, table_cell );

	return table_cell;
}

TABLE_CELL *table_cell_double_set(
		LIST *table_row_cell_list,
		double data_double,
		TABLE_COLUMN *table_column )
{
	TABLE_CELL *table_cell;

	table_cell =
		table_cell_double_new(
			data_double,
			table_column );

	list_set( table_row_cell_list, table_cell );

	return table_cell;
}

TABLE *table_new( char *table_title )
{
	TABLE *table;

	if ( ! ( table = calloc( 1, sizeof ( TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	table->table_title = table_title;

	return table;
}

TABLE_COLUMN *table_column_new(
		char *column_name,
		enum table_column_type table_column_type )
{
	TABLE_COLUMN *table_column;

	if ( ! ( table_column = calloc( 1, sizeof ( TABLE_COLUMN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	table_column->column_name = column_name;
	table_column->table_column_type = table_column_type;

	return table_column;
}

TABLE_COLUMN *table_column_set(
		LIST *table_column_list,
		char *column_name,
		enum table_column_type table_column_type )
{
	TABLE_COLUMN *table_column;

	table_column =
		table_column_new(
			column_name,
			table_column_type );

	list_set( table_column_list, table_column );

	return table_column;
}

TABLE_ROW *table_row_set(
		LIST *table_row_list,
		int row_number )
{
	TABLE_ROW *table_row;

	table_row = table_row_new( row_number );
	list_set( table_row_list, table_row );

	return table_row;
}

