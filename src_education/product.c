/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "product.h"

char *product_primary_where( char *product_name )
{
	char static where[ 128 ];

	sprintf( where,
		 "product_name = '%s'",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 product_name_escape( product_name ) );

	return where;
}

char *product_name_escape( char *product_name )
{
	static char name[ 256 ];

	return string_escape_quote( name, product_name );
}

PRODUCT *product_calloc( void )
{
	PRODUCT *product;

	if ( ! ( product = calloc( 1, sizeof( PRODUCT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return product;
}

PRODUCT *product_new( char *product_name )
{
	PRODUCT *product = product_calloc();

	product->product_name = product_name;
	return product;
}

char *product_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		PRODUCT_TABLE,
		where );

	return strdup( sys_string );
}

PRODUCT *product_fetch(	char *product_name,
			boolean fetch_program )
{
	if ( !product_name || !*product_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty product_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
		product_parse(
			pipe2string(
				product_sys_string(
		 			/* --------------------- */
		 			/* Returns static memory */
		 			/* --------------------- */
		 			product_primary_where(
						product_name ) ) ),
			fetch_program );
}

PRODUCT *product_parse( char *input,
			boolean fetch_program )
{
	char product_name[ 128 ];
	char program_name[ 128 ];
	char revenue_account[ 128 ];
	char retail_price[ 128 ];
	char sale_total[ 128 ];
	char refund_total[ 128 ];
	PRODUCT *product;

	if ( !input || !*input ) return (PRODUCT *)0;

	/* See: attribute_list */
	/* ------------------- */
	piece( product_name, SQL_DELIMITER, input, 0 );

	product =
		product_new(
			strdup( product_name ) );

	piece( program_name, SQL_DELIMITER, input, 1 );
	product->program_name = strdup( program_name );

	piece( revenue_account, SQL_DELIMITER, input, 2 );
	product->revenue_account = strdup( revenue_account );

	piece( retail_price, SQL_DELIMITER, input, 3 );
	product->retail_price = atof( retail_price );

	piece( sale_total, SQL_DELIMITER, input, 3 );
	product->sale_total = atof( sale_total );

	piece( refund_total, SQL_DELIMITER, input, 3 );
	product->refund_total = atof( refund_total );

	if ( fetch_program )
	{
		product->program =
			program_fetch(
				product->program_name,
				0 /* not fetch_alias_list */ );
	}

	return product;
}

LIST *product_list( void )
{
	return product_system_list(
			product_sys_string(
				"1 = 1" /* where */ ),
			0 /* not fetch_program */ );
}

LIST *product_system_list(
			char *sys_string,
			boolean fetch_program )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *product_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			product_list,
			product_parse(
				input,
				fetch_program ) );
	}

	pclose( input_pipe );
	return product_list;
}

PRODUCT *product_list_seek(
			char *product_name,
			LIST *product_list )
{
	PRODUCT *product;

	if ( !list_rewind( product_list ) ) return (PRODUCT *)0;

	do {
		product =
			list_get(
				product_list );

		if ( string_strcmp(	product->product_name,
					product_name ) == 0 )
		{
			return product;
		}
	} while ( list_next( product_list ) );

	return (PRODUCT *)0;
}

LIST *product_name_list( LIST *product_list )
{
	LIST *name_list;
	PRODUCT *product;

	if ( !list_rewind( product_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		product = list_get( product_list );

		list_set( name_list, product->product_name );

	} while ( list_next( product_list ) );

	return name_list;
}

void product_fetch_update( char *product_name )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"product_sale_total.sh \"%s\"",
		product_name_escape( product_name ) );

	if ( system( sys_string ) ){};

	sprintf(sys_string,
		"product_refund_total.sh \"%s\"",
		product_name_escape( product_name ) );

	if ( system( sys_string ) ){};
}

void product_list_fetch_update(
			LIST *product_name_list )
{
	if ( !list_rewind( product_name_list ) ) return;

	do {
		product_fetch_update( list_get( product_name_list ) );

	} while ( list_next( product_name_list ) );
}

