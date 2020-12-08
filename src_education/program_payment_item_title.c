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
#include "product.h"
#include "tuition_payment_fns.h"
#include "program_payment_item_title.h"

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
		if ( !transaction_type_E
		||   !piece(	program_block,
				',',
				transaction_type_E,
				program_number - 1 ) )
		{
			return (char *)0;
		}
	}

	return program_block;
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
			program_payment_item_title_block(
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
			program_payment_item_title_block(
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

