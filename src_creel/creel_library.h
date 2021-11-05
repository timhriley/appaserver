/* ---------------------------------------------------	*/
/* src_creel/creel_library.h				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef CREEL_LIBRARY_H
#define CREEL_LIBRARY_H

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DISPLAY_CATCHES_HORIZONTALLY			"horizontally"
#define DISPLAY_CATCHES_VERTICALLY			"vertically"
#define DISPLAY_CATCHES_FILTERED			"filtered"
#define CREEL_LIBRARY_STATISTICS_TEMPLATE		"%s/creel_effort_statistics_%s_%s_%s_%s_%d"

#define CREEL_LIBRARY_FISHING_TRIPS_SELECT "fishing_purpose,census_date,%s.interview_location,interview_number,interview_time,trip_origin_location,fishing_purpose,family,genus,species_preferred,fishing_area,fishing_party_composition,trip_hours,hours_fishing,number_of_people_fishing,permit_code,recreational_angler_reside"

#define CREEL_CENSUS_SPORT			"sport"
#define CREEL_CENSUS_GUIDE			"guide"
#define CREEL_CENSUS_COMMERCIAL			"commercial"
#define CREEL_LIBRARY_UNKNOWN_RESEARCHER	"unknown"


/* Structures */
/* ---------- */
typedef struct
{
	char *family;
	char *genus;
	char *species;
	int caught_or_kept;
} CATCH_PER_UNIT_EFFORT_CATCH;

typedef struct
{
	char *fishing_purpose;
	char *census_date;
	char *interview_location;
	int interview_number;
	int hours_fishing;
	int party_count;
	char *family;
	char *genus;
	char *species_preferred;
	char *fishing_area;
	LIST *catch_list;
} CATCH_PER_UNIT_EFFORT_FISHING_TRIP;

typedef struct
{
	char *census_date;
	char *fishing_purpose;
	char *interview_location;
	int interview_number;
	char *family_preferred;
	char *genus_preferred;
	char *species_preferred;
	char *family_caught;
	char *genus_caught;
	char *species_caught;
	char *fishing_area;
	int hours_fishing;
	int party_count;
	int catches;
	int effort;
	int catch_records_count;
	char *statistics_filename;
	FILE *statistics_pipe;
/*
	char *complete_statistics_filename;
	FILE *complete_statistics_pipe;
*/
} CATCH_PER_UNIT_EFFORT_SPECIES;

typedef struct
{
	LIST *fishing_trip_list;
	LIST *results_list;
	LIST *results_per_species_per_area_list;
	LIST *results_per_species_list;
} CATCH_PER_UNIT_EFFORT;

typedef struct
{
	char fishing_purpose[ 16 ];
	char census_date[ 16 ];
	char day_of_week[ 16 ];
	char interview_location[ 64 ];
	char interview_number[ 16 ];
	char interview_time[ 8 ];
	char trip_origin_location[ 64 ];
	char family[ 64 ];
	char genus[ 64 ];
	char species_preferred[ 64 ];
	char fishing_area[ 16 ];
	char fishing_party_composition[ 64 ];
	char fishing_trip_duration_hours[ 64 ];
	char fishing_duration_hours[ 64 ];
	char number_of_people_fishing[ 64 ];
	char permit_code[ 64 ];
	char recreational_angler_reside[ 64 ];
} FISHING_TRIPS;

typedef struct
{
	char interview_location[ 64 ];
	char interview_number[ 16 ];
	char census_date[ 16 ];
	char fishing_area[ 16 ];
	char family[ 64 ];
	char genus[ 64 ];
	char species[ 64 ];
	char common_name[ 64 ];
	char florida_state_code[ 16 ];
	char catch_measurement_number[ 16 ];
	char length_millimeters[ 16 ];
	int length_millimeters_integer;
	int total_length_millimeters_integer;
	int summary_count;
} CATCH_MEASUREMENTS;

typedef struct
{
	char fishing_purpose[ 16 ];
	char census_date[ 16 ];
	char interview_location[ 64 ];
	int interview_number;
	char family[ 64 ];
	char genus[ 64 ];
	char species[ 64 ];
	char common_name[ 64 ];
	char florida_state_code[ 16 ];
	char kept_count[ 16 ];
	char released_count[ 16 ];
	int caught_integer;
	int kept_integer;
	int released_integer;
	char fishing_area[ 16 ];
	int hours_fishing;
	int number_of_people_fishing;
} CATCHES;

typedef struct
{
	char guide_angler_name[ 128 ];
	char street_address[ 128 ];
	char city[ 64 ];
	char state_code[ 16 ];
	char zip_code[ 16 ];
	char phone_number[ 32 ];
	char email_address[ 64 ];
	char permit_code[ 16 ];
	char issued_date[ 16 ];
	char expiration_date[ 16 ];
} GUIDE_ANGLER;

/* Prototypes */
/* ---------- */
CATCH_PER_UNIT_EFFORT_CATCH *catch_per_unit_effort_catch_seek(
					LIST *catch_list,
					char *family,
					char *genus,
					char *species_name );

CATCH_PER_UNIT_EFFORT_SPECIES *catch_per_unit_effort_species_seek(
					LIST *species_list,
					char *family,
					char *genus,
					char *species_name,
					char *fishing_area );

CATCH_PER_UNIT_EFFORT_SPECIES *catch_per_unit_effort_species_new(
					char *family_caught,
					char *genus_caught,
					char *species_caught,
					char *fishing_area );

CATCH_PER_UNIT_EFFORT_SPECIES *catch_per_unit_effort_get_per_species(
				LIST *species_list,
				char *family,
				char *genus,
				char *species_name,
				char *fishing_area,
				char *appaserver_data_directory );

void creel_library_get_CPUE_results_list(
				LIST *results_list,
				LIST *results_per_species_per_area_list,
				LIST *results_per_species_list,
				LIST *fishing_trip_list,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				boolean summary_only,
				char *appaserver_data_directory );

LIST *creel_library_get_CPUE_fishing_trip_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				char *fishing_purpose,
				char *catch_harvest,
				boolean exclude_zero_catches );

CATCH_PER_UNIT_EFFORT_FISHING_TRIP *catch_per_unit_effort_fishing_trip_new(
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				int interview_number,
				char *family,
				char *genus,
				char *species_preferred,
				char *fishing_area,
				int hours_fishing,
				int party_count );

CATCH_PER_UNIT_EFFORT *creel_library_catch_per_unit_effort_new(
				void );

CATCH_MEASUREMENTS *
	creel_library_get_summary_catch_measurements(
				HASH_TABLE
				   *catch_measurements_hash_table,
				char *family,
				char *genus,
				char *species,
				char *common_name,
				char *florida_state_code );

CATCHES *creel_library_get_summary_catches(
				HASH_TABLE *catches_hash_table,
				char *family,
				char *genus,
				char *species,
				char *common_name,
				char *florida_state_code );

CATCHES *creel_library_get_fishing_area_summary_catches(
				HASH_TABLE *catches_hash_table,
				char *family,
				char *genus,
				char *species,
				char *fishing_area,
				char *common_name,
				char *florida_state_code );

boolean creel_library_get_fishing_trip(
				FISHING_TRIPS *fishing_trips,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location );

boolean creel_library_census_date_get_catches(
				CATCHES *catches,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *begin_date_string,
				char *end_date_string );

boolean creel_library_census_date_get_catches_fresh_database_fetch(
				CATCHES *catches,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number );

boolean creel_library_get_catch_measurements(
				CATCH_MEASUREMENTS
					*catch_measurements,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number );

boolean creel_library_date_range_get_catches(
				CATCHES *catches,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				char *fishing_purpose );

boolean creel_library_date_range_get_catch_measurements(
				CATCH_MEASUREMENTS *catch_measurements,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string );

int creel_library_get_trailer_count(
				char *application_name,
				char *census_date,
				char *parking_lot );

void creel_library_build_family_list_where_clause(
				char *family_list_where_clause,
				char *application_name,
				char *folder_name,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string );

void creel_library_get_fishing_trips_census_date_where_clause(
				char *where_clause,
				char *application_name,
				char *fishing_area_list_string,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_purpose );

boolean creel_library_get_fishing_trips_between_dates(
				FISHING_TRIPS *fishing_trips,
				char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string );

void creel_library_parse_fishing_trips(
				FISHING_TRIPS *fishing_trips,
				char *input_buffer );

char *creel_library_get_fishing_composition(
				char *application_name,
				char *fishing_party_composition,
				char *fishing_party_composition_code );

char *creel_library_get_recreational_angler_reside(
				char *application_name,
				char *recreational_angler_reside,
				char *recreational_angler_reside_code );

char *creel_library_get_trip_origin(
				char *application_name,
				char *trip_origin_location,
				char *trip_origin_location_code );

char *creel_library_get_interview_location(
				char *application_name,
				char *interview_location,
				char *interview_location_code );

char *creel_library_day_of_week(
			char *application_name,
			char *fishing_purpose,
			char *census_date,
			char *interview_location );

char *creel_library_get_researcher(
			char *application_name,
			char *researcher,
			char *researcher_code );

char *creel_library_get_species(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *species_code );

/*
char *creel_library_get_family(
				char *application_name,
				char *family_name,
				char *family_code );
*/

char *creel_library_get_species_hash_table_key(
				char *family,
				char *genus,
				char *species );

CATCHES *creel_library_catches_new(
				void );

CATCH_MEASUREMENTS *creel_library_catch_measurements_new(
				void );

char *creel_library_get_census_researcher(
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location );

char *creel_library_get_census_researcher_code(
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location );

char *creel_library_get_census_hash_table_key(
			char *fishing_purpose,
			char *census_date,
			char *interview_location );

boolean creel_library_get_expired_guide_angler(
				GUIDE_ANGLER *guide_angler,
				char *application_name,
				char *days_offset,
				char *mail_merge_letter,
				boolean order_by_expiration_date );

void creel_library_get_expired_permit_where_clause(
				char *where_clause,
				char *days_offset,
				char *mail_merge_letter );

void creel_library_get_begin_end_dates(
				char **begin_date_string,
				char **end_date_string,
				char *days_offset,
				char *mail_merge_letter );

boolean creel_library_exists_fishing_area(
				char *application_name,
				char *fishing_area );

char *creel_library_get_recreational_angler_reside_code(
				char *application_name,
				char *recreational_angler_reside );

void catch_per_unit_effort_set_detail_species(
				CATCH_PER_UNIT_EFFORT_SPECIES *species,
				char *census_date,
				char *fishing_purpose,
				char *interview_location,
				int interview_number,
				char *family_preferred,
				char *genus_preferred,
				char *species_preferred,
				int hours_fishing,
				int party_count,
				char *family_caught,
				char *genus_caught,
				char *species_caught,
				int caught_integer );

boolean creel_library_exists_selected_species(
				char *family,
				char *genus,
				char *species,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string );

char *creel_library_get_species_fishing_area_hash_table_key(
				char *family,
				char *genus,
				char *species,
				char *fishing_area );

char *creel_library_get_effort_species_statistics_filename(
				char *family,
				char *genus,
				char *species,
				char *fishing_area,
				char *appaserver_data_directory );

void creel_library_fetch_species_summary_statistics_filename(
				double *cpue_mean,
				double *cpue_standard_deviation,
				double *cpue_standard_error_of_mean,
				int *sample_size,
				char *statistics_filename );

void creel_library_close_statistics_pipes(
				LIST *results_species_summary_list );

void creel_library_remove_statistics_files(
				LIST *results_species_summary_list );

boolean creel_library_species_get_measurements(
				CATCH_MEASUREMENTS *catch_measurements,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *family,
				char *genus,
				char *species,
				char *begin_date_string,
				char *end_date_string );

void creel_library_separate_fishing_area_zone(
				char **fishing_area,
				char **fishing_zone,
				char *fishing_area_zone );

LIST *creel_library_get_measurements_record_list(
				char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string );

char *creel_library_get_measurements_record_search_string(
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *family,
				char *genus,
				char *species);

boolean creel_library_catch_exists_measurements(
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *family,
				char *genus,
				char *species,
				char *begin_date_string,
				char *end_date_string );

/*
int creel_library_get_complete_effort(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_purpose );

int creel_library_get_complete_sample_size(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_purpose );
*/

CATCHES *creel_library_get_or_set_catches(
				LIST *catches_list,
				char *family,
				char *genus,
				char *species );

CATCH_PER_UNIT_EFFORT_CATCH *creel_library_catch_per_unit_effort_catch_new(
				char *family,
				char *genus,
				char *species,
				int caught_or_kept );

boolean creel_library_get_fishing_trips_between_dates_preferred_caught(
				FISHING_TRIPS *fishing_trips,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *family,
				char *genus,
				char *species_preferred,
				char *fishing_purpose );

FILE *creel_library_get_fishing_trips_input_pipe(
				char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string,
				char *family,
				char *genus,
				char *species_preferred );

char *creel_library_get_where_preferred_caught(
				char *application_name,
				char *family,
				char *genus,
				char *species_preferred );

LIST *creel_library_get_CPUE_preferred_species_fishing_trip_list(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *preferred_family_string,
					char *preferred_genus_string,
					char *preferred_species_string,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches );

boolean creel_library_date_range_get_fishing_trip(
				FISHING_TRIPS *fishing_trip,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *fishing_purpose,
				char *preferred_family_string,
				char *preferred_genus_string,
				char *preferred_species_string,
				boolean exclude_zero_catches );

FILE *creel_library_date_range_get_fishing_trip_input_pipe(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *fishing_purpose,
				char *preferred_family_string,
				char *preferred_genus_string,
				char *preferred_species_string,
				boolean exclude_zero_catches );

char *creel_library_get_species_where(
				char *family,
				char *genus,
				char *species,
				char *species_attribute_name,
				char *catches_table_name );

char *creel_library_get_fishing_area_where(
				char *fishing_area_list_string );

char *creel_library_get_interview_location_where(
				char *interview_location );

char *creel_library_get_fishing_trips_join_where(
				char *related_folder );

#endif
