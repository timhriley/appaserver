/* /usr2/lib/report.h 					  */
/* ------------------------------------------------------
This header file defines the REPORT ADT.

Tim Riley
--------------------------------------------------------- */


#ifndef REPORT_H
#define REPORT_H

#include <stdio.h>

#define NO_CENTER               0
#define CENTER                  1

#define MAX_LINES	        6
#define MAX_REPORT_WIDTH        300

#define NO_CLOSE                0
#define WITHOUT_CLOSE           0
#define WITH_CLOSE              1

typedef struct {
        char *title_line_str;
        int is_centered;
} TITLE_TYPE;

typedef struct 
{
        int lines_per_page;
        int report_width;
        int current_line;
        int page_num;
        int report_count;
        int form_feed_ok;

        int num_title_lines;
        TITLE_TYPE title_line[ MAX_LINES ];

        char *header_line[ MAX_LINES ];
        int num_header_lines;

        char *footer_line[ MAX_LINES ];
        int num_footer_lines;

        char line_str[ 1024 ];

        int output_to_file;                     /* Default is yes, if no */
                                                /* then output to pipe.  */
        FILE *outfile;
        char *date_stamp;
        char *time_stamp;

	int supress_end_of_report;
	int supress_date;
	int page_number_to_footer;

} REPORT;

/* Prototypes */
/* ---------- */
void output_blank_lines( REPORT *report, int blank_lines );
REPORT *init_report( void );
void set_page_number( REPORT *report, int page_number );
void set_supress_end_of_report( REPORT *report );
void set_suppress_date( REPORT *report );
void set_page_number_to_footer( REPORT *report, char *footer_message );
void set_lines_per_page( REPORT *report, int lines_per_page );
void report_free( REPORT *report );
void set_report_width( REPORT *report, int report_width );
void increment_count( REPORT *report );
void reset_page_num( REPORT *report );
void reset_title( REPORT *report );
void set_title( REPORT *report, char *title, int is_centered );
void reset_header( REPORT *report);
void reset_footer( REPORT *report);
void set_header ( REPORT *report, char *header_line );
void set_footer ( REPORT *report, char *footer_line );
void set_no_form_feed( REPORT *report );
void open_report_pipe( REPORT *report, char *pipe_str );
void open_report_outfile( REPORT *report, char *outfile_str );
void start_title_header( REPORT *report );
void output_title_ff( REPORT *report );
void output_title_center( REPORT *report, int x );
void output_title_left( REPORT *report, int x );
void output_title_no_ff( REPORT *report );
void group_next( REPORT *report, int num_group );
void output_header( REPORT *report );
void output_line_str( REPORT *report );
void output_line( REPORT *report, char *line );
void output_form_feed( REPORT *report );
char *get_date_stamp( REPORT *report );
char *get_time_stamp( REPORT *report );
void end_of_report( REPORT *report, int with_close );
void report_bye( char *s );
void output_footer( REPORT *report );

#endif
