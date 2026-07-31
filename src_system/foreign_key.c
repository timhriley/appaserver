/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_system/foreign_key.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "appaserver_error.h"
#include "environ.h"
#include "boolean.h"
#include "folder_attribute.h"
#include "relation_mto1.h"
#include "relation.h"

void foreign_key_display(
		LIST *seek_list );

int main( int argc, char **argv )
{
	char *many_folder_name;
	char *one_folder_name;
	LIST *primary_key_list;
	LIST *mto1_list;
	LIST *seek_list;

	(void)environ_exit_application_name( argv[ 0 ] );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s many_folder one_folder\n",
			argv[ 0 ] );
		exit( 1 );
	}

	many_folder_name = argv[ 1 ];
	one_folder_name = argv[ 2 ];

	primary_key_list =
		folder_attribute_primary_key_list(
			many_folder_name,
			folder_attribute_list(
				many_folder_name,
				(LIST *)0 /* exclude_attribute... */,
				0 /* not fetch_attribute */,
				0 /* not cache_boolean */ ) );

	mto1_list =
		relation_mto1_list(
			many_folder_name,
			primary_key_list );

	seek_list =
		relation_mto1_seek_list(
			one_folder_name,
			mto1_list );

	foreign_key_display( seek_list );

	mto1_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			many_folder_name,
			primary_key_list,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	seek_list =
		relation_mto1_seek_list(
			one_folder_name,
			mto1_list );

	foreign_key_display( seek_list );

	return 0;
}

void foreign_key_display( LIST *seek_list )
{
	RELATION_MTO1 *relation_mto1;

	if ( list_rewind( seek_list ) )
	do {
		relation_mto1 = list_get( seek_list );

		if ( !list_length(
			relation_mto1->
				relation_foreign_key_list ) )
		{
			fprintf( stderr,
			"ERROR: relation_foreign_key_list is empty.\n" );

			exit( 1 );
		}

		printf( "%s\n",
			list_display_delimited(
				relation_mto1->relation_foreign_key_list,
				',' ) );

	} while ( list_next( seek_list ) );
}

