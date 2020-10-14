/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/populate_print_checks_entity.c	*/
/* --------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "transaction.h"
#include "entity.h"
#include "liability.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_entity_list(
			FILE *output_pipe,
			LIST *entity_list );

void populate_print_checks_entity(
			void );

int main( int argc, char **argv )
{
	if ( argc ){};
	environ_exit_application_name( argv[ 0 ] );
	populate_print_checks_entity();

	return 0;
}

void populate_print_checks_entity( void )
{
	FILE *output_pipe;
	LIABILITY *liability;

	liability = liability_calloc();

	liability->liability_account_entity_list =
		liability_account_entity_list();

	liability->liability_current_account_list =
		liability_current_account_list();

	liability->liability_tax_redirect_account_list =
		liability_tax_redirect_account_list(
			liability->liability_current_account_list,
			liability->liability_account_entity_list );

	liability->liability_entity_list =
		liability_entity_list(
			liability->
				liability_tax_redirect_account_list,
			(LIST *)0 /* input_entity_list */ );

	output_pipe = popen( "sort", "w" );

	output_entity_list(
		output_pipe,
		liability->
			liability_entity_list );

	pclose( output_pipe );
}

void output_entity_list(
			FILE *output_pipe,
			LIST *entity_list )
{
	ENTITY *entity;

	if ( !list_rewind( entity_list ) ) return;

	do {
		entity = list_get( entity_list );

		if ( !list_length( entity->liability_entity_journal_list ) )
			continue;

		if ( ( entity->liability_entity_amount_due =
			liability_entity_amount_due(
				entity->liability_entity_journal_list ) ) )
		{
			fprintf( output_pipe,
			 	"%s^%s [%.2lf]\n",
			 	entity->full_name,
			 	entity->street_address,
			 	entity->liability_entity_amount_due );
		}

	} while( list_next( entity_list ) );
}

