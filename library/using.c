/* using.c */
/* ------- */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "using.h"

static int count_chars( char c, char *s )
{
        int count = 0;
        while ( *s && *s == c )
        {
                count++;
                s++;
        }
        return count;
}

static int LeftJust_str( char *fmt_str, char *from_str, char *to_str)
{
        int num_of_s = count_chars( 's', fmt_str );
        sprintf( to_str, "%-*s", num_of_s, from_str );
        return num_of_s;
}

static int RightJust_str( char *fmt_str, char *from_str, char *to_str)
{
        int num_of_S = count_chars( 'S', fmt_str );
        sprintf( to_str, "%*s", num_of_S, from_str );
        return num_of_S;
}


static int LeftJust_int( char *fmt_str, int from_int, char *to_str)
{
        int num_of_d = count_chars( 'd', fmt_str );
        sprintf( to_str, "%-*d", num_of_d, from_int );
        return num_of_d;
}

static int RightJust_int( char *fmt_str, int from_int, char *to_str)
{
        int num_of_D = count_chars( 'D', fmt_str );
        sprintf( to_str, "%*d", num_of_D, from_int );
        return num_of_D;
}


static int LeftJust_dbl( char *fmt_str, double from_dbl, char *to_str)
{
        char fmt[20];

        int num_of_f1, num_of_f2;

        num_of_f1 = count_chars( 'f', fmt_str );

        /* Skip over first part plus '.' */
        /* ----------------------------- */
        fmt_str += num_of_f1;
        fmt_str++;

        num_of_f2 = count_chars( 'f', fmt_str );

        /* Width is left of '.' + right of '.' + '.' */
        /* ----------------------------------------- */
        num_of_f1 += num_of_f2;
        num_of_f1++;

        sprintf( fmt, "%c-%d.%dlf", '%', num_of_f1, num_of_f2 );
        sprintf( to_str, fmt, from_dbl );
        return num_of_f1;
}


static int RightJust_dbl( char *fmt_str, double from_dbl, char *to_str)
{
        char fmt[20];

        int num_of_F1 = count_chars( 'F', fmt_str );
        int num_of_F2;


        /* Skip over first part plus '.' */
        /* ----------------------------- */
        fmt_str += num_of_F1;
        fmt_str++;

        num_of_F2 = count_chars( 'F', fmt_str );

        /* Width is left of '.' + right of '.' + '.' */
        /* ----------------------------------------- */
        num_of_F1 += num_of_F2;
        num_of_F1++;

        sprintf( fmt, "%c%d.%dlf", '%', num_of_F1, num_of_F2 );
        sprintf( to_str, fmt, from_dbl );
        return num_of_F1;
}



char *using( char *buffer, char *fmt_str, ... )
{
        va_list arg_ptr;
        int move_on;
        char *buf_ptr = buffer;

        /* Make a temporary copy of the format string */
        /* ------------------------------------------ */
        char *fmt_tmp = fmt_str = strdup( fmt_str );

        char *str_val;
        int int_val;
        double dbl_val;

        va_start( arg_ptr, fmt_str );
        while ( *fmt_str )
        {
                if ( *fmt_str != '~' )
                {
                        *buf_ptr++ = *fmt_str++;
                        continue;
                }

                /* Replace the '~' with the following character */
                /* -------------------------------------------- */
                *fmt_str = *(fmt_str + 1);
                switch ( *fmt_str )
                {
                        case 's' :
                                str_val = va_arg( arg_ptr, char * );
                                move_on = LeftJust_str( fmt_str,
                                                        str_val,
                                                        buf_ptr);
                                fmt_str += move_on;
                                buf_ptr += move_on;
                                break;

                        case 'S' :
                                str_val = va_arg( arg_ptr, char * );
                                move_on = RightJust_str(fmt_str,
                                                        str_val,
                                                        buf_ptr);
                                fmt_str += move_on;
                                buf_ptr += move_on;
                                break;

                        case 'd' :
                                int_val = va_arg( arg_ptr, int );
                                move_on = LeftJust_int( fmt_str,
                                                        int_val,
                                                        buf_ptr);
                                fmt_str += move_on;
                                buf_ptr += move_on;
                                break;

                        case 'D' :
                                int_val = va_arg( arg_ptr, int );
                                move_on = RightJust_int(fmt_str,
                                                        int_val,
                                                        buf_ptr);
                                fmt_str += move_on;
                                buf_ptr += move_on;
                                break;

                        case 'f' :
                                dbl_val = va_arg( arg_ptr, double );
                                move_on = LeftJust_dbl( fmt_str,
                                                        dbl_val,
                                                        buf_ptr);
                                fmt_str += move_on;
                                buf_ptr += move_on;
                                break;

                        case 'F' :
                                dbl_val = va_arg( arg_ptr, double );
                                move_on = RightJust_dbl(fmt_str,
                                                        dbl_val,
                                                        buf_ptr);
                                fmt_str += move_on;
                                buf_ptr += move_on;
                                break;
                } /* switch */

        } /* while */

        free( fmt_tmp );
        *buf_ptr = '\0';
        return buffer;

} /* using() */

