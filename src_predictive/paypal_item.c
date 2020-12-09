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
#include "entity.h"
#include "program.h"
#include "product.h"
#include "tuition_payment_fns.h"
#include "item_title.h"

ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
	item_title_tuition_payment_enrollment_calloc(
			void )
{
	ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *p;

	if ( ! ( p =
		calloc(	1,
			sizeof( ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return p;
}

ITEM_TITLE_TUITION_PAYMENT *item_title_tuition_payment_calloc(
			void )
{
	ITEM_TITLE_TUITION_PAYMENT *p;

	if ( ! ( p = calloc( 1, sizeof( ITEM_TITLE_TUITION_PAYMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return p;
}

ITEM_TITLE_TUITION_PAYMENT *item_title_tuition_payment_new(
			char *item_title_P,
			int student_number )
{
	ITEM_TITLE_TUITION_PAYMENT *item_title_tuition_payment;
	ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *enrollment;

	if ( ( enrollment =
		item_title_tuition_payment_enrollment_new(
			item_title_P,
			student_number ) ) )
	{
		item_title_tuition_payment =
			item_title_tuition_payment_calloc();

		item_title_tuition_payment->item_title_enrollment = enrollment;
		return item_title_tuition_payment;
	}
	else
	{
		return (ITEM_TITLE_TUITION_PAYMENT *)0;
	}
}

/* ------------------- */
/* Sample item_title_P */
/* ------------------- */
/*

PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)

*/

ENTITY *item_title_tuition_payment_entity(
		ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *item_title_enrollment )
{
	ENTITY *student_entity;
	char *street_address;

	if ( !item_title_enrollment ) return (ENTITY *)0;

	if ( ( street_address =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			entity_street_address(
				item_title_enrollment->student_full_name ) ) )
	{
		student_entity =
			entity_new(
				item_title_enrollment->student_full_name,
				street_address );
	}
	else
	{
		student_entity =
			entity_new(
				item_title_enrollment->student_full_name,
				ENTITY_STREET_ADDRESS_UNKNOWN );
	}
	return student_entity;
}

char *item_title_tuition_payment_course_name(
		ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *item_title_enrollment )
{
	if ( !item_title_enrollment ) return (char *)0;

	return item_title_enrollment->course_name;
}

ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
	item_title_tuition_payment_enrollment_new(
			char *item_title_P,
			int student_number )
{
	ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *item_title_enrollment = {0};

/* item_title_P looks like:
Shakespeare FUN-tensive PLUS!- Freshmen and Beyond! (Child: Ethan Fife), PLAY THEORY in Performance - A Kidsummer Night's Dream (Child: Ethan Fife), ADVANCED Improv for High Schoolers (Spring 2020) (Child: Ethan Fife)

or
Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)
*/

	if ( !tuition_payment_is_tuition( item_title_P ) )
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;

	item_title_enrollment =
		item_title_tuition_payment_enrollment_block_A(
			item_title_P,
			student_number );

	if ( !item_title_enrollment )
	{
		item_title_enrollment =
			item_title_tuition_payment_enrollment_block_B(
				item_title_P,
				student_number );
	}

	return item_title_enrollment;
}

/* item_title_P for block A looks like:
PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)
*/
ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
	item_title_tuition_payment_enrollment_block_A(
			char *item_title_P,
			int student_number )
{
	LIST *block_list;
	char *block;
	char course_name[ 256 ];
	char student_full_name[ 256 ];
	ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *item_title_enrollment;

	block_list =
		list_string_list(
			item_title_P /* list_string */,
			',' /* delimiter */ );

	if ( ! ( block = list_seek_offset( block_list, student_number - 1 ) ) )
	{
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;
	}

	if ( !string_character_exists( block, ':' ) )
	{
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;
	}

	/* Build course_name */
	/* ----------------- */
	piece( course_name, ':', block, 0 );

	if ( strlen( course_name ) < 8 )
	{
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;
	}

	/* Zap <sp>(Child */
	/* -------------- */
	*( course_name + strlen( course_name ) - 7 ) = '\0';

	/* Build student_full_name */
	/* ----------------------- */
	piece( student_full_name, ':', block, 1 );

	/* If block B */
	/* ---------- */
	if ( string_character_exists( student_full_name, ',' ) )
	{
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;
	}

	/* Trim the trailing ')' */
	/* --------------------- */
	string_trim_right( student_full_name, 1 );

	item_title_enrollment =
		item_title_tuition_payment_enrollment_calloc();

	item_title_enrollment->course_name = strdup( course_name );
	item_title_enrollment->student_full_name = strdup( student_full_name );

	return item_title_enrollment;
}

/* item_title_P for block B looks like:
Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)
*/
ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *
	item_title_tuition_payment_enrollment_block_B(
			char *item_title_P,
			int student_number )
{
	char course_name[ 256 ];
	char student_full_name_block[ 256 ];
	char student_full_name[ 128 ];
	ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *item_title_enrollment;

	if ( !string_character_exists( student_full_name, ':' ) )
	{
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;
	}

	/* Build course_name */
	/* ----------------- */
	piece( course_name, ':', item_title_P, 0 );

	if ( strlen( course_name ) < 8 )
	{
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;
	}

	/* Zap <sp>(Child */
	/* -------------- */
	*( course_name + strlen( course_name ) - 7 ) = '\0';

	/* Build student_full_name */
	/* ----------------------- */
	piece( student_full_name_block, ':', item_title_P, 1 );

	if ( !piece(	student_full_name,
			',',
			item_title_P,
			student_number - 1 ) )
	{
		return (ITEM_TITLE_TUITION_PAYMENT_ENROLLMENT *)0;
	}

	string_trim_character( student_full_name, ')' );

	item_title_enrollment =
		item_title_tuition_payment_enrollment_calloc();

	item_title_enrollment->course_name = strdup( course_name );
	item_title_enrollment->student_full_name = strdup( student_full_name );

	return item_title_enrollment;
}

/* ------------------- */
/* Sample item_title_P */
/* ------------------- */
/*

Mary Poppins Junior Tickets: Saturday, March 28, 7:00pm, Mary Poppins Junior Tickets: Monday, March 30, 7:00pm

*/

char *item_title_program_payment_block(
			char *item_title_P,
			char *transaction_type_E,
			int program_number )
{
	char program_block[ 512 ];

/* Only doing list of 1 for now. */
/* ----------------------------- */
if ( program_number > 1 ) return (char *)0;

	if ( !piece(	program_block,
			',',
			item_title_P,
			program_number - 1 ) )
	{
		return (char *)0;
	}

	if ( !*program_block )
	{
		if ( !transaction_type_E
		||   !piece(	program_block,
				',',
				transaction_type_E,
				program_number - 1 ) )
		{
			return (char *)0;
		}
	}

	return strdup( program_block );
}

char *item_title_program_payment_name(
			char *item_title_P,
			char *transaction_type_E,
			int program_number,
			LIST *program_list )
{
	char *item_title_block;
	PROGRAM *program;

	if ( tuition_payment_is_tuition( item_title_P ) )
	{
		return (char *)0;
	}

/* Program item title block like:

Mary Poppins Junior Tickets

*/
	if ( ! ( item_title_block =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			item_title_program_payment_block(
				item_title_P,
				transaction_type_E,
				program_number ) ) )
	{
		return (char *)0;
	}

	if ( ! ( program = 
			program_seek_name(
				item_title_block
					/* program_name */,
				program_list ) ) )
	{
		return (char *)0;
	}
	return program->program_name;
}

char *item_title_product_payment_name(
			char *item_title_P,
			int program_number,
			LIST *product_list )
{
	char *item_title_block;

	if ( tuition_payment_is_tuition( item_title_P ) )
	{
		return (char *)0;
	}

/* Product item title block like:

Mary Poppins Junior DVD

*/

	if ( ! ( item_title_block =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			item_title_program_payment_block(
				item_title_P,
				(char *)0 /* transaction_type_E */,
				program_number ) ) )
	{
		return (char *)0;
	}

	return
		product_seek_name(
			item_title_block /* product_name */,
			product_list );
}

