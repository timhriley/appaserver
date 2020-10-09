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

void output_checks_not_taxes(
			FILE *output_pipe );

void populate_print_checks_entity(
			void );

int main( int argc, char **argv )
{
	char *application_name;

	application_name = environ_exit_application_name( argv[ 0 ] );

	populate_print_checks_entity()

	return 0;
}

void populate_print_checks_entity( void )
{
	FILE *output_pipe;
	LIABILITY *liability;

	liability = liability_calloc();

	p->input.current_liability_account_list =
		pay_liabilities_current_liability_account_list(
			fund_name,
			pay_liabilities_liability_account_name_list()
				/* exclude_account_name_list */ );

/*
fprintf( stderr, "%s/%s()/%d: got input current_liability_account_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
ledger_account_list_display( p->input.current_liability_account_list ) );
*/

	p->input.liability_account_entity_list =
		pay_liabilities_fetch_liability_account_entity_list();

/*
fprintf( stderr, "%s/%s()/%d: got input liability_account_entity_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
entity_list_display( p->input.liability_account_entity_list ) );
*/

	p->process.current_liability_entity_list =
		pay_liabilities_current_liability_entity_list(
			p->input.current_liability_account_list,
			p->input.liability_account_entity_list
				/* exclude_entity_list */ );

/*
fprintf( stderr, "%s/%s()/%d: got process current_liability_entity_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
entity_list_display( p->process.current_liability_entity_list ) );
*/

	output_pipe = popen( "sort", "w" );

	output_entity_list(	output_pipe,
				p->input.liability_account_entity_list );

	output_entity_list(	output_pipe,
				p->process.current_liability_entity_list );

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

		fprintf( output_pipe,
			 "%s^%s [%.2lf]\n",
			 entity->full_name,
			 entity->street_address,
			 entity->sum_balance );

	} while( list_next( entity_list ) );
}

void output_checks_not_taxes(
			FILE *output_pipe,
			char *fund_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "populate_print_checks_entity.sh %s \"%s\" not_taxes",
		 environment_application_name(),
		 fund_name );

	input_pipe = popen( sys_string, "r" );

	while( string_input( input_buffer, input_pipe, 512 ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
}

