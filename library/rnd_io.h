/* rnd_io.h */
/* -------- */

#ifndef RANDOM_IO_H
#define RANDOM_IO_H

#define MAX_ERROR_BUFFER 100

typedef struct {
        char *filename;
        char error_buffer[ MAX_ERROR_BUFFER ];
        FILE *file_ptr;
        int  record_len;
} RANDOM_IO;

RANDOM_IO *init_random_io( void );
int  set_filename_random_io( RANDOM_IO *f, char *filename );
int  set_rec_len_random_io( RANDOM_IO *f, int rec_len );
int  open_random_io( RANDOM_IO *f, char *type );
int  get_rec_random_io( RANDOM_IO *f, unsigned long offset );
int  set_previous_rec_random_io( RANDOM_IO *f );
int  fetch_rec_random_io( RANDOM_IO *f, void *rec );
int  free_random_io( RANDOM_IO *f );
unsigned long rnd_io_get_file_size( RANDOM_IO *f );
int  set_error_random_io( RANDOM_IO *f, char *error_msg );
char *get_error_random_io( RANDOM_IO *f );

#endif /* end ifndef RANDOM_IO_H */
