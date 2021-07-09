/* utility/label.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "name_arg.h"
#include "list.h"
#include "piece.h"

#define MAX_LINES       10
#define MAX_LABEL_WIDTH 100

/* Global Variables */
/* ---------------- */
LIST *global_list;
FILE *exception_file;
int up;
int tchars;
int tlines;
int margin;
int startline;
int IS_LASER;
int IS_INKJET;
int null_override;
int ff;

typedef struct {

        char first_name[ 128 ];
        char last_name[ 128 ];
        char street_address[ 128 ];
        char apt_number[ 128 ];
        char city[ 128 ];
        char state[ 128 ];
        char zip_code[ 128 ];
	char attention_line[ 128 ];

        /* Output Lines */
        /* ------------ */
        char line[ MAX_LINES ][ MAX_LABEL_WIDTH + 1 ];

} LABEL;

/* Prototypes */
/* ---------- */
void set_first_name( LABEL *label, char *s );
void set_last_name( LABEL *label, char *s );
void set_street_address( LABEL *label, char *s );
void set_apt_number( LABEL *label, char *s );
void set_city( LABEL *label, char *s );
void set_state( LABEL *label, char *s );
void set_zip_code( LABEL *label, char *s );
void set_attention_line( LABEL *label, char *s );
void set_output_lines( LABEL *label );
void reset_label( LABEL *label );
void alignment_check( void );
void spool_label( LABEL *label2 );
void print_spooled( void );
void do_labels( void );
void setup_arg( NAME_ARG *arg, int argc, char **argv );

void set_first_name( LABEL *label, char *s )
{
	strcpy( label->first_name, trim( s ) );
}

void set_last_name( LABEL *label, char *s )
{
        strcpy( label->last_name, trim( s ) );
}

void set_street_address( LABEL *label, char *s )
{
        strcpy( label->street_address, trim( s ) );
}

void set_apt_number( LABEL *label, char *s )
{
        strcpy( label->apt_number, trim( s ) );
}

void set_city( LABEL *label, char *s )
{
        strcpy( label->city, trim( s ) );
}

void set_state( LABEL *label, char *s )
{
        strcpy( label->state, trim( s ) );
}

void set_zip_code( LABEL *label, char *s )
{
        strcpy( label->zip_code, trim( s ) );
}

void set_attention_line( LABEL *label, char *s )
{
        strcpy( label->attention_line, trim( s ) );
}


void set_output_lines( LABEL *label )
{
        int i;

        /* First reset output lines */
        /* ------------------------ */
        for( i = 0; i < MAX_LINES; i++ )
                *label->line[ i ] = '\0';

        i = startline - 1;

        sprintf( label->line[ i++ ], "%s %s",
                label->first_name, label->last_name );

	if ( *label->street_address )
	{
        	sprintf( label->line[ i++ ], "%s",
        		label->street_address );
	}

        if ( *label->apt_number )
	{
		strcat( label->line[ i - 1 ], " #" );
		strcat( label->line[ i - 1 ], label->apt_number );
/*
                sprintf( label->line[ i++ ], "#%s",
			label->apt_number );
                sprintf( label->line[ i++ ], "Apt: %s",
			label->apt_number );
*/
	}

	if ( *label->city && *label->state && *label->zip_code )
	{
        	sprintf( label->line[ i++ ], "%s, %s %s",
                	label->city, label->state, label->zip_code );
	}
	else
	{
        	strcpy( label->line[ i++ ], "" );
	}

	if ( *label->attention_line )
	{
        	strcpy( label->line[ i++ ], "" );
        	sprintf( label->line[ i++ ], "%s",
                	label->attention_line );
	}

}




void reset_label( LABEL *label )
{
        set_first_name( label, "" );
        set_last_name( label, "" );
        set_street_address( label, "" );
        set_apt_number( label, "" );
        set_city( label, "" );
        set_state( label, "" );
        set_zip_code( label, "" );
        set_attention_line( label, "" );
        set_output_lines( label );

}



void alignment_check( void )
{
        LABEL label;

        up = 1;
        reset_label( &label );

        set_first_name( &label, "First" );
        set_last_name( &label, "Last" );
        set_street_address( &label, "Street Addr" );
        set_apt_number( &label, "Street Addr" );
        set_city( &label, "City" );
        set_state( &label, "ST" );
        set_zip_code( &label, "Zip" );

        spool_label( &label );

}



void spool_label( LABEL *label2 )
{
        static int this_up = 0;
        LABEL label1;

        set_output_lines( label2 );
        memcpy( &label1, label2, sizeof( LABEL ) );

        if ( !add_item( global_list, &label1, sizeof( LABEL ), ADD_TAIL ) )
        {
                fprintf( stderr, "label.e: add_item failed\n" );
                exit( 1 );
        }

        if ( ++this_up == up )
        {
                print_spooled();
                destroy_list( global_list );
                global_list = create_list();
                this_up = 0;
        }

}



void print_spooled( void )
{
        LABEL *label;
        int i;
	static int this_ff = 0;

	/* Print form feed for every new page */
	/* ---------------------------------- */
	if (ff != 0)	{
        	if ( this_ff >= ff )
        	{
                	printf( "\f" );
                	this_ff = 0;
        	}
		else
			this_ff++;
	}

        for( i = 0; i < tlines; i++ )
        {
                if( !go_head( global_list ) ) return;

                do {
                        label = (LABEL *)retrieve_item_ptr( global_list );
                        printf( "%*c%-*s",
                                margin, ' ', tchars, label->line[ i ] );
                } while( next_item( global_list ) );

                if ( IS_INKJET )
                        printf( "\r\n" );
                else
                        printf( "\n" );
        }
        if ( IS_INKJET )
                printf( "\f" );

}


void do_labels( void )
{
        char instring[ 500 ];
        char buffer[ MAX_LABEL_WIDTH ];
        LABEL label;

        reset_label( &label );

        while( get_line( instring, stdin ) )
        {
		if ( !*instring || (*instring == '#' ) ) continue;

                if ( !piece( buffer, '|', instring, 0 ) )
                        return;
                else
                        set_first_name( &label, buffer );

                if ( !piece( buffer, '|', instring, 1 ) )
                        return;
                else
                        set_last_name( &label, buffer );

                if ( !piece( buffer, '|', instring, 2 ) )
                        return;
                else
                        set_street_address( &label, buffer );

                if ( !piece( buffer, '|', instring, 3 ) )
                        return;
                else
                        set_apt_number( &label, buffer );

                if ( !piece( buffer, '|', instring, 4 ) )
                        return;
                else
                        set_city( &label, buffer );

                if ( !piece( buffer, '|', instring, 5 ) )
                        return;
                else
                        set_state( &label, buffer );

                if ( !piece( buffer, '|', instring, 6 ) )
                        return;
                else
                        set_zip_code( &label, buffer );

                if ( piece( buffer, '|', instring, 7 ) )
                        set_attention_line( &label, buffer );

                spool_label( &label );

        } /* while not EOF */

        print_spooled();

/*
        if ( IS_LASER ) printf("%c\n", 12 );
*/

        return;

}



void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "option" );
        add_valid_value( arg, ticket, "labels" );
        add_valid_value( arg, ticket, "alignment" );
        set_default_value( arg, ticket, "labels" );

        ticket = add_valid_option( arg, "exception" );
        set_default_value( arg, ticket, "/tmp/label.bad" );

        ticket = add_valid_option( arg, "up" );
        set_default_value( arg, ticket, "1" );

        ticket = add_valid_option( arg, "tchars" );
        set_default_value( arg, ticket, "30" );

        ticket = add_valid_option( arg, "margin" );
        set_default_value( arg, ticket, "0" );

        ticket = add_valid_option( arg, "startline" );
        set_default_value( arg, ticket, "1" );

        ticket = add_valid_option( arg, "tlines" );
        set_default_value( arg, ticket, "8" );

        ticket = add_valid_option( arg, "laser" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "inkjet" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "null_override" );
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "ff" );
        set_default_value( arg, ticket, "0" );


        set_comment( arg, " First | Last | Street Addr | Apt(opt) | City | State | Zip\n option=alignment,labels         prints an alignment check or labels\n up=n                            n is 1 up, or 2 up, or 3 up, ...\n tchars=n                        total charaters wide for each label\n tlines=n                        total lines for each label\n margin=n                        how many spaces in front of each label\n startline=n                     the line number for the first line\n exception=exception_file.bad    labels with missing information go here\n laser=[y][n]                    if laser then output <FF> on last page\n null_override=[y][n]            if null_override then no exception on null\n inkjet=[y][n]                   if inkjet then output special characters\n ff=n                   		form feed for how many labels on one page ");

        ins_all( arg, argc, argv );

}




int main( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
        char *ret_arg;

        global_list = create_list();
        setup_arg( arg, argc, argv );

        ret_arg = fetch_arg( arg, "option" );
        up = atoi( fetch_arg( arg, "up" ) );
        tchars = atoi( fetch_arg( arg, "tchars" ) );
        tlines = atoi( fetch_arg( arg, "tlines" ) );
        margin = atoi( fetch_arg( arg, "margin" ) );
        startline = atoi( fetch_arg( arg, "startline" ) );
        ff = atoi( fetch_arg( arg, "ff" ) );
        exception_file = fopen( fetch_arg( arg, "exception" ), "w" );

/*
	if ( !exception_file )
	{
		fprintf( stderr, "File open error: %s\n",
			 fetch_arg( arg, "exception" ) );
		exit( 1 );
	}
*/

        IS_LASER = ( *fetch_arg( arg, "laser" ) == 'y' );
        IS_INKJET = ( *fetch_arg( arg, "inkjet" ) == 'y' );
        null_override = ( *fetch_arg( arg, "null_override" ) == 'y' );

        if ( strcmp( ret_arg, "alignment" ) == 0 )
                alignment_check();
        else
	{
                do_labels();
        	print_spooled();
	}

	return 0;

}

