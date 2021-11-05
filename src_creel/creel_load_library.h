/* creel_load_library.h */
/* -------------------- */

#ifndef CREEL_LOAD_LIBRARY_H
#define CREEL_LOAD_LIBRARY_H

/* Sport catch spreadsheet */
/* ----------------------- */
#define SPORT_INTERVIEW_NUMBER_PIECE			0
#define SPORT_CENSUS_DATE_PIECE				1
#define SPORT_DAY_OF_WEEK_PIECE				2
#define SPORT_FISHING_TRIP_DURATION_PIECE		3
#define SPORT_ONBOARD_FISHING_COUNT_PIECE		4
#define SPORT_FISHING_DURATION_PIECE			5
#define SPORT_TARGETED_FLORIDA_CODE_PIECE		6
#define SPORT_LOCATION_AREA_ZONE_PIECE			7
#define SPORT_INTERVIEW_LOCATION_PIECE			9
#define SPORT_INTERVIEW_TIME_PIECE			10
#define SPORT_RESEARCHER_CODE_PIECE			11
#define SPORT_FISHING_PARTY_COMPOSITION_PIECE		12
#define SPORT_TRIP_ORIGIN_LOCATION_PIECE		13
#define SPORT_RECREATIONAL_ANGLER_RESIDE_PIECE		14
#define SPORT_CATCH_FLORIDA_CODE_PIECE			15

/* Guide catch spreadsheet */
/* ----------------------- */
#define GUIDE_CATCH_INTERVIEW_NUMBER_PIECE		0
#define GUIDE_CATCH_PERMIT_CODE_PIECE			1
#define GUIDE_CATCH_CENSUS_DATE_PIECE			2
#define GUIDE_CATCH_FISHING_DURATION_PIECE		3
#define GUIDE_CATCH_ONBOARD_FISHING_COUNT_PIECE		4
#define GUIDE_CATCH_TARGETED_FLORIDA_CODE_PIECE		5
#define GUIDE_CATCH_STARTING_FLORIDA_CODE_PIECE		6
#define GUIDE_CATCH_FISHING_AREA_PIECE			9
#define GUIDE_CATCH_FISHING_AREA_ZONE_PIECE		10

/* Guide submission spreadsheet */
/* ---------------------------- */
#define GUIDE_SUBMISSION_ANGLER_NAME_PIECE		0
#define GUIDE_SUBMISSION_PERMIT_CODE_PIECE		1
#define GUIDE_SUBMISSION_CENSUS_DATE_PIECE		2
#define GUIDE_SUBMISSION_FISHING_DURATION_PIECE		3
#define GUIDE_SUBMISSION_ONBOARD_FISHING_COUNT_PIECE	4
#define GUIDE_SUBMISSION_TARGETED_COMMON_NAME_PIECE	5
#define GUIDE_SUBMISSION_STARTING_COMMON_NAME_PIECE	8
#define GUIDE_SUBMISSION_FISHING_AREA_PIECE		11
#define GUIDE_SUBMISSION_FISHING_AREA_ZONE_PIECE	12

/* Guide permit spreadsheet */
/* ------------------------ */
#define GUIDE_LAST_NAME_PIECE				0
#define GUIDE_FIRST_NAME_PIECE				1
#define GUIDE_STREET_ADDRESS_PIECE			2
#define GUIDE_CITY_PIECE				3
#define GUIDE_STATE_CODE_PIECE				4
#define GUIDE_ZIP_CODE_PIECE				5
#define GUIDE_DATE_ISSUED_PIECE				6
#define GUIDE_DATE_EXPIRED_PIECE			7
#define GUIDE_PERMIT_CODE_PIECE				8
#define GUIDE_STARTING_DECAL_PIECE			9
#define GUIDE_PHONE_NUMBER_PIECE			13
#define GUIDE_EMAIL_ADDRESS_PIECE			14

#define GUIDE_ABBREVIATED_DATE_ISSUED_PIECE		2
#define GUIDE_ABBREVIATED_DATE_EXPIRED_PIECE		3
#define GUIDE_ABBREVIATED_PERMIT_CODE_PIECE		4

/* Sport catch measurements spreadsheet */
/* ------------------------------------ */
#define SPORT_MEASUREMENTS_CENSUS_DATE_PIECE		0
#define SPORT_MEASUREMENTS_INTERVIEW_NUMBER_PIECE	1
#define SPORT_MEASUREMENTS_INTERVIEW_LOCATION_PIECE	2
#define SPORT_MEASUREMENTS_FLORIDA_CODE_PIECE		3
#define SPORT_MEASUREMENTS_LOCATION_AREA_PIECE		4
#define SPORT_MEASUREMENTS_LOCATION_ZONE_PIECE		5
/* -------------------------------- */
/* Note: column 6 is specific area. */
/* -------------------------------- */
#define SPORT_MEASUREMENTS_STARTING_LENGTH_PIECE	7

/*
#define CATCHES_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count"
#define FISHING_TRIPS_FIELD_LIST "fishing_purpose,census_date,interview_location,interview_number,interview_time,trip_origin_location,family,genus,species_preferred,fishing_area,fishing_party_composition,trip_hours,hours_fishing,number_of_people_fishing,recreational_angler_reside,last_changed_by,validation_date"
*/

boolean creel_load_library_get_family_genus_species(
				char *family,
				char *genus,
				char *species,
				char *florida_code_string,
				char *application_name );

boolean creel_load_library_get_family(
				char *family,
				char *florida_code_string,
				char *application_name );

DICTIONARY *creel_load_library_get_species_florida_code_dictionary(
				char *application_name );

DICTIONARY *creel_load_library_get_family_florida_code_dictionary(
				char *application_name );

boolean creel_load_library_populate_census_date_mdyy(
				char *census_date_mdyy,
				char *census_date_input );

void creel_load_library_pad_time_four(
				char *interview_time_string );

char *creel_load_library_get_guide_angler_name(
				char *first_name,
				char *last_name,
				char *application_name,
				char *permit_code );

boolean creel_load_library_common_get_species(
				char *family,
				char *genus,
				char *species,
				char *common_name,
				char *application_name );

DICTIONARY *creel_load_library_get_species_common_name_dictionary(
				char *application_name );

#endif
