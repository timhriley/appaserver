/* $APPASERVER_HOME/library/scatterplot.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org	 		*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "scatterplot.h"
#include "application.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "grace_retire.h"
#include "column.h"
#include "regression.h"

SCATTERPLOT *scatterplot_new( void )
{
	SCATTERPLOT *scatterplot;

	if ( ! ( scatterplot = calloc( 1, sizeof ( SCATTERPLOT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s(): cannot allocate memory.\n",
			 __FUNCTION__ );
		exit( 1 );
	}

	return scatterplot;
}

PLOT *scatterplot_plot_new(	char *anchor_entity_datatype,
				char *compare_entity_datatype )
{
	PLOT *plot;

	if ( ! ( plot = calloc( 1, sizeof ( PLOT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s(): cannot allocate memory.\n",
			 __FUNCTION__ );
		exit( 1 );
	}

	plot->anchor_entity_datatype = anchor_entity_datatype;
	plot->compare_entity_datatype = compare_entity_datatype;
	plot->point_list = list_new();
	return plot;
}

POINT *scatterplot_point_new(	double anchor_value,
				double compare_value )
{
	POINT *point;

	if ( ! ( point = calloc( 1, sizeof ( POINT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s(): cannot allocate memory.\n",
			 __FUNCTION__ );
		exit( 1 );
	}
	point->anchor_value = anchor_value;
	point->compare_value = compare_value;
	return point;
}

LIST *scatterplot_get_plot_list(	char *anchor_entity,
					char *anchor_datatype,
					LIST *compare_value_dictionary_list,
					char *sys_string,
					int measurement_date_time_piece,
					int datatype_piece,
					int value_piece,
					char record_delimiter,
					char entity_datatype_delimiter )
{
	LIST *plot_list;
	static char anchor_entity_datatype[ 1024 ];
	DICTIONARY *anchor_value_dictionary;
	COMPARE_VALUE_DICTIONARY *compare_value_dictionary;
	LIST *anchor_key_list;
	char *anchor_key;
	char *anchor_value_string;
	char *compare_value_string;
	PLOT *plot;
	POINT *point;

	sprintf(	anchor_entity_datatype,
			"%s%c%s",
			anchor_entity,
			entity_datatype_delimiter,
			anchor_datatype );

	scatterplot_populate_compare_value_dictionary_list(
			&anchor_value_dictionary,
			compare_value_dictionary_list,		
			anchor_entity_datatype,
			sys_string,
			measurement_date_time_piece,
			datatype_piece,
			value_piece,
			record_delimiter );

	anchor_key_list = dictionary_key_list( anchor_value_dictionary );

	if ( !list_rewind( anchor_key_list ) ) return (LIST *)0;

	plot_list = list_new();

	do {
		anchor_key = list_get_pointer( anchor_key_list );

		anchor_value_string =
			dictionary_pointer(
				anchor_key,
				anchor_value_dictionary );

		list_rewind( compare_value_dictionary_list );

		do {
			compare_value_dictionary =
				list_get_pointer(
					compare_value_dictionary_list );

			if ( !( compare_value_string =
				dictionary_pointer(
					anchor_key,
					compare_value_dictionary->
						dictionary ) ) )
			{
				continue;
			}

			plot =
				scatterplot_get_or_set_plot(
					plot_list,
					anchor_entity_datatype,
					compare_value_dictionary->
						entity_datatype );

			point = scatterplot_point_new(
						atof( anchor_value_string ),
						atof( compare_value_string ) );

			list_append_pointer(	plot->point_list,
						point );

		} while( list_next( compare_value_dictionary_list ) );
	} while( list_next( anchor_key_list ) );

	return plot_list;

}

COMPARE_VALUE_DICTIONARY *compare_value_dictionary_new(
				char *entity_datatype )
{
	COMPARE_VALUE_DICTIONARY *compare_value_dictionary;

	if ( ! ( compare_value_dictionary =
			calloc( 1, sizeof ( COMPARE_VALUE_DICTIONARY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s(): cannot allocate memory.\n",
			 __FUNCTION__ );
		exit( 1 );
	}

	compare_value_dictionary->entity_datatype = entity_datatype;
	compare_value_dictionary->dictionary = dictionary_new();
	return compare_value_dictionary;
}

LIST *scatterplot_get_compare_value_dictionary_list(
			LIST *compare_entity_name_list,
			LIST *compare_datatype_name_list )
{
	char *compare_entity_name;
	char *compare_datatype_name;
	LIST *compare_value_dictionary_list;
	char compare_entity_datatype[ 256 ];
	COMPARE_VALUE_DICTIONARY *compare_value_dictionary;

	if ( !list_length( compare_entity_name_list )
	||	list_length( compare_entity_name_list ) !=
		list_length( compare_datatype_name_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: incongruent entity and datatype lists.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_rewind( compare_entity_name_list );
	list_rewind( compare_datatype_name_list );
	compare_value_dictionary_list = list_new();

	do {
		compare_entity_name =
			list_get_pointer(
				compare_entity_name_list );

		compare_datatype_name =
			list_get_pointer(
				compare_datatype_name_list );

		sprintf( compare_entity_datatype,
			 "%s/%s",
			 compare_entity_name,
			 compare_datatype_name );

		compare_value_dictionary =
			compare_value_dictionary_new(
				strdup( compare_entity_datatype ) );

		list_append_pointer(
			compare_value_dictionary_list,
			compare_value_dictionary );

		list_next( compare_datatype_name_list );
	} while( list_next( compare_entity_name_list ) );

	return compare_value_dictionary_list;
			
}

COMPARE_VALUE_DICTIONARY *scatterplot_compare_value_dictionary_seek(
				char *compare_entity_datatype,
				LIST *compare_value_dictionary_list )
{
	COMPARE_VALUE_DICTIONARY *compare_value_dictionary;

	if ( !list_rewind( compare_value_dictionary_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty compare_value_dictionary_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {

		compare_value_dictionary =
			list_get_pointer(
				compare_value_dictionary_list );

		if ( strcasecmp(compare_value_dictionary->
					entity_datatype,
				compare_entity_datatype ) == 0 )
		{
			return compare_value_dictionary;
		}
	} while( list_next( compare_value_dictionary_list ) );

	fprintf( stderr,
"ERROR in %s/%s()/%d: cannot find (%s) in compare_value_dictionary_list.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__,
		 compare_entity_datatype );
	exit( 1 );
}

PLOT *scatterplot_get_or_set_plot(	LIST *plot_list,
					char *anchor_entity_datatype,
					char *compare_entity_datatype )
{
	PLOT *plot;

	if ( list_rewind( plot_list ) )
	{
		do {
			plot = list_get_pointer( plot_list );

			if ( strcasecmp(plot->compare_entity_datatype,
					compare_entity_datatype ) == 0 )
			{
				return plot;
			}
		} while( list_next( plot_list ) );
	}

	plot = scatterplot_plot_new(
			anchor_entity_datatype,
			compare_entity_datatype );

	list_append_pointer( plot_list, plot );
	return plot;

}

void scatterplot_populate_compare_value_dictionary_list(
			DICTIONARY **anchor_value_dictionary_pointer,
			LIST *compare_value_dictionary_list,		
			char *anchor_entity_datatype,
			char *sys_string,
			int measurement_date_time_piece,
			int datatype_piece,
			int value_piece,
			char record_delimiter )
{
	DICTIONARY *anchor_value_dictionary;
	FILE *input_pipe;
	char measurement_date_time[ 128 ];
	char input_entity_datatype[ 1024 ];
	char value_string[ 128 ];
	COMPARE_VALUE_DICTIONARY *compare_value_dictionary;
	char input_buffer[ 1024 ];

	anchor_value_dictionary = dictionary_new();
	*anchor_value_dictionary_pointer = anchor_value_dictionary;

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date_time,
			record_delimiter,
			input_buffer,
			measurement_date_time_piece );

		piece(	input_entity_datatype,
			record_delimiter,
			input_buffer,
			datatype_piece );

		piece(	value_string,
			record_delimiter,
			input_buffer,
			value_piece );

		if ( strcasecmp(input_entity_datatype,
				anchor_entity_datatype ) == 0 )
		{
			if ( *value_string
			&&   strcmp( value_string, "null" ) != 0 )
			{
				dictionary_set_pointer(
					anchor_value_dictionary,
					strdup( measurement_date_time ),
					strdup( value_string ) );
			}
		}
		else
		{
			compare_value_dictionary =
				scatterplot_compare_value_dictionary_seek(
				input_entity_datatype,
				compare_value_dictionary_list );

			dictionary_set_pointer(
				compare_value_dictionary->dictionary,
				strdup( measurement_date_time ),
				strdup( value_string ) );
		}
	}

	pclose( input_pipe );

}

void scatterplot_output_scatter_plot(
				char *application_name,
				char *sys_string,
				char *anchor_entity,
				char *anchor_datatype,
				char *sub_title,
				char *appaserver_data_directory,
				LIST *compare_entity_name_list,
				LIST *compare_datatype_name_list,
				enum aggregate_level aggregate_level,
				char *anchor_units,
				LIST *compare_datatype_units_list,
				int measurement_date_time_piece,
				int datatype_piece,
				int value_piece,
				char record_delimiter,
				char entity_datatype_delimiter )
{
	char scatterplot_key[ 512 ];
	char grace_scatterplot_filename[ 512 ];
	FILE *scatterplot_pipe;
	FILE *input_file;
	char input_buffer[ 1024 ];
	char ftp_agr_filename[ 512 ];
	char pdf_output_filename[ 512 ];
	char output_filename[ 512 ];
	char statistics_filename[ 512 ];
	PLOT *plot;
	POINT *point;
	char prompt[ 512 ];
	char title[ 512 ];
	char xaxis_label[ 512 ];
	char *compare_datatype_units;
	char yaxis_label[ 512 ];
	SCATTERPLOT *scatterplot;

	scatterplot = scatterplot_new();
	scatterplot->anchor_entity = anchor_entity;
	scatterplot->anchor_datatype = anchor_datatype;
	scatterplot->compare_entity_name_list = compare_entity_name_list;
	scatterplot->compare_datatype_name_list = compare_datatype_name_list;

	scatterplot->compare_value_dictionary_list =
		scatterplot_get_compare_value_dictionary_list(
			scatterplot->compare_entity_name_list,
			scatterplot->compare_datatype_name_list );

	if ( !list_length( scatterplot->compare_value_dictionary_list ) )
	{
		document_basic_output( application_name );

		printf( "<h3>ERROR: please select datatypes to compare.</h3>" );
		document_close();
		exit( 0 );
	}

	scatterplot->plot_list =
		scatterplot_get_plot_list(
				scatterplot->anchor_entity,
				scatterplot->anchor_datatype,
				scatterplot->compare_value_dictionary_list,
				sys_string,
				measurement_date_time_piece,
				datatype_piece,
				value_piece,
				record_delimiter,
				entity_datatype_delimiter );

	if ( !list_length( scatterplot->plot_list ) )
	{
		document_basic_output( application_name );

		printf( "<h3>ERROR: no data retrieved.</h3>" );
		document_close();
		exit( 0 );
	}

	document_basic_output( application_name );

	grace_output_screen_title();

	sprintf(	xaxis_label,
			"%s/%s (%s)",
			scatterplot->anchor_entity,
			scatterplot->anchor_datatype,
			anchor_units );

	format_initial_capital( xaxis_label, xaxis_label );

	list_rewind( scatterplot->plot_list );
	list_rewind( compare_datatype_units_list );

	do {
		plot = list_get_pointer( scatterplot->plot_list );

		compare_datatype_units =
			list_get_pointer( compare_datatype_units_list );

		/* Compare_Station_Datatype has a slash delimiter */
		/* ---------------------------------------------- */
		sprintf( scatterplot_key,
			 "%s_%d",
			 plot->compare_entity_datatype,
			 getpid() );

		search_replace_character( scatterplot_key, '/', '_' );
		search_replace_character( scatterplot_key, ' ', '_' );

		sprintf(grace_scatterplot_filename,
		 	"%s/grace_scatterplot_%s.dat",
		 	appaserver_data_directory,
		 	scatterplot_key );

		sprintf(title,
			"Compare %s values",
			aggregate_level_string( aggregate_level ) );

		format_initial_capital( title, title );

		sprintf(	yaxis_label,
				"%s (%s)",
				plot->compare_entity_datatype,
				compare_datatype_units );

		format_initial_capital( yaxis_label, yaxis_label );

		sprintf(	sys_string,
	"grace_scatterplot.e %s %s \"%s\" \"%s\" \"%s\" \"%s\" > %s",
				application_name,
				scatterplot_key,
				title,
				sub_title,
				xaxis_label,
				yaxis_label,
				grace_scatterplot_filename );

		scatterplot_pipe = popen( sys_string, "w" );

		if ( list_rewind( plot->point_list ) )
		{
			do {
				point =
					list_get_pointer(
						plot->point_list );

				fprintf( scatterplot_pipe,
					 "%.5lf %.5lf\n",
					 point->anchor_value,
					 point->compare_value );

			} while( list_next( plot->point_list ) );
		}

		pclose( scatterplot_pipe );
	
		input_file = fopen( grace_scatterplot_filename, "r" );
		if ( !input_file )
		{
			fprintf( stderr,
				 "%s/%s()/%d: cannot open %s for read.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 grace_scatterplot_filename );
			exit( 1 );
		}
	
		if ( !get_line( input_buffer, input_file ) )
		{
			printf(
		"<h3>Warning: no records were selected for %s.</h3>\n",
				plot->compare_entity_datatype );

			fclose( input_file );
			sprintf( sys_string,
				 "rm -f %s",
				 grace_scatterplot_filename );
			if ( system( sys_string ) ){};
			continue;
		}
	
		column( ftp_agr_filename, 0, input_buffer );

		column( pdf_output_filename, 1, input_buffer );
		column( output_filename, 2, input_buffer );
		column( statistics_filename, 3, input_buffer );

		fclose( input_file );
		sprintf( sys_string, "rm -f %s", grace_scatterplot_filename );
		if ( system( sys_string ) ){};

		sprintf( prompt,
	"<br>&lt;Left Click&gt; for %s chart or &lt;Right Click&gt; to save.",
			 plot->compare_entity_datatype );

		if ( strcmp( ftp_agr_filename, "null" ) != 0 )
		{
			appaserver_library_output_ftp_prompt(
				pdf_output_filename, 
				prompt,
				scatterplot_key    /* target */,
				"application/pdf" );

			appaserver_library_output_ftp_prompt(
				ftp_agr_filename, 
				grace_get_agr_prompt(),
				(char *)0    /* target */,
				"application/agr" );

			grace_output_grace_home_link();
		}
		else
		{
			printf(
		"<H3>Warning: too much data for grace to handle.</H3>\n" );
		}

		regression_output_statistics_table( statistics_filename );

		list_next( compare_datatype_units_list );
	} while( list_next( scatterplot->plot_list ) );

	document_close();

}

