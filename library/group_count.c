/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/group_count.c		   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "document.h"
#include "sql.h"
#include "application.h"
#include "html.h"
#include "group_count.h"

GROUP_RELATION *group_relation_new(
		char *application_name,
		char *folder_name,
		char *data_directory,
		char *query_table_edit_where_string,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_float_name_list,
		unsigned long group_count_row_count,
		RELATION_MTO1 *relation_mto1,
		pid_t group_relation_process_id_increment )
{
	GROUP_RELATION *group_relation;

	if ( !application_name
	||   !folder_name
	||   !data_directory
	||   !group_count_row_count
	||   !relation_mto1
	||   !list_length( relation_mto1->relation_foreign_key_list )
	||   !relation_mto1->relation_prompt
	||   !group_relation_process_id_increment )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	group_relation = group_relation_calloc();

	group_relation->relation_mto1 = relation_mto1;

	group_relation->query_group =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_group_new(
			application_name,
			folder_name,
			query_table_edit_where_string,
			relation_mto1_isa_list,
			folder_attribute_float_name_list,
			group_count_row_count,
			relation_mto1->relation_foreign_key_list );

	group_relation->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			data_directory,
			"group_relation" /* filename_stem */,
			application_name,
			group_relation_process_id_increment,
			(char *)0 /* session_key */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" /* extension */ );

	group_relation->prompt_filename =
		group_relation->
			appaserver_link->
			appaserver_link_prompt->
			filename;

	group_relation->output_filename =
		group_relation->
			appaserver_link->
			appaserver_link_output->
			filename;

	group_relation->anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		group_relation_anchor_html(
			relation_mto1->one_folder_name,
			group_relation->prompt_filename );

	group_relation->title =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		group_relation_title(
			folder_name,
			relation_mto1->relation_prompt );

	group_relation->html_table_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		group_relation_html_table_system_string(
			SQL_DELIMITER,
			HTML_TABLE_QUEUE_TOP_BOTTOM,
			folder_attribute_float_name_list,
			relation_mto1->relation_foreign_key_list,
			group_relation->title );

	group_relation->csv_heading_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		group_relation_csv_heading_string(
			folder_attribute_float_name_list,
			relation_mto1->relation_foreign_key_list );

	group_relation->csv_heading_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		group_relation_csv_heading_system_string(
			group_relation->output_filename,
			group_relation->csv_heading_string );

	group_relation->csv_output_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		group_relation_csv_output_system_string(
			SQL_DELIMITER,
			group_relation->output_filename );

	return group_relation;
}

GROUP_RELATION *group_relation_calloc( void )
{
	GROUP_RELATION *group_relation;

	if ( ! ( group_relation = calloc( 1, sizeof ( GROUP_RELATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return group_relation;
}

char *group_relation_title(
		char *folder_name,
		char *relation_prompt )
{
	char title[ 256 ];
	char destination[ 128 ];

	if ( !folder_name
	||   !relation_prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title,
		"%s: %s",
		string_initial_capital(
			destination,
			folder_name ),
		relation_prompt );

	return strdup( title );
}

char *group_relation_csv_heading_string(
		LIST *folder_attribute_float_name_list,
		LIST *foreign_key_list )
{
	static char heading_string[ 512 ];
	char *ptr = heading_string;
	char destination[ 128 ];

	if ( !list_rewind( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		ptr += sprintf(
			ptr,
			"%s,",
			string_initial_capital(
				destination,
				(char *)list_get(
					foreign_key_list ) ) );

	} while ( list_next( foreign_key_list ) );

	ptr += sprintf(
		ptr,
		"count,percent" );

	if ( list_rewind( folder_attribute_float_name_list ) )
	{
		do {
			ptr += sprintf(
				ptr,
				",Avg(%s)",
				(char *)list_get(
					folder_attribute_float_name_list ) );

		} while ( list_next( folder_attribute_float_name_list ) );
	}

	return heading_string;
}

char *group_relation_csv_heading_system_string(
		char *output_filename,
		char *group_relation_csv_heading_string )
{
	char system_string[ 1024 ];

	if ( !output_filename
	||   !group_relation_csv_heading_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"/bin/echo \"%s\" > %s",
		group_relation_csv_heading_string,
		output_filename );

	return strdup( system_string );
}

char *group_relation_html_table_system_string(
		char sql_delimiter,
		int html_table_queue_top_bottom,
		LIST *folder_attribute_float_name_list,
		LIST *foreign_key_list,
		char *group_relation_title )
{
	char system_string[ 1024 ];
	char *html_heading_string;
	char *html_justify_string;

	if ( !sql_delimiter
	||   !foreign_key_list
	||   !group_relation_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	html_heading_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		group_relation_html_heading_string(
			folder_attribute_float_name_list,
			foreign_key_list );

	html_justify_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		group_relation_html_justify_string(
			list_length( foreign_key_list )
				/* foreign_key_list_length */ );

	sprintf(system_string,
		"queue_top_bottom_lines.e %d 		|"
		"html_table.e '^^%s' '%s' '%c' '%s'	 ",
		html_table_queue_top_bottom,
		group_relation_title,
		html_heading_string,
		sql_delimiter,
		html_justify_string );

	return strdup( system_string );
}

char *group_relation_html_heading_string(
		LIST *folder_attribute_float_name_list,
		LIST *foreign_key_list )
{
	static char heading_string[ 512 ];
	char *ptr = heading_string;
	char *tmp;

	if ( !list_length( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"%s,count,percent",
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		( tmp = list_display_delimited(
				foreign_key_list,
				',' ) ) );

	free( tmp );

	if ( list_rewind( folder_attribute_float_name_list ) )
	{
		do {
			ptr += sprintf(
				ptr,
				",average<br>%s",
				(char *)list_get(
					folder_attribute_float_name_list ) );

		} while ( list_next( folder_attribute_float_name_list ) );
	}

	return heading_string;
}

char *group_relation_html_justify_string( int foreign_key_list_length )
{
	static char justify_string[ 128 ];
	char *ptr = justify_string;
	int i;

	if ( !foreign_key_list_length )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list_length is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	for ( i = 1; i <= foreign_key_list_length; i++ )
		ptr += sprintf( ptr, "left," );

	ptr += sprintf( ptr, "right" );

	return justify_string;
}

char *group_relation_csv_output_system_string(
		char sql_delimiter,
		char *output_filename )
{
	char system_string[ 1024 ];

	if ( !sql_delimiter
	||   !output_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"double_quote_comma_delimited.e '%c' >> %s",
		sql_delimiter,
		output_filename );

	return strdup( system_string );
}

char *group_relation_prompt_message( char *one_folder_name )
{
	static char prompt_message[ 128 ];
	char destination[ 128 ];

	if ( !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(prompt_message,
		"Link to %s",
		string_initial_capital(
			destination,
			one_folder_name ) );

	return prompt_message;
}

char *group_relation_anchor_html(
		char *one_folder_name,
		char *prompt_filename )
{
	if ( !one_folder_name
	||   !prompt_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	appaserver_link_anchor_html(
		prompt_filename,
		"_new" /* target_window */,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		group_relation_prompt_message(
			one_folder_name ) );
}

void group_relation_output(
		unsigned long group_count_row_count,
		GROUP_RELATION *group_relation )
{
	FILE *csv_output_pipe;
	FILE *html_output_pipe;
	FILE *input_pipe;
	char input[ 1024 ];
	char *row_count_string;

	if ( !group_relation
	||   !group_relation->csv_heading_system_string
	||   !group_relation->csv_output_system_string
	||   !group_relation->html_table_system_string
	||   !group_relation->relation_mto1
	||   !group_relation->query_group
	||   !group_relation->query_group->query_system_string
	||   !group_relation->anchor_html )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( system( group_relation->csv_heading_system_string ) ){}

	csv_output_pipe =
		group_relation_csv_output_pipe(
			group_relation->csv_output_system_string );

	html_output_pipe =
		group_relation_html_output_pipe(
			group_relation->html_table_system_string );

	input_pipe =
		group_relation_input_pipe(
			group_relation->
				query_group->
				query_system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		fprintf(html_output_pipe,
			"%s\n",
		      	input );

		fprintf(
			csv_output_pipe,
			"%s\n",
			input );
	}

	row_count_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		group_relation_html_row_count_string(
			SQL_DELIMITER,
			group_count_row_count,
			list_length(
				group_relation->
					relation_mto1->
					relation_foreign_key_list )
						/* foreign_key_list_length */ );

	fprintf(
		html_output_pipe,
		"%s\n",
		row_count_string );

	pclose( input_pipe );
	pclose( html_output_pipe );
	pclose( csv_output_pipe );

	printf(	"%s<p>\n",
		group_relation->anchor_html );
	fflush( stdout );
}

FILE *group_relation_csv_output_pipe( char *csv_output_system_string )
{
	if ( !csv_output_system_string )
	{
		char message[ 128 ];

		sprintf(message, "csv_output_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	popen( csv_output_system_string, "w" );
}

FILE *group_relation_html_output_pipe( char *html_table_system_string )
{
	if ( !html_table_system_string )
	{
		char message[ 128 ];

		sprintf(message, "html_table_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	popen( html_table_system_string, "w" );
}

FILE *group_relation_input_pipe( char *query_system_string )
{
	if ( !query_system_string )
	{
		char message[ 128 ];

		sprintf(message, "query_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	popen( query_system_string, "r" );
}

char *group_relation_html_row_count_string(
		char sql_delimiter,
		unsigned long group_count_row_count,
		int foreign_key_list_length )
{
	static char row_count_string[ 128 ];
	char *ptr = row_count_string;
	int i;

	if ( !foreign_key_list_length )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list_length is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr, "Row Count" );

	for( i = 0; i < foreign_key_list_length; i++ )
		ptr += sprintf( ptr, "%c", sql_delimiter );

	ptr += sprintf(
		ptr,
		"%ld",
		group_count_row_count );

	return row_count_string;
}

GROUP_COUNT *group_count_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory )
{
	GROUP_COUNT *group_count;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name
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

	group_count = group_count_calloc();

	group_count->group_count_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		group_count_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string );

	if ( !list_length(
		group_count->
			group_count_input->
			relation_mto1_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"group_count_input->relation_mto1_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	group_count->query_statistic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_statistic_new(
			application_name,
			folder_name,
			group_count->
				group_count_input->
				security_entity->
				where,
			group_count->
				group_count_input->
				dictionary_separate->
				non_prefixed_dictionary
					/* query_dictionary */,
			group_count->
				group_count_input->
				folder_attribute_append_isa_list,
			group_count->
				group_count_input->
				relation_mto1_list,
			group_count->
				group_count_input->
				relation_mto1_isa_list,
			group_count->
				group_count_input->
				primary_key_first,
			(LIST *)0 /* attribute_float_name_list */,
			(LIST *)0 /* attribute_date_name_list */ );

	group_count->query_statistic->query_statistic_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_statistic_fetch_new(
			SQL_DELIMITER,
			group_count->
				group_count_input->
				primary_key_first,
			(LIST *)0 /* attribute_float_name_list */,
			(LIST *)0 /* attribute_date_name_list */,
			group_count->query_statistic->query_system_string );


	group_count->row_count =
		group_count_row_count(
			group_count->query_statistic->query_statistic_fetch );

	group_count->group_relation_list =
		group_relation_list(
			application_name,
			folder_name,
			data_directory,
			group_count->
				query_statistic->
				query_table_edit_where->
				string,
			group_count->
				group_count_input->
				relation_mto1_list,
			group_count->
				group_count_input->
				relation_mto1_isa_list,
			group_count->
				group_count_input->
				folder_attribute_float_name_list,
			group_count->
				group_count_input->
				dictionary_separate->
				no_display_name_list,
			group_count->row_count,
			getpid() /* process_id */ );

	group_count->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		group_count_title_string(
			folder_name );

	group_count->sub_title_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		group_count_sub_title_string(
			group_count->
				query_statistic->
				query_table_edit_where->
				string );

	return group_count;
}

GROUP_COUNT *group_count_calloc( void )
{
	GROUP_COUNT *group_count;

	if ( ! ( group_count = calloc( 1, sizeof ( GROUP_COUNT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return group_count;
}

char *group_count_input_primary_key_first( char *primary_key_first )
{
	if ( !primary_key_first )
	{
		char message[ 128 ];

		sprintf(message, "primary_key_first is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return primary_key_first;
}

char *group_count_title_string( char *folder_name )
{
	static char title_string[ 128 ];
	char destination[ 64 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title_string,
		"Group %s",
		string_initial_capital(
			destination,
			folder_name ) );

	return title_string;
}

char *group_count_sub_title_string(
		char *query_table_edit_where_string )
{
	char sub_title_string[ STRING_WHERE_BUFFER ];

	if ( !query_table_edit_where_string )
	{
		query_table_edit_where_string = "Entire table";
	}

	if (	strlen( query_table_edit_where_string ) + 7 >=
		STRING_WHERE_BUFFER )
	{
		*sub_title_string = '\0';
	}
	else
	{
		sprintf(sub_title_string,
			"Where: %s",
			query_table_edit_where_string );
	}

	return strdup( sub_title_string );
}

unsigned long group_count_row_count(
		QUERY_STATISTIC_FETCH *query_statistic_fetch )
{
	QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute;

	if ( !query_statistic_fetch )
	{
		char message[ 128 ];

		sprintf(message, "query_statistic_fetch is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( query_statistic_fetch_attribute =
			list_first(
				query_statistic_fetch->
					attribute_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "list_first() returned  empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_statistic_fetch_attribute->row_count;
}

LIST *group_relation_list(
		char *application_name,
		char *folder_name,
		char *data_directory,
		char *query_table_edit_where_string,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_float_name_list,
		LIST *no_display_name_list,
		unsigned long group_count_row_count,
		pid_t process_id )
{
	LIST *relation_list;
	RELATION_MTO1 *relation_mto1;

	if ( !application_name
	||   !folder_name
	||   !data_directory
	||   !list_rewind( relation_mto1_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_list = list_new();

	do {
		relation_mto1 =
			list_get(
				relation_mto1_list );

		if ( !list_length(
			list_subtract(
				relation_mto1->relation_foreign_key_list,
				no_display_name_list ) ) )
					continue;

		process_id =
			group_relation_process_id_increment(
				process_id );

		list_set(
			relation_list,
			group_relation_new(
				application_name,
				folder_name,
				data_directory,
				query_table_edit_where_string,
				relation_mto1_isa_list,
				folder_attribute_float_name_list,
				group_count_row_count,
				relation_mto1,
				process_id ) );

	} while ( list_next( relation_mto1_list ) );

	return relation_list;
}

void group_count_output(
		char *application_name,
		GROUP_COUNT *group_count )
{
	GROUP_RELATION *group_relation;

	if ( !application_name
	||   !group_count
	||   !group_count->title_string
	||   !group_count->sub_title_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		group_count->title_string );

	printf(	"<h2>%s</h2>\n",
		group_count->sub_title_string );

	fflush( stdout );

	if ( list_rewind( group_count->group_relation_list ) )
	do {
		group_relation =
			list_get(
				group_count->group_relation_list );

		group_relation_output(
			group_count->row_count,
			group_relation );

	} while ( list_next( group_count->group_relation_list ) );

	printf(	"%s\n%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_body_close_tag(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_close_tag() );
}

GROUP_COUNT_INPUT *group_count_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string )
{
	GROUP_COUNT_INPUT *group_count_input;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	group_count_input = group_count_input_calloc();

	group_count_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_role_attribute_exclude_list */ );

	group_count_input->role_attribute_exclude_lookup_name_list =
		role_attribute_exclude_lookup_name_list(
			ROLE_PERMISSION_LOOKUP,
			ROLE_PERMISSION_UPDATE,
			group_count_input->role->role_attribute_exclude_list );

	group_count_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			group_count_input->
				role_attribute_exclude_lookup_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	group_count_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			group_count_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	group_count_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			group_count_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	group_count_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			group_count_input->folder->folder_attribute_list,
			group_count_input->relation_mto1_isa_list );

	group_count_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			group_count_input->folder_attribute_append_isa_list );

	group_count_input->folder_attribute_float_name_list =
		folder_attribute_float_name_list(
			group_count_input->folder_attribute_append_isa_list );

	group_count_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	group_count_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			group_count_input->
				folder_row_level_restriction->
				non_owner_forbid,
			group_count_input->
				role->
				override_row_restrictions );

	group_count_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			dictionary_string );

	group_count_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_table_edit_new(
			group_count_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			group_count_input->folder_attribute_date_name_list,
			group_count_input->folder_attribute_append_isa_list );

	group_count_input->primary_key_first =
		/* ------------------------- */
		/* Returns primary_key_first */
		/* ------------------------- */
		group_count_input_primary_key_first(
			list_first(
				group_count_input->
					folder->
					folder_attribute_primary_key_list )
					/* primary_key_first */ );

	return group_count_input;
}

GROUP_COUNT_INPUT *group_count_input_calloc( void )
{
	GROUP_COUNT_INPUT *group_count_input;

	if ( ! ( group_count_input =
			calloc( 1,
				sizeof ( GROUP_COUNT_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return group_count_input;
}

pid_t group_relation_process_id_increment( pid_t process_id )
{
	return process_id + 1;
}

