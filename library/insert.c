/* $APPASERVER_HOME/library/insert.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "process.h"
#include "appaserver_library.h"
#include "relation.h"
#include "list.h"
#include "security.h"
#include "insert.h"

INSERT *insert_new(
			char *application_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *relation_mto1_non_isa_list,
			LIST *folder_attribute_name_list,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			char *post_change_command_line )
{
	INSERT *insert = insert_calloc();
	int row;
	int highest_row;
	char *table_name;

	highest_row =
		dictionary_highest_row(
			row_dictionary );

	if ( highest_row == -1 )
	{
		free( insert );
		return (INSERT *)0;
	}

	insert->insert_row_list = list_new();

	table_name =
		folder_table_name(
			application_name,
			folder_name );

	for ( row = 0; row <= highest_row; row++ )
	{
		list_set(
			insert->insert_row_list,
			insert_row_new(
				table_name,
				primary_key_list,
				relation_mto1_non_isa_list,
				folder_attribute_name_list,
				folder_attribute_list,
				row_dictionary,
				post_change_command_line,
				row ) );

	}

	if ( !list_length( insert->insert_row_list ) )
	{
		list_free( insert->insert_row_list );
		free( insert );
		return (INSERT *)0;
	}

	return insert;
}

LIST *insert_sql_statement_list(
			LIST *insert_row_list )
{
	INSERT_ROW *insert_row;
	LIST *sql_statement_list;

	if ( !list_rewind( insert_row_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		insert_row = list_get( insert_row_list );

		if ( !insert_row->sql_statement )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			sql_statement_list,
			insert_row->sql_statement );

	} while ( list_next( insert_row_list ) );

	return sql_statement_list;
}

LIST *insert_post_change_command_line_list(
			LIST *insert_row_list )
{
	INSERT_ROW *insert_row;
	LIST *command_line_list;

	if ( !list_rewind( insert_row_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		insert_row = list_get( insert_row_list );

		if ( !insert_row->command_line )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: command_line is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			command_line_list,
			insert_row->command_line );

	} while ( list_next( insert_row_list ) );

	return command_line_list;
}

INSERT_ROW *insert_row_calloc( void )
{
	INSERT_ROW *insert_row;

	if ( ! ( insert_row = calloc( 1, sizeof( INSERT_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_row;
}

INSERT_ROW *insert_row_new(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *relation_mto1_non_isa_list,
			LIST *folder_attribute_name_list,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			char *post_change_command_line,
			int row )
{
	INSERT_ROW *insert_row = insert_row_calloc();

	insert_row->insert_data_list = list_new();

	if ( list_rewind( relation_mto1_non_isa_list ) )
	{
		do {
			list_set_list(
				insert_row->insert_data_list,
				insert_data_relation_extract_list(
					list_get( relation_mto1_non_isa_list ),
					folder_attribute_list,
					row_dictionary,
					row ) );
		} while ( list_next( relation_mto1_non_isa_list ) );
	}

	if ( list_rewind( folder_attribute_name_list ) )
	{
		do {
			list_set(
				insert_row->insert_data_list,
				insert_data_attribute_extract(
					list_get( folder_attribute_name_list ),
					folder_attribute_list,
					row_dictionary,
					row ) );
		} while ( list_next( folder_attribute_name_list ) );
	}

	if ( !insert_row_primary_key_okay(
			insert_data_name_list(
				insert_row->insert_data_list ),
			primary_key_list ) )
	{
		list_free( insert_row->insert_data_list );
		free( insert_row );
		return (INSERT_ROW *)0;
	}

	if ( !list_length( insert_row->insert_data_list ) )
	{
		list_free( insert_row->insert_data_list );
		free( insert_row );
		return (INSERT_ROW *)0;
	}
	else
	{
		insert_row->sql_statement =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			insert_row_sql_statement(
				folder_table_name,
				insert_data_name_list(
					insert_row->insert_data_list ),
				insert_data_extract_list(
					insert_row->insert_data_list ) );

		if ( !insert_row->sql_statement )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: insert_row_sql_statement() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		insert_row->command_line =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			insert_row_command_line(
				post_change_command_line,
				insert_row->insert_data_list );

		if ( !insert_row->command_line )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: insert_row_command_line() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return insert_row;
	}
}

boolean insert_row_primary_key_okay(
			LIST *insert_data_name_list,
			LIST *primary_key_list )
{
	LIST *results_list;

	results_list =
		list_subtract(
			primary_key_list,
			insert_data_name_list );

	if ( list_length( results_list ) )
		return 0;
	else
		return 1;
}

char *insert_row_sql_statement(
			char *folder_table_name,
			LIST *data_name_list,
			LIST *data_extract_list )
{
	char sql_statement[ 1024 ];
	char buffer[ 1024 ];

	if ( !folder_table_name ) return (char *)0;

	if ( !list_length( data_name_list ) ) return (char *)0;

	if ( list_length( data_name_list ) != list_length( data_extract_list ) )
		return (char *)0;

	sprintf(sql_statement,
		"insert into %s (%s) values (%s)",
		folder_table_name,
		list_display_delimited(
			data_name_list,
			',' ),
		list_display_quoted_delimited(
			buffer,
			data_extract_list,
			',' ) );

	return strdup( sql_statement );
}

char *insert_row_command_line(
			char *post_change_command_line,
			LIST *insert_data_list )
{
	char command_line[ 1024 ];
	INSERT_DATA *insert_data;
	char search[ 256 ];
	char replace[ 256 ];

	if ( !post_change_command_line )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: post_change_command_line is null.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !*post_change_command_line ) return (char *)0;

	if ( !list_rewind( insert_data_list ) ) return (char *)0;

	string_strcpy( command_line, post_change_command_line, 1024 );
	free( post_change_command_line );

	do {
		insert_data = list_get( insert_data_list );

		sprintf(search,
			" %s",
			insert_data->attribute_name );

		sprintf(replace,
			" \"%s\"",
			insert_data->escaped_replaced_data );

		search_replace_word(
			command_line,
			search, 
			replace );

	} while ( list_next( insert_data_list ) );

	return strdup( command_line );
}

LIST *insert_data_relation_extract_list(
			RELATION *relation_mto1,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			int row )
{
	char *primary_key;
	char *foreign_key;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *data;
	LIST *insert_data_list = {0};
	INSERT_DATA *insert_data;
	int results;

	if ( !list_length( relation_mto1->foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( relation_mto1->foreign_key_list ) !=
	     list_length( relation_mto1->one_folder->primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length(fk) != length(pk).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_rewind( relation_mto1->one_folder->primary_key_list );
	list_rewind( relation_mto1->foreign_key_list );

	do {
		primary_key =
			list_get(
				relation_mto1->
					one_folder->
					primary_key_list );

		if ( ( results =
				dictionary_index_data(
					&data,
					row_dictionary,
					primary_key,
					row ) ) > -1 )
		{
			foreign_key =
				list_get(
					relation_mto1->
						foreign_key_list );

			if ( ! ( folder_attribute =
					folder_attribute_seek(
						foreign_key,
						folder_attribute_list ) ) )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_seek(%s) returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					foreign_key );
				exit( 1 );
			}

			insert_data =
				insert_data_new(
					foreign_key,
					security_sql_injection_escape(
					   security_replace_special_characters(
						string_trim_number_characters(
						     data,
						     folder_attribute->
							attribute->
						     	datatype_name ) ) ) );

			if ( !insert_data_list )
				insert_data_list =
					list_new();

			list_set(
				insert_data_list,
				insert_data );
		}
		list_next( relation_mto1->foreign_key_list );

	} while ( list_next( 
			relation_mto1->
				one_folder->
				primary_key_list ) );

	return insert_data_list;
}

INSERT_DATA *insert_data_attribute_extract(
			char *attribute_name,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			int row )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *data;
	int results;

	if ( ( results =
			dictionary_index_data(
				&data,
				row_dictionary,
				attribute_name,
				row ) ) > -1 )
	{
		if ( ! ( folder_attribute =
				folder_attribute_seek(
					attribute_name,
					folder_attribute_list ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				attribute_name );
			exit( 1 );
		}

		return
		insert_data_new(
			attribute_name,
			security_sql_injection_escape(
				security_replace_special_characters(
					string_trim_number_characters(
						data,
						folder_attribute->
							attribute->
							datatype_name ) ) ) );
	}
	return (INSERT_DATA *)0;
}

LIST *insert_data_name_list(
			LIST *insert_data_list )
{
	INSERT_DATA *insert_data;
	LIST *name_list;

	if ( !list_rewind( insert_data_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		insert_data =
			list_get(
				insert_data_list );

		list_set(
			name_list,
			insert_data->attribute_name );

	} while ( list_next( insert_data_list ) );

	return name_list;
}

LIST *insert_data_extract_list(
			LIST *insert_data_list )
{
	INSERT_DATA *insert_data;
	LIST *data_list;

	if ( !list_rewind( insert_data_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		insert_data =
			list_get(
				insert_data_list );

		list_set(
			data_list,
			insert_data->attribute_name );

	} while ( list_next( insert_data_list ) );

	return data_list;
}

INSERT *insert_calloc( void )
{
	INSERT *insert;

	if ( ! ( insert = calloc( 1, sizeof( INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert;
}

INSERT_DATA *insert_data_calloc( void )
{
	INSERT_DATA *insert_data;

	if ( ! ( insert_data = calloc( 1, sizeof( INSERT_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_data;
}

INSERT_DATA *insert_data_new(
			char *attribute_name,
			char *escaped_replaced_data )
{
	INSERT_DATA *insert_data = insert_data_calloc();

	insert_data->attribute_name = attribute_name;

	insert_data->escaped_replaced_data =
		escaped_replaced_data;

	return insert_data;
}

#ifdef NOT_DEFINED
		process_convert_parameters(
			&post_change_process->executable,
			application_name,
			session,
			"insert" /* state */,
			login_name,
			folder_name,
			role_name,
			(char *)0 /* target_frame */,
			row_dictionary /* parameter_dictionary */,
			row_dictionary /* where_clause_dictionary */,
			post_change_process->attribute_list,
			(LIST *)0 /* prompt_list */,
			primary_key_list,
			(LIST *)0 /* primary_data_list */,
			0 /* row */,
			post_change_process->process_name,
			(PROCESS_SET *)0,
			(char *)0 /* one2m_folder_name_for_processes */,
			(char *)0 /* operation_row_count_string */,
			(char *)0 /* prompt */ );
#endif
