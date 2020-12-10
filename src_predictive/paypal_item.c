/* ---------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_item.c	*/
/* ---------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "paypal.h"
#include "paypal_item.h"

/* ------------------- */
/* Sample item_title_P */
/* ------------------- */

/*
PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)

Shakespeare FUN-tensive PLUS!- Freshmen and Beyond! (Child: Ethan Fife), PLAY THEORY in Performance - A Kidsummer Night's Dream (Child: Ethan Fife), ADVANCED Improv for High Schoolers (Spring 2020) (Child: Ethan Fife)

PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)

Mary Poppins Junior Tickets: Saturday, March 28, 7:00pm, Mary Poppins Junior Tickets: Monday, March 30, 7:00pm

Joseph Cast - Principles (Child: Henry Thomas,Ian Thomas), Chitty Chitty Bang Bang October 18th: 7pm, Chitty Chitty Bang Bang October 19th: 7pm, Chitty Chitty Bang Bang October 21st: 7pm

*/

/* Returns list of 1 */
/* ----------------- */
LIST *paypal_nonentity_item_list(
			LIST *not_found_item_list,
			char *item_data,
			LIST *allowed_item_list )
{
	PAYPAL_ITEM *paypal_item;
	LIST *item_list;

	if ( string_character_exists( item_data, '(' ) )
	{
		return (LIST *)0;
	}

/* Sample entity_piece:
Child: Atticus Weaver,Andy Madrigal Villalobos)
*/
	if ( !paypal_item_is_entity( item_data ) )
	{
		return (LIST *)0;
	}

	item_list = list_new();

	if ( !list_string_exists( item_data, allowed_item_list ) )
	{
		if ( not_found_item_list )
		{
			list_set( not_found_item_list, strdup( item_data ) );
		}
		return (LIST *)0;
	}

	paypal_item = paypal_item_calloc();
	paypal_item->item_data = strdup( item_data );
	list_set( item_list, paypal_item );

	return item_list;
}

LIST *paypal_entity_item_list(
			LIST *not_found_item_list,
			char *item_title_P_piece,
			LIST *allowed_item_list )
{
	PAYPAL_ITEM *paypal_item;
	LIST *item_list;
	char item_data[ 256 ];
	char entity_piece[ 256 ];
	char full_name[ 128 ];
	int p;

	if ( !string_character_exists( item_title_P_piece, '(' ) )
	{
		return (LIST *)0;
	}

	item_list = list_new();

	string_rtrim( piece( item_data, '(', item_title_P_piece, 0 ) );

	if ( !list_string_exists( item_data, allowed_item_list ) )
	{
		if ( not_found_item_list )
		{
			list_set( not_found_item_list, strdup( item_data ) );
		}
		return (LIST *)0;
	}

	piece( entity_piece, '(', item_title_P_piece, 1 );

/* Sample entity_piece:
Child: Atticus Weaver,Andy Madrigal Villalobos)
*/
	if ( !paypal_item_is_entity( entity_piece ) )
	{
		return (LIST *)0;
	}

	/* Zap Child:<sp> */
	/* -------------- */
	string_strcpy( entity_piece, entity_piece + 7, 0 );

	/* Zap closing paren */
	/* ----------------- */
	string_trim_right( entity_piece, 1 );

	for(	p = 0;
		piece( full_name, PAYPAL_ENTITY_DELIMITER, entity_piece, p );
		p++ )
	{
		paypal_item = paypal_item_calloc();

		paypal_item->item_data = strdup( item_data );

		paypal_item->entity = paypal_entity( full_name );

		list_set( item_list, paypal_item );
	}
	return item_list;
}

ENTITY *paypal_entity( char *full_name )
{
	return entity_full_name_entity( full_name );
}

PAYPAL_ITEM *paypal_item_calloc( void )
{
	PAYPAL_ITEM *paypal_item;

	if ( ! ( paypal_item = calloc( 1, sizeof( PAYPAL_ITEM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return paypal_item;
}

LIST *paypal_item_list(
			LIST *not_found_item_list,
			char *entity_delimited_date_removed,
			char *transaction_type_E,
			LIST *allowed_item_list )
{
	LIST *item_list = list_new();
	char item_title_P_piece[ 1024 ];
	int p;

	for(	p = 0;
		piece(	item_title_P_piece,
			PAYPAL_ITEM_DELIMITER,
			entity_delimited_date_removed,
			p );
		p++ )
	{
		if ( !*item_title_P_piece ) continue;

		list_append_list(
			item_list,
			paypal_entity_item_list(
				not_found_item_list,
				item_title_P_piece,
				allowed_item_list ) );

		list_append_list(
			item_list,
			paypal_nonentity_item_list(
				not_found_item_list,
				item_title_P_piece,
				allowed_item_list ) );
	}

	if ( !list_length( item_list ) && transaction_type_E )
	{
		list_append_list(
			item_list,
			paypal_nonentity_item_list(
				(LIST *)0 /* not_found_item_list */,
				transaction_type_E,
				allowed_item_list ) );
	}
	return item_list;
}

double paypal_item_value(
			double expected_revenue,
			double deposit_amount,
			double expected_revenue_total,
			double nonexpected_list_length )
{
	return 0.0;
}

double paypal_item_fee(
			double deposit_amount,
			double transaction_fee,
			double paypal_item_value )
{
	return 0.0;
}

double paypal_gain_donation(
			double expected_revenue,
			double deposit_amount,
			double expected_revenue_total,
			double nonexpected_list_length )
{
	return 0.0;
}

boolean paypal_item_is_entity(
			char *entity_piece )
{
	if ( instr(	"Child: " /* substr */,
			entity_piece /* string */,
			1 /* occurrence */ ) >= 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

