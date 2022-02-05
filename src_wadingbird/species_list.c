/* src_wadingbird/species_list.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "dictionary.h"
#include "folder.h"
#include "timlib.h"
#include "query.h"
#include "piece.h"
#include "attribute.h"

/* Prototypes */
/* ---------- */
char *get_species_sys_string(
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *sys_string;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s application login_name\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];

	sys_string = get_species_sys_string(
				application_name );

	system( sys_string );

	return 0;
}

char *get_species_sys_string(	char *application_name )
{
	char sys_string[ 2048 ];
	char *species_table_name;
	char *constant_select = "common_name";

	species_table_name =
		get_table_name(	application_name,
				"species" );

	sprintf( sys_string,
"echo \"select %s from %s where species_code is not null;\" | sql.e",
		 constant_select,
		 species_table_name );

	return strdup( sys_string );

}
