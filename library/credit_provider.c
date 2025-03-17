/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/credit_provider.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_error.h"
#include "entity.h"
#include "entity_self.h"
#include "credit_provider.h"

static char *provider_name_array[] = {
        "Visa",
        "MasterCard",
        "American Express",
        "Discover",
        "Diner's Club",
        "Carte Blanche",
        "Carte Blanche International" };

static CC_ID_TYPE cc_id_array[] = {
        { 0, 13, '4', '\0', '\0' }, /* Visa */
        { 0, 16, '4', '\0', '\0' }, /* Visa */
        { 1, 16, '5', '\0', '\0' }, /* Master Card*/
        { 2, 15, '3',  '4', '\0' }, /* American Express */
        { 2, 15, '3',  '7', '\0' }, /* American Express */
        { 3, 16, '6',  '0',  '1' }, /* Discover Card */
        { 4, 14, '3',  '0', '\0' }, /* Diner's Club */
        { 4, 14, '3',  '6', '\0' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '1' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '2' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '3' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '4' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '5' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '6' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '7' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '8' }, /* Diner's Club */
        { 5, 14, '3',  '8',  '9' }, /* Carte Blanche */
        { 6, 14, '9', '\0', '\0' }, /* Carte Blanche International */
        { 99, 99, '9', '\0', '\0' } };

char *credit_provider_number_stripped( char *credit_card_number )
{
	static char number_stripped[ 128 ];
	char *ptr = number_stripped;

	if ( !credit_card_number )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"credit_card_number is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

        while( *credit_card_number )
        {
                if( isdigit( *credit_card_number ) )
                        *ptr++ = *credit_card_number++;
                else
                        credit_card_number++;
        }

        *ptr = '\0';

	return number_stripped;
}

int credit_provider_array_offset( char *number_stripped )
{
        int  cc_number_size;
        int  index;

	if ( !number_stripped )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"number_stripped is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

        cc_number_size = strlen( number_stripped );

        if( cc_number_size < MIN_CC_SIZE || cc_number_size > MAX_CC_SIZE )
                return -1;

        for( index = 0; cc_id_array[ index ].cc_id != 99; index++ )
        {
                if( cc_id_array[ index ].cc_size != ( char )cc_number_size )
                {
                        continue;
                }

                if( cc_id_array[ index ].first_digit != number_stripped[ 0 ] )
                {
                        continue;
                }

                if( ( cc_id_array[ index ].secnd_digit != '\0' )
                &&  ( cc_id_array[ index ].secnd_digit != number_stripped[ 1 ]))
                {
                        continue;
                }

                if( ( cc_id_array[ index ].third_digit != '\0' )
                &&  ( cc_id_array[ index ].third_digit != number_stripped[ 2 ]))
                {
                        continue;
                }

                return cc_id_array[ index ].cc_id;
        }

        return -1;
}

boolean credit_provider_check_digit_boolean(
		char *number_stripped )
{
        int cc_number_size;
        int index;
        int weight = 2;
        int product;
        int current_number;
        int accummulator = 0;

        cc_number_size = strlen( number_stripped );
        index = cc_number_size - 2;

        if( cc_number_size < MIN_CC_SIZE || cc_number_size > MAX_CC_SIZE )
                return 0;

        for( ; index >= 0; index-- )
        {
                current_number = number_stripped[ index ] - 48;
                product = current_number * weight;
                accummulator += product / 10;
                accummulator += product % 10;

                if( weight == 2 )
                        weight = 1;
                else
                        weight = 2;
        }

        accummulator %= 10;
        accummulator = 10 - accummulator;

        if( accummulator == 10 )
                accummulator = 0;

        if ( accummulator == ( number_stripped[ cc_number_size - 1 ] - 48 ) )
                return 1;
	else
        	return 0;
}

char *credit_provider_name( int array_offset )
{
        if( ( array_offset >= 0 ) || ( array_offset <= 6 ) )
                return provider_name_array[ array_offset ];
        else
                return (char *)0;
}

CREDIT_PROVIDER *credit_provider_new(
		char *full_name,
		char *street_address,
		char *credit_card_number )
{
	CREDIT_PROVIDER *credit_provider;

	if ( !full_name
	||   !street_address
	||   !credit_card_number )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	credit_provider = credit_provider_calloc();

	credit_provider->number_stripped =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		credit_provider_number_stripped(
			credit_card_number );

	credit_provider->check_digit_boolean =
		credit_provider_check_digit_boolean(
			credit_provider->number_stripped );

	if ( credit_provider->check_digit_boolean )
	{
		credit_provider->array_offset =
			credit_provider_array_offset(
				credit_provider->number_stripped );

		credit_provider->name =
			/* --------------------------------------------- */
			/* Returns component of global structure or null */
			/* --------------------------------------------- */
			credit_provider_name(
				credit_provider->array_offset );
	}

	credit_provider->sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		credit_provider_sql_statement(
			ENTITY_SELF_TABLE,
			full_name,
			street_address,
			credit_provider->name );

	return credit_provider;
}

CREDIT_PROVIDER *credit_provider_calloc( void )
{
	CREDIT_PROVIDER *credit_provider;

	if ( ! ( credit_provider = calloc( 1, sizeof ( CREDIT_PROVIDER ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return credit_provider;
}

char *credit_provider_sql_statement(
		const char *entity_self_table,
		char *full_name,
		char *street_address,
		char *credit_provider_name )
{
	char set_clause[ 128 ];
	char sql_statement[ 1024 ];

	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !credit_provider_name )
	{
		strcpy( set_clause, "credit_provider = null" );
	}
	else
	{
		snprintf(
			set_clause,
			sizeof ( set_clause ),
			"credit_provider = '%s'",
			credit_provider_name );
	}

	snprintf(
		sql_statement,
		sizeof ( sql_statement ),
		"update %s set %s where %s;",
		entity_self_table,
		set_clause,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			full_name,
			street_address ) );

	return strdup( sql_statement );
}

