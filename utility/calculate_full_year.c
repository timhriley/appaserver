/* $APPASERVER_HOME/utility/calculate_full_year.c	*/
/* ---------------------------------------------------- */
/* Input: 	two digit year				*/
/* Output:	full year (with century added) 		*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include "calculate_full_year.h"

int calculate_full_year( int *difference,
			 int year_entered, 
			 int years_future_event,
			 int optional_current_year )
{
	int current_century;
	int previous_century;
	int next_century;
	int right_two_digits;
	int full_year;
	
	/* If entered a 4 digit year then just return it. */
	/* ---------------------------------------------- */
	if ( year_entered >= 1900 ) return year_entered;

	current_century = get_current_century( optional_current_year );
	previous_century = get_previous_century( optional_current_year );
	next_century = get_next_century( optional_current_year );
	right_two_digits = 
		get_right_two_digits_of_current_year( optional_current_year );

	*difference = year_entered - right_two_digits;

	if ( right_two_digits > years_future_event )
	{
		if ( year_entered > years_future_event || *difference >= 0 )
			full_year = current_century +
				    year_entered;
		else
			full_year = next_century +
				    year_entered;
	}
	else
	{
		if ( year_entered <= years_future_event || *difference < 0 )
			full_year = current_century +
				    year_entered;
		else
			full_year = previous_century +
				    year_entered;
	}

	return full_year;
}


int get_right_two_digits_of_current_year( int optional_current_year )
{
	return 
	get_full_year_with_sed( "\\(..\\)\\(.*\\)/\\2", optional_current_year );

}

int get_left_two_digits_of_current_year( int optional_current_year )
{
	return 
	get_full_year_with_sed( "\\(..\\)\\(.*\\)/\\1", optional_current_year );

}

int get_current_century( int optional_current_year )
{
	return 
	get_left_two_digits_of_current_year( optional_current_year ) * 100;

}

int get_next_century( int optional_current_year )
{
	int current_century = get_current_century( optional_current_year );
	return current_century + 100;

}

int get_previous_century( int optional_current_year )
{
	int current_century = get_current_century( optional_current_year );
	return current_century - 100;

}

int get_current_year( int optional_current_year )
{
	return get_full_year_with_sed( ".*", optional_current_year );

}


int get_full_year_with_sed( char *sed_command, int optional_current_year )
{
	int full_year;
	FILE *p;
	char sys_str[ 1024 ];

	if ( optional_current_year )
	{
		sprintf(sys_str,
		 	"echo %d			|"
		 	"sed 's/%s/'			 ",
			optional_current_year,
			sed_command );
	}
	else
	{
		sprintf(sys_str,
		 	"current_year.sh		|"
		 	"sed 's/%s/'			 ",
			sed_command );
	}

	p = popen( sys_str, "r" );
	if ( fscanf( p, "%d", &full_year ) ){}
	pclose( p );
	return full_year;
}

