/* $APPASERVER_HOME/library/query_isa.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "relation_mto1.h"
#include "appaserver_error.h"
#include "query_isa.h"

QUERY_ISA *query_isa_where_new(
			char *application_name,
			LIST *relation_mto1_isa_list )
{
	QUERY_ISA *query_isa;

	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_isa = query_isa_calloc();

	if ( !list_length( relation_mto1_isa_list ) ) return query_isa;

	query_isa->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_isa_where(
			application_name,
			relation_mto1_isa_list );

	return query_isa;
}

QUERY_ISA *query_isa_calloc( void )
{
	QUERY_ISA *query_isa;

	if ( ! ( query_isa = calloc( 1, sizeof ( QUERY_ISA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_isa;
}

char *query_isa_where(	char *application_name,
			LIST *relation_mto1_isa_list )
{
	char where[ 2048 ];
	char *ptr = where;
	RELATION_MTO1 *relation_mto1;

	if ( !application_name
	||   !list_rewind( relation_mto1_isa_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		relation_mto1 = list_get( relation_mto1_isa_list );

		if ( !relation_mto1->many_folder_name
		||   !relation_mto1->one_folder_name
		||   !list_length( relation_mto1->relation_foreign_key_list )
		||   !relation_mto1->one_folder
		||   !list_length(
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list ) )
		{
			char message[ 128 ];

			sprintf(message, "relation_mto1 is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf( ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_isa_where_join(
				application_name,
				relation_mto1->many_folder_name,
				relation_mto1->
					relation_foreign_key_list,
				relation_mto1->
					one_folder_name,
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list ) );

	} while ( list_next( relation_mto1_isa_list ) );

	return strdup( where );
}

char *query_isa_where_join(
			char *application_name,
			char *many_folder_name,
			LIST *relation_foreign_key_list,
			char *one_folder_name,
			LIST *one_folder_primary_key_list )
{
	static char join[ 256 ];
	char *ptr = join;
	char *many_table_name;
	char *one_table_name;

	if ( !application_name
	||   !many_folder_name
	||   !list_rewind( relation_foreign_key_list )
	||   !one_folder_name
	||   !list_rewind( one_folder_primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	many_table_name =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				many_folder_name ) );

	one_table_name =
		folder_table_name(
			application_name,
			one_folder_name );

	do {
		if ( ptr != join ) ptr += sprintf( ptr, " and " );

		ptr +=
			sprintf(
				ptr,
				"%s.%s = %s.%s",
				many_table_name,
				(char *)list_get( relation_foreign_key_list ),
				one_table_name,
				(char *)list_get(
					one_folder_primary_key_list ) );

		list_next( relation_foreign_key_list );

	} while ( list_next( one_folder_primary_key_list ) );

	free( many_table_name );
	return join;
}
