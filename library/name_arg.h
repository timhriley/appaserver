/* $APPASERVER_HOMElibrary/name_arg.h		 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef NAME_ARG_H
#define NAME_ARG_H

#include "list.h"
#include "boolean.h"

/* ------------------------------- */
/* List of command line arguments. */
/* This list is build with calls   */
/* to ins_arg().                   */
/* ------------------------------- */
struct command_line_args {
        char *option;                           /* left side of equal sign  */
        char *value;                            /* right side of equal sign */
};

/* ------------------------------------ */
/* List of valid options and values.    */
/* These fields are build with calls    */
/* to valid_option(),                   */
/*    valid_value(), and                */
/*    set_default_value().              */
/* ------------------------------------ */
struct valid_option {
        int ticket_number;
        char *option;
        LIST *valid_value;
        char *default_value;
};

typedef struct {
        LIST *command_list_arg;     /* list of struct command_line_args */
        LIST *valid_option;         /* list of struct valid_option */
        LIST *comments;
	char *usage_filename;
	boolean did_any;
} NAME_ARG;

/* function prototypes */
/* ------------------- */
void exit_usage( NAME_ARG *arg );
void name_arg_bye( char *s );
void ins_arg( NAME_ARG *arg, char *this_arg );
void set_comment( NAME_ARG *arg, char *comment );
int  add_valid_option( NAME_ARG *arg, char *option );
void set_default_value( NAME_ARG *arg, int ticket, char *default_value);
void add_valid_value( NAME_ARG *arg, int ticket, char *value );

void ins_all( NAME_ARG *arg, int argc, char **argv );
void name_arg_insert( NAME_ARG *arg, int argc, char **argv );
void name_arg_insert_argv( NAME_ARG *arg, int argc, char **argv );
void insert_argv( NAME_ARG *arg, int argc, char **argv );

char *fetch_arg( NAME_ARG *arg, char *option );

void free_arg( NAME_ARG *arg );

NAME_ARG *init_arg( char *argv_0 );

NAME_ARG *name_arg_new( char *argv_0 );

void add_possible_value( NAME_ARG *arg, int ticket, char *value );

#endif
