/* $APPASERVER_HOME/library/name_arg.c		 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "name_arg.h"
#include "piece.h"
#include "basename.h"
#include "timlib.h"

void set_comment( NAME_ARG *arg, char *comment )
{
	list_append_pointer(
		arg->comments,
		comment );

}

void set_usage_filename( NAME_ARG *arg, char *usage_filename )
{
	arg->usage_filename = usage_filename;

}


void add_possible_value( NAME_ARG *arg, int ticket, char *value )
{
	add_valid_value( arg, ticket, value );
}

void add_valid_value( NAME_ARG *arg, int ticket, char *value )
{
        struct valid_option *find_ticket(), *v_option = find_ticket(arg,ticket);

        if (v_option)
        {
                if ( !add_item( v_option->valid_value,
                                value,
                                strlen(value) + 1,
                                ADD_TAIL) )
                {
                        fprintf( stderr, "malloc failed in add_valid_value\n" );

                        exit( 1 );
                }
        }
        else
                name_arg_bye ("Can't Find Ticket Number");

}


int add_valid_option( NAME_ARG *arg, char *option )
/* ------------------------------------------------- */
/* This function inserts the name of a valid option  */
/* (the left side of the equal sign).                */
/* ------------------------------------------------- */
{
        static int this_ticket = 0;
        struct valid_option v_option;

        v_option.ticket_number = this_ticket;
        v_option.option = option;
        v_option.valid_value = create_list();
        v_option.default_value = (char *)0;

        if ( !add_item( arg->valid_option,
                        &v_option,
                        sizeof(struct valid_option),
                        ADD_HEAD) )
        {
                fprintf( stderr, "memory allocation error in add_valid_option");

                exit( 1 );
        }

        return this_ticket++;

}


void set_default_value( NAME_ARG *arg, int ticket, char *default_value )
{
        struct valid_option *find_ticket(), *v_option = find_ticket(arg,ticket);

        if (v_option)
                v_option->default_value = default_value;
        else
                name_arg_bye ("Can't Find Ticket Number");
}



char *fetch_arg( NAME_ARG *arg, char *option )
{
        struct command_line_args *this_arg;
        struct valid_option *v_option, *find_option();

        if ( go_head( arg->command_list_arg ) )
	{
                do {
                        this_arg =
				(struct command_line_args *)
                              		retrieve_item_ptr(
						arg->command_list_arg );

                        if ( timlib_strncmp(
					option,
					this_arg->option ) == 0 )
			{
                                if ( !*this_arg->value )
				{
					return (char *)0;
				}
				else
				{
                                	return this_arg->value;
				}
			}

                } while ( next_item( arg->command_list_arg ) );
	}

        /* Check if default value exists */
        /* ----------------------------- */
        v_option = find_option(arg,option);
        if (v_option)
                if (v_option->default_value)
                        return v_option->default_value;

        exit_usage(arg);

	return (char *)0;

}

NAME_ARG *name_arg_new( char *argv_0 )
{
	return init_arg( argv_0 );
}

NAME_ARG *init_arg( char *argv_0 )
/* --------------------------------------- */
/* Initialize Argumemnt Abstract Data Type */
/* with a list head.                       */
/* --------------------------------------- */
{
        NAME_ARG *arg = (NAME_ARG *)malloc(sizeof(NAME_ARG));

        arg->command_list_arg = create_list();
        arg->valid_option = create_list();
        arg->comments = create_list();

	if ( argv_0 )
	{
		arg->usage_filename = basename_get_base_name( argv_0, 0 );
	}

        return arg;

}


void free_arg( NAME_ARG *arg )
{
	destroy_list( arg->command_list_arg );
	destroy_list( arg->valid_option );
	/* destroy_list( arg->comments ); */
	free( arg );

}


void name_arg_insert_argv( NAME_ARG *arg, int argc, char **argv )
{
	insert_argv( arg, argc, argv );
}

void name_arg_insert( NAME_ARG *arg, int argc, char **argv )
{
	insert_argv( arg, argc, argv );
}

void ins_all( NAME_ARG *arg, int argc, char **argv )
{
	insert_argv( arg, argc, argv );
}

void insert_argv( NAME_ARG *arg, int argc, char **argv )
/* --------------------------------------------------------------------- */
/* This function inserts all of the command line arguments into the list */
/* --------------------------------------------------------------------- */
{
        while (--argc)
	{
                if (instr( "=", *++argv, 1 ) != -1 )
                {
                        ins_arg(arg,*argv);
                        *argv = ( char * )0;
			arg->did_any = 1;
                }
	}

}


void ins_arg( NAME_ARG *arg, char *this_arg )
/* --------------------------------- */
/* Inserts a argument into the list. */
/* Assume: option=value              */
/* --------------------------------- */
{
        struct command_line_args new_arg;
        struct valid_option *v_option, *find_option();
        char *v_value;
        int str_len;
        int ok_add;
        char buffer[ 1000 ];

        if ( *this_arg == '=' ) exit_usage( arg );

        new_arg.option = strdup( piece( buffer, '=', this_arg, 0 ) );
        new_arg.value = strdup( this_arg + strlen( new_arg.option ) + 1 );
        str_len = strlen( new_arg.value );

        /* Search for option in list (left side of equal sign) */
        /* --------------------------------------------------- */
        v_option = find_option( arg, new_arg.option );
        if (v_option){

                /* Found a valid option, now check for valid value */
                /* ----------------------------------------------- */
                ok_add = 0;
                if (go_head(v_option->valid_value))
                {
                        do {
                                v_value = (char *)retrieve_item_ptr(
                                                v_option->valid_value);

                                if ( strncmp(	new_arg.value,
						v_value,
						str_len )== 0 )
                                {
                                        ok_add = 1;
                                        break;
                                } /* if */

                        } while (next_item (v_option->valid_value));
                }
                else
                        ok_add = 1;

                if (ok_add)
                {
                        if ( !add_item( arg->command_list_arg,
                                        &new_arg,
                                         sizeof (struct command_line_args),
                                         ADD_HEAD ) )
                        {
                                fprintf( stderr, "malloc failed in ins_arg\n" );
                                exit( 1 );
                        }
                        return;
                }
        } /* if */

        exit_usage( arg );

}



struct valid_option *find_ticket( NAME_ARG *arg, int ticket )
{
        struct valid_option *v_option;

        if (go_head(arg->valid_option))
                do {
                        v_option = (struct valid_option *)
                                        retrieve_item_ptr(arg->valid_option);

                        if (ticket == v_option->ticket_number)
                                return v_option;

                } while (next_item(arg->valid_option));

        return (struct valid_option *)0;

}

struct valid_option *find_option( NAME_ARG *arg, char *option )
{
        struct valid_option *v_option;
        int str_len = strlen(option);

        if (go_head(arg->valid_option))
                do {
                        v_option = (struct valid_option *)
                                        retrieve_item_ptr(arg->valid_option);

                        if (strncmp(v_option->option,option,str_len) == 0)
                                return v_option;

                } while (next_item(arg->valid_option));

        return (struct valid_option *)0;

}


void exit_usage( NAME_ARG *arg )
{
        struct valid_option *find_ticket(), *v_option;
        char *v_value;
        int ticket = 0;

	if ( arg->usage_filename )
        	fprintf( stderr, "Usage: %s\n", arg->usage_filename );
	else
        	fprintf( stderr, "Usage:\n" );

        while ( (v_option = find_ticket( arg, ticket++ ) ) )
	{

                fprintf( stderr, "     %s=",v_option->option);

                /* Find valid_values */
                /* ----------------- */
                if ( go_head( v_option->valid_value ) )
		{
                        do {
                                v_value =
					(char *)
                                	retrieve_item_ptr(
						v_option->valid_value );

                                fprintf( stderr, "(%s) ",v_value);
                        } while ( next_item( v_option->valid_value ) );
		}
                else
		{
                        fprintf( stderr, "(?) ");
		}

                if (v_option->default_value)
                        if (*v_option->default_value)
                                fprintf( stderr, "(default %s) ",
                                        v_option->default_value);

                if (v_option->default_value)
                        fprintf( stderr, "[optional]");

                fprintf( stderr, "\n");

        } /* while */

        /* Display comments */
        /* ---------------- */
        if ( go_head( arg->comments ) )
        {
                char *comment;

		fprintf( stderr, "\n" );
                do {
                        comment = list_get( arg->comments );
                        fprintf( stderr, "%s\n", comment );
                } while ( next_item ( arg->comments ) );
        }

        exit(1);

}

void name_arg_bye( char *s )
{
        fprintf( stderr, "%s\n", s );
        exit( 1 );
}

