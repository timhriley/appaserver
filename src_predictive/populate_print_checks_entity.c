/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/populate_print_checks_entity.c	*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "appaserver_error.h"
#include "environ.h"
#include "liability.h"

int main( int argc, char **argv )
{
	LIABILITY_CALCULATE *liability_calculate;

	if ( argc ){};

	(void *)environ_exit_application_name( argv[ 0 ] );

	if ( ( liability_calculate =
		liability_calculate_new() ) )
	{
		liability_calculate_stdout(
			liability_calculate->entity_contact_key_boolean,
			liability_calculate->
				liability_entity_list );
	}

	return 0;
}

