/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/make_checks.c	       	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "timlib.h"
#include "latex.h"
#include "piece.h"
#include "dollar.h"

/* Constants */
/* --------- */
#define CHECK_DATE_COMMAND	"now.sh pretty | column.e 0"
#define LATEX_FILENAME_TEMPLATE	"%s/make_checks_%d.tex"
#define PDF_FILENAME_TEMPLATE	"%s/make_checks_%d.pdf"

/* Prototypes */
/* ---------- */
void make_checks_document_heading(
			FILE *latex_file );

void make_checks_document_footer(
			FILE *latex_file );

void make_checks_argv(	int argc, char **argv );

void make_checks_stdin(	boolean personal_size );

void make_checks_dollar_text_personal_size(
			FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *dollar_text_string,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage );

void make_checks(	FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage,
			boolean personal_size );

int main( int argc, char **argv )
{
	if ( argc == 3 && strcmp( argv[ 1 ], "stdin" ) == 0 )
	{
		boolean personal_size;

		appaserver_error_stderr( argc, argv );

		personal_size = ( *argv[ 2 ] == 'y' );

		make_checks_stdin( personal_size );
	}
	else
	{
		make_checks_argv( argc, argv );
	}

	return 0;
}

void make_checks_argv( int argc, char **argv )
{
	char *payable_to;
	double dollar_amount;
	char *memo;
	char *appaserver_data_directory;
	char latex_filename[ 128 ];
	char pdf_filename[ 128 ];
	pid_t process_id = getpid();
	FILE *latex_file;
	char *check_date;
	boolean personal_size;
	int check_number;

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s payable_to|stdin dollar_amount memo check_number personal_size_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	payable_to = argv[ 1 ];
	dollar_amount = atof( argv[ 2 ] );
	memo = argv[ 3 ];
	check_number = atoi( argv[ 4 ] );
	personal_size = ( *argv[ 5 ] == 'y' );

	appaserver_error_stderr( argc, argv );

	check_date = pipe2string( CHECK_DATE_COMMAND );

	appaserver_data_directory =
		pipe2string(
			"get_appaserver_data_directory.e" );

	if ( !appaserver_data_directory
	||   !*appaserver_data_directory )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get appaserver_data_directory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( latex_filename,
		 LATEX_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	sprintf( pdf_filename,
		 PDF_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	if ( ! ( latex_file = fopen( latex_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 latex_filename );
		exit( 1 );
	}

	make_checks_document_heading( latex_file );

	make_checks(	latex_file,
			payable_to,
			dollar_amount,
			memo,
			check_number,
			check_date,
			0 /* not with_newpage */,
			personal_size );

	make_checks_document_footer( latex_file );

	fclose( latex_file );

	/* Creates pdf_filename */
	/* -------------------- */
	latex_tex2pdf(	latex_filename,
			appaserver_data_directory
				/* working_directory */ );

	printf(	"%s\n", pdf_filename );

}

void make_checks_stdin( boolean personal_size )
{
	char payable_to[ 128 ];
	char dollar_amount_string[ 128 ];
	char memo[ 128 ];
	double dollar_amount_double;
	char check_number_string[ 16 ];
	char *appaserver_data_directory;
	char latex_filename[ 128 ];
	char pdf_filename[ 128 ];
	pid_t process_id = getpid();
	char input_buffer[ 1024 ];
	FILE *latex_file;
	boolean first_time = 1;
	char *check_date;

	check_date = pipe2string( CHECK_DATE_COMMAND );

	appaserver_data_directory =
		pipe2string(
			"get_appaserver_data_directory.e" );

	if ( !appaserver_data_directory
	||   !*appaserver_data_directory )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get appaserver_data_directory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( latex_filename,
		 LATEX_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	sprintf( pdf_filename,
		 PDF_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	if ( ! ( latex_file = fopen( latex_filename, "w" ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot open %s for write.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
			latex_filename );
		exit( 1 );
	}

	make_checks_document_heading( latex_file );

	while( get_line( input_buffer, stdin ) )
	{
		if ( character_count( '^', input_buffer ) != 3 )
		{
			fprintf( stderr,
				 "Warning in %s/%s()/%d: ignoring (%s)n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			continue;
		}

		piece( payable_to, '^', input_buffer, 0 );
		piece( dollar_amount_string, '^', input_buffer, 1 );
		piece( memo, '^', input_buffer, 2 );

		if ( strcmp( memo, "memo" ) == 0 ) *memo = '\0';

		piece( check_number_string, '^', input_buffer, 3 );

		dollar_amount_double = atof( dollar_amount_string );

		make_checks(	latex_file,
				payable_to,
				dollar_amount_double,
				memo,
				atoi( check_number_string ),
				check_date,
				1 - first_time /* with_newpage */,
				personal_size );

		first_time = 0;

	} /* while( get_line() */

	make_checks_document_footer( latex_file );

	fclose( latex_file );

	/* Creates pdf_filename */
	/* -------------------- */
	latex_tex2pdf(	latex_filename,
			appaserver_data_directory
				/* working_directory */ );

	printf(	"%s\n", pdf_filename );

}

void make_checks(	FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage,
			boolean personal_size )
{
	char dollar_text_string[ 256 ];

	dollar_text( dollar_text_string, dollar_amount );

	if ( personal_size )
	{
		make_checks_dollar_text_personal_size(
			latex_file,
			payable_to,
			dollar_amount,
			dollar_text_string,
			memo,
			check_number,
			check_date,
			with_newpage );
	}
	else
	{
/*
		make_checks_dollar_text_business_size(
			latex_file,
			payable_to,
			dollar_amount,
			dollar_text_string,
			memo,
			check_number,
			check_date,
			with_newpage );
*/
		make_checks_dollar_text_personal_size(
			latex_file,
			payable_to,
			dollar_amount,
			dollar_text_string,
			memo,
			check_number,
			check_date,
			with_newpage );
	}

}

void make_checks_document_heading(
			FILE *latex_file )
{
/*
"\\usepackage{rotating}\n"
*/
	fprintf(latex_file,
"\\documentclass{report}\n"
"\\usepackage[	portrait,\n"
"		top=0in,\n"
"		left=0in,\n"
"		paperheight=2.875in,\n"
"		paperwidth=8.5in,\n"
"		textheight=2.875in,\n"
"		textwidth=8.5in,\n"
"		noheadfoot]{geometry}\n"
"\\pagenumbering{gobble}\n"
"\\begin{document}\n" );

}

void make_checks_document_footer(
			FILE *latex_file )
{
	fprintf( latex_file,
"\\end{document}\n" );

}

void make_checks_dollar_text_personal_size(
			FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *dollar_text_string,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage )
{
	char payable_to_escaped[ 128 ];

	latex_escape_data( payable_to_escaped, payable_to, 128 );

	if ( with_newpage )
	{
		fprintf( latex_file,
"\\newpage\n\n" );
	}

#ifdef MAKE_SIDEWAYS
	fprintf( latex_file,
"\\begin{sideways}\n"
"\\begin{minipage}{8.0in}\n"
"\\vspace{0.50in}\n"
"\\begin{tabular}{p{4.9in}l}\n" );
#endif

/* Move down to first row. */
/* ----------------------- */
	fprintf( latex_file,
"\\begin{tabular}l\n"
"\\end{tabular}\n\n"
"\\vspace{0.45in}\n" );

/* Print date on stub and check */
/* ---------------------------- */
	fprintf( latex_file,
"\\begin{tabular}{p{0.2in}p{6.6in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
		 check_date,
		 check_date );

/* Print vendor on stub and (vendor and dollar amount on check) */
/* ------------------------------------------------------------ */
	fprintf( latex_file,
"\\begin{tabular}{p{0.2in}p{2.5in}p{4.1in}l}\n"
"& %.26s & %s & %s\n"
"\\end{tabular}\n\n",
		 payable_to_escaped,
		 payable_to_escaped,
		 place_commas_in_money( dollar_amount ) );

/* Place dollar amount on stub */
/* --------------------------- */
	fprintf( latex_file,
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& %s\n"
"\\end{tabular}\n",
		 place_commas_in_money( dollar_amount ) );

/* Print dollar text on check */
/* -------------------------- */
		fprintf( latex_file,
"\\vspace{0.10in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& & %s\n"
"\\end{tabular}\n\n",
		 	dollar_text_string );

/* Print memo on stub and check */
/* ---------------------------- */
	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		fprintf( latex_file,
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.5in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
			memo,
			memo );
	}
	else
	{
		fprintf( latex_file,
"\\vspace{0.20in}\n\n"
"\\begin{tabular}l\n"
"\\end{tabular}\n\n" );
	}

/* Place check number on stub */
/* -------------------------- */
	fprintf( latex_file,
"\\vspace{0.05in}\n\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& Check: %d\n"
"\\end{tabular}\n",
		 check_number );

}

