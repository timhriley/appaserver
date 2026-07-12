/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/relation_omit_update.c 			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver.h"
#include "String.h"
#include "relation_one2m.h"
#include "relation_omit_update.h"

RELATION_OMIT_UPDATE *relation_omit_update_new(
		char *folder_name,
		LIST *relation_mto1_recursive_list,
		LIST *relation_one2m_list,
		LIST *query_fetch_row_list )
{
	RELATION_OMIT_UPDATE *relation_omit_update;

	if ( !folder_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	relation_omit_update = relation_omit_update_calloc();

	relation_omit_update->relation_one2m_list =
		relation_omit_update_one2m_list(
			relation_mto1_recursive_list,
			relation_one2m_list );

	relation_omit_update->viewonly_boolean =
		relation_omit_update_set_viewonly_boolean(
			query_fetch_row_list /* in/out */,
			relation_omit_update->relation_one2m_list );

	return relation_omit_update;
}

RELATION_OMIT_UPDATE *relation_omit_update_calloc( void )
{
	RELATION_OMIT_UPDATE *relation_omit_update;

	if ( ! ( relation_omit_update =
			calloc( 1,
				sizeof ( RELATION_OMIT_UPDATE ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return relation_omit_update;
}

LIST *relation_omit_update_one2m_list(
		LIST *relation_mto1_recursive_list,
		LIST *relation_one2m_list )
{
	RELATION_MTO1 *relation_mto1;
	RELATION_ONE2M *relation_one2m;
	LIST *list = list_new();

	if ( list_rewind( relation_mto1_recursive_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_recursive_list );

		if ( list_rewind(
			relation_mto1->
				relation_one2m_list ) )
		do {
			relation_one2m =
				list_get(
					relation_mto1->
						relation_one2m_list );

			if ( relation_one2m->relation->omit_update )
			{
				list_set( list, relation_one2m );
			}

		} while ( list_next( 
				relation_mto1->
					relation_one2m_list ) );

	} while ( list_next( relation_mto1_recursive_list ) );

	if ( list_rewind( relation_one2m_list ) )
	do {
		relation_one2m = list_get( relation_one2m_list );

		if ( relation_one2m->relation->omit_update )
		{
			list_set( list, relation_one2m );
		}

	} while ( list_next( relation_one2m_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

boolean relation_omit_update_set_viewonly_boolean(
		LIST *query_fetch_row_list,
		LIST *relation_omit_update_one2m_list )
{
	QUERY_ROW *query_row;
	boolean viewonly_boolean = 0;

	if ( list_rewind( query_fetch_row_list ) )
	do {
		query_row = list_get( query_fetch_row_list );

		if ( relation_omit_update_row_set_viewonly_boolean(
			query_row,
			relation_omit_update_one2m_list ) )
		{
			viewonly_boolean = 1;
		}

	} while ( list_next( query_fetch_row_list ) );

	return viewonly_boolean;
}

boolean relation_omit_update_row_set_viewonly_boolean(
		QUERY_ROW *query_row,
		LIST *relation_omit_update_one2m_list )
{
	RELATION_ONE2M *relation_one2m;
	LIST *query_cell_list;
	char *where_string;
	char *system_string;
	boolean viewonly_boolean = 0;

	if ( !query_row )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"query_row is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( relation_omit_update_one2m_list ) )
	do {
		relation_one2m =
			list_get(
				relation_omit_update_one2m_list );

		query_cell_list =
			/* ------------------------------- */
			/* Returns query_cell_list or null */
			/* ------------------------------- */
			query_cell_attribute_list(
				relation_one2m->
					one_folder_primary_key_list
						/* attribute_name_list */,
				query_row->cell_list );

		if ( list_length( query_cell_list ) )
		{
			if ( query_cell_list_set_attribute_name(
				relation_one2m->relation_foreign_key_list
					/* attribute_name_list */,
				query_cell_list /* in/out */ ) )
			{
				where_string =
					/* --------------------------- */
					/* Returns heap memory or null */
					/* --------------------------- */
					query_cell_where_string(
						query_cell_list );

				system_string =
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					appaserver_system_string(
						"count(1)" /* select */,
						/* --------------------- */
						/* Returns static memory */
						/* --------------------- */
						appaserver_table_name(
							relation_one2m->
							many_folder_name ),
						where_string );

				query_row->viewonly_boolean =
					(boolean)string_atoi(
						string_system_input(
							system_string ) );

				if ( query_row->viewonly_boolean )
					viewonly_boolean = 1;

				free( where_string );
				free( system_string );
			}
		}

	} while ( list_next( relation_omit_update_one2m_list ) );

	return viewonly_boolean;
}

