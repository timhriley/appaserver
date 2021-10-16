/* ---------------------------------------------------	*/
/* src_benthic/benthic_species.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "benthic_species.h"
#include "list.h"
#include "piece.h"
#include "timlib.h"
#include "appaserver_library.h"

BENTHIC_SPECIES *benthic_species_new(
			char *scientific_name )
{
	BENTHIC_SPECIES *benthic_species;

	if ( ! ( benthic_species =
		(BENTHIC_SPECIES *)
			calloc( 1, sizeof( BENTHIC_SPECIES ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	benthic_species->scientific_name = scientific_name;
	return benthic_species;

} /* benthic_species_new() */

LIST *benthic_species_with_faunal_group_get_species_list(
			char *application_name,
			char *faunal_group )
{
	LIST *benthic_species_list;
	char *record;
	char scientific_name[ 128 ];
	char common_name[ 128 ];
	static LIST *benthic_species_record_list = {0};
	BENTHIC_SPECIES *benthic_species;

	if ( !benthic_species_record_list )
	{
		benthic_species_record_list =
			benthic_species_get_record_list(
				application_name );
	}

	benthic_species_list = list_new();

	if ( !list_rewind( benthic_species_record_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty benthic_species_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record =
			list_get_pointer(
				benthic_species_record_list );

		if ( timlib_strncmp( record, faunal_group ) == 0 )
		{
			/* Piece family,genus,species */
			/* -------------------------- */
			piece(	scientific_name,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			benthic_species =
				benthic_species_new(
					strdup( scientific_name ) );

			piece(	common_name,
				FOLDER_DATA_DELIMITER,
				record,
				2 );

			benthic_species->common_name =
				strdup( common_name );

			benthic_species->faunal_group =
				faunal_group;

			list_append_pointer(
				benthic_species_list,
				benthic_species );
		}

	} while( list_next( benthic_species_record_list ) );

	return benthic_species_list;

} /* benthic_species_with_faunal_group_get_species_list() */

LIST *benthic_species_get_record_list(
			char *application_name )
{
	char sys_string[ 1024 ];
	char select[ 256 ];
	char *order;

	sprintf( select,
		 "faunal_group,%s,common_name",
		 benthic_species_get_scientific_name_expression(
					"benthic_species" /* table_name */ ) );

	order = "family_name,genus_name,species_name";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=benthic_species		"
		 "			order=%s			",
		 application_name,
		 select,
		 order );

	return pipe2list( sys_string );

} /* benthic_species_get_record_list() */

LIST *benthic_species_get_species_name_list(
			char *application_name,
			char *faunal_group )
{
	char *select;
	char sys_string[ 1024 ];
	char where[ 128 ];

	select =
		benthic_species_get_scientific_name_expression(
			"benthic_species" );

	sprintf( where, "faunal_group = '%s'", faunal_group );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=benthic_species		"
		 "			where=\"%s\"			"
		 "			order=select			",
		 application_name,
		 select,
		 where );
	return pipe2list( sys_string );
} /* benthic_species_get_species_name_list() */

LIST *benthic_species_get_vegetation_species_name_list(
			char *application_name,
			boolean total )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	if ( total )
	{
		strcpy( where, "vegetation_name like '%total%'" );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=vegetation_name		"
		 "			folder=vegetation_species	"
		 "			where=\"%s\"			",
		 application_name,
		 where );
	return pipe2list( sys_string );
} /* benthic_species_get_vegetation_species_name_list() */

LIST *benthic_species_get_common_name_list(
			char *application_name,
			char *faunal_group )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *order;

	sprintf( where, "faunal_group = '%s'", faunal_group );
	order = "family_name,genus_name,species_name";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=common_name		"
		 "			folder=benthic_species		"
		 "			where=\"%s\"			"
		 "			order=%s			",
		 application_name,
		 where,
		 order );
	return pipe2list( sys_string );
} /* benthic_species_get_common_name_list() */

char *benthic_species_get_scientific_name_expression(
					char *table_name )
{
	static char scientific_name_expression[ 128 ];

	sprintf(	scientific_name_expression,
			BENTHIC_SPECIES_SCIENTIFIC_NAME_TEMPLATE,
			table_name,
			table_name,
			table_name );

	return scientific_name_expression;

} /* benthic_species_get_scientific_name_expression() */

