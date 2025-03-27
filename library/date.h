/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/date.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DATE_H
#define DATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "dictionary.h"

#define DATE_GMT		"Etc/GMT"
#define DATE_DEFAULT_UTC_OFFSET	-5
#define SECONDS_IN_DAY		86400
#define SECONDS_IN_MINUTE	60
#define SECONDS_IN_WEEK		( SECONDS_IN_DAY * 7 )
#define SECONDS_IN_HOUR		( SECONDS_IN_MINUTE * 60 )
#define SECONDS_IN_HALF_HOUR	( SECONDS_IN_MINUTE * 30 )

#define WDAY_SUNDAY			0
#define WDAY_MONDAY			1
#define WDAY_TUESDAY			2
#define WDAY_WEDNESDAY			3
#define WDAY_THURSDAY			4
#define WDAY_FRIDAY			5
#define WDAY_SATURDAY			6

#define DATE_END_OF_WEEK_INDICATOR	4

typedef struct
{
	time_t current;
	struct tm *tm;
	boolean is_daylight_time;
} DATE;

DATE *date_new(
		int year,
		int month,
		int day );

DATE *date_time_new(
		int year,
		int month,
		int day,
		int hour,
		int minute,
		int second );

DATE *date_increment_week(
		DATE *d );

DATE *increment_week(
		DATE *d );

DATE *date_increment_day(
		DATE *d );

DATE *date_decrement_day(
		DATE *d );

DATE *date_increment(
		DATE *d );

/* ------------------- */
/* Trims trailing time */
/* ------------------- */
DATE *date_yyyy_mm_dd_new(
		char *date_string );

DATE *date_yyyymmdd_new(
		char *date_string );

DATE *date_19new(
		char *date_time_string );

DATE *date_yyyy_mm_dd_hms_new(
		char *date_time_string );

DATE *date_yyyy_mm_dd_hm_new(
		char *date_time_string );

DATE *date_increment_hours(
		DATE *date,
			double hours );

DATE *date_add_seconds(
		DATE *date,
		int seconds );

DATE *date_increment_second(
		DATE *date,
		int seconds );

DATE *date_increment_seconds(
		DATE *date,
		int seconds );

DATE *date_decrement_second(
		DATE *date,
		int seconds );

DATE *date_increment_minutes(
		DATE *date,
		int minutes );

DATE *date_add_minutes(
		DATE *date,
		int minutes );

int date_month_integer(
		DATE *date );

int date_month_day_integer(
		DATE *date );

int date_day_offset(
		DATE *date );

/* Starts on zero */
/* -------------- */
int date_days_between(
		char *early_date,
		char *late_date );

int date_years_between(
		char *early_date,
		char *late_date );

int date_minutes_between(
		char *early_date_string,
		char *early_time_string,
		char *late_date_string,
       		char *late_time_string,
		boolean add_one );

int date_age(	char *birth_date );

void date_add_slashes_maybe(
		char *d,
		char *s );

/* Returns heap memory */
/* ------------------- */
char *date_yyyy_mm_dd(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_hhmm(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_colon_hhmmss(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_hhmmss(
		DATE *date );

/* Returns heap memory or "" */
/* ------------------------- */
char *date_display_yyyy_mm_dd(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_yyyy_mm_dd_hhmm_display(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_display19(
		DATE *date );

char *date_display_19(
		DATE *date );

char *date_display_yyyy_mm_dd_colon_hms(
		DATE *date );

char *date_yyyy_mm_dd_colon_hms_display(
		DATE *date );

char *date_hms(	DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_display_yyyy_mm_dd_hhmm(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_now_yyyy_mm_dd_hhmm(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_display_yyyy_mm_dd_colon_hhmm(
		DATE *date );

/* Returns heap memory or "" */
/* ------------------------- */
char *date_yyyy_mm_dd_string(
		DATE *date );

char *date_yesterday_yyyy_mm_dd_string(
		int utc_offset );

DATE *date_today_new(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now_yyyy_mm_dd(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now_hhmm(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now_hhmmss(
		int utc_offset );

char *date_today_yyyy_mm_dd(
		int utc_offset );

char *date_yesterday_yyyy_mm_dd(
			int utc_offset );

DATE *date_new_yyyy_mm_dd_date(
		char *date_field );

DATE *date_new_yyyy_mm_dd(
		char *date_field );

DATE *date_new_yyyy_mm_dd_hhmm(
		char *date_string,
		char *time_string );

DATE *date_yyyy_mm_dd_hhmmss_new(
		char *date_string,
		char *time_string );

boolean date_set_yyyy_mm_dd(
		DATE *date,
		char *yyyy_mm_dd );

boolean date_set_yyyy_mm_dd_hhmm_delimited(
		DATE *date,
		char *yyyy_mm_dd_hhmm,
		int date_piece,
		int time_piece,
		char delimiter,
		int utc_offset );

int date_week_of_year(
		DATE *date );

void date_free( DATE *d );

DATE *date_yyyy_mm_dd_hhmm_new(
		char *date_string,
		char *time_string );

DATE *date_set_time(
		DATE *date,
		int hour,
		int minutes );

DATE *date_set_time_second(
		DATE *date,
		int hour,
		int minute,
		int second );

DATE *date_set_day(
		DATE *date,
		int day,
		int utc_offset );

DATE *date_set_year(
		DATE *date,
		int year,
		int utc_offset );

/* Returns static memory or "" */
/* --------------------------- */
char *date_hhmm_static_display(
		DATE *date );

/* Returns heap memory or "" */
/* ------------------------- */
char *date_hhmm_display(
		DATE *date );

/* Returns heap memory or "" */
/* ------------------------- */
char *date_display_hhmm(
		DATE *date );

/* Returns heap memory or "" */
/* ------------------------- */
char *date_display_hhmmss(
		DATE *date );

int date_subtract_days(
		DATE *later_date,
		DATE *earlier_date );

int date_subtract_minutes(
		DATE *later_date,
		DATE *earlier_date );

int date_week_number(
		DATE *date );

int date_month_number(
		DATE *date );

int date_year_number(
		DATE *date );

int date_day_number(
		DATE *date );

boolean date_set_time_hhmm(
		DATE *date,
		char *hhmm );

boolean date_set_time_hhmmss(
		DATE *date,
		char *hhmmss );

int date_minutes_number(
		DATE *date );

int date_hour_number(
		DATE *date );

double date_yyyy_mm_dd_to_julian(
		char *yyyy_mm_dd );

double date_yyyy_mm_dd_hhmm_to_julian(
		char *yyyy_mm_dd,
		char *hhmm );

/* Returns date_destination or date_calloc() */
/* ----------------------------------------- */
DATE *date_copy(
		DATE *date_destination,
		DATE *date_source );

DATE *date_round2five_minutes(
		DATE *date );

DATE *date_decrement_hour(
		DATE *d );

DATE *date_increment_hour(
		DATE *d );

DATE *date_decrement_minute(
		DATE *d );

DATE *date_increment_minute(
		DATE *d );

/* Returns static memory */
/* --------------------- */
char *date_current_yyyy_mm_dd_string(
		int utc_offset );

char *date_current_hhmm(
		int utc_offset );

/* Returns date */
/* ------------ */
DATE *date_increment_days(
		DATE *date,
		double days );

DATE *date_decrement_days(
		DATE *d,
		double days );

char *date_hhmm_string(
		DATE *d );

DATE *date_calloc(
		void );

DATE *date_new_date_time(
		int year,
		int month,
		int day,
		int hour,
		int minutes,
		int seconds,
		int utc_offset );

DATE *date_set_date_integers(
		DATE *date,
		int year,
		int month,
		int day,
		int utc_offset );

DATE *date_set_time_integers(
		DATE *date,
		int hour,
		int minute,
		int second,
		int utc_offset );

char *date_yyyy_mm_dd_hhmm( 
		DATE *date );

void date_increment_weekly_ceiling(
		DATE *date );

int date_day_of_week(
		DATE *d );

/* Returns static memory */
/* --------------------- */
char *date_now_yyyymmdd(
		void );

/* Returns static memory or "" */
/* --------------------------- */
char *date_display_yyyymmdd(
		DATE *date );

char *date_display(
		DATE *date );

boolean date_same_day(
		DATE *old_date,
		DATE *new_date );

boolean date_compare(
		DATE *date1,
		DATE *date2 );

boolean date_tomorrow(
		DATE *old_date, 
		DATE *new_date );

boolean date_year_in_future(
		DATE *date,
		int year );

char *date_day_of_week_yyyy_mm_dd(
		int wday_of_week,
		int utc_offset );

time_t date_yyyy_mm_dd_time_hhmm_to_time_t(
		char *date_string,
		char *time_string );

void date_set_tm_structures(
		DATE *date,
		time_t current,
		int utc_offset );

void date_output_hour_error(
		char *function_name,
		int hour );

void date_set_tm_structures_pre_1970(
		DATE *date /* in/out */,
		time_t current,
		int utc_offset );

int date_year(	DATE *d );

int date_hour(	DATE *d );

int date_minute(DATE *d );

int date_minutes(
		DATE *d );

int date_second(DATE *d );

int date_seconds(
		DATE *d );

int date_day_of_month(
		DATE *d );

int date_month(	DATE *d );

time_t date_tm_to_current(
		struct tm *tm,
		int utc_offset );

time_t date_tm_to_current_pre_1970(
		struct tm *tm,
		int utc_offset );

time_t date_mktime(
		struct tm *tm,
		int utc_offset );

char *date_day_of_week_string(
		DATE *d );

char *date_display_mdy(
		DATE *date );

int date_month_day_number(
		DATE *date );

int date_days_in_month(
		int month,
		int year );

char *date_prior_day_of_week_yyyy_mm_dd_string(
		DATE *d,
		int day_of_week,
		int utc_offset );

/* Returns static memory */
/* --------------------- */
char *date_yyyy_mm_dd_display(
		DATE *date );

/* Returns static memory or "" */
/* --------------------------- */
char *date_yyyy_mm_dd_static_display(
		DATE *date );

/* Returns static memory or "" */
/* --------------------------- */
char *date_static_display_yyyy_mm_dd(
		DATE *date );

char *date_current_static_yyyy_mm_dd_string(
		time_t current,
		int utc_offset );

char *date_static_display(
		DATE *date );

void date_time_parse(
		int *hours,
		int *minutes,
		char *hhmm );

boolean date_parse(
		int *year,
		int *month,
		int *day,
		char *date_string );

char *date_unix_now_string(
		void );

boolean date_is_greatgreatgrandfather(
		DATE *d );

boolean date_is_greatgrandfather(
		DATE *d );

boolean date_is_grandfather(
		DATE *d );

boolean date_is_sunday(
		DATE *d );

boolean date_is_monday(
		DATE *d );

boolean date_is_tuesday(
		DATE *d );

boolean date_is_wednesday(
		DATE *d );

boolean date_is_thursday(
		DATE *d );

boolean date_is_friday(
		DATE *d );

boolean date_is_saturday(
		DATE *d );

boolean date_is_father(
		DATE *d );

boolean date_is_day_of_week(
		DATE *d,
		int day_of_week );

DATE *date_back_to_first_month(
		DATE *d,
		int utc_offset );

DATE *date_current_new(
		time_t current,
		int utc_offset );

DATE *date_decrement_years(
		DATE *d,
		int years );

DATE *date_increment_months(
		DATE *d,
		int months );

DATE *date_subtract_year(
		DATE *date,
		int years );

DATE *date_increment_years(
		DATE *date,
		int years );

DATE *date_forward_to_first_month(
		DATE *d,
		int utc_offset );

char *date_colon_now_time(
		void );

void date_place_colon_in_time(
		char *time_string );

void date_remove_colon_in_time(
		char *time_string );

DATE *date_prior_day(
		DATE *date,
		int wday_parameter );

DATE *date_prior_saturday(
		DATE *date );

/* Returns static memory or "" */
/* --------------------------- */
char *date_append_hhmmss(
		char *date_string );

/* Returns heap memory */
/* ------------------- */
char *date_now_time_second(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now_time_hhmm(
		int utc_offset );

char *date_remove_colon_from_time(
		char *time_string );

/* Safely returns */
/* -------------- */
DATE *date_now_new(
		int utc_offset );

void date_set_date_time_integers(
		DATE *date,
		int year,
		int month,
		int day,
		int hours,
		int minutes,
		int seconds,
		int utc_offset );

boolean date_set_yyyy_mm_dd_hhmm(
		DATE *date,
		char *yyyy_mm_dd,
		char *hhmm,
		int utc_offset );

int date_last_month_day(
		int month,
		int year );

DATE *date_yyyy_mm_dd_colon_hm_new(
		char *date_time_string );

int date_utc_offset(
		void );

boolean date_name_to_value(
		char *variable_value,
		char *variable_name );

char *date_environment(
		char *variable_name );

/* Returns heap memory */
/* ------------------- */
char *date_time_now(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now16(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_time_display(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_display_yyyy_mm_dd_hh_colon_mm(
		DATE *date );

int date_months_between(DATE *from_date,
		DATE *to_date );

int date_days_in_year(
		int year );

void date_environ_set_TZ(
		char *TZ );

boolean date_set_yyyy_mm_dd_hh_mm_ss_colon(
		DATE *date,
		/* ------------------- */
		/* 2017-12-01 11:00:00 */
		/* ------------------- */
		char *date_time_string );

/* Returns static memory */
/* --------------------- */
char *date_month_number_string(
		DATE *date );

/* Returns static memory */
/* --------------------- */
char *date_day_number_string(
		DATE *date );

/* Safely returns */
/* ------------- */
DATE *date_now(	int utc_offset );

DATE *date_set_now(
		DATE *date,
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now_hh_colon_mm_colon_ss(
		int utc_offset );

int date_seconds(
		DATE *d );

int date_minutes(
		DATE *d );

int date_hour(	DATE *d );

int date_day_of_month(
		DATE *d );

int date_year(	DATE *d );

int date_day(	DATE *d );

int date_month(	DATE *d );

/* Returns heap memory */
/* ------------------- */
char *date_time_now19(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now19(
		int utc_offset );

char *date_display_day_name(
		DATE *date );

char *date_display_month_name(
		DATE *date );

char *date_display_day(
		DATE *date );

/* Returns static memory */
/* --------------------- */
char *date_display_time_ampm(
		DATE *date );

/* Returns program memory */
/* ---------------------- */
char *date_display_ampm(
		DATE *date );

int date_month_name_integer(
		char *month_name );

char *date_month_military(
		int month_offset );

int date_military_month_integer(
		char *military_month );

char *date_month_string(
		int month_offset );

int date_day_of_week(
		DATE *date );

int date_month_offset(
		DATE *date );

int date_day_integer(
		DATE *date );

int date_hour_integer(
		DATE *date );

/* Used when followed by am or pm */
/* ------------------------------ */
int date_hour_time(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_time_display(
		DATE *date );

/* Returns program memory */
/* ---------------------- */
char *date_display_th_st_rd_nd(
		DATE *date );

/* Returns heap memory */
/* ------------------- */
char *date_now_hhmm_colon_ss(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_yyyy_mm_dd_hhmmss(
		DATE *date );

/* Usage */
/* ----- */

/* Returns heap memory or date_string */
/* ---------------------------------- */
char *date_append_time(
		char *date_string,
		int width );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *date_now_colon_hhmm(
		int utc_offset );

/* Returns heap memory */
/* ------------------- */
char *date_now_hh_colon_mm(
		int utc_offset );

boolean date_now_daylight_time(
		void );

boolean date_year_leap_boolean(
		int year );

boolean date_greater_year_boolean(
		char *early_yyyy_mm_dd_string,
		char *late_yyyy_mm_dd_string );

#endif
