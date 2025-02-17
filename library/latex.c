/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/latex.c					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "String.h"
#include "float.h"
#include "latex.h"

LATEX *latex_new(
		char *tex_filename,
		char *working_directory,
		boolean landscape_boolean,
		char *logo_filename )
{
	LATEX *latex;

	if ( !tex_filename
	||   !working_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex = latex_calloc();

	latex->directory_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		latex_directory_filename(
			tex_filename,
			working_directory );

	latex->documentclass =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		latex_documentclass(
			landscape_boolean );

	latex->usepackage =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		latex_usepackage();

	latex->footer_declaration =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		latex_footer_declaration(
			date_now16( date_utc_offset() ),
			LATEX_PBJ );

	latex->begin_document =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		latex_begin_document();

	if ( timlib_file_exists( logo_filename ) )
	{
		latex->logo_display =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			latex_logo_display(
				logo_filename );
	}

	latex->end_document =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		latex_end_document();

	return latex;
}

char *latex_directory_filename(
		char *tex_filename,
		char *working_directory )
{
	static char directory_filename[ 256 ];

	if ( !tex_filename
	||   !working_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(directory_filename,
		"%s/%s",
		working_directory,
		tex_filename );

	return directory_filename;
}

void latex_tex2pdf(
		char *tex_filename,
		char *working_directory )
{
	char system_string[ 1024 ];

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

	sprintf( system_string,
		 "pdflatex %s 1>&2",
		 tex_filename );

/*
	sprintf( system_string,
		 "pdflatex %s 1>/dev/null 2>&1",
		 tex_filename );
	sprintf( system_string,
		 "pdflatex %s 2>/tmp/latex.err 1>&2",
		 tex_filename );
*/

	/* ------------------------------------ */
	/* Need to run it four times because	*/
	/* header widths may need adjustments.	*/
	/* ------------------------------------ */
	if ( system( system_string ) ){}
	if ( system( system_string ) ){}
	if ( system( system_string ) ){}
	if ( system( system_string ) ){}
}

LATEX *latex_calloc( void )
{
	LATEX *latex;

	if ( ! ( latex = calloc( 1, sizeof ( LATEX ) ) ) )
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

LATEX_COLUMN *latex_column_new(
		char *heading_string,
		enum latex_column_enum latex_column_enum,
		int float_decimal_count,
		char *paragraph_size,
		boolean first_column_boolean )
{
	LATEX_COLUMN *latex_column = latex_column_calloc();

	latex_column->heading_string = heading_string;
	latex_column->latex_column_enum = latex_column_enum;
	latex_column->float_decimal_count = float_decimal_count;
	latex_column->paragraph_size = paragraph_size;
	latex_column->first_column_boolean = first_column_boolean;

	latex_column->right_justify_boolean =
		latex_column_right_justify_boolean(
			latex_column_enum );

	latex_column->display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		latex_column_display(
			heading_string );

	return latex_column;
}

LATEX_COLUMN *latex_column_calloc( void )
{
	LATEX_COLUMN *latex_column;

	if ( ! ( latex_column = calloc( 1, sizeof ( LATEX_COLUMN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return latex_column;
}

boolean latex_column_right_justify_boolean(
		enum latex_column_enum latex_column_enum )
{
	if (	latex_column_enum == latex_column_integer
	||	latex_column_enum == latex_column_float )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *latex_column_display( char *heading_string )
{
	char display[ 128 ];

	if ( !heading_string ) heading_string = "";

	string_initial_capital(
		display,
		heading_string );

	return strdup( display );
}

char *latex_column_header_format_line( LIST *latex_column_list )
{
	char format_line[ 256 ];
	char *ptr = format_line;
	LATEX_COLUMN *latex_column;

	ptr += sprintf( ptr, "{|" );

	if ( list_rewind( latex_column_list ) )
	do {
		latex_column = list_get( latex_column_list );

		if ( latex_column->paragraph_size )
		{
			/*
			ptr += sprintf( ptr,
			">{\\RaggedRight\\arraybackslash}p{%s}|",
			latex_column->paragraph_size );
			*/
			ptr += sprintf( ptr,
			"p{%s}|",
			latex_column->paragraph_size );
		}
		else
		if ( latex_column->right_justify_boolean )
		{
			ptr += sprintf( ptr, "r|" );
		}
		else
		{
			ptr += sprintf( ptr, "l|" );
		}

	} while ( list_next( latex_column_list ) );

	ptr += sprintf( ptr, "} \\hline" );

	return strdup( format_line );
}

char *latex_column_header_text_line( LIST *latex_column_list )
{
	char text_line[ 1024 ];
	char *ptr = text_line;
	LATEX_COLUMN *latex_column;
	boolean first_boolean;

	*ptr = '\0';

	if ( list_rewind( latex_column_list ) )
	do {
		latex_column = list_get( latex_column_list );

		first_boolean =
			list_first_boolean(
				latex_column_list );

		if ( !first_boolean ) ptr += sprintf( ptr, " & " );

		ptr += sprintf( ptr,
			"%s",
			latex_column->display );

	} while ( list_next( latex_column_list ) );

	ptr += sprintf( ptr, "\\\\[0.5ex]" );

	return strdup( text_line );
}

LATEX_ROW *latex_row_new( LIST *cell_list )
{
	LATEX_ROW *latex_row;

	if ( !list_length( cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "cell_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_row = latex_row_calloc();
	latex_row->cell_list = cell_list;

	return latex_row;
}

LATEX_ROW *latex_row_calloc( void )
{
	LATEX_ROW *latex_row;

	if ( ! ( latex_row = calloc( 1, sizeof ( LATEX_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return latex_row;
}

void latex_row_list_output(
		LIST *latex_row_list,
		FILE *latex_output_file )
{
	LATEX_ROW *latex_row;
	char *display;

	if ( !latex_output_file )
	{
		char message[ 128 ];

		sprintf(message, "latex_output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( latex_row_list ) )
	do {
		latex_row = list_get( latex_row_list );

		if ( latex_row->preceed_double_line_boolean )
		{
			fprintf(latex_output_file,
				"\\hline \\hline\n" );
		}

		display =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			latex_cell_list_display(
				latex_row->cell_list );

		fprintf(latex_output_file,
			"%s\\\\\n",
			display );

		free( display );

		latex_cell_list_free( latex_row->cell_list );

	} while ( list_next( latex_row_list ) );

	list_free( latex_row_list );
}

LATEX_CELL *latex_cell_new(
		LATEX_COLUMN *latex_column,
		boolean first_row_boolean,
		char *datum,
		boolean large_boolean,
		boolean bold_boolean )
{
	LATEX_CELL *latex_cell;
	char *column_delimiter;
	char *markup;
	char *formatted_datum;

	if ( !latex_column )
	{
		char message[ 128 ];

		sprintf(message, "latex_column is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_cell = latex_cell_calloc();

	if ( !datum ) datum = "";

	column_delimiter =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		latex_cell_column_delimiter(
			latex_column->first_column_boolean );

	markup =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		latex_cell_markup(
			large_boolean,
			bold_boolean );

	formatted_datum =
		/* ------------------------------------- */
		/* Returns null, static memory, or datum */
		/* ------------------------------------- */
		latex_cell_formatted_datum(
			datum,
			latex_column->latex_column_enum,
			latex_column->float_decimal_count,
			latex_cell_dollar_sign_boolean(
				latex_column->dollar_sign_boolean,
				first_row_boolean ) );

	latex_cell->display =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		latex_cell_display(
			column_delimiter,
			markup,
			formatted_datum );

	return latex_cell;
}

LATEX_CELL *latex_cell_calloc( void )
{
	LATEX_CELL *latex_cell;

	if ( ! ( latex_cell = calloc( 1, sizeof ( LATEX_CELL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return latex_cell;
}

char *latex_cell_column_delimiter( boolean first_column_boolean )
{
	if ( first_column_boolean )
		return "";
	else
		return " &";
}

char *latex_cell_markup(
		boolean large_boolean,
		boolean bold_boolean )
{
	static char markup[ 128 ];
	char *ptr = markup;

	*ptr = '\0';

	if ( large_boolean ) ptr += sprintf( ptr, "\\large " );

	if ( bold_boolean ) ptr += sprintf( ptr, "\\bf " );

	return markup;
}

char *latex_cell_formatted_datum(
		char *datum,
		enum latex_column_enum latex_column_enum,
		int float_decimal_count,
		boolean dollar_sign_boolean )
{
	if ( !datum || !*datum ) return (char *)0;

	if ( latex_column_enum == latex_column_text )
	{
		datum =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			latex_cell_escape( datum );
	}

	if ( latex_column_enum == latex_column_float )
	{
		static char formatted_datum[ 128 ];

		sprintf(formatted_datum,
			"%s%s",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			latex_cell_dollar_sign( dollar_sign_boolean ),
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_double(
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				string_atof( datum ),
				float_decimal_count ) );

		return formatted_datum;
	}
	else
	{
		return datum;
	}
}

char *latex_cell_dollar_sign( boolean dollar_sign_boolean )
{
	if ( dollar_sign_boolean )
		return "\\$";
	else
		return "";
}

char *latex_cell_display(
		char *latex_cell_column_delimiter,
		char *latex_cell_markup,
		char *latex_cell_formatted_datum )
{
	char display[ 4096 ];

	if ( !latex_cell_column_delimiter
	||   !latex_cell_markup )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(display,
		"%s %s %s",
		latex_cell_column_delimiter,
		(latex_cell_formatted_datum)
			? latex_cell_markup
			: "",
		(latex_cell_formatted_datum)
	     		? latex_cell_formatted_datum
			: "" );

	return strdup( display );
}

char *latex_cell_escape( char *datum )
{
	static char escape[ 2048 ];

	string_strcpy( escape, datum, 2048 );

	string_search_replace( escape, "$", "\\$" );
	string_search_replace( escape, "&", "\\&" );
	string_search_replace( escape, "%", "\\%" );
	string_search_replace( escape, "#", "\\#" );
	string_search_replace( escape, "_", "\\_" );

	return escape;
}

char *latex_cell_list_display( LIST *latex_cell_list )
{
	char display[ 65536 ];
	char *ptr = display;
	LATEX_CELL *latex_cell;

	*ptr = '\0';

	if ( list_rewind( latex_cell_list ) )
	do {
		latex_cell = list_get( latex_cell_list );

		if (	strlen( display ) +
			string_strlen( latex_cell->display ) +
			1 >= 65536 )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				65536 );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( latex_cell->display )
		{
			ptr +=
				sprintf(
					ptr,
					"%s",
					latex_cell->display );
		}

	} while ( list_next( latex_cell_list ) );

	return strdup( display );
}

void latex_cell_list_free( LIST *latex_cell_list )
{
	LATEX_CELL *latex_cell;

	if ( list_rewind( latex_cell_list ) )
	do {
		latex_cell = list_get( latex_cell_list );

		if ( latex_cell->display ) free( latex_cell->display );

	} while ( list_next( latex_cell_list ) );

	list_free( latex_cell_list );
}

LATEX_TABLE *latex_table_new(
		char *title,
		LIST *latex_column_list,
		LIST *latex_row_list )
{
	LATEX_TABLE *latex_table;

	if ( !title
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_table = latex_table_calloc();

	latex_table->latex_column_list = latex_column_list;
	latex_table->latex_row_list = latex_row_list;

	latex_table->begin_longtable =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		latex_table_begin_longtable();

	latex_table->latex_column_header_format_line =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		latex_column_header_format_line(
			latex_column_list );

	latex_table->caption_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		latex_table_caption_display(
			title );

	latex_table->latex_column_header_text_line =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		latex_column_header_text_line(
			latex_column_list );

	latex_table->footer_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		latex_table_footer_display();

	latex_table->end_longtable =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		latex_table_end_longtable();

	return latex_table;
}

LATEX_TABLE *latex_table_calloc( void )
{
	LATEX_TABLE *latex_table;

	if ( ! ( latex_table = calloc( 1, sizeof ( LATEX_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return latex_table;
}

void latex_output_document_head(
		LATEX *latex,
		FILE *latex_output_file )
{
	if ( !latex
	||   !latex_output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( latex_output_file ) fclose( latex_output_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(latex_output_file,
		"%s\n",
		latex->documentclass );

	fprintf(latex_output_file,
		"%s\n",
		latex->usepackage );

	if ( latex->footer_declaration )
	{
		fprintf(latex_output_file,
			"%s\n",
			latex->footer_declaration );
	}

	fprintf(latex_output_file,
		"%s\n",
		latex->begin_document );

	if ( latex->logo_display )
	{
		fprintf(latex_output_file,
			"%s\n",
			latex->logo_display );
	}
}

void latex_table_output_head(
		LATEX_TABLE *latex_table,
		FILE *latex_output_file )
{
	if ( !latex_table
	||   !latex_output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( latex_output_file ) fclose( latex_output_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(latex_output_file,
		"%s",
		latex_table->begin_longtable );

	fprintf(latex_output_file,
		"%s\n",
		latex_table->latex_column_header_format_line );

	fprintf(latex_output_file,
		"%s\n",
		latex_table->caption_display );

	fprintf(latex_output_file,
		"%s\n",
		latex_table->latex_column_header_text_line );

	fprintf(latex_output_file,
		"%s\n",
		latex_table->footer_display );
}

void latex_table_output_row_list(
		LIST *latex_row_list,
		FILE *latex_output_file )
{
	if ( !latex_output_file )
	{
		char message[ 128 ];

		sprintf(message, "latex_output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_row_list_output(
		latex_row_list /* freed */,
		latex_output_file );
}

void latex_table_output_tail(
		char *end_longtable,
		FILE *latex_output_file )
{
	if ( !end_longtable
	||   !latex_output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( latex_output_file ) fclose( latex_output_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(latex_output_file,
		"%s\n",
		(end_longtable) ? end_longtable : "" );
}

void latex_output_document_tail(
		char *end_document,
		FILE *latex_output_file )
{
	if ( !end_document
	||   !latex_output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( latex_output_file ) fclose( latex_output_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(latex_output_file,
		"%s\n",
		end_document );
}

char *latex_documentclass( boolean landscape_boolean )
{
	if ( landscape_boolean )
	{
		return
		"\\documentclass[a4paper,landscape]{report}\n"
		"\\special{landscape}";
	}
	else
	{
		return
		"\\documentclass[a4paper]{report}";
	}
}

char *latex_usepackage( void )
{
	return
	"\\usepackage{graphics}\n"
	"\\usepackage{fancyhdr}\n"
	"\\usepackage[labelformat=empty]{caption}\n"
	"\\usepackage{longtable}\n"
	"\\usepackage[\n"
	"	margin=1.5cm,\n"
	"	vmargin=1.5cm,\n"
	"	nohead]{geometry}";
}

char *latex_footer_declaration(
		char *date_now16,
		char *latex_pbj )
{
	static char footer_declaration[ 256 ];

	if ( !date_now16
	||   !latex_pbj )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(footer_declaration,
		"\\pagestyle{fancy}\n"
		"\\fancyhf{}\n"
		"\\cfoot{%s (%s)}\n"
		"\\rfoot{Page \\thepage}",
		date_now16,
		latex_pbj );

	return footer_declaration;
}

char *latex_begin_document( void )
{
	return "\\begin{document}";
}

char *latex_logo_display( char *logo_filename )
{
	static char logo_display[ 256 ];

	if ( !logo_filename )
	{
		char message[ 128 ];

		sprintf(message, "logo_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(logo_display,
		"\\begin{center}\n"
		"\\includegraphics{%s}\n"
		"\\end{center}\n",
	 	 logo_filename );

	return logo_display;
}

char *latex_table_begin_longtable( void )
{
	return "\\begin{longtable}";
}

char *latex_table_caption_display( char *title )
{
	char buffer[ 128 ];
	char caption_display[ 256 ];

	if ( !title )
	{
		char message[ 128 ];

		sprintf(message, "title is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(caption_display,
		"\\caption {%s}\\\\ \\hline",
		string_initial_capital(
			buffer,
			title ) );

	return strdup( caption_display );
}

char *latex_table_footer_display( void )
{
	return
	"\\hline \\hline \\endhead\n"
	"\\hline \\multicolumn{2}{r}"
	"{\\textit{Continued on next page}} \\\\\n"
	"\\hline\n"
	"\\endfoot\n"
	"\\endlastfoot";
}

char *latex_table_end_longtable( void )
{
	return
	"\n"
	"\\hline\n"
	"\\end{longtable}";
}

char *latex_end_document( void )
{
	return "\\end{document}";
}

LIST *latex_column_float_list(
		LIST *heading_string_list,
		boolean first_column_boolean,
		boolean dollar_sign_boolean )
{
	char *heading_string;
	LIST *list;
	LATEX_COLUMN *latex_column;

	if ( !list_rewind( heading_string_list ) ) return NULL;

	list = list_new();

	do {
		heading_string = list_get( heading_string_list );

		latex_column =
			latex_column_new(
				heading_string,
				latex_column_float /* latex_column_enum */,
				2 /* float_decimal_count */,
				(char *)0 /* paragraph_size */,
				first_column_boolean );

		latex_column->dollar_sign_boolean = dollar_sign_boolean;
		list_set( list, latex_column );
		first_column_boolean = 0;

	} while ( list_next( heading_string_list ) );

	return list;
}

LIST *latex_column_text_list(
		LIST *heading_string_list,
		boolean first_column_boolean,
		boolean right_justify_boolean )
{
	char *heading_string;
	LIST *list;
	LATEX_COLUMN *latex_column;

	if ( !list_rewind( heading_string_list ) ) return NULL;

	list = list_new();

	do {
		heading_string = list_get( heading_string_list );

		latex_column =
			latex_column_new(
				heading_string,
				latex_column_text /* latex_column_enum */,
				0 /* float_decimal_count */,
				(char *)0 /* paragraph_size */,
				first_column_boolean );

		latex_column->right_justify_boolean = right_justify_boolean;
		list_set( list, latex_column );
		first_column_boolean = 0;

	} while ( list_next( heading_string_list ) );

	return list;
}

char *latex_omit_page_numbers( void )
{
	return "\\pagenumbering{gobble}";
}

FILE *latex_output_file( char *directory_filename )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	appaserver_output_file( directory_filename );
}

boolean latex_cell_dollar_sign_boolean(
		boolean dollar_sign_boolean,
		boolean first_row_boolean )
{
	return
	dollar_sign_boolean &&
	first_row_boolean;
}

LATEX_CELL *latex_cell_float_new(
		LATEX_COLUMN *latex_column,
		double value )
{
	if ( !latex_column )
	{
		char message[ 128 ];

		sprintf(message, "latex_column is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_cell_small_new(
		latex_column,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		float_string(
			value,
			latex_column->float_decimal_count )
			/* datum */ );
}

LATEX_CELL *latex_cell_small_new(
		LATEX_COLUMN *latex_column,
		char *datum )
{
	if ( !latex_column )
	{
		char message[ 128 ];

		sprintf(message, "latex_column is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_cell_new(
		latex_column,
		0 /* not first_row_boolean */,
		datum,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );
}

LIST *latex_cell_list(
		const char *calling_function,
		LIST *latex_column_list,
		LIST *data_list )
{
	LIST *list;
	char *datum;
	LATEX_COLUMN *latex_column;

	if ( list_past_end( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"latex_column_list is past the end. Called by %s()",
			calling_function );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* No list_rewind( latex_column_list ) */

	list = list_new();

	if ( list_rewind( data_list ) )
	do {
		datum = list_get( data_list );
		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );

		list_set(
			list,
			latex_cell_small_new(
				latex_column,
				datum ) );

	} while ( list_next( data_list ) );

	return list;
}

void latex_table_output(
		char *tex_filename,
		char *tex_anchor_html,
		char *pdf_anchor_html,
		char *working_directory,
		LATEX *latex,
		LATEX_TABLE *latex_table )
{
	FILE *output_file;

	if ( !tex_filename
	||   !tex_anchor_html
	||   !pdf_anchor_html
	||   !working_directory
	||   !latex
	||   !latex_table )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		latex_output_file(
			latex->directory_filename );

	latex_output_document_head(
		latex,
		output_file );

	latex_table_output_head(
		latex_table,
		output_file );

	latex_table_output_row_list(
		latex_table->latex_row_list /* freed */,
		output_file );

	latex_table_output_tail(
		latex_table->end_longtable,
		output_file );

	latex_output_document_tail(
		latex->end_document,
		output_file );

	fclose( output_file );

	latex_tex2pdf(
		tex_filename,
		working_directory );

	printf( "%s<br>%s\n",
		pdf_anchor_html,
		tex_anchor_html );
}

void latex_table_list_output(
		char *tex_filename,
		char *tex_anchor_html,
		char *pdf_anchor_html,
		char *working_directory,
		LATEX *latex,
		LIST *latex_table_list )
{
	FILE *output_file;
	LATEX_TABLE *latex_table;

	if ( !tex_filename
	||   !tex_anchor_html
	||   !pdf_anchor_html
	||   !working_directory
	||   !latex )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		latex_output_file(
			latex->directory_filename );

	latex_output_document_head(
		latex,
		output_file );

	if ( list_rewind( latex_table_list ) )
	do {
		latex_table = list_get( latex_table_list );

		latex_table_output_head(
			latex_table,
			output_file );

		latex_table_output_row_list(
			latex_table->latex_row_list /* freed */,
			output_file );

		latex_table_output_tail(
			latex_table->end_longtable,
			output_file );

	} while ( list_next( latex_table_list ) );

	latex_output_document_tail(
		latex->end_document,
		output_file );

	fclose( output_file );

	latex_tex2pdf(
		tex_filename,
		working_directory );

	printf( "%s<br>%s\n",
		pdf_anchor_html,
		tex_anchor_html );
}

