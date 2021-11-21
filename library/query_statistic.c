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
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "application.h"
#include "query_statistic.h"
#include "html_table.h"

QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
	query_statistic_attribute_aggregate_read_file(
			boolean is_date,
			char *output_filename )
{
	char input_buffer[ 256 ];
	char label[ 128 ];
	char data[ 128 ];
	FILE *file;
	QUERY_ATTRIBUTE_STATISTIC_AGGREGATE *
		query_attribute_statistic_aggregate =
			query_attribute_statistic_aggregate_calloc();

	if ( ! ( file = fopen( output_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			output_filename );
		exit( 1 );
	}

	while( get_line( input_buffer, file ) )
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
			if (	strcmp( label,
				STATISTICS_WEIGHTED_AVERAGE ) == 0 )
			{
				query_attribute_statistics->average =
					atof( data );
			}
			else
			if (	strcmp( label,
				STATISTICS_WEIGHTED_SUM ) == 0 )
			{
				query_attribute_statistics->sum =
					atof( data );
			}
			else
			if (	strcmp( label,
				STATISTICS_WEIGHTED_AVERAGE ) == 0 )
			{
				query_attribute_statistics->average =
					atof( data );
			}
			else
			if (
			strcmp( label,
				STATISTICS_WEIGHTED_STANDARD_DEVIATION )
				== 0 )
			{
				query_attribute_statistics->
					standard_deviation =
					atof( data );
			}
			else
			if (
			strcmp( label,
			STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION )
				== 0 )
			{
				query_attribute_statistics->
					coefficient_of_variation =
					atof( data );
			}
			else
			if (	strcmp( label,
				STATISTICS_WEIGHTED_MINIMUM ) == 0 )
			{
				query_attribute_statistics->minimum =
					atof( data );
			}
			else
			if (	strcmp( label,
					STATISTICS_WEIGHTED_MEDIAN ) == 0 )
			{
				query_attribute_statistics->median =
					atof( data );
			}
			else
			if (	strcmp( label,
					STATISTICS_WEIGHTED_MAXIMUM ) == 0 )
			{
				query_attribute_statistics->maximum =
					atof( data );
			}
			else
			if (	strcmp( label,
					STATISTICS_WEIGHTED_RANGE ) == 0 )
			{
				query_attribute_statistics->range =
					atof( data );
			}
			else
			if (	strcmp( label,
					STATISTICS_WEIGHTED_COUNT ) == 0 )
			{
				query_attribute_statistics->count =
					atof( data );
			}
			else
			if (	strcmp( label,
					STATISTICS_WEIGHTED_PERCENT_MISSING )
					== 0 )
			{
				query_attribute_statistics->
					percent_missing =
					atof( data );
			}
		}
		else
		{
			if (	strcmp( label,
				DATE_MINIMUM ) == 0 )
			{
				query_attribute_statistics->begin_date =
					strdup( data );
			}
			else
			if (	strcmp( label,
				DATE_MAXIMUM ) == 0 )
			{
				query_attribute_statistics->end_date =
					strdup( data );
			}
			else
			if (	strcmp( label,
				DATE_COUNT ) == 0 )
			{
				query_attribute_statistics->count =
					atoi( data );
			}
			else
			if (	strcmp( label,
				DATE_PERCENT_MISSING )
				== 0 )
			{
				query_attribute_statistics->
					percent_missing =
					atof( data );
			}
		}
	}

	fclose( file );

	return query_attribute_statistic_aggregate;
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

char *query_statistic_attribute_html(
			char *folder_attribute_name,
			boolean is_date,
			QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
				query_statistic_attribute_aggregate )
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

QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
	query_statistic_attribute_aggregate_calloc(
			void )
{
	QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
		query_statistic_attribute_aggregate;

	if ( ! ( query_statistic_attribute_aggregate =
		calloc( 1, sizeof( QUERY_STATISTIC_ATTRIBUTE_AGGREGATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_statistic_attribute_aggregate;
}

QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
	query_statistic_attribute_aggregate_new(
			boolean is_date,
			int piece_offset,
			LIST *record_list )
{
	QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
		query_statistic_attribute_aggregate;

	char *outut_filename = timlib_temp_filename();

	FILE *output_pipe =
		query_statistic_attribute_aggregate_output_pipe(
			is_date,
			output_filename );

	query_statistic_attribute_aggregate_generate(
		output_pipe,
		piece_offset,
		record_list );

	pclose( output_pipe );

	query_statistic_attribute_aggregate =
		query_statistic_attribute_aggregate_file_read(
			is_date,
			output_filename );

	timlib_remove_file( output_filename );

	return query_statistic_attribute_aggregate;
}

FILE *query_statistic_attribute_aggregate_output_pipe(
			boolean is_date,
			char *output_filename )
{
	char system_string[ 1024 ];

	if ( !is_date )
	{
		sprintf(system_string,
			"statistics_weighted.e >%s"
			output_filename );
	}
	else
	{
		sprintf(system_string,
			"date_min_max.e >%s",
			output_filename );
	}

	return popen( system_string, "w" );
}

void query_statistic_attribute_aggregate_generate(
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

QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
	query_statistic_attribute_aggregate_file_read(
			boolean is_date,
			char *output_filename )
{
	QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
		query_statistic_attribute_aggregate =
			query_statistic_attribute_aggregate_calloc();
}

