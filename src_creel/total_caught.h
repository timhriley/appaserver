/* ---------------------------------------------------	*/
/* src_creel/total_caught.h				*/
/* ---------------------------------------------------	*/
/* This is the header file for the Species Estimated    */
/* Total Caught Algorithm.				*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef TOTAL_CAUGHT_H
#define TOTAL_CAUGHT_H

#include "list.h"
#include "dictionary.h"
#include "date.h"
#include "hash_table.h"

/* Enumerated Types */
/* ---------------- */
enum performed_census_weekend {	day_saturday_census_saturday,
				day_sunday_census_sunday,
				day_saturday_census_missed,
				day_sunday_census_missed_saturday_not_missed,
				day_sunday_census_weekend_missed,
				day_weekday_census_saturday_only,
				day_weekday_census_sunday_only,
				day_weekday_census_saturday_and_sunday,
				day_weekday_census_missed_saturday_and_sunday };

/* Constants */
/* --------- */
#define AERIAL_SURVEY_ACCURACY_YEAR		1960
#define WEEKENDS_TO_GO_BACK			20

#define DAYS_PER_MONTH				31
#define MONTHS_PER_YEAR				12
#define DICTIONARY_KEY_DELIMITER		'|'

#define FLAMINGO_BEGINNING_FISHING_AREA		1
#define FLAMINGO_ENDING_FISHING_AREA		5
#define EVERGLADES_CITY_FISHING_AREA		6

#define PARKING_LOT_FLORIDA_BAY			"florida_bay"
#define PARKING_LOT_WHITEWATER_BAY		"white_water_bay"

#define INTERVIEW_LOCATION_FLAMINGO		"flamingo"
#define INTERVIEW_LOCATION_EVERGLADES_CITY	"everglades_city"

/* Structures */
/* ---------- */
typedef struct
{
	char *family;
	char *genus;
	char *species_name;
	int florida_state_code;
} SPECIES;

typedef struct
{
	SPECIES *species;
	int kept;
	int released;
} CATCH;

typedef struct
{
	char *fishing_purpose;
	DATE *census_date;
	char *interview_location;
	int interview_number;
	int number_of_people_fishing;
	int hours_fishing;
	int fishing_area_integer;
	char *trip_origin_location;
	LIST *catch_list;
} FISHING_TRIP;

typedef struct
{
	LIST *fishing_trip_list;
} FISHING_TRIP_LIST;

typedef struct
{
	int begin_month;
	int end_month;
	int year;
	double areas_1_5_coefficient;
	double areas_1_5_y_intercept;
	double area_6_coefficient;
	double area_6_y_intercept;
	LIST *species_list;
	DICTIONARY *trailer_count_dictionary;
	HASH_TABLE *fishing_trip_hash_table;
	FISHING_TRIP_LIST *fishing_trip_list_array	[ MONTHS_PER_YEAR ]
							[ DAYS_PER_MONTH ];
	DICTIONARY *weekend_creel_census_dictionary;
} ESTIMATED_TOTAL_CAUGHT_INPUT;

typedef struct
{
	int areas_1_5_sample_kept_saturday;
	int areas_1_5_sample_kept_sunday;
	int areas_1_5_sample_released_saturday;
	int areas_1_5_sample_released_sunday;
	int area_6_sample_kept_saturday;
	int area_6_sample_kept_sunday;
	int area_6_sample_released_saturday;
	int area_6_sample_released_sunday;
	int areas_1_5_sample_kept_weekend;
	int area_6_sample_kept_weekend;
	int areas_1_5_sample_released_weekend;
	int area_6_sample_released_weekend;
	double areas_1_5_sample_kept;
	double areas_1_5_sample_released;
	double areas_1_5_sample_caught;
	double area_6_sample_kept;
	double area_6_sample_released;
	double area_6_sample_caught;
	double areas_1_6_sample_kept;
	double areas_1_6_sample_released;
	double areas_1_6_sample_caught;
	double average_kept_per_flamingo_fishing_vessel;
	double average_released_per_flamingo_fishing_vessel;
	double average_kept_per_everglades_city_fishing_vessel;
	double average_released_per_everglades_city_fishing_vessel;
	double areas_1_5_estimated_kept;
	double areas_1_5_estimated_released;
	double areas_1_5_estimated_caught;
	double area_6_estimated_kept;
	double area_6_estimated_released;
	double area_6_estimated_caught;
	double areas_1_6_estimated_kept;
	double areas_1_6_estimated_released;
	double areas_1_6_estimated_caught;
	SPECIES *species;
} CATCH_AREA;

typedef struct
{
	int florida_bay_trailer_count;
	int whitewater_bay_trailer_count;
	int total_trailer_count;
	double estimated_areas_1_5_vessels;
	double estimated_area_6_vessels;
	double estimated_areas_1_6_vessels;
	double estimated_flamingo_fishing_vessels;
	double estimated_everglades_city_fishing_vessels;
	double estimated_park_fishing_vessels;
	double flamingo_fishing_boating_ratio;
	double everglades_city_fishing_boating_ratio;
} MONTH_ROW_VESSEL;

typedef struct
{
	int flamingo_fishing_saturday;
	int flamingo_pleasure_saturday;
	int flamingo_fishing_sunday;
	int flamingo_pleasure_sunday;
	int flamingo_fishing_weekend;
	int flamingo_boating_weekend;
	double flamingo_fishing_day;
	double flamingo_boating_day;
	int flamingo_fishing_hours_saturday;
	int flamingo_fishing_hours_sunday;
	int flamingo_fishing_hours_weekend;
	double flamingo_fishing_hours_day;
	int flamingo_angler_count_saturday;
	int flamingo_angler_count_sunday;
	int flamingo_angler_count_weekend;
	double flamingo_angler_count_day;
	double flamingo_fishing_effort_hours_day;
	double average_effort_hours_per_flamingo_fishing_vessel;
	double estimated_flamingo_effort_hours;

	int everglades_city_fishing_saturday;
	int everglades_city_pleasure_saturday;
	int everglades_city_fishing_sunday;
	int everglades_city_pleasure_sunday;
	int everglades_city_fishing_weekend;
	int everglades_city_boating_weekend;
	double everglades_city_fishing_day;
	double everglades_city_boating_day;
	int everglades_city_fishing_hours_saturday;
	int everglades_city_fishing_hours_sunday;
	int everglades_city_fishing_hours_weekend;
	double everglades_city_fishing_hours_day;
	int everglades_city_angler_count_saturday;
	int everglades_city_angler_count_sunday;
	int everglades_city_angler_count_weekend;
	double everglades_city_angler_count_day;
	double everglades_city_fishing_effort_hours_day;
	double average_effort_hours_per_everglades_city_fishing_vessel;
	double estimated_everglades_city_effort_hours;

	DATE *flamingo_inverview_date_saturday;
	DATE *flamingo_interview_date_sunday;
	DATE *everglades_city_inverview_date_saturday;
	DATE *everglades_city_interview_date_sunday;
	double park_fishing_hours_day;
	double average_hours_per_everglades_city_fishing_vessel;
	double park_fishing_day;
	double park_boating_day;
	double park_effort_hours_day;
} MONTH_ROW_FISHING_TRIP;

typedef struct
{
	int day;
	DATE *row_date;
	LIST *catch_area_list;
	enum performed_census_weekend performed_flamingo_census_weekend;
	enum performed_census_weekend performed_everglades_city_census_weekend;
	DATE *flamingo_interview_date_saturday;
	DATE *flamingo_interview_date_sunday;
	DATE *everglades_city_interview_date_saturday;
	DATE *everglades_city_interview_date_sunday;
	MONTH_ROW_VESSEL *vessel;
	MONTH_ROW_FISHING_TRIP *fishing_trip;
} MONTH_ROW;

typedef struct
{
	int month;
	MONTH_ROW *total_row;
	LIST *row_list;
} MONTH_SHEET;

typedef struct
{
	char *month;
	int florida_bay_trailer_count;
	int whitewater_bay_trailer_count;
	int total_trailer_count;
	double flamingo_fishing_day;
	double flamingo_boating_day;
	double park_fishing_day;
	double park_boating_day;
	double park_effort_hours_day;
	double estimated_areas_1_5_vessels;
	double estimated_area_6_vessels;
	double estimated_areas_1_6_vessels;
	double estimated_park_fishing_vessels;
	LIST *catch_area_list;
} TOTAL_ROW;

typedef struct
{
	int year;
	LIST *total_row_list;
	TOTAL_ROW *total_row;
} TOTAL_SHEET;

typedef struct
{
	double grand_areas_1_5_estimated_kept;
	double grand_areas_1_5_estimated_released;
	double grand_areas_1_5_estimated_caught;
	double grand_areas_1_6_estimated_kept;
	double grand_areas_1_6_estimated_released;
	double grand_areas_1_6_estimated_caught;
	LIST *month_sheet_list;
	TOTAL_SHEET *total_sheet;
} ESTIMATED_TOTAL_CAUGHT_OUTPUT;

typedef struct
{
	ESTIMATED_TOTAL_CAUGHT_INPUT *input;
	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output;
} ESTIMATED_TOTAL_CAUGHT;

/* Prototypes */
/* ---------- */
CATCH *total_caught_catch_new(	void );

SPECIES *total_caught_species_new(
				char *family,
				char *genus,
				char *species_name );

FISHING_TRIP *total_caught_fishing_trip_new(
				char *fishing_purpose,
				char *census_date_string,
				char *interview_location,
				int interview_number,
				char *trip_origin_location );

ESTIMATED_TOTAL_CAUGHT *estimated_total_caught_new(
				int begin_month,
				int end_month,
				int year,
				char *application_name );

ESTIMATED_TOTAL_CAUGHT_INPUT *total_caught_input_new(
				int begin_month,
				int end_month,
				int year,
				char *application_name );

DICTIONARY *total_caught_get_trailer_count_dictionary(
				char *application_name,
				int begin_month,
				int end_month,
				int year );

LIST *total_caught_get_species_list(
				boolean *all_species,
				char *application_name,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string );

HASH_TABLE *total_caught_get_fishing_trip_hash_table(
				FISHING_TRIP_LIST *fishing_trip_list_array
							[ MONTHS_PER_YEAR ]
							[ DAYS_PER_MONTH ],
				char *application_name,
				int begin_month,
				int end_month,
				int year );

char *total_caught_get_fishing_trip_key(
				char *fishing_purpose,
				char *census_date_string,
				char *interview_location,
				char *interview_number_string );

char *total_caught_get_species_key(
				char *family,
				char *genus,
				char *species_name );

char *total_caught_get_weekend_key(
				char *census_date_string,
				char *interview_location );

char *total_caught_get_trailer_key(
				char *trailer_count_date_string,
				char *parking_lot );

void total_caught_populate_catch_list(
				HASH_TABLE *fishing_trip_hash_table,
				LIST *species_list,
				char *application_name,
				int begin_month,
				int end_month,
				int year );

ESTIMATED_TOTAL_CAUGHT_OUTPUT *total_caught_get_output(
				int begin_month,
				int end_month,
				int year,
				LIST *species_list,
				DICTIONARY *trailer_count_dictionary,
				FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
				DICTIONARY *weekend_creel_census_dictionary,
				double areas_1_5_coefficient,
				double areas_1_5_y_intercept,
				double area_6_coefficient,
				double area_6_y_intercept,
				boolean omit_catch_area_list );

void total_caught_get_begin_end_date_string(
				char *begin_date_string,
				char *end_date_string,
				int begin_month,
				int end_month,
				int year );

ESTIMATED_TOTAL_CAUGHT_OUTPUT *total_caught_output_new(
				void );

LIST *total_caught_get_month_sheet_list(
			int begin_month,
			int end_month,
			int year,
			DICTIONARY *trailer_count_dictionary,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			LIST *species_list,
			DICTIONARY *weekend_creel_census_dictionary,
			double areas_1_5_coefficient,
			double areas_1_5_y_intercept,
			double area_6_coefficient,
			double area_6_y_intercept,
			boolean omit_catch_area_list );

MONTH_SHEET *total_caught_get_month_sheet(
				int month,
				int year,
				DICTIONARY *trailer_count_dictionary,
				FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
				LIST *species_list,
				DICTIONARY *weekend_creel_census_dictionary,
				double areas_1_5_coefficient,
				double areas_1_5_y_intercept,
				double area_6_coefficient,
				double area_6_y_intercept,
				boolean omit_catch_area_list );

MONTH_SHEET *total_caught_month_sheet_new(
				int month );

MONTH_ROW *total_caught_month_row_new(
				int day );

MONTH_ROW *total_caught_get_month_row(
			int day,
			char *row_date_string,
			DICTIONARY *trailer_count_dictionary,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			LIST *species_list,
			DICTIONARY *weekend_creel_census_dictionary,
			double areas_1_5_coefficient,
			double areas_1_5_y_intercept,
			double area_6_coefficient,
			double area_6_y_intercept,
			boolean omit_catch_area_list );

int total_caught_get_fishing_interviews_count(
				DATE *search_date,
				FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
				boolean is_fishing_trips,
				boolean is_flamingo );

char *total_caught_get_trailer_count_weekend_string(
				DATE *row_date,
				boolean is_saturday );

LIST *total_caught_get_catch_area_list(
	FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
	DATE *flamingo_interview_date_saturday,
	DATE *flamingo_interview_date_sunday,
	DATE *everglades_city_interview_date_saturday,
	DATE *everglades_city_interview_date_sunday,
	LIST *species_list,
	double flamingo_fishing_day,
	double everglades_city_fishing_day,
	double estimated_areas_1_5_fishing_vessels,
	double estimated_area_6_fishing_vessels,
	enum performed_census_weekend performed_flamingo_census_weekend,
	enum performed_census_weekend performed_everglades_city_census_weekend);

CATCH_AREA *total_caught_catch_area_new(
				void );

void total_caught_set_catch_area(
				CATCH_AREA *catch_area,
				CATCH *catch,
				int area,
				int flamingo_fishing_trips_day,
				int flamingo_boating_trips_day,
				double areas_1_5_vessels,
				int park_fishing_trips_day,
				int park_boating_trips_day,
				double total_area_vessels );

SPECIES *total_caught_seek_species(
				LIST *species_list,
				char *family,
				char *genus,
				char *species_name );

CATCH *total_caught_get_or_set_fishing_trip_catch(
				LIST *catch_list,
				char *family,
				char *genus,
				char *species_name,
				SPECIES *species );

CATCH_AREA *total_caught_get_or_set_catch_area(
				LIST *catch_area_list,
				char *family,
				char *genus,
				char *species_name,
				SPECIES *species);

MONTH_ROW *total_caught_get_month_sheet_total_row(
				LIST *month_sheet_row_list );

/*
void total_caught_get_month_row_fishing_trip_weekend(
			int *flamingo_fishing_trips_saturday,
			int *flamingo_fishing_trips_sunday,
			int *flamingo_pleasure_trips_saturday,
			int *flamingo_pleasure_trips_sunday,
			int *everglades_city_fishing_trips_saturday,
			int *everglades_city_fishing_trips_sunday,
			int *everglades_city_pleasure_trips_saturday,
			int *everglades_city_pleasure_trips_sunday,
			DATE *flamingo_fishing_trip_search_date_saturday,
			DATE *flamingo_fishing_trip_search_date_sunday,
			DATE *everglades_city_fishing_trip_search_date_saturday,
			DATE *everglades_city_fishing_trip_search_date_sunday,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ] );
*/

int total_caught_get_catches_weekend(
				LIST *fishing_trip_list,
				char *family,
				char *genus,
				char *species_name,
				boolean is_areas_1_5,
				boolean is_kept );

TOTAL_SHEET *total_caught_get_total_sheet(
				LIST *month_sheet_list,
				int begin_month,
				int year );

TOTAL_SHEET *total_caught_total_sheet_new(
				int year );

TOTAL_ROW *total_caught_total_row_new(
				char *month );

TOTAL_ROW *total_caught_get_total_sheet_total_row(
				LIST *total_row_list );

void total_caught_populate_total_row(
				TOTAL_ROW *total_row,
				LIST *month_row_list );

void total_caught_populate_grand_totals(
				double *grand_areas_1_5_estimated_kept,
				double *grand_areas_1_5_estimated_released,
				double *grand_areas_1_5_estimated_caught,
				double *grand_areas_1_6_estimated_kept,
				double *grand_areas_1_6_estimated_released,
				double *grand_areas_1_6_estimated_caught,
				LIST *catch_area_list );

DICTIONARY *total_caught_get_weekend_creel_census_dictionary(
				char *application_name,
				int begin_month,
				int end_month,
				int year );

enum performed_census_weekend total_caught_get_performed_census_weekend(
				DATE *row_date,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location );

DATE *total_caught_get_fishing_trip_search_date(
				char *row_search_date_string,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location,
				boolean is_saturday );

FISHING_TRIP_LIST *total_caught_fishing_trip_list_new(
				void );

boolean	total_caught_get_census_date_month_day(
				int *census_date_month,
				int *census_date_day,
				char *census_date_string );

LIST *total_caught_with_date_get_fishing_trip_list(
				FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
				char *date_string );

boolean total_caught_get_coefficients(
				double *areas_1_5_coefficient,
				double *areas_1_5_y_intercept,
				double *area_6_coefficient,
				double *area_6_y_intercept,
				char *application_name );

boolean total_caught_exists_census(
				char *census_date_string,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location );

int total_caught_get_total_trailer_count(
				int *florida_bay_trailer_count,
				int *whitewater_bay_trailer_count,
				DATE *row_date,
				DICTIONARY *trailer_count_dictionary );

void total_caught_get_fishing_interviews_day(
			double *fishing_interviews_day,
			double *pleasure_interviews_day,
			double *boating_interviews_day,
			int *fishing_interviews_saturday,
			int *pleasure_interviews_saturday,
			int *boating_interviews_saturday,
			int *fishing_interviews_sunday,
			int *pleasure_interviews_sunday,
			int *boating_interviews_sunday,
			int *fishing_interviews_weekend,
			int *pleasure_interviews_weekend,
			int *boating_interviews_weekend,
			enum performed_census_weekend performed_census_weekend,
			DATE *row_date,
			DICTIONARY *weekend_creel_census_dictionary,
	  		FISHING_TRIP_LIST *fishing_trip_list_array
							[ MONTHS_PER_YEAR ]
							[ DAYS_PER_MONTH ],
			char *interview_location );

DATE *total_caught_get_preceeding_full_census_saturday(
			DATE *row_date,
			DICTIONARY *weekend_creel_census_dictionary,
			char *interview_location );

void total_caught_get_preceeding_weekend(
			double *fishing_day,
			double *pleasure_day,
			double *boating_day,
			int *fishing_saturday,
			int *pleasure_saturday,
			int *boating_saturday,
			int *fishing_sunday,
			int *pleasure_sunday,
			int *boating_sunday,
			int *fishing_weekend,
			int *pleasure_weekend,
			int *boating_weekend,
			DATE *row_date,
			DICTIONARY *weekend_creel_census_dictionary,
	  		FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			char *interview_location );

boolean total_caught_get_saturday_sunday_interview_dates(
		DATE **interview_date_saturday,
		DATE **interview_date_sunday,
		DATE *row_date,
		enum performed_census_weekend performed_census_weekend,
		DICTIONARY *weekend_creel_census_dictionary,
		char *interview_location );

MONTH_ROW_VESSEL *total_caught_month_row_vessel_new(
			void );

MONTH_ROW_FISHING_TRIP *total_caught_month_row_fishing_trip_new(
			void );

int total_caught_get_fishing_trips_weekend(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_fishing_trips,
			boolean is_flamingo );

int total_caught_get_fishing_hours_weekend(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_flamingo );

MONTH_ROW_FISHING_TRIP *total_caught_month_row_get_fishing_trip(
			DATE *flamingo_interview_date_saturday,
			DATE *flamingo_interview_date_sunday,
			DATE *everglades_city_interview_date_saturday,
			DATE *everglades_city_interview_date_sunday,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			enum performed_census_weekend
				performed_flamingo_census_weekend,
			enum performed_census_weekend
				performed_everglades_city_census_weekend );

MONTH_ROW_VESSEL *total_caught_month_row_get_vessel(
			DATE *row_date,
			DICTIONARY *trailer_count_dictionary,
			double areas_1_5_coefficient,
			double areas_1_5_y_intercept,
			double area_6_coefficient,
			double area_6_y_intercept,
			double flamingo_fishing_day,
			double flamingo_boating_day,
			double everglades_city_fishing_day,
			double everglades_city_boating_day );

int total_caught_get_angler_count_weekend(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_flamingo );

#endif
