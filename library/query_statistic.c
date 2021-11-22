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
#include "statistics_weighted.h"
#include "query_statistic.h"

QUERY_STATISTIC_AGGREGATE *
	query_statistic_aggregate_read(
			boolean is_date,
			char *aggregate_filename )
{
	char input_buffer[ 256 ];
	char label[ 128 ];
	char data[ 128 ];
	FILE *file;

	QUERY_STATISTIC_AGGREGATE *q =
		query_statistic_aggregate_calloc();

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

	char *aggregate_filename =
		timlib_temp_filename(
			(char *)0 /* key */ );

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
			"statistics_weighted.e >%s",
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

	query_statistic_attribute->folder_attribute_name =
		folder_attribute_name;

	query_statistic_attribute->query_statistic_aggregate =
		query_statistic_aggregate_new(
			is_date,
			piece_offset,
			record_list );

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
		string_initial_capital(
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
		}

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

	ptr += sprintf( ptr, "<table border>\n<tr>\n" );

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

QUERY_STATISTIC *query_statistic_calloc( void )
{
	QUERY_STATISTIC *query_statistic;

	if ( ! ( query_statistic = calloc( 1, sizeof( QUERY_STATISTIC ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_statistic;
}

QUERY_STATISTIC *query_statistic_new(
			LIST *folder_attribute_number_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_date_time_name_list,
			char *from_clause,
			char *where_clause )
{
	QUERY_STATISTIC *query_statistic = query_statistic_calloc();

	query_statistic->record_list =
		query_statistic_record_list(
			folder_attribute_number_name_list,
			folder_attribute_date_name_list,
			folder_attribute_date_time_name_list,
			from_clause,
			where_clause );

	query_statistic->attribute_list =
		query_statistic_attribute_list(
			folder_attribute_number_name_list,
			folder_attribute_date_name_list,
			folder_attribute_date_time_name_list,
			query_statistic->record_list );

	query_statistic->html =
		query_statistic_html(
			from_clause,
			where_clause,
			query_statistic->attribute_list );

	return query_statistic;
}

LIST *query_statistic_record_list(
			LIST *folder_attribute_number_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_date_time_name_list,
			char *from_clause,
			char *where_clause )
{
	char system_string[ 4096 ];
	char *select;

	if ( ! ( select =
			query_statistic_select(
				folder_attribute_number_name_list,
				folder_attribute_date_name_list,
				folder_attribute_date_time_name_list ) ) )
	{
		return (LIST *)0;
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		from_clause,
		where_clause );

	return list_pipe_fetch( system_string );
}

char *query_statistic_select(
			LIST *folder_attribute_number_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_date_time_name_list )
{
	static char select[ 4096 ];
	char *ptr = select;

	if ( list_length( folder_attribute_number_name_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s",
			list_display_delimited(
				folder_attribute_number_name_list,
				',' ) );
	}

	if ( list_length( folder_attribute_date_name_list ) )
	{
		if ( ptr != select ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			list_display_delimited(
				folder_attribute_date_name_list,
				',' ) );
	}

	if ( list_length( folder_attribute_date_time_name_list ) )
	{
		if ( ptr != select ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			list_display_delimited(
				folder_attribute_date_time_name_list,
				',' ) );
	}

	if ( ptr == select )
		return (char *)0;
	else
		return select;
}
