/* bsrch.c */
/* ------- */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
/* #include <unistd.h> */
#include "bsrch.h"
#include "timlib.h"

/* need prototype for private function */
/* ----------------------------------- */
int search_first_bsearch( B_SEARCH *b_search );

/* initialize structure */
/* -------------------- */
B_SEARCH *init_bsearch( char *filename, int rec_len )
{
        B_SEARCH *tmp = ( B_SEARCH *) calloc( 1, sizeof( B_SEARCH ) );

        if( tmp == NULL ) return NULL;

        tmp->file_info = init_random_io();
        set_bsearch_filename( tmp, filename );
        set_bsearch_rec_len( tmp, rec_len );
        tmp->search_key = NULL;

        if( open_random_io( tmp->file_info, "rb" ) == -1 )
        {
		return NULL;
        }

        return tmp;

}


/* set file name */
/* ------------- */
int  set_bsearch_filename( B_SEARCH *b_search, char *filename )
{
        if( b_search == NULL ) return -1;

        if( ( !filename )
        ||  ( !*filename )
        ||  ( isspace( *filename ) ) )
        {
                set_error_bsearch( b_search, "Invalid file name\n" );
                return -1;
        }

        return set_filename_random_io( b_search->file_info, filename );

}


/* set the file bsearch record length */
/* ---------------------------------- */
int  set_bsearch_rec_len( B_SEARCH *b_search, int rec_len )
{
        if( b_search == NULL ) return -1;

        if( rec_len <= 0 )
        {
                set_error_bsearch( b_search, "Invalid record len\n" );
                return -1;
        }

        b_search->record_len = rec_len;

        return set_rec_len_random_io( b_search->file_info, rec_len );

}

/* set the search key */
/* ------------------ */
int set_bsearch_key( B_SEARCH *b_search, char *key )
{
        if( b_search == NULL ) return -1;

        if( ( !key ) ||  ( !*key ) )
        {
                set_error_bsearch( b_search, "Invalid search key\n" );
                return -1;
        }

        if( b_search->search_key != NULL )
                free( b_search->search_key );

        b_search->search_key = (char *)strdup( key );

        return 0;
}


/* check the bsearch data structure */
/* -------------------------------- */
int check_bsearch( B_SEARCH *b_search )
{
        if( b_search == NULL ) return -1;

        if( b_search->record_len <= 0 )
        {
                set_error_bsearch( b_search, "Invalid record len\n" );
                return -1;
        }

        if( ( !( b_search->search_key ) )
        ||  ( !(*( b_search->search_key ) ) ) )
        {
                set_error_bsearch( b_search, "Invalid search key\n" );
                return -1;
        }

        return 0;
}


/* reset, insert key, search for first occurence */
/* --------------------------------------------- */
int reset_fetch_bsearch( B_SEARCH *b_search, char *key )
{
        unsigned long file_size;

        if( b_search == NULL ) return -1;

        set_bsearch_key( b_search, key );

        if( check_bsearch( b_search ) == -1 )
        {
		fprintf( stderr, "check_bsearch failed\n" );
                return -1;
        }

        file_size = get_file_size( b_search->file_info->filename );

        b_search->num_of_records = 
		(unsigned long)file_size / (unsigned long)b_search->record_len;

        if( ( ( (file_size) % ( b_search->record_len ) ) != 0 )
        ||  ( file_size == 0 ) )
        {
                fprintf( stderr, "Invalid record/file size\n" );
                return -1;
        }

        return search_first_bsearch( b_search );
}


/* binary search */
/* ------------- */
int binsearch( B_SEARCH *b_search )
{
        unsigned long low, high, mid;
        char buffer[ 100 ];
	int ret_value;

        if( b_search == NULL ) return -1;

        low  = 0;
        high = b_search->num_of_records;

        while( low <= high )
        {
                mid = ( low + high ) / 2;
                get_rec_random_io( b_search->file_info, mid );

                fetch_rec_random_io( b_search->file_info, buffer );

		ret_value = strncmp(	b_search->search_key, 
					buffer,
                        		strlen( b_search->search_key ) );

		if ( ret_value < 0 )
		{
                        high = mid - 1;
		}
                else 
		if( ret_value > 0 )
		{
                        low = mid + 1;
		}
		/* ----------- */
		/* Found match */
		/* ----------- */
                else
		{
                        return mid;
		}
        }

	/* No match */
	/* -------- */
        return -1;
}


/* search for first occurence, used by  reset_fetch_bsearch */
/* -------------------------------------------------------- */
int  search_first_bsearch( B_SEARCH *b_search )
{
        char buffer[ 80 ];

        if( b_search == NULL ) return -1;

        if( binsearch( b_search ) == -1 ) return -1;

        while( 1 )
        {
                if( set_previous_rec_random_io( b_search->file_info ) == -1 )
                        return 0;

                fetch_rec_random_io( b_search->file_info, buffer );

                if( strncmp(	b_search->search_key, 
				buffer,
                        	strlen( b_search->search_key) ) == 0 )
                {
                        if( set_previous_rec_random_io( 
				b_search->file_info ) == -1 )
			{
                                return 0;
			}
                }
                else
                {
                        return 0;
                }

        } /* end while */
}

/* fetch each item from search */
/* --------------------------- */
char *fetch_each_bsearch( B_SEARCH *b_search )
{
        char buffer[ 80 ];
        int key_len = strlen( b_search->search_key );

        if( b_search == NULL ) return NULL;

        if( fetch_rec_random_io( b_search->file_info, buffer ) == 0 )
                return NULL;

        if( strncmp( b_search->search_key, buffer, key_len) == 0 )
        {
                memcpy(	b_search->return_val, 
			buffer,
			(int)b_search->record_len );

		/* Trim <LF/CR> */
		/* ------------ */
                b_search->return_val[ (int)b_search->record_len - 2 ] = '\0';

                return b_search->return_val;
        }
        else
                return NULL;
}

/* free binary search structure */
/* ---------------------------- */
int free_bsearch( B_SEARCH *b_search )
{
        if( b_search == NULL ) return -1;
        free( b_search->search_key );
        free_random_io( b_search->file_info );
        free( b_search );
	return 1;
}


int set_error_bsearch( B_SEARCH *b_search, char *error_msg )
{
        if( b_search == NULL ) return -1;

        sprintf( b_search->error_buffer,
                "%-*s",MAX_ERROR_BUFFER, error_msg );

	return 1;
}


char *get_error_bsearch( B_SEARCH *b_search )
{
        if( b_search == NULL ) return NULL;

        return b_search->error_buffer;

}
