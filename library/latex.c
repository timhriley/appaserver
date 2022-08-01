/* $APPASERVER_HOME/library/latex.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "latex.h"

LATEX *latex_new_latex(
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean landscape_flag,
			char *logo_filename )
{
	return latex_new(	tex_filename,
				dvi_filename,
				working_directory,
				landscape_flag,
				logo_filename );
}

LATEX *latex_new(	char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean landscape_flag,
			char *logo_filename )
{
	LATEX *latex;

	if ( !tex_filename
	||   !dvi_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex = latex_calloc();

	latex->full_path =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		latex_full_path(
			tex_filename,
			working_directory );

	if ( ! ( latex->output_stream = fopen( latex->full_path, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			latex->full_path );
		exit( 1 );
	}

	latex->table_list = list_new();

	latex->tex_filename = tex_filename;
	latex->dvi_filename = dvi_filename;
	latex->working_directory = working_directory;
	latex->landscape_flag = landscape_flag;
	latex->logo_filename = logo_filename;

	return latex;
}

char *latex_full_path(	char *tex_filename,
			char *working_directory )
{
	static char full_path[ 256 ];

	if ( working_directory && *working_directory )
	{
		sprintf(full_path,
			"%s/%s",
			working_directory,
			tex_filename );
	}
	else
	{
		strcpy( full_path, tex_filename );
	}

	return full_path;
}

LATEX_TABLE *latex_new_latex_table( char *caption )
{
	return latex_table_new( caption );
}

LATEX_TABLE *latex_table_new( char *caption )
{
	LATEX_TABLE *latex_table = latex_table_calloc();

	latex_table->caption = caption;

	return latex_table;
}

LATEX_TABLE_HEADING *latex_new_latex_table_heading( void )
{
	return
	latex_table_heading_new(
		(char *)0, 0, (char *)0 );
}

LATEX_TABLE_HEADING *latex_table_heading_new(
			char *heading,
			boolean right_justified_flag,
			char *paragraph_size )
{
	LATEX_TABLE_HEADING *latex_table_heading =
		latex_table_heading_calloc();

	latex_table_heading->heading = heading;
	latex_table_heading->right_justified_flag = right_justified_flag;
	latex_table_heading->paragraph_size = paragraph_size;

	return latex_table_heading;
}

LATEX_TABLE_HEADING *latex_table_heading_calloc( void )
{
	LATEX_TABLE_HEADING *latex_table_heading;

	if ( ! ( latex_table_heading =
			calloc( 1, sizeof( LATEX_TABLE_HEADING ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return latex_table_heading;
}

void latex_output_table_footer( FILE *output_stream )
{
	fprintf( output_stream,
		 "\n\\hline\n\\end{tabular}\n" );
}

void latex_output_longtable_footer( FILE *output_stream )
{
	fprintf( output_stream,
		 "\n\\hline\n\\end{longtable}\n" );
}

void latex_output_document_footer( FILE *output_stream )
{
	fprintf( output_stream,
		 "\\end{document}\n" );
}

void latex_output_longtable_document_heading(
			FILE *output_stream,
			boolean landscape_flag,
			boolean table_package_flag,
			char *logo_filename,
			boolean omit_page_numbers,
			char *footline )
{
	if ( landscape_flag )
	{
		fprintf(output_stream,
			"\\documentclass[a4paper,landscape]{report}\n"
			"\\usepackage{graphics}\n" );
		fprintf(output_stream,
			"\\special{landscape}\n" );
	}
	else
	{
		fprintf(output_stream,
			"\\documentclass[a4paper]{report}\n"
			"\\usepackage{graphics}\n" );
	}


	if ( footline && *footline )
	{
		fprintf(output_stream,
			"\\usepackage{fancyhdr}\n" );
	}

	if ( table_package_flag )
	{
		if ( WITH_CAPTION )
		{
			fprintf(
			output_stream,
			"\\usepackage[labelformat=empty]{caption}\n"
			"\\usepackage{longtable}\n"
			"\\usepackage[  margin=1.5cm,"
                			 "vmargin=1.5cm,"
                			 "nohead]{geometry}\n" );
		}
		else
		{
			fprintf(
			output_stream,
			 "\\usepackage{longtable}\n"
			 "\\usepackage[  margin=1.5cm,"
                			 "vmargin=1.5cm,"
                			 "nohead]{geometry}\n" );
		}
	}
 

	if ( footline && *footline )
	{
		fprintf( output_stream,
"\\pagestyle{fancy}\n"
"\\fancyhf{}\n"
"\\cfoot{%s (%s)}\n"
"\\rfoot{Page \\thepage}\n",
			 footline,
			 LATEX_PBJ );
	}
	else
	if ( omit_page_numbers )
	{
		fprintf( output_stream,
			 "\\pagenumbering{gobble}\n" );
	}

	fprintf( output_stream,
		 "\\begin{document}\n" );

	if ( logo_filename
	&&   *logo_filename
	&&   timlib_file_exists( logo_filename ) )
	{
		fprintf( output_stream,
"\\begin{center}\n"
"\\includegraphics{%s}\n"
"\\end{center}\n",
		 	 logo_filename );
	}

}

void latex_output_longtable_heading(	FILE *output_stream,
					char *caption,
					LIST *heading_list )
{
	LATEX_TABLE_HEADING *latex_table_heading;
	int first_time;
	char buffer[ 256 ];

	if ( !WITH_CAPTION )
	{
		if ( caption && *caption )
		{
			fprintf(output_stream,
				"\\begin{center} { \\bf %s} \\end{center}\n",
				escape_character(
					buffer,
					caption,
					'_' /* character_to_escape */ ) );
		}
	}

	fprintf( output_stream,
		 "\\begin{longtable}{|" );

	if ( !list_rewind( heading_list ) ) return;

	do {
		latex_table_heading = list_get_pointer( heading_list );

		if ( latex_table_heading->paragraph_size )
		{
			fprintf(output_stream,
				"p{%s}|",
				latex_table_heading->paragraph_size );
		}
		else
		if ( latex_table_heading->right_justified_flag )
			fprintf( output_stream, "r|" );
		else
			fprintf( output_stream, "l|" );

	} while( list_next( heading_list ) );

	fprintf( output_stream, "} \\hline\n" );

	if ( WITH_CAPTION )
	{
		if ( caption && *caption )
		{
			fprintf(output_stream,
				"\\caption {%s} \\\\ \\hline\n",
				escape_character(
					buffer,
					caption,
					'_' /* character_to_escape */ ) );
		}
	}

	list_rewind( heading_list );

	first_time = 1;
	do {
		latex_table_heading = list_get_pointer( heading_list );

		if ( first_time )
			first_time = 0;
		else
			fprintf( output_stream, " & " );

		if ( latex_table_heading->heading
		&&   *latex_table_heading->heading )
		{
			fprintf( output_stream,
				 "%s",
				 format_initial_capital(
					buffer,
				 	latex_table_heading->heading ) );
		}

	} while( list_next( heading_list ) );

	fprintf( output_stream, "\\\\ \\hline \\hline \\endhead\n" );

	fprintf(output_stream,
"\\hline \\multicolumn{2}{r}{\\textit{Continued on next page}} \\\\\n" );

 	fprintf( output_stream, "\\hline\n" );
	fprintf( output_stream, "\\endfoot\n" );
	fprintf( output_stream, "\\endlastfoot\n" );

}

void latex_output_table_row_list(	FILE *output_stream,
					LIST *row_list,
					int heading_list_length )
{
	int first_time;
	LATEX_COLUMN_DATA *column_data;
	LATEX_ROW *row;
	char buffer[ 1024 ];
	int column_count;

	if ( !row_list || !list_rewind( row_list ) ) return;

	do {
		row = list_get_pointer( row_list );

		if ( !row->column_data_list
		||   !list_rewind( row->column_data_list ) )
		{
			continue;
		}

		if  ( row->preceed_double_line )
		{
			fprintf( output_stream,
				 "\\hline \\hline\n" );
		}

		first_time = 1;
		column_count = 0;

		do {
			column_data = list_get_pointer( row->column_data_list );

			if ( first_time )
				first_time = 0;
			else
				fprintf( output_stream, " & " );

			if ( column_data )
			{
				if  ( row->preceed_double_line
				||    column_data->large_bold )
				{
/*
					fprintf( output_stream,
				 		"\\large \\bf " );
*/
					fprintf( output_stream,
				 		"\\bf " );
				}


				if ( column_data->column_data )
				{
					fprintf(output_stream,
						"%s",
						latex_escape_data(
							buffer,
							column_data->
								column_data,
							1024 ) );
				}
			}

			column_count++;

		} while( list_next( row->column_data_list ) );

		if ( column_count < heading_list_length )
		{
			for( 	;
				column_count < heading_list_length;
				column_count++ )
			{
				fprintf( output_stream, " & " );
			}
		}

		fprintf( output_stream, "\\\\\n" );

	} while( list_next( row_list ) );

}

void latex_output_table_vertical_padding(
				FILE *output_stream,
				int length_heading_list,
				int empty_vertical_rows )
{
	int i;
	int j;

	if ( length_heading_list < 2 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): invalid length_heading_list.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	for( i = 0; i < empty_vertical_rows; i++ )
	{
		for( j = 0; j < length_heading_list - 1; j++  )
		{
			fprintf( output_stream, "&" );
		}
		fprintf( output_stream, "\\\\\n" );
	}

}

LATEX_ROW *latex_new_latex_row( void )
{
	return latex_row_new();
}

LATEX_ROW *latex_row_new( void )
{
	LATEX_ROW *latex_row = latex_row_calloc();

	latex_row->column_data_list = list_new();

	return latex_row;
}

void latex_tex2pdf(	char *tex_filename,
			char *working_directory )
{
	char sys_string[ 1024 ];

	if ( chdir( working_directory ) == -1 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot chdir(%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			working_directory );
		exit( 1 );
	}

	sprintf( sys_string,
		 "pdflatex %s 1>&2",
		 tex_filename );
/*
	sprintf( sys_string,
		 "pdflatex %s 1>/dev/null 2>&1",
		 tex_filename );
	sprintf( sys_string,
		 "pdflatex %s 2>/tmp/latex.err 1>&2",
		 tex_filename );
*/

	/* ------------------------------------ */
	/* Need to run it several times because	*/
	/* header widths may need adjustments.	*/
	/* ------------------------------------ */
	if ( system( sys_string ) );
	if ( system( sys_string ) );
	if ( system( sys_string ) );

}

void latex_longtable_output(	FILE *output_stream,
				boolean landscape_flag,
				LIST *table_list,
				char *logo_filename,
				boolean omit_page_numbers,
				char *footline )
{
	LATEX_TABLE *table;

	latex_output_longtable_document_heading(
		output_stream,
		landscape_flag,
		1 /* table_package_flag */,
		logo_filename,
		omit_page_numbers,
		footline );

	if ( !list_rewind( table_list ) ) return;

	do {
		table = list_get_pointer( table_list );

		latex_output_longtable_heading(
			output_stream,
			table->caption,
			table->heading_list );

		latex_output_table_row_list(
			output_stream,
			table->row_list,
			list_length( table->heading_list ) );

		latex_output_longtable_footer(
			output_stream );

	} while( list_next( table_list ) );

	latex_output_document_footer( output_stream );
}

void latex_output_documentclass_letter(	FILE *output_stream,
					boolean omit_page_numbers )
{
	fprintf( output_stream,
"\\documentclass{letter}\n"
"\\usepackage{graphics}\n"
"\\usepackage[	top=0.25in,\n"
"		left=0.625in,\n"
"		bottom=0.5in,\n"
"		right=1.0in,\n"
"		nohead]{geometry}\n" );

	if ( omit_page_numbers )
	{
		fprintf( output_stream,
			 "\\pagenumbering{gobble}\n" );
	}

	fprintf( output_stream,
		 "\\begin{document}\n" );

}

void latex_output_letterhead_document_heading(
					FILE *output_stream,
					char *logo_filename,
					char *organization_name,
					char *street_address,
					char *city_state_zip,
					char *date_string )
{
	if ( !logo_filename
	||   !*logo_filename
	||   !timlib_file_exists( logo_filename ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: emtpy logo_filename.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fprintf( output_stream,
"\\parbox{1.5in}{\n"
"\\includegraphics{%s}\n"
"}\n",
		 logo_filename );

	fprintf( output_stream,
"\\hfill\n"
"\\begin{tabular}{c}\n"
"\\Large \\bf %s\\\\\n"
"\\normalsize \\bf %s\\\\\n"
"\\normalsize \\bf %s\\\\\n"
"\\normalsize %s\n"
"\\end{tabular}\n",
		 organization_name,
		 street_address,
		 city_state_zip,
		 date_string );

	fprintf( output_stream,
"\\hfill\n"
"\\parbox{2.5in}{ }\n\n" );

}

void latex_output_return_envelope_document_heading(
					FILE *output_stream,
					char *logo_filename,
					char *organization_name,
					char *street_address,
					char *city_state_zip,
					char *date_string )
{
	if ( !logo_filename
	||   !*logo_filename
	||   !timlib_file_exists( logo_filename ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: emtpy logo_filename.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}


	fprintf( output_stream,
"\\begin{tabular}{p{3.0in}c}\n"
"\\vspace{-0.60in}\n"
"\\begin{tabular}{l}\n"
"%s\\\\\n"
"%s\\\\\n"
"%s\n"
"\\end{tabular}\n",
		 organization_name,
		 street_address,
		 city_state_zip );

	fprintf( output_stream,
"&\n" );

	fprintf( output_stream,
"\\begin{tabular}{c}\n"
"\\includegraphics{%s}\\\\"
"%s\n"
"\\end{tabular}\n"
"\\end{tabular}\n\n",
		 logo_filename,
		 date_string );

}

void latex_output_indented_address(	FILE *output_stream,
					char *full_name,
					char *street_address,
					char *city_state_zip )
{
	char full_name_buffer[ 128 ];
	char street_address_buffer[ 128 ];
	char city_state_zip_buffer[ 128 ];

	fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s\\\\\n"
"& %s\\\\\n"
"& %s\n"
"\\end{tabular}\n\n",
		 latex_escape_data(
			full_name_buffer,
		 	full_name,
			128 ),
		 latex_escape_data(
			street_address_buffer,
			street_address,
			128 ),
		 latex_escape_data(
			city_state_zip_buffer,
			city_state_zip,
			128 ) );

}

void latex_output_table_heading(	FILE *output_stream,
					char *caption,
					LIST *heading_list )
{
	LATEX_TABLE_HEADING *latex_table_heading;
	int first_time;
	char buffer[ 128 ];

	if ( !WITH_CAPTION )
	{
		if ( caption && *caption )
		{
			fprintf(output_stream,
				"\\begin{center} { \\bf %s} \\end{center}\n",
				caption );
		}
	}

	fprintf( output_stream,
		 "\\begin{tabular}{|" );

	if ( !list_rewind( heading_list ) ) return;

	do {
		latex_table_heading = list_get_pointer( heading_list );

		if ( latex_table_heading->paragraph_size )
		{
			fprintf(output_stream,
				"p{%s}|",
				latex_table_heading->paragraph_size );
		}
		else
		if ( latex_table_heading->right_justified_flag )
			fprintf( output_stream, "r|" );
		else
			fprintf( output_stream, "l|" );

	} while( list_next( heading_list ) );

	fprintf( output_stream, "} \\hline\n" );

	if ( WITH_CAPTION )
	{
		if ( caption && *caption )
		{
			fprintf(	output_stream,
					"\\caption {%s} \\\\ \\hline\n",
					caption );
		}
	}

	list_rewind( heading_list );

	first_time = 1;
	do {
		latex_table_heading = list_get_pointer( heading_list );

		if ( first_time )
			first_time = 0;
		else
			fprintf( output_stream, " & " );

		if ( latex_table_heading->heading
		&&   *latex_table_heading->heading )
		{
			fprintf( output_stream,
				 "%s",
				 format_initial_capital(
					buffer,
				 	latex_table_heading->heading ) );
		}

	} while( list_next( heading_list ) );

	fprintf( output_stream, "\\\\ \\hline \\hline\n" );

}

char *latex_escape_data(	char *destination,
				char *source,
				int buffer_size )
{
	if ( !source ) return "";

	timlib_strcpy( destination, source, buffer_size );

	search_replace_string(
		destination,
		"#",
		"Number " );

	search_replace_string(
		destination,
		"&",
		"\\&" );

	search_replace_string(
		destination,
		"%",
		"\\%" );

	search_replace_string(
		destination,
		"_",
		"\\_" );

	return destination;

}

void latex_column_data_list_set(
			LIST *column_data_list,
			LIST *data_list )
{
	if ( !list_rewind( data_list ) ) return;

	do {
		latex_column_data_set(
			column_data_list,
			(char *)list_get( data_list ),
			0 /* not large_bold */ );

	} while ( list_next( data_list ) );
}

void latex_column_data_set(
			LIST *column_data_list,
			char *column_data,
			boolean large_bold )
{
	if ( !column_data ) column_data = "";

	list_set(
		column_data_list,
		latex_column_data_new(
			column_data,
			large_bold ) );
}

LIST *latex_table_right_heading_list(
			LIST *heading_list )
{
	LIST *latex_heading_list;

	if ( !list_rewind( heading_list ) ) return (LIST *)0;

	latex_heading_list = list_new();

	do {
		list_set(
			latex_heading_list,
			latex_table_heading_new(
				(char *)list_get( heading_list ),
				1 /* right_justified_flag */,
				(char *)0 /* paragraph_size */ ) );

	} while ( list_next( heading_list ) );

	return latex_heading_list;
}

LATEX_COLUMN_DATA *latex_column_data_calloc( void )
{
	LATEX_COLUMN_DATA *latex_column_data;

	if ( ! ( latex_column_data =
			calloc( 1, sizeof( LATEX_COLUMN_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return latex_column_data;
}

LATEX_ROW *latex_row_calloc( void )
{
	LATEX_ROW *latex_row;

	if ( ! ( latex_row = calloc( 1, sizeof( LATEX_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return latex_row;
}

LATEX_TABLE *latex_table_calloc( void )
{
	LATEX_TABLE *latex_table;

	if ( ! ( latex_table = calloc( 1, sizeof( LATEX_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return latex_table;
}

LATEX *latex_calloc( void )
{
	LATEX *latex;

	if ( ! ( latex = calloc( 1, sizeof( LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return latex;
}

LATEX_COLUMN_DATA *latex_column_data_new(
			char *column_data,
			boolean large_bold )
{
	LATEX_COLUMN_DATA *latex_column_data;

	if ( !column_data )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: column_data is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_column_data = latex_column_data_calloc();

	latex_column_data->column_data = column_data;
	latex_column_data->large_bold = large_bold;

	return latex_column_data;
}

