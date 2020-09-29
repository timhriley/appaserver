/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/program_payment_item_title.c	*/
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
#include "program_payment_item_title.h"

PROGRAM_PAYMENT_ITEM_TITLE *program_payment_item_title_new(
			char *item_title_P,
			int program_number )
{
	PROGRAM_PAYMENT_ITEM_TITLE *p;

	if ( ! ( p = calloc( 1, sizeof( PROGRAM_PAYMENT_ITEM_TITLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	p->item_title_P = item_title_P;
	p->program_number = program_number;
	return p;
}

/* ------------------- */
/* Sample item_title_P */
/* ------------------- */
/*

Mary Poppins Junior Tickets: Saturday, March 28, 7:00pm, Mary Poppins Junior Tickets: Monday, March 30, 7:00pm

*/

char *program_payment_item_title_block(
			char *item_title_P,
			int program_number )
{
	static char program_block[ 512 ];

/* Only doing list of 1 for now. */
/* ----------------------------- */
if ( program_number > 1 ) return (char *)0;

	if ( !piece( program_block, ':', item_title_P, program_number - 1 ) )
	{
		return (char *)0;
	}

	return program_block;
}

char *program_payment_item_title_name(
			LIST *not_exists_program_name_list,
			char *item_title_P,
			int program_number,
			LIST *program_list )
{
	char *item_title_block;
	char *program_name;

	if ( program_payment_is_tuition( item_title_P ) )
	{
		return (char *)0;
	}

/* Program item title block like:

Mary Poppins Junior Tickets

*/
	if ( ! ( item_title_block =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			program_payment_item_title_block(
				item_title_P,
				program_number ) ) )
	{
		return (char *)0;
	}

	if ( !*item_title_block ) return (char *)0;

	program_name =
		program_seek_name(
			program_list,
			item_title_block
				/* program_name */ );

	if ( ( !program_name || !*program_name )
	&&     not_exists_program_name_list )
	{
		list_set_unique(
			not_exists_program_name_list,
			strdup( item_title_block ) );
	}
	return program_name;
}

