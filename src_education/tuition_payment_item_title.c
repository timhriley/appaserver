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
#include "tuition_payment_item_title.h"

TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title_new(
			char *item_title_P,
			int student_number )
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

	p->item_title_P = item_title_P;
	p->student_number = student_number;
	return p;
}

/* ------------------- */
/* Sample item_title_P */
/* ------------------- */
/*

Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Eli James (EJ) Crans)

PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Macie Postak), Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Blake Postak)

*/

char *tuition_payment_item_title_enrollment_block(
			char *item_title_P,
			int student_number )
{
	static char enrollment_block[ 512 ];

	if ( !program_payment_is_tuition( item_title_P ) )
		return (char *)0;

	if ( !piece( enrollment_block, ',', item_title_P, student_number - 1 ) )
	{
		return (char *)0;
	}
	return enrollment_block;
}

ENTITY *tuition_payment_item_title_entity(
			char *item_title_P,
			int student_number )
{
	ENTITY *student_entity;
	char student_full_name[ 128 ];
	char *street_address;
	char *enrollment_block;

	if ( !program_payment_is_tuition( item_title_P ) )
		return (ENTITY *)0;

/* Enrollment_block looks like:
Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Eli James (EJ) Crans)
*/
	if ( ! ( enrollment_block =
			tuition_payment_item_title_enrollment_block(
				item_title_P,
				student_number ) ) )
	{
		return (ENTITY *)0;
	}

	if ( !piece( student_full_name, ':', enrollment_block, 1 ) )
	{
		return (ENTITY *)0;
	}

	/* Zap the closing paren */
	/* --------------------- */
	*( student_full_name + strlen( student_full_name ) - 1 ) = '\0';

	if ( ( street_address =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			entity_street_address(
				student_full_name ) ) )
	{
		student_entity =
			entity_new(
				strdup( student_full_name ),
				street_address );
	}
	else
	{
		student_entity =
			entity_new(
				strdup( student_full_name ),
				ENTITY_STREET_ADDRESS_UNKNOWN );
	}
	return student_entity;
}

char *tuition_payment_item_title_course_name(
			char *item_title_P,
			int student_number )
{
	char course_name[ 1024 ];
	char *enrollment_block;

	if ( !program_payment_is_tuition( item_title_P ) )
		return (char *)0;

	if ( ! ( enrollment_block =
			tuition_payment_item_title_enrollment_block(
				item_title_P,
				student_number ) ) )
	{
		return (char *)0;
	}

/* Enrollment_block looks like:
Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Eli James (EJ) Crans)
*/
	if ( !piece( course_name, ':', enrollment_block, 0 ) )
	{
		return (char *)0;
	}

	if ( strlen( course_name ) < 8 )
	{
		return (char *)0;
	}

	/* Zap the trailing characters */
	/* --------------------------- */
	*( course_name + strlen( course_name ) - 7 ) = '\0';

	return strdup( course_name );
}
