/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/form_field.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
#include "post.h"
#include "appaserver_error.h"
#include "form_field.h"

FORM_FIELD_DATUM *form_field_datum_new(
		char *field_name,
		char *field_datum,
		char *message_datum,
		int primary_key_index )
{
	FORM_FIELD_DATUM *form_field_datum;

	if ( !field_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: field_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_field_datum = form_field_datum_calloc();

	form_field_datum->field_name = field_name;

	form_field_datum->field_datum =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		form_field_datum_field(
			field_datum,
			message_datum );

	if ( !form_field_datum->field_datum )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_field_datum() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_field_datum->insert_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		insert_datum_new(
			field_name /* attribute_name */,
			form_field_datum->field_datum /* insert_datum */,
			primary_key_index,
			0 /* not attribute_is_number */ );

	return form_field_datum;
}

FORM_FIELD_DATUM *form_field_datum_calloc( void )
{
	FORM_FIELD_DATUM *form_field_datum;

	if ( ! ( form_field_datum =
			calloc( 1,
				sizeof ( FORM_FIELD_DATUM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_field_datum;
}

char *form_field_datum_field(
		char *field_datum,
		char *message_datum )
{
	if ( field_datum )
		return field_datum;
	else
		return message_datum;
}

LIST *form_field_datum_insert_list( LIST *form_field_datum_list )
{
	LIST *datum_insert_list = list_new();
	FORM_FIELD_DATUM *form_field_datum;

	if ( list_rewind( form_field_datum_list ) )
	do {
		form_field_datum = list_get( form_field_datum_list );

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: insert_datum=[%x,%s,%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
(unsigned int)(long)form_field_datum->insert_datum,
form_field_datum->insert_datum->attribute_name,
form_field_datum->insert_datum->datum );
msg( (char *)0, message );
}
		list_set(
			datum_insert_list,
			form_field_datum->insert_datum );

	} while ( list_next( form_field_datum_list ) );

	if ( !list_length( datum_insert_list ) )
	{
		list_free( datum_insert_list );
		datum_insert_list = NULL;
	}

	return datum_insert_list;
}

LIST *form_field_datum_fetch_list(
		char *email_address,
		char *timestamp )
{
	LIST *list = list_new();
	char *primary_where;
	char *system_string;
	FILE *input_pipe;
	char input[ STRING_65K ];
	FORM_FIELD_DATUM *form_field_datum;

	if ( !email_address
	||   !timestamp )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_primary_where(
			email_address,
			timestamp );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			FORM_FIELD_DATUM_SELECT,
			FORM_FIELD_DATUM_TABLE,
			primary_where );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		form_field_datum =
			form_field_datum_parse(
				input );

		list_set( list, form_field_datum );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

FORM_FIELD_DATUM *form_field_datum_parse( char *string_input )
{
	FORM_FIELD_DATUM *form_field_datum;
	char buffer[ STRING_64K ];

	if ( !string_input ) return NULL;

	form_field_datum = form_field_datum_calloc();

	if ( !string_input ) return NULL;

	/* See FORM_FIELD_DATUM_SELECT */
	/* --------------------------- */
	piece( buffer, SQL_DELIMITER, string_input, 0 );
	if ( *buffer )
		form_field_datum->form_name =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_input, 1 );
	if ( *buffer )
		form_field_datum->field_name =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_input, 2 );
	if ( *buffer )
		form_field_datum->field_datum =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_input, 3 );
	if ( *buffer )
		form_field_datum->message_datum =
			strdup( buffer );

	return form_field_datum;
}

FORM_FIELD_DATUM *form_field_datum_seek(
		LIST *form_field_datum_list,
		char *field_name )
{
	FORM_FIELD_DATUM *form_field_datum;

	if ( !field_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: field_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( form_field_datum_list ) )
	do {
		form_field_datum =
			list_get(
				form_field_datum_list );

		if ( string_strcmp(
			form_field_datum->field_name,
			field_name ) == 0 )
		{
			return form_field_datum;
		}

	} while ( list_next( form_field_datum_list ) );

	return NULL;
}

char *form_field_datum_insert_sql_statement(
		const char *form_field_datum_table,
		LIST *form_field_datum_list )
{
	LIST *insert_list;
	char *sql_statement;

	insert_list =
		form_field_datum_insert_list(
			form_field_datum_list );

	sql_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_sql_statement(
			(char *)form_field_datum_table /* table_name */,
			insert_list /* insert_datum_list */ );

	return sql_statement;
}

LIST *form_field_datum_insert_statement_list(
		const char *form_field_datum_table,
		LIST *form_field_insert_list )
{
	LIST *insert_statement_list = list_new();
	FORM_FIELD_INSERT *form_field_insert;
	LIST *datum_insert_list;
	char *sql_statement;

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: should generate %d insert statements, only generating one.\n",
__FILE__,
__FUNCTION__,
__LINE__,
list_length( form_field_insert_list ) );
msg( (char *)0, message );
}
	if ( list_rewind( form_field_insert_list ) )
	do {
		form_field_insert = list_get( form_field_insert_list );

		datum_insert_list =
			form_field_datum_insert_list(
				form_field_insert->form_field_datum_list );

		if ( !list_length( datum_insert_list ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: form_field_datum_insert_list() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: list_length(datum_insert_list)=%d\n",
__FILE__,
__FUNCTION__,
__LINE__,
list_length( datum_insert_list ) );
msg( (char *)0, message );
}
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_field_datum_insert_sql_statement(
				form_field_datum_table /* table_name */,
				datum_insert_list
					/* insert_datum_list */ );

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: sql_statement=[%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
sql_statement );
msg( (char *)0, message );
}
		list_set(
			insert_statement_list,
			sql_statement );

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: BREAKING OUT OF LOOP.\n",
__FILE__,
__FUNCTION__,
__LINE__ );
msg( (char *)0, message );
}
break;
	} while ( list_next( form_field_insert_list ) );

	if ( !list_length( insert_statement_list ) )
	{
		list_free( insert_statement_list );
		insert_statement_list = NULL;
	}

	return insert_statement_list;
}

FORM_FIELD_INSERT *form_field_insert_new(
		char *email_address,
		char *form_name,
		char *timestamp,
		LIST *form_field_datum_list )
{
	FORM_FIELD_INSERT *form_field_insert;
	FORM_FIELD_DATUM *form_field_datum;

	if ( !email_address
	||   !form_name
	||   !timestamp
	||   !list_length( form_field_datum_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: BEGINNING form_field_datum_list=[%x,length=%d]\n",
__FILE__,
__FUNCTION__,
__LINE__,
(unsigned int)(long)form_field_datum_list,
list_length( form_field_datum_list ) );
msg( (char *)0, message );
}
	form_field_insert = form_field_insert_calloc();
	form_field_insert->form_field_datum_list = form_field_datum_list;

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"timestamp" /* field_name */,
			timestamp /* field_datum */,
			(char *)0 /* message_datum */,
			1 /* primary_key_index */ );

	list_set(
		form_field_datum_list,
		form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"email_address" /* field_name */,
			email_address /* field_datum */,
			(char *)0 /* message_datum */,
			2 /* primary_key_index */ );

	list_set(
		form_field_datum_list,
		form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"form_name" /* field_name */,
			form_name /* field_datum */,
			(char *)0 /* message_datum */,
			3 /* primary_key_index */ );

	list_set(
		form_field_datum_list,
		form_field_datum );

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: ENDING form_field_datum_list=[%x,length=%d]\n",
__FILE__,
__FUNCTION__,
__LINE__,
(unsigned int)(long)form_field_datum_list,
list_length( form_field_datum_list ) );
msg( (char *)0, message );
}
	return form_field_insert;
}

FORM_FIELD_INSERT *form_field_insert_calloc( void )
{
	FORM_FIELD_INSERT *form_field_insert;

	if ( ! ( form_field_insert =
			calloc( 1,
				sizeof ( FORM_FIELD_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_field_insert;
}

FORM_FIELD_INSERT_LIST *form_field_insert_list_new( void )
{
	FORM_FIELD_INSERT_LIST *form_field_insert_list =
		form_field_insert_list_calloc();

	form_field_insert_list->list = list_new();

	return form_field_insert_list;
}

FORM_FIELD_INSERT_LIST *form_field_insert_list_calloc( void )
{
	FORM_FIELD_INSERT_LIST *form_field_insert_list;

	if ( ! ( form_field_insert_list =
			calloc( 1,
				sizeof ( FORM_FIELD_INSERT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_field_insert_list;
}

