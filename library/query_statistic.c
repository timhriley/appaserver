/* $APPASERVER_HOME/library/query_statistic.c				*/
/* -------------------------------------------------------------------- */
/* This ADT supports the statistics push button on the prompt screens.  */
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "application.h"
#include "query_statistic.h"
#include "html_table.h"

QUERY_STATISTIC_AGGREGATE *
	query_statistic_aggregate_read(
			boolean is_date,
			char *aggregate_filename )
{
	char input_buffer[ 256 ];
	char label[ 128 ];
	char data[ 128 ];
	FILE *file;

	QUERY_ATTRIBUTE_STATISTIC_AGGREGATE *q =
		query_attribute_statistic_aggregate_calloc();

	if ( ! ( file = fopen( aggregate_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			aggregate_filename );
		exit( 1 );
	}

	while( string_input( input_buffer, file, 256 ) )
	{
		if ( character_count( ':', input_buffer ) < 1 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid input with (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			continue;
		}

		piece( label, ':', input_buffer, 0 );
		piece( data, ':', input_buffer, 1 );

		if ( !is_date )
		{
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_AVERAGE ) == 0 )
			{
				q->average = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_SUM ) == 0 )
			{
				q->sum = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_AVERAGE ) == 0 )
			{
				q->average = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_STANDARD_DEVIATION ) == 0 )
			{
				q->standard_deviation = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION )
					== 0 )
			{
				q->coefficient_of_variation = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_MINIMUM ) == 0 )
			{
				q->minimum = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_MEDIAN ) == 0 )
			{
				q->median = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_MAXIMUM ) == 0 )
			{
				q->maximum = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_RANGE ) == 0 )
			{
				q->range = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_COUNT ) == 0 )
			{
				q->count = atof( data );
			}
			else
			if ( strcmp(
				label,
				STATISTICS_WEIGHTED_PERCENT_MISSING ) == 0 )
			{
				q-> percent_missing = atof( data );
			}
		}
		else
		{
			if ( strcmp(
				label,
				DATE_MINIMUM ) == 0 )
			{
				q->begin_date = strdup( data );
			}
			else
			if ( strcmp(
				label,
				DATE_MAXIMUM ) == 0 )
			{
				q->end_date = strdup( data );
			}
			else
			if ( strcmp(
				label,
				DATE_COUNT ) == 0 )
			{
				q->count = atoi( data );
			}
			else
			if ( strcmp(
				label,
				DATE_PERCENT_MISSING ) == 0 )
			{
				q->percent_missing = atof( data );
			}
		}
	}

	fclose( file );

	return q;
}

LIST *query_statistics_select_attribute_name_list(
			char *application_name,
			LIST *query_statistics )
{
	QUERY_ATTRIBUTE_STATISTICS *query_attribute_statistics;
	char folder_attribute[ 1024 ];
	LIST *select_attribute_name_list;

	if ( !list_rewind( query_statistics ) ) return (LIST *)0;

	select_attribute_name_list = list_new();

	do {
		query_attribute_statistics =
			(QUERY_ATTRIBUTE_STATISTICS *)
				list_get(
					query_statistics );

		if ( !query_attribute_statistics->folder_name )
		{
			list_append_pointer(	select_attribute_name_list,
						query_attribute_statistics->
							attribute_name );
		}
		else
		{
			sprintf( folder_attribute,
				 "%s.%s",
				 get_table_name(
					application_name,
				 	query_attribute_statistics->
						folder_name ),
				 query_attribute_statistics->attribute_name );

			list_append_pointer(	select_attribute_name_list,
						strdup( folder_attribute ) );
		}

	} while( list_next( query_statistics ) );

	return select_attribute_name_list;

}

char *query_statistics_build_each_temp_file_sys_string(
			char *application_name,
			char *folder_name,
			LIST *query_statistics,
			char *where_clause )
{
	QUERY_ATTRIBUTE_STATISTICS *query_attribute_statistics;
	LIST *select_attribute_name_list;
	char sys_string[ QUERY_WHERE_BUFFER ];
	char *table_name;
	char *ptr = sys_string;

	table_name = get_multi_table_name( application_name, folder_name );

	select_attribute_name_list =
		query_statistics_select_attribute_name_list(
			application_name,
			query_statistics );

	if ( !select_attribute_name_list ) return (char *)0;

	list_rewind( query_statistics );

	if ( where_clause && *where_clause )
	{
		ptr += sprintf(
			ptr,
		 	"echo 	      \"select %s			     "
		 	"	 	from %s				     "
		 	"	 	where %s;\"		            |"
		 	"sql_quick.e '%c'		     		    |",
		 	list_display_delimited(
				select_attribute_name_list, ',' ),
			table_name,
			where_clause,
			QUERY_ATTRIBUTE_STATISTICS_DELIMITER );
	}
	else
	{
		ptr += sprintf(
			ptr,
		 	"echo 	      \"select %s			     "
		 	"	 	from %s;\"			    |"
		 	"sql_quick.e '%c'		     		    |",
		 	list_display_delimited(
				select_attribute_name_list, ',' ),
			table_name,
			QUERY_ATTRIBUTE_STATISTICS_DELIMITER );
	}

	do {
		query_attribute_statistics =
			list_get(
				query_statistics );

		if ( query_attribute_is_valid_non_date_datatype(
			query_attribute_statistics->attribute_datatype ) )
		{
			ptr += sprintf(
				ptr,
				"tee_process.e 				 "
				"	\"piece.e '%c' %d 	 	|"
				"	  statistics_weighted.e '%c' 	|"
				"	  cat > %s\"			|",
				QUERY_ATTRIBUTE_STATISTICS_DELIMITER,
				query_attribute_statistics->input_piece,
				QUERY_ATTRIBUTE_STATISTICS_DELIMITER,
				query_attribute_statistics->
					attribute_statistics_temp_filename );
		}
		else
		if ( query_attribute_is_valid_date_datatype(
					query_attribute_statistics->
						attribute_datatype ) )
		{
			ptr += sprintf(
				ptr,
				"tee_process.e 				 "
				"	\"date_min_max.e %d '%c' '%s' 	|"
				"	  cat > %s\"		|",
				query_attribute_statistics->input_piece,
				QUERY_ATTRIBUTE_STATISTICS_DELIMITER,
				"" /* database_management_system */,
				query_attribute_statistics->
					attribute_statistics_temp_filename );
		}
	} while( list_next( query_statistics ) );

	sprintf( ptr, "cat > /dev/null" );

	return strdup( sys_string );

}

void query_statistics_output_folder_count(
			char *application_name,
			char *folder_name,
			char *where_clause,
			LIST *query_statistics )
{
	HTML_TABLE *html_table;
	char title[ 65536 ];
	char buffer[ 1024 ];
	LIST *heading_list;
	QUERY_ATTRIBUTE_STATISTICS *query_attribute_statistics;
	char count[ 128 ];
	char display_where_clause[ QUERY_WHERE_BUFFER ];

	query_attribute_display_where_clause(
		display_where_clause,
		where_clause,
		application_name,
		folder_name );

	sprintf(title, 
		"%s: %s",
		format_initial_capital( buffer, folder_name ),
		display_where_clause );

	html_table = new_html_table(
			title,
			(char *)0 /* sub_title */ );

	html_table_set_number_left_justified_columns(
			html_table, 1 );

	html_table_set_number_right_justified_columns(
			html_table, 9 );

	heading_list = new_list();
	list_append_pointer( heading_list, "Count" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );


	if ( !list_length( query_statistics ) ) return;

	query_attribute_statistics =
			list_first_pointer(
				query_statistics );

	if ( query_attribute_statistics->count != -1 )
	{
		sprintf( count,
		 	"%ld",
		 	query_attribute_statistics->count );

		html_table_set_data(	html_table->data_list,
					strdup( count ) );
	}
	else
	{
		html_table_set_data(	html_table->data_list,
					strdup( "" ) );
	}

	html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );


	html_table_close();

}

char *query_statistic_html(
			LIST *query_statistic_attribute_list )
{
	HTML_TABLE *html_table;
	char title[ QUERY_WHERE_BUFFER ];
	char buffer[ 1024 ];
	LIST *heading_list;
	QUERY_ATTRIBUTE_STATISTICS *query_attribute_statistics;
	char count[ 128 ];
	char sum[ 128 ];
	char average[ 128 ];
	char standard_deviation[ 128 ];
	char coefficient_of_variation[ 128 ];
	char minimum[ 128 ];
	char median[ 128 ];
	char maximum[ 128 ];
	char range[ 128 ];
	char percent_missing[ 128 ];
	char local_where_clause[ QUERY_WHERE_BUFFER ];
	char remove_name_string[ 1024 ];
	char attribute_buffer[ 128 ];

	strcpy( local_where_clause, where_clause );
	search_replace_string( 
		local_where_clause, "1 = 1 and ", "" );

	search_replace_string( 
		local_where_clause, "1 = 1", "entire folder" );

	sprintf(remove_name_string,
		"%s.",
		get_table_name( application_name,
				folder_name ) );

	search_replace_string(
		local_where_clause,
		remove_name_string,
		"" );

	sprintf( remove_name_string, "%s.", folder_name );

	search_replace_string(	local_where_clause,
				remove_name_string,
				"" );

	sprintf(title,
		"%s: %s",
		format_initial_capital( buffer, folder_name ),
		local_where_clause );

	html_table =
		new_html_table(
			title,
			(char *)0 /* sub_title */ );

	html_table_set_number_left_justified_columns(
			html_table, 1 );

	html_table_set_number_right_justified_columns(
			html_table, 10 );

	heading_list = new_list();
	list_append_pointer( heading_list, "Attribute" );
	list_append_pointer( heading_list, "Count" );
	list_append_pointer( heading_list, "Sum" );
	list_append_pointer( heading_list, "Average" );
	list_append_pointer( heading_list, "Minimum" );
	list_append_pointer( heading_list, "Median" );
	list_append_pointer( heading_list, "Maximum" );
	list_append_pointer( heading_list, "Range" );
	list_append_pointer( heading_list, "Standard Deviation" );
	list_append_pointer( heading_list, "Coefficient of Variation" );
	list_append_pointer( heading_list, "Percent Missing" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );


	if ( !list_rewind( query_statistics ) ) return;

	do {
		query_attribute_statistics =
			list_get(
				query_statistics );

		if ( query_attribute_statistics->units_string
		&&   *query_attribute_statistics->units_string )
		{
			if ( *query_attribute_statistics->
					units_string == '(' )
			{
				sprintf(attribute_buffer,
				"%s %s",
				query_attribute_statistics->attribute_name,
				query_attribute_statistics->units_string );
			}
			else
			{
				sprintf(attribute_buffer,
				"%s (%s)",
				query_attribute_statistics->attribute_name,
				query_attribute_statistics->units_string );
			}
		}
		else
		{
			if ( query_attribute_statistics->primary_key )
			{
				sprintf( attribute_buffer,
					 "*%s",
					 query_attribute_statistics->
						attribute_name );
			}
			else
			{
				strcpy( attribute_buffer, 
					query_attribute_statistics->
						attribute_name );
			}
		}

		format_initial_capital(
				attribute_buffer,
				attribute_buffer );

		html_table_set_data( html_table->data_list,
				     strdup( attribute_buffer ) );
	
		if ( query_attribute_statistics->count == -1 )
		{
			strcpy( count, "" );
			strcpy( percent_missing, "" );
		}
		else
		{
/*
			sprintf( count,
			 	"%ld",
			 	query_attribute_statistics->count );
*/
			sprintf( count,
				 "%s",
				 place_commas_in_unsigned_long(
					query_attribute_statistics->count ) );

			sprintf( percent_missing,
			 	"%.3lf",
			 	query_attribute_statistics->percent_missing );
		}
	
		if ( strcmp( 	query_attribute_statistics->attribute_datatype,
				"float" ) == 0
		||   strcmp( 	query_attribute_statistics->attribute_datatype,
				"reference_number" ) == 0
		||   strcmp( 	query_attribute_statistics->attribute_datatype,
				"integer" ) == 0 )
		{
			sprintf( average,
				 "%.5lf",
				 query_attribute_statistics->average );
	
			sprintf( sum,
				 "%s",
				 place_commas_in_double(
					query_attribute_statistics->sum ) );

			if ( query_attribute_statistics->count
			&&   query_attribute_statistics->sum
			&&   !query_attribute_statistics->minimum
			&&   !query_attribute_statistics->median
			&&   !query_attribute_statistics->maximum )
			{
				strcpy( minimum, "Overflow" );
				strcpy( median, "Overflow" );
				strcpy( maximum, "Overflow" );
				strcpy( range, "Overflow" );
				strcpy( standard_deviation, "Overflow" );
				strcpy( coefficient_of_variation, "Overflow" );
			}
			else
			{
				sprintf( minimum,
					 "%s",
				 	 place_commas_in_double(
						query_attribute_statistics->
							minimum ) );
		
				sprintf( median,
					 "%s",
				 	 place_commas_in_double(
						query_attribute_statistics->
							median ) );
		
				sprintf( maximum,
					 "%s",
				 	 place_commas_in_double(
						query_attribute_statistics->
							maximum ) );
		
				sprintf( range,
					 "%s",
				 	 place_commas_in_double(
						query_attribute_statistics->
							range ) );
		
				sprintf( standard_deviation,
					 "%.2lf",
					 query_attribute_statistics->
						standard_deviation );
		
				sprintf( coefficient_of_variation,
					 "%.2lf",
					 query_attribute_statistics->
						coefficient_of_variation );
			}
	
			html_table_set_data(	html_table->data_list,
						strdup( count ) );
			html_table_set_data(	html_table->data_list,
						strdup( sum ) );
			html_table_set_data(	html_table->data_list,
						strdup( average ) );
			html_table_set_data(	html_table->data_list,
						strdup( minimum ) );
			html_table_set_data(	html_table->data_list,
						strdup( median ) );
			html_table_set_data(	html_table->data_list,
						strdup( maximum ) );
			html_table_set_data(	html_table->data_list,
						strdup( range ) );
			html_table_set_data(	html_table->data_list,
						strdup( standard_deviation ) );
			html_table_set_data(	html_table->data_list,
						strdup(
						coefficient_of_variation ) );
			html_table_set_data(
					html_table->data_list,
					strdup( percent_missing ) );
		}
		else
		{
			html_table_set_data(	html_table->data_list,
						strdup( count ) );
			html_table_set_data(	html_table->data_list,
						"" );
			html_table_set_data(	html_table->data_list,
						"" );
			html_table_set_data(	html_table->data_list,
						strdup(
						query_attribute_statistics->
							begin_date ) );
			html_table_set_data(	html_table->data_list,
						"" );
			html_table_set_data(	html_table->data_list,
						strdup(
						query_attribute_statistics->
							end_date ) );
			html_table_set_data(	html_table->data_list,
						"" );
			html_table_set_data(	html_table->data_list,
						"" );
			html_table_set_data(	html_table->data_list,
						"" );
			html_table_set_data(
					html_table->data_list,
					strdup( percent_missing ) );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		html_table->data_list = list_new();

	} while( list_next( query_statistics ) );

	html_table_close();
}

void query_attribute_display_where_clause(
		char *display_where_clause,
		char *where_clause,
		char *application_name,
		char *folder_name )
{
	char remove_folder_name_string[ 1024 ];

	strcpy( display_where_clause, where_clause );
	search_replace_string( 
		display_where_clause, "1 = 1 and ", "" );

	search_replace_string( 
		display_where_clause, "1 = 1", "entire folder" );

	sprintf( remove_folder_name_string, "%s_%s.",
		 application_name,
		 folder_name );

	search_replace_string(	display_where_clause,
				remove_folder_name_string,
				"" );

	sprintf( remove_folder_name_string, "%s.", folder_name );

	search_replace_string(	display_where_clause,
				remove_folder_name_string,
				"" );
}

QUERY_STATISTIC_AGGREGATE *
	query_statistic_aggregate_calloc(
			void )
{
	QUERY_STATISTIC_AGGREGATE *
		query_statistic_aggregate;

	if ( ! ( query_statistic_aggregate =
			calloc(
			     1,
			     sizeof( QUERY_STATISTIC_AGGREGATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_statistic_aggregate;
}

QUERY_STATISTIC_AGGREGATE *
	query_statistic_aggregate_new(
			boolean is_date,
			int piece_offset,
			LIST *record_list )
{
	QUERY_STATISTIC_AGGREGATE *
		query_statistic_aggregate;

	char *aggregate_filename = timlib_temp_filename();

	FILE *output_pipe =
		query_statistic_aggregate_output_pipe(
			is_date,
			aggregate_filename );

	query_statistic_aggregate_generate(
		output_pipe,
		piece_offset,
		record_list );

	pclose( output_pipe );

	query_statistic_aggregate =
		query_statistic_aggregate_read(
			is_date,
			aggregate_filename );

	timlib_remove_file( aggregate_filename );

	return query_statistic_aggregate;
}

FILE *query_statistic_aggregate_output_pipe(
			boolean is_date,
			char *aggregate_filename )
{
	char system_string[ 1024 ];

	if ( !is_date )
	{
		sprintf(system_string,
			"statistics_weighted.e >%s"
			aggregate_filename );
	}
	else
	{
		sprintf(system_string,
			"date_min_max.e >%s",
			aggregate_filename );
	}

	return popen( system_string, "w" );
}

void query_statistic_aggregate_generate(
			FILE *output_pipe,
			int piece_offset,
			LIST *record_list )
{
	char value[ 128 ];

	if ( !list_rewind( record_list ) ) return;

	do {
		if ( !piece(	value,
				SQL_DELIMITER,
				list_get( record_list ),
				piece_offset ) )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: piece(%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				piece_offset );
			continue;
		}

		fprintf( output_pipe, "%s\n", value );

	} while ( list_next( record_list ) );
}

QUERY_STATISTIC_ATTRIBUTE *query_statistic_attribute_calloc( void )
{
	QUERY_STATISTIC_ATTRIBUTE *
		query_statistic_attribute;

	if ( ! ( query_statistic_attribute =
			calloc(
				1,
				sizeof( QUERY_STATISTIC_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_statistic_attribute;
}

QUERY_STATISTIC_ATTRIBUTE *
	query_statistic_attribute_new(
			char *folder_attribute_name,
			boolean is_date,
			int piece_offset,
			LIST *record_list )
{
	QUERY_STATISTIC_ATTRIBUTE *query_statistic_attribute =
		query_statistic_attribute_calloc();

	return query_statistic_attribute;
}

LIST *query_statistic_attribute_list(
			LIST *folder_attribute_number_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_date_time_name_list,
			LIST *query_statistic_record_list )
{
	QUERY_STATISTIC_ATTRIBUTE *query_statistic_attribute;
	LIST *attribute_list = {0};
	int piece_offset = 0;

	if ( list_rewind( folder_attribute_number_name_list ) )
	{
		do {
			query_statistic_attribute =
				query_statistic_attribute_new(
					list_get(
					   folder_attribute_number_name_list ),
					0 /* not is_date */,
					piece_offset++,
					query_statistic_record_list );

			list_set(
				attribute_list,
				query_statistic_attribute );

		} while ( list_next( folder_attribute_number_name_list ) );
	}

	if ( list_rewind( folder_attribute_date_name_list ) )
	{
		do {
			query_statistic_attribute =
				query_statistic_attribute_new(
					list_get(
					   folder_attribute_date_name_list ),
					1 /* is_date */,
					piece_offset++,
					query_statistic_record_list );

			list_set(
				attribute_list,
				query_statistic_attribute );

		} while ( list_next( folder_attribute_date_name_list ) );
	}

	if ( list_rewind( folder_attribute_date_time_name_list ) )
	{
		do {
			query_statistic_attribute =
				query_statistic_attribute_new(
					list_get(
					 folder_attribute_date_time_name_list ),
					1 /* is_date */,
					piece_offset++,
					query_statistic_record_list );

			list_set(
				attribute_list,
				query_statistic_attribute );

		} while ( list_next( folder_attribute_date_time_name_list ) );
	}

	return attribute_list;
}

char *query_statistic_attribute_html(
			char *folder_attribute_name,
			boolean is_date,
			QUERY_STATISTIC_AGGREGATE *query_statistic_aggregate )
{
	static char html[ 1024 ];
	char *ptr = html;
	char buffer[ 128 ];
	char minimum[ 128 ];
	char median[ 128 ];
	char maximum[ 128 ];
	char range[ 128 ];
	char standard_deviation[ 128 ];
	char coefficient_of_variation[ 128 ];

	if ( !query_statistic_aggregate )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_statistic_aggregate is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<tr>\n<td align=left>%s\n",
		string_format_capital(
			buffer,
			folder_attribute_name ) );

	ptr += sprintf(
		ptr,
		"<td align=right>%s\n",
		place_commas_in_unsigned_long(
			query_statistic_aggregate->count ) );

	if ( !is_date )
	{
		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			place_commas_in_double(
				query_statistic_aggregate->sum ) );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			place_commas_in_double(
				query_statistic_aggregate->average ) );

		if ( query_statistic_aggregate->count
		&&   query_statistic_aggregate->sum
		&&   !query_statistic_aggregate->minimum
		&&   !query_statistic_aggregate->median
		&&   !query_statistic_aggregate->maximum )
		{
			strcpy( minimum, "Overflow" );
			strcpy( median, "Overflow" );
			strcpy( maximum, "Overflow" );
			strcpy( range, "Overflow" );
			strcpy( standard_deviation, "Overflow" );
			strcpy( coefficient_of_variation, "Overflow" );
		}
		else
		{
			sprintf( minimum,
				 "%s",
			 	 place_commas_in_double(
					query_statistic_aggregate->
						minimum ) );

			sprintf( median,
				 "%s",
			 	 place_commas_in_double(
					query_statistic_aggregate->
						median ) );
		
			sprintf( maximum,
				 "%s",
			 	 place_commas_in_double(
					query_statistic_aggregate->
						maximum ) );
		
			sprintf( range,
				 "%s",
			 	 place_commas_in_double(
					query_statistic_aggregate->
						range ) );
		
			sprintf( standard_deviation,
				 "%.2lf",
				 query_statistic_aggregate->
					standard_deviation );
		
			sprintf( coefficient_of_variation,
				 "%.2lf",
				 query_statistic_aggregate->
					coefficient_of_variation );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			minimum );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			median );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			maximum );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			range );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			standard_deviation );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			coefficient_of_variation );
	}
	else
	{
		/* Skip sum and average */
		/* -------------------- */
		ptr += sprintf( ptr, "<td><td>\n" );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			query_statistic_aggregate->begin_date );

		/* Skip median */
		/* ----------- */
		ptr += sprintf( ptr, "<td>\n" );

		ptr += sprintf(
			ptr,
			"<td align=right>%s\n",
			query_statistic_aggregate->end_date );

		/* Skip range, standard deviation, and coefficient */
		/* ----------------------------------------------- */
		ptr += sprintf( ptr, "<td><td><td>\n" );
	}

	ptr += sprintf(
		ptr,
		"<td align=right>%.3lf\n",
		 query_statistic_aggregate->percent_missing );

	return html;
}

char *query_statistic_title_html(
			char *from_clause,
			char *where_clause )
{
	char buffer[ 1024 ];
	char html[ STRING_ONE_MEG ];
	char *ptr = html;

	ptr += sprintf(
		ptr,
		"<h1>%s: %s</h1>\n",
		string_initial_capital( buffer, from_clause ),
		where_clause );

	search_replace_string(
		html,
		"1 = 1 and ", "" );

	search_replace_string( 
		html,
		"1 = 1", "entire folder" );

	return strdup( html );
}

char *query_statistic_heading_html( void )
{
	static char html[ 1024 ];
	char *ptr = html;

	ptr += sprintf( ptr, "<table border>\n<tr>\n" );
	ptr += sprintf( ptr, "<th align=left>Attribute</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Count</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Sum</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Average</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Minimum</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Median</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Maxium</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Range</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Standard Deviation</th>\n" );
	ptr += sprintf( ptr, "<th align=right>Coefficient of Variation</th>\n");
	ptr += sprintf( ptr, "<th align=right>Percent Missing</th>\n" );

	return html;
}

char *query_statistic_html(
			char *from_clause,
			char *where_clause,
			LIST *query_statistic_attribute_list )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	QUERY_STATISTIC_ATTRIBUTE *query_statistic_attribute;
	char *title_html;

	ptr += sprintf(
		ptr,
		"%s\n",
		( title_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_statistic_title_html(
				from_clause,
				where_clause ) ) );

	free( title_html );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_statistic_heading_html() );

	if ( list_rewind( query_statistic_attribute_list ) )
	{
		do {
			query_statistic_attribute =
				list_get(
					query_statistic_attribute_list );

			ptr += sprintf(
				ptr,
				"%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_statistic_attribute_html(
					query_statistic_attribute->
						folder_attribute_name,
					query_statistic_attribute->
						is_date,
					query_statistic_attribute->
						query_statistic_aggregate ) );

		} while ( list_next( query_statistic_attribute_list ) );
	}

	ptr += sprintf( ptr, "</table>\n" );

	return strdup( html );
}
