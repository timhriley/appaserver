/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/tuition_payment_item_title.c	*/
/* -----------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

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
#include "tuition_payment_fns.h"
#include "tuition_payment_item_title.h"

TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
	tuition_payment_item_title_enrollment_calloc(
			void )
{
	TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *p;

	if ( ! ( p =
		calloc(	1,
			sizeof( TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT ) ) ) )
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

TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title_calloc(
			void )
{
	TUITION_PAYMENT_ITEM_TITLE *p;

	if ( ! ( p = calloc( 1, sizeof( TUITION_PAYMENT_ITEM_TITLE ) ) ) )
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

TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title_new(
			char *item_title_P,
			int student_number )
{
	TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title;
	TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *enrollment;

	if ( ( enrollment =
		tuition_payment_item_title_enrollment_new(
			item_title_P,
			student_number ) ) )
	{
		tuition_payment_item_title =
			tuition_payment_item_title_calloc();

		tuition_payment_item_title->item_title_enrollment = enrollment;
		return tuition_payment_item_title;
	}
	else
	{
		return (TUITION_PAYMENT_ITEM_TITLE *)0;
	}
}

/* ------------------- */
/* Sample item_title_P */
/* ------------------- */
/*

PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)

*/

ENTITY *tuition_payment_item_title_entity(
		TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *item_title_enrollment )
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

char *tuition_payment_item_title_course_name(
		TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *item_title_enrollment )
{
	if ( !item_title_enrollment ) return (char *)0;

	return item_title_enrollment->course_name;
}

TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
	tuition_payment_item_title_enrollment_new(
			char *item_title_P,
			int student_number )
{
	TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *item_title_enrollment = {0};

/* item_title_P looks like:
Shakespeare FUN-tensive PLUS!- Freshmen and Beyond! (Child: Ethan Fife), PLAY THEORY in Performance - A Kidsummer Night's Dream (Child: Ethan Fife), ADVANCED Improv for High Schoolers (Spring 2020) (Child: Ethan Fife)

or
Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)
*/

	if ( !tuition_payment_is_tuition( item_title_P ) )
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;

	item_title_enrollment =
		tuition_payment_item_title_enrollment_block_A(
			item_title_P,
			student_number );

	if ( !item_title_enrollment )
	{
		item_title_enrollment =
			tuition_payment_item_title_enrollment_block_B(
				item_title_P,
				student_number );
	}

	return item_title_enrollment;
}

/* item_title_P for block A looks like:
PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)
*/
TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
	tuition_payment_item_title_enrollment_block_A(
			char *item_title_P,
			int student_number )
{
	LIST *block_list;
	char *block;
	char course_name[ 256 ];
	char student_full_name[ 256 ];
	TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *item_title_enrollment;

	block_list =
		list_string_list(
			item_title_P /* list_string */,
			',' /* delimiter */ );

	if ( ! ( block = list_seek_offset( block_list, student_number - 1 ) ) )
	{
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;
	}

	if ( !string_character_exists( block, ':' ) )
	{
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;
	}

	/* Build course_name */
	/* ----------------- */
	piece( course_name, ':', block, 0 );

	if ( strlen( course_name ) < 8 )
	{
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;
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
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;
	}

	/* Trim the trailing ')' */
	/* --------------------- */
	string_trim_right( student_full_name, 1 );

	item_title_enrollment =
		tuition_payment_item_title_enrollment_calloc();

	item_title_enrollment->course_name = strdup( course_name );
	item_title_enrollment->student_full_name = strdup( student_full_name );

	return item_title_enrollment;
}

/* item_title_P for block B looks like:
Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)
*/
TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *
	tuition_payment_item_title_enrollment_block_B(
			char *item_title_P,
			int student_number )
{
	char course_name[ 256 ];
	char student_full_name_block[ 256 ];
	char student_full_name[ 128 ];
	TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *item_title_enrollment;

	if ( !string_character_exists( student_full_name, ':' ) )
	{
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;
	}

	/* Build course_name */
	/* ----------------- */
	piece( course_name, ':', item_title_P, 0 );

	if ( strlen( course_name ) < 8 )
	{
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;
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
		return (TUITION_PAYMENT_ITEM_TITLE_ENROLLMENT *)0;
	}

	string_trim_character( student_full_name, ')' );

	item_title_enrollment =
		tuition_payment_item_title_enrollment_calloc();

	item_title_enrollment->course_name = strdup( course_name );
	item_title_enrollment->student_full_name = strdup( student_full_name );

	return item_title_enrollment;
}

