/* bsrch.h */
/* ------- */

#ifndef B_SEARCH_PKG_H
#define B_SEARCH_PKG_H

#define MAX_B_SEARCH_RET_VAL 100
#define MAX_ERROR_BUFFER     100

#include <stdio.h>

#include "rnd_io.h"

typedef struct {
        RANDOM_IO *file_info;
        char *search_key;
        char return_val[ MAX_B_SEARCH_RET_VAL ];
        char error_buffer[ MAX_ERROR_BUFFER ];
        unsigned long num_of_records;
        unsigned long record_len;
} B_SEARCH;


/* function prototypes */
/* ------------------- */
B_SEARCH *init_bsearch( char *filename, int rec_len );
int  set_bsearch_filename( B_SEARCH *b_search, char *filename );
int  set_bsearch_rec_len( B_SEARCH *b_search, int rec_len );
int  set_bsearch_key( B_SEARCH *b_search, char *key );
int  reset_fetch_bsearch( B_SEARCH *b_search, char *key );
char *fetch_each_bsearch( B_SEARCH *b_search );
int  free_bsearch( B_SEARCH *b_search );
int  set_error_bsearch( B_SEARCH *b_search, char *error_msg );
char *get_error_bsearch( B_SEARCH *b_search );

#endif /* end ifndef B_SEARCH_PKG_H */
