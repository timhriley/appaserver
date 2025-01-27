/* --------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/dollar.c			   		*/
/* --------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "dollar.h"

char *dollar_text( char *destination, double dollar )
{
	char dollar_string[ 32 ];
	char piece_buffer[ 32 ];
	int integer_part;
	boolean need_comma = 0;

	*destination = '\0';

	sprintf( dollar_string, "%13.2f", dollar);

	if ( !(boolean)dollar )
	{
		strcpy( destination, "Zero");

		/* Fool function into making "Dollars" plural. */
		/* ------------------------------------------- */
		need_comma = 1;
	}

	if ( dollar >= 1000.0 )
	{
		integer_part = (int)( dollar / 1000.0 );

		dollar_part(
			destination,
			integer_part,
			0 /* not need_comma */,
			1 /* omit_dollars */ );

		strcat( destination, " Thousand");
		dollar = dollar - (int)( dollar / 1000.0 ) * 1000;
		need_comma = 1;
	}

	dollar_part(	destination,
			(int)dollar,
			need_comma,
			0 /* not omit_dollars */ );

	pennies_part(	destination,
			atoi( piece( piece_buffer, '.', dollar_string, 1 ) ) );

	return destination;
}

void dollar_part(	char *destination,
			int integer_part,
			boolean need_comma,
			boolean omit_dollars )
{
	boolean need_dash = 0;
	boolean singular = 1;

	if ( need_comma ) singular = 0;

	static char *ones[]=
	{
		"invalid ones",
		"One",
		"Two",
		"Three",
		"Four",
		"Five",
		"Six",
		"Seven",
		"Eight",
		"Nine"
	};
	static char *teen[]=
	{
		"Ten",
		"Eleven",
		"Twelve",
		"Thirteen",
		"Fourteen",
		"Fifteen",
		"Sixteen",
		"Seventeen",
		"Eighteen",
		"Nineteen"
	};
	static char *tens[]=
	{
		"invalid tens0 ",
		"invalid tens1 ",
		"Twenty",
		"Thirty",
		"Forty",
		"Fifty",
		"Sixty",
		"Seventy",
		"Eighty",
		"Ninety"
	};

	if ( integer_part >= 100 )
	{
		if ( need_comma ) strcat( destination, ", " );

		strcat( destination, ones [ integer_part / 100 ] );
		strcat( destination, " Hundred" );
		integer_part = integer_part % 100;
		need_comma = 1;
		singular = 0;
	}

	if ( integer_part >= 20 )
	{
		if ( need_comma ) strcat( destination, ", " );
		strcat ( destination, tens [ integer_part / 10 ] );
		integer_part = integer_part % 10;
		need_dash = 1;
		singular = 0;
	}

	if ( integer_part >= 10 )
	{
		if ( need_comma ) strcat( destination, ", " );
		if ( need_dash ) strcat( destination, "-" );
		strcat ( destination, teen [ integer_part % 10 ] );
		integer_part = 0;
		need_comma = 0;
		need_dash = 1;
		singular = 0;
	}

	if ( integer_part >= 1 )
	{
		if ( need_dash )
		{
			strcat( destination, "-" );
		}
		else
		{
			if ( need_comma ) strcat( destination, ", " );
		}
		strcat( destination, ones [ integer_part ] );

		if ( integer_part > 1 )
			singular = 0;
	}

	if ( !omit_dollars )
	{
		if ( singular )
			strcat( destination, " Dollar" );
		else
			strcat( destination, " Dollars" );
	}
}

void pennies_part(	char *destination,
			int decimal_part )
{
	char pennies[ 3 ];

	if ( !decimal_part )
		strcpy( pennies, "0" );
	else
		sprintf( pennies, "%d", decimal_part );

	strcat( destination, " and ");
	strcat( destination, pennies);
	strcat( destination, "/100");
}

