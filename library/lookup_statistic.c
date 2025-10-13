/* $APPASERVER_HOME/library/lookup_statistic.c				*/
/* -------------------------------------------------------------------- */
/* This ADT supports the statistics push button on the prompt screens.  */
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sql.h"
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "relation_mto1.h"
#include "document.h"
#include "query_statistic.h"
#include "lookup_statistic.h"

LOOKUP_STATISTIC *lookup_statistic_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string )
{
	LOOKUP_STATISTIC *lookup_statistic;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_statistic = lookup_statistic_calloc();

	lookup_statistic->lookup_statistic_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_statistic_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string );

	lookup_statistic->query_statistic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_statistic_new(
			application_name,
			folder_name,
			lookup_statistic->
				lookup_statistic_input->
				/* --------------------- */
				/* Static memory or null */
				/* --------------------- */
				security_entity->
				where,
			lookup_statistic->
				lookup_statistic_input->
				dictionary_separate->
				non_prefixed_dictionary
					/* query_dictionary */,
			lookup_statistic->
				lookup_statistic_input->
				folder_attribute_append_isa_list,
			lookup_statistic->
				lookup_statistic_input->
				relation_mto1_list,
			lookup_statistic->
				lookup_statistic_input->
				relation_mto1_isa_list,
			lookup_statistic->
				lookup_statistic_input->
				primary_key_first,
			lookup_statistic->
				lookup_statistic_input->
				attribute_number_name_list,
			lookup_statistic->
				lookup_statistic_input->
				attribute_date_name_list );

	lookup_statistic->query_statistic->query_statistic_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_statistic_fetch_new(
			SQL_DELIMITER,
			lookup_statistic->
				lookup_statistic_input->
				primary_key_first,
			lookup_statistic->
				lookup_statistic_input->
				attribute_number_name_list,
			lookup_statistic->
				lookup_statistic_input->
				attribute_date_name_list,
			lookup_statistic->
				query_statistic->
				query_system_string );

	lookup_statistic->after_minimum_blank_count =
		lookup_statistic_after_minimum_blank_count();

	lookup_statistic->after_range_blank_count =
		lookup_statistic_after_range_blank_count(
			lookup_statistic->
				query_statistic->
				query_statistic_fetch->
				outlier_low_heading_string,
			lookup_statistic->
				query_statistic->
				query_statistic_fetch->
				outlier_high_heading_string );

	lookup_statistic->lookup_statistic_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_statistic_table_new(
			lookup_statistic->
				lookup_statistic_input->
				attribute_number_count,
			lookup_statistic->
				query_statistic->
				query_statistic_fetch->
				attribute_list,
			lookup_statistic->
				query_statistic->
				query_statistic_fetch->
				outlier_low_heading_string,
			lookup_statistic->
				query_statistic->
				query_statistic_fetch->
				outlier_high_heading_string,
			lookup_statistic->after_minimum_blank_count,
			lookup_statistic->after_range_blank_count );

	lookup_statistic->sub_title_string =
		/* ------------------------------------------------------- */
		/* Returns query_table_edit_where_string or program memory */
		/* ------------------------------------------------------- */
		lookup_statistic_sub_title_string(
			lookup_statistic->
				query_statistic->
				query_table_edit_where->
				string );

	return lookup_statistic;
}

LOOKUP_STATISTIC *lookup_statistic_calloc( void )
{
	LOOKUP_STATISTIC *lookup_statistic;

	if ( ! ( lookup_statistic =
			calloc( 1,
				sizeof ( LOOKUP_STATISTIC ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_statistic;
}

char *lookup_statistic_input_primary_key_first(
		char *primary_key_first )
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

char *lookup_statistic_sub_title_string(
		char *query_table_edit_where_string )
{
	if (	string_strlen( query_table_edit_where_string ) >
		1024 )
	{
		query_table_edit_where_string = "Overflow";
	}
	else
	if ( !query_table_edit_where_string )
	{
		query_table_edit_where_string = "Entire Table";
	}

	return query_table_edit_where_string;
}

void lookup_statistic_output(
		char *application_name,
		LOOKUP_STATISTIC *lookup_statistic )
{
	if ( !application_name
	||   !lookup_statistic
	||   !lookup_statistic->lookup_statistic_table
	||   !lookup_statistic->lookup_statistic_table->html_table
	||   !lookup_statistic->sub_title_string
	||   !lookup_statistic->lookup_statistic_input->folder )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		lookup_statistic->
			lookup_statistic_input->
			folder->
			folder_name /* process_name */ );

	printf( "<h2>Where: %s</h2>\n",
		lookup_statistic->sub_title_string );

	html_table_output(
		lookup_statistic->
			lookup_statistic_table->
			html_table,
		0 /* html_table_rows_between_heading */ );

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

LIST *lookup_statistic_table_primary_cell_list(
		unsigned long row_count,
		char *primary_label,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count )
{
	LIST *cell_list;
	int blank_count;
	int i;

	if ( !primary_label )
	{
		char message[ 128 ];

		sprintf(message, "primary_label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			primary_label,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			place_commas_in_unsigned_long( row_count ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	blank_count =
		lookup_statistic_table_blank_count(
			lookup_statistic_after_minimum_blank_count,
			lookup_statistic_after_range_blank_count );

	for (	i = 0;
		i < blank_count;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new( (char *)0, 0, 0 ) );
	}

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_calculate_percent_missing_string(
				0.0 /* percent_missing */ ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return cell_list;
}

LIST *lookup_statistic_table_date_cell_list(
		STATISTIC_DATE *statistic_date,
		char *label,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count )
{
	LIST *cell_list;
	int i;

	if ( !statistic_date
	||   !label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			label /* data */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );


	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			place_commas_in_unsigned_long(
				statistic_date->value_count ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new( (char *)0 /* sum */, 0, 0 ) );

	list_set(
		cell_list,
		html_cell_new(
			statistic_date->minimum_value,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	for (	i = 0;
		i < lookup_statistic_after_minimum_blank_count;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new( (char *)0, 0, 0 ) );
	}

	list_set(
		cell_list,
		html_cell_new(
			statistic_date->maximum_value,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_quartile_range_string(
				(double)statistic_date->range ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	for (	i = 0;
		i < lookup_statistic_after_range_blank_count;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new( (char *)0, 0, 0 ) );
	}

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_calculate_percent_missing_string(
				statistic_date->percent_missing ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return cell_list;
}

LIST *lookup_statistic_table_number_cell_list(
		int attribute_number_count,
		STATISTIC *statistic,
		char *label )
{
	if ( !statistic
	||   !statistic->statistic_calculate
	||   !label )
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
	statistic_table_cell_list(
		statistic,
		label,
		lookup_statistic_table_outlier_boolean(
			attribute_number_count ) );
}

LIST *lookup_statistic_table_cell_list(
		int attribute_number_count,
		QUERY_STATISTIC_FETCH_ATTRIBUTE *
			query_statistic_fetch_attribute,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count )
{
	if ( !query_statistic_fetch_attribute )
	{
		char message[ 128 ];

		sprintf(message, "query_statistic_fetch_attribute is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( query_statistic_fetch_attribute->attribute_is_primary_first )
	{
		char *primary_label;

		primary_label =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			lookup_statistic_table_primary_label(
				query_statistic_fetch_attribute->
					attribute_name );

		if ( query_statistic_fetch_attribute->attribute_is_number )
		{
			if ( !query_statistic_fetch_attribute->
				statistic->
				statistic_calculate )
			{
				char message[ 128 ];

				sprintf(message,
				"statistic->statistic_calculate is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			return
			lookup_statistic_table_number_cell_list(
				attribute_number_count,
				query_statistic_fetch_attribute->statistic,
				primary_label );
		}
		else
		if ( query_statistic_fetch_attribute->attribute_is_date )
		{
			return
			lookup_statistic_table_date_cell_list(
				query_statistic_fetch_attribute->statistic_date,
				primary_label,
				lookup_statistic_after_minimum_blank_count,
				lookup_statistic_after_range_blank_count );
		}
		else
		{
			return
			lookup_statistic_table_primary_cell_list(
				query_statistic_fetch_attribute->row_count,
				primary_label,
				lookup_statistic_after_minimum_blank_count,
				lookup_statistic_after_range_blank_count );
		}
	}

	if ( query_statistic_fetch_attribute->attribute_is_number )
	{
		if ( !query_statistic_fetch_attribute->
			statistic->
			statistic_calculate )
		{
			char message[ 128 ];

			sprintf(message,
			"statistic->statistic_calculate is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return
		lookup_statistic_table_number_cell_list(
			attribute_number_count,
			query_statistic_fetch_attribute->statistic,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			lookup_statistic_table_label(
				query_statistic_fetch_attribute->
					attribute_name ) );
	}
	else
	if ( query_statistic_fetch_attribute->attribute_is_date )
	{
		return
		lookup_statistic_table_date_cell_list(
			query_statistic_fetch_attribute->statistic_date,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			lookup_statistic_table_label(
			query_statistic_fetch_attribute->
					attribute_name ),
			lookup_statistic_after_minimum_blank_count,
			lookup_statistic_after_range_blank_count );
	}

	appaserver_error_stderr_exit(
		__FILE__,
		__FUNCTION__,
		__LINE__,
"All three are zero: attribute_is_primary_first, attribute_is_number, attribute_is_date." );

	/* Stub */
	/* ---- */
	return (LIST *)0;
}

char *lookup_statistic_table_label( char *attribute_name )
{
	char label[ 128 ];

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup(
		string_initial_capital(
			label,
			attribute_name ) );
}

char *lookup_statistic_table_primary_label( char *attribute_name )
{
	char primary_label[ 128 ];
	char destination[ 128 ];

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(primary_label,
		"*%s",
		string_initial_capital(
			destination,
			attribute_name ) );

	return strdup( primary_label );
}

HTML_ROW *lookup_statistic_table_row_new(
		int attribute_number_count,
		QUERY_STATISTIC_FETCH_ATTRIBUTE *
			query_statistic_fetch_attribute,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count )
{
	HTML_ROW *html_row;
	static int row_number;

	if ( !query_statistic_fetch_attribute )
	{
		char message[ 128 ];

		sprintf(message, "query_statistic_fetch_attribute is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	html_row = html_row_calloc();

	if ( row_number++ % 2 ) html_row->background_shaded_boolean = 1;

	html_row->cell_list =
		lookup_statistic_table_cell_list(
			attribute_number_count,
			query_statistic_fetch_attribute,
			lookup_statistic_after_minimum_blank_count,
			lookup_statistic_after_range_blank_count );

	return html_row;
}

LIST *lookup_statistic_table_row_list(
		int attribute_number_count,
		LIST *query_statistic_fetch_attribute_list,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count )
{
	LIST *row_list;
	QUERY_STATISTIC_FETCH_ATTRIBUTE *
		query_statistic_fetch_attribute;

	row_list = list_new();

	if ( list_rewind( query_statistic_fetch_attribute_list ) )
	do {
		query_statistic_fetch_attribute =
			list_get(
				query_statistic_fetch_attribute_list );

		list_set(
			row_list,
			lookup_statistic_table_row_new(
				attribute_number_count,
				query_statistic_fetch_attribute,
				lookup_statistic_after_minimum_blank_count,
				lookup_statistic_after_range_blank_count ) );

	} while ( list_next( query_statistic_fetch_attribute_list ) );

	return row_list;
}

LOOKUP_STATISTIC_TABLE *lookup_statistic_table_new(
		int attribute_number_count,
		LIST *query_statistic_fetch_attribute_list,
		char *outlier_low_heading_string,
		char *outlier_high_heading_string,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count )
{
	LOOKUP_STATISTIC_TABLE *lookup_statistic_table;

	if ( !list_length( query_statistic_fetch_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"query_statistic_fetch_attribute_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_statistic_table = lookup_statistic_table_calloc();

	lookup_statistic_table->html_table =
		html_table_new(
			(char *)0 /* title */,
			(char *)0 /* sub_title */,
			(char *)0 /* sub_sub_title */ );

	lookup_statistic_table->html_table->column_list =
		statistic_table_column_list(
			outlier_low_heading_string,
			outlier_high_heading_string );

	lookup_statistic_table->html_table->row_list =
		lookup_statistic_table_row_list(
			attribute_number_count,
			query_statistic_fetch_attribute_list,
			lookup_statistic_after_minimum_blank_count,
			lookup_statistic_after_range_blank_count );

	return lookup_statistic_table;
}

LOOKUP_STATISTIC_TABLE *lookup_statistic_table_calloc( void )
{
	LOOKUP_STATISTIC_TABLE *lookup_statistic_table;

	if ( ! ( lookup_statistic_table =
			calloc( 1,
				sizeof ( LOOKUP_STATISTIC_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_statistic_table;
}

LOOKUP_STATISTIC_INPUT *lookup_statistic_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string )
{
	LOOKUP_STATISTIC_INPUT *lookup_statistic_input;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_statistic_input = lookup_statistic_input_calloc();

	lookup_statistic_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_role_attribute_exclude_list */ );

	lookup_statistic_input->role_attribute_exclude_lookup_name_list =
		role_attribute_exclude_lookup_name_list(
			APPASERVER_LOOKUP_STATE,
			APPASERVER_UPDATE_STATE,
			lookup_statistic_input->
				role->
				role_attribute_exclude_list );

	lookup_statistic_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			lookup_statistic_input->
				role_attribute_exclude_lookup_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	lookup_statistic_input->relation_mto1_list =
		relation_mto1_list(
			folder_name
				/* many_folder_name */,
			lookup_statistic_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	lookup_statistic_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			lookup_statistic_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	lookup_statistic_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			lookup_statistic_input->folder->folder_attribute_list,
			lookup_statistic_input->relation_mto1_isa_list );

	lookup_statistic_input->primary_key_first =
		/* ------------------------- */
		/* Returns primary_key_first */
		/* ------------------------- */
		lookup_statistic_input_primary_key_first(
			list_first(
				lookup_statistic_input->
					folder->
					folder_attribute_primary_key_list )
					/* primary_key_first */ );

	lookup_statistic_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	lookup_statistic_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			lookup_statistic_input->
				folder_row_level_restriction->
				non_owner_forbid,
			lookup_statistic_input->
				role->
				override_row_restrictions );

	lookup_statistic_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			dictionary_string );

	lookup_statistic_input->folder_attribute_number_name_list =
		folder_attribute_number_name_list(
			lookup_statistic_input->
				folder_attribute_append_isa_list );

	lookup_statistic_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			lookup_statistic_input->
				folder_attribute_append_isa_list );

	lookup_statistic_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_prompt_lookup_new(
			lookup_statistic_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			lookup_statistic_input->
				relation_mto1_list,
			lookup_statistic_input->
				folder->
				folder_attribute_name_list,
			lookup_statistic_input->
				folder_attribute_date_name_list,
			lookup_statistic_input->
				folder_attribute_append_isa_list );

	lookup_statistic_input->attribute_number_name_list =
		lookup_statistic_input_attribute_number_name_list(
			lookup_statistic_input->
				folder_attribute_number_name_list,
			lookup_statistic_input->
				dictionary_separate->
				no_display_name_list );

	lookup_statistic_input->attribute_number_count =
		lookup_statistic_input_attribute_number_count(
			lookup_statistic_input->
				attribute_number_name_list );

	lookup_statistic_input->attribute_date_name_list =
		lookup_statistic_input_attribute_date_name_list(
			lookup_statistic_input->
				folder_attribute_date_name_list,
			lookup_statistic_input->
				dictionary_separate->
				no_display_name_list );

	return lookup_statistic_input;
}

LOOKUP_STATISTIC_INPUT *lookup_statistic_input_calloc( void )
{
	LOOKUP_STATISTIC_INPUT *lookup_statistic_input;

	if ( ! ( lookup_statistic_input =
			calloc( 1,
				sizeof ( LOOKUP_STATISTIC_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_statistic_input;
}

int lookup_statistic_table_blank_count(
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count )
{
	return
	4 +
	lookup_statistic_after_minimum_blank_count +
	lookup_statistic_after_range_blank_count;
}

int lookup_statistic_after_minimum_blank_count( void )
{
	return 4;
}

int lookup_statistic_after_range_blank_count(
		char *outlier_low_heading_string,
		char *outlier_high_heading_string )
{
	int blank_count = 3;

	if ( outlier_low_heading_string ) blank_count++;
	if ( outlier_high_heading_string ) blank_count++;

	return blank_count;
}

LIST *lookup_statistic_input_attribute_number_name_list(
		LIST *folder_attribute_number_name_list,
		LIST *no_display_name_list )
{
	return
	list_subtract(
		folder_attribute_number_name_list /* list */,
		no_display_name_list /* subtract_this */ );
}

LIST *lookup_statistic_input_attribute_date_name_list(
		LIST *folder_attribute_date_name_list,
		LIST *no_display_name_list )
{
	return
	list_subtract(
		folder_attribute_date_name_list /* list */,
		no_display_name_list /* subtract_this */ );
}

int lookup_statistic_input_attribute_number_count(
	LIST *lookup_statistic_input_attribute_number_name_list )
{
	return
	list_length(
		lookup_statistic_input_attribute_number_name_list );
}

boolean lookup_statistic_table_outlier_boolean( int attribute_number_count )
{
	return
	( attribute_number_count == 1 );
}

