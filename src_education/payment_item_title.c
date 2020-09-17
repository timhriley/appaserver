/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/payment_item_title.c	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "entity.h"
#include "payment_item_title.h"

/* Sample item_title_P */
/* ------------------- */
/*

Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Eli James (EJ) Crans)

PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Camille Pojda), PLAY THEORY in Performance -- Mary Poppins Jr. EXTENDED CAST (Child: Allison Pojda)

Play Theory in Improv for Junior High Schoolers (Spring 2020) (Child: Macie Postak), Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Blake Postak)

*/

ENTITY *payment_item_title_entity(
			char *item_title_P,
			int student_number )
{
	return (ENTITY *)0;
}

char *payment_item_title_course_name(
			char *item_title_P,
			int student_number )
{
	return (char *)0;
}

PAYMENT_ITEM_TITLE *payment_item_title_new(
			char *item_title_P,
			int student_number )
{
	PAYMENT_ITEM_TITLE *p;

	if ( ! ( p = calloc( 1, sizeof( PAYMENT_ITEM_TITLE ) ) ) )
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

