/* calculate_full_year.h */
/* --------------------- */
/* Input: 	two digit year			*/
/* Output:	full year (with century added) 	*/
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#ifndef CALCULATE_FULL_YEAR_H
#define CALCULATE_FULL_YEAR_H

int calculate_full_year( int *difference,
			 int year_entered, 
			 int years_future_event,
			 int optional_current_year );
int get_full_year_with_sed( char *sed_command, int optional_current_year );
int get_right_two_digits_of_current_year( int optional_current_year );
int get_left_two_digits_of_current_year( int optional_current_year );
int get_current_century( int optional_current_year );
int get_previous_century( int optional_current_year );
int get_next_century( int optional_current_year );

#endif
