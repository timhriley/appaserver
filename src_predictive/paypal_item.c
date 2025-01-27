/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/paypal_item.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "paypal.h"
#include "paypal_item.h"

/* ----------- */
/* Looks like:
PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)

Shakespeare FUN-tensive PLUS!- Freshmen and Beyond! (Child: Ethan Fife), PLAY THEORY in Performance - A Kidsummer Night's Dream (Child: Ethan Fife), ADVANCED Improv for High Schoolers (Spring 2020) (Child: Ethan Fife)

PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)

Joseph Cast - Principles (Child: Henry Thomas,Ian Thomas), Chitty Chitty Bang Bang October 18th: 7pm, Chitty Chitty Bang Bang October 19th: 7pm, Chitty Chitty Bang Bang October 21st: 7pm

Mary Poppins Junior Tickets: Saturday, March 28, 7:00pm, Mary Poppins Junior Tickets: Monday, March 30, 7:00pm

Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Eli James (EJ) Crans)

*/

LIST *paypal_item_list_no_entity(
		/* ------------------- */
		/* Expect stack memory */
		/* ------------------- */
		char *item_data,
		LIST *allowed_item_list )
{
	PAYPAL_ITEM *paypal_item;
	LIST *item_list;

	if ( paypal_item_is_entity( item_data ) )
	{
		return (LIST *)0;
	}

	item_list = list_new();

	if ( !list_string_exists( item_data, allowed_item_list ) )
	{
		return (LIST *)0;
	}

	paypal_item = paypal_item_calloc();
	paypal_item->item_data = strdup( item_data );
	list_set( item_list, paypal_item );

	return item_list;
}

LIST *paypal_item_list_benefit_entity(
		/* ------------------- */
		/* Expect stack memory */
		/* ------------------- */
		char *item_title_P_piece,
		LIST *allowed_item_list )
{
	PAYPAL_ITEM *paypal_item;
	LIST *item_list;
	char item_data[ 256 ];
	char entity_data[ 256 ];
	char item_title_P_full_name[ 128 ];
	int p;

/* Sample item_title_P_piece:
The Class (Child: Atticus Weaver^Andy Madrigal Villalobos)

Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Eli James (EJ) Crans)

*/
	if ( !paypal_item_is_entity( item_title_P_piece ) )
	{
		return (LIST *)0;
	}

	item_list = list_new();

	string_rtrim(
		piece_string(
			item_data,
			"(Child: ",
			item_title_P_piece,
			0 ) );

	piece_string( entity_data, "(Child: ", item_title_P_piece, 1 );

	/* Trim closing paren */
	/* ------------------ */
	string_trim_right( entity_data, 1 );

	if ( !list_string_exists( item_data, allowed_item_list ) )
	{
		return (LIST *)0;
	}

	for(	p = 0;
		piece(	item_title_P_full_name,
			PAYPAL_ENTITY_DELIMITER,
			entity_data,
			p );
		p++ )
	{
		paypal_item = paypal_item_calloc();

		paypal_item->item_data = strdup( item_data );

		paypal_item->benefit_entity =
			entity_full_name_entity(
				/* ------------------- */
				/* Expect stack memory */
				/* ------------------- */
				item_title_P_full_name );

		list_set( item_list, paypal_item );
	}

	return item_list;
}

PAYPAL_ITEM *paypal_item_new( char *item_data )
{
	PAYPAL_ITEM *paypal_item;

	if ( !item_data )
	{
		char message[ 128 ];

		sprintf(message, "item_data is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	paypal_item = paypal_item_calloc();
	paypal_item->item_data = item_data;
	return paypal_item;
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
		char *item_title_P,
		char *transaction_type_E,
		LIST *allowed_item_list,
		LIST *event_label_list )
{
	LIST *item_list;
	char item_title_P_piece[ 1024 ];
	int p;

	if ( ( item_list =
		paypal_item_list_event(
			item_title_P,
			event_label_list ) ) )
	{
		return item_list;
	}

	item_list = list_new();

	for(	p = 0;
		piece(	item_title_P_piece,
			PAYPAL_ITEM_DELIMITER,
			item_title_P,
			p );
		p++ )
	{
		if ( !*item_title_P_piece ) continue;

		list_append_list(
			item_list,
			paypal_item_list_benefit_entity(
				item_title_P_piece,
				allowed_item_list ) );

		list_append_list(
			item_list,
			paypal_item_list_no_entity(
				item_title_P_piece,
				allowed_item_list ) );
	}

	if ( !list_length( item_list ) && transaction_type_E )
	{
		list_append_list(
			item_list,
			paypal_item_list_no_entity(
				transaction_type_E,
				allowed_item_list ) );
	}
	return item_list;
}

LIST *paypal_item_list_event(
		char *item_title_P,
		LIST *event_label_list )
{
	char *event_label;
	LIST *paypal_item_list = {0};

	if ( !list_rewind( event_label_list ) ) return (LIST *)0;

	do {
		event_label = list_get( event_label_list );

		if ( string_exists_substr(
			item_title_P,
			event_label ) )
		{
			if ( !paypal_item_list )
				paypal_item_list =
					list_new();

			list_set(
				paypal_item_list,
				paypal_item_new(
					event_label
						/* item_data */ ) );
		}

	} while ( list_next( event_label_list ) );

	return paypal_item_list;
}

double paypal_payment_item_value(
			double paypal_amount,
			double expected_revenue,
			double expected_revenue_total,
			int nonexpected_revenue_length )
{
	double item_value = 0.0;

	if ( expected_revenue )
	{
		if ( paypal_amount < expected_revenue )
			item_value = paypal_amount;
		else
			item_value = expected_revenue;
	}
	else
	{
		double remaining_cost;

		remaining_cost = paypal_amount - expected_revenue_total;

		item_value =	remaining_cost /
				(double)nonexpected_revenue_length;
	}

	return item_value;
}

double paypal_refund_item_value(
			double paypal_amount,
			double expected_revenue,
			double expected_revenue_total,
			int nonexpected_revenue_length )
{
	double item_value = 0.0;

	if ( expected_revenue )
	{
		if ( float_abs( paypal_amount ) < expected_revenue )
			item_value = paypal_amount;
		else
			item_value = 0.0 - expected_revenue;
	}
	else
	{
		double remaining_cost;

		remaining_cost =
			float_abs( paypal_amount ) -
			expected_revenue_total;

		item_value =	0.0 -
				( remaining_cost /
				(double)nonexpected_revenue_length );
	}

	return item_value;
}

double paypal_payment_item_fee(
			double paypal_amount,
			double expected_revenue,
			double transaction_fee,
			double item_value )
{
	double item_percent;
	double item_fee;

	/* Case 1: this is a refund of an enrollment */
	/* ----------------------------------------- */
	if ( expected_revenue && paypal_amount < 0.0 )
	{
		item_percent = item_value / ( 0.0 - paypal_amount );
		item_fee = transaction_fee * item_percent;
	}
	else
	/* Case 2: this is a refund */
	/* ------------------------ */
	if ( paypal_amount < 0.0 )
	{
		item_fee = transaction_fee;
	}
	else
	{
		item_percent = item_value / paypal_amount;
		item_fee = transaction_fee * item_percent;
	}
	return item_fee;
}

double paypal_refund_item_fee(
			double paypal_amount,
			double transaction_fee,
			double item_value )
{
	double item_percent;
	double item_fee;

	item_percent =
		item_value /
		paypal_amount;

	item_fee = transaction_fee * item_percent;
	return item_fee;
}

double paypal_item_gain(
			double paypal_amount,
			double expected_revenue_total,
			int nonexpected_revenue_length,
			int expected_revenue_length )
{
	double item_gain;

	if ( nonexpected_revenue_length )
	{
		item_gain = 0.0;
	}
	else
	if ( paypal_amount < expected_revenue_total )
	{
		item_gain = 0.0;
	}
	else
	{
		item_gain =
			( paypal_amount - expected_revenue_total ) /
			(double)expected_revenue_length;
	}
	return item_gain;
}

boolean paypal_item_is_entity( char *entity_piece )
{
	if ( instr(	"(Child: " /* substr */,
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

LIST *paypal_item_list_steady_state(
			LIST *paypal_item_list,
			double paypal_amount,
			double transaction_fee,
			double expected_revenue_total,
			int nonexpected_revenue_length,
			int expected_revenue_length )
{
	PAYPAL_ITEM *paypal_item;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	do {
		paypal_item = list_get( paypal_item_list );

		paypal_item =
			paypal_item_steady_state(
				paypal_item,
				paypal_item->expected_revenue,
				paypal_amount,
				transaction_fee,
				expected_revenue_total,
				nonexpected_revenue_length,
				expected_revenue_length );

	} while ( list_next( paypal_item_list ) );

	return paypal_item_list;
}

double paypal_item_expected_revenue_total(
			LIST *paypal_item_list )
{
	double expected_revenue_total;
	PAYPAL_ITEM *paypal_item;

	if ( !list_rewind( paypal_item_list ) ) return 0.0;

	expected_revenue_total = 0.0;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->expected_revenue )
		{
			expected_revenue_total += 
				paypal_item->expected_revenue;
		}

	} while ( list_next( paypal_item_list ) );

	return expected_revenue_total;
}

int paypal_item_nonexpected_revenue_length(
			LIST *paypal_item_list )
{
	int nonexpected_revenue_length;
	PAYPAL_ITEM *paypal_item;

	if ( !list_rewind( paypal_item_list ) ) return 0;

	nonexpected_revenue_length = 0;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( !paypal_item->expected_revenue )
			nonexpected_revenue_length++;

	} while ( list_next( paypal_item_list ) );

	return nonexpected_revenue_length;
}

int paypal_item_expected_revenue_length(
			LIST *paypal_item_list )
{
	return	list_length( paypal_item_list ) -
		paypal_item_nonexpected_revenue_length(
			paypal_item_list );
}

/* Looks like:
Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)
*/

char *paypal_item_entity_delimit( char *item_title_P )
{
	boolean inside_entity = 0;
	char *ptr = item_title_P;

	if ( !ptr ) return (char *)0;

	while( *ptr )
	{
		if ( string_strncmp( ptr, "(Child: " ) == 0 )
		{
			inside_entity = 1;
			ptr += 8;
			continue;
		}
		if ( *ptr == ')' )
		{
			inside_entity = 0;
			ptr++;
			continue;
		}
		if ( *ptr == PAYPAL_ITEM_DELIMITER && inside_entity )
		{
			*ptr = PAYPAL_ENTITY_DELIMITER;
		}
		ptr++;
	}
	return item_title_P;
}

PAYPAL_ITEM *paypal_item_steady_state(
			PAYPAL_ITEM *paypal_item,
			double expected_revenue,
			double paypal_amount,
			double transaction_fee,
			double expected_revenue_total,
			int nonexpected_revenue_length,
			int expected_revenue_length )
{
	if ( !paypal_amount ) return paypal_item;

	if ( paypal_amount > 0.0 )
	{
		return 
			paypal_item_payment_steady_state(
				paypal_item,
				expected_revenue,
				paypal_amount,
				transaction_fee,
				expected_revenue_total,
				nonexpected_revenue_length,
				expected_revenue_length );
	}
	else
	{
		return
			paypal_item_refund_steady_state(
				paypal_item,
				expected_revenue,
				paypal_amount,
				transaction_fee,
				expected_revenue_total,
				nonexpected_revenue_length );
	}
}

PAYPAL_ITEM *paypal_item_payment_steady_state(
			PAYPAL_ITEM *paypal_item,
			double expected_revenue,
			double paypal_amount,
			double transaction_fee,
			double expected_revenue_total,
			int nonexpected_revenue_length,
			int expected_revenue_length )
{
	paypal_item->item_value =
		paypal_payment_item_value(
			paypal_amount,
			expected_revenue,
			expected_revenue_total,
			nonexpected_revenue_length );

	paypal_item->item_fee =
		paypal_payment_item_fee(
			paypal_amount,
			expected_revenue,
			transaction_fee,
			paypal_item->item_value );

	paypal_item->item_gain =
		paypal_item_gain(
			paypal_amount,
			expected_revenue_total,
			nonexpected_revenue_length,
			expected_revenue_length );

	return paypal_item;
}

PAYPAL_ITEM *paypal_item_refund_steady_state(
			PAYPAL_ITEM *paypal_item,
			double expected_revenue,
			double paypal_amount,
			double transaction_fee,
			double expected_revenue_total,
			int nonexpected_revenue_length )
{
	paypal_item->item_value =
		paypal_refund_item_value(
			paypal_amount,
			expected_revenue,
			expected_revenue_total,
			nonexpected_revenue_length );

	paypal_item->item_fee =
		paypal_refund_item_fee(
			paypal_amount,
			transaction_fee,
			paypal_item->item_value );

	return paypal_item;
}

