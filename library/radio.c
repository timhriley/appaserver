/* ------------------------------------------------------- */
/* $APPASERVER_HOME/library/radio.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "radio.h"

RADIO_VALUE *radio_value_new(
			char *radio_list_name,
			RADIO_PAIR *radio_pair,
			char *initial_label )
{
	RADIO_VALUE *radio_value = radio_value_calloc();

	if ( !radio_pair )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: radio_pair is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	radio_value->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		radio_value_html(
			radio_list_name,
			radio_pair->radio_name,
			radio_pair->radio_label,
			initial_label );

	return radio_value;
}

char *radio_value_html(
			char *radio_list_name,
			char *radio_name,
			char *radio_label,
			char *initial_label )
{
	char html[ 256 ];
	char *ptr = html;
	char buffer[ 128 ];

	if ( !radio_list_name || !radio_name || !radio_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<td><input name=%s type=radio value=%s class=%s",
		radio_list_name,
		radio_name,
		RADIO_CLASS );

	if ( string_strcmp( radio_label, initial_label ) == 0 )
	{
		ptr += sprintf( ptr, " checked" );
	}

	ptr += sprintf(
		ptr,
		">%s",
		string_initial_capital(
			buffer,
			radio_label ) );

	return strdup( html );
}

RADIO_VALUE *radio_value_calloc( void )
{
	RADIO_VALUE *radio_value;

	if ( ! ( radio_value = calloc( 1, sizeof( RADIO_VALUE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return radio_value;
}

RADIO_LIST *radio_list_new(
			char *form_radio_list_name,
			LIST *radio_pair_list,
			char *initial_label )
{
	RADIO_LIST *radio_list = radio_list_calloc();
	RADIO_PAIR *radio_pair;

	if ( !form_radio_list_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: form_radio_list_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( radio_pair_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: radio_pair_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	radio_list->radio_value_list = list_new();

	do {
		radio_pair = list_get( radio_pair_list );

		list_set(
			radio_list->radio_value_list,
			radio_value_new(
				form_radio_list_name,
				radio_pair,
				initial_label ) );

	} while ( list_next( radio_pair_list ) );

	radio_list->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		radio_list_html(
			radio_list->radio_value_list );

	return radio_list;
}

char *radio_list_html( LIST *radio_value_list )
{
	char html[ 2048 ];
	char *ptr = html;
	RADIO_VALUE *radio_value;

	ptr += sprintf(
		ptr,
		"<table border=1><tr>" );

	if ( !list_rewind( radio_value_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: radio_value_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		radio_value = list_get( radio_value_list );

		ptr += sprintf(
			ptr,
			"\n%s",
			radio_value->html );

		radio_value_free( radio_value );

	} while ( list_next( radio_value_list ) );

	ptr += sprintf( ptr, "\n</table>" );

	return strdup( html );
}

RADIO_LIST *radio_list_calloc( void )
{
	RADIO_LIST *radio_list;

	if ( ! ( radio_list = calloc( 1, sizeof( RADIO_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return radio_list;
}

void radio_value_free( RADIO_VALUE *radio_value )
{
	free( radio_value->html );
	free( radio_value );
}

RADIO_PAIR *radio_pair_new(
			char *radio_name,
			char *radio_label )
{
	RADIO_PAIR *radio_pair = radio_pair_calloc();

	radio_pair->radio_name = radio_name;
	radio_pair->radio_label = radio_label;

	return radio_pair;
}

RADIO_PAIR *radio_pair_calloc( void )
{
	RADIO_PAIR *radio_pair;

	if ( ! ( radio_pair = calloc( 1, sizeof( RADIO_PAIR ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return radio_pair;
}

