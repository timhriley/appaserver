/* name_dvr.c */
/* ---------- */

/* ------------------------------------ */
/* Test driver program for NAME_ARG ADT */
/*                                      */
/* Tim Riley                            */
/* ------------------------------------ */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "name_arg.h"

void test1( int argc, char **argv );
void test2( int argc, char **argv );

int main(argc,argv)
int argc;
char **argv;
{
	/* test1( argc, argv ); */
	test2( argc, argv );
	return 0;
}

void test1( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
        char *param1, *param2, *param3, *param4, *param5;
        int ticket;

        set_comment( arg, "name_dvr.e" );
        set_comment( arg, "---------" );
        set_comment( arg, "Here are some test comments" );

        ticket = add_valid_option( arg, "param1");
        add_valid_value( arg, ticket, "one");
        add_valid_value( arg, ticket, "two");
        add_valid_value( arg, ticket, "three");
        set_default_value( arg, ticket, "ninty-nine");

        ticket = add_valid_option( arg, "param2");
        add_valid_value( arg, ticket, "one");
        add_valid_value( arg, ticket, "two");
        add_valid_value( arg, ticket, "three");
        set_default_value( arg, ticket, "ninty-nine");

        ticket = add_valid_option( arg, "param3");
        add_valid_value( arg, ticket, "one");
        add_valid_value( arg, ticket, "two");
        add_valid_value( arg, ticket, "three");
        set_default_value( arg, ticket,"");

        ticket = add_valid_option( arg, "param4");
        add_valid_value( arg, ticket, "one");
        add_valid_value( arg, ticket, "two");
        add_valid_value( arg, ticket, "three");

        add_valid_option( arg, "param5" );

        name_arg_insert_argv(arg,argc,argv);

        param1 = fetch_arg(arg, "param1");
        param2 = fetch_arg(arg, "param2");
        param3 = fetch_arg(arg, "param3");
        param4 = fetch_arg(arg, "param4");
        param5 = fetch_arg(arg, "param5");

        printf("param1: %s\n", param1);
        printf("param2: %s\n", param2);
        printf("param3: %s\n", param3);
        printf("param4: %s\n", param4);
        printf("param5: %s\n", param5);
}

void test2( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
        char *table_name;
	char *field_name_list_string;
	char *delimiter_string;
        int ticket;
	char legacy_usage_string[ 128 ];

	sprintf(legacy_usage_string,
		"Legacy usage: %s table_name field_1,...,field_n [delimiter]",
		argv[ 0 ] );

        set_comment( arg, legacy_usage_string );

        ticket = add_valid_option( arg, "table_name");

        ticket = add_valid_option( arg, "field_list");

        ticket = add_valid_option( arg, "delimiter");
        set_default_value( arg, ticket,"|");

        name_arg_insert_argv( arg, argc, argv );

	if ( !arg->did_any && argc > 2 )
	{
		table_name = argv[ 1 ];
		field_name_list_string = argv[ 2 ];

		if ( argc == 4 )
			delimiter_string = argv[ 3 ];
		else
			delimiter_string = "|";
	}
	else
	{
        	table_name = fetch_arg(arg, "table_name");
        	field_name_list_string = fetch_arg(arg, "field_list");
        	delimiter_string = fetch_arg(arg, "delimiter");
	}

        printf("table_name: %s\n", table_name );
        printf("field_name_list_string: %s\n", field_name_list_string );
        printf("delimiter_string: %s\n", delimiter_string);
}

