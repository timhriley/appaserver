/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/menu.c					*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environ.h"
#include "menutree.h"

int main( int argc, char **argv )
{
	MENU_TREE *m = menu_tree_init();
	char menu_filename[ 128 ];

	if ( argc == 1 )
		sprintf( menu_filename, "%s/menu.dat", getenv( "HOME" ) );
	else
		strcpy( menu_filename, argv[ 1 ] );

	if ( menu_tree_load( m, menu_filename ) == -1 )
	{
		fprintf( stderr, "%s: %s\n",
			argv[ 0 ],
			menu_tree_get_error( m ) );
		exit( 1 );
	}

	menu_tree_goto_root( m );
	interact( m );

	return 0;
}


