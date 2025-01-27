/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_self.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "entity.h"
#include "entity_self.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *preupdate_inventory_cost_method_string;
	ENTITY_SELF *self;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s preupdate_inventory_cost_method\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	preupdate_inventory_cost_method_string = argv[ 1 ];

	if ( strcmp(	preupdate_inventory_cost_method_string,
			"preupdate_inventory_cost_method" ) == 0 )
	{
		exit( 0 );
	}

	if ( ! ( self =
			entity_self_fetch(
				0 /* not fetch_entity_boolean */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

/*
	char sys_string[ 128 ];
	if ( ( self->inventory_cost_method != inventory_not_set )
	&&   (	self->inventory_cost_method !=
		preupdate_inventory_cost_method ) )
	{
		sprintf( sys_string,
			 "inventory_cost_method_propagate %s",
		 	 application_name );
		system( sys_string );
	}
*/

	return 0;
}

