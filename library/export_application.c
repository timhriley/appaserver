/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/export_application.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "role.h"
#include "folder.h"
#include "role_folder.h"
#include "export_table.h"
#include "appaserver.h"
#include "pkzip.h"
#include "export_application.h"

EXPORT_APPLICATION *export_application_new(
		char *application_name,
		boolean system_folders_boolean,
		char *data_directory )
{
	EXPORT_APPLICATION *export_application;
	EXPORT_TABLE *export_table;

	if ( !application_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	export_application = export_application_calloc();

	export_application->where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		export_application_where(
			ROLE_FOLDER_TABLE,
			ROLE_SYSTEM,
			ROLE_SUPERVISOR,
			FOLDER_PRIMARY_KEY,
			system_folders_boolean );

	export_application->folder_name_list =
		export_application_folder_name_list(
			FOLDER_PRIMARY_KEY,
			FOLDER_TABLE,
			export_application->where );

	export_application->export_table_list = list_new();

	if ( list_rewind( export_application->folder_name_list ) )
	do {
		export_table =
			export_table_new(
				application_name,
				list_get(
					export_application->
						folder_name_list )
							/* folder_name */,
				data_directory,
				(char *)0 /* where */ );

		if ( export_table )
		{
			list_set(
				export_application->export_table_list,
				export_table );
		}

	} while ( list_next( export_application->folder_name_list ) );

	return export_application;
}

EXPORT_APPLICATION *export_application_calloc( void )
{
	EXPORT_APPLICATION *export_application;

	if ( ! ( export_application =
			calloc( 1,
				sizeof ( EXPORT_APPLICATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return export_application;
}

LIST *export_application_folder_name_list(
		const char *folder_primary_key,
		const char *folder_table,
		char *export_application_where )
{
	if ( !export_application_where )
	{
		char message[ 128 ];

		sprintf(message, "export_application_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	list_pipe_fetch(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)folder_primary_key /* select */,
			(char *)folder_table,
			export_application_where ) );
}

char *export_application_where(
		const char *role_folder_table,
		const char *role_system,
		const char *role_supervisor,
		const char *folder_primary_key,
		boolean system_folders_boolean )
{
	char *subquery;
	static char where[ 256 ];

	subquery =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		export_application_role_folder_subquery(
			role_folder_table,
			role_system,
			role_supervisor,
			folder_primary_key,
			system_folders_boolean );

	snprintf(
		where,
		sizeof ( where ),
		"ifnull(exclude_application_export_yn,'n' ) != 'y' "
		"and %s "
		"and %s != 'session'"
		"and %s != 'null'",
		subquery,
		folder_primary_key,
		folder_primary_key );

	return where;
}

char *export_application_role_folder_subquery(
		const char *role_folder_table,
		const char *role_system,
		const char *role_supervisor,
		const char *folder_primary_key,
		boolean system_folders_boolean )
{
	static char subquery[ 128 ];

	snprintf(
		subquery,
		sizeof ( subquery ),
		"%s in (select distinct %s from %s where "
		"role = '%s')",
		folder_primary_key,
		folder_primary_key,
		role_folder_table,
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		export_application_role(
			role_system,
			role_supervisor,
			system_folders_boolean ) );
	return subquery;
}

void export_application_spreadsheet_output(
		char *application_name,
		LIST *export_table_list,
		char *data_directory )
{
	EXPORT_TABLE *export_table;
	LIST *filename_list;
	PKZIP *pkzip;

	if ( !application_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( export_table_list ) ) return;

	do {
		export_table = list_get( export_table_list );

		export_table_spreadsheet_output(
			export_table->appaserver_system_string,
			export_table->spreadsheet_filename,
			export_table->
		  	appaserver_spreadsheet_heading_string,
			export_table->
		  	appaserver_spreadsheet_output_system_string,
			(char *)0 /* appaserver_link_output_html */ );

	} while ( list_next( export_table_list ) );

	filename_list =
		export_table_spreadsheet_filename_list(
			export_table_list );

	pkzip =
		pkzip_new(
			application_name,
			filename_list,
			"export" /* filename_stem */,
			data_directory );

	if ( pkzip
	&&   pkzip->system_string
	&&   pkzip->appaserver_link_anchor_html )
	{
		if ( system( pkzip->system_string ) ){}

		printf( "%s\n", pkzip->appaserver_link_anchor_html );
	}
}

void export_application_shell_output(
		char *application_name,
		LIST *export_table_list,
		char *data_directory )
{
	EXPORT_TABLE *export_table;
	LIST *filename_list;
	PKZIP *pkzip;

	if ( !application_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( export_table_list ) )
	do {
		export_table = list_get( export_table_list );

		export_table_shell_output(
			(char *)0 /* application_name -- no if statement */,
			export_table->application_table_name,
			export_table->appaserver_select,
			export_table->appaserver_system_string,
			export_table->shell_filename,
			export_table->execute_bit_system_string,
			0 /* not output_prompt_boolean */ );

	} while ( list_next( export_table_list ) );

	filename_list =
		export_table_shell_filename_list(
			export_table_list );

	pkzip =
		pkzip_new(
			application_name,
			filename_list,
			"export" /* filename_stem */,
			data_directory );

	if ( pkzip
	&&   pkzip->system_string
	&&   pkzip->appaserver_link_anchor_html )
	{
		if ( system( pkzip->system_string ) ){}

		printf( "%s\n", pkzip->appaserver_link_anchor_html );
	}
}

void export_application_html_output( LIST *export_table_list )
{
	EXPORT_TABLE *export_table;

	if ( list_rewind( export_table_list ) )
	do {
		export_table = list_get( export_table_list );

		export_table_html_output(
			export_table->appaserver_system_string,
			export_table->html_table_system_string );

	} while ( list_next( export_table_list ) );
}

const char *export_application_role(
		const char *role_system,
		const char *role_supervisor,
		boolean system_folders_boolean )
{
	if ( system_folders_boolean )
		return role_system;
	else
		return role_supervisor;
}
