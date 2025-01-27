/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/payroll_period.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "name_arg.h"
#include "employee.h"
#include "appaserver_error.h"
#include "date.h"

/* See also $APPASERVER_HOME/src_predictive/employee.h 	*/
/* ---------------------------------------------------- */
#define PAYROLL_PERIOD_NUMBER_LABEL	".payroll_period_number"
#define PAYROLL_BEGIN_DATE_LABEL	".payroll_begin_date"
#define PAYROLL_END_DATE_LABEL		".payroll_end_date"

#define MIDMONTH_DAY			15

int payroll_period_days_shift(
		char *beginday );

char *payroll_period_begin_date_string(
		char *period,
		int year,
		int period_number );

void payroll_period_prior(
		char *period,
		char *beginday );

int payroll_period_output_monthly_period(
		char *date_string );

void payroll_period_output_monthly_dates(
		int year,
		int period_number );

int payroll_period_output_semimonthly_period(
		char *date_string );

void payroll_period_output_semimonthly_dates(
		int year,
		int period_number );

int payroll_period_output_biweekly_period(
		char **year_string,
		char *date_string,
		int days_shift );

void payroll_period_output_biweekly_dates(
		int year,
		int period_number,
		int days_shift );

int payroll_period_output_weekly_period(
		char **year_string,
		char *date_string,
		int days_shift );

void payroll_period_output_weekly_dates(
		int year,
		int period_number,
		int days_shift );

int payroll_period_biweekly_period(
		int week_of_year );

boolean payroll_period_weekly_dates(
		char *begin_work_date,
		char *end_work_date,
		int year,
		int period_number,
		int days_shift );

boolean payroll_period_semimonthly_dates(
		char *begin_work_date,
		char *end_work_date,
		int year,
		int period_number );

boolean payroll_period_monthly_dates(
		char *begin_work_date,
		char *end_work_date,
		int year,
		int period_number );

/* Returns period_number (maybe) */
/* ----------------------------- */
int payroll_period(
		char **year_string,
		char *argv_0,
		char *period,
		char *date_string,
		int period_number,
		int year,
		char *beginday );

void setup_arg(	NAME_ARG *arg,
		int argc,
		char **argv );

void fetch_parameters(
		char **period, 
		char **date_string,
		char **period_number_string,
		char **year_string,
		char **prior_yn,
		char **beginday,
		NAME_ARG *arg );

int main( int argc, char **argv )
{
        NAME_ARG *arg;
	char *period = {0};
	char *prior_yn = "";
	char *beginday = "";
	char *date_string = "";
	char *period_number_string = "";
	char *year_string = "";
	int period_number;

#ifdef EMPLOYEE_PAYROLL_PERIOD_DEBUG
	appaserver_error_usage_stderr( argc, argv );
#endif

	arg = init_arg( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(
			&period,
			&date_string,
			&period_number_string,
			&year_string,
			&prior_yn,
			&beginday,
			arg );

	if ( strcmp( prior_yn, "yes" ) == 0 )
	{
		payroll_period_prior(
			period,
			beginday );
	}
	else
	{
		period_number =
			payroll_period(
				&year_string,
				argv[ 0 ],
				period,
				date_string,
				atoi( period_number_string ),
				atoi( year_string ),
				beginday );

		if ( *date_string && period_number )
		{
			char sys_string[ 1024 ];

			sprintf(sys_string,
				"payroll_period.e"
				" period=%s"
				" number=%d"
				" beginday=%s"
				" year=%d",
				period,
				period_number,
				beginday,
				(*year_string)
					? atoi( year_string )
					: atoi( date_string ) /* year */ );

#ifdef EMPLOYEE_PAYROLL_PERIOD_DEBUG
fprintf( stderr, "%s/%s()/%d: sys_string = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
fflush( stderr );
#endif

			if ( system( sys_string ) ){};
		}
	}

	return 0;
}

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "period" );
	add_possible_value( arg, ticket, "weekly" );
	add_possible_value( arg, ticket, "biweekly" );
	add_possible_value( arg, ticket, "semimonthly" );
	add_possible_value( arg, ticket, "monthly" );

        ticket = add_valid_option( arg, "prior" );
	add_possible_value( arg, ticket, "yes" );
	add_possible_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

	ticket = add_valid_option( arg, "beginday" );
	add_possible_value( arg, ticket, "sunday" );
	add_possible_value( arg, ticket, "monday" );
	add_possible_value( arg, ticket, "tuesday" );
	add_possible_value( arg, ticket, "wednesday" );
	add_possible_value( arg, ticket, "thursday" );
	add_possible_value( arg, ticket, "friday" );
	add_possible_value( arg, ticket, "saturday" );
	set_default_value( arg, ticket, "sunday" );

        ticket = add_valid_option( arg, "date" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "number" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "year" );
        set_default_value( arg, ticket, "" );

        name_arg_insert( arg, argc, argv );
}

void fetch_parameters(	char **period, 
			char **date_string,
			char **period_number_string,
			char **year_string,
			char **prior_yn,
			char **beginday,
			NAME_ARG *arg )
{
	*period = fetch_arg( arg, "period" );
	*date_string = fetch_arg( arg, "date" );
	*period_number_string = fetch_arg( arg, "number" );
	*year_string = fetch_arg( arg, "year" );
	*prior_yn = fetch_arg( arg, "prior" );
	*beginday = fetch_arg( arg, "beginday" );
}

int payroll_period(
		char **year_string,
		char *argv_0,
		char *period,
		char *date_string,
		int period_number,
		int year,
		char *beginday )
{
	boolean output_error = 0;
	int local_period_number = 0;
	int days_shift;

	days_shift = payroll_period_days_shift( beginday );

	if ( timlib_strcmp( period, "weekly" ) == 0 )
	{
		if ( *date_string )
		{
			local_period_number =
				payroll_period_output_weekly_period(
					year_string,
					date_string,
					days_shift );
		}
		else
		if ( period_number )
		{
			payroll_period_output_weekly_dates(
				year,
				period_number,
				days_shift );
		}
		else
		{
			output_error = 1;		
		}
	}
	else
	if ( timlib_strcmp( period, "biweekly" ) == 0 )
	{
		if ( *date_string )
		{
			local_period_number =
				payroll_period_output_biweekly_period(
					year_string,
					date_string,
					days_shift );
		}
		else
		if ( period_number )
		{
			payroll_period_output_biweekly_dates(
				year,
				period_number,
				days_shift );
		}
		else
		{
			output_error = 1;		
		}
	}
	else
	if ( timlib_strcmp( period, "semimonthly" ) == 0 )
	{
		if ( *date_string )
		{
			local_period_number =
				payroll_period_output_semimonthly_period(
					date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_output_semimonthly_dates(
				year,
				period_number );
		}
		else
		{
			output_error = 1;		
		}
	}
	else
	if ( timlib_strcmp( period, "monthly" ) == 0 )
	{
		if ( *date_string )
		{
			local_period_number =
				payroll_period_output_monthly_period(
					date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_output_monthly_dates(
				year,
				period_number );
		}
		else
		{
			output_error = 1;		
		}
	}

	if ( output_error )
	{
		fprintf( stderr,
			 "Error in %s: invalid combination of parameters.\n",
			 argv_0 );
	}

	return local_period_number;
}

int payroll_period_output_weekly_period(
		char **year_string,
		char *date_string,
		int days_shift )
{
	DATE *d;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	if ( days_shift )
	{
		char local_year_string[ 5 ];

		date_increment_days(
			d,
			-(double)days_shift );

		sprintf( local_year_string, "%d", date_year_number( d ) );
		*year_string = strdup( local_year_string );
	}

	period_number = date_week_of_year( d );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;
}

void payroll_period_output_weekly_dates(
		int year,
		int period_number,
		int days_shift )
{
	char begin_work_date[ 16 ];
	char end_work_date[ 16 ];

	*begin_work_date = '\0';
	*end_work_date = '\0';

	if ( period_number < 1 || period_number > 53 ) return;

	if ( payroll_period_weekly_dates(
			begin_work_date,
			end_work_date,
			year,
			period_number,
			days_shift ) )
	{
		printf( "%s %s\n",
			PAYROLL_BEGIN_DATE_LABEL,
			begin_work_date );

		printf( "%s %s\n",
			PAYROLL_END_DATE_LABEL,
			end_work_date );
	}
}

boolean payroll_period_weekly_dates(
		char *begin_work_date,
		char *end_work_date,
		int year,
		int period_number,
		int days_shift )
{
	DATE *d;
	char date_string[ 16 ];
	int week_of_year;
	boolean all_done = 0;
	boolean first_time = 1;

	if ( period_number > 53 ) return 0;

	if ( period_number == 1 )
	{
		sprintf( date_string, "%d-12-25", year - 1 );
	}
	else
	if ( period_number >= 52 )
	{
		sprintf( date_string, "%d-12-15", year );
	}
	else
	{
		sprintf( date_string, "%d-01-01", year );
	}

	d = date_yyyy_mm_dd_new( date_string );

	week_of_year = date_week_of_year_number( d );

	while ( !all_done )
	{
		if ( week_of_year == period_number )
		{
			if ( first_time )
			{
				DATE *d2;

				d2 = date_calloc();
				date_copy( d2, d );

				if ( days_shift )
				{
					date_increment_days(
						d2,
						(double)days_shift );
				}

				strcpy( begin_work_date,
					date_yyyy_mm_dd( d2 ) );

				first_time = 0;
			}
		}

		date_increment_days( d, 1.0 );

		week_of_year = date_week_of_year_number( d );

		if ( period_number == 1 )
		{
			if ( week_of_year == 2 ) all_done = 1;
		}
		else
		if ( period_number == 52 )
		{
			if ( week_of_year == 53
			||   week_of_year == 1 )
			{
				all_done = 1;
			}
		}
		else
		if ( period_number == 53 )
		{
			if ( week_of_year == 1 )
			{
				all_done = 1;
			}
		}
		else
		if ( week_of_year > period_number )
		{
			all_done = 1;
		}
	}

	if ( !*begin_work_date ) return 0;

	date_increment_days(
		d,
		(double)days_shift - 1.0 );

	strcpy( end_work_date,
		date_yyyy_mm_dd( d ) );

	return 1;
}

int payroll_period_output_biweekly_period(
		char **year_string,
		char *date_string,
		int days_shift )
{
	DATE *d;
	int week_of_year;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	if ( days_shift )
	{
		char local_year_string[ 5 ];

		date_increment_days(
			d,
			-(double)days_shift );

		sprintf( local_year_string, "%d", date_year_number( d ) );
		*year_string = strdup( local_year_string );
	}

	week_of_year = date_week_of_year_number( d );

	period_number =
		payroll_period_biweekly_period(
			week_of_year );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;
}

void payroll_period_output_biweekly_dates(
		int year,
		int period_number,
		int days_shift )
{
	char begin_work_date[ 16 ];
	char ignore_work_date[ 16 ];
	char end_work_date[ 16 ];
	int period_number_times_two;

	if ( period_number < 1 || period_number > 27 ) return;

	*begin_work_date = '\0';
	*end_work_date = '\0';

	if ( period_number == 27 )
	{
		if ( !payroll_period_weekly_dates(
			begin_work_date,
			end_work_date,
			year,
			53,
			days_shift ) )
		{
			return;
		}
	}
	else
	{
		period_number_times_two = period_number * 2;

		if ( period_number_times_two >= 54 )
			period_number_times_two = 53;

		if ( !payroll_period_weekly_dates(
				begin_work_date,
				ignore_work_date,
				year,
				period_number_times_two - 1,
				days_shift ) )
		{
			return;
		}

		if ( !payroll_period_weekly_dates(
				ignore_work_date,
				end_work_date,
				year,
				period_number_times_two,
				days_shift ) )
		{
			strcpy( end_work_date, ignore_work_date );
		}
	} /* if not ( period_number == 27 ) */

	printf( "%s %s\n",
		PAYROLL_BEGIN_DATE_LABEL,
		begin_work_date );

	printf( "%s %s\n",
		PAYROLL_END_DATE_LABEL,
		end_work_date );
}

int payroll_period_biweekly_period( int week_of_year )
{
	int period_number;

	period_number = (int)( ( (double)week_of_year / 2.0 ) + 0.5 );

	return period_number;
}

int payroll_period_output_semimonthly_period( char *date_string )
{
	DATE *d;
	int month;
	int day;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	if ( !d ) return 0;

	month = date_month_number( d );
	day = date_day_number( d );

	if ( day <= MIDMONTH_DAY )
	{
		period_number = ( month * 2 ) - 1;
	}
	else
	{
		period_number = month * 2;
	}

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;
}

void payroll_period_output_semimonthly_dates(
		int year,
		int period_number )
{
	char begin_work_date[ 16 ];
	char end_work_date[ 16 ];

	if ( period_number < 1 || period_number > 24 ) return;

	if ( payroll_period_semimonthly_dates(
			begin_work_date,
			end_work_date,
			year,
			period_number ) )
	{
		printf( "%s %s\n",
			PAYROLL_BEGIN_DATE_LABEL,
			begin_work_date );

		printf( "%s %s\n",
			PAYROLL_END_DATE_LABEL,
			end_work_date );
	}
}

boolean payroll_period_semimonthly_dates(
		char *begin_work_date,
		char *end_work_date,
		int year,
		int period_number )
{
	boolean payroll_period_odd;
	int month;
	int begin_day;
	int end_day;

	payroll_period_odd = ( period_number % 2 );

	if ( payroll_period_odd )
	{
		month = ( period_number / 2 ) + 1;
		begin_day = 1;
		end_day = MIDMONTH_DAY;
	}
	else
	{
		month = period_number / 2;
		begin_day = MIDMONTH_DAY + 1;
		end_day = date_last_month_day(
				month,
				year );
	}

	sprintf( begin_work_date, "%d-%.2d-%.2d", year, month, begin_day );
	sprintf( end_work_date, "%d-%.2d-%d", year, month, end_day );

	return 1;
}

boolean payroll_period_monthly_dates(
		char *begin_work_date,
		char *end_work_date,
		int year,
		int period_number )
{
	int end_day;

	end_day =
		date_last_month_day(
			period_number,
			year );

	sprintf( begin_work_date, "%d-%.2d-01", year, period_number );
	sprintf( end_work_date, "%d-%.2d-%d", year, period_number, end_day );

	return 1;
}

int payroll_period_output_monthly_period( char *date_string )
{
	DATE *d;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	if ( !d ) return 0;

	period_number = date_month_number( d );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;
}

void payroll_period_output_monthly_dates(
		int year,
		int period_number )
{
	char begin_work_date[ 16 ];
	char end_work_date[ 16 ];

	if ( period_number < 1 || period_number > 12 ) return;

	if ( payroll_period_monthly_dates(
			begin_work_date,
			end_work_date,
			year,
			period_number ) )
	{
		printf( "%s %s\n",
			PAYROLL_BEGIN_DATE_LABEL,
			begin_work_date );

		printf( "%s %s\n",
			PAYROLL_END_DATE_LABEL,
			end_work_date );
	}
}

char *payroll_period_begin_date_string(
		char *period,
		int year,
		int period_number )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf(sys_string,
		"payroll_period.e	 "
		" period=%s		 "
		" number=%d		 "
		" year=%d		|"
		"grep %s		|"
		"column.e 1		 ",
		period,
		period_number,
		year,
		PAYROLL_BEGIN_DATE_LABEL );

#ifdef EMPLOYEE_PAYROLL_PERIOD_DEBUG
fprintf( stderr, "%s/%s()/%d: sys_string = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
fflush( stderr );
#endif

	results = pipe2string( sys_string );

	if ( !results || !*results )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get begin_date_string.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return results;
}

void payroll_period_prior(
		char *period,
		char *beginday )
{
	DATE *d;
	int week_of_year;
	int period_number;
	int month;
	int day;
	int year;
	char *begin_date_string;
	char sys_string[ 1024 ];
	int days_shift;

	d = date_now_new( date_utc_offset() );

	days_shift = payroll_period_days_shift( beginday );

	if ( days_shift )
	{
		date_increment_days(
			d,
			-(double)days_shift );
	}

	month = date_month_number( d );
	day = date_day_number( d );
	year = date_year_number( d );

	if ( timlib_strcmp( period, "weekly" ) == 0 )
	{
		period_number =
			date_week_of_year_number( d );

	}
	else
	if ( timlib_strcmp( period, "biweekly" ) == 0 )
	{
		week_of_year =
			date_week_of_year_number( d );

		period_number =
			payroll_period_biweekly_period(
				week_of_year );
	}
	else
	if ( timlib_strcmp( period, "semimonthly" ) == 0 )
	{
		if ( day < MIDMONTH_DAY )
		{
			period_number = ( month * 2 ) - 1;
		}
		else
		{
			period_number = month * 2;
		}
	}
	else
	if ( timlib_strcmp( period, "monthly" ) == 0 )
	{
		period_number = month;
	}

	begin_date_string =
		payroll_period_begin_date_string(
			period,
			year,
			period_number );

	d = date_yyyy_mm_dd_new( begin_date_string );

	date_increment_days( d, -1.0 );

	begin_date_string = date_yyyy_mm_dd( d );

	sprintf(sys_string,
		"payroll_period.e"
		" period=%s"
		" date=%s"
		" beginday=%s",
		period,
		begin_date_string,
		beginday );

#ifdef EMPLOYEE_PAYROLL_PERIOD_DEBUG
fprintf( stderr, "%s/%s()/%d: sys_string = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
fflush( stderr );
#endif

	if ( system( sys_string ) ){};
}

int payroll_period_days_shift( char *beginday )
{
	if ( strcmp( beginday, "sunday" ) == 0 )
		return 0;
	else
	if ( strcmp( beginday, "monday" ) == 0 )
		return 1;
	else
	if ( strcmp( beginday, "tuesday" ) == 0 )
		return 2;
	else
	if ( strcmp( beginday, "wednesday" ) == 0 )
		return 3;
	else
	if ( strcmp( beginday, "thursday" ) == 0 )
		return 4;
	else
	if ( strcmp( beginday, "friday" ) == 0 )
		return 5;
	else
	if ( strcmp( beginday, "saturday" ) == 0 )
		return 6;
	else
		return 6;
}
