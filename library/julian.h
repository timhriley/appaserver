/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/julian.h					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef JULIAN_H
#define JULIAN_H

#include "aggregate_level.h"

#define JULIAN_ADJUSTMENT	0.0005
#define DAYS_PER_MINUTE		(1.0 / 1440.0)
#define MINUTES_PER_DAY		1440.0
#define SECONDS_PER_DAY		(MINUTES_PER_DAY * 60.0)

typedef struct
{
	double current;
} JULIAN;

JULIAN *julian_new_julian( double current );
JULIAN *julian_new( double current );
JULIAN *julian_yyyy_mm_dd_new( char *yyyy_mm_dd );
JULIAN *julian_new_yyyy_mm_dd( char *yyyy_mm_dd );
JULIAN *julian_yyyy_mm_dd_hhmm_new( char *yyyy_mm_dd, char *hhmm );

JULIAN *julian_date_time_new(
			int year,
			int month,
			int day,
			int hour,
			int minute );

void julian_free(	JULIAN *julian );

char *julian_display_yyyy_mm_dd( double current );
char *julian_display_yyyymmdd( double current );
char *julian_get_yyyy_mm_dd_string( double current );
char *julian_get_yyyy_mm_dd( double current );
char *julian_get_hhmm_string( double current );
char *julian_get_hhmm( double current );
char *julian_display_hhmm( double current );
char *julian_display_hhmmss( double current );
char *julian_yyyy_mm_dd_string( double current );
char *julian_display( double current );
double julian_yyyy_mm_dd_time_hhmm_to_julian( char *yyyy_mm_dd, char *hhmm );
double julian_yyyymmdd_time_hhmm_to_julian( char *yyyymmdd, char *hhmm );
double julian_yyyy_mm_dd_to_julian( char *yyyy_mm_dd );
char *julian_to_yyyy_mm_dd( double julian );
char *julian_to_yyyymmdd( double julian );
char *julian_to_hhmm( double julian );
char *julian_hhmm_string( double julian );
double greg2jul( int mon, int day, int year, int h, int mi, double se );
void jul2greg( 	double jd, int *m, int *d, int *y,
		int *h, int *mi, double *sec );
double julian_increment_seconds( double julian, int seconds );
double julian_increment_minutes( double julian, int minutes );
double julian_increment_hours( double julian, int hours );
double julian_increment_hours_double( double julian, double hours );
double julian_increment_day( double current );
double julian_decrement_day( double current );
double julian_increment_month( double current );
double julian_increment_year( double current );
int julian_get_year_number( double current );
double julian_add_minutes( double julian, int minutes );
double julian_subtract( JULIAN *begin_julian, JULIAN *end_julian );
void julian_add_julian( JULIAN *julian, double add_julian );
char *julian_make_y2k_year( char *two_digit_year_buffer );

/*
JULIAN *julian_oracle_format_new( char *date_buffer );
char *julian_display_oracle_format( double current );
*/

/* Returns hhmm */
/* ------------ */
char *julian_clean_hhmm(
		char *hhmm );

void julian_get_hour_minute( int *hour, int *minute, char *hhmm );
void julian_set_yyyy_mm_dd_hhmm(	JULIAN *julian,
					char *yyyy_mm_dd,
					char *hhmm );
void julian_copy(			JULIAN *destination_julian,
					JULIAN *source_julian );
int julian_get_month_number( double current );
int julian_get_half_hour_number( double current );
int julian_get_hour_number( double current );
int julian_get_minute_number( double current );
void julian_set_yyyy_mm_dd(	JULIAN *julian,
				char *yyyy_mm_dd );
void julian_set_time_hhmm(	JULIAN *julian,
				char *hhmm );
int julian_get_week_number( double current );
int julian_get_day_number( double current );
int julian_get_day_of_month_number( double current );
void julian_set_year_month_day( JULIAN *julian, int year, int month, int day );

double julian_consistent_date_aggregation(
		double current,
		enum aggregate_level,
		JULIAN *end_date_julian );

void julian_decrement_days(
		JULIAN *julian,
		double number_of_days );

double julian_round_to_half_hour(
		double current );

char *julian_display_yyyy_mm_dd_hhmm(
		double current );

void julian_current_century(
		char *current_century,
		char *prior_century /* in/out */,
		int *current_two_digit_year );

#endif
