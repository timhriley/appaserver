/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/credit_provider.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CREDIT_PROVIDER_H
#define CREDIT_PROVIDER_H

#define MAX_CC_SIZE  16
#define MIN_CC_SIZE  10  /* Carte Blanche Intern'l is 10 digits */

typedef struct {
        char cc_id;             /* cc id number */
        char cc_size;           /* string len of cc number   */
        char first_digit;       /* first digit of cc number  */
        char secnd_digit;       /* second digit of cc number */
        char third_digit;       /* third digit of cc number  */
} CC_ID_TYPE;

void strip_non_digits( char *Dest, char *Src  );
int cc_identify( char *cc_string );
int cc_chk_digit( char *cc_string );
char *get_cc_name( int cc_number );

#endif
