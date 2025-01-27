/* /usr2/lib/report.c 					  */
/* ------------------------------------------------------
These routines support the REPORT ADT.

Tim Riley
--------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "report.h"
#include "timlib.h"
#include "piece.h"

REPORT *init_report()
{
        char in_line[ 128 ];
        char buffer[ 128 ];
        FILE *fp;
        REPORT *ret_report = ( REPORT *)calloc( 1, sizeof( REPORT ) );

        if ( !ret_report )
                report_bye ("malloc failed in init_report()" );

        ret_report->num_header_lines = 0;
        ret_report->num_footer_lines = 0;
        ret_report->num_title_lines = 0;
        ret_report->current_line = 0;
        ret_report->page_num = 0;
        ret_report->outfile = ( FILE *)0;
        ret_report->output_to_file = 0;
        ret_report->report_count = 0;
        ret_report->form_feed_ok = 1;

	/* Get date */
	/* -------- */
        fp = popen( "date '+%D %T'", "r" );
        fgets( in_line, 127, fp );
	in_line[ strlen( in_line ) - 1 ] = '\0';
        pclose( fp );

        ret_report->date_stamp = strdup( piece( buffer, ' ', in_line, 0 ) );
        ret_report->time_stamp = strdup( piece( buffer, ' ', in_line, 1 ) );

        return ret_report;

} /* init_report() */


void set_lines_per_page( REPORT *report, int lines_per_page )
{
        report->lines_per_page = lines_per_page;

} /* set_lines_per_page() */


void report_free( REPORT *report )
{
	free( report );
} /* report_free() */

void set_report_width( REPORT *report, int report_width )
{
        int x;
        char buffer[ 80 ];
        char *line_ptr = report->line_str;

        if ( report_width > MAX_REPORT_WIDTH )
        {
                sprintf( buffer, "max report width: %d\n", MAX_REPORT_WIDTH );
                report_bye( buffer );
        }

        /* Set underline */
        /* ------------- */
        for ( x = 0; x < report_width; x++ )
                *line_ptr++ = '_';

        *line_ptr = '\0';

        report->report_width = report_width;

} /* set_report_width() */

void increment_count( REPORT *report )
{
        report->report_count++;
}

void reset_page_num( REPORT *report )
{
        report->page_num = 0;
} /* reset_page_num() */


void reset_title( REPORT *report )
{
        report->num_title_lines = 0;
} /* reset_title() */


void set_title( REPORT *report, char *title, int is_centered )
{
        if ( report->num_title_lines == MAX_LINES )
                report_bye( "Max title lines exceeded" );

        report->title_line[ report->num_title_lines ].title_line_str = title;
        report->title_line[ report->num_title_lines ].is_centered = is_centered;
        report->num_title_lines++;
} /* set_title() */


void reset_footer( REPORT *report)
{
        report->num_footer_lines = 0;
} /* reset_footer() */

void reset_header( REPORT *report)
{
        report->num_header_lines = 0;

} /* reset_header() */


void set_footer ( REPORT *report, char *footer_line )
{
        if ( report->num_footer_lines == MAX_LINES )
                report_bye ("Max footer lines exceeded");

        report->footer_line[ report->num_footer_lines++ ] = footer_line;

} /* set_footer() */

void set_header ( REPORT *report, char *header_line )
{
        if ( report->num_header_lines == MAX_LINES )
                report_bye ("Max header lines exceeded");

        report->header_line[ report->num_header_lines++ ] = header_line;

} /* set_header() */


void set_no_form_feed( REPORT *report )
{
        report->form_feed_ok = 0;

} /* set_no_form_feed() */


void open_report_pipe( REPORT *report, char *pipe_str )
{
        report->outfile = popen( pipe_str, "w" );

	if ( !report->outfile ) report_bye( "open_report_pipe failed" );

        report->output_to_file = 0;

} /* open_report_pipe() */


void open_report_outfile( REPORT *report, char *outfile_str )
{
        report->outfile = fopen( outfile_str, "w" );

	if ( !report->outfile ) report_bye( "open_report_outfile failed" );

        report->output_to_file = 1;

} /* open_report_outfile() */


void start_title_header( REPORT *report )
{
        output_title_no_ff( report );
        output_header( report );

} /* start_title_header() */



void output_title_ff( REPORT *report )
{
        output_form_feed( report );
        output_title_no_ff( report );
}


void output_title_center( REPORT *report, int x )
{
	char *date_stamp = get_date_stamp( report );

        if ( !report->outfile ) report->outfile = stdout;

        if ( x == 0 )
	{
		if ( report->supress_date || report->page_number_to_footer )
		{
                	fprintf( report->outfile, "%s\n",
		   	center(
			report->title_line[ x ].title_line_str,
                        report->report_width ) );
		}
		else
		{
                	fprintf( report->outfile, "%s%s %4d\n",
			date_stamp,
                	right(
		   	center(
				report->title_line[ x ].title_line_str,
                        	report->report_width - strlen( date_stamp ) * 2 
			),
			"Page",
                   	report->report_width - ( 6 + strlen( date_stamp ) ) ),
                	++report->page_num );
		}
	}
        else
	{
                fprintf( report->outfile, "%s\n",
                        center( report->title_line[ x ].title_line_str,
                                report->report_width ) );
	}

} /* output_title_center() */


void output_title_left( REPORT *report, int x )
{
        if ( x == 0 )
                fprintf( report->outfile, "%s %4d\n",
                        right( report->title_line[ x ].title_line_str,
                        "Page",
                        report->report_width - 6 ),
                        ++report->page_num );
        else
                fprintf( report->outfile, "%s\n",
                        report->title_line[ x ].title_line_str );

} /* output_title_left() */


void output_title_no_ff( REPORT *report )
{
        int x;

        if ( !report->outfile ) report->outfile = stdout;

        report->current_line = 0;

        for( x = 0; x < report->num_title_lines; x++ )

                if ( report->title_line[ x ].is_centered )
                        output_title_center( report, x );
                else
                        output_title_left( report, x );

        fprintf( report->outfile, "\n");

        report->current_line += ( report->num_title_lines + 1 );

} /* output_title_no_ff() */



void group_next( REPORT *report, int num_group )
{
        int diff =      report->current_line +
                        num_group -
                        report->lines_per_page;

        if ( diff >= 0 )
        {
                output_footer( report );
                output_title_ff( report );
                output_header( report );
        }

} /* group_next() */



void output_footer( REPORT *report )
{
        int x;
	char page_string[ 64 ];

        if ( !report->outfile ) report->outfile = stdout;

        fprintf( report->outfile, "%s\n", report->line_str );
	fflush( report->outfile );

        for( x = 0; x < report->num_footer_lines; x++ )
        {
        	if ( x == 0 && report->page_number_to_footer )
		{
			sprintf( page_string, "%d", ++report->page_num );
                	fprintf( report->outfile, "%s%s\n",
				 report->footer_line[ 0 ],
		   		 center( page_string,
                        		 report->report_width - 
					 strlen( report->footer_line[ 0 ] ) * 2 
					) );
		}
		else
		{
                	fprintf( report->outfile, 
				 "%s\n", 
				 report->footer_line[ x ] );
		}
        }

	fflush( report->outfile );

} /* output_footer() */

void output_header( REPORT *report )
{
        int x;

        if ( !report->outfile ) report->outfile = stdout;

        output_line_str( report );

        for( x = 0; x < report->num_header_lines; x++ )
        {
                fprintf( report->outfile, "%s\n", report->header_line[ x ] );
        }

        report->current_line += report->num_header_lines;
	fflush( report->outfile );
        output_line_str( report );

} /* output_header() */


void output_line_str( REPORT *report )
{
        return output_line( report, report->line_str );

} /* output_line_str() */


void output_line( REPORT *report, char *line )
{
        if ( !report->outfile ) report->outfile = stdout;

        /* Output new page (maybe ) */
        /* ------------------------ */
        if ( *line 
	&& ( report->current_line >= report->lines_per_page - 1 +
        			  report->num_footer_lines ) )
        {
        	output_footer( report );
        	output_title_ff( report );
        	output_header( report );
        }

        fprintf( report->outfile, "%s\n", line );
	fflush( report->outfile );
        report->current_line++;

} /* output_line() */



void output_form_feed( REPORT *report )
{
        if ( !report->outfile ) report->outfile = stdout;

        if ( report->form_feed_ok )
                fprintf( report->outfile, "%c", 12 );

        report->current_line = 0;

} /* output_form_feed() */


char *get_date_stamp( REPORT *report )
{
        return report->date_stamp;

} /* get_date_stamp() */


char *get_time_stamp( REPORT *report )
{
        return report->time_stamp;

} /* get_time_stamp() */


void output_blank_lines( REPORT *report, int blank_lines )
{
	if ( blank_lines > 0 ) while( blank_lines-- ) output_line( report, "" );
}

void end_of_report( REPORT *report, int with_close )
{
        char buffer[ 100 ];

	if ( report->supress_end_of_report )
	{
		output_blank_lines( 	report, 
					( report->lines_per_page - 1 ) +
        			  	report->num_footer_lines -
					report->current_line );
		output_footer( report );
		return;
	}

        output_line( report, "" );

        if ( report->report_count )
                sprintf( buffer, "Report Count: %d  ", report->report_count );
        else
                strcpy( buffer, "End of Report" );

        strcat( buffer, "         " );
        strcat( buffer, get_date_stamp( report ) );
        strcat( buffer, " " );
        strcat( buffer, get_time_stamp( report ) );

        output_line( report, buffer );

        if ( with_close )
	{
                if ( report->output_to_file )
		{
			if ( report->outfile != stdout )
                        	fclose( report->outfile );
		}
                else
		{
                        pclose( report->outfile );
		}
	}
} /* end_of_report() */


void set_page_number( REPORT *report, int page_number )
{
	/* page_num is zero based */
	/* ---------------------- */
        report->page_num = page_number - 1;
}

void set_supress_end_of_report( REPORT *report )
{
	report->supress_end_of_report = 1;
}

void set_suppress_date( REPORT *report )
{
	report->supress_date = 1;
}

void set_page_number_to_footer( REPORT *report, char *footer_message )
{
	report->page_number_to_footer = 1;
	set_footer( report, footer_message );
}

void report_bye( char *s )
{
        fprintf( stderr, "%s\n", s );
        exit( 1 );
}
